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
struct Cyc_Absyn_New_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp*
f2; } ; static const int Cyc_Absyn_Sizeoftyp_e_tag= 16; struct Cyc_Absyn_Sizeoftyp_e_struct{
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
30; struct Cyc_Absyn_Malloc_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; void*
f2; } ; static const int Cyc_Absyn_UnresolvedMem_e_tag= 31; struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2; } ; static const int
Cyc_Absyn_StmtExp_e_tag= 32; struct Cyc_Absyn_StmtExp_e_struct{ int tag; struct
Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Codegen_e_tag= 33; struct Cyc_Absyn_Codegen_e_struct{
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
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Region_s_tag= 17;
struct Cyc_Absyn_Region_s_struct{ int tag; struct Cyc_Absyn_Tvar* f1; struct Cyc_Absyn_Vardecl*
f2; struct Cyc_Absyn_Stmt* f3; } ; struct Cyc_Absyn_Stmt{ void* r; struct Cyc_Position_Segment*
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
rgn; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc;
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
_temp108[ 7u]="printf"; static char _temp111[ 9u]="region_t"; static char
_temp114[ 7u]="region"; static char _temp117[ 9u]="register"; static char
_temp120[ 9u]="restrict"; static char _temp123[ 7u]="return"; static char
_temp126[ 8u]="rmalloc"; static char _temp129[ 5u]="rnew"; static char _temp132[
6u]="scanf"; static char _temp135[ 6u]="short"; static char _temp138[ 7u]="signed";
static char _temp141[ 7u]="sizeof"; static char _temp144[ 7u]="splice"; static
char _temp147[ 7u]="sscanf"; static char _temp150[ 7u]="static"; static char
_temp153[ 7u]="struct"; static char _temp156[ 7u]="switch"; static char _temp159[
6u]="throw"; static char _temp162[ 4u]="try"; static char _temp165[ 7u]="tunion";
static char _temp168[ 8u]="typedef"; static char _temp171[ 6u]="union"; static
char _temp174[ 9u]="unsigned"; static char _temp177[ 6u]="using"; static char
_temp180[ 5u]="void"; static char _temp183[ 9u]="volatile"; static char _temp186[
6u]="while"; static char _temp189[ 8u]="xtunion"; static char _temp192[ 8u]="xprintf";
static struct _tuple9 Cyc_Lex_rw_array[ 62u]={( struct _tuple9){.f1=( struct
_tagged_string){ _temp9, _temp9, _temp9 + 14u},.f2= (short)351},( struct _tuple9){.f1=(
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
(short)315},( struct _tuple9){.f1=( struct _tagged_string){ _temp114, _temp114,
_temp114 + 7u},.f2= (short)316},( struct _tuple9){.f1=( struct _tagged_string){
_temp117, _temp117, _temp117 + 9u},.f2= (short)259},( struct _tuple9){.f1=(
struct _tagged_string){ _temp120, _temp120, _temp120 + 9u},.f2= (short)274},(
struct _tuple9){.f1=( struct _tagged_string){ _temp123, _temp123, _temp123 + 7u},.f2=
(short)289},( struct _tuple9){.f1=( struct _tagged_string){ _temp126, _temp126,
_temp126 + 8u},.f2= (short)318},( struct _tuple9){.f1=( struct _tagged_string){
_temp129, _temp129, _temp129 + 5u},.f2= (short)317},( struct _tuple9){.f1=(
struct _tagged_string){ _temp132, _temp132, _temp132 + 6u},.f2= (short)311},(
struct _tuple9){.f1=( struct _tagged_string){ _temp135, _temp135, _temp135 + 6u},.f2=
(short)265},( struct _tuple9){.f1=( struct _tagged_string){ _temp138, _temp138,
_temp138 + 7u},.f2= (short)270},( struct _tuple9){.f1=( struct _tagged_string){
_temp141, _temp141, _temp141 + 7u},.f2= (short)290},( struct _tuple9){.f1=(
struct _tagged_string){ _temp144, _temp144, _temp144 + 7u},.f2= (short)307},(
struct _tuple9){.f1=( struct _tagged_string){ _temp147, _temp147, _temp147 + 7u},.f2=
(short)313},( struct _tuple9){.f1=( struct _tagged_string){ _temp150, _temp150,
_temp150 + 7u},.f2= (short)260},( struct _tuple9){.f1=( struct _tagged_string){
_temp153, _temp153, _temp153 + 7u},.f2= (short)275},( struct _tuple9){.f1=(
struct _tagged_string){ _temp156, _temp156, _temp156 + 7u},.f2= (short)282},(
struct _tuple9){.f1=( struct _tagged_string){ _temp159, _temp159, _temp159 + 6u},.f2=
(short)294},( struct _tuple9){.f1=( struct _tagged_string){ _temp162, _temp162,
_temp162 + 4u},.f2= (short)295},( struct _tuple9){.f1=( struct _tagged_string){
_temp165, _temp165, _temp165 + 7u},.f2= (short)302},( struct _tuple9){.f1=(
struct _tagged_string){ _temp168, _temp168, _temp168 + 8u},.f2= (short)262},(
struct _tuple9){.f1=( struct _tagged_string){ _temp171, _temp171, _temp171 + 6u},.f2=
(short)276},( struct _tuple9){.f1=( struct _tagged_string){ _temp174, _temp174,
_temp174 + 9u},.f2= (short)271},( struct _tuple9){.f1=( struct _tagged_string){
_temp177, _temp177, _temp177 + 6u},.f2= (short)300},( struct _tuple9){.f1=(
struct _tagged_string){ _temp180, _temp180, _temp180 + 5u},.f2= (short)263},(
struct _tuple9){.f1=( struct _tagged_string){ _temp183, _temp183, _temp183 + 9u},.f2=
(short)273},( struct _tuple9){.f1=( struct _tagged_string){ _temp186, _temp186,
_temp186 + 6u},.f2= (short)283},( struct _tuple9){.f1=( struct _tagged_string){
_temp189, _temp189, _temp189 + 8u},.f2= (short)303},( struct _tuple9){.f1=(
struct _tagged_string){ _temp192, _temp192, _temp192 + 8u},.f2= (short)310}};
static int Cyc_Lex_str_index( struct _tagged_string buff, int offset, int len){
int i= offset; int last=( offset + len) - 1; struct Cyc_Lex_Trie* t= Cyc_Lex_ids_trie;
while( i <= last) { int ch=( int)({ struct _tagged_string _temp193= buff; char*
_temp195= _temp193.curr + i; if( _temp193.base == 0? 1:( _temp195 < _temp193.base?
1: _temp195 >= _temp193.last_plus_one)){ _throw( Null_Exception);}* _temp195;})
- 48; if(({ struct Cyc_Lex_Trie* _temp196= t; if( _temp196 == 0){ _throw(
Null_Exception);} _temp196->children;}) == 0){ while( i <= last) { struct Cyc_Lex_Trie**
temp=( struct Cyc_Lex_Trie**)({ unsigned int _temp197= 75u; struct Cyc_Lex_Trie**
_temp198=( struct Cyc_Lex_Trie**) GC_malloc( sizeof( struct Cyc_Lex_Trie*) *
_temp197); unsigned int i; for( i= 0; i < _temp197; i ++){ _temp198[ i]= 0;}
_temp198;}); struct Cyc_Lex_Trie*** temp2=({ struct Cyc_Lex_Trie*** _temp199=(
struct Cyc_Lex_Trie***) GC_malloc( sizeof( struct Cyc_Lex_Trie**) * 1); _temp199[
0]= temp; _temp199;});({ struct Cyc_Lex_Trie* _temp200= t; if( _temp200 == 0){
_throw( Null_Exception);} _temp200->children=( struct Cyc_Lex_Trie***) temp2;});({
struct Cyc_Lex_Trie** _temp203=({ struct Cyc_Lex_Trie*** _temp202=({ struct Cyc_Lex_Trie*
_temp201= t; if( _temp201 == 0){ _throw( Null_Exception);} _temp201->children;});
if( _temp202 == 0){ _throw( Null_Exception);}* _temp202;}); unsigned int
_temp204= ch; if( _temp204 >= 75u){ _throw( Null_Exception);} _temp203[ _temp204]=({
struct Cyc_Lex_Trie* _temp205=( struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct
Cyc_Lex_Trie)); _temp205->children= 0; _temp205->shared_str= - 1; _temp205;});});
t=({ struct Cyc_Lex_Trie** _temp208=({ struct Cyc_Lex_Trie*** _temp207=({ struct
Cyc_Lex_Trie* _temp206= t; if( _temp206 == 0){ _throw( Null_Exception);}
_temp206->children;}); if( _temp207 == 0){ _throw( Null_Exception);}* _temp207;});
unsigned int _temp209= ch; if( _temp209 >= 75u){ _throw( Null_Exception);}
_temp208[ _temp209];}); ++ i; ch=( int)({ struct _tagged_string _temp210= buff;
char* _temp212= _temp210.curr + i; if( _temp210.base == 0? 1:( _temp212 <
_temp210.base? 1: _temp212 >= _temp210.last_plus_one)){ _throw( Null_Exception);}*
_temp212;}) - 48;}{ struct _tagged_string newstr= Cyc_Core_new_string( len + 1);
Cyc_String_zstrncpy( newstr, 0, buff, offset,( unsigned int) len);{ int ans=((
int(*)( struct Cyc_Xarray_Xarray*, struct _tagged_string*)) Cyc_Xarray_add_ind)((
struct Cyc_Xarray_Xarray*)({ struct Cyc_Xarray_Xarray* _temp213= Cyc_Lex_symbols;
if( _temp213 == 0){ _throw( Null_Exception);} _temp213;}),({ struct
_tagged_string* _temp214=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp214[ 0]= newstr; _temp214;}));({ struct Cyc_Lex_Trie*
_temp215= t; if( _temp215 == 0){ _throw( Null_Exception);} _temp215->shared_str=
ans;}); return ans;}}} if(({ struct Cyc_Lex_Trie** _temp218=({ struct Cyc_Lex_Trie***
_temp217=({ struct Cyc_Lex_Trie* _temp216= t; if( _temp216 == 0){ _throw(
Null_Exception);} _temp216->children;}); if( _temp217 == 0){ _throw(
Null_Exception);}* _temp217;}); unsigned int _temp219= ch; if( _temp219 >= 75u){
_throw( Null_Exception);} _temp218[ _temp219];}) == 0){({ struct Cyc_Lex_Trie**
_temp222=({ struct Cyc_Lex_Trie*** _temp221=({ struct Cyc_Lex_Trie* _temp220= t;
if( _temp220 == 0){ _throw( Null_Exception);} _temp220->children;}); if(
_temp221 == 0){ _throw( Null_Exception);}* _temp221;}); unsigned int _temp223=
ch; if( _temp223 >= 75u){ _throw( Null_Exception);} _temp222[ _temp223]=({
struct Cyc_Lex_Trie* _temp224=( struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct
Cyc_Lex_Trie)); _temp224->children= 0; _temp224->shared_str= - 1; _temp224;});});}
t=({ struct Cyc_Lex_Trie** _temp227=({ struct Cyc_Lex_Trie*** _temp226=({ struct
Cyc_Lex_Trie* _temp225= t; if( _temp225 == 0){ _throw( Null_Exception);}
_temp225->children;}); if( _temp226 == 0){ _throw( Null_Exception);}* _temp226;});
unsigned int _temp228= ch; if( _temp228 >= 75u){ _throw( Null_Exception);}
_temp227[ _temp228];}); ++ i;} if(({ struct Cyc_Lex_Trie* _temp229= t; if(
_temp229 == 0){ _throw( Null_Exception);} _temp229->shared_str;}) == - 1){
struct _tagged_string newstr= Cyc_Core_new_string( len + 1); Cyc_String_zstrncpy(
newstr, 0, buff, offset,( unsigned int) len);{ int ans=(( int(*)( struct Cyc_Xarray_Xarray*,
struct _tagged_string*)) Cyc_Xarray_add_ind)(( struct Cyc_Xarray_Xarray*)({
struct Cyc_Xarray_Xarray* _temp230= Cyc_Lex_symbols; if( _temp230 == 0){ _throw(
Null_Exception);} _temp230;}),({ struct _tagged_string* _temp231=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp231[ 0]=
newstr; _temp231;}));({ struct Cyc_Lex_Trie* _temp232= t; if( _temp232 == 0){
_throw( Null_Exception);} _temp232->shared_str= ans;}); return ans;}} return({
struct Cyc_Lex_Trie* _temp233= t; if( _temp233 == 0){ _throw( Null_Exception);}
_temp233->shared_str;});} static int Cyc_Lex_str_index_lbuf( struct Cyc_Lexing_lexbuf*
lbuf){ return Cyc_Lex_str_index( lbuf->lex_buffer, lbuf->lex_start_pos, lbuf->lex_curr_pos
- lbuf->lex_start_pos);} static void Cyc_Lex_insert_typedef( struct
_tagged_string* sptr){ struct _tagged_string s=* sptr; int len=( int)(({ struct
_tagged_string _temp234= s;( unsigned int)( _temp234.last_plus_one - _temp234.curr);})
- 1); struct Cyc_Lex_Trie* t= Cyc_Lex_typedefs_trie;{ int i= 0; for( 0; i < len;
++ i){ int ch=( int)({ struct _tagged_string _temp235= s; char* _temp237=
_temp235.curr + i; if( _temp235.base == 0? 1:( _temp237 < _temp235.base? 1:
_temp237 >= _temp235.last_plus_one)){ _throw( Null_Exception);}* _temp237;}) -
48; if(({ struct Cyc_Lex_Trie* _temp238= t; if( _temp238 == 0){ _throw(
Null_Exception);} _temp238->children;}) == 0){ while( i < len) {({ struct Cyc_Lex_Trie*
_temp239= t; if( _temp239 == 0){ _throw( Null_Exception);} _temp239->children=(
struct Cyc_Lex_Trie***)({ struct _tuple10* _temp240=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp240->f1=({ unsigned int _temp241= 75u;
struct Cyc_Lex_Trie** _temp242=( struct Cyc_Lex_Trie**) GC_malloc( sizeof(
struct Cyc_Lex_Trie*) * _temp241); unsigned int i; for( i= 0; i < _temp241; i ++){
_temp242[ i]= 0;} _temp242;}); _temp240;});});({ struct Cyc_Lex_Trie** _temp245=({
struct Cyc_Lex_Trie*** _temp244=({ struct Cyc_Lex_Trie* _temp243= t; if(
_temp243 == 0){ _throw( Null_Exception);} _temp243->children;}); if( _temp244 ==
0){ _throw( Null_Exception);}* _temp244;}); unsigned int _temp246= ch; if(
_temp246 >= 75u){ _throw( Null_Exception);} _temp245[ _temp246]=({ struct Cyc_Lex_Trie*
_temp247=( struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie));
_temp247->children= 0; _temp247->shared_str= 0; _temp247;});}); t=({ struct Cyc_Lex_Trie**
_temp250=({ struct Cyc_Lex_Trie*** _temp249=({ struct Cyc_Lex_Trie* _temp248= t;
if( _temp248 == 0){ _throw( Null_Exception);} _temp248->children;}); if(
_temp249 == 0){ _throw( Null_Exception);}* _temp249;}); unsigned int _temp251=
ch; if( _temp251 >= 75u){ _throw( Null_Exception);} _temp250[ _temp251];}); ++ i;
ch=( int)({ struct _tagged_string _temp252= s; char* _temp254= _temp252.curr + i;
if( _temp252.base == 0? 1:( _temp254 < _temp252.base? 1: _temp254 >= _temp252.last_plus_one)){
_throw( Null_Exception);}* _temp254;}) - 48;}({ struct Cyc_Lex_Trie* _temp255= t;
if( _temp255 == 0){ _throw( Null_Exception);} _temp255->shared_str= 1;});
return;} if(({ struct Cyc_Lex_Trie** _temp258=({ struct Cyc_Lex_Trie*** _temp257=({
struct Cyc_Lex_Trie* _temp256= t; if( _temp256 == 0){ _throw( Null_Exception);}
_temp256->children;}); if( _temp257 == 0){ _throw( Null_Exception);}* _temp257;});
unsigned int _temp259= ch; if( _temp259 >= 75u){ _throw( Null_Exception);}
_temp258[ _temp259];}) == 0){({ struct Cyc_Lex_Trie** _temp262=({ struct Cyc_Lex_Trie***
_temp261=({ struct Cyc_Lex_Trie* _temp260= t; if( _temp260 == 0){ _throw(
Null_Exception);} _temp260->children;}); if( _temp261 == 0){ _throw(
Null_Exception);}* _temp261;}); unsigned int _temp263= ch; if( _temp263 >= 75u){
_throw( Null_Exception);} _temp262[ _temp263]=({ struct Cyc_Lex_Trie* _temp264=(
struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp264->children=
0; _temp264->shared_str= 0; _temp264;});});} t=({ struct Cyc_Lex_Trie** _temp267=({
struct Cyc_Lex_Trie*** _temp266=({ struct Cyc_Lex_Trie* _temp265= t; if(
_temp265 == 0){ _throw( Null_Exception);} _temp265->children;}); if( _temp266 ==
0){ _throw( Null_Exception);}* _temp266;}); unsigned int _temp268= ch; if(
_temp268 >= 75u){ _throw( Null_Exception);} _temp267[ _temp268];});}}({ struct
Cyc_Lex_Trie* _temp269= t; if( _temp269 == 0){ _throw( Null_Exception);}
_temp269->shared_str= 1;}); return;} static struct _tagged_string* Cyc_Lex_get_symbol(
int symbol_num){ return(( struct _tagged_string*(*)( struct Cyc_Xarray_Xarray*,
int)) Cyc_Xarray_get)(( struct Cyc_Xarray_Xarray*)({ struct Cyc_Xarray_Xarray*
_temp270= Cyc_Lex_symbols; if( _temp270 == 0){ _throw( Null_Exception);}
_temp270;}), symbol_num);} static int Cyc_Lex_int_of_char( char c){ if('0' <= c?
c <='9': 0){ return c -'0';} else{ if('a' <= c? c <='f': 0){ return( 10 + c) -'a';}
else{ if('A' <= c? c <='F': 0){ return( 10 + c) -'A';} else{( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Core_InvalidArg_struct* _temp271=( struct
Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp271[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp272; _temp272.tag= Cyc_Core_InvalidArg_tag;
_temp272.f1=( struct _tagged_string)({ char* _temp273=( char*)"string to integer conversion";
struct _tagged_string _temp274; _temp274.curr= _temp273; _temp274.base= _temp273;
_temp274.last_plus_one= _temp273 + 29; _temp274;}); _temp272;}); _temp271;}));}}}}
struct _tuple3* Cyc_Lex_intconst( struct Cyc_Lexing_lexbuf* lbuf, int start, int
base){ unsigned int n= 0; int end= lbuf->lex_curr_pos; struct _tagged_string
buff= lbuf->lex_buffer; int i= start + lbuf->lex_start_pos;{ int i= start + lbuf->lex_start_pos;
for( 0; i < end; ++ i){ char c=({ struct _tagged_string _temp275= buff; char*
_temp277= _temp275.curr + i; if( _temp275.base == 0? 1:( _temp277 < _temp275.base?
1: _temp277 >= _temp275.last_plus_one)){ _throw( Null_Exception);}* _temp277;});
switch( c){ case 'u': _LL278: goto _LL279; case 'U': _LL279: return({ struct
_tuple3* _temp281=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3));
_temp281->f1=( void*) Cyc_Absyn_Unsigned; _temp281->f2=( int) n; _temp281;});
case 'l': _LL280: break; case 'L': _LL282: break; default: _LL283: n= n * base +(
unsigned int) Cyc_Lex_int_of_char( c); break;}}} return({ struct _tuple3*
_temp285=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3)); _temp285->f1=(
void*) Cyc_Absyn_Signed; _temp285->f2=( int) n; _temp285;});} static char Cyc_Lex_char_for_octal_code(
struct Cyc_Lexing_lexbuf* lb, int start, int howmany){ int c= 0;{ int i= 0; for(
0; i < howmany; i ++){ c=( 8 * c +(( char(*)( struct Cyc_Lexing_lexbuf*, int))
Cyc_Lexing_lexeme_char)( lb, start + i)) - 48;}} return( char) c;} static char
Cyc_Lex_char_for_hex_code( struct _tagged_string s, int start){ int c= 0; int
len=( int) Cyc_String_strlen( s);{ int i= 0; for( 0; start + i < len; i ++){
char x=({ struct _tagged_string _temp286= s; char* _temp288= _temp286.curr +(
start + i); if( _temp286.base == 0? 1:( _temp288 < _temp286.base? 1: _temp288 >=
_temp286.last_plus_one)){ _throw( Null_Exception);}* _temp288;}); if('0' <= x? x
<='9': 0){ c=( 16 * c +( int) x) -( int)'0';} else{ if('A' <= x? x <='F': 0){ c=(
16 * c +( int) x) -( int)'A';} else{ if('a' <= x? x <='f': 0){ c=( 16 * c +( int)
x) -( int)'a';} else{ break;}}}}} return( char) c;} static char _temp291[ 11u]="xxxxxxxxxx";
struct _tagged_string Cyc_Lex_string_buffer=( struct _tagged_string){ _temp291,
_temp291, _temp291 + 11u}; int Cyc_Lex_string_pos= 0; void Cyc_Lex_store_string_char(
char c){ int sz=( int)({ struct _tagged_string _temp292= Cyc_Lex_string_buffer;(
unsigned int)( _temp292.last_plus_one - _temp292.curr);}); if( Cyc_Lex_string_pos
>= sz){ int newsz= sz; while( Cyc_Lex_string_pos >= newsz) { newsz= newsz * 2;}{
struct _tagged_string str=({ unsigned int _temp296=( unsigned int) newsz; char*
_temp297=( char*) GC_malloc_atomic( sizeof( char) * _temp296); unsigned int i;
struct _tagged_string _temp298={ _temp297, _temp297, _temp297 + _temp296}; for(
i= 0; i < _temp296; i ++){ _temp297[ i]= i < sz?({ struct _tagged_string
_temp293= Cyc_Lex_string_buffer; char* _temp295= _temp293.curr +( int) i; if(
_temp293.base == 0? 1:( _temp295 < _temp293.base? 1: _temp295 >= _temp293.last_plus_one)){
_throw( Null_Exception);}* _temp295;}):'\000';} _temp298;}); Cyc_Lex_string_buffer=
str;}}({ struct _tagged_string _temp299= Cyc_Lex_string_buffer; char* _temp301=
_temp299.curr + Cyc_Lex_string_pos; if( _temp299.base == 0? 1:( _temp301 <
_temp299.base? 1: _temp301 >= _temp299.last_plus_one)){ _throw( Null_Exception);}*
_temp301= c;}); ++ Cyc_Lex_string_pos;} struct _tagged_string Cyc_Lex_get_stored_string(){
struct _tagged_string str= Cyc_String_substring( Cyc_Lex_string_buffer, 0,(
unsigned int) Cyc_Lex_string_pos); Cyc_Lex_string_pos= 0; return str;} struct
Cyc_Lex_Ldecls{ struct Cyc_Set_Set* typedefs; struct Cyc_Set_Set* namespaces; }
; typedef struct Cyc_Lex_Ldecls* Cyc_Lex_ldecls_t; struct Cyc_Lex_Lvis{ struct
Cyc_List_List* current_namespace; struct Cyc_List_List* imported_namespaces; } ;
typedef struct Cyc_Lex_Lvis* Cyc_Lex_lvis; struct Cyc_Lex_Lstate{ struct Cyc_List_List*
lstack; struct Cyc_Dict_Dict* decls; } ; typedef struct Cyc_Lex_Lstate* Cyc_Lex_lstate_t;
static struct Cyc_Core_Opt* Cyc_Lex_lstate= 0; static void Cyc_Lex_typedef_init(){
struct Cyc_Lex_Lvis* init_lvis=({ struct Cyc_Lex_Lvis* _temp302=( struct Cyc_Lex_Lvis*)
GC_malloc( sizeof( struct Cyc_Lex_Lvis)); _temp302->current_namespace= 0;
_temp302->imported_namespaces= 0; _temp302;}); struct Cyc_List_List* init_lstack=({
struct Cyc_List_List* _temp303=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp303->hd=( void*) init_lvis; _temp303->tl= 0;
_temp303;}); struct Cyc_Dict_Dict* init_decls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Lex_Ldecls* data))
Cyc_Dict_insert)((( struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*,
struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp), 0,({ struct
Cyc_Lex_Ldecls* _temp304=( struct Cyc_Lex_Ldecls*) GC_malloc( sizeof( struct Cyc_Lex_Ldecls));
_temp304->typedefs=(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tagged_string*,
struct _tagged_string*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp); _temp304->namespaces=((
struct Cyc_Set_Set*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp); _temp304;})); Cyc_Lex_lstate=({
struct Cyc_Core_Opt* _temp305=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp305->v=( void*)({ struct Cyc_Lex_Lstate* _temp306=( struct
Cyc_Lex_Lstate*) GC_malloc( sizeof( struct Cyc_Lex_Lstate)); _temp306->lstack=
init_lstack; _temp306->decls= init_decls; _temp306;}); _temp305;});} static
struct Cyc_List_List* Cyc_Lex_get_absolute_namespace( struct Cyc_List_List* ns){
struct _tagged_string* n=( struct _tagged_string*) ns->hd;{ struct Cyc_List_List*
ls=( struct Cyc_List_List*)(( struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt*
_temp307= Cyc_Lex_lstate; if( _temp307 == 0){ _throw( Null_Exception);} _temp307->v;}))->lstack;
for( 0; ls != 0; ls=({ struct Cyc_List_List* _temp308= ls; if( _temp308 == 0){
_throw( Null_Exception);} _temp308->tl;})){ struct Cyc_Lex_Lvis* lv=( struct Cyc_Lex_Lvis*)({
struct Cyc_List_List* _temp309= ls; if( _temp309 == 0){ _throw( Null_Exception);}
_temp309->hd;}); struct Cyc_List_List* x=({ struct Cyc_List_List* _temp310=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp310->hd=(
void*) lv->current_namespace; _temp310->tl= lv->imported_namespaces; _temp310;});
for( 0; x != 0; x=({ struct Cyc_List_List* _temp311= x; if( _temp311 == 0){
_throw( Null_Exception);} _temp311->tl;})){ struct Cyc_Lex_Ldecls* ld=(( struct
Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(((
struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt* _temp312= Cyc_Lex_lstate; if(
_temp312 == 0){ _throw( Null_Exception);} _temp312->v;}))->decls,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp313= x; if( _temp313 == 0){ _throw( Null_Exception);}
_temp313->hd;})); if((( int(*)( struct Cyc_Set_Set* s, struct _tagged_string*
elt)) Cyc_Set_member)( ld->namespaces, n)){ return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(( struct Cyc_List_List*)({
struct Cyc_List_List* _temp314= x; if( _temp314 == 0){ _throw( Null_Exception);}
_temp314->hd;}),( struct Cyc_List_List*) ns);}}}} Cyc_yyerror(({ struct
_tagged_string _temp317= Cyc_String_str_sepstr(( struct Cyc_List_List*) ns,(
struct _tagged_string)({ char* _temp315=( char*)"::"; struct _tagged_string
_temp316; _temp316.curr= _temp315; _temp316.base= _temp315; _temp316.last_plus_one=
_temp315 + 3; _temp316;})); xprintf("undeclared namespace %.*s", _temp317.last_plus_one
- _temp317.curr, _temp317.curr);})); return 0;} static void Cyc_Lex_recompute_typedefs(){
Cyc_Lex_typedefs_trie=({ struct Cyc_Lex_Trie* _temp318=( struct Cyc_Lex_Trie*)
GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp318->children= 0; _temp318->shared_str=
0; _temp318;});{ struct Cyc_List_List* ls=( struct Cyc_List_List*)(( struct Cyc_Lex_Lstate*)({
struct Cyc_Core_Opt* _temp319= Cyc_Lex_lstate; if( _temp319 == 0){ _throw(
Null_Exception);} _temp319->v;}))->lstack; for( 0; ls != 0; ls=({ struct Cyc_List_List*
_temp320= ls; if( _temp320 == 0){ _throw( Null_Exception);} _temp320->tl;})){
struct Cyc_Lex_Lvis* lv=( struct Cyc_Lex_Lvis*)({ struct Cyc_List_List* _temp321=
ls; if( _temp321 == 0){ _throw( Null_Exception);} _temp321->hd;}); struct Cyc_List_List*
x=({ struct Cyc_List_List* _temp322=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp322->hd=( void*) lv->current_namespace; _temp322->tl=
lv->imported_namespaces; _temp322;}); for( 0; x != 0; x=({ struct Cyc_List_List*
_temp323= x; if( _temp323 == 0){ _throw( Null_Exception);} _temp323->tl;})){
struct Cyc_Lex_Ldecls* ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)({ struct
Cyc_Core_Opt* _temp324= Cyc_Lex_lstate; if( _temp324 == 0){ _throw(
Null_Exception);} _temp324->v;}))->decls,( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp325= x; if( _temp325 == 0){ _throw( Null_Exception);} _temp325->hd;}));((
void(*)( void(* f)( struct _tagged_string*), struct Cyc_Set_Set* s)) Cyc_Set_iter)(
Cyc_Lex_insert_typedef, ld->typedefs);}}}} static int Cyc_Lex_is_typedef_in_namespace(
struct Cyc_List_List* ns, struct _tagged_string* v){ struct Cyc_List_List* ans=
Cyc_Lex_get_absolute_namespace( ns); struct _handler_cons _temp326;
_push_handler(& _temp326);{ struct _xtunion_struct* _temp327=( struct
_xtunion_struct*) setjmp( _temp326.handler); if( ! _temp327){{ struct Cyc_Lex_Ldecls*
ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt*
_temp328= Cyc_Lex_lstate; if( _temp328 == 0){ _throw( Null_Exception);} _temp328->v;}))->decls,
ans); int _temp329=(( int(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt))
Cyc_Set_member)( ld->typedefs, v); _npop_handler( 0u); return _temp329;};
_pop_handler();} else{ struct _xtunion_struct* _temp331= _temp327; _LL333: if(
_temp331->tag == Cyc_Dict_Absent_tag){ goto _LL334;} else{ goto _LL335;} _LL335:
goto _LL336; _LL334: return 0; _LL336:( void) _throw( _temp331); _LL332:;}}}
static int Cyc_Lex_is_typedef( struct Cyc_List_List* ns, struct _tagged_string*
v){ if( ns != 0){ return Cyc_Lex_is_typedef_in_namespace(( struct Cyc_List_List*)({
struct Cyc_List_List* _temp337= ns; if( _temp337 == 0){ _throw( Null_Exception);}
_temp337;}), v);}{ struct _tagged_string s=* v; int len=( int)(({ struct
_tagged_string _temp338= s;( unsigned int)( _temp338.last_plus_one - _temp338.curr);})
- 1); struct Cyc_Lex_Trie* t= Cyc_Lex_typedefs_trie;{ int i= 0; for( 0; i < len;
++ i){ if( t == 0? 1:({ struct Cyc_Lex_Trie* _temp339= t; if( _temp339 == 0){
_throw( Null_Exception);} _temp339->children;}) == 0){ return 0;} else{ t=({
struct Cyc_Lex_Trie** _temp345=({ struct Cyc_Lex_Trie*** _temp341=({ struct Cyc_Lex_Trie*
_temp340= t; if( _temp340 == 0){ _throw( Null_Exception);} _temp340->children;});
if( _temp341 == 0){ _throw( Null_Exception);}* _temp341;}); unsigned int
_temp346=( int)({ struct _tagged_string _temp342= s; char* _temp344= _temp342.curr
+ i; if( _temp342.base == 0? 1:( _temp344 < _temp342.base? 1: _temp344 >=
_temp342.last_plus_one)){ _throw( Null_Exception);}* _temp344;}) - 48; if(
_temp346 >= 75u){ _throw( Null_Exception);} _temp345[ _temp346];});}}} if( t ==
0){ return 0;} return({ struct Cyc_Lex_Trie* _temp347= t; if( _temp347 == 0){
_throw( Null_Exception);} _temp347->shared_str;});}} void Cyc_Lex_enter_namespace(
struct _tagged_string* s){ struct Cyc_List_List* ns=(( struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt* _temp348= Cyc_Lex_lstate; if(
_temp348 == 0){ _throw( Null_Exception);} _temp348->v;}))->lstack)->hd)->current_namespace;
struct Cyc_List_List* new_ns=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( ns,({ struct Cyc_List_List*
_temp349=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp349->hd=( void*) s; _temp349->tl= 0; _temp349;}));(( struct Cyc_Lex_Lstate*)({
struct Cyc_Core_Opt* _temp350= Cyc_Lex_lstate; if( _temp350 == 0){ _throw(
Null_Exception);} _temp350->v;}))->lstack=({ struct Cyc_List_List* _temp351=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp351->hd=(
void*)({ struct Cyc_Lex_Lvis* _temp353=( struct Cyc_Lex_Lvis*) GC_malloc(
sizeof( struct Cyc_Lex_Lvis)); _temp353->current_namespace= new_ns; _temp353->imported_namespaces=
0; _temp353;}); _temp351->tl=( struct Cyc_List_List*)(( struct Cyc_Lex_Lstate*)({
struct Cyc_Core_Opt* _temp352= Cyc_Lex_lstate; if( _temp352 == 0){ _throw(
Null_Exception);} _temp352->v;}))->lstack; _temp351;});{ struct Cyc_Lex_Ldecls*
ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt*
_temp354= Cyc_Lex_lstate; if( _temp354 == 0){ _throw( Null_Exception);} _temp354->v;}))->decls,
ns); if( !(( int(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)(
ld->namespaces, s)){(( struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt* _temp355=
Cyc_Lex_lstate; if( _temp355 == 0){ _throw( Null_Exception);} _temp355->v;}))->decls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Lex_Ldecls* data)) Cyc_Dict_insert)((( struct Cyc_Lex_Lstate*)({
struct Cyc_Core_Opt* _temp356= Cyc_Lex_lstate; if( _temp356 == 0){ _throw(
Null_Exception);} _temp356->v;}))->decls, ns,({ struct Cyc_Lex_Ldecls* _temp357=(
struct Cyc_Lex_Ldecls*) GC_malloc( sizeof( struct Cyc_Lex_Ldecls)); _temp357->typedefs=
ld->typedefs; _temp357->namespaces=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_insert)( ld->namespaces, s); _temp357;}));((
struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt* _temp358= Cyc_Lex_lstate; if(
_temp358 == 0){ _throw( Null_Exception);} _temp358->v;}))->decls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Lex_Ldecls* data))
Cyc_Dict_insert)((( struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt* _temp359= Cyc_Lex_lstate;
if( _temp359 == 0){ _throw( Null_Exception);} _temp359->v;}))->decls, new_ns,({
struct Cyc_Lex_Ldecls* _temp360=( struct Cyc_Lex_Ldecls*) GC_malloc( sizeof(
struct Cyc_Lex_Ldecls)); _temp360->typedefs=(( struct Cyc_Set_Set*(*)( int(*
comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp);
_temp360->namespaces=(( struct Cyc_Set_Set*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp);
_temp360;}));} Cyc_Lex_recompute_typedefs();}} void Cyc_Lex_leave_namespace(){((
struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt* _temp361= Cyc_Lex_lstate; if(
_temp361 == 0){ _throw( Null_Exception);} _temp361->v;}))->lstack=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp363=((( struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt*
_temp362= Cyc_Lex_lstate; if( _temp362 == 0){ _throw( Null_Exception);} _temp362->v;}))->lstack)->tl;
if( _temp363 == 0){ _throw( Null_Exception);} _temp363;}); Cyc_Lex_recompute_typedefs();}
void Cyc_Lex_enter_using( struct _tuple0* q){ struct Cyc_List_List* ns;{ void*
_temp364=(* q).f1; struct Cyc_List_List* _temp372; struct Cyc_List_List*
_temp374; _LL366: if(( int) _temp364 == Cyc_Absyn_Loc_n){ goto _LL367;} else{
goto _LL368;} _LL368: if(( unsigned int) _temp364 > 1u?(( struct _tunion_struct*)
_temp364)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL373: _temp372=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp364)->f1; goto _LL369;} else{ goto _LL370;}
_LL370: if(( unsigned int) _temp364 > 1u?(( struct _tunion_struct*) _temp364)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL375: _temp374=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp364)->f1; goto _LL371;} else{ goto _LL365;} _LL367:
ns=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp376=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp376->hd=( void*)(* q).f2;
_temp376->tl= 0; _temp376;}); goto _LL365; _LL369: _temp374= _temp372; goto
_LL371; _LL371: ns=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp378=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp374,({ struct Cyc_List_List* _temp377=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp377->hd=( void*)(* q).f2; _temp377->tl= 0;
_temp377;})); if( _temp378 == 0){ _throw( Null_Exception);} _temp378;}); goto
_LL365; _LL365:;}{ struct Cyc_List_List* ans= Cyc_Lex_get_absolute_namespace( ns);
struct Cyc_List_List* imported=(( struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)({
struct Cyc_Core_Opt* _temp379= Cyc_Lex_lstate; if( _temp379 == 0){ _throw(
Null_Exception);} _temp379->v;}))->lstack)->hd)->imported_namespaces;(( struct
Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt* _temp380= Cyc_Lex_lstate;
if( _temp380 == 0){ _throw( Null_Exception);} _temp380->v;}))->lstack)->hd)->imported_namespaces=({
struct Cyc_List_List* _temp381=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp381->hd=( void*) ans; _temp381->tl= imported;
_temp381;}); Cyc_Lex_recompute_typedefs();}} void Cyc_Lex_leave_using(){ struct
Cyc_List_List* imported=(( struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)({
struct Cyc_Core_Opt* _temp382= Cyc_Lex_lstate; if( _temp382 == 0){ _throw(
Null_Exception);} _temp382->v;}))->lstack)->hd)->imported_namespaces;(( struct
Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt* _temp383= Cyc_Lex_lstate;
if( _temp383 == 0){ _throw( Null_Exception);} _temp383->v;}))->lstack)->hd)->imported_namespaces=({
struct Cyc_List_List* _temp384= imported; if( _temp384 == 0){ _throw(
Null_Exception);} _temp384->tl;}); Cyc_Lex_recompute_typedefs();} void Cyc_Lex_register_typedef(
struct _tuple0* q){ struct Cyc_List_List* current=(( struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt* _temp385= Cyc_Lex_lstate; if(
_temp385 == 0){ _throw( Null_Exception);} _temp385->v;}))->lstack)->hd)->current_namespace;
struct Cyc_Dict_Dict* ds=(( struct Cyc_Lex_Lstate*)({ struct Cyc_Core_Opt*
_temp386= Cyc_Lex_lstate; if( _temp386 == 0){ _throw( Null_Exception);} _temp386->v;}))->decls;
struct Cyc_Lex_Ldecls* ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( ds, current); struct Cyc_Lex_Ldecls*
new_ld=({ struct Cyc_Lex_Ldecls* _temp387=( struct Cyc_Lex_Ldecls*) GC_malloc(
sizeof( struct Cyc_Lex_Ldecls)); _temp387->typedefs=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)( ld->typedefs,(*
q).f2); _temp387->namespaces= ld->namespaces; _temp387;});(( struct Cyc_Lex_Lstate*)({
struct Cyc_Core_Opt* _temp388= Cyc_Lex_lstate; if( _temp388 == 0){ _throw(
Null_Exception);} _temp388->v;}))->decls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Lex_Ldecls* data)) Cyc_Dict_insert)( ds,
current, new_ld); Cyc_Lex_insert_typedef((* q).f2);} static short Cyc_Lex_process_id(
struct Cyc_Lexing_lexbuf* lbuf){ int symbol_num=(( int(*)( struct Cyc_Lexing_lexbuf*
lbuf)) Cyc_Lex_str_index_lbuf)( lbuf); if( symbol_num < Cyc_Lex_num_kws){ return(
short)({ struct _tagged_ptr0 _temp389= Cyc_Lex_kw_nums; int* _temp391= _temp389.curr
+ symbol_num; if( _temp389.base == 0? 1:( _temp391 < _temp389.base? 1: _temp391
>= _temp389.last_plus_one)){ _throw( Null_Exception);}* _temp391;});}{ struct
_tagged_string* s= Cyc_Lex_get_symbol( symbol_num); if( Cyc_Lex_is_typedef( 0, s)){
Cyc_Lex_token_qvar=({ struct _tuple0* _temp392=( struct _tuple0*) GC_malloc(
sizeof( struct _tuple0)); _temp392->f1=( void*)({ struct Cyc_Absyn_Rel_n_struct*
_temp393=( struct Cyc_Absyn_Rel_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp393[ 0]=({ struct Cyc_Absyn_Rel_n_struct _temp394; _temp394.tag= Cyc_Absyn_Rel_n_tag;
_temp394.f1= 0; _temp394;}); _temp393;}); _temp392->f2= s; _temp392;}); return
(short)350;} Cyc_Lex_token_string=* s; return (short)343;}} static short Cyc_Lex_process_qual_id(
struct Cyc_Lexing_lexbuf* lbuf){ int i= lbuf->lex_start_pos; int end= lbuf->lex_curr_pos;
struct _tagged_string s= lbuf->lex_buffer; struct Cyc_List_List* rev_vs= 0;
while( i < end) { int start= i; for( 0; i < end?({ struct _tagged_string
_temp395= s; char* _temp397= _temp395.curr + i; if( _temp395.base == 0? 1:(
_temp397 < _temp395.base? 1: _temp397 >= _temp395.last_plus_one)){ _throw(
Null_Exception);}* _temp397;}) !=':': 0; i ++){;} if( start == i){( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp398=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp398[ 0]=({ struct Cyc_Core_Impossible_struct _temp399; _temp399.tag= Cyc_Core_Impossible_tag;
_temp399.f1=( struct _tagged_string)({ char* _temp400=( char*)"bad namespace";
struct _tagged_string _temp401; _temp401.curr= _temp400; _temp401.base= _temp400;
_temp401.last_plus_one= _temp400 + 14; _temp401;}); _temp399;}); _temp398;}));}{
int vlen= i - start; struct _tagged_string* v= Cyc_Lex_get_symbol( Cyc_Lex_str_index(
s, start, vlen)); rev_vs=({ struct Cyc_List_List* _temp402=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp402->hd=( void*) v; _temp402->tl=
rev_vs; _temp402;}); i += 2;}} if( rev_vs == 0){( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp403=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp403[ 0]=({ struct
Cyc_Core_Impossible_struct _temp404; _temp404.tag= Cyc_Core_Impossible_tag;
_temp404.f1=( struct _tagged_string)({ char* _temp405=( char*)"bad namespace";
struct _tagged_string _temp406; _temp406.curr= _temp405; _temp406.base= _temp405;
_temp406.last_plus_one= _temp405 + 14; _temp406;}); _temp404;}); _temp403;}));}{
struct _tagged_string* v=( struct _tagged_string*)({ struct Cyc_List_List*
_temp407= rev_vs; if( _temp407 == 0){ _throw( Null_Exception);} _temp407->hd;});
struct Cyc_List_List* vs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)(({ struct Cyc_List_List* _temp408= rev_vs; if( _temp408 == 0){
_throw( Null_Exception);} _temp408->tl;})); Cyc_Lex_token_qvar=({ struct _tuple0*
_temp409=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp409->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp410=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp410[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp411; _temp411.tag= Cyc_Absyn_Rel_n_tag; _temp411.f1= vs; _temp411;});
_temp410;}); _temp409->f2= v; _temp409;}); if( Cyc_Lex_is_typedef( vs, v)){
return (short)350;} else{ return (short)349;}}} extern int Cyc_Lex_token( struct
Cyc_Lexing_lexbuf*); extern int Cyc_Lex_strng( struct Cyc_Lexing_lexbuf*);
extern int Cyc_Lex_comment( struct Cyc_Lexing_lexbuf*); int Cyc_yylex(){ int ans=((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_token)(( struct Cyc_Lexing_lexbuf*)({
struct Cyc_Core_Opt* _temp412= Cyc_Parse_lbuf; if( _temp412 == 0){ _throw(
Null_Exception);} _temp412->v;})); Cyc_yylloc.first_line=(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme_start)(( struct Cyc_Lexing_lexbuf*)({ struct Cyc_Core_Opt*
_temp413= Cyc_Parse_lbuf; if( _temp413 == 0){ _throw( Null_Exception);} _temp413->v;}));
Cyc_yylloc.last_line=(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_end)((
struct Cyc_Lexing_lexbuf*)({ struct Cyc_Core_Opt* _temp414= Cyc_Parse_lbuf; if(
_temp414 == 0){ _throw( Null_Exception);} _temp414->v;})); switch( ans){ case
343: _LL415: Cyc_yylval=( struct _xtunion_struct*)({ struct Cyc_String_tok_struct*
_temp417=( struct Cyc_String_tok_struct*) GC_malloc( sizeof( struct Cyc_String_tok_struct));
_temp417[ 0]=({ struct Cyc_String_tok_struct _temp418; _temp418.tag= Cyc_String_tok_tag;
_temp418.f1= Cyc_Lex_token_string; _temp418;}); _temp417;}); break; case 349:
_LL416: Cyc_yylval=( struct _xtunion_struct*)({ struct Cyc_QualId_tok_struct*
_temp420=( struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));
_temp420[ 0]=({ struct Cyc_QualId_tok_struct _temp421; _temp421.tag= Cyc_QualId_tok_tag;
_temp421.f1= Cyc_Lex_token_qvar; _temp421;}); _temp420;}); break; case 350:
_LL419: Cyc_yylval=( struct _xtunion_struct*)({ struct Cyc_QualId_tok_struct*
_temp423=( struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));
_temp423[ 0]=({ struct Cyc_QualId_tok_struct _temp424; _temp424.tag= Cyc_QualId_tok_tag;
_temp424.f1= Cyc_Lex_token_qvar; _temp424;}); _temp423;}); break; case 348:
_LL422: Cyc_yylval=( struct _xtunion_struct*)({ struct Cyc_String_tok_struct*
_temp426=( struct Cyc_String_tok_struct*) GC_malloc( sizeof( struct Cyc_String_tok_struct));
_temp426[ 0]=({ struct Cyc_String_tok_struct _temp427; _temp427.tag= Cyc_String_tok_tag;
_temp427.f1= Cyc_Lex_token_string; _temp427;}); _temp426;}); break; case 344:
_LL425: Cyc_yylval=( struct _xtunion_struct*)({ struct Cyc_Int_tok_struct*
_temp429=( struct Cyc_Int_tok_struct*) GC_malloc( sizeof( struct Cyc_Int_tok_struct));
_temp429[ 0]=({ struct Cyc_Int_tok_struct _temp430; _temp430.tag= Cyc_Int_tok_tag;
_temp430.f1= Cyc_Lex_token_int; _temp430;}); _temp429;}); break; case 346:
_LL428: Cyc_yylval=( struct _xtunion_struct*)({ struct Cyc_Char_tok_struct*
_temp432=( struct Cyc_Char_tok_struct*) GC_malloc( sizeof( struct Cyc_Char_tok_struct));
_temp432[ 0]=({ struct Cyc_Char_tok_struct _temp433; _temp433.tag= Cyc_Char_tok_tag;
_temp433.f1= Cyc_Lex_token_char; _temp433;}); _temp432;}); break; case 347:
_LL431: Cyc_yylval=( struct _xtunion_struct*)({ struct Cyc_String_tok_struct*
_temp435=( struct Cyc_String_tok_struct*) GC_malloc( sizeof( struct Cyc_String_tok_struct));
_temp435[ 0]=({ struct Cyc_String_tok_struct _temp436; _temp436.tag= Cyc_String_tok_tag;
_temp436.f1= Cyc_Lex_token_string; _temp436;}); _temp435;}); break; case 345:
_LL434: Cyc_yylval=( struct _xtunion_struct*)({ struct Cyc_String_tok_struct*
_temp438=( struct Cyc_String_tok_struct*) GC_malloc( sizeof( struct Cyc_String_tok_struct));
_temp438[ 0]=({ struct Cyc_String_tok_struct _temp439; _temp439.tag= Cyc_String_tok_tag;
_temp439.f1= Cyc_Lex_token_string; _temp439;}); _temp438;}); break; default:
_LL437: break;} return ans;} struct Cyc_Lexing_lex_tables* Cyc_Lex_lt= 0; int
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
_temp441=( struct Cyc_Lexing_lex_tables*) GC_malloc( sizeof( struct Cyc_Lexing_lex_tables));
_temp441->lex_base=( struct _tagged_ptr0)({ int* _temp450=( int*)(( int*) Cyc_Lex_lbase);
struct _tagged_ptr0 _temp451; _temp451.curr= _temp450; _temp451.base= _temp450;
_temp451.last_plus_one= _temp450 + 151; _temp451;}); _temp441->lex_backtrk=(
struct _tagged_ptr0)({ int* _temp448=( int*)(( int*) Cyc_Lex_lbacktrk); struct
_tagged_ptr0 _temp449; _temp449.curr= _temp448; _temp449.base= _temp448;
_temp449.last_plus_one= _temp448 + 151; _temp449;}); _temp441->lex_default=(
struct _tagged_ptr0)({ int* _temp446=( int*)(( int*) Cyc_Lex_ldefault); struct
_tagged_ptr0 _temp447; _temp447.curr= _temp446; _temp447.base= _temp446;
_temp447.last_plus_one= _temp446 + 151; _temp447;}); _temp441->lex_trans=(
struct _tagged_ptr0)({ int* _temp444=( int*)(( int*) Cyc_Lex_ltrans); struct
_tagged_ptr0 _temp445; _temp445.curr= _temp444; _temp445.base= _temp444;
_temp445.last_plus_one= _temp444 + 1813; _temp445;}); _temp441->lex_check=(
struct _tagged_ptr0)({ int* _temp442=( int*)(( int*) Cyc_Lex_lcheck); struct
_tagged_ptr0 _temp443; _temp443.curr= _temp442; _temp443.base= _temp442;
_temp443.last_plus_one= _temp442 + 1813; _temp443;}); _temp441;});} lexstate=((
int(*)( struct Cyc_Lexing_lex_tables*, int, struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lex_engine)((
struct Cyc_Lexing_lex_tables*)({ struct Cyc_Lexing_lex_tables* _temp452= Cyc_Lex_lt;
if( _temp452 == 0){ _throw( Null_Exception);} _temp452;}), lexstate, lexbuf);
switch( lexstate){ case 0: _LL453: return( int)(( short(*)( struct Cyc_Lexing_lexbuf*
lbuf)) Cyc_Lex_process_id)( lexbuf); case 1: _LL454: return( int)(( short(*)(
struct Cyc_Lexing_lexbuf* lbuf)) Cyc_Lex_process_id)( lexbuf); case 2: _LL455:
return( int)(( short(*)( struct Cyc_Lexing_lexbuf* lbuf)) Cyc_Lex_process_qual_id)(
lexbuf); case 3: _LL456: Cyc_Lex_token_string=* Cyc_Lex_get_symbol((( int(*)(
struct Cyc_Lexing_lexbuf* lbuf)) Cyc_Lex_str_index_lbuf)( lexbuf)); return 348;
case 4: _LL457: Cyc_Lex_token_int=(( struct _tuple3*(*)( struct Cyc_Lexing_lexbuf*
lbuf, int start, int base)) Cyc_Lex_intconst)( lexbuf, 2, 16); return 344; case
5: _LL458: Cyc_Lex_token_int=(( struct _tuple3*(*)( struct Cyc_Lexing_lexbuf*
lbuf, int start, int base)) Cyc_Lex_intconst)( lexbuf, 0, 8); return 344; case 6:
_LL459: Cyc_Lex_token_int=(( struct _tuple3*(*)( struct Cyc_Lexing_lexbuf* lbuf,
int start, int base)) Cyc_Lex_intconst)( lexbuf, 0, 10); return 344; case 7:
_LL460: Cyc_Lex_token_int=(( struct _tuple3*(*)( struct Cyc_Lexing_lexbuf* lbuf,
int start, int base)) Cyc_Lex_intconst)( lexbuf, 0, 10); return 344; case 8:
_LL461: Cyc_Lex_token_string=(( struct _tagged_string(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme)( lexbuf); return 347; case 9: _LL462: Cyc_Lex_token_string=((
struct _tagged_string(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)( lexbuf);
return 347; case 10: _LL463: return 326; case 11: _LL464: return 327; case 12:
_LL465: return 324; case 13: _LL466: return 325; case 14: _LL467: return 320;
case 15: _LL468: return 321; case 16: _LL469: return 333; case 17: _LL470:
return 334; case 18: _LL471: return 330; case 19: _LL472: return 331; case 20:
_LL473: return 332; case 21: _LL474: return 339; case 22: _LL475: return 338;
case 23: _LL476: return 337; case 24: _LL477: return 335; case 25: _LL478:
return 336; case 26: _LL479: return 328; case 27: _LL480: return 329; case 28:
_LL481: return 322; case 29: _LL482: return 323; case 30: _LL483: return 341;
case 31: _LL484: return 319; case 32: _LL485: return 340; case 33: _LL486:
return 342; case 34: _LL487: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_token)(
lexbuf); case 35: _LL488: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_token)(
lexbuf); case 36: _LL489: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_token)(
lexbuf); case 37: _LL490: Cyc_Lex_comment_depth= 1; Cyc_Lex_runaway_start=(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_start)( lexbuf);(( int(*)( struct
Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_token)( lexbuf); case 38: _LL491: Cyc_Lex_string_pos= 0; Cyc_Lex_runaway_start=((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_start)( lexbuf);(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); Cyc_Lex_token_string= Cyc_Lex_get_stored_string();
return 345; case 39: _LL492: Cyc_Lex_token_char='\a'; return 346; case 40:
_LL493: Cyc_Lex_token_char='\b'; return 346; case 41: _LL494: Cyc_Lex_token_char='\f';
return 346; case 42: _LL495: Cyc_Lex_token_char='\n'; return 346; case 43:
_LL496: Cyc_Lex_token_char='\r'; return 346; case 44: _LL497: Cyc_Lex_token_char='\t';
return 346; case 45: _LL498: Cyc_Lex_token_char='\v'; return 346; case 46:
_LL499: Cyc_Lex_token_char='\\'; return 346; case 47: _LL500: Cyc_Lex_token_char='\'';
return 346; case 48: _LL501: Cyc_Lex_token_char='"'; return 346; case 49: _LL502:
Cyc_Lex_token_char='?'; return 346; case 50: _LL503: Cyc_Lex_token_char=(( char(*)(
struct Cyc_Lexing_lexbuf* lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)(
lexbuf, 2, 3); return 346; case 51: _LL504: Cyc_Lex_token_char=(( char(*)(
struct Cyc_Lexing_lexbuf* lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)(
lexbuf, 2, 2); return 346; case 52: _LL505: Cyc_Lex_token_char=(( char(*)(
struct Cyc_Lexing_lexbuf* lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)(
lexbuf, 2, 1); return 346; case 53: _LL506: Cyc_Lex_token_char= Cyc_Lex_char_for_hex_code(((
struct _tagged_string(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)( lexbuf),
3); return 346; case 54: _LL507: Cyc_Lex_token_char=(( char(*)( struct Cyc_Lexing_lexbuf*,
int)) Cyc_Lexing_lexeme_char)( lexbuf, 1); return 346; case 55: _LL508: return -
1; case 56: _LL509: return( int)(( char(*)( struct Cyc_Lexing_lexbuf*, int)) Cyc_Lexing_lexeme_char)(
lexbuf, 0); default: _LL510:( lexbuf->refill_buff)( lexbuf); return(( int(*)(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_token_rec)( lexbuf,
lexstate);}( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Lexing_Error_struct*
_temp512=( struct Cyc_Lexing_Error_struct*) GC_malloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp512[ 0]=({ struct Cyc_Lexing_Error_struct _temp513; _temp513.tag= Cyc_Lexing_Error_tag;
_temp513.f1=( struct _tagged_string)({ char* _temp514=( char*)"some action didn't return!";
struct _tagged_string _temp515; _temp515.curr= _temp514; _temp515.base= _temp514;
_temp515.last_plus_one= _temp514 + 27; _temp515;}); _temp513;}); _temp512;}));}
int Cyc_Lex_token( struct Cyc_Lexing_lexbuf* lexbuf){ return(( int(*)( struct
Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_token_rec)( lexbuf, 0);} int
Cyc_Lex_strng_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ if( Cyc_Lex_lt
== 0){ Cyc_Lex_lt=({ struct Cyc_Lexing_lex_tables* _temp516=( struct Cyc_Lexing_lex_tables*)
GC_malloc( sizeof( struct Cyc_Lexing_lex_tables)); _temp516->lex_base=( struct
_tagged_ptr0)({ int* _temp525=( int*)(( int*) Cyc_Lex_lbase); struct
_tagged_ptr0 _temp526; _temp526.curr= _temp525; _temp526.base= _temp525;
_temp526.last_plus_one= _temp525 + 151; _temp526;}); _temp516->lex_backtrk=(
struct _tagged_ptr0)({ int* _temp523=( int*)(( int*) Cyc_Lex_lbacktrk); struct
_tagged_ptr0 _temp524; _temp524.curr= _temp523; _temp524.base= _temp523;
_temp524.last_plus_one= _temp523 + 151; _temp524;}); _temp516->lex_default=(
struct _tagged_ptr0)({ int* _temp521=( int*)(( int*) Cyc_Lex_ldefault); struct
_tagged_ptr0 _temp522; _temp522.curr= _temp521; _temp522.base= _temp521;
_temp522.last_plus_one= _temp521 + 151; _temp522;}); _temp516->lex_trans=(
struct _tagged_ptr0)({ int* _temp519=( int*)(( int*) Cyc_Lex_ltrans); struct
_tagged_ptr0 _temp520; _temp520.curr= _temp519; _temp520.base= _temp519;
_temp520.last_plus_one= _temp519 + 1813; _temp520;}); _temp516->lex_check=(
struct _tagged_ptr0)({ int* _temp517=( int*)(( int*) Cyc_Lex_lcheck); struct
_tagged_ptr0 _temp518; _temp518.curr= _temp517; _temp518.base= _temp517;
_temp518.last_plus_one= _temp517 + 1813; _temp518;}); _temp516;});} lexstate=((
int(*)( struct Cyc_Lexing_lex_tables*, int, struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lex_engine)((
struct Cyc_Lexing_lex_tables*)({ struct Cyc_Lexing_lex_tables* _temp527= Cyc_Lex_lt;
if( _temp527 == 0){ _throw( Null_Exception);} _temp527;}), lexstate, lexbuf);
switch( lexstate){ case 0: _LL528: return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 1: _LL529: return 0; case 2: _LL530: return(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 3: _LL531: Cyc_Lex_store_string_char('\a');
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 4:
_LL532: Cyc_Lex_store_string_char('\b'); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 5: _LL533: Cyc_Lex_store_string_char('\f'); return((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 6: _LL534: Cyc_Lex_store_string_char('\n');
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 7:
_LL535: Cyc_Lex_store_string_char('\r'); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 8: _LL536: Cyc_Lex_store_string_char('\t'); return((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 9: _LL537: Cyc_Lex_store_string_char('\v');
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 10:
_LL538: Cyc_Lex_store_string_char('\\'); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 11: _LL539: Cyc_Lex_store_string_char('\'');
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 12:
_LL540: Cyc_Lex_store_string_char('"'); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 13: _LL541: Cyc_Lex_store_string_char('?'); return((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 14: _LL542:
Cyc_Lex_store_string_char((( char(*)( struct Cyc_Lexing_lexbuf* lb, int start,
int howmany)) Cyc_Lex_char_for_octal_code)( lexbuf, 1, 3)); return(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 15: _LL543: Cyc_Lex_store_string_char(((
char(*)( struct Cyc_Lexing_lexbuf* lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)(
lexbuf, 1, 2)); return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)(
lexbuf); case 16: _LL544: Cyc_Lex_store_string_char((( char(*)( struct Cyc_Lexing_lexbuf*
lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)( lexbuf, 1, 1));
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 17:
_LL545: Cyc_Lex_store_string_char( Cyc_Lex_char_for_hex_code((( struct
_tagged_string(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)( lexbuf), 2));
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 18:
_LL546: Cyc_Lex_store_string_char((( char(*)( struct Cyc_Lexing_lexbuf*, int))
Cyc_Lexing_lexeme_char)( lexbuf, 0)); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 19: _LL547:(( void(*)( struct _tagged_string msg,
struct Cyc_Lexing_lexbuf* lb)) Cyc_Lex_runaway_err)(( struct _tagged_string)({
char* _temp549=( char*)"string ends in newline"; struct _tagged_string _temp550;
_temp550.curr= _temp549; _temp550.base= _temp549; _temp550.last_plus_one=
_temp549 + 23; _temp550;}), lexbuf); return 0; case 20: _LL548:(( void(*)(
struct _tagged_string msg, struct Cyc_Lexing_lexbuf* lb)) Cyc_Lex_runaway_err)((
struct _tagged_string)({ char* _temp552=( char*)"unterminated string"; struct
_tagged_string _temp553; _temp553.curr= _temp552; _temp553.base= _temp552;
_temp553.last_plus_one= _temp552 + 20; _temp553;}), lexbuf); return 0; case 21:
_LL551:(( void(*)( struct _tagged_string msg, struct Cyc_Lexing_lexbuf* lb)) Cyc_Lex_err)((
struct _tagged_string)({ char* _temp555=( char*)"bad character following backslash in string";
struct _tagged_string _temp556; _temp556.curr= _temp555; _temp556.base= _temp555;
_temp556.last_plus_one= _temp555 + 44; _temp556;}), lexbuf); return(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); default: _LL554:( lexbuf->refill_buff)(
lexbuf); return(( int(*)( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_strng_rec)(
lexbuf, lexstate);}( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Lexing_Error_struct*
_temp558=( struct Cyc_Lexing_Error_struct*) GC_malloc( sizeof( struct Cyc_Lexing_Error_struct));
_temp558[ 0]=({ struct Cyc_Lexing_Error_struct _temp559; _temp559.tag= Cyc_Lexing_Error_tag;
_temp559.f1=( struct _tagged_string)({ char* _temp560=( char*)"some action didn't return!";
struct _tagged_string _temp561; _temp561.curr= _temp560; _temp561.base= _temp560;
_temp561.last_plus_one= _temp560 + 27; _temp561;}); _temp559;}); _temp558;}));}
int Cyc_Lex_strng( struct Cyc_Lexing_lexbuf* lexbuf){ return(( int(*)( struct
Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_strng_rec)( lexbuf, 1);} int
Cyc_Lex_comment_rec( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate){ if( Cyc_Lex_lt
== 0){ Cyc_Lex_lt=({ struct Cyc_Lexing_lex_tables* _temp562=( struct Cyc_Lexing_lex_tables*)
GC_malloc( sizeof( struct Cyc_Lexing_lex_tables)); _temp562->lex_base=( struct
_tagged_ptr0)({ int* _temp571=( int*)(( int*) Cyc_Lex_lbase); struct
_tagged_ptr0 _temp572; _temp572.curr= _temp571; _temp572.base= _temp571;
_temp572.last_plus_one= _temp571 + 151; _temp572;}); _temp562->lex_backtrk=(
struct _tagged_ptr0)({ int* _temp569=( int*)(( int*) Cyc_Lex_lbacktrk); struct
_tagged_ptr0 _temp570; _temp570.curr= _temp569; _temp570.base= _temp569;
_temp570.last_plus_one= _temp569 + 151; _temp570;}); _temp562->lex_default=(
struct _tagged_ptr0)({ int* _temp567=( int*)(( int*) Cyc_Lex_ldefault); struct
_tagged_ptr0 _temp568; _temp568.curr= _temp567; _temp568.base= _temp567;
_temp568.last_plus_one= _temp567 + 151; _temp568;}); _temp562->lex_trans=(
struct _tagged_ptr0)({ int* _temp565=( int*)(( int*) Cyc_Lex_ltrans); struct
_tagged_ptr0 _temp566; _temp566.curr= _temp565; _temp566.base= _temp565;
_temp566.last_plus_one= _temp565 + 1813; _temp566;}); _temp562->lex_check=(
struct _tagged_ptr0)({ int* _temp563=( int*)(( int*) Cyc_Lex_lcheck); struct
_tagged_ptr0 _temp564; _temp564.curr= _temp563; _temp564.base= _temp563;
_temp564.last_plus_one= _temp563 + 1813; _temp564;}); _temp562;});} lexstate=((
int(*)( struct Cyc_Lexing_lex_tables*, int, struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lex_engine)((
struct Cyc_Lexing_lex_tables*)({ struct Cyc_Lexing_lex_tables* _temp573= Cyc_Lex_lt;
if( _temp573 == 0){ _throw( Null_Exception);} _temp573;}), lexstate, lexbuf);
switch( lexstate){ case 0: _LL574: ++ Cyc_Lex_comment_depth; return(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf); case 1: _LL575: -- Cyc_Lex_comment_depth;
if( Cyc_Lex_comment_depth > 0){ return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)(
lexbuf);} return 0; case 2: _LL576:(( void(*)( struct _tagged_string msg, struct
Cyc_Lexing_lexbuf* lb)) Cyc_Lex_runaway_err)(( struct _tagged_string)({ char*
_temp578=( char*)"unterminated comment"; struct _tagged_string _temp579;
_temp579.curr= _temp578; _temp579.base= _temp578; _temp579.last_plus_one=
_temp578 + 21; _temp579;}), lexbuf); return 0; case 3: _LL577: return(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf); case 4: _LL580: return((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf); case 5: _LL581:
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf); default:
_LL582:( lexbuf->refill_buff)( lexbuf); return(( int(*)( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate)) Cyc_Lex_comment_rec)( lexbuf, lexstate);}( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Lexing_Error_struct* _temp584=( struct Cyc_Lexing_Error_struct*)
GC_malloc( sizeof( struct Cyc_Lexing_Error_struct)); _temp584[ 0]=({ struct Cyc_Lexing_Error_struct
_temp585; _temp585.tag= Cyc_Lexing_Error_tag; _temp585.f1=( struct
_tagged_string)({ char* _temp586=( char*)"some action didn't return!"; struct
_tagged_string _temp587; _temp587.curr= _temp586; _temp587.base= _temp586;
_temp587.last_plus_one= _temp586 + 27; _temp587;}); _temp585;}); _temp584;}));}
int Cyc_Lex_comment( struct Cyc_Lexing_lexbuf* lexbuf){ return(( int(*)( struct
Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_comment_rec)( lexbuf, 2);}
void Cyc_Lex_lex_init(){ Cyc_Lex_ids_trie=({ struct Cyc_Lex_Trie* _temp588=(
struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie) * 1); _temp588[ 0]=({
struct Cyc_Lex_Trie _temp589; _temp589.children= 0; _temp589.shared_str= - 1;
_temp589;}); _temp588;}); Cyc_Lex_typedefs_trie=({ struct Cyc_Lex_Trie* _temp590=(
struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie) * 1); _temp590[ 0]=({
struct Cyc_Lex_Trie _temp591; _temp591.children= 0; _temp591.shared_str= 0;
_temp591;}); _temp590;}); Cyc_Lex_symbols=( struct Cyc_Xarray_Xarray*)(( struct
Cyc_Xarray_Xarray*(*)( int, struct _tagged_string*)) Cyc_Xarray_create)( 101,({
struct _tagged_string* _temp592=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp592[ 0]=( struct _tagged_string)({ char*
_temp593=( char*)""; struct _tagged_string _temp594; _temp594.curr= _temp593;
_temp594.base= _temp593; _temp594.last_plus_one= _temp593 + 1; _temp594;});
_temp592;})); Cyc_Lex_num_kws=( int) 62u; Cyc_Lex_kw_nums=({ unsigned int
_temp595=( unsigned int) Cyc_Lex_num_kws; int* _temp596=( int*) GC_malloc_atomic(
sizeof( int) * _temp595); unsigned int i; struct _tagged_ptr0 _temp597={
_temp596, _temp596, _temp596 + _temp595}; for( i= 0; i < _temp595; i ++){
_temp596[ i]= 0;} _temp597;});{ int i= 0; for( 0; i < Cyc_Lex_num_kws; ++ i){
struct _tagged_string str=({ struct _tuple9* _temp598=( struct _tuple9*) Cyc_Lex_rw_array;
unsigned int _temp599= i; if( _temp599 >= 62u){ _throw( Null_Exception);}
_temp598[ _temp599];}).f1; Cyc_Lex_str_index( str, 0,( int) Cyc_String_strlen(
str));({ struct _tagged_ptr0 _temp600= Cyc_Lex_kw_nums; int* _temp602= _temp600.curr
+ i; if( _temp600.base == 0? 1:( _temp602 < _temp600.base? 1: _temp602 >=
_temp600.last_plus_one)){ _throw( Null_Exception);}* _temp602=( int)({ struct
_tuple9* _temp603=( struct _tuple9*) Cyc_Lex_rw_array; unsigned int _temp604= i;
if( _temp604 >= 62u){ _throw( Null_Exception);} _temp603[ _temp604];}).f2;});}}
Cyc_Lex_typedef_init(); Cyc_Lex_comment_depth= 0;}