#include "cyc_include.h"

 struct _tagged_ptr0{ int* curr; int* base; int* last_plus_one; } ; struct
_tagged_ptr1{ void** curr; void** base; void** last_plus_one; } ; struct _tuple0{
void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct Cyc_Core_Opt* f1;
struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ void* f1; int f2; } ; struct
_tuple4{ struct Cyc_Declarator* f1; struct Cyc_Absyn_Exp* f2; } ; struct _tuple5{
struct Cyc_Absyn_Tqual* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; struct _tuple6{ struct Cyc_Declarator* f1; struct Cyc_Core_Opt* f2; } ;
struct _tuple7{ struct Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3; } ;
struct _tuple8{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct
_tuple9{ struct _tagged_string f1; short f2; } ; struct _tuple10{ struct Cyc_Lex_Trie**
f1; } ; typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef
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
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string(
int); extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found_tag[ 10u]; struct
Cyc_Core_Not_found_struct{ char* tag; } ; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern char Cyc_Lexing_Error_tag[ 6u]; struct Cyc_Lexing_Error_struct{
char* tag; struct _tagged_string f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct
_tagged_string lex_buffer; int lex_buffer_len; int lex_abs_pos; int
lex_start_pos; int lex_curr_pos; int lex_last_pos; int lex_last_action; int
lex_eof_reached; } ; typedef struct Cyc_Lexing_lexbuf* Cyc_Lexing_Lexbuf; struct
Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_string, int,
void*); void* read_fun_state; } ; typedef struct Cyc_Lexing_function_lexbuf_state*
Cyc_Lexing_Function_lexbuf_state; struct Cyc_Lexing_lex_tables{ struct
_tagged_ptr0 lex_base; struct _tagged_ptr0 lex_backtrk; struct _tagged_ptr0
lex_default; struct _tagged_ptr0 lex_trans; struct _tagged_ptr0 lex_check; } ;
typedef struct Cyc_Lexing_lex_tables* Cyc_Lexing_LexTables; extern struct
_tagged_string Cyc_Lexing_lexeme( struct Cyc_Lexing_lexbuf*); extern char Cyc_Lexing_lexeme_char(
struct Cyc_Lexing_lexbuf*, int); extern int Cyc_Lexing_lexeme_start( struct Cyc_Lexing_lexbuf*);
extern int Cyc_Lexing_lexeme_end( struct Cyc_Lexing_lexbuf*); extern int Cyc_Lexing_lex_engine(
struct Cyc_Lexing_lex_tables*, int, struct Cyc_Lexing_lexbuf*); struct Cyc_Set_Set;
typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern struct Cyc_Set_Set* Cyc_Set_empty(
int(* comp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert( struct
Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member( struct Cyc_Set_Set* s,
void* elt); extern void Cyc_Set_iter( void(* f)( void*), struct Cyc_Set_Set* s);
extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{ char* tag; }
; extern unsigned int Cyc_String_strlen( struct _tagged_string s); extern int
Cyc_String_zstrptrcmp( struct _tagged_string*, struct _tagged_string*); extern
struct _tagged_string Cyc_String_str_sepstr( struct Cyc_List_List*, struct
_tagged_string); extern struct _tagged_string Cyc_String_zstrncpy( struct
_tagged_string, int, struct _tagged_string, int, unsigned int); extern struct
_tagged_string Cyc_String_substring( struct _tagged_string, int ofs,
unsigned int n); struct Cyc_Xarray_Xarray{ struct _tagged_ptr1 elmts; int
num_elmts; } ; typedef struct Cyc_Xarray_Xarray* Cyc_Xarray_xarray_t; extern
void* Cyc_Xarray_get( struct Cyc_Xarray_Xarray*, int); extern struct Cyc_Xarray_Xarray*
Cyc_Xarray_create( int, void*); extern int Cyc_Xarray_add_ind( struct Cyc_Xarray_Xarray*,
void*); struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct
_tagged_string line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos*
Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[ 5u]; struct Cyc_Position_Exit_struct{
char* tag; } ; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment*
Cyc_Position_seg_t; extern struct Cyc_Position_Segment* Cyc_Position_segment_of_abs(
int, int); typedef void* Cyc_Position_Error_kind; static const unsigned int Cyc_Position_Lex=
0; static const unsigned int Cyc_Position_Parse= 1; static const unsigned int
Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{
struct _tagged_string source; struct Cyc_Position_Segment* seg; void* kind;
struct _tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern struct Cyc_Position_Error* Cyc_Position_mk_err_lex( struct Cyc_Position_Segment*,
struct _tagged_string); extern struct Cyc_Position_Error* Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*, struct _tagged_string); extern char Cyc_Position_Nocontext_tag[
10u]; struct Cyc_Position_Nocontext_struct{ char* tag; } ; extern void Cyc_Position_post_error(
struct Cyc_Position_Error*); typedef struct _tagged_string* Cyc_Absyn_field_name_t;
typedef struct _tagged_string* Cyc_Absyn_var_t; typedef struct _tagged_string*
Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_Nmspace; typedef void* Cyc_Absyn_nmspace_t;
typedef struct _tuple0* Cyc_Absyn_qvar_t; typedef struct _tuple0* Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t;
struct Cyc_Absyn_Tvar; typedef void* Cyc_Absyn_Scope; struct Cyc_Absyn_Tqual;
typedef void* Cyc_Absyn_Size_of; typedef void* Cyc_Absyn_Kind; typedef void* Cyc_Absyn_Array_kind;
typedef void* Cyc_Absyn_Sign; struct Cyc_Absyn_Conref; typedef void* Cyc_Absyn_Constraint;
typedef void* Cyc_Absyn_Bounds; struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_FnInfo;
struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_XTunionInfo; struct Cyc_Absyn_TunionFieldInfo;
struct Cyc_Absyn_XTunionFieldInfo; typedef void* Cyc_Absyn_Type; typedef void*
Cyc_Absyn_Funcparams; typedef void* Cyc_Absyn_Type_modifier; typedef void* Cyc_Absyn_Cnst;
typedef void* Cyc_Absyn_Primop; typedef void* Cyc_Absyn_Incrementor; typedef
void* Cyc_Absyn_Raw_exp; struct Cyc_Absyn_Exp; typedef void* Cyc_Absyn_Raw_stmt;
struct Cyc_Absyn_Stmt; typedef void* Cyc_Absyn_Raw_pat; struct Cyc_Absyn_Pat;
typedef void* Cyc_Absyn_Binding; struct Cyc_Absyn_Switch_clause; struct Cyc_Absyn_Fndecl;
struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl; struct Cyc_Absyn_Tuniondecl;
struct Cyc_Absyn_XTuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
typedef void* Cyc_Absyn_Raw_decl; struct Cyc_Absyn_Decl; typedef void* Cyc_Absyn_Designator;
typedef struct _xtunion_struct* Cyc_Absyn_StmtAnnot; typedef void* Cyc_Absyn_Attribute;
struct Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_scope_t; typedef struct
Cyc_Absyn_Tqual* Cyc_Absyn_tqual_t; typedef void* Cyc_Absyn_size_of_t; typedef
void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar_t; typedef
void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref_t;
typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t; typedef
struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_FnInfo
Cyc_Absyn_fn_info_t; typedef struct Cyc_Absyn_TunionInfo Cyc_Absyn_tunion_info_t;
typedef struct Cyc_Absyn_XTunionInfo Cyc_Absyn_xtunion_info_t; typedef struct
Cyc_Absyn_TunionFieldInfo Cyc_Absyn_tunion_field_info_t; typedef struct Cyc_Absyn_XTunionFieldInfo
Cyc_Absyn_xtunion_field_info_t; typedef void* Cyc_Absyn_type_t; typedef void*
Cyc_Absyn_funcparams_t; typedef void* Cyc_Absyn_type_modifier_t; typedef void*
Cyc_Absyn_cnst_t; typedef void* Cyc_Absyn_primop_t; typedef void* Cyc_Absyn_incrementor_t;
typedef void* Cyc_Absyn_raw_exp_t; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_t;
typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt_t; typedef void* Cyc_Absyn_raw_stmt_t;
typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt_t; typedef struct Cyc_Absyn_Stmt*
Cyc_Absyn_stmt_opt_t; typedef void* Cyc_Absyn_raw_pat_t; typedef struct Cyc_Absyn_Pat*
Cyc_Absyn_pat_t; typedef void* Cyc_Absyn_binding_t; typedef struct Cyc_Absyn_Switch_clause*
Cyc_Absyn_switch_clause_t; typedef struct Cyc_Absyn_Fndecl* Cyc_Absyn_fndecl_t;
typedef struct Cyc_Absyn_Structdecl* Cyc_Absyn_structdecl_t; typedef struct Cyc_Absyn_Uniondecl*
Cyc_Absyn_uniondecl_t; typedef struct Cyc_Absyn_Tunionfield* Cyc_Absyn_tunionfield_t;
typedef struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_tuniondecl_t; typedef struct Cyc_Absyn_XTuniondecl*
Cyc_Absyn_xtuniondecl_t; typedef struct Cyc_Absyn_Typedefdecl* Cyc_Absyn_typedefdecl_t;
typedef struct Cyc_Absyn_Enumfield* Cyc_Absyn_enumfield_t; typedef struct Cyc_Absyn_Enumdecl*
Cyc_Absyn_enumdecl_t; typedef struct Cyc_Absyn_Vardecl* Cyc_Absyn_vardecl_t;
typedef void* Cyc_Absyn_raw_decl_t; typedef struct Cyc_Absyn_Decl* Cyc_Absyn_decl_t;
typedef void* Cyc_Absyn_designator_t; typedef struct _xtunion_struct* Cyc_Absyn_stmt_annot_t;
typedef void* Cyc_Absyn_attribute_t; typedef struct Cyc_List_List* Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Structfield* Cyc_Absyn_structfield_t; static const
unsigned int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n_tag= 0; struct
Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1; } ; static const int
Cyc_Absyn_Abs_n_tag= 1; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1; } ; static const unsigned int Cyc_Absyn_Static= 0; static const unsigned int
Cyc_Absyn_Abstract= 1; static const unsigned int Cyc_Absyn_Public= 2; static
const unsigned int Cyc_Absyn_Extern= 3; static const unsigned int Cyc_Absyn_ExternC=
4; struct Cyc_Absyn_Tqual{ int q_const: 1; int q_volatile: 1; int q_restrict: 1;
} ; static const unsigned int Cyc_Absyn_B1= 0; static const unsigned int Cyc_Absyn_B2=
1; static const unsigned int Cyc_Absyn_B4= 2; static const unsigned int Cyc_Absyn_B8=
3; static const unsigned int Cyc_Absyn_AnyKind= 0; static const unsigned int Cyc_Absyn_MemKind=
1; static const unsigned int Cyc_Absyn_BoxKind= 2; static const unsigned int Cyc_Absyn_RgnKind=
3; static const unsigned int Cyc_Absyn_EffKind= 4; static const unsigned int Cyc_Absyn_Signed=
0; static const unsigned int Cyc_Absyn_Unsigned= 1; struct Cyc_Absyn_Conref{
void* v; } ; static const int Cyc_Absyn_Eq_constr_tag= 0; struct Cyc_Absyn_Eq_constr_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr_tag= 1; struct
Cyc_Absyn_Forward_constr_struct{ int tag; struct Cyc_Absyn_Conref* f1; } ;
static const unsigned int Cyc_Absyn_No_constr= 0; struct Cyc_Absyn_Tvar{ struct
_tagged_string* name; struct Cyc_Absyn_Conref* kind; } ; static const
unsigned int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b_tag= 0;
struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct
Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual* tq; struct Cyc_Absyn_Conref* bounds; } ;
struct Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt*
effect; void* ret_typ; struct Cyc_List_List* args; int varargs; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_TunionInfo{ struct _tuple0* name; struct Cyc_List_List*
targs; void* rgn; struct Cyc_Absyn_Tuniondecl* tud; } ; struct Cyc_Absyn_XTunionInfo{
struct _tuple0* name; void* rgn; struct Cyc_Absyn_XTuniondecl* xtud; } ; struct
Cyc_Absyn_TunionFieldInfo{ struct _tuple0* name; struct Cyc_List_List* targs;
struct _tuple0* fname; struct Cyc_Absyn_Tuniondecl* tud; struct Cyc_Absyn_Tunionfield*
tufd; } ; struct Cyc_Absyn_XTunionFieldInfo{ struct _tuple0* name; struct
_tuple0* fname; struct Cyc_Absyn_XTuniondecl* xtud; struct Cyc_Absyn_Tunionfield*
xtufd; } ; static const unsigned int Cyc_Absyn_VoidType= 0; static const int Cyc_Absyn_Evar_tag=
0; struct Cyc_Absyn_Evar_struct{ int tag; void* f1; struct Cyc_Core_Opt* f2; int
f3; } ; static const int Cyc_Absyn_VarType_tag= 1; struct Cyc_Absyn_VarType_struct{
int tag; struct Cyc_Absyn_Tvar* f1; } ; static const int Cyc_Absyn_TunionType_tag=
2; struct Cyc_Absyn_TunionType_struct{ int tag; struct Cyc_Absyn_TunionInfo f1;
} ; static const int Cyc_Absyn_XTunionType_tag= 3; struct Cyc_Absyn_XTunionType_struct{
int tag; struct Cyc_Absyn_XTunionInfo f1; } ; static const int Cyc_Absyn_TunionFieldType_tag=
4; struct Cyc_Absyn_TunionFieldType_struct{ int tag; struct Cyc_Absyn_TunionFieldInfo
f1; } ; static const int Cyc_Absyn_XTunionFieldType_tag= 5; struct Cyc_Absyn_XTunionFieldType_struct{
int tag; struct Cyc_Absyn_XTunionFieldInfo f1; } ; static const int Cyc_Absyn_PointerType_tag=
6; struct Cyc_Absyn_PointerType_struct{ int tag; struct Cyc_Absyn_PtrInfo f1; }
; static const int Cyc_Absyn_IntType_tag= 7; struct Cyc_Absyn_IntType_struct{
int tag; void* f1; void* f2; } ; static const unsigned int Cyc_Absyn_FloatType=
1; static const unsigned int Cyc_Absyn_DoubleType= 2; static const int Cyc_Absyn_ArrayType_tag=
8; struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual*
f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_FnType_tag= 9;
struct Cyc_Absyn_FnType_struct{ int tag; struct Cyc_Absyn_FnInfo f1; } ; static
const int Cyc_Absyn_TupleType_tag= 10; struct Cyc_Absyn_TupleType_struct{ int
tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_StructType_tag= 11;
struct Cyc_Absyn_StructType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Structdecl** f3; } ; static const int Cyc_Absyn_UnionType_tag=
12; struct Cyc_Absyn_UnionType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_EnumType_tag=
13; struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType_tag= 14; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType_tag= 15; struct
Cyc_Absyn_TypedefType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Core_Opt* f3; } ; static const unsigned int Cyc_Absyn_HeapRgn= 3;
static const int Cyc_Absyn_AccessEff_tag= 16; struct Cyc_Absyn_AccessEff_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff_tag= 17; struct Cyc_Absyn_JoinEff_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_NoTypes_tag= 0;
struct Cyc_Absyn_NoTypes_struct{ int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment*
f2; } ; static const int Cyc_Absyn_WithTypes_tag= 1; struct Cyc_Absyn_WithTypes_struct{
int tag; struct Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3; } ; typedef
void* Cyc_Absyn_Pointer_Sort; static const int Cyc_Absyn_NonNullable_ps_tag= 0;
struct Cyc_Absyn_NonNullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Nullable_ps_tag= 1; struct Cyc_Absyn_Nullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const unsigned int Cyc_Absyn_TaggedArray_ps=
0; static const int Cyc_Absyn_Regparm_att_tag= 0; struct Cyc_Absyn_Regparm_att_struct{
int tag; int f1; } ; static const unsigned int Cyc_Absyn_Stdcall_att= 0; static
const unsigned int Cyc_Absyn_Cdecl_att= 1; static const unsigned int Cyc_Absyn_Noreturn_att=
2; static const unsigned int Cyc_Absyn_Const_att= 3; static const int Cyc_Absyn_Aligned_att_tag=
1; struct Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; static const
unsigned int Cyc_Absyn_Packed_att= 4; static const int Cyc_Absyn_Section_att_tag=
2; struct Cyc_Absyn_Section_att_struct{ int tag; struct _tagged_string f1; } ;
static const unsigned int Cyc_Absyn_Nocommon_att= 5; static const unsigned int
Cyc_Absyn_Shared_att= 6; static const unsigned int Cyc_Absyn_Unused_att= 7;
static const unsigned int Cyc_Absyn_Weak_att= 8; static const unsigned int Cyc_Absyn_Dllimport_att=
9; static const unsigned int Cyc_Absyn_Dllexport_att= 10; static const
unsigned int Cyc_Absyn_No_instrument_function_att= 11; static const unsigned int
Cyc_Absyn_Constructor_att= 12; static const unsigned int Cyc_Absyn_Destructor_att=
13; static const unsigned int Cyc_Absyn_No_check_memory_usage_att= 14; static
const unsigned int Cyc_Absyn_Carray_mod= 0; static const int Cyc_Absyn_ConstArray_mod_tag=
0; struct Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; }
; static const int Cyc_Absyn_Pointer_mod_tag= 1; struct Cyc_Absyn_Pointer_mod_struct{
int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual* f3; } ; static const int
Cyc_Absyn_Function_mod_tag= 2; struct Cyc_Absyn_Function_mod_struct{ int tag;
void* f1; } ; static const int Cyc_Absyn_TypeParams_mod_tag= 3; struct Cyc_Absyn_TypeParams_mod_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3; } ;
static const int Cyc_Absyn_Attributes_mod_tag= 4; struct Cyc_Absyn_Attributes_mod_struct{
int tag; struct Cyc_Position_Segment* f1; struct Cyc_List_List* f2; } ; static
const int Cyc_Absyn_Char_c_tag= 0; struct Cyc_Absyn_Char_c_struct{ int tag; void*
f1; char f2; } ; static const int Cyc_Absyn_Short_c_tag= 1; struct Cyc_Absyn_Short_c_struct{
int tag; void* f1; short f2; } ; static const int Cyc_Absyn_Int_c_tag= 2; struct
Cyc_Absyn_Int_c_struct{ int tag; void* f1; int f2; } ; static const int Cyc_Absyn_LongLong_c_tag=
3; struct Cyc_Absyn_LongLong_c_struct{ int tag; void* f1; long long f2; } ;
static const int Cyc_Absyn_Float_c_tag= 4; struct Cyc_Absyn_Float_c_struct{ int
tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_String_c_tag= 5;
struct Cyc_Absyn_String_c_struct{ int tag; struct _tagged_string f1; } ; static
const unsigned int Cyc_Absyn_Null_c= 0; static const unsigned int Cyc_Absyn_Plus=
0; static const unsigned int Cyc_Absyn_Times= 1; static const unsigned int Cyc_Absyn_Minus=
2; static const unsigned int Cyc_Absyn_Div= 3; static const unsigned int Cyc_Absyn_Mod=
4; static const unsigned int Cyc_Absyn_Eq= 5; static const unsigned int Cyc_Absyn_Neq=
6; static const unsigned int Cyc_Absyn_Gt= 7; static const unsigned int Cyc_Absyn_Lt=
8; static const unsigned int Cyc_Absyn_Gte= 9; static const unsigned int Cyc_Absyn_Lte=
10; static const unsigned int Cyc_Absyn_Not= 11; static const unsigned int Cyc_Absyn_Bitnot=
12; static const unsigned int Cyc_Absyn_Bitand= 13; static const unsigned int
Cyc_Absyn_Bitor= 14; static const unsigned int Cyc_Absyn_Bitxor= 15; static
const unsigned int Cyc_Absyn_Bitlshift= 16; static const unsigned int Cyc_Absyn_Bitlrshift=
17; static const unsigned int Cyc_Absyn_Bitarshift= 18; static const
unsigned int Cyc_Absyn_Size= 19; static const unsigned int Cyc_Absyn_Printf= 20;
static const unsigned int Cyc_Absyn_Fprintf= 21; static const unsigned int Cyc_Absyn_Xprintf=
22; static const unsigned int Cyc_Absyn_Scanf= 23; static const unsigned int Cyc_Absyn_Fscanf=
24; static const unsigned int Cyc_Absyn_Sscanf= 25; static const unsigned int
Cyc_Absyn_PreInc= 0; static const unsigned int Cyc_Absyn_PostInc= 1; static
const unsigned int Cyc_Absyn_PreDec= 2; static const unsigned int Cyc_Absyn_PostDec=
3; static const int Cyc_Absyn_Const_e_tag= 0; struct Cyc_Absyn_Const_e_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_Var_e_tag= 1; struct Cyc_Absyn_Var_e_struct{
int tag; struct _tuple0* f1; void* f2; } ; static const int Cyc_Absyn_UnknownId_e_tag=
2; struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple0* f1; } ; static
const int Cyc_Absyn_Primop_e_tag= 3; struct Cyc_Absyn_Primop_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_AssignOp_e_tag=
4; struct Cyc_Absyn_AssignOp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Increment_e_tag=
5; struct Cyc_Absyn_Increment_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; void*
f2; } ; static const int Cyc_Absyn_Conditional_e_tag= 6; struct Cyc_Absyn_Conditional_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_SeqExp_e_tag= 7; struct Cyc_Absyn_SeqExp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_UnknownCall_e_tag= 8; struct Cyc_Absyn_UnknownCall_e_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const int
Cyc_Absyn_FnCall_e_tag= 9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Throw_e_tag= 10;
struct Cyc_Absyn_Throw_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_NoInstantiate_e_tag= 11; struct Cyc_Absyn_NoInstantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Instantiate_e_tag=
12; struct Cyc_Absyn_Instantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Cast_e_tag= 13; struct
Cyc_Absyn_Cast_e_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2; } ; static
const int Cyc_Absyn_Address_e_tag= 14; struct Cyc_Absyn_Address_e_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_New_e_tag= 15;
struct Cyc_Absyn_New_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_Sizeoftyp_e_tag= 16; struct Cyc_Absyn_Sizeoftyp_e_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_Sizeofexp_e_tag= 17; struct
Cyc_Absyn_Sizeofexp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_Deref_e_tag= 18; struct Cyc_Absyn_Deref_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_StructMember_e_tag= 19;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_string* f2; } ; static const int Cyc_Absyn_StructArrow_e_tag= 20;
struct Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e_tag= 21; struct
Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp*
f2; } ; static const int Cyc_Absyn_Tuple_e_tag= 22; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_CompoundLit_e_tag=
23; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple1* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e_tag= 24; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e_tag=
25; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e_tag=
26; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_Tunion_e_tag= 27; struct Cyc_Absyn_Tunion_e_struct{ int tag;
struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ; static
const int Cyc_Absyn_XTunion_e_tag= 28; struct Cyc_Absyn_XTunion_e_struct{ int
tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_XTuniondecl*
f3; struct Cyc_Absyn_Tunionfield* f4; } ; static const int Cyc_Absyn_Enum_e_tag=
29; struct Cyc_Absyn_Enum_e_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl*
f2; struct Cyc_Absyn_Enumfield* f3; } ; static const int Cyc_Absyn_Malloc_e_tag=
30; struct Cyc_Absyn_Malloc_e_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_UnresolvedMem_e_tag= 31; struct Cyc_Absyn_UnresolvedMem_e_struct{ int
tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e_tag=
32; struct Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Codegen_e_tag= 33; struct Cyc_Absyn_Codegen_e_struct{
int tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e_tag=
34; struct Cyc_Absyn_Fill_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt* topt; void* r; struct Cyc_Position_Segment*
loc; } ; static const unsigned int Cyc_Absyn_Skip_s= 0; static const int Cyc_Absyn_Exp_s_tag=
0; struct Cyc_Absyn_Exp_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_Seq_s_tag= 1; struct Cyc_Absyn_Seq_s_struct{ int tag; struct
Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Return_s_tag=
2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_IfThenElse_s_tag= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s_tag= 4; struct Cyc_Absyn_While_s_struct{
int tag; struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s_tag=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s_tag= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s_tag= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s_tag= 8; struct Cyc_Absyn_For_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tuple2 f2; struct _tuple2 f3; struct
Cyc_Absyn_Stmt* f4; } ; static const int Cyc_Absyn_Switch_s_tag= 9; struct Cyc_Absyn_Switch_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Fallthru_s_tag= 10; struct Cyc_Absyn_Fallthru_s_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Absyn_Switch_clause** f2; } ; static const
int Cyc_Absyn_Decl_s_tag= 11; struct Cyc_Absyn_Decl_s_struct{ int tag; struct
Cyc_Absyn_Decl* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Cut_s_tag=
12; struct Cyc_Absyn_Cut_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Splice_s_tag= 13; struct Cyc_Absyn_Splice_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Label_s_tag=
14; struct Cyc_Absyn_Label_s_struct{ int tag; struct _tagged_string* f1; struct
Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Do_s_tag= 15; struct Cyc_Absyn_Do_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct _tuple2 f2; } ; static const int Cyc_Absyn_TryCatch_s_tag=
16; struct Cyc_Absyn_TryCatch_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1;
struct Cyc_List_List* f2; } ; struct Cyc_Absyn_Stmt{ void* r; struct Cyc_Position_Segment*
loc; struct Cyc_List_List* non_local_preds; int try_depth; struct
_xtunion_struct* annot; } ; static const unsigned int Cyc_Absyn_Wild_p= 0;
static const int Cyc_Absyn_Var_p_tag= 0; struct Cyc_Absyn_Var_p_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const unsigned int Cyc_Absyn_Null_p= 1;
static const int Cyc_Absyn_Int_p_tag= 1; struct Cyc_Absyn_Int_p_struct{ int tag;
void* f1; int f2; } ; static const int Cyc_Absyn_Char_p_tag= 2; struct Cyc_Absyn_Char_p_struct{
int tag; char f1; } ; static const int Cyc_Absyn_Float_p_tag= 3; struct Cyc_Absyn_Float_p_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Tuple_p_tag= 4;
struct Cyc_Absyn_Tuple_p_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_Pointer_p_tag= 5; struct Cyc_Absyn_Pointer_p_struct{ int tag;
struct Cyc_Absyn_Pat* f1; } ; static const int Cyc_Absyn_Reference_p_tag= 6;
struct Cyc_Absyn_Reference_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Struct_p_tag= 7; struct Cyc_Absyn_Struct_p_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_List_List* f4; } ; static const int Cyc_Absyn_Tunion_p_tag= 8;
struct Cyc_Absyn_Tunion_p_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; struct Cyc_Absyn_Tuniondecl*
f5; struct Cyc_Absyn_Tunionfield* f6; } ; static const int Cyc_Absyn_XTunion_p_tag=
9; struct Cyc_Absyn_XTunion_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_XTuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_p_tag= 10; struct Cyc_Absyn_Enum_p_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_UnknownId_p_tag= 11; struct Cyc_Absyn_UnknownId_p_struct{
int tag; struct _tuple0* f1; } ; static const int Cyc_Absyn_UnknownCall_p_tag=
12; struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple0* f1; struct
Cyc_List_List* f2; struct Cyc_List_List* f3; } ; static const int Cyc_Absyn_UnknownFields_p_tag=
13; struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple0* f1; struct
Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_Absyn_Pat{ void* r;
struct Cyc_Core_Opt* topt; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat* pattern; struct Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp*
where_clause; struct Cyc_Absyn_Stmt* body; struct Cyc_Position_Segment* loc; } ;
static const unsigned int Cyc_Absyn_Unresolved_b= 0; static const int Cyc_Absyn_Global_b_tag=
0; struct Cyc_Absyn_Global_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Funname_b_tag= 1; struct Cyc_Absyn_Funname_b_struct{
int tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Param_b_tag=
2; struct Cyc_Absyn_Param_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Local_b_tag= 3; struct Cyc_Absyn_Local_b_struct{ int
tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Pat_b_tag= 4;
struct Cyc_Absyn_Pat_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; struct
Cyc_Absyn_Vardecl{ void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual* tq;
void* type; struct Cyc_Absyn_Exp* initializer; int shadow; struct Cyc_Core_Opt*
region; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc;
int is_inline; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt*
body; struct Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structfield{ struct
_tagged_string* name; struct Cyc_Absyn_Tqual* tq; void* type; struct Cyc_Core_Opt*
width; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structdecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Uniondecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Tunionfield{
struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_List_List* typs;
struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Tuniondecl{ void* sc;
struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_XTuniondecl{ void* sc; struct _tuple0* name; struct
Cyc_List_List* fields; } ; struct Cyc_Absyn_Enumfield{ struct _tuple0* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple0* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple0* name; struct Cyc_List_List* tvs; void* defn; } ; static const
int Cyc_Absyn_Var_d_tag= 0; struct Cyc_Absyn_Var_d_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; static const int Cyc_Absyn_Fn_d_tag= 1; struct Cyc_Absyn_Fn_d_struct{
int tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Let_d_tag=
2; struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt*
f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4; int f5; } ; static const
int Cyc_Absyn_Struct_d_tag= 3; struct Cyc_Absyn_Struct_d_struct{ int tag; struct
Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Absyn_Union_d_tag= 4; struct
Cyc_Absyn_Union_d_struct{ int tag; struct Cyc_Absyn_Uniondecl* f1; } ; static
const int Cyc_Absyn_Tunion_d_tag= 5; struct Cyc_Absyn_Tunion_d_struct{ int tag;
struct Cyc_Absyn_Tuniondecl* f1; } ; static const int Cyc_Absyn_XTunion_d_tag= 6;
struct Cyc_Absyn_XTunion_d_struct{ int tag; struct Cyc_Absyn_XTuniondecl* f1; }
; static const int Cyc_Absyn_Enum_d_tag= 7; struct Cyc_Absyn_Enum_d_struct{ int
tag; struct Cyc_Absyn_Enumdecl* f1; } ; static const int Cyc_Absyn_Typedef_d_tag=
8; struct Cyc_Absyn_Typedef_d_struct{ int tag; struct Cyc_Absyn_Typedefdecl* f1;
} ; static const int Cyc_Absyn_Namespace_d_tag= 9; struct Cyc_Absyn_Namespace_d_struct{
int tag; struct _tagged_string* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Using_d_tag= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct
_tuple0* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d_tag=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement_tag= 0; struct Cyc_Absyn_ArrayElement_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName_tag=
1; struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern char Cyc_Absyn_EmptyAnnot_tag[ 11u]; struct Cyc_Absyn_EmptyAnnot_struct{
char* tag; } ; extern int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*, struct
Cyc_List_List*); extern struct Cyc_Core_Opt* Cyc_Parse_lbuf; typedef void* Cyc_Struct_or_union;
typedef void* Cyc_struct_or_union_t; typedef void* Cyc_Blockitem; typedef void*
Cyc_blockitem_t; typedef void* Cyc_Type_specifier; typedef void* Cyc_type_specifier_t;
typedef void* Cyc_Storage_class; typedef void* Cyc_storage_class_t; struct Cyc_Declaration_spec;
typedef struct Cyc_Declaration_spec* Cyc_decl_spec_t; struct Cyc_Declarator;
typedef struct Cyc_Declarator* Cyc_declarator_t; struct Cyc_Abstractdeclarator;
typedef struct Cyc_Abstractdeclarator* Cyc_abstractdeclarator_t; typedef struct
_xtunion_struct* Cyc_YYSTYPE; extern char Cyc_Okay_tok_tag[ 9u]; struct Cyc_Okay_tok_struct{
char* tag; } ; extern char Cyc_Int_tok_tag[ 8u]; struct Cyc_Int_tok_struct{ char*
tag; struct _tuple3* f1; } ; extern char Cyc_Char_tok_tag[ 9u]; struct Cyc_Char_tok_struct{
char* tag; char f1; } ; extern char Cyc_Pointer_Sort_tok_tag[ 17u]; struct Cyc_Pointer_Sort_tok_struct{
char* tag; void* f1; } ; extern char Cyc_Short_tok_tag[ 10u]; struct Cyc_Short_tok_struct{
char* tag; short f1; } ; extern char Cyc_String_tok_tag[ 11u]; struct Cyc_String_tok_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stringopt_tok_tag[ 14u];
struct Cyc_Stringopt_tok_struct{ char* tag; struct Cyc_Core_Opt* f1; } ; extern
char Cyc_Type_tok_tag[ 9u]; struct Cyc_Type_tok_struct{ char* tag; void* f1; } ;
extern char Cyc_TypeList_tok_tag[ 13u]; struct Cyc_TypeList_tok_struct{ char*
tag; struct Cyc_List_List* f1; } ; extern char Cyc_Exp_tok_tag[ 8u]; struct Cyc_Exp_tok_struct{
char* tag; struct Cyc_Absyn_Exp* f1; } ; extern char Cyc_ExpList_tok_tag[ 12u];
struct Cyc_ExpList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; extern
char Cyc_Primop_tok_tag[ 11u]; struct Cyc_Primop_tok_struct{ char* tag; void* f1;
} ; extern char Cyc_Primopopt_tok_tag[ 14u]; struct Cyc_Primopopt_tok_struct{
char* tag; struct Cyc_Core_Opt* f1; } ; extern char Cyc_QualId_tok_tag[ 11u];
struct Cyc_QualId_tok_struct{ char* tag; struct _tuple0* f1; } ; extern char Cyc_Stmt_tok_tag[
9u]; struct Cyc_Stmt_tok_struct{ char* tag; struct Cyc_Absyn_Stmt* f1; } ;
extern char Cyc_SwitchClauseList_tok_tag[ 21u]; struct Cyc_SwitchClauseList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Pattern_tok_tag[ 12u];
struct Cyc_Pattern_tok_struct{ char* tag; struct Cyc_Absyn_Pat* f1; } ; extern
char Cyc_PatternList_tok_tag[ 16u]; struct Cyc_PatternList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; extern char Cyc_FnDecl_tok_tag[ 11u]; struct Cyc_FnDecl_tok_struct{
char* tag; struct Cyc_Absyn_Fndecl* f1; } ; extern char Cyc_DeclList_tok_tag[ 13u];
struct Cyc_DeclList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; extern
char Cyc_DeclSpec_tok_tag[ 13u]; struct Cyc_DeclSpec_tok_struct{ char* tag;
struct Cyc_Declaration_spec* f1; } ; extern char Cyc_InitDecl_tok_tag[ 13u];
struct Cyc_InitDecl_tok_struct{ char* tag; struct _tuple4* f1; } ; extern char
Cyc_InitDeclList_tok_tag[ 17u]; struct Cyc_InitDeclList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; extern char Cyc_StorageClass_tok_tag[ 17u]; struct
Cyc_StorageClass_tok_struct{ char* tag; void* f1; } ; extern char Cyc_TypeSpecifier_tok_tag[
18u]; struct Cyc_TypeSpecifier_tok_struct{ char* tag; void* f1; } ; extern char
Cyc_QualSpecList_tok_tag[ 17u]; struct Cyc_QualSpecList_tok_struct{ char* tag;
struct _tuple5* f1; } ; extern char Cyc_TypeQual_tok_tag[ 13u]; struct Cyc_TypeQual_tok_struct{
char* tag; struct Cyc_Absyn_Tqual* f1; } ; extern char Cyc_StructFieldDeclList_tok_tag[
24u]; struct Cyc_StructFieldDeclList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_StructFieldDeclListList_tok_tag[ 28u]; struct Cyc_StructFieldDeclListList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Declarator_tok_tag[ 15u];
struct Cyc_Declarator_tok_struct{ char* tag; struct Cyc_Declarator* f1; } ;
extern char Cyc_DeclaratorExpopt_tok_tag[ 21u]; struct Cyc_DeclaratorExpopt_tok_struct{
char* tag; struct _tuple6* f1; } ; extern char Cyc_DeclaratorExpoptList_tok_tag[
25u]; struct Cyc_DeclaratorExpoptList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_AbstractDeclarator_tok_tag[ 23u]; struct Cyc_AbstractDeclarator_tok_struct{
char* tag; struct Cyc_Abstractdeclarator* f1; } ; extern char Cyc_TunionField_tok_tag[
16u]; struct Cyc_TunionField_tok_struct{ char* tag; struct Cyc_Absyn_Tunionfield*
f1; } ; extern char Cyc_TunionFieldList_tok_tag[ 20u]; struct Cyc_TunionFieldList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_ParamDecl_tok_tag[ 14u];
struct Cyc_ParamDecl_tok_struct{ char* tag; struct _tuple1* f1; } ; extern char
Cyc_ParamDeclList_tok_tag[ 18u]; struct Cyc_ParamDeclList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; extern char Cyc_ParamDeclListBool_tok_tag[ 22u];
struct Cyc_ParamDeclListBool_tok_struct{ char* tag; struct _tuple7* f1; } ;
extern char Cyc_StructOrUnion_tok_tag[ 18u]; struct Cyc_StructOrUnion_tok_struct{
char* tag; void* f1; } ; extern char Cyc_IdList_tok_tag[ 11u]; struct Cyc_IdList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Designator_tok_tag[ 15u];
struct Cyc_Designator_tok_struct{ char* tag; void* f1; } ; extern char Cyc_DesignatorList_tok_tag[
19u]; struct Cyc_DesignatorList_tok_struct{ char* tag; struct Cyc_List_List* f1;
} ; extern char Cyc_TypeModifierList_tok_tag[ 21u]; struct Cyc_TypeModifierList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Rgn_tok_tag[ 8u];
struct Cyc_Rgn_tok_struct{ char* tag; void* f1; } ; extern char Cyc_InitializerList_tok_tag[
20u]; struct Cyc_InitializerList_tok_struct{ char* tag; struct Cyc_List_List* f1;
} ; extern char Cyc_FieldPattern_tok_tag[ 17u]; struct Cyc_FieldPattern_tok_struct{
char* tag; struct _tuple8* f1; } ; extern char Cyc_FieldPatternList_tok_tag[ 21u];
struct Cyc_FieldPatternList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
extern char Cyc_BlockItem_tok_tag[ 14u]; struct Cyc_BlockItem_tok_struct{ char*
tag; void* f1; } ; extern char Cyc_Kind_tok_tag[ 9u]; struct Cyc_Kind_tok_struct{
char* tag; void* f1; } ; extern char Cyc_Attribute_tok_tag[ 14u]; struct Cyc_Attribute_tok_struct{
char* tag; void* f1; } ; extern char Cyc_AttributeList_tok_tag[ 18u]; struct Cyc_AttributeList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Enumfield_tok_tag[ 14u];
struct Cyc_Enumfield_tok_struct{ char* tag; struct Cyc_Absyn_Enumfield* f1; } ;
extern char Cyc_EnumfieldList_tok_tag[ 18u]; struct Cyc_EnumfieldList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; struct Cyc_Yyltype{ int timestamp; int
first_line; int first_column; int last_line; int last_column; struct
_tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; extern struct
Cyc_Yyltype Cyc_yylloc; extern struct _xtunion_struct* Cyc_yylval; struct Cyc_Dict_Dict;
typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict*
Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{
char* tag; } ; extern char Cyc_Dict_Absent_tag[ 7u]; extern struct Cyc_Dict_Dict*
Cyc_Dict_empty( int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); void Cyc_yyerror( struct _tagged_string s){
Cyc_Position_post_error( Cyc_Position_mk_err_parse( Cyc_Position_segment_of_abs(
Cyc_yylloc.first_line, Cyc_yylloc.last_line), s));} struct Cyc_Lex_Trie; typedef
struct Cyc_Lex_Trie* Cyc_Lex_trie_t; typedef struct Cyc_Lex_Trie** Cyc_Lex_trie_child_v_t;
typedef struct Cyc_Lex_Trie*** Cyc_Lex_trie_child_t; struct Cyc_Lex_Trie{ struct
Cyc_Lex_Trie*** children; int shared_str; } ; static int Cyc_Lex_num_kws= 0;
static struct _tagged_ptr0 Cyc_Lex_kw_nums={ 0, 0, 0}; static struct Cyc_Xarray_Xarray*
Cyc_Lex_symbols= 0; static struct Cyc_Lex_Trie* Cyc_Lex_ids_trie= 0; static
struct Cyc_Lex_Trie* Cyc_Lex_typedefs_trie= 0; static int Cyc_Lex_comment_depth=
0; static struct _tuple3 Cyc_Lex_token_int_pair=( struct _tuple3){.f1=( void*) 0u,.f2=
0}; static char _temp2[ 8u]="*bogus*"; static struct _tagged_string Cyc_Lex_bogus_string=(
struct _tagged_string){ _temp2, _temp2, _temp2 + 8u}; static struct Cyc_Absyn_Abs_n_struct
Cyc_Lex_absn_null={ 1u, 0}; static struct _tuple0 Cyc_Lex_token_id_pair=( struct
_tuple0){.f1=( void*)& Cyc_Lex_absn_null,.f2=& Cyc_Lex_bogus_string}; static
char Cyc_Lex_token_char='\000'; static char _temp6[ 1u]=""; static struct
_tagged_string Cyc_Lex_token_string=( struct _tagged_string){ _temp6, _temp6,
_temp6 + 1u}; static struct _tuple3* Cyc_Lex_token_int=& Cyc_Lex_token_int_pair;
static struct _tuple0* Cyc_Lex_token_qvar=& Cyc_Lex_token_id_pair; static int
Cyc_Lex_runaway_start= 0; static void Cyc_Lex_err( struct _tagged_string msg,
struct Cyc_Lexing_lexbuf* lb){ struct Cyc_Position_Segment* s= Cyc_Position_segment_of_abs(((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_start)( lb),(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_end)( lb)); Cyc_Position_post_error(
Cyc_Position_mk_err_lex( s, msg));} static void Cyc_Lex_runaway_err( struct
_tagged_string msg, struct Cyc_Lexing_lexbuf* lb){ struct Cyc_Position_Segment*
s= Cyc_Position_segment_of_abs( Cyc_Lex_runaway_start,(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme_start)( lb)); Cyc_Position_post_error( Cyc_Position_mk_err_lex(
s, msg));} static char _temp9[ 14u]="__attribute__"; static char _temp12[ 9u]="abstract";
static char _temp15[ 5u]="auto"; static char _temp18[ 6u]="break"; static char
_temp21[ 5u]="case"; static char _temp24[ 6u]="catch"; static char _temp27[ 5u]="char";
static char _temp30[ 8u]="codegen"; static char _temp33[ 6u]="const"; static
char _temp36[ 9u]="continue"; static char _temp39[ 4u]="cut"; static char
_temp42[ 8u]="default"; static char _temp45[ 3u]="do"; static char _temp48[ 7u]="double";
static char _temp51[ 5u]="else"; static char _temp54[ 5u]="enum"; static char
_temp57[ 7u]="extern"; static char _temp60[ 9u]="fallthru"; static char _temp63[
5u]="fill"; static char _temp66[ 6u]="float"; static char _temp69[ 4u]="for";
static char _temp72[ 8u]="fprintf"; static char _temp75[ 7u]="fscanf"; static
char _temp78[ 5u]="goto"; static char _temp81[ 3u]="if"; static char _temp84[ 7u]="inline";
static char _temp87[ 4u]="int"; static char _temp90[ 4u]="let"; static char
_temp93[ 5u]="long"; static char _temp96[ 7u]="malloc"; static char _temp99[ 10u]="namespace";
static char _temp102[ 4u]="new"; static char _temp105[ 5u]="null"; static char
_temp108[ 7u]="printf"; static char _temp111[ 9u]="register"; static char
_temp114[ 9u]="restrict"; static char _temp117[ 7u]="return"; static char
_temp120[ 6u]="scanf"; static char _temp123[ 6u]="short"; static char _temp126[
7u]="signed"; static char _temp129[ 7u]="sizeof"; static char _temp132[ 7u]="splice";
static char _temp135[ 7u]="sscanf"; static char _temp138[ 7u]="static"; static
char _temp141[ 7u]="struct"; static char _temp144[ 7u]="switch"; static char
_temp147[ 6u]="throw"; static char _temp150[ 4u]="try"; static char _temp153[ 7u]="tunion";
static char _temp156[ 8u]="typedef"; static char _temp159[ 6u]="union"; static
char _temp162[ 9u]="unsigned"; static char _temp165[ 6u]="using"; static char
_temp168[ 5u]="void"; static char _temp171[ 9u]="volatile"; static char _temp174[
6u]="while"; static char _temp177[ 8u]="xtunion"; static char _temp180[ 8u]="xprintf";
static struct _tuple9 Cyc_Lex_rw_array[ 58u]={( struct _tuple9){.f1=( struct
_tagged_string){ _temp9, _temp9, _temp9 + 14u},.f2= (short)347},( struct _tuple9){.f1=(
struct _tagged_string){ _temp12, _temp12, _temp12 + 9u},.f2= (short)298},(
struct _tuple9){.f1=( struct _tagged_string){ _temp15, _temp15, _temp15 + 5u},.f2=
(short)258},( struct _tuple9){.f1=( struct _tagged_string){ _temp18, _temp18,
_temp18 + 6u},.f2= (short)288},( struct _tuple9){.f1=( struct _tagged_string){
_temp21, _temp21, _temp21 + 5u},.f2= (short)277},( struct _tuple9){.f1=( struct
_tagged_string){ _temp24, _temp24, _temp24 + 6u},.f2= (short)296},( struct
_tuple9){.f1=( struct _tagged_string){ _temp27, _temp27, _temp27 + 5u},.f2=
(short)264},( struct _tuple9){.f1=( struct _tagged_string){ _temp30, _temp30,
_temp30 + 8u},.f2= (short)305},( struct _tuple9){.f1=( struct _tagged_string){
_temp33, _temp33, _temp33 + 6u},.f2= (short)272},( struct _tuple9){.f1=( struct
_tagged_string){ _temp36, _temp36, _temp36 + 9u},.f2= (short)287},( struct
_tuple9){.f1=( struct _tagged_string){ _temp39, _temp39, _temp39 + 4u},.f2=
(short)306},( struct _tuple9){.f1=( struct _tagged_string){ _temp42, _temp42,
_temp42 + 8u},.f2= (short)278},( struct _tuple9){.f1=( struct _tagged_string){
_temp45, _temp45, _temp45 + 3u},.f2= (short)284},( struct _tuple9){.f1=( struct
_tagged_string){ _temp48, _temp48, _temp48 + 7u},.f2= (short)269},( struct
_tuple9){.f1=( struct _tagged_string){ _temp51, _temp51, _temp51 + 5u},.f2=
(short)281},( struct _tuple9){.f1=( struct _tagged_string){ _temp54, _temp54,
_temp54 + 5u},.f2= (short)291},( struct _tuple9){.f1=( struct _tagged_string){
_temp57, _temp57, _temp57 + 7u},.f2= (short)261},( struct _tuple9){.f1=( struct
_tagged_string){ _temp60, _temp60, _temp60 + 9u},.f2= (short)299},( struct
_tuple9){.f1=( struct _tagged_string){ _temp63, _temp63, _temp63 + 5u},.f2=
(short)304},( struct _tuple9){.f1=( struct _tagged_string){ _temp66, _temp66,
_temp66 + 6u},.f2= (short)268},( struct _tuple9){.f1=( struct _tagged_string){
_temp69, _temp69, _temp69 + 4u},.f2= (short)285},( struct _tuple9){.f1=( struct
_tagged_string){ _temp72, _temp72, _temp72 + 8u},.f2= (short)309},( struct
_tuple9){.f1=( struct _tagged_string){ _temp75, _temp75, _temp75 + 7u},.f2=
(short)312},( struct _tuple9){.f1=( struct _tagged_string){ _temp78, _temp78,
_temp78 + 5u},.f2= (short)286},( struct _tuple9){.f1=( struct _tagged_string){
_temp81, _temp81, _temp81 + 3u},.f2= (short)280},( struct _tuple9){.f1=( struct
_tagged_string){ _temp84, _temp84, _temp84 + 7u},.f2= (short)279},( struct
_tuple9){.f1=( struct _tagged_string){ _temp87, _temp87, _temp87 + 4u},.f2=
(short)266},( struct _tuple9){.f1=( struct _tagged_string){ _temp90, _temp90,
_temp90 + 4u},.f2= (short)293},( struct _tuple9){.f1=( struct _tagged_string){
_temp93, _temp93, _temp93 + 5u},.f2= (short)267},( struct _tuple9){.f1=( struct
_tagged_string){ _temp96, _temp96, _temp96 + 7u},.f2= (short)314},( struct
_tuple9){.f1=( struct _tagged_string){ _temp99, _temp99, _temp99 + 10u},.f2=
(short)301},( struct _tuple9){.f1=( struct _tagged_string){ _temp102, _temp102,
_temp102 + 4u},.f2= (short)297},( struct _tuple9){.f1=( struct _tagged_string){
_temp105, _temp105, _temp105 + 5u},.f2= (short)292},( struct _tuple9){.f1=(
struct _tagged_string){ _temp108, _temp108, _temp108 + 7u},.f2= (short)308},(
struct _tuple9){.f1=( struct _tagged_string){ _temp111, _temp111, _temp111 + 9u},.f2=
(short)259},( struct _tuple9){.f1=( struct _tagged_string){ _temp114, _temp114,
_temp114 + 9u},.f2= (short)274},( struct _tuple9){.f1=( struct _tagged_string){
_temp117, _temp117, _temp117 + 7u},.f2= (short)289},( struct _tuple9){.f1=(
struct _tagged_string){ _temp120, _temp120, _temp120 + 6u},.f2= (short)311},(
struct _tuple9){.f1=( struct _tagged_string){ _temp123, _temp123, _temp123 + 6u},.f2=
(short)265},( struct _tuple9){.f1=( struct _tagged_string){ _temp126, _temp126,
_temp126 + 7u},.f2= (short)270},( struct _tuple9){.f1=( struct _tagged_string){
_temp129, _temp129, _temp129 + 7u},.f2= (short)290},( struct _tuple9){.f1=(
struct _tagged_string){ _temp132, _temp132, _temp132 + 7u},.f2= (short)307},(
struct _tuple9){.f1=( struct _tagged_string){ _temp135, _temp135, _temp135 + 7u},.f2=
(short)313},( struct _tuple9){.f1=( struct _tagged_string){ _temp138, _temp138,
_temp138 + 7u},.f2= (short)260},( struct _tuple9){.f1=( struct _tagged_string){
_temp141, _temp141, _temp141 + 7u},.f2= (short)275},( struct _tuple9){.f1=(
struct _tagged_string){ _temp144, _temp144, _temp144 + 7u},.f2= (short)282},(
struct _tuple9){.f1=( struct _tagged_string){ _temp147, _temp147, _temp147 + 6u},.f2=
(short)294},( struct _tuple9){.f1=( struct _tagged_string){ _temp150, _temp150,
_temp150 + 4u},.f2= (short)295},( struct _tuple9){.f1=( struct _tagged_string){
_temp153, _temp153, _temp153 + 7u},.f2= (short)302},( struct _tuple9){.f1=(
struct _tagged_string){ _temp156, _temp156, _temp156 + 8u},.f2= (short)262},(
struct _tuple9){.f1=( struct _tagged_string){ _temp159, _temp159, _temp159 + 6u},.f2=
(short)276},( struct _tuple9){.f1=( struct _tagged_string){ _temp162, _temp162,
_temp162 + 9u},.f2= (short)271},( struct _tuple9){.f1=( struct _tagged_string){
_temp165, _temp165, _temp165 + 6u},.f2= (short)300},( struct _tuple9){.f1=(
struct _tagged_string){ _temp168, _temp168, _temp168 + 5u},.f2= (short)263},(
struct _tuple9){.f1=( struct _tagged_string){ _temp171, _temp171, _temp171 + 9u},.f2=
(short)273},( struct _tuple9){.f1=( struct _tagged_string){ _temp174, _temp174,
_temp174 + 6u},.f2= (short)283},( struct _tuple9){.f1=( struct _tagged_string){
_temp177, _temp177, _temp177 + 8u},.f2= (short)303},( struct _tuple9){.f1=(
struct _tagged_string){ _temp180, _temp180, _temp180 + 8u},.f2= (short)310}};
static int Cyc_Lex_str_index( struct _tagged_string buff, int offset, int len){
int i= offset; int last=( offset + len) - 1; struct Cyc_Lex_Trie* t= Cyc_Lex_ids_trie;
while( i <= last) { int ch=( int)({ struct _tagged_string _temp181= buff; char*
_temp183= _temp181.curr + i; if( _temp181.base == 0? 1:( _temp183 < _temp181.base?
1: _temp183 >= _temp181.last_plus_one)){ _throw( Null_Exception);}* _temp183;})
- 48; if(({ struct Cyc_Lex_Trie* _temp184= t; if( _temp184 == 0){ _throw(
Null_Exception);} _temp184->children;}) == 0){ while( i <= last) { struct Cyc_Lex_Trie**
temp=( struct Cyc_Lex_Trie**)({ unsigned int _temp185= 75u; struct Cyc_Lex_Trie**
_temp186=( struct Cyc_Lex_Trie**) GC_malloc( sizeof( struct Cyc_Lex_Trie*) *
_temp185); unsigned int i; for( i= 0; i < _temp185; i ++){ _temp186[ i]= 0;}
_temp186;}); struct Cyc_Lex_Trie*** temp2=({ struct Cyc_Lex_Trie*** _temp187=(
struct Cyc_Lex_Trie***) GC_malloc( sizeof( struct Cyc_Lex_Trie**) * 1); _temp187[
0]= temp; _temp187;});({ struct Cyc_Lex_Trie* _temp188= t; if( _temp188 == 0){
_throw( Null_Exception);} _temp188->children=( struct Cyc_Lex_Trie***) temp2;});({
struct Cyc_Lex_Trie** _temp191=({ struct Cyc_Lex_Trie*** _temp190=({ struct Cyc_Lex_Trie*
_temp189= t; if( _temp189 == 0){ _throw( Null_Exception);} _temp189->children;});
if( _temp190 == 0){ _throw( Null_Exception);}* _temp190;}); unsigned int
_temp192= ch; if( _temp192 >= 75u){ _throw( Null_Exception);} _temp191[ _temp192]=({
struct Cyc_Lex_Trie* _temp193=( struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct
Cyc_Lex_Trie)); _temp193->children= 0; _temp193->shared_str= - 1; _temp193;});});
t=({ struct Cyc_Lex_Trie** _temp196=({ struct Cyc_Lex_Trie*** _temp195=({ struct
Cyc_Lex_Trie* _temp194= t; if( _temp194 == 0){ _throw( Null_Exception);}
_temp194->children;}); if( _temp195 == 0){ _throw( Null_Exception);}* _temp195;});
unsigned int _temp197= ch; if( _temp197 >= 75u){ _throw( Null_Exception);}
_temp196[ _temp197];}); ++ i; ch=( int)({ struct _tagged_string _temp198= buff;
char* _temp200= _temp198.curr + i; if( _temp198.base == 0? 1:( _temp200 <
_temp198.base? 1: _temp200 >= _temp198.last_plus_one)){ _throw( Null_Exception);}*
_temp200;}) - 48;}{ struct _tagged_string newstr= Cyc_Core_new_string( len + 1);
Cyc_String_zstrncpy( newstr, 0, buff, offset,( unsigned int) len);{ int ans=((
int(*)( struct Cyc_Xarray_Xarray*, struct _tagged_string*)) Cyc_Xarray_add_ind)((
struct Cyc_Xarray_Xarray*)({ struct Cyc_Xarray_Xarray* _temp201= Cyc_Lex_symbols;
if( _temp201 == 0){ _throw( Null_Exception);} _temp201;}),({ struct
_tagged_string* _temp202=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp202[ 0]= newstr; _temp202;}));({ struct Cyc_Lex_Trie*
_temp203= t; if( _temp203 == 0){ _throw( Null_Exception);} _temp203->shared_str=
ans;}); return ans;}}} if(({ struct Cyc_Lex_Trie** _temp206=({ struct Cyc_Lex_Trie***
_temp205=({ struct Cyc_Lex_Trie* _temp204= t; if( _temp204 == 0){ _throw(
Null_Exception);} _temp204->children;}); if( _temp205 == 0){ _throw(
Null_Exception);}* _temp205;}); unsigned int _temp207= ch; if( _temp207 >= 75u){
_throw( Null_Exception);} _temp206[ _temp207];}) == 0){({ struct Cyc_Lex_Trie**
_temp210=({ struct Cyc_Lex_Trie*** _temp209=({ struct Cyc_Lex_Trie* _temp208= t;
if( _temp208 == 0){ _throw( Null_Exception);} _temp208->children;}); if(
_temp209 == 0){ _throw( Null_Exception);}* _temp209;}); unsigned int _temp211=
ch; if( _temp211 >= 75u){ _throw( Null_Exception);} _temp210[ _temp211]=({
struct Cyc_Lex_Trie* _temp212=( struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct
Cyc_Lex_Trie)); _temp212->children= 0; _temp212->shared_str= - 1; _temp212;});});}
t=({ struct Cyc_Lex_Trie** _temp215=({ struct Cyc_Lex_Trie*** _temp214=({ struct
Cyc_Lex_Trie* _temp213= t; if( _temp213 == 0){ _throw( Null_Exception);}
_temp213->children;}); if( _temp214 == 0){ _throw( Null_Exception);}* _temp214;});
unsigned int _temp216= ch; if( _temp216 >= 75u){ _throw( Null_Exception);}
_temp215[ _temp216];}); ++ i;} if(({ struct Cyc_Lex_Trie* _temp217= t; if(
_temp217 == 0){ _throw( Null_Exception);} _temp217->shared_str;}) == - 1){
struct _tagged_string newstr= Cyc_Core_new_string( len + 1); Cyc_String_zstrncpy(
newstr, 0, buff, offset,( unsigned int) len);{ int ans=(( int(*)( struct Cyc_Xarray_Xarray*,
struct _tagged_string*)) Cyc_Xarray_add_ind)(( struct Cyc_Xarray_Xarray*)({
struct Cyc_Xarray_Xarray* _temp218= Cyc_Lex_symbols; if( _temp218 == 0){ _throw(
Null_Exception);} _temp218;}),({ struct _tagged_string* _temp219=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp219[ 0]=
newstr; _temp219;}));({ struct Cyc_Lex_Trie* _temp220= t; if( _temp220 == 0){
_throw( Null_Exception);} _temp220->shared_str= ans;}); return ans;}} return({
struct Cyc_Lex_Trie* _temp221= t; if( _temp221 == 0){ _throw( Null_Exception);}
_temp221->shared_str;});} static int Cyc_Lex_str_index_lbuf( struct Cyc_Lexing_lexbuf*
lbuf){ return Cyc_Lex_str_index( lbuf->lex_buffer, lbuf->lex_start_pos, lbuf->lex_curr_pos
- lbuf->lex_start_pos);} static void Cyc_Lex_insert_typedef( struct
_tagged_string* sptr){ struct _tagged_string s=* sptr; int len=( int)(({ struct
_tagged_string _temp222= s;( unsigned int)( _temp222.last_plus_one - _temp222.curr);})
- 1); struct Cyc_Lex_Trie* t= Cyc_Lex_typedefs_trie;{ int i= 0; for( 0; i < len;
++ i){ int ch=( int)({ struct _tagged_string _temp223= s; char* _temp225=
_temp223.curr + i; if( _temp223.base == 0? 1:( _temp225 < _temp223.base? 1:
_temp225 >= _temp223.last_plus_one)){ _throw( Null_Exception);}* _temp225;}) -
48; if(({ struct Cyc_Lex_Trie* _temp226= t; if( _temp226 == 0){ _throw(
Null_Exception);} _temp226->children;}) == 0){ while( i < len) {({ struct Cyc_Lex_Trie*
_temp227= t; if( _temp227 == 0){ _throw( Null_Exception);} _temp227->children=(
struct Cyc_Lex_Trie***)({ struct _tuple10* _temp228=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp228->f1=({ unsigned int _temp229= 75u;
struct Cyc_Lex_Trie** _temp230=( struct Cyc_Lex_Trie**) GC_malloc( sizeof(
struct Cyc_Lex_Trie*) * _temp229); unsigned int i; for( i= 0; i < _temp229; i ++){
_temp230[ i]= 0;} _temp230;}); _temp228;});});({ struct Cyc_Lex_Trie** _temp233=({
struct Cyc_Lex_Trie*** _temp232=({ struct Cyc_Lex_Trie* _temp231= t; if(
_temp231 == 0){ _throw( Null_Exception);} _temp231->children;}); if( _temp232 ==
0){ _throw( Null_Exception);}* _temp232;}); unsigned int _temp234= ch; if(
_temp234 >= 75u){ _throw( Null_Exception);} _temp233[ _temp234]=({ struct Cyc_Lex_Trie*
_temp235=( struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie));
_temp235->children= 0; _temp235->shared_str= 0; _temp235;});}); t=({ struct Cyc_Lex_Trie**
_temp238=({ struct Cyc_Lex_Trie*** _temp237=({ struct Cyc_Lex_Trie* _temp236= t;
if( _temp236 == 0){ _throw( Null_Exception);} _temp236->children;}); if(
_temp237 == 0){ _throw( Null_Exception);}* _temp237;}); unsigned int _temp239=
ch; if( _temp239 >= 75u){ _throw( Null_Exception);} _temp238[ _temp239];}); ++ i;
ch=( int)({ struct _tagged_string _temp240= s; char* _temp242= _temp240.curr + i;
if( _temp240.base == 0? 1:( _temp242 < _temp240.base? 1: _temp242 >= _temp240.last_plus_one)){
_throw( Null_Exception);}* _temp242;}) - 48;}({ struct Cyc_Lex_Trie* _temp243= t;
if( _temp243 == 0){ _throw( Null_Exception);} _temp243->shared_str= 1;});
return;} if(({ struct Cyc_Lex_Trie** _temp246=({ struct Cyc_Lex_Trie*** _temp245=({
struct Cyc_Lex_Trie* _temp244= t; if( _temp244 == 0){ _throw( Null_Exception);}
_temp244->children;}); if( _temp245 == 0){ _throw( Null_Exception);}* _temp245;});
unsigned int _temp247= ch; if( _temp247 >= 75u){ _throw( Null_Exception);}
_temp246[ _temp247];}) == 0){({ struct Cyc_Lex_Trie** _temp250=({ struct Cyc_Lex_Trie***
_temp249=({ struct Cyc_Lex_Trie* _temp248= t; if( _temp248 == 0){ _throw(
Null_Exception);} _temp248->children;}); if( _temp249 == 0){ _throw(
Null_Exception);}* _temp249;}); unsigned int _temp251= ch; if( _temp251 >= 75u){
_throw( Null_Exception);} _temp250[ _temp251]=({ struct Cyc_Lex_Trie* _temp252=(
struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp252->children=
0; _temp252->shared_str= 0; _temp252;});});} t=({ struct Cyc_Lex_Trie** _temp255=({
struct Cyc_Lex_Trie*** _temp254=({ struct Cyc_Lex_Trie* _temp253= t; if(
_temp253 == 0){ _throw( Null_Exception);} _temp253->children;}); if( _temp254 ==
0){ _throw( Null_Exception);}* _temp254;}); unsigned int _temp256= ch; if(
_temp256 >= 75u){ _throw( Null_Exception);} _temp255[ _temp256];});}}({ struct
Cyc_Lex_Trie* _temp257= t; if( _temp257 == 0){ _throw( Null_Exception);}
_temp257->shared_str= 1;}); return;} static struct _tagged_string* Cyc_Lex_get_symbol(
int symbol_num){ return(( struct _tagged_string*(*)( struct Cyc_Xarray_Xarray*,
int)) Cyc_Xarray_get)(( struct Cyc_Xarray_Xarray*)({ struct Cyc_Xarray_Xarray*
_temp258= Cyc_Lex_symbols; if( _temp258 == 0){ _throw( Null_Exception);}
_temp258;}), symbol_num);} static int Cyc_Lex_int_of_char( char c){ if('0' <= c?
c <='9': 0){ return c -'0';} else{ if('a' <= c? c <='f': 0){ return( 10 + c) -'a';}
else{ if('A' <= c? c <='F': 0){ return( 10 + c) -'A';} else{( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp259=( struct
Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp259[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp260; _temp260.tag= Cyc_Core_InvalidArg_tag;
_temp260.f1=( struct _tagged_string)({ char* _temp261=( char*)"string to integer conversion";
struct _tagged_string _temp262; _temp262.curr= _temp261; _temp262.base= _temp261;
_temp262.last_plus_one= _temp261 + 29; _temp262;}); _temp260;}); _temp259;}));}}}}
struct _tuple3* Cyc_Lex_intconst( struct Cyc_Lexing_lexbuf* lbuf, int start, int
base){ unsigned int n= 0; int end= lbuf->lex_curr_pos; struct _tagged_string
buff= lbuf->lex_buffer; int i= start + lbuf->lex_start_pos;{ int i= start + lbuf->lex_start_pos;
for( 0; i < end; ++ i){ char c=({ struct _tagged_string _temp263= buff; char*
_temp265= _temp263.curr + i; if( _temp263.base == 0? 1:( _temp265 < _temp263.base?
1: _temp265 >= _temp263.last_plus_one)){ _throw( Null_Exception);}* _temp265;});
switch( c){ case 'u': _LL266: goto _LL267; case 'U': _LL267: return({ struct
_tuple3* _temp269=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3));
_temp269->f1=( void*) Cyc_Absyn_Unsigned; _temp269->f2=( int) n; _temp269;});
case 'l': _LL268: break; case 'L': _LL270: break; default: _LL271: n= n * base +(
unsigned int) Cyc_Lex_int_of_char( c); break;}}} return({ struct _tuple3*
_temp273=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp273->f1=(
void*) Cyc_Absyn_Signed; _temp273->f2=( int) n; _temp273;});} static char Cyc_Lex_char_for_octal_code(
struct Cyc_Lexing_lexbuf* lb, int start, int howmany){ int c= 0;{ int i= 0; for(
0; i < howmany; i ++){ c=( 8 * c +(( char(*)( struct Cyc_Lexing_lexbuf*, int))
Cyc_Lexing_lexeme_char)( lb, start + i)) - 48;}} return( char) c;} static char
Cyc_Lex_char_for_hex_code( struct _tagged_string s, int start){ int c= 0; int
len=( int) Cyc_String_strlen( s);{ int i= 0; for( 0; start + i < len; i ++){
char x=({ struct _tagged_string _temp274= s; char* _temp276= _temp274.curr +(
start + i); if( _temp274.base == 0? 1:( _temp276 < _temp274.base? 1: _temp276 >=
_temp274.last_plus_one)){ _throw( Null_Exception);}* _temp276;}); if('0' <= x? x
<='9': 0){ c=( 16 * c +( int) x) -( int)'0';} else{ if('A' <= x? x <='F': 0){ c=(
16 * c +( int) x) -( int)'A';} else{ if('a' <= x? x <='f': 0){ c=( 16 * c +( int)
x) -( int)'a';} else{ break;}}}}} return( char) c;} static char _temp279[ 11u]="xxxxxxxxxx";
struct _tagged_string Cyc_Lex_string_buffer=( struct _tagged_string){ _temp279,
_temp279, _temp279 + 11u}; int Cyc_Lex_string_pos= 0; void Cyc_Lex_store_string_char(
char c){ int sz=( int)({ struct _tagged_string _temp280= Cyc_Lex_string_buffer;(
unsigned int)( _temp280.last_plus_one - _temp280.curr);}); if( Cyc_Lex_string_pos
>= sz){ int newsz= sz; while( Cyc_Lex_string_pos >= newsz) { newsz= newsz * 2;}{
struct _tagged_string str=({ unsigned int _temp284=( unsigned int) newsz; char*
_temp285=( char*) GC_malloc_atomic( sizeof( char) * _temp284); unsigned int i;
struct _tagged_string _temp286={ _temp285, _temp285, _temp285 + _temp284}; for(
i= 0; i < _temp284; i ++){ _temp285[ i]= i < sz?({ struct _tagged_string
_temp281= Cyc_Lex_string_buffer; char* _temp283= _temp281.curr +( int) i; if(
_temp281.base == 0? 1:( _temp283 < _temp281.base? 1: _temp283 >= _temp281.last_plus_one)){
_throw( Null_Exception);}* _temp283;}):'\000';} _temp286;}); Cyc_Lex_string_buffer=
str;}}({ struct _tagged_string _temp287= Cyc_Lex_string_buffer; char* _temp289=
_temp287.curr + Cyc_Lex_string_pos; if( _temp287.base == 0? 1:( _temp289 <
_temp287.base? 1: _temp289 >= _temp287.last_plus_one)){ _throw( Null_Exception);}*
_temp289= c;}); ++ Cyc_Lex_string_pos;} struct _tagged_string Cyc_Lex_get_stored_string(){
struct _tagged_string str= Cyc_String_substring( Cyc_Lex_string_buffer, 0,(
unsigned int) Cyc_Lex_string_pos); Cyc_Lex_string_pos= 0; return str;} struct
Cyc_Lex_Ldecls{ struct Cyc_Set_Set* typedefs; struct Cyc_Set_Set* namespaces; }
; typedef struct Cyc_Lex_Ldecls* Cyc_Lex_ldecls_t; struct Cyc_Lex_Lvis{ struct
Cyc_List_List* current_namespace; struct Cyc_List_List* imported_namespaces; } ;
typedef struct Cyc_Lex_Lvis* Cyc_Lex_lvis; struct Cyc_Lex_Lstate{ struct Cyc_List_List*
lstack; struct Cyc_Dict_Dict* decls; } ; typedef struct Cyc_Lex_Lstate* Cyc_Lex_lstate_t;
static struct Cyc_Core_Opt* Cyc_Lex_lstate= 0; static void Cyc_Lex_typedef_init(){
struct Cyc_Lex_Lvis* init_lvis=({ struct Cyc_Lex_Lvis* _temp290=( struct Cyc_Lex_Lvis*)
GC_malloc( sizeof( struct Cyc_Lex_Lvis)); _temp290->current_namespace= 0;
_temp290->imported_namespaces= 0; _temp290;}); struct Cyc_List_List* init_lstack=({
struct Cyc_List_List* _temp291=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp291->hd=( void*) init_lvis; _temp291->tl= 0;
_temp291;}); struct Cyc_Dict_Dict* init_decls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Lex_Ldecls* data))
Cyc_Dict_insert)((( struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*,
struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp), 0,({ struct
Cyc_Lex_Ldecls* _temp292=( struct Cyc_Lex_Ldecls*) GC_malloc( sizeof( struct Cyc_Lex_Ldecls));
_temp292->typedefs=(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tagged_string*,
struct _tagged_string*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp); _temp292->namespaces=((
struct Cyc_Set_Set*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp); _temp292;})); Cyc_Lex_lstate=({
struct Cyc_Core_Opt* _temp293=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp293->v=( void*)({ struct Cyc_Lex_Lstate* _temp294=( struct
Cyc_Lex_Lstate*) GC_malloc( sizeof( struct Cyc_Lex_Lstate)); _temp294->lstack=
init_lstack; _temp294->decls= init_decls; _temp294;}); _temp293;});} static
struct Cyc_List_List* Cyc_Lex_get_absolute_namespace( struct Cyc_List_List* ns){
struct _tagged_string* n=( struct _tagged_string*) ns->hd;{ struct Cyc_List_List*
ls=( struct Cyc_List_List*)(( struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt*
_temp295= Cyc_Lex_lstate; if( _temp295 == 0){ _throw( Null_Exception);} _temp295->v;}))->lstack;
for( 0; ls != 0; ls=({ struct Cyc_List_List* _temp296= ls; if( _temp296 == 0){
_throw( Null_Exception);} _temp296->tl;})){ struct Cyc_Lex_Lvis* lv=( struct Cyc_Lex_Lvis*)({
struct Cyc_List_List* _temp297= ls; if( _temp297 == 0){ _throw( Null_Exception);}
_temp297->hd;}); struct Cyc_List_List* x=({ struct Cyc_List_List* _temp298=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp298->hd=(
void*) lv->current_namespace; _temp298->tl= lv->imported_namespaces; _temp298;});
for( 0; x != 0; x=({ struct Cyc_List_List* _temp299= x; if( _temp299 == 0){
_throw( Null_Exception);} _temp299->tl;})){ struct Cyc_Lex_Ldecls* ld=(( struct
Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(((
struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt* _temp300= Cyc_Lex_lstate; if(
_temp300 == 0){ _throw( Null_Exception);} _temp300->v;}))->decls,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp301= x; if( _temp301 == 0){ _throw( Null_Exception);}
_temp301->hd;})); if((( int(*)( struct Cyc_Set_Set* s, struct _tagged_string*
elt)) Cyc_Set_member)( ld->namespaces, n)){ return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(( struct Cyc_List_List*)({
struct Cyc_List_List* _temp302= x; if( _temp302 == 0){ _throw( Null_Exception);}
_temp302->hd;}),( struct Cyc_List_List*) ns);}}}} Cyc_yyerror(({ struct
_tagged_string _temp305= Cyc_String_str_sepstr(( struct Cyc_List_List*) ns,(
struct _tagged_string)({ char* _temp303=( char*)"::"; struct _tagged_string
_temp304; _temp304.curr= _temp303; _temp304.base= _temp303; _temp304.last_plus_one=
_temp303 + 3; _temp304;})); xprintf("undeclared namespace %.*s", _temp305.last_plus_one
- _temp305.curr, _temp305.curr);})); return 0;} static void Cyc_Lex_recompute_typedefs(){
Cyc_Lex_typedefs_trie=({ struct Cyc_Lex_Trie* _temp306=( struct Cyc_Lex_Trie*)
GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp306->children= 0; _temp306->shared_str=
0; _temp306;});{ struct Cyc_List_List* ls=( struct Cyc_List_List*)(( struct Cyc_Lex_Lstate*)({
struct Cyc_Core_Opt* _temp307= Cyc_Lex_lstate; if( _temp307 == 0){ _throw(
Null_Exception);} _temp307->v;}))->lstack; for( 0; ls != 0; ls=({ struct Cyc_List_List*
_temp308= ls; if( _temp308 == 0){ _throw( Null_Exception);} _temp308->tl;})){
struct Cyc_Lex_Lvis* lv=( struct Cyc_Lex_Lvis*)({ struct Cyc_List_List* _temp309=
ls; if( _temp309 == 0){ _throw( Null_Exception);} _temp309->hd;}); struct Cyc_List_List*
x=({ struct Cyc_List_List* _temp310=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp310->hd=( void*) lv->current_namespace; _temp310->tl=
lv->imported_namespaces; _temp310;}); for( 0; x != 0; x=({ struct Cyc_List_List*
_temp311= x; if( _temp311 == 0){ _throw( Null_Exception);} _temp311->tl;})){
struct Cyc_Lex_Ldecls* ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)({ struct
Cyc_Core_Opt* _temp312= Cyc_Lex_lstate; if( _temp312 == 0){ _throw(
Null_Exception);} _temp312->v;}))->decls,( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp313= x; if( _temp313 == 0){ _throw( Null_Exception);} _temp313->hd;}));((
void(*)( void(* f)( struct _tagged_string*), struct Cyc_Set_Set* s)) Cyc_Set_iter)(
Cyc_Lex_insert_typedef, ld->typedefs);}}}} static int Cyc_Lex_is_typedef_in_namespace(
struct Cyc_List_List* ns, struct _tagged_string* v){ struct Cyc_List_List* ans=
Cyc_Lex_get_absolute_namespace( ns); struct _handler_cons _temp314;
_push_handler(& _temp314);{ struct _xtunion_struct* _temp315=( struct
_xtunion_struct*) setjmp( _temp314.handler); if( ! _temp315){{ struct Cyc_Lex_Ldecls*
ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt*
_temp316= Cyc_Lex_lstate; if( _temp316 == 0){ _throw( Null_Exception);} _temp316->v;}))->decls,
ans); int _temp317=(( int(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt))
Cyc_Set_member)( ld->typedefs, v); _npop_handler( 0u); return _temp317;};
_pop_handler();} else{ struct _xtunion_struct* _temp319= _temp315; _LL321: if(
_temp319->tag == Cyc_Dict_Absent_tag){ goto _LL322;} else{ goto _LL323;} _LL323:
goto _LL324; _LL322: return 0; _LL324:( void) _throw( _temp319); _LL320:;}}}
static int Cyc_Lex_is_typedef( struct Cyc_List_List* ns, struct _tagged_string*
v){ if( ns != 0){ return Cyc_Lex_is_typedef_in_namespace(( struct Cyc_List_List*)({
struct Cyc_List_List* _temp325= ns; if( _temp325 == 0){ _throw( Null_Exception);}
_temp325;}), v);}{ struct _tagged_string s=* v; int len=( int)(({ struct
_tagged_string _temp326= s;( unsigned int)( _temp326.last_plus_one - _temp326.curr);})
- 1); struct Cyc_Lex_Trie* t= Cyc_Lex_typedefs_trie;{ int i= 0; for( 0; i < len;
++ i){ if( t == 0? 1:({ struct Cyc_Lex_Trie* _temp327= t; if( _temp327 == 0){
_throw( Null_Exception);} _temp327->children;}) == 0){ return 0;} else{ t=({
struct Cyc_Lex_Trie** _temp333=({ struct Cyc_Lex_Trie*** _temp329=({ struct Cyc_Lex_Trie*
_temp328= t; if( _temp328 == 0){ _throw( Null_Exception);} _temp328->children;});
if( _temp329 == 0){ _throw( Null_Exception);}* _temp329;}); unsigned int
_temp334=( int)({ struct _tagged_string _temp330= s; char* _temp332= _temp330.curr
+ i; if( _temp330.base == 0? 1:( _temp332 < _temp330.base? 1: _temp332 >=
_temp330.last_plus_one)){ _throw( Null_Exception);}* _temp332;}) - 48; if(
_temp334 >= 75u){ _throw( Null_Exception);} _temp333[ _temp334];});}}} if( t ==
0){ return 0;} return({ struct Cyc_Lex_Trie* _temp335= t; if( _temp335 == 0){
_throw( Null_Exception);} _temp335->shared_str;});}} void Cyc_Lex_enter_namespace(
struct _tagged_string* s){ struct Cyc_List_List* ns=(( struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt* _temp336= Cyc_Lex_lstate; if(
_temp336 == 0){ _throw( Null_Exception);} _temp336->v;}))->lstack)->hd)->current_namespace;
struct Cyc_List_List* new_ns=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( ns,({ struct Cyc_List_List*
_temp337=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp337->hd=( void*) s; _temp337->tl= 0; _temp337;}));(( struct Cyc_Lex_Lstate*)({
struct Cyc_Core_Opt* _temp338= Cyc_Lex_lstate; if( _temp338 == 0){ _throw(
Null_Exception);} _temp338->v;}))->lstack=({ struct Cyc_List_List* _temp339=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp339->hd=(
void*)({ struct Cyc_Lex_Lvis* _temp341=( struct Cyc_Lex_Lvis*) GC_malloc(
sizeof( struct Cyc_Lex_Lvis)); _temp341->current_namespace= new_ns; _temp341->imported_namespaces=
0; _temp341;}); _temp339->tl=( struct Cyc_List_List*)(( struct Cyc_Lex_Lstate*)({
struct Cyc_Core_Opt* _temp340= Cyc_Lex_lstate; if( _temp340 == 0){ _throw(
Null_Exception);} _temp340->v;}))->lstack; _temp339;});{ struct Cyc_Lex_Ldecls*
ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt*
_temp342= Cyc_Lex_lstate; if( _temp342 == 0){ _throw( Null_Exception);} _temp342->v;}))->decls,
ns); if( !(( int(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)(
ld->namespaces, s)){(( struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt* _temp343=
Cyc_Lex_lstate; if( _temp343 == 0){ _throw( Null_Exception);} _temp343->v;}))->decls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Lex_Ldecls* data)) Cyc_Dict_insert)((( struct Cyc_Lex_Lstate*)({
struct Cyc_Core_Opt* _temp344= Cyc_Lex_lstate; if( _temp344 == 0){ _throw(
Null_Exception);} _temp344->v;}))->decls, ns,({ struct Cyc_Lex_Ldecls* _temp345=(
struct Cyc_Lex_Ldecls*) GC_malloc( sizeof( struct Cyc_Lex_Ldecls)); _temp345->typedefs=
ld->typedefs; _temp345->namespaces=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_insert)( ld->namespaces, s); _temp345;}));((
struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt* _temp346= Cyc_Lex_lstate; if(
_temp346 == 0){ _throw( Null_Exception);} _temp346->v;}))->decls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Lex_Ldecls* data))
Cyc_Dict_insert)((( struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt* _temp347= Cyc_Lex_lstate;
if( _temp347 == 0){ _throw( Null_Exception);} _temp347->v;}))->decls, new_ns,({
struct Cyc_Lex_Ldecls* _temp348=( struct Cyc_Lex_Ldecls*) GC_malloc( sizeof(
struct Cyc_Lex_Ldecls)); _temp348->typedefs=(( struct Cyc_Set_Set*(*)( int(*
comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp);
_temp348->namespaces=(( struct Cyc_Set_Set*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp);
_temp348;}));} Cyc_Lex_recompute_typedefs();}} void Cyc_Lex_leave_namespace(){((
struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt* _temp349= Cyc_Lex_lstate; if(
_temp349 == 0){ _throw( Null_Exception);} _temp349->v;}))->lstack=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp351=((( struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt*
_temp350= Cyc_Lex_lstate; if( _temp350 == 0){ _throw( Null_Exception);} _temp350->v;}))->lstack)->tl;
if( _temp351 == 0){ _throw( Null_Exception);} _temp351;}); Cyc_Lex_recompute_typedefs();}
void Cyc_Lex_enter_using( struct _tuple0* q){ struct Cyc_List_List* ns;{ void*
_temp352=(* q).f1; struct Cyc_List_List* _temp360; struct Cyc_List_List*
_temp362; _LL354: if(( int) _temp352 == Cyc_Absyn_Loc_n){ goto _LL355;} else{
goto _LL356;} _LL356: if(( unsigned int) _temp352 > 1u?(( struct _tunion_struct*)
_temp352)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL361: _temp360=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp352)->f1; goto _LL357;} else{ goto _LL358;}
_LL358: if(( unsigned int) _temp352 > 1u?(( struct _tunion_struct*) _temp352)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL363: _temp362=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp352)->f1; goto _LL359;} else{ goto _LL353;} _LL355:
ns=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp364=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp364->hd=( void*)(* q).f2;
_temp364->tl= 0; _temp364;}); goto _LL353; _LL357: _temp362= _temp360; goto
_LL359; _LL359: ns=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp366=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp362,({ struct Cyc_List_List* _temp365=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp365->hd=( void*)(* q).f2; _temp365->tl= 0;
_temp365;})); if( _temp366 == 0){ _throw( Null_Exception);} _temp366;}); goto
_LL353; _LL353:;}{ struct Cyc_List_List* ans= Cyc_Lex_get_absolute_namespace( ns);
struct Cyc_List_List* imported=(( struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)({
struct Cyc_Core_Opt* _temp367= Cyc_Lex_lstate; if( _temp367 == 0){ _throw(
Null_Exception);} _temp367->v;}))->lstack)->hd)->imported_namespaces;(( struct
Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt* _temp368= Cyc_Lex_lstate;
if( _temp368 == 0){ _throw( Null_Exception);} _temp368->v;}))->lstack)->hd)->imported_namespaces=({
struct Cyc_List_List* _temp369=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp369->hd=( void*) ans; _temp369->tl= imported;
_temp369;}); Cyc_Lex_recompute_typedefs();}} void Cyc_Lex_leave_using(){ struct
Cyc_List_List* imported=(( struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)({
struct Cyc_Core_Opt* _temp370= Cyc_Lex_lstate; if( _temp370 == 0){ _throw(
Null_Exception);} _temp370->v;}))->lstack)->hd)->imported_namespaces;(( struct
Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt* _temp371= Cyc_Lex_lstate;
if( _temp371 == 0){ _throw( Null_Exception);} _temp371->v;}))->lstack)->hd)->imported_namespaces=({
struct Cyc_List_List* _temp372= imported; if( _temp372 == 0){ _throw(
Null_Exception);} _temp372->tl;}); Cyc_Lex_recompute_typedefs();} void Cyc_Lex_register_typedef(
struct _tuple0* q){ struct Cyc_List_List* current=(( struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt* _temp373= Cyc_Lex_lstate; if(
_temp373 == 0){ _throw( Null_Exception);} _temp373->v;}))->lstack)->hd)->current_namespace;
struct Cyc_Dict_Dict* ds=(( struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt*
_temp374= Cyc_Lex_lstate; if( _temp374 == 0){ _throw( Null_Exception);} _temp374->v;}))->decls;
struct Cyc_Lex_Ldecls* ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( ds, current); struct Cyc_Lex_Ldecls*
new_ld=({ struct Cyc_Lex_Ldecls* _temp375=( struct Cyc_Lex_Ldecls*) GC_malloc(
sizeof( struct Cyc_Lex_Ldecls)); _temp375->typedefs=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)( ld->typedefs,(*
q).f2); _temp375->namespaces= ld->namespaces; _temp375;});(( struct Cyc_Lex_Lstate*)({
struct Cyc_Core_Opt* _temp376= Cyc_Lex_lstate; if( _temp376 == 0){ _throw(
Null_Exception);} _temp376->v;}))->decls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Lex_Ldecls* data)) Cyc_Dict_insert)( ds,
current, new_ld); Cyc_Lex_insert_typedef((* q).f2);} static short Cyc_Lex_process_id(
struct Cyc_Lexing_lexbuf* lbuf){ int symbol_num=(( int(*)( struct Cyc_Lexing_lexbuf*
lbuf)) Cyc_Lex_str_index_lbuf)( lbuf); if( symbol_num < Cyc_Lex_num_kws){ return(
short)({ struct _tagged_ptr0 _temp377= Cyc_Lex_kw_nums; int* _temp379= _temp377.curr
+ symbol_num; if( _temp377.base == 0? 1:( _temp379 < _temp377.base? 1: _temp379
>= _temp377.last_plus_one)){ _throw( Null_Exception);}* _temp379;});}{ struct
_tagged_string* s= Cyc_Lex_get_symbol( symbol_num); if( Cyc_Lex_is_typedef( 0, s)){
Cyc_Lex_token_qvar=({ struct _tuple0* _temp380=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp380->f1=( void*)({ struct Cyc_Absyn_Rel_n_struct*
_temp381=( struct Cyc_Absyn_Rel_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp381[ 0]=({ struct Cyc_Absyn_Rel_n_struct _temp382; _temp382.tag= Cyc_Absyn_Rel_n_tag;
_temp382.f1= 0; _temp382;}); _temp381;}); _temp380->f2= s; _temp380;}); return
(short)346;} Cyc_Lex_token_string=* s; return (short)339;}} static short Cyc_Lex_process_qual_id(
struct Cyc_Lexing_lexbuf* lbuf){ int i= lbuf->lex_start_pos; int end= lbuf->lex_curr_pos;
struct _tagged_string s= lbuf->lex_buffer; struct Cyc_List_List* rev_vs= 0;
while( i < end) { int start= i; for( 0; i < end?({ struct _tagged_string
_temp383= s; char* _temp385= _temp383.curr + i; if( _temp383.base == 0? 1:(
_temp385 < _temp383.base? 1: _temp385 >= _temp383.last_plus_one)){ _throw(
Null_Exception);}* _temp385;}) !=':': 0; i ++){;} if( start == i){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp386=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp386[ 0]=({ struct Cyc_Core_Impossible_struct _temp387; _temp387.tag= Cyc_Core_Impossible_tag;
_temp387.f1=( struct _tagged_string)({ char* _temp388=( char*)"bad namespace";
struct _tagged_string _temp389; _temp389.curr= _temp388; _temp389.base= _temp388;
_temp389.last_plus_one= _temp388 + 14; _temp389;}); _temp387;}); _temp386;}));}{
int vlen= i - start; struct _tagged_string* v= Cyc_Lex_get_symbol( Cyc_Lex_str_index(
s, start, vlen)); rev_vs=({ struct Cyc_List_List* _temp390=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp390->hd=( void*) v; _temp390->tl=
rev_vs; _temp390;}); i += 2;}} if( rev_vs == 0){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp391=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp391[ 0]=({ struct
Cyc_Core_Impossible_struct _temp392; _temp392.tag= Cyc_Core_Impossible_tag;
_temp392.f1=( struct _tagged_string)({ char* _temp393=( char*)"bad namespace";
struct _tagged_string _temp394; _temp394.curr= _temp393; _temp394.base= _temp393;
_temp394.last_plus_one= _temp393 + 14; _temp394;}); _temp392;}); _temp391;}));}{
struct _tagged_string* v=( struct _tagged_string*)({ struct Cyc_List_List*
_temp395= rev_vs; if( _temp395 == 0){ _throw( Null_Exception);} _temp395->hd;});
struct Cyc_List_List* vs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp396= rev_vs; if( _temp396 == 0){
_throw( Null_Exception);} _temp396->tl;})); Cyc_Lex_token_qvar=({ struct _tuple0*
_temp397=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp397->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp398=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp398[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp399; _temp399.tag= Cyc_Absyn_Rel_n_tag; _temp399.f1= vs; _temp399;});
_temp398;}); _temp397->f2= v; _temp397;}); if( Cyc_Lex_is_typedef( vs, v)){
return (short)346;} else{ return (short)345;}}} extern int Cyc_Lex_token( struct
Cyc_Lexing_lexbuf*); extern int Cyc_Lex_strng( struct Cyc_Lexing_lexbuf*);
extern int Cyc_Lex_comment( struct Cyc_Lexing_lexbuf*); int Cyc_yylex(){ int ans=((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_token)(( struct Cyc_Lexing_lexbuf*)({
struct Cyc_Core_Opt* _temp400= Cyc_Parse_lbuf; if( _temp400 == 0){ _throw(
Null_Exception);} _temp400->v;})); Cyc_yylloc.first_line=(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme_start)(( struct Cyc_Lexing_lexbuf*)({ struct Cyc_Core_Opt*
_temp401= Cyc_Parse_lbuf; if( _temp401 == 0){ _throw( Null_Exception);} _temp401->v;}));
Cyc_yylloc.last_line=(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_end)((
struct Cyc_Lexing_lexbuf*)({ struct Cyc_Core_Opt* _temp402= Cyc_Parse_lbuf; if(
_temp402 == 0){ _throw( Null_Exception);} _temp402->v;})); switch( ans){ case
339: _LL403: Cyc_yylval=( struct _xtunion_struct*)({ struct Cyc_String_tok_struct*
_temp405=( struct Cyc_String_tok_struct*) GC_malloc( sizeof( struct Cyc_String_tok_struct));
_temp405[ 0]=({ struct Cyc_String_tok_struct _temp406; _temp406.tag= Cyc_String_tok_tag;
_temp406.f1= Cyc_Lex_token_string; _temp406;}); _temp405;}); break; case 345:
_LL404: Cyc_yylval=( struct _xtunion_struct*)({ struct Cyc_QualId_tok_struct*
_temp408=( struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));
_temp408[ 0]=({ struct Cyc_QualId_tok_struct _temp409; _temp409.tag= Cyc_QualId_tok_tag;
_temp409.f1= Cyc_Lex_token_qvar; _temp409;}); _temp408;}); break; case 346:
_LL407: Cyc_yylval=( struct _xtunion_struct*)({ struct Cyc_QualId_tok_struct*
_temp411=( struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));
_temp411[ 0]=({ struct Cyc_QualId_tok_struct _temp412; _temp412.tag= Cyc_QualId_tok_tag;
_temp412.f1= Cyc_Lex_token_qvar; _temp412;}); _temp411;}); break; case 344:
_LL410: Cyc_yylval=( struct _xtunion_struct*)({ struct Cyc_String_tok_struct*
_temp414=( struct Cyc_String_tok_struct*) GC_malloc( sizeof( struct Cyc_String_tok_struct));
_temp414[ 0]=({ struct Cyc_String_tok_struct _temp415; _temp415.tag= Cyc_String_tok_tag;
_temp415.f1= Cyc_Lex_token_string; _temp415;}); _temp414;}); break; case 340:
_LL413: Cyc_yylval=( struct _xtunion_struct*)({ struct Cyc_Int_tok_struct*
_temp417=( struct Cyc_Int_tok_struct*) GC_malloc( sizeof( struct Cyc_Int_tok_struct));
_temp417[ 0]=({ struct Cyc_Int_tok_struct _temp418; _temp418.tag= Cyc_Int_tok_tag;
_temp418.f1= Cyc_Lex_token_int; _temp418;}); _temp417;}); break; case 342:
_LL416: Cyc_yylval=( struct _xtunion_struct*)({ struct Cyc_Char_tok_struct*
_temp420=( struct Cyc_Char_tok_struct*) GC_malloc( sizeof( struct Cyc_Char_tok_struct));
_temp420[ 0]=({ struct Cyc_Char_tok_struct _temp421; _temp421.tag= Cyc_Char_tok_tag;
_temp421.f1= Cyc_Lex_token_char; _temp421;}); _temp420;}); break; case 343:
_LL419: Cyc_yylval=( struct _xtunion_struct*)({ struct Cyc_String_tok_struct*
_temp423=( struct Cyc_String_tok_struct*) GC_malloc( sizeof( struct Cyc_String_tok_struct));
_temp423[ 0]=({ struct Cyc_String_tok_struct _temp424; _temp424.tag= Cyc_String_tok_tag;
_temp424.f1= Cyc_Lex_token_string; _temp424;}); _temp423;}); break; case 341:
_LL422: Cyc_yylval=( struct _xtunion_struct*)({ struct Cyc_String_tok_struct*
_temp426=( struct Cyc_String_tok_struct*) GC_malloc( sizeof( struct Cyc_String_tok_struct));
_temp426[ 0]=({ struct Cyc_String_tok_struct _temp427; _temp427.tag= Cyc_String_tok_tag;
_temp427.f1= Cyc_Lex_token_string; _temp427;}); _temp426;}); break; default:
_LL425: break;} return ans;} struct Cyc_Lexing_lex_tables* Cyc_Lex_lt= 0; int
Cyc_Lex_lbase[ 151u]={ 0, 113, 17, 83, 16, 2, - 3, - 1, - 2, - 19, - 20, 4, 118,
119, - 21, 5, - 13, - 12, 85, - 14, - 11, - 4, - 5, - 6, - 7, - 8, - 9, - 10,
209, 305, 111, - 15, 166, - 20, - 57, 8, 30, - 39, 12, 31, 116, 175, 32, 135,
141, 192, 145, 384, 427, 83, 82, 84, 87, 497, 85, 572, 647, 94, - 56, - 22, - 28,
722, 797, 93, 855, 930, 98, - 23, 1005, 96, - 26, 119, - 31, - 25, - 34, 1080,
1109, 292, 96, 107, 394, 1119, 1149, 465, 1090, 1182, 1213, 1251, 103, 114, 1321,
1359, 106, 117, 109, 119, 114, 128, - 6, - 38, 13, - 37, 6, 138, 1291, - 33, -
16, - 18, - 32, - 17, - 19, 2, 1399, 150, 158, 413, 162, 168, 169, 170, 171, 173,
180, 185, 189, 1472, 1556, - 54, - 46, - 45, - 44, - 43, - 42, - 41, - 40, - 47,
- 50, - 53, 488, - 52, 191, - 51, - 48, - 49, - 55, - 27, - 24, 14, - 35, 18,
477}; int Cyc_Lex_lbacktrk[ 151u]={ - 1, - 1, - 1, 5, 3, 4, - 1, - 1, - 1, - 1,
- 1, 18, 1, 21, - 1, 2, - 1, - 1, 16, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 17, 15, - 1, - 1, - 1, - 1, 35, 56, - 1, 56, 56, 56, 56, 56, 56, 56, 56,
56, 5, 7, 56, 56, 56, 56, 0, 56, 56, 56, 56, - 1, - 1, - 1, 3, 1, - 1, - 1, 2, -
1, - 1, 0, 29, - 1, 28, - 1, - 1, - 1, 9, 7, - 1, 7, 7, - 1, 8, 9, - 1, - 1, 9,
5, 6, 5, 5, - 1, 4, 4, 4, 6, 6, 5, 5, - 1, - 1, - 1, - 1, 36, - 1, 9, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 34, 35}; int Cyc_Lex_ldefault[
151u]={ 34, 9, 3, 3, - 1, - 1, 0, 0, 0, 0, 0, - 1, - 1, - 1, 0, - 1, 0, 0, - 1,
0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, - 1, 0, - 1, 0, 0, - 1, - 1, 0, 147, - 1, -
1, 111, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 0, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, 0, - 1, 0, 0, 0, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 0, 0, 100, 0, - 1, - 1, - 1, 0, 0, 0, 0, 0, 0, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, 0, - 1, 0, 0, 0, 0, 0, 0, 147, 0, - 1, - 1}; int
Cyc_Lex_ltrans[ 1813u]={ 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 35, 35, 35, 35, 33, 6,
101, 150, 150, 150, 150, 150, 148, 101, 148, 149, 102, 149, 148, 0, 0, 0, 35, 36,
37, 38, 0, 39, 40, 41, 150, 144, 42, 43, 7, 44, 45, 46, 47, 48, 48, 48, 48, 48,
48, 48, 48, 48, 49, 4, 50, 51, 52, 8, 5, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 17, 14, 110, 54,
55, 56, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 10, 57, - 1, 11, 32, 32, 6, - 1, 32, 15, 30, 30,
30, 30, 30, 30, 30, 30, 74, 71, 16, 72, 20, 67, 12, 19, 69, 32, 64, 7, 16, 145,
59, 64, 70, 17, 31, 31, 31, 31, 31, 31, 31, 31, 18, 18, 18, 18, 18, 18, 18, 18,
32, 32, 146, 31, 32, 73, 25, 19, 25, 105, - 1, 106, 99, 23, 143, 23, 22, 100, 22,
24, 24, 109, 142, 32, 98, 7, 136, 107, 108, 98, 13, 33, 135, 134, 133, 132, 20,
131, 25, - 1, 25, 21, 22, 60, 130, 23, 23, 23, 22, 129, 22, 24, 24, 128, 24, 141,
98, 0, 25, 0, 26, 98, 27, 103, 28, 104, 104, 104, 104, 104, 104, 104, 104, 104,
104, 0, 0, 0, 0, 0, 0, 58, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 112, - 1, - 1,
- 1, 0, 0, 6, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 0, 0, 0, 0, 0, 0, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 0, 0,
0, 80, 0, 80, 0, - 1, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 0, 0, 0, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 0, 0, 0, 0, 0, 0, 14, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 0, 0,
0, 0, 0, 0, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 0, 0, 75, - 1, 86, 86, 86, 86, 86, 86, 86,
86, 87, 87, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 137, 77, 0, 0, 0, 0, 0, 0,
88, 138, 138, 138, 138, 138, 138, 138, 138, 89, 0, 0, 90, 75, 0, 76, 76, 76, 76,
76, 76, 76, 76, 76, 76, 77, 150, 150, 150, 150, 150, 0, 88, 0, 0, 0, 77, 0, 0, 0,
0, 89, 0, 78, 90, 0, 0, 0, 84, 150, 84, 0, 79, 85, 85, 85, 85, 85, 85, 85, 85,
85, 85, 0, 0, 0, 0, 139, 77, 0, 0, 0, 0, 0, 0, 78, 140, 140, 140, 140, 140, 140,
140, 140, 79, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 63, 0, 0, 0, 0, 0, 0, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 0, 0, 0, 0, 68, 0, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 63, 0, 0, 0, 0, 0, 0, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 0, 0, 0, 0,
62, 0, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 0, 0, 0,
0, 0, 0, 0, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 0, 0, 0, 0, 61, 0, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 0, 0, 0, 0, 0, 0, 0, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 0, 0, 0, 0, 61, 0, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 63, 0, 0, 0, 0, 0, 0, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 0, 0, 0, 0, 62, 0, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 0, 0, 0, 0, 65, 0, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 66, 0, 0, 0, 0, 0, 0, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 0, 0, 0, 0, 65, 0, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 63, 0, 0, 0, 0, 0, 0, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 0, 0, 0, 0, 68, 0, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 85, 85, 85, 85, 85,
85, 85, 85, 85, 85, 0, 83, 27, 0, 0, 0, 0, 75, 27, 76, 76, 76, 76, 76, 76, 76,
76, 76, 76, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 0, 77, 0, 0, 83, 27, 0, 0,
78, 0, 0, 27, 26, 0, 0, 0, 0, 79, 26, 0, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82,
0, 0, 0, 77, 0, 0, 0, 0, 0, 0, 78, 83, 27, 0, 26, 0, 0, 0, 27, 79, 26, 0, 0, 85,
85, 85, 85, 85, 85, 85, 85, 85, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 83, 27, 27, 0,
0, 0, 0, 27, 27, 75, 0, 86, 86, 86, 86, 86, 86, 86, 86, 87, 87, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 77, 0, 27, 0, 0, 0, 0, 96, 27, 0, 0, 0, 0, 0, 0, 75, 97, 87, 87,
87, 87, 87, 87, 87, 87, 87, 87, 0, 0, 0, 0, 0, 77, 0, 0, 0, 0, 0, 77, 96, 0, 0,
0, 0, 0, 94, 0, 0, 97, 0, 0, 0, 0, 0, 95, 0, 0, 104, 104, 104, 104, 104, 104,
104, 104, 104, 104, 0, 0, 0, 77, 0, 0, 0, 0, 0, 0, 94, 83, 27, 0, 0, 0, 0, 0, 27,
95, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 0, 0, 0, 0, 0, 0, 0, 91, 91, 91, 91,
91, 91, 83, 27, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 91, 91, 91, 91, 91, 91,
91, 91, 91, 91, 0, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 0, 0, 0, 113,
0, 92, 0, 0, 114, 0, 0, 0, 0, 0, 93, 0, 0, 115, 115, 115, 115, 115, 115, 115,
115, 0, 91, 91, 91, 91, 91, 91, 116, 0, 0, 0, 0, 92, 0, 0, 0, 0, 0, 0, 0, 0, 93,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 117, 0, 0, 0, 0, 118, 119, 0, 0, 0,
120, 0, 0, 0, 0, 0, 0, 0, 121, 0, 0, 0, 122, 0, 123, 0, 124, 0, 125, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 127, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int Cyc_Lex_lcheck[ 1813u]={ - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 0,
0, 0, 0, 11, 15, 102, 35, 35, 35, 35, 35, 38, 100, 147, 38, 100, 147, 149, - 1,
- 1, - 1, 0, 0, 0, 0, - 1, 0, 0, 0, 35, 111, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 2, 0, 0, 0, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 39, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 1, 12, 12, 13, 3, 12,
13, 18, 18, 18, 18, 18, 18, 18, 18, 49, 50, 50, 50, 51, 54, 1, 52, 52, 12, 63,
12, 13, 40, 57, 66, 69, 13, 30, 30, 30, 30, 30, 30, 30, 30, 13, 13, 13, 13, 13,
13, 13, 13, 32, 32, 40, 43, 32, 71, 78, 13, 79, 103, 41, 44, 46, 88, 113, 89, 92,
46, 93, 94, 95, 43, 114, 32, 96, 32, 116, 44, 44, 97, 1, 46, 117, 118, 119, 120,
13, 121, 78, 41, 79, 13, 13, 57, 122, 88, 13, 89, 92, 123, 93, 94, 95, 124, 13,
140, 96, - 1, 13, - 1, 13, 97, 13, 45, 13, 45, 45, 45, 45, 45, 45, 45, 45, 45,
45, - 1, - 1, - 1, - 1, - 1, - 1, 0, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 41,
38, 100, 147, - 1, - 1, 2, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, - 1, - 1, - 1, - 1, - 1, - 1,
28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
28, 28, 28, 28, 28, 28, - 1, - 1, - 1, 77, - 1, 77, - 1, 3, 77, 77, 77, 77, 77,
77, 77, 77, 77, 77, - 1, - 1, - 1, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, - 1,
- 1, - 1, - 1, - 1, - 1, 1, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, - 1, - 1, - 1, - 1, - 1, - 1,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, - 1, - 1, 47, 41, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 115, 47, - 1, - 1, - 1, - 1, - 1, - 1,
47, 115, 115, 115, 115, 115, 115, 115, 115, 47, - 1, - 1, 47, 48, - 1, 48, 48,
48, 48, 48, 48, 48, 48, 48, 48, 47, 150, 150, 150, 150, 150, - 1, 47, - 1, - 1,
- 1, 48, - 1, - 1, - 1, - 1, 47, - 1, 48, 47, - 1, - 1, - 1, 83, 150, 83, - 1,
48, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, - 1, - 1, - 1, - 1, 138, 48, - 1, -
1, - 1, - 1, - 1, - 1, 48, 138, 138, 138, 138, 138, 138, 138, 138, 48, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, - 1, - 1, - 1, - 1, - 1, - 1, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
53, 53, - 1, - 1, - 1, - 1, 53, - 1, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 55, 55, 55, 55, 55,
55, 55, 55, 55, 55, 55, - 1, - 1, - 1, - 1, - 1, - 1, 55, 55, 55, 55, 55, 55, 55,
55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, - 1,
- 1, - 1, - 1, 55, - 1, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,
55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 56, 56, 56, 56, 56, 56, 56, 56,
56, 56, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 56, 56, 56, 56, 56, 56, 56, 56, 56,
56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, - 1, - 1, -
1, - 1, 56, - 1, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, - 1, - 1, - 1, - 1,
61, - 1, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, - 1,
- 1, - 1, - 1, - 1, - 1, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, - 1, - 1, - 1, - 1, 62, - 1, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, - 1, - 1, - 1, - 1, 64, - 1, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, - 1, - 1, - 1, - 1,
- 1, - 1, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, - 1, - 1, - 1, - 1, 65, - 1, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, - 1, - 1, - 1, - 1, - 1, - 1, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, - 1, - 1, - 1, - 1, 68, - 1, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 75, 75,
75, 75, 75, 75, 75, 75, 75, 75, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, - 1, 75,
75, - 1, - 1, - 1, - 1, 76, 75, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 81, 81,
81, 81, 81, 81, 81, 81, 81, 81, - 1, 76, - 1, - 1, 75, 75, - 1, - 1, 76, - 1, -
1, 75, 81, - 1, - 1, - 1, - 1, 76, 81, - 1, 82, 82, 82, 82, 82, 82, 82, 82, 82,
82, - 1, - 1, - 1, 76, - 1, - 1, - 1, - 1, - 1, - 1, 76, 82, 82, - 1, 81, - 1, -
1, - 1, 82, 76, 81, - 1, - 1, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 82, 82, 85, - 1, - 1, - 1, - 1, 82, 85,
86, - 1, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 86, - 1, 85, - 1, - 1, - 1, - 1, 86, 85, - 1, - 1, - 1, -
1, - 1, - 1, 87, 86, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, - 1, - 1, - 1, - 1,
- 1, 86, - 1, - 1, - 1, - 1, - 1, 87, 86, - 1, - 1, - 1, - 1, - 1, 87, - 1, - 1,
86, - 1, - 1, - 1, - 1, - 1, 87, - 1, - 1, 104, 104, 104, 104, 104, 104, 104,
104, 104, 104, - 1, - 1, - 1, 87, - 1, - 1, - 1, - 1, - 1, - 1, 87, 104, 104, -
1, - 1, - 1, - 1, - 1, 104, 87, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 90, 90, 90, 90, 90, 90, 104, 104, - 1, - 1, - 1, - 1, -
1, 104, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 91, 91, 91, 91, 91, 91, 91, 91, 91,
91, - 1, 90, 90, 90, 90, 90, 90, 91, 91, 91, 91, 91, 91, - 1, - 1, - 1, 112, - 1,
91, - 1, - 1, 112, - 1, - 1, - 1, - 1, - 1, 91, - 1, - 1, 112, 112, 112, 112,
112, 112, 112, 112, - 1, 91, 91, 91, 91, 91, 91, 112, - 1, - 1, - 1, - 1, 91, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 91, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, - 1, - 1, 112, 112, - 1, - 1, - 1,
112, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, - 1, 112, - 1, 112, - 1,
112, - 1, 112, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, - 1, - 1,
- 1, - 1, - 1, - 1, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 126, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, - 1, - 1, - 1, - 1, - 1, - 1, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1}; int Cyc_Lex_token_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ if( Cyc_Lex_lt == 0){ Cyc_Lex_lt=({ struct Cyc_Lexing_lex_tables*
_temp429=( struct Cyc_Lexing_lex_tables*) GC_malloc( sizeof( struct Cyc_Lexing_lex_tables));
_temp429->lex_base=( struct _tagged_ptr0)({ int* _temp438=( int*)(( int*) Cyc_Lex_lbase);
struct _tagged_ptr0 _temp439; _temp439.curr= _temp438; _temp439.base= _temp438;
_temp439.last_plus_one= _temp438 + 151; _temp439;}); _temp429->lex_backtrk=(
struct _tagged_ptr0)({ int* _temp436=( int*)(( int*) Cyc_Lex_lbacktrk); struct
_tagged_ptr0 _temp437; _temp437.curr= _temp436; _temp437.base= _temp436;
_temp437.last_plus_one= _temp436 + 151; _temp437;}); _temp429->lex_default=(
struct _tagged_ptr0)({ int* _temp434=( int*)(( int*) Cyc_Lex_ldefault); struct
_tagged_ptr0 _temp435; _temp435.curr= _temp434; _temp435.base= _temp434;
_temp435.last_plus_one= _temp434 + 151; _temp435;}); _temp429->lex_trans=(
struct _tagged_ptr0)({ int* _temp432=( int*)(( int*) Cyc_Lex_ltrans); struct
_tagged_ptr0 _temp433; _temp433.curr= _temp432; _temp433.base= _temp432;
_temp433.last_plus_one= _temp432 + 1813; _temp433;}); _temp429->lex_check=(
struct _tagged_ptr0)({ int* _temp430=( int*)(( int*) Cyc_Lex_lcheck); struct
_tagged_ptr0 _temp431; _temp431.curr= _temp430; _temp431.base= _temp430;
_temp431.last_plus_one= _temp430 + 1813; _temp431;}); _temp429;});} lexstate=((
int(*)( struct Cyc_Lexing_lex_tables*, int, struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lex_engine)((
struct Cyc_Lexing_lex_tables*)({ struct Cyc_Lexing_lex_tables* _temp440= Cyc_Lex_lt;
if( _temp440 == 0){ _throw( Null_Exception);} _temp440;}), lexstate, lexbuf);
switch( lexstate){ case 0: _LL441: return( int)(( short(*)( struct Cyc_Lexing_lexbuf*
lbuf)) Cyc_Lex_process_id)( lexbuf); case 1: _LL442: return( int)(( short(*)(
struct Cyc_Lexing_lexbuf* lbuf)) Cyc_Lex_process_id)( lexbuf); case 2: _LL443:
return( int)(( short(*)( struct Cyc_Lexing_lexbuf* lbuf)) Cyc_Lex_process_qual_id)(
lexbuf); case 3: _LL444: Cyc_Lex_token_string=* Cyc_Lex_get_symbol((( int(*)(
struct Cyc_Lexing_lexbuf* lbuf)) Cyc_Lex_str_index_lbuf)( lexbuf)); return 344;
case 4: _LL445: Cyc_Lex_token_int=(( struct _tuple3*(*)( struct Cyc_Lexing_lexbuf*
lbuf, int start, int base)) Cyc_Lex_intconst)( lexbuf, 2, 16); return 340; case
5: _LL446: Cyc_Lex_token_int=(( struct _tuple3*(*)( struct Cyc_Lexing_lexbuf*
lbuf, int start, int base)) Cyc_Lex_intconst)( lexbuf, 0, 8); return 340; case 6:
_LL447: Cyc_Lex_token_int=(( struct _tuple3*(*)( struct Cyc_Lexing_lexbuf* lbuf,
int start, int base)) Cyc_Lex_intconst)( lexbuf, 0, 10); return 340; case 7:
_LL448: Cyc_Lex_token_int=(( struct _tuple3*(*)( struct Cyc_Lexing_lexbuf* lbuf,
int start, int base)) Cyc_Lex_intconst)( lexbuf, 0, 10); return 340; case 8:
_LL449: Cyc_Lex_token_string=(( struct _tagged_string(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme)( lexbuf); return 343; case 9: _LL450: Cyc_Lex_token_string=((
struct _tagged_string(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)( lexbuf);
return 343; case 10: _LL451: return 322; case 11: _LL452: return 323; case 12:
_LL453: return 320; case 13: _LL454: return 321; case 14: _LL455: return 316;
case 15: _LL456: return 317; case 16: _LL457: return 329; case 17: _LL458:
return 330; case 18: _LL459: return 326; case 19: _LL460: return 327; case 20:
_LL461: return 328; case 21: _LL462: return 335; case 22: _LL463: return 334;
case 23: _LL464: return 333; case 24: _LL465: return 331; case 25: _LL466:
return 332; case 26: _LL467: return 324; case 27: _LL468: return 325; case 28:
_LL469: return 318; case 29: _LL470: return 319; case 30: _LL471: return 337;
case 31: _LL472: return 315; case 32: _LL473: return 336; case 33: _LL474:
return 338; case 34: _LL475: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_token)(
lexbuf); case 35: _LL476: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_token)(
lexbuf); case 36: _LL477: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_token)(
lexbuf); case 37: _LL478: Cyc_Lex_comment_depth= 1; Cyc_Lex_runaway_start=(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_start)( lexbuf);(( int(*)( struct
Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_token)( lexbuf); case 38: _LL479: Cyc_Lex_string_pos= 0; Cyc_Lex_runaway_start=((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_start)( lexbuf);(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); Cyc_Lex_token_string= Cyc_Lex_get_stored_string();
return 341; case 39: _LL480: Cyc_Lex_token_char='\a'; return 342; case 40:
_LL481: Cyc_Lex_token_char='\b'; return 342; case 41: _LL482: Cyc_Lex_token_char='\f';
return 342; case 42: _LL483: Cyc_Lex_token_char='\n'; return 342; case 43:
_LL484: Cyc_Lex_token_char='\r'; return 342; case 44: _LL485: Cyc_Lex_token_char='\t';
return 342; case 45: _LL486: Cyc_Lex_token_char='\v'; return 342; case 46:
_LL487: Cyc_Lex_token_char='\\'; return 342; case 47: _LL488: Cyc_Lex_token_char='\'';
return 342; case 48: _LL489: Cyc_Lex_token_char='"'; return 342; case 49: _LL490:
Cyc_Lex_token_char='?'; return 342; case 50: _LL491: Cyc_Lex_token_char=(( char(*)(
struct Cyc_Lexing_lexbuf* lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)(
lexbuf, 2, 3); return 342; case 51: _LL492: Cyc_Lex_token_char=(( char(*)(
struct Cyc_Lexing_lexbuf* lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)(
lexbuf, 2, 2); return 342; case 52: _LL493: Cyc_Lex_token_char=(( char(*)(
struct Cyc_Lexing_lexbuf* lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)(
lexbuf, 2, 1); return 342; case 53: _LL494: Cyc_Lex_token_char= Cyc_Lex_char_for_hex_code(((
struct _tagged_string(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)( lexbuf),
3); return 342; case 54: _LL495: Cyc_Lex_token_char=(( char(*)( struct Cyc_Lexing_lexbuf*,
int)) Cyc_Lexing_lexeme_char)( lexbuf, 1); return 342; case 55: _LL496: return -
1; case 56: _LL497: return( int)(( char(*)( struct Cyc_Lexing_lexbuf*, int)) Cyc_Lexing_lexeme_char)(
lexbuf, 0); default: _LL498:( lexbuf->refill_buff)( lexbuf); return(( int(*)(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_token_rec)( lexbuf,
lexstate);}( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Lexing_Error_struct*
_temp500=( struct Cyc_Lexing_Error_struct*) GC_malloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp500[ 0]=({ struct Cyc_Lexing_Error_struct _temp501; _temp501.tag= Cyc_Lexing_Error_tag;
_temp501.f1=( struct _tagged_string)({ char* _temp502=( char*)"some action didn't return!";
struct _tagged_string _temp503; _temp503.curr= _temp502; _temp503.base= _temp502;
_temp503.last_plus_one= _temp502 + 27; _temp503;}); _temp501;}); _temp500;}));}
int Cyc_Lex_token( struct Cyc_Lexing_lexbuf* lexbuf){ return(( int(*)( struct
Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_token_rec)( lexbuf, 0);} int
Cyc_Lex_strng_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ if( Cyc_Lex_lt
== 0){ Cyc_Lex_lt=({ struct Cyc_Lexing_lex_tables* _temp504=( struct Cyc_Lexing_lex_tables*)
GC_malloc( sizeof( struct Cyc_Lexing_lex_tables)); _temp504->lex_base=( struct
_tagged_ptr0)({ int* _temp513=( int*)(( int*) Cyc_Lex_lbase); struct
_tagged_ptr0 _temp514; _temp514.curr= _temp513; _temp514.base= _temp513;
_temp514.last_plus_one= _temp513 + 151; _temp514;}); _temp504->lex_backtrk=(
struct _tagged_ptr0)({ int* _temp511=( int*)(( int*) Cyc_Lex_lbacktrk); struct
_tagged_ptr0 _temp512; _temp512.curr= _temp511; _temp512.base= _temp511;
_temp512.last_plus_one= _temp511 + 151; _temp512;}); _temp504->lex_default=(
struct _tagged_ptr0)({ int* _temp509=( int*)(( int*) Cyc_Lex_ldefault); struct
_tagged_ptr0 _temp510; _temp510.curr= _temp509; _temp510.base= _temp509;
_temp510.last_plus_one= _temp509 + 151; _temp510;}); _temp504->lex_trans=(
struct _tagged_ptr0)({ int* _temp507=( int*)(( int*) Cyc_Lex_ltrans); struct
_tagged_ptr0 _temp508; _temp508.curr= _temp507; _temp508.base= _temp507;
_temp508.last_plus_one= _temp507 + 1813; _temp508;}); _temp504->lex_check=(
struct _tagged_ptr0)({ int* _temp505=( int*)(( int*) Cyc_Lex_lcheck); struct
_tagged_ptr0 _temp506; _temp506.curr= _temp505; _temp506.base= _temp505;
_temp506.last_plus_one= _temp505 + 1813; _temp506;}); _temp504;});} lexstate=((
int(*)( struct Cyc_Lexing_lex_tables*, int, struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lex_engine)((
struct Cyc_Lexing_lex_tables*)({ struct Cyc_Lexing_lex_tables* _temp515= Cyc_Lex_lt;
if( _temp515 == 0){ _throw( Null_Exception);} _temp515;}), lexstate, lexbuf);
switch( lexstate){ case 0: _LL516: return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 1: _LL517: return 0; case 2: _LL518: return(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 3: _LL519: Cyc_Lex_store_string_char('\a');
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 4:
_LL520: Cyc_Lex_store_string_char('\b'); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 5: _LL521: Cyc_Lex_store_string_char('\f'); return((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 6: _LL522: Cyc_Lex_store_string_char('\n');
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 7:
_LL523: Cyc_Lex_store_string_char('\r'); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 8: _LL524: Cyc_Lex_store_string_char('\t'); return((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 9: _LL525: Cyc_Lex_store_string_char('\v');
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 10:
_LL526: Cyc_Lex_store_string_char('\\'); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 11: _LL527: Cyc_Lex_store_string_char('\'');
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 12:
_LL528: Cyc_Lex_store_string_char('"'); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 13: _LL529: Cyc_Lex_store_string_char('?'); return((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 14: _LL530:
Cyc_Lex_store_string_char((( char(*)( struct Cyc_Lexing_lexbuf* lb, int start,
int howmany)) Cyc_Lex_char_for_octal_code)( lexbuf, 1, 3)); return(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 15: _LL531: Cyc_Lex_store_string_char(((
char(*)( struct Cyc_Lexing_lexbuf* lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)(
lexbuf, 1, 2)); return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)(
lexbuf); case 16: _LL532: Cyc_Lex_store_string_char((( char(*)( struct Cyc_Lexing_lexbuf*
lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)( lexbuf, 1, 1));
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 17:
_LL533: Cyc_Lex_store_string_char( Cyc_Lex_char_for_hex_code((( struct
_tagged_string(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)( lexbuf), 2));
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 18:
_LL534: Cyc_Lex_store_string_char((( char(*)( struct Cyc_Lexing_lexbuf*, int))
Cyc_Lexing_lexeme_char)( lexbuf, 0)); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 19: _LL535:(( void(*)( struct _tagged_string msg,
struct Cyc_Lexing_lexbuf* lb)) Cyc_Lex_runaway_err)(( struct _tagged_string)({
char* _temp537=( char*)"string ends in newline"; struct _tagged_string _temp538;
_temp538.curr= _temp537; _temp538.base= _temp537; _temp538.last_plus_one=
_temp537 + 23; _temp538;}), lexbuf); return 0; case 20: _LL536:(( void(*)(
struct _tagged_string msg, struct Cyc_Lexing_lexbuf* lb)) Cyc_Lex_runaway_err)((
struct _tagged_string)({ char* _temp540=( char*)"unterminated string"; struct
_tagged_string _temp541; _temp541.curr= _temp540; _temp541.base= _temp540;
_temp541.last_plus_one= _temp540 + 20; _temp541;}), lexbuf); return 0; case 21:
_LL539:(( void(*)( struct _tagged_string msg, struct Cyc_Lexing_lexbuf* lb)) Cyc_Lex_err)((
struct _tagged_string)({ char* _temp543=( char*)"bad character following backslash in string";
struct _tagged_string _temp544; _temp544.curr= _temp543; _temp544.base= _temp543;
_temp544.last_plus_one= _temp543 + 44; _temp544;}), lexbuf); return(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); default: _LL542:( lexbuf->refill_buff)(
lexbuf); return(( int(*)( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_strng_rec)(
lexbuf, lexstate);}( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Lexing_Error_struct*
_temp546=( struct Cyc_Lexing_Error_struct*) GC_malloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp546[ 0]=({ struct Cyc_Lexing_Error_struct _temp547; _temp547.tag= Cyc_Lexing_Error_tag;
_temp547.f1=( struct _tagged_string)({ char* _temp548=( char*)"some action didn't return!";
struct _tagged_string _temp549; _temp549.curr= _temp548; _temp549.base= _temp548;
_temp549.last_plus_one= _temp548 + 27; _temp549;}); _temp547;}); _temp546;}));}
int Cyc_Lex_strng( struct Cyc_Lexing_lexbuf* lexbuf){ return(( int(*)( struct
Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_strng_rec)( lexbuf, 1);} int
Cyc_Lex_comment_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ if( Cyc_Lex_lt
== 0){ Cyc_Lex_lt=({ struct Cyc_Lexing_lex_tables* _temp550=( struct Cyc_Lexing_lex_tables*)
GC_malloc( sizeof( struct Cyc_Lexing_lex_tables)); _temp550->lex_base=( struct
_tagged_ptr0)({ int* _temp559=( int*)(( int*) Cyc_Lex_lbase); struct
_tagged_ptr0 _temp560; _temp560.curr= _temp559; _temp560.base= _temp559;
_temp560.last_plus_one= _temp559 + 151; _temp560;}); _temp550->lex_backtrk=(
struct _tagged_ptr0)({ int* _temp557=( int*)(( int*) Cyc_Lex_lbacktrk); struct
_tagged_ptr0 _temp558; _temp558.curr= _temp557; _temp558.base= _temp557;
_temp558.last_plus_one= _temp557 + 151; _temp558;}); _temp550->lex_default=(
struct _tagged_ptr0)({ int* _temp555=( int*)(( int*) Cyc_Lex_ldefault); struct
_tagged_ptr0 _temp556; _temp556.curr= _temp555; _temp556.base= _temp555;
_temp556.last_plus_one= _temp555 + 151; _temp556;}); _temp550->lex_trans=(
struct _tagged_ptr0)({ int* _temp553=( int*)(( int*) Cyc_Lex_ltrans); struct
_tagged_ptr0 _temp554; _temp554.curr= _temp553; _temp554.base= _temp553;
_temp554.last_plus_one= _temp553 + 1813; _temp554;}); _temp550->lex_check=(
struct _tagged_ptr0)({ int* _temp551=( int*)(( int*) Cyc_Lex_lcheck); struct
_tagged_ptr0 _temp552; _temp552.curr= _temp551; _temp552.base= _temp551;
_temp552.last_plus_one= _temp551 + 1813; _temp552;}); _temp550;});} lexstate=((
int(*)( struct Cyc_Lexing_lex_tables*, int, struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lex_engine)((
struct Cyc_Lexing_lex_tables*)({ struct Cyc_Lexing_lex_tables* _temp561= Cyc_Lex_lt;
if( _temp561 == 0){ _throw( Null_Exception);} _temp561;}), lexstate, lexbuf);
switch( lexstate){ case 0: _LL562: ++ Cyc_Lex_comment_depth; return(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf); case 1: _LL563: -- Cyc_Lex_comment_depth;
if( Cyc_Lex_comment_depth > 0){ return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)(
lexbuf);} return 0; case 2: _LL564:(( void(*)( struct _tagged_string msg, struct
Cyc_Lexing_lexbuf* lb)) Cyc_Lex_runaway_err)(( struct _tagged_string)({ char*
_temp566=( char*)"unterminated comment"; struct _tagged_string _temp567;
_temp567.curr= _temp566; _temp567.base= _temp566; _temp567.last_plus_one=
_temp566 + 21; _temp567;}), lexbuf); return 0; case 3: _LL565: return(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf); case 4: _LL568: return((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf); case 5: _LL569:
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf); default:
_LL570:( lexbuf->refill_buff)( lexbuf); return(( int(*)( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate)) Cyc_Lex_comment_rec)( lexbuf, lexstate);}( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Lexing_Error_struct* _temp572=( struct Cyc_Lexing_Error_struct*)
GC_malloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp572[ 0]=({ struct Cyc_Lexing_Error_struct
_temp573; _temp573.tag= Cyc_Lexing_Error_tag; _temp573.f1=( struct
_tagged_string)({ char* _temp574=( char*)"some action didn't return!"; struct
_tagged_string _temp575; _temp575.curr= _temp574; _temp575.base= _temp574;
_temp575.last_plus_one= _temp574 + 27; _temp575;}); _temp573;}); _temp572;}));}
int Cyc_Lex_comment( struct Cyc_Lexing_lexbuf* lexbuf){ return(( int(*)( struct
Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_comment_rec)( lexbuf, 2);}
void Cyc_Lex_lex_init(){ Cyc_Lex_ids_trie=({ struct Cyc_Lex_Trie* _temp576=(
struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie) * 1); _temp576[ 0]=({
struct Cyc_Lex_Trie _temp577; _temp577.children= 0; _temp577.shared_str= - 1;
_temp577;}); _temp576;}); Cyc_Lex_typedefs_trie=({ struct Cyc_Lex_Trie* _temp578=(
struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie) * 1); _temp578[ 0]=({
struct Cyc_Lex_Trie _temp579; _temp579.children= 0; _temp579.shared_str= 0;
_temp579;}); _temp578;}); Cyc_Lex_symbols=( struct Cyc_Xarray_Xarray*)(( struct
Cyc_Xarray_Xarray*(*)( int, struct _tagged_string*)) Cyc_Xarray_create)( 101,({
struct _tagged_string* _temp580=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp580[ 0]=( struct _tagged_string)({ char*
_temp581=( char*)""; struct _tagged_string _temp582; _temp582.curr= _temp581;
_temp582.base= _temp581; _temp582.last_plus_one= _temp581 + 1; _temp582;});
_temp580;})); Cyc_Lex_num_kws=( int) 58u; Cyc_Lex_kw_nums=({ unsigned int
_temp583=( unsigned int) Cyc_Lex_num_kws; int* _temp584=( int*) GC_malloc_atomic(
sizeof( int) * _temp583); unsigned int i; struct _tagged_ptr0 _temp585={
_temp584, _temp584, _temp584 + _temp583}; for( i= 0; i < _temp583; i ++){
_temp584[ i]= 0;} _temp585;});{ int i= 0; for( 0; i < Cyc_Lex_num_kws; ++ i){
struct _tagged_string str=({ struct _tuple9* _temp586=( struct _tuple9*) Cyc_Lex_rw_array;
unsigned int _temp587= i; if( _temp587 >= 58u){ _throw( Null_Exception);}
_temp586[ _temp587];}).f1; Cyc_Lex_str_index( str, 0,( int) Cyc_String_strlen(
str));({ struct _tagged_ptr0 _temp588= Cyc_Lex_kw_nums; int* _temp590= _temp588.curr
+ i; if( _temp588.base == 0? 1:( _temp590 < _temp588.base? 1: _temp590 >=
_temp588.last_plus_one)){ _throw( Null_Exception);}* _temp590=( int)({ struct
_tuple9* _temp591=( struct _tuple9*) Cyc_Lex_rw_array; unsigned int _temp592= i;
if( _temp592 >= 58u){ _throw( Null_Exception);} _temp591[ _temp592];}).f2;});}}
Cyc_Lex_typedef_init(); Cyc_Lex_comment_depth= 0;}