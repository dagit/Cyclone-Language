#include "cyc_include.h"

 struct _tagged_ptr0{ int* curr; int* base; int* last_plus_one; } ; struct
_tuple0{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; struct _tuple1{
void* f1; struct _tagged_string* f2; } ; struct _tuple2{ struct Cyc_Core_Opt* f1;
struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple4{ struct _tagged_string* f1;
struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple5{ void* f1; struct Cyc_Core_Opt*
f2; } ; struct _tuple6{ struct Cyc_Absyn_Tqual* f1; void* f2; struct Cyc_List_List*
f3; struct Cyc_List_List* f4; } ; struct _tuple7{ struct _tuple1* f1; struct Cyc_Absyn_Tqual*
f2; void* f3; struct Cyc_List_List* f4; struct Cyc_List_List* f5; } ; struct
_tuple8{ struct _tuple7* f1; struct Cyc_Core_Opt* f2; } ; struct _tuple9{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; struct Cyc_List_List* f4;
} ; struct _tuple10{ struct Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple11{
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; struct _tuple12{
void* f1; int f2; } ; struct _tuple13{ struct Cyc_Parse_Declarator* f1; struct
Cyc_Absyn_Exp* f2; } ; struct _tuple14{ struct Cyc_Absyn_Tqual* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; struct _tuple15{ struct Cyc_Parse_Declarator*
f1; struct Cyc_Core_Opt* f2; } ; struct _tuple16{ struct Cyc_List_List* f1; int
f2; struct Cyc_Core_Opt* f3; } ; struct _tuple17{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Pat* f2; } ; struct _tagged_ptr1{ short* curr; short* base;
short* last_plus_one; } ; struct _tagged_ptr2{ struct _xtunion_struct** curr;
struct _xtunion_struct** base; struct _xtunion_struct** last_plus_one; } ;
struct _tagged_ptr3{ struct Cyc_Yyltype* curr; struct Cyc_Yyltype* base; struct
Cyc_Yyltype* last_plus_one; } ; struct _tuple18{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; typedef int Cyc_ptrdiff_t; typedef unsigned int
Cyc_size_t; typedef unsigned short Cyc_wchar_t; typedef unsigned int Cyc_wint_t;
typedef char Cyc_u_char; typedef unsigned short Cyc_u_short; typedef
unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long; typedef unsigned short
Cyc_ushort; typedef unsigned int Cyc_uint; typedef unsigned int Cyc_clock_t;
typedef int Cyc_time_t; struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct
Cyc_itimerspec{ struct Cyc_timespec it_interval; struct Cyc_timespec it_value; }
; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t; typedef unsigned int Cyc_ino_t;
typedef unsigned int Cyc_vm_offset_t; typedef unsigned int Cyc_vm_size_t;
typedef char Cyc_int8_t; typedef char Cyc_u_int8_t; typedef short Cyc_int16_t;
typedef unsigned short Cyc_u_int16_t; typedef int Cyc_int32_t; typedef
unsigned int Cyc_u_int32_t; typedef long long Cyc_int64_t; typedef
unsigned long long Cyc_u_int64_t; typedef int Cyc_register_t; typedef short Cyc_dev_t;
typedef int Cyc_off_t; typedef unsigned short Cyc_uid_t; typedef unsigned short
Cyc_gid_t; typedef int Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t;
typedef char* Cyc_addr_t; typedef int Cyc_mode_t; typedef unsigned short Cyc_nlink_t;
typedef int Cyc_fd_mask; struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ;
typedef struct Cyc__types_fd_set Cyc__types_fd_set; typedef char* Cyc_Cstring;
typedef struct _tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t;
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void*
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u]; struct
Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ; extern char
Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char* tag; } ;
extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; extern char Cyc_Lexing_Error_tag[
6u]; struct Cyc_Lexing_Error_struct{ char* tag; struct _tagged_string f1; } ;
struct Cyc_Lexing_lexbuf{ void(* refill_buff)( struct Cyc_Lexing_lexbuf*); void*
refill_state; struct _tagged_string lex_buffer; int lex_buffer_len; int
lex_abs_pos; int lex_start_pos; int lex_curr_pos; int lex_last_pos; int
lex_last_action; int lex_eof_reached; } ; typedef struct Cyc_Lexing_lexbuf* Cyc_Lexing_Lexbuf;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_string,
int, void*); void* read_fun_state; } ; typedef struct Cyc_Lexing_function_lexbuf_state*
Cyc_Lexing_Function_lexbuf_state; struct Cyc_Lexing_lex_tables{ struct
_tagged_ptr0 lex_base; struct _tagged_ptr0 lex_backtrk; struct _tagged_ptr0
lex_default; struct _tagged_ptr0 lex_trans; struct _tagged_ptr0 lex_check; } ;
typedef struct Cyc_Lexing_lex_tables* Cyc_Lexing_LexTables; extern struct Cyc_Lexing_lexbuf*
Cyc_Lexing_from_file( struct Cyc_Stdio___sFILE*); struct Cyc_List_List{ void* hd;
struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern void Cyc_List_iter(
void(* f)( void*), struct Cyc_List_List* x); extern void Cyc_List_iter_c( void(*
f)( void*, void*), void* env, struct Cyc_List_List* x); extern void* Cyc_List_fold_right(
void*(* f)( void*, void*), struct Cyc_List_List* x, void* accum); extern struct
Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_flatten( struct Cyc_List_List* x);
extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{ char* tag; } ;
extern struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct _tuple0 Cyc_List_split( struct Cyc_List_List* x); extern
unsigned int Cyc_String_strlen( struct _tagged_string s); extern int Cyc_String_strcmp(
struct _tagged_string s1, struct _tagged_string s2); extern int Cyc_String_zstrcmp(
struct _tagged_string, struct _tagged_string); extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); extern struct _tagged_string
Cyc_String_strcat( struct _tagged_string dest, struct _tagged_string src);
extern struct _tagged_string Cyc_String_strcpy( struct _tagged_string dest,
struct _tagged_string src); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t;
typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{ char* tag; }
; struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct
_tagged_string line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos*
Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[ 5u]; struct Cyc_Position_Exit_struct{
char* tag; } ; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment*
Cyc_Position_seg_t; extern struct Cyc_Position_Segment* Cyc_Position_segment_of_abs(
int, int); extern struct Cyc_Position_Segment* Cyc_Position_segment_join( struct
Cyc_Position_Segment*, struct Cyc_Position_Segment*); extern struct
_tagged_string Cyc_Position_string_of_segment( struct Cyc_Position_Segment*);
typedef void* Cyc_Position_Error_kind; static const unsigned int Cyc_Position_Lex=
0; static const unsigned int Cyc_Position_Parse= 1; static const unsigned int
Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{
struct _tagged_string source; struct Cyc_Position_Segment* seg; void* kind;
struct _tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern struct Cyc_Position_Error* Cyc_Position_mk_err_parse( struct Cyc_Position_Segment*,
struct _tagged_string); extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag; } ; extern void Cyc_Position_post_error( struct Cyc_Position_Error*);
typedef struct _tagged_string* Cyc_Absyn_field_name_t; typedef struct
_tagged_string* Cyc_Absyn_var_t; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
typedef void* Cyc_Absyn_Nmspace; typedef void* Cyc_Absyn_nmspace_t; typedef
struct _tuple1* Cyc_Absyn_qvar_t; typedef struct _tuple1* Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1* Cyc_Absyn_typedef_name_t; typedef struct _tuple1* Cyc_Absyn_typedef_name_opt_t;
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
attributes; } ; struct Cyc_Absyn_TunionInfo{ struct _tuple1* name; struct Cyc_List_List*
targs; void* rgn; struct Cyc_Absyn_Tuniondecl* tud; } ; struct Cyc_Absyn_XTunionInfo{
struct _tuple1* name; void* rgn; struct Cyc_Absyn_XTuniondecl* xtud; } ; struct
Cyc_Absyn_TunionFieldInfo{ struct _tuple1* name; struct Cyc_List_List* targs;
struct _tuple1* fname; struct Cyc_Absyn_Tuniondecl* tud; struct Cyc_Absyn_Tunionfield*
tufd; } ; struct Cyc_Absyn_XTunionFieldInfo{ struct _tuple1* name; struct
_tuple1* fname; struct Cyc_Absyn_XTuniondecl* xtud; struct Cyc_Absyn_Tunionfield*
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
struct Cyc_Absyn_StructType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Structdecl** f3; } ; static const int Cyc_Absyn_UnionType_tag=
12; struct Cyc_Absyn_UnionType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_EnumType_tag=
13; struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType_tag= 14; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType_tag= 15; struct
Cyc_Absyn_TypedefType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
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
int tag; struct _tuple1* f1; void* f2; } ; static const int Cyc_Absyn_UnknownId_e_tag=
2; struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple1* f1; } ; static
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
23; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e_tag= 24; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e_tag=
25; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e_tag=
26; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_Tunion_e_tag= 27; struct Cyc_Absyn_Tunion_e_struct{ int tag;
struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ; static
const int Cyc_Absyn_XTunion_e_tag= 28; struct Cyc_Absyn_XTunion_e_struct{ int
tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_XTuniondecl*
f3; struct Cyc_Absyn_Tunionfield* f4; } ; static const int Cyc_Absyn_Enum_e_tag=
29; struct Cyc_Absyn_Enum_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl*
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
int tag; struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s_tag=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s_tag= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s_tag= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s_tag= 8; struct Cyc_Absyn_For_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tuple3 f2; struct _tuple3 f3; struct
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
int tag; struct Cyc_Absyn_Stmt* f1; struct _tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s_tag=
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
struct Cyc_Absyn_Tunion_p_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; struct Cyc_Absyn_Tuniondecl*
f5; struct Cyc_Absyn_Tunionfield* f6; } ; static const int Cyc_Absyn_XTunion_p_tag=
9; struct Cyc_Absyn_XTunion_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_XTuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_p_tag= 10; struct Cyc_Absyn_Enum_p_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_UnknownId_p_tag= 11; struct Cyc_Absyn_UnknownId_p_struct{
int tag; struct _tuple1* f1; } ; static const int Cyc_Absyn_UnknownCall_p_tag=
12; struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple1* f1; struct
Cyc_List_List* f2; struct Cyc_List_List* f3; } ; static const int Cyc_Absyn_UnknownFields_p_tag=
13; struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple1* f1; struct
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
Cyc_Absyn_Vardecl{ void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual* tq;
void* type; struct Cyc_Absyn_Exp* initializer; int shadow; struct Cyc_Core_Opt*
region; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc;
int is_inline; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt*
body; struct Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structfield{ struct
_tagged_string* name; struct Cyc_Absyn_Tqual* tq; void* type; struct Cyc_Core_Opt*
width; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structdecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Uniondecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Tunionfield{
struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_List_List* typs;
struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Tuniondecl{ void* sc;
struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_XTuniondecl{ void* sc; struct _tuple1* name; struct
Cyc_List_List* fields; } ; struct Cyc_Absyn_Enumfield{ struct _tuple1* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple1* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple1* name; struct Cyc_List_List* tvs; void* defn; } ; static const
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
_tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d_tag=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement_tag= 0; struct Cyc_Absyn_ArrayElement_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName_tag=
1; struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern char Cyc_Absyn_EmptyAnnot_tag[ 11u]; struct Cyc_Absyn_EmptyAnnot_struct{
char* tag; } ; extern struct Cyc_Absyn_Tqual* Cyc_Absyn_combine_tqual( struct
Cyc_Absyn_Tqual* x, struct Cyc_Absyn_Tqual* y); extern struct Cyc_Absyn_Tqual*
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern void*
Cyc_Absyn_new_evar( void*); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_double_t;
extern void* Cyc_Absyn_starb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual* tq,
void* b); extern void* Cyc_Absyn_atb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual*
tq, void* b); extern void* Cyc_Absyn_tagged_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual*
tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp( struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*,
int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp(
int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp(
char c, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp(
struct _tagged_string f, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_string_exp( struct _tagged_string s, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp( void*, struct Cyc_List_List*
es, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp(
void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct
Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_pre_inc_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_pre_dec_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_instantiate_exp( struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_cast_exp( void*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp( void* t, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeofexp_exp( struct Cyc_Absyn_Exp* e,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*, struct _tagged_string*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp( struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp( struct Cyc_Absyn_Stmt*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt( void*
s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_ifthenelse_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1,
struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_while_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt(
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List* el, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_do_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_trycatch_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* scs, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct _tagged_string* lab, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Pat* Cyc_Absyn_new_pat( void* p, struct Cyc_Position_Segment*
s); extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_let_decl( struct Cyc_Absyn_Pat* p,
struct Cyc_Core_Opt* t_opt, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple1* x,
void* t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Decl* Cyc_Absyn_struct_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Decl* Cyc_Absyn_union_decl( void* s, struct Cyc_Core_Opt* n, struct
Cyc_List_List* ts, struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_tunion_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_xtunion_decl(
void* s, struct _tuple1* n, struct Cyc_List_List* fs, struct Cyc_Position_Segment*
loc); extern void* Cyc_Absyn_function_typ( struct Cyc_List_List* tvs, struct Cyc_Core_Opt*
eff_typ, void* ret_typ, struct Cyc_List_List* args, int varargs, struct Cyc_List_List*);
extern struct _tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern void Cyc_Lex_register_typedef( struct _tuple1* s); extern
void Cyc_Lex_enter_namespace( struct _tagged_string*); extern void Cyc_Lex_leave_namespace();
extern void Cyc_Lex_enter_using( struct _tuple1*); extern void Cyc_Lex_leave_using();
typedef void* Cyc_Parse_Struct_or_union; static const unsigned int Cyc_Parse_Struct_su=
0; static const unsigned int Cyc_Parse_Union_su= 1; typedef void* Cyc_Parse_struct_or_union_t;
typedef void* Cyc_Parse_Blockitem; static const int Cyc_Parse_TopDecls_bl_tag= 0;
struct Cyc_Parse_TopDecls_bl_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Parse_Stmt_bl_tag= 1; struct Cyc_Parse_Stmt_bl_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Parse_FnDecl_bl_tag= 2;
struct Cyc_Parse_FnDecl_bl_struct{ int tag; struct Cyc_Absyn_Fndecl* f1; } ;
typedef void* Cyc_Parse_blockitem_t; typedef void* Cyc_Parse_Type_specifier;
static const int Cyc_Parse_Signed_spec_tag= 0; struct Cyc_Parse_Signed_spec_struct{
int tag; struct Cyc_Position_Segment* f1; } ; static const int Cyc_Parse_Unsigned_spec_tag=
1; struct Cyc_Parse_Unsigned_spec_struct{ int tag; struct Cyc_Position_Segment*
f1; } ; static const int Cyc_Parse_Short_spec_tag= 2; struct Cyc_Parse_Short_spec_struct{
int tag; struct Cyc_Position_Segment* f1; } ; static const int Cyc_Parse_Long_spec_tag=
3; struct Cyc_Parse_Long_spec_struct{ int tag; struct Cyc_Position_Segment* f1;
} ; static const int Cyc_Parse_Type_spec_tag= 4; struct Cyc_Parse_Type_spec_struct{
int tag; void* f1; struct Cyc_Position_Segment* f2; } ; static const int Cyc_Parse_Decl_spec_tag=
5; struct Cyc_Parse_Decl_spec_struct{ int tag; struct Cyc_Absyn_Decl* f1; } ;
typedef void* Cyc_Parse_type_specifier_t; typedef void* Cyc_Parse_Storage_class;
static const unsigned int Cyc_Parse_Typedef_sc= 0; static const unsigned int Cyc_Parse_Extern_sc=
1; static const unsigned int Cyc_Parse_ExternC_sc= 2; static const unsigned int
Cyc_Parse_Static_sc= 3; static const unsigned int Cyc_Parse_Auto_sc= 4; static
const unsigned int Cyc_Parse_Register_sc= 5; static const unsigned int Cyc_Parse_Abstract_sc=
6; typedef void* Cyc_Parse_storage_class_t; struct Cyc_Parse_Declaration_spec{
struct Cyc_Core_Opt* sc; struct Cyc_Absyn_Tqual* tq; struct Cyc_List_List*
type_specs; int is_inline; struct Cyc_List_List* attributes; } ; typedef struct
Cyc_Parse_Declaration_spec* Cyc_Parse_decl_spec_t; struct Cyc_Parse_Declarator{
struct _tuple1* id; struct Cyc_List_List* tms; } ; typedef struct Cyc_Parse_Declarator*
Cyc_Parse_declarator_t; struct Cyc_Parse_Abstractdeclarator{ struct Cyc_List_List*
tms; } ; typedef struct Cyc_Parse_Abstractdeclarator* Cyc_Parse_abstractdeclarator_t;
static struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct Cyc_Position_Segment*
loc, struct _tuple2* t); static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc); static struct
_tuple6 Cyc_Parse_apply_tms( struct Cyc_Absyn_Tqual*, void*, struct Cyc_List_List*,
struct Cyc_List_List*); static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t); struct Cyc_Core_Opt* Cyc_Parse_lbuf=
0; static struct Cyc_List_List* Cyc_Parse_parse_result= 0; static void Cyc_Parse_err(
struct _tagged_string msg, struct Cyc_Position_Segment* sg){ Cyc_Position_post_error(
Cyc_Position_mk_err_parse( sg, msg));} static void* Cyc_Parse_abort( struct
_tagged_string msg, struct Cyc_Position_Segment* sg){ Cyc_Parse_err( msg, sg);(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Position_Exit_struct*
_temp0=( struct Cyc_Position_Exit_struct*) GC_malloc( sizeof( struct Cyc_Position_Exit_struct));
_temp0[ 0]=({ struct Cyc_Position_Exit_struct _temp1; _temp1.tag= Cyc_Position_Exit_tag;
_temp1;}); _temp0;}));} static void Cyc_Parse_warn( struct _tagged_string msg,
struct Cyc_Position_Segment* sg){({ struct _tagged_string _temp2= Cyc_Position_string_of_segment(
sg); struct _tagged_string _temp3= msg; fprintf( Cyc_Stdio_stderr,"%.*s: Warning: %.*s\n",
_temp2.last_plus_one - _temp2.curr, _temp2.curr, _temp3.last_plus_one - _temp3.curr,
_temp3.curr);}); return;} static void* Cyc_Parse_unimp( struct _tagged_string
msg, struct Cyc_Position_Segment* sg){ return(( void*(*)( struct _tagged_string
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct _tagged_string
_temp4= msg; xprintf("%.*s unimplemented", _temp4.last_plus_one - _temp4.curr,
_temp4.curr);}), sg);} static void Cyc_Parse_unimp2( struct _tagged_string msg,
struct Cyc_Position_Segment* sg){({ struct _tagged_string _temp5= Cyc_Position_string_of_segment(
sg); struct _tagged_string _temp6= msg; fprintf( Cyc_Stdio_stderr,"%.*s: Warning: Cyclone does not yet support %.*s\n",
_temp5.last_plus_one - _temp5.curr, _temp5.curr, _temp6.last_plus_one - _temp6.curr,
_temp6.curr);}); return;} static struct Cyc_Absyn_Structfield* Cyc_Parse_make_struct_field(
struct Cyc_Position_Segment* loc, struct _tuple8* field_info){ struct Cyc_Core_Opt*
_temp9; struct _tuple7* _temp11; struct _tuple8 _temp7=* field_info; _LL12:
_temp11= _temp7.f1; goto _LL10; _LL10: _temp9= _temp7.f2; goto _LL8; _LL8: if((*
_temp11).f4 != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp13=( char*)"bad type params in struct field";
struct _tagged_string _temp14; _temp14.curr= _temp13; _temp14.base= _temp13;
_temp14.last_plus_one= _temp13 + 32; _temp14;}), loc);}{ struct _tuple1* qid=(*
_temp11).f1;{ void* _temp15=(* qid).f1; struct Cyc_List_List* _temp25; struct
Cyc_List_List* _temp27; _LL17: if(( unsigned int) _temp15 > 1u?(( struct
_tunion_struct*) _temp15)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL26: _temp25=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp15)->f1; if(
_temp25 == 0){ goto _LL18;} else{ goto _LL19;}} else{ goto _LL19;} _LL19: if((
unsigned int) _temp15 > 1u?(( struct _tunion_struct*) _temp15)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL28: _temp27=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp15)->f1; if( _temp27 == 0){ goto _LL20;} else{ goto _LL21;}} else{ goto
_LL21;} _LL21: if(( int) _temp15 == Cyc_Absyn_Loc_n){ goto _LL22;} else{ goto
_LL23;} _LL23: goto _LL24; _LL18: goto _LL16; _LL20: goto _LL16; _LL22: goto
_LL16; _LL24: Cyc_Parse_err(( struct _tagged_string)({ char* _temp29=( char*)"struct field cannot be qualified with a module name";
struct _tagged_string _temp30; _temp30.curr= _temp29; _temp30.base= _temp29;
_temp30.last_plus_one= _temp29 + 52; _temp30;}), loc); goto _LL16; _LL16:;}{
struct Cyc_List_List* atts=(* _temp11).f5; return({ struct Cyc_Absyn_Structfield*
_temp31=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp31->name=(* qid).f2; _temp31->tq=(* _temp11).f2; _temp31->type=( void*)(*
_temp11).f3; _temp31->width= _temp9; _temp31->attributes= atts; _temp31;});}}}
static struct _tuple2* Cyc_Parse_make_param( struct Cyc_Position_Segment* loc,
struct _tuple9* field){ struct _tuple9 _temp34; struct Cyc_List_List* _temp35;
void* _temp37; struct Cyc_Absyn_Tqual* _temp39; struct Cyc_Core_Opt* _temp41;
struct _tuple9* _temp32= field; _temp34=* _temp32; _LL42: _temp41= _temp34.f1;
goto _LL40; _LL40: _temp39= _temp34.f2; goto _LL38; _LL38: _temp37= _temp34.f3;
goto _LL36; _LL36: _temp35= _temp34.f4; goto _LL33; _LL33: { struct Cyc_Core_Opt*
idopt= 0; if( _temp41 != 0){ idopt=({ struct Cyc_Core_Opt* _temp43=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp43->v=( void*)(*(( struct _tuple1*)
_temp41->v)).f2; _temp43;});{ void* _temp44=(*(( struct _tuple1*) _temp41->v)).f1;
struct Cyc_List_List* _temp54; struct Cyc_List_List* _temp56; _LL46: if((
unsigned int) _temp44 > 1u?(( struct _tunion_struct*) _temp44)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL55: _temp54=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp44)->f1; if( _temp54 == 0){ goto _LL47;} else{ goto _LL48;}} else{ goto
_LL48;} _LL48: if(( unsigned int) _temp44 > 1u?(( struct _tunion_struct*)
_temp44)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL57: _temp56=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp44)->f1; if( _temp56 == 0){ goto _LL49;}
else{ goto _LL50;}} else{ goto _LL50;} _LL50: if(( int) _temp44 == Cyc_Absyn_Loc_n){
goto _LL51;} else{ goto _LL52;} _LL52: goto _LL53; _LL47: goto _LL45; _LL49:
goto _LL45; _LL51: goto _LL45; _LL53: Cyc_Parse_err(( struct _tagged_string)({
char* _temp58=( char*)"parameter cannot be qualified with a module name"; struct
_tagged_string _temp59; _temp59.curr= _temp58; _temp59.base= _temp58; _temp59.last_plus_one=
_temp58 + 49; _temp59;}), loc); goto _LL45; _LL45:;}} if( _temp35 != 0){(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp60=( char*)"parameter should have no type parameters";
struct _tagged_string _temp61; _temp61.curr= _temp60; _temp61.base= _temp60;
_temp61.last_plus_one= _temp60 + 41; _temp61;}), loc);} return({ struct _tuple2*
_temp62=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp62->f1=
idopt; _temp62->f2= _temp39; _temp62->f3= _temp37; _temp62;});}} static void*
Cyc_Parse_type_spec( void* t, struct Cyc_Position_Segment* loc){ return( void*)({
struct Cyc_Parse_Type_spec_struct* _temp63=( struct Cyc_Parse_Type_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct)); _temp63[ 0]=({ struct
Cyc_Parse_Type_spec_struct _temp64; _temp64.tag= Cyc_Parse_Type_spec_tag;
_temp64.f1=( void*) t; _temp64.f2= loc; _temp64;}); _temp63;});} static void*
Cyc_Parse_array2ptr( void* t){ void* _temp65= t; struct Cyc_Absyn_Exp* _temp71;
struct Cyc_Absyn_Tqual* _temp73; void* _temp75; _LL67: if(( unsigned int)
_temp65 > 4u?(( struct _tunion_struct*) _temp65)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL76: _temp75=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp65)->f1;
goto _LL74; _LL74: _temp73=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp65)->f2; goto _LL72; _LL72: _temp71=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp65)->f3; goto _LL68;} else{ goto _LL69;} _LL69: goto _LL70; _LL68: return
Cyc_Absyn_starb_typ( _temp75,( void*) Cyc_Absyn_HeapRgn, _temp73, _temp71 == 0?(
void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp77=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp77[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp78; _temp78.tag= Cyc_Absyn_Upper_b_tag;
_temp78.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp79= _temp71;
if( _temp79 == 0){ _throw( Null_Exception);} _temp79;}); _temp78;}); _temp77;}));
_LL70: return t; _LL66:;} static void Cyc_Parse_arg_array2ptr( struct _tuple2* x){(*
x).f3= Cyc_Parse_array2ptr((* x).f3);} static struct _tuple10* Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment* loc, struct _tuple2* t){ return({ struct _tuple10*
_temp80=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp80->f1=(*
t).f2; _temp80->f2=(* t).f3; _temp80;});} static void Cyc_Parse_only_vardecl(
struct Cyc_List_List* params, struct Cyc_Absyn_Decl* x){ struct _tagged_string
decl_kind;{ void* _temp81=( void*) x->r; struct Cyc_Absyn_Vardecl* _temp107; int
_temp109; struct Cyc_Absyn_Exp* _temp111; struct Cyc_Core_Opt* _temp113; struct
Cyc_Core_Opt* _temp115; struct Cyc_Absyn_Pat* _temp117; struct Cyc_Absyn_Fndecl*
_temp119; struct Cyc_Absyn_Structdecl* _temp121; struct Cyc_Absyn_Uniondecl*
_temp123; struct Cyc_Absyn_Tuniondecl* _temp125; struct Cyc_Absyn_Typedefdecl*
_temp127; struct Cyc_Absyn_XTuniondecl* _temp129; struct Cyc_Absyn_Enumdecl*
_temp131; struct Cyc_List_List* _temp133; struct _tagged_string* _temp135;
struct Cyc_List_List* _temp137; struct _tuple1* _temp139; struct Cyc_List_List*
_temp141; _LL83: if((( struct _tunion_struct*) _temp81)->tag == Cyc_Absyn_Var_d_tag){
_LL108: _temp107=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp81)->f1; goto _LL84;} else{ goto _LL85;} _LL85: if((( struct _tunion_struct*)
_temp81)->tag == Cyc_Absyn_Let_d_tag){ _LL118: _temp117=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp81)->f1; goto _LL116; _LL116: _temp115=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp81)->f2; goto
_LL114; _LL114: _temp113=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp81)->f3; goto _LL112; _LL112: _temp111=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Let_d_struct*) _temp81)->f4; goto _LL110; _LL110: _temp109=( int)((
struct Cyc_Absyn_Let_d_struct*) _temp81)->f5; goto _LL86;} else{ goto _LL87;}
_LL87: if((( struct _tunion_struct*) _temp81)->tag == Cyc_Absyn_Fn_d_tag){
_LL120: _temp119=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp81)->f1; goto _LL88;} else{ goto _LL89;} _LL89: if((( struct _tunion_struct*)
_temp81)->tag == Cyc_Absyn_Struct_d_tag){ _LL122: _temp121=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp81)->f1; goto _LL90;} else{ goto _LL91;}
_LL91: if((( struct _tunion_struct*) _temp81)->tag == Cyc_Absyn_Union_d_tag){
_LL124: _temp123=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp81)->f1; goto _LL92;} else{ goto _LL93;} _LL93: if((( struct _tunion_struct*)
_temp81)->tag == Cyc_Absyn_Tunion_d_tag){ _LL126: _temp125=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp81)->f1; goto _LL94;} else{ goto _LL95;}
_LL95: if((( struct _tunion_struct*) _temp81)->tag == Cyc_Absyn_Typedef_d_tag){
_LL128: _temp127=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp81)->f1; goto _LL96;} else{ goto _LL97;} _LL97: if((( struct _tunion_struct*)
_temp81)->tag == Cyc_Absyn_XTunion_d_tag){ _LL130: _temp129=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_d_struct*) _temp81)->f1; goto _LL98;} else{ goto _LL99;}
_LL99: if((( struct _tunion_struct*) _temp81)->tag == Cyc_Absyn_Enum_d_tag){
_LL132: _temp131=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp81)->f1; goto _LL100;} else{ goto _LL101;} _LL101: if((( struct
_tunion_struct*) _temp81)->tag == Cyc_Absyn_Namespace_d_tag){ _LL136: _temp135=(
struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp81)->f1;
goto _LL134; _LL134: _temp133=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp81)->f2; goto _LL102;} else{ goto _LL103;} _LL103: if((( struct
_tunion_struct*) _temp81)->tag == Cyc_Absyn_Using_d_tag){ _LL140: _temp139=(
struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*) _temp81)->f1; goto _LL138;
_LL138: _temp137=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp81)->f2; goto _LL104;} else{ goto _LL105;} _LL105: if((( struct
_tunion_struct*) _temp81)->tag == Cyc_Absyn_ExternC_d_tag){ _LL142: _temp141=(
struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*) _temp81)->f1; goto
_LL106;} else{ goto _LL82;} _LL84: if( _temp107->initializer != 0){(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp143=( char*)"initializers are not allowed in parameter declarations";
struct _tagged_string _temp144; _temp144.curr= _temp143; _temp144.base= _temp143;
_temp144.last_plus_one= _temp143 + 55; _temp144;}), x->loc);}{ void* _temp145=(*
_temp107->name).f1; struct Cyc_List_List* _temp155; struct Cyc_List_List*
_temp157; _LL147: if(( int) _temp145 == Cyc_Absyn_Loc_n){ goto _LL148;} else{
goto _LL149;} _LL149: if(( unsigned int) _temp145 > 1u?(( struct _tunion_struct*)
_temp145)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL156: _temp155=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp145)->f1; if( _temp155 == 0){ goto _LL150;}
else{ goto _LL151;}} else{ goto _LL151;} _LL151: if(( unsigned int) _temp145 > 1u?((
struct _tunion_struct*) _temp145)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL158:
_temp157=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp145)->f1;
if( _temp157 == 0){ goto _LL152;} else{ goto _LL153;}} else{ goto _LL153;}
_LL153: goto _LL154; _LL148: goto _LL146; _LL150: goto _LL146; _LL152: goto
_LL146; _LL154: Cyc_Parse_err(( struct _tagged_string)({ char* _temp159=( char*)"module names not allowed on parameter declarations";
struct _tagged_string _temp160; _temp160.curr= _temp159; _temp160.base= _temp159;
_temp160.last_plus_one= _temp159 + 51; _temp160;}), x->loc); goto _LL146; _LL146:;}{
int found= 0; for( 0; params != 0; params= params->tl){ if( Cyc_String_zstrptrcmp((*
_temp107->name).f2,( struct _tagged_string*) params->hd) == 0){ found= 1; break;}}
if( ! found){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp161=*(* _temp107->name).f2;
xprintf("%.*s is not listed as a parameter", _temp161.last_plus_one - _temp161.curr,
_temp161.curr);}), x->loc);} return;} _LL86: decl_kind=( struct _tagged_string)({
char* _temp162=( char*)"let declaration"; struct _tagged_string _temp163;
_temp163.curr= _temp162; _temp163.base= _temp162; _temp163.last_plus_one=
_temp162 + 16; _temp163;}); goto _LL82; _LL88: decl_kind=( struct _tagged_string)({
char* _temp164=( char*)"function declaration"; struct _tagged_string _temp165;
_temp165.curr= _temp164; _temp165.base= _temp164; _temp165.last_plus_one=
_temp164 + 21; _temp165;}); goto _LL82; _LL90: decl_kind=( struct _tagged_string)({
char* _temp166=( char*)"struct declaration"; struct _tagged_string _temp167;
_temp167.curr= _temp166; _temp167.base= _temp166; _temp167.last_plus_one=
_temp166 + 19; _temp167;}); goto _LL82; _LL92: decl_kind=( struct _tagged_string)({
char* _temp168=( char*)"union declaration"; struct _tagged_string _temp169;
_temp169.curr= _temp168; _temp169.base= _temp168; _temp169.last_plus_one=
_temp168 + 18; _temp169;}); goto _LL82; _LL94: decl_kind=( struct _tagged_string)({
char* _temp170=( char*)"tunion declaration"; struct _tagged_string _temp171;
_temp171.curr= _temp170; _temp171.base= _temp170; _temp171.last_plus_one=
_temp170 + 19; _temp171;}); goto _LL82; _LL96: decl_kind=( struct _tagged_string)({
char* _temp172=( char*)"typedef"; struct _tagged_string _temp173; _temp173.curr=
_temp172; _temp173.base= _temp172; _temp173.last_plus_one= _temp172 + 8;
_temp173;}); goto _LL82; _LL98: decl_kind=( struct _tagged_string)({ char*
_temp174=( char*)"xtunion declaration"; struct _tagged_string _temp175; _temp175.curr=
_temp174; _temp175.base= _temp174; _temp175.last_plus_one= _temp174 + 20;
_temp175;}); goto _LL82; _LL100: decl_kind=( struct _tagged_string)({ char*
_temp176=( char*)"enum declaration"; struct _tagged_string _temp177; _temp177.curr=
_temp176; _temp177.base= _temp176; _temp177.last_plus_one= _temp176 + 17;
_temp177;}); goto _LL82; _LL102: decl_kind=( struct _tagged_string)({ char*
_temp178=( char*)"namespace declaration"; struct _tagged_string _temp179;
_temp179.curr= _temp178; _temp179.base= _temp178; _temp179.last_plus_one=
_temp178 + 22; _temp179;}); goto _LL82; _LL104: decl_kind=( struct
_tagged_string)({ char* _temp180=( char*)"using declaration"; struct
_tagged_string _temp181; _temp181.curr= _temp180; _temp181.base= _temp180;
_temp181.last_plus_one= _temp180 + 18; _temp181;}); goto _LL82; _LL106:
decl_kind=( struct _tagged_string)({ char* _temp182=( char*)"extern C declaration";
struct _tagged_string _temp183; _temp183.curr= _temp182; _temp183.base= _temp182;
_temp183.last_plus_one= _temp182 + 21; _temp183;}); goto _LL82; _LL82:;}(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp184= decl_kind; xprintf("%.*s appears in parameter type",
_temp184.last_plus_one - _temp184.curr, _temp184.curr);}), x->loc); return;}
static struct _tuple2* Cyc_Parse_get_param_type( struct _tuple11* env, struct
_tagged_string* x){ struct _tuple11 _temp187; struct Cyc_Position_Segment*
_temp188; struct Cyc_List_List* _temp190; struct _tuple11* _temp185= env;
_temp187=* _temp185; _LL191: _temp190= _temp187.f1; goto _LL189; _LL189:
_temp188= _temp187.f2; goto _LL186; _LL186: if( _temp190 == 0){ return(( struct
_tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp192=* x; xprintf("missing type for parameter %.*s",
_temp192.last_plus_one - _temp192.curr, _temp192.curr);}), _temp188);}{ void*
_temp193=( void*)(( struct Cyc_Absyn_Decl*) _temp190->hd)->r; struct Cyc_Absyn_Vardecl*
_temp199; _LL195: if((( struct _tunion_struct*) _temp193)->tag == Cyc_Absyn_Var_d_tag){
_LL200: _temp199=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp193)->f1; goto _LL196;} else{ goto _LL197;} _LL197: goto _LL198; _LL196:{
void* _temp201=(* _temp199->name).f1; struct Cyc_List_List* _temp211; struct Cyc_List_List*
_temp213; _LL203: if(( int) _temp201 == Cyc_Absyn_Loc_n){ goto _LL204;} else{
goto _LL205;} _LL205: if(( unsigned int) _temp201 > 1u?(( struct _tunion_struct*)
_temp201)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL212: _temp211=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp201)->f1; if( _temp211 == 0){ goto _LL206;}
else{ goto _LL207;}} else{ goto _LL207;} _LL207: if(( unsigned int) _temp201 > 1u?((
struct _tunion_struct*) _temp201)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL214:
_temp213=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp201)->f1;
if( _temp213 == 0){ goto _LL208;} else{ goto _LL209;}} else{ goto _LL209;}
_LL209: goto _LL210; _LL204: goto _LL202; _LL206: goto _LL202; _LL208: goto
_LL202; _LL210: Cyc_Parse_err(( struct _tagged_string)({ char* _temp215=( char*)"module name not allowed on parameter";
struct _tagged_string _temp216; _temp216.curr= _temp215; _temp216.base= _temp215;
_temp216.last_plus_one= _temp215 + 37; _temp216;}), _temp188); goto _LL202;
_LL202:;} if( Cyc_String_zstrptrcmp((* _temp199->name).f2, x) == 0){ return({
struct _tuple2* _temp217=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2) *
1); _temp217[ 0]=({ struct _tuple2 _temp218; _temp218.f1=({ struct Cyc_Core_Opt*
_temp219=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp219[ 0]=({ struct Cyc_Core_Opt _temp220; _temp220.v=( void*)(* _temp199->name).f2;
_temp220;}); _temp219;}); _temp218.f2= _temp199->tq; _temp218.f3=( void*)
_temp199->type; _temp218;}); _temp217;});} else{ return Cyc_Parse_get_param_type(({
struct _tuple11* _temp221=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp221->f1= _temp190->tl; _temp221->f2= _temp188; _temp221;}), x);} _LL198:
return(( struct _tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp222=( char*)"non-variable declaration";
struct _tagged_string _temp223; _temp223.curr= _temp222; _temp223.base= _temp222;
_temp223.last_plus_one= _temp222 + 25; _temp223;}),(( struct Cyc_Absyn_Decl*)
_temp190->hd)->loc); _LL194:;}} static int Cyc_Parse_is_typeparam( void* tm){
void* _temp224= tm; int _temp230; struct Cyc_Position_Segment* _temp232; struct
Cyc_List_List* _temp234; _LL226: if(( unsigned int) _temp224 > 1u?(( struct
_tunion_struct*) _temp224)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){ _LL235:
_temp234=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp224)->f1; goto _LL233; _LL233: _temp232=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp224)->f2; goto _LL231; _LL231:
_temp230=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp224)->f3; goto
_LL227;} else{ goto _LL228;} _LL228: goto _LL229; _LL227: return 1; _LL229:
return 0; _LL225:;} static void* Cyc_Parse_id2type( struct _tagged_string s,
struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({
char* _temp236=( char*)"`H"; struct _tagged_string _temp237; _temp237.curr=
_temp236; _temp237.base= _temp236; _temp237.last_plus_one= _temp236 + 3;
_temp237;})) == 0){ return( void*) Cyc_Absyn_HeapRgn;} else{ return( void*)({
struct Cyc_Absyn_VarType_struct* _temp238=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp238[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp239; _temp239.tag= Cyc_Absyn_VarType_tag; _temp239.f1=({ struct Cyc_Absyn_Tvar*
_temp240=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp240->name=({ struct _tagged_string* _temp241=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp241[ 0]= s; _temp241;});
_temp240->kind= k; _temp240;}); _temp239;}); _temp238;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp242=
t; struct Cyc_Absyn_Tvar* _temp248; _LL244: if(( unsigned int) _temp242 > 4u?((
struct _tunion_struct*) _temp242)->tag == Cyc_Absyn_VarType_tag: 0){ _LL249:
_temp248=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp242)->f1;
goto _LL245;} else{ goto _LL246;} _LL246: goto _LL247; _LL245: return _temp248;
_LL247: return(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char*
_temp250=( char*)"expecting a list of type variables, not types"; struct
_tagged_string _temp251; _temp251.curr= _temp250; _temp251.base= _temp250;
_temp251.last_plus_one= _temp250 + 46; _temp251;}), loc); _LL243:;} static void*
Cyc_Parse_tvar2typ( struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp252=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp252[ 0]=({ struct Cyc_Absyn_VarType_struct _temp253; _temp253.tag= Cyc_Absyn_VarType_tag;
_temp253.f1= pr; _temp253;}); _temp252;});} static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List* tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment*
loc){ if( tds == 0){ return tms;} if( tms == 0){ return 0;}{ void* _temp254=(
void*) tms->hd; void* _temp260; _LL256: if(( unsigned int) _temp254 > 1u?((
struct _tunion_struct*) _temp254)->tag == Cyc_Absyn_Function_mod_tag: 0){ _LL261:
_temp260=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp254)->f1; goto
_LL257;} else{ goto _LL258;} _LL258: goto _LL259; _LL257: if( tms->tl == 0? 1:(
Cyc_Parse_is_typeparam(( void*)( tms->tl)->hd)?( tms->tl)->tl == 0: 0)){ void*
_temp262= _temp260; struct Cyc_Core_Opt* _temp268; int _temp270; struct Cyc_List_List*
_temp272; struct Cyc_Position_Segment* _temp274; struct Cyc_List_List* _temp276;
_LL264: if((( struct _tunion_struct*) _temp262)->tag == Cyc_Absyn_WithTypes_tag){
_LL273: _temp272=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp262)->f1; goto _LL271; _LL271: _temp270=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp262)->f2; goto _LL269; _LL269: _temp268=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp262)->f3; goto _LL265;} else{ goto _LL266;}
_LL266: if((( struct _tunion_struct*) _temp262)->tag == Cyc_Absyn_NoTypes_tag){
_LL277: _temp276=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp262)->f1; goto _LL275; _LL275: _temp274=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_NoTypes_struct*) _temp262)->f2; goto _LL267;} else{ goto _LL263;}
_LL265: Cyc_Parse_warn(( struct _tagged_string)({ char* _temp278=( char*)"function declaration with both new- and old-style parameterdeclarations; ignoring old-style";
struct _tagged_string _temp279; _temp279.curr= _temp278; _temp279.base= _temp278;
_temp279.last_plus_one= _temp278 + 92; _temp279;}), loc); return tms; _LL267:((
void(*)( void(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl,
_temp276, tds);{ struct _tuple11* env=({ struct _tuple11* _temp280=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp280->f1= tds; _temp280->f2=
loc; _temp280;}); return({ struct Cyc_List_List* _temp281=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp281->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp282=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp282[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp283; _temp283.tag= Cyc_Absyn_Function_mod_tag;
_temp283.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp284=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp284[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp285; _temp285.tag= Cyc_Absyn_WithTypes_tag;
_temp285.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple11*,
struct _tagged_string*), struct _tuple11* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, env, _temp276); _temp285.f2= 0; _temp285.f3= 0;
_temp285;}); _temp284;})); _temp283;}); _temp282;})); _temp281->tl= 0; _temp281;});}
_LL263:;} else{ return({ struct Cyc_List_List* _temp286=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp286->hd=( void*)(( void*) tms->hd);
_temp286->tl= Cyc_Parse_oldstyle2newstyle( tms->tl, tds, loc); _temp286;});}
_LL259: return({ struct Cyc_List_List* _temp287=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp287->hd=( void*)(( void*) tms->hd);
_temp287->tl= Cyc_Parse_oldstyle2newstyle( tms->tl, tds, loc); _temp287;});
_LL255:;}} static struct Cyc_Absyn_Fndecl* Cyc_Parse_make_function( struct Cyc_Core_Opt*
dso, struct Cyc_Parse_Declarator* d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt*
body, struct Cyc_Position_Segment* loc){ if( tds != 0){ d=({ struct Cyc_Parse_Declarator*
_temp288=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp288->id= d->id; _temp288->tms= Cyc_Parse_oldstyle2newstyle( d->tms, tds,
loc); _temp288;});}{ void* sc=( void*) Cyc_Absyn_Public; struct Cyc_List_List*
tss= 0; struct Cyc_Absyn_Tqual* tq= Cyc_Absyn_empty_tqual(); int is_inline= 0;
struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct Cyc_Parse_Declaration_spec*)
dso->v)->type_specs; tq=(( struct Cyc_Parse_Declaration_spec*) dso->v)->tq;
is_inline=(( struct Cyc_Parse_Declaration_spec*) dso->v)->is_inline; atts=((
struct Cyc_Parse_Declaration_spec*) dso->v)->attributes; if((( struct Cyc_Parse_Declaration_spec*)
dso->v)->sc != 0){ void* _temp289=( void*)((( struct Cyc_Parse_Declaration_spec*)
dso->v)->sc)->v; _LL291: if(( int) _temp289 == Cyc_Parse_Extern_sc){ goto _LL292;}
else{ goto _LL293;} _LL293: if(( int) _temp289 == Cyc_Parse_ExternC_sc){ goto
_LL294;} else{ goto _LL295;} _LL295: if(( int) _temp289 == Cyc_Parse_Static_sc){
goto _LL296;} else{ goto _LL297;} _LL297: goto _LL298; _LL292: sc=( void*) Cyc_Absyn_Extern;
goto _LL290; _LL294: sc=( void*) Cyc_Absyn_ExternC; goto _LL290; _LL296: sc=(
void*) Cyc_Absyn_Static; goto _LL290; _LL298: Cyc_Parse_err(( struct
_tagged_string)({ char* _temp299=( char*)"bad storage class on function"; struct
_tagged_string _temp300; _temp300.curr= _temp299; _temp300.base= _temp299;
_temp300.last_plus_one= _temp299 + 30; _temp300;}), loc); goto _LL290; _LL290:;}}{
struct Cyc_Core_Opt* _temp303; void* _temp305; struct _tuple5 _temp301= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL306: _temp305= _temp301.f1; goto _LL304; _LL304: _temp303=
_temp301.f2; goto _LL302; _LL302: { struct Cyc_List_List* _temp309; struct Cyc_List_List*
_temp311; void* _temp313; struct Cyc_Absyn_Tqual* _temp315; struct _tuple6
_temp307= Cyc_Parse_apply_tms( tq, _temp305, atts, d->tms); _LL316: _temp315=
_temp307.f1; goto _LL314; _LL314: _temp313= _temp307.f2; goto _LL312; _LL312:
_temp311= _temp307.f3; goto _LL310; _LL310: _temp309= _temp307.f4; goto _LL308;
_LL308: if( _temp303 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char*
_temp317=( char*)"nested type declaration within function prototype"; struct
_tagged_string _temp318; _temp318.curr= _temp317; _temp318.base= _temp317;
_temp318.last_plus_one= _temp317 + 50; _temp318;}), loc);} if( _temp311 != 0){
Cyc_Parse_warn(( struct _tagged_string)({ char* _temp319=( char*)"bad type params, ignoring";
struct _tagged_string _temp320; _temp320.curr= _temp319; _temp320.base= _temp319;
_temp320.last_plus_one= _temp319 + 26; _temp320;}), loc);}{ void* _temp321=
_temp313; struct Cyc_Absyn_FnInfo _temp327; struct Cyc_List_List* _temp329; int
_temp331; struct Cyc_List_List* _temp333; void* _temp335; struct Cyc_Core_Opt*
_temp337; struct Cyc_List_List* _temp339; _LL323: if(( unsigned int) _temp321 >
4u?(( struct _tunion_struct*) _temp321)->tag == Cyc_Absyn_FnType_tag: 0){ _LL328:
_temp327=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp321)->f1;
_LL340: _temp339=( struct Cyc_List_List*) _temp327.tvars; goto _LL338; _LL338:
_temp337=( struct Cyc_Core_Opt*) _temp327.effect; goto _LL336; _LL336: _temp335=(
void*) _temp327.ret_typ; goto _LL334; _LL334: _temp333=( struct Cyc_List_List*)
_temp327.args; goto _LL332; _LL332: _temp331=( int) _temp327.varargs; goto
_LL330; _LL330: _temp329=( struct Cyc_List_List*) _temp327.attributes; goto
_LL324;} else{ goto _LL325;} _LL325: goto _LL326; _LL324: { struct Cyc_List_List*
args2=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp333); return({ struct Cyc_Absyn_Fndecl*
_temp341=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp341->sc=( void*) sc; _temp341->name= d->id; _temp341->tvs= _temp339;
_temp341->is_inline= is_inline; _temp341->effect= _temp337; _temp341->ret_type=(
void*) _temp335; _temp341->args= args2; _temp341->varargs= _temp331; _temp341->body=
body; _temp341->cached_typ= 0; _temp341->param_vardecls= 0; _temp341->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp329, _temp309); _temp341;});} _LL326: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp342=( char*)"declarator is not a function prototype";
struct _tagged_string _temp343; _temp343.curr= _temp342; _temp343.base= _temp342;
_temp343.last_plus_one= _temp342 + 39; _temp343;}), loc); _LL322:;}}}}} static
struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct Cyc_Position_Segment* loc,
struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp344=( char*)"missing argument variable in function prototype"; struct
_tagged_string _temp345; _temp345.curr= _temp344; _temp345.base= _temp344;
_temp345.last_plus_one= _temp344 + 48; _temp345;}), loc); return({ struct
_tuple4* _temp346=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp346->f1=({ struct _tagged_string* _temp347=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp347[ 0]=( struct
_tagged_string)({ char* _temp348=( char*)"?"; struct _tagged_string _temp349;
_temp349.curr= _temp348; _temp349.base= _temp348; _temp349.last_plus_one=
_temp348 + 2; _temp349;}); _temp347;}); _temp346->f2=(* t).f2; _temp346->f3=(* t).f3;
_temp346;});} else{ return({ struct _tuple4* _temp350=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp350->f1=( struct _tagged_string*)((* t).f1)->v;
_temp350->f2=(* t).f2; _temp350->f3=(* t).f3; _temp350;});}} static char
_temp353[ 52u]="at most one type may appear within a type specifier"; static
struct _tagged_string Cyc_Parse_msg1=( struct _tagged_string){ _temp353,
_temp353, _temp353 + 52u}; static char _temp356[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg2=( struct _tagged_string){ _temp356,
_temp356, _temp356 + 63u}; static char _temp359[ 50u]="type specifier includes more than one declaration";
static struct _tagged_string Cyc_Parse_msg3=( struct _tagged_string){ _temp359,
_temp359, _temp359 + 50u}; static char _temp362[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg4=( struct _tagged_string){ _temp362,
_temp362, _temp362 + 60u}; static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
declopt= 0; int seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*)
Cyc_Absyn_VoidType; void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed;
struct Cyc_Position_Segment* last_loc= loc; for( 0; ts != 0; ts= ts->tl){ void*
_temp363=( void*) ts->hd; struct Cyc_Position_Segment* _temp377; void* _temp379;
struct Cyc_Position_Segment* _temp381; struct Cyc_Position_Segment* _temp383;
struct Cyc_Position_Segment* _temp385; struct Cyc_Position_Segment* _temp387;
struct Cyc_Absyn_Decl* _temp389; _LL365: if((( struct _tunion_struct*) _temp363)->tag
== Cyc_Parse_Type_spec_tag){ _LL380: _temp379=( void*)(( struct Cyc_Parse_Type_spec_struct*)
_temp363)->f1; goto _LL378; _LL378: _temp377=( struct Cyc_Position_Segment*)((
struct Cyc_Parse_Type_spec_struct*) _temp363)->f2; goto _LL366;} else{ goto
_LL367;} _LL367: if((( struct _tunion_struct*) _temp363)->tag == Cyc_Parse_Signed_spec_tag){
_LL382: _temp381=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Signed_spec_struct*)
_temp363)->f1; goto _LL368;} else{ goto _LL369;} _LL369: if((( struct
_tunion_struct*) _temp363)->tag == Cyc_Parse_Unsigned_spec_tag){ _LL384:
_temp383=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Unsigned_spec_struct*)
_temp363)->f1; goto _LL370;} else{ goto _LL371;} _LL371: if((( struct
_tunion_struct*) _temp363)->tag == Cyc_Parse_Short_spec_tag){ _LL386: _temp385=(
struct Cyc_Position_Segment*)(( struct Cyc_Parse_Short_spec_struct*) _temp363)->f1;
goto _LL372;} else{ goto _LL373;} _LL373: if((( struct _tunion_struct*) _temp363)->tag
== Cyc_Parse_Long_spec_tag){ _LL388: _temp387=( struct Cyc_Position_Segment*)((
struct Cyc_Parse_Long_spec_struct*) _temp363)->f1; goto _LL374;} else{ goto
_LL375;} _LL375: if((( struct _tunion_struct*) _temp363)->tag == Cyc_Parse_Decl_spec_tag){
_LL390: _temp389=( struct Cyc_Absyn_Decl*)(( struct Cyc_Parse_Decl_spec_struct*)
_temp363)->f1; goto _LL376;} else{ goto _LL364;} _LL366: if( seen_type){ Cyc_Parse_err(
Cyc_Parse_msg1, _temp377);} last_loc= _temp377; seen_type= 1; t= _temp379; goto
_LL364; _LL368: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp381);} if(
seen_type){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp391=( char*)"signed qualifier must come before type";
struct _tagged_string _temp392; _temp392.curr= _temp391; _temp392.base= _temp391;
_temp392.last_plus_one= _temp391 + 39; _temp392;}), _temp381);} last_loc=
_temp381; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed; goto _LL364; _LL370: if(
seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp383);} if( seen_type){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp393=( char*)"signed qualifier must come before type";
struct _tagged_string _temp394; _temp394.curr= _temp393; _temp394.base= _temp393;
_temp394.last_plus_one= _temp393 + 39; _temp394;}), _temp383);} last_loc=
_temp383; seen_sign= 1; sgn=( void*) Cyc_Absyn_Unsigned; goto _LL364; _LL372:
if( seen_size){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp395=( char*)"integral size may appear only once within a type specifier";
struct _tagged_string _temp396; _temp396.curr= _temp395; _temp396.base= _temp395;
_temp396.last_plus_one= _temp395 + 59; _temp396;}), _temp385);} if( seen_type){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp397=( char*)"short modifier must come before base type";
struct _tagged_string _temp398; _temp398.curr= _temp397; _temp398.base= _temp397;
_temp398.last_plus_one= _temp397 + 42; _temp398;}), _temp385);} last_loc=
_temp385; seen_size= 1; sz=( void*) Cyc_Absyn_B2; goto _LL364; _LL374: if(
seen_type){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp399=( char*)"long modifier must come before base type";
struct _tagged_string _temp400; _temp400.curr= _temp399; _temp400.base= _temp399;
_temp400.last_plus_one= _temp399 + 41; _temp400;}), _temp387);} if( seen_size){
void* _temp401= sz; _LL403: if(( int) _temp401 == Cyc_Absyn_B4){ goto _LL404;}
else{ goto _LL405;} _LL405: goto _LL406; _LL404: sz=( void*) Cyc_Absyn_B8; goto
_LL402; _LL406: Cyc_Parse_err(( struct _tagged_string)({ char* _temp407=( char*)"extra long in type specifier";
struct _tagged_string _temp408; _temp408.curr= _temp407; _temp408.base= _temp407;
_temp408.last_plus_one= _temp407 + 29; _temp408;}), _temp387); goto _LL402;
_LL402:;} last_loc= _temp387; seen_size= 1; goto _LL364; _LL376: last_loc=
_temp389->loc; if( declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp409=(
void*) _temp389->r; struct Cyc_Absyn_Structdecl* _temp423; struct Cyc_Absyn_Tuniondecl*
_temp425; struct Cyc_Absyn_XTuniondecl* _temp427; struct Cyc_Absyn_Uniondecl*
_temp429; struct Cyc_Absyn_Enumdecl* _temp431; _LL411: if((( struct
_tunion_struct*) _temp409)->tag == Cyc_Absyn_Struct_d_tag){ _LL424: _temp423=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp409)->f1;
goto _LL412;} else{ goto _LL413;} _LL413: if((( struct _tunion_struct*) _temp409)->tag
== Cyc_Absyn_Tunion_d_tag){ _LL426: _temp425=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp409)->f1; goto _LL414;} else{ goto
_LL415;} _LL415: if((( struct _tunion_struct*) _temp409)->tag == Cyc_Absyn_XTunion_d_tag){
_LL428: _temp427=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_d_struct*)
_temp409)->f1; goto _LL416;} else{ goto _LL417;} _LL417: if((( struct
_tunion_struct*) _temp409)->tag == Cyc_Absyn_Union_d_tag){ _LL430: _temp429=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp409)->f1;
goto _LL418;} else{ goto _LL419;} _LL419: if((( struct _tunion_struct*) _temp409)->tag
== Cyc_Absyn_Enum_d_tag){ _LL432: _temp431=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp409)->f1; goto _LL420;} else{ goto _LL421;}
_LL421: goto _LL422; _LL412: { struct Cyc_List_List* args=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp423->tvs); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp433=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp433[ 0]=({ struct Cyc_Absyn_StructType_struct _temp434; _temp434.tag= Cyc_Absyn_StructType_tag;
_temp434.f1=( struct _tuple1*)(( struct _tuple1*)( _temp423->name)->v); _temp434.f2=
args; _temp434.f3= 0; _temp434;}); _temp433;}); if( _temp423->fields != 0){
declopt=({ struct Cyc_Core_Opt* _temp435=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp435->v=( void*) _temp389; _temp435;});} goto
_LL410;} _LL414: { struct Cyc_List_List* args=(( struct Cyc_List_List*(*)( void*(*
f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ,
_temp425->tvs); t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp436=(
struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp436[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp437; _temp437.tag= Cyc_Absyn_TunionType_tag;
_temp437.f1=({ struct Cyc_Absyn_TunionInfo _temp438; _temp438.name=( struct
_tuple1*)( _temp425->name)->v; _temp438.targs= args; _temp438.rgn=( void*)((
void*) Cyc_Absyn_HeapRgn); _temp438.tud= 0; _temp438;}); _temp437;}); _temp436;});
if( _temp425->fields != 0){ declopt=({ struct Cyc_Core_Opt* _temp439=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp439->v=( void*)
_temp389; _temp439;});} goto _LL410;} _LL416: t=( void*)({ struct Cyc_Absyn_XTunionType_struct*
_temp440=( struct Cyc_Absyn_XTunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct));
_temp440[ 0]=({ struct Cyc_Absyn_XTunionType_struct _temp441; _temp441.tag= Cyc_Absyn_XTunionType_tag;
_temp441.f1=({ struct Cyc_Absyn_XTunionInfo _temp442; _temp442.name= _temp427->name;
_temp442.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp442.xtud= 0; _temp442;});
_temp441;}); _temp440;}); if( _temp427->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp443=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp443->v=( void*) _temp389; _temp443;});} goto _LL410; _LL418: { struct Cyc_List_List*
args=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp429->tvs); t=( void*)({
struct Cyc_Absyn_UnionType_struct* _temp444=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp444[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp445; _temp445.tag= Cyc_Absyn_UnionType_tag;
_temp445.f1=( struct _tuple1*)(( struct _tuple1*)( _temp429->name)->v); _temp445.f2=
args; _temp445.f3= 0; _temp445;}); _temp444;}); if( _temp429->fields != 0){
declopt=({ struct Cyc_Core_Opt* _temp446=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp446->v=( void*) _temp389; _temp446;});} goto
_LL410;} _LL420: t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp447=(
struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp447[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp448; _temp448.tag= Cyc_Absyn_EnumType_tag;
_temp448.f1= _temp431->name; _temp448.f2= 0; _temp448;}); _temp447;}); declopt=({
struct Cyc_Core_Opt* _temp449=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp449->v=( void*) _temp389; _temp449;}); goto _LL410; _LL422:((
void(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp450=( char*)"bad declaration within type specifier";
struct _tagged_string _temp451; _temp451.curr= _temp450; _temp451.base= _temp450;
_temp451.last_plus_one= _temp450 + 38; _temp451;}), _temp389->loc); goto _LL410;
_LL410:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3, _temp389->loc);} goto _LL364;
_LL364:;} if( ! seen_type){ if( ! seen_sign? ! seen_size: 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp452=( char*)"missing type withing specifier";
struct _tagged_string _temp453; _temp453.curr= _temp452; _temp453.base= _temp452;
_temp453.last_plus_one= _temp452 + 31; _temp453;}), last_loc);} t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp454=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp454[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp455; _temp455.tag= Cyc_Absyn_IntType_tag; _temp455.f1=( void*) sgn;
_temp455.f2=( void*) sz; _temp455;}); _temp454;});} else{ if( seen_sign){ void*
_temp456= t; void* _temp462; void* _temp464; _LL458: if(( unsigned int) _temp456
> 4u?(( struct _tunion_struct*) _temp456)->tag == Cyc_Absyn_IntType_tag: 0){
_LL465: _temp464=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp456)->f1;
goto _LL463; _LL463: _temp462=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp456)->f2; goto _LL459;} else{ goto _LL460;} _LL460: goto _LL461; _LL459: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp466=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp466[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp467; _temp467.tag= Cyc_Absyn_IntType_tag; _temp467.f1=( void*) sgn;
_temp467.f2=( void*) _temp462; _temp467;}); _temp466;}); goto _LL457; _LL461:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp468=( char*)"sign specification on non-integral type";
struct _tagged_string _temp469; _temp469.curr= _temp468; _temp469.base= _temp468;
_temp469.last_plus_one= _temp468 + 40; _temp469;}), last_loc); goto _LL457;
_LL457:;} if( seen_size){ void* _temp470= t; void* _temp476; void* _temp478;
_LL472: if(( unsigned int) _temp470 > 4u?(( struct _tunion_struct*) _temp470)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL479: _temp478=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp470)->f1; goto _LL477; _LL477: _temp476=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp470)->f2; goto _LL473;} else{ goto _LL474;} _LL474: goto _LL475; _LL473: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp480=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp480[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp481; _temp481.tag= Cyc_Absyn_IntType_tag; _temp481.f1=( void*) _temp478;
_temp481.f2=( void*) sz; _temp481;}); _temp480;}); goto _LL471; _LL475: Cyc_Parse_err((
struct _tagged_string)({ char* _temp482=( char*)"size qualifier on non-integral type";
struct _tagged_string _temp483; _temp483.curr= _temp482; _temp483.base= _temp482;
_temp483.last_plus_one= _temp482 + 36; _temp483;}), last_loc); goto _LL471;
_LL471:;}} return({ struct _tuple5 _temp484; _temp484.f1= t; _temp484.f2=
declopt; _temp484;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss( struct
Cyc_Absyn_Tqual* tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* d=( struct
Cyc_Parse_Declarator*) ds->hd; struct _tuple1* q= d->id; struct Cyc_List_List*
_temp487; struct Cyc_List_List* _temp489; void* _temp491; struct Cyc_Absyn_Tqual*
_temp493; struct _tuple6 _temp485= Cyc_Parse_apply_tms( tq, t, shared_atts, d->tms);
_LL494: _temp493= _temp485.f1; goto _LL492; _LL492: _temp491= _temp485.f2; goto
_LL490; _LL490: _temp489= _temp485.f3; goto _LL488; _LL488: _temp487= _temp485.f4;
goto _LL486; _LL486: return({ struct Cyc_List_List* _temp495=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp495->hd=( void*)({ struct
_tuple7* _temp496=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp496->f1= q; _temp496->f2= _temp493; _temp496->f3= _temp491; _temp496->f4=
_temp489; _temp496->f5= _temp487; _temp496;}); _temp495->tl= Cyc_Parse_apply_tmss(
_temp493, t, ds->tl, shared_atts); _temp495;});}} static int Cyc_Parse_fn_type_att(
void* a){ void* _temp497= a; int _temp511; _LL499: if(( unsigned int) _temp497 >
15u?(( struct _tunion_struct*) _temp497)->tag == Cyc_Absyn_Regparm_att_tag: 0){
_LL512: _temp511=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp497)->f1;
goto _LL500;} else{ goto _LL501;} _LL501: if(( int) _temp497 == Cyc_Absyn_Stdcall_att){
goto _LL502;} else{ goto _LL503;} _LL503: if(( int) _temp497 == Cyc_Absyn_Cdecl_att){
goto _LL504;} else{ goto _LL505;} _LL505: if(( int) _temp497 == Cyc_Absyn_Noreturn_att){
goto _LL506;} else{ goto _LL507;} _LL507: if(( int) _temp497 == Cyc_Absyn_Const_att){
goto _LL508;} else{ goto _LL509;} _LL509: goto _LL510; _LL500: goto _LL502;
_LL502: goto _LL504; _LL504: goto _LL506; _LL506: goto _LL508; _LL508: return 1;
_LL510: return 0; _LL498:;} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual* tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp513; _temp513.f1= tq; _temp513.f2=
t; _temp513.f3= 0; _temp513.f4= atts; _temp513;});}{ void* _temp514=( void*) tms->hd;
struct Cyc_Absyn_Exp* _temp528; void* _temp530; int _temp532; struct Cyc_Position_Segment*
_temp534; struct Cyc_List_List* _temp536; struct Cyc_Absyn_Tqual* _temp538; void*
_temp540; void* _temp542; struct Cyc_List_List* _temp544; struct Cyc_Position_Segment*
_temp546; _LL516: if(( int) _temp514 == Cyc_Absyn_Carray_mod){ goto _LL517;}
else{ goto _LL518;} _LL518: if(( unsigned int) _temp514 > 1u?(( struct
_tunion_struct*) _temp514)->tag == Cyc_Absyn_ConstArray_mod_tag: 0){ _LL529:
_temp528=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp514)->f1; goto _LL519;} else{ goto _LL520;} _LL520: if(( unsigned int)
_temp514 > 1u?(( struct _tunion_struct*) _temp514)->tag == Cyc_Absyn_Function_mod_tag:
0){ _LL531: _temp530=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp514)->f1;
goto _LL521;} else{ goto _LL522;} _LL522: if(( unsigned int) _temp514 > 1u?((
struct _tunion_struct*) _temp514)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){
_LL537: _temp536=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp514)->f1; goto _LL535; _LL535: _temp534=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp514)->f2; goto _LL533; _LL533:
_temp532=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp514)->f3; goto
_LL523;} else{ goto _LL524;} _LL524: if(( unsigned int) _temp514 > 1u?(( struct
_tunion_struct*) _temp514)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL543:
_temp542=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp514)->f1; goto
_LL541; _LL541: _temp540=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp514)->f2; goto _LL539; _LL539: _temp538=( struct Cyc_Absyn_Tqual*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp514)->f3; goto _LL525;} else{ goto _LL526;}
_LL526: if(( unsigned int) _temp514 > 1u?(( struct _tunion_struct*) _temp514)->tag
== Cyc_Absyn_Attributes_mod_tag: 0){ _LL547: _temp546=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_Attributes_mod_struct*) _temp514)->f1; goto _LL545; _LL545:
_temp544=( struct Cyc_List_List*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp514)->f2; goto _LL527;} else{ goto _LL515;} _LL517: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp548=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp548[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp549; _temp549.tag= Cyc_Absyn_ArrayType_tag;
_temp549.f1=( void*) t; _temp549.f2= tq; _temp549.f3= 0; _temp549;}); _temp548;}),
atts, tms->tl); _LL519: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp550=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp550[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp551; _temp551.tag= Cyc_Absyn_ArrayType_tag;
_temp551.f1=( void*) t; _temp551.f2= tq; _temp551.f3=( struct Cyc_Absyn_Exp*)
_temp528; _temp551;}); _temp550;}), atts, tms->tl); _LL521: { void* _temp552=
_temp530; struct Cyc_Core_Opt* _temp558; int _temp560; struct Cyc_List_List*
_temp562; struct Cyc_Position_Segment* _temp564; struct Cyc_List_List* _temp566;
_LL554: if((( struct _tunion_struct*) _temp552)->tag == Cyc_Absyn_WithTypes_tag){
_LL563: _temp562=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp552)->f1; goto _LL561; _LL561: _temp560=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp552)->f2; goto _LL559; _LL559: _temp558=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp552)->f3; goto _LL555;} else{ goto _LL556;}
_LL556: if((( struct _tunion_struct*) _temp552)->tag == Cyc_Absyn_NoTypes_tag){
_LL567: _temp566=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp552)->f1; goto _LL565; _LL565: _temp564=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_NoTypes_struct*) _temp552)->f2; goto _LL557;} else{ goto _LL553;}
_LL555: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts; for( 0; as
!= 0; as= as->tl){ if( Cyc_Parse_fn_type_att(( void*) as->hd)){ fn_atts=({
struct Cyc_List_List* _temp568=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp568->hd=( void*)(( void*) as->hd); _temp568->tl=
fn_atts; _temp568;});} else{ new_atts=({ struct Cyc_List_List* _temp569=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp569->hd=( void*)((
void*) as->hd); _temp569->tl= new_atts; _temp569;});}}} if( tms->tl != 0){ void*
_temp570=( void*)( tms->tl)->hd; int _temp576; struct Cyc_Position_Segment*
_temp578; struct Cyc_List_List* _temp580; _LL572: if(( unsigned int) _temp570 >
1u?(( struct _tunion_struct*) _temp570)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){
_LL581: _temp580=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp570)->f1; goto _LL579; _LL579: _temp578=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp570)->f2; goto _LL577; _LL577:
_temp576=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp570)->f3; goto
_LL573;} else{ goto _LL574;} _LL574: goto _LL575; _LL573: typvars= _temp580; tms=
tms->tl; goto _LL571; _LL575: goto _LL571; _LL571:;} if(((( ! _temp560? _temp562
!= 0: 0)? _temp562->tl == 0: 0)?(*(( struct _tuple2*) _temp562->hd)).f1 == 0: 0)?(*((
struct _tuple2*) _temp562->hd)).f3 ==( void*) Cyc_Absyn_VoidType: 0){ _temp562=
0; _temp560= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)( void(* f)( struct
_tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr,
_temp562); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp558, t, _temp562, _temp560, fn_atts), new_atts, tms->tl);} _LL557:(
void) _throw((( struct _xtunion_struct*(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char*
_temp582=( char*)"function declaration without parameter types"; struct
_tagged_string _temp583; _temp583.curr= _temp582; _temp583.base= _temp582;
_temp583.last_plus_one= _temp582 + 45; _temp583;}), _temp564)); _LL553:;} _LL523:
if( tms->tl == 0){ return({ struct _tuple6 _temp584; _temp584.f1= tq; _temp584.f2=
t; _temp584.f3= _temp536; _temp584.f4= atts; _temp584;});}( void) _throw(((
struct _xtunion_struct*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp585=( char*)"type parameters must appear before function arguments in declarator";
struct _tagged_string _temp586; _temp586.curr= _temp585; _temp586.base= _temp585;
_temp586.last_plus_one= _temp585 + 68; _temp586;}), _temp534)); _LL525: { void*
_temp587= _temp542; struct Cyc_Absyn_Exp* _temp595; struct Cyc_Absyn_Exp*
_temp597; _LL589: if(( unsigned int) _temp587 > 1u?(( struct _tunion_struct*)
_temp587)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL596: _temp595=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp587)->f1; goto
_LL590;} else{ goto _LL591;} _LL591: if(( unsigned int) _temp587 > 1u?(( struct
_tunion_struct*) _temp587)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL598:
_temp597=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp587)->f1; goto _LL592;} else{ goto _LL593;} _LL593: if(( int) _temp587 ==
Cyc_Absyn_TaggedArray_ps){ goto _LL594;} else{ goto _LL588;} _LL590: return Cyc_Parse_apply_tms(
_temp538, Cyc_Absyn_atb_typ( t, _temp540, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp599=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp599[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp600; _temp600.tag= Cyc_Absyn_Upper_b_tag;
_temp600.f1= _temp595; _temp600;}); _temp599;})), atts, tms->tl); _LL592: return
Cyc_Parse_apply_tms( _temp538, Cyc_Absyn_starb_typ( t, _temp540, tq,( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp601=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp601[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp602; _temp602.tag= Cyc_Absyn_Upper_b_tag; _temp602.f1= _temp597; _temp602;});
_temp601;})), atts, tms->tl); _LL594: return Cyc_Parse_apply_tms( _temp538, Cyc_Absyn_tagged_typ(
t, _temp540, tq), atts, tms->tl); _LL588:;} _LL527: return Cyc_Parse_apply_tms(
tq, t,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( atts, _temp544), tms->tl); _LL515:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp605; void* _temp607; struct _tuple5 _temp603= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL608: _temp607= _temp603.f1; goto _LL606; _LL606: _temp605=
_temp603.f2; goto _LL604; _LL604: if( _temp605 != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp609=( char*)"ignoring nested type declaration(s) in specifier list";
struct _tagged_string _temp610; _temp610.curr= _temp609; _temp610.base= _temp609;
_temp610.last_plus_one= _temp609 + 54; _temp610;}), loc);} return _temp607;}
static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d,
struct Cyc_Absyn_Stmt* s){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct*
_temp611=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp611[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp612; _temp612.tag= Cyc_Absyn_Decl_s_tag;
_temp612.f1= d; _temp612.f2= s; _temp612;}); _temp611;}), Cyc_Position_segment_join(
d->loc, s->loc));} static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_declarations(
struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt* s){ return(( struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*, struct Cyc_Absyn_Stmt*),
struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum)) Cyc_List_fold_right)(
Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List* Cyc_Parse_make_declarations(
struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List* ids, struct Cyc_Position_Segment*
loc){ struct Cyc_List_List* tss= ds->type_specs; struct Cyc_Absyn_Tqual* tq= ds->tq;
int istypedef= 0; void* s=( void*) Cyc_Absyn_Public; struct Cyc_List_List* atts=
ds->attributes; if( ds->is_inline){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp613=( char*)"inline is only allowed on function definitions"; struct
_tagged_string _temp614; _temp614.curr= _temp613; _temp614.base= _temp613;
_temp614.last_plus_one= _temp613 + 47; _temp614;}), loc);} if( tss == 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp615=( char*)"missing type specifiers in declaration";
struct _tagged_string _temp616; _temp616.curr= _temp615; _temp616.base= _temp615;
_temp616.last_plus_one= _temp615 + 39; _temp616;}), loc); return 0;} if( ds->sc
!= 0){ void* _temp617=( void*)( ds->sc)->v; _LL619: if(( int) _temp617 == Cyc_Parse_Typedef_sc){
goto _LL620;} else{ goto _LL621;} _LL621: if(( int) _temp617 == Cyc_Parse_Extern_sc){
goto _LL622;} else{ goto _LL623;} _LL623: if(( int) _temp617 == Cyc_Parse_ExternC_sc){
goto _LL624;} else{ goto _LL625;} _LL625: if(( int) _temp617 == Cyc_Parse_Static_sc){
goto _LL626;} else{ goto _LL627;} _LL627: if(( int) _temp617 == Cyc_Parse_Auto_sc){
goto _LL628;} else{ goto _LL629;} _LL629: if(( int) _temp617 == Cyc_Parse_Register_sc){
goto _LL630;} else{ goto _LL631;} _LL631: if(( int) _temp617 == Cyc_Parse_Abstract_sc){
goto _LL632;} else{ goto _LL618;} _LL620: istypedef= 1; goto _LL618; _LL622: s=(
void*) Cyc_Absyn_Extern; goto _LL618; _LL624: s=( void*) Cyc_Absyn_ExternC; goto
_LL618; _LL626: s=( void*) Cyc_Absyn_Static; goto _LL618; _LL628: s=( void*) Cyc_Absyn_Public;
goto _LL618; _LL630: s=( void*) Cyc_Absyn_Public; goto _LL618; _LL632: s=( void*)
Cyc_Absyn_Abstract; goto _LL618; _LL618:;}{ struct Cyc_List_List* _temp635;
struct Cyc_List_List* _temp637; struct _tuple0 _temp633=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL638: _temp637= _temp633.f1;
goto _LL636; _LL636: _temp635= _temp633.f2; goto _LL634; _LL634: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp635; for( 0; es != 0; es= es->tl){
if(( struct Cyc_Absyn_Exp*) es->hd != 0){ exps_empty= 0; break;}}}{ struct
_tuple5 ts_info= Cyc_Parse_collapse_type_specifiers( tss, loc); if( _temp637 ==
0){ struct Cyc_Core_Opt* _temp641; void* _temp643; struct _tuple5 _temp639=
ts_info; _LL644: _temp643= _temp639.f1; goto _LL642; _LL642: _temp641= _temp639.f2;
goto _LL640; _LL640: if( _temp641 != 0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)
_temp641->v;{ void* _temp645=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp659;
struct Cyc_Absyn_Structdecl* _temp661; struct Cyc_Absyn_Uniondecl* _temp663;
struct Cyc_Absyn_Tuniondecl* _temp665; struct Cyc_Absyn_XTuniondecl* _temp667;
_LL647: if((( struct _tunion_struct*) _temp645)->tag == Cyc_Absyn_Enum_d_tag){
_LL660: _temp659=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp645)->f1; goto _LL648;} else{ goto _LL649;} _LL649: if((( struct
_tunion_struct*) _temp645)->tag == Cyc_Absyn_Struct_d_tag){ _LL662: _temp661=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp645)->f1;
goto _LL650;} else{ goto _LL651;} _LL651: if((( struct _tunion_struct*) _temp645)->tag
== Cyc_Absyn_Union_d_tag){ _LL664: _temp663=( struct Cyc_Absyn_Uniondecl*)((
struct Cyc_Absyn_Union_d_struct*) _temp645)->f1; goto _LL652;} else{ goto _LL653;}
_LL653: if((( struct _tunion_struct*) _temp645)->tag == Cyc_Absyn_Tunion_d_tag){
_LL666: _temp665=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*)
_temp645)->f1; goto _LL654;} else{ goto _LL655;} _LL655: if((( struct
_tunion_struct*) _temp645)->tag == Cyc_Absyn_XTunion_d_tag){ _LL668: _temp667=(
struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_d_struct*) _temp645)->f1;
goto _LL656;} else{ goto _LL657;} _LL657: goto _LL658; _LL648:( void*)( _temp659->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp669=( char*)"bad attributes on enum"; struct _tagged_string _temp670;
_temp670.curr= _temp669; _temp670.base= _temp669; _temp670.last_plus_one=
_temp669 + 23; _temp670;}), loc);} goto _LL646; _LL650:( void*)( _temp661->sc=(
void*) s); _temp661->attributes= atts; goto _LL646; _LL652:( void*)( _temp663->sc=(
void*) s); _temp663->attributes= atts; goto _LL646; _LL654:( void*)( _temp665->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp671=( char*)"bad attributes on tunion"; struct _tagged_string _temp672;
_temp672.curr= _temp671; _temp672.base= _temp671; _temp672.last_plus_one=
_temp671 + 25; _temp672;}), loc);} goto _LL646; _LL656:( void*)( _temp667->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp673=( char*)"bad attributes on xtunion"; struct _tagged_string _temp674;
_temp674.curr= _temp673; _temp674.base= _temp673; _temp674.last_plus_one=
_temp673 + 26; _temp674;}), loc);} goto _LL646; _LL658: Cyc_Parse_err(( struct
_tagged_string)({ char* _temp675=( char*)"bad declaration"; struct
_tagged_string _temp676; _temp676.curr= _temp675; _temp676.base= _temp675;
_temp676.last_plus_one= _temp675 + 16; _temp676;}), loc); return 0; _LL646:;}
return({ struct Cyc_List_List* _temp677=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp677->hd=( void*) d; _temp677->tl= 0;
_temp677;});} else{ void* _temp678= _temp643; struct Cyc_Absyn_Structdecl**
_temp692; struct Cyc_List_List* _temp694; struct _tuple1* _temp696; struct Cyc_Absyn_TunionInfo
_temp698; struct Cyc_Absyn_Tuniondecl* _temp700; void* _temp702; struct Cyc_List_List*
_temp704; struct _tuple1* _temp706; struct Cyc_Absyn_XTunionInfo _temp708;
struct Cyc_Absyn_XTuniondecl* _temp710; void* _temp712; struct _tuple1* _temp714;
struct Cyc_Absyn_Uniondecl** _temp716; struct Cyc_List_List* _temp718; struct
_tuple1* _temp720; struct Cyc_Absyn_Enumdecl* _temp722; struct _tuple1* _temp724;
_LL680: if(( unsigned int) _temp678 > 4u?(( struct _tunion_struct*) _temp678)->tag
== Cyc_Absyn_StructType_tag: 0){ _LL697: _temp696=( struct _tuple1*)(( struct
Cyc_Absyn_StructType_struct*) _temp678)->f1; goto _LL695; _LL695: _temp694=(
struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*) _temp678)->f2;
goto _LL693; _LL693: _temp692=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp678)->f3; goto _LL681;} else{ goto _LL682;} _LL682: if(( unsigned int)
_temp678 > 4u?(( struct _tunion_struct*) _temp678)->tag == Cyc_Absyn_TunionType_tag:
0){ _LL699: _temp698=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp678)->f1; _LL707: _temp706=( struct _tuple1*) _temp698.name; goto _LL705;
_LL705: _temp704=( struct Cyc_List_List*) _temp698.targs; goto _LL703; _LL703:
_temp702=( void*) _temp698.rgn; goto _LL701; _LL701: _temp700=( struct Cyc_Absyn_Tuniondecl*)
_temp698.tud; goto _LL683;} else{ goto _LL684;} _LL684: if(( unsigned int)
_temp678 > 4u?(( struct _tunion_struct*) _temp678)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL709: _temp708=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp678)->f1; _LL715: _temp714=( struct _tuple1*) _temp708.name; goto _LL713;
_LL713: _temp712=( void*) _temp708.rgn; goto _LL711; _LL711: _temp710=( struct
Cyc_Absyn_XTuniondecl*) _temp708.xtud; goto _LL685;} else{ goto _LL686;} _LL686:
if(( unsigned int) _temp678 > 4u?(( struct _tunion_struct*) _temp678)->tag ==
Cyc_Absyn_UnionType_tag: 0){ _LL721: _temp720=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp678)->f1; goto _LL719; _LL719: _temp718=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp678)->f2; goto _LL717; _LL717: _temp716=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp678)->f3;
goto _LL687;} else{ goto _LL688;} _LL688: if(( unsigned int) _temp678 > 4u?((
struct _tunion_struct*) _temp678)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL725:
_temp724=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp678)->f1;
goto _LL723; _LL723: _temp722=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp678)->f2; goto _LL689;} else{ goto _LL690;} _LL690: goto _LL691; _LL681: {
struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp694);
struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl* _temp726=(
struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp726->sc=( void*) s; _temp726->name=({ struct Cyc_Core_Opt* _temp727=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp727->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp728= _temp696; if( _temp728 ==
0){ _throw( Null_Exception);} _temp728;})); _temp727;}); _temp726->tvs= ts2;
_temp726->fields= 0; _temp726->attributes= 0; _temp726;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp729=( char*)"bad attributes on struct";
struct _tagged_string _temp730; _temp730.curr= _temp729; _temp730.base= _temp729;
_temp730.last_plus_one= _temp729 + 25; _temp730;}), loc);} return({ struct Cyc_List_List*
_temp731=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp731->hd=( void*)({ struct Cyc_Absyn_Decl* _temp732=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp732->r=( void*)(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp733=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp733[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp734; _temp734.tag= Cyc_Absyn_Struct_d_tag;
_temp734.f1= sd; _temp734;}); _temp733;})); _temp732->loc= loc; _temp732;});
_temp731->tl= 0; _temp731;});} _LL683: { struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp704); struct Cyc_Absyn_Decl* tud= Cyc_Absyn_tunion_decl( s,({ struct Cyc_Core_Opt*
_temp735=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp735->v=( void*)(( struct _tuple1*) _temp706); _temp735;}), ts2, 0, loc);
if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp736=( char*)"bad attributes on tunion";
struct _tagged_string _temp737; _temp737.curr= _temp736; _temp737.base= _temp736;
_temp737.last_plus_one= _temp736 + 25; _temp737;}), loc);} return({ struct Cyc_List_List*
_temp738=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp738->hd=( void*) tud; _temp738->tl= 0; _temp738;});} _LL685: { struct Cyc_Absyn_XTuniondecl*
xtud=({ struct Cyc_Absyn_XTuniondecl* _temp739=( struct Cyc_Absyn_XTuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_XTuniondecl)); _temp739->sc=( void*) s;
_temp739->name= _temp714; _temp739->fields= 0; _temp739;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp740=( char*)"bad attributes on xtunion";
struct _tagged_string _temp741; _temp741.curr= _temp740; _temp741.base= _temp740;
_temp741.last_plus_one= _temp740 + 26; _temp741;}), loc);} return({ struct Cyc_List_List*
_temp742=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp742->hd=( void*)({ struct Cyc_Absyn_Decl* _temp743=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp743->r=( void*)(( void*)({
struct Cyc_Absyn_XTunion_d_struct* _temp744=( struct Cyc_Absyn_XTunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunion_d_struct)); _temp744[ 0]=({ struct
Cyc_Absyn_XTunion_d_struct _temp745; _temp745.tag= Cyc_Absyn_XTunion_d_tag;
_temp745.f1= xtud; _temp745;}); _temp744;})); _temp743->loc= loc; _temp743;});
_temp742->tl= 0; _temp742;});} _LL687: { struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp718); struct Cyc_Absyn_Uniondecl* ud=({ struct Cyc_Absyn_Uniondecl*
_temp746=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp746->sc=( void*) s; _temp746->name=({ struct Cyc_Core_Opt* _temp747=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp747->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp748= _temp720; if( _temp748 ==
0){ _throw( Null_Exception);} _temp748;})); _temp747;}); _temp746->tvs= ts2;
_temp746->fields= 0; _temp746->attributes= 0; _temp746;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp749=( char*)"bad attributes on union";
struct _tagged_string _temp750; _temp750.curr= _temp749; _temp750.base= _temp749;
_temp750.last_plus_one= _temp749 + 24; _temp750;}), loc);} return({ struct Cyc_List_List*
_temp751=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp751->hd=( void*)({ struct Cyc_Absyn_Decl* _temp752=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp752->r=( void*)(( void*)({
struct Cyc_Absyn_Union_d_struct* _temp753=( struct Cyc_Absyn_Union_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp753[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp754; _temp754.tag= Cyc_Absyn_Union_d_tag; _temp754.f1= ud; _temp754;});
_temp753;})); _temp752->loc= loc; _temp752;}); _temp751->tl= 0; _temp751;});}
_LL689: { struct Cyc_Absyn_Enumdecl* ed=({ struct Cyc_Absyn_Enumdecl* _temp755=(
struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp755->sc=( void*) s; _temp755->name= _temp724; _temp755->fields= 0; _temp755;});
if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp756=( char*)"bad attributes on enum";
struct _tagged_string _temp757; _temp757.curr= _temp756; _temp757.base= _temp756;
_temp757.last_plus_one= _temp756 + 23; _temp757;}), loc);} return({ struct Cyc_List_List*
_temp758=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp758->hd=( void*)({ struct Cyc_Absyn_Decl* _temp759=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp759->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp760=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp760[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp761; _temp761.tag= Cyc_Absyn_Enum_d_tag; _temp761.f1= ed; _temp761;});
_temp760;})); _temp759->loc= loc; _temp759;}); _temp758->tl= 0; _temp758;});}
_LL691: Cyc_Parse_err(( struct _tagged_string)({ char* _temp762=( char*)"missing declarator";
struct _tagged_string _temp763; _temp763.curr= _temp762; _temp763.base= _temp762;
_temp763.last_plus_one= _temp762 + 19; _temp763;}), loc); return 0; _LL679:;}}
else{ void* t= ts_info.f1; struct Cyc_List_List* fields= Cyc_Parse_apply_tmss(
tq, t, _temp637, atts); if( istypedef){ if( ! exps_empty){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp764=( char*)"initializer in typedef declaration";
struct _tagged_string _temp765; _temp765.curr= _temp764; _temp765.base= _temp764;
_temp765.last_plus_one= _temp764 + 35; _temp765;}), loc);}{ struct Cyc_List_List*
decls=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*,
struct _tuple7*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef, loc, fields); if( ts_info.f2 != 0){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)( ts_info.f2)->v;{ void*
_temp766=( void*) d->r; struct Cyc_Absyn_Structdecl* _temp780; struct Cyc_Absyn_Tuniondecl*
_temp782; struct Cyc_Absyn_XTuniondecl* _temp784; struct Cyc_Absyn_Uniondecl*
_temp786; struct Cyc_Absyn_Enumdecl* _temp788; _LL768: if((( struct
_tunion_struct*) _temp766)->tag == Cyc_Absyn_Struct_d_tag){ _LL781: _temp780=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp766)->f1;
goto _LL769;} else{ goto _LL770;} _LL770: if((( struct _tunion_struct*) _temp766)->tag
== Cyc_Absyn_Tunion_d_tag){ _LL783: _temp782=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp766)->f1; goto _LL771;} else{ goto
_LL772;} _LL772: if((( struct _tunion_struct*) _temp766)->tag == Cyc_Absyn_XTunion_d_tag){
_LL785: _temp784=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_d_struct*)
_temp766)->f1; goto _LL773;} else{ goto _LL774;} _LL774: if((( struct
_tunion_struct*) _temp766)->tag == Cyc_Absyn_Union_d_tag){ _LL787: _temp786=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp766)->f1;
goto _LL775;} else{ goto _LL776;} _LL776: if((( struct _tunion_struct*) _temp766)->tag
== Cyc_Absyn_Enum_d_tag){ _LL789: _temp788=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp766)->f1; goto _LL777;} else{ goto _LL778;}
_LL778: goto _LL779; _LL769:( void*)( _temp780->sc=( void*) s); _temp780->attributes=
atts; atts= 0; goto _LL767; _LL771:( void*)( _temp782->sc=( void*) s); goto
_LL767; _LL773:( void*)( _temp784->sc=( void*) s); goto _LL767; _LL775:( void*)(
_temp786->sc=( void*) s); goto _LL767; _LL777:( void*)( _temp788->sc=( void*) s);
goto _LL767; _LL779: Cyc_Parse_err(( struct _tagged_string)({ char* _temp790=(
char*)"declaration within typedef is not a struct, union, tunion, or xtunion";
struct _tagged_string _temp791; _temp791.curr= _temp790; _temp791.base= _temp790;
_temp791.last_plus_one= _temp790 + 70; _temp791;}), loc); goto _LL767; _LL767:;}
decls=({ struct Cyc_List_List* _temp792=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp792->hd=( void*) d; _temp792->tl= decls;
_temp792;});} if( atts != 0){ Cyc_Parse_err(({ struct _tagged_string _temp793=
Cyc_Absyn_attribute2string(( void*) atts->hd); xprintf("bad attribute %.*s in typedef",
_temp793.last_plus_one - _temp793.curr, _temp793.curr);}), loc);} return decls;}}
else{ if( ts_info.f2 != 0){ Cyc_Parse_unimp2(( struct _tagged_string)({ char*
_temp794=( char*)"nested type declaration within declarator"; struct
_tagged_string _temp795; _temp795.curr= _temp794; _temp795.base= _temp794;
_temp795.last_plus_one= _temp794 + 42; _temp795;}), loc);}{ struct Cyc_List_List*
decls= 0;{ struct Cyc_List_List* ds= fields; for( 0; ds != 0; ds= ds->tl,
_temp635= _temp635->tl){ struct _tuple7 _temp798; struct Cyc_List_List* _temp799;
struct Cyc_List_List* _temp801; void* _temp803; struct Cyc_Absyn_Tqual* _temp805;
struct _tuple1* _temp807; struct _tuple7* _temp796=( struct _tuple7*) ds->hd;
_temp798=* _temp796; _LL808: _temp807= _temp798.f1; goto _LL806; _LL806:
_temp805= _temp798.f2; goto _LL804; _LL804: _temp803= _temp798.f3; goto _LL802;
_LL802: _temp801= _temp798.f4; goto _LL800; _LL800: _temp799= _temp798.f5; goto
_LL797; _LL797: if( _temp801 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp809=( char*)"bad type params, ignoring"; struct _tagged_string
_temp810; _temp810.curr= _temp809; _temp810.base= _temp809; _temp810.last_plus_one=
_temp809 + 26; _temp810;}), loc);} if( _temp635 == 0){(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp811=( char*)"unexpected null in parse!"; struct
_tagged_string _temp812; _temp812.curr= _temp811; _temp812.base= _temp811;
_temp812.last_plus_one= _temp811 + 26; _temp812;}), loc);}{ struct Cyc_Absyn_Exp*
eopt=( struct Cyc_Absyn_Exp*) _temp635->hd; struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl(
_temp807, _temp803, eopt); vd->tq= _temp805;( void*)( vd->sc=( void*) s); vd->attributes=
_temp799;{ struct Cyc_Absyn_Decl* d=({ struct Cyc_Absyn_Decl* _temp813=( struct
Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp813->r=( void*)((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp814=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp814[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp815; _temp815.tag= Cyc_Absyn_Var_d_tag; _temp815.f1= vd; _temp815;});
_temp814;})); _temp813->loc= loc; _temp813;}); decls=({ struct Cyc_List_List*
_temp816=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp816->hd=( void*) d; _temp816->tl= decls; _temp816;});}}}} return(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}}
static void* Cyc_Parse_id_to_kind( struct _tagged_string s, struct Cyc_Position_Segment*
loc){ if( Cyc_String_strlen( s) !=( unsigned int) 1){ Cyc_Parse_err(({ struct
_tagged_string _temp817= s; xprintf("bad kind: %.*s", _temp817.last_plus_one -
_temp817.curr, _temp817.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;} else{
switch(({ struct _tagged_string _temp818= s; char* _temp820= _temp818.curr + 0;
if( _temp820 < _temp818.base? 1: _temp820 >= _temp818.last_plus_one){ _throw(
Null_Exception);}* _temp820;})){ case 'A': _LL821: return( void*) Cyc_Absyn_AnyKind;
case 'M': _LL822: return( void*) Cyc_Absyn_MemKind; case 'B': _LL823: return(
void*) Cyc_Absyn_BoxKind; case 'R': _LL824: return( void*) Cyc_Absyn_RgnKind;
case 'E': _LL825: return( void*) Cyc_Absyn_EffKind; default: _LL826: Cyc_Parse_err(({
struct _tagged_string _temp828= s; xprintf("bad kind: %.*s", _temp828.last_plus_one
- _temp828.curr, _temp828.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;}}}
static struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts == 0){
return tms;} else{ return({ struct Cyc_List_List* _temp829=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp829->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp830=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp830[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp831; _temp831.tag= Cyc_Absyn_Attributes_mod_tag;
_temp831.f1= loc; _temp831.f2= atts; _temp831;}); _temp830;})); _temp829->tl=
tms; _temp829;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1;
Cyc_Lex_register_typedef( x); if((* t).f5 != 0){ Cyc_Parse_err(({ struct
_tagged_string _temp832= Cyc_Absyn_attribute2string(( void*)((* t).f5)->hd);
xprintf("bad attribute %.*s within typedef", _temp832.last_plus_one - _temp832.curr,
_temp832.curr);}), loc);} return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct*
_temp833=( struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp833[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp834; _temp834.tag= Cyc_Absyn_Typedef_d_tag;
_temp834.f1=({ struct Cyc_Absyn_Typedefdecl* _temp835=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp835->name= x; _temp835->tvs=(*
t).f4; _temp835->defn=( void*)(* t).f3; _temp835;}); _temp834;}); _temp833;}),
loc);} typedef struct _xtunion_struct* Cyc_YYSTYPE; char Cyc_Okay_tok_tag[ 9u]="Okay_tok";
struct Cyc_Okay_tok_struct{ char* tag; } ; char Cyc_Int_tok_tag[ 8u]="Int_tok";
struct Cyc_Int_tok_struct{ char* tag; struct _tuple12* f1; } ; char Cyc_Char_tok_tag[
9u]="Char_tok"; struct Cyc_Char_tok_struct{ char* tag; char f1; } ; char Cyc_Pointer_Sort_tok_tag[
17u]="Pointer_Sort_tok"; struct Cyc_Pointer_Sort_tok_struct{ char* tag; void* f1;
} ; char Cyc_Short_tok_tag[ 10u]="Short_tok"; struct Cyc_Short_tok_struct{ char*
tag; short f1; } ; char Cyc_String_tok_tag[ 11u]="String_tok"; struct Cyc_String_tok_struct{
char* tag; struct _tagged_string f1; } ; char Cyc_Stringopt_tok_tag[ 14u]="Stringopt_tok";
struct Cyc_Stringopt_tok_struct{ char* tag; struct Cyc_Core_Opt* f1; } ; char
Cyc_Type_tok_tag[ 9u]="Type_tok"; struct Cyc_Type_tok_struct{ char* tag; void*
f1; } ; char Cyc_TypeList_tok_tag[ 13u]="TypeList_tok"; struct Cyc_TypeList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_Exp_tok_tag[ 8u]="Exp_tok";
struct Cyc_Exp_tok_struct{ char* tag; struct Cyc_Absyn_Exp* f1; } ; char Cyc_ExpList_tok_tag[
12u]="ExpList_tok"; struct Cyc_ExpList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; char Cyc_Primop_tok_tag[ 11u]="Primop_tok"; struct Cyc_Primop_tok_struct{
char* tag; void* f1; } ; char Cyc_Primopopt_tok_tag[ 14u]="Primopopt_tok";
struct Cyc_Primopopt_tok_struct{ char* tag; struct Cyc_Core_Opt* f1; } ; char
Cyc_QualId_tok_tag[ 11u]="QualId_tok"; struct Cyc_QualId_tok_struct{ char* tag;
struct _tuple1* f1; } ; char Cyc_Stmt_tok_tag[ 9u]="Stmt_tok"; struct Cyc_Stmt_tok_struct{
char* tag; struct Cyc_Absyn_Stmt* f1; } ; char Cyc_SwitchClauseList_tok_tag[ 21u]="SwitchClauseList_tok";
struct Cyc_SwitchClauseList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
char Cyc_Pattern_tok_tag[ 12u]="Pattern_tok"; struct Cyc_Pattern_tok_struct{
char* tag; struct Cyc_Absyn_Pat* f1; } ; char Cyc_PatternList_tok_tag[ 16u]="PatternList_tok";
struct Cyc_PatternList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; char
Cyc_FnDecl_tok_tag[ 11u]="FnDecl_tok"; struct Cyc_FnDecl_tok_struct{ char* tag;
struct Cyc_Absyn_Fndecl* f1; } ; char Cyc_DeclList_tok_tag[ 13u]="DeclList_tok";
struct Cyc_DeclList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; char
Cyc_DeclSpec_tok_tag[ 13u]="DeclSpec_tok"; struct Cyc_DeclSpec_tok_struct{ char*
tag; struct Cyc_Parse_Declaration_spec* f1; } ; char Cyc_InitDecl_tok_tag[ 13u]="InitDecl_tok";
struct Cyc_InitDecl_tok_struct{ char* tag; struct _tuple13* f1; } ; char Cyc_InitDeclList_tok_tag[
17u]="InitDeclList_tok"; struct Cyc_InitDeclList_tok_struct{ char* tag; struct
Cyc_List_List* f1; } ; char Cyc_StorageClass_tok_tag[ 17u]="StorageClass_tok";
struct Cyc_StorageClass_tok_struct{ char* tag; void* f1; } ; char Cyc_TypeSpecifier_tok_tag[
18u]="TypeSpecifier_tok"; struct Cyc_TypeSpecifier_tok_struct{ char* tag; void*
f1; } ; char Cyc_QualSpecList_tok_tag[ 17u]="QualSpecList_tok"; struct Cyc_QualSpecList_tok_struct{
char* tag; struct _tuple14* f1; } ; char Cyc_TypeQual_tok_tag[ 13u]="TypeQual_tok";
struct Cyc_TypeQual_tok_struct{ char* tag; struct Cyc_Absyn_Tqual* f1; } ; char
Cyc_StructFieldDeclList_tok_tag[ 24u]="StructFieldDeclList_tok"; struct Cyc_StructFieldDeclList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_StructFieldDeclListList_tok_tag[
28u]="StructFieldDeclListList_tok"; struct Cyc_StructFieldDeclListList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_Declarator_tok_tag[ 15u]="Declarator_tok";
struct Cyc_Declarator_tok_struct{ char* tag; struct Cyc_Parse_Declarator* f1; }
; char Cyc_DeclaratorExpopt_tok_tag[ 21u]="DeclaratorExpopt_tok"; struct Cyc_DeclaratorExpopt_tok_struct{
char* tag; struct _tuple15* f1; } ; char Cyc_DeclaratorExpoptList_tok_tag[ 25u]="DeclaratorExpoptList_tok";
struct Cyc_DeclaratorExpoptList_tok_struct{ char* tag; struct Cyc_List_List* f1;
} ; char Cyc_AbstractDeclarator_tok_tag[ 23u]="AbstractDeclarator_tok"; struct
Cyc_AbstractDeclarator_tok_struct{ char* tag; struct Cyc_Parse_Abstractdeclarator*
f1; } ; char Cyc_TunionField_tok_tag[ 16u]="TunionField_tok"; struct Cyc_TunionField_tok_struct{
char* tag; struct Cyc_Absyn_Tunionfield* f1; } ; char Cyc_TunionFieldList_tok_tag[
20u]="TunionFieldList_tok"; struct Cyc_TunionFieldList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; char Cyc_ParamDecl_tok_tag[ 14u]="ParamDecl_tok";
struct Cyc_ParamDecl_tok_struct{ char* tag; struct _tuple2* f1; } ; char Cyc_ParamDeclList_tok_tag[
18u]="ParamDeclList_tok"; struct Cyc_ParamDeclList_tok_struct{ char* tag; struct
Cyc_List_List* f1; } ; char Cyc_ParamDeclListBool_tok_tag[ 22u]="ParamDeclListBool_tok";
struct Cyc_ParamDeclListBool_tok_struct{ char* tag; struct _tuple16* f1; } ;
char Cyc_StructOrUnion_tok_tag[ 18u]="StructOrUnion_tok"; struct Cyc_StructOrUnion_tok_struct{
char* tag; void* f1; } ; char Cyc_IdList_tok_tag[ 11u]="IdList_tok"; struct Cyc_IdList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_Designator_tok_tag[ 15u]="Designator_tok";
struct Cyc_Designator_tok_struct{ char* tag; void* f1; } ; char Cyc_DesignatorList_tok_tag[
19u]="DesignatorList_tok"; struct Cyc_DesignatorList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; char Cyc_TypeModifierList_tok_tag[ 21u]="TypeModifierList_tok";
struct Cyc_TypeModifierList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
char Cyc_Rgn_tok_tag[ 8u]="Rgn_tok"; struct Cyc_Rgn_tok_struct{ char* tag; void*
f1; } ; char Cyc_InitializerList_tok_tag[ 20u]="InitializerList_tok"; struct Cyc_InitializerList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_FieldPattern_tok_tag[ 17u]="FieldPattern_tok";
struct Cyc_FieldPattern_tok_struct{ char* tag; struct _tuple17* f1; } ; char Cyc_FieldPatternList_tok_tag[
21u]="FieldPatternList_tok"; struct Cyc_FieldPatternList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; char Cyc_BlockItem_tok_tag[ 14u]="BlockItem_tok";
struct Cyc_BlockItem_tok_struct{ char* tag; void* f1; } ; char Cyc_Kind_tok_tag[
9u]="Kind_tok"; struct Cyc_Kind_tok_struct{ char* tag; void* f1; } ; char Cyc_Attribute_tok_tag[
14u]="Attribute_tok"; struct Cyc_Attribute_tok_struct{ char* tag; void* f1; } ;
char Cyc_AttributeList_tok_tag[ 18u]="AttributeList_tok"; struct Cyc_AttributeList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_Enumfield_tok_tag[ 14u]="Enumfield_tok";
struct Cyc_Enumfield_tok_struct{ char* tag; struct Cyc_Absyn_Enumfield* f1; } ;
char Cyc_EnumfieldList_tok_tag[ 18u]="EnumfieldList_tok"; struct Cyc_EnumfieldList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; static char _temp838[ 8u]="Int_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp838, _temp838, _temp838 + 8u}}; struct _tuple12*
Cyc_yyget_Int_tok( struct _xtunion_struct* yy1){ struct _xtunion_struct*
_temp839= yy1; struct _tuple12* _temp845; _LL841: if((*(( struct _xtunion_struct*)
_temp839)).tag == Cyc_Int_tok_tag){ _LL846: _temp845=(( struct Cyc_Int_tok_struct*)
_temp839)->f1; goto _LL842;} else{ goto _LL843;} _LL843: goto _LL844; _LL842:
return _temp845; _LL844:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Int_tok);
_LL840:;} static char _temp849[ 11u]="String_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp849,
_temp849, _temp849 + 11u}}; struct _tagged_string Cyc_yyget_String_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp850= yy1; struct
_tagged_string _temp856; _LL852: if((*(( struct _xtunion_struct*) _temp850)).tag
== Cyc_String_tok_tag){ _LL857: _temp856=(( struct Cyc_String_tok_struct*)
_temp850)->f1; goto _LL853;} else{ goto _LL854;} _LL854: goto _LL855; _LL853:
return _temp856; _LL855:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_String_tok);
_LL851:;} static char _temp860[ 9u]="Char_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Char_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp860,
_temp860, _temp860 + 9u}}; char Cyc_yyget_Char_tok( struct _xtunion_struct* yy1){
struct _xtunion_struct* _temp861= yy1; char _temp867; _LL863: if((*(( struct
_xtunion_struct*) _temp861)).tag == Cyc_Char_tok_tag){ _LL868: _temp867=((
struct Cyc_Char_tok_struct*) _temp861)->f1; goto _LL864;} else{ goto _LL865;}
_LL865: goto _LL866; _LL864: return _temp867; _LL866:( void) _throw(( struct
_xtunion_struct*)& Cyc_yyfail_Char_tok); _LL862:;} static char _temp871[ 17u]="Pointer_Sort_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp871, _temp871, _temp871 + 17u}}; void* Cyc_yyget_Pointer_Sort_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp872= yy1; void*
_temp878; _LL874: if((*(( struct _xtunion_struct*) _temp872)).tag == Cyc_Pointer_Sort_tok_tag){
_LL879: _temp878=(( struct Cyc_Pointer_Sort_tok_struct*) _temp872)->f1; goto
_LL875;} else{ goto _LL876;} _LL876: goto _LL877; _LL875: return _temp878;
_LL877:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Pointer_Sort_tok);
_LL873:;} static char _temp882[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Exp_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp882,
_temp882, _temp882 + 8u}}; struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp883= yy1; struct Cyc_Absyn_Exp*
_temp889; _LL885: if((*(( struct _xtunion_struct*) _temp883)).tag == Cyc_Exp_tok_tag){
_LL890: _temp889=(( struct Cyc_Exp_tok_struct*) _temp883)->f1; goto _LL886;}
else{ goto _LL887;} _LL887: goto _LL888; _LL886: return _temp889; _LL888:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_Exp_tok); _LL884:;} static char
_temp893[ 12u]="ExpList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp893, _temp893, _temp893 + 12u}};
struct Cyc_List_List* Cyc_yyget_ExpList_tok( struct _xtunion_struct* yy1){
struct _xtunion_struct* _temp894= yy1; struct Cyc_List_List* _temp900; _LL896:
if((*(( struct _xtunion_struct*) _temp894)).tag == Cyc_ExpList_tok_tag){ _LL901:
_temp900=(( struct Cyc_ExpList_tok_struct*) _temp894)->f1; goto _LL897;} else{
goto _LL898;} _LL898: goto _LL899; _LL897: return _temp900; _LL899:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_ExpList_tok); _LL895:;} static
char _temp904[ 20u]="InitializerList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp904, _temp904, _temp904 + 20u}}; struct Cyc_List_List* Cyc_yyget_InitializerList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp905= yy1; struct Cyc_List_List*
_temp911; _LL907: if((*(( struct _xtunion_struct*) _temp905)).tag == Cyc_InitializerList_tok_tag){
_LL912: _temp911=(( struct Cyc_InitializerList_tok_struct*) _temp905)->f1; goto
_LL908;} else{ goto _LL909;} _LL909: goto _LL910; _LL908: return _temp911;
_LL910:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_InitializerList_tok);
_LL906:;} static char _temp915[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp915,
_temp915, _temp915 + 11u}}; void* Cyc_yyget_Primop_tok( struct _xtunion_struct*
yy1){ struct _xtunion_struct* _temp916= yy1; void* _temp922; _LL918: if((*((
struct _xtunion_struct*) _temp916)).tag == Cyc_Primop_tok_tag){ _LL923: _temp922=((
struct Cyc_Primop_tok_struct*) _temp916)->f1; goto _LL919;} else{ goto _LL920;}
_LL920: goto _LL921; _LL919: return _temp922; _LL921:( void) _throw(( struct
_xtunion_struct*)& Cyc_yyfail_Primop_tok); _LL917:;} static char _temp926[ 14u]="Primopopt_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp926, _temp926, _temp926 + 14u}}; struct Cyc_Core_Opt*
Cyc_yyget_Primopopt_tok( struct _xtunion_struct* yy1){ struct _xtunion_struct*
_temp927= yy1; struct Cyc_Core_Opt* _temp933; _LL929: if((*(( struct
_xtunion_struct*) _temp927)).tag == Cyc_Primopopt_tok_tag){ _LL934: _temp933=((
struct Cyc_Primopopt_tok_struct*) _temp927)->f1; goto _LL930;} else{ goto _LL931;}
_LL931: goto _LL932; _LL930: return _temp933; _LL932:( void) _throw(( struct
_xtunion_struct*)& Cyc_yyfail_Primopopt_tok); _LL928:;} static char _temp937[ 11u]="QualId_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp937, _temp937, _temp937 + 11u}}; struct _tuple1*
Cyc_yyget_QualId_tok( struct _xtunion_struct* yy1){ struct _xtunion_struct*
_temp938= yy1; struct _tuple1* _temp944; _LL940: if((*(( struct _xtunion_struct*)
_temp938)).tag == Cyc_QualId_tok_tag){ _LL945: _temp944=(( struct Cyc_QualId_tok_struct*)
_temp938)->f1; goto _LL941;} else{ goto _LL942;} _LL942: goto _LL943; _LL941:
return _temp944; _LL943:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_QualId_tok);
_LL939:;} static char _temp948[ 9u]="Stmt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp948,
_temp948, _temp948 + 9u}}; struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp949= yy1; struct Cyc_Absyn_Stmt*
_temp955; _LL951: if((*(( struct _xtunion_struct*) _temp949)).tag == Cyc_Stmt_tok_tag){
_LL956: _temp955=(( struct Cyc_Stmt_tok_struct*) _temp949)->f1; goto _LL952;}
else{ goto _LL953;} _LL953: goto _LL954; _LL952: return _temp955; _LL954:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_Stmt_tok); _LL950:;} static char
_temp959[ 14u]="BlockItem_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_BlockItem_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp959, _temp959, _temp959 + 14u}};
void* Cyc_yyget_BlockItem_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp960= yy1; void* _temp966; _LL962: if((*(( struct
_xtunion_struct*) _temp960)).tag == Cyc_BlockItem_tok_tag){ _LL967: _temp966=((
struct Cyc_BlockItem_tok_struct*) _temp960)->f1; goto _LL963;} else{ goto _LL964;}
_LL964: goto _LL965; _LL963: return _temp966; _LL965:( void) _throw(( struct
_xtunion_struct*)& Cyc_yyfail_BlockItem_tok); _LL961:;} static char _temp970[ 21u]="SwitchClauseList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp970, _temp970, _temp970 + 21u}}; struct Cyc_List_List*
Cyc_yyget_SwitchClauseList_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp971= yy1; struct Cyc_List_List* _temp977; _LL973: if((*((
struct _xtunion_struct*) _temp971)).tag == Cyc_SwitchClauseList_tok_tag){ _LL978:
_temp977=(( struct Cyc_SwitchClauseList_tok_struct*) _temp971)->f1; goto _LL974;}
else{ goto _LL975;} _LL975: goto _LL976; _LL974: return _temp977; _LL976:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_SwitchClauseList_tok); _LL972:;}
static char _temp981[ 12u]="Pattern_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp981,
_temp981, _temp981 + 12u}}; struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp982= yy1; struct Cyc_Absyn_Pat*
_temp988; _LL984: if((*(( struct _xtunion_struct*) _temp982)).tag == Cyc_Pattern_tok_tag){
_LL989: _temp988=(( struct Cyc_Pattern_tok_struct*) _temp982)->f1; goto _LL985;}
else{ goto _LL986;} _LL986: goto _LL987; _LL985: return _temp988; _LL987:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_Pattern_tok); _LL983:;} static
char _temp992[ 16u]="PatternList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_PatternList_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp992, _temp992, _temp992 + 16u}};
struct Cyc_List_List* Cyc_yyget_PatternList_tok( struct _xtunion_struct* yy1){
struct _xtunion_struct* _temp993= yy1; struct Cyc_List_List* _temp999; _LL995:
if((*(( struct _xtunion_struct*) _temp993)).tag == Cyc_PatternList_tok_tag){
_LL1000: _temp999=(( struct Cyc_PatternList_tok_struct*) _temp993)->f1; goto
_LL996;} else{ goto _LL997;} _LL997: goto _LL998; _LL996: return _temp999;
_LL998:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_PatternList_tok);
_LL994:;} static char _temp1003[ 17u]="FieldPattern_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FieldPattern_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1003, _temp1003, _temp1003 + 17u}}; struct _tuple17* Cyc_yyget_FieldPattern_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1004= yy1; struct
_tuple17* _temp1010; _LL1006: if((*(( struct _xtunion_struct*) _temp1004)).tag
== Cyc_FieldPattern_tok_tag){ _LL1011: _temp1010=(( struct Cyc_FieldPattern_tok_struct*)
_temp1004)->f1; goto _LL1007;} else{ goto _LL1008;} _LL1008: goto _LL1009;
_LL1007: return _temp1010; _LL1009:( void) _throw(( struct _xtunion_struct*)&
Cyc_yyfail_FieldPattern_tok); _LL1005:;} static char _temp1014[ 21u]="FieldPatternList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1014, _temp1014, _temp1014 + 21u}}; struct Cyc_List_List*
Cyc_yyget_FieldPatternList_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1015= yy1; struct Cyc_List_List* _temp1021; _LL1017: if((*((
struct _xtunion_struct*) _temp1015)).tag == Cyc_FieldPatternList_tok_tag){
_LL1022: _temp1021=(( struct Cyc_FieldPatternList_tok_struct*) _temp1015)->f1;
goto _LL1018;} else{ goto _LL1019;} _LL1019: goto _LL1020; _LL1018: return
_temp1021; _LL1020:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_FieldPatternList_tok);
_LL1016:;} static char _temp1025[ 11u]="FnDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1025,
_temp1025, _temp1025 + 11u}}; struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1026= yy1; struct Cyc_Absyn_Fndecl*
_temp1032; _LL1028: if((*(( struct _xtunion_struct*) _temp1026)).tag == Cyc_FnDecl_tok_tag){
_LL1033: _temp1032=(( struct Cyc_FnDecl_tok_struct*) _temp1026)->f1; goto
_LL1029;} else{ goto _LL1030;} _LL1030: goto _LL1031; _LL1029: return _temp1032;
_LL1031:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_FnDecl_tok);
_LL1027:;} static char _temp1036[ 13u]="DeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1036, _temp1036, _temp1036 + 13u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1037= yy1; struct Cyc_List_List*
_temp1043; _LL1039: if((*(( struct _xtunion_struct*) _temp1037)).tag == Cyc_DeclList_tok_tag){
_LL1044: _temp1043=(( struct Cyc_DeclList_tok_struct*) _temp1037)->f1; goto
_LL1040;} else{ goto _LL1041;} _LL1041: goto _LL1042; _LL1040: return _temp1043;
_LL1042:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_DeclList_tok);
_LL1038:;} static char _temp1047[ 13u]="DeclSpec_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1047, _temp1047, _temp1047 + 13u}}; struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1048= yy1; struct Cyc_Parse_Declaration_spec*
_temp1054; _LL1050: if((*(( struct _xtunion_struct*) _temp1048)).tag == Cyc_DeclSpec_tok_tag){
_LL1055: _temp1054=(( struct Cyc_DeclSpec_tok_struct*) _temp1048)->f1; goto
_LL1051;} else{ goto _LL1052;} _LL1052: goto _LL1053; _LL1051: return _temp1054;
_LL1053:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_DeclSpec_tok);
_LL1049:;} static char _temp1058[ 13u]="InitDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1058, _temp1058, _temp1058 + 13u}}; struct _tuple13* Cyc_yyget_InitDecl_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1059= yy1; struct
_tuple13* _temp1065; _LL1061: if((*(( struct _xtunion_struct*) _temp1059)).tag
== Cyc_InitDecl_tok_tag){ _LL1066: _temp1065=(( struct Cyc_InitDecl_tok_struct*)
_temp1059)->f1; goto _LL1062;} else{ goto _LL1063;} _LL1063: goto _LL1064;
_LL1062: return _temp1065; _LL1064:( void) _throw(( struct _xtunion_struct*)&
Cyc_yyfail_InitDecl_tok); _LL1060:;} static char _temp1069[ 17u]="InitDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1069, _temp1069, _temp1069 + 17u}}; struct Cyc_List_List*
Cyc_yyget_InitDeclList_tok( struct _xtunion_struct* yy1){ struct _xtunion_struct*
_temp1070= yy1; struct Cyc_List_List* _temp1076; _LL1072: if((*(( struct
_xtunion_struct*) _temp1070)).tag == Cyc_InitDeclList_tok_tag){ _LL1077:
_temp1076=(( struct Cyc_InitDeclList_tok_struct*) _temp1070)->f1; goto _LL1073;}
else{ goto _LL1074;} _LL1074: goto _LL1075; _LL1073: return _temp1076; _LL1075:(
void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_InitDeclList_tok); _LL1071:;}
static char _temp1080[ 17u]="StorageClass_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1080, _temp1080, _temp1080 + 17u}}; void* Cyc_yyget_StorageClass_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1081= yy1; void*
_temp1087; _LL1083: if((*(( struct _xtunion_struct*) _temp1081)).tag == Cyc_StorageClass_tok_tag){
_LL1088: _temp1087=(( struct Cyc_StorageClass_tok_struct*) _temp1081)->f1; goto
_LL1084;} else{ goto _LL1085;} _LL1085: goto _LL1086; _LL1084: return _temp1087;
_LL1086:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_StorageClass_tok);
_LL1082:;} static char _temp1091[ 18u]="TypeSpecifier_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1091, _temp1091, _temp1091 + 18u}}; void* Cyc_yyget_TypeSpecifier_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1092= yy1; void*
_temp1098; _LL1094: if((*(( struct _xtunion_struct*) _temp1092)).tag == Cyc_TypeSpecifier_tok_tag){
_LL1099: _temp1098=(( struct Cyc_TypeSpecifier_tok_struct*) _temp1092)->f1; goto
_LL1095;} else{ goto _LL1096;} _LL1096: goto _LL1097; _LL1095: return _temp1098;
_LL1097:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TypeSpecifier_tok);
_LL1093:;} static char _temp1102[ 18u]="StructOrUnion_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1102, _temp1102, _temp1102 + 18u}}; void* Cyc_yyget_StructOrUnion_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1103= yy1; void*
_temp1109; _LL1105: if((*(( struct _xtunion_struct*) _temp1103)).tag == Cyc_StructOrUnion_tok_tag){
_LL1110: _temp1109=(( struct Cyc_StructOrUnion_tok_struct*) _temp1103)->f1; goto
_LL1106;} else{ goto _LL1107;} _LL1107: goto _LL1108; _LL1106: return _temp1109;
_LL1108:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_StructOrUnion_tok);
_LL1104:;} static char _temp1113[ 13u]="TypeQual_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1113, _temp1113, _temp1113 + 13u}}; struct Cyc_Absyn_Tqual* Cyc_yyget_TypeQual_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1114= yy1; struct Cyc_Absyn_Tqual*
_temp1120; _LL1116: if((*(( struct _xtunion_struct*) _temp1114)).tag == Cyc_TypeQual_tok_tag){
_LL1121: _temp1120=(( struct Cyc_TypeQual_tok_struct*) _temp1114)->f1; goto
_LL1117;} else{ goto _LL1118;} _LL1118: goto _LL1119; _LL1117: return _temp1120;
_LL1119:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TypeQual_tok);
_LL1115:;} static char _temp1124[ 24u]="StructFieldDeclList_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1124, _temp1124, _temp1124 + 24u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclList_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1125= yy1; struct Cyc_List_List* _temp1131; _LL1127: if((*((
struct _xtunion_struct*) _temp1125)).tag == Cyc_StructFieldDeclList_tok_tag){
_LL1132: _temp1131=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp1125)->f1;
goto _LL1128;} else{ goto _LL1129;} _LL1129: goto _LL1130; _LL1128: return
_temp1131; _LL1130:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL1126:;} static char _temp1135[ 28u]="StructFieldDeclListList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclListList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1135, _temp1135, _temp1135 + 28u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclListList_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1136= yy1; struct Cyc_List_List* _temp1142; _LL1138: if((*((
struct _xtunion_struct*) _temp1136)).tag == Cyc_StructFieldDeclListList_tok_tag){
_LL1143: _temp1142=(( struct Cyc_StructFieldDeclListList_tok_struct*) _temp1136)->f1;
goto _LL1139;} else{ goto _LL1140;} _LL1140: goto _LL1141; _LL1139: return
_temp1142; _LL1141:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL1137:;} static char _temp1146[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1146, _temp1146, _temp1146 + 21u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1147= yy1; struct Cyc_List_List*
_temp1153; _LL1149: if((*(( struct _xtunion_struct*) _temp1147)).tag == Cyc_TypeModifierList_tok_tag){
_LL1154: _temp1153=(( struct Cyc_TypeModifierList_tok_struct*) _temp1147)->f1;
goto _LL1150;} else{ goto _LL1151;} _LL1151: goto _LL1152; _LL1150: return
_temp1153; _LL1152:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TypeModifierList_tok);
_LL1148:;} static char _temp1157[ 8u]="Rgn_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgn_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1157,
_temp1157, _temp1157 + 8u}}; void* Cyc_yyget_Rgn_tok( struct _xtunion_struct*
yy1){ struct _xtunion_struct* _temp1158= yy1; void* _temp1164; _LL1160: if((*((
struct _xtunion_struct*) _temp1158)).tag == Cyc_Rgn_tok_tag){ _LL1165: _temp1164=((
struct Cyc_Rgn_tok_struct*) _temp1158)->f1; goto _LL1161;} else{ goto _LL1162;}
_LL1162: goto _LL1163; _LL1161: return _temp1164; _LL1163:( void) _throw((
struct _xtunion_struct*)& Cyc_yyfail_Rgn_tok); _LL1159:;} static char _temp1168[
15u]="Declarator_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1168, _temp1168, _temp1168 +
15u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp1169= yy1; struct Cyc_Parse_Declarator*
_temp1175; _LL1171: if((*(( struct _xtunion_struct*) _temp1169)).tag == Cyc_Declarator_tok_tag){
_LL1176: _temp1175=(( struct Cyc_Declarator_tok_struct*) _temp1169)->f1; goto
_LL1172;} else{ goto _LL1173;} _LL1173: goto _LL1174; _LL1172: return _temp1175;
_LL1174:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Declarator_tok);
_LL1170:;} static char _temp1179[ 21u]="DeclaratorExpopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1179, _temp1179, _temp1179 + 21u}}; struct _tuple15* Cyc_yyget_DeclaratorExpopt_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1180= yy1; struct
_tuple15* _temp1186; _LL1182: if((*(( struct _xtunion_struct*) _temp1180)).tag
== Cyc_DeclaratorExpopt_tok_tag){ _LL1187: _temp1186=(( struct Cyc_DeclaratorExpopt_tok_struct*)
_temp1180)->f1; goto _LL1183;} else{ goto _LL1184;} _LL1184: goto _LL1185;
_LL1183: return _temp1186; _LL1185:( void) _throw(( struct _xtunion_struct*)&
Cyc_yyfail_DeclaratorExpopt_tok); _LL1181:;} static char _temp1190[ 25u]="DeclaratorExpoptList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1190, _temp1190, _temp1190 + 25u}}; struct Cyc_List_List*
Cyc_yyget_DeclaratorExpoptList_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1191= yy1; struct Cyc_List_List* _temp1197; _LL1193: if((*((
struct _xtunion_struct*) _temp1191)).tag == Cyc_DeclaratorExpoptList_tok_tag){
_LL1198: _temp1197=(( struct Cyc_DeclaratorExpoptList_tok_struct*) _temp1191)->f1;
goto _LL1194;} else{ goto _LL1195;} _LL1195: goto _LL1196; _LL1194: return
_temp1197; _LL1196:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_DeclaratorExpoptList_tok);
_LL1192:;} static char _temp1201[ 23u]="AbstractDeclarator_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1201, _temp1201, _temp1201 + 23u}}; struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_AbstractDeclarator_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1202= yy1; struct Cyc_Parse_Abstractdeclarator* _temp1208;
_LL1204: if((*(( struct _xtunion_struct*) _temp1202)).tag == Cyc_AbstractDeclarator_tok_tag){
_LL1209: _temp1208=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1202)->f1;
goto _LL1205;} else{ goto _LL1206;} _LL1206: goto _LL1207; _LL1205: return
_temp1208; _LL1207:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL1203:;} static char _temp1212[ 16u]="TunionField_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1212, _temp1212, _temp1212 + 16u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1213= yy1; struct Cyc_Absyn_Tunionfield*
_temp1219; _LL1215: if((*(( struct _xtunion_struct*) _temp1213)).tag == Cyc_TunionField_tok_tag){
_LL1220: _temp1219=(( struct Cyc_TunionField_tok_struct*) _temp1213)->f1; goto
_LL1216;} else{ goto _LL1217;} _LL1217: goto _LL1218; _LL1216: return _temp1219;
_LL1218:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TunionField_tok);
_LL1214:;} static char _temp1223[ 20u]="TunionFieldList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionFieldList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1223, _temp1223, _temp1223 + 20u}}; struct Cyc_List_List* Cyc_yyget_TunionFieldList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1224= yy1; struct Cyc_List_List*
_temp1230; _LL1226: if((*(( struct _xtunion_struct*) _temp1224)).tag == Cyc_TunionFieldList_tok_tag){
_LL1231: _temp1230=(( struct Cyc_TunionFieldList_tok_struct*) _temp1224)->f1;
goto _LL1227;} else{ goto _LL1228;} _LL1228: goto _LL1229; _LL1227: return
_temp1230; _LL1229:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TunionFieldList_tok);
_LL1225:;} static char _temp1234[ 17u]="QualSpecList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1234, _temp1234, _temp1234 + 17u}}; struct _tuple14* Cyc_yyget_QualSpecList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1235= yy1; struct
_tuple14* _temp1241; _LL1237: if((*(( struct _xtunion_struct*) _temp1235)).tag
== Cyc_QualSpecList_tok_tag){ _LL1242: _temp1241=(( struct Cyc_QualSpecList_tok_struct*)
_temp1235)->f1; goto _LL1238;} else{ goto _LL1239;} _LL1239: goto _LL1240;
_LL1238: return _temp1241; _LL1240:( void) _throw(( struct _xtunion_struct*)&
Cyc_yyfail_QualSpecList_tok); _LL1236:;} static char _temp1245[ 11u]="IdList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1245, _temp1245, _temp1245 + 11u}}; struct Cyc_List_List*
Cyc_yyget_IdList_tok( struct _xtunion_struct* yy1){ struct _xtunion_struct*
_temp1246= yy1; struct Cyc_List_List* _temp1252; _LL1248: if((*(( struct
_xtunion_struct*) _temp1246)).tag == Cyc_IdList_tok_tag){ _LL1253: _temp1252=((
struct Cyc_IdList_tok_struct*) _temp1246)->f1; goto _LL1249;} else{ goto _LL1250;}
_LL1250: goto _LL1251; _LL1249: return _temp1252; _LL1251:( void) _throw((
struct _xtunion_struct*)& Cyc_yyfail_IdList_tok); _LL1247:;} static char
_temp1256[ 14u]="ParamDecl_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1256, _temp1256, _temp1256 +
14u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok( struct _xtunion_struct* yy1){
struct _xtunion_struct* _temp1257= yy1; struct _tuple2* _temp1263; _LL1259: if((*((
struct _xtunion_struct*) _temp1257)).tag == Cyc_ParamDecl_tok_tag){ _LL1264:
_temp1263=(( struct Cyc_ParamDecl_tok_struct*) _temp1257)->f1; goto _LL1260;}
else{ goto _LL1261;} _LL1261: goto _LL1262; _LL1260: return _temp1263; _LL1262:(
void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_ParamDecl_tok); _LL1258:;}
static char _temp1267[ 18u]="ParamDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1267, _temp1267, _temp1267 + 18u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1268= yy1; struct Cyc_List_List*
_temp1274; _LL1270: if((*(( struct _xtunion_struct*) _temp1268)).tag == Cyc_ParamDeclList_tok_tag){
_LL1275: _temp1274=(( struct Cyc_ParamDeclList_tok_struct*) _temp1268)->f1; goto
_LL1271;} else{ goto _LL1272;} _LL1272: goto _LL1273; _LL1271: return _temp1274;
_LL1273:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_ParamDeclList_tok);
_LL1269:;} static char _temp1278[ 22u]="ParamDeclListBool_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1278, _temp1278, _temp1278 + 22u}}; struct _tuple16*
Cyc_yyget_ParamDeclListBool_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1279= yy1; struct _tuple16* _temp1285; _LL1281: if((*((
struct _xtunion_struct*) _temp1279)).tag == Cyc_ParamDeclListBool_tok_tag){
_LL1286: _temp1285=(( struct Cyc_ParamDeclListBool_tok_struct*) _temp1279)->f1;
goto _LL1282;} else{ goto _LL1283;} _LL1283: goto _LL1284; _LL1282: return
_temp1285; _LL1284:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_ParamDeclListBool_tok);
_LL1280:;} static char _temp1289[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1289, _temp1289, _temp1289 + 13u}}; struct Cyc_List_List* Cyc_yyget_TypeList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1290= yy1; struct Cyc_List_List*
_temp1296; _LL1292: if((*(( struct _xtunion_struct*) _temp1290)).tag == Cyc_TypeList_tok_tag){
_LL1297: _temp1296=(( struct Cyc_TypeList_tok_struct*) _temp1290)->f1; goto
_LL1293;} else{ goto _LL1294;} _LL1294: goto _LL1295; _LL1293: return _temp1296;
_LL1295:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TypeList_tok);
_LL1291:;} static char _temp1300[ 19u]="DesignatorList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1300, _temp1300, _temp1300 + 19u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1301= yy1; struct Cyc_List_List*
_temp1307; _LL1303: if((*(( struct _xtunion_struct*) _temp1301)).tag == Cyc_DesignatorList_tok_tag){
_LL1308: _temp1307=(( struct Cyc_DesignatorList_tok_struct*) _temp1301)->f1;
goto _LL1304;} else{ goto _LL1305;} _LL1305: goto _LL1306; _LL1304: return
_temp1307; _LL1306:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_DesignatorList_tok);
_LL1302:;} static char _temp1311[ 15u]="Designator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Designator_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1311, _temp1311, _temp1311 + 15u}}; void* Cyc_yyget_Designator_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp1312= yy1; void* _temp1318;
_LL1314: if((*(( struct _xtunion_struct*) _temp1312)).tag == Cyc_Designator_tok_tag){
_LL1319: _temp1318=(( struct Cyc_Designator_tok_struct*) _temp1312)->f1; goto
_LL1315;} else{ goto _LL1316;} _LL1316: goto _LL1317; _LL1315: return _temp1318;
_LL1317:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Designator_tok);
_LL1313:;} static char _temp1322[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1322,
_temp1322, _temp1322 + 9u}}; void* Cyc_yyget_Kind_tok( struct _xtunion_struct*
yy1){ struct _xtunion_struct* _temp1323= yy1; void* _temp1329; _LL1325: if((*((
struct _xtunion_struct*) _temp1323)).tag == Cyc_Kind_tok_tag){ _LL1330:
_temp1329=(( struct Cyc_Kind_tok_struct*) _temp1323)->f1; goto _LL1326;} else{
goto _LL1327;} _LL1327: goto _LL1328; _LL1326: return _temp1329; _LL1328:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_Kind_tok); _LL1324:;} static char
_temp1333[ 9u]="Type_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1333, _temp1333, _temp1333 +
9u}}; void* Cyc_yyget_Type_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1334= yy1; void* _temp1340; _LL1336: if((*(( struct
_xtunion_struct*) _temp1334)).tag == Cyc_Type_tok_tag){ _LL1341: _temp1340=((
struct Cyc_Type_tok_struct*) _temp1334)->f1; goto _LL1337;} else{ goto _LL1338;}
_LL1338: goto _LL1339; _LL1337: return _temp1340; _LL1339:( void) _throw((
struct _xtunion_struct*)& Cyc_yyfail_Type_tok); _LL1335:;} static char _temp1344[
18u]="AttributeList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1344, _temp1344, _temp1344 +
18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok( struct _xtunion_struct*
yy1){ struct _xtunion_struct* _temp1345= yy1; struct Cyc_List_List* _temp1351;
_LL1347: if((*(( struct _xtunion_struct*) _temp1345)).tag == Cyc_AttributeList_tok_tag){
_LL1352: _temp1351=(( struct Cyc_AttributeList_tok_struct*) _temp1345)->f1; goto
_LL1348;} else{ goto _LL1349;} _LL1349: goto _LL1350; _LL1348: return _temp1351;
_LL1350:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_AttributeList_tok);
_LL1346:;} static char _temp1355[ 14u]="Attribute_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1355, _temp1355, _temp1355 + 14u}}; void* Cyc_yyget_Attribute_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp1356= yy1; void* _temp1362;
_LL1358: if((*(( struct _xtunion_struct*) _temp1356)).tag == Cyc_Attribute_tok_tag){
_LL1363: _temp1362=(( struct Cyc_Attribute_tok_struct*) _temp1356)->f1; goto
_LL1359;} else{ goto _LL1360;} _LL1360: goto _LL1361; _LL1359: return _temp1362;
_LL1361:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Attribute_tok);
_LL1357:;} static char _temp1366[ 14u]="Enumfield_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1366, _temp1366, _temp1366 + 14u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1367= yy1; struct Cyc_Absyn_Enumfield*
_temp1373; _LL1369: if((*(( struct _xtunion_struct*) _temp1367)).tag == Cyc_Enumfield_tok_tag){
_LL1374: _temp1373=(( struct Cyc_Enumfield_tok_struct*) _temp1367)->f1; goto
_LL1370;} else{ goto _LL1371;} _LL1371: goto _LL1372; _LL1370: return _temp1373;
_LL1372:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Enumfield_tok);
_LL1368:;} static char _temp1377[ 18u]="EnumfieldList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1377, _temp1377, _temp1377 + 18u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1378= yy1; struct Cyc_List_List*
_temp1384; _LL1380: if((*(( struct _xtunion_struct*) _temp1378)).tag == Cyc_EnumfieldList_tok_tag){
_LL1385: _temp1384=(( struct Cyc_EnumfieldList_tok_struct*) _temp1378)->f1; goto
_LL1381;} else{ goto _LL1382;} _LL1382: goto _LL1383; _LL1381: return _temp1384;
_LL1383:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_EnumfieldList_tok);
_LL1379:;} struct Cyc_Yyltype{ int timestamp; int first_line; int first_column;
int last_line; int last_column; struct _tagged_string text; } ; typedef struct
Cyc_Yyltype Cyc_yyltype; struct Cyc_Yyltype Cyc_yynewloc(){ return({ struct Cyc_Yyltype
_temp1386; _temp1386.timestamp= 0; _temp1386.first_line= 0; _temp1386.first_column=
0; _temp1386.last_line= 0; _temp1386.last_column= 0; _temp1386.text=( struct
_tagged_string)({ char* _temp1387=( char*)""; struct _tagged_string _temp1388;
_temp1388.curr= _temp1387; _temp1388.base= _temp1387; _temp1388.last_plus_one=
_temp1387 + 1; _temp1388;}); _temp1386;});} static char _temp1391[ 1u]="";
struct Cyc_Yyltype Cyc_yylloc=( struct Cyc_Yyltype){.timestamp= 0,.first_line= 0,.first_column=
0,.last_line= 0,.last_column= 0,.text=( struct _tagged_string){ _temp1391,
_temp1391, _temp1391 + 1u}}; static short Cyc_yytranslate[ 348u]={ (short)0,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)119, (short)2, (short)2, (short)101, (short)117, (short)113, (short)2,
(short)97, (short)98, (short)108, (short)111, (short)99, (short)112, (short)105,
(short)116, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)104, (short)93, (short)102, (short)96,
(short)103, (short)110, (short)109, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)106, (short)2,
(short)107, (short)115, (short)100, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)94,
(short)114, (short)95, (short)118, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)1, (short)2, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, (short)22, (short)23, (short)24, (short)25,
(short)26, (short)27, (short)28, (short)29, (short)30, (short)31, (short)32,
(short)33, (short)34, (short)35, (short)36, (short)37, (short)38, (short)39,
(short)40, (short)41, (short)42, (short)43, (short)44, (short)45, (short)46,
(short)47, (short)48, (short)49, (short)50, (short)51, (short)52, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, (short)60,
(short)61, (short)62, (short)63, (short)64, (short)65, (short)66, (short)67,
(short)68, (short)69, (short)70, (short)71, (short)72, (short)73, (short)74,
(short)75, (short)76, (short)77, (short)78, (short)79, (short)80, (short)81,
(short)82, (short)83, (short)84, (short)85, (short)86, (short)87, (short)88,
(short)89, (short)90, (short)91, (short)92}; static char _temp1394[ 2u]="$";
static char _temp1397[ 6u]="error"; static char _temp1400[ 12u]="$undefined.";
static char _temp1403[ 5u]="AUTO"; static char _temp1406[ 9u]="REGISTER"; static
char _temp1409[ 7u]="STATIC"; static char _temp1412[ 7u]="EXTERN"; static char
_temp1415[ 8u]="TYPEDEF"; static char _temp1418[ 5u]="VOID"; static char
_temp1421[ 5u]="CHAR"; static char _temp1424[ 6u]="SHORT"; static char _temp1427[
4u]="INT"; static char _temp1430[ 5u]="LONG"; static char _temp1433[ 6u]="FLOAT";
static char _temp1436[ 7u]="DOUBLE"; static char _temp1439[ 7u]="SIGNED"; static
char _temp1442[ 9u]="UNSIGNED"; static char _temp1445[ 6u]="CONST"; static char
_temp1448[ 9u]="VOLATILE"; static char _temp1451[ 9u]="RESTRICT"; static char
_temp1454[ 7u]="STRUCT"; static char _temp1457[ 6u]="UNION"; static char
_temp1460[ 5u]="CASE"; static char _temp1463[ 8u]="DEFAULT"; static char
_temp1466[ 7u]="INLINE"; static char _temp1469[ 3u]="IF"; static char _temp1472[
5u]="ELSE"; static char _temp1475[ 7u]="SWITCH"; static char _temp1478[ 6u]="WHILE";
static char _temp1481[ 3u]="DO"; static char _temp1484[ 4u]="FOR"; static char
_temp1487[ 5u]="GOTO"; static char _temp1490[ 9u]="CONTINUE"; static char
_temp1493[ 6u]="BREAK"; static char _temp1496[ 7u]="RETURN"; static char
_temp1499[ 7u]="SIZEOF"; static char _temp1502[ 5u]="ENUM"; static char
_temp1505[ 8u]="NULL_kw"; static char _temp1508[ 4u]="LET"; static char
_temp1511[ 6u]="THROW"; static char _temp1514[ 4u]="TRY"; static char _temp1517[
6u]="CATCH"; static char _temp1520[ 4u]="NEW"; static char _temp1523[ 9u]="ABSTRACT";
static char _temp1526[ 9u]="FALLTHRU"; static char _temp1529[ 6u]="USING";
static char _temp1532[ 10u]="NAMESPACE"; static char _temp1535[ 7u]="TUNION";
static char _temp1538[ 8u]="XTUNION"; static char _temp1541[ 5u]="FILL"; static
char _temp1544[ 8u]="CODEGEN"; static char _temp1547[ 4u]="CUT"; static char
_temp1550[ 7u]="SPLICE"; static char _temp1553[ 7u]="PRINTF"; static char
_temp1556[ 8u]="FPRINTF"; static char _temp1559[ 8u]="XPRINTF"; static char
_temp1562[ 6u]="SCANF"; static char _temp1565[ 7u]="FSCANF"; static char
_temp1568[ 7u]="SSCANF"; static char _temp1571[ 7u]="MALLOC"; static char
_temp1574[ 7u]="PTR_OP"; static char _temp1577[ 7u]="INC_OP"; static char
_temp1580[ 7u]="DEC_OP"; static char _temp1583[ 8u]="LEFT_OP"; static char
_temp1586[ 9u]="RIGHT_OP"; static char _temp1589[ 6u]="LE_OP"; static char
_temp1592[ 6u]="GE_OP"; static char _temp1595[ 6u]="EQ_OP"; static char
_temp1598[ 6u]="NE_OP"; static char _temp1601[ 7u]="AND_OP"; static char
_temp1604[ 6u]="OR_OP"; static char _temp1607[ 11u]="MUL_ASSIGN"; static char
_temp1610[ 11u]="DIV_ASSIGN"; static char _temp1613[ 11u]="MOD_ASSIGN"; static
char _temp1616[ 11u]="ADD_ASSIGN"; static char _temp1619[ 11u]="SUB_ASSIGN";
static char _temp1622[ 12u]="LEFT_ASSIGN"; static char _temp1625[ 13u]="RIGHT_ASSIGN";
static char _temp1628[ 11u]="AND_ASSIGN"; static char _temp1631[ 11u]="XOR_ASSIGN";
static char _temp1634[ 10u]="OR_ASSIGN"; static char _temp1637[ 9u]="ELLIPSIS";
static char _temp1640[ 11u]="LEFT_RIGHT"; static char _temp1643[ 12u]="COLON_COLON";
static char _temp1646[ 11u]="IDENTIFIER"; static char _temp1649[ 17u]="INTEGER_CONSTANT";
static char _temp1652[ 7u]="STRING"; static char _temp1655[ 19u]="CHARACTER_CONSTANT";
static char _temp1658[ 18u]="FLOATING_CONSTANT"; static char _temp1661[ 9u]="TYPE_VAR";
static char _temp1664[ 16u]="QUAL_IDENTIFIER"; static char _temp1667[ 18u]="QUAL_TYPEDEF_NAME";
static char _temp1670[ 10u]="ATTRIBUTE"; static char _temp1673[ 4u]="';'";
static char _temp1676[ 4u]="'{'"; static char _temp1679[ 4u]="'}'"; static char
_temp1682[ 4u]="'='"; static char _temp1685[ 4u]="'('"; static char _temp1688[ 4u]="')'";
static char _temp1691[ 4u]="','"; static char _temp1694[ 4u]="'_'"; static char
_temp1697[ 4u]="'$'"; static char _temp1700[ 4u]="'<'"; static char _temp1703[ 4u]="'>'";
static char _temp1706[ 4u]="':'"; static char _temp1709[ 4u]="'.'"; static char
_temp1712[ 4u]="'['"; static char _temp1715[ 4u]="']'"; static char _temp1718[ 4u]="'*'";
static char _temp1721[ 4u]="'@'"; static char _temp1724[ 4u]="'?'"; static char
_temp1727[ 4u]="'+'"; static char _temp1730[ 4u]="'-'"; static char _temp1733[ 4u]="'&'";
static char _temp1736[ 4u]="'|'"; static char _temp1739[ 4u]="'^'"; static char
_temp1742[ 4u]="'/'"; static char _temp1745[ 4u]="'%'"; static char _temp1748[ 4u]="'~'";
static char _temp1751[ 4u]="'!'"; static char _temp1754[ 5u]="prog"; static char
_temp1757[ 17u]="translation_unit"; static char _temp1760[ 21u]="translation_unit_opt";
static char _temp1763[ 21u]="external_declaration"; static char _temp1766[ 20u]="function_definition";
static char _temp1769[ 21u]="function_definition2"; static char _temp1772[ 13u]="using_action";
static char _temp1775[ 15u]="unusing_action"; static char _temp1778[ 17u]="namespace_action";
static char _temp1781[ 19u]="unnamespace_action"; static char _temp1784[ 12u]="declaration";
static char _temp1787[ 17u]="declaration_list"; static char _temp1790[ 23u]="declaration_specifiers";
static char _temp1793[ 24u]="storage_class_specifier"; static char _temp1796[ 15u]="attributes_opt";
static char _temp1799[ 11u]="attributes"; static char _temp1802[ 15u]="attribute_list";
static char _temp1805[ 10u]="attribute"; static char _temp1808[ 15u]="type_specifier";
static char _temp1811[ 5u]="kind"; static char _temp1814[ 15u]="type_qualifier";
static char _temp1817[ 15u]="enum_specifier"; static char _temp1820[ 11u]="enum_field";
static char _temp1823[ 22u]="enum_declaration_list"; static char _temp1826[ 26u]="struct_or_union_specifier";
static char _temp1829[ 16u]="type_params_opt"; static char _temp1832[ 16u]="struct_or_union";
static char _temp1835[ 24u]="struct_declaration_list"; static char _temp1838[ 25u]="struct_declaration_list0";
static char _temp1841[ 21u]="init_declarator_list"; static char _temp1844[ 22u]="init_declarator_list0";
static char _temp1847[ 16u]="init_declarator"; static char _temp1850[ 19u]="struct_declaration";
static char _temp1853[ 25u]="specifier_qualifier_list"; static char _temp1856[
23u]="struct_declarator_list"; static char _temp1859[ 24u]="struct_declarator_list0";
static char _temp1862[ 18u]="struct_declarator"; static char _temp1865[ 17u]="tunion_specifier";
static char _temp1868[ 17u]="tunionfield_list"; static char _temp1871[ 12u]="tunionfield";
static char _temp1874[ 11u]="declarator"; static char _temp1877[ 18u]="direct_declarator";
static char _temp1880[ 8u]="pointer"; static char _temp1883[ 13u]="pointer_char";
static char _temp1886[ 8u]="rgn_opt"; static char _temp1889[ 4u]="rgn"; static
char _temp1892[ 20u]="type_qualifier_list"; static char _temp1895[ 20u]="parameter_type_list";
static char _temp1898[ 11u]="effect_set"; static char _temp1901[ 14u]="atomic_effect";
static char _temp1904[ 11u]="region_set"; static char _temp1907[ 15u]="parameter_list";
static char _temp1910[ 22u]="parameter_declaration"; static char _temp1913[ 16u]="identifier_list";
static char _temp1916[ 17u]="identifier_list0"; static char _temp1919[ 12u]="initializer";
static char _temp1922[ 18u]="array_initializer"; static char _temp1925[ 17u]="initializer_list";
static char _temp1928[ 12u]="designation"; static char _temp1931[ 16u]="designator_list";
static char _temp1934[ 11u]="designator"; static char _temp1937[ 10u]="type_name";
static char _temp1940[ 14u]="any_type_name"; static char _temp1943[ 15u]="type_name_list";
static char _temp1946[ 20u]="abstract_declarator"; static char _temp1949[ 27u]="direct_abstract_declarator";
static char _temp1952[ 10u]="statement"; static char _temp1955[ 18u]="labeled_statement";
static char _temp1958[ 21u]="expression_statement"; static char _temp1961[ 19u]="compound_statement";
static char _temp1964[ 16u]="block_item_list"; static char _temp1967[ 11u]="block_item";
static char _temp1970[ 20u]="selection_statement"; static char _temp1973[ 15u]="switch_clauses";
static char _temp1976[ 20u]="iteration_statement"; static char _temp1979[ 15u]="jump_statement";
static char _temp1982[ 8u]="pattern"; static char _temp1985[ 19u]="tuple_pattern_list";
static char _temp1988[ 20u]="tuple_pattern_list0"; static char _temp1991[ 14u]="field_pattern";
static char _temp1994[ 19u]="field_pattern_list"; static char _temp1997[ 20u]="field_pattern_list0";
static char _temp2000[ 11u]="expression"; static char _temp2003[ 22u]="assignment_expression";
static char _temp2006[ 20u]="assignment_operator"; static char _temp2009[ 23u]="conditional_expression";
static char _temp2012[ 20u]="constant_expression"; static char _temp2015[ 22u]="logical_or_expression";
static char _temp2018[ 23u]="logical_and_expression"; static char _temp2021[ 24u]="inclusive_or_expression";
static char _temp2024[ 24u]="exclusive_or_expression"; static char _temp2027[ 15u]="and_expression";
static char _temp2030[ 20u]="equality_expression"; static char _temp2033[ 22u]="relational_expression";
static char _temp2036[ 17u]="shift_expression"; static char _temp2039[ 20u]="additive_expression";
static char _temp2042[ 26u]="multiplicative_expression"; static char _temp2045[
16u]="cast_expression"; static char _temp2048[ 17u]="unary_expression"; static
char _temp2051[ 14u]="format_primop"; static char _temp2054[ 15u]="unary_operator";
static char _temp2057[ 19u]="postfix_expression"; static char _temp2060[ 19u]="primary_expression";
static char _temp2063[ 25u]="argument_expression_list"; static char _temp2066[
26u]="argument_expression_list0"; static char _temp2069[ 9u]="constant"; static
char _temp2072[ 20u]="qual_opt_identifier"; static struct _tagged_string Cyc_yytname[
227u]={( struct _tagged_string){ _temp1394, _temp1394, _temp1394 + 2u},( struct
_tagged_string){ _temp1397, _temp1397, _temp1397 + 6u},( struct _tagged_string){
_temp1400, _temp1400, _temp1400 + 12u},( struct _tagged_string){ _temp1403,
_temp1403, _temp1403 + 5u},( struct _tagged_string){ _temp1406, _temp1406,
_temp1406 + 9u},( struct _tagged_string){ _temp1409, _temp1409, _temp1409 + 7u},(
struct _tagged_string){ _temp1412, _temp1412, _temp1412 + 7u},( struct
_tagged_string){ _temp1415, _temp1415, _temp1415 + 8u},( struct _tagged_string){
_temp1418, _temp1418, _temp1418 + 5u},( struct _tagged_string){ _temp1421,
_temp1421, _temp1421 + 5u},( struct _tagged_string){ _temp1424, _temp1424,
_temp1424 + 6u},( struct _tagged_string){ _temp1427, _temp1427, _temp1427 + 4u},(
struct _tagged_string){ _temp1430, _temp1430, _temp1430 + 5u},( struct
_tagged_string){ _temp1433, _temp1433, _temp1433 + 6u},( struct _tagged_string){
_temp1436, _temp1436, _temp1436 + 7u},( struct _tagged_string){ _temp1439,
_temp1439, _temp1439 + 7u},( struct _tagged_string){ _temp1442, _temp1442,
_temp1442 + 9u},( struct _tagged_string){ _temp1445, _temp1445, _temp1445 + 6u},(
struct _tagged_string){ _temp1448, _temp1448, _temp1448 + 9u},( struct
_tagged_string){ _temp1451, _temp1451, _temp1451 + 9u},( struct _tagged_string){
_temp1454, _temp1454, _temp1454 + 7u},( struct _tagged_string){ _temp1457,
_temp1457, _temp1457 + 6u},( struct _tagged_string){ _temp1460, _temp1460,
_temp1460 + 5u},( struct _tagged_string){ _temp1463, _temp1463, _temp1463 + 8u},(
struct _tagged_string){ _temp1466, _temp1466, _temp1466 + 7u},( struct
_tagged_string){ _temp1469, _temp1469, _temp1469 + 3u},( struct _tagged_string){
_temp1472, _temp1472, _temp1472 + 5u},( struct _tagged_string){ _temp1475,
_temp1475, _temp1475 + 7u},( struct _tagged_string){ _temp1478, _temp1478,
_temp1478 + 6u},( struct _tagged_string){ _temp1481, _temp1481, _temp1481 + 3u},(
struct _tagged_string){ _temp1484, _temp1484, _temp1484 + 4u},( struct
_tagged_string){ _temp1487, _temp1487, _temp1487 + 5u},( struct _tagged_string){
_temp1490, _temp1490, _temp1490 + 9u},( struct _tagged_string){ _temp1493,
_temp1493, _temp1493 + 6u},( struct _tagged_string){ _temp1496, _temp1496,
_temp1496 + 7u},( struct _tagged_string){ _temp1499, _temp1499, _temp1499 + 7u},(
struct _tagged_string){ _temp1502, _temp1502, _temp1502 + 5u},( struct
_tagged_string){ _temp1505, _temp1505, _temp1505 + 8u},( struct _tagged_string){
_temp1508, _temp1508, _temp1508 + 4u},( struct _tagged_string){ _temp1511,
_temp1511, _temp1511 + 6u},( struct _tagged_string){ _temp1514, _temp1514,
_temp1514 + 4u},( struct _tagged_string){ _temp1517, _temp1517, _temp1517 + 6u},(
struct _tagged_string){ _temp1520, _temp1520, _temp1520 + 4u},( struct
_tagged_string){ _temp1523, _temp1523, _temp1523 + 9u},( struct _tagged_string){
_temp1526, _temp1526, _temp1526 + 9u},( struct _tagged_string){ _temp1529,
_temp1529, _temp1529 + 6u},( struct _tagged_string){ _temp1532, _temp1532,
_temp1532 + 10u},( struct _tagged_string){ _temp1535, _temp1535, _temp1535 + 7u},(
struct _tagged_string){ _temp1538, _temp1538, _temp1538 + 8u},( struct
_tagged_string){ _temp1541, _temp1541, _temp1541 + 5u},( struct _tagged_string){
_temp1544, _temp1544, _temp1544 + 8u},( struct _tagged_string){ _temp1547,
_temp1547, _temp1547 + 4u},( struct _tagged_string){ _temp1550, _temp1550,
_temp1550 + 7u},( struct _tagged_string){ _temp1553, _temp1553, _temp1553 + 7u},(
struct _tagged_string){ _temp1556, _temp1556, _temp1556 + 8u},( struct
_tagged_string){ _temp1559, _temp1559, _temp1559 + 8u},( struct _tagged_string){
_temp1562, _temp1562, _temp1562 + 6u},( struct _tagged_string){ _temp1565,
_temp1565, _temp1565 + 7u},( struct _tagged_string){ _temp1568, _temp1568,
_temp1568 + 7u},( struct _tagged_string){ _temp1571, _temp1571, _temp1571 + 7u},(
struct _tagged_string){ _temp1574, _temp1574, _temp1574 + 7u},( struct
_tagged_string){ _temp1577, _temp1577, _temp1577 + 7u},( struct _tagged_string){
_temp1580, _temp1580, _temp1580 + 7u},( struct _tagged_string){ _temp1583,
_temp1583, _temp1583 + 8u},( struct _tagged_string){ _temp1586, _temp1586,
_temp1586 + 9u},( struct _tagged_string){ _temp1589, _temp1589, _temp1589 + 6u},(
struct _tagged_string){ _temp1592, _temp1592, _temp1592 + 6u},( struct
_tagged_string){ _temp1595, _temp1595, _temp1595 + 6u},( struct _tagged_string){
_temp1598, _temp1598, _temp1598 + 6u},( struct _tagged_string){ _temp1601,
_temp1601, _temp1601 + 7u},( struct _tagged_string){ _temp1604, _temp1604,
_temp1604 + 6u},( struct _tagged_string){ _temp1607, _temp1607, _temp1607 + 11u},(
struct _tagged_string){ _temp1610, _temp1610, _temp1610 + 11u},( struct
_tagged_string){ _temp1613, _temp1613, _temp1613 + 11u},( struct _tagged_string){
_temp1616, _temp1616, _temp1616 + 11u},( struct _tagged_string){ _temp1619,
_temp1619, _temp1619 + 11u},( struct _tagged_string){ _temp1622, _temp1622,
_temp1622 + 12u},( struct _tagged_string){ _temp1625, _temp1625, _temp1625 + 13u},(
struct _tagged_string){ _temp1628, _temp1628, _temp1628 + 11u},( struct
_tagged_string){ _temp1631, _temp1631, _temp1631 + 11u},( struct _tagged_string){
_temp1634, _temp1634, _temp1634 + 10u},( struct _tagged_string){ _temp1637,
_temp1637, _temp1637 + 9u},( struct _tagged_string){ _temp1640, _temp1640,
_temp1640 + 11u},( struct _tagged_string){ _temp1643, _temp1643, _temp1643 + 12u},(
struct _tagged_string){ _temp1646, _temp1646, _temp1646 + 11u},( struct
_tagged_string){ _temp1649, _temp1649, _temp1649 + 17u},( struct _tagged_string){
_temp1652, _temp1652, _temp1652 + 7u},( struct _tagged_string){ _temp1655,
_temp1655, _temp1655 + 19u},( struct _tagged_string){ _temp1658, _temp1658,
_temp1658 + 18u},( struct _tagged_string){ _temp1661, _temp1661, _temp1661 + 9u},(
struct _tagged_string){ _temp1664, _temp1664, _temp1664 + 16u},( struct
_tagged_string){ _temp1667, _temp1667, _temp1667 + 18u},( struct _tagged_string){
_temp1670, _temp1670, _temp1670 + 10u},( struct _tagged_string){ _temp1673,
_temp1673, _temp1673 + 4u},( struct _tagged_string){ _temp1676, _temp1676,
_temp1676 + 4u},( struct _tagged_string){ _temp1679, _temp1679, _temp1679 + 4u},(
struct _tagged_string){ _temp1682, _temp1682, _temp1682 + 4u},( struct
_tagged_string){ _temp1685, _temp1685, _temp1685 + 4u},( struct _tagged_string){
_temp1688, _temp1688, _temp1688 + 4u},( struct _tagged_string){ _temp1691,
_temp1691, _temp1691 + 4u},( struct _tagged_string){ _temp1694, _temp1694,
_temp1694 + 4u},( struct _tagged_string){ _temp1697, _temp1697, _temp1697 + 4u},(
struct _tagged_string){ _temp1700, _temp1700, _temp1700 + 4u},( struct
_tagged_string){ _temp1703, _temp1703, _temp1703 + 4u},( struct _tagged_string){
_temp1706, _temp1706, _temp1706 + 4u},( struct _tagged_string){ _temp1709,
_temp1709, _temp1709 + 4u},( struct _tagged_string){ _temp1712, _temp1712,
_temp1712 + 4u},( struct _tagged_string){ _temp1715, _temp1715, _temp1715 + 4u},(
struct _tagged_string){ _temp1718, _temp1718, _temp1718 + 4u},( struct
_tagged_string){ _temp1721, _temp1721, _temp1721 + 4u},( struct _tagged_string){
_temp1724, _temp1724, _temp1724 + 4u},( struct _tagged_string){ _temp1727,
_temp1727, _temp1727 + 4u},( struct _tagged_string){ _temp1730, _temp1730,
_temp1730 + 4u},( struct _tagged_string){ _temp1733, _temp1733, _temp1733 + 4u},(
struct _tagged_string){ _temp1736, _temp1736, _temp1736 + 4u},( struct
_tagged_string){ _temp1739, _temp1739, _temp1739 + 4u},( struct _tagged_string){
_temp1742, _temp1742, _temp1742 + 4u},( struct _tagged_string){ _temp1745,
_temp1745, _temp1745 + 4u},( struct _tagged_string){ _temp1748, _temp1748,
_temp1748 + 4u},( struct _tagged_string){ _temp1751, _temp1751, _temp1751 + 4u},(
struct _tagged_string){ _temp1754, _temp1754, _temp1754 + 5u},( struct
_tagged_string){ _temp1757, _temp1757, _temp1757 + 17u},( struct _tagged_string){
_temp1760, _temp1760, _temp1760 + 21u},( struct _tagged_string){ _temp1763,
_temp1763, _temp1763 + 21u},( struct _tagged_string){ _temp1766, _temp1766,
_temp1766 + 20u},( struct _tagged_string){ _temp1769, _temp1769, _temp1769 + 21u},(
struct _tagged_string){ _temp1772, _temp1772, _temp1772 + 13u},( struct
_tagged_string){ _temp1775, _temp1775, _temp1775 + 15u},( struct _tagged_string){
_temp1778, _temp1778, _temp1778 + 17u},( struct _tagged_string){ _temp1781,
_temp1781, _temp1781 + 19u},( struct _tagged_string){ _temp1784, _temp1784,
_temp1784 + 12u},( struct _tagged_string){ _temp1787, _temp1787, _temp1787 + 17u},(
struct _tagged_string){ _temp1790, _temp1790, _temp1790 + 23u},( struct
_tagged_string){ _temp1793, _temp1793, _temp1793 + 24u},( struct _tagged_string){
_temp1796, _temp1796, _temp1796 + 15u},( struct _tagged_string){ _temp1799,
_temp1799, _temp1799 + 11u},( struct _tagged_string){ _temp1802, _temp1802,
_temp1802 + 15u},( struct _tagged_string){ _temp1805, _temp1805, _temp1805 + 10u},(
struct _tagged_string){ _temp1808, _temp1808, _temp1808 + 15u},( struct
_tagged_string){ _temp1811, _temp1811, _temp1811 + 5u},( struct _tagged_string){
_temp1814, _temp1814, _temp1814 + 15u},( struct _tagged_string){ _temp1817,
_temp1817, _temp1817 + 15u},( struct _tagged_string){ _temp1820, _temp1820,
_temp1820 + 11u},( struct _tagged_string){ _temp1823, _temp1823, _temp1823 + 22u},(
struct _tagged_string){ _temp1826, _temp1826, _temp1826 + 26u},( struct
_tagged_string){ _temp1829, _temp1829, _temp1829 + 16u},( struct _tagged_string){
_temp1832, _temp1832, _temp1832 + 16u},( struct _tagged_string){ _temp1835,
_temp1835, _temp1835 + 24u},( struct _tagged_string){ _temp1838, _temp1838,
_temp1838 + 25u},( struct _tagged_string){ _temp1841, _temp1841, _temp1841 + 21u},(
struct _tagged_string){ _temp1844, _temp1844, _temp1844 + 22u},( struct
_tagged_string){ _temp1847, _temp1847, _temp1847 + 16u},( struct _tagged_string){
_temp1850, _temp1850, _temp1850 + 19u},( struct _tagged_string){ _temp1853,
_temp1853, _temp1853 + 25u},( struct _tagged_string){ _temp1856, _temp1856,
_temp1856 + 23u},( struct _tagged_string){ _temp1859, _temp1859, _temp1859 + 24u},(
struct _tagged_string){ _temp1862, _temp1862, _temp1862 + 18u},( struct
_tagged_string){ _temp1865, _temp1865, _temp1865 + 17u},( struct _tagged_string){
_temp1868, _temp1868, _temp1868 + 17u},( struct _tagged_string){ _temp1871,
_temp1871, _temp1871 + 12u},( struct _tagged_string){ _temp1874, _temp1874,
_temp1874 + 11u},( struct _tagged_string){ _temp1877, _temp1877, _temp1877 + 18u},(
struct _tagged_string){ _temp1880, _temp1880, _temp1880 + 8u},( struct
_tagged_string){ _temp1883, _temp1883, _temp1883 + 13u},( struct _tagged_string){
_temp1886, _temp1886, _temp1886 + 8u},( struct _tagged_string){ _temp1889,
_temp1889, _temp1889 + 4u},( struct _tagged_string){ _temp1892, _temp1892,
_temp1892 + 20u},( struct _tagged_string){ _temp1895, _temp1895, _temp1895 + 20u},(
struct _tagged_string){ _temp1898, _temp1898, _temp1898 + 11u},( struct
_tagged_string){ _temp1901, _temp1901, _temp1901 + 14u},( struct _tagged_string){
_temp1904, _temp1904, _temp1904 + 11u},( struct _tagged_string){ _temp1907,
_temp1907, _temp1907 + 15u},( struct _tagged_string){ _temp1910, _temp1910,
_temp1910 + 22u},( struct _tagged_string){ _temp1913, _temp1913, _temp1913 + 16u},(
struct _tagged_string){ _temp1916, _temp1916, _temp1916 + 17u},( struct
_tagged_string){ _temp1919, _temp1919, _temp1919 + 12u},( struct _tagged_string){
_temp1922, _temp1922, _temp1922 + 18u},( struct _tagged_string){ _temp1925,
_temp1925, _temp1925 + 17u},( struct _tagged_string){ _temp1928, _temp1928,
_temp1928 + 12u},( struct _tagged_string){ _temp1931, _temp1931, _temp1931 + 16u},(
struct _tagged_string){ _temp1934, _temp1934, _temp1934 + 11u},( struct
_tagged_string){ _temp1937, _temp1937, _temp1937 + 10u},( struct _tagged_string){
_temp1940, _temp1940, _temp1940 + 14u},( struct _tagged_string){ _temp1943,
_temp1943, _temp1943 + 15u},( struct _tagged_string){ _temp1946, _temp1946,
_temp1946 + 20u},( struct _tagged_string){ _temp1949, _temp1949, _temp1949 + 27u},(
struct _tagged_string){ _temp1952, _temp1952, _temp1952 + 10u},( struct
_tagged_string){ _temp1955, _temp1955, _temp1955 + 18u},( struct _tagged_string){
_temp1958, _temp1958, _temp1958 + 21u},( struct _tagged_string){ _temp1961,
_temp1961, _temp1961 + 19u},( struct _tagged_string){ _temp1964, _temp1964,
_temp1964 + 16u},( struct _tagged_string){ _temp1967, _temp1967, _temp1967 + 11u},(
struct _tagged_string){ _temp1970, _temp1970, _temp1970 + 20u},( struct
_tagged_string){ _temp1973, _temp1973, _temp1973 + 15u},( struct _tagged_string){
_temp1976, _temp1976, _temp1976 + 20u},( struct _tagged_string){ _temp1979,
_temp1979, _temp1979 + 15u},( struct _tagged_string){ _temp1982, _temp1982,
_temp1982 + 8u},( struct _tagged_string){ _temp1985, _temp1985, _temp1985 + 19u},(
struct _tagged_string){ _temp1988, _temp1988, _temp1988 + 20u},( struct
_tagged_string){ _temp1991, _temp1991, _temp1991 + 14u},( struct _tagged_string){
_temp1994, _temp1994, _temp1994 + 19u},( struct _tagged_string){ _temp1997,
_temp1997, _temp1997 + 20u},( struct _tagged_string){ _temp2000, _temp2000,
_temp2000 + 11u},( struct _tagged_string){ _temp2003, _temp2003, _temp2003 + 22u},(
struct _tagged_string){ _temp2006, _temp2006, _temp2006 + 20u},( struct
_tagged_string){ _temp2009, _temp2009, _temp2009 + 23u},( struct _tagged_string){
_temp2012, _temp2012, _temp2012 + 20u},( struct _tagged_string){ _temp2015,
_temp2015, _temp2015 + 22u},( struct _tagged_string){ _temp2018, _temp2018,
_temp2018 + 23u},( struct _tagged_string){ _temp2021, _temp2021, _temp2021 + 24u},(
struct _tagged_string){ _temp2024, _temp2024, _temp2024 + 24u},( struct
_tagged_string){ _temp2027, _temp2027, _temp2027 + 15u},( struct _tagged_string){
_temp2030, _temp2030, _temp2030 + 20u},( struct _tagged_string){ _temp2033,
_temp2033, _temp2033 + 22u},( struct _tagged_string){ _temp2036, _temp2036,
_temp2036 + 17u},( struct _tagged_string){ _temp2039, _temp2039, _temp2039 + 20u},(
struct _tagged_string){ _temp2042, _temp2042, _temp2042 + 26u},( struct
_tagged_string){ _temp2045, _temp2045, _temp2045 + 16u},( struct _tagged_string){
_temp2048, _temp2048, _temp2048 + 17u},( struct _tagged_string){ _temp2051,
_temp2051, _temp2051 + 14u},( struct _tagged_string){ _temp2054, _temp2054,
_temp2054 + 15u},( struct _tagged_string){ _temp2057, _temp2057, _temp2057 + 19u},(
struct _tagged_string){ _temp2060, _temp2060, _temp2060 + 19u},( struct
_tagged_string){ _temp2063, _temp2063, _temp2063 + 25u},( struct _tagged_string){
_temp2066, _temp2066, _temp2066 + 26u},( struct _tagged_string){ _temp2069,
_temp2069, _temp2069 + 9u},( struct _tagged_string){ _temp2072, _temp2072,
_temp2072 + 20u}}; static short Cyc_yyr1[ 389u]={ (short)0, (short)120, (short)121,
(short)121, (short)121, (short)121, (short)121, (short)121, (short)121, (short)122,
(short)122, (short)123, (short)123, (short)124, (short)124, (short)124, (short)124,
(short)125, (short)125, (short)126, (short)127, (short)128, (short)129, (short)130,
(short)130, (short)130, (short)131, (short)131, (short)132, (short)132, (short)132,
(short)132, (short)132, (short)132, (short)132, (short)132, (short)133, (short)133,
(short)133, (short)133, (short)133, (short)133, (short)133, (short)134, (short)134,
(short)135, (short)136, (short)136, (short)137, (short)137, (short)137, (short)137,
(short)138, (short)138, (short)138, (short)138, (short)138, (short)138, (short)138,
(short)138, (short)138, (short)138, (short)138, (short)138, (short)138, (short)138,
(short)138, (short)138, (short)138, (short)139, (short)139, (short)140, (short)140,
(short)140, (short)141, (short)141, (short)142, (short)142, (short)143, (short)143,
(short)144, (short)144, (short)144, (short)144, (short)144, (short)145, (short)145,
(short)145, (short)146, (short)146, (short)147, (short)148, (short)148, (short)149,
(short)150, (short)150, (short)151, (short)151, (short)152, (short)153, (short)153,
(short)153, (short)153, (short)154, (short)155, (short)155, (short)156, (short)156,
(short)156, (short)157, (short)157, (short)157, (short)157, (short)157, (short)157,
(short)157, (short)157, (short)158, (short)158, (short)158, (short)158, (short)159,
(short)159, (short)160, (short)160, (short)161, (short)161, (short)161, (short)161,
(short)161, (short)161, (short)161, (short)161, (short)161, (short)161, (short)161,
(short)162, (short)162, (short)162, (short)162, (short)163, (short)163, (short)163,
(short)163, (short)163, (short)164, (short)164, (short)165, (short)165, (short)165,
(short)166, (short)166, (short)167, (short)167, (short)167, (short)168, (short)168,
(short)169, (short)169, (short)169, (short)169, (short)170, (short)170, (short)170,
(short)170, (short)171, (short)171, (short)172, (short)172, (short)172, (short)173,
(short)174, (short)174, (short)175, (short)175, (short)176, (short)176, (short)176,
(short)176, (short)177, (short)177, (short)177, (short)177, (short)178, (short)179,
(short)179, (short)180, (short)180, (short)181, (short)181, (short)182, (short)182,
(short)182, (short)182, (short)183, (short)183, (short)184, (short)184, (short)184,
(short)185, (short)185, (short)185, (short)185, (short)185, (short)185, (short)185,
(short)185, (short)185, (short)185, (short)185, (short)185, (short)185, (short)185,
(short)186, (short)186, (short)186, (short)186, (short)186, (short)186, (short)186,
(short)186, (short)187, (short)188, (short)188, (short)189, (short)189, (short)190,
(short)190, (short)191, (short)191, (short)191, (short)192, (short)192, (short)192,
(short)192, (short)193, (short)193, (short)193, (short)193, (short)193, (short)193,
(short)194, (short)194, (short)194, (short)194, (short)194, (short)194, (short)194,
(short)194, (short)194, (short)194, (short)194, (short)194, (short)194, (short)194,
(short)195, (short)195, (short)195, (short)195, (short)195, (short)195, (short)195,
(short)195, (short)196, (short)196, (short)196, (short)196, (short)196, (short)196,
(short)196, (short)196, (short)196, (short)196, (short)196, (short)196, (short)196,
(short)196, (short)197, (short)197, (short)198, (short)198, (short)199, (short)199,
(short)200, (short)201, (short)201, (short)202, (short)202, (short)203, (short)203,
(short)204, (short)204, (short)204, (short)204, (short)204, (short)204, (short)204,
(short)204, (short)204, (short)204, (short)204, (short)205, (short)205, (short)205,
(short)205, (short)205, (short)206, (short)207, (short)207, (short)208, (short)208,
(short)209, (short)209, (short)210, (short)210, (short)211, (short)211, (short)212,
(short)212, (short)212, (short)213, (short)213, (short)213, (short)213, (short)213,
(short)214, (short)214, (short)214, (short)215, (short)215, (short)215, (short)216,
(short)216, (short)216, (short)216, (short)217, (short)217, (short)218, (short)218,
(short)218, (short)218, (short)218, (short)218, (short)218, (short)218, (short)218,
(short)218, (short)218, (short)219, (short)219, (short)219, (short)219, (short)219,
(short)219, (short)220, (short)220, (short)220, (short)221, (short)221, (short)221,
(short)221, (short)221, (short)221, (short)221, (short)221, (short)221, (short)221,
(short)221, (short)221, (short)221, (short)221, (short)222, (short)222, (short)222,
(short)222, (short)222, (short)222, (short)222, (short)222, (short)222, (short)223,
(short)224, (short)224, (short)225, (short)225, (short)225, (short)225, (short)226,
(short)226}; static short Cyc_yyr2[ 389u]={ (short)0, (short)1, (short)1,
(short)2, (short)3, (short)5, (short)3, (short)5, (short)6, (short)0, (short)1,
(short)1, (short)1, (short)2, (short)3, (short)3, (short)4, (short)3, (short)4,
(short)2, (short)1, (short)2, (short)1, (short)2, (short)3, (short)5, (short)1,
(short)2, (short)2, (short)3, (short)2, (short)3, (short)2, (short)3, (short)2,
(short)3, (short)1, (short)1, (short)1, (short)1, (short)2, (short)1, (short)1,
(short)0, (short)1, (short)6, (short)1, (short)3, (short)1, (short)1, (short)4,
(short)4, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)3,
(short)2, (short)4, (short)1, (short)1, (short)1, (short)1, (short)1, (short)5,
(short)2, (short)1, (short)3, (short)1, (short)3, (short)4, (short)6, (short)6,
(short)3, (short)3, (short)0, (short)3, (short)3, (short)1, (short)1, (short)1,
(short)1, (short)2, (short)1, (short)1, (short)3, (short)1, (short)3, (short)3,
(short)2, (short)3, (short)2, (short)3, (short)1, (short)1, (short)3, (short)1,
(short)2, (short)3, (short)6, (short)3, (short)4, (short)5, (short)5, (short)2,
(short)3, (short)4, (short)1, (short)2, (short)3, (short)3, (short)1, (short)5,
(short)1, (short)2, (short)1, (short)3, (short)3, (short)4, (short)4, (short)3,
(short)5, (short)4, (short)4, (short)4, (short)2, (short)3, (short)4, (short)4,
(short)5, (short)1, (short)1, (short)4, (short)4, (short)1, (short)0, (short)1,
(short)1, (short)3, (short)1, (short)1, (short)2, (short)1, (short)3, (short)3,
(short)1, (short)3, (short)2, (short)3, (short)1, (short)3, (short)1, (short)3,
(short)3, (short)5, (short)1, (short)3, (short)2, (short)1, (short)2, (short)1,
(short)1, (short)3, (short)1, (short)1, (short)2, (short)3, (short)4, (short)8,
(short)1, (short)2, (short)3, (short)4, (short)2, (short)1, (short)2, (short)3,
(short)2, (short)1, (short)2, (short)1, (short)2, (short)3, (short)3, (short)1,
(short)3, (short)1, (short)1, (short)2, (short)3, (short)2, (short)3, (short)3,
(short)4, (short)2, (short)4, (short)3, (short)3, (short)5, (short)4, (short)4,
(short)4, (short)2, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)2, (short)2, (short)3, (short)1, (short)2, (short)2, (short)3, (short)1,
(short)2, (short)1, (short)1, (short)1, (short)5, (short)7, (short)7, (short)6,
(short)0, (short)3, (short)4, (short)5, (short)6, (short)7, (short)5, (short)7,
(short)6, (short)7, (short)7, (short)8, (short)7, (short)8, (short)8, (short)9,
(short)6, (short)7, (short)7, (short)8, (short)3, (short)2, (short)2, (short)2,
(short)3, (short)2, (short)4, (short)5, (short)1, (short)3, (short)1, (short)2,
(short)1, (short)1, (short)1, (short)1, (short)5, (short)4, (short)4, (short)5,
(short)2, (short)2, (short)0, (short)1, (short)1, (short)3, (short)1, (short)2,
(short)1, (short)1, (short)3, (short)1, (short)3, (short)1, (short)3, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)5, (short)2, (short)2, (short)2, (short)1,
(short)1, (short)3, (short)1, (short)3, (short)1, (short)3, (short)1, (short)3,
(short)1, (short)3, (short)1, (short)3, (short)3, (short)1, (short)3, (short)3,
(short)3, (short)3, (short)1, (short)3, (short)3, (short)1, (short)3, (short)3,
(short)1, (short)3, (short)3, (short)3, (short)1, (short)4, (short)1, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)4, (short)2, (short)4,
(short)7, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)4, (short)3, (short)4, (short)3, (short)3,
(short)3, (short)3, (short)2, (short)2, (short)6, (short)7, (short)4, (short)4,
(short)1, (short)1, (short)1, (short)3, (short)2, (short)5, (short)4, (short)4,
(short)5, (short)1, (short)1, (short)3, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1}; static short Cyc_yydefact[ 729u]={ (short)0, (short)36,
(short)37, (short)38, (short)39, (short)41, (short)52, (short)54, (short)55,
(short)56, (short)57, (short)58, (short)59, (short)60, (short)61, (short)71,
(short)72, (short)73, (short)88, (short)89, (short)43, (short)0, (short)0,
(short)42, (short)0, (short)0, (short)0, (short)0, (short)387, (short)65,
(short)388, (short)85, (short)0, (short)53, (short)0, (short)140, (short)141,
(short)144, (short)1, (short)2, (short)11, (short)0, (short)0, (short)12,
(short)0, (short)43, (short)43, (short)43, (short)62, (short)63, (short)0,
(short)64, (short)0, (short)123, (short)0, (short)145, (short)125, (short)40,
(short)0, (short)34, (short)44, (short)75, (short)269, (short)265, (short)268,
(short)267, (short)0, (short)263, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)270, (short)19, (short)21, (short)147, (short)149, (short)0,
(short)85, (short)0, (short)114, (short)0, (short)0, (short)67, (short)0,
(short)0, (short)0, (short)0, (short)3, (short)0, (short)0, (short)0, (short)0,
(short)23, (short)0, (short)93, (short)94, (short)96, (short)28, (short)30,
(short)32, (short)85, (short)0, (short)85, (short)39, (short)0, (short)26,
(short)0, (short)0, (short)13, (short)0, (short)0, (short)0, (short)135, (short)124,
(short)43, (short)146, (short)0, (short)0, (short)35, (short)0, (short)0,
(short)277, (short)276, (short)266, (short)275, (short)0, (short)0, (short)0,
(short)85, (short)0, (short)110, (short)115, (short)0, (short)0, (short)69,
(short)70, (short)66, (short)0, (short)43, (short)43, (short)188, (short)190,
(short)194, (short)0, (short)126, (short)168, (short)0, (short)165, (short)0,
(short)386, (short)0, (short)0, (short)0, (short)0, (short)348, (short)349,
(short)350, (short)351, (short)352, (short)353, (short)0, (short)0, (short)0,
(short)383, (short)373, (short)384, (short)385, (short)0, (short)0, (short)0,
(short)0, (short)356, (short)0, (short)354, (short)355, (short)0, (short)288,
(short)301, (short)307, (short)309, (short)311, (short)313, (short)315, (short)317,
(short)320, (short)325, (short)328, (short)331, (short)335, (short)0, (short)0,
(short)337, (short)357, (short)372, (short)371, (short)0, (short)4, (short)0,
(short)6, (short)0, (short)24, (short)0, (short)0, (short)0, (short)14, (short)29,
(short)31, (short)33, (short)84, (short)0, (short)90, (short)91, (short)0,
(short)83, (short)40, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)387,
(short)222, (short)224, (short)0, (short)230, (short)228, (short)0, (short)229,
(short)213, (short)214, (short)215, (short)0, (short)226, (short)216, (short)217,
(short)218, (short)0, (short)286, (short)27, (short)15, (short)96, (short)171,
(short)0, (short)130, (short)0, (short)152, (short)0, (short)170, (short)0,
(short)127, (short)0, (short)136, (short)0, (short)0, (short)78, (short)0,
(short)76, (short)264, (short)279, (short)0, (short)278, (short)0, (short)0,
(short)277, (short)148, (short)111, (short)85, (short)0, (short)0, (short)117,
(short)121, (short)116, (short)161, (short)191, (short)0, (short)99, (short)101,
(short)0, (short)0, (short)196, (short)189, (short)197, (short)0, (short)87,
(short)0, (short)86, (short)0, (short)167, (short)196, (short)169, (short)68,
(short)0, (short)0, (short)345, (short)303, (short)335, (short)0, (short)304,
(short)305, (short)0, (short)0, (short)0, (short)0, (short)338, (short)339,
(short)0, (short)0, (short)0, (short)0, (short)341, (short)342, (short)340,
(short)142, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)291, (short)292, (short)293,
(short)294, (short)295, (short)296, (short)297, (short)298, (short)299, (short)300,
(short)290, (short)0, (short)0, (short)343, (short)0, (short)365, (short)366,
(short)0, (short)0, (short)0, (short)375, (short)0, (short)0, (short)143,
(short)20, (short)9, (short)22, (short)9, (short)95, (short)97, (short)174,
(short)173, (short)16, (short)0, (short)80, (short)92, (short)0, (short)0,
(short)103, (short)104, (short)106, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)256, (short)257, (short)258, (short)0,
(short)0, (short)260, (short)0, (short)219, (short)220, (short)0, (short)0,
(short)96, (short)225, (short)227, (short)223, (short)0, (short)159, (short)0,
(short)0, (short)155, (short)129, (short)0, (short)0, (short)132, (short)0,
(short)134, (short)133, (short)128, (short)150, (short)138, (short)137, (short)9,
(short)49, (short)48, (short)0, (short)46, (short)0, (short)74, (short)0,
(short)272, (short)0, (short)25, (short)273, (short)0, (short)0, (short)0,
(short)0, (short)184, (short)281, (short)284, (short)0, (short)283, (short)0,
(short)112, (short)0, (short)113, (short)118, (short)0, (short)0, (short)0,
(short)0, (short)192, (short)100, (short)102, (short)0, (short)204, (short)0,
(short)0, (short)200, (short)0, (short)198, (short)0, (short)0, (short)0,
(short)212, (short)193, (short)195, (short)166, (short)0, (short)0, (short)175,
(short)179, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)374, (short)381, (short)0, (short)380, (short)308,
(short)0, (short)310, (short)312, (short)314, (short)316, (short)318, (short)319,
(short)323, (short)324, (short)321, (short)322, (short)326, (short)327, (short)329,
(short)330, (short)332, (short)333, (short)334, (short)289, (short)0, (short)363,
(short)364, (short)359, (short)0, (short)361, (short)362, (short)0, (short)0,
(short)0, (short)10, (short)5, (short)7, (short)0, (short)306, (short)107,
(short)98, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)255, (short)259, (short)0, (short)0,
(short)0, (short)221, (short)0, (short)17, (short)287, (short)0, (short)157,
(short)0, (short)131, (short)0, (short)154, (short)153, (short)172, (short)151,
(short)139, (short)8, (short)0, (short)0, (short)0, (short)79, (short)77,
(short)280, (short)187, (short)0, (short)282, (short)183, (short)185, (short)274,
(short)0, (short)271, (short)109, (short)120, (short)119, (short)0, (short)163,
(short)162, (short)0, (short)206, (short)199, (short)202, (short)0, (short)207,
(short)0, (short)0, (short)201, (short)0, (short)344, (short)0, (short)176,
(short)0, (short)180, (short)369, (short)370, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)336, (short)377, (short)0, (short)0, (short)346,
(short)360, (short)358, (short)378, (short)0, (short)0, (short)82, (short)105,
(short)108, (short)81, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)235, (short)261, (short)0, (short)18,
(short)160, (short)158, (short)156, (short)0, (short)0, (short)45, (short)47,
(short)186, (short)285, (short)0, (short)0, (short)205, (short)0, (short)209,
(short)211, (short)210, (short)203, (short)0, (short)177, (short)181, (short)0,
(short)0, (short)379, (short)0, (short)382, (short)302, (short)376, (short)231,
(short)235, (short)241, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)262,
(short)50, (short)51, (short)122, (short)164, (short)208, (short)0, (short)182,
(short)0, (short)367, (short)0, (short)0, (short)0, (short)0, (short)243,
(short)0, (short)0, (short)0, (short)251, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)234, (short)0, (short)347,
(short)368, (short)232, (short)233, (short)242, (short)244, (short)245, (short)0,
(short)253, (short)252, (short)0, (short)247, (short)0, (short)0, (short)0,
(short)0, (short)235, (short)236, (short)0, (short)246, (short)254, (short)248,
(short)249, (short)0, (short)0, (short)235, (short)237, (short)178, (short)250,
(short)235, (short)238, (short)235, (short)239, (short)240, (short)0, (short)0,
(short)0}; static short Cyc_yydefgoto[ 107u]={ (short)726, (short)515, (short)516,
(short)39, (short)40, (short)234, (short)41, (short)367, (short)42, (short)369,
(short)43, (short)205, (short)44, (short)45, (short)59, (short)60, (short)424,
(short)425, (short)46, (short)138, (short)47, (short)48, (short)264, (short)265,
(short)49, (short)84, (short)50, (short)211, (short)212, (short)95, (short)96,
(short)97, (short)213, (short)147, (short)379, (short)380, (short)381, (short)51,
(short)278, (short)279, (short)52, (short)53, (short)54, (short)55, (short)116,
(short)78, (short)420, (short)456, (short)408, (short)409, (short)284, (short)255,
(short)149, (short)256, (short)257, (short)471, (short)372, (short)472, (short)473,
(short)436, (short)437, (short)143, (short)144, (short)145, (short)457, (short)291,
(short)237, (short)238, (short)239, (short)240, (short)241, (short)242, (short)243,
(short)662, (short)244, (short)245, (short)268, (short)269, (short)270, (short)439,
(short)440, (short)441, (short)246, (short)247, (short)353, (short)178, (short)520,
(short)179, (short)180, (short)181, (short)182, (short)183, (short)184, (short)185,
(short)186, (short)187, (short)188, (short)189, (short)190, (short)191, (short)192,
(short)193, (short)194, (short)483, (short)484, (short)195, (short)196}; static
short Cyc_yypact[ 729u]={ (short)1452, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)50, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)35, (short)88, (short)491, - (short)-32768,
(short)88, (short)13, (short)95, (short)95, - (short)-32768, (short)37, -
(short)-32768, (short)72, (short)404, - (short)-32768, (short)94, (short)107,
(short)109, - (short)-32768, - (short)-32768, (short)1452, - (short)-32768, -
(short)16, (short)296, - (short)-32768, (short)342, - (short)35, - (short)35, -
(short)35, - (short)-32768, - (short)-32768, (short)130, - (short)-32768,
(short)1764, (short)280, (short)47, (short)61, - (short)-32768, (short)117,
(short)96, (short)1813, - (short)-32768, (short)119, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)491, - (short)-32768, (short)134,
(short)185, (short)213, (short)491, (short)183, (short)194, - (short)-32768, -
(short)-32768, (short)204, - (short)-32768, (short)88, (short)184, (short)88,
(short)82, (short)89, (short)1979, - (short)-32768, (short)226, (short)2038,
(short)3503, (short)3503, - (short)-32768, (short)1452, (short)1452, (short)1452,
(short)1452, - (short)-32768, (short)241, (short)227, - (short)-32768, (short)1714,
(short)1813, (short)1813, (short)1813, (short)72, (short)2038, (short)72,
(short)251, (short)994, - (short)-32768, (short)1764, (short)342, - (short)-32768,
(short)1391, (short)1979, (short)2431, - (short)-32768, (short)280, - (short)35,
- (short)-32768, (short)1452, (short)248, - (short)-32768, (short)88, (short)282,
(short)491, - (short)-32768, - (short)-32768, - (short)-32768, (short)3503,
(short)218, (short)89, (short)72, (short)88, (short)263, - (short)-32768,
(short)88, (short)88, - (short)-32768, - (short)-32768, - (short)-32768, (short)182,
- (short)35, - (short)35, (short)151, - (short)-32768, (short)285, (short)2, -
(short)-32768, (short)441, (short)321, - (short)-32768, (short)3704, - (short)-32768,
(short)3503, (short)3570, (short)277, (short)316, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)318,
(short)3771, (short)3771, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)1223, (short)324, (short)3838, (short)3838, - (short)-32768,
(short)3838, - (short)-32768, - (short)-32768, (short)354, - (short)-32768, -
(short)49, (short)391, (short)349, (short)352, (short)363, (short)357, (short)63,
(short)366, (short)322, (short)122, - (short)-32768, (short)549, (short)390,
(short)3838, (short)242, - (short)-32768, - (short)-32768, (short)58, (short)394,
- (short)-32768, (short)411, - (short)-32768, (short)416, - (short)-32768,
(short)404, (short)2498, (short)1764, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)428, (short)439, (short)2038, - (short)-32768,
(short)371, (short)432, - (short)-32768, (short)451, (short)457, (short)462,
(short)1866, (short)467, (short)452, (short)477, (short)478, (short)2565,
(short)1866, (short)169, (short)1866, (short)1866, (short)473, - (short)-32768,
- (short)-32768, (short)486, - (short)-32768, - (short)-32768, (short)342, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)489,
(short)1111, - (short)-32768, - (short)-32768, - (short)-32768, (short)208, -
(short)-32768, - (short)-32768, - (short)-32768, (short)494, - (short)-32768,
(short)153, - (short)-32768, (short)487, (short)220, (short)495, (short)488,
(short)12, - (short)-32768, (short)493, (short)99, (short)499, (short)22,
(short)497, (short)510, (short)511, - (short)-32768, - (short)-32768, (short)508,
(short)516, (short)224, (short)3874, (short)491, - (short)-32768, - (short)-32768,
(short)72, (short)88, (short)515, (short)245, (short)45, - (short)-32768,
(short)3, - (short)-32768, (short)523, (short)2038, (short)2038, (short)1606,
(short)2632, (short)17, - (short)-32768, (short)281, (short)153, - (short)-32768,
(short)1979, - (short)-32768, (short)1559, - (short)-32768, (short)139, -
(short)-32768, - (short)-32768, (short)2038, (short)1223, - (short)-32768, -
(short)-32768, - (short)-32768, (short)2091, - (short)-32768, (short)541,
(short)3503, (short)1503, (short)595, (short)1223, - (short)-32768, - (short)-32768,
(short)1111, (short)534, (short)358, (short)3503, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)3838, (short)3503, (short)3838, (short)3838,
(short)3838, (short)3838, (short)3838, (short)3838, (short)3838, (short)3838,
(short)3838, (short)3838, (short)3838, (short)3838, (short)3838, (short)3838,
(short)3838, (short)3838, (short)3838, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)3503,
(short)3503, - (short)-32768, (short)121, - (short)-32768, - (short)-32768,
(short)2699, (short)135, (short)3503, - (short)-32768, (short)2346, (short)532,
- (short)-32768, - (short)-32768, (short)1452, - (short)-32768, (short)1452, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)2038, - (short)-32768, - (short)-32768, (short)3503, (short)547, (short)543,
- (short)-32768, (short)539, (short)2038, (short)3503, (short)3503, (short)3503,
(short)616, (short)714, (short)553, - (short)-32768, - (short)-32768, - (short)-32768,
(short)257, (short)606, - (short)-32768, (short)2766, - (short)-32768, - (short)-32768,
(short)1866, (short)1335, (short)1714, - (short)-32768, - (short)-32768, -
(short)-32768, (short)3503, (short)565, (short)246, (short)552, (short)544, -
(short)-32768, (short)153, (short)2024, - (short)-32768, (short)568, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)99,
(short)1452, - (short)-32768, (short)557, (short)559, (short)564, (short)88, -
(short)-32768, (short)3503, - (short)-32768, (short)491, - (short)-32768, -
(short)-32768, (short)583, (short)3503, (short)491, (short)50, - (short)-32768,
- (short)-32768, - (short)-32768, (short)573, (short)571, (short)574, - (short)-32768,
(short)576, - (short)-32768, (short)88, (short)88, (short)577, (short)89,
(short)588, - (short)-32768, - (short)-32768, - (short)-32768, (short)153, -
(short)-32768, (short)580, (short)581, - (short)-32768, (short)575, (short)281,
(short)1921, (short)1979, (short)2833, - (short)-32768, - (short)-32768, (short)285,
- (short)-32768, (short)582, (short)597, - (short)-32768, - (short)-32768,
(short)304, (short)2498, (short)360, (short)587, (short)404, (short)590, (short)591,
(short)593, (short)3637, - (short)-32768, - (short)-32768, (short)592, (short)594,
(short)391, (short)255, (short)349, (short)352, (short)363, (short)357, (short)63,
(short)63, (short)366, (short)366, (short)366, (short)366, (short)322, (short)322,
(short)122, (short)122, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)596, - (short)-32768, - (short)-32768, - (short)-32768,
(short)598, - (short)-32768, - (short)-32768, - (short)9, (short)309, (short)1979,
- (short)-32768, - (short)-32768, - (short)-32768, (short)600, - (short)-32768,
- (short)-32768, - (short)-32768, (short)371, (short)3503, (short)602, (short)367,
(short)372, (short)397, (short)601, (short)2900, (short)2967, (short)259, -
(short)-32768, - (short)-32768, (short)605, (short)599, (short)603, - (short)-32768,
(short)1764, - (short)-32768, - (short)-32768, (short)89, - (short)-32768,
(short)607, - (short)-32768, (short)153, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)417,
(short)608, (short)22, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)584, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)3904, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)2038, (short)604, - (short)-32768, (short)609, - (short)-32768,
- (short)-32768, - (short)-32768, (short)153, - (short)-32768, (short)610,
(short)35, - (short)-32768, (short)629, (short)611, (short)612, - (short)-32768,
(short)2176, - (short)-32768, - (short)-32768, - (short)-32768, (short)1764,
(short)2038, (short)611, (short)614, (short)2346, - (short)-32768, - (short)-32768,
(short)3503, (short)3503, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)2346, (short)307, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)1866, (short)615, (short)1866, (short)3503,
(short)3034, (short)268, (short)3101, (short)276, (short)3168, (short)482, -
(short)-32768, (short)617, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)618, (short)639, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)418, (short)588, - (short)-32768, (short)641,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)3503,
- (short)-32768, - (short)-32768, (short)2498, (short)642, - (short)-32768,
(short)312, - (short)-32768, - (short)-32768, - (short)-32768, (short)674,
(short)482, - (short)-32768, (short)421, (short)1866, (short)431, (short)3235,
(short)1866, (short)434, (short)3302, (short)3369, (short)295, (short)491,
(short)637, (short)620, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)272, - (short)-32768,
(short)645, - (short)-32768, (short)2261, (short)1866, (short)649, (short)652, -
(short)-32768, (short)1866, (short)1866, (short)447, - (short)-32768, (short)1866,
(short)1866, (short)454, (short)1866, (short)463, (short)3436, (short)5, (short)1111,
- (short)-32768, (short)3503, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)1866,
- (short)-32768, - (short)-32768, (short)1866, - (short)-32768, (short)1866,
(short)1866, (short)468, (short)3503, (short)864, - (short)-32768, (short)317, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)1866,
(short)298, (short)482, - (short)-32768, - (short)-32768, - (short)-32768,
(short)864, - (short)-32768, (short)482, - (short)-32768, - (short)-32768,
(short)713, (short)746, - (short)-32768}; static short Cyc_yypgoto[ 107u]={ -
(short)-32768, (short)202, - (short)325, - (short)-32768, (short)437, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)48,
- (short)36, - (short)52, - (short)-32768, (short)42, - (short)42, (short)200, -
(short)-32768, (short)197, - (short)124, - (short)69, - (short)-32768, - (short)-32768,
(short)329, - (short)-32768, (short)66, - (short)-32768, - (short)234, - (short)-32768,
- (short)-32768, - (short)-32768, (short)545, (short)546, (short)152, - (short)-32768,
- (short)-32768, (short)237, - (short)-32768, - (short)248, - (short)-32768, -
(short)14, - (short)28, - (short)110, - (short)-32768, - (short)-32768, (short)53,
- (short)-32768, - (short)98, - (short)370, (short)485, - (short)387, - (short)84,
- (short)282, - (short)-32768, - (short)-32768, - (short)201, (short)621, -
(short)339, - (short)204, - (short)-32768, (short)343, - (short)131, (short)466,
- (short)100, (short)258, - (short)101, - (short)66, - (short)-32768, - (short)-32768,
- (short)46, - (short)211, - (short)-32768, - (short)-32768, - (short)596, -
(short)-32768, - (short)-32768, - (short)7, (short)492, - (short)-32768, (short)214,
- (short)-32768, - (short)-32768, - (short)56, - (short)78, - (short)-32768, -
(short)144, - (short)359, (short)627, (short)458, (short)459, (short)456,
(short)461, (short)455, (short)210, (short)110, (short)233, (short)236, -
(short)89, (short)512, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)289, - (short)-32768, - (short)-32768, (short)1}; static
short Cyc_yytable[ 4018u]={ (short)109, (short)56, (short)148, (short)371,
(short)107, (short)274, (short)110, (short)120, (short)304, (short)177, (short)197,
(short)114, (short)258, (short)254, (short)141, (short)72, (short)108, (short)141,
(short)85, (short)467, (short)543, (short)323, (short)61, (short)73, (short)513,
(short)74, (short)115, (short)79, (short)81, (short)444, (short)98, (short)403,
(short)289, (short)56, (short)141, (short)260, (short)57, (short)298, (short)316,
(short)422, (short)56, (short)546, (short)141, (short)141, (short)517, (short)56,
(short)109, (short)207, (short)208, (short)209, (short)107, (short)104, (short)206,
(short)675, (short)236, (short)56, (short)109, (short)58, (short)235, (short)122,
(short)248, (short)324, (short)249, (short)571, (short)126, (short)505, (short)293,
(short)73, (short)435, (short)556, (short)509, (short)271, (short)73, (short)114,
(short)707, (short)559, (short)415, (short)90, (short)91, (short)130, (short)80,
(short)133, (short)319, (short)320, (short)572, (short)321, (short)449, (short)99,
(short)100, (short)101, (short)405, (short)56, (short)56, (short)56, (short)56,
(short)250, (short)551, (short)75, (short)600, (short)635, (short)141, (short)294,
(short)450, (short)355, (short)479, (short)295, (short)423, (short)536, (short)117,
(short)708, (short)56, (short)294, (short)718, (short)317, (short)287, (short)416,
(short)15, (short)16, (short)17, (short)56, (short)82, (short)722, (short)266,
(short)288, (short)73, (short)724, (short)373, (short)725, (short)331, (short)332,
(short)467, (short)28, (short)276, (short)297, (short)294, (short)280, (short)281,
(short)30, (short)636, (short)128, (short)362, (short)518, - (short)85, (short)141,
(short)32, (short)132, (short)561, (short)83, (short)56, (short)524, (short)76,
(short)419, (short)363, (short)109, (short)387, (short)433, (short)434, (short)248,
(short)261, (short)374, (short)394, (short)77, (short)397, (short)398, (short)606,
(short)333, (short)334, (short)364, (short)210, (short)393, (short)215, (short)468,
(short)28, (short)136, (short)83, (short)623, (short)134, (short)289, (short)30,
(short)28, (short)137, (short)478, (short)285, (short)286, (short)76, (short)30,
(short)298, (short)135, (short)460, (short)250, (short)236, (short)86, (short)418,
(short)119, (short)235, (short)77, (short)275, (short)460, (short)567, (short)568,
(short)382, (short)87, (short)38, (short)88, (short)56, (short)506, (short)633,
(short)35, (short)36, (short)37, (short)459, (short)118, (short)507, (short)121,
(short)28, (short)56, (short)141, (short)141, (short)141, (short)510, (short)30,
(short)102, (short)401, (short)28, (short)103, (short)141, (short)511, (short)141,
(short)373, (short)30, (short)339, (short)123, (short)141, (short)141, (short)519,
(short)142, (short)296, (short)56, (short)340, (short)341, (short)482, (short)89,
(short)406, (short)141, (short)667, (short)288, (short)317, (short)407, (short)287,
(short)464, (short)501, (short)502, (short)503, (short)474, (short)644, (short)214,
(short)317, (short)288, (short)476, (short)35, (short)36, (short)37, (short)395,
(short)236, (short)142, (short)438, (short)396, (short)235, (short)486, (short)124,
(short)115, (short)282, (short)586, (short)73, (short)73, (short)504, (short)482,
(short)283, (short)280, (short)127, (short)140, (short)482, (short)85, (short)140,
(short)519, (short)373, (short)83, (short)129, - (short)85, (short)131, (short)519,
- (short)85, (short)198, (short)199, (short)200, (short)201, (short)83, (short)56,
(short)125, (short)56, (short)140, (short)404, (short)356, (short)357, (short)358,
(short)512, (short)141, (short)405, (short)140, (short)140, (short)550, (short)56,
(short)272, (short)411, (short)141, (short)273, (short)148, (short)431, (short)482,
(short)412, (short)262, (short)142, (short)482, (short)405, (short)146, (short)570,
(short)203, (short)540, (short)525, (short)526, (short)527, (short)141, (short)531,
(short)537, (short)202, (short)282, (short)109, (short)216, (short)446, (short)359,
(short)530, (short)542, (short)443, (short)141, (short)447, (short)263, (short)448,
(short)360, (short)361, (short)109, (short)533, (short)549, (short)616, (short)107,
(short)405, (short)539, (short)405, (short)277, (short)405, (short)597, (short)435,
(short)654, (short)579, (short)578, (short)214, (short)538, (short)140, (short)405,
(short)56, (short)657, (short)56, (short)405, (short)58, (short)58, (short)309,
(short)405, (short)691, (short)111, (short)461, (short)519, (short)267, (short)641,
(short)112, (short)462, (short)640, (short)581, (short)113, (short)463, (short)687,
(short)92, (short)93, (short)594, (short)141, (short)141, (short)405, (short)373,
(short)292, (short)405, (short)641, (short)584, (short)290, (short)640, (short)721,
(short)585, (short)601, (short)299, (short)294, (short)672, (short)602, (short)140,
(short)647, (short)673, (short)719, (short)310, (short)603, (short)311, (short)405,
(short)621, (short)464, (short)300, (short)301, (short)318, (short)56, (short)557,
(short)329, (short)330, (short)28, (short)266, (short)560, (short)335, (short)336,
(short)73, (short)30, (short)337, (short)338, (short)94, (short)73, (short)452,
(short)453, (short)32, (short)670, (short)493, (short)494, (short)495, (short)496,
(short)141, (short)142, (short)280, (short)280, (short)322, (short)35, (short)36,
(short)37, (short)646, (short)142, (short)28, (short)481, (short)405, (short)587,
(short)405, (short)325, (short)30, (short)589, (short)326, (short)142, (short)608,
(short)405, (short)327, (short)32, (short)641, (short)609, (short)405, (short)640,
(short)613, (short)615, (short)378, (short)328, (short)56, (short)709, (short)35,
(short)36, (short)37, (short)140, (short)140, (short)140, (short)630, (short)109,
(short)354, (short)28, (short)365, (short)248, (short)140, (short)620, (short)140,
(short)30, (short)610, (short)405, (short)717, (short)140, (short)140, (short)141,
(short)32, (short)624, (short)625, (short)660, (short)661, (short)366, (short)373,
(short)382, (short)140, (short)723, (short)368, (short)35, (short)36, (short)37,
(short)373, (short)666, (short)301, (short)645, (short)676, (short)405, (short)141,
(short)375, (short)56, (short)373, (short)28, (short)383, (short)214, (short)62,
(short)678, (short)405, (short)30, (short)682, (short)405, (short)376, (short)214,
(short)389, (short)109, (short)296, (short)491, (short)492, (short)107, (short)648,
(short)206, (short)650, (short)699, (short)405, (short)288, (short)384, (short)35,
(short)36, (short)37, (short)702, (short)405, (short)385, (short)651, (short)653,
(short)438, (short)656, (short)386, (short)659, (short)704, (short)405, (short)373,
(short)388, (short)73, (short)715, (short)405, (short)497, (short)498, (short)390,
(short)391, (short)140, (short)499, (short)500, (short)28, (short)63, (short)399,
(short)64, (short)65, (short)140, (short)30, (short)669, (short)400, (short)402,
(short)410, (short)677, (short)414, (short)66, (short)681, (short)204, (short)67,
(short)68, (short)413, (short)421, (short)373, (short)426, (short)140, (short)680,
(short)69, (short)417, (short)684, (short)686, (short)70, (short)71, (short)427,
(short)429, (short)428, (short)694, (short)140, (short)445, (short)323, (short)697,
(short)698, (short)142, (short)430, (short)700, (short)701, (short)451, (short)703,
(short)342, (short)343, (short)344, (short)345, (short)346, (short)347, (short)348,
(short)349, (short)350, (short)351, (short)477, (short)706, (short)480, (short)711,
(short)514, (short)710, (short)712, (short)236, (short)713, (short)714, (short)521,
(short)235, (short)522, (short)523, (short)528, (short)352, (short)532, (short)534,
(short)541, (short)720, (short)544, (short)716, (short)548, (short)688, (short)552,
(short)545, (short)236, (short)553, (short)140, (short)140, (short)235, (short)73,
(short)303, (short)554, (short)305, (short)305, (short)142, (short)558, (short)563,
(short)236, (short)564, (short)566, (short)565, (short)235, (short)569, (short)313,
(short)314, (short)282, (short)573, (short)574, (short)582, (short)583, (short)575,
(short)305, (short)305, (short)588, (short)305, (short)590, (short)592, (short)591,
(short)595, (short)628, (short)618, (short)596, (short)598, (short)604, (short)599,
(short)607, (short)611, (short)617, (short)674, (short)619, (short)622, (short)631,
(short)305, (short)593, (short)626, (short)632, (short)634, (short)649, (short)663,
(short)140, (short)643, (short)727, (short)638, (short)690, (short)664, (short)1,
(short)2, (short)3, (short)105, (short)5, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)637, (short)665, (short)20, (short)668,
(short)671, (short)689, (short)642, (short)692, (short)695, (short)696, (short)728,
(short)475, (short)370, (short)150, (short)21, (short)151, (short)22, (short)152,
(short)627, (short)555, (short)153, (short)23, (short)377, (short)605, (short)466,
(short)26, (short)27, (short)154, (short)155, (short)442, (short)140, (short)156,
(short)157, (short)158, (short)159, (short)160, (short)161, (short)162, (short)307,
(short)163, (short)164, (short)465, (short)629, (short)562, (short)308, (short)485,
(short)488, (short)490, (short)487, (short)0, (short)0, (short)140, (short)489,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)165, (short)166, (short)167, (short)168, (short)29,
(short)30, (short)31, (short)0, (short)529, (short)0, (short)0, (short)0,
(short)169, (short)0, (short)0, (short)33, (short)233, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)171, (short)0, (short)0, (short)172,
(short)173, (short)174, (short)0, (short)0, (short)0, (short)0, (short)175,
(short)176, (short)0, (short)305, (short)0, (short)305, (short)305, (short)305,
(short)305, (short)305, (short)305, (short)305, (short)305, (short)305, (short)305,
(short)305, (short)305, (short)305, (short)305, (short)305, (short)305, (short)305,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)1, (short)2, (short)3,
(short)105, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)660, (short)661, (short)20, (short)217, (short)305, (short)218,
(short)219, (short)220, (short)221, (short)222, (short)223, (short)224, (short)225,
(short)150, (short)21, (short)151, (short)22, (short)152, (short)226, (short)0,
(short)153, (short)23, (short)227, (short)0, (short)0, (short)26, (short)27,
(short)154, (short)155, (short)228, (short)229, (short)156, (short)157, (short)158,
(short)159, (short)160, (short)161, (short)162, (short)0, (short)163, (short)164,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)305, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)305, (short)0, (short)230, (short)165,
(short)166, (short)167, (short)168, (short)29, (short)30, (short)31, (short)0,
(short)231, (short)106, (short)0, (short)0, (short)169, (short)0, (short)0,
(short)33, (short)233, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)171, (short)0, (short)0, (short)172, (short)173, (short)174, (short)0,
(short)0, (short)0, (short)0, (short)175, (short)176, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)305, (short)0,
(short)0, (short)0, (short)0, (short)1, (short)2, (short)3, (short)105, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)20, (short)217, (short)0, (short)218, (short)219, (short)220,
(short)221, (short)222, (short)223, (short)224, (short)225, (short)150, (short)21,
(short)151, (short)22, (short)152, (short)226, (short)305, (short)153, (short)23,
(short)227, (short)0, (short)0, (short)26, (short)27, (short)154, (short)155,
(short)228, (short)229, (short)156, (short)157, (short)158, (short)159, (short)160,
(short)161, (short)162, (short)0, (short)163, (short)164, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)230, (short)165, (short)166, (short)167,
(short)168, (short)29, (short)30, (short)31, (short)0, (short)231, (short)106,
(short)232, (short)0, (short)169, (short)0, (short)0, (short)33, (short)233,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)171, (short)0,
(short)0, (short)172, (short)173, (short)174, (short)0, (short)305, (short)0,
(short)0, (short)175, (short)176, (short)1, (short)2, (short)3, (short)105,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)0, (short)0, (short)20, (short)217, (short)0, (short)218, (short)219,
(short)220, (short)221, (short)222, (short)223, (short)224, (short)225, (short)150,
(short)21, (short)151, (short)22, (short)152, (short)226, (short)0, (short)153,
(short)23, (short)227, (short)0, (short)0, (short)26, (short)27, (short)154,
(short)155, (short)228, (short)229, (short)156, (short)157, (short)158, (short)159,
(short)160, (short)161, (short)162, (short)0, (short)163, (short)164, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)230, (short)165, (short)166,
(short)167, (short)168, (short)29, (short)30, (short)31, (short)0, (short)231,
(short)106, (short)0, (short)0, (short)169, (short)0, (short)0, (short)33,
(short)233, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)171,
(short)0, (short)0, (short)172, (short)173, (short)174, (short)0, (short)0,
(short)0, (short)0, (short)175, (short)176, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)150, (short)21, (short)151, (short)0, (short)152,
(short)0, (short)0, (short)153, (short)0, (short)0, (short)0, (short)0, (short)26,
(short)27, (short)154, (short)155, (short)0, (short)0, (short)156, (short)157,
(short)158, (short)159, (short)160, (short)161, (short)162, (short)0, (short)163,
(short)164, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)165,
(short)166, (short)167, (short)168, (short)29, (short)30, (short)31, (short)0,
(short)0, (short)315, (short)0, (short)0, (short)169, (short)0, (short)0,
(short)33, (short)233, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)171, (short)0, (short)0, (short)172, (short)173, (short)174, (short)0,
(short)0, (short)0, (short)0, (short)175, (short)176, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)150, (short)21, (short)151, (short)0,
(short)152, (short)0, (short)0, (short)153, (short)0, (short)0, (short)0,
(short)0, (short)26, (short)27, (short)154, (short)155, (short)0, (short)0,
(short)156, (short)157, (short)158, (short)159, (short)160, (short)161, (short)162,
(short)0, (short)163, (short)164, (short)0, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)28, (short)165, (short)166, (short)167,
(short)168, (short)29, (short)30, (short)31, (short)21, (short)0, (short)0,
(short)0, (short)0, (short)169, (short)0, (short)0, (short)33, (short)233,
(short)0, (short)26, (short)27, (short)0, (short)0, (short)0, (short)171,
(short)0, (short)0, (short)172, (short)173, (short)174, (short)0, (short)0,
(short)0, (short)0, (short)175, (short)176, (short)1, (short)2, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)0, (short)251, (short)20, (short)0, (short)0, (short)0,
(short)29, (short)0, (short)31, (short)0, (short)252, (short)0, (short)0,
(short)0, (short)21, (short)253, (short)22, (short)33, (short)34, (short)0,
(short)0, (short)23, (short)0, (short)24, (short)25, (short)26, (short)27,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)1, (short)2, (short)3,
(short)105, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)0, (short)0, (short)20, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)28, (short)0, (short)0, (short)21,
(short)0, (short)29, (short)30, (short)31, (short)0, (short)0, (short)23,
(short)0, (short)0, (short)32, (short)26, (short)27, (short)33, (short)34,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)35, (short)36,
(short)37, (short)0, (short)0, (short)0, (short)0, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)28, (short)0, (short)0, (short)0, (short)0,
(short)29, (short)30, (short)31, (short)21, (short)0, (short)0, (short)0,
(short)0, (short)32, (short)0, (short)0, (short)33, (short)34, (short)0, (short)26,
(short)27, (short)0, (short)0, (short)0, (short)35, (short)36, (short)37,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)21, (short)28, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)30, (short)31, (short)0, (short)454,
(short)26, (short)27, (short)0, (short)296, (short)455, (short)0, (short)33,
(short)34, (short)0, (short)0, (short)0, (short)0, (short)288, (short)0, (short)35,
(short)36, (short)37, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)0, (short)31, (short)0, (short)454,
(short)0, (short)0, (short)0, (short)287, (short)455, (short)0, (short)33,
(short)34, (short)0, (short)0, (short)0, (short)0, (short)288, (short)0, (short)35,
(short)36, (short)37, (short)1, (short)2, (short)3, (short)105, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)20, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)21, (short)0, (short)22,
(short)0, (short)0, (short)0, (short)0, (short)23, (short)0, (short)0, (short)0,
(short)26, (short)27, (short)0, (short)0, (short)0, (short)0, (short)1, (short)2,
(short)3, (short)105, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)0, (short)0, (short)20, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)21, (short)0, (short)22, (short)29, (short)0, (short)31, (short)0,
(short)23, (short)106, (short)0, (short)204, (short)26, (short)27, (short)0,
(short)33, (short)34, (short)1, (short)2, (short)3, (short)105, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)20, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)21, (short)0, (short)0,
(short)0, (short)29, (short)0, (short)31, (short)23, (short)0, (short)106,
(short)0, (short)26, (short)27, (short)0, (short)0, (short)33, (short)34,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)217, (short)0, (short)218, (short)219, (short)220, (short)221,
(short)222, (short)223, (short)224, (short)225, (short)150, (short)29, (short)151,
(short)31, (short)152, (short)226, (short)0, (short)153, (short)0, (short)227,
(short)0, (short)0, (short)33, (short)34, (short)154, (short)155, (short)228,
(short)229, (short)156, (short)157, (short)158, (short)159, (short)160, (short)161,
(short)162, (short)0, (short)163, (short)164, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)230, (short)165, (short)166,
(short)167, (short)168, (short)0, (short)30, (short)21, (short)0, (short)231,
(short)106, (short)0, (short)0, (short)169, (short)0, (short)0, (short)0,
(short)170, (short)26, (short)27, (short)0, (short)0, (short)0, (short)0,
(short)171, (short)0, (short)0, (short)172, (short)173, (short)174, (short)0,
(short)0, (short)0, (short)0, (short)175, (short)176, (short)0, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)29, (short)0, (short)31, (short)0, (short)576, (short)21, (short)0,
(short)0, (short)0, (short)577, (short)0, (short)33, (short)34, (short)0,
(short)0, (short)0, (short)26, (short)27, (short)0, (short)0, (short)0, (short)0,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)21,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29,
(short)0, (short)31, (short)26, (short)27, (short)139, (short)21, (short)0,
(short)0, (short)0, (short)0, (short)33, (short)34, (short)0, (short)0, (short)0,
(short)0, (short)26, (short)27, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)547, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)0, (short)31,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)469, (short)0, (short)0,
(short)33, (short)34, (short)150, (short)29, (short)151, (short)31, (short)152,
(short)0, (short)0, (short)153, (short)0, (short)0, (short)0, (short)0, (short)33,
(short)34, (short)154, (short)155, (short)0, (short)0, (short)156, (short)157,
(short)158, (short)159, (short)160, (short)161, (short)162, (short)0, (short)163,
(short)164, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)165,
(short)166, (short)167, (short)168, (short)0, (short)30, (short)0, (short)0,
(short)0, (short)306, (short)470, (short)0, (short)169, (short)0, (short)0,
(short)0, (short)170, (short)0, (short)0, (short)0, (short)433, (short)434,
(short)0, (short)171, (short)0, (short)0, (short)172, (short)173, (short)174,
(short)0, (short)0, (short)0, (short)0, (short)175, (short)176, (short)150,
(short)0, (short)151, (short)0, (short)152, (short)0, (short)0, (short)153,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)154, (short)155,
(short)0, (short)0, (short)156, (short)157, (short)158, (short)159, (short)160,
(short)161, (short)162, (short)0, (short)163, (short)164, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)28, (short)165, (short)166, (short)167,
(short)168, (short)0, (short)30, (short)0, (short)0, (short)0, (short)306,
(short)639, (short)0, (short)169, (short)0, (short)0, (short)0, (short)170,
(short)0, (short)0, (short)0, (short)433, (short)434, (short)0, (short)171,
(short)0, (short)0, (short)172, (short)173, (short)174, (short)0, (short)0,
(short)0, (short)0, (short)175, (short)176, (short)150, (short)0, (short)151,
(short)0, (short)152, (short)0, (short)0, (short)153, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)154, (short)155, (short)0,
(short)0, (short)156, (short)157, (short)158, (short)159, (short)160, (short)161,
(short)162, (short)0, (short)163, (short)164, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)28, (short)165, (short)166, (short)167, (short)168,
(short)0, (short)30, (short)0, (short)0, (short)0, (short)306, (short)693,
(short)0, (short)169, (short)0, (short)0, (short)0, (short)170, (short)0,
(short)0, (short)0, (short)433, (short)434, (short)0, (short)171, (short)0,
(short)0, (short)172, (short)173, (short)174, (short)0, (short)0, (short)0,
(short)0, (short)175, (short)176, (short)150, (short)0, (short)151, (short)0,
(short)152, (short)0, (short)0, (short)153, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)154, (short)155, (short)0, (short)0,
(short)156, (short)157, (short)158, (short)159, (short)160, (short)161, (short)162,
(short)0, (short)163, (short)164, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)165, (short)166, (short)167, (short)168, (short)0,
(short)30, (short)0, (short)0, (short)0, (short)306, (short)0, (short)0, (short)169,
(short)0, (short)0, (short)0, (short)170, (short)0, (short)0, (short)0, (short)433,
(short)434, (short)0, (short)171, (short)0, (short)0, (short)172, (short)173,
(short)174, (short)0, (short)0, (short)0, (short)0, (short)175, (short)176,
(short)150, (short)0, (short)151, (short)0, (short)152, (short)0, (short)0,
(short)153, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)154,
(short)155, (short)0, (short)0, (short)156, (short)157, (short)158, (short)159,
(short)160, (short)161, (short)162, (short)0, (short)163, (short)164, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)28, (short)165, (short)166,
(short)167, (short)168, (short)0, (short)30, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)169, (short)0, (short)0, (short)0, (short)170,
(short)150, (short)0, (short)151, (short)0, (short)152, (short)259, (short)171,
(short)153, (short)0, (short)172, (short)173, (short)174, (short)0, (short)0,
(short)154, (short)155, (short)175, (short)176, (short)156, (short)157, (short)158,
(short)159, (short)160, (short)161, (short)162, (short)0, (short)163, (short)164,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)165, (short)166,
(short)167, (short)168, (short)0, (short)30, (short)0, (short)0, (short)0,
(short)306, (short)0, (short)0, (short)169, (short)0, (short)0, (short)0,
(short)170, (short)150, (short)0, (short)151, (short)0, (short)152, (short)0,
(short)171, (short)153, (short)0, (short)172, (short)173, (short)174, (short)0,
(short)0, (short)154, (short)155, (short)175, (short)176, (short)156, (short)157,
(short)158, (short)159, (short)160, (short)161, (short)162, (short)0, (short)163,
(short)164, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)165,
(short)166, (short)167, (short)168, (short)0, (short)30, (short)0, (short)0,
(short)392, (short)0, (short)0, (short)0, (short)169, (short)0, (short)0,
(short)0, (short)170, (short)150, (short)0, (short)151, (short)0, (short)152,
(short)0, (short)171, (short)153, (short)0, (short)172, (short)173, (short)174,
(short)0, (short)0, (short)154, (short)155, (short)175, (short)176, (short)156,
(short)157, (short)158, (short)159, (short)160, (short)161, (short)162, (short)0,
(short)163, (short)164, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)28, (short)165, (short)166, (short)167, (short)168, (short)0, (short)30,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)169, (short)0,
(short)0, (short)0, (short)170, (short)150, (short)0, (short)151, (short)0,
(short)152, (short)458, (short)171, (short)153, (short)0, (short)172, (short)173,
(short)174, (short)0, (short)0, (short)154, (short)155, (short)175, (short)176,
(short)156, (short)157, (short)158, (short)159, (short)160, (short)161, (short)162,
(short)0, (short)163, (short)164, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)165, (short)166, (short)167, (short)168, (short)0,
(short)30, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)169,
(short)508, (short)0, (short)0, (short)170, (short)150, (short)0, (short)151,
(short)0, (short)152, (short)0, (short)171, (short)153, (short)0, (short)172,
(short)173, (short)174, (short)0, (short)0, (short)154, (short)155, (short)175,
(short)176, (short)156, (short)157, (short)158, (short)159, (short)160, (short)161,
(short)162, (short)0, (short)163, (short)164, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)28, (short)165, (short)166, (short)167, (short)168,
(short)0, (short)30, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)169, (short)535, (short)0, (short)0, (short)170, (short)150, (short)0,
(short)151, (short)0, (short)152, (short)0, (short)171, (short)153, (short)0,
(short)172, (short)173, (short)174, (short)0, (short)0, (short)154, (short)155,
(short)175, (short)176, (short)156, (short)157, (short)158, (short)159, (short)160,
(short)161, (short)162, (short)0, (short)163, (short)164, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)28, (short)165, (short)166, (short)167,
(short)168, (short)0, (short)30, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)169, (short)0, (short)0, (short)0, (short)170, (short)150,
(short)0, (short)151, (short)0, (short)152, (short)580, (short)171, (short)153,
(short)0, (short)172, (short)173, (short)174, (short)0, (short)0, (short)154,
(short)155, (short)175, (short)176, (short)156, (short)157, (short)158, (short)159,
(short)160, (short)161, (short)162, (short)0, (short)163, (short)164, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)28, (short)165, (short)166,
(short)167, (short)168, (short)0, (short)30, (short)0, (short)0, (short)612,
(short)0, (short)0, (short)0, (short)169, (short)0, (short)0, (short)0, (short)170,
(short)150, (short)0, (short)151, (short)0, (short)152, (short)0, (short)171,
(short)153, (short)0, (short)172, (short)173, (short)174, (short)0, (short)0,
(short)154, (short)155, (short)175, (short)176, (short)156, (short)157, (short)158,
(short)159, (short)160, (short)161, (short)162, (short)0, (short)163, (short)164,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)165, (short)166,
(short)167, (short)168, (short)0, (short)30, (short)0, (short)0, (short)614,
(short)0, (short)0, (short)0, (short)169, (short)0, (short)0, (short)0, (short)170,
(short)150, (short)0, (short)151, (short)0, (short)152, (short)0, (short)171,
(short)153, (short)0, (short)172, (short)173, (short)174, (short)0, (short)0,
(short)154, (short)155, (short)175, (short)176, (short)156, (short)157, (short)158,
(short)159, (short)160, (short)161, (short)162, (short)0, (short)163, (short)164,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)165, (short)166,
(short)167, (short)168, (short)0, (short)30, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)169, (short)652, (short)0, (short)0,
(short)170, (short)150, (short)0, (short)151, (short)0, (short)152, (short)0,
(short)171, (short)153, (short)0, (short)172, (short)173, (short)174, (short)0,
(short)0, (short)154, (short)155, (short)175, (short)176, (short)156, (short)157,
(short)158, (short)159, (short)160, (short)161, (short)162, (short)0, (short)163,
(short)164, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)165,
(short)166, (short)167, (short)168, (short)0, (short)30, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)169, (short)655, (short)0,
(short)0, (short)170, (short)150, (short)0, (short)151, (short)0, (short)152,
(short)0, (short)171, (short)153, (short)0, (short)172, (short)173, (short)174,
(short)0, (short)0, (short)154, (short)155, (short)175, (short)176, (short)156,
(short)157, (short)158, (short)159, (short)160, (short)161, (short)162, (short)0,
(short)163, (short)164, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)28, (short)165, (short)166, (short)167, (short)168, (short)0, (short)30,
(short)0, (short)0, (short)658, (short)0, (short)0, (short)0, (short)169,
(short)0, (short)0, (short)0, (short)170, (short)150, (short)0, (short)151,
(short)0, (short)152, (short)0, (short)171, (short)153, (short)0, (short)172,
(short)173, (short)174, (short)0, (short)0, (short)154, (short)155, (short)175,
(short)176, (short)156, (short)157, (short)158, (short)159, (short)160, (short)161,
(short)162, (short)0, (short)163, (short)164, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)28, (short)165, (short)166, (short)167, (short)168,
(short)0, (short)30, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)169, (short)679, (short)0, (short)0, (short)170, (short)150, (short)0,
(short)151, (short)0, (short)152, (short)0, (short)171, (short)153, (short)0,
(short)172, (short)173, (short)174, (short)0, (short)0, (short)154, (short)155,
(short)175, (short)176, (short)156, (short)157, (short)158, (short)159, (short)160,
(short)161, (short)162, (short)0, (short)163, (short)164, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)28, (short)165, (short)166, (short)167,
(short)168, (short)0, (short)30, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)169, (short)683, (short)0, (short)0, (short)170, (short)150,
(short)0, (short)151, (short)0, (short)152, (short)0, (short)171, (short)153,
(short)0, (short)172, (short)173, (short)174, (short)0, (short)0, (short)154,
(short)155, (short)175, (short)176, (short)156, (short)157, (short)158, (short)159,
(short)160, (short)161, (short)162, (short)0, (short)163, (short)164, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)28, (short)165, (short)166,
(short)167, (short)168, (short)0, (short)30, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)169, (short)685, (short)0, (short)0,
(short)170, (short)150, (short)0, (short)151, (short)0, (short)152, (short)0,
(short)171, (short)153, (short)0, (short)172, (short)173, (short)174, (short)0,
(short)0, (short)154, (short)155, (short)175, (short)176, (short)156, (short)157,
(short)158, (short)159, (short)160, (short)161, (short)162, (short)0, (short)163,
(short)164, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)165,
(short)166, (short)167, (short)168, (short)0, (short)30, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)169, (short)705, (short)0,
(short)0, (short)170, (short)150, (short)0, (short)151, (short)0, (short)152,
(short)0, (short)171, (short)153, (short)0, (short)172, (short)173, (short)174,
(short)0, (short)0, (short)154, (short)155, (short)175, (short)176, (short)156,
(short)157, (short)158, (short)159, (short)160, (short)161, (short)162, (short)0,
(short)163, (short)164, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)28, (short)165, (short)166, (short)167, (short)168, (short)0, (short)30,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)169, (short)0,
(short)0, (short)0, (short)170, (short)150, (short)0, (short)151, (short)0,
(short)0, (short)0, (short)171, (short)0, (short)0, (short)172, (short)173,
(short)174, (short)0, (short)0, (short)154, (short)155, (short)175, (short)176,
(short)156, (short)157, (short)158, (short)159, (short)160, (short)161, (short)162,
(short)0, (short)163, (short)164, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)165, (short)166, (short)167, (short)168, (short)0,
(short)30, (short)0, (short)0, (short)0, (short)306, (short)0, (short)0, (short)169,
(short)0, (short)0, (short)0, (short)170, (short)150, (short)0, (short)151,
(short)0, (short)0, (short)0, (short)171, (short)0, (short)0, (short)172,
(short)173, (short)174, (short)0, (short)0, (short)154, (short)155, (short)175,
(short)176, (short)156, (short)157, (short)158, (short)159, (short)160, (short)161,
(short)162, (short)0, (short)163, (short)164, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)28, (short)165, (short)166, (short)167, (short)168,
(short)0, (short)30, (short)0, (short)0, (short)0, (short)593, (short)0, (short)0,
(short)169, (short)0, (short)0, (short)0, (short)170, (short)150, (short)0,
(short)151, (short)0, (short)0, (short)0, (short)171, (short)0, (short)0,
(short)172, (short)173, (short)174, (short)0, (short)0, (short)154, (short)155,
(short)175, (short)176, (short)156, (short)157, (short)158, (short)159, (short)160,
(short)161, (short)162, (short)0, (short)163, (short)164, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)28, (short)165, (short)166, (short)167,
(short)168, (short)0, (short)30, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)302, (short)0, (short)0, (short)0, (short)170, (short)150,
(short)0, (short)151, (short)0, (short)0, (short)0, (short)171, (short)0,
(short)0, (short)172, (short)173, (short)174, (short)0, (short)0, (short)154,
(short)155, (short)175, (short)176, (short)156, (short)157, (short)158, (short)159,
(short)160, (short)161, (short)162, (short)0, (short)163, (short)164, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)28, (short)165, (short)166,
(short)167, (short)168, (short)0, (short)30, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)312, (short)0, (short)0, (short)0, (short)170,
(short)150, (short)0, (short)151, (short)0, (short)0, (short)0, (short)171,
(short)0, (short)0, (short)172, (short)173, (short)174, (short)0, (short)0,
(short)154, (short)155, (short)175, (short)176, (short)156, (short)157, (short)158,
(short)159, (short)160, (short)161, (short)162, (short)0, (short)163, (short)164,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)62, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)165, (short)166,
(short)167, (short)168, (short)0, (short)30, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)169, (short)0, (short)0, (short)0, (short)170,
(short)0, (short)62, (short)0, (short)0, (short)0, (short)0, (short)171, (short)0,
(short)0, (short)172, (short)173, (short)174, (short)0, (short)0, (short)0,
(short)0, (short)175, (short)176, (short)28, (short)63, (short)0, (short)64,
(short)65, (short)0, (short)30, (short)0, (short)0, (short)0, (short)0, (short)432,
(short)0, (short)66, (short)0, (short)0, (short)67, (short)68, (short)0, (short)0,
(short)0, (short)433, (short)434, (short)0, (short)69, (short)0, (short)0,
(short)0, (short)70, (short)71, (short)28, (short)63, (short)0, (short)64,
(short)65, (short)0, (short)30, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)66, (short)0, (short)0, (short)67, (short)68, (short)0, (short)0,
(short)0, (short)433, (short)434, (short)0, (short)69, (short)0, (short)0,
(short)0, (short)70, (short)71}; static short Cyc_yycheck[ 4018u]={ (short)52,
(short)0, (short)86, (short)204, (short)52, (short)129, (short)52, (short)59,
(short)152, (short)87, (short)88, (short)53, (short)112, (short)111, (short)83,
(short)22, (short)52, (short)86, (short)32, (short)301, (short)407, (short)70,
(short)21, (short)22, (short)363, (short)24, (short)54, (short)26, (short)27,
(short)277, (short)44, (short)242, (short)142, (short)32, (short)103, (short)113,
(short)86, (short)147, (short)169, (short)17, (short)39, (short)411, (short)111,
(short)112, (short)369, (short)44, (short)98, (short)99, (short)100, (short)101,
(short)98, (short)50, (short)98, (short)649, (short)106, (short)54, (short)108,
(short)92, (short)106, (short)66, (short)108, (short)110, (short)108, (short)450,
(short)71, (short)354, (short)64, (short)66, (short)272, (short)428, (short)359,
(short)127, (short)71, (short)115, (short)69, (short)434, (short)64, (short)93,
(short)94, (short)78, (short)27, (short)80, (short)171, (short)172, (short)454,
(short)174, (short)83, (short)45, (short)46, (short)47, (short)99, (short)90,
(short)91, (short)92, (short)93, (short)109, (short)421, (short)84, (short)107,
(short)64, (short)169, (short)99, (short)99, (short)192, (short)315, (short)103,
(short)84, (short)396, (short)55, (short)104, (short)109, (short)99, (short)708,
(short)169, (short)97, (short)103, (short)17, (short)18, (short)19, (short)118,
(short)83, (short)717, (short)121, (short)106, (short)123, (short)721, (short)204,
(short)723, (short)65, (short)66, (short)412, (short)84, (short)131, (short)147,
(short)99, (short)134, (short)135, (short)90, (short)103, (short)73, (short)82,
(short)375, (short)97, (short)212, (short)97, (short)79, (short)96, (short)102,
(short)147, (short)383, (short)89, (short)261, (short)94, (short)205, (short)220,
(short)105, (short)106, (short)205, (short)116, (short)205, (short)226, (short)100,
(short)228, (short)229, (short)523, (short)102, (short)103, (short)109, (short)102,
(short)225, (short)104, (short)302, (short)84, (short)84, (short)102, (short)545,
(short)94, (short)287, (short)90, (short)84, (short)91, (short)312, (short)140,
(short)141, (short)89, (short)90, (short)296, (short)105, (short)289, (short)203,
(short)242, (short)97, (short)261, (short)97, (short)242, (short)100, (short)130,
(short)298, (short)446, (short)447, (short)214, (short)94, (short)0, (short)94,
(short)203, (short)84, (short)576, (short)108, (short)109, (short)110, (short)288,
(short)94, (short)91, (short)94, (short)84, (short)214, (short)285, (short)286,
(short)287, (short)84, (short)90, (short)91, (short)236, (short)84, (short)94,
(short)294, (short)91, (short)296, (short)306, (short)90, (short)108, (short)97,
(short)301, (short)302, (short)378, (short)83, (short)97, (short)236, (short)116,
(short)117, (short)318, (short)39, (short)89, (short)312, (short)631, (short)106,
(short)302, (short)94, (short)97, (short)291, (short)339, (short)340, (short)341,
(short)309, (short)593, (short)103, (short)312, (short)106, (short)310, (short)108,
(short)109, (short)110, (short)93, (short)315, (short)112, (short)272, (short)97,
(short)315, (short)324, (short)84, (short)298, (short)89, (short)473, (short)272,
(short)273, (short)353, (short)354, (short)95, (short)277, (short)96, (short)83,
(short)359, (short)296, (short)86, (short)428, (short)363, (short)102, (short)83,
(short)94, (short)105, (short)434, (short)97, (short)90, (short)91, (short)92,
(short)93, (short)102, (short)296, (short)85, (short)298, (short)103, (short)93,
(short)60, (short)61, (short)62, (short)361, (short)375, (short)99, (short)111,
(short)112, (short)420, (short)310, (short)94, (short)93, (short)383, (short)97,
(short)400, (short)93, (short)396, (short)99, (short)118, (short)169, (short)400,
(short)99, (short)98, (short)449, (short)99, (short)405, (short)384, (short)385,
(short)386, (short)400, (short)388, (short)399, (short)93, (short)89, (short)388,
(short)86, (short)93, (short)97, (short)388, (short)95, (short)276, (short)412,
(short)99, (short)97, (short)280, (short)105, (short)106, (short)401, (short)93,
(short)420, (short)93, (short)401, (short)99, (short)401, (short)99, (short)94,
(short)99, (short)104, (short)564, (short)93, (short)462, (short)461, (short)212,
(short)401, (short)169, (short)99, (short)367, (short)93, (short)369, (short)99,
(short)92, (short)92, (short)97, (short)99, (short)104, (short)97, (short)97,
(short)523, (short)98, (short)585, (short)102, (short)102, (short)585, (short)463,
(short)106, (short)106, (short)93, (short)93, (short)94, (short)480, (short)461,
(short)462, (short)99, (short)473, (short)111, (short)99, (short)602, (short)95,
(short)142, (short)602, (short)104, (short)99, (short)95, (short)147, (short)99,
(short)95, (short)99, (short)212, (short)103, (short)99, (short)95, (short)97,
(short)514, (short)97, (short)99, (short)541, (short)460, (short)98, (short)99,
(short)97, (short)421, (short)430, (short)67, (short)68, (short)84, (short)426,
(short)435, (short)63, (short)64, (short)430, (short)90, (short)111, (short)112,
(short)93, (short)435, (short)285, (short)286, (short)97, (short)641, (short)331,
(short)332, (short)333, (short)334, (short)514, (short)294, (short)446, (short)447,
(short)95, (short)108, (short)109, (short)110, (short)597, (short)302, (short)84,
(short)98, (short)99, (short)98, (short)99, (short)69, (short)90, (short)476,
(short)114, (short)312, (short)98, (short)99, (short)115, (short)97, (short)673,
(short)98, (short)99, (short)673, (short)529, (short)530, (short)104, (short)113,
(short)476, (short)689, (short)108, (short)109, (short)110, (short)285, (short)286,
(short)287, (short)569, (short)538, (short)97, (short)84, (short)95, (short)538,
(short)294, (short)538, (short)296, (short)90, (short)98, (short)99, (short)708,
(short)301, (short)302, (short)569, (short)97, (short)85, (short)86, (short)22,
(short)23, (short)95, (short)585, (short)522, (short)312, (short)721, (short)95,
(short)108, (short)109, (short)110, (short)593, (short)98, (short)99, (short)596,
(short)98, (short)99, (short)590, (short)94, (short)522, (short)602, (short)84,
(short)94, (short)375, (short)37, (short)98, (short)99, (short)90, (short)98,
(short)99, (short)95, (short)383, (short)84, (short)589, (short)97, (short)329,
(short)330, (short)589, (short)608, (short)589, (short)610, (short)98, (short)99,
(short)106, (short)97, (short)108, (short)109, (short)110, (short)98, (short)99,
(short)97, (short)611, (short)612, (short)564, (short)614, (short)97, (short)616,
(short)98, (short)99, (short)641, (short)97, (short)564, (short)98, (short)99,
(short)335, (short)336, (short)93, (short)93, (short)375, (short)337, (short)338,
(short)84, (short)85, (short)104, (short)87, (short)88, (short)383, (short)90,
(short)638, (short)97, (short)95, (short)98, (short)652, (short)99, (short)97,
(short)655, (short)96, (short)100, (short)101, (short)98, (short)95, (short)673,
(short)99, (short)400, (short)654, (short)108, (short)107, (short)657, (short)658,
(short)112, (short)113, (short)95, (short)98, (short)96, (short)674, (short)412,
(short)95, (short)70, (short)678, (short)679, (short)462, (short)99, (short)682,
(short)683, (short)95, (short)685, (short)71, (short)72, (short)73, (short)74,
(short)75, (short)76, (short)77, (short)78, (short)79, (short)80, (short)35,
(short)687, (short)98, (short)699, (short)102, (short)691, (short)702, (short)689,
(short)704, (short)705, (short)93, (short)689, (short)99, (short)104, (short)28,
(short)96, (short)93, (short)41, (short)83, (short)715, (short)98, (short)707,
(short)84, (short)660, (short)97, (short)111, (short)708, (short)98, (short)461,
(short)462, (short)708, (short)660, (short)150, (short)99, (short)152, (short)153,
(short)514, (short)84, (short)95, (short)721, (short)99, (short)95, (short)98,
(short)721, (short)97, (short)163, (short)164, (short)89, (short)98, (short)98,
(short)98, (short)84, (short)107, (short)171, (short)172, (short)98, (short)174,
(short)97, (short)95, (short)98, (short)98, (short)107, (short)93, (short)99,
(short)98, (short)95, (short)98, (short)95, (short)97, (short)94, (short)26,
(short)98, (short)95, (short)99, (short)192, (short)94, (short)98, (short)98,
(short)98, (short)94, (short)93, (short)514, (short)98, (short)0, (short)102,
(short)95, (short)98, (short)3, (short)4, (short)5, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)20, (short)21, (short)107,
(short)98, (short)24, (short)98, (short)98, (short)104, (short)590, (short)98,
(short)95, (short)93, (short)0, (short)310, (short)203, (short)35, (short)36,
(short)37, (short)38, (short)39, (short)554, (short)426, (short)42, (short)43,
(short)212, (short)522, (short)294, (short)47, (short)48, (short)49, (short)50,
(short)273, (short)569, (short)53, (short)54, (short)55, (short)56, (short)57,
(short)58, (short)59, (short)153, (short)61, (short)62, (short)292, (short)564,
(short)436, (short)153, (short)323, (short)326, (short)328, (short)325, -
(short)1, - (short)1, (short)590, (short)327, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84,
(short)85, (short)86, (short)87, (short)88, (short)89, (short)90, (short)91, -
(short)1, (short)93, - (short)1, - (short)1, - (short)1, (short)97, - (short)1,
- (short)1, (short)100, (short)101, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)108, - (short)1, - (short)1, (short)111,
(short)112, (short)113, - (short)1, - (short)1, - (short)1, - (short)1, (short)118,
(short)119, - (short)1, (short)323, - (short)1, (short)325, (short)326, (short)327,
(short)328, (short)329, (short)330, (short)331, (short)332, (short)333, (short)334,
(short)335, (short)336, (short)337, (short)338, (short)339, (short)340, (short)341,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, (short)22, (short)23, (short)24,
(short)25, (short)378, (short)27, (short)28, (short)29, (short)30, (short)31,
(short)32, (short)33, (short)34, (short)35, (short)36, (short)37, (short)38,
(short)39, (short)40, - (short)1, (short)42, (short)43, (short)44, - (short)1, -
(short)1, (short)47, (short)48, (short)49, (short)50, (short)51, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, (short)59, -
(short)1, (short)61, (short)62, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)428, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)434, - (short)1, (short)84, (short)85, (short)86,
(short)87, (short)88, (short)89, (short)90, (short)91, - (short)1, (short)93,
(short)94, - (short)1, - (short)1, (short)97, - (short)1, - (short)1, (short)100,
(short)101, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)108, - (short)1, - (short)1, (short)111, (short)112, (short)113,
- (short)1, - (short)1, - (short)1, - (short)1, (short)118, (short)119, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)480, - (short)1, - (short)1, - (short)1, - (short)1, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, (short)25, -
(short)1, (short)27, (short)28, (short)29, (short)30, (short)31, (short)32,
(short)33, (short)34, (short)35, (short)36, (short)37, (short)38, (short)39,
(short)40, (short)523, (short)42, (short)43, (short)44, - (short)1, - (short)1,
(short)47, (short)48, (short)49, (short)50, (short)51, (short)52, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1,
(short)61, (short)62, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)84, (short)85, (short)86, (short)87,
(short)88, (short)89, (short)90, (short)91, - (short)1, (short)93, (short)94,
(short)95, - (short)1, (short)97, - (short)1, - (short)1, (short)100, (short)101,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)108,
- (short)1, - (short)1, (short)111, (short)112, (short)113, - (short)1, (short)597,
- (short)1, - (short)1, (short)118, (short)119, (short)3, (short)4, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, (short)25, - (short)1, (short)27,
(short)28, (short)29, (short)30, (short)31, (short)32, (short)33, (short)34,
(short)35, (short)36, (short)37, (short)38, (short)39, (short)40, - (short)1,
(short)42, (short)43, (short)44, - (short)1, - (short)1, (short)47, (short)48,
(short)49, (short)50, (short)51, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, (short)89,
(short)90, (short)91, - (short)1, (short)93, (short)94, - (short)1, - (short)1,
(short)97, - (short)1, - (short)1, (short)100, (short)101, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)108, - (short)1, -
(short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1, - (short)1,
- (short)1, (short)118, (short)119, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)35, (short)36, (short)37, - (short)1,
(short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1, - (short)1,
- (short)1, (short)47, (short)48, (short)49, (short)50, - (short)1, - (short)1,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, (short)59, -
(short)1, (short)61, (short)62, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)84, (short)85, (short)86,
(short)87, (short)88, (short)89, (short)90, (short)91, - (short)1, - (short)1,
(short)94, - (short)1, - (short)1, (short)97, - (short)1, - (short)1, (short)100,
(short)101, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)108, - (short)1, - (short)1, (short)111, (short)112, (short)113,
- (short)1, - (short)1, - (short)1, - (short)1, (short)118, (short)119, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)35, (short)36,
(short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42, - (short)1,
- (short)1, - (short)1, - (short)1, (short)47, (short)48, (short)49, (short)50,
- (short)1, - (short)1, (short)53, (short)54, (short)55, (short)56, (short)57,
(short)58, (short)59, - (short)1, (short)61, (short)62, - (short)1, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84, (short)85,
(short)86, (short)87, (short)88, (short)89, (short)90, (short)91, (short)36, -
(short)1, - (short)1, - (short)1, - (short)1, (short)97, - (short)1, - (short)1,
(short)100, (short)101, - (short)1, (short)47, (short)48, - (short)1, - (short)1,
- (short)1, (short)108, - (short)1, - (short)1, (short)111, (short)112, (short)113,
- (short)1, - (short)1, - (short)1, - (short)1, (short)118, (short)119, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, (short)84, (short)24, - (short)1, -
(short)1, - (short)1, (short)89, - (short)1, (short)91, - (short)1, (short)93, -
(short)1, - (short)1, - (short)1, (short)36, (short)98, (short)38, (short)100,
(short)101, - (short)1, - (short)1, (short)43, - (short)1, (short)45, (short)46,
(short)47, (short)48, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)84, - (short)1, - (short)1, (short)36, - (short)1, (short)89,
(short)90, (short)91, - (short)1, - (short)1, (short)43, - (short)1, - (short)1,
(short)97, (short)47, (short)48, (short)100, (short)101, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)108, (short)109, (short)110,
- (short)1, - (short)1, - (short)1, - (short)1, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)84, - (short)1, - (short)1, - (short)1,
- (short)1, (short)89, (short)90, (short)91, (short)36, - (short)1, - (short)1,
- (short)1, - (short)1, (short)97, - (short)1, - (short)1, (short)100, (short)101,
- (short)1, (short)47, (short)48, - (short)1, - (short)1, - (short)1, (short)108,
(short)109, (short)110, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)36, (short)84, - (short)1, - (short)1,
- (short)1, - (short)1, (short)89, (short)90, (short)91, - (short)1, (short)93,
(short)47, (short)48, - (short)1, (short)97, (short)98, - (short)1, (short)100,
(short)101, - (short)1, - (short)1, - (short)1, - (short)1, (short)106, -
(short)1, (short)108, (short)109, (short)110, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)89, - (short)1, (short)91, - (short)1,
(short)93, - (short)1, - (short)1, - (short)1, (short)97, (short)98, - (short)1,
(short)100, (short)101, - (short)1, - (short)1, - (short)1, - (short)1, (short)106,
- (short)1, (short)108, (short)109, (short)110, (short)3, (short)4, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)36, - (short)1, (short)38, - (short)1, - (short)1,
- (short)1, - (short)1, (short)43, - (short)1, - (short)1, - (short)1, (short)47,
(short)48, - (short)1, - (short)1, - (short)1, - (short)1, (short)3, (short)4,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)36, - (short)1, (short)38, (short)89, -
(short)1, (short)91, - (short)1, (short)43, (short)94, - (short)1, (short)96,
(short)47, (short)48, - (short)1, (short)100, (short)101, (short)3, (short)4,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)36, - (short)1, - (short)1, - (short)1,
(short)89, - (short)1, (short)91, (short)43, - (short)1, (short)94, - (short)1,
(short)47, (short)48, - (short)1, - (short)1, (short)100, (short)101, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)25, - (short)1,
(short)27, (short)28, (short)29, (short)30, (short)31, (short)32, (short)33,
(short)34, (short)35, (short)89, (short)37, (short)91, (short)39, (short)40, -
(short)1, (short)42, - (short)1, (short)44, - (short)1, - (short)1, (short)100,
(short)101, (short)49, (short)50, (short)51, (short)52, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, (short)61,
(short)62, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, - (short)1,
(short)90, (short)36, - (short)1, (short)93, (short)94, - (short)1, - (short)1,
(short)97, - (short)1, - (short)1, - (short)1, (short)101, (short)47, (short)48,
- (short)1, - (short)1, - (short)1, - (short)1, (short)108, - (short)1, -
(short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1, - (short)1,
- (short)1, (short)118, (short)119, - (short)1, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)89,
- (short)1, (short)91, - (short)1, (short)93, (short)36, - (short)1, - (short)1,
- (short)1, (short)98, - (short)1, (short)100, (short)101, - (short)1, - (short)1,
- (short)1, (short)47, (short)48, - (short)1, - (short)1, - (short)1, - (short)1,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21,
(short)36, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)89, - (short)1, (short)91, (short)47, (short)48, (short)94,
(short)36, - (short)1, - (short)1, - (short)1, - (short)1, (short)100, (short)101,
- (short)1, - (short)1, - (short)1, - (short)1, (short)47, (short)48, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)81, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)89, - (short)1,
(short)91, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)30,
- (short)1, - (short)1, (short)100, (short)101, (short)35, (short)89, (short)37,
(short)91, (short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1,
- (short)1, - (short)1, (short)100, (short)101, (short)49, (short)50, - (short)1,
- (short)1, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, (short)61, (short)62, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84, (short)85,
(short)86, (short)87, (short)88, - (short)1, (short)90, - (short)1, - (short)1,
- (short)1, (short)94, (short)95, - (short)1, (short)97, - (short)1, - (short)1,
- (short)1, (short)101, - (short)1, - (short)1, - (short)1, (short)105, (short)106,
- (short)1, (short)108, - (short)1, - (short)1, (short)111, (short)112, (short)113,
- (short)1, - (short)1, - (short)1, - (short)1, (short)118, (short)119, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)49,
(short)50, - (short)1, - (short)1, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)84, (short)85, (short)86, (short)87, (short)88, - (short)1, (short)90, -
(short)1, - (short)1, - (short)1, (short)94, (short)95, - (short)1, (short)97, -
(short)1, - (short)1, - (short)1, (short)101, - (short)1, - (short)1, - (short)1,
(short)105, (short)106, - (short)1, (short)108, - (short)1, - (short)1, (short)111,
(short)112, (short)113, - (short)1, - (short)1, - (short)1, - (short)1, (short)118,
(short)119, (short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1,
- (short)1, (short)42, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)49, (short)50, - (short)1, - (short)1, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, (short)61,
(short)62, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, -
(short)1, (short)90, - (short)1, - (short)1, - (short)1, (short)94, (short)95, -
(short)1, (short)97, - (short)1, - (short)1, - (short)1, (short)101, - (short)1,
- (short)1, - (short)1, (short)105, (short)106, - (short)1, (short)108, -
(short)1, - (short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1,
- (short)1, - (short)1, (short)118, (short)119, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)49, (short)50, - (short)1,
- (short)1, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, (short)61, (short)62, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84, (short)85,
(short)86, (short)87, (short)88, - (short)1, (short)90, - (short)1, - (short)1,
- (short)1, (short)94, - (short)1, - (short)1, (short)97, - (short)1, - (short)1,
- (short)1, (short)101, - (short)1, - (short)1, - (short)1, (short)105, (short)106,
- (short)1, (short)108, - (short)1, - (short)1, (short)111, (short)112, (short)113,
- (short)1, - (short)1, - (short)1, - (short)1, (short)118, (short)119, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)49,
(short)50, - (short)1, - (short)1, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)84, (short)85, (short)86, (short)87, (short)88, - (short)1, (short)90, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)97,
- (short)1, - (short)1, - (short)1, (short)101, (short)35, - (short)1, (short)37,
- (short)1, (short)39, (short)107, (short)108, (short)42, - (short)1, (short)111,
(short)112, (short)113, - (short)1, - (short)1, (short)49, (short)50, (short)118,
(short)119, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, (short)61, (short)62, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84, (short)85,
(short)86, (short)87, (short)88, - (short)1, (short)90, - (short)1, - (short)1,
- (short)1, (short)94, - (short)1, - (short)1, (short)97, - (short)1, - (short)1,
- (short)1, (short)101, (short)35, - (short)1, (short)37, - (short)1, (short)39,
- (short)1, (short)108, (short)42, - (short)1, (short)111, (short)112, (short)113,
- (short)1, - (short)1, (short)49, (short)50, (short)118, (short)119, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1,
(short)61, (short)62, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)84, (short)85, (short)86, (short)87,
(short)88, - (short)1, (short)90, - (short)1, - (short)1, (short)93, - (short)1,
- (short)1, - (short)1, (short)97, - (short)1, - (short)1, - (short)1, (short)101,
(short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1, (short)108,
(short)42, - (short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1,
(short)49, (short)50, (short)118, (short)119, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, - (short)1,
(short)90, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)97, - (short)1, - (short)1, - (short)1, (short)101, (short)35, - (short)1,
(short)37, - (short)1, (short)39, (short)107, (short)108, (short)42, - (short)1,
(short)111, (short)112, (short)113, - (short)1, - (short)1, (short)49, (short)50,
(short)118, (short)119, (short)53, (short)54, (short)55, (short)56, (short)57,
(short)58, (short)59, - (short)1, (short)61, (short)62, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84,
(short)85, (short)86, (short)87, (short)88, - (short)1, (short)90, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)97, (short)98,
- (short)1, - (short)1, (short)101, (short)35, - (short)1, (short)37, - (short)1,
(short)39, - (short)1, (short)108, (short)42, - (short)1, (short)111, (short)112,
(short)113, - (short)1, - (short)1, (short)49, (short)50, (short)118, (short)119,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, (short)59, -
(short)1, (short)61, (short)62, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)84, (short)85, (short)86,
(short)87, (short)88, - (short)1, (short)90, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)97, (short)98, - (short)1, - (short)1,
(short)101, (short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1,
(short)108, (short)42, - (short)1, (short)111, (short)112, (short)113, - (short)1,
- (short)1, (short)49, (short)50, (short)118, (short)119, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, (short)61,
(short)62, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, -
(short)1, (short)90, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)97, - (short)1, - (short)1, - (short)1, (short)101, (short)35,
- (short)1, (short)37, - (short)1, (short)39, (short)107, (short)108, (short)42,
- (short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1, (short)49,
(short)50, (short)118, (short)119, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)84, (short)85, (short)86, (short)87, (short)88, - (short)1, (short)90, -
(short)1, - (short)1, (short)93, - (short)1, - (short)1, - (short)1, (short)97,
- (short)1, - (short)1, - (short)1, (short)101, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, (short)108, (short)42, - (short)1, (short)111,
(short)112, (short)113, - (short)1, - (short)1, (short)49, (short)50, (short)118,
(short)119, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, (short)61, (short)62, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84, (short)85,
(short)86, (short)87, (short)88, - (short)1, (short)90, - (short)1, - (short)1,
(short)93, - (short)1, - (short)1, - (short)1, (short)97, - (short)1, - (short)1,
- (short)1, (short)101, (short)35, - (short)1, (short)37, - (short)1, (short)39,
- (short)1, (short)108, (short)42, - (short)1, (short)111, (short)112, (short)113,
- (short)1, - (short)1, (short)49, (short)50, (short)118, (short)119, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1,
(short)61, (short)62, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)84, (short)85, (short)86, (short)87,
(short)88, - (short)1, (short)90, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)97, (short)98, - (short)1, - (short)1, (short)101,
(short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1, (short)108,
(short)42, - (short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1,
(short)49, (short)50, (short)118, (short)119, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, - (short)1,
(short)90, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)97, (short)98, - (short)1, - (short)1, (short)101, (short)35, - (short)1,
(short)37, - (short)1, (short)39, - (short)1, (short)108, (short)42, - (short)1,
(short)111, (short)112, (short)113, - (short)1, - (short)1, (short)49, (short)50,
(short)118, (short)119, (short)53, (short)54, (short)55, (short)56, (short)57,
(short)58, (short)59, - (short)1, (short)61, (short)62, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84,
(short)85, (short)86, (short)87, (short)88, - (short)1, (short)90, - (short)1, -
(short)1, (short)93, - (short)1, - (short)1, - (short)1, (short)97, - (short)1,
- (short)1, - (short)1, (short)101, (short)35, - (short)1, (short)37, - (short)1,
(short)39, - (short)1, (short)108, (short)42, - (short)1, (short)111, (short)112,
(short)113, - (short)1, - (short)1, (short)49, (short)50, (short)118, (short)119,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, (short)59, -
(short)1, (short)61, (short)62, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)84, (short)85, (short)86,
(short)87, (short)88, - (short)1, (short)90, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)97, (short)98, - (short)1, - (short)1,
(short)101, (short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1,
(short)108, (short)42, - (short)1, (short)111, (short)112, (short)113, - (short)1,
- (short)1, (short)49, (short)50, (short)118, (short)119, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, (short)61,
(short)62, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, -
(short)1, (short)90, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)97, (short)98, - (short)1, - (short)1, (short)101, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, (short)108, (short)42,
- (short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1, (short)49,
(short)50, (short)118, (short)119, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)84, (short)85, (short)86, (short)87, (short)88, - (short)1, (short)90, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)97,
(short)98, - (short)1, - (short)1, (short)101, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, (short)108, (short)42, - (short)1, (short)111,
(short)112, (short)113, - (short)1, - (short)1, (short)49, (short)50, (short)118,
(short)119, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, (short)61, (short)62, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84, (short)85,
(short)86, (short)87, (short)88, - (short)1, (short)90, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)97, (short)98, - (short)1,
- (short)1, (short)101, (short)35, - (short)1, (short)37, - (short)1, (short)39,
- (short)1, (short)108, (short)42, - (short)1, (short)111, (short)112, (short)113,
- (short)1, - (short)1, (short)49, (short)50, (short)118, (short)119, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1,
(short)61, (short)62, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)84, (short)85, (short)86, (short)87,
(short)88, - (short)1, (short)90, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)97, - (short)1, - (short)1, - (short)1, (short)101,
(short)35, - (short)1, (short)37, - (short)1, - (short)1, - (short)1, (short)108,
- (short)1, - (short)1, (short)111, (short)112, (short)113, - (short)1, -
(short)1, (short)49, (short)50, (short)118, (short)119, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, (short)61,
(short)62, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, -
(short)1, (short)90, - (short)1, - (short)1, - (short)1, (short)94, - (short)1,
- (short)1, (short)97, - (short)1, - (short)1, - (short)1, (short)101, (short)35,
- (short)1, (short)37, - (short)1, - (short)1, - (short)1, (short)108, - (short)1,
- (short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1, (short)49,
(short)50, (short)118, (short)119, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)84, (short)85, (short)86, (short)87, (short)88, - (short)1, (short)90, -
(short)1, - (short)1, - (short)1, (short)94, - (short)1, - (short)1, (short)97,
- (short)1, - (short)1, - (short)1, (short)101, (short)35, - (short)1, (short)37,
- (short)1, - (short)1, - (short)1, (short)108, - (short)1, - (short)1, (short)111,
(short)112, (short)113, - (short)1, - (short)1, (short)49, (short)50, (short)118,
(short)119, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, (short)61, (short)62, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84, (short)85,
(short)86, (short)87, (short)88, - (short)1, (short)90, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)97, - (short)1, - (short)1,
- (short)1, (short)101, (short)35, - (short)1, (short)37, - (short)1, - (short)1,
- (short)1, (short)108, - (short)1, - (short)1, (short)111, (short)112, (short)113,
- (short)1, - (short)1, (short)49, (short)50, (short)118, (short)119, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1,
(short)61, (short)62, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)84, (short)85, (short)86, (short)87,
(short)88, - (short)1, (short)90, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)97, - (short)1, - (short)1, - (short)1, (short)101,
(short)35, - (short)1, (short)37, - (short)1, - (short)1, - (short)1, (short)108,
- (short)1, - (short)1, (short)111, (short)112, (short)113, - (short)1, -
(short)1, (short)49, (short)50, (short)118, (short)119, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, (short)61,
(short)62, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)37, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, -
(short)1, (short)90, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)97, - (short)1, - (short)1, - (short)1, (short)101, - (short)1,
(short)37, - (short)1, - (short)1, - (short)1, - (short)1, (short)108, - (short)1,
- (short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1, -
(short)1, - (short)1, (short)118, (short)119, (short)84, (short)85, - (short)1,
(short)87, (short)88, - (short)1, (short)90, - (short)1, - (short)1, - (short)1,
- (short)1, (short)95, - (short)1, (short)97, - (short)1, - (short)1, (short)100,
(short)101, - (short)1, - (short)1, - (short)1, (short)105, (short)106, -
(short)1, (short)108, - (short)1, - (short)1, - (short)1, (short)112, (short)113,
(short)84, (short)85, - (short)1, (short)87, (short)88, - (short)1, (short)90, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)97,
- (short)1, - (short)1, (short)100, (short)101, - (short)1, - (short)1, -
(short)1, (short)105, (short)106, - (short)1, (short)108, - (short)1, - (short)1,
- (short)1, (short)112, (short)113}; char Cyc_Yyimpossible_tag[ 13u]="Yyimpossible";
struct Cyc_Yyimpossible_struct{ char* tag; } ; char Cyc_Yystack_overflow_tag[ 17u]="Yystack_overflow";
struct Cyc_Yystack_overflow_struct{ char* tag; } ; extern void Cyc_yyerror(
struct _tagged_string); extern int Cyc_yylex(); char Cyc_YYINITIALSVAL_tag[ 14u]="YYINITIALSVAL";
struct Cyc_YYINITIALSVAL_struct{ char* tag; } ; static int Cyc_yychar=( int)'\000';
struct Cyc_YYINITIALSVAL_struct Cyc_yyinitval={ Cyc_YYINITIALSVAL_tag}; struct
_xtunion_struct* Cyc_yylval=( struct _xtunion_struct*)& Cyc_yyinitval; static
int Cyc_yynerrs= 0; int Cyc_yyparse(){ int yystate; int yyn= 0; int yyerrstatus;
int yychar1= 0; int yyssp_offset; struct _tagged_ptr1 yyss=( struct _tagged_ptr1)({
short* _temp2075=( short*)({ unsigned int _temp2073= 200u; short* _temp2074=(
short*) GC_malloc_atomic( sizeof( short) * _temp2073); unsigned int i; for( i= 0;
i < _temp2073; i ++){ _temp2074[ i]= (short)0;} _temp2074;}); struct
_tagged_ptr1 _temp2076; _temp2076.curr= _temp2075; _temp2076.base= _temp2075;
_temp2076.last_plus_one= _temp2075 + 200; _temp2076;}); int yyvsp_offset; struct
_tagged_ptr2 yyvs=( struct _tagged_ptr2)({ struct _xtunion_struct** _temp2079=(
struct _xtunion_struct**)({ unsigned int _temp2077= 200u; struct _xtunion_struct**
_temp2078=( struct _xtunion_struct**) GC_malloc( sizeof( struct _xtunion_struct*)
* _temp2077); unsigned int i; for( i= 0; i < _temp2077; i ++){ _temp2078[ i]=
Cyc_yylval;} _temp2078;}); struct _tagged_ptr2 _temp2080; _temp2080.curr=
_temp2079; _temp2080.base= _temp2079; _temp2080.last_plus_one= _temp2079 + 200;
_temp2080;}); int yylsp_offset; struct _tagged_ptr3 yyls=( struct _tagged_ptr3)({
struct Cyc_Yyltype* _temp2083=( struct Cyc_Yyltype*)({ unsigned int _temp2081=
200u; struct Cyc_Yyltype* _temp2082=( struct Cyc_Yyltype*) GC_malloc( sizeof(
struct Cyc_Yyltype) * _temp2081); unsigned int i; for( i= 0; i < _temp2081; i ++){
_temp2082[ i]= Cyc_yynewloc();} _temp2082;}); struct _tagged_ptr3 _temp2084;
_temp2084.curr= _temp2083; _temp2084.base= _temp2083; _temp2084.last_plus_one=
_temp2083 + 200; _temp2084;}); int yystacksize= 200; struct _xtunion_struct*
yyval= Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar=
- 2; yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate:({ struct
_tagged_ptr1 _temp2085= yyss; short* _temp2087= _temp2085.curr +( ++
yyssp_offset); if( _temp2087 < _temp2085.base? 1: _temp2087 >= _temp2085.last_plus_one){
_throw( Null_Exception);}* _temp2087=( short) yystate;}); if( yyssp_offset >=
yystacksize - 1){ if( yystacksize >= 10000){ Cyc_yyerror(( struct _tagged_string)({
char* _temp2088=( char*)"parser stack overflow"; struct _tagged_string _temp2089;
_temp2089.curr= _temp2088; _temp2089.base= _temp2088; _temp2089.last_plus_one=
_temp2088 + 22; _temp2089;}));( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_Yystack_overflow_struct* _temp2090=( struct Cyc_Yystack_overflow_struct*)
GC_malloc( sizeof( struct Cyc_Yystack_overflow_struct)); _temp2090[ 0]=({ struct
Cyc_Yystack_overflow_struct _temp2091; _temp2091.tag= Cyc_Yystack_overflow_tag;
_temp2091;}); _temp2090;}));} yystacksize *= 2; if( yystacksize > 10000){
yystacksize= 10000;}{ struct _tagged_ptr1 yyss1=({ unsigned int _temp2092=(
unsigned int) yystacksize; short* _temp2093=( short*) GC_malloc_atomic( sizeof(
short) * _temp2092); unsigned int i; struct _tagged_ptr1 _temp2094={ _temp2093,
_temp2093, _temp2093 + _temp2092}; for( i= 0; i < _temp2092; i ++){ _temp2093[ i]=
(short)0;} _temp2094;}); struct _tagged_ptr2 yyvs1=({ unsigned int _temp2095=(
unsigned int) yystacksize; struct _xtunion_struct** _temp2096=( struct
_xtunion_struct**) GC_malloc( sizeof( struct _xtunion_struct*) * _temp2095);
unsigned int i; struct _tagged_ptr2 _temp2097={ _temp2096, _temp2096, _temp2096
+ _temp2095}; for( i= 0; i < _temp2095; i ++){ _temp2096[ i]= Cyc_yylval;}
_temp2097;}); struct _tagged_ptr3 yyls1=({ unsigned int _temp2098=( unsigned int)
yystacksize; struct Cyc_Yyltype* _temp2099=( struct Cyc_Yyltype*) GC_malloc(
sizeof( struct Cyc_Yyltype) * _temp2098); unsigned int i; struct _tagged_ptr3
_temp2100={ _temp2099, _temp2099, _temp2099 + _temp2098}; for( i= 0; i <
_temp2098; i ++){ _temp2099[ i]= Cyc_yynewloc();} _temp2100;});{ int i= 0; for(
0; i <= yyssp_offset; i ++){({ struct _tagged_ptr1 _temp2101= yyss1; short*
_temp2103= _temp2101.curr + i; if( _temp2103 < _temp2101.base? 1: _temp2103 >=
_temp2101.last_plus_one){ _throw( Null_Exception);}* _temp2103=({ struct
_tagged_ptr1 _temp2104= yyss; short* _temp2106= _temp2104.curr + i; if(
_temp2106 < _temp2104.base? 1: _temp2106 >= _temp2104.last_plus_one){ _throw(
Null_Exception);}* _temp2106;});});({ struct _tagged_ptr2 _temp2107= yyvs1;
struct _xtunion_struct** _temp2109= _temp2107.curr + i; if( _temp2109 <
_temp2107.base? 1: _temp2109 >= _temp2107.last_plus_one){ _throw( Null_Exception);}*
_temp2109=({ struct _tagged_ptr2 _temp2110= yyvs; struct _xtunion_struct**
_temp2112= _temp2110.curr + i; if( _temp2112 < _temp2110.base? 1: _temp2112 >=
_temp2110.last_plus_one){ _throw( Null_Exception);}* _temp2112;});});({ struct
_tagged_ptr3 _temp2113= yyls1; struct Cyc_Yyltype* _temp2115= _temp2113.curr + i;
if( _temp2115 < _temp2113.base? 1: _temp2115 >= _temp2113.last_plus_one){ _throw(
Null_Exception);}* _temp2115=({ struct _tagged_ptr3 _temp2116= yyls; struct Cyc_Yyltype*
_temp2118= _temp2116.curr + i; if( _temp2118 < _temp2116.base? 1: _temp2118 >=
_temp2116.last_plus_one){ _throw( Null_Exception);}* _temp2118;});});}} yyss=
yyss1; yyvs= yyvs1; yyls= yyls1;}} goto yybackup; yybackup: yyn=( int)({ short*
_temp2119=( short*) Cyc_yypact; unsigned int _temp2120= yystate; if( _temp2120
>= 729u){ _throw( Null_Exception);} _temp2119[ _temp2120];}); if( yyn == - 32768){
goto yydefault;} if( Cyc_yychar == - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar
<= 0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <=
347: 0)?( int)({ short* _temp2121=( short*) Cyc_yytranslate; unsigned int
_temp2122= Cyc_yychar; if( _temp2122 >= 348u){ _throw( Null_Exception);}
_temp2121[ _temp2122];}): 227;} yyn += yychar1; if(( yyn < 0? 1: yyn > 4017)? 1:(
int)({ short* _temp2123=( short*) Cyc_yycheck; unsigned int _temp2124= yyn; if(
_temp2124 >= 4018u){ _throw( Null_Exception);} _temp2123[ _temp2124];}) !=
yychar1){ goto yydefault;} yyn=( int)({ short* _temp2125=( short*) Cyc_yytable;
unsigned int _temp2126= yyn; if( _temp2126 >= 4018u){ _throw( Null_Exception);}
_temp2125[ _temp2126];}); if( yyn < 0){ if( yyn == - 32768){ goto yyerrlab;} yyn=
- yyn; goto yyreduce;} else{ if( yyn == 0){ goto yyerrlab;}} if( yyn == 728){
return 0;} if( Cyc_yychar != 0){ Cyc_yychar= - 2;}({ struct _tagged_ptr2
_temp2127= yyvs; struct _xtunion_struct** _temp2129= _temp2127.curr +( ++
yyvsp_offset); if( _temp2129 < _temp2127.base? 1: _temp2129 >= _temp2127.last_plus_one){
_throw( Null_Exception);}* _temp2129= Cyc_yylval;});({ struct _tagged_ptr3
_temp2130= yyls; struct Cyc_Yyltype* _temp2132= _temp2130.curr +( ++
yylsp_offset); if( _temp2132 < _temp2130.base? 1: _temp2132 >= _temp2130.last_plus_one){
_throw( Null_Exception);}* _temp2132= Cyc_yylloc;}); if( yyerrstatus != 0){
yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int)({ short*
_temp2133=( short*) Cyc_yydefact; unsigned int _temp2134= yystate; if( _temp2134
>= 729u){ _throw( Null_Exception);} _temp2133[ _temp2134];}); if( yyn == 0){
goto yyerrlab;} yyreduce: yylen=( int)({ short* _temp2135=( short*) Cyc_yyr2;
unsigned int _temp2136= yyn; if( _temp2136 >= 389u){ _throw( Null_Exception);}
_temp2135[ _temp2136];}); if( yylen > 0){ yyval=({ struct _tagged_ptr2 _temp2137=
yyvs; struct _xtunion_struct** _temp2139= _temp2137.curr +(( yyvsp_offset + 1) -
yylen); if( _temp2139 < _temp2137.base? 1: _temp2139 >= _temp2137.last_plus_one){
_throw( Null_Exception);}* _temp2139;});} switch( yyn){ case 1: _LL2140: yyval=({
struct _tagged_ptr2 _temp2142= yyvs; struct _xtunion_struct** _temp2144=
_temp2142.curr + yyvsp_offset; if( _temp2144 < _temp2142.base? 1: _temp2144 >=
_temp2142.last_plus_one){ _throw( Null_Exception);}* _temp2144;}); Cyc_Parse_parse_result=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2145= yyvs; struct
_xtunion_struct** _temp2147= _temp2145.curr + yyvsp_offset; if( _temp2147 <
_temp2145.base? 1: _temp2147 >= _temp2145.last_plus_one){ _throw( Null_Exception);}*
_temp2147;})); break; case 2: _LL2141: yyval=({ struct _tagged_ptr2 _temp2149=
yyvs; struct _xtunion_struct** _temp2151= _temp2149.curr + yyvsp_offset; if(
_temp2151 < _temp2149.base? 1: _temp2151 >= _temp2149.last_plus_one){ _throw(
Null_Exception);}* _temp2151;}); break; case 3: _LL2148: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2153=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2153[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2154; _temp2154.tag= Cyc_DeclList_tok_tag; _temp2154.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2155= yyvs; struct _xtunion_struct** _temp2157=
_temp2155.curr +( yyvsp_offset - 1); if( _temp2157 < _temp2155.base? 1:
_temp2157 >= _temp2155.last_plus_one){ _throw( Null_Exception);}* _temp2157;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2158= yyvs; struct
_xtunion_struct** _temp2160= _temp2158.curr + yyvsp_offset; if( _temp2160 <
_temp2158.base? 1: _temp2160 >= _temp2158.last_plus_one){ _throw( Null_Exception);}*
_temp2160;}))); _temp2154;}); _temp2153;}); break; case 4: _LL2152: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2162=( struct
Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2162[ 0]=({ struct Cyc_DeclList_tok_struct _temp2163; _temp2163.tag= Cyc_DeclList_tok_tag;
_temp2163.f1=({ struct Cyc_List_List* _temp2164=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2164->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2165=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2165->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2172=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2172[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2173; _temp2173.tag= Cyc_Absyn_Using_d_tag;
_temp2173.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2174= yyvs;
struct _xtunion_struct** _temp2176= _temp2174.curr +( yyvsp_offset - 2); if(
_temp2176 < _temp2174.base? 1: _temp2176 >= _temp2174.last_plus_one){ _throw(
Null_Exception);}* _temp2176;})); _temp2173.f2= Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2177= yyvs; struct _xtunion_struct** _temp2179= _temp2177.curr
+ yyvsp_offset; if( _temp2179 < _temp2177.base? 1: _temp2179 >= _temp2177.last_plus_one){
_throw( Null_Exception);}* _temp2179;})); _temp2173;}); _temp2172;})); _temp2165->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2166= yyls; struct Cyc_Yyltype*
_temp2168= _temp2166.curr +( yylsp_offset - 2); if( _temp2168 < _temp2166.base?
1: _temp2168 >= _temp2166.last_plus_one){ _throw( Null_Exception);}* _temp2168;}).first_line,({
struct _tagged_ptr3 _temp2169= yyls; struct Cyc_Yyltype* _temp2171= _temp2169.curr
+ yylsp_offset; if( _temp2171 < _temp2169.base? 1: _temp2171 >= _temp2169.last_plus_one){
_throw( Null_Exception);}* _temp2171;}).last_line); _temp2165;}); _temp2164->tl=
0; _temp2164;}); _temp2163;}); _temp2162;}); Cyc_Lex_leave_using(); break; case
5: _LL2161: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2181=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2181[ 0]=({ struct Cyc_DeclList_tok_struct _temp2182; _temp2182.tag= Cyc_DeclList_tok_tag;
_temp2182.f1=({ struct Cyc_List_List* _temp2183=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2183->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2187=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2187->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2194=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2194[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2195; _temp2195.tag= Cyc_Absyn_Using_d_tag;
_temp2195.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2196= yyvs;
struct _xtunion_struct** _temp2198= _temp2196.curr +( yyvsp_offset - 4); if(
_temp2198 < _temp2196.base? 1: _temp2198 >= _temp2196.last_plus_one){ _throw(
Null_Exception);}* _temp2198;})); _temp2195.f2= Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2199= yyvs; struct _xtunion_struct** _temp2201= _temp2199.curr
+( yyvsp_offset - 2); if( _temp2201 < _temp2199.base? 1: _temp2201 >= _temp2199.last_plus_one){
_throw( Null_Exception);}* _temp2201;})); _temp2195;}); _temp2194;})); _temp2187->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2188= yyls; struct Cyc_Yyltype*
_temp2190= _temp2188.curr +( yylsp_offset - 4); if( _temp2190 < _temp2188.base?
1: _temp2190 >= _temp2188.last_plus_one){ _throw( Null_Exception);}* _temp2190;}).first_line,({
struct _tagged_ptr3 _temp2191= yyls; struct Cyc_Yyltype* _temp2193= _temp2191.curr
+( yylsp_offset - 1); if( _temp2193 < _temp2191.base? 1: _temp2193 >= _temp2191.last_plus_one){
_throw( Null_Exception);}* _temp2193;}).last_line); _temp2187;}); _temp2183->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2184= yyvs; struct
_xtunion_struct** _temp2186= _temp2184.curr + yyvsp_offset; if( _temp2186 <
_temp2184.base? 1: _temp2186 >= _temp2184.last_plus_one){ _throw( Null_Exception);}*
_temp2186;})); _temp2183;}); _temp2182;}); _temp2181;}); break; case 6: _LL2180:
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2203=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2203[ 0]=({ struct Cyc_DeclList_tok_struct _temp2204; _temp2204.tag= Cyc_DeclList_tok_tag;
_temp2204.f1=({ struct Cyc_List_List* _temp2205=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2205->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2206=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2206->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2213=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2213[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2214; _temp2214.tag=
Cyc_Absyn_Namespace_d_tag; _temp2214.f1=({ struct _tagged_string* _temp2215=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp2215[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2216= yyvs;
struct _xtunion_struct** _temp2218= _temp2216.curr +( yyvsp_offset - 2); if(
_temp2218 < _temp2216.base? 1: _temp2218 >= _temp2216.last_plus_one){ _throw(
Null_Exception);}* _temp2218;})); _temp2215;}); _temp2214.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2219= yyvs; struct _xtunion_struct** _temp2221=
_temp2219.curr + yyvsp_offset; if( _temp2221 < _temp2219.base? 1: _temp2221 >=
_temp2219.last_plus_one){ _throw( Null_Exception);}* _temp2221;})); _temp2214;});
_temp2213;})); _temp2206->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2207= yyls; struct Cyc_Yyltype* _temp2209= _temp2207.curr +(
yylsp_offset - 2); if( _temp2209 < _temp2207.base? 1: _temp2209 >= _temp2207.last_plus_one){
_throw( Null_Exception);}* _temp2209;}).first_line,({ struct _tagged_ptr3
_temp2210= yyls; struct Cyc_Yyltype* _temp2212= _temp2210.curr + yylsp_offset;
if( _temp2212 < _temp2210.base? 1: _temp2212 >= _temp2210.last_plus_one){ _throw(
Null_Exception);}* _temp2212;}).last_line); _temp2206;}); _temp2205->tl= 0;
_temp2205;}); _temp2204;}); _temp2203;}); Cyc_Lex_leave_namespace(); break; case
7: _LL2202: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2223=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2223[ 0]=({ struct Cyc_DeclList_tok_struct _temp2224; _temp2224.tag= Cyc_DeclList_tok_tag;
_temp2224.f1=({ struct Cyc_List_List* _temp2225=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2225->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2229=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2229->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2236=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2236[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2237; _temp2237.tag=
Cyc_Absyn_Namespace_d_tag; _temp2237.f1=({ struct _tagged_string* _temp2238=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp2238[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2239= yyvs;
struct _xtunion_struct** _temp2241= _temp2239.curr +( yyvsp_offset - 4); if(
_temp2241 < _temp2239.base? 1: _temp2241 >= _temp2239.last_plus_one){ _throw(
Null_Exception);}* _temp2241;})); _temp2238;}); _temp2237.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2242= yyvs; struct _xtunion_struct** _temp2244=
_temp2242.curr +( yyvsp_offset - 2); if( _temp2244 < _temp2242.base? 1:
_temp2244 >= _temp2242.last_plus_one){ _throw( Null_Exception);}* _temp2244;}));
_temp2237;}); _temp2236;})); _temp2229->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2230= yyls; struct Cyc_Yyltype* _temp2232= _temp2230.curr
+( yylsp_offset - 4); if( _temp2232 < _temp2230.base? 1: _temp2232 >= _temp2230.last_plus_one){
_throw( Null_Exception);}* _temp2232;}).first_line,({ struct _tagged_ptr3
_temp2233= yyls; struct Cyc_Yyltype* _temp2235= _temp2233.curr +( yylsp_offset -
1); if( _temp2235 < _temp2233.base? 1: _temp2235 >= _temp2233.last_plus_one){
_throw( Null_Exception);}* _temp2235;}).last_line); _temp2229;}); _temp2225->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2226= yyvs; struct
_xtunion_struct** _temp2228= _temp2226.curr + yyvsp_offset; if( _temp2228 <
_temp2226.base? 1: _temp2228 >= _temp2226.last_plus_one){ _throw( Null_Exception);}*
_temp2228;})); _temp2225;}); _temp2224;}); _temp2223;}); break; case 8: _LL2222:
if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2246=
yyvs; struct _xtunion_struct** _temp2248= _temp2246.curr +( yyvsp_offset - 4);
if( _temp2248 < _temp2246.base? 1: _temp2248 >= _temp2246.last_plus_one){ _throw(
Null_Exception);}* _temp2248;})),( struct _tagged_string)({ char* _temp2249=(
char*)"C"; struct _tagged_string _temp2250; _temp2250.curr= _temp2249; _temp2250.base=
_temp2249; _temp2250.last_plus_one= _temp2249 + 2; _temp2250;})) != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2251=( char*)"only extern \"C\" { ... } is allowed";
struct _tagged_string _temp2252; _temp2252.curr= _temp2251; _temp2252.base=
_temp2251; _temp2252.last_plus_one= _temp2251 + 35; _temp2252;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2253= yyls; struct Cyc_Yyltype* _temp2255= _temp2253.curr
+( yylsp_offset - 5); if( _temp2255 < _temp2253.base? 1: _temp2255 >= _temp2253.last_plus_one){
_throw( Null_Exception);}* _temp2255;}).first_line,({ struct _tagged_ptr3
_temp2256= yyls; struct Cyc_Yyltype* _temp2258= _temp2256.curr +( yylsp_offset -
4); if( _temp2258 < _temp2256.base? 1: _temp2258 >= _temp2256.last_plus_one){
_throw( Null_Exception);}* _temp2258;}).last_line));} yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2259=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2259[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2260; _temp2260.tag= Cyc_DeclList_tok_tag; _temp2260.f1=({ struct Cyc_List_List*
_temp2261=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2261->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2265=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2265->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp2272=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp2272[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp2273; _temp2273.tag= Cyc_Absyn_ExternC_d_tag;
_temp2273.f1= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2274= yyvs;
struct _xtunion_struct** _temp2276= _temp2274.curr +( yyvsp_offset - 2); if(
_temp2276 < _temp2274.base? 1: _temp2276 >= _temp2274.last_plus_one){ _throw(
Null_Exception);}* _temp2276;})); _temp2273;}); _temp2272;})); _temp2265->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2266= yyls; struct Cyc_Yyltype*
_temp2268= _temp2266.curr +( yylsp_offset - 5); if( _temp2268 < _temp2266.base?
1: _temp2268 >= _temp2266.last_plus_one){ _throw( Null_Exception);}* _temp2268;}).first_line,({
struct _tagged_ptr3 _temp2269= yyls; struct Cyc_Yyltype* _temp2271= _temp2269.curr
+( yylsp_offset - 1); if( _temp2271 < _temp2269.base? 1: _temp2271 >= _temp2269.last_plus_one){
_throw( Null_Exception);}* _temp2271;}).last_line); _temp2265;}); _temp2261->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2262= yyvs; struct
_xtunion_struct** _temp2264= _temp2262.curr + yyvsp_offset; if( _temp2264 <
_temp2262.base? 1: _temp2264 >= _temp2262.last_plus_one){ _throw( Null_Exception);}*
_temp2264;})); _temp2261;}); _temp2260;}); _temp2259;}); break; case 9: _LL2245:
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2278=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2278[ 0]=({ struct Cyc_DeclList_tok_struct _temp2279; _temp2279.tag= Cyc_DeclList_tok_tag;
_temp2279.f1= 0; _temp2279;}); _temp2278;}); break; case 10: _LL2277: yyval=({
struct _tagged_ptr2 _temp2281= yyvs; struct _xtunion_struct** _temp2283=
_temp2281.curr + yyvsp_offset; if( _temp2283 < _temp2281.base? 1: _temp2283 >=
_temp2281.last_plus_one){ _throw( Null_Exception);}* _temp2283;}); break; case
11: _LL2280: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2285=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2285[ 0]=({ struct Cyc_DeclList_tok_struct _temp2286; _temp2286.tag= Cyc_DeclList_tok_tag;
_temp2286.f1=({ struct Cyc_List_List* _temp2287=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2287->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2288=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp2288[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp2289; _temp2289.tag= Cyc_Absyn_Fn_d_tag; _temp2289.f1= Cyc_yyget_FnDecl_tok(({
struct _tagged_ptr2 _temp2290= yyvs; struct _xtunion_struct** _temp2292=
_temp2290.curr + yyvsp_offset; if( _temp2292 < _temp2290.base? 1: _temp2292 >=
_temp2290.last_plus_one){ _throw( Null_Exception);}* _temp2292;})); _temp2289;});
_temp2288;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2293= yyls;
struct Cyc_Yyltype* _temp2295= _temp2293.curr + yylsp_offset; if( _temp2295 <
_temp2293.base? 1: _temp2295 >= _temp2293.last_plus_one){ _throw( Null_Exception);}*
_temp2295;}).first_line,({ struct _tagged_ptr3 _temp2296= yyls; struct Cyc_Yyltype*
_temp2298= _temp2296.curr + yylsp_offset; if( _temp2298 < _temp2296.base? 1:
_temp2298 >= _temp2296.last_plus_one){ _throw( Null_Exception);}* _temp2298;}).last_line));
_temp2287->tl= 0; _temp2287;}); _temp2286;}); _temp2285;}); break; case 12:
_LL2284: yyval=({ struct _tagged_ptr2 _temp2300= yyvs; struct _xtunion_struct**
_temp2302= _temp2300.curr + yyvsp_offset; if( _temp2302 < _temp2300.base? 1:
_temp2302 >= _temp2300.last_plus_one){ _throw( Null_Exception);}* _temp2302;});
break; case 13: _LL2299: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2304=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2304[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2305; _temp2305.tag= Cyc_FnDecl_tok_tag;
_temp2305.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp2306= yyvs; struct _xtunion_struct** _temp2308= _temp2306.curr
+( yyvsp_offset - 1); if( _temp2308 < _temp2306.base? 1: _temp2308 >= _temp2306.last_plus_one){
_throw( Null_Exception);}* _temp2308;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2309= yyvs; struct _xtunion_struct** _temp2311= _temp2309.curr
+ yyvsp_offset; if( _temp2311 < _temp2309.base? 1: _temp2311 >= _temp2309.last_plus_one){
_throw( Null_Exception);}* _temp2311;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2312= yyls; struct Cyc_Yyltype* _temp2314= _temp2312.curr +(
yylsp_offset - 1); if( _temp2314 < _temp2312.base? 1: _temp2314 >= _temp2312.last_plus_one){
_throw( Null_Exception);}* _temp2314;}).first_line,({ struct _tagged_ptr3
_temp2315= yyls; struct Cyc_Yyltype* _temp2317= _temp2315.curr + yylsp_offset;
if( _temp2317 < _temp2315.base? 1: _temp2317 >= _temp2315.last_plus_one){ _throw(
Null_Exception);}* _temp2317;}).last_line)); _temp2305;}); _temp2304;}); break;
case 14: _LL2303: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2319=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2319[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2320; _temp2320.tag= Cyc_FnDecl_tok_tag;
_temp2320.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2321=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2321->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2322= yyvs; struct
_xtunion_struct** _temp2324= _temp2322.curr +( yyvsp_offset - 2); if( _temp2324
< _temp2322.base? 1: _temp2324 >= _temp2322.last_plus_one){ _throw(
Null_Exception);}* _temp2324;})); _temp2321;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2325= yyvs; struct _xtunion_struct** _temp2327=
_temp2325.curr +( yyvsp_offset - 1); if( _temp2327 < _temp2325.base? 1:
_temp2327 >= _temp2325.last_plus_one){ _throw( Null_Exception);}* _temp2327;})),
0, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2328= yyvs; struct
_xtunion_struct** _temp2330= _temp2328.curr + yyvsp_offset; if( _temp2330 <
_temp2328.base? 1: _temp2330 >= _temp2328.last_plus_one){ _throw( Null_Exception);}*
_temp2330;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2331=
yyls; struct Cyc_Yyltype* _temp2333= _temp2331.curr +( yylsp_offset - 2); if(
_temp2333 < _temp2331.base? 1: _temp2333 >= _temp2331.last_plus_one){ _throw(
Null_Exception);}* _temp2333;}).first_line,({ struct _tagged_ptr3 _temp2334=
yyls; struct Cyc_Yyltype* _temp2336= _temp2334.curr + yylsp_offset; if(
_temp2336 < _temp2334.base? 1: _temp2336 >= _temp2334.last_plus_one){ _throw(
Null_Exception);}* _temp2336;}).last_line)); _temp2320;}); _temp2319;}); break;
case 15: _LL2318: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2338=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2338[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2339; _temp2339.tag= Cyc_FnDecl_tok_tag;
_temp2339.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp2340= yyvs; struct _xtunion_struct** _temp2342= _temp2340.curr
+( yyvsp_offset - 2); if( _temp2342 < _temp2340.base? 1: _temp2342 >= _temp2340.last_plus_one){
_throw( Null_Exception);}* _temp2342;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2343= yyvs; struct _xtunion_struct** _temp2345= _temp2343.curr
+( yyvsp_offset - 1); if( _temp2345 < _temp2343.base? 1: _temp2345 >= _temp2343.last_plus_one){
_throw( Null_Exception);}* _temp2345;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2346= yyvs; struct _xtunion_struct** _temp2348= _temp2346.curr
+ yyvsp_offset; if( _temp2348 < _temp2346.base? 1: _temp2348 >= _temp2346.last_plus_one){
_throw( Null_Exception);}* _temp2348;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2349= yyls; struct Cyc_Yyltype* _temp2351= _temp2349.curr +(
yylsp_offset - 2); if( _temp2351 < _temp2349.base? 1: _temp2351 >= _temp2349.last_plus_one){
_throw( Null_Exception);}* _temp2351;}).first_line,({ struct _tagged_ptr3
_temp2352= yyls; struct Cyc_Yyltype* _temp2354= _temp2352.curr + yylsp_offset;
if( _temp2354 < _temp2352.base? 1: _temp2354 >= _temp2352.last_plus_one){ _throw(
Null_Exception);}* _temp2354;}).last_line)); _temp2339;}); _temp2338;}); break;
case 16: _LL2337: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2356=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2356[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2357; _temp2357.tag= Cyc_FnDecl_tok_tag;
_temp2357.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2358=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2358->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2359= yyvs; struct
_xtunion_struct** _temp2361= _temp2359.curr +( yyvsp_offset - 3); if( _temp2361
< _temp2359.base? 1: _temp2361 >= _temp2359.last_plus_one){ _throw(
Null_Exception);}* _temp2361;})); _temp2358;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2362= yyvs; struct _xtunion_struct** _temp2364=
_temp2362.curr +( yyvsp_offset - 2); if( _temp2364 < _temp2362.base? 1:
_temp2364 >= _temp2362.last_plus_one){ _throw( Null_Exception);}* _temp2364;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2365= yyvs; struct
_xtunion_struct** _temp2367= _temp2365.curr +( yyvsp_offset - 1); if( _temp2367
< _temp2365.base? 1: _temp2367 >= _temp2365.last_plus_one){ _throw(
Null_Exception);}* _temp2367;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2368= yyvs; struct _xtunion_struct** _temp2370= _temp2368.curr +
yyvsp_offset; if( _temp2370 < _temp2368.base? 1: _temp2370 >= _temp2368.last_plus_one){
_throw( Null_Exception);}* _temp2370;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2371= yyls; struct Cyc_Yyltype* _temp2373= _temp2371.curr +(
yylsp_offset - 3); if( _temp2373 < _temp2371.base? 1: _temp2373 >= _temp2371.last_plus_one){
_throw( Null_Exception);}* _temp2373;}).first_line,({ struct _tagged_ptr3
_temp2374= yyls; struct Cyc_Yyltype* _temp2376= _temp2374.curr + yylsp_offset;
if( _temp2376 < _temp2374.base? 1: _temp2376 >= _temp2374.last_plus_one){ _throw(
Null_Exception);}* _temp2376;}).last_line)); _temp2357;}); _temp2356;}); break;
case 17: _LL2355: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2378=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2378[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2379; _temp2379.tag= Cyc_FnDecl_tok_tag;
_temp2379.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2380=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2380->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2381= yyvs; struct
_xtunion_struct** _temp2383= _temp2381.curr +( yyvsp_offset - 2); if( _temp2383
< _temp2381.base? 1: _temp2383 >= _temp2381.last_plus_one){ _throw(
Null_Exception);}* _temp2383;})); _temp2380;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2384= yyvs; struct _xtunion_struct** _temp2386=
_temp2384.curr +( yyvsp_offset - 1); if( _temp2386 < _temp2384.base? 1:
_temp2386 >= _temp2384.last_plus_one){ _throw( Null_Exception);}* _temp2386;})),
0, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2387= yyvs; struct
_xtunion_struct** _temp2389= _temp2387.curr + yyvsp_offset; if( _temp2389 <
_temp2387.base? 1: _temp2389 >= _temp2387.last_plus_one){ _throw( Null_Exception);}*
_temp2389;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2390=
yyls; struct Cyc_Yyltype* _temp2392= _temp2390.curr +( yylsp_offset - 2); if(
_temp2392 < _temp2390.base? 1: _temp2392 >= _temp2390.last_plus_one){ _throw(
Null_Exception);}* _temp2392;}).first_line,({ struct _tagged_ptr3 _temp2393=
yyls; struct Cyc_Yyltype* _temp2395= _temp2393.curr + yylsp_offset; if(
_temp2395 < _temp2393.base? 1: _temp2395 >= _temp2393.last_plus_one){ _throw(
Null_Exception);}* _temp2395;}).last_line)); _temp2379;}); _temp2378;}); break;
case 18: _LL2377: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2397=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2397[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2398; _temp2398.tag= Cyc_FnDecl_tok_tag;
_temp2398.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2399=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2399->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2400= yyvs; struct
_xtunion_struct** _temp2402= _temp2400.curr +( yyvsp_offset - 3); if( _temp2402
< _temp2400.base? 1: _temp2402 >= _temp2400.last_plus_one){ _throw(
Null_Exception);}* _temp2402;})); _temp2399;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2403= yyvs; struct _xtunion_struct** _temp2405=
_temp2403.curr +( yyvsp_offset - 2); if( _temp2405 < _temp2403.base? 1:
_temp2405 >= _temp2403.last_plus_one){ _throw( Null_Exception);}* _temp2405;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2406= yyvs; struct
_xtunion_struct** _temp2408= _temp2406.curr +( yyvsp_offset - 1); if( _temp2408
< _temp2406.base? 1: _temp2408 >= _temp2406.last_plus_one){ _throw(
Null_Exception);}* _temp2408;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2409= yyvs; struct _xtunion_struct** _temp2411= _temp2409.curr +
yyvsp_offset; if( _temp2411 < _temp2409.base? 1: _temp2411 >= _temp2409.last_plus_one){
_throw( Null_Exception);}* _temp2411;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2412= yyls; struct Cyc_Yyltype* _temp2414= _temp2412.curr +(
yylsp_offset - 3); if( _temp2414 < _temp2412.base? 1: _temp2414 >= _temp2412.last_plus_one){
_throw( Null_Exception);}* _temp2414;}).first_line,({ struct _tagged_ptr3
_temp2415= yyls; struct Cyc_Yyltype* _temp2417= _temp2415.curr + yylsp_offset;
if( _temp2417 < _temp2415.base? 1: _temp2417 >= _temp2415.last_plus_one){ _throw(
Null_Exception);}* _temp2417;}).last_line)); _temp2398;}); _temp2397;}); break;
case 19: _LL2396: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp2419= yyvs; struct _xtunion_struct** _temp2421= _temp2419.curr
+ yyvsp_offset; if( _temp2421 < _temp2419.base? 1: _temp2421 >= _temp2419.last_plus_one){
_throw( Null_Exception);}* _temp2421;}))); yyval=({ struct _tagged_ptr2
_temp2422= yyvs; struct _xtunion_struct** _temp2424= _temp2422.curr +
yyvsp_offset; if( _temp2424 < _temp2422.base? 1: _temp2424 >= _temp2422.last_plus_one){
_throw( Null_Exception);}* _temp2424;}); break; case 20: _LL2418: Cyc_Lex_leave_using();
break; case 21: _LL2425: Cyc_Lex_enter_namespace(({ struct _tagged_string*
_temp2427=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2427[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2428= yyvs;
struct _xtunion_struct** _temp2430= _temp2428.curr + yyvsp_offset; if( _temp2430
< _temp2428.base? 1: _temp2430 >= _temp2428.last_plus_one){ _throw(
Null_Exception);}* _temp2430;})); _temp2427;})); yyval=({ struct _tagged_ptr2
_temp2431= yyvs; struct _xtunion_struct** _temp2433= _temp2431.curr +
yyvsp_offset; if( _temp2433 < _temp2431.base? 1: _temp2433 >= _temp2431.last_plus_one){
_throw( Null_Exception);}* _temp2433;}); break; case 22: _LL2426: Cyc_Lex_leave_namespace();
break; case 23: _LL2434: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2436=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2436[ 0]=({ struct Cyc_DeclList_tok_struct _temp2437; _temp2437.tag= Cyc_DeclList_tok_tag;
_temp2437.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2438= yyvs; struct _xtunion_struct** _temp2440= _temp2438.curr
+( yyvsp_offset - 1); if( _temp2440 < _temp2438.base? 1: _temp2440 >= _temp2438.last_plus_one){
_throw( Null_Exception);}* _temp2440;})), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2441= yyls; struct Cyc_Yyltype* _temp2443= _temp2441.curr
+( yylsp_offset - 1); if( _temp2443 < _temp2441.base? 1: _temp2443 >= _temp2441.last_plus_one){
_throw( Null_Exception);}* _temp2443;}).first_line,({ struct _tagged_ptr3
_temp2444= yyls; struct Cyc_Yyltype* _temp2446= _temp2444.curr +( yylsp_offset -
1); if( _temp2446 < _temp2444.base? 1: _temp2446 >= _temp2444.last_plus_one){
_throw( Null_Exception);}* _temp2446;}).last_line)); _temp2437;}); _temp2436;});
break; case 24: _LL2435: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2448=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2448[ 0]=({ struct Cyc_DeclList_tok_struct _temp2449; _temp2449.tag= Cyc_DeclList_tok_tag;
_temp2449.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2450= yyvs; struct _xtunion_struct** _temp2452= _temp2450.curr
+( yyvsp_offset - 2); if( _temp2452 < _temp2450.base? 1: _temp2452 >= _temp2450.last_plus_one){
_throw( Null_Exception);}* _temp2452;})), Cyc_yyget_InitDeclList_tok(({ struct
_tagged_ptr2 _temp2453= yyvs; struct _xtunion_struct** _temp2455= _temp2453.curr
+( yyvsp_offset - 1); if( _temp2455 < _temp2453.base? 1: _temp2455 >= _temp2453.last_plus_one){
_throw( Null_Exception);}* _temp2455;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2456= yyls; struct Cyc_Yyltype* _temp2458= _temp2456.curr +(
yylsp_offset - 2); if( _temp2458 < _temp2456.base? 1: _temp2458 >= _temp2456.last_plus_one){
_throw( Null_Exception);}* _temp2458;}).first_line,({ struct _tagged_ptr3
_temp2459= yyls; struct Cyc_Yyltype* _temp2461= _temp2459.curr + yylsp_offset;
if( _temp2461 < _temp2459.base? 1: _temp2461 >= _temp2459.last_plus_one){ _throw(
Null_Exception);}* _temp2461;}).last_line)); _temp2449;}); _temp2448;}); break;
case 25: _LL2447: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2463=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2463[ 0]=({ struct Cyc_DeclList_tok_struct _temp2464; _temp2464.tag= Cyc_DeclList_tok_tag;
_temp2464.f1=({ struct Cyc_List_List* _temp2465=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2465->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp2466= yyvs; struct
_xtunion_struct** _temp2468= _temp2466.curr +( yyvsp_offset - 3); if( _temp2468
< _temp2466.base? 1: _temp2468 >= _temp2466.last_plus_one){ _throw(
Null_Exception);}* _temp2468;})), 0, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp2469= yyvs; struct _xtunion_struct** _temp2471= _temp2469.curr +(
yyvsp_offset - 1); if( _temp2471 < _temp2469.base? 1: _temp2471 >= _temp2469.last_plus_one){
_throw( Null_Exception);}* _temp2471;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2472= yyls; struct Cyc_Yyltype* _temp2474= _temp2472.curr +(
yylsp_offset - 4); if( _temp2474 < _temp2472.base? 1: _temp2474 >= _temp2472.last_plus_one){
_throw( Null_Exception);}* _temp2474;}).first_line,({ struct _tagged_ptr3
_temp2475= yyls; struct Cyc_Yyltype* _temp2477= _temp2475.curr + yylsp_offset;
if( _temp2477 < _temp2475.base? 1: _temp2477 >= _temp2475.last_plus_one){ _throw(
Null_Exception);}* _temp2477;}).last_line)); _temp2465->tl= 0; _temp2465;});
_temp2464;}); _temp2463;}); break; case 26: _LL2462: yyval=({ struct
_tagged_ptr2 _temp2479= yyvs; struct _xtunion_struct** _temp2481= _temp2479.curr
+ yyvsp_offset; if( _temp2481 < _temp2479.base? 1: _temp2481 >= _temp2479.last_plus_one){
_throw( Null_Exception);}* _temp2481;}); break; case 27: _LL2478: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2483=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2483[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2484; _temp2484.tag= Cyc_DeclList_tok_tag; _temp2484.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2485= yyvs; struct _xtunion_struct** _temp2487=
_temp2485.curr +( yyvsp_offset - 1); if( _temp2487 < _temp2485.base? 1:
_temp2487 >= _temp2485.last_plus_one){ _throw( Null_Exception);}* _temp2487;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2488= yyvs; struct
_xtunion_struct** _temp2490= _temp2488.curr + yyvsp_offset; if( _temp2490 <
_temp2488.base? 1: _temp2490 >= _temp2488.last_plus_one){ _throw( Null_Exception);}*
_temp2490;}))); _temp2484;}); _temp2483;}); break; case 28: _LL2482: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2492=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2492[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2493; _temp2493.tag= Cyc_DeclSpec_tok_tag;
_temp2493.f1=({ struct Cyc_Parse_Declaration_spec* _temp2494=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2494->sc=({ struct
Cyc_Core_Opt* _temp2498=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2498->v=( void*) Cyc_yyget_StorageClass_tok(({ struct _tagged_ptr2
_temp2499= yyvs; struct _xtunion_struct** _temp2501= _temp2499.curr +(
yyvsp_offset - 1); if( _temp2501 < _temp2499.base? 1: _temp2501 >= _temp2499.last_plus_one){
_throw( Null_Exception);}* _temp2501;})); _temp2498;}); _temp2494->tq= Cyc_Absyn_empty_tqual();
_temp2494->type_specs= 0; _temp2494->is_inline= 0; _temp2494->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2495= yyvs; struct _xtunion_struct** _temp2497=
_temp2495.curr + yyvsp_offset; if( _temp2497 < _temp2495.base? 1: _temp2497 >=
_temp2495.last_plus_one){ _throw( Null_Exception);}* _temp2497;})); _temp2494;});
_temp2493;}); _temp2492;}); break; case 29: _LL2491: if(( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2503= yyvs; struct _xtunion_struct** _temp2505=
_temp2503.curr + yyvsp_offset; if( _temp2505 < _temp2503.base? 1: _temp2505 >=
_temp2503.last_plus_one){ _throw( Null_Exception);}* _temp2505;})))->sc != 0){
Cyc_Parse_warn(( struct _tagged_string)({ char* _temp2506=( char*)"Only one storage class is allowed in a declaration";
struct _tagged_string _temp2507; _temp2507.curr= _temp2506; _temp2507.base=
_temp2506; _temp2507.last_plus_one= _temp2506 + 51; _temp2507;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2508= yyls; struct Cyc_Yyltype* _temp2510= _temp2508.curr
+( yylsp_offset - 2); if( _temp2510 < _temp2508.base? 1: _temp2510 >= _temp2508.last_plus_one){
_throw( Null_Exception);}* _temp2510;}).first_line,({ struct _tagged_ptr3
_temp2511= yyls; struct Cyc_Yyltype* _temp2513= _temp2511.curr +( yylsp_offset -
1); if( _temp2513 < _temp2511.base? 1: _temp2513 >= _temp2511.last_plus_one){
_throw( Null_Exception);}* _temp2513;}).last_line));} yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2514=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2514[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2515; _temp2515.tag= Cyc_DeclSpec_tok_tag; _temp2515.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2516=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2516->sc=({ struct Cyc_Core_Opt* _temp2532=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2532->v=( void*) Cyc_yyget_StorageClass_tok(({
struct _tagged_ptr2 _temp2533= yyvs; struct _xtunion_struct** _temp2535=
_temp2533.curr +( yyvsp_offset - 2); if( _temp2535 < _temp2533.base? 1:
_temp2535 >= _temp2533.last_plus_one){ _throw( Null_Exception);}* _temp2535;}));
_temp2532;}); _temp2516->tq=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2529= yyvs; struct _xtunion_struct** _temp2531= _temp2529.curr +
yyvsp_offset; if( _temp2531 < _temp2529.base? 1: _temp2531 >= _temp2529.last_plus_one){
_throw( Null_Exception);}* _temp2531;})))->tq; _temp2516->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2526= yyvs; struct _xtunion_struct** _temp2528=
_temp2526.curr + yyvsp_offset; if( _temp2528 < _temp2526.base? 1: _temp2528 >=
_temp2526.last_plus_one){ _throw( Null_Exception);}* _temp2528;})))->type_specs;
_temp2516->is_inline=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2523=
yyvs; struct _xtunion_struct** _temp2525= _temp2523.curr + yyvsp_offset; if(
_temp2525 < _temp2523.base? 1: _temp2525 >= _temp2523.last_plus_one){ _throw(
Null_Exception);}* _temp2525;})))->is_inline; _temp2516->attributes=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2517= yyvs; struct
_xtunion_struct** _temp2519= _temp2517.curr +( yyvsp_offset - 1); if( _temp2519
< _temp2517.base? 1: _temp2519 >= _temp2517.last_plus_one){ _throw(
Null_Exception);}* _temp2519;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2520= yyvs; struct _xtunion_struct** _temp2522= _temp2520.curr +
yyvsp_offset; if( _temp2522 < _temp2520.base? 1: _temp2522 >= _temp2520.last_plus_one){
_throw( Null_Exception);}* _temp2522;})))->attributes); _temp2516;}); _temp2515;});
_temp2514;}); break; case 30: _LL2502: yyval=( struct _xtunion_struct*)({ struct
Cyc_DeclSpec_tok_struct* _temp2537=( struct Cyc_DeclSpec_tok_struct*) GC_malloc(
sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2537[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2538; _temp2538.tag= Cyc_DeclSpec_tok_tag; _temp2538.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2539=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2539->sc= 0; _temp2539->tq= Cyc_Absyn_empty_tqual(); _temp2539->type_specs=({
struct Cyc_List_List* _temp2543=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2543->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp2544= yyvs; struct _xtunion_struct** _temp2546=
_temp2544.curr +( yyvsp_offset - 1); if( _temp2546 < _temp2544.base? 1:
_temp2546 >= _temp2544.last_plus_one){ _throw( Null_Exception);}* _temp2546;}));
_temp2543->tl= 0; _temp2543;}); _temp2539->is_inline= 0; _temp2539->attributes=
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2540= yyvs; struct
_xtunion_struct** _temp2542= _temp2540.curr + yyvsp_offset; if( _temp2542 <
_temp2540.base? 1: _temp2542 >= _temp2540.last_plus_one){ _throw( Null_Exception);}*
_temp2542;})); _temp2539;}); _temp2538;}); _temp2537;}); break; case 31: _LL2536:
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2548=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2548[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2549; _temp2549.tag= Cyc_DeclSpec_tok_tag;
_temp2549.f1=({ struct Cyc_Parse_Declaration_spec* _temp2550=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2550->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2570= yyvs; struct _xtunion_struct** _temp2572=
_temp2570.curr + yyvsp_offset; if( _temp2572 < _temp2570.base? 1: _temp2572 >=
_temp2570.last_plus_one){ _throw( Null_Exception);}* _temp2572;})))->sc;
_temp2550->tq=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2567= yyvs;
struct _xtunion_struct** _temp2569= _temp2567.curr + yyvsp_offset; if( _temp2569
< _temp2567.base? 1: _temp2569 >= _temp2567.last_plus_one){ _throw(
Null_Exception);}* _temp2569;})))->tq; _temp2550->type_specs=({ struct Cyc_List_List*
_temp2560=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2560->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2564= yyvs; struct _xtunion_struct** _temp2566= _temp2564.curr +(
yyvsp_offset - 2); if( _temp2566 < _temp2564.base? 1: _temp2566 >= _temp2564.last_plus_one){
_throw( Null_Exception);}* _temp2566;})); _temp2560->tl=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2561= yyvs; struct _xtunion_struct** _temp2563=
_temp2561.curr + yyvsp_offset; if( _temp2563 < _temp2561.base? 1: _temp2563 >=
_temp2561.last_plus_one){ _throw( Null_Exception);}* _temp2563;})))->type_specs;
_temp2560;}); _temp2550->is_inline=( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2557= yyvs; struct _xtunion_struct** _temp2559= _temp2557.curr
+ yyvsp_offset; if( _temp2559 < _temp2557.base? 1: _temp2559 >= _temp2557.last_plus_one){
_throw( Null_Exception);}* _temp2559;})))->is_inline; _temp2550->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2551= yyvs; struct
_xtunion_struct** _temp2553= _temp2551.curr +( yyvsp_offset - 1); if( _temp2553
< _temp2551.base? 1: _temp2553 >= _temp2551.last_plus_one){ _throw(
Null_Exception);}* _temp2553;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2554= yyvs; struct _xtunion_struct** _temp2556= _temp2554.curr +
yyvsp_offset; if( _temp2556 < _temp2554.base? 1: _temp2556 >= _temp2554.last_plus_one){
_throw( Null_Exception);}* _temp2556;})))->attributes); _temp2550;}); _temp2549;});
_temp2548;}); break; case 32: _LL2547: yyval=( struct _xtunion_struct*)({ struct
Cyc_DeclSpec_tok_struct* _temp2574=( struct Cyc_DeclSpec_tok_struct*) GC_malloc(
sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2574[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2575; _temp2575.tag= Cyc_DeclSpec_tok_tag; _temp2575.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2576=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2576->sc= 0; _temp2576->tq= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2
_temp2580= yyvs; struct _xtunion_struct** _temp2582= _temp2580.curr +(
yyvsp_offset - 1); if( _temp2582 < _temp2580.base? 1: _temp2582 >= _temp2580.last_plus_one){
_throw( Null_Exception);}* _temp2582;})); _temp2576->type_specs= 0; _temp2576->is_inline=
0; _temp2576->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2577= yyvs; struct _xtunion_struct** _temp2579= _temp2577.curr +
yyvsp_offset; if( _temp2579 < _temp2577.base? 1: _temp2579 >= _temp2577.last_plus_one){
_throw( Null_Exception);}* _temp2579;})); _temp2576;}); _temp2575;}); _temp2574;});
break; case 33: _LL2573: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct*
_temp2584=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2584[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2585; _temp2585.tag= Cyc_DeclSpec_tok_tag;
_temp2585.f1=({ struct Cyc_Parse_Declaration_spec* _temp2586=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2586->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2605= yyvs; struct _xtunion_struct** _temp2607=
_temp2605.curr + yyvsp_offset; if( _temp2607 < _temp2605.base? 1: _temp2607 >=
_temp2605.last_plus_one){ _throw( Null_Exception);}* _temp2607;})))->sc;
_temp2586->tq= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp2599= yyvs; struct _xtunion_struct** _temp2601= _temp2599.curr
+( yyvsp_offset - 2); if( _temp2601 < _temp2599.base? 1: _temp2601 >= _temp2599.last_plus_one){
_throw( Null_Exception);}* _temp2601;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2602= yyvs; struct _xtunion_struct** _temp2604= _temp2602.curr
+ yyvsp_offset; if( _temp2604 < _temp2602.base? 1: _temp2604 >= _temp2602.last_plus_one){
_throw( Null_Exception);}* _temp2604;})))->tq); _temp2586->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2596= yyvs; struct _xtunion_struct** _temp2598=
_temp2596.curr + yyvsp_offset; if( _temp2598 < _temp2596.base? 1: _temp2598 >=
_temp2596.last_plus_one){ _throw( Null_Exception);}* _temp2598;})))->type_specs;
_temp2586->is_inline=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2593=
yyvs; struct _xtunion_struct** _temp2595= _temp2593.curr + yyvsp_offset; if(
_temp2595 < _temp2593.base? 1: _temp2595 >= _temp2593.last_plus_one){ _throw(
Null_Exception);}* _temp2595;})))->is_inline; _temp2586->attributes=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2587= yyvs; struct
_xtunion_struct** _temp2589= _temp2587.curr +( yyvsp_offset - 1); if( _temp2589
< _temp2587.base? 1: _temp2589 >= _temp2587.last_plus_one){ _throw(
Null_Exception);}* _temp2589;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2590= yyvs; struct _xtunion_struct** _temp2592= _temp2590.curr +
yyvsp_offset; if( _temp2592 < _temp2590.base? 1: _temp2592 >= _temp2590.last_plus_one){
_throw( Null_Exception);}* _temp2592;})))->attributes); _temp2586;}); _temp2585;});
_temp2584;}); break; case 34: _LL2583: yyval=( struct _xtunion_struct*)({ struct
Cyc_DeclSpec_tok_struct* _temp2609=( struct Cyc_DeclSpec_tok_struct*) GC_malloc(
sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2609[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2610; _temp2610.tag= Cyc_DeclSpec_tok_tag; _temp2610.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2611=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2611->sc= 0; _temp2611->tq= Cyc_Absyn_empty_tqual(); _temp2611->type_specs=
0; _temp2611->is_inline= 1; _temp2611->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2612= yyvs; struct _xtunion_struct** _temp2614=
_temp2612.curr + yyvsp_offset; if( _temp2614 < _temp2612.base? 1: _temp2614 >=
_temp2612.last_plus_one){ _throw( Null_Exception);}* _temp2614;})); _temp2611;});
_temp2610;}); _temp2609;}); break; case 35: _LL2608: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2616=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2616[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2617; _temp2617.tag= Cyc_DeclSpec_tok_tag; _temp2617.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2618=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2618->sc=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2631= yyvs;
struct _xtunion_struct** _temp2633= _temp2631.curr + yyvsp_offset; if( _temp2633
< _temp2631.base? 1: _temp2633 >= _temp2631.last_plus_one){ _throw(
Null_Exception);}* _temp2633;})))->sc; _temp2618->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2628= yyvs; struct _xtunion_struct** _temp2630=
_temp2628.curr + yyvsp_offset; if( _temp2630 < _temp2628.base? 1: _temp2630 >=
_temp2628.last_plus_one){ _throw( Null_Exception);}* _temp2630;})))->tq;
_temp2618->type_specs=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2625=
yyvs; struct _xtunion_struct** _temp2627= _temp2625.curr + yyvsp_offset; if(
_temp2627 < _temp2625.base? 1: _temp2627 >= _temp2625.last_plus_one){ _throw(
Null_Exception);}* _temp2627;})))->type_specs; _temp2618->is_inline= 1;
_temp2618->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2619= yyvs; struct _xtunion_struct** _temp2621=
_temp2619.curr +( yyvsp_offset - 1); if( _temp2621 < _temp2619.base? 1:
_temp2621 >= _temp2619.last_plus_one){ _throw( Null_Exception);}* _temp2621;})),(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2622= yyvs; struct
_xtunion_struct** _temp2624= _temp2622.curr + yyvsp_offset; if( _temp2624 <
_temp2622.base? 1: _temp2624 >= _temp2622.last_plus_one){ _throw( Null_Exception);}*
_temp2624;})))->attributes); _temp2618;}); _temp2617;}); _temp2616;}); break;
case 36: _LL2615: yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct*
_temp2635=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2635[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2636; _temp2636.tag=
Cyc_StorageClass_tok_tag; _temp2636.f1=( void*)(( void*) Cyc_Parse_Auto_sc);
_temp2636;}); _temp2635;}); break; case 37: _LL2634: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2638=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2638[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2639; _temp2639.tag= Cyc_StorageClass_tok_tag;
_temp2639.f1=( void*)(( void*) Cyc_Parse_Register_sc); _temp2639;}); _temp2638;});
break; case 38: _LL2637: yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct*
_temp2641=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2641[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2642; _temp2642.tag=
Cyc_StorageClass_tok_tag; _temp2642.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp2642;}); _temp2641;}); break; case 39: _LL2640: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2644=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2644[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2645; _temp2645.tag= Cyc_StorageClass_tok_tag;
_temp2645.f1=( void*)(( void*) Cyc_Parse_Extern_sc); _temp2645;}); _temp2644;});
break; case 40: _LL2643: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2647= yyvs; struct _xtunion_struct** _temp2649= _temp2647.curr
+ yyvsp_offset; if( _temp2649 < _temp2647.base? 1: _temp2649 >= _temp2647.last_plus_one){
_throw( Null_Exception);}* _temp2649;})),( struct _tagged_string)({ char*
_temp2650=( char*)"C"; struct _tagged_string _temp2651; _temp2651.curr=
_temp2650; _temp2651.base= _temp2650; _temp2651.last_plus_one= _temp2650 + 2;
_temp2651;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2652=(
char*)"only extern or extern \"C\" is allowed"; struct _tagged_string _temp2653;
_temp2653.curr= _temp2652; _temp2653.base= _temp2652; _temp2653.last_plus_one=
_temp2652 + 37; _temp2653;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2654= yyls; struct Cyc_Yyltype* _temp2656= _temp2654.curr +( yylsp_offset -
1); if( _temp2656 < _temp2654.base? 1: _temp2656 >= _temp2654.last_plus_one){
_throw( Null_Exception);}* _temp2656;}).first_line,({ struct _tagged_ptr3
_temp2657= yyls; struct Cyc_Yyltype* _temp2659= _temp2657.curr + yylsp_offset;
if( _temp2659 < _temp2657.base? 1: _temp2659 >= _temp2657.last_plus_one){ _throw(
Null_Exception);}* _temp2659;}).last_line));} yyval=( struct _xtunion_struct*)({
struct Cyc_StorageClass_tok_struct* _temp2660=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2660[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2661; _temp2661.tag= Cyc_StorageClass_tok_tag;
_temp2661.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp2661;}); _temp2660;});
break; case 41: _LL2646: yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct*
_temp2663=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2663[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2664; _temp2664.tag=
Cyc_StorageClass_tok_tag; _temp2664.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp2664;}); _temp2663;}); break; case 42: _LL2662: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2666=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2666[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2667; _temp2667.tag= Cyc_StorageClass_tok_tag;
_temp2667.f1=( void*)(( void*) Cyc_Parse_Abstract_sc); _temp2667;}); _temp2666;});
break; case 43: _LL2665: yyval=( struct _xtunion_struct*)({ struct Cyc_AttributeList_tok_struct*
_temp2669=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2669[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2670; _temp2670.tag=
Cyc_AttributeList_tok_tag; _temp2670.f1= 0; _temp2670;}); _temp2669;}); break;
case 44: _LL2668: yyval=({ struct _tagged_ptr2 _temp2672= yyvs; struct
_xtunion_struct** _temp2674= _temp2672.curr + yyvsp_offset; if( _temp2674 <
_temp2672.base? 1: _temp2674 >= _temp2672.last_plus_one){ _throw( Null_Exception);}*
_temp2674;}); break; case 45: _LL2671: yyval=( struct _xtunion_struct*)({ struct
Cyc_AttributeList_tok_struct* _temp2676=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2676[ 0]=({
struct Cyc_AttributeList_tok_struct _temp2677; _temp2677.tag= Cyc_AttributeList_tok_tag;
_temp2677.f1= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2678= yyvs;
struct _xtunion_struct** _temp2680= _temp2678.curr +( yyvsp_offset - 2); if(
_temp2680 < _temp2678.base? 1: _temp2680 >= _temp2678.last_plus_one){ _throw(
Null_Exception);}* _temp2680;})); _temp2677;}); _temp2676;}); break; case 46:
_LL2675: yyval=( struct _xtunion_struct*)({ struct Cyc_AttributeList_tok_struct*
_temp2682=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2682[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2683; _temp2683.tag=
Cyc_AttributeList_tok_tag; _temp2683.f1=({ struct Cyc_List_List* _temp2684=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2684->hd=(
void*) Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2685= yyvs; struct
_xtunion_struct** _temp2687= _temp2685.curr + yyvsp_offset; if( _temp2687 <
_temp2685.base? 1: _temp2687 >= _temp2685.last_plus_one){ _throw( Null_Exception);}*
_temp2687;})); _temp2684->tl= 0; _temp2684;}); _temp2683;}); _temp2682;});
break; case 47: _LL2681: yyval=( struct _xtunion_struct*)({ struct Cyc_AttributeList_tok_struct*
_temp2689=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2689[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2690; _temp2690.tag=
Cyc_AttributeList_tok_tag; _temp2690.f1=({ struct Cyc_List_List* _temp2691=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2691->hd=(
void*) Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2695= yyvs; struct
_xtunion_struct** _temp2697= _temp2695.curr +( yyvsp_offset - 2); if( _temp2697
< _temp2695.base? 1: _temp2697 >= _temp2695.last_plus_one){ _throw(
Null_Exception);}* _temp2697;})); _temp2691->tl= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2692= yyvs; struct _xtunion_struct** _temp2694=
_temp2692.curr + yyvsp_offset; if( _temp2694 < _temp2692.base? 1: _temp2694 >=
_temp2692.last_plus_one){ _throw( Null_Exception);}* _temp2694;})); _temp2691;});
_temp2690;}); _temp2689;}); break; case 48: _LL2688: { struct _tagged_string s=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2699= yyvs; struct
_xtunion_struct** _temp2701= _temp2699.curr + yyvsp_offset; if( _temp2701 <
_temp2699.base? 1: _temp2701 >= _temp2699.last_plus_one){ _throw( Null_Exception);}*
_temp2701;})); void* a; if( Cyc_String_zstrcmp( s,( struct _tagged_string)({
char* _temp2702=( char*)"stdcall"; struct _tagged_string _temp2703; _temp2703.curr=
_temp2702; _temp2703.base= _temp2702; _temp2703.last_plus_one= _temp2702 + 8;
_temp2703;})) == 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2704=( char*)"__stdcall__"; struct _tagged_string _temp2705; _temp2705.curr=
_temp2704; _temp2705.base= _temp2704; _temp2705.last_plus_one= _temp2704 + 12;
_temp2705;})) == 0){ a=( void*) Cyc_Absyn_Stdcall_att;} else{ if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2706=( char*)"cdecl"; struct
_tagged_string _temp2707; _temp2707.curr= _temp2706; _temp2707.base= _temp2706;
_temp2707.last_plus_one= _temp2706 + 6; _temp2707;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2708=( char*)"__cdecl__"; struct
_tagged_string _temp2709; _temp2709.curr= _temp2708; _temp2709.base= _temp2708;
_temp2709.last_plus_one= _temp2708 + 10; _temp2709;})) == 0){ a=( void*) Cyc_Absyn_Cdecl_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2710=(
char*)"noreturn"; struct _tagged_string _temp2711; _temp2711.curr= _temp2710;
_temp2711.base= _temp2710; _temp2711.last_plus_one= _temp2710 + 9; _temp2711;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2712=( char*)"__noreturn__";
struct _tagged_string _temp2713; _temp2713.curr= _temp2712; _temp2713.base=
_temp2712; _temp2713.last_plus_one= _temp2712 + 13; _temp2713;})) == 0){ a=(
void*) Cyc_Absyn_Noreturn_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2714=( char*)"noreturn"; struct _tagged_string
_temp2715; _temp2715.curr= _temp2714; _temp2715.base= _temp2714; _temp2715.last_plus_one=
_temp2714 + 9; _temp2715;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2716=( char*)"__noreturn__"; struct _tagged_string
_temp2717; _temp2717.curr= _temp2716; _temp2717.base= _temp2716; _temp2717.last_plus_one=
_temp2716 + 13; _temp2717;})) == 0){ a=( void*) Cyc_Absyn_Noreturn_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2718=( char*)"__const__";
struct _tagged_string _temp2719; _temp2719.curr= _temp2718; _temp2719.base=
_temp2718; _temp2719.last_plus_one= _temp2718 + 10; _temp2719;})) == 0){ a=(
void*) Cyc_Absyn_Const_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2720=( char*)"aligned"; struct _tagged_string
_temp2721; _temp2721.curr= _temp2720; _temp2721.base= _temp2720; _temp2721.last_plus_one=
_temp2720 + 8; _temp2721;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2722=( char*)"__aligned__"; struct _tagged_string
_temp2723; _temp2723.curr= _temp2722; _temp2723.base= _temp2722; _temp2723.last_plus_one=
_temp2722 + 12; _temp2723;})) == 0){ a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp2724=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct));
_temp2724[ 0]=({ struct Cyc_Absyn_Aligned_att_struct _temp2725; _temp2725.tag=
Cyc_Absyn_Aligned_att_tag; _temp2725.f1= - 1; _temp2725;}); _temp2724;});} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2726=( char*)"packed";
struct _tagged_string _temp2727; _temp2727.curr= _temp2726; _temp2727.base=
_temp2726; _temp2727.last_plus_one= _temp2726 + 7; _temp2727;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2728=( char*)"__packed__"; struct
_tagged_string _temp2729; _temp2729.curr= _temp2728; _temp2729.base= _temp2728;
_temp2729.last_plus_one= _temp2728 + 11; _temp2729;})) == 0){ a=( void*) Cyc_Absyn_Packed_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2730=(
char*)"shared"; struct _tagged_string _temp2731; _temp2731.curr= _temp2730;
_temp2731.base= _temp2730; _temp2731.last_plus_one= _temp2730 + 7; _temp2731;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2732=( char*)"__shared__";
struct _tagged_string _temp2733; _temp2733.curr= _temp2732; _temp2733.base=
_temp2732; _temp2733.last_plus_one= _temp2732 + 11; _temp2733;})) == 0){ a=(
void*) Cyc_Absyn_Shared_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2734=( char*)"unused"; struct _tagged_string
_temp2735; _temp2735.curr= _temp2734; _temp2735.base= _temp2734; _temp2735.last_plus_one=
_temp2734 + 7; _temp2735;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2736=( char*)"__unused__"; struct _tagged_string
_temp2737; _temp2737.curr= _temp2736; _temp2737.base= _temp2736; _temp2737.last_plus_one=
_temp2736 + 11; _temp2737;})) == 0){ a=( void*) Cyc_Absyn_Unused_att;} else{ if(
Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2738=( char*)"weak";
struct _tagged_string _temp2739; _temp2739.curr= _temp2738; _temp2739.base=
_temp2738; _temp2739.last_plus_one= _temp2738 + 5; _temp2739;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2740=( char*)"__weak__"; struct
_tagged_string _temp2741; _temp2741.curr= _temp2740; _temp2741.base= _temp2740;
_temp2741.last_plus_one= _temp2740 + 9; _temp2741;})) == 0){ a=( void*) Cyc_Absyn_Weak_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2742=(
char*)"dllimport"; struct _tagged_string _temp2743; _temp2743.curr= _temp2742;
_temp2743.base= _temp2742; _temp2743.last_plus_one= _temp2742 + 10; _temp2743;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2744=( char*)"__dllimport__";
struct _tagged_string _temp2745; _temp2745.curr= _temp2744; _temp2745.base=
_temp2744; _temp2745.last_plus_one= _temp2744 + 14; _temp2745;})) == 0){ a=(
void*) Cyc_Absyn_Dllimport_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2746=( char*)"dllexport"; struct _tagged_string
_temp2747; _temp2747.curr= _temp2746; _temp2747.base= _temp2746; _temp2747.last_plus_one=
_temp2746 + 10; _temp2747;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2748=( char*)"__dllexport__"; struct _tagged_string
_temp2749; _temp2749.curr= _temp2748; _temp2749.base= _temp2748; _temp2749.last_plus_one=
_temp2748 + 14; _temp2749;})) == 0){ a=( void*) Cyc_Absyn_Dllexport_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2750=( char*)"no_instrument_function";
struct _tagged_string _temp2751; _temp2751.curr= _temp2750; _temp2751.base=
_temp2750; _temp2751.last_plus_one= _temp2750 + 23; _temp2751;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2752=( char*)"__no_instrument_function__";
struct _tagged_string _temp2753; _temp2753.curr= _temp2752; _temp2753.base=
_temp2752; _temp2753.last_plus_one= _temp2752 + 27; _temp2753;})) == 0){ a=(
void*) Cyc_Absyn_No_instrument_function_att;} else{ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2754=( char*)"constructor"; struct
_tagged_string _temp2755; _temp2755.curr= _temp2754; _temp2755.base= _temp2754;
_temp2755.last_plus_one= _temp2754 + 12; _temp2755;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2756=( char*)"__constructor__"; struct
_tagged_string _temp2757; _temp2757.curr= _temp2756; _temp2757.base= _temp2756;
_temp2757.last_plus_one= _temp2756 + 16; _temp2757;})) == 0){ a=( void*) Cyc_Absyn_Constructor_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2758=(
char*)"destructor"; struct _tagged_string _temp2759; _temp2759.curr= _temp2758;
_temp2759.base= _temp2758; _temp2759.last_plus_one= _temp2758 + 11; _temp2759;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2760=( char*)"__destructor__";
struct _tagged_string _temp2761; _temp2761.curr= _temp2760; _temp2761.base=
_temp2760; _temp2761.last_plus_one= _temp2760 + 15; _temp2761;})) == 0){ a=(
void*) Cyc_Absyn_Destructor_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2762=( char*)"no_check_memory_usage"; struct
_tagged_string _temp2763; _temp2763.curr= _temp2762; _temp2763.base= _temp2762;
_temp2763.last_plus_one= _temp2762 + 22; _temp2763;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2764=( char*)"__no_check_memory_usage__";
struct _tagged_string _temp2765; _temp2765.curr= _temp2764; _temp2765.base=
_temp2764; _temp2765.last_plus_one= _temp2764 + 26; _temp2765;})) == 0){ a=(
void*) Cyc_Absyn_No_check_memory_usage_att;} else{ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2766=( char*)"unrecognized attribute"; struct
_tagged_string _temp2767; _temp2767.curr= _temp2766; _temp2767.base= _temp2766;
_temp2767.last_plus_one= _temp2766 + 23; _temp2767;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2768= yyls; struct Cyc_Yyltype* _temp2770= _temp2768.curr
+ yylsp_offset; if( _temp2770 < _temp2768.base? 1: _temp2770 >= _temp2768.last_plus_one){
_throw( Null_Exception);}* _temp2770;}).first_line,({ struct _tagged_ptr3
_temp2771= yyls; struct Cyc_Yyltype* _temp2773= _temp2771.curr + yylsp_offset;
if( _temp2773 < _temp2771.base? 1: _temp2773 >= _temp2771.last_plus_one){ _throw(
Null_Exception);}* _temp2773;}).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;}}}}}}}}}}}}}}}}
yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct* _temp2774=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2774[ 0]=({ struct Cyc_Attribute_tok_struct _temp2775; _temp2775.tag= Cyc_Attribute_tok_tag;
_temp2775.f1=( void*) a; _temp2775;}); _temp2774;}); break;} case 49: _LL2698:
yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct* _temp2777=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2777[ 0]=({ struct Cyc_Attribute_tok_struct _temp2778; _temp2778.tag= Cyc_Attribute_tok_tag;
_temp2778.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp2778;}); _temp2777;});
break; case 50: _LL2776: { struct _tagged_string s= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2780= yyvs; struct _xtunion_struct** _temp2782=
_temp2780.curr +( yyvsp_offset - 3); if( _temp2782 < _temp2780.base? 1:
_temp2782 >= _temp2780.last_plus_one){ _throw( Null_Exception);}* _temp2782;}));
struct _tuple12 _temp2788; int _temp2789; void* _temp2791; struct _tuple12*
_temp2786= Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp2783= yyvs; struct
_xtunion_struct** _temp2785= _temp2783.curr +( yyvsp_offset - 1); if( _temp2785
< _temp2783.base? 1: _temp2785 >= _temp2783.last_plus_one){ _throw(
Null_Exception);}* _temp2785;})); _temp2788=* _temp2786; _LL2792: _temp2791=
_temp2788.f1; goto _LL2790; _LL2790: _temp2789= _temp2788.f2; goto _LL2787;
_LL2787: { void* a; if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2793=( char*)"regparm"; struct _tagged_string _temp2794; _temp2794.curr=
_temp2793; _temp2794.base= _temp2793; _temp2794.last_plus_one= _temp2793 + 8;
_temp2794;})) == 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2795=( char*)"__regparm__"; struct _tagged_string _temp2796; _temp2796.curr=
_temp2795; _temp2796.base= _temp2795; _temp2796.last_plus_one= _temp2795 + 12;
_temp2796;})) == 0){ if( _temp2789 <= 0? 1: _temp2789 > 3){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2797=( char*)"regparm requires value between 1 and 3";
struct _tagged_string _temp2798; _temp2798.curr= _temp2797; _temp2798.base=
_temp2797; _temp2798.last_plus_one= _temp2797 + 39; _temp2798;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2799= yyls; struct Cyc_Yyltype* _temp2801= _temp2799.curr
+( yylsp_offset - 1); if( _temp2801 < _temp2799.base? 1: _temp2801 >= _temp2799.last_plus_one){
_throw( Null_Exception);}* _temp2801;}).first_line,({ struct _tagged_ptr3
_temp2802= yyls; struct Cyc_Yyltype* _temp2804= _temp2802.curr +( yylsp_offset -
1); if( _temp2804 < _temp2802.base? 1: _temp2804 >= _temp2802.last_plus_one){
_throw( Null_Exception);}* _temp2804;}).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp2805=( struct Cyc_Absyn_Regparm_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Regparm_att_struct));
_temp2805[ 0]=({ struct Cyc_Absyn_Regparm_att_struct _temp2806; _temp2806.tag=
Cyc_Absyn_Regparm_att_tag; _temp2806.f1= _temp2789; _temp2806;}); _temp2805;});}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2807=(
char*)"aligned"; struct _tagged_string _temp2808; _temp2808.curr= _temp2807;
_temp2808.base= _temp2807; _temp2808.last_plus_one= _temp2807 + 8; _temp2808;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2809=( char*)"__aligned__";
struct _tagged_string _temp2810; _temp2810.curr= _temp2809; _temp2810.base=
_temp2809; _temp2810.last_plus_one= _temp2809 + 12; _temp2810;})) == 0){ if(
_temp2789 < 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2811=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2812; _temp2812.curr= _temp2811; _temp2812.base=
_temp2811; _temp2812.last_plus_one= _temp2811 + 39; _temp2812;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2813= yyls; struct Cyc_Yyltype* _temp2815= _temp2813.curr
+( yylsp_offset - 1); if( _temp2815 < _temp2813.base? 1: _temp2815 >= _temp2813.last_plus_one){
_throw( Null_Exception);}* _temp2815;}).first_line,({ struct _tagged_ptr3
_temp2816= yyls; struct Cyc_Yyltype* _temp2818= _temp2816.curr +( yylsp_offset -
1); if( _temp2818 < _temp2816.base? 1: _temp2818 >= _temp2816.last_plus_one){
_throw( Null_Exception);}* _temp2818;}).last_line));}{ unsigned int j=(
unsigned int) _temp2789; for( 0;( j &( unsigned int) 1) ==( unsigned int) 0; j=
j >>( unsigned int) 1){;} j= j >>( unsigned int) 1; if( j !=( unsigned int) 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2819=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2820; _temp2820.curr= _temp2819; _temp2820.base=
_temp2819; _temp2820.last_plus_one= _temp2819 + 39; _temp2820;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2821= yyls; struct Cyc_Yyltype* _temp2823= _temp2821.curr
+( yylsp_offset - 1); if( _temp2823 < _temp2821.base? 1: _temp2823 >= _temp2821.last_plus_one){
_throw( Null_Exception);}* _temp2823;}).first_line,({ struct _tagged_ptr3
_temp2824= yyls; struct Cyc_Yyltype* _temp2826= _temp2824.curr +( yylsp_offset -
1); if( _temp2826 < _temp2824.base? 1: _temp2826 >= _temp2824.last_plus_one){
_throw( Null_Exception);}* _temp2826;}).last_line));} a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp2827=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct));
_temp2827[ 0]=({ struct Cyc_Absyn_Aligned_att_struct _temp2828; _temp2828.tag=
Cyc_Absyn_Aligned_att_tag; _temp2828.f1= _temp2789; _temp2828;}); _temp2827;});}}
else{ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2829=( char*)"unrecognized attribute";
struct _tagged_string _temp2830; _temp2830.curr= _temp2829; _temp2830.base=
_temp2829; _temp2830.last_plus_one= _temp2829 + 23; _temp2830;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2831= yyls; struct Cyc_Yyltype* _temp2833= _temp2831.curr
+( yylsp_offset - 3); if( _temp2833 < _temp2831.base? 1: _temp2833 >= _temp2831.last_plus_one){
_throw( Null_Exception);}* _temp2833;}).first_line,({ struct _tagged_ptr3
_temp2834= yyls; struct Cyc_Yyltype* _temp2836= _temp2834.curr +( yylsp_offset -
3); if( _temp2836 < _temp2834.base? 1: _temp2836 >= _temp2834.last_plus_one){
_throw( Null_Exception);}* _temp2836;}).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;}}
yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct* _temp2837=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2837[ 0]=({ struct Cyc_Attribute_tok_struct _temp2838; _temp2838.tag= Cyc_Attribute_tok_tag;
_temp2838.f1=( void*) a; _temp2838;}); _temp2837;}); break;}} case 51: _LL2779: {
struct _tagged_string s= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2840=
yyvs; struct _xtunion_struct** _temp2842= _temp2840.curr +( yyvsp_offset - 3);
if( _temp2842 < _temp2840.base? 1: _temp2842 >= _temp2840.last_plus_one){ _throw(
Null_Exception);}* _temp2842;})); struct _tagged_string str= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2843= yyvs; struct _xtunion_struct** _temp2845=
_temp2843.curr +( yyvsp_offset - 1); if( _temp2845 < _temp2843.base? 1:
_temp2845 >= _temp2843.last_plus_one){ _throw( Null_Exception);}* _temp2845;}));
void* a; if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2846=(
char*)"section"; struct _tagged_string _temp2847; _temp2847.curr= _temp2846;
_temp2847.base= _temp2846; _temp2847.last_plus_one= _temp2846 + 8; _temp2847;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2848=( char*)"__section__";
struct _tagged_string _temp2849; _temp2849.curr= _temp2848; _temp2849.base=
_temp2848; _temp2849.last_plus_one= _temp2848 + 12; _temp2849;}))){ a=( void*)({
struct Cyc_Absyn_Section_att_struct* _temp2850=( struct Cyc_Absyn_Section_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct)); _temp2850[ 0]=({
struct Cyc_Absyn_Section_att_struct _temp2851; _temp2851.tag= Cyc_Absyn_Section_att_tag;
_temp2851.f1= str; _temp2851;}); _temp2850;});} else{ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2852=( char*)"unrecognized attribute"; struct
_tagged_string _temp2853; _temp2853.curr= _temp2852; _temp2853.base= _temp2852;
_temp2853.last_plus_one= _temp2852 + 23; _temp2853;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2854= yyls; struct Cyc_Yyltype* _temp2856= _temp2854.curr
+( yylsp_offset - 3); if( _temp2856 < _temp2854.base? 1: _temp2856 >= _temp2854.last_plus_one){
_throw( Null_Exception);}* _temp2856;}).first_line,({ struct _tagged_ptr3
_temp2857= yyls; struct Cyc_Yyltype* _temp2859= _temp2857.curr +( yylsp_offset -
3); if( _temp2859 < _temp2857.base? 1: _temp2859 >= _temp2857.last_plus_one){
_throw( Null_Exception);}* _temp2859;}).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;}
yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct* _temp2860=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2860[ 0]=({ struct Cyc_Attribute_tok_struct _temp2861; _temp2861.tag= Cyc_Attribute_tok_tag;
_temp2861.f1=( void*) a; _temp2861;}); _temp2860;}); break;} case 52: _LL2839:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2863=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2863[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2864; _temp2864.tag=
Cyc_TypeSpecifier_tok_tag; _temp2864.f1=( void*) Cyc_Parse_type_spec(( void*)
Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2865=
yyls; struct Cyc_Yyltype* _temp2867= _temp2865.curr + yylsp_offset; if(
_temp2867 < _temp2865.base? 1: _temp2867 >= _temp2865.last_plus_one){ _throw(
Null_Exception);}* _temp2867;}).first_line,({ struct _tagged_ptr3 _temp2868=
yyls; struct Cyc_Yyltype* _temp2870= _temp2868.curr + yylsp_offset; if(
_temp2870 < _temp2868.base? 1: _temp2870 >= _temp2868.last_plus_one){ _throw(
Null_Exception);}* _temp2870;}).last_line)); _temp2864;}); _temp2863;}); break;
case 53: _LL2862: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2872=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2872[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2873; _temp2873.tag=
Cyc_TypeSpecifier_tok_tag; _temp2873.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2874= yyls; struct Cyc_Yyltype* _temp2876= _temp2874.curr + yylsp_offset;
if( _temp2876 < _temp2874.base? 1: _temp2876 >= _temp2874.last_plus_one){ _throw(
Null_Exception);}* _temp2876;}).first_line,({ struct _tagged_ptr3 _temp2877=
yyls; struct Cyc_Yyltype* _temp2879= _temp2877.curr + yylsp_offset; if(
_temp2879 < _temp2877.base? 1: _temp2879 >= _temp2877.last_plus_one){ _throw(
Null_Exception);}* _temp2879;}).last_line)); _temp2873;}); _temp2872;}); break;
case 54: _LL2871: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2881=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2881[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2882; _temp2882.tag=
Cyc_TypeSpecifier_tok_tag; _temp2882.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2883= yyls; struct Cyc_Yyltype*
_temp2885= _temp2883.curr + yylsp_offset; if( _temp2885 < _temp2883.base? 1:
_temp2885 >= _temp2883.last_plus_one){ _throw( Null_Exception);}* _temp2885;}).first_line,({
struct _tagged_ptr3 _temp2886= yyls; struct Cyc_Yyltype* _temp2888= _temp2886.curr
+ yylsp_offset; if( _temp2888 < _temp2886.base? 1: _temp2888 >= _temp2886.last_plus_one){
_throw( Null_Exception);}* _temp2888;}).last_line)); _temp2882;}); _temp2881;});
break; case 55: _LL2880: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2890=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2890[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2891; _temp2891.tag=
Cyc_TypeSpecifier_tok_tag; _temp2891.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp2892=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp2892[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp2893; _temp2893.tag=
Cyc_Parse_Short_spec_tag; _temp2893.f1= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2894= yyls; struct Cyc_Yyltype* _temp2896= _temp2894.curr +
yylsp_offset; if( _temp2896 < _temp2894.base? 1: _temp2896 >= _temp2894.last_plus_one){
_throw( Null_Exception);}* _temp2896;}).first_line,({ struct _tagged_ptr3
_temp2897= yyls; struct Cyc_Yyltype* _temp2899= _temp2897.curr + yylsp_offset;
if( _temp2899 < _temp2897.base? 1: _temp2899 >= _temp2897.last_plus_one){ _throw(
Null_Exception);}* _temp2899;}).last_line); _temp2893;}); _temp2892;}));
_temp2891;}); _temp2890;}); break; case 56: _LL2889: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2901=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2901[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2902; _temp2902.tag= Cyc_TypeSpecifier_tok_tag;
_temp2902.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2903= yyls; struct Cyc_Yyltype* _temp2905= _temp2903.curr
+ yylsp_offset; if( _temp2905 < _temp2903.base? 1: _temp2905 >= _temp2903.last_plus_one){
_throw( Null_Exception);}* _temp2905;}).first_line,({ struct _tagged_ptr3
_temp2906= yyls; struct Cyc_Yyltype* _temp2908= _temp2906.curr + yylsp_offset;
if( _temp2908 < _temp2906.base? 1: _temp2908 >= _temp2906.last_plus_one){ _throw(
Null_Exception);}* _temp2908;}).last_line)); _temp2902;}); _temp2901;}); break;
case 57: _LL2900: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2910=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2910[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2911; _temp2911.tag=
Cyc_TypeSpecifier_tok_tag; _temp2911.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp2912=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp2912[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp2913; _temp2913.tag= Cyc_Parse_Long_spec_tag;
_temp2913.f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2914= yyls;
struct Cyc_Yyltype* _temp2916= _temp2914.curr + yylsp_offset; if( _temp2916 <
_temp2914.base? 1: _temp2916 >= _temp2914.last_plus_one){ _throw( Null_Exception);}*
_temp2916;}).first_line,({ struct _tagged_ptr3 _temp2917= yyls; struct Cyc_Yyltype*
_temp2919= _temp2917.curr + yylsp_offset; if( _temp2919 < _temp2917.base? 1:
_temp2919 >= _temp2917.last_plus_one){ _throw( Null_Exception);}* _temp2919;}).last_line);
_temp2913;}); _temp2912;})); _temp2911;}); _temp2910;}); break; case 58: _LL2909:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2921=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2921[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2922; _temp2922.tag=
Cyc_TypeSpecifier_tok_tag; _temp2922.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2923= yyls; struct Cyc_Yyltype*
_temp2925= _temp2923.curr + yylsp_offset; if( _temp2925 < _temp2923.base? 1:
_temp2925 >= _temp2923.last_plus_one){ _throw( Null_Exception);}* _temp2925;}).first_line,({
struct _tagged_ptr3 _temp2926= yyls; struct Cyc_Yyltype* _temp2928= _temp2926.curr
+ yylsp_offset; if( _temp2928 < _temp2926.base? 1: _temp2928 >= _temp2926.last_plus_one){
_throw( Null_Exception);}* _temp2928;}).last_line)); _temp2922;}); _temp2921;});
break; case 59: _LL2920: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2930=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2930[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2931; _temp2931.tag=
Cyc_TypeSpecifier_tok_tag; _temp2931.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2932= yyls; struct Cyc_Yyltype*
_temp2934= _temp2932.curr + yylsp_offset; if( _temp2934 < _temp2932.base? 1:
_temp2934 >= _temp2932.last_plus_one){ _throw( Null_Exception);}* _temp2934;}).first_line,({
struct _tagged_ptr3 _temp2935= yyls; struct Cyc_Yyltype* _temp2937= _temp2935.curr
+ yylsp_offset; if( _temp2937 < _temp2935.base? 1: _temp2937 >= _temp2935.last_plus_one){
_throw( Null_Exception);}* _temp2937;}).last_line)); _temp2931;}); _temp2930;});
break; case 60: _LL2929: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2939=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2939[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2940; _temp2940.tag=
Cyc_TypeSpecifier_tok_tag; _temp2940.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp2941=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp2941[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp2942; _temp2942.tag=
Cyc_Parse_Signed_spec_tag; _temp2942.f1= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2943= yyls; struct Cyc_Yyltype* _temp2945= _temp2943.curr +
yylsp_offset; if( _temp2945 < _temp2943.base? 1: _temp2945 >= _temp2943.last_plus_one){
_throw( Null_Exception);}* _temp2945;}).first_line,({ struct _tagged_ptr3
_temp2946= yyls; struct Cyc_Yyltype* _temp2948= _temp2946.curr + yylsp_offset;
if( _temp2948 < _temp2946.base? 1: _temp2948 >= _temp2946.last_plus_one){ _throw(
Null_Exception);}* _temp2948;}).last_line); _temp2942;}); _temp2941;}));
_temp2940;}); _temp2939;}); break; case 61: _LL2938: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2950=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2950[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2951; _temp2951.tag= Cyc_TypeSpecifier_tok_tag;
_temp2951.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp2952=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp2952[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp2953; _temp2953.tag= Cyc_Parse_Unsigned_spec_tag; _temp2953.f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2954= yyls; struct Cyc_Yyltype* _temp2956= _temp2954.curr
+ yylsp_offset; if( _temp2956 < _temp2954.base? 1: _temp2956 >= _temp2954.last_plus_one){
_throw( Null_Exception);}* _temp2956;}).first_line,({ struct _tagged_ptr3
_temp2957= yyls; struct Cyc_Yyltype* _temp2959= _temp2957.curr + yylsp_offset;
if( _temp2959 < _temp2957.base? 1: _temp2959 >= _temp2957.last_plus_one){ _throw(
Null_Exception);}* _temp2959;}).last_line); _temp2953;}); _temp2952;}));
_temp2951;}); _temp2950;}); break; case 62: _LL2949: yyval=({ struct
_tagged_ptr2 _temp2961= yyvs; struct _xtunion_struct** _temp2963= _temp2961.curr
+ yyvsp_offset; if( _temp2963 < _temp2961.base? 1: _temp2963 >= _temp2961.last_plus_one){
_throw( Null_Exception);}* _temp2963;}); break; case 63: _LL2960: yyval=({
struct _tagged_ptr2 _temp2965= yyvs; struct _xtunion_struct** _temp2967=
_temp2965.curr + yyvsp_offset; if( _temp2967 < _temp2965.base? 1: _temp2967 >=
_temp2965.last_plus_one){ _throw( Null_Exception);}* _temp2967;}); break; case
64: _LL2964: yyval=({ struct _tagged_ptr2 _temp2969= yyvs; struct
_xtunion_struct** _temp2971= _temp2969.curr + yyvsp_offset; if( _temp2971 <
_temp2969.base? 1: _temp2971 >= _temp2969.last_plus_one){ _throw( Null_Exception);}*
_temp2971;}); break; case 65: _LL2968: yyval=( struct _xtunion_struct*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2973=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2973[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2974; _temp2974.tag= Cyc_TypeSpecifier_tok_tag;
_temp2974.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2975= yyvs; struct _xtunion_struct** _temp2977=
_temp2975.curr + yyvsp_offset; if( _temp2977 < _temp2975.base? 1: _temp2977 >=
_temp2975.last_plus_one){ _throw( Null_Exception);}* _temp2977;})),(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)()), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2978= yyls; struct Cyc_Yyltype* _temp2980= _temp2978.curr + yylsp_offset;
if( _temp2980 < _temp2978.base? 1: _temp2980 >= _temp2978.last_plus_one){ _throw(
Null_Exception);}* _temp2980;}).first_line,({ struct _tagged_ptr3 _temp2981=
yyls; struct Cyc_Yyltype* _temp2983= _temp2981.curr + yylsp_offset; if(
_temp2983 < _temp2981.base? 1: _temp2983 >= _temp2981.last_plus_one){ _throw(
Null_Exception);}* _temp2983;}).last_line)); _temp2974;}); _temp2973;}); break;
case 66: _LL2972: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2985=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2985[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2986; _temp2986.tag=
Cyc_TypeSpecifier_tok_tag; _temp2986.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2987= yyvs; struct
_xtunion_struct** _temp2989= _temp2987.curr +( yyvsp_offset - 2); if( _temp2989
< _temp2987.base? 1: _temp2989 >= _temp2987.last_plus_one){ _throw(
Null_Exception);}* _temp2989;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp2990= yyvs; struct
_xtunion_struct** _temp2992= _temp2990.curr + yyvsp_offset; if( _temp2992 <
_temp2990.base? 1: _temp2992 >= _temp2990.last_plus_one){ _throw( Null_Exception);}*
_temp2992;})))), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2993=
yyls; struct Cyc_Yyltype* _temp2995= _temp2993.curr +( yylsp_offset - 2); if(
_temp2995 < _temp2993.base? 1: _temp2995 >= _temp2993.last_plus_one){ _throw(
Null_Exception);}* _temp2995;}).first_line,({ struct _tagged_ptr3 _temp2996=
yyls; struct Cyc_Yyltype* _temp2998= _temp2996.curr + yylsp_offset; if(
_temp2998 < _temp2996.base? 1: _temp2998 >= _temp2996.last_plus_one){ _throw(
Null_Exception);}* _temp2998;}).last_line)); _temp2986;}); _temp2985;}); break;
case 67: _LL2984: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3000=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3000[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3001; _temp3001.tag=
Cyc_TypeSpecifier_tok_tag; _temp3001.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp3002=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp3002[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp3003; _temp3003.tag= Cyc_Absyn_TypedefType_tag;
_temp3003.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3004= yyvs;
struct _xtunion_struct** _temp3006= _temp3004.curr +( yyvsp_offset - 1); if(
_temp3006 < _temp3004.base? 1: _temp3006 >= _temp3004.last_plus_one){ _throw(
Null_Exception);}* _temp3006;})); _temp3003.f2= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3007= yyvs; struct _xtunion_struct** _temp3009= _temp3007.curr
+ yyvsp_offset; if( _temp3009 < _temp3007.base? 1: _temp3009 >= _temp3007.last_plus_one){
_throw( Null_Exception);}* _temp3009;})); _temp3003.f3= 0; _temp3003;});
_temp3002;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3010= yyls;
struct Cyc_Yyltype* _temp3012= _temp3010.curr +( yylsp_offset - 1); if(
_temp3012 < _temp3010.base? 1: _temp3012 >= _temp3010.last_plus_one){ _throw(
Null_Exception);}* _temp3012;}).first_line,({ struct _tagged_ptr3 _temp3013=
yyls; struct Cyc_Yyltype* _temp3015= _temp3013.curr + yylsp_offset; if(
_temp3015 < _temp3013.base? 1: _temp3015 >= _temp3013.last_plus_one){ _throw(
Null_Exception);}* _temp3015;}).last_line)); _temp3001;}); _temp3000;}); break;
case 68: _LL2999: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3017=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3017[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3018; _temp3018.tag=
Cyc_TypeSpecifier_tok_tag; _temp3018.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp3019=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp3019[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp3020; _temp3020.tag= Cyc_Absyn_TupleType_tag;
_temp3020.f1=(( struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3021= yyls; struct Cyc_Yyltype* _temp3023= _temp3021.curr +(
yylsp_offset - 1); if( _temp3023 < _temp3021.base? 1: _temp3023 >= _temp3021.last_plus_one){
_throw( Null_Exception);}* _temp3023;}).first_line,({ struct _tagged_ptr3
_temp3024= yyls; struct Cyc_Yyltype* _temp3026= _temp3024.curr +( yylsp_offset -
1); if( _temp3026 < _temp3024.base? 1: _temp3026 >= _temp3024.last_plus_one){
_throw( Null_Exception);}* _temp3026;}).last_line),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3027= yyvs; struct _xtunion_struct** _temp3029=
_temp3027.curr +( yyvsp_offset - 1); if( _temp3029 < _temp3027.base? 1:
_temp3029 >= _temp3027.last_plus_one){ _throw( Null_Exception);}* _temp3029;}))));
_temp3020;}); _temp3019;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3030= yyls; struct Cyc_Yyltype* _temp3032= _temp3030.curr +( yylsp_offset -
3); if( _temp3032 < _temp3030.base? 1: _temp3032 >= _temp3030.last_plus_one){
_throw( Null_Exception);}* _temp3032;}).first_line,({ struct _tagged_ptr3
_temp3033= yyls; struct Cyc_Yyltype* _temp3035= _temp3033.curr + yylsp_offset;
if( _temp3035 < _temp3033.base? 1: _temp3035 >= _temp3033.last_plus_one){ _throw(
Null_Exception);}* _temp3035;}).last_line)); _temp3018;}); _temp3017;}); break;
case 69: _LL3016: yyval=( struct _xtunion_struct*)({ struct Cyc_Kind_tok_struct*
_temp3037=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp3037[ 0]=({ struct Cyc_Kind_tok_struct _temp3038; _temp3038.tag= Cyc_Kind_tok_tag;
_temp3038.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp3039= yyvs; struct _xtunion_struct** _temp3041= _temp3039.curr
+ yyvsp_offset; if( _temp3041 < _temp3039.base? 1: _temp3041 >= _temp3039.last_plus_one){
_throw( Null_Exception);}* _temp3041;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3042= yyls; struct Cyc_Yyltype* _temp3044= _temp3042.curr +
yylsp_offset; if( _temp3044 < _temp3042.base? 1: _temp3044 >= _temp3042.last_plus_one){
_throw( Null_Exception);}* _temp3044;}).first_line,({ struct _tagged_ptr3
_temp3045= yyls; struct Cyc_Yyltype* _temp3047= _temp3045.curr + yylsp_offset;
if( _temp3047 < _temp3045.base? 1: _temp3047 >= _temp3045.last_plus_one){ _throw(
Null_Exception);}* _temp3047;}).last_line)); _temp3038;}); _temp3037;}); break;
case 70: _LL3036: { struct _tagged_string* _temp3054; void* _temp3056; struct
_tuple1 _temp3052=* Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3049= yyvs;
struct _xtunion_struct** _temp3051= _temp3049.curr + yyvsp_offset; if( _temp3051
< _temp3049.base? 1: _temp3051 >= _temp3049.last_plus_one){ _throw(
Null_Exception);}* _temp3051;})); _LL3057: _temp3056= _temp3052.f1; goto _LL3055;
_LL3055: _temp3054= _temp3052.f2; goto _LL3053; _LL3053: if( _temp3056 !=( void*)
Cyc_Absyn_Loc_n){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp3058=(
char*)"bad kind in type specifier"; struct _tagged_string _temp3059; _temp3059.curr=
_temp3058; _temp3059.base= _temp3058; _temp3059.last_plus_one= _temp3058 + 27;
_temp3059;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3060= yyls;
struct Cyc_Yyltype* _temp3062= _temp3060.curr + yylsp_offset; if( _temp3062 <
_temp3060.base? 1: _temp3062 >= _temp3060.last_plus_one){ _throw( Null_Exception);}*
_temp3062;}).first_line,({ struct _tagged_ptr3 _temp3063= yyls; struct Cyc_Yyltype*
_temp3065= _temp3063.curr + yylsp_offset; if( _temp3065 < _temp3063.base? 1:
_temp3065 >= _temp3063.last_plus_one){ _throw( Null_Exception);}* _temp3065;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_Kind_tok_struct* _temp3066=(
struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp3066[ 0]=({ struct Cyc_Kind_tok_struct _temp3067; _temp3067.tag= Cyc_Kind_tok_tag;
_temp3067.f1=( void*) Cyc_Parse_id_to_kind(* _temp3054, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3068= yyls; struct Cyc_Yyltype* _temp3070= _temp3068.curr
+ yylsp_offset; if( _temp3070 < _temp3068.base? 1: _temp3070 >= _temp3068.last_plus_one){
_throw( Null_Exception);}* _temp3070;}).first_line,({ struct _tagged_ptr3
_temp3071= yyls; struct Cyc_Yyltype* _temp3073= _temp3071.curr + yylsp_offset;
if( _temp3073 < _temp3071.base? 1: _temp3073 >= _temp3071.last_plus_one){ _throw(
Null_Exception);}* _temp3073;}).last_line)); _temp3067;}); _temp3066;}); break;}
case 71: _LL3048: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeQual_tok_struct*
_temp3075=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp3075[ 0]=({ struct Cyc_TypeQual_tok_struct _temp3076; _temp3076.tag= Cyc_TypeQual_tok_tag;
_temp3076.f1=({ struct Cyc_Absyn_Tqual* _temp3077=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp3077->q_const= 1;
_temp3077->q_volatile= 0; _temp3077->q_restrict= 0; _temp3077;}); _temp3076;});
_temp3075;}); break; case 72: _LL3074: yyval=( struct _xtunion_struct*)({ struct
Cyc_TypeQual_tok_struct* _temp3079=( struct Cyc_TypeQual_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeQual_tok_struct)); _temp3079[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3080; _temp3080.tag= Cyc_TypeQual_tok_tag; _temp3080.f1=({ struct Cyc_Absyn_Tqual*
_temp3081=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp3081->q_const= 0; _temp3081->q_volatile= 1; _temp3081->q_restrict= 0;
_temp3081;}); _temp3080;}); _temp3079;}); break; case 73: _LL3078: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeQual_tok_struct* _temp3083=( struct
Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp3083[ 0]=({ struct Cyc_TypeQual_tok_struct _temp3084; _temp3084.tag= Cyc_TypeQual_tok_tag;
_temp3084.f1=({ struct Cyc_Absyn_Tqual* _temp3085=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp3085->q_const= 0;
_temp3085->q_volatile= 0; _temp3085->q_restrict= 1; _temp3085;}); _temp3084;});
_temp3083;}); break; case 74: _LL3082: yyval=( struct _xtunion_struct*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp3087=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3087[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3088; _temp3088.tag= Cyc_TypeSpecifier_tok_tag;
_temp3088.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp3089=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3089[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3090; _temp3090.tag= Cyc_Parse_Decl_spec_tag;
_temp3090.f1=({ struct Cyc_Absyn_Decl* _temp3091=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp3091->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp3098=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp3098[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp3099; _temp3099.tag= Cyc_Absyn_Enum_d_tag; _temp3099.f1=({ struct Cyc_Absyn_Enumdecl*
_temp3100=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp3100->sc=( void*)(( void*) Cyc_Absyn_Public); _temp3100->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3104= yyvs; struct _xtunion_struct** _temp3106=
_temp3104.curr +( yyvsp_offset - 3); if( _temp3106 < _temp3104.base? 1:
_temp3106 >= _temp3104.last_plus_one){ _throw( Null_Exception);}* _temp3106;}));
_temp3100->fields= Cyc_yyget_EnumfieldList_tok(({ struct _tagged_ptr2 _temp3101=
yyvs; struct _xtunion_struct** _temp3103= _temp3101.curr +( yyvsp_offset - 1);
if( _temp3103 < _temp3101.base? 1: _temp3103 >= _temp3101.last_plus_one){ _throw(
Null_Exception);}* _temp3103;})); _temp3100;}); _temp3099;}); _temp3098;}));
_temp3091->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3092=
yyls; struct Cyc_Yyltype* _temp3094= _temp3092.curr +( yylsp_offset - 4); if(
_temp3094 < _temp3092.base? 1: _temp3094 >= _temp3092.last_plus_one){ _throw(
Null_Exception);}* _temp3094;}).first_line,({ struct _tagged_ptr3 _temp3095=
yyls; struct Cyc_Yyltype* _temp3097= _temp3095.curr + yylsp_offset; if(
_temp3097 < _temp3095.base? 1: _temp3097 >= _temp3095.last_plus_one){ _throw(
Null_Exception);}* _temp3097;}).last_line); _temp3091;}); _temp3090;});
_temp3089;})); _temp3088;}); _temp3087;}); break; case 75: _LL3086: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3108=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3108[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3109; _temp3109.tag=
Cyc_TypeSpecifier_tok_tag; _temp3109.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp3110=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp3110[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp3111; _temp3111.tag= Cyc_Absyn_EnumType_tag;
_temp3111.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3112= yyvs;
struct _xtunion_struct** _temp3114= _temp3112.curr + yyvsp_offset; if( _temp3114
< _temp3112.base? 1: _temp3114 >= _temp3112.last_plus_one){ _throw(
Null_Exception);}* _temp3114;})); _temp3111.f2= 0; _temp3111;}); _temp3110;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3115= yyls; struct Cyc_Yyltype*
_temp3117= _temp3115.curr +( yylsp_offset - 1); if( _temp3117 < _temp3115.base?
1: _temp3117 >= _temp3115.last_plus_one){ _throw( Null_Exception);}* _temp3117;}).first_line,({
struct _tagged_ptr3 _temp3118= yyls; struct Cyc_Yyltype* _temp3120= _temp3118.curr
+ yylsp_offset; if( _temp3120 < _temp3118.base? 1: _temp3120 >= _temp3118.last_plus_one){
_throw( Null_Exception);}* _temp3120;}).last_line)); _temp3109;}); _temp3108;});
break; case 76: _LL3107: yyval=( struct _xtunion_struct*)({ struct Cyc_Enumfield_tok_struct*
_temp3122=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp3122[ 0]=({ struct Cyc_Enumfield_tok_struct _temp3123; _temp3123.tag= Cyc_Enumfield_tok_tag;
_temp3123.f1=({ struct Cyc_Absyn_Enumfield* _temp3124=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3124->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3131= yyvs; struct _xtunion_struct** _temp3133=
_temp3131.curr + yyvsp_offset; if( _temp3133 < _temp3131.base? 1: _temp3133 >=
_temp3131.last_plus_one){ _throw( Null_Exception);}* _temp3133;})); _temp3124->tag=
0; _temp3124->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3125=
yyls; struct Cyc_Yyltype* _temp3127= _temp3125.curr + yylsp_offset; if(
_temp3127 < _temp3125.base? 1: _temp3127 >= _temp3125.last_plus_one){ _throw(
Null_Exception);}* _temp3127;}).first_line,({ struct _tagged_ptr3 _temp3128=
yyls; struct Cyc_Yyltype* _temp3130= _temp3128.curr + yylsp_offset; if(
_temp3130 < _temp3128.base? 1: _temp3130 >= _temp3128.last_plus_one){ _throw(
Null_Exception);}* _temp3130;}).last_line); _temp3124;}); _temp3123;});
_temp3122;}); break; case 77: _LL3121: yyval=( struct _xtunion_struct*)({ struct
Cyc_Enumfield_tok_struct* _temp3135=( struct Cyc_Enumfield_tok_struct*)
GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct)); _temp3135[ 0]=({ struct
Cyc_Enumfield_tok_struct _temp3136; _temp3136.tag= Cyc_Enumfield_tok_tag;
_temp3136.f1=({ struct Cyc_Absyn_Enumfield* _temp3137=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3137->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3147= yyvs; struct _xtunion_struct** _temp3149=
_temp3147.curr +( yyvsp_offset - 2); if( _temp3149 < _temp3147.base? 1:
_temp3149 >= _temp3147.last_plus_one){ _throw( Null_Exception);}* _temp3149;}));
_temp3137->tag=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp3144= yyvs; struct _xtunion_struct** _temp3146= _temp3144.curr +
yyvsp_offset; if( _temp3146 < _temp3144.base? 1: _temp3146 >= _temp3144.last_plus_one){
_throw( Null_Exception);}* _temp3146;})); _temp3137->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3138= yyls; struct Cyc_Yyltype* _temp3140= _temp3138.curr
+( yylsp_offset - 2); if( _temp3140 < _temp3138.base? 1: _temp3140 >= _temp3138.last_plus_one){
_throw( Null_Exception);}* _temp3140;}).first_line,({ struct _tagged_ptr3
_temp3141= yyls; struct Cyc_Yyltype* _temp3143= _temp3141.curr + yylsp_offset;
if( _temp3143 < _temp3141.base? 1: _temp3143 >= _temp3141.last_plus_one){ _throw(
Null_Exception);}* _temp3143;}).last_line); _temp3137;}); _temp3136;});
_temp3135;}); break; case 78: _LL3134: yyval=( struct _xtunion_struct*)({ struct
Cyc_EnumfieldList_tok_struct* _temp3151=( struct Cyc_EnumfieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct)); _temp3151[ 0]=({
struct Cyc_EnumfieldList_tok_struct _temp3152; _temp3152.tag= Cyc_EnumfieldList_tok_tag;
_temp3152.f1=({ struct Cyc_List_List* _temp3153=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3153->hd=( void*) Cyc_yyget_Enumfield_tok(({
struct _tagged_ptr2 _temp3154= yyvs; struct _xtunion_struct** _temp3156=
_temp3154.curr + yyvsp_offset; if( _temp3156 < _temp3154.base? 1: _temp3156 >=
_temp3154.last_plus_one){ _throw( Null_Exception);}* _temp3156;})); _temp3153->tl=
0; _temp3153;}); _temp3152;}); _temp3151;}); break; case 79: _LL3150: yyval=(
struct _xtunion_struct*)({ struct Cyc_EnumfieldList_tok_struct* _temp3158=(
struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp3158[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp3159; _temp3159.tag=
Cyc_EnumfieldList_tok_tag; _temp3159.f1=({ struct Cyc_List_List* _temp3160=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3160->hd=(
void*) Cyc_yyget_Enumfield_tok(({ struct _tagged_ptr2 _temp3164= yyvs; struct
_xtunion_struct** _temp3166= _temp3164.curr +( yyvsp_offset - 2); if( _temp3166
< _temp3164.base? 1: _temp3166 >= _temp3164.last_plus_one){ _throw(
Null_Exception);}* _temp3166;})); _temp3160->tl= Cyc_yyget_EnumfieldList_tok(({
struct _tagged_ptr2 _temp3161= yyvs; struct _xtunion_struct** _temp3163=
_temp3161.curr + yyvsp_offset; if( _temp3163 < _temp3161.base? 1: _temp3163 >=
_temp3161.last_plus_one){ _throw( Null_Exception);}* _temp3163;})); _temp3160;});
_temp3159;}); _temp3158;}); break; case 80: _LL3157: { struct Cyc_Absyn_Decl* d;{
void* _temp3171= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3168=
yyvs; struct _xtunion_struct** _temp3170= _temp3168.curr +( yyvsp_offset - 3);
if( _temp3170 < _temp3168.base? 1: _temp3170 >= _temp3168.last_plus_one){ _throw(
Null_Exception);}* _temp3170;})); _LL3173: if(( int) _temp3171 == Cyc_Parse_Struct_su){
goto _LL3174;} else{ goto _LL3175;} _LL3175: if(( int) _temp3171 == Cyc_Parse_Union_su){
goto _LL3176;} else{ goto _LL3172;} _LL3174: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public, 0, 0,({ struct Cyc_Core_Opt* _temp3177=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3177->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3178= yyvs; struct _xtunion_struct** _temp3180=
_temp3178.curr +( yyvsp_offset - 1); if( _temp3180 < _temp3178.base? 1:
_temp3180 >= _temp3178.last_plus_one){ _throw( Null_Exception);}* _temp3180;}));
_temp3177;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3181=
yyls; struct Cyc_Yyltype* _temp3183= _temp3181.curr +( yylsp_offset - 3); if(
_temp3183 < _temp3181.base? 1: _temp3183 >= _temp3181.last_plus_one){ _throw(
Null_Exception);}* _temp3183;}).first_line,({ struct _tagged_ptr3 _temp3184=
yyls; struct Cyc_Yyltype* _temp3186= _temp3184.curr + yylsp_offset; if(
_temp3186 < _temp3184.base? 1: _temp3186 >= _temp3184.last_plus_one){ _throw(
Null_Exception);}* _temp3186;}).last_line)); goto _LL3172; _LL3176: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public, 0, 0,({ struct Cyc_Core_Opt* _temp3187=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3187->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3188= yyvs; struct _xtunion_struct** _temp3190=
_temp3188.curr +( yyvsp_offset - 1); if( _temp3190 < _temp3188.base? 1:
_temp3190 >= _temp3188.last_plus_one){ _throw( Null_Exception);}* _temp3190;}));
_temp3187;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3191=
yyls; struct Cyc_Yyltype* _temp3193= _temp3191.curr +( yylsp_offset - 3); if(
_temp3193 < _temp3191.base? 1: _temp3193 >= _temp3191.last_plus_one){ _throw(
Null_Exception);}* _temp3193;}).first_line,({ struct _tagged_ptr3 _temp3194=
yyls; struct Cyc_Yyltype* _temp3196= _temp3194.curr + yylsp_offset; if(
_temp3196 < _temp3194.base? 1: _temp3196 >= _temp3194.last_plus_one){ _throw(
Null_Exception);}* _temp3196;}).last_line)); goto _LL3172; _LL3172:;} yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3197=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3197[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3198; _temp3198.tag=
Cyc_TypeSpecifier_tok_tag; _temp3198.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3199=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3199[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3200; _temp3200.tag= Cyc_Parse_Decl_spec_tag;
_temp3200.f1= d; _temp3200;}); _temp3199;})); _temp3198;}); _temp3197;}); Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp3201=( char*)"anonymous structs/unions";
struct _tagged_string _temp3202; _temp3202.curr= _temp3201; _temp3202.base=
_temp3201; _temp3202.last_plus_one= _temp3201 + 25; _temp3202;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3203= yyls; struct Cyc_Yyltype* _temp3205= _temp3203.curr
+( yylsp_offset - 3); if( _temp3205 < _temp3203.base? 1: _temp3205 >= _temp3203.last_plus_one){
_throw( Null_Exception);}* _temp3205;}).first_line,({ struct _tagged_ptr3
_temp3206= yyls; struct Cyc_Yyltype* _temp3208= _temp3206.curr + yylsp_offset;
if( _temp3208 < _temp3206.base? 1: _temp3208 >= _temp3206.last_plus_one){ _throw(
Null_Exception);}* _temp3208;}).last_line)); break;} case 81: _LL3167: { struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3210= yyls; struct Cyc_Yyltype* _temp3212= _temp3210.curr
+( yylsp_offset - 3); if( _temp3212 < _temp3210.base? 1: _temp3212 >= _temp3210.last_plus_one){
_throw( Null_Exception);}* _temp3212;}).first_line,({ struct _tagged_ptr3
_temp3213= yyls; struct Cyc_Yyltype* _temp3215= _temp3213.curr +( yylsp_offset -
3); if( _temp3215 < _temp3213.base? 1: _temp3215 >= _temp3213.last_plus_one){
_throw( Null_Exception);}* _temp3215;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3216= yyvs; struct _xtunion_struct** _temp3218=
_temp3216.curr +( yyvsp_offset - 3); if( _temp3218 < _temp3216.base? 1:
_temp3218 >= _temp3216.last_plus_one){ _throw( Null_Exception);}* _temp3218;})));
struct Cyc_Absyn_Decl* d;{ void* _temp3222= Cyc_yyget_StructOrUnion_tok(({
struct _tagged_ptr2 _temp3219= yyvs; struct _xtunion_struct** _temp3221=
_temp3219.curr +( yyvsp_offset - 5); if( _temp3221 < _temp3219.base? 1:
_temp3221 >= _temp3219.last_plus_one){ _throw( Null_Exception);}* _temp3221;}));
_LL3224: if(( int) _temp3222 == Cyc_Parse_Struct_su){ goto _LL3225;} else{ goto
_LL3226;} _LL3226: if(( int) _temp3222 == Cyc_Parse_Union_su){ goto _LL3227;}
else{ goto _LL3223;} _LL3225: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp3228=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3228->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3229= yyvs; struct _xtunion_struct** _temp3231= _temp3229.curr
+( yyvsp_offset - 4); if( _temp3231 < _temp3229.base? 1: _temp3231 >= _temp3229.last_plus_one){
_throw( Null_Exception);}* _temp3231;})); _temp3228;}), ts,({ struct Cyc_Core_Opt*
_temp3232=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3232->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp3233= yyvs; struct _xtunion_struct** _temp3235= _temp3233.curr +(
yyvsp_offset - 1); if( _temp3235 < _temp3233.base? 1: _temp3235 >= _temp3233.last_plus_one){
_throw( Null_Exception);}* _temp3235;})); _temp3232;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3236= yyls; struct Cyc_Yyltype* _temp3238= _temp3236.curr
+( yylsp_offset - 5); if( _temp3238 < _temp3236.base? 1: _temp3238 >= _temp3236.last_plus_one){
_throw( Null_Exception);}* _temp3238;}).first_line,({ struct _tagged_ptr3
_temp3239= yyls; struct Cyc_Yyltype* _temp3241= _temp3239.curr + yylsp_offset;
if( _temp3241 < _temp3239.base? 1: _temp3241 >= _temp3239.last_plus_one){ _throw(
Null_Exception);}* _temp3241;}).last_line)); goto _LL3223; _LL3227: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp3242=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3242->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3243= yyvs; struct _xtunion_struct** _temp3245=
_temp3243.curr +( yyvsp_offset - 4); if( _temp3245 < _temp3243.base? 1:
_temp3245 >= _temp3243.last_plus_one){ _throw( Null_Exception);}* _temp3245;}));
_temp3242;}), ts,({ struct Cyc_Core_Opt* _temp3246=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3246->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3247= yyvs; struct _xtunion_struct** _temp3249=
_temp3247.curr +( yyvsp_offset - 1); if( _temp3249 < _temp3247.base? 1:
_temp3249 >= _temp3247.last_plus_one){ _throw( Null_Exception);}* _temp3249;}));
_temp3246;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3250=
yyls; struct Cyc_Yyltype* _temp3252= _temp3250.curr +( yylsp_offset - 5); if(
_temp3252 < _temp3250.base? 1: _temp3252 >= _temp3250.last_plus_one){ _throw(
Null_Exception);}* _temp3252;}).first_line,({ struct _tagged_ptr3 _temp3253=
yyls; struct Cyc_Yyltype* _temp3255= _temp3253.curr + yylsp_offset; if(
_temp3255 < _temp3253.base? 1: _temp3255 >= _temp3253.last_plus_one){ _throw(
Null_Exception);}* _temp3255;}).last_line)); goto _LL3223; _LL3223:;} yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3256=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3256[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3257; _temp3257.tag=
Cyc_TypeSpecifier_tok_tag; _temp3257.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3258=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3258[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3259; _temp3259.tag= Cyc_Parse_Decl_spec_tag;
_temp3259.f1= d; _temp3259;}); _temp3258;})); _temp3257;}); _temp3256;}); break;}
case 82: _LL3209: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3261= yyls; struct Cyc_Yyltype* _temp3263= _temp3261.curr
+( yylsp_offset - 3); if( _temp3263 < _temp3261.base? 1: _temp3263 >= _temp3261.last_plus_one){
_throw( Null_Exception);}* _temp3263;}).first_line,({ struct _tagged_ptr3
_temp3264= yyls; struct Cyc_Yyltype* _temp3266= _temp3264.curr +( yylsp_offset -
3); if( _temp3266 < _temp3264.base? 1: _temp3266 >= _temp3264.last_plus_one){
_throw( Null_Exception);}* _temp3266;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3267= yyvs; struct _xtunion_struct** _temp3269=
_temp3267.curr +( yyvsp_offset - 3); if( _temp3269 < _temp3267.base? 1:
_temp3269 >= _temp3267.last_plus_one){ _throw( Null_Exception);}* _temp3269;})));
struct Cyc_Absyn_Decl* d;{ void* _temp3273= Cyc_yyget_StructOrUnion_tok(({
struct _tagged_ptr2 _temp3270= yyvs; struct _xtunion_struct** _temp3272=
_temp3270.curr +( yyvsp_offset - 5); if( _temp3272 < _temp3270.base? 1:
_temp3272 >= _temp3270.last_plus_one){ _throw( Null_Exception);}* _temp3272;}));
_LL3275: if(( int) _temp3273 == Cyc_Parse_Struct_su){ goto _LL3276;} else{ goto
_LL3277;} _LL3277: if(( int) _temp3273 == Cyc_Parse_Union_su){ goto _LL3278;}
else{ goto _LL3274;} _LL3276: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp3279=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3279->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3280= yyvs; struct _xtunion_struct** _temp3282= _temp3280.curr
+( yyvsp_offset - 4); if( _temp3282 < _temp3280.base? 1: _temp3282 >= _temp3280.last_plus_one){
_throw( Null_Exception);}* _temp3282;})); _temp3279;}), ts,({ struct Cyc_Core_Opt*
_temp3283=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3283->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp3284= yyvs; struct _xtunion_struct** _temp3286= _temp3284.curr +(
yyvsp_offset - 1); if( _temp3286 < _temp3284.base? 1: _temp3286 >= _temp3284.last_plus_one){
_throw( Null_Exception);}* _temp3286;})); _temp3283;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3287= yyls; struct Cyc_Yyltype* _temp3289= _temp3287.curr
+( yylsp_offset - 5); if( _temp3289 < _temp3287.base? 1: _temp3289 >= _temp3287.last_plus_one){
_throw( Null_Exception);}* _temp3289;}).first_line,({ struct _tagged_ptr3
_temp3290= yyls; struct Cyc_Yyltype* _temp3292= _temp3290.curr + yylsp_offset;
if( _temp3292 < _temp3290.base? 1: _temp3292 >= _temp3290.last_plus_one){ _throw(
Null_Exception);}* _temp3292;}).last_line)); goto _LL3274; _LL3278: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp3293=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3293->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3294= yyvs; struct _xtunion_struct** _temp3296=
_temp3294.curr +( yyvsp_offset - 4); if( _temp3296 < _temp3294.base? 1:
_temp3296 >= _temp3294.last_plus_one){ _throw( Null_Exception);}* _temp3296;}));
_temp3293;}), ts,({ struct Cyc_Core_Opt* _temp3297=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3297->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3298= yyvs; struct _xtunion_struct** _temp3300=
_temp3298.curr +( yyvsp_offset - 1); if( _temp3300 < _temp3298.base? 1:
_temp3300 >= _temp3298.last_plus_one){ _throw( Null_Exception);}* _temp3300;}));
_temp3297;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3301=
yyls; struct Cyc_Yyltype* _temp3303= _temp3301.curr +( yylsp_offset - 5); if(
_temp3303 < _temp3301.base? 1: _temp3303 >= _temp3301.last_plus_one){ _throw(
Null_Exception);}* _temp3303;}).first_line,({ struct _tagged_ptr3 _temp3304=
yyls; struct Cyc_Yyltype* _temp3306= _temp3304.curr + yylsp_offset; if(
_temp3306 < _temp3304.base? 1: _temp3306 >= _temp3304.last_plus_one){ _throw(
Null_Exception);}* _temp3306;}).last_line)); goto _LL3274; _LL3274:;} yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3307=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3307[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3308; _temp3308.tag=
Cyc_TypeSpecifier_tok_tag; _temp3308.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3309=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3309[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3310; _temp3310.tag= Cyc_Parse_Decl_spec_tag;
_temp3310.f1= d; _temp3310;}); _temp3309;})); _temp3308;}); _temp3307;}); break;}
case 83: _LL3260:{ void* _temp3315= Cyc_yyget_StructOrUnion_tok(({ struct
_tagged_ptr2 _temp3312= yyvs; struct _xtunion_struct** _temp3314= _temp3312.curr
+( yyvsp_offset - 2); if( _temp3314 < _temp3312.base? 1: _temp3314 >= _temp3312.last_plus_one){
_throw( Null_Exception);}* _temp3314;})); _LL3317: if(( int) _temp3315 == Cyc_Parse_Struct_su){
goto _LL3318;} else{ goto _LL3319;} _LL3319: if(( int) _temp3315 == Cyc_Parse_Union_su){
goto _LL3320;} else{ goto _LL3316;} _LL3318: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeSpecifier_tok_struct* _temp3321=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3321[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3322; _temp3322.tag= Cyc_TypeSpecifier_tok_tag;
_temp3322.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_StructType_struct*
_temp3323=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp3323[ 0]=({ struct Cyc_Absyn_StructType_struct _temp3324; _temp3324.tag=
Cyc_Absyn_StructType_tag; _temp3324.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3325= yyvs; struct _xtunion_struct** _temp3327=
_temp3325.curr +( yyvsp_offset - 1); if( _temp3327 < _temp3325.base? 1:
_temp3327 >= _temp3325.last_plus_one){ _throw( Null_Exception);}* _temp3327;}));
_temp3324.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3328= yyvs;
struct _xtunion_struct** _temp3330= _temp3328.curr + yyvsp_offset; if( _temp3330
< _temp3328.base? 1: _temp3330 >= _temp3328.last_plus_one){ _throw(
Null_Exception);}* _temp3330;})); _temp3324.f3= 0; _temp3324;}); _temp3323;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3331= yyls; struct Cyc_Yyltype*
_temp3333= _temp3331.curr +( yylsp_offset - 2); if( _temp3333 < _temp3331.base?
1: _temp3333 >= _temp3331.last_plus_one){ _throw( Null_Exception);}* _temp3333;}).first_line,({
struct _tagged_ptr3 _temp3334= yyls; struct Cyc_Yyltype* _temp3336= _temp3334.curr
+ yylsp_offset; if( _temp3336 < _temp3334.base? 1: _temp3336 >= _temp3334.last_plus_one){
_throw( Null_Exception);}* _temp3336;}).last_line)); _temp3322;}); _temp3321;});
goto _LL3316; _LL3320: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3337=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3337[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3338; _temp3338.tag=
Cyc_TypeSpecifier_tok_tag; _temp3338.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp3339=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp3339[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp3340; _temp3340.tag= Cyc_Absyn_UnionType_tag;
_temp3340.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3341= yyvs; struct _xtunion_struct** _temp3343= _temp3341.curr +(
yyvsp_offset - 1); if( _temp3343 < _temp3341.base? 1: _temp3343 >= _temp3341.last_plus_one){
_throw( Null_Exception);}* _temp3343;})); _temp3340.f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3344= yyvs; struct _xtunion_struct** _temp3346=
_temp3344.curr + yyvsp_offset; if( _temp3346 < _temp3344.base? 1: _temp3346 >=
_temp3344.last_plus_one){ _throw( Null_Exception);}* _temp3346;})); _temp3340.f3=
0; _temp3340;}); _temp3339;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3347= yyls; struct Cyc_Yyltype* _temp3349= _temp3347.curr +(
yylsp_offset - 2); if( _temp3349 < _temp3347.base? 1: _temp3349 >= _temp3347.last_plus_one){
_throw( Null_Exception);}* _temp3349;}).first_line,({ struct _tagged_ptr3
_temp3350= yyls; struct Cyc_Yyltype* _temp3352= _temp3350.curr + yylsp_offset;
if( _temp3352 < _temp3350.base? 1: _temp3352 >= _temp3350.last_plus_one){ _throw(
Null_Exception);}* _temp3352;}).last_line)); _temp3338;}); _temp3337;}); goto
_LL3316; _LL3316:;} break; case 84: _LL3311:{ void* _temp3357= Cyc_yyget_StructOrUnion_tok(({
struct _tagged_ptr2 _temp3354= yyvs; struct _xtunion_struct** _temp3356=
_temp3354.curr +( yyvsp_offset - 2); if( _temp3356 < _temp3354.base? 1:
_temp3356 >= _temp3354.last_plus_one){ _throw( Null_Exception);}* _temp3356;}));
_LL3359: if(( int) _temp3357 == Cyc_Parse_Struct_su){ goto _LL3360;} else{ goto
_LL3361;} _LL3361: if(( int) _temp3357 == Cyc_Parse_Union_su){ goto _LL3362;}
else{ goto _LL3358;} _LL3360: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3363=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3363[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3364; _temp3364.tag=
Cyc_TypeSpecifier_tok_tag; _temp3364.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp3365=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3365[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp3366; _temp3366.tag= Cyc_Absyn_StructType_tag;
_temp3366.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3367= yyvs; struct _xtunion_struct** _temp3369= _temp3367.curr +(
yyvsp_offset - 1); if( _temp3369 < _temp3367.base? 1: _temp3369 >= _temp3367.last_plus_one){
_throw( Null_Exception);}* _temp3369;})); _temp3366.f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3370= yyvs; struct _xtunion_struct** _temp3372=
_temp3370.curr + yyvsp_offset; if( _temp3372 < _temp3370.base? 1: _temp3372 >=
_temp3370.last_plus_one){ _throw( Null_Exception);}* _temp3372;})); _temp3366.f3=
0; _temp3366;}); _temp3365;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3373= yyls; struct Cyc_Yyltype* _temp3375= _temp3373.curr +(
yylsp_offset - 2); if( _temp3375 < _temp3373.base? 1: _temp3375 >= _temp3373.last_plus_one){
_throw( Null_Exception);}* _temp3375;}).first_line,({ struct _tagged_ptr3
_temp3376= yyls; struct Cyc_Yyltype* _temp3378= _temp3376.curr + yylsp_offset;
if( _temp3378 < _temp3376.base? 1: _temp3378 >= _temp3376.last_plus_one){ _throw(
Null_Exception);}* _temp3378;}).last_line)); _temp3364;}); _temp3363;}); goto
_LL3358; _LL3362: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3379=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3379[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3380; _temp3380.tag=
Cyc_TypeSpecifier_tok_tag; _temp3380.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp3381=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp3381[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp3382; _temp3382.tag= Cyc_Absyn_UnionType_tag;
_temp3382.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3383= yyvs; struct _xtunion_struct** _temp3385= _temp3383.curr +(
yyvsp_offset - 1); if( _temp3385 < _temp3383.base? 1: _temp3385 >= _temp3383.last_plus_one){
_throw( Null_Exception);}* _temp3385;})); _temp3382.f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3386= yyvs; struct _xtunion_struct** _temp3388=
_temp3386.curr + yyvsp_offset; if( _temp3388 < _temp3386.base? 1: _temp3388 >=
_temp3386.last_plus_one){ _throw( Null_Exception);}* _temp3388;})); _temp3382.f3=
0; _temp3382;}); _temp3381;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3389= yyls; struct Cyc_Yyltype* _temp3391= _temp3389.curr +(
yylsp_offset - 2); if( _temp3391 < _temp3389.base? 1: _temp3391 >= _temp3389.last_plus_one){
_throw( Null_Exception);}* _temp3391;}).first_line,({ struct _tagged_ptr3
_temp3392= yyls; struct Cyc_Yyltype* _temp3394= _temp3392.curr + yylsp_offset;
if( _temp3394 < _temp3392.base? 1: _temp3394 >= _temp3392.last_plus_one){ _throw(
Null_Exception);}* _temp3394;}).last_line)); _temp3380;}); _temp3379;}); goto
_LL3358; _LL3358:;} break; case 85: _LL3353: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeList_tok_struct* _temp3396=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3396[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3397; _temp3397.tag= Cyc_TypeList_tok_tag; _temp3397.f1= 0; _temp3397;});
_temp3396;}); break; case 86: _LL3395: yyval=( struct _xtunion_struct*)({ struct
Cyc_TypeList_tok_struct* _temp3399=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp3399[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3400; _temp3400.tag= Cyc_TypeList_tok_tag; _temp3400.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3401= yyvs; struct _xtunion_struct** _temp3403= _temp3401.curr
+( yyvsp_offset - 1); if( _temp3403 < _temp3401.base? 1: _temp3403 >= _temp3401.last_plus_one){
_throw( Null_Exception);}* _temp3403;}))); _temp3400;}); _temp3399;}); break;
case 87: _LL3398:(( struct Cyc_Lexing_lexbuf*) Cyc_Parse_lbuf->v)->lex_curr_pos
-= 1; yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct*
_temp3405=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3405[ 0]=({ struct Cyc_TypeList_tok_struct _temp3406; _temp3406.tag= Cyc_TypeList_tok_tag;
_temp3406.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3407= yyvs; struct
_xtunion_struct** _temp3409= _temp3407.curr +( yyvsp_offset - 1); if( _temp3409
< _temp3407.base? 1: _temp3409 >= _temp3407.last_plus_one){ _throw(
Null_Exception);}* _temp3409;}))); _temp3406;}); _temp3405;}); break; case 88:
_LL3404: yyval=( struct _xtunion_struct*)({ struct Cyc_StructOrUnion_tok_struct*
_temp3411=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp3411[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp3412; _temp3412.tag=
Cyc_StructOrUnion_tok_tag; _temp3412.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp3412;}); _temp3411;}); break; case 89: _LL3410: yyval=( struct
_xtunion_struct*)({ struct Cyc_StructOrUnion_tok_struct* _temp3414=( struct Cyc_StructOrUnion_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct)); _temp3414[ 0]=({
struct Cyc_StructOrUnion_tok_struct _temp3415; _temp3415.tag= Cyc_StructOrUnion_tok_tag;
_temp3415.f1=( void*)(( void*) Cyc_Parse_Union_su); _temp3415;}); _temp3414;});
break; case 90: _LL3413: yyval=( struct _xtunion_struct*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3417=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp3417[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp3418; _temp3418.tag= Cyc_StructFieldDeclList_tok_tag; _temp3418.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3419= yyvs; struct _xtunion_struct** _temp3421=
_temp3419.curr + yyvsp_offset; if( _temp3421 < _temp3419.base? 1: _temp3421 >=
_temp3419.last_plus_one){ _throw( Null_Exception);}* _temp3421;})))); _temp3418;});
_temp3417;}); break; case 91: _LL3416: yyval=( struct _xtunion_struct*)({ struct
Cyc_StructFieldDeclListList_tok_struct* _temp3423=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp3423[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp3424; _temp3424.tag= Cyc_StructFieldDeclListList_tok_tag;
_temp3424.f1=({ struct Cyc_List_List* _temp3425=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3425->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3426= yyvs; struct _xtunion_struct** _temp3428=
_temp3426.curr + yyvsp_offset; if( _temp3428 < _temp3426.base? 1: _temp3428 >=
_temp3426.last_plus_one){ _throw( Null_Exception);}* _temp3428;})); _temp3425->tl=
0; _temp3425;}); _temp3424;}); _temp3423;}); break; case 92: _LL3422: yyval=(
struct _xtunion_struct*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3430=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp3430[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp3431; _temp3431.tag= Cyc_StructFieldDeclListList_tok_tag; _temp3431.f1=({
struct Cyc_List_List* _temp3432=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3432->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3436= yyvs; struct _xtunion_struct** _temp3438=
_temp3436.curr + yyvsp_offset; if( _temp3438 < _temp3436.base? 1: _temp3438 >=
_temp3436.last_plus_one){ _throw( Null_Exception);}* _temp3438;})); _temp3432->tl=
Cyc_yyget_StructFieldDeclListList_tok(({ struct _tagged_ptr2 _temp3433= yyvs;
struct _xtunion_struct** _temp3435= _temp3433.curr +( yyvsp_offset - 1); if(
_temp3435 < _temp3433.base? 1: _temp3435 >= _temp3433.last_plus_one){ _throw(
Null_Exception);}* _temp3435;})); _temp3432;}); _temp3431;}); _temp3430;});
break; case 93: _LL3429: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDeclList_tok_struct*
_temp3440=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3440[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp3441; _temp3441.tag=
Cyc_InitDeclList_tok_tag; _temp3441.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(({ struct _tagged_ptr2
_temp3442= yyvs; struct _xtunion_struct** _temp3444= _temp3442.curr +
yyvsp_offset; if( _temp3444 < _temp3442.base? 1: _temp3444 >= _temp3442.last_plus_one){
_throw( Null_Exception);}* _temp3444;}))); _temp3441;}); _temp3440;}); break;
case 94: _LL3439: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDeclList_tok_struct*
_temp3446=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3446[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp3447; _temp3447.tag=
Cyc_InitDeclList_tok_tag; _temp3447.f1=({ struct Cyc_List_List* _temp3448=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3448->hd=(
void*) Cyc_yyget_InitDecl_tok(({ struct _tagged_ptr2 _temp3449= yyvs; struct
_xtunion_struct** _temp3451= _temp3449.curr + yyvsp_offset; if( _temp3451 <
_temp3449.base? 1: _temp3451 >= _temp3449.last_plus_one){ _throw( Null_Exception);}*
_temp3451;})); _temp3448->tl= 0; _temp3448;}); _temp3447;}); _temp3446;});
break; case 95: _LL3445: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDeclList_tok_struct*
_temp3453=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3453[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp3454; _temp3454.tag=
Cyc_InitDeclList_tok_tag; _temp3454.f1=({ struct Cyc_List_List* _temp3455=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3455->hd=(
void*) Cyc_yyget_InitDecl_tok(({ struct _tagged_ptr2 _temp3459= yyvs; struct
_xtunion_struct** _temp3461= _temp3459.curr + yyvsp_offset; if( _temp3461 <
_temp3459.base? 1: _temp3461 >= _temp3459.last_plus_one){ _throw( Null_Exception);}*
_temp3461;})); _temp3455->tl= Cyc_yyget_InitDeclList_tok(({ struct _tagged_ptr2
_temp3456= yyvs; struct _xtunion_struct** _temp3458= _temp3456.curr +(
yyvsp_offset - 2); if( _temp3458 < _temp3456.base? 1: _temp3458 >= _temp3456.last_plus_one){
_throw( Null_Exception);}* _temp3458;})); _temp3455;}); _temp3454;}); _temp3453;});
break; case 96: _LL3452: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDecl_tok_struct*
_temp3463=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp3463[ 0]=({ struct Cyc_InitDecl_tok_struct _temp3464; _temp3464.tag= Cyc_InitDecl_tok_tag;
_temp3464.f1=({ struct _tuple13* _temp3465=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp3465->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3466= yyvs; struct _xtunion_struct** _temp3468= _temp3466.curr
+ yyvsp_offset; if( _temp3468 < _temp3466.base? 1: _temp3468 >= _temp3466.last_plus_one){
_throw( Null_Exception);}* _temp3468;})); _temp3465->f2= 0; _temp3465;});
_temp3464;}); _temp3463;}); break; case 97: _LL3462: yyval=( struct
_xtunion_struct*)({ struct Cyc_InitDecl_tok_struct* _temp3470=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp3470[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp3471; _temp3471.tag= Cyc_InitDecl_tok_tag; _temp3471.f1=({ struct _tuple13*
_temp3472=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp3472->f1=
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3476= yyvs; struct
_xtunion_struct** _temp3478= _temp3476.curr +( yyvsp_offset - 2); if( _temp3478
< _temp3476.base? 1: _temp3478 >= _temp3476.last_plus_one){ _throw(
Null_Exception);}* _temp3478;})); _temp3472->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3473= yyvs; struct _xtunion_struct** _temp3475=
_temp3473.curr + yyvsp_offset; if( _temp3475 < _temp3473.base? 1: _temp3475 >=
_temp3473.last_plus_one){ _throw( Null_Exception);}* _temp3475;})); _temp3472;});
_temp3471;}); _temp3470;}); break; case 98: _LL3469: { struct Cyc_Absyn_Tqual*
tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3480= yyvs; struct
_xtunion_struct** _temp3482= _temp3480.curr +( yyvsp_offset - 2); if( _temp3482
< _temp3480.base? 1: _temp3482 >= _temp3480.last_plus_one){ _throw(
Null_Exception);}* _temp3482;}))).f1; struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3483= yyvs; struct _xtunion_struct** _temp3485=
_temp3483.curr +( yyvsp_offset - 2); if( _temp3485 < _temp3483.base? 1:
_temp3485 >= _temp3483.last_plus_one){ _throw( Null_Exception);}* _temp3485;}))).f3;
void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3486= yyvs; struct _xtunion_struct** _temp3488= _temp3486.curr
+( yyvsp_offset - 2); if( _temp3488 < _temp3486.base? 1: _temp3488 >= _temp3486.last_plus_one){
_throw( Null_Exception);}* _temp3488;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3489= yyls; struct Cyc_Yyltype* _temp3491= _temp3489.curr
+( yylsp_offset - 2); if( _temp3491 < _temp3489.base? 1: _temp3491 >= _temp3489.last_plus_one){
_throw( Null_Exception);}* _temp3491;}).first_line,({ struct _tagged_ptr3
_temp3492= yyls; struct Cyc_Yyltype* _temp3494= _temp3492.curr +( yylsp_offset -
2); if( _temp3494 < _temp3492.base? 1: _temp3494 >= _temp3492.last_plus_one){
_throw( Null_Exception);}* _temp3494;}).last_line)); struct Cyc_List_List*
_temp3500; struct Cyc_List_List* _temp3502; struct _tuple0 _temp3498=(( struct
_tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3495= yyvs; struct _xtunion_struct** _temp3497=
_temp3495.curr +( yyvsp_offset - 1); if( _temp3497 < _temp3495.base? 1:
_temp3497 >= _temp3495.last_plus_one){ _throw( Null_Exception);}* _temp3497;})));
_LL3503: _temp3502= _temp3498.f1; goto _LL3501; _LL3501: _temp3500= _temp3498.f2;
goto _LL3499; _LL3499: { struct Cyc_List_List* info=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( Cyc_Parse_apply_tmss(
tq, t, _temp3502, atts), _temp3500); yyval=( struct _xtunion_struct*)({ struct
Cyc_StructFieldDeclList_tok_struct* _temp3504=( struct Cyc_StructFieldDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct)); _temp3504[ 0]=({
struct Cyc_StructFieldDeclList_tok_struct _temp3505; _temp3505.tag= Cyc_StructFieldDeclList_tok_tag;
_temp3505.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Position_Segment*, struct _tuple8*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3506= yyls; struct Cyc_Yyltype* _temp3508= _temp3506.curr
+( yylsp_offset - 2); if( _temp3508 < _temp3506.base? 1: _temp3508 >= _temp3506.last_plus_one){
_throw( Null_Exception);}* _temp3508;}).first_line,({ struct _tagged_ptr3
_temp3509= yyls; struct Cyc_Yyltype* _temp3511= _temp3509.curr +( yylsp_offset -
1); if( _temp3511 < _temp3509.base? 1: _temp3511 >= _temp3509.last_plus_one){
_throw( Null_Exception);}* _temp3511;}).last_line), info); _temp3505;});
_temp3504;}); break;}} case 99: _LL3479: yyval=( struct _xtunion_struct*)({
struct Cyc_QualSpecList_tok_struct* _temp3513=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp3513[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp3514; _temp3514.tag= Cyc_QualSpecList_tok_tag;
_temp3514.f1=({ struct _tuple14* _temp3515=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3515->f1= Cyc_Absyn_empty_tqual(); _temp3515->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp3519=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3519->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3520= yyvs; struct _xtunion_struct** _temp3522=
_temp3520.curr +( yyvsp_offset - 1); if( _temp3522 < _temp3520.base? 1:
_temp3522 >= _temp3520.last_plus_one){ _throw( Null_Exception);}* _temp3522;}));
_temp3519->tl= 0; _temp3519;}); _temp3515->f3= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3516= yyvs; struct _xtunion_struct** _temp3518=
_temp3516.curr + yyvsp_offset; if( _temp3518 < _temp3516.base? 1: _temp3518 >=
_temp3516.last_plus_one){ _throw( Null_Exception);}* _temp3518;})); _temp3515;});
_temp3514;}); _temp3513;}); break; case 100: _LL3512: yyval=( struct
_xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct* _temp3524=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp3524[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp3525; _temp3525.tag= Cyc_QualSpecList_tok_tag;
_temp3525.f1=({ struct _tuple14* _temp3526=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3526->f1=(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3540= yyvs; struct _xtunion_struct** _temp3542= _temp3540.curr
+ yyvsp_offset; if( _temp3542 < _temp3540.base? 1: _temp3542 >= _temp3540.last_plus_one){
_throw( Null_Exception);}* _temp3542;}))).f1; _temp3526->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp3533=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3533->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3537= yyvs; struct _xtunion_struct** _temp3539=
_temp3537.curr +( yyvsp_offset - 2); if( _temp3539 < _temp3537.base? 1:
_temp3539 >= _temp3537.last_plus_one){ _throw( Null_Exception);}* _temp3539;}));
_temp3533->tl=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3534=
yyvs; struct _xtunion_struct** _temp3536= _temp3534.curr + yyvsp_offset; if(
_temp3536 < _temp3534.base? 1: _temp3536 >= _temp3534.last_plus_one){ _throw(
Null_Exception);}* _temp3536;}))).f2; _temp3533;}); _temp3526->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3527= yyvs; struct _xtunion_struct** _temp3529=
_temp3527.curr +( yyvsp_offset - 1); if( _temp3529 < _temp3527.base? 1:
_temp3529 >= _temp3527.last_plus_one){ _throw( Null_Exception);}* _temp3529;})),(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3530= yyvs; struct
_xtunion_struct** _temp3532= _temp3530.curr + yyvsp_offset; if( _temp3532 <
_temp3530.base? 1: _temp3532 >= _temp3530.last_plus_one){ _throw( Null_Exception);}*
_temp3532;}))).f3); _temp3526;}); _temp3525;}); _temp3524;}); break; case 101:
_LL3523: yyval=( struct _xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct*
_temp3544=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3544[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3545; _temp3545.tag=
Cyc_QualSpecList_tok_tag; _temp3545.f1=({ struct _tuple14* _temp3546=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3546->f1= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3550= yyvs; struct _xtunion_struct** _temp3552=
_temp3550.curr +( yyvsp_offset - 1); if( _temp3552 < _temp3550.base? 1:
_temp3552 >= _temp3550.last_plus_one){ _throw( Null_Exception);}* _temp3552;}));
_temp3546->f2= 0; _temp3546->f3= Cyc_yyget_AttributeList_tok(({ struct
_tagged_ptr2 _temp3547= yyvs; struct _xtunion_struct** _temp3549= _temp3547.curr
+ yyvsp_offset; if( _temp3549 < _temp3547.base? 1: _temp3549 >= _temp3547.last_plus_one){
_throw( Null_Exception);}* _temp3549;})); _temp3546;}); _temp3545;}); _temp3544;});
break; case 102: _LL3543: yyval=( struct _xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct*
_temp3554=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3554[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3555; _temp3555.tag=
Cyc_QualSpecList_tok_tag; _temp3555.f1=({ struct _tuple14* _temp3556=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3556->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp3566= yyvs; struct
_xtunion_struct** _temp3568= _temp3566.curr +( yyvsp_offset - 2); if( _temp3568
< _temp3566.base? 1: _temp3568 >= _temp3566.last_plus_one){ _throw(
Null_Exception);}* _temp3568;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3569= yyvs; struct _xtunion_struct** _temp3571= _temp3569.curr
+ yyvsp_offset; if( _temp3571 < _temp3569.base? 1: _temp3571 >= _temp3569.last_plus_one){
_throw( Null_Exception);}* _temp3571;}))).f1); _temp3556->f2=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3563= yyvs; struct _xtunion_struct** _temp3565=
_temp3563.curr + yyvsp_offset; if( _temp3565 < _temp3563.base? 1: _temp3565 >=
_temp3563.last_plus_one){ _throw( Null_Exception);}* _temp3565;}))).f2;
_temp3556->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp3557= yyvs; struct _xtunion_struct** _temp3559= _temp3557.curr +(
yyvsp_offset - 1); if( _temp3559 < _temp3557.base? 1: _temp3559 >= _temp3557.last_plus_one){
_throw( Null_Exception);}* _temp3559;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3560= yyvs; struct _xtunion_struct** _temp3562= _temp3560.curr
+ yyvsp_offset; if( _temp3562 < _temp3560.base? 1: _temp3562 >= _temp3560.last_plus_one){
_throw( Null_Exception);}* _temp3562;}))).f3); _temp3556;}); _temp3555;});
_temp3554;}); break; case 103: _LL3553: yyval=( struct _xtunion_struct*)({
struct Cyc_DeclaratorExpoptList_tok_struct* _temp3573=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3573[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp3574; _temp3574.tag= Cyc_DeclaratorExpoptList_tok_tag;
_temp3574.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok(({ struct _tagged_ptr2 _temp3575= yyvs;
struct _xtunion_struct** _temp3577= _temp3575.curr + yyvsp_offset; if( _temp3577
< _temp3575.base? 1: _temp3577 >= _temp3575.last_plus_one){ _throw(
Null_Exception);}* _temp3577;}))); _temp3574;}); _temp3573;}); break; case 104:
_LL3572: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclaratorExpoptList_tok_struct*
_temp3579=( struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof(
struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3579[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct
_temp3580; _temp3580.tag= Cyc_DeclaratorExpoptList_tok_tag; _temp3580.f1=({
struct Cyc_List_List* _temp3581=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3581->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(({
struct _tagged_ptr2 _temp3582= yyvs; struct _xtunion_struct** _temp3584=
_temp3582.curr + yyvsp_offset; if( _temp3584 < _temp3582.base? 1: _temp3584 >=
_temp3582.last_plus_one){ _throw( Null_Exception);}* _temp3584;})); _temp3581->tl=
0; _temp3581;}); _temp3580;}); _temp3579;}); break; case 105: _LL3578: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp3586=(
struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct));
_temp3586[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct _temp3587; _temp3587.tag=
Cyc_DeclaratorExpoptList_tok_tag; _temp3587.f1=({ struct Cyc_List_List*
_temp3588=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3588->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(({ struct _tagged_ptr2
_temp3592= yyvs; struct _xtunion_struct** _temp3594= _temp3592.curr +
yyvsp_offset; if( _temp3594 < _temp3592.base? 1: _temp3594 >= _temp3592.last_plus_one){
_throw( Null_Exception);}* _temp3594;})); _temp3588->tl= Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3589= yyvs; struct _xtunion_struct** _temp3591=
_temp3589.curr +( yyvsp_offset - 2); if( _temp3591 < _temp3589.base? 1:
_temp3591 >= _temp3589.last_plus_one){ _throw( Null_Exception);}* _temp3591;}));
_temp3588;}); _temp3587;}); _temp3586;}); break; case 106: _LL3585: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp3596=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp3596[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp3597; _temp3597.tag=
Cyc_DeclaratorExpopt_tok_tag; _temp3597.f1=({ struct _tuple15* _temp3598=(
struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp3598->f1= Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3599= yyvs; struct _xtunion_struct** _temp3601=
_temp3599.curr + yyvsp_offset; if( _temp3601 < _temp3599.base? 1: _temp3601 >=
_temp3599.last_plus_one){ _throw( Null_Exception);}* _temp3601;})); _temp3598->f2=
0; _temp3598;}); _temp3597;}); _temp3596;}); break; case 107: _LL3595: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp3603=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp3603[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp3604; _temp3604.tag=
Cyc_DeclaratorExpopt_tok_tag; _temp3604.f1=({ struct _tuple15* _temp3605=(
struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp3605->f1=({ struct
Cyc_Parse_Declarator* _temp3610=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3610->id=({ struct _tuple1*
_temp3611=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3611->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp3615=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp3615[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp3616; _temp3616.tag= Cyc_Absyn_Rel_n_tag; _temp3616.f1= 0; _temp3616;});
_temp3615;}); _temp3611->f2=({ struct _tagged_string* _temp3612=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3612[ 0]=(
struct _tagged_string)({ char* _temp3613=( char*)""; struct _tagged_string
_temp3614; _temp3614.curr= _temp3613; _temp3614.base= _temp3613; _temp3614.last_plus_one=
_temp3613 + 1; _temp3614;}); _temp3612;}); _temp3611;}); _temp3610->tms= 0;
_temp3610;}); _temp3605->f2=({ struct Cyc_Core_Opt* _temp3606=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3606->v=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3607= yyvs; struct _xtunion_struct** _temp3609=
_temp3607.curr + yyvsp_offset; if( _temp3609 < _temp3607.base? 1: _temp3609 >=
_temp3607.last_plus_one){ _throw( Null_Exception);}* _temp3609;})); _temp3606;});
_temp3605;}); _temp3604;}); _temp3603;}); break; case 108: _LL3602: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp3618=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp3618[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp3619; _temp3619.tag=
Cyc_DeclaratorExpopt_tok_tag; _temp3619.f1=({ struct _tuple15* _temp3620=(
struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp3620->f1= Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3625= yyvs; struct _xtunion_struct** _temp3627=
_temp3625.curr +( yyvsp_offset - 2); if( _temp3627 < _temp3625.base? 1:
_temp3627 >= _temp3625.last_plus_one){ _throw( Null_Exception);}* _temp3627;}));
_temp3620->f2=({ struct Cyc_Core_Opt* _temp3621=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3621->v=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3622= yyvs; struct _xtunion_struct** _temp3624=
_temp3622.curr + yyvsp_offset; if( _temp3624 < _temp3622.base? 1: _temp3624 >=
_temp3622.last_plus_one){ _throw( Null_Exception);}* _temp3624;})); _temp3621;});
_temp3620;}); _temp3619;}); _temp3618;}); break; case 109: _LL3617: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3629=
yyls; struct Cyc_Yyltype* _temp3631= _temp3629.curr +( yylsp_offset - 3); if(
_temp3631 < _temp3629.base? 1: _temp3631 >= _temp3629.last_plus_one){ _throw(
Null_Exception);}* _temp3631;}).first_line,({ struct _tagged_ptr3 _temp3632=
yyls; struct Cyc_Yyltype* _temp3634= _temp3632.curr +( yylsp_offset - 3); if(
_temp3634 < _temp3632.base? 1: _temp3634 >= _temp3632.last_plus_one){ _throw(
Null_Exception);}* _temp3634;}).last_line), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3635= yyvs; struct _xtunion_struct** _temp3637= _temp3635.curr
+( yyvsp_offset - 3); if( _temp3637 < _temp3635.base? 1: _temp3637 >= _temp3635.last_plus_one){
_throw( Null_Exception);}* _temp3637;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_TypeSpecifier_tok_struct* _temp3638=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3638[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3639; _temp3639.tag= Cyc_TypeSpecifier_tok_tag;
_temp3639.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp3640=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3640[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3641; _temp3641.tag= Cyc_Parse_Decl_spec_tag;
_temp3641.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp3642=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3642->v=( void*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3643=
yyvs; struct _xtunion_struct** _temp3645= _temp3643.curr +( yyvsp_offset - 4);
if( _temp3645 < _temp3643.base? 1: _temp3645 >= _temp3643.last_plus_one){ _throw(
Null_Exception);}* _temp3645;})); _temp3642;}), ts,({ struct Cyc_Core_Opt*
_temp3646=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3646->v=( void*) Cyc_yyget_TunionFieldList_tok(({ struct _tagged_ptr2
_temp3647= yyvs; struct _xtunion_struct** _temp3649= _temp3647.curr +(
yyvsp_offset - 1); if( _temp3649 < _temp3647.base? 1: _temp3649 >= _temp3647.last_plus_one){
_throw( Null_Exception);}* _temp3649;})); _temp3646;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3650= yyls; struct Cyc_Yyltype* _temp3652= _temp3650.curr
+( yylsp_offset - 5); if( _temp3652 < _temp3650.base? 1: _temp3652 >= _temp3650.last_plus_one){
_throw( Null_Exception);}* _temp3652;}).first_line,({ struct _tagged_ptr3
_temp3653= yyls; struct Cyc_Yyltype* _temp3655= _temp3653.curr + yylsp_offset;
if( _temp3655 < _temp3653.base? 1: _temp3655 >= _temp3653.last_plus_one){ _throw(
Null_Exception);}* _temp3655;}).last_line)); _temp3641;}); _temp3640;}));
_temp3639;}); _temp3638;}); break;} case 110: _LL3628: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3657=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3657[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3658; _temp3658.tag= Cyc_TypeSpecifier_tok_tag;
_temp3658.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp3659=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp3659[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp3660; _temp3660.tag=
Cyc_Absyn_TunionType_tag; _temp3660.f1=({ struct Cyc_Absyn_TunionInfo _temp3661;
_temp3661.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3665= yyvs;
struct _xtunion_struct** _temp3667= _temp3665.curr +( yyvsp_offset - 1); if(
_temp3667 < _temp3665.base? 1: _temp3667 >= _temp3665.last_plus_one){ _throw(
Null_Exception);}* _temp3667;})); _temp3661.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3662= yyvs; struct _xtunion_struct** _temp3664=
_temp3662.curr + yyvsp_offset; if( _temp3664 < _temp3662.base? 1: _temp3664 >=
_temp3662.last_plus_one){ _throw( Null_Exception);}* _temp3664;})); _temp3661.rgn=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp3661.tud= 0; _temp3661;}); _temp3660;});
_temp3659;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3668= yyls;
struct Cyc_Yyltype* _temp3670= _temp3668.curr +( yylsp_offset - 2); if(
_temp3670 < _temp3668.base? 1: _temp3670 >= _temp3668.last_plus_one){ _throw(
Null_Exception);}* _temp3670;}).first_line,({ struct _tagged_ptr3 _temp3671=
yyls; struct Cyc_Yyltype* _temp3673= _temp3671.curr + yylsp_offset; if(
_temp3673 < _temp3671.base? 1: _temp3673 >= _temp3671.last_plus_one){ _throw(
Null_Exception);}* _temp3673;}).last_line)); _temp3658;}); _temp3657;}); break;
case 111: _LL3656: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3675=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3675[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3676; _temp3676.tag=
Cyc_TypeSpecifier_tok_tag; _temp3676.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp3677=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp3677[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp3678; _temp3678.tag= Cyc_Absyn_TunionType_tag;
_temp3678.f1=({ struct Cyc_Absyn_TunionInfo _temp3679; _temp3679.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3686= yyvs; struct _xtunion_struct** _temp3688=
_temp3686.curr +( yyvsp_offset - 1); if( _temp3688 < _temp3686.base? 1:
_temp3688 >= _temp3686.last_plus_one){ _throw( Null_Exception);}* _temp3688;}));
_temp3679.targs= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3683= yyvs;
struct _xtunion_struct** _temp3685= _temp3683.curr + yyvsp_offset; if( _temp3685
< _temp3683.base? 1: _temp3685 >= _temp3683.last_plus_one){ _throw(
Null_Exception);}* _temp3685;})); _temp3679.rgn=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3680= yyvs; struct _xtunion_struct** _temp3682=
_temp3680.curr +( yyvsp_offset - 2); if( _temp3682 < _temp3680.base? 1:
_temp3682 >= _temp3680.last_plus_one){ _throw( Null_Exception);}* _temp3682;}));
_temp3679.tud= 0; _temp3679;}); _temp3678;}); _temp3677;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3689= yyls; struct Cyc_Yyltype* _temp3691= _temp3689.curr
+( yylsp_offset - 3); if( _temp3691 < _temp3689.base? 1: _temp3691 >= _temp3689.last_plus_one){
_throw( Null_Exception);}* _temp3691;}).first_line,({ struct _tagged_ptr3
_temp3692= yyls; struct Cyc_Yyltype* _temp3694= _temp3692.curr + yylsp_offset;
if( _temp3694 < _temp3692.base? 1: _temp3694 >= _temp3692.last_plus_one){ _throw(
Null_Exception);}* _temp3694;}).last_line)); _temp3676;}); _temp3675;}); break;
case 112: _LL3674: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3696=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3696[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3697; _temp3697.tag=
Cyc_TypeSpecifier_tok_tag; _temp3697.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionFieldType_struct* _temp3698=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp3698[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp3699; _temp3699.tag= Cyc_Absyn_TunionFieldType_tag;
_temp3699.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp3700; _temp3700.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3707= yyvs; struct _xtunion_struct** _temp3709=
_temp3707.curr +( yyvsp_offset - 3); if( _temp3709 < _temp3707.base? 1:
_temp3709 >= _temp3707.last_plus_one){ _throw( Null_Exception);}* _temp3709;}));
_temp3700.targs= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3704= yyvs;
struct _xtunion_struct** _temp3706= _temp3704.curr + yyvsp_offset; if( _temp3706
< _temp3704.base? 1: _temp3706 >= _temp3704.last_plus_one){ _throw(
Null_Exception);}* _temp3706;})); _temp3700.fname= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3701= yyvs; struct _xtunion_struct** _temp3703=
_temp3701.curr +( yyvsp_offset - 1); if( _temp3703 < _temp3701.base? 1:
_temp3703 >= _temp3701.last_plus_one){ _throw( Null_Exception);}* _temp3703;}));
_temp3700.tud= 0; _temp3700.tufd= 0; _temp3700;}); _temp3699;}); _temp3698;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3710= yyls; struct Cyc_Yyltype*
_temp3712= _temp3710.curr +( yylsp_offset - 4); if( _temp3712 < _temp3710.base?
1: _temp3712 >= _temp3710.last_plus_one){ _throw( Null_Exception);}* _temp3712;}).first_line,({
struct _tagged_ptr3 _temp3713= yyls; struct Cyc_Yyltype* _temp3715= _temp3713.curr
+ yylsp_offset; if( _temp3715 < _temp3713.base? 1: _temp3715 >= _temp3713.last_plus_one){
_throw( Null_Exception);}* _temp3715;}).last_line)); _temp3697;}); _temp3696;});
break; case 113: _LL3695: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3717=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3717[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3718; _temp3718.tag=
Cyc_TypeSpecifier_tok_tag; _temp3718.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3719=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3719[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3720; _temp3720.tag= Cyc_Parse_Decl_spec_tag;
_temp3720.f1= Cyc_Absyn_xtunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3721= yyvs; struct _xtunion_struct** _temp3723=
_temp3721.curr +( yyvsp_offset - 3); if( _temp3723 < _temp3721.base? 1:
_temp3723 >= _temp3721.last_plus_one){ _throw( Null_Exception);}* _temp3723;})),
Cyc_yyget_TunionFieldList_tok(({ struct _tagged_ptr2 _temp3724= yyvs; struct
_xtunion_struct** _temp3726= _temp3724.curr +( yyvsp_offset - 1); if( _temp3726
< _temp3724.base? 1: _temp3726 >= _temp3724.last_plus_one){ _throw(
Null_Exception);}* _temp3726;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3727= yyls; struct Cyc_Yyltype* _temp3729= _temp3727.curr +(
yylsp_offset - 4); if( _temp3729 < _temp3727.base? 1: _temp3729 >= _temp3727.last_plus_one){
_throw( Null_Exception);}* _temp3729;}).first_line,({ struct _tagged_ptr3
_temp3730= yyls; struct Cyc_Yyltype* _temp3732= _temp3730.curr + yylsp_offset;
if( _temp3732 < _temp3730.base? 1: _temp3732 >= _temp3730.last_plus_one){ _throw(
Null_Exception);}* _temp3732;}).last_line)); _temp3720;}); _temp3719;}));
_temp3718;}); _temp3717;}); break; case 114: _LL3716: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3734=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3734[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3735; _temp3735.tag= Cyc_TypeSpecifier_tok_tag;
_temp3735.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_XTunionType_struct*
_temp3736=( struct Cyc_Absyn_XTunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct));
_temp3736[ 0]=({ struct Cyc_Absyn_XTunionType_struct _temp3737; _temp3737.tag=
Cyc_Absyn_XTunionType_tag; _temp3737.f1=({ struct Cyc_Absyn_XTunionInfo
_temp3738; _temp3738.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3739=
yyvs; struct _xtunion_struct** _temp3741= _temp3739.curr + yyvsp_offset; if(
_temp3741 < _temp3739.base? 1: _temp3741 >= _temp3739.last_plus_one){ _throw(
Null_Exception);}* _temp3741;})); _temp3738.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp3738.xtud= 0; _temp3738;}); _temp3737;}); _temp3736;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3742= yyls; struct Cyc_Yyltype* _temp3744= _temp3742.curr
+( yylsp_offset - 1); if( _temp3744 < _temp3742.base? 1: _temp3744 >= _temp3742.last_plus_one){
_throw( Null_Exception);}* _temp3744;}).first_line,({ struct _tagged_ptr3
_temp3745= yyls; struct Cyc_Yyltype* _temp3747= _temp3745.curr + yylsp_offset;
if( _temp3747 < _temp3745.base? 1: _temp3747 >= _temp3745.last_plus_one){ _throw(
Null_Exception);}* _temp3747;}).last_line)); _temp3735;}); _temp3734;}); break;
case 115: _LL3733: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3749=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3749[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3750; _temp3750.tag=
Cyc_TypeSpecifier_tok_tag; _temp3750.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_XTunionType_struct* _temp3751=( struct Cyc_Absyn_XTunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct)); _temp3751[ 0]=({
struct Cyc_Absyn_XTunionType_struct _temp3752; _temp3752.tag= Cyc_Absyn_XTunionType_tag;
_temp3752.f1=({ struct Cyc_Absyn_XTunionInfo _temp3753; _temp3753.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3757= yyvs; struct _xtunion_struct** _temp3759=
_temp3757.curr + yyvsp_offset; if( _temp3759 < _temp3757.base? 1: _temp3759 >=
_temp3757.last_plus_one){ _throw( Null_Exception);}* _temp3759;})); _temp3753.rgn=(
void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp3754= yyvs; struct
_xtunion_struct** _temp3756= _temp3754.curr +( yyvsp_offset - 1); if( _temp3756
< _temp3754.base? 1: _temp3756 >= _temp3754.last_plus_one){ _throw(
Null_Exception);}* _temp3756;})); _temp3753.xtud= 0; _temp3753;}); _temp3752;});
_temp3751;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3760= yyls;
struct Cyc_Yyltype* _temp3762= _temp3760.curr +( yylsp_offset - 2); if(
_temp3762 < _temp3760.base? 1: _temp3762 >= _temp3760.last_plus_one){ _throw(
Null_Exception);}* _temp3762;}).first_line,({ struct _tagged_ptr3 _temp3763=
yyls; struct Cyc_Yyltype* _temp3765= _temp3763.curr + yylsp_offset; if(
_temp3765 < _temp3763.base? 1: _temp3765 >= _temp3763.last_plus_one){ _throw(
Null_Exception);}* _temp3765;}).last_line)); _temp3750;}); _temp3749;}); break;
case 116: _LL3748: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3767=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3767[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3768; _temp3768.tag=
Cyc_TypeSpecifier_tok_tag; _temp3768.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_XTunionFieldType_struct* _temp3769=( struct Cyc_Absyn_XTunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunionFieldType_struct)); _temp3769[ 0]=({
struct Cyc_Absyn_XTunionFieldType_struct _temp3770; _temp3770.tag= Cyc_Absyn_XTunionFieldType_tag;
_temp3770.f1=({ struct Cyc_Absyn_XTunionFieldInfo _temp3771; _temp3771.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3775= yyvs; struct _xtunion_struct** _temp3777=
_temp3775.curr +( yyvsp_offset - 2); if( _temp3777 < _temp3775.base? 1:
_temp3777 >= _temp3775.last_plus_one){ _throw( Null_Exception);}* _temp3777;}));
_temp3771.fname= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3772= yyvs;
struct _xtunion_struct** _temp3774= _temp3772.curr + yyvsp_offset; if( _temp3774
< _temp3772.base? 1: _temp3774 >= _temp3772.last_plus_one){ _throw(
Null_Exception);}* _temp3774;})); _temp3771.xtud= 0; _temp3771.xtufd= 0;
_temp3771;}); _temp3770;}); _temp3769;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3778= yyls; struct Cyc_Yyltype* _temp3780= _temp3778.curr +(
yylsp_offset - 3); if( _temp3780 < _temp3778.base? 1: _temp3780 >= _temp3778.last_plus_one){
_throw( Null_Exception);}* _temp3780;}).first_line,({ struct _tagged_ptr3
_temp3781= yyls; struct Cyc_Yyltype* _temp3783= _temp3781.curr + yylsp_offset;
if( _temp3783 < _temp3781.base? 1: _temp3783 >= _temp3781.last_plus_one){ _throw(
Null_Exception);}* _temp3783;}).last_line)); _temp3768;}); _temp3767;}); break;
case 117: _LL3766: yyval=( struct _xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct*
_temp3785=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp3785[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp3786; _temp3786.tag= Cyc_TunionFieldList_tok_tag; _temp3786.f1=({ struct
Cyc_List_List* _temp3787=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3787->hd=( void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2
_temp3788= yyvs; struct _xtunion_struct** _temp3790= _temp3788.curr +
yyvsp_offset; if( _temp3790 < _temp3788.base? 1: _temp3790 >= _temp3788.last_plus_one){
_throw( Null_Exception);}* _temp3790;})); _temp3787->tl= 0; _temp3787;});
_temp3786;}); _temp3785;}); break; case 118: _LL3784: yyval=( struct
_xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct* _temp3792=( struct
Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3792[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3793; _temp3793.tag=
Cyc_TunionFieldList_tok_tag; _temp3793.f1=({ struct Cyc_List_List* _temp3794=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3794->hd=(
void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2 _temp3795= yyvs; struct
_xtunion_struct** _temp3797= _temp3795.curr +( yyvsp_offset - 1); if( _temp3797
< _temp3795.base? 1: _temp3797 >= _temp3795.last_plus_one){ _throw(
Null_Exception);}* _temp3797;})); _temp3794->tl= 0; _temp3794;}); _temp3793;});
_temp3792;}); break; case 119: _LL3791: yyval=( struct _xtunion_struct*)({
struct Cyc_TunionFieldList_tok_struct* _temp3799=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp3799[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp3800; _temp3800.tag= Cyc_TunionFieldList_tok_tag;
_temp3800.f1=({ struct Cyc_List_List* _temp3801=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3801->hd=( void*) Cyc_yyget_TunionField_tok(({
struct _tagged_ptr2 _temp3805= yyvs; struct _xtunion_struct** _temp3807=
_temp3805.curr +( yyvsp_offset - 2); if( _temp3807 < _temp3805.base? 1:
_temp3807 >= _temp3805.last_plus_one){ _throw( Null_Exception);}* _temp3807;}));
_temp3801->tl= Cyc_yyget_TunionFieldList_tok(({ struct _tagged_ptr2 _temp3802=
yyvs; struct _xtunion_struct** _temp3804= _temp3802.curr + yyvsp_offset; if(
_temp3804 < _temp3802.base? 1: _temp3804 >= _temp3802.last_plus_one){ _throw(
Null_Exception);}* _temp3804;})); _temp3801;}); _temp3800;}); _temp3799;});
break; case 120: _LL3798: yyval=( struct _xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct*
_temp3809=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp3809[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp3810; _temp3810.tag= Cyc_TunionFieldList_tok_tag; _temp3810.f1=({ struct
Cyc_List_List* _temp3811=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3811->hd=( void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2
_temp3815= yyvs; struct _xtunion_struct** _temp3817= _temp3815.curr +(
yyvsp_offset - 2); if( _temp3817 < _temp3815.base? 1: _temp3817 >= _temp3815.last_plus_one){
_throw( Null_Exception);}* _temp3817;})); _temp3811->tl= Cyc_yyget_TunionFieldList_tok(({
struct _tagged_ptr2 _temp3812= yyvs; struct _xtunion_struct** _temp3814=
_temp3812.curr + yyvsp_offset; if( _temp3814 < _temp3812.base? 1: _temp3814 >=
_temp3812.last_plus_one){ _throw( Null_Exception);}* _temp3814;})); _temp3811;});
_temp3810;}); _temp3809;}); break; case 121: _LL3808: yyval=( struct
_xtunion_struct*)({ struct Cyc_TunionField_tok_struct* _temp3819=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp3819[ 0]=({ struct
Cyc_TunionField_tok_struct _temp3820; _temp3820.tag= Cyc_TunionField_tok_tag;
_temp3820.f1=({ struct Cyc_Absyn_Tunionfield* _temp3821=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3821->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3828= yyvs; struct _xtunion_struct** _temp3830=
_temp3828.curr + yyvsp_offset; if( _temp3830 < _temp3828.base? 1: _temp3830 >=
_temp3828.last_plus_one){ _throw( Null_Exception);}* _temp3830;})); _temp3821->tvs=
0; _temp3821->typs= 0; _temp3821->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3822= yyls; struct Cyc_Yyltype* _temp3824= _temp3822.curr +
yylsp_offset; if( _temp3824 < _temp3822.base? 1: _temp3824 >= _temp3822.last_plus_one){
_throw( Null_Exception);}* _temp3824;}).first_line,({ struct _tagged_ptr3
_temp3825= yyls; struct Cyc_Yyltype* _temp3827= _temp3825.curr + yylsp_offset;
if( _temp3827 < _temp3825.base? 1: _temp3827 >= _temp3825.last_plus_one){ _throw(
Null_Exception);}* _temp3827;}).last_line); _temp3821;}); _temp3820;});
_temp3819;}); break; case 122: _LL3818: { struct Cyc_List_List* typs=(( struct
Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*, struct
_tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3832= yyls; struct Cyc_Yyltype* _temp3834= _temp3832.curr +( yylsp_offset -
1); if( _temp3834 < _temp3832.base? 1: _temp3834 >= _temp3832.last_plus_one){
_throw( Null_Exception);}* _temp3834;}).first_line,({ struct _tagged_ptr3
_temp3835= yyls; struct Cyc_Yyltype* _temp3837= _temp3835.curr +( yylsp_offset -
1); if( _temp3837 < _temp3835.base? 1: _temp3837 >= _temp3835.last_plus_one){
_throw( Null_Exception);}* _temp3837;}).last_line),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3838= yyvs; struct _xtunion_struct** _temp3840=
_temp3838.curr +( yyvsp_offset - 1); if( _temp3840 < _temp3838.base? 1:
_temp3840 >= _temp3838.last_plus_one){ _throw( Null_Exception);}* _temp3840;}))));
struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3841= yyls; struct Cyc_Yyltype* _temp3843= _temp3841.curr
+( yylsp_offset - 3); if( _temp3843 < _temp3841.base? 1: _temp3843 >= _temp3841.last_plus_one){
_throw( Null_Exception);}* _temp3843;}).first_line,({ struct _tagged_ptr3
_temp3844= yyls; struct Cyc_Yyltype* _temp3846= _temp3844.curr +( yylsp_offset -
3); if( _temp3846 < _temp3844.base? 1: _temp3846 >= _temp3844.last_plus_one){
_throw( Null_Exception);}* _temp3846;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3847= yyvs; struct _xtunion_struct** _temp3849=
_temp3847.curr +( yyvsp_offset - 3); if( _temp3849 < _temp3847.base? 1:
_temp3849 >= _temp3847.last_plus_one){ _throw( Null_Exception);}* _temp3849;})));
yyval=( struct _xtunion_struct*)({ struct Cyc_TunionField_tok_struct* _temp3850=(
struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp3850[ 0]=({ struct Cyc_TunionField_tok_struct _temp3851; _temp3851.tag= Cyc_TunionField_tok_tag;
_temp3851.f1=({ struct Cyc_Absyn_Tunionfield* _temp3852=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3852->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3859= yyvs; struct _xtunion_struct** _temp3861=
_temp3859.curr +( yyvsp_offset - 4); if( _temp3861 < _temp3859.base? 1:
_temp3861 >= _temp3859.last_plus_one){ _throw( Null_Exception);}* _temp3861;}));
_temp3852->tvs= tvs; _temp3852->typs= typs; _temp3852->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3853= yyls; struct Cyc_Yyltype* _temp3855= _temp3853.curr
+( yylsp_offset - 4); if( _temp3855 < _temp3853.base? 1: _temp3855 >= _temp3853.last_plus_one){
_throw( Null_Exception);}* _temp3855;}).first_line,({ struct _tagged_ptr3
_temp3856= yyls; struct Cyc_Yyltype* _temp3858= _temp3856.curr + yylsp_offset;
if( _temp3858 < _temp3856.base? 1: _temp3858 >= _temp3856.last_plus_one){ _throw(
Null_Exception);}* _temp3858;}).last_line); _temp3852;}); _temp3851;});
_temp3850;}); break;} case 123: _LL3831: yyval=({ struct _tagged_ptr2 _temp3863=
yyvs; struct _xtunion_struct** _temp3865= _temp3863.curr + yyvsp_offset; if(
_temp3865 < _temp3863.base? 1: _temp3865 >= _temp3863.last_plus_one){ _throw(
Null_Exception);}* _temp3865;}); break; case 124: _LL3862: yyval=( struct
_xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp3867=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3867[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3868; _temp3868.tag= Cyc_Declarator_tok_tag;
_temp3868.f1=({ struct Cyc_Parse_Declarator* _temp3869=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3869->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3876= yyvs; struct _xtunion_struct** _temp3878=
_temp3876.curr + yyvsp_offset; if( _temp3878 < _temp3876.base? 1: _temp3878 >=
_temp3876.last_plus_one){ _throw( Null_Exception);}* _temp3878;})))->id;
_temp3869->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2
_temp3870= yyvs; struct _xtunion_struct** _temp3872= _temp3870.curr +(
yyvsp_offset - 1); if( _temp3872 < _temp3870.base? 1: _temp3872 >= _temp3870.last_plus_one){
_throw( Null_Exception);}* _temp3872;})),( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3873= yyvs; struct _xtunion_struct** _temp3875= _temp3873.curr
+ yyvsp_offset; if( _temp3875 < _temp3873.base? 1: _temp3875 >= _temp3873.last_plus_one){
_throw( Null_Exception);}* _temp3875;})))->tms); _temp3869;}); _temp3868;});
_temp3867;}); break; case 125: _LL3866: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp3880=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3880[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3881; _temp3881.tag= Cyc_Declarator_tok_tag;
_temp3881.f1=({ struct Cyc_Parse_Declarator* _temp3882=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3882->id= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3883= yyvs; struct _xtunion_struct** _temp3885=
_temp3883.curr + yyvsp_offset; if( _temp3885 < _temp3883.base? 1: _temp3885 >=
_temp3883.last_plus_one){ _throw( Null_Exception);}* _temp3885;})); _temp3882->tms=
0; _temp3882;}); _temp3881;}); _temp3880;}); break; case 126: _LL3879: yyval=({
struct _tagged_ptr2 _temp3887= yyvs; struct _xtunion_struct** _temp3889=
_temp3887.curr +( yyvsp_offset - 1); if( _temp3889 < _temp3887.base? 1:
_temp3889 >= _temp3887.last_plus_one){ _throw( Null_Exception);}* _temp3889;});
break; case 127: _LL3886: yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct*
_temp3891=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3891[ 0]=({ struct Cyc_Declarator_tok_struct _temp3892; _temp3892.tag= Cyc_Declarator_tok_tag;
_temp3892.f1=({ struct Cyc_Parse_Declarator* _temp3893=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3893->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3898= yyvs; struct _xtunion_struct** _temp3900=
_temp3898.curr +( yyvsp_offset - 2); if( _temp3900 < _temp3898.base? 1:
_temp3900 >= _temp3898.last_plus_one){ _throw( Null_Exception);}* _temp3900;})))->id;
_temp3893->tms=({ struct Cyc_List_List* _temp3894=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3894->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp3894->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3895= yyvs;
struct _xtunion_struct** _temp3897= _temp3895.curr +( yyvsp_offset - 2); if(
_temp3897 < _temp3895.base? 1: _temp3897 >= _temp3895.last_plus_one){ _throw(
Null_Exception);}* _temp3897;})))->tms; _temp3894;}); _temp3893;}); _temp3892;});
_temp3891;}); break; case 128: _LL3890: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp3902=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3902[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3903; _temp3903.tag= Cyc_Declarator_tok_tag;
_temp3903.f1=({ struct Cyc_Parse_Declarator* _temp3904=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3904->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3914= yyvs; struct _xtunion_struct** _temp3916=
_temp3914.curr +( yyvsp_offset - 3); if( _temp3916 < _temp3914.base? 1:
_temp3916 >= _temp3914.last_plus_one){ _throw( Null_Exception);}* _temp3916;})))->id;
_temp3904->tms=({ struct Cyc_List_List* _temp3905=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3905->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp3909=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp3909[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp3910; _temp3910.tag= Cyc_Absyn_ConstArray_mod_tag;
_temp3910.f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3911= yyvs; struct
_xtunion_struct** _temp3913= _temp3911.curr +( yyvsp_offset - 1); if( _temp3913
< _temp3911.base? 1: _temp3913 >= _temp3911.last_plus_one){ _throw(
Null_Exception);}* _temp3913;})); _temp3910;}); _temp3909;})); _temp3905->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3906= yyvs; struct
_xtunion_struct** _temp3908= _temp3906.curr +( yyvsp_offset - 3); if( _temp3908
< _temp3906.base? 1: _temp3908 >= _temp3906.last_plus_one){ _throw(
Null_Exception);}* _temp3908;})))->tms; _temp3905;}); _temp3904;}); _temp3903;});
_temp3902;}); break; case 129: _LL3901: { struct _tuple16 _temp3923; struct Cyc_Core_Opt*
_temp3924; int _temp3926; struct Cyc_List_List* _temp3928; struct _tuple16*
_temp3921= Cyc_yyget_ParamDeclListBool_tok(({ struct _tagged_ptr2 _temp3918=
yyvs; struct _xtunion_struct** _temp3920= _temp3918.curr +( yyvsp_offset - 1);
if( _temp3920 < _temp3918.base? 1: _temp3920 >= _temp3918.last_plus_one){ _throw(
Null_Exception);}* _temp3920;})); _temp3923=* _temp3921; _LL3929: _temp3928=
_temp3923.f1; goto _LL3927; _LL3927: _temp3926= _temp3923.f2; goto _LL3925;
_LL3925: _temp3924= _temp3923.f3; goto _LL3922; _LL3922: yyval=( struct
_xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp3930=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3930[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3931; _temp3931.tag= Cyc_Declarator_tok_tag;
_temp3931.f1=({ struct Cyc_Parse_Declarator* _temp3932=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3932->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3941= yyvs; struct _xtunion_struct** _temp3943=
_temp3941.curr +( yyvsp_offset - 3); if( _temp3943 < _temp3941.base? 1:
_temp3943 >= _temp3941.last_plus_one){ _throw( Null_Exception);}* _temp3943;})))->id;
_temp3932->tms=({ struct Cyc_List_List* _temp3933=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3933->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3937=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3937[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3938; _temp3938.tag= Cyc_Absyn_Function_mod_tag;
_temp3938.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3939=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3939[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3940; _temp3940.tag= Cyc_Absyn_WithTypes_tag;
_temp3940.f1= _temp3928; _temp3940.f2= _temp3926; _temp3940.f3= _temp3924;
_temp3940;}); _temp3939;})); _temp3938;}); _temp3937;})); _temp3933->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3934= yyvs; struct _xtunion_struct** _temp3936=
_temp3934.curr +( yyvsp_offset - 3); if( _temp3936 < _temp3934.base? 1:
_temp3936 >= _temp3934.last_plus_one){ _throw( Null_Exception);}* _temp3936;})))->tms;
_temp3933;}); _temp3932;}); _temp3931;}); _temp3930;}); break;} case 130:
_LL3917: yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct*
_temp3945=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3945[ 0]=({ struct Cyc_Declarator_tok_struct _temp3946; _temp3946.tag= Cyc_Declarator_tok_tag;
_temp3946.f1=({ struct Cyc_Parse_Declarator* _temp3947=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3947->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3956= yyvs; struct _xtunion_struct** _temp3958=
_temp3956.curr +( yyvsp_offset - 2); if( _temp3958 < _temp3956.base? 1:
_temp3958 >= _temp3956.last_plus_one){ _throw( Null_Exception);}* _temp3958;})))->id;
_temp3947->tms=({ struct Cyc_List_List* _temp3948=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3948->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3952=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3952[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3953; _temp3953.tag= Cyc_Absyn_Function_mod_tag;
_temp3953.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3954=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3954[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3955; _temp3955.tag= Cyc_Absyn_WithTypes_tag;
_temp3955.f1= 0; _temp3955.f2= 0; _temp3955.f3= 0; _temp3955;}); _temp3954;}));
_temp3953;}); _temp3952;})); _temp3948->tl=( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3949= yyvs; struct _xtunion_struct** _temp3951= _temp3949.curr
+( yyvsp_offset - 2); if( _temp3951 < _temp3949.base? 1: _temp3951 >= _temp3949.last_plus_one){
_throw( Null_Exception);}* _temp3951;})))->tms; _temp3948;}); _temp3947;});
_temp3946;}); _temp3945;}); break; case 131: _LL3944: yyval=( struct
_xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp3960=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3960[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3961; _temp3961.tag= Cyc_Declarator_tok_tag;
_temp3961.f1=({ struct Cyc_Parse_Declarator* _temp3962=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3962->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3977= yyvs; struct _xtunion_struct** _temp3979=
_temp3977.curr +( yyvsp_offset - 4); if( _temp3979 < _temp3977.base? 1:
_temp3979 >= _temp3977.last_plus_one){ _throw( Null_Exception);}* _temp3979;})))->id;
_temp3962->tms=({ struct Cyc_List_List* _temp3963=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3963->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3967=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3967[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3968; _temp3968.tag= Cyc_Absyn_Function_mod_tag;
_temp3968.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3969=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3969[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3970; _temp3970.tag= Cyc_Absyn_WithTypes_tag;
_temp3970.f1= 0; _temp3970.f2= 0; _temp3970.f3=({ struct Cyc_Core_Opt* _temp3971=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3971->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3972=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp3972[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp3973; _temp3973.tag= Cyc_Absyn_JoinEff_tag;
_temp3973.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3974= yyvs;
struct _xtunion_struct** _temp3976= _temp3974.curr +( yyvsp_offset - 1); if(
_temp3976 < _temp3974.base? 1: _temp3976 >= _temp3974.last_plus_one){ _throw(
Null_Exception);}* _temp3976;})); _temp3973;}); _temp3972;})); _temp3971;});
_temp3970;}); _temp3969;})); _temp3968;}); _temp3967;})); _temp3963->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3964= yyvs; struct _xtunion_struct** _temp3966=
_temp3964.curr +( yyvsp_offset - 4); if( _temp3966 < _temp3964.base? 1:
_temp3966 >= _temp3964.last_plus_one){ _throw( Null_Exception);}* _temp3966;})))->tms;
_temp3963;}); _temp3962;}); _temp3961;}); _temp3960;}); break; case 132: _LL3959:
yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp3981=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3981[ 0]=({ struct Cyc_Declarator_tok_struct _temp3982; _temp3982.tag= Cyc_Declarator_tok_tag;
_temp3982.f1=({ struct Cyc_Parse_Declarator* _temp3983=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3983->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4001= yyvs; struct _xtunion_struct** _temp4003=
_temp4001.curr +( yyvsp_offset - 3); if( _temp4003 < _temp4001.base? 1:
_temp4003 >= _temp4001.last_plus_one){ _throw( Null_Exception);}* _temp4003;})))->id;
_temp3983->tms=({ struct Cyc_List_List* _temp3984=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3984->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3988=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3988[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3989; _temp3989.tag= Cyc_Absyn_Function_mod_tag;
_temp3989.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp3990=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp3990[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp3991; _temp3991.tag= Cyc_Absyn_NoTypes_tag;
_temp3991.f1= Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp3992= yyvs;
struct _xtunion_struct** _temp3994= _temp3992.curr +( yyvsp_offset - 1); if(
_temp3994 < _temp3992.base? 1: _temp3994 >= _temp3992.last_plus_one){ _throw(
Null_Exception);}* _temp3994;})); _temp3991.f2= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3995= yyls; struct Cyc_Yyltype* _temp3997= _temp3995.curr
+( yylsp_offset - 3); if( _temp3997 < _temp3995.base? 1: _temp3997 >= _temp3995.last_plus_one){
_throw( Null_Exception);}* _temp3997;}).first_line,({ struct _tagged_ptr3
_temp3998= yyls; struct Cyc_Yyltype* _temp4000= _temp3998.curr + yylsp_offset;
if( _temp4000 < _temp3998.base? 1: _temp4000 >= _temp3998.last_plus_one){ _throw(
Null_Exception);}* _temp4000;}).last_line); _temp3991;}); _temp3990;}));
_temp3989;}); _temp3988;})); _temp3984->tl=( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3985= yyvs; struct _xtunion_struct** _temp3987= _temp3985.curr
+( yyvsp_offset - 3); if( _temp3987 < _temp3985.base? 1: _temp3987 >= _temp3985.last_plus_one){
_throw( Null_Exception);}* _temp3987;})))->tms; _temp3984;}); _temp3983;});
_temp3982;}); _temp3981;}); break; case 133: _LL3980: { struct Cyc_List_List* ts=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4005=
yyls; struct Cyc_Yyltype* _temp4007= _temp4005.curr +( yylsp_offset - 2); if(
_temp4007 < _temp4005.base? 1: _temp4007 >= _temp4005.last_plus_one){ _throw(
Null_Exception);}* _temp4007;}).first_line,({ struct _tagged_ptr3 _temp4008=
yyls; struct Cyc_Yyltype* _temp4010= _temp4008.curr + yylsp_offset; if(
_temp4010 < _temp4008.base? 1: _temp4010 >= _temp4008.last_plus_one){ _throw(
Null_Exception);}* _temp4010;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4011= yyvs; struct _xtunion_struct** _temp4013= _temp4011.curr
+( yyvsp_offset - 1); if( _temp4013 < _temp4011.base? 1: _temp4013 >= _temp4011.last_plus_one){
_throw( Null_Exception);}* _temp4013;})))); yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp4014=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4014[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4015; _temp4015.tag= Cyc_Declarator_tok_tag;
_temp4015.f1=({ struct Cyc_Parse_Declarator* _temp4016=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4016->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4029= yyvs; struct _xtunion_struct** _temp4031=
_temp4029.curr +( yyvsp_offset - 3); if( _temp4031 < _temp4029.base? 1:
_temp4031 >= _temp4029.last_plus_one){ _throw( Null_Exception);}* _temp4031;})))->id;
_temp4016->tms=({ struct Cyc_List_List* _temp4017=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4017->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp4021=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp4021[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp4022; _temp4022.tag= Cyc_Absyn_TypeParams_mod_tag;
_temp4022.f1= ts; _temp4022.f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4023= yyls; struct Cyc_Yyltype* _temp4025= _temp4023.curr +(
yylsp_offset - 3); if( _temp4025 < _temp4023.base? 1: _temp4025 >= _temp4023.last_plus_one){
_throw( Null_Exception);}* _temp4025;}).first_line,({ struct _tagged_ptr3
_temp4026= yyls; struct Cyc_Yyltype* _temp4028= _temp4026.curr + yylsp_offset;
if( _temp4028 < _temp4026.base? 1: _temp4028 >= _temp4026.last_plus_one){ _throw(
Null_Exception);}* _temp4028;}).last_line); _temp4022.f3= 0; _temp4022;});
_temp4021;})); _temp4017->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp4018= yyvs; struct _xtunion_struct** _temp4020= _temp4018.curr +(
yyvsp_offset - 3); if( _temp4020 < _temp4018.base? 1: _temp4020 >= _temp4018.last_plus_one){
_throw( Null_Exception);}* _temp4020;})))->tms; _temp4017;}); _temp4016;});
_temp4015;}); _temp4014;}); break;} case 134: _LL4004:(( struct Cyc_Lexing_lexbuf*)
Cyc_Parse_lbuf->v)->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4033= yyls; struct Cyc_Yyltype* _temp4035= _temp4033.curr
+( yylsp_offset - 2); if( _temp4035 < _temp4033.base? 1: _temp4035 >= _temp4033.last_plus_one){
_throw( Null_Exception);}* _temp4035;}).first_line,({ struct _tagged_ptr3
_temp4036= yyls; struct Cyc_Yyltype* _temp4038= _temp4036.curr + yylsp_offset;
if( _temp4038 < _temp4036.base? 1: _temp4038 >= _temp4036.last_plus_one){ _throw(
Null_Exception);}* _temp4038;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4039= yyvs; struct _xtunion_struct** _temp4041= _temp4039.curr
+( yyvsp_offset - 1); if( _temp4041 < _temp4039.base? 1: _temp4041 >= _temp4039.last_plus_one){
_throw( Null_Exception);}* _temp4041;})))); yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp4042=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4042[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4043; _temp4043.tag= Cyc_Declarator_tok_tag;
_temp4043.f1=({ struct Cyc_Parse_Declarator* _temp4044=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4044->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4057= yyvs; struct _xtunion_struct** _temp4059=
_temp4057.curr +( yyvsp_offset - 3); if( _temp4059 < _temp4057.base? 1:
_temp4059 >= _temp4057.last_plus_one){ _throw( Null_Exception);}* _temp4059;})))->id;
_temp4044->tms=({ struct Cyc_List_List* _temp4045=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4045->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp4049=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp4049[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp4050; _temp4050.tag= Cyc_Absyn_TypeParams_mod_tag;
_temp4050.f1= ts; _temp4050.f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4051= yyls; struct Cyc_Yyltype* _temp4053= _temp4051.curr +(
yylsp_offset - 3); if( _temp4053 < _temp4051.base? 1: _temp4053 >= _temp4051.last_plus_one){
_throw( Null_Exception);}* _temp4053;}).first_line,({ struct _tagged_ptr3
_temp4054= yyls; struct Cyc_Yyltype* _temp4056= _temp4054.curr + yylsp_offset;
if( _temp4056 < _temp4054.base? 1: _temp4056 >= _temp4054.last_plus_one){ _throw(
Null_Exception);}* _temp4056;}).last_line); _temp4050.f3= 0; _temp4050;});
_temp4049;})); _temp4045->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp4046= yyvs; struct _xtunion_struct** _temp4048= _temp4046.curr +(
yyvsp_offset - 3); if( _temp4048 < _temp4046.base? 1: _temp4048 >= _temp4046.last_plus_one){
_throw( Null_Exception);}* _temp4048;})))->tms; _temp4045;}); _temp4044;});
_temp4043;}); _temp4042;}); break;} case 135: _LL4032: yyval=( struct
_xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp4061=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4061[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4062; _temp4062.tag= Cyc_Declarator_tok_tag;
_temp4062.f1=({ struct Cyc_Parse_Declarator* _temp4063=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4063->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4079= yyvs; struct _xtunion_struct** _temp4081=
_temp4079.curr +( yyvsp_offset - 1); if( _temp4081 < _temp4079.base? 1:
_temp4081 >= _temp4079.last_plus_one){ _throw( Null_Exception);}* _temp4081;})))->id;
_temp4063->tms=({ struct Cyc_List_List* _temp4064=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4064->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp4068=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp4068[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp4069; _temp4069.tag= Cyc_Absyn_Attributes_mod_tag;
_temp4069.f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4070= yyls;
struct Cyc_Yyltype* _temp4072= _temp4070.curr + yylsp_offset; if( _temp4072 <
_temp4070.base? 1: _temp4072 >= _temp4070.last_plus_one){ _throw( Null_Exception);}*
_temp4072;}).first_line,({ struct _tagged_ptr3 _temp4073= yyls; struct Cyc_Yyltype*
_temp4075= _temp4073.curr + yylsp_offset; if( _temp4075 < _temp4073.base? 1:
_temp4075 >= _temp4073.last_plus_one){ _throw( Null_Exception);}* _temp4075;}).last_line);
_temp4069.f2= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4076= yyvs;
struct _xtunion_struct** _temp4078= _temp4076.curr + yyvsp_offset; if( _temp4078
< _temp4076.base? 1: _temp4078 >= _temp4076.last_plus_one){ _throw(
Null_Exception);}* _temp4078;})); _temp4069;}); _temp4068;})); _temp4064->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp4065= yyvs; struct
_xtunion_struct** _temp4067= _temp4065.curr +( yyvsp_offset - 1); if( _temp4067
< _temp4065.base? 1: _temp4067 >= _temp4065.last_plus_one){ _throw(
Null_Exception);}* _temp4067;})))->tms; _temp4064;}); _temp4063;}); _temp4062;});
_temp4061;}); break; case 136: _LL4060: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeModifierList_tok_struct* _temp4083=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp4083[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp4084; _temp4084.tag= Cyc_TypeModifierList_tok_tag;
_temp4084.f1=({ struct Cyc_List_List* _temp4085=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4085->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp4095=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4095[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4096; _temp4096.tag= Cyc_Absyn_Pointer_mod_tag;
_temp4096.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4097= yyvs; struct _xtunion_struct** _temp4099= _temp4097.curr +(
yyvsp_offset - 2); if( _temp4099 < _temp4097.base? 1: _temp4099 >= _temp4097.last_plus_one){
_throw( Null_Exception);}* _temp4099;})); _temp4096.f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp4100= yyvs; struct _xtunion_struct** _temp4102=
_temp4100.curr +( yyvsp_offset - 1); if( _temp4102 < _temp4100.base? 1:
_temp4102 >= _temp4100.last_plus_one){ _throw( Null_Exception);}* _temp4102;}));
_temp4096.f3= Cyc_Absyn_empty_tqual(); _temp4096;}); _temp4095;})); _temp4085->tl=
Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4086= yyls; struct Cyc_Yyltype* _temp4088= _temp4086.curr + yylsp_offset;
if( _temp4088 < _temp4086.base? 1: _temp4088 >= _temp4086.last_plus_one){ _throw(
Null_Exception);}* _temp4088;}).first_line,({ struct _tagged_ptr3 _temp4089=
yyls; struct Cyc_Yyltype* _temp4091= _temp4089.curr + yylsp_offset; if(
_temp4091 < _temp4089.base? 1: _temp4091 >= _temp4089.last_plus_one){ _throw(
Null_Exception);}* _temp4091;}).last_line), Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp4092= yyvs; struct _xtunion_struct** _temp4094=
_temp4092.curr + yyvsp_offset; if( _temp4094 < _temp4092.base? 1: _temp4094 >=
_temp4092.last_plus_one){ _throw( Null_Exception);}* _temp4094;})), 0);
_temp4085;}); _temp4084;}); _temp4083;}); break; case 137: _LL4082: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeModifierList_tok_struct* _temp4104=(
struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp4104[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp4105; _temp4105.tag=
Cyc_TypeModifierList_tok_tag; _temp4105.f1=({ struct Cyc_List_List* _temp4106=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4106->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4116=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4116[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4117; _temp4117.tag= Cyc_Absyn_Pointer_mod_tag;
_temp4117.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4118= yyvs; struct _xtunion_struct** _temp4120= _temp4118.curr +(
yyvsp_offset - 3); if( _temp4120 < _temp4118.base? 1: _temp4120 >= _temp4118.last_plus_one){
_throw( Null_Exception);}* _temp4120;})); _temp4117.f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp4121= yyvs; struct _xtunion_struct** _temp4123=
_temp4121.curr +( yyvsp_offset - 2); if( _temp4123 < _temp4121.base? 1:
_temp4123 >= _temp4121.last_plus_one){ _throw( Null_Exception);}* _temp4123;}));
_temp4117.f3= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp4124= yyvs;
struct _xtunion_struct** _temp4126= _temp4124.curr + yyvsp_offset; if( _temp4126
< _temp4124.base? 1: _temp4126 >= _temp4124.last_plus_one){ _throw(
Null_Exception);}* _temp4126;})); _temp4117;}); _temp4116;})); _temp4106->tl=
Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4107= yyls; struct Cyc_Yyltype* _temp4109= _temp4107.curr +( yylsp_offset -
1); if( _temp4109 < _temp4107.base? 1: _temp4109 >= _temp4107.last_plus_one){
_throw( Null_Exception);}* _temp4109;}).first_line,({ struct _tagged_ptr3
_temp4110= yyls; struct Cyc_Yyltype* _temp4112= _temp4110.curr +( yylsp_offset -
1); if( _temp4112 < _temp4110.base? 1: _temp4112 >= _temp4110.last_plus_one){
_throw( Null_Exception);}* _temp4112;}).last_line), Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp4113= yyvs; struct _xtunion_struct** _temp4115=
_temp4113.curr +( yyvsp_offset - 1); if( _temp4115 < _temp4113.base? 1:
_temp4115 >= _temp4113.last_plus_one){ _throw( Null_Exception);}* _temp4115;})),
0); _temp4106;}); _temp4105;}); _temp4104;}); break; case 138: _LL4103: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeModifierList_tok_struct* _temp4128=(
struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp4128[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp4129; _temp4129.tag=
Cyc_TypeModifierList_tok_tag; _temp4129.f1=({ struct Cyc_List_List* _temp4130=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4130->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4143=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4143[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4144; _temp4144.tag= Cyc_Absyn_Pointer_mod_tag;
_temp4144.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4145= yyvs; struct _xtunion_struct** _temp4147= _temp4145.curr +(
yyvsp_offset - 3); if( _temp4147 < _temp4145.base? 1: _temp4147 >= _temp4145.last_plus_one){
_throw( Null_Exception);}* _temp4147;})); _temp4144.f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp4148= yyvs; struct _xtunion_struct** _temp4150=
_temp4148.curr +( yyvsp_offset - 2); if( _temp4150 < _temp4148.base? 1:
_temp4150 >= _temp4148.last_plus_one){ _throw( Null_Exception);}* _temp4150;}));
_temp4144.f3= Cyc_Absyn_empty_tqual(); _temp4144;}); _temp4143;})); _temp4130->tl=
Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4131= yyls; struct Cyc_Yyltype* _temp4133= _temp4131.curr +( yylsp_offset -
1); if( _temp4133 < _temp4131.base? 1: _temp4133 >= _temp4131.last_plus_one){
_throw( Null_Exception);}* _temp4133;}).first_line,({ struct _tagged_ptr3
_temp4134= yyls; struct Cyc_Yyltype* _temp4136= _temp4134.curr +( yylsp_offset -
1); if( _temp4136 < _temp4134.base? 1: _temp4136 >= _temp4134.last_plus_one){
_throw( Null_Exception);}* _temp4136;}).last_line), Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp4137= yyvs; struct _xtunion_struct** _temp4139=
_temp4137.curr +( yyvsp_offset - 1); if( _temp4139 < _temp4137.base? 1:
_temp4139 >= _temp4137.last_plus_one){ _throw( Null_Exception);}* _temp4139;})),
Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp4140= yyvs; struct
_xtunion_struct** _temp4142= _temp4140.curr + yyvsp_offset; if( _temp4142 <
_temp4140.base? 1: _temp4142 >= _temp4140.last_plus_one){ _throw( Null_Exception);}*
_temp4142;}))); _temp4130;}); _temp4129;}); _temp4128;}); break; case 139:
_LL4127: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeModifierList_tok_struct*
_temp4152=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp4152[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp4153; _temp4153.tag= Cyc_TypeModifierList_tok_tag; _temp4153.f1=({ struct
Cyc_List_List* _temp4154=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4154->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4167=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp4167[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp4168; _temp4168.tag=
Cyc_Absyn_Pointer_mod_tag; _temp4168.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp4169= yyvs; struct _xtunion_struct** _temp4171=
_temp4169.curr +( yyvsp_offset - 4); if( _temp4171 < _temp4169.base? 1:
_temp4171 >= _temp4169.last_plus_one){ _throw( Null_Exception);}* _temp4171;}));
_temp4168.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4172= yyvs;
struct _xtunion_struct** _temp4174= _temp4172.curr +( yyvsp_offset - 3); if(
_temp4174 < _temp4172.base? 1: _temp4174 >= _temp4172.last_plus_one){ _throw(
Null_Exception);}* _temp4174;})); _temp4168.f3= Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp4175= yyvs; struct _xtunion_struct** _temp4177= _temp4175.curr
+( yyvsp_offset - 1); if( _temp4177 < _temp4175.base? 1: _temp4177 >= _temp4175.last_plus_one){
_throw( Null_Exception);}* _temp4177;})); _temp4168;}); _temp4167;})); _temp4154->tl=
Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4155= yyls; struct Cyc_Yyltype* _temp4157= _temp4155.curr +( yylsp_offset -
2); if( _temp4157 < _temp4155.base? 1: _temp4157 >= _temp4155.last_plus_one){
_throw( Null_Exception);}* _temp4157;}).first_line,({ struct _tagged_ptr3
_temp4158= yyls; struct Cyc_Yyltype* _temp4160= _temp4158.curr +( yylsp_offset -
2); if( _temp4160 < _temp4158.base? 1: _temp4160 >= _temp4158.last_plus_one){
_throw( Null_Exception);}* _temp4160;}).last_line), Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp4161= yyvs; struct _xtunion_struct** _temp4163=
_temp4161.curr +( yyvsp_offset - 2); if( _temp4163 < _temp4161.base? 1:
_temp4163 >= _temp4161.last_plus_one){ _throw( Null_Exception);}* _temp4163;})),
Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp4164= yyvs; struct
_xtunion_struct** _temp4166= _temp4164.curr + yyvsp_offset; if( _temp4166 <
_temp4164.base? 1: _temp4166 >= _temp4164.last_plus_one){ _throw( Null_Exception);}*
_temp4166;}))); _temp4154;}); _temp4153;}); _temp4152;}); break; case 140:
_LL4151: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4179=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4179[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4180; _temp4180.tag=
Cyc_Pointer_Sort_tok_tag; _temp4180.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp4181=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp4181[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp4182; _temp4182.tag=
Cyc_Absyn_Nullable_ps_tag; _temp4182.f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4183= yyls; struct Cyc_Yyltype* _temp4185= _temp4183.curr
+ yylsp_offset; if( _temp4185 < _temp4183.base? 1: _temp4185 >= _temp4183.last_plus_one){
_throw( Null_Exception);}* _temp4185;}).first_line,({ struct _tagged_ptr3
_temp4186= yyls; struct Cyc_Yyltype* _temp4188= _temp4186.curr + yylsp_offset;
if( _temp4188 < _temp4186.base? 1: _temp4188 >= _temp4186.last_plus_one){ _throw(
Null_Exception);}* _temp4188;}).last_line)); _temp4182;}); _temp4181;}));
_temp4180;}); _temp4179;}); break; case 141: _LL4178: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct* _temp4190=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp4190[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp4191; _temp4191.tag= Cyc_Pointer_Sort_tok_tag;
_temp4191.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp4192=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp4192[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp4193; _temp4193.tag= Cyc_Absyn_NonNullable_ps_tag; _temp4193.f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4194= yyls; struct
Cyc_Yyltype* _temp4196= _temp4194.curr + yylsp_offset; if( _temp4196 < _temp4194.base?
1: _temp4196 >= _temp4194.last_plus_one){ _throw( Null_Exception);}* _temp4196;}).first_line,({
struct _tagged_ptr3 _temp4197= yyls; struct Cyc_Yyltype* _temp4199= _temp4197.curr
+ yylsp_offset; if( _temp4199 < _temp4197.base? 1: _temp4199 >= _temp4197.last_plus_one){
_throw( Null_Exception);}* _temp4199;}).last_line)); _temp4193;}); _temp4192;}));
_temp4191;}); _temp4190;}); break; case 142: _LL4189: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct* _temp4201=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp4201[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp4202; _temp4202.tag= Cyc_Pointer_Sort_tok_tag;
_temp4202.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp4203=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp4203[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp4204; _temp4204.tag=
Cyc_Absyn_Nullable_ps_tag; _temp4204.f1= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4205= yyvs; struct _xtunion_struct** _temp4207= _temp4205.curr
+( yyvsp_offset - 1); if( _temp4207 < _temp4205.base? 1: _temp4207 >= _temp4205.last_plus_one){
_throw( Null_Exception);}* _temp4207;})); _temp4204;}); _temp4203;})); _temp4202;});
_temp4201;}); break; case 143: _LL4200: yyval=( struct _xtunion_struct*)({
struct Cyc_Pointer_Sort_tok_struct* _temp4209=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp4209[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp4210; _temp4210.tag= Cyc_Pointer_Sort_tok_tag;
_temp4210.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp4211=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp4211[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp4212; _temp4212.tag= Cyc_Absyn_NonNullable_ps_tag; _temp4212.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4213= yyvs; struct _xtunion_struct** _temp4215=
_temp4213.curr +( yyvsp_offset - 1); if( _temp4215 < _temp4213.base? 1:
_temp4215 >= _temp4213.last_plus_one){ _throw( Null_Exception);}* _temp4215;}));
_temp4212;}); _temp4211;})); _temp4210;}); _temp4209;}); break; case 144:
_LL4208: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4217=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4217[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4218; _temp4218.tag=
Cyc_Pointer_Sort_tok_tag; _temp4218.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps);
_temp4218;}); _temp4217;}); break; case 145: _LL4216: yyval=( struct
_xtunion_struct*)({ struct Cyc_Rgn_tok_struct* _temp4220=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4220[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4221; _temp4221.tag= Cyc_Rgn_tok_tag; _temp4221.f1=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp4221;}); _temp4220;}); break; case 146: _LL4219: yyval=({ struct
_tagged_ptr2 _temp4223= yyvs; struct _xtunion_struct** _temp4225= _temp4223.curr
+ yyvsp_offset; if( _temp4225 < _temp4223.base? 1: _temp4225 >= _temp4223.last_plus_one){
_throw( Null_Exception);}* _temp4225;}); break; case 147: _LL4222: yyval=(
struct _xtunion_struct*)({ struct Cyc_Rgn_tok_struct* _temp4227=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4227[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4228; _temp4228.tag= Cyc_Rgn_tok_tag; _temp4228.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4229= yyvs; struct
_xtunion_struct** _temp4231= _temp4229.curr + yyvsp_offset; if( _temp4231 <
_temp4229.base? 1: _temp4231 >= _temp4229.last_plus_one){ _throw( Null_Exception);}*
_temp4231;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind)); _temp4228;}); _temp4227;}); break; case 148: _LL4226:
if( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp4233= yyvs; struct
_xtunion_struct** _temp4235= _temp4233.curr + yyvsp_offset; if( _temp4235 <
_temp4233.base? 1: _temp4235 >= _temp4233.last_plus_one){ _throw( Null_Exception);}*
_temp4235;})) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp4236=( char*)"expecting region kind\n"; struct
_tagged_string _temp4237; _temp4237.curr= _temp4236; _temp4237.base= _temp4236;
_temp4237.last_plus_one= _temp4236 + 23; _temp4237;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4238= yyls; struct Cyc_Yyltype* _temp4240= _temp4238.curr
+ yylsp_offset; if( _temp4240 < _temp4238.base? 1: _temp4240 >= _temp4238.last_plus_one){
_throw( Null_Exception);}* _temp4240;}).first_line,({ struct _tagged_ptr3
_temp4241= yyls; struct Cyc_Yyltype* _temp4243= _temp4241.curr + yylsp_offset;
if( _temp4243 < _temp4241.base? 1: _temp4243 >= _temp4241.last_plus_one){ _throw(
Null_Exception);}* _temp4243;}).last_line));} yyval=( struct _xtunion_struct*)({
struct Cyc_Rgn_tok_struct* _temp4244=( struct Cyc_Rgn_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgn_tok_struct)); _temp4244[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4245; _temp4245.tag= Cyc_Rgn_tok_tag; _temp4245.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4246= yyvs; struct
_xtunion_struct** _temp4248= _temp4246.curr +( yyvsp_offset - 2); if( _temp4248
< _temp4246.base? 1: _temp4248 >= _temp4246.last_plus_one){ _throw(
Null_Exception);}* _temp4248;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind)); _temp4245;}); _temp4244;}); break; case 149: _LL4232:
yyval=( struct _xtunion_struct*)({ struct Cyc_Rgn_tok_struct* _temp4250=( struct
Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4250[
0]=({ struct Cyc_Rgn_tok_struct _temp4251; _temp4251.tag= Cyc_Rgn_tok_tag;
_temp4251.f1=( void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); _temp4251;});
_temp4250;}); break; case 150: _LL4249: yyval=({ struct _tagged_ptr2 _temp4253=
yyvs; struct _xtunion_struct** _temp4255= _temp4253.curr + yyvsp_offset; if(
_temp4255 < _temp4253.base? 1: _temp4255 >= _temp4253.last_plus_one){ _throw(
Null_Exception);}* _temp4255;}); break; case 151: _LL4252: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeQual_tok_struct* _temp4257=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp4257[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp4258; _temp4258.tag= Cyc_TypeQual_tok_tag; _temp4258.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp4259= yyvs; struct
_xtunion_struct** _temp4261= _temp4259.curr +( yyvsp_offset - 1); if( _temp4261
< _temp4259.base? 1: _temp4261 >= _temp4259.last_plus_one){ _throw(
Null_Exception);}* _temp4261;})), Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2
_temp4262= yyvs; struct _xtunion_struct** _temp4264= _temp4262.curr +
yyvsp_offset; if( _temp4264 < _temp4262.base? 1: _temp4264 >= _temp4262.last_plus_one){
_throw( Null_Exception);}* _temp4264;}))); _temp4258;}); _temp4257;}); break;
case 152: _LL4256: yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp4266=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp4266[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp4267; _temp4267.tag= Cyc_ParamDeclListBool_tok_tag; _temp4267.f1=({ struct
_tuple16* _temp4268=( struct _tuple16*) GC_malloc( sizeof( struct _tuple16));
_temp4268->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2 _temp4269= yyvs; struct
_xtunion_struct** _temp4271= _temp4269.curr + yyvsp_offset; if( _temp4271 <
_temp4269.base? 1: _temp4271 >= _temp4269.last_plus_one){ _throw( Null_Exception);}*
_temp4271;}))); _temp4268->f2= 0; _temp4268->f3= 0; _temp4268;}); _temp4267;});
_temp4266;}); break; case 153: _LL4265: yyval=( struct _xtunion_struct*)({
struct Cyc_ParamDeclListBool_tok_struct* _temp4273=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp4273[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp4274; _temp4274.tag= Cyc_ParamDeclListBool_tok_tag;
_temp4274.f1=({ struct _tuple16* _temp4275=( struct _tuple16*) GC_malloc(
sizeof( struct _tuple16)); _temp4275->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2
_temp4276= yyvs; struct _xtunion_struct** _temp4278= _temp4276.curr +(
yyvsp_offset - 2); if( _temp4278 < _temp4276.base? 1: _temp4278 >= _temp4276.last_plus_one){
_throw( Null_Exception);}* _temp4278;}))); _temp4275->f2= 1; _temp4275->f3= 0;
_temp4275;}); _temp4274;}); _temp4273;}); break; case 154: _LL4272: yyval=(
struct _xtunion_struct*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp4280=(
struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp4280[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp4281; _temp4281.tag=
Cyc_ParamDeclListBool_tok_tag; _temp4281.f1=({ struct _tuple16* _temp4282=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp4282->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4289= yyvs; struct _xtunion_struct** _temp4291=
_temp4289.curr +( yyvsp_offset - 2); if( _temp4291 < _temp4289.base? 1:
_temp4291 >= _temp4289.last_plus_one){ _throw( Null_Exception);}* _temp4291;})));
_temp4282->f2= 0; _temp4282->f3=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp4283=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp4283->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4284=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4284[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4285; _temp4285.tag= Cyc_Absyn_JoinEff_tag;
_temp4285.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4286= yyvs;
struct _xtunion_struct** _temp4288= _temp4286.curr + yyvsp_offset; if( _temp4288
< _temp4286.base? 1: _temp4288 >= _temp4286.last_plus_one){ _throw(
Null_Exception);}* _temp4288;})); _temp4285;}); _temp4284;})); _temp4283;});
_temp4282;}); _temp4281;}); _temp4280;}); break; case 155: _LL4279: yyval=({
struct _tagged_ptr2 _temp4293= yyvs; struct _xtunion_struct** _temp4295=
_temp4293.curr + yyvsp_offset; if( _temp4295 < _temp4293.base? 1: _temp4295 >=
_temp4293.last_plus_one){ _throw( Null_Exception);}* _temp4295;}); break; case
156: _LL4292: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct*
_temp4297=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4297[ 0]=({ struct Cyc_TypeList_tok_struct _temp4298; _temp4298.tag= Cyc_TypeList_tok_tag;
_temp4298.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2
_temp4299= yyvs; struct _xtunion_struct** _temp4301= _temp4299.curr +(
yyvsp_offset - 2); if( _temp4301 < _temp4299.base? 1: _temp4301 >= _temp4299.last_plus_one){
_throw( Null_Exception);}* _temp4301;})), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4302= yyvs; struct _xtunion_struct** _temp4304= _temp4302.curr
+ yyvsp_offset; if( _temp4304 < _temp4302.base? 1: _temp4304 >= _temp4302.last_plus_one){
_throw( Null_Exception);}* _temp4304;}))); _temp4298;}); _temp4297;}); break;
case 157: _LL4296: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct*
_temp4306=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4306[ 0]=({ struct Cyc_TypeList_tok_struct _temp4307; _temp4307.tag= Cyc_TypeList_tok_tag;
_temp4307.f1= 0; _temp4307;}); _temp4306;}); break; case 158: _LL4305: yyval=({
struct _tagged_ptr2 _temp4309= yyvs; struct _xtunion_struct** _temp4311=
_temp4309.curr +( yyvsp_offset - 1); if( _temp4311 < _temp4309.base? 1:
_temp4311 >= _temp4309.last_plus_one){ _throw( Null_Exception);}* _temp4311;});
break; case 159: _LL4308: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct*
_temp4313=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4313[ 0]=({ struct Cyc_TypeList_tok_struct _temp4314; _temp4314.tag= Cyc_TypeList_tok_tag;
_temp4314.f1=({ struct Cyc_List_List* _temp4315=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4315->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4316= yyvs; struct
_xtunion_struct** _temp4318= _temp4316.curr + yyvsp_offset; if( _temp4318 <
_temp4316.base? 1: _temp4318 >= _temp4316.last_plus_one){ _throw( Null_Exception);}*
_temp4318;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_EffKind)); _temp4315->tl= 0; _temp4315;}); _temp4314;});
_temp4313;}); break; case 160: _LL4312: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp4320= yyvs; struct _xtunion_struct** _temp4322= _temp4320.curr
+ yyvsp_offset; if( _temp4322 < _temp4320.base? 1: _temp4322 >= _temp4320.last_plus_one){
_throw( Null_Exception);}* _temp4322;})) !=( void*) Cyc_Absyn_EffKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4323=( char*)"expecing effect kind (E)";
struct _tagged_string _temp4324; _temp4324.curr= _temp4323; _temp4324.base=
_temp4323; _temp4324.last_plus_one= _temp4323 + 25; _temp4324;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4325= yyls; struct Cyc_Yyltype* _temp4327= _temp4325.curr
+ yylsp_offset; if( _temp4327 < _temp4325.base? 1: _temp4327 >= _temp4325.last_plus_one){
_throw( Null_Exception);}* _temp4327;}).first_line,({ struct _tagged_ptr3
_temp4328= yyls; struct Cyc_Yyltype* _temp4330= _temp4328.curr + yylsp_offset;
if( _temp4330 < _temp4328.base? 1: _temp4330 >= _temp4328.last_plus_one){ _throw(
Null_Exception);}* _temp4330;}).last_line));} yyval=( struct _xtunion_struct*)({
struct Cyc_TypeList_tok_struct* _temp4331=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4331[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4332; _temp4332.tag= Cyc_TypeList_tok_tag; _temp4332.f1=({ struct Cyc_List_List*
_temp4333=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4333->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4334= yyvs; struct _xtunion_struct** _temp4336= _temp4334.curr
+( yyvsp_offset - 2); if( _temp4336 < _temp4334.base? 1: _temp4336 >= _temp4334.last_plus_one){
_throw( Null_Exception);}* _temp4336;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp4333->tl= 0; _temp4333;});
_temp4332;}); _temp4331;}); break; case 161: _LL4319: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4338=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4338[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4339; _temp4339.tag= Cyc_TypeList_tok_tag; _temp4339.f1=({ struct Cyc_List_List*
_temp4340=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4340->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4341=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4341[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4342; _temp4342.tag= Cyc_Absyn_AccessEff_tag;
_temp4342.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4343= yyvs; struct _xtunion_struct** _temp4345= _temp4343.curr
+ yyvsp_offset; if( _temp4345 < _temp4343.base? 1: _temp4345 >= _temp4343.last_plus_one){
_throw( Null_Exception);}* _temp4345;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp4342;}); _temp4341;}));
_temp4340->tl= 0; _temp4340;}); _temp4339;}); _temp4338;}); break; case 162:
_LL4337: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct*
_temp4347=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4347[ 0]=({ struct Cyc_TypeList_tok_struct _temp4348; _temp4348.tag= Cyc_TypeList_tok_tag;
_temp4348.f1=({ struct Cyc_List_List* _temp4349=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4349->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp4353=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp4353[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp4354; _temp4354.tag= Cyc_Absyn_AccessEff_tag;
_temp4354.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4355= yyvs; struct _xtunion_struct** _temp4357= _temp4355.curr
+( yyvsp_offset - 2); if( _temp4357 < _temp4355.base? 1: _temp4357 >= _temp4355.last_plus_one){
_throw( Null_Exception);}* _temp4357;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp4354;}); _temp4353;}));
_temp4349->tl= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4350= yyvs;
struct _xtunion_struct** _temp4352= _temp4350.curr + yyvsp_offset; if( _temp4352
< _temp4350.base? 1: _temp4352 >= _temp4350.last_plus_one){ _throw(
Null_Exception);}* _temp4352;})); _temp4349;}); _temp4348;}); _temp4347;});
break; case 163: _LL4346: if( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2
_temp4359= yyvs; struct _xtunion_struct** _temp4361= _temp4359.curr +
yyvsp_offset; if( _temp4361 < _temp4359.base? 1: _temp4361 >= _temp4359.last_plus_one){
_throw( Null_Exception);}* _temp4361;})) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4362=( char*)"expecting region kind (R)";
struct _tagged_string _temp4363; _temp4363.curr= _temp4362; _temp4363.base=
_temp4362; _temp4363.last_plus_one= _temp4362 + 26; _temp4363;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4364= yyls; struct Cyc_Yyltype* _temp4366= _temp4364.curr
+ yylsp_offset; if( _temp4366 < _temp4364.base? 1: _temp4366 >= _temp4364.last_plus_one){
_throw( Null_Exception);}* _temp4366;}).first_line,({ struct _tagged_ptr3
_temp4367= yyls; struct Cyc_Yyltype* _temp4369= _temp4367.curr + yylsp_offset;
if( _temp4369 < _temp4367.base? 1: _temp4369 >= _temp4367.last_plus_one){ _throw(
Null_Exception);}* _temp4369;}).last_line));} yyval=( struct _xtunion_struct*)({
struct Cyc_TypeList_tok_struct* _temp4370=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4370[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4371; _temp4371.tag= Cyc_TypeList_tok_tag; _temp4371.f1=({ struct Cyc_List_List*
_temp4372=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4372->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4373=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4373[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4374; _temp4374.tag= Cyc_Absyn_AccessEff_tag;
_temp4374.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4375= yyvs; struct _xtunion_struct** _temp4377= _temp4375.curr
+( yyvsp_offset - 2); if( _temp4377 < _temp4375.base? 1: _temp4377 >= _temp4375.last_plus_one){
_throw( Null_Exception);}* _temp4377;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp4374;}); _temp4373;}));
_temp4372->tl= 0; _temp4372;}); _temp4371;}); _temp4370;}); break; case 164:
_LL4358: if( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp4379= yyvs; struct
_xtunion_struct** _temp4381= _temp4379.curr +( yyvsp_offset - 2); if( _temp4381
< _temp4379.base? 1: _temp4381 >= _temp4379.last_plus_one){ _throw(
Null_Exception);}* _temp4381;})) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4382=( char*)"expecting region kind (R)";
struct _tagged_string _temp4383; _temp4383.curr= _temp4382; _temp4383.base=
_temp4382; _temp4383.last_plus_one= _temp4382 + 26; _temp4383;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4384= yyls; struct Cyc_Yyltype* _temp4386= _temp4384.curr
+( yylsp_offset - 2); if( _temp4386 < _temp4384.base? 1: _temp4386 >= _temp4384.last_plus_one){
_throw( Null_Exception);}* _temp4386;}).first_line,({ struct _tagged_ptr3
_temp4387= yyls; struct Cyc_Yyltype* _temp4389= _temp4387.curr +( yylsp_offset -
2); if( _temp4389 < _temp4387.base? 1: _temp4389 >= _temp4387.last_plus_one){
_throw( Null_Exception);}* _temp4389;}).last_line));} yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4390=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4390[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4391; _temp4391.tag= Cyc_TypeList_tok_tag; _temp4391.f1=({ struct Cyc_List_List*
_temp4392=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4392->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4396=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4396[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4397; _temp4397.tag= Cyc_Absyn_AccessEff_tag;
_temp4397.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4398= yyvs; struct _xtunion_struct** _temp4400= _temp4398.curr
+( yyvsp_offset - 4); if( _temp4400 < _temp4398.base? 1: _temp4400 >= _temp4398.last_plus_one){
_throw( Null_Exception);}* _temp4400;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp4397;}); _temp4396;}));
_temp4392->tl= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4393= yyvs;
struct _xtunion_struct** _temp4395= _temp4393.curr + yyvsp_offset; if( _temp4395
< _temp4393.base? 1: _temp4395 >= _temp4393.last_plus_one){ _throw(
Null_Exception);}* _temp4395;})); _temp4392;}); _temp4391;}); _temp4390;});
break; case 165: _LL4378: yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDeclList_tok_struct*
_temp4402=( struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp4402[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp4403; _temp4403.tag=
Cyc_ParamDeclList_tok_tag; _temp4403.f1=({ struct Cyc_List_List* _temp4404=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4404->hd=(
void*) Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4405= yyvs; struct
_xtunion_struct** _temp4407= _temp4405.curr + yyvsp_offset; if( _temp4407 <
_temp4405.base? 1: _temp4407 >= _temp4405.last_plus_one){ _throw( Null_Exception);}*
_temp4407;})); _temp4404->tl= 0; _temp4404;}); _temp4403;}); _temp4402;});
break; case 166: _LL4401: yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDeclList_tok_struct*
_temp4409=( struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp4409[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp4410; _temp4410.tag=
Cyc_ParamDeclList_tok_tag; _temp4410.f1=({ struct Cyc_List_List* _temp4411=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4411->hd=(
void*) Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4415= yyvs; struct
_xtunion_struct** _temp4417= _temp4415.curr + yyvsp_offset; if( _temp4417 <
_temp4415.base? 1: _temp4417 >= _temp4415.last_plus_one){ _throw( Null_Exception);}*
_temp4417;})); _temp4411->tl= Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2
_temp4412= yyvs; struct _xtunion_struct** _temp4414= _temp4412.curr +(
yyvsp_offset - 2); if( _temp4414 < _temp4412.base? 1: _temp4414 >= _temp4412.last_plus_one){
_throw( Null_Exception);}* _temp4414;})); _temp4411;}); _temp4410;}); _temp4409;});
break; case 167: _LL4408: { void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4419= yyvs; struct _xtunion_struct** _temp4421=
_temp4419.curr +( yyvsp_offset - 1); if( _temp4421 < _temp4419.base? 1:
_temp4421 >= _temp4419.last_plus_one){ _throw( Null_Exception);}* _temp4421;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4422= yyls; struct Cyc_Yyltype*
_temp4424= _temp4422.curr +( yylsp_offset - 1); if( _temp4424 < _temp4422.base?
1: _temp4424 >= _temp4422.last_plus_one){ _throw( Null_Exception);}* _temp4424;}).first_line,({
struct _tagged_ptr3 _temp4425= yyls; struct Cyc_Yyltype* _temp4427= _temp4425.curr
+( yylsp_offset - 1); if( _temp4427 < _temp4425.base? 1: _temp4427 >= _temp4425.last_plus_one){
_throw( Null_Exception);}* _temp4427;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4428= yyvs; struct
_xtunion_struct** _temp4430= _temp4428.curr +( yyvsp_offset - 1); if( _temp4430
< _temp4428.base? 1: _temp4430 >= _temp4428.last_plus_one){ _throw(
Null_Exception);}* _temp4430;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4431= yyvs; struct _xtunion_struct** _temp4433=
_temp4431.curr +( yyvsp_offset - 1); if( _temp4433 < _temp4431.base? 1:
_temp4433 >= _temp4431.last_plus_one){ _throw( Null_Exception);}* _temp4433;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp4434= yyvs; struct _xtunion_struct** _temp4436= _temp4434.curr +
yyvsp_offset; if( _temp4436 < _temp4434.base? 1: _temp4436 >= _temp4434.last_plus_one){
_throw( Null_Exception);}* _temp4436;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp4437=( char*)"parameter with bad type params"; struct _tagged_string
_temp4438; _temp4438.curr= _temp4437; _temp4438.base= _temp4437; _temp4438.last_plus_one=
_temp4437 + 31; _temp4438;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4439= yyls; struct Cyc_Yyltype* _temp4441= _temp4439.curr + yylsp_offset;
if( _temp4441 < _temp4439.base? 1: _temp4441 >= _temp4439.last_plus_one){ _throw(
Null_Exception);}* _temp4441;}).first_line,({ struct _tagged_ptr3 _temp4442=
yyls; struct Cyc_Yyltype* _temp4444= _temp4442.curr + yylsp_offset; if(
_temp4444 < _temp4442.base? 1: _temp4444 >= _temp4442.last_plus_one){ _throw(
Null_Exception);}* _temp4444;}).last_line));}{ struct _tuple1* q=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4445= yyvs; struct _xtunion_struct** _temp4447=
_temp4445.curr + yyvsp_offset; if( _temp4447 < _temp4445.base? 1: _temp4447 >=
_temp4445.last_plus_one){ _throw( Null_Exception);}* _temp4447;})))->id;{ void*
_temp4448=(* q).f1; struct Cyc_List_List* _temp4458; struct Cyc_List_List*
_temp4460; _LL4450: if(( int) _temp4448 == Cyc_Absyn_Loc_n){ goto _LL4451;}
else{ goto _LL4452;} _LL4452: if(( unsigned int) _temp4448 > 1u?(( struct
_tunion_struct*) _temp4448)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL4459: _temp4458=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp4448)->f1; if(
_temp4458 == 0){ goto _LL4453;} else{ goto _LL4454;}} else{ goto _LL4454;}
_LL4454: if(( unsigned int) _temp4448 > 1u?(( struct _tunion_struct*) _temp4448)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL4461: _temp4460=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp4448)->f1; if( _temp4460 == 0){ goto _LL4455;}
else{ goto _LL4456;}} else{ goto _LL4456;} _LL4456: goto _LL4457; _LL4451: goto
_LL4449; _LL4453: goto _LL4449; _LL4455: goto _LL4449; _LL4457: Cyc_Parse_err((
struct _tagged_string)({ char* _temp4462=( char*)"parameter cannot be qualified with a module name";
struct _tagged_string _temp4463; _temp4463.curr= _temp4462; _temp4463.base=
_temp4462; _temp4463.last_plus_one= _temp4462 + 49; _temp4463;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4464= yyls; struct Cyc_Yyltype* _temp4466= _temp4464.curr
+( yylsp_offset - 1); if( _temp4466 < _temp4464.base? 1: _temp4466 >= _temp4464.last_plus_one){
_throw( Null_Exception);}* _temp4466;}).first_line,({ struct _tagged_ptr3
_temp4467= yyls; struct Cyc_Yyltype* _temp4469= _temp4467.curr +( yylsp_offset -
1); if( _temp4469 < _temp4467.base? 1: _temp4469 >= _temp4467.last_plus_one){
_throw( Null_Exception);}* _temp4469;}).last_line)); goto _LL4449; _LL4449:;}{
struct Cyc_Core_Opt* idopt=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp4470=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp4470->v=( void*)(* q).f2; _temp4470;}); if( t_info.f4 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4471=( char*)"extra attributes on parameter, ignoring";
struct _tagged_string _temp4472; _temp4472.curr= _temp4471; _temp4472.base=
_temp4471; _temp4472.last_plus_one= _temp4471 + 40; _temp4472;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4473= yyls; struct Cyc_Yyltype* _temp4475= _temp4473.curr
+( yylsp_offset - 1); if( _temp4475 < _temp4473.base? 1: _temp4475 >= _temp4473.last_plus_one){
_throw( Null_Exception);}* _temp4475;}).first_line,({ struct _tagged_ptr3
_temp4476= yyls; struct Cyc_Yyltype* _temp4478= _temp4476.curr + yylsp_offset;
if( _temp4478 < _temp4476.base? 1: _temp4478 >= _temp4476.last_plus_one){ _throw(
Null_Exception);}* _temp4478;}).last_line));} if( t_info.f3 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4479=( char*)"extra type variables on parameter, ignoring";
struct _tagged_string _temp4480; _temp4480.curr= _temp4479; _temp4480.base=
_temp4479; _temp4480.last_plus_one= _temp4479 + 44; _temp4480;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4481= yyls; struct Cyc_Yyltype* _temp4483= _temp4481.curr
+( yylsp_offset - 1); if( _temp4483 < _temp4481.base? 1: _temp4483 >= _temp4481.last_plus_one){
_throw( Null_Exception);}* _temp4483;}).first_line,({ struct _tagged_ptr3
_temp4484= yyls; struct Cyc_Yyltype* _temp4486= _temp4484.curr + yylsp_offset;
if( _temp4486 < _temp4484.base? 1: _temp4486 >= _temp4484.last_plus_one){ _throw(
Null_Exception);}* _temp4486;}).last_line));} yyval=( struct _xtunion_struct*)({
struct Cyc_ParamDecl_tok_struct* _temp4487=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4487[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4488; _temp4488.tag= Cyc_ParamDecl_tok_tag;
_temp4488.f1=({ struct _tuple2* _temp4489=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4489->f1= idopt; _temp4489->f2= t_info.f1; _temp4489->f3=
t_info.f2; _temp4489;}); _temp4488;}); _temp4487;}); break;}}} case 168: _LL4418: {
void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp4491= yyvs; struct _xtunion_struct** _temp4493= _temp4491.curr
+ yyvsp_offset; if( _temp4493 < _temp4491.base? 1: _temp4493 >= _temp4491.last_plus_one){
_throw( Null_Exception);}* _temp4493;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4494= yyls; struct Cyc_Yyltype* _temp4496= _temp4494.curr
+ yylsp_offset; if( _temp4496 < _temp4494.base? 1: _temp4496 >= _temp4494.last_plus_one){
_throw( Null_Exception);}* _temp4496;}).first_line,({ struct _tagged_ptr3
_temp4497= yyls; struct Cyc_Yyltype* _temp4499= _temp4497.curr + yylsp_offset;
if( _temp4499 < _temp4497.base? 1: _temp4499 >= _temp4497.last_plus_one){ _throw(
Null_Exception);}* _temp4499;}).last_line)); struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4500= yyvs; struct _xtunion_struct** _temp4502=
_temp4500.curr + yyvsp_offset; if( _temp4502 < _temp4500.base? 1: _temp4502 >=
_temp4500.last_plus_one){ _throw( Null_Exception);}* _temp4502;}))).f3; struct
Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4503= yyvs; struct _xtunion_struct** _temp4505= _temp4503.curr +
yyvsp_offset; if( _temp4505 < _temp4503.base? 1: _temp4505 >= _temp4503.last_plus_one){
_throw( Null_Exception);}* _temp4505;}))).f1; if( atts != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4506=( char*)"bad attributes on parameter, ignoring";
struct _tagged_string _temp4507; _temp4507.curr= _temp4506; _temp4507.base=
_temp4506; _temp4507.last_plus_one= _temp4506 + 38; _temp4507;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4508= yyls; struct Cyc_Yyltype* _temp4510= _temp4508.curr
+ yylsp_offset; if( _temp4510 < _temp4508.base? 1: _temp4510 >= _temp4508.last_plus_one){
_throw( Null_Exception);}* _temp4510;}).first_line,({ struct _tagged_ptr3
_temp4511= yyls; struct Cyc_Yyltype* _temp4513= _temp4511.curr + yylsp_offset;
if( _temp4513 < _temp4511.base? 1: _temp4513 >= _temp4511.last_plus_one){ _throw(
Null_Exception);}* _temp4513;}).last_line));} yyval=( struct _xtunion_struct*)({
struct Cyc_ParamDecl_tok_struct* _temp4514=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4514[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4515; _temp4515.tag= Cyc_ParamDecl_tok_tag;
_temp4515.f1=({ struct _tuple2* _temp4516=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4516->f1= 0; _temp4516->f2= tq; _temp4516->f3= t;
_temp4516;}); _temp4515;}); _temp4514;}); break;} case 169: _LL4490: { void* t=
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4518= yyvs; struct _xtunion_struct** _temp4520= _temp4518.curr +(
yyvsp_offset - 1); if( _temp4520 < _temp4518.base? 1: _temp4520 >= _temp4518.last_plus_one){
_throw( Null_Exception);}* _temp4520;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4521= yyls; struct Cyc_Yyltype* _temp4523= _temp4521.curr
+( yylsp_offset - 1); if( _temp4523 < _temp4521.base? 1: _temp4523 >= _temp4521.last_plus_one){
_throw( Null_Exception);}* _temp4523;}).first_line,({ struct _tagged_ptr3
_temp4524= yyls; struct Cyc_Yyltype* _temp4526= _temp4524.curr +( yylsp_offset -
1); if( _temp4526 < _temp4524.base? 1: _temp4526 >= _temp4524.last_plus_one){
_throw( Null_Exception);}* _temp4526;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4527= yyvs; struct
_xtunion_struct** _temp4529= _temp4527.curr +( yyvsp_offset - 1); if( _temp4529
< _temp4527.base? 1: _temp4529 >= _temp4527.last_plus_one){ _throw(
Null_Exception);}* _temp4529;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4530= yyvs; struct _xtunion_struct** _temp4532=
_temp4530.curr +( yyvsp_offset - 1); if( _temp4532 < _temp4530.base? 1:
_temp4532 >= _temp4530.last_plus_one){ _throw( Null_Exception);}* _temp4532;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4533= yyvs; struct _xtunion_struct** _temp4535= _temp4533.curr
+ yyvsp_offset; if( _temp4535 < _temp4533.base? 1: _temp4535 >= _temp4533.last_plus_one){
_throw( Null_Exception);}* _temp4535;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp4536=( char*)"bad type parameters on formal argument, ignoring";
struct _tagged_string _temp4537; _temp4537.curr= _temp4536; _temp4537.base=
_temp4536; _temp4537.last_plus_one= _temp4536 + 49; _temp4537;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4538= yyls; struct Cyc_Yyltype* _temp4540= _temp4538.curr
+( yylsp_offset - 1); if( _temp4540 < _temp4538.base? 1: _temp4540 >= _temp4538.last_plus_one){
_throw( Null_Exception);}* _temp4540;}).first_line,({ struct _tagged_ptr3
_temp4541= yyls; struct Cyc_Yyltype* _temp4543= _temp4541.curr + yylsp_offset;
if( _temp4543 < _temp4541.base? 1: _temp4543 >= _temp4541.last_plus_one){ _throw(
Null_Exception);}* _temp4543;}).last_line));} if( t_info.f4 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4544=( char*)"bad attributes on parameter, ignoring";
struct _tagged_string _temp4545; _temp4545.curr= _temp4544; _temp4545.base=
_temp4544; _temp4545.last_plus_one= _temp4544 + 38; _temp4545;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4546= yyls; struct Cyc_Yyltype* _temp4548= _temp4546.curr
+( yylsp_offset - 1); if( _temp4548 < _temp4546.base? 1: _temp4548 >= _temp4546.last_plus_one){
_throw( Null_Exception);}* _temp4548;}).first_line,({ struct _tagged_ptr3
_temp4549= yyls; struct Cyc_Yyltype* _temp4551= _temp4549.curr + yylsp_offset;
if( _temp4551 < _temp4549.base? 1: _temp4551 >= _temp4549.last_plus_one){ _throw(
Null_Exception);}* _temp4551;}).last_line));} yyval=( struct _xtunion_struct*)({
struct Cyc_ParamDecl_tok_struct* _temp4552=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4552[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4553; _temp4553.tag= Cyc_ParamDecl_tok_tag;
_temp4553.f1=({ struct _tuple2* _temp4554=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4554->f1= 0; _temp4554->f2= t_info.f1; _temp4554->f3=
t_info.f2; _temp4554;}); _temp4553;}); _temp4552;}); break;} case 170: _LL4517:
yyval=( struct _xtunion_struct*)({ struct Cyc_IdList_tok_struct* _temp4556=(
struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4556[ 0]=({ struct Cyc_IdList_tok_struct _temp4557; _temp4557.tag= Cyc_IdList_tok_tag;
_temp4557.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp4558= yyvs; struct
_xtunion_struct** _temp4560= _temp4558.curr + yyvsp_offset; if( _temp4560 <
_temp4558.base? 1: _temp4560 >= _temp4558.last_plus_one){ _throw( Null_Exception);}*
_temp4560;}))); _temp4557;}); _temp4556;}); break; case 171: _LL4555: yyval=(
struct _xtunion_struct*)({ struct Cyc_IdList_tok_struct* _temp4562=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp4562[ 0]=({ struct Cyc_IdList_tok_struct
_temp4563; _temp4563.tag= Cyc_IdList_tok_tag; _temp4563.f1=({ struct Cyc_List_List*
_temp4564=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4564->hd=( void*)({ struct _tagged_string* _temp4565=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp4565[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4566= yyvs; struct
_xtunion_struct** _temp4568= _temp4566.curr + yyvsp_offset; if( _temp4568 <
_temp4566.base? 1: _temp4568 >= _temp4566.last_plus_one){ _throw( Null_Exception);}*
_temp4568;})); _temp4565;}); _temp4564->tl= 0; _temp4564;}); _temp4563;});
_temp4562;}); break; case 172: _LL4561: yyval=( struct _xtunion_struct*)({
struct Cyc_IdList_tok_struct* _temp4570=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp4570[ 0]=({ struct Cyc_IdList_tok_struct
_temp4571; _temp4571.tag= Cyc_IdList_tok_tag; _temp4571.f1=({ struct Cyc_List_List*
_temp4572=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4572->hd=( void*)({ struct _tagged_string* _temp4576=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp4576[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4577= yyvs; struct
_xtunion_struct** _temp4579= _temp4577.curr + yyvsp_offset; if( _temp4579 <
_temp4577.base? 1: _temp4579 >= _temp4577.last_plus_one){ _throw( Null_Exception);}*
_temp4579;})); _temp4576;}); _temp4572->tl= Cyc_yyget_IdList_tok(({ struct
_tagged_ptr2 _temp4573= yyvs; struct _xtunion_struct** _temp4575= _temp4573.curr
+( yyvsp_offset - 2); if( _temp4575 < _temp4573.base? 1: _temp4575 >= _temp4573.last_plus_one){
_throw( Null_Exception);}* _temp4575;})); _temp4572;}); _temp4571;}); _temp4570;});
break; case 173: _LL4569: yyval=({ struct _tagged_ptr2 _temp4581= yyvs; struct
_xtunion_struct** _temp4583= _temp4581.curr + yyvsp_offset; if( _temp4583 <
_temp4581.base? 1: _temp4583 >= _temp4581.last_plus_one){ _throw( Null_Exception);}*
_temp4583;}); break; case 174: _LL4580: yyval=({ struct _tagged_ptr2 _temp4585=
yyvs; struct _xtunion_struct** _temp4587= _temp4585.curr + yyvsp_offset; if(
_temp4587 < _temp4585.base? 1: _temp4587 >= _temp4585.last_plus_one){ _throw(
Null_Exception);}* _temp4587;}); break; case 175: _LL4584: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp4589=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4589[ 0]=({ struct Cyc_Exp_tok_struct
_temp4590; _temp4590.tag= Cyc_Exp_tok_tag; _temp4590.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4591=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4591[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4592; _temp4592.tag= Cyc_Absyn_UnresolvedMem_e_tag;
_temp4592.f1= 0; _temp4592.f2= 0; _temp4592;}); _temp4591;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4593= yyls; struct Cyc_Yyltype* _temp4595= _temp4593.curr
+( yylsp_offset - 1); if( _temp4595 < _temp4593.base? 1: _temp4595 >= _temp4593.last_plus_one){
_throw( Null_Exception);}* _temp4595;}).first_line,({ struct _tagged_ptr3
_temp4596= yyls; struct Cyc_Yyltype* _temp4598= _temp4596.curr + yylsp_offset;
if( _temp4598 < _temp4596.base? 1: _temp4598 >= _temp4596.last_plus_one){ _throw(
Null_Exception);}* _temp4598;}).last_line)); _temp4590;}); _temp4589;}); break;
case 176: _LL4588: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp4600=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4600[ 0]=({ struct Cyc_Exp_tok_struct _temp4601; _temp4601.tag= Cyc_Exp_tok_tag;
_temp4601.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp4602=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp4602[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp4603; _temp4603.tag= Cyc_Absyn_UnresolvedMem_e_tag; _temp4603.f1= 0;
_temp4603.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2 _temp4604= yyvs; struct
_xtunion_struct** _temp4606= _temp4604.curr +( yyvsp_offset - 1); if( _temp4606
< _temp4604.base? 1: _temp4606 >= _temp4604.last_plus_one){ _throw(
Null_Exception);}* _temp4606;}))); _temp4603;}); _temp4602;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4607= yyls; struct Cyc_Yyltype* _temp4609= _temp4607.curr
+( yylsp_offset - 2); if( _temp4609 < _temp4607.base? 1: _temp4609 >= _temp4607.last_plus_one){
_throw( Null_Exception);}* _temp4609;}).first_line,({ struct _tagged_ptr3
_temp4610= yyls; struct Cyc_Yyltype* _temp4612= _temp4610.curr + yylsp_offset;
if( _temp4612 < _temp4610.base? 1: _temp4612 >= _temp4610.last_plus_one){ _throw(
Null_Exception);}* _temp4612;}).last_line)); _temp4601;}); _temp4600;}); break;
case 177: _LL4599: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp4614=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4614[ 0]=({ struct Cyc_Exp_tok_struct _temp4615; _temp4615.tag= Cyc_Exp_tok_tag;
_temp4615.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp4616=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp4616[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp4617; _temp4617.tag= Cyc_Absyn_UnresolvedMem_e_tag; _temp4617.f1= 0;
_temp4617.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2 _temp4618= yyvs; struct
_xtunion_struct** _temp4620= _temp4618.curr +( yyvsp_offset - 2); if( _temp4620
< _temp4618.base? 1: _temp4620 >= _temp4618.last_plus_one){ _throw(
Null_Exception);}* _temp4620;}))); _temp4617;}); _temp4616;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4621= yyls; struct Cyc_Yyltype* _temp4623= _temp4621.curr
+( yylsp_offset - 3); if( _temp4623 < _temp4621.base? 1: _temp4623 >= _temp4621.last_plus_one){
_throw( Null_Exception);}* _temp4623;}).first_line,({ struct _tagged_ptr3
_temp4624= yyls; struct Cyc_Yyltype* _temp4626= _temp4624.curr + yylsp_offset;
if( _temp4626 < _temp4624.base? 1: _temp4626 >= _temp4624.last_plus_one){ _throw(
Null_Exception);}* _temp4626;}).last_line)); _temp4615;}); _temp4614;}); break;
case 178: _LL4613: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp4628=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4628[ 0]=({ struct Cyc_Exp_tok_struct _temp4629; _temp4629.tag= Cyc_Exp_tok_tag;
_temp4629.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Comprehension_e_struct*
_temp4630=( struct Cyc_Absyn_Comprehension_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Comprehension_e_struct)); _temp4630[ 0]=({ struct Cyc_Absyn_Comprehension_e_struct
_temp4631; _temp4631.tag= Cyc_Absyn_Comprehension_e_tag; _temp4631.f1= Cyc_Absyn_new_vardecl(({
struct _tuple1* _temp4632=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp4632->f1=( void*) Cyc_Absyn_Loc_n; _temp4632->f2=({ struct _tagged_string*
_temp4633=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp4633[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4634= yyvs;
struct _xtunion_struct** _temp4636= _temp4634.curr +( yyvsp_offset - 5); if(
_temp4636 < _temp4634.base? 1: _temp4636 >= _temp4634.last_plus_one){ _throw(
Null_Exception);}* _temp4636;})); _temp4633;}); _temp4632;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4637= yyls; struct Cyc_Yyltype* _temp4639= _temp4637.curr
+( yylsp_offset - 5); if( _temp4639 < _temp4637.base? 1: _temp4639 >= _temp4637.last_plus_one){
_throw( Null_Exception);}* _temp4639;}).first_line,({ struct _tagged_ptr3
_temp4640= yyls; struct Cyc_Yyltype* _temp4642= _temp4640.curr +( yylsp_offset -
5); if( _temp4642 < _temp4640.base? 1: _temp4642 >= _temp4640.last_plus_one){
_throw( Null_Exception);}* _temp4642;}).last_line))); _temp4631.f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4643= yyvs; struct _xtunion_struct** _temp4645=
_temp4643.curr +( yyvsp_offset - 3); if( _temp4645 < _temp4643.base? 1:
_temp4645 >= _temp4643.last_plus_one){ _throw( Null_Exception);}* _temp4645;}));
_temp4631.f3= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4646= yyvs; struct
_xtunion_struct** _temp4648= _temp4646.curr +( yyvsp_offset - 1); if( _temp4648
< _temp4646.base? 1: _temp4648 >= _temp4646.last_plus_one){ _throw(
Null_Exception);}* _temp4648;})); _temp4631;}); _temp4630;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4649= yyls; struct Cyc_Yyltype* _temp4651= _temp4649.curr
+( yylsp_offset - 7); if( _temp4651 < _temp4649.base? 1: _temp4651 >= _temp4649.last_plus_one){
_throw( Null_Exception);}* _temp4651;}).first_line,({ struct _tagged_ptr3
_temp4652= yyls; struct Cyc_Yyltype* _temp4654= _temp4652.curr + yylsp_offset;
if( _temp4654 < _temp4652.base? 1: _temp4654 >= _temp4652.last_plus_one){ _throw(
Null_Exception);}* _temp4654;}).last_line)); _temp4629;}); _temp4628;}); break;
case 179: _LL4627: yyval=( struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct*
_temp4656=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4656[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4657; _temp4657.tag= Cyc_InitializerList_tok_tag; _temp4657.f1=({ struct
Cyc_List_List* _temp4658=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4658->hd=( void*)({ struct _tuple18* _temp4659=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4659->f1= 0; _temp4659->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4660= yyvs; struct _xtunion_struct** _temp4662=
_temp4660.curr + yyvsp_offset; if( _temp4662 < _temp4660.base? 1: _temp4662 >=
_temp4660.last_plus_one){ _throw( Null_Exception);}* _temp4662;})); _temp4659;});
_temp4658->tl= 0; _temp4658;}); _temp4657;}); _temp4656;}); break; case 180:
_LL4655: yyval=( struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct*
_temp4664=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4664[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4665; _temp4665.tag= Cyc_InitializerList_tok_tag; _temp4665.f1=({ struct
Cyc_List_List* _temp4666=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4666->hd=( void*)({ struct _tuple18* _temp4667=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4667->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4671= yyvs; struct _xtunion_struct** _temp4673=
_temp4671.curr +( yyvsp_offset - 1); if( _temp4673 < _temp4671.base? 1:
_temp4673 >= _temp4671.last_plus_one){ _throw( Null_Exception);}* _temp4673;}));
_temp4667->f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4668= yyvs; struct
_xtunion_struct** _temp4670= _temp4668.curr + yyvsp_offset; if( _temp4670 <
_temp4668.base? 1: _temp4670 >= _temp4668.last_plus_one){ _throw( Null_Exception);}*
_temp4670;})); _temp4667;}); _temp4666->tl= 0; _temp4666;}); _temp4665;});
_temp4664;}); break; case 181: _LL4663: yyval=( struct _xtunion_struct*)({
struct Cyc_InitializerList_tok_struct* _temp4675=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp4675[ 0]=({
struct Cyc_InitializerList_tok_struct _temp4676; _temp4676.tag= Cyc_InitializerList_tok_tag;
_temp4676.f1=({ struct Cyc_List_List* _temp4677=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4677->hd=( void*)({ struct
_tuple18* _temp4681=( struct _tuple18*) GC_malloc( sizeof( struct _tuple18));
_temp4681->f1= 0; _temp4681->f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4682= yyvs; struct _xtunion_struct** _temp4684= _temp4682.curr +
yyvsp_offset; if( _temp4684 < _temp4682.base? 1: _temp4684 >= _temp4682.last_plus_one){
_throw( Null_Exception);}* _temp4684;})); _temp4681;}); _temp4677->tl= Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4678= yyvs; struct _xtunion_struct** _temp4680=
_temp4678.curr +( yyvsp_offset - 2); if( _temp4680 < _temp4678.base? 1:
_temp4680 >= _temp4678.last_plus_one){ _throw( Null_Exception);}* _temp4680;}));
_temp4677;}); _temp4676;}); _temp4675;}); break; case 182: _LL4674: yyval=(
struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct* _temp4686=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp4686[ 0]=({ struct Cyc_InitializerList_tok_struct _temp4687; _temp4687.tag=
Cyc_InitializerList_tok_tag; _temp4687.f1=({ struct Cyc_List_List* _temp4688=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4688->hd=(
void*)({ struct _tuple18* _temp4692=( struct _tuple18*) GC_malloc( sizeof(
struct _tuple18)); _temp4692->f1= Cyc_yyget_DesignatorList_tok(({ struct
_tagged_ptr2 _temp4696= yyvs; struct _xtunion_struct** _temp4698= _temp4696.curr
+( yyvsp_offset - 1); if( _temp4698 < _temp4696.base? 1: _temp4698 >= _temp4696.last_plus_one){
_throw( Null_Exception);}* _temp4698;})); _temp4692->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4693= yyvs; struct _xtunion_struct** _temp4695=
_temp4693.curr + yyvsp_offset; if( _temp4695 < _temp4693.base? 1: _temp4695 >=
_temp4693.last_plus_one){ _throw( Null_Exception);}* _temp4695;})); _temp4692;});
_temp4688->tl= Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2 _temp4689=
yyvs; struct _xtunion_struct** _temp4691= _temp4689.curr +( yyvsp_offset - 3);
if( _temp4691 < _temp4689.base? 1: _temp4691 >= _temp4689.last_plus_one){ _throw(
Null_Exception);}* _temp4691;})); _temp4688;}); _temp4687;}); _temp4686;});
break; case 183: _LL4685: yyval=( struct _xtunion_struct*)({ struct Cyc_DesignatorList_tok_struct*
_temp4700=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4700[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4701; _temp4701.tag=
Cyc_DesignatorList_tok_tag; _temp4701.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2
_temp4702= yyvs; struct _xtunion_struct** _temp4704= _temp4702.curr +(
yyvsp_offset - 1); if( _temp4704 < _temp4702.base? 1: _temp4704 >= _temp4702.last_plus_one){
_throw( Null_Exception);}* _temp4704;}))); _temp4701;}); _temp4700;}); break;
case 184: _LL4699: yyval=( struct _xtunion_struct*)({ struct Cyc_DesignatorList_tok_struct*
_temp4706=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4706[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4707; _temp4707.tag=
Cyc_DesignatorList_tok_tag; _temp4707.f1=({ struct Cyc_List_List* _temp4708=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4708->hd=(
void*) Cyc_yyget_Designator_tok(({ struct _tagged_ptr2 _temp4709= yyvs; struct
_xtunion_struct** _temp4711= _temp4709.curr + yyvsp_offset; if( _temp4711 <
_temp4709.base? 1: _temp4711 >= _temp4709.last_plus_one){ _throw( Null_Exception);}*
_temp4711;})); _temp4708->tl= 0; _temp4708;}); _temp4707;}); _temp4706;});
break; case 185: _LL4705: yyval=( struct _xtunion_struct*)({ struct Cyc_DesignatorList_tok_struct*
_temp4713=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4713[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4714; _temp4714.tag=
Cyc_DesignatorList_tok_tag; _temp4714.f1=({ struct Cyc_List_List* _temp4715=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4715->hd=(
void*) Cyc_yyget_Designator_tok(({ struct _tagged_ptr2 _temp4719= yyvs; struct
_xtunion_struct** _temp4721= _temp4719.curr + yyvsp_offset; if( _temp4721 <
_temp4719.base? 1: _temp4721 >= _temp4719.last_plus_one){ _throw( Null_Exception);}*
_temp4721;})); _temp4715->tl= Cyc_yyget_DesignatorList_tok(({ struct
_tagged_ptr2 _temp4716= yyvs; struct _xtunion_struct** _temp4718= _temp4716.curr
+( yyvsp_offset - 1); if( _temp4718 < _temp4716.base? 1: _temp4718 >= _temp4716.last_plus_one){
_throw( Null_Exception);}* _temp4718;})); _temp4715;}); _temp4714;}); _temp4713;});
break; case 186: _LL4712: yyval=( struct _xtunion_struct*)({ struct Cyc_Designator_tok_struct*
_temp4723=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4723[ 0]=({ struct Cyc_Designator_tok_struct _temp4724; _temp4724.tag= Cyc_Designator_tok_tag;
_temp4724.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp4725=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp4725[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp4726; _temp4726.tag=
Cyc_Absyn_ArrayElement_tag; _temp4726.f1= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4727= yyvs; struct _xtunion_struct** _temp4729= _temp4727.curr
+( yyvsp_offset - 1); if( _temp4729 < _temp4727.base? 1: _temp4729 >= _temp4727.last_plus_one){
_throw( Null_Exception);}* _temp4729;})); _temp4726;}); _temp4725;})); _temp4724;});
_temp4723;}); break; case 187: _LL4722: yyval=( struct _xtunion_struct*)({
struct Cyc_Designator_tok_struct* _temp4731=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp4731[ 0]=({ struct
Cyc_Designator_tok_struct _temp4732; _temp4732.tag= Cyc_Designator_tok_tag;
_temp4732.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp4733=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp4733[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp4734; _temp4734.tag= Cyc_Absyn_FieldName_tag;
_temp4734.f1=({ struct _tagged_string* _temp4735=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4735[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4736= yyvs; struct _xtunion_struct** _temp4738=
_temp4736.curr + yyvsp_offset; if( _temp4738 < _temp4736.base? 1: _temp4738 >=
_temp4736.last_plus_one){ _throw( Null_Exception);}* _temp4738;})); _temp4735;});
_temp4734;}); _temp4733;})); _temp4732;}); _temp4731;}); break; case 188:
_LL4730: { void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4740= yyvs; struct _xtunion_struct** _temp4742=
_temp4740.curr + yyvsp_offset; if( _temp4742 < _temp4740.base? 1: _temp4742 >=
_temp4740.last_plus_one){ _throw( Null_Exception);}* _temp4742;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4743= yyls; struct Cyc_Yyltype* _temp4745= _temp4743.curr
+ yylsp_offset; if( _temp4745 < _temp4743.base? 1: _temp4745 >= _temp4743.last_plus_one){
_throw( Null_Exception);}* _temp4745;}).first_line,({ struct _tagged_ptr3
_temp4746= yyls; struct Cyc_Yyltype* _temp4748= _temp4746.curr + yylsp_offset;
if( _temp4748 < _temp4746.base? 1: _temp4748 >= _temp4746.last_plus_one){ _throw(
Null_Exception);}* _temp4748;}).last_line)); struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4749= yyvs; struct _xtunion_struct** _temp4751=
_temp4749.curr + yyvsp_offset; if( _temp4751 < _temp4749.base? 1: _temp4751 >=
_temp4749.last_plus_one){ _throw( Null_Exception);}* _temp4751;}))).f3; if( atts
!= 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4752=( char*)"ignoring attributes in type";
struct _tagged_string _temp4753; _temp4753.curr= _temp4752; _temp4753.base=
_temp4752; _temp4753.last_plus_one= _temp4752 + 28; _temp4753;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4754= yyls; struct Cyc_Yyltype* _temp4756= _temp4754.curr
+ yylsp_offset; if( _temp4756 < _temp4754.base? 1: _temp4756 >= _temp4754.last_plus_one){
_throw( Null_Exception);}* _temp4756;}).first_line,({ struct _tagged_ptr3
_temp4757= yyls; struct Cyc_Yyltype* _temp4759= _temp4757.curr + yylsp_offset;
if( _temp4759 < _temp4757.base? 1: _temp4759 >= _temp4757.last_plus_one){ _throw(
Null_Exception);}* _temp4759;}).last_line));}{ struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4760= yyvs; struct _xtunion_struct** _temp4762=
_temp4760.curr + yyvsp_offset; if( _temp4762 < _temp4760.base? 1: _temp4762 >=
_temp4760.last_plus_one){ _throw( Null_Exception);}* _temp4762;}))).f1; yyval=(
struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4763=( struct
Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4763[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4764; _temp4764.tag= Cyc_ParamDecl_tok_tag;
_temp4764.f1=({ struct _tuple2* _temp4765=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2) * 1); _temp4765[ 0]=({ struct _tuple2 _temp4766; _temp4766.f1= 0;
_temp4766.f2= tq; _temp4766.f3= t; _temp4766;}); _temp4765;}); _temp4764;});
_temp4763;}); break;}} case 189: _LL4739: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4768= yyvs; struct
_xtunion_struct** _temp4770= _temp4768.curr +( yyvsp_offset - 1); if( _temp4770
< _temp4768.base? 1: _temp4770 >= _temp4768.last_plus_one){ _throw(
Null_Exception);}* _temp4770;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4771= yyls; struct Cyc_Yyltype* _temp4773= _temp4771.curr +(
yylsp_offset - 1); if( _temp4773 < _temp4771.base? 1: _temp4773 >= _temp4771.last_plus_one){
_throw( Null_Exception);}* _temp4773;}).first_line,({ struct _tagged_ptr3
_temp4774= yyls; struct Cyc_Yyltype* _temp4776= _temp4774.curr +( yylsp_offset -
1); if( _temp4776 < _temp4774.base? 1: _temp4776 >= _temp4774.last_plus_one){
_throw( Null_Exception);}* _temp4776;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4777= yyvs; struct
_xtunion_struct** _temp4779= _temp4777.curr +( yyvsp_offset - 1); if( _temp4779
< _temp4777.base? 1: _temp4779 >= _temp4777.last_plus_one){ _throw(
Null_Exception);}* _temp4779;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4780= yyvs; struct _xtunion_struct** _temp4782=
_temp4780.curr +( yyvsp_offset - 1); if( _temp4782 < _temp4780.base? 1:
_temp4782 >= _temp4780.last_plus_one){ _throw( Null_Exception);}* _temp4782;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4783= yyvs; struct _xtunion_struct** _temp4785= _temp4783.curr
+ yyvsp_offset; if( _temp4785 < _temp4783.base? 1: _temp4785 >= _temp4783.last_plus_one){
_throw( Null_Exception);}* _temp4785;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp4786=( char*)"bad type params, ignoring"; struct _tagged_string
_temp4787; _temp4787.curr= _temp4786; _temp4787.base= _temp4786; _temp4787.last_plus_one=
_temp4786 + 26; _temp4787;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4788= yyls; struct Cyc_Yyltype* _temp4790= _temp4788.curr + yylsp_offset;
if( _temp4790 < _temp4788.base? 1: _temp4790 >= _temp4788.last_plus_one){ _throw(
Null_Exception);}* _temp4790;}).first_line,({ struct _tagged_ptr3 _temp4791=
yyls; struct Cyc_Yyltype* _temp4793= _temp4791.curr + yylsp_offset; if(
_temp4793 < _temp4791.base? 1: _temp4793 >= _temp4791.last_plus_one){ _throw(
Null_Exception);}* _temp4793;}).last_line));} if( t_info.f4 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4794=( char*)"bad specifiers, ignoring";
struct _tagged_string _temp4795; _temp4795.curr= _temp4794; _temp4795.base=
_temp4794; _temp4795.last_plus_one= _temp4794 + 25; _temp4795;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4796= yyls; struct Cyc_Yyltype* _temp4798= _temp4796.curr
+ yylsp_offset; if( _temp4798 < _temp4796.base? 1: _temp4798 >= _temp4796.last_plus_one){
_throw( Null_Exception);}* _temp4798;}).first_line,({ struct _tagged_ptr3
_temp4799= yyls; struct Cyc_Yyltype* _temp4801= _temp4799.curr + yylsp_offset;
if( _temp4801 < _temp4799.base? 1: _temp4801 >= _temp4799.last_plus_one){ _throw(
Null_Exception);}* _temp4801;}).last_line));} yyval=( struct _xtunion_struct*)({
struct Cyc_ParamDecl_tok_struct* _temp4802=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4802[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4803; _temp4803.tag= Cyc_ParamDecl_tok_tag;
_temp4803.f1=({ struct _tuple2* _temp4804=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4804->f1= 0; _temp4804->f2= t_info.f1; _temp4804->f3=
t_info.f2; _temp4804;}); _temp4803;}); _temp4802;}); break;} case 190: _LL4767:
yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct* _temp4806=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4806[ 0]=({ struct Cyc_Type_tok_struct _temp4807; _temp4807.tag= Cyc_Type_tok_tag;
_temp4807.f1=( void*)(* Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4808=
yyvs; struct _xtunion_struct** _temp4810= _temp4808.curr + yyvsp_offset; if(
_temp4810 < _temp4808.base? 1: _temp4810 >= _temp4808.last_plus_one){ _throw(
Null_Exception);}* _temp4810;}))).f3; _temp4807;}); _temp4806;}); break; case
191: _LL4805: yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct*
_temp4812=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4812[ 0]=({ struct Cyc_Type_tok_struct _temp4813; _temp4813.tag= Cyc_Type_tok_tag;
_temp4813.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4814=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4814[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4815; _temp4815.tag= Cyc_Absyn_JoinEff_tag;
_temp4815.f1= 0; _temp4815;}); _temp4814;})); _temp4813;}); _temp4812;}); break;
case 192: _LL4811: yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct*
_temp4817=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4817[ 0]=({ struct Cyc_Type_tok_struct _temp4818; _temp4818.tag= Cyc_Type_tok_tag;
_temp4818.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4819=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4819[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4820; _temp4820.tag= Cyc_Absyn_JoinEff_tag;
_temp4820.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4821= yyvs;
struct _xtunion_struct** _temp4823= _temp4821.curr +( yyvsp_offset - 1); if(
_temp4823 < _temp4821.base? 1: _temp4823 >= _temp4821.last_plus_one){ _throw(
Null_Exception);}* _temp4823;})); _temp4820;}); _temp4819;})); _temp4818;});
_temp4817;}); break; case 193: _LL4816: yyval=( struct _xtunion_struct*)({
struct Cyc_Type_tok_struct* _temp4825=( struct Cyc_Type_tok_struct*) GC_malloc(
sizeof( struct Cyc_Type_tok_struct)); _temp4825[ 0]=({ struct Cyc_Type_tok_struct
_temp4826; _temp4826.tag= Cyc_Type_tok_tag; _temp4826.f1=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp4827=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4827[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4828; _temp4828.tag= Cyc_Absyn_JoinEff_tag;
_temp4828.f1=({ struct Cyc_List_List* _temp4829=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4829->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4833= yyvs; struct _xtunion_struct** _temp4835=
_temp4833.curr +( yyvsp_offset - 2); if( _temp4835 < _temp4833.base? 1:
_temp4835 >= _temp4833.last_plus_one){ _throw( Null_Exception);}* _temp4835;}));
_temp4829->tl= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4830= yyvs;
struct _xtunion_struct** _temp4832= _temp4830.curr + yyvsp_offset; if( _temp4832
< _temp4830.base? 1: _temp4832 >= _temp4830.last_plus_one){ _throw(
Null_Exception);}* _temp4832;})); _temp4829;}); _temp4828;}); _temp4827;}));
_temp4826;}); _temp4825;}); break; case 194: _LL4824: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4837=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4837[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4838; _temp4838.tag= Cyc_TypeList_tok_tag; _temp4838.f1=({ struct Cyc_List_List*
_temp4839=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4839->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4840= yyvs;
struct _xtunion_struct** _temp4842= _temp4840.curr + yyvsp_offset; if( _temp4842
< _temp4840.base? 1: _temp4842 >= _temp4840.last_plus_one){ _throw(
Null_Exception);}* _temp4842;})); _temp4839->tl= 0; _temp4839;}); _temp4838;});
_temp4837;}); break; case 195: _LL4836: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeList_tok_struct* _temp4844=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4844[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4845; _temp4845.tag= Cyc_TypeList_tok_tag; _temp4845.f1=({ struct Cyc_List_List*
_temp4846=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4846->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4850= yyvs;
struct _xtunion_struct** _temp4852= _temp4850.curr + yyvsp_offset; if( _temp4852
< _temp4850.base? 1: _temp4852 >= _temp4850.last_plus_one){ _throw(
Null_Exception);}* _temp4852;})); _temp4846->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4847= yyvs; struct _xtunion_struct** _temp4849=
_temp4847.curr +( yyvsp_offset - 2); if( _temp4849 < _temp4847.base? 1:
_temp4849 >= _temp4847.last_plus_one){ _throw( Null_Exception);}* _temp4849;}));
_temp4846;}); _temp4845;}); _temp4844;}); break; case 196: _LL4843: yyval=(
struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4854=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4854[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4855; _temp4855.tag=
Cyc_AbstractDeclarator_tok_tag; _temp4855.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4856=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4856->tms= Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp4857=
yyvs; struct _xtunion_struct** _temp4859= _temp4857.curr + yyvsp_offset; if(
_temp4859 < _temp4857.base? 1: _temp4859 >= _temp4857.last_plus_one){ _throw(
Null_Exception);}* _temp4859;})); _temp4856;}); _temp4855;}); _temp4854;});
break; case 197: _LL4853: yyval=({ struct _tagged_ptr2 _temp4861= yyvs; struct
_xtunion_struct** _temp4863= _temp4861.curr + yyvsp_offset; if( _temp4863 <
_temp4861.base? 1: _temp4863 >= _temp4861.last_plus_one){ _throw( Null_Exception);}*
_temp4863;}); break; case 198: _LL4860: yyval=( struct _xtunion_struct*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp4865=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4865[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp4866; _temp4866.tag= Cyc_AbstractDeclarator_tok_tag;
_temp4866.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4867=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4867->tms=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp4868= yyvs; struct
_xtunion_struct** _temp4870= _temp4868.curr +( yyvsp_offset - 1); if( _temp4870
< _temp4868.base? 1: _temp4870 >= _temp4868.last_plus_one){ _throw(
Null_Exception);}* _temp4870;})),( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4871= yyvs; struct _xtunion_struct** _temp4873= _temp4871.curr
+ yyvsp_offset; if( _temp4873 < _temp4871.base? 1: _temp4873 >= _temp4871.last_plus_one){
_throw( Null_Exception);}* _temp4873;})))->tms); _temp4867;}); _temp4866;});
_temp4865;}); break; case 199: _LL4864: yyval=({ struct _tagged_ptr2 _temp4875=
yyvs; struct _xtunion_struct** _temp4877= _temp4875.curr +( yyvsp_offset - 1);
if( _temp4877 < _temp4875.base? 1: _temp4877 >= _temp4875.last_plus_one){ _throw(
Null_Exception);}* _temp4877;}); break; case 200: _LL4874: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4879=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4879[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4880; _temp4880.tag=
Cyc_AbstractDeclarator_tok_tag; _temp4880.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4881=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4881->tms=({ struct Cyc_List_List* _temp4882=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4882->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4882->tl= 0; _temp4882;}); _temp4881;}); _temp4880;}); _temp4879;}); break;
case 201: _LL4878: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4884=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4884[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4885; _temp4885.tag= Cyc_AbstractDeclarator_tok_tag; _temp4885.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4886=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4886->tms=({
struct Cyc_List_List* _temp4887=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4887->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4887->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4888= yyvs; struct _xtunion_struct** _temp4890= _temp4888.curr +(
yyvsp_offset - 2); if( _temp4890 < _temp4888.base? 1: _temp4890 >= _temp4888.last_plus_one){
_throw( Null_Exception);}* _temp4890;})))->tms; _temp4887;}); _temp4886;});
_temp4885;}); _temp4884;}); break; case 202: _LL4883: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4892=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4892[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4893; _temp4893.tag=
Cyc_AbstractDeclarator_tok_tag; _temp4893.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4894=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4894->tms=({ struct Cyc_List_List* _temp4895=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4895->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp4896=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp4896[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp4897; _temp4897.tag= Cyc_Absyn_ConstArray_mod_tag;
_temp4897.f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4898= yyvs; struct
_xtunion_struct** _temp4900= _temp4898.curr +( yyvsp_offset - 1); if( _temp4900
< _temp4898.base? 1: _temp4900 >= _temp4898.last_plus_one){ _throw(
Null_Exception);}* _temp4900;})); _temp4897;}); _temp4896;})); _temp4895->tl= 0;
_temp4895;}); _temp4894;}); _temp4893;}); _temp4892;}); break; case 203: _LL4891:
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4902=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4902[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4903; _temp4903.tag= Cyc_AbstractDeclarator_tok_tag; _temp4903.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4904=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4904->tms=({
struct Cyc_List_List* _temp4905=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4905->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4909=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4909[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp4910; _temp4910.tag= Cyc_Absyn_ConstArray_mod_tag; _temp4910.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4911= yyvs; struct _xtunion_struct** _temp4913=
_temp4911.curr +( yyvsp_offset - 1); if( _temp4913 < _temp4911.base? 1:
_temp4913 >= _temp4911.last_plus_one){ _throw( Null_Exception);}* _temp4913;}));
_temp4910;}); _temp4909;})); _temp4905->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4906= yyvs; struct _xtunion_struct** _temp4908=
_temp4906.curr +( yyvsp_offset - 3); if( _temp4908 < _temp4906.base? 1:
_temp4908 >= _temp4906.last_plus_one){ _throw( Null_Exception);}* _temp4908;})))->tms;
_temp4905;}); _temp4904;}); _temp4903;}); _temp4902;}); break; case 204: _LL4901:
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4915=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4915[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4916; _temp4916.tag= Cyc_AbstractDeclarator_tok_tag; _temp4916.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4917=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4917->tms=({
struct Cyc_List_List* _temp4918=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4918->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4919=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4919[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4920; _temp4920.tag=
Cyc_Absyn_Function_mod_tag; _temp4920.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4921=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4921[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4922; _temp4922.tag= Cyc_Absyn_WithTypes_tag;
_temp4922.f1= 0; _temp4922.f2= 0; _temp4922.f3= 0; _temp4922;}); _temp4921;}));
_temp4920;}); _temp4919;})); _temp4918->tl= 0; _temp4918;}); _temp4917;});
_temp4916;}); _temp4915;}); break; case 205: _LL4914: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4924=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4924[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4925; _temp4925.tag=
Cyc_AbstractDeclarator_tok_tag; _temp4925.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4926=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4926->tms=({ struct Cyc_List_List* _temp4927=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4927->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4928=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4928[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4929; _temp4929.tag= Cyc_Absyn_Function_mod_tag;
_temp4929.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp4930=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4930[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4931; _temp4931.tag= Cyc_Absyn_WithTypes_tag;
_temp4931.f1= 0; _temp4931.f2= 0; _temp4931.f3=({ struct Cyc_Core_Opt* _temp4932=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4932->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4933=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4933[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4934; _temp4934.tag= Cyc_Absyn_JoinEff_tag;
_temp4934.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4935= yyvs;
struct _xtunion_struct** _temp4937= _temp4935.curr +( yyvsp_offset - 1); if(
_temp4937 < _temp4935.base? 1: _temp4937 >= _temp4935.last_plus_one){ _throw(
Null_Exception);}* _temp4937;})); _temp4934;}); _temp4933;})); _temp4932;});
_temp4931;}); _temp4930;})); _temp4929;}); _temp4928;})); _temp4927->tl= 0;
_temp4927;}); _temp4926;}); _temp4925;}); _temp4924;}); break; case 206: _LL4923: {
struct _tuple16 _temp4944; struct Cyc_Core_Opt* _temp4945; int _temp4947; struct
Cyc_List_List* _temp4949; struct _tuple16* _temp4942= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp4939= yyvs; struct _xtunion_struct** _temp4941=
_temp4939.curr +( yyvsp_offset - 1); if( _temp4941 < _temp4939.base? 1:
_temp4941 >= _temp4939.last_plus_one){ _throw( Null_Exception);}* _temp4941;}));
_temp4944=* _temp4942; _LL4950: _temp4949= _temp4944.f1; goto _LL4948; _LL4948:
_temp4947= _temp4944.f2; goto _LL4946; _LL4946: _temp4945= _temp4944.f3; goto
_LL4943; _LL4943: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4951=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4951[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4952; _temp4952.tag= Cyc_AbstractDeclarator_tok_tag; _temp4952.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4953=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4953->tms=({
struct Cyc_List_List* _temp4954=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4954->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4955=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4955[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4956; _temp4956.tag=
Cyc_Absyn_Function_mod_tag; _temp4956.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4957=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4957[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4958; _temp4958.tag= Cyc_Absyn_WithTypes_tag;
_temp4958.f1= _temp4949; _temp4958.f2= _temp4947; _temp4958.f3= _temp4945;
_temp4958;}); _temp4957;})); _temp4956;}); _temp4955;})); _temp4954->tl= 0;
_temp4954;}); _temp4953;}); _temp4952;}); _temp4951;}); break;} case 207:
_LL4938: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4960=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4960[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4961; _temp4961.tag= Cyc_AbstractDeclarator_tok_tag; _temp4961.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4962=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4962->tms=({
struct Cyc_List_List* _temp4963=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4963->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4967=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4967[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4968; _temp4968.tag=
Cyc_Absyn_Function_mod_tag; _temp4968.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4969=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4969[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4970; _temp4970.tag= Cyc_Absyn_WithTypes_tag;
_temp4970.f1= 0; _temp4970.f2= 0; _temp4970.f3= 0; _temp4970;}); _temp4969;}));
_temp4968;}); _temp4967;})); _temp4963->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4964= yyvs; struct _xtunion_struct** _temp4966=
_temp4964.curr +( yyvsp_offset - 2); if( _temp4966 < _temp4964.base? 1:
_temp4966 >= _temp4964.last_plus_one){ _throw( Null_Exception);}* _temp4966;})))->tms;
_temp4963;}); _temp4962;}); _temp4961;}); _temp4960;}); break; case 208: _LL4959: {
struct Cyc_Core_Opt* eff=({ struct Cyc_Core_Opt* _temp4972=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4972->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp4973=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4973[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4974; _temp4974.tag= Cyc_Absyn_JoinEff_tag;
_temp4974.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4975= yyvs;
struct _xtunion_struct** _temp4977= _temp4975.curr +( yyvsp_offset - 1); if(
_temp4977 < _temp4975.base? 1: _temp4977 >= _temp4975.last_plus_one){ _throw(
Null_Exception);}* _temp4977;})); _temp4974;}); _temp4973;})); _temp4972;});
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4978=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4978[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4979; _temp4979.tag= Cyc_AbstractDeclarator_tok_tag; _temp4979.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4980=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4980->tms=({
struct Cyc_List_List* _temp4981=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4981->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4985=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4985[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4986; _temp4986.tag=
Cyc_Absyn_Function_mod_tag; _temp4986.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4987=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4987[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4988; _temp4988.tag= Cyc_Absyn_WithTypes_tag;
_temp4988.f1= 0; _temp4988.f2= 0; _temp4988.f3= eff; _temp4988;}); _temp4987;}));
_temp4986;}); _temp4985;})); _temp4981->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4982= yyvs; struct _xtunion_struct** _temp4984=
_temp4982.curr +( yyvsp_offset - 4); if( _temp4984 < _temp4982.base? 1:
_temp4984 >= _temp4982.last_plus_one){ _throw( Null_Exception);}* _temp4984;})))->tms;
_temp4981;}); _temp4980;}); _temp4979;}); _temp4978;}); break;} case 209:
_LL4971: { struct _tuple16 _temp4995; struct Cyc_Core_Opt* _temp4996; int
_temp4998; struct Cyc_List_List* _temp5000; struct _tuple16* _temp4993= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp4990= yyvs; struct _xtunion_struct** _temp4992=
_temp4990.curr +( yyvsp_offset - 1); if( _temp4992 < _temp4990.base? 1:
_temp4992 >= _temp4990.last_plus_one){ _throw( Null_Exception);}* _temp4992;}));
_temp4995=* _temp4993; _LL5001: _temp5000= _temp4995.f1; goto _LL4999; _LL4999:
_temp4998= _temp4995.f2; goto _LL4997; _LL4997: _temp4996= _temp4995.f3; goto
_LL4994; _LL4994: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5002=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5002[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5003; _temp5003.tag= Cyc_AbstractDeclarator_tok_tag; _temp5003.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp5004=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5004->tms=({
struct Cyc_List_List* _temp5005=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5005->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp5009=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp5009[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp5010; _temp5010.tag=
Cyc_Absyn_Function_mod_tag; _temp5010.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp5011=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5011[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5012; _temp5012.tag= Cyc_Absyn_WithTypes_tag;
_temp5012.f1= _temp5000; _temp5012.f2= _temp4998; _temp5012.f3= _temp4996;
_temp5012;}); _temp5011;})); _temp5010;}); _temp5009;})); _temp5005->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5006= yyvs; struct _xtunion_struct** _temp5008=
_temp5006.curr +( yyvsp_offset - 3); if( _temp5008 < _temp5006.base? 1:
_temp5008 >= _temp5006.last_plus_one){ _throw( Null_Exception);}* _temp5008;})))->tms;
_temp5005;}); _temp5004;}); _temp5003;}); _temp5002;}); break;} case 210:
_LL4989: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5014= yyls; struct Cyc_Yyltype* _temp5016= _temp5014.curr
+( yylsp_offset - 2); if( _temp5016 < _temp5014.base? 1: _temp5016 >= _temp5014.last_plus_one){
_throw( Null_Exception);}* _temp5016;}).first_line,({ struct _tagged_ptr3
_temp5017= yyls; struct Cyc_Yyltype* _temp5019= _temp5017.curr + yylsp_offset;
if( _temp5019 < _temp5017.base? 1: _temp5019 >= _temp5017.last_plus_one){ _throw(
Null_Exception);}* _temp5019;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp5020= yyvs; struct _xtunion_struct** _temp5022= _temp5020.curr
+( yyvsp_offset - 1); if( _temp5022 < _temp5020.base? 1: _temp5022 >= _temp5020.last_plus_one){
_throw( Null_Exception);}* _temp5022;})))); yyval=( struct _xtunion_struct*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp5023=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5023[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5024; _temp5024.tag= Cyc_AbstractDeclarator_tok_tag;
_temp5024.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5025=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5025->tms=({
struct Cyc_List_List* _temp5026=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5026->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp5030=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp5030[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp5031; _temp5031.tag= Cyc_Absyn_TypeParams_mod_tag; _temp5031.f1= ts;
_temp5031.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5032= yyls;
struct Cyc_Yyltype* _temp5034= _temp5032.curr +( yylsp_offset - 2); if(
_temp5034 < _temp5032.base? 1: _temp5034 >= _temp5032.last_plus_one){ _throw(
Null_Exception);}* _temp5034;}).first_line,({ struct _tagged_ptr3 _temp5035=
yyls; struct Cyc_Yyltype* _temp5037= _temp5035.curr + yylsp_offset; if(
_temp5037 < _temp5035.base? 1: _temp5037 >= _temp5035.last_plus_one){ _throw(
Null_Exception);}* _temp5037;}).last_line); _temp5031.f3= 0; _temp5031;});
_temp5030;})); _temp5026->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp5027= yyvs; struct _xtunion_struct** _temp5029= _temp5027.curr
+( yyvsp_offset - 3); if( _temp5029 < _temp5027.base? 1: _temp5029 >= _temp5027.last_plus_one){
_throw( Null_Exception);}* _temp5029;})))->tms; _temp5026;}); _temp5025;});
_temp5024;}); _temp5023;}); break;} case 211: _LL5013:(( struct Cyc_Lexing_lexbuf*)
Cyc_Parse_lbuf->v)->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5039= yyls; struct Cyc_Yyltype* _temp5041= _temp5039.curr
+( yylsp_offset - 2); if( _temp5041 < _temp5039.base? 1: _temp5041 >= _temp5039.last_plus_one){
_throw( Null_Exception);}* _temp5041;}).first_line,({ struct _tagged_ptr3
_temp5042= yyls; struct Cyc_Yyltype* _temp5044= _temp5042.curr + yylsp_offset;
if( _temp5044 < _temp5042.base? 1: _temp5044 >= _temp5042.last_plus_one){ _throw(
Null_Exception);}* _temp5044;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp5045= yyvs; struct _xtunion_struct** _temp5047= _temp5045.curr
+( yyvsp_offset - 1); if( _temp5047 < _temp5045.base? 1: _temp5047 >= _temp5045.last_plus_one){
_throw( Null_Exception);}* _temp5047;})))); yyval=( struct _xtunion_struct*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp5048=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5048[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5049; _temp5049.tag= Cyc_AbstractDeclarator_tok_tag;
_temp5049.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5050=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5050->tms=({
struct Cyc_List_List* _temp5051=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5051->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp5055=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp5055[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp5056; _temp5056.tag= Cyc_Absyn_TypeParams_mod_tag; _temp5056.f1= ts;
_temp5056.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5057= yyls;
struct Cyc_Yyltype* _temp5059= _temp5057.curr +( yylsp_offset - 2); if(
_temp5059 < _temp5057.base? 1: _temp5059 >= _temp5057.last_plus_one){ _throw(
Null_Exception);}* _temp5059;}).first_line,({ struct _tagged_ptr3 _temp5060=
yyls; struct Cyc_Yyltype* _temp5062= _temp5060.curr + yylsp_offset; if(
_temp5062 < _temp5060.base? 1: _temp5062 >= _temp5060.last_plus_one){ _throw(
Null_Exception);}* _temp5062;}).last_line); _temp5056.f3= 0; _temp5056;});
_temp5055;})); _temp5051->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp5052= yyvs; struct _xtunion_struct** _temp5054= _temp5052.curr
+( yyvsp_offset - 3); if( _temp5054 < _temp5052.base? 1: _temp5054 >= _temp5052.last_plus_one){
_throw( Null_Exception);}* _temp5054;})))->tms; _temp5051;}); _temp5050;});
_temp5049;}); _temp5048;}); break;} case 212: _LL5038: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5064=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5064[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5065; _temp5065.tag=
Cyc_AbstractDeclarator_tok_tag; _temp5065.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5066=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5066->tms=({ struct Cyc_List_List* _temp5067=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5067->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp5071=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp5071[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp5072; _temp5072.tag= Cyc_Absyn_Attributes_mod_tag;
_temp5072.f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5073= yyls;
struct Cyc_Yyltype* _temp5075= _temp5073.curr + yylsp_offset; if( _temp5075 <
_temp5073.base? 1: _temp5075 >= _temp5073.last_plus_one){ _throw( Null_Exception);}*
_temp5075;}).first_line,({ struct _tagged_ptr3 _temp5076= yyls; struct Cyc_Yyltype*
_temp5078= _temp5076.curr + yylsp_offset; if( _temp5078 < _temp5076.base? 1:
_temp5078 >= _temp5076.last_plus_one){ _throw( Null_Exception);}* _temp5078;}).last_line);
_temp5072.f2= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp5079= yyvs;
struct _xtunion_struct** _temp5081= _temp5079.curr + yyvsp_offset; if( _temp5081
< _temp5079.base? 1: _temp5081 >= _temp5079.last_plus_one){ _throw(
Null_Exception);}* _temp5081;})); _temp5072;}); _temp5071;})); _temp5067->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp5068= yyvs; struct
_xtunion_struct** _temp5070= _temp5068.curr +( yyvsp_offset - 1); if( _temp5070
< _temp5068.base? 1: _temp5070 >= _temp5068.last_plus_one){ _throw(
Null_Exception);}* _temp5070;})))->tms; _temp5067;}); _temp5066;}); _temp5065;});
_temp5064;}); break; case 213: _LL5063: yyval=({ struct _tagged_ptr2 _temp5083=
yyvs; struct _xtunion_struct** _temp5085= _temp5083.curr + yyvsp_offset; if(
_temp5085 < _temp5083.base? 1: _temp5085 >= _temp5083.last_plus_one){ _throw(
Null_Exception);}* _temp5085;}); break; case 214: _LL5082: yyval=({ struct
_tagged_ptr2 _temp5087= yyvs; struct _xtunion_struct** _temp5089= _temp5087.curr
+ yyvsp_offset; if( _temp5089 < _temp5087.base? 1: _temp5089 >= _temp5087.last_plus_one){
_throw( Null_Exception);}* _temp5089;}); break; case 215: _LL5086: yyval=({
struct _tagged_ptr2 _temp5091= yyvs; struct _xtunion_struct** _temp5093=
_temp5091.curr + yyvsp_offset; if( _temp5093 < _temp5091.base? 1: _temp5093 >=
_temp5091.last_plus_one){ _throw( Null_Exception);}* _temp5093;}); break; case
216: _LL5090: yyval=({ struct _tagged_ptr2 _temp5095= yyvs; struct
_xtunion_struct** _temp5097= _temp5095.curr + yyvsp_offset; if( _temp5097 <
_temp5095.base? 1: _temp5097 >= _temp5095.last_plus_one){ _throw( Null_Exception);}*
_temp5097;}); break; case 217: _LL5094: yyval=({ struct _tagged_ptr2 _temp5099=
yyvs; struct _xtunion_struct** _temp5101= _temp5099.curr + yyvsp_offset; if(
_temp5101 < _temp5099.base? 1: _temp5101 >= _temp5099.last_plus_one){ _throw(
Null_Exception);}* _temp5101;}); break; case 218: _LL5098: yyval=({ struct
_tagged_ptr2 _temp5103= yyvs; struct _xtunion_struct** _temp5105= _temp5103.curr
+ yyvsp_offset; if( _temp5105 < _temp5103.base? 1: _temp5105 >= _temp5103.last_plus_one){
_throw( Null_Exception);}* _temp5105;}); break; case 219: _LL5102: yyval=(
struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5107=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5107[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5108; _temp5108.tag= Cyc_Stmt_tok_tag; _temp5108.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Cut_s_struct* _temp5109=( struct Cyc_Absyn_Cut_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct)); _temp5109[ 0]=({ struct Cyc_Absyn_Cut_s_struct
_temp5110; _temp5110.tag= Cyc_Absyn_Cut_s_tag; _temp5110.f1= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5111= yyvs; struct _xtunion_struct** _temp5113=
_temp5111.curr + yyvsp_offset; if( _temp5113 < _temp5111.base? 1: _temp5113 >=
_temp5111.last_plus_one){ _throw( Null_Exception);}* _temp5113;})); _temp5110;});
_temp5109;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5114= yyls;
struct Cyc_Yyltype* _temp5116= _temp5114.curr +( yylsp_offset - 1); if(
_temp5116 < _temp5114.base? 1: _temp5116 >= _temp5114.last_plus_one){ _throw(
Null_Exception);}* _temp5116;}).first_line,({ struct _tagged_ptr3 _temp5117=
yyls; struct Cyc_Yyltype* _temp5119= _temp5117.curr + yylsp_offset; if(
_temp5119 < _temp5117.base? 1: _temp5119 >= _temp5117.last_plus_one){ _throw(
Null_Exception);}* _temp5119;}).last_line)); _temp5108;}); _temp5107;}); break;
case 220: _LL5106: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5121=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5121[ 0]=({ struct Cyc_Stmt_tok_struct _temp5122; _temp5122.tag= Cyc_Stmt_tok_tag;
_temp5122.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp5123=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp5123[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp5124; _temp5124.tag= Cyc_Absyn_Splice_s_tag;
_temp5124.f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5125= yyvs; struct
_xtunion_struct** _temp5127= _temp5125.curr + yyvsp_offset; if( _temp5127 <
_temp5125.base? 1: _temp5127 >= _temp5125.last_plus_one){ _throw( Null_Exception);}*
_temp5127;})); _temp5124;}); _temp5123;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5128= yyls; struct Cyc_Yyltype* _temp5130= _temp5128.curr +(
yylsp_offset - 1); if( _temp5130 < _temp5128.base? 1: _temp5130 >= _temp5128.last_plus_one){
_throw( Null_Exception);}* _temp5130;}).first_line,({ struct _tagged_ptr3
_temp5131= yyls; struct Cyc_Yyltype* _temp5133= _temp5131.curr + yylsp_offset;
if( _temp5133 < _temp5131.base? 1: _temp5133 >= _temp5131.last_plus_one){ _throw(
Null_Exception);}* _temp5133;}).last_line)); _temp5122;}); _temp5121;}); break;
case 221: _LL5120: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5135=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5135[ 0]=({ struct Cyc_Stmt_tok_struct _temp5136; _temp5136.tag= Cyc_Stmt_tok_tag;
_temp5136.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp5137=( struct Cyc_Absyn_Label_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp5137[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp5138; _temp5138.tag= Cyc_Absyn_Label_s_tag;
_temp5138.f1=({ struct _tagged_string* _temp5139=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp5139[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp5140= yyvs; struct _xtunion_struct** _temp5142=
_temp5140.curr +( yyvsp_offset - 2); if( _temp5142 < _temp5140.base? 1:
_temp5142 >= _temp5140.last_plus_one){ _throw( Null_Exception);}* _temp5142;}));
_temp5139;}); _temp5138.f2= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5143=
yyvs; struct _xtunion_struct** _temp5145= _temp5143.curr + yyvsp_offset; if(
_temp5145 < _temp5143.base? 1: _temp5145 >= _temp5143.last_plus_one){ _throw(
Null_Exception);}* _temp5145;})); _temp5138;}); _temp5137;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5146= yyls; struct Cyc_Yyltype* _temp5148= _temp5146.curr
+( yylsp_offset - 2); if( _temp5148 < _temp5146.base? 1: _temp5148 >= _temp5146.last_plus_one){
_throw( Null_Exception);}* _temp5148;}).first_line,({ struct _tagged_ptr3
_temp5149= yyls; struct Cyc_Yyltype* _temp5151= _temp5149.curr + yylsp_offset;
if( _temp5151 < _temp5149.base? 1: _temp5151 >= _temp5149.last_plus_one){ _throw(
Null_Exception);}* _temp5151;}).last_line)); _temp5136;}); _temp5135;}); break;
case 222: _LL5134: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5153=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5153[ 0]=({ struct Cyc_Stmt_tok_struct _temp5154; _temp5154.tag= Cyc_Stmt_tok_tag;
_temp5154.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5155= yyls; struct Cyc_Yyltype* _temp5157= _temp5155.curr +
yylsp_offset; if( _temp5157 < _temp5155.base? 1: _temp5157 >= _temp5155.last_plus_one){
_throw( Null_Exception);}* _temp5157;}).first_line,({ struct _tagged_ptr3
_temp5158= yyls; struct Cyc_Yyltype* _temp5160= _temp5158.curr + yylsp_offset;
if( _temp5160 < _temp5158.base? 1: _temp5160 >= _temp5158.last_plus_one){ _throw(
Null_Exception);}* _temp5160;}).last_line)); _temp5154;}); _temp5153;}); break;
case 223: _LL5152: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5162=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5162[ 0]=({ struct Cyc_Stmt_tok_struct _temp5163; _temp5163.tag= Cyc_Stmt_tok_tag;
_temp5163.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5164= yyvs; struct _xtunion_struct** _temp5166= _temp5164.curr +(
yyvsp_offset - 1); if( _temp5166 < _temp5164.base? 1: _temp5166 >= _temp5164.last_plus_one){
_throw( Null_Exception);}* _temp5166;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5167= yyls; struct Cyc_Yyltype* _temp5169= _temp5167.curr +(
yylsp_offset - 1); if( _temp5169 < _temp5167.base? 1: _temp5169 >= _temp5167.last_plus_one){
_throw( Null_Exception);}* _temp5169;}).first_line,({ struct _tagged_ptr3
_temp5170= yyls; struct Cyc_Yyltype* _temp5172= _temp5170.curr + yylsp_offset;
if( _temp5172 < _temp5170.base? 1: _temp5172 >= _temp5170.last_plus_one){ _throw(
Null_Exception);}* _temp5172;}).last_line)); _temp5163;}); _temp5162;}); break;
case 224: _LL5161: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5174=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5174[ 0]=({ struct Cyc_Stmt_tok_struct _temp5175; _temp5175.tag= Cyc_Stmt_tok_tag;
_temp5175.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5176= yyls; struct Cyc_Yyltype* _temp5178= _temp5176.curr +(
yylsp_offset - 1); if( _temp5178 < _temp5176.base? 1: _temp5178 >= _temp5176.last_plus_one){
_throw( Null_Exception);}* _temp5178;}).first_line,({ struct _tagged_ptr3
_temp5179= yyls; struct Cyc_Yyltype* _temp5181= _temp5179.curr + yylsp_offset;
if( _temp5181 < _temp5179.base? 1: _temp5181 >= _temp5179.last_plus_one){ _throw(
Null_Exception);}* _temp5181;}).last_line)); _temp5175;}); _temp5174;}); break;
case 225: _LL5173: yyval=({ struct _tagged_ptr2 _temp5183= yyvs; struct
_xtunion_struct** _temp5185= _temp5183.curr +( yyvsp_offset - 1); if( _temp5185
< _temp5183.base? 1: _temp5185 >= _temp5183.last_plus_one){ _throw(
Null_Exception);}* _temp5185;}); break; case 226: _LL5182:{ void* _temp5190= Cyc_yyget_BlockItem_tok(({
struct _tagged_ptr2 _temp5187= yyvs; struct _xtunion_struct** _temp5189=
_temp5187.curr + yyvsp_offset; if( _temp5189 < _temp5187.base? 1: _temp5189 >=
_temp5187.last_plus_one){ _throw( Null_Exception);}* _temp5189;})); struct Cyc_List_List*
_temp5198; struct Cyc_Absyn_Fndecl* _temp5200; struct Cyc_Absyn_Stmt* _temp5202;
_LL5192: if((( struct _tunion_struct*) _temp5190)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL5199: _temp5198=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp5190)->f1; goto _LL5193;} else{ goto _LL5194;} _LL5194: if((( struct
_tunion_struct*) _temp5190)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL5201: _temp5200=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp5190)->f1;
goto _LL5195;} else{ goto _LL5196;} _LL5196: if((( struct _tunion_struct*)
_temp5190)->tag == Cyc_Parse_Stmt_bl_tag){ _LL5203: _temp5202=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp5190)->f1; goto _LL5197;} else{ goto
_LL5191;} _LL5193: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5204=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5204[ 0]=({ struct Cyc_Stmt_tok_struct _temp5205; _temp5205.tag= Cyc_Stmt_tok_tag;
_temp5205.f1= Cyc_Parse_flatten_declarations( _temp5198, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5206= yyls; struct Cyc_Yyltype*
_temp5208= _temp5206.curr + yylsp_offset; if( _temp5208 < _temp5206.base? 1:
_temp5208 >= _temp5206.last_plus_one){ _throw( Null_Exception);}* _temp5208;}).first_line,({
struct _tagged_ptr3 _temp5209= yyls; struct Cyc_Yyltype* _temp5211= _temp5209.curr
+ yylsp_offset; if( _temp5211 < _temp5209.base? 1: _temp5211 >= _temp5209.last_plus_one){
_throw( Null_Exception);}* _temp5211;}).last_line))); _temp5205;}); _temp5204;});
goto _LL5191; _LL5195: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5212=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5212[ 0]=({ struct Cyc_Stmt_tok_struct _temp5213; _temp5213.tag= Cyc_Stmt_tok_tag;
_temp5213.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp5214=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5214[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5215; _temp5215.tag= Cyc_Absyn_Fn_d_tag;
_temp5215.f1= _temp5200; _temp5215;}); _temp5214;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5216= yyls; struct Cyc_Yyltype* _temp5218= _temp5216.curr
+ yylsp_offset; if( _temp5218 < _temp5216.base? 1: _temp5218 >= _temp5216.last_plus_one){
_throw( Null_Exception);}* _temp5218;}).first_line,({ struct _tagged_ptr3
_temp5219= yyls; struct Cyc_Yyltype* _temp5221= _temp5219.curr + yylsp_offset;
if( _temp5221 < _temp5219.base? 1: _temp5221 >= _temp5219.last_plus_one){ _throw(
Null_Exception);}* _temp5221;}).last_line)), Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5222= yyls; struct Cyc_Yyltype* _temp5224= _temp5222.curr
+ yylsp_offset; if( _temp5224 < _temp5222.base? 1: _temp5224 >= _temp5222.last_plus_one){
_throw( Null_Exception);}* _temp5224;}).first_line,({ struct _tagged_ptr3
_temp5225= yyls; struct Cyc_Yyltype* _temp5227= _temp5225.curr + yylsp_offset;
if( _temp5227 < _temp5225.base? 1: _temp5227 >= _temp5225.last_plus_one){ _throw(
Null_Exception);}* _temp5227;}).last_line))); _temp5213;}); _temp5212;}); goto
_LL5191; _LL5197: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5228=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5228[ 0]=({ struct Cyc_Stmt_tok_struct _temp5229; _temp5229.tag= Cyc_Stmt_tok_tag;
_temp5229.f1= _temp5202; _temp5229;}); _temp5228;}); goto _LL5191; _LL5191:;}
break; case 227: _LL5186:{ void* _temp5234= Cyc_yyget_BlockItem_tok(({ struct
_tagged_ptr2 _temp5231= yyvs; struct _xtunion_struct** _temp5233= _temp5231.curr
+( yyvsp_offset - 1); if( _temp5233 < _temp5231.base? 1: _temp5233 >= _temp5231.last_plus_one){
_throw( Null_Exception);}* _temp5233;})); struct Cyc_List_List* _temp5242;
struct Cyc_Absyn_Fndecl* _temp5244; struct Cyc_Absyn_Stmt* _temp5246; _LL5236:
if((( struct _tunion_struct*) _temp5234)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL5243: _temp5242=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp5234)->f1; goto _LL5237;} else{ goto _LL5238;} _LL5238: if((( struct
_tunion_struct*) _temp5234)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL5245: _temp5244=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp5234)->f1;
goto _LL5239;} else{ goto _LL5240;} _LL5240: if((( struct _tunion_struct*)
_temp5234)->tag == Cyc_Parse_Stmt_bl_tag){ _LL5247: _temp5246=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp5234)->f1; goto _LL5241;} else{ goto
_LL5235;} _LL5237: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5248=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5248[ 0]=({ struct Cyc_Stmt_tok_struct _temp5249; _temp5249.tag= Cyc_Stmt_tok_tag;
_temp5249.f1= Cyc_Parse_flatten_declarations( _temp5242, Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5250= yyvs; struct _xtunion_struct** _temp5252=
_temp5250.curr + yyvsp_offset; if( _temp5252 < _temp5250.base? 1: _temp5252 >=
_temp5250.last_plus_one){ _throw( Null_Exception);}* _temp5252;}))); _temp5249;});
_temp5248;}); goto _LL5235; _LL5239: yyval=( struct _xtunion_struct*)({ struct
Cyc_Stmt_tok_struct* _temp5253=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5253[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5254; _temp5254.tag= Cyc_Stmt_tok_tag; _temp5254.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp5255=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5255[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5256; _temp5256.tag= Cyc_Absyn_Fn_d_tag;
_temp5256.f1= _temp5244; _temp5256;}); _temp5255;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5257= yyls; struct Cyc_Yyltype* _temp5259= _temp5257.curr
+( yylsp_offset - 1); if( _temp5259 < _temp5257.base? 1: _temp5259 >= _temp5257.last_plus_one){
_throw( Null_Exception);}* _temp5259;}).first_line,({ struct _tagged_ptr3
_temp5260= yyls; struct Cyc_Yyltype* _temp5262= _temp5260.curr +( yylsp_offset -
1); if( _temp5262 < _temp5260.base? 1: _temp5262 >= _temp5260.last_plus_one){
_throw( Null_Exception);}* _temp5262;}).last_line)), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5263= yyvs; struct _xtunion_struct** _temp5265=
_temp5263.curr + yyvsp_offset; if( _temp5265 < _temp5263.base? 1: _temp5265 >=
_temp5263.last_plus_one){ _throw( Null_Exception);}* _temp5265;}))); _temp5254;});
_temp5253;}); goto _LL5235; _LL5241: yyval=( struct _xtunion_struct*)({ struct
Cyc_Stmt_tok_struct* _temp5266=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5266[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5267; _temp5267.tag= Cyc_Stmt_tok_tag; _temp5267.f1= Cyc_Absyn_seq_stmt(
_temp5246, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5268= yyvs; struct
_xtunion_struct** _temp5270= _temp5268.curr + yyvsp_offset; if( _temp5270 <
_temp5268.base? 1: _temp5270 >= _temp5268.last_plus_one){ _throw( Null_Exception);}*
_temp5270;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5271=
yyls; struct Cyc_Yyltype* _temp5273= _temp5271.curr +( yylsp_offset - 1); if(
_temp5273 < _temp5271.base? 1: _temp5273 >= _temp5271.last_plus_one){ _throw(
Null_Exception);}* _temp5273;}).first_line,({ struct _tagged_ptr3 _temp5274=
yyls; struct Cyc_Yyltype* _temp5276= _temp5274.curr + yylsp_offset; if(
_temp5276 < _temp5274.base? 1: _temp5276 >= _temp5274.last_plus_one){ _throw(
Null_Exception);}* _temp5276;}).last_line)); _temp5267;}); _temp5266;}); goto
_LL5235; _LL5235:;} break; case 228: _LL5230: yyval=( struct _xtunion_struct*)({
struct Cyc_BlockItem_tok_struct* _temp5278=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp5278[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp5279; _temp5279.tag= Cyc_BlockItem_tok_tag;
_temp5279.f1=( void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp5280=(
struct Cyc_Parse_TopDecls_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct));
_temp5280[ 0]=({ struct Cyc_Parse_TopDecls_bl_struct _temp5281; _temp5281.tag=
Cyc_Parse_TopDecls_bl_tag; _temp5281.f1= Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp5282= yyvs; struct _xtunion_struct** _temp5284= _temp5282.curr
+ yyvsp_offset; if( _temp5284 < _temp5282.base? 1: _temp5284 >= _temp5282.last_plus_one){
_throw( Null_Exception);}* _temp5284;})); _temp5281;}); _temp5280;})); _temp5279;});
_temp5278;}); break; case 229: _LL5277: yyval=( struct _xtunion_struct*)({
struct Cyc_BlockItem_tok_struct* _temp5286=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp5286[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp5287; _temp5287.tag= Cyc_BlockItem_tok_tag;
_temp5287.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp5288=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp5288[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp5289; _temp5289.tag= Cyc_Parse_Stmt_bl_tag;
_temp5289.f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5290= yyvs; struct
_xtunion_struct** _temp5292= _temp5290.curr + yyvsp_offset; if( _temp5292 <
_temp5290.base? 1: _temp5292 >= _temp5290.last_plus_one){ _throw( Null_Exception);}*
_temp5292;})); _temp5289;}); _temp5288;})); _temp5287;}); _temp5286;}); break;
case 230: _LL5285: yyval=( struct _xtunion_struct*)({ struct Cyc_BlockItem_tok_struct*
_temp5294=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp5294[ 0]=({ struct Cyc_BlockItem_tok_struct _temp5295; _temp5295.tag= Cyc_BlockItem_tok_tag;
_temp5295.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp5296=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp5296[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp5297; _temp5297.tag= Cyc_Parse_FnDecl_bl_tag;
_temp5297.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp5298= yyvs;
struct _xtunion_struct** _temp5300= _temp5298.curr + yyvsp_offset; if( _temp5300
< _temp5298.base? 1: _temp5300 >= _temp5298.last_plus_one){ _throw(
Null_Exception);}* _temp5300;})); _temp5297;}); _temp5296;})); _temp5295;});
_temp5294;}); break; case 231: _LL5293: yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5302=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5302[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5303; _temp5303.tag= Cyc_Stmt_tok_tag; _temp5303.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5304= yyvs; struct _xtunion_struct**
_temp5306= _temp5304.curr +( yyvsp_offset - 2); if( _temp5306 < _temp5304.base?
1: _temp5306 >= _temp5304.last_plus_one){ _throw( Null_Exception);}* _temp5306;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5307= yyvs; struct
_xtunion_struct** _temp5309= _temp5307.curr + yyvsp_offset; if( _temp5309 <
_temp5307.base? 1: _temp5309 >= _temp5307.last_plus_one){ _throw( Null_Exception);}*
_temp5309;})), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5310= yyls; struct Cyc_Yyltype* _temp5312= _temp5310.curr +(
yylsp_offset - 4); if( _temp5312 < _temp5310.base? 1: _temp5312 >= _temp5310.last_plus_one){
_throw( Null_Exception);}* _temp5312;}).first_line,({ struct _tagged_ptr3
_temp5313= yyls; struct Cyc_Yyltype* _temp5315= _temp5313.curr + yylsp_offset;
if( _temp5315 < _temp5313.base? 1: _temp5315 >= _temp5313.last_plus_one){ _throw(
Null_Exception);}* _temp5315;}).last_line)); _temp5303;}); _temp5302;}); break;
case 232: _LL5301: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5317=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5317[ 0]=({ struct Cyc_Stmt_tok_struct _temp5318; _temp5318.tag= Cyc_Stmt_tok_tag;
_temp5318.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5319= yyvs; struct _xtunion_struct** _temp5321= _temp5319.curr
+( yyvsp_offset - 4); if( _temp5321 < _temp5319.base? 1: _temp5321 >= _temp5319.last_plus_one){
_throw( Null_Exception);}* _temp5321;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5322= yyvs; struct _xtunion_struct** _temp5324= _temp5322.curr
+( yyvsp_offset - 2); if( _temp5324 < _temp5322.base? 1: _temp5324 >= _temp5322.last_plus_one){
_throw( Null_Exception);}* _temp5324;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5325= yyvs; struct _xtunion_struct** _temp5327= _temp5325.curr
+ yyvsp_offset; if( _temp5327 < _temp5325.base? 1: _temp5327 >= _temp5325.last_plus_one){
_throw( Null_Exception);}* _temp5327;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5328= yyls; struct Cyc_Yyltype* _temp5330= _temp5328.curr +(
yylsp_offset - 6); if( _temp5330 < _temp5328.base? 1: _temp5330 >= _temp5328.last_plus_one){
_throw( Null_Exception);}* _temp5330;}).first_line,({ struct _tagged_ptr3
_temp5331= yyls; struct Cyc_Yyltype* _temp5333= _temp5331.curr + yylsp_offset;
if( _temp5333 < _temp5331.base? 1: _temp5333 >= _temp5331.last_plus_one){ _throw(
Null_Exception);}* _temp5333;}).last_line)); _temp5318;}); _temp5317;}); break;
case 233: _LL5316: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5335=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5335[ 0]=({ struct Cyc_Stmt_tok_struct _temp5336; _temp5336.tag= Cyc_Stmt_tok_tag;
_temp5336.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5337= yyvs; struct _xtunion_struct** _temp5339= _temp5337.curr +(
yyvsp_offset - 4); if( _temp5339 < _temp5337.base? 1: _temp5339 >= _temp5337.last_plus_one){
_throw( Null_Exception);}* _temp5339;})), Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp5340= yyvs; struct _xtunion_struct** _temp5342=
_temp5340.curr +( yyvsp_offset - 1); if( _temp5342 < _temp5340.base? 1:
_temp5342 >= _temp5340.last_plus_one){ _throw( Null_Exception);}* _temp5342;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5343= yyls; struct Cyc_Yyltype*
_temp5345= _temp5343.curr +( yylsp_offset - 6); if( _temp5345 < _temp5343.base?
1: _temp5345 >= _temp5343.last_plus_one){ _throw( Null_Exception);}* _temp5345;}).first_line,({
struct _tagged_ptr3 _temp5346= yyls; struct Cyc_Yyltype* _temp5348= _temp5346.curr
+ yylsp_offset; if( _temp5348 < _temp5346.base? 1: _temp5348 >= _temp5346.last_plus_one){
_throw( Null_Exception);}* _temp5348;}).last_line)); _temp5336;}); _temp5335;});
break; case 234: _LL5334: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5350=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5350[ 0]=({ struct Cyc_Stmt_tok_struct _temp5351; _temp5351.tag= Cyc_Stmt_tok_tag;
_temp5351.f1= Cyc_Absyn_trycatch_stmt( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5352= yyvs; struct _xtunion_struct** _temp5354= _temp5352.curr +(
yyvsp_offset - 4); if( _temp5354 < _temp5352.base? 1: _temp5354 >= _temp5352.last_plus_one){
_throw( Null_Exception);}* _temp5354;})), Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp5355= yyvs; struct _xtunion_struct** _temp5357=
_temp5355.curr +( yyvsp_offset - 1); if( _temp5357 < _temp5355.base? 1:
_temp5357 >= _temp5355.last_plus_one){ _throw( Null_Exception);}* _temp5357;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5358= yyls; struct Cyc_Yyltype*
_temp5360= _temp5358.curr +( yylsp_offset - 5); if( _temp5360 < _temp5358.base?
1: _temp5360 >= _temp5358.last_plus_one){ _throw( Null_Exception);}* _temp5360;}).first_line,({
struct _tagged_ptr3 _temp5361= yyls; struct Cyc_Yyltype* _temp5363= _temp5361.curr
+ yylsp_offset; if( _temp5363 < _temp5361.base? 1: _temp5363 >= _temp5361.last_plus_one){
_throw( Null_Exception);}* _temp5363;}).last_line)); _temp5351;}); _temp5350;});
break; case 235: _LL5349: yyval=( struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp5365=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp5365[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp5366; _temp5366.tag= Cyc_SwitchClauseList_tok_tag; _temp5366.f1= 0;
_temp5366;}); _temp5365;}); break; case 236: _LL5364: yyval=( struct
_xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5368=( struct
Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5368[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5369; _temp5369.tag=
Cyc_SwitchClauseList_tok_tag; _temp5369.f1=({ struct Cyc_List_List* _temp5370=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5370->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5371=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5371->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5381= yyls; struct Cyc_Yyltype* _temp5383= _temp5381.curr +( yylsp_offset -
2); if( _temp5383 < _temp5381.base? 1: _temp5383 >= _temp5381.last_plus_one){
_throw( Null_Exception);}* _temp5383;}).first_line,({ struct _tagged_ptr3
_temp5384= yyls; struct Cyc_Yyltype* _temp5386= _temp5384.curr +( yylsp_offset -
2); if( _temp5386 < _temp5384.base? 1: _temp5386 >= _temp5384.last_plus_one){
_throw( Null_Exception);}* _temp5386;}).last_line)); _temp5371->pat_vars= 0;
_temp5371->where_clause= 0; _temp5371->body= Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5378= yyvs; struct _xtunion_struct** _temp5380= _temp5378.curr
+ yyvsp_offset; if( _temp5380 < _temp5378.base? 1: _temp5380 >= _temp5378.last_plus_one){
_throw( Null_Exception);}* _temp5380;})); _temp5371->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5372= yyls; struct Cyc_Yyltype* _temp5374= _temp5372.curr
+( yylsp_offset - 2); if( _temp5374 < _temp5372.base? 1: _temp5374 >= _temp5372.last_plus_one){
_throw( Null_Exception);}* _temp5374;}).first_line,({ struct _tagged_ptr3
_temp5375= yyls; struct Cyc_Yyltype* _temp5377= _temp5375.curr + yylsp_offset;
if( _temp5377 < _temp5375.base? 1: _temp5377 >= _temp5375.last_plus_one){ _throw(
Null_Exception);}* _temp5377;}).last_line); _temp5371;}); _temp5370->tl= 0;
_temp5370;}); _temp5369;}); _temp5368;}); break; case 237: _LL5367: yyval=(
struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5388=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5388[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5389; _temp5389.tag=
Cyc_SwitchClauseList_tok_tag; _temp5389.f1=({ struct Cyc_List_List* _temp5390=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5390->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5394=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5394->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5407= yyvs; struct _xtunion_struct** _temp5409=
_temp5407.curr +( yyvsp_offset - 2); if( _temp5409 < _temp5407.base? 1:
_temp5409 >= _temp5407.last_plus_one){ _throw( Null_Exception);}* _temp5409;}));
_temp5394->pat_vars= 0; _temp5394->where_clause= 0; _temp5394->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5401= yyls; struct Cyc_Yyltype*
_temp5403= _temp5401.curr +( yylsp_offset - 1); if( _temp5403 < _temp5401.base?
1: _temp5403 >= _temp5401.last_plus_one){ _throw( Null_Exception);}* _temp5403;}).first_line,({
struct _tagged_ptr3 _temp5404= yyls; struct Cyc_Yyltype* _temp5406= _temp5404.curr
+( yylsp_offset - 1); if( _temp5406 < _temp5404.base? 1: _temp5406 >= _temp5404.last_plus_one){
_throw( Null_Exception);}* _temp5406;}).last_line)); _temp5394->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5395= yyls; struct Cyc_Yyltype* _temp5397= _temp5395.curr
+( yylsp_offset - 3); if( _temp5397 < _temp5395.base? 1: _temp5397 >= _temp5395.last_plus_one){
_throw( Null_Exception);}* _temp5397;}).first_line,({ struct _tagged_ptr3
_temp5398= yyls; struct Cyc_Yyltype* _temp5400= _temp5398.curr + yylsp_offset;
if( _temp5400 < _temp5398.base? 1: _temp5400 >= _temp5398.last_plus_one){ _throw(
Null_Exception);}* _temp5400;}).last_line); _temp5394;}); _temp5390->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp5391= yyvs; struct _xtunion_struct** _temp5393=
_temp5391.curr + yyvsp_offset; if( _temp5393 < _temp5391.base? 1: _temp5393 >=
_temp5391.last_plus_one){ _throw( Null_Exception);}* _temp5393;})); _temp5390;});
_temp5389;}); _temp5388;}); break; case 238: _LL5387: yyval=( struct
_xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5411=( struct
Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5411[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5412; _temp5412.tag=
Cyc_SwitchClauseList_tok_tag; _temp5412.f1=({ struct Cyc_List_List* _temp5413=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5413->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5417=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5417->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5427= yyvs; struct _xtunion_struct** _temp5429=
_temp5427.curr +( yyvsp_offset - 3); if( _temp5429 < _temp5427.base? 1:
_temp5429 >= _temp5427.last_plus_one){ _throw( Null_Exception);}* _temp5429;}));
_temp5417->pat_vars= 0; _temp5417->where_clause= 0; _temp5417->body= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5424= yyvs; struct _xtunion_struct** _temp5426=
_temp5424.curr +( yyvsp_offset - 1); if( _temp5426 < _temp5424.base? 1:
_temp5426 >= _temp5424.last_plus_one){ _throw( Null_Exception);}* _temp5426;}));
_temp5417->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5418=
yyls; struct Cyc_Yyltype* _temp5420= _temp5418.curr +( yylsp_offset - 4); if(
_temp5420 < _temp5418.base? 1: _temp5420 >= _temp5418.last_plus_one){ _throw(
Null_Exception);}* _temp5420;}).first_line,({ struct _tagged_ptr3 _temp5421=
yyls; struct Cyc_Yyltype* _temp5423= _temp5421.curr +( yylsp_offset - 1); if(
_temp5423 < _temp5421.base? 1: _temp5423 >= _temp5421.last_plus_one){ _throw(
Null_Exception);}* _temp5423;}).last_line); _temp5417;}); _temp5413->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp5414= yyvs; struct _xtunion_struct** _temp5416=
_temp5414.curr + yyvsp_offset; if( _temp5416 < _temp5414.base? 1: _temp5416 >=
_temp5414.last_plus_one){ _throw( Null_Exception);}* _temp5416;})); _temp5413;});
_temp5412;}); _temp5411;}); break; case 239: _LL5410: yyval=( struct
_xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5431=( struct
Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5431[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5432; _temp5432.tag=
Cyc_SwitchClauseList_tok_tag; _temp5432.f1=({ struct Cyc_List_List* _temp5433=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5433->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5437=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5437->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5453= yyvs; struct _xtunion_struct** _temp5455=
_temp5453.curr +( yyvsp_offset - 4); if( _temp5455 < _temp5453.base? 1:
_temp5455 >= _temp5453.last_plus_one){ _throw( Null_Exception);}* _temp5455;}));
_temp5437->pat_vars= 0; _temp5437->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5450= yyvs; struct _xtunion_struct** _temp5452=
_temp5450.curr +( yyvsp_offset - 2); if( _temp5452 < _temp5450.base? 1:
_temp5452 >= _temp5450.last_plus_one){ _throw( Null_Exception);}* _temp5452;}));
_temp5437->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5444= yyls; struct Cyc_Yyltype* _temp5446= _temp5444.curr +(
yylsp_offset - 1); if( _temp5446 < _temp5444.base? 1: _temp5446 >= _temp5444.last_plus_one){
_throw( Null_Exception);}* _temp5446;}).first_line,({ struct _tagged_ptr3
_temp5447= yyls; struct Cyc_Yyltype* _temp5449= _temp5447.curr +( yylsp_offset -
1); if( _temp5449 < _temp5447.base? 1: _temp5449 >= _temp5447.last_plus_one){
_throw( Null_Exception);}* _temp5449;}).last_line)); _temp5437->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5438= yyls; struct Cyc_Yyltype* _temp5440= _temp5438.curr
+( yylsp_offset - 5); if( _temp5440 < _temp5438.base? 1: _temp5440 >= _temp5438.last_plus_one){
_throw( Null_Exception);}* _temp5440;}).first_line,({ struct _tagged_ptr3
_temp5441= yyls; struct Cyc_Yyltype* _temp5443= _temp5441.curr + yylsp_offset;
if( _temp5443 < _temp5441.base? 1: _temp5443 >= _temp5441.last_plus_one){ _throw(
Null_Exception);}* _temp5443;}).last_line); _temp5437;}); _temp5433->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp5434= yyvs; struct _xtunion_struct** _temp5436=
_temp5434.curr + yyvsp_offset; if( _temp5436 < _temp5434.base? 1: _temp5436 >=
_temp5434.last_plus_one){ _throw( Null_Exception);}* _temp5436;})); _temp5433;});
_temp5432;}); _temp5431;}); break; case 240: _LL5430: yyval=( struct
_xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5457=( struct
Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5457[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5458; _temp5458.tag=
Cyc_SwitchClauseList_tok_tag; _temp5458.f1=({ struct Cyc_List_List* _temp5459=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5459->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5463=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5463->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5476= yyvs; struct _xtunion_struct** _temp5478=
_temp5476.curr +( yyvsp_offset - 5); if( _temp5478 < _temp5476.base? 1:
_temp5478 >= _temp5476.last_plus_one){ _throw( Null_Exception);}* _temp5478;}));
_temp5463->pat_vars= 0; _temp5463->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5473= yyvs; struct _xtunion_struct** _temp5475=
_temp5473.curr +( yyvsp_offset - 3); if( _temp5475 < _temp5473.base? 1:
_temp5475 >= _temp5473.last_plus_one){ _throw( Null_Exception);}* _temp5475;}));
_temp5463->body= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5470= yyvs;
struct _xtunion_struct** _temp5472= _temp5470.curr +( yyvsp_offset - 1); if(
_temp5472 < _temp5470.base? 1: _temp5472 >= _temp5470.last_plus_one){ _throw(
Null_Exception);}* _temp5472;})); _temp5463->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5464= yyls; struct Cyc_Yyltype* _temp5466= _temp5464.curr
+( yylsp_offset - 6); if( _temp5466 < _temp5464.base? 1: _temp5466 >= _temp5464.last_plus_one){
_throw( Null_Exception);}* _temp5466;}).first_line,({ struct _tagged_ptr3
_temp5467= yyls; struct Cyc_Yyltype* _temp5469= _temp5467.curr + yylsp_offset;
if( _temp5469 < _temp5467.base? 1: _temp5469 >= _temp5467.last_plus_one){ _throw(
Null_Exception);}* _temp5469;}).last_line); _temp5463;}); _temp5459->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp5460= yyvs; struct _xtunion_struct** _temp5462=
_temp5460.curr + yyvsp_offset; if( _temp5462 < _temp5460.base? 1: _temp5462 >=
_temp5460.last_plus_one){ _throw( Null_Exception);}* _temp5462;})); _temp5459;});
_temp5458;}); _temp5457;}); break; case 241: _LL5456: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5480=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5480[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5481; _temp5481.tag= Cyc_Stmt_tok_tag; _temp5481.f1= Cyc_Absyn_while_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5482= yyvs; struct _xtunion_struct**
_temp5484= _temp5482.curr +( yyvsp_offset - 2); if( _temp5484 < _temp5482.base?
1: _temp5484 >= _temp5482.last_plus_one){ _throw( Null_Exception);}* _temp5484;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5485= yyvs; struct
_xtunion_struct** _temp5487= _temp5485.curr + yyvsp_offset; if( _temp5487 <
_temp5485.base? 1: _temp5487 >= _temp5485.last_plus_one){ _throw( Null_Exception);}*
_temp5487;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5488=
yyls; struct Cyc_Yyltype* _temp5490= _temp5488.curr +( yylsp_offset - 4); if(
_temp5490 < _temp5488.base? 1: _temp5490 >= _temp5488.last_plus_one){ _throw(
Null_Exception);}* _temp5490;}).first_line,({ struct _tagged_ptr3 _temp5491=
yyls; struct Cyc_Yyltype* _temp5493= _temp5491.curr + yylsp_offset; if(
_temp5493 < _temp5491.base? 1: _temp5493 >= _temp5491.last_plus_one){ _throw(
Null_Exception);}* _temp5493;}).last_line)); _temp5481;}); _temp5480;}); break;
case 242: _LL5479: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5495=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5495[ 0]=({ struct Cyc_Stmt_tok_struct _temp5496; _temp5496.tag= Cyc_Stmt_tok_tag;
_temp5496.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5497= yyvs; struct _xtunion_struct** _temp5499= _temp5497.curr +(
yyvsp_offset - 5); if( _temp5499 < _temp5497.base? 1: _temp5499 >= _temp5497.last_plus_one){
_throw( Null_Exception);}* _temp5499;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5500= yyvs; struct _xtunion_struct** _temp5502= _temp5500.curr
+( yyvsp_offset - 2); if( _temp5502 < _temp5500.base? 1: _temp5502 >= _temp5500.last_plus_one){
_throw( Null_Exception);}* _temp5502;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5503= yyls; struct Cyc_Yyltype* _temp5505= _temp5503.curr +(
yylsp_offset - 6); if( _temp5505 < _temp5503.base? 1: _temp5505 >= _temp5503.last_plus_one){
_throw( Null_Exception);}* _temp5505;}).first_line,({ struct _tagged_ptr3
_temp5506= yyls; struct Cyc_Yyltype* _temp5508= _temp5506.curr + yylsp_offset;
if( _temp5508 < _temp5506.base? 1: _temp5508 >= _temp5506.last_plus_one){ _throw(
Null_Exception);}* _temp5508;}).last_line)); _temp5496;}); _temp5495;}); break;
case 243: _LL5494: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5510=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5510[ 0]=({ struct Cyc_Stmt_tok_struct _temp5511; _temp5511.tag= Cyc_Stmt_tok_tag;
_temp5511.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5512=
yyvs; struct _xtunion_struct** _temp5514= _temp5512.curr + yyvsp_offset; if(
_temp5514 < _temp5512.base? 1: _temp5514 >= _temp5512.last_plus_one){ _throw(
Null_Exception);}* _temp5514;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5515= yyls; struct Cyc_Yyltype* _temp5517= _temp5515.curr +(
yylsp_offset - 5); if( _temp5517 < _temp5515.base? 1: _temp5517 >= _temp5515.last_plus_one){
_throw( Null_Exception);}* _temp5517;}).first_line,({ struct _tagged_ptr3
_temp5518= yyls; struct Cyc_Yyltype* _temp5520= _temp5518.curr + yylsp_offset;
if( _temp5520 < _temp5518.base? 1: _temp5520 >= _temp5518.last_plus_one){ _throw(
Null_Exception);}* _temp5520;}).last_line)); _temp5511;}); _temp5510;}); break;
case 244: _LL5509: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5522=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5522[ 0]=({ struct Cyc_Stmt_tok_struct _temp5523; _temp5523.tag= Cyc_Stmt_tok_tag;
_temp5523.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5524= yyvs; struct _xtunion_struct**
_temp5526= _temp5524.curr +( yyvsp_offset - 2); if( _temp5526 < _temp5524.base?
1: _temp5526 >= _temp5524.last_plus_one){ _throw( Null_Exception);}* _temp5526;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5527= yyvs; struct
_xtunion_struct** _temp5529= _temp5527.curr + yyvsp_offset; if( _temp5529 <
_temp5527.base? 1: _temp5529 >= _temp5527.last_plus_one){ _throw( Null_Exception);}*
_temp5529;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5530=
yyls; struct Cyc_Yyltype* _temp5532= _temp5530.curr +( yylsp_offset - 6); if(
_temp5532 < _temp5530.base? 1: _temp5532 >= _temp5530.last_plus_one){ _throw(
Null_Exception);}* _temp5532;}).first_line,({ struct _tagged_ptr3 _temp5533=
yyls; struct Cyc_Yyltype* _temp5535= _temp5533.curr + yylsp_offset; if(
_temp5535 < _temp5533.base? 1: _temp5535 >= _temp5533.last_plus_one){ _throw(
Null_Exception);}* _temp5535;}).last_line)); _temp5523;}); _temp5522;}); break;
case 245: _LL5521: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5537=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5537[ 0]=({ struct Cyc_Stmt_tok_struct _temp5538; _temp5538.tag= Cyc_Stmt_tok_tag;
_temp5538.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5539= yyvs; struct _xtunion_struct** _temp5541=
_temp5539.curr +( yyvsp_offset - 3); if( _temp5541 < _temp5539.base? 1:
_temp5541 >= _temp5539.last_plus_one){ _throw( Null_Exception);}* _temp5541;})),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5542=
yyvs; struct _xtunion_struct** _temp5544= _temp5542.curr + yyvsp_offset; if(
_temp5544 < _temp5542.base? 1: _temp5544 >= _temp5542.last_plus_one){ _throw(
Null_Exception);}* _temp5544;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5545= yyls; struct Cyc_Yyltype* _temp5547= _temp5545.curr +(
yylsp_offset - 6); if( _temp5547 < _temp5545.base? 1: _temp5547 >= _temp5545.last_plus_one){
_throw( Null_Exception);}* _temp5547;}).first_line,({ struct _tagged_ptr3
_temp5548= yyls; struct Cyc_Yyltype* _temp5550= _temp5548.curr + yylsp_offset;
if( _temp5550 < _temp5548.base? 1: _temp5550 >= _temp5548.last_plus_one){ _throw(
Null_Exception);}* _temp5550;}).last_line)); _temp5538;}); _temp5537;}); break;
case 246: _LL5536: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5552=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5552[ 0]=({ struct Cyc_Stmt_tok_struct _temp5553; _temp5553.tag= Cyc_Stmt_tok_tag;
_temp5553.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5554= yyvs; struct _xtunion_struct** _temp5556=
_temp5554.curr +( yyvsp_offset - 4); if( _temp5556 < _temp5554.base? 1:
_temp5556 >= _temp5554.last_plus_one){ _throw( Null_Exception);}* _temp5556;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5557= yyvs; struct _xtunion_struct**
_temp5559= _temp5557.curr +( yyvsp_offset - 2); if( _temp5559 < _temp5557.base?
1: _temp5559 >= _temp5557.last_plus_one){ _throw( Null_Exception);}* _temp5559;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5560= yyvs; struct
_xtunion_struct** _temp5562= _temp5560.curr + yyvsp_offset; if( _temp5562 <
_temp5560.base? 1: _temp5562 >= _temp5560.last_plus_one){ _throw( Null_Exception);}*
_temp5562;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5563=
yyls; struct Cyc_Yyltype* _temp5565= _temp5563.curr +( yylsp_offset - 7); if(
_temp5565 < _temp5563.base? 1: _temp5565 >= _temp5563.last_plus_one){ _throw(
Null_Exception);}* _temp5565;}).first_line,({ struct _tagged_ptr3 _temp5566=
yyls; struct Cyc_Yyltype* _temp5568= _temp5566.curr +( yylsp_offset - 1); if(
_temp5568 < _temp5566.base? 1: _temp5568 >= _temp5566.last_plus_one){ _throw(
Null_Exception);}* _temp5568;}).last_line)); _temp5553;}); _temp5552;}); break;
case 247: _LL5551: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5570=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5570[ 0]=({ struct Cyc_Stmt_tok_struct _temp5571; _temp5571.tag= Cyc_Stmt_tok_tag;
_temp5571.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5572= yyvs; struct _xtunion_struct** _temp5574= _temp5572.curr +(
yyvsp_offset - 4); if( _temp5574 < _temp5572.base? 1: _temp5574 >= _temp5572.last_plus_one){
_throw( Null_Exception);}* _temp5574;})), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5575= yyvs; struct
_xtunion_struct** _temp5577= _temp5575.curr + yyvsp_offset; if( _temp5577 <
_temp5575.base? 1: _temp5577 >= _temp5575.last_plus_one){ _throw( Null_Exception);}*
_temp5577;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5578=
yyls; struct Cyc_Yyltype* _temp5580= _temp5578.curr +( yylsp_offset - 6); if(
_temp5580 < _temp5578.base? 1: _temp5580 >= _temp5578.last_plus_one){ _throw(
Null_Exception);}* _temp5580;}).first_line,({ struct _tagged_ptr3 _temp5581=
yyls; struct Cyc_Yyltype* _temp5583= _temp5581.curr + yylsp_offset; if(
_temp5583 < _temp5581.base? 1: _temp5583 >= _temp5581.last_plus_one){ _throw(
Null_Exception);}* _temp5583;}).last_line)); _temp5571;}); _temp5570;}); break;
case 248: _LL5569: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5585=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5585[ 0]=({ struct Cyc_Stmt_tok_struct _temp5586; _temp5586.tag= Cyc_Stmt_tok_tag;
_temp5586.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5587= yyvs; struct _xtunion_struct** _temp5589= _temp5587.curr +(
yyvsp_offset - 5); if( _temp5589 < _temp5587.base? 1: _temp5589 >= _temp5587.last_plus_one){
_throw( Null_Exception);}* _temp5589;})), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5590= yyvs; struct _xtunion_struct** _temp5592=
_temp5590.curr +( yyvsp_offset - 2); if( _temp5592 < _temp5590.base? 1:
_temp5592 >= _temp5590.last_plus_one){ _throw( Null_Exception);}* _temp5592;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5593= yyvs; struct
_xtunion_struct** _temp5595= _temp5593.curr + yyvsp_offset; if( _temp5595 <
_temp5593.base? 1: _temp5595 >= _temp5593.last_plus_one){ _throw( Null_Exception);}*
_temp5595;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5596=
yyls; struct Cyc_Yyltype* _temp5598= _temp5596.curr +( yylsp_offset - 7); if(
_temp5598 < _temp5596.base? 1: _temp5598 >= _temp5596.last_plus_one){ _throw(
Null_Exception);}* _temp5598;}).first_line,({ struct _tagged_ptr3 _temp5599=
yyls; struct Cyc_Yyltype* _temp5601= _temp5599.curr + yylsp_offset; if(
_temp5601 < _temp5599.base? 1: _temp5601 >= _temp5599.last_plus_one){ _throw(
Null_Exception);}* _temp5601;}).last_line)); _temp5586;}); _temp5585;}); break;
case 249: _LL5584: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5603=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5603[ 0]=({ struct Cyc_Stmt_tok_struct _temp5604; _temp5604.tag= Cyc_Stmt_tok_tag;
_temp5604.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5605= yyvs; struct _xtunion_struct** _temp5607= _temp5605.curr +(
yyvsp_offset - 5); if( _temp5607 < _temp5605.base? 1: _temp5607 >= _temp5605.last_plus_one){
_throw( Null_Exception);}* _temp5607;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5608= yyvs; struct _xtunion_struct** _temp5610= _temp5608.curr
+( yyvsp_offset - 3); if( _temp5610 < _temp5608.base? 1: _temp5610 >= _temp5608.last_plus_one){
_throw( Null_Exception);}* _temp5610;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5611= yyvs; struct _xtunion_struct** _temp5613=
_temp5611.curr + yyvsp_offset; if( _temp5613 < _temp5611.base? 1: _temp5613 >=
_temp5611.last_plus_one){ _throw( Null_Exception);}* _temp5613;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5614= yyls; struct Cyc_Yyltype* _temp5616= _temp5614.curr
+( yylsp_offset - 7); if( _temp5616 < _temp5614.base? 1: _temp5616 >= _temp5614.last_plus_one){
_throw( Null_Exception);}* _temp5616;}).first_line,({ struct _tagged_ptr3
_temp5617= yyls; struct Cyc_Yyltype* _temp5619= _temp5617.curr + yylsp_offset;
if( _temp5619 < _temp5617.base? 1: _temp5619 >= _temp5617.last_plus_one){ _throw(
Null_Exception);}* _temp5619;}).last_line)); _temp5604;}); _temp5603;}); break;
case 250: _LL5602: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5621=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5621[ 0]=({ struct Cyc_Stmt_tok_struct _temp5622; _temp5622.tag= Cyc_Stmt_tok_tag;
_temp5622.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5623= yyvs; struct _xtunion_struct** _temp5625= _temp5623.curr +(
yyvsp_offset - 6); if( _temp5625 < _temp5623.base? 1: _temp5625 >= _temp5623.last_plus_one){
_throw( Null_Exception);}* _temp5625;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5626= yyvs; struct _xtunion_struct** _temp5628= _temp5626.curr
+( yyvsp_offset - 4); if( _temp5628 < _temp5626.base? 1: _temp5628 >= _temp5626.last_plus_one){
_throw( Null_Exception);}* _temp5628;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5629= yyvs; struct _xtunion_struct** _temp5631= _temp5629.curr
+( yyvsp_offset - 2); if( _temp5631 < _temp5629.base? 1: _temp5631 >= _temp5629.last_plus_one){
_throw( Null_Exception);}* _temp5631;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5632= yyvs; struct _xtunion_struct** _temp5634= _temp5632.curr
+ yyvsp_offset; if( _temp5634 < _temp5632.base? 1: _temp5634 >= _temp5632.last_plus_one){
_throw( Null_Exception);}* _temp5634;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5635= yyls; struct Cyc_Yyltype* _temp5637= _temp5635.curr +(
yylsp_offset - 8); if( _temp5637 < _temp5635.base? 1: _temp5637 >= _temp5635.last_plus_one){
_throw( Null_Exception);}* _temp5637;}).first_line,({ struct _tagged_ptr3
_temp5638= yyls; struct Cyc_Yyltype* _temp5640= _temp5638.curr + yylsp_offset;
if( _temp5640 < _temp5638.base? 1: _temp5640 >= _temp5638.last_plus_one){ _throw(
Null_Exception);}* _temp5640;}).last_line)); _temp5622;}); _temp5621;}); break;
case 251: _LL5620: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5642= yyvs; struct _xtunion_struct** _temp5644=
_temp5642.curr +( yyvsp_offset - 3); if( _temp5644 < _temp5642.base? 1:
_temp5644 >= _temp5642.last_plus_one){ _throw( Null_Exception);}* _temp5644;}));
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5645=
yyvs; struct _xtunion_struct** _temp5647= _temp5645.curr + yyvsp_offset; if(
_temp5647 < _temp5645.base? 1: _temp5647 >= _temp5645.last_plus_one){ _throw(
Null_Exception);}* _temp5647;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5648= yyls; struct Cyc_Yyltype* _temp5650= _temp5648.curr +(
yylsp_offset - 5); if( _temp5650 < _temp5648.base? 1: _temp5650 >= _temp5648.last_plus_one){
_throw( Null_Exception);}* _temp5650;}).first_line,({ struct _tagged_ptr3
_temp5651= yyls; struct Cyc_Yyltype* _temp5653= _temp5651.curr + yylsp_offset;
if( _temp5653 < _temp5651.base? 1: _temp5653 >= _temp5651.last_plus_one){ _throw(
Null_Exception);}* _temp5653;}).last_line)); yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5654=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5654[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5655; _temp5655.tag= Cyc_Stmt_tok_tag; _temp5655.f1= Cyc_Parse_flatten_declarations(
decls, s); _temp5655;}); _temp5654;}); break;} case 252: _LL5641: { struct Cyc_List_List*
decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp5657= yyvs; struct
_xtunion_struct** _temp5659= _temp5657.curr +( yyvsp_offset - 4); if( _temp5659
< _temp5657.base? 1: _temp5659 >= _temp5657.last_plus_one){ _throw(
Null_Exception);}* _temp5659;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5660=
yyvs; struct _xtunion_struct** _temp5662= _temp5660.curr +( yyvsp_offset - 3);
if( _temp5662 < _temp5660.base? 1: _temp5662 >= _temp5660.last_plus_one){ _throw(
Null_Exception);}* _temp5662;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5663= yyvs; struct _xtunion_struct** _temp5665=
_temp5663.curr + yyvsp_offset; if( _temp5665 < _temp5663.base? 1: _temp5665 >=
_temp5663.last_plus_one){ _throw( Null_Exception);}* _temp5665;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5666= yyls; struct Cyc_Yyltype* _temp5668= _temp5666.curr
+( yylsp_offset - 6); if( _temp5668 < _temp5666.base? 1: _temp5668 >= _temp5666.last_plus_one){
_throw( Null_Exception);}* _temp5668;}).first_line,({ struct _tagged_ptr3
_temp5669= yyls; struct Cyc_Yyltype* _temp5671= _temp5669.curr + yylsp_offset;
if( _temp5671 < _temp5669.base? 1: _temp5671 >= _temp5669.last_plus_one){ _throw(
Null_Exception);}* _temp5671;}).last_line)); yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5672=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5672[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5673; _temp5673.tag= Cyc_Stmt_tok_tag; _temp5673.f1= Cyc_Parse_flatten_declarations(
decls, s); _temp5673;}); _temp5672;}); break;} case 253: _LL5656: { struct Cyc_List_List*
decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp5675= yyvs; struct
_xtunion_struct** _temp5677= _temp5675.curr +( yyvsp_offset - 4); if( _temp5677
< _temp5675.base? 1: _temp5677 >= _temp5675.last_plus_one){ _throw(
Null_Exception);}* _temp5677;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5678= yyvs; struct _xtunion_struct** _temp5680= _temp5678.curr
+( yyvsp_offset - 2); if( _temp5680 < _temp5678.base? 1: _temp5680 >= _temp5678.last_plus_one){
_throw( Null_Exception);}* _temp5680;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5681= yyvs; struct _xtunion_struct** _temp5683= _temp5681.curr
+ yyvsp_offset; if( _temp5683 < _temp5681.base? 1: _temp5683 >= _temp5681.last_plus_one){
_throw( Null_Exception);}* _temp5683;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5684= yyls; struct Cyc_Yyltype* _temp5686= _temp5684.curr +(
yylsp_offset - 6); if( _temp5686 < _temp5684.base? 1: _temp5686 >= _temp5684.last_plus_one){
_throw( Null_Exception);}* _temp5686;}).first_line,({ struct _tagged_ptr3
_temp5687= yyls; struct Cyc_Yyltype* _temp5689= _temp5687.curr + yylsp_offset;
if( _temp5689 < _temp5687.base? 1: _temp5689 >= _temp5687.last_plus_one){ _throw(
Null_Exception);}* _temp5689;}).last_line)); yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5690=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5690[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5691; _temp5691.tag= Cyc_Stmt_tok_tag; _temp5691.f1= Cyc_Parse_flatten_declarations(
decls, s); _temp5691;}); _temp5690;}); break;} case 254: _LL5674: { struct Cyc_List_List*
decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp5693= yyvs; struct
_xtunion_struct** _temp5695= _temp5693.curr +( yyvsp_offset - 5); if( _temp5695
< _temp5693.base? 1: _temp5695 >= _temp5693.last_plus_one){ _throw(
Null_Exception);}* _temp5695;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5696=
yyvs; struct _xtunion_struct** _temp5698= _temp5696.curr +( yyvsp_offset - 4);
if( _temp5698 < _temp5696.base? 1: _temp5698 >= _temp5696.last_plus_one){ _throw(
Null_Exception);}* _temp5698;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5699= yyvs; struct _xtunion_struct** _temp5701= _temp5699.curr +(
yyvsp_offset - 2); if( _temp5701 < _temp5699.base? 1: _temp5701 >= _temp5699.last_plus_one){
_throw( Null_Exception);}* _temp5701;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5702= yyvs; struct _xtunion_struct** _temp5704= _temp5702.curr
+ yyvsp_offset; if( _temp5704 < _temp5702.base? 1: _temp5704 >= _temp5702.last_plus_one){
_throw( Null_Exception);}* _temp5704;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5705= yyls; struct Cyc_Yyltype* _temp5707= _temp5705.curr +(
yylsp_offset - 7); if( _temp5707 < _temp5705.base? 1: _temp5707 >= _temp5705.last_plus_one){
_throw( Null_Exception);}* _temp5707;}).first_line,({ struct _tagged_ptr3
_temp5708= yyls; struct Cyc_Yyltype* _temp5710= _temp5708.curr + yylsp_offset;
if( _temp5710 < _temp5708.base? 1: _temp5710 >= _temp5708.last_plus_one){ _throw(
Null_Exception);}* _temp5710;}).last_line)); yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5711=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5711[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5712; _temp5712.tag= Cyc_Stmt_tok_tag; _temp5712.f1= Cyc_Parse_flatten_declarations(
decls, s); _temp5712;}); _temp5711;}); break;} case 255: _LL5692: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5714=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5714[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5715; _temp5715.tag= Cyc_Stmt_tok_tag; _temp5715.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_string* _temp5716=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp5716[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5717= yyvs; struct _xtunion_struct** _temp5719= _temp5717.curr
+( yyvsp_offset - 1); if( _temp5719 < _temp5717.base? 1: _temp5719 >= _temp5717.last_plus_one){
_throw( Null_Exception);}* _temp5719;})); _temp5716;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5720= yyls; struct Cyc_Yyltype* _temp5722= _temp5720.curr
+( yylsp_offset - 2); if( _temp5722 < _temp5720.base? 1: _temp5722 >= _temp5720.last_plus_one){
_throw( Null_Exception);}* _temp5722;}).first_line,({ struct _tagged_ptr3
_temp5723= yyls; struct Cyc_Yyltype* _temp5725= _temp5723.curr +( yylsp_offset -
1); if( _temp5725 < _temp5723.base? 1: _temp5725 >= _temp5723.last_plus_one){
_throw( Null_Exception);}* _temp5725;}).last_line)); _temp5715;}); _temp5714;});
break; case 256: _LL5713: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5727=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5727[ 0]=({ struct Cyc_Stmt_tok_struct _temp5728; _temp5728.tag= Cyc_Stmt_tok_tag;
_temp5728.f1= Cyc_Absyn_continue_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5729= yyls; struct Cyc_Yyltype* _temp5731= _temp5729.curr +(
yylsp_offset - 1); if( _temp5731 < _temp5729.base? 1: _temp5731 >= _temp5729.last_plus_one){
_throw( Null_Exception);}* _temp5731;}).first_line,({ struct _tagged_ptr3
_temp5732= yyls; struct Cyc_Yyltype* _temp5734= _temp5732.curr +( yylsp_offset -
1); if( _temp5734 < _temp5732.base? 1: _temp5734 >= _temp5732.last_plus_one){
_throw( Null_Exception);}* _temp5734;}).last_line)); _temp5728;}); _temp5727;});
break; case 257: _LL5726: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5736=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5736[ 0]=({ struct Cyc_Stmt_tok_struct _temp5737; _temp5737.tag= Cyc_Stmt_tok_tag;
_temp5737.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5738= yyls; struct Cyc_Yyltype* _temp5740= _temp5738.curr +(
yylsp_offset - 1); if( _temp5740 < _temp5738.base? 1: _temp5740 >= _temp5738.last_plus_one){
_throw( Null_Exception);}* _temp5740;}).first_line,({ struct _tagged_ptr3
_temp5741= yyls; struct Cyc_Yyltype* _temp5743= _temp5741.curr +( yylsp_offset -
1); if( _temp5743 < _temp5741.base? 1: _temp5743 >= _temp5741.last_plus_one){
_throw( Null_Exception);}* _temp5743;}).last_line)); _temp5737;}); _temp5736;});
break; case 258: _LL5735: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5745=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5745[ 0]=({ struct Cyc_Stmt_tok_struct _temp5746; _temp5746.tag= Cyc_Stmt_tok_tag;
_temp5746.f1= Cyc_Absyn_return_stmt( 0, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5747= yyls; struct Cyc_Yyltype* _temp5749= _temp5747.curr +(
yylsp_offset - 1); if( _temp5749 < _temp5747.base? 1: _temp5749 >= _temp5747.last_plus_one){
_throw( Null_Exception);}* _temp5749;}).first_line,({ struct _tagged_ptr3
_temp5750= yyls; struct Cyc_Yyltype* _temp5752= _temp5750.curr +( yylsp_offset -
1); if( _temp5752 < _temp5750.base? 1: _temp5752 >= _temp5750.last_plus_one){
_throw( Null_Exception);}* _temp5752;}).last_line)); _temp5746;}); _temp5745;});
break; case 259: _LL5744: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5754=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5754[ 0]=({ struct Cyc_Stmt_tok_struct _temp5755; _temp5755.tag= Cyc_Stmt_tok_tag;
_temp5755.f1= Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5756= yyvs; struct _xtunion_struct** _temp5758=
_temp5756.curr +( yyvsp_offset - 1); if( _temp5758 < _temp5756.base? 1:
_temp5758 >= _temp5756.last_plus_one){ _throw( Null_Exception);}* _temp5758;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5759= yyls; struct Cyc_Yyltype*
_temp5761= _temp5759.curr +( yylsp_offset - 2); if( _temp5761 < _temp5759.base?
1: _temp5761 >= _temp5759.last_plus_one){ _throw( Null_Exception);}* _temp5761;}).first_line,({
struct _tagged_ptr3 _temp5762= yyls; struct Cyc_Yyltype* _temp5764= _temp5762.curr
+( yylsp_offset - 1); if( _temp5764 < _temp5762.base? 1: _temp5764 >= _temp5762.last_plus_one){
_throw( Null_Exception);}* _temp5764;}).last_line)); _temp5755;}); _temp5754;});
break; case 260: _LL5753: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5766=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5766[ 0]=({ struct Cyc_Stmt_tok_struct _temp5767; _temp5767.tag= Cyc_Stmt_tok_tag;
_temp5767.f1= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5768= yyls; struct Cyc_Yyltype* _temp5770= _temp5768.curr +(
yylsp_offset - 1); if( _temp5770 < _temp5768.base? 1: _temp5770 >= _temp5768.last_plus_one){
_throw( Null_Exception);}* _temp5770;}).first_line,({ struct _tagged_ptr3
_temp5771= yyls; struct Cyc_Yyltype* _temp5773= _temp5771.curr +( yylsp_offset -
1); if( _temp5773 < _temp5771.base? 1: _temp5773 >= _temp5771.last_plus_one){
_throw( Null_Exception);}* _temp5773;}).last_line)); _temp5767;}); _temp5766;});
break; case 261: _LL5765: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5775=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5775[ 0]=({ struct Cyc_Stmt_tok_struct _temp5776; _temp5776.tag= Cyc_Stmt_tok_tag;
_temp5776.f1= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5777= yyls; struct Cyc_Yyltype* _temp5779= _temp5777.curr +(
yylsp_offset - 3); if( _temp5779 < _temp5777.base? 1: _temp5779 >= _temp5777.last_plus_one){
_throw( Null_Exception);}* _temp5779;}).first_line,({ struct _tagged_ptr3
_temp5780= yyls; struct Cyc_Yyltype* _temp5782= _temp5780.curr +( yylsp_offset -
3); if( _temp5782 < _temp5780.base? 1: _temp5782 >= _temp5780.last_plus_one){
_throw( Null_Exception);}* _temp5782;}).last_line)); _temp5776;}); _temp5775;});
break; case 262: _LL5774: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5784=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5784[ 0]=({ struct Cyc_Stmt_tok_struct _temp5785; _temp5785.tag= Cyc_Stmt_tok_tag;
_temp5785.f1= Cyc_Absyn_fallthru_stmt( Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp5786= yyvs; struct _xtunion_struct** _temp5788= _temp5786.curr
+( yyvsp_offset - 2); if( _temp5788 < _temp5786.base? 1: _temp5788 >= _temp5786.last_plus_one){
_throw( Null_Exception);}* _temp5788;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5789= yyls; struct Cyc_Yyltype* _temp5791= _temp5789.curr +(
yylsp_offset - 4); if( _temp5791 < _temp5789.base? 1: _temp5791 >= _temp5789.last_plus_one){
_throw( Null_Exception);}* _temp5791;}).first_line,({ struct _tagged_ptr3
_temp5792= yyls; struct Cyc_Yyltype* _temp5794= _temp5792.curr +( yylsp_offset -
4); if( _temp5794 < _temp5792.base? 1: _temp5794 >= _temp5792.last_plus_one){
_throw( Null_Exception);}* _temp5794;}).last_line)); _temp5785;}); _temp5784;});
break; case 263: _LL5783: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5796=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5796[ 0]=({ struct Cyc_Pattern_tok_struct _temp5797; _temp5797.tag= Cyc_Pattern_tok_tag;
_temp5797.f1= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5798= yyls; struct Cyc_Yyltype* _temp5800= _temp5798.curr
+ yylsp_offset; if( _temp5800 < _temp5798.base? 1: _temp5800 >= _temp5798.last_plus_one){
_throw( Null_Exception);}* _temp5800;}).first_line,({ struct _tagged_ptr3
_temp5801= yyls; struct Cyc_Yyltype* _temp5803= _temp5801.curr + yylsp_offset;
if( _temp5803 < _temp5801.base? 1: _temp5803 >= _temp5801.last_plus_one){ _throw(
Null_Exception);}* _temp5803;}).last_line)); _temp5797;}); _temp5796;}); break;
case 264: _LL5795: yyval=({ struct _tagged_ptr2 _temp5805= yyvs; struct
_xtunion_struct** _temp5807= _temp5805.curr +( yyvsp_offset - 1); if( _temp5807
< _temp5805.base? 1: _temp5807 >= _temp5805.last_plus_one){ _throw(
Null_Exception);}* _temp5807;}); break; case 265: _LL5804: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5809=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5809[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5810; _temp5810.tag= Cyc_Pattern_tok_tag; _temp5810.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp5811=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp5811[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp5812; _temp5812.tag= Cyc_Absyn_Int_p_tag; _temp5812.f1=( void*)(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5813= yyvs; struct _xtunion_struct** _temp5815=
_temp5813.curr + yyvsp_offset; if( _temp5815 < _temp5813.base? 1: _temp5815 >=
_temp5813.last_plus_one){ _throw( Null_Exception);}* _temp5815;}))).f1;
_temp5812.f2=(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp5816= yyvs; struct
_xtunion_struct** _temp5818= _temp5816.curr + yyvsp_offset; if( _temp5818 <
_temp5816.base? 1: _temp5818 >= _temp5816.last_plus_one){ _throw( Null_Exception);}*
_temp5818;}))).f2; _temp5812;}); _temp5811;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5819= yyls; struct Cyc_Yyltype* _temp5821= _temp5819.curr
+ yylsp_offset; if( _temp5821 < _temp5819.base? 1: _temp5821 >= _temp5819.last_plus_one){
_throw( Null_Exception);}* _temp5821;}).first_line,({ struct _tagged_ptr3
_temp5822= yyls; struct Cyc_Yyltype* _temp5824= _temp5822.curr + yylsp_offset;
if( _temp5824 < _temp5822.base? 1: _temp5824 >= _temp5822.last_plus_one){ _throw(
Null_Exception);}* _temp5824;}).last_line)); _temp5810;}); _temp5809;}); break;
case 266: _LL5808: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5826=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5826[ 0]=({ struct Cyc_Pattern_tok_struct _temp5827; _temp5827.tag= Cyc_Pattern_tok_tag;
_temp5827.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp5828=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp5828[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp5829; _temp5829.tag= Cyc_Absyn_Int_p_tag;
_temp5829.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp5829.f2= -(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5830= yyvs; struct _xtunion_struct** _temp5832=
_temp5830.curr + yyvsp_offset; if( _temp5832 < _temp5830.base? 1: _temp5832 >=
_temp5830.last_plus_one){ _throw( Null_Exception);}* _temp5832;}))).f2;
_temp5829;}); _temp5828;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5833= yyls; struct Cyc_Yyltype* _temp5835= _temp5833.curr +( yylsp_offset -
1); if( _temp5835 < _temp5833.base? 1: _temp5835 >= _temp5833.last_plus_one){
_throw( Null_Exception);}* _temp5835;}).first_line,({ struct _tagged_ptr3
_temp5836= yyls; struct Cyc_Yyltype* _temp5838= _temp5836.curr + yylsp_offset;
if( _temp5838 < _temp5836.base? 1: _temp5838 >= _temp5836.last_plus_one){ _throw(
Null_Exception);}* _temp5838;}).last_line)); _temp5827;}); _temp5826;}); break;
case 267: _LL5825: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5840=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5840[ 0]=({ struct Cyc_Pattern_tok_struct _temp5841; _temp5841.tag= Cyc_Pattern_tok_tag;
_temp5841.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Float_p_struct*
_temp5842=( struct Cyc_Absyn_Float_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct));
_temp5842[ 0]=({ struct Cyc_Absyn_Float_p_struct _temp5843; _temp5843.tag= Cyc_Absyn_Float_p_tag;
_temp5843.f1= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5844= yyvs;
struct _xtunion_struct** _temp5846= _temp5844.curr + yyvsp_offset; if( _temp5846
< _temp5844.base? 1: _temp5846 >= _temp5844.last_plus_one){ _throw(
Null_Exception);}* _temp5846;})); _temp5843;}); _temp5842;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5847= yyls; struct Cyc_Yyltype* _temp5849= _temp5847.curr
+ yylsp_offset; if( _temp5849 < _temp5847.base? 1: _temp5849 >= _temp5847.last_plus_one){
_throw( Null_Exception);}* _temp5849;}).first_line,({ struct _tagged_ptr3
_temp5850= yyls; struct Cyc_Yyltype* _temp5852= _temp5850.curr + yylsp_offset;
if( _temp5852 < _temp5850.base? 1: _temp5852 >= _temp5850.last_plus_one){ _throw(
Null_Exception);}* _temp5852;}).last_line)); _temp5841;}); _temp5840;}); break;
case 268: _LL5839: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5854=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5854[ 0]=({ struct Cyc_Pattern_tok_struct _temp5855; _temp5855.tag= Cyc_Pattern_tok_tag;
_temp5855.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp5856=( struct Cyc_Absyn_Char_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_p_struct));
_temp5856[ 0]=({ struct Cyc_Absyn_Char_p_struct _temp5857; _temp5857.tag= Cyc_Absyn_Char_p_tag;
_temp5857.f1= Cyc_yyget_Char_tok(({ struct _tagged_ptr2 _temp5858= yyvs; struct
_xtunion_struct** _temp5860= _temp5858.curr + yyvsp_offset; if( _temp5860 <
_temp5858.base? 1: _temp5860 >= _temp5858.last_plus_one){ _throw( Null_Exception);}*
_temp5860;})); _temp5857;}); _temp5856;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5861= yyls; struct Cyc_Yyltype* _temp5863= _temp5861.curr +
yylsp_offset; if( _temp5863 < _temp5861.base? 1: _temp5863 >= _temp5861.last_plus_one){
_throw( Null_Exception);}* _temp5863;}).first_line,({ struct _tagged_ptr3
_temp5864= yyls; struct Cyc_Yyltype* _temp5866= _temp5864.curr + yylsp_offset;
if( _temp5866 < _temp5864.base? 1: _temp5866 >= _temp5864.last_plus_one){ _throw(
Null_Exception);}* _temp5866;}).last_line)); _temp5855;}); _temp5854;}); break;
case 269: _LL5853: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5868=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5868[ 0]=({ struct Cyc_Pattern_tok_struct _temp5869; _temp5869.tag= Cyc_Pattern_tok_tag;
_temp5869.f1= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5870= yyls; struct Cyc_Yyltype* _temp5872= _temp5870.curr
+ yylsp_offset; if( _temp5872 < _temp5870.base? 1: _temp5872 >= _temp5870.last_plus_one){
_throw( Null_Exception);}* _temp5872;}).first_line,({ struct _tagged_ptr3
_temp5873= yyls; struct Cyc_Yyltype* _temp5875= _temp5873.curr + yylsp_offset;
if( _temp5875 < _temp5873.base? 1: _temp5875 >= _temp5873.last_plus_one){ _throw(
Null_Exception);}* _temp5875;}).last_line)); _temp5869;}); _temp5868;}); break;
case 270: _LL5867: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5877=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5877[ 0]=({ struct Cyc_Pattern_tok_struct _temp5878; _temp5878.tag= Cyc_Pattern_tok_tag;
_temp5878.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp5879=( struct Cyc_Absyn_UnknownId_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp5879[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp5880; _temp5880.tag=
Cyc_Absyn_UnknownId_p_tag; _temp5880.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp5881= yyvs; struct _xtunion_struct** _temp5883= _temp5881.curr
+ yyvsp_offset; if( _temp5883 < _temp5881.base? 1: _temp5883 >= _temp5881.last_plus_one){
_throw( Null_Exception);}* _temp5883;})); _temp5880;}); _temp5879;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5884= yyls; struct Cyc_Yyltype* _temp5886= _temp5884.curr
+ yylsp_offset; if( _temp5886 < _temp5884.base? 1: _temp5886 >= _temp5884.last_plus_one){
_throw( Null_Exception);}* _temp5886;}).first_line,({ struct _tagged_ptr3
_temp5887= yyls; struct Cyc_Yyltype* _temp5889= _temp5887.curr + yylsp_offset;
if( _temp5889 < _temp5887.base? 1: _temp5889 >= _temp5887.last_plus_one){ _throw(
Null_Exception);}* _temp5889;}).last_line)); _temp5878;}); _temp5877;}); break;
case 271: _LL5876: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5891= yyls; struct Cyc_Yyltype* _temp5893= _temp5891.curr
+( yylsp_offset - 3); if( _temp5893 < _temp5891.base? 1: _temp5893 >= _temp5891.last_plus_one){
_throw( Null_Exception);}* _temp5893;}).first_line,({ struct _tagged_ptr3
_temp5894= yyls; struct Cyc_Yyltype* _temp5896= _temp5894.curr +( yylsp_offset -
3); if( _temp5896 < _temp5894.base? 1: _temp5896 >= _temp5894.last_plus_one){
_throw( Null_Exception);}* _temp5896;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5897= yyvs; struct _xtunion_struct** _temp5899=
_temp5897.curr +( yyvsp_offset - 3); if( _temp5899 < _temp5897.base? 1:
_temp5899 >= _temp5897.last_plus_one){ _throw( Null_Exception);}* _temp5899;})));
yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5900=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5900[ 0]=({ struct Cyc_Pattern_tok_struct _temp5901; _temp5901.tag= Cyc_Pattern_tok_tag;
_temp5901.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp5902=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp5902[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp5903; _temp5903.tag= Cyc_Absyn_UnknownCall_p_tag; _temp5903.f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp5904= yyvs; struct _xtunion_struct** _temp5906=
_temp5904.curr +( yyvsp_offset - 4); if( _temp5906 < _temp5904.base? 1:
_temp5906 >= _temp5904.last_plus_one){ _throw( Null_Exception);}* _temp5906;}));
_temp5903.f2= tvs; _temp5903.f3= Cyc_yyget_PatternList_tok(({ struct
_tagged_ptr2 _temp5907= yyvs; struct _xtunion_struct** _temp5909= _temp5907.curr
+( yyvsp_offset - 1); if( _temp5909 < _temp5907.base? 1: _temp5909 >= _temp5907.last_plus_one){
_throw( Null_Exception);}* _temp5909;})); _temp5903;}); _temp5902;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5910= yyls; struct Cyc_Yyltype* _temp5912= _temp5910.curr
+( yylsp_offset - 4); if( _temp5912 < _temp5910.base? 1: _temp5912 >= _temp5910.last_plus_one){
_throw( Null_Exception);}* _temp5912;}).first_line,({ struct _tagged_ptr3
_temp5913= yyls; struct Cyc_Yyltype* _temp5915= _temp5913.curr + yylsp_offset;
if( _temp5915 < _temp5913.base? 1: _temp5915 >= _temp5913.last_plus_one){ _throw(
Null_Exception);}* _temp5915;}).last_line)); _temp5901;}); _temp5900;}); break;}
case 272: _LL5890: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5917=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5917[ 0]=({ struct Cyc_Pattern_tok_struct _temp5918; _temp5918.tag= Cyc_Pattern_tok_tag;
_temp5918.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp5919=( struct Cyc_Absyn_Tuple_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp5919[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp5920; _temp5920.tag= Cyc_Absyn_Tuple_p_tag;
_temp5920.f1= Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp5921= yyvs;
struct _xtunion_struct** _temp5923= _temp5921.curr +( yyvsp_offset - 1); if(
_temp5923 < _temp5921.base? 1: _temp5923 >= _temp5921.last_plus_one){ _throw(
Null_Exception);}* _temp5923;})); _temp5920;}); _temp5919;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5924= yyls; struct Cyc_Yyltype* _temp5926= _temp5924.curr
+( yylsp_offset - 3); if( _temp5926 < _temp5924.base? 1: _temp5926 >= _temp5924.last_plus_one){
_throw( Null_Exception);}* _temp5926;}).first_line,({ struct _tagged_ptr3
_temp5927= yyls; struct Cyc_Yyltype* _temp5929= _temp5927.curr + yylsp_offset;
if( _temp5929 < _temp5927.base? 1: _temp5929 >= _temp5927.last_plus_one){ _throw(
Null_Exception);}* _temp5929;}).last_line)); _temp5918;}); _temp5917;}); break;
case 273: _LL5916: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5931= yyls; struct Cyc_Yyltype* _temp5933= _temp5931.curr
+( yylsp_offset - 2); if( _temp5933 < _temp5931.base? 1: _temp5933 >= _temp5931.last_plus_one){
_throw( Null_Exception);}* _temp5933;}).first_line,({ struct _tagged_ptr3
_temp5934= yyls; struct Cyc_Yyltype* _temp5936= _temp5934.curr +( yylsp_offset -
2); if( _temp5936 < _temp5934.base? 1: _temp5936 >= _temp5934.last_plus_one){
_throw( Null_Exception);}* _temp5936;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5937= yyvs; struct _xtunion_struct** _temp5939=
_temp5937.curr +( yyvsp_offset - 2); if( _temp5939 < _temp5937.base? 1:
_temp5939 >= _temp5937.last_plus_one){ _throw( Null_Exception);}* _temp5939;})));
yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5940=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5940[ 0]=({ struct Cyc_Pattern_tok_struct _temp5941; _temp5941.tag= Cyc_Pattern_tok_tag;
_temp5941.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp5942=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp5942[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp5943; _temp5943.tag= Cyc_Absyn_UnknownFields_p_tag; _temp5943.f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp5944= yyvs; struct _xtunion_struct** _temp5946=
_temp5944.curr +( yyvsp_offset - 3); if( _temp5946 < _temp5944.base? 1:
_temp5946 >= _temp5944.last_plus_one){ _throw( Null_Exception);}* _temp5946;}));
_temp5943.f2= tvs; _temp5943.f3= 0; _temp5943;}); _temp5942;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5947= yyls; struct Cyc_Yyltype* _temp5949= _temp5947.curr
+( yylsp_offset - 3); if( _temp5949 < _temp5947.base? 1: _temp5949 >= _temp5947.last_plus_one){
_throw( Null_Exception);}* _temp5949;}).first_line,({ struct _tagged_ptr3
_temp5950= yyls; struct Cyc_Yyltype* _temp5952= _temp5950.curr + yylsp_offset;
if( _temp5952 < _temp5950.base? 1: _temp5952 >= _temp5950.last_plus_one){ _throw(
Null_Exception);}* _temp5952;}).last_line)); _temp5941;}); _temp5940;}); break;}
case 274: _LL5930: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5954= yyls; struct Cyc_Yyltype* _temp5956= _temp5954.curr
+( yylsp_offset - 3); if( _temp5956 < _temp5954.base? 1: _temp5956 >= _temp5954.last_plus_one){
_throw( Null_Exception);}* _temp5956;}).first_line,({ struct _tagged_ptr3
_temp5957= yyls; struct Cyc_Yyltype* _temp5959= _temp5957.curr +( yylsp_offset -
3); if( _temp5959 < _temp5957.base? 1: _temp5959 >= _temp5957.last_plus_one){
_throw( Null_Exception);}* _temp5959;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5960= yyvs; struct _xtunion_struct** _temp5962=
_temp5960.curr +( yyvsp_offset - 3); if( _temp5962 < _temp5960.base? 1:
_temp5962 >= _temp5960.last_plus_one){ _throw( Null_Exception);}* _temp5962;})));
yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5963=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5963[ 0]=({ struct Cyc_Pattern_tok_struct _temp5964; _temp5964.tag= Cyc_Pattern_tok_tag;
_temp5964.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp5965=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp5965[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp5966; _temp5966.tag= Cyc_Absyn_UnknownFields_p_tag; _temp5966.f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp5967= yyvs; struct _xtunion_struct** _temp5969=
_temp5967.curr +( yyvsp_offset - 4); if( _temp5969 < _temp5967.base? 1:
_temp5969 >= _temp5967.last_plus_one){ _throw( Null_Exception);}* _temp5969;}));
_temp5966.f2= tvs; _temp5966.f3= Cyc_yyget_FieldPatternList_tok(({ struct
_tagged_ptr2 _temp5970= yyvs; struct _xtunion_struct** _temp5972= _temp5970.curr
+( yyvsp_offset - 1); if( _temp5972 < _temp5970.base? 1: _temp5972 >= _temp5970.last_plus_one){
_throw( Null_Exception);}* _temp5972;})); _temp5966;}); _temp5965;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5973= yyls; struct Cyc_Yyltype* _temp5975= _temp5973.curr
+( yylsp_offset - 4); if( _temp5975 < _temp5973.base? 1: _temp5975 >= _temp5973.last_plus_one){
_throw( Null_Exception);}* _temp5975;}).first_line,({ struct _tagged_ptr3
_temp5976= yyls; struct Cyc_Yyltype* _temp5978= _temp5976.curr + yylsp_offset;
if( _temp5978 < _temp5976.base? 1: _temp5978 >= _temp5976.last_plus_one){ _throw(
Null_Exception);}* _temp5978;}).last_line)); _temp5964;}); _temp5963;}); break;}
case 275: _LL5953: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5980=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5980[ 0]=({ struct Cyc_Pattern_tok_struct _temp5981; _temp5981.tag= Cyc_Pattern_tok_tag;
_temp5981.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp5982=( struct Cyc_Absyn_Pointer_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp5982[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp5983; _temp5983.tag= Cyc_Absyn_Pointer_p_tag;
_temp5983.f1= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5984= yyvs;
struct _xtunion_struct** _temp5986= _temp5984.curr + yyvsp_offset; if( _temp5986
< _temp5984.base? 1: _temp5986 >= _temp5984.last_plus_one){ _throw(
Null_Exception);}* _temp5986;})); _temp5983;}); _temp5982;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5987= yyls; struct Cyc_Yyltype* _temp5989= _temp5987.curr
+( yylsp_offset - 1); if( _temp5989 < _temp5987.base? 1: _temp5989 >= _temp5987.last_plus_one){
_throw( Null_Exception);}* _temp5989;}).first_line,({ struct _tagged_ptr3
_temp5990= yyls; struct Cyc_Yyltype* _temp5992= _temp5990.curr + yylsp_offset;
if( _temp5992 < _temp5990.base? 1: _temp5992 >= _temp5990.last_plus_one){ _throw(
Null_Exception);}* _temp5992;}).last_line)); _temp5981;}); _temp5980;}); break;
case 276: _LL5979: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5994=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5994[ 0]=({ struct Cyc_Pattern_tok_struct _temp5995; _temp5995.tag= Cyc_Pattern_tok_tag;
_temp5995.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp5996=( struct Cyc_Absyn_Reference_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp5996[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp5997; _temp5997.tag=
Cyc_Absyn_Reference_p_tag; _temp5997.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp5998=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp5998->f1=(
void*) Cyc_Absyn_Loc_n; _temp5998->f2=({ struct _tagged_string* _temp5999=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp5999[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6000= yyvs;
struct _xtunion_struct** _temp6002= _temp6000.curr + yyvsp_offset; if( _temp6002
< _temp6000.base? 1: _temp6002 >= _temp6000.last_plus_one){ _throw(
Null_Exception);}* _temp6002;})); _temp5999;}); _temp5998;}),( void*) Cyc_Absyn_VoidType,
0); _temp5997;}); _temp5996;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6003= yyls; struct Cyc_Yyltype* _temp6005= _temp6003.curr +(
yylsp_offset - 1); if( _temp6005 < _temp6003.base? 1: _temp6005 >= _temp6003.last_plus_one){
_throw( Null_Exception);}* _temp6005;}).first_line,({ struct _tagged_ptr3
_temp6006= yyls; struct Cyc_Yyltype* _temp6008= _temp6006.curr + yylsp_offset;
if( _temp6008 < _temp6006.base? 1: _temp6008 >= _temp6006.last_plus_one){ _throw(
Null_Exception);}* _temp6008;}).last_line)); _temp5995;}); _temp5994;}); break;
case 277: _LL5993: yyval=( struct _xtunion_struct*)({ struct Cyc_PatternList_tok_struct*
_temp6010=( struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6010[ 0]=({ struct Cyc_PatternList_tok_struct _temp6011; _temp6011.tag= Cyc_PatternList_tok_tag;
_temp6011.f1= 0; _temp6011;}); _temp6010;}); break; case 278: _LL6009: yyval=(
struct _xtunion_struct*)({ struct Cyc_PatternList_tok_struct* _temp6013=( struct
Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6013[ 0]=({ struct Cyc_PatternList_tok_struct _temp6014; _temp6014.tag= Cyc_PatternList_tok_tag;
_temp6014.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp6015= yyvs; struct
_xtunion_struct** _temp6017= _temp6015.curr + yyvsp_offset; if( _temp6017 <
_temp6015.base? 1: _temp6017 >= _temp6015.last_plus_one){ _throw( Null_Exception);}*
_temp6017;}))); _temp6014;}); _temp6013;}); break; case 279: _LL6012: yyval=(
struct _xtunion_struct*)({ struct Cyc_PatternList_tok_struct* _temp6019=( struct
Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6019[ 0]=({ struct Cyc_PatternList_tok_struct _temp6020; _temp6020.tag= Cyc_PatternList_tok_tag;
_temp6020.f1=({ struct Cyc_List_List* _temp6021=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6021->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp6022= yyvs; struct _xtunion_struct** _temp6024=
_temp6022.curr + yyvsp_offset; if( _temp6024 < _temp6022.base? 1: _temp6024 >=
_temp6022.last_plus_one){ _throw( Null_Exception);}* _temp6024;})); _temp6021->tl=
0; _temp6021;}); _temp6020;}); _temp6019;}); break; case 280: _LL6018: yyval=(
struct _xtunion_struct*)({ struct Cyc_PatternList_tok_struct* _temp6026=( struct
Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6026[ 0]=({ struct Cyc_PatternList_tok_struct _temp6027; _temp6027.tag= Cyc_PatternList_tok_tag;
_temp6027.f1=({ struct Cyc_List_List* _temp6028=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6028->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp6032= yyvs; struct _xtunion_struct** _temp6034=
_temp6032.curr + yyvsp_offset; if( _temp6034 < _temp6032.base? 1: _temp6034 >=
_temp6032.last_plus_one){ _throw( Null_Exception);}* _temp6034;})); _temp6028->tl=
Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp6029= yyvs; struct
_xtunion_struct** _temp6031= _temp6029.curr +( yyvsp_offset - 2); if( _temp6031
< _temp6029.base? 1: _temp6031 >= _temp6029.last_plus_one){ _throw(
Null_Exception);}* _temp6031;})); _temp6028;}); _temp6027;}); _temp6026;});
break; case 281: _LL6025: yyval=( struct _xtunion_struct*)({ struct Cyc_FieldPattern_tok_struct*
_temp6036=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6036[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6037; _temp6037.tag=
Cyc_FieldPattern_tok_tag; _temp6037.f1=({ struct _tuple17* _temp6038=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp6038->f1= 0; _temp6038->f2=
Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6039= yyvs; struct
_xtunion_struct** _temp6041= _temp6039.curr + yyvsp_offset; if( _temp6041 <
_temp6039.base? 1: _temp6041 >= _temp6039.last_plus_one){ _throw( Null_Exception);}*
_temp6041;})); _temp6038;}); _temp6037;}); _temp6036;}); break; case 282:
_LL6035: yyval=( struct _xtunion_struct*)({ struct Cyc_FieldPattern_tok_struct*
_temp6043=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6043[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6044; _temp6044.tag=
Cyc_FieldPattern_tok_tag; _temp6044.f1=({ struct _tuple17* _temp6045=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp6045->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp6049= yyvs; struct _xtunion_struct** _temp6051=
_temp6049.curr +( yyvsp_offset - 1); if( _temp6051 < _temp6049.base? 1:
_temp6051 >= _temp6049.last_plus_one){ _throw( Null_Exception);}* _temp6051;}));
_temp6045->f2= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6046= yyvs;
struct _xtunion_struct** _temp6048= _temp6046.curr + yyvsp_offset; if( _temp6048
< _temp6046.base? 1: _temp6048 >= _temp6046.last_plus_one){ _throw(
Null_Exception);}* _temp6048;})); _temp6045;}); _temp6044;}); _temp6043;});
break; case 283: _LL6042: yyval=( struct _xtunion_struct*)({ struct Cyc_FieldPatternList_tok_struct*
_temp6053=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp6053[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp6054; _temp6054.tag= Cyc_FieldPatternList_tok_tag; _temp6054.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp6055= yyvs; struct _xtunion_struct** _temp6057=
_temp6055.curr + yyvsp_offset; if( _temp6057 < _temp6055.base? 1: _temp6057 >=
_temp6055.last_plus_one){ _throw( Null_Exception);}* _temp6057;}))); _temp6054;});
_temp6053;}); break; case 284: _LL6052: yyval=( struct _xtunion_struct*)({
struct Cyc_FieldPatternList_tok_struct* _temp6059=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp6059[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp6060; _temp6060.tag= Cyc_FieldPatternList_tok_tag;
_temp6060.f1=({ struct Cyc_List_List* _temp6061=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6061->hd=( void*) Cyc_yyget_FieldPattern_tok(({
struct _tagged_ptr2 _temp6062= yyvs; struct _xtunion_struct** _temp6064=
_temp6062.curr + yyvsp_offset; if( _temp6064 < _temp6062.base? 1: _temp6064 >=
_temp6062.last_plus_one){ _throw( Null_Exception);}* _temp6064;})); _temp6061->tl=
0; _temp6061;}); _temp6060;}); _temp6059;}); break; case 285: _LL6058: yyval=(
struct _xtunion_struct*)({ struct Cyc_FieldPatternList_tok_struct* _temp6066=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp6066[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp6067; _temp6067.tag=
Cyc_FieldPatternList_tok_tag; _temp6067.f1=({ struct Cyc_List_List* _temp6068=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp6068->hd=(
void*) Cyc_yyget_FieldPattern_tok(({ struct _tagged_ptr2 _temp6072= yyvs; struct
_xtunion_struct** _temp6074= _temp6072.curr + yyvsp_offset; if( _temp6074 <
_temp6072.base? 1: _temp6074 >= _temp6072.last_plus_one){ _throw( Null_Exception);}*
_temp6074;})); _temp6068->tl= Cyc_yyget_FieldPatternList_tok(({ struct
_tagged_ptr2 _temp6069= yyvs; struct _xtunion_struct** _temp6071= _temp6069.curr
+( yyvsp_offset - 2); if( _temp6071 < _temp6069.base? 1: _temp6071 >= _temp6069.last_plus_one){
_throw( Null_Exception);}* _temp6071;})); _temp6068;}); _temp6067;}); _temp6066;});
break; case 286: _LL6065: yyval=({ struct _tagged_ptr2 _temp6076= yyvs; struct
_xtunion_struct** _temp6078= _temp6076.curr + yyvsp_offset; if( _temp6078 <
_temp6076.base? 1: _temp6078 >= _temp6076.last_plus_one){ _throw( Null_Exception);}*
_temp6078;}); break; case 287: _LL6075: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6080=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6080[ 0]=({ struct Cyc_Exp_tok_struct
_temp6081; _temp6081.tag= Cyc_Exp_tok_tag; _temp6081.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6082= yyvs; struct _xtunion_struct** _temp6084=
_temp6082.curr +( yyvsp_offset - 2); if( _temp6084 < _temp6082.base? 1:
_temp6084 >= _temp6082.last_plus_one){ _throw( Null_Exception);}* _temp6084;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6085= yyvs; struct _xtunion_struct**
_temp6087= _temp6085.curr + yyvsp_offset; if( _temp6087 < _temp6085.base? 1:
_temp6087 >= _temp6085.last_plus_one){ _throw( Null_Exception);}* _temp6087;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6088= yyls; struct Cyc_Yyltype*
_temp6090= _temp6088.curr +( yylsp_offset - 2); if( _temp6090 < _temp6088.base?
1: _temp6090 >= _temp6088.last_plus_one){ _throw( Null_Exception);}* _temp6090;}).first_line,({
struct _tagged_ptr3 _temp6091= yyls; struct Cyc_Yyltype* _temp6093= _temp6091.curr
+ yylsp_offset; if( _temp6093 < _temp6091.base? 1: _temp6093 >= _temp6091.last_plus_one){
_throw( Null_Exception);}* _temp6093;}).last_line)); _temp6081;}); _temp6080;});
break; case 288: _LL6079: yyval=({ struct _tagged_ptr2 _temp6095= yyvs; struct
_xtunion_struct** _temp6097= _temp6095.curr + yyvsp_offset; if( _temp6097 <
_temp6095.base? 1: _temp6097 >= _temp6095.last_plus_one){ _throw( Null_Exception);}*
_temp6097;}); break; case 289: _LL6094: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6099=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6099[ 0]=({ struct Cyc_Exp_tok_struct
_temp6100; _temp6100.tag= Cyc_Exp_tok_tag; _temp6100.f1= Cyc_Absyn_assignop_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6101= yyvs; struct _xtunion_struct**
_temp6103= _temp6101.curr +( yyvsp_offset - 2); if( _temp6103 < _temp6101.base?
1: _temp6103 >= _temp6101.last_plus_one){ _throw( Null_Exception);}* _temp6103;})),
Cyc_yyget_Primopopt_tok(({ struct _tagged_ptr2 _temp6104= yyvs; struct
_xtunion_struct** _temp6106= _temp6104.curr +( yyvsp_offset - 1); if( _temp6106
< _temp6104.base? 1: _temp6106 >= _temp6104.last_plus_one){ _throw(
Null_Exception);}* _temp6106;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6107= yyvs; struct _xtunion_struct** _temp6109= _temp6107.curr +
yyvsp_offset; if( _temp6109 < _temp6107.base? 1: _temp6109 >= _temp6107.last_plus_one){
_throw( Null_Exception);}* _temp6109;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6110= yyls; struct Cyc_Yyltype* _temp6112= _temp6110.curr +(
yylsp_offset - 2); if( _temp6112 < _temp6110.base? 1: _temp6112 >= _temp6110.last_plus_one){
_throw( Null_Exception);}* _temp6112;}).first_line,({ struct _tagged_ptr3
_temp6113= yyls; struct Cyc_Yyltype* _temp6115= _temp6113.curr + yylsp_offset;
if( _temp6115 < _temp6113.base? 1: _temp6115 >= _temp6113.last_plus_one){ _throw(
Null_Exception);}* _temp6115;}).last_line)); _temp6100;}); _temp6099;}); break;
case 290: _LL6098: yyval=( struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct*
_temp6117=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6117[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6118; _temp6118.tag= Cyc_Primopopt_tok_tag;
_temp6118.f1= 0; _temp6118;}); _temp6117;}); break; case 291: _LL6116: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6120=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6120[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6121; _temp6121.tag= Cyc_Primopopt_tok_tag;
_temp6121.f1=({ struct Cyc_Core_Opt* _temp6122=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6122->v=( void*)(( void*) Cyc_Absyn_Times);
_temp6122;}); _temp6121;}); _temp6120;}); break; case 292: _LL6119: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6124=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6124[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6125; _temp6125.tag= Cyc_Primopopt_tok_tag;
_temp6125.f1=({ struct Cyc_Core_Opt* _temp6126=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6126->v=( void*)(( void*) Cyc_Absyn_Div);
_temp6126;}); _temp6125;}); _temp6124;}); break; case 293: _LL6123: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6128=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6128[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6129; _temp6129.tag= Cyc_Primopopt_tok_tag;
_temp6129.f1=({ struct Cyc_Core_Opt* _temp6130=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6130->v=( void*)(( void*) Cyc_Absyn_Mod);
_temp6130;}); _temp6129;}); _temp6128;}); break; case 294: _LL6127: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6132=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6132[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6133; _temp6133.tag= Cyc_Primopopt_tok_tag;
_temp6133.f1=({ struct Cyc_Core_Opt* _temp6134=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6134->v=( void*)(( void*) Cyc_Absyn_Plus);
_temp6134;}); _temp6133;}); _temp6132;}); break; case 295: _LL6131: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6136=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6136[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6137; _temp6137.tag= Cyc_Primopopt_tok_tag;
_temp6137.f1=({ struct Cyc_Core_Opt* _temp6138=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6138->v=( void*)(( void*) Cyc_Absyn_Minus);
_temp6138;}); _temp6137;}); _temp6136;}); break; case 296: _LL6135: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6140=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6140[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6141; _temp6141.tag= Cyc_Primopopt_tok_tag;
_temp6141.f1=({ struct Cyc_Core_Opt* _temp6142=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6142->v=( void*)(( void*) Cyc_Absyn_Bitlshift);
_temp6142;}); _temp6141;}); _temp6140;}); break; case 297: _LL6139: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6144=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6144[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6145; _temp6145.tag= Cyc_Primopopt_tok_tag;
_temp6145.f1=({ struct Cyc_Core_Opt* _temp6146=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6146->v=( void*)(( void*) Cyc_Absyn_Bitlrshift);
_temp6146;}); _temp6145;}); _temp6144;}); break; case 298: _LL6143: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6148=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6148[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6149; _temp6149.tag= Cyc_Primopopt_tok_tag;
_temp6149.f1=({ struct Cyc_Core_Opt* _temp6150=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6150->v=( void*)(( void*) Cyc_Absyn_Bitand);
_temp6150;}); _temp6149;}); _temp6148;}); break; case 299: _LL6147: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6152=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6152[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6153; _temp6153.tag= Cyc_Primopopt_tok_tag;
_temp6153.f1=({ struct Cyc_Core_Opt* _temp6154=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6154->v=( void*)(( void*) Cyc_Absyn_Bitxor);
_temp6154;}); _temp6153;}); _temp6152;}); break; case 300: _LL6151: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6156=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6156[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6157; _temp6157.tag= Cyc_Primopopt_tok_tag;
_temp6157.f1=({ struct Cyc_Core_Opt* _temp6158=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6158->v=( void*)(( void*) Cyc_Absyn_Bitor);
_temp6158;}); _temp6157;}); _temp6156;}); break; case 301: _LL6155: yyval=({
struct _tagged_ptr2 _temp6160= yyvs; struct _xtunion_struct** _temp6162=
_temp6160.curr + yyvsp_offset; if( _temp6162 < _temp6160.base? 1: _temp6162 >=
_temp6160.last_plus_one){ _throw( Null_Exception);}* _temp6162;}); break; case
302: _LL6159: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6164=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6164[ 0]=({ struct Cyc_Exp_tok_struct _temp6165; _temp6165.tag= Cyc_Exp_tok_tag;
_temp6165.f1= Cyc_Absyn_conditional_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6166= yyvs; struct _xtunion_struct** _temp6168= _temp6166.curr
+( yyvsp_offset - 4); if( _temp6168 < _temp6166.base? 1: _temp6168 >= _temp6166.last_plus_one){
_throw( Null_Exception);}* _temp6168;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6169= yyvs; struct _xtunion_struct** _temp6171= _temp6169.curr
+( yyvsp_offset - 2); if( _temp6171 < _temp6169.base? 1: _temp6171 >= _temp6169.last_plus_one){
_throw( Null_Exception);}* _temp6171;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6172= yyvs; struct _xtunion_struct** _temp6174= _temp6172.curr
+ yyvsp_offset; if( _temp6174 < _temp6172.base? 1: _temp6174 >= _temp6172.last_plus_one){
_throw( Null_Exception);}* _temp6174;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6175= yyls; struct Cyc_Yyltype* _temp6177= _temp6175.curr +(
yylsp_offset - 4); if( _temp6177 < _temp6175.base? 1: _temp6177 >= _temp6175.last_plus_one){
_throw( Null_Exception);}* _temp6177;}).first_line,({ struct _tagged_ptr3
_temp6178= yyls; struct Cyc_Yyltype* _temp6180= _temp6178.curr + yylsp_offset;
if( _temp6180 < _temp6178.base? 1: _temp6180 >= _temp6178.last_plus_one){ _throw(
Null_Exception);}* _temp6180;}).last_line)); _temp6165;}); _temp6164;}); break;
case 303: _LL6163: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6182=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6182[ 0]=({ struct Cyc_Exp_tok_struct _temp6183; _temp6183.tag= Cyc_Exp_tok_tag;
_temp6183.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6184= yyvs; struct _xtunion_struct** _temp6186= _temp6184.curr +
yyvsp_offset; if( _temp6186 < _temp6184.base? 1: _temp6186 >= _temp6184.last_plus_one){
_throw( Null_Exception);}* _temp6186;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6187= yyls; struct Cyc_Yyltype* _temp6189= _temp6187.curr +(
yylsp_offset - 1); if( _temp6189 < _temp6187.base? 1: _temp6189 >= _temp6187.last_plus_one){
_throw( Null_Exception);}* _temp6189;}).first_line,({ struct _tagged_ptr3
_temp6190= yyls; struct Cyc_Yyltype* _temp6192= _temp6190.curr + yylsp_offset;
if( _temp6192 < _temp6190.base? 1: _temp6192 >= _temp6190.last_plus_one){ _throw(
Null_Exception);}* _temp6192;}).last_line)); _temp6183;}); _temp6182;}); break;
case 304: _LL6181: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6194=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6194[ 0]=({ struct Cyc_Exp_tok_struct _temp6195; _temp6195.tag= Cyc_Exp_tok_tag;
_temp6195.f1= Cyc_Absyn_New_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6196= yyvs; struct _xtunion_struct** _temp6198= _temp6196.curr +
yyvsp_offset; if( _temp6198 < _temp6196.base? 1: _temp6198 >= _temp6196.last_plus_one){
_throw( Null_Exception);}* _temp6198;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6199= yyls; struct Cyc_Yyltype* _temp6201= _temp6199.curr +(
yylsp_offset - 1); if( _temp6201 < _temp6199.base? 1: _temp6201 >= _temp6199.last_plus_one){
_throw( Null_Exception);}* _temp6201;}).first_line,({ struct _tagged_ptr3
_temp6202= yyls; struct Cyc_Yyltype* _temp6204= _temp6202.curr +( yylsp_offset +
1); if( _temp6204 < _temp6202.base? 1: _temp6204 >= _temp6202.last_plus_one){
_throw( Null_Exception);}* _temp6204;}).last_line)); _temp6195;}); _temp6194;});
break; case 305: _LL6193: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6206=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6206[ 0]=({ struct Cyc_Exp_tok_struct _temp6207; _temp6207.tag= Cyc_Exp_tok_tag;
_temp6207.f1= Cyc_Absyn_New_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6208= yyvs; struct _xtunion_struct** _temp6210= _temp6208.curr +
yyvsp_offset; if( _temp6210 < _temp6208.base? 1: _temp6210 >= _temp6208.last_plus_one){
_throw( Null_Exception);}* _temp6210;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6211= yyls; struct Cyc_Yyltype* _temp6213= _temp6211.curr +(
yylsp_offset - 1); if( _temp6213 < _temp6211.base? 1: _temp6213 >= _temp6211.last_plus_one){
_throw( Null_Exception);}* _temp6213;}).first_line,({ struct _tagged_ptr3
_temp6214= yyls; struct Cyc_Yyltype* _temp6216= _temp6214.curr +( yylsp_offset +
1); if( _temp6216 < _temp6214.base? 1: _temp6216 >= _temp6214.last_plus_one){
_throw( Null_Exception);}* _temp6216;}).last_line)); _temp6207;}); _temp6206;});
break; case 306: _LL6205: yyval=({ struct _tagged_ptr2 _temp6218= yyvs; struct
_xtunion_struct** _temp6220= _temp6218.curr + yyvsp_offset; if( _temp6220 <
_temp6218.base? 1: _temp6220 >= _temp6218.last_plus_one){ _throw( Null_Exception);}*
_temp6220;}); break; case 307: _LL6217: yyval=({ struct _tagged_ptr2 _temp6222=
yyvs; struct _xtunion_struct** _temp6224= _temp6222.curr + yyvsp_offset; if(
_temp6224 < _temp6222.base? 1: _temp6224 >= _temp6222.last_plus_one){ _throw(
Null_Exception);}* _temp6224;}); break; case 308: _LL6221: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6226=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6226[ 0]=({ struct Cyc_Exp_tok_struct
_temp6227; _temp6227.tag= Cyc_Exp_tok_tag; _temp6227.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6228= yyvs; struct _xtunion_struct** _temp6230=
_temp6228.curr +( yyvsp_offset - 2); if( _temp6230 < _temp6228.base? 1:
_temp6230 >= _temp6228.last_plus_one){ _throw( Null_Exception);}* _temp6230;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6231= yyvs; struct _xtunion_struct**
_temp6233= _temp6231.curr + yyvsp_offset; if( _temp6233 < _temp6231.base? 1:
_temp6233 >= _temp6231.last_plus_one){ _throw( Null_Exception);}* _temp6233;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6234= yyls; struct Cyc_Yyltype*
_temp6236= _temp6234.curr +( yylsp_offset - 2); if( _temp6236 < _temp6234.base?
1: _temp6236 >= _temp6234.last_plus_one){ _throw( Null_Exception);}* _temp6236;}).first_line,({
struct _tagged_ptr3 _temp6237= yyls; struct Cyc_Yyltype* _temp6239= _temp6237.curr
+ yylsp_offset; if( _temp6239 < _temp6237.base? 1: _temp6239 >= _temp6237.last_plus_one){
_throw( Null_Exception);}* _temp6239;}).last_line)); _temp6227;}); _temp6226;});
break; case 309: _LL6225: yyval=({ struct _tagged_ptr2 _temp6241= yyvs; struct
_xtunion_struct** _temp6243= _temp6241.curr + yyvsp_offset; if( _temp6243 <
_temp6241.base? 1: _temp6243 >= _temp6241.last_plus_one){ _throw( Null_Exception);}*
_temp6243;}); break; case 310: _LL6240: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6245=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6245[ 0]=({ struct Cyc_Exp_tok_struct
_temp6246; _temp6246.tag= Cyc_Exp_tok_tag; _temp6246.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6247= yyvs; struct _xtunion_struct** _temp6249=
_temp6247.curr +( yyvsp_offset - 2); if( _temp6249 < _temp6247.base? 1:
_temp6249 >= _temp6247.last_plus_one){ _throw( Null_Exception);}* _temp6249;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6250= yyvs; struct _xtunion_struct**
_temp6252= _temp6250.curr + yyvsp_offset; if( _temp6252 < _temp6250.base? 1:
_temp6252 >= _temp6250.last_plus_one){ _throw( Null_Exception);}* _temp6252;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6253= yyls; struct Cyc_Yyltype*
_temp6255= _temp6253.curr +( yylsp_offset - 2); if( _temp6255 < _temp6253.base?
1: _temp6255 >= _temp6253.last_plus_one){ _throw( Null_Exception);}* _temp6255;}).first_line,({
struct _tagged_ptr3 _temp6256= yyls; struct Cyc_Yyltype* _temp6258= _temp6256.curr
+ yylsp_offset; if( _temp6258 < _temp6256.base? 1: _temp6258 >= _temp6256.last_plus_one){
_throw( Null_Exception);}* _temp6258;}).last_line)); _temp6246;}); _temp6245;});
break; case 311: _LL6244: yyval=({ struct _tagged_ptr2 _temp6260= yyvs; struct
_xtunion_struct** _temp6262= _temp6260.curr + yyvsp_offset; if( _temp6262 <
_temp6260.base? 1: _temp6262 >= _temp6260.last_plus_one){ _throw( Null_Exception);}*
_temp6262;}); break; case 312: _LL6259: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6264=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6264[ 0]=({ struct Cyc_Exp_tok_struct
_temp6265; _temp6265.tag= Cyc_Exp_tok_tag; _temp6265.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6266= yyvs;
struct _xtunion_struct** _temp6268= _temp6266.curr +( yyvsp_offset - 2); if(
_temp6268 < _temp6266.base? 1: _temp6268 >= _temp6266.last_plus_one){ _throw(
Null_Exception);}* _temp6268;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6269= yyvs; struct _xtunion_struct** _temp6271= _temp6269.curr +
yyvsp_offset; if( _temp6271 < _temp6269.base? 1: _temp6271 >= _temp6269.last_plus_one){
_throw( Null_Exception);}* _temp6271;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6272= yyls; struct Cyc_Yyltype* _temp6274= _temp6272.curr +(
yylsp_offset - 2); if( _temp6274 < _temp6272.base? 1: _temp6274 >= _temp6272.last_plus_one){
_throw( Null_Exception);}* _temp6274;}).first_line,({ struct _tagged_ptr3
_temp6275= yyls; struct Cyc_Yyltype* _temp6277= _temp6275.curr + yylsp_offset;
if( _temp6277 < _temp6275.base? 1: _temp6277 >= _temp6275.last_plus_one){ _throw(
Null_Exception);}* _temp6277;}).last_line)); _temp6265;}); _temp6264;}); break;
case 313: _LL6263: yyval=({ struct _tagged_ptr2 _temp6279= yyvs; struct
_xtunion_struct** _temp6281= _temp6279.curr + yyvsp_offset; if( _temp6281 <
_temp6279.base? 1: _temp6281 >= _temp6279.last_plus_one){ _throw( Null_Exception);}*
_temp6281;}); break; case 314: _LL6278: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6283=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6283[ 0]=({ struct Cyc_Exp_tok_struct
_temp6284; _temp6284.tag= Cyc_Exp_tok_tag; _temp6284.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6285=
yyvs; struct _xtunion_struct** _temp6287= _temp6285.curr +( yyvsp_offset - 2);
if( _temp6287 < _temp6285.base? 1: _temp6287 >= _temp6285.last_plus_one){ _throw(
Null_Exception);}* _temp6287;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6288= yyvs; struct _xtunion_struct** _temp6290= _temp6288.curr +
yyvsp_offset; if( _temp6290 < _temp6288.base? 1: _temp6290 >= _temp6288.last_plus_one){
_throw( Null_Exception);}* _temp6290;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6291= yyls; struct Cyc_Yyltype* _temp6293= _temp6291.curr +(
yylsp_offset - 2); if( _temp6293 < _temp6291.base? 1: _temp6293 >= _temp6291.last_plus_one){
_throw( Null_Exception);}* _temp6293;}).first_line,({ struct _tagged_ptr3
_temp6294= yyls; struct Cyc_Yyltype* _temp6296= _temp6294.curr + yylsp_offset;
if( _temp6296 < _temp6294.base? 1: _temp6296 >= _temp6294.last_plus_one){ _throw(
Null_Exception);}* _temp6296;}).last_line)); _temp6284;}); _temp6283;}); break;
case 315: _LL6282: yyval=({ struct _tagged_ptr2 _temp6298= yyvs; struct
_xtunion_struct** _temp6300= _temp6298.curr + yyvsp_offset; if( _temp6300 <
_temp6298.base? 1: _temp6300 >= _temp6298.last_plus_one){ _throw( Null_Exception);}*
_temp6300;}); break; case 316: _LL6297: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6302=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6302[ 0]=({ struct Cyc_Exp_tok_struct
_temp6303; _temp6303.tag= Cyc_Exp_tok_tag; _temp6303.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6304=
yyvs; struct _xtunion_struct** _temp6306= _temp6304.curr +( yyvsp_offset - 2);
if( _temp6306 < _temp6304.base? 1: _temp6306 >= _temp6304.last_plus_one){ _throw(
Null_Exception);}* _temp6306;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6307= yyvs; struct _xtunion_struct** _temp6309= _temp6307.curr +
yyvsp_offset; if( _temp6309 < _temp6307.base? 1: _temp6309 >= _temp6307.last_plus_one){
_throw( Null_Exception);}* _temp6309;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6310= yyls; struct Cyc_Yyltype* _temp6312= _temp6310.curr +(
yylsp_offset - 2); if( _temp6312 < _temp6310.base? 1: _temp6312 >= _temp6310.last_plus_one){
_throw( Null_Exception);}* _temp6312;}).first_line,({ struct _tagged_ptr3
_temp6313= yyls; struct Cyc_Yyltype* _temp6315= _temp6313.curr + yylsp_offset;
if( _temp6315 < _temp6313.base? 1: _temp6315 >= _temp6313.last_plus_one){ _throw(
Null_Exception);}* _temp6315;}).last_line)); _temp6303;}); _temp6302;}); break;
case 317: _LL6301: yyval=({ struct _tagged_ptr2 _temp6317= yyvs; struct
_xtunion_struct** _temp6319= _temp6317.curr + yyvsp_offset; if( _temp6319 <
_temp6317.base? 1: _temp6319 >= _temp6317.last_plus_one){ _throw( Null_Exception);}*
_temp6319;}); break; case 318: _LL6316: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6321=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6321[ 0]=({ struct Cyc_Exp_tok_struct
_temp6322; _temp6322.tag= Cyc_Exp_tok_tag; _temp6322.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6323= yyvs; struct _xtunion_struct** _temp6325=
_temp6323.curr +( yyvsp_offset - 2); if( _temp6325 < _temp6323.base? 1:
_temp6325 >= _temp6323.last_plus_one){ _throw( Null_Exception);}* _temp6325;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6326= yyvs; struct _xtunion_struct**
_temp6328= _temp6326.curr + yyvsp_offset; if( _temp6328 < _temp6326.base? 1:
_temp6328 >= _temp6326.last_plus_one){ _throw( Null_Exception);}* _temp6328;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6329= yyls; struct Cyc_Yyltype*
_temp6331= _temp6329.curr +( yylsp_offset - 2); if( _temp6331 < _temp6329.base?
1: _temp6331 >= _temp6329.last_plus_one){ _throw( Null_Exception);}* _temp6331;}).first_line,({
struct _tagged_ptr3 _temp6332= yyls; struct Cyc_Yyltype* _temp6334= _temp6332.curr
+ yylsp_offset; if( _temp6334 < _temp6332.base? 1: _temp6334 >= _temp6332.last_plus_one){
_throw( Null_Exception);}* _temp6334;}).last_line)); _temp6322;}); _temp6321;});
break; case 319: _LL6320: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6336=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6336[ 0]=({ struct Cyc_Exp_tok_struct _temp6337; _temp6337.tag= Cyc_Exp_tok_tag;
_temp6337.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6338= yyvs; struct _xtunion_struct** _temp6340= _temp6338.curr +(
yyvsp_offset - 2); if( _temp6340 < _temp6338.base? 1: _temp6340 >= _temp6338.last_plus_one){
_throw( Null_Exception);}* _temp6340;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6341= yyvs; struct _xtunion_struct** _temp6343= _temp6341.curr
+ yyvsp_offset; if( _temp6343 < _temp6341.base? 1: _temp6343 >= _temp6341.last_plus_one){
_throw( Null_Exception);}* _temp6343;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6344= yyls; struct Cyc_Yyltype* _temp6346= _temp6344.curr +(
yylsp_offset - 2); if( _temp6346 < _temp6344.base? 1: _temp6346 >= _temp6344.last_plus_one){
_throw( Null_Exception);}* _temp6346;}).first_line,({ struct _tagged_ptr3
_temp6347= yyls; struct Cyc_Yyltype* _temp6349= _temp6347.curr + yylsp_offset;
if( _temp6349 < _temp6347.base? 1: _temp6349 >= _temp6347.last_plus_one){ _throw(
Null_Exception);}* _temp6349;}).last_line)); _temp6337;}); _temp6336;}); break;
case 320: _LL6335: yyval=({ struct _tagged_ptr2 _temp6351= yyvs; struct
_xtunion_struct** _temp6353= _temp6351.curr + yyvsp_offset; if( _temp6353 <
_temp6351.base? 1: _temp6353 >= _temp6351.last_plus_one){ _throw( Null_Exception);}*
_temp6353;}); break; case 321: _LL6350: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6355=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6355[ 0]=({ struct Cyc_Exp_tok_struct
_temp6356; _temp6356.tag= Cyc_Exp_tok_tag; _temp6356.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6357= yyvs; struct _xtunion_struct** _temp6359=
_temp6357.curr +( yyvsp_offset - 2); if( _temp6359 < _temp6357.base? 1:
_temp6359 >= _temp6357.last_plus_one){ _throw( Null_Exception);}* _temp6359;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6360= yyvs; struct _xtunion_struct**
_temp6362= _temp6360.curr + yyvsp_offset; if( _temp6362 < _temp6360.base? 1:
_temp6362 >= _temp6360.last_plus_one){ _throw( Null_Exception);}* _temp6362;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6363= yyls; struct Cyc_Yyltype*
_temp6365= _temp6363.curr +( yylsp_offset - 2); if( _temp6365 < _temp6363.base?
1: _temp6365 >= _temp6363.last_plus_one){ _throw( Null_Exception);}* _temp6365;}).first_line,({
struct _tagged_ptr3 _temp6366= yyls; struct Cyc_Yyltype* _temp6368= _temp6366.curr
+ yylsp_offset; if( _temp6368 < _temp6366.base? 1: _temp6368 >= _temp6366.last_plus_one){
_throw( Null_Exception);}* _temp6368;}).last_line)); _temp6356;}); _temp6355;});
break; case 322: _LL6354: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6370=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6370[ 0]=({ struct Cyc_Exp_tok_struct _temp6371; _temp6371.tag= Cyc_Exp_tok_tag;
_temp6371.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6372= yyvs; struct _xtunion_struct** _temp6374= _temp6372.curr +(
yyvsp_offset - 2); if( _temp6374 < _temp6372.base? 1: _temp6374 >= _temp6372.last_plus_one){
_throw( Null_Exception);}* _temp6374;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6375= yyvs; struct _xtunion_struct** _temp6377= _temp6375.curr
+ yyvsp_offset; if( _temp6377 < _temp6375.base? 1: _temp6377 >= _temp6375.last_plus_one){
_throw( Null_Exception);}* _temp6377;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6378= yyls; struct Cyc_Yyltype* _temp6380= _temp6378.curr +(
yylsp_offset - 2); if( _temp6380 < _temp6378.base? 1: _temp6380 >= _temp6378.last_plus_one){
_throw( Null_Exception);}* _temp6380;}).first_line,({ struct _tagged_ptr3
_temp6381= yyls; struct Cyc_Yyltype* _temp6383= _temp6381.curr + yylsp_offset;
if( _temp6383 < _temp6381.base? 1: _temp6383 >= _temp6381.last_plus_one){ _throw(
Null_Exception);}* _temp6383;}).last_line)); _temp6371;}); _temp6370;}); break;
case 323: _LL6369: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6385=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6385[ 0]=({ struct Cyc_Exp_tok_struct _temp6386; _temp6386.tag= Cyc_Exp_tok_tag;
_temp6386.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6387= yyvs; struct _xtunion_struct** _temp6389= _temp6387.curr +(
yyvsp_offset - 2); if( _temp6389 < _temp6387.base? 1: _temp6389 >= _temp6387.last_plus_one){
_throw( Null_Exception);}* _temp6389;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6390= yyvs; struct _xtunion_struct** _temp6392= _temp6390.curr
+ yyvsp_offset; if( _temp6392 < _temp6390.base? 1: _temp6392 >= _temp6390.last_plus_one){
_throw( Null_Exception);}* _temp6392;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6393= yyls; struct Cyc_Yyltype* _temp6395= _temp6393.curr +(
yylsp_offset - 2); if( _temp6395 < _temp6393.base? 1: _temp6395 >= _temp6393.last_plus_one){
_throw( Null_Exception);}* _temp6395;}).first_line,({ struct _tagged_ptr3
_temp6396= yyls; struct Cyc_Yyltype* _temp6398= _temp6396.curr + yylsp_offset;
if( _temp6398 < _temp6396.base? 1: _temp6398 >= _temp6396.last_plus_one){ _throw(
Null_Exception);}* _temp6398;}).last_line)); _temp6386;}); _temp6385;}); break;
case 324: _LL6384: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6400=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6400[ 0]=({ struct Cyc_Exp_tok_struct _temp6401; _temp6401.tag= Cyc_Exp_tok_tag;
_temp6401.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6402= yyvs; struct _xtunion_struct** _temp6404= _temp6402.curr +(
yyvsp_offset - 2); if( _temp6404 < _temp6402.base? 1: _temp6404 >= _temp6402.last_plus_one){
_throw( Null_Exception);}* _temp6404;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6405= yyvs; struct _xtunion_struct** _temp6407= _temp6405.curr
+ yyvsp_offset; if( _temp6407 < _temp6405.base? 1: _temp6407 >= _temp6405.last_plus_one){
_throw( Null_Exception);}* _temp6407;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6408= yyls; struct Cyc_Yyltype* _temp6410= _temp6408.curr +(
yylsp_offset - 2); if( _temp6410 < _temp6408.base? 1: _temp6410 >= _temp6408.last_plus_one){
_throw( Null_Exception);}* _temp6410;}).first_line,({ struct _tagged_ptr3
_temp6411= yyls; struct Cyc_Yyltype* _temp6413= _temp6411.curr + yylsp_offset;
if( _temp6413 < _temp6411.base? 1: _temp6413 >= _temp6411.last_plus_one){ _throw(
Null_Exception);}* _temp6413;}).last_line)); _temp6401;}); _temp6400;}); break;
case 325: _LL6399: yyval=({ struct _tagged_ptr2 _temp6415= yyvs; struct
_xtunion_struct** _temp6417= _temp6415.curr + yyvsp_offset; if( _temp6417 <
_temp6415.base? 1: _temp6417 >= _temp6415.last_plus_one){ _throw( Null_Exception);}*
_temp6417;}); break; case 326: _LL6414: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6419=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6419[ 0]=({ struct Cyc_Exp_tok_struct
_temp6420; _temp6420.tag= Cyc_Exp_tok_tag; _temp6420.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6421=
yyvs; struct _xtunion_struct** _temp6423= _temp6421.curr +( yyvsp_offset - 2);
if( _temp6423 < _temp6421.base? 1: _temp6423 >= _temp6421.last_plus_one){ _throw(
Null_Exception);}* _temp6423;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6424= yyvs; struct _xtunion_struct** _temp6426= _temp6424.curr +
yyvsp_offset; if( _temp6426 < _temp6424.base? 1: _temp6426 >= _temp6424.last_plus_one){
_throw( Null_Exception);}* _temp6426;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6427= yyls; struct Cyc_Yyltype* _temp6429= _temp6427.curr +(
yylsp_offset - 2); if( _temp6429 < _temp6427.base? 1: _temp6429 >= _temp6427.last_plus_one){
_throw( Null_Exception);}* _temp6429;}).first_line,({ struct _tagged_ptr3
_temp6430= yyls; struct Cyc_Yyltype* _temp6432= _temp6430.curr + yylsp_offset;
if( _temp6432 < _temp6430.base? 1: _temp6432 >= _temp6430.last_plus_one){ _throw(
Null_Exception);}* _temp6432;}).last_line)); _temp6420;}); _temp6419;}); break;
case 327: _LL6418: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6434=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6434[ 0]=({ struct Cyc_Exp_tok_struct _temp6435; _temp6435.tag= Cyc_Exp_tok_tag;
_temp6435.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6436= yyvs; struct _xtunion_struct** _temp6438=
_temp6436.curr +( yyvsp_offset - 2); if( _temp6438 < _temp6436.base? 1:
_temp6438 >= _temp6436.last_plus_one){ _throw( Null_Exception);}* _temp6438;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6439= yyvs; struct _xtunion_struct**
_temp6441= _temp6439.curr + yyvsp_offset; if( _temp6441 < _temp6439.base? 1:
_temp6441 >= _temp6439.last_plus_one){ _throw( Null_Exception);}* _temp6441;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6442= yyls; struct Cyc_Yyltype*
_temp6444= _temp6442.curr +( yylsp_offset - 2); if( _temp6444 < _temp6442.base?
1: _temp6444 >= _temp6442.last_plus_one){ _throw( Null_Exception);}* _temp6444;}).first_line,({
struct _tagged_ptr3 _temp6445= yyls; struct Cyc_Yyltype* _temp6447= _temp6445.curr
+ yylsp_offset; if( _temp6447 < _temp6445.base? 1: _temp6447 >= _temp6445.last_plus_one){
_throw( Null_Exception);}* _temp6447;}).last_line)); _temp6435;}); _temp6434;});
break; case 328: _LL6433: yyval=({ struct _tagged_ptr2 _temp6449= yyvs; struct
_xtunion_struct** _temp6451= _temp6449.curr + yyvsp_offset; if( _temp6451 <
_temp6449.base? 1: _temp6451 >= _temp6449.last_plus_one){ _throw( Null_Exception);}*
_temp6451;}); break; case 329: _LL6448: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6453=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6453[ 0]=({ struct Cyc_Exp_tok_struct
_temp6454; _temp6454.tag= Cyc_Exp_tok_tag; _temp6454.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6455= yyvs;
struct _xtunion_struct** _temp6457= _temp6455.curr +( yyvsp_offset - 2); if(
_temp6457 < _temp6455.base? 1: _temp6457 >= _temp6455.last_plus_one){ _throw(
Null_Exception);}* _temp6457;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6458= yyvs; struct _xtunion_struct** _temp6460= _temp6458.curr +
yyvsp_offset; if( _temp6460 < _temp6458.base? 1: _temp6460 >= _temp6458.last_plus_one){
_throw( Null_Exception);}* _temp6460;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6461= yyls; struct Cyc_Yyltype* _temp6463= _temp6461.curr +(
yylsp_offset - 2); if( _temp6463 < _temp6461.base? 1: _temp6463 >= _temp6461.last_plus_one){
_throw( Null_Exception);}* _temp6463;}).first_line,({ struct _tagged_ptr3
_temp6464= yyls; struct Cyc_Yyltype* _temp6466= _temp6464.curr + yylsp_offset;
if( _temp6466 < _temp6464.base? 1: _temp6466 >= _temp6464.last_plus_one){ _throw(
Null_Exception);}* _temp6466;}).last_line)); _temp6454;}); _temp6453;}); break;
case 330: _LL6452: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6468=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6468[ 0]=({ struct Cyc_Exp_tok_struct _temp6469; _temp6469.tag= Cyc_Exp_tok_tag;
_temp6469.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6470= yyvs; struct _xtunion_struct** _temp6472=
_temp6470.curr +( yyvsp_offset - 2); if( _temp6472 < _temp6470.base? 1:
_temp6472 >= _temp6470.last_plus_one){ _throw( Null_Exception);}* _temp6472;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6473= yyvs; struct _xtunion_struct**
_temp6475= _temp6473.curr + yyvsp_offset; if( _temp6475 < _temp6473.base? 1:
_temp6475 >= _temp6473.last_plus_one){ _throw( Null_Exception);}* _temp6475;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6476= yyls; struct Cyc_Yyltype*
_temp6478= _temp6476.curr +( yylsp_offset - 2); if( _temp6478 < _temp6476.base?
1: _temp6478 >= _temp6476.last_plus_one){ _throw( Null_Exception);}* _temp6478;}).first_line,({
struct _tagged_ptr3 _temp6479= yyls; struct Cyc_Yyltype* _temp6481= _temp6479.curr
+ yylsp_offset; if( _temp6481 < _temp6479.base? 1: _temp6481 >= _temp6479.last_plus_one){
_throw( Null_Exception);}* _temp6481;}).last_line)); _temp6469;}); _temp6468;});
break; case 331: _LL6467: yyval=({ struct _tagged_ptr2 _temp6483= yyvs; struct
_xtunion_struct** _temp6485= _temp6483.curr + yyvsp_offset; if( _temp6485 <
_temp6483.base? 1: _temp6485 >= _temp6483.last_plus_one){ _throw( Null_Exception);}*
_temp6485;}); break; case 332: _LL6482: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6487=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6487[ 0]=({ struct Cyc_Exp_tok_struct
_temp6488; _temp6488.tag= Cyc_Exp_tok_tag; _temp6488.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Times, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6489= yyvs;
struct _xtunion_struct** _temp6491= _temp6489.curr +( yyvsp_offset - 2); if(
_temp6491 < _temp6489.base? 1: _temp6491 >= _temp6489.last_plus_one){ _throw(
Null_Exception);}* _temp6491;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6492= yyvs; struct _xtunion_struct** _temp6494= _temp6492.curr +
yyvsp_offset; if( _temp6494 < _temp6492.base? 1: _temp6494 >= _temp6492.last_plus_one){
_throw( Null_Exception);}* _temp6494;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6495= yyls; struct Cyc_Yyltype* _temp6497= _temp6495.curr +(
yylsp_offset - 2); if( _temp6497 < _temp6495.base? 1: _temp6497 >= _temp6495.last_plus_one){
_throw( Null_Exception);}* _temp6497;}).first_line,({ struct _tagged_ptr3
_temp6498= yyls; struct Cyc_Yyltype* _temp6500= _temp6498.curr + yylsp_offset;
if( _temp6500 < _temp6498.base? 1: _temp6500 >= _temp6498.last_plus_one){ _throw(
Null_Exception);}* _temp6500;}).last_line)); _temp6488;}); _temp6487;}); break;
case 333: _LL6486: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6502=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6502[ 0]=({ struct Cyc_Exp_tok_struct _temp6503; _temp6503.tag= Cyc_Exp_tok_tag;
_temp6503.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6504= yyvs; struct _xtunion_struct** _temp6506=
_temp6504.curr +( yyvsp_offset - 2); if( _temp6506 < _temp6504.base? 1:
_temp6506 >= _temp6504.last_plus_one){ _throw( Null_Exception);}* _temp6506;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6507= yyvs; struct _xtunion_struct**
_temp6509= _temp6507.curr + yyvsp_offset; if( _temp6509 < _temp6507.base? 1:
_temp6509 >= _temp6507.last_plus_one){ _throw( Null_Exception);}* _temp6509;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6510= yyls; struct Cyc_Yyltype*
_temp6512= _temp6510.curr +( yylsp_offset - 2); if( _temp6512 < _temp6510.base?
1: _temp6512 >= _temp6510.last_plus_one){ _throw( Null_Exception);}* _temp6512;}).first_line,({
struct _tagged_ptr3 _temp6513= yyls; struct Cyc_Yyltype* _temp6515= _temp6513.curr
+ yylsp_offset; if( _temp6515 < _temp6513.base? 1: _temp6515 >= _temp6513.last_plus_one){
_throw( Null_Exception);}* _temp6515;}).last_line)); _temp6503;}); _temp6502;});
break; case 334: _LL6501: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6517=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6517[ 0]=({ struct Cyc_Exp_tok_struct _temp6518; _temp6518.tag= Cyc_Exp_tok_tag;
_temp6518.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6519= yyvs; struct _xtunion_struct** _temp6521=
_temp6519.curr +( yyvsp_offset - 2); if( _temp6521 < _temp6519.base? 1:
_temp6521 >= _temp6519.last_plus_one){ _throw( Null_Exception);}* _temp6521;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6522= yyvs; struct _xtunion_struct**
_temp6524= _temp6522.curr + yyvsp_offset; if( _temp6524 < _temp6522.base? 1:
_temp6524 >= _temp6522.last_plus_one){ _throw( Null_Exception);}* _temp6524;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6525= yyls; struct Cyc_Yyltype*
_temp6527= _temp6525.curr +( yylsp_offset - 2); if( _temp6527 < _temp6525.base?
1: _temp6527 >= _temp6525.last_plus_one){ _throw( Null_Exception);}* _temp6527;}).first_line,({
struct _tagged_ptr3 _temp6528= yyls; struct Cyc_Yyltype* _temp6530= _temp6528.curr
+ yylsp_offset; if( _temp6530 < _temp6528.base? 1: _temp6530 >= _temp6528.last_plus_one){
_throw( Null_Exception);}* _temp6530;}).last_line)); _temp6518;}); _temp6517;});
break; case 335: _LL6516: yyval=({ struct _tagged_ptr2 _temp6532= yyvs; struct
_xtunion_struct** _temp6534= _temp6532.curr + yyvsp_offset; if( _temp6534 <
_temp6532.base? 1: _temp6534 >= _temp6532.last_plus_one){ _throw( Null_Exception);}*
_temp6534;}); break; case 336: _LL6531: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6536=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6536[ 0]=({ struct Cyc_Exp_tok_struct
_temp6537; _temp6537.tag= Cyc_Exp_tok_tag; _temp6537.f1= Cyc_Absyn_cast_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6538= yyvs; struct
_xtunion_struct** _temp6540= _temp6538.curr +( yyvsp_offset - 2); if( _temp6540
< _temp6538.base? 1: _temp6540 >= _temp6538.last_plus_one){ _throw(
Null_Exception);}* _temp6540;}))).f3, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6541= yyvs; struct _xtunion_struct** _temp6543= _temp6541.curr +
yyvsp_offset; if( _temp6543 < _temp6541.base? 1: _temp6543 >= _temp6541.last_plus_one){
_throw( Null_Exception);}* _temp6543;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6544= yyls; struct Cyc_Yyltype* _temp6546= _temp6544.curr +(
yylsp_offset - 3); if( _temp6546 < _temp6544.base? 1: _temp6546 >= _temp6544.last_plus_one){
_throw( Null_Exception);}* _temp6546;}).first_line,({ struct _tagged_ptr3
_temp6547= yyls; struct Cyc_Yyltype* _temp6549= _temp6547.curr + yylsp_offset;
if( _temp6549 < _temp6547.base? 1: _temp6549 >= _temp6547.last_plus_one){ _throw(
Null_Exception);}* _temp6549;}).last_line)); _temp6537;}); _temp6536;}); break;
case 337: _LL6535: yyval=({ struct _tagged_ptr2 _temp6551= yyvs; struct
_xtunion_struct** _temp6553= _temp6551.curr + yyvsp_offset; if( _temp6553 <
_temp6551.base? 1: _temp6553 >= _temp6551.last_plus_one){ _throw( Null_Exception);}*
_temp6553;}); break; case 338: _LL6550: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6555=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6555[ 0]=({ struct Cyc_Exp_tok_struct
_temp6556; _temp6556.tag= Cyc_Exp_tok_tag; _temp6556.f1= Cyc_Absyn_pre_inc_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6557= yyvs; struct _xtunion_struct**
_temp6559= _temp6557.curr + yyvsp_offset; if( _temp6559 < _temp6557.base? 1:
_temp6559 >= _temp6557.last_plus_one){ _throw( Null_Exception);}* _temp6559;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6560= yyls; struct Cyc_Yyltype*
_temp6562= _temp6560.curr +( yylsp_offset - 1); if( _temp6562 < _temp6560.base?
1: _temp6562 >= _temp6560.last_plus_one){ _throw( Null_Exception);}* _temp6562;}).first_line,({
struct _tagged_ptr3 _temp6563= yyls; struct Cyc_Yyltype* _temp6565= _temp6563.curr
+ yylsp_offset; if( _temp6565 < _temp6563.base? 1: _temp6565 >= _temp6563.last_plus_one){
_throw( Null_Exception);}* _temp6565;}).last_line)); _temp6556;}); _temp6555;});
break; case 339: _LL6554: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6567=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6567[ 0]=({ struct Cyc_Exp_tok_struct _temp6568; _temp6568.tag= Cyc_Exp_tok_tag;
_temp6568.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6569= yyvs; struct _xtunion_struct** _temp6571= _temp6569.curr +
yyvsp_offset; if( _temp6571 < _temp6569.base? 1: _temp6571 >= _temp6569.last_plus_one){
_throw( Null_Exception);}* _temp6571;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6572= yyls; struct Cyc_Yyltype* _temp6574= _temp6572.curr +(
yylsp_offset - 1); if( _temp6574 < _temp6572.base? 1: _temp6574 >= _temp6572.last_plus_one){
_throw( Null_Exception);}* _temp6574;}).first_line,({ struct _tagged_ptr3
_temp6575= yyls; struct Cyc_Yyltype* _temp6577= _temp6575.curr + yylsp_offset;
if( _temp6577 < _temp6575.base? 1: _temp6577 >= _temp6575.last_plus_one){ _throw(
Null_Exception);}* _temp6577;}).last_line)); _temp6568;}); _temp6567;}); break;
case 340: _LL6566: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6579=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6579[ 0]=({ struct Cyc_Exp_tok_struct _temp6580; _temp6580.tag= Cyc_Exp_tok_tag;
_temp6580.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6581= yyvs; struct _xtunion_struct** _temp6583= _temp6581.curr +
yyvsp_offset; if( _temp6583 < _temp6581.base? 1: _temp6583 >= _temp6581.last_plus_one){
_throw( Null_Exception);}* _temp6583;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6584= yyls; struct Cyc_Yyltype* _temp6586= _temp6584.curr +(
yylsp_offset - 1); if( _temp6586 < _temp6584.base? 1: _temp6586 >= _temp6584.last_plus_one){
_throw( Null_Exception);}* _temp6586;}).first_line,({ struct _tagged_ptr3
_temp6587= yyls; struct Cyc_Yyltype* _temp6589= _temp6587.curr + yylsp_offset;
if( _temp6589 < _temp6587.base? 1: _temp6589 >= _temp6587.last_plus_one){ _throw(
Null_Exception);}* _temp6589;}).last_line)); _temp6580;}); _temp6579;}); break;
case 341: _LL6578: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6591=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6591[ 0]=({ struct Cyc_Exp_tok_struct _temp6592; _temp6592.tag= Cyc_Exp_tok_tag;
_temp6592.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6593= yyvs; struct _xtunion_struct** _temp6595= _temp6593.curr +
yyvsp_offset; if( _temp6595 < _temp6593.base? 1: _temp6595 >= _temp6593.last_plus_one){
_throw( Null_Exception);}* _temp6595;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6596= yyls; struct Cyc_Yyltype* _temp6598= _temp6596.curr +(
yylsp_offset - 1); if( _temp6598 < _temp6596.base? 1: _temp6598 >= _temp6596.last_plus_one){
_throw( Null_Exception);}* _temp6598;}).first_line,({ struct _tagged_ptr3
_temp6599= yyls; struct Cyc_Yyltype* _temp6601= _temp6599.curr + yylsp_offset;
if( _temp6601 < _temp6599.base? 1: _temp6601 >= _temp6599.last_plus_one){ _throw(
Null_Exception);}* _temp6601;}).last_line)); _temp6592;}); _temp6591;}); break;
case 342: _LL6590: yyval=({ struct _tagged_ptr2 _temp6603= yyvs; struct
_xtunion_struct** _temp6605= _temp6603.curr + yyvsp_offset; if( _temp6605 <
_temp6603.base? 1: _temp6605 >= _temp6603.last_plus_one){ _throw( Null_Exception);}*
_temp6605;}); break; case 343: _LL6602: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6607=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6607[ 0]=({ struct Cyc_Exp_tok_struct
_temp6608; _temp6608.tag= Cyc_Exp_tok_tag; _temp6608.f1= Cyc_Absyn_prim1_exp(
Cyc_yyget_Primop_tok(({ struct _tagged_ptr2 _temp6609= yyvs; struct
_xtunion_struct** _temp6611= _temp6609.curr +( yyvsp_offset - 1); if( _temp6611
< _temp6609.base? 1: _temp6611 >= _temp6609.last_plus_one){ _throw(
Null_Exception);}* _temp6611;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6612= yyvs; struct _xtunion_struct** _temp6614= _temp6612.curr +
yyvsp_offset; if( _temp6614 < _temp6612.base? 1: _temp6614 >= _temp6612.last_plus_one){
_throw( Null_Exception);}* _temp6614;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6615= yyls; struct Cyc_Yyltype* _temp6617= _temp6615.curr +(
yylsp_offset - 1); if( _temp6617 < _temp6615.base? 1: _temp6617 >= _temp6615.last_plus_one){
_throw( Null_Exception);}* _temp6617;}).first_line,({ struct _tagged_ptr3
_temp6618= yyls; struct Cyc_Yyltype* _temp6620= _temp6618.curr + yylsp_offset;
if( _temp6620 < _temp6618.base? 1: _temp6620 >= _temp6618.last_plus_one){ _throw(
Null_Exception);}* _temp6620;}).last_line)); _temp6608;}); _temp6607;}); break;
case 344: _LL6606: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6622=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6622[ 0]=({ struct Cyc_Exp_tok_struct _temp6623; _temp6623.tag= Cyc_Exp_tok_tag;
_temp6623.f1= Cyc_Absyn_sizeoftyp_exp((* Cyc_yyget_ParamDecl_tok(({ struct
_tagged_ptr2 _temp6624= yyvs; struct _xtunion_struct** _temp6626= _temp6624.curr
+( yyvsp_offset - 1); if( _temp6626 < _temp6624.base? 1: _temp6626 >= _temp6624.last_plus_one){
_throw( Null_Exception);}* _temp6626;}))).f3, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6627= yyls; struct Cyc_Yyltype* _temp6629= _temp6627.curr
+( yylsp_offset - 3); if( _temp6629 < _temp6627.base? 1: _temp6629 >= _temp6627.last_plus_one){
_throw( Null_Exception);}* _temp6629;}).first_line,({ struct _tagged_ptr3
_temp6630= yyls; struct Cyc_Yyltype* _temp6632= _temp6630.curr + yylsp_offset;
if( _temp6632 < _temp6630.base? 1: _temp6632 >= _temp6630.last_plus_one){ _throw(
Null_Exception);}* _temp6632;}).last_line)); _temp6623;}); _temp6622;}); break;
case 345: _LL6621: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6634=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6634[ 0]=({ struct Cyc_Exp_tok_struct _temp6635; _temp6635.tag= Cyc_Exp_tok_tag;
_temp6635.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6636= yyvs; struct _xtunion_struct** _temp6638= _temp6636.curr +
yyvsp_offset; if( _temp6638 < _temp6636.base? 1: _temp6638 >= _temp6636.last_plus_one){
_throw( Null_Exception);}* _temp6638;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6639= yyls; struct Cyc_Yyltype* _temp6641= _temp6639.curr +(
yylsp_offset - 1); if( _temp6641 < _temp6639.base? 1: _temp6641 >= _temp6639.last_plus_one){
_throw( Null_Exception);}* _temp6641;}).first_line,({ struct _tagged_ptr3
_temp6642= yyls; struct Cyc_Yyltype* _temp6644= _temp6642.curr + yylsp_offset;
if( _temp6644 < _temp6642.base? 1: _temp6644 >= _temp6642.last_plus_one){ _throw(
Null_Exception);}* _temp6644;}).last_line)); _temp6635;}); _temp6634;}); break;
case 346: _LL6633: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6646=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6646[ 0]=({ struct Cyc_Exp_tok_struct _temp6647; _temp6647.tag= Cyc_Exp_tok_tag;
_temp6647.f1= Cyc_Absyn_primop_exp( Cyc_yyget_Primop_tok(({ struct _tagged_ptr2
_temp6648= yyvs; struct _xtunion_struct** _temp6650= _temp6648.curr +(
yyvsp_offset - 3); if( _temp6650 < _temp6648.base? 1: _temp6650 >= _temp6648.last_plus_one){
_throw( Null_Exception);}* _temp6650;})), Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp6651= yyvs; struct _xtunion_struct** _temp6653= _temp6651.curr
+( yyvsp_offset - 1); if( _temp6653 < _temp6651.base? 1: _temp6653 >= _temp6651.last_plus_one){
_throw( Null_Exception);}* _temp6653;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6654= yyls; struct Cyc_Yyltype* _temp6656= _temp6654.curr +(
yylsp_offset - 3); if( _temp6656 < _temp6654.base? 1: _temp6656 >= _temp6654.last_plus_one){
_throw( Null_Exception);}* _temp6656;}).first_line,({ struct _tagged_ptr3
_temp6657= yyls; struct Cyc_Yyltype* _temp6659= _temp6657.curr + yylsp_offset;
if( _temp6659 < _temp6657.base? 1: _temp6659 >= _temp6657.last_plus_one){ _throw(
Null_Exception);}* _temp6659;}).last_line)); _temp6647;}); _temp6646;}); break;
case 347: _LL6645: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6661=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6661[ 0]=({ struct Cyc_Exp_tok_struct _temp6662; _temp6662.tag= Cyc_Exp_tok_tag;
_temp6662.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp6663=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp6663[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp6664; _temp6664.tag= Cyc_Absyn_Malloc_e_tag;
_temp6664.f1=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp6665= yyvs; struct _xtunion_struct** _temp6667=
_temp6665.curr +( yyvsp_offset - 2); if( _temp6667 < _temp6665.base? 1:
_temp6667 >= _temp6665.last_plus_one){ _throw( Null_Exception);}* _temp6667;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6668= yyls; struct Cyc_Yyltype*
_temp6670= _temp6668.curr +( yylsp_offset - 2); if( _temp6670 < _temp6668.base?
1: _temp6670 >= _temp6668.last_plus_one){ _throw( Null_Exception);}* _temp6670;}).first_line,({
struct _tagged_ptr3 _temp6671= yyls; struct Cyc_Yyltype* _temp6673= _temp6671.curr
+( yylsp_offset - 2); if( _temp6673 < _temp6671.base? 1: _temp6673 >= _temp6671.last_plus_one){
_throw( Null_Exception);}* _temp6673;}).last_line)); _temp6664;}); _temp6663;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6674= yyls; struct Cyc_Yyltype*
_temp6676= _temp6674.curr +( yylsp_offset - 6); if( _temp6676 < _temp6674.base?
1: _temp6676 >= _temp6674.last_plus_one){ _throw( Null_Exception);}* _temp6676;}).first_line,({
struct _tagged_ptr3 _temp6677= yyls; struct Cyc_Yyltype* _temp6679= _temp6677.curr
+ yylsp_offset; if( _temp6679 < _temp6677.base? 1: _temp6679 >= _temp6677.last_plus_one){
_throw( Null_Exception);}* _temp6679;}).last_line)); _temp6662;}); _temp6661;});
break; case 348: _LL6660: yyval=( struct _xtunion_struct*)({ struct Cyc_Primop_tok_struct*
_temp6681=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6681[ 0]=({ struct Cyc_Primop_tok_struct _temp6682; _temp6682.tag= Cyc_Primop_tok_tag;
_temp6682.f1=( void*)(( void*) Cyc_Absyn_Printf); _temp6682;}); _temp6681;});
break; case 349: _LL6680: yyval=( struct _xtunion_struct*)({ struct Cyc_Primop_tok_struct*
_temp6684=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6684[ 0]=({ struct Cyc_Primop_tok_struct _temp6685; _temp6685.tag= Cyc_Primop_tok_tag;
_temp6685.f1=( void*)(( void*) Cyc_Absyn_Fprintf); _temp6685;}); _temp6684;});
break; case 350: _LL6683: yyval=( struct _xtunion_struct*)({ struct Cyc_Primop_tok_struct*
_temp6687=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6687[ 0]=({ struct Cyc_Primop_tok_struct _temp6688; _temp6688.tag= Cyc_Primop_tok_tag;
_temp6688.f1=( void*)(( void*) Cyc_Absyn_Xprintf); _temp6688;}); _temp6687;});
break; case 351: _LL6686: yyval=( struct _xtunion_struct*)({ struct Cyc_Primop_tok_struct*
_temp6690=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6690[ 0]=({ struct Cyc_Primop_tok_struct _temp6691; _temp6691.tag= Cyc_Primop_tok_tag;
_temp6691.f1=( void*)(( void*) Cyc_Absyn_Scanf); _temp6691;}); _temp6690;});
break; case 352: _LL6689: yyval=( struct _xtunion_struct*)({ struct Cyc_Primop_tok_struct*
_temp6693=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6693[ 0]=({ struct Cyc_Primop_tok_struct _temp6694; _temp6694.tag= Cyc_Primop_tok_tag;
_temp6694.f1=( void*)(( void*) Cyc_Absyn_Fscanf); _temp6694;}); _temp6693;});
break; case 353: _LL6692: yyval=( struct _xtunion_struct*)({ struct Cyc_Primop_tok_struct*
_temp6696=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6696[ 0]=({ struct Cyc_Primop_tok_struct _temp6697; _temp6697.tag= Cyc_Primop_tok_tag;
_temp6697.f1=( void*)(( void*) Cyc_Absyn_Sscanf); _temp6697;}); _temp6696;});
break; case 354: _LL6695: yyval=( struct _xtunion_struct*)({ struct Cyc_Primop_tok_struct*
_temp6699=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6699[ 0]=({ struct Cyc_Primop_tok_struct _temp6700; _temp6700.tag= Cyc_Primop_tok_tag;
_temp6700.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp6700;}); _temp6699;});
break; case 355: _LL6698: yyval=( struct _xtunion_struct*)({ struct Cyc_Primop_tok_struct*
_temp6702=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6702[ 0]=({ struct Cyc_Primop_tok_struct _temp6703; _temp6703.tag= Cyc_Primop_tok_tag;
_temp6703.f1=( void*)(( void*) Cyc_Absyn_Not); _temp6703;}); _temp6702;});
break; case 356: _LL6701: yyval=( struct _xtunion_struct*)({ struct Cyc_Primop_tok_struct*
_temp6705=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6705[ 0]=({ struct Cyc_Primop_tok_struct _temp6706; _temp6706.tag= Cyc_Primop_tok_tag;
_temp6706.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp6706;}); _temp6705;});
break; case 357: _LL6704: yyval=({ struct _tagged_ptr2 _temp6708= yyvs; struct
_xtunion_struct** _temp6710= _temp6708.curr + yyvsp_offset; if( _temp6710 <
_temp6708.base? 1: _temp6710 >= _temp6708.last_plus_one){ _throw( Null_Exception);}*
_temp6710;}); break; case 358: _LL6707: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6712=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6712[ 0]=({ struct Cyc_Exp_tok_struct
_temp6713; _temp6713.tag= Cyc_Exp_tok_tag; _temp6713.f1= Cyc_Absyn_subscript_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6714= yyvs; struct _xtunion_struct**
_temp6716= _temp6714.curr +( yyvsp_offset - 3); if( _temp6716 < _temp6714.base?
1: _temp6716 >= _temp6714.last_plus_one){ _throw( Null_Exception);}* _temp6716;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6717= yyvs; struct _xtunion_struct**
_temp6719= _temp6717.curr +( yyvsp_offset - 1); if( _temp6719 < _temp6717.base?
1: _temp6719 >= _temp6717.last_plus_one){ _throw( Null_Exception);}* _temp6719;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6720= yyls; struct Cyc_Yyltype*
_temp6722= _temp6720.curr +( yylsp_offset - 3); if( _temp6722 < _temp6720.base?
1: _temp6722 >= _temp6720.last_plus_one){ _throw( Null_Exception);}* _temp6722;}).first_line,({
struct _tagged_ptr3 _temp6723= yyls; struct Cyc_Yyltype* _temp6725= _temp6723.curr
+ yylsp_offset; if( _temp6725 < _temp6723.base? 1: _temp6725 >= _temp6723.last_plus_one){
_throw( Null_Exception);}* _temp6725;}).last_line)); _temp6713;}); _temp6712;});
break; case 359: _LL6711: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6727=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6727[ 0]=({ struct Cyc_Exp_tok_struct _temp6728; _temp6728.tag= Cyc_Exp_tok_tag;
_temp6728.f1= Cyc_Absyn_unknowncall_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6729= yyvs; struct _xtunion_struct** _temp6731= _temp6729.curr
+( yyvsp_offset - 2); if( _temp6731 < _temp6729.base? 1: _temp6731 >= _temp6729.last_plus_one){
_throw( Null_Exception);}* _temp6731;})), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6732= yyls; struct Cyc_Yyltype* _temp6734= _temp6732.curr
+( yylsp_offset - 2); if( _temp6734 < _temp6732.base? 1: _temp6734 >= _temp6732.last_plus_one){
_throw( Null_Exception);}* _temp6734;}).first_line,({ struct _tagged_ptr3
_temp6735= yyls; struct Cyc_Yyltype* _temp6737= _temp6735.curr + yylsp_offset;
if( _temp6737 < _temp6735.base? 1: _temp6737 >= _temp6735.last_plus_one){ _throw(
Null_Exception);}* _temp6737;}).last_line)); _temp6728;}); _temp6727;}); break;
case 360: _LL6726: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6739=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6739[ 0]=({ struct Cyc_Exp_tok_struct _temp6740; _temp6740.tag= Cyc_Exp_tok_tag;
_temp6740.f1= Cyc_Absyn_unknowncall_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6741= yyvs; struct _xtunion_struct** _temp6743= _temp6741.curr
+( yyvsp_offset - 3); if( _temp6743 < _temp6741.base? 1: _temp6743 >= _temp6741.last_plus_one){
_throw( Null_Exception);}* _temp6743;})), Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp6744= yyvs; struct _xtunion_struct** _temp6746= _temp6744.curr
+( yyvsp_offset - 1); if( _temp6746 < _temp6744.base? 1: _temp6746 >= _temp6744.last_plus_one){
_throw( Null_Exception);}* _temp6746;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6747= yyls; struct Cyc_Yyltype* _temp6749= _temp6747.curr +(
yylsp_offset - 3); if( _temp6749 < _temp6747.base? 1: _temp6749 >= _temp6747.last_plus_one){
_throw( Null_Exception);}* _temp6749;}).first_line,({ struct _tagged_ptr3
_temp6750= yyls; struct Cyc_Yyltype* _temp6752= _temp6750.curr + yylsp_offset;
if( _temp6752 < _temp6750.base? 1: _temp6752 >= _temp6750.last_plus_one){ _throw(
Null_Exception);}* _temp6752;}).last_line)); _temp6740;}); _temp6739;}); break;
case 361: _LL6738: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6754=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6754[ 0]=({ struct Cyc_Exp_tok_struct _temp6755; _temp6755.tag= Cyc_Exp_tok_tag;
_temp6755.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6756= yyvs; struct _xtunion_struct** _temp6758= _temp6756.curr
+( yyvsp_offset - 2); if( _temp6758 < _temp6756.base? 1: _temp6758 >= _temp6756.last_plus_one){
_throw( Null_Exception);}* _temp6758;})),({ struct _tagged_string* _temp6759=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp6759[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6760= yyvs;
struct _xtunion_struct** _temp6762= _temp6760.curr + yyvsp_offset; if( _temp6762
< _temp6760.base? 1: _temp6762 >= _temp6760.last_plus_one){ _throw(
Null_Exception);}* _temp6762;})); _temp6759;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6763= yyls; struct Cyc_Yyltype* _temp6765= _temp6763.curr
+( yylsp_offset - 2); if( _temp6765 < _temp6763.base? 1: _temp6765 >= _temp6763.last_plus_one){
_throw( Null_Exception);}* _temp6765;}).first_line,({ struct _tagged_ptr3
_temp6766= yyls; struct Cyc_Yyltype* _temp6768= _temp6766.curr + yylsp_offset;
if( _temp6768 < _temp6766.base? 1: _temp6768 >= _temp6766.last_plus_one){ _throw(
Null_Exception);}* _temp6768;}).last_line)); _temp6755;}); _temp6754;}); break;
case 362: _LL6753: { struct _tuple1* q= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp6770= yyvs; struct _xtunion_struct** _temp6772= _temp6770.curr
+ yyvsp_offset; if( _temp6772 < _temp6770.base? 1: _temp6772 >= _temp6770.last_plus_one){
_throw( Null_Exception);}* _temp6772;}));{ void* _temp6773=(* q).f1; struct Cyc_List_List*
_temp6783; struct Cyc_List_List* _temp6785; _LL6775: if(( int) _temp6773 == Cyc_Absyn_Loc_n){
goto _LL6776;} else{ goto _LL6777;} _LL6777: if(( unsigned int) _temp6773 > 1u?((
struct _tunion_struct*) _temp6773)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL6784:
_temp6783=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp6773)->f1;
if( _temp6783 == 0){ goto _LL6778;} else{ goto _LL6779;}} else{ goto _LL6779;}
_LL6779: if(( unsigned int) _temp6773 > 1u?(( struct _tunion_struct*) _temp6773)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6786: _temp6785=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6773)->f1; if( _temp6785 == 0){ goto _LL6780;}
else{ goto _LL6781;}} else{ goto _LL6781;} _LL6781: goto _LL6782; _LL6776: goto
_LL6774; _LL6778: goto _LL6774; _LL6780: goto _LL6774; _LL6782: Cyc_Parse_err((
struct _tagged_string)({ char* _temp6787=( char*)"struct field name is qualified";
struct _tagged_string _temp6788; _temp6788.curr= _temp6787; _temp6788.base=
_temp6787; _temp6788.last_plus_one= _temp6787 + 31; _temp6788;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6789= yyls; struct Cyc_Yyltype* _temp6791= _temp6789.curr
+ yylsp_offset; if( _temp6791 < _temp6789.base? 1: _temp6791 >= _temp6789.last_plus_one){
_throw( Null_Exception);}* _temp6791;}).first_line,({ struct _tagged_ptr3
_temp6792= yyls; struct Cyc_Yyltype* _temp6794= _temp6792.curr + yylsp_offset;
if( _temp6794 < _temp6792.base? 1: _temp6794 >= _temp6792.last_plus_one){ _throw(
Null_Exception);}* _temp6794;}).last_line)); goto _LL6774; _LL6774:;} yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6795=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6795[ 0]=({ struct Cyc_Exp_tok_struct
_temp6796; _temp6796.tag= Cyc_Exp_tok_tag; _temp6796.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6797= yyvs; struct _xtunion_struct**
_temp6799= _temp6797.curr +( yyvsp_offset - 2); if( _temp6799 < _temp6797.base?
1: _temp6799 >= _temp6797.last_plus_one){ _throw( Null_Exception);}* _temp6799;})),(*
q).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6800= yyls;
struct Cyc_Yyltype* _temp6802= _temp6800.curr +( yylsp_offset - 2); if(
_temp6802 < _temp6800.base? 1: _temp6802 >= _temp6800.last_plus_one){ _throw(
Null_Exception);}* _temp6802;}).first_line,({ struct _tagged_ptr3 _temp6803=
yyls; struct Cyc_Yyltype* _temp6805= _temp6803.curr + yylsp_offset; if(
_temp6805 < _temp6803.base? 1: _temp6805 >= _temp6803.last_plus_one){ _throw(
Null_Exception);}* _temp6805;}).last_line)); _temp6796;}); _temp6795;}); break;}
case 363: _LL6769: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6807=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6807[ 0]=({ struct Cyc_Exp_tok_struct _temp6808; _temp6808.tag= Cyc_Exp_tok_tag;
_temp6808.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6809= yyvs; struct _xtunion_struct** _temp6811= _temp6809.curr
+( yyvsp_offset - 2); if( _temp6811 < _temp6809.base? 1: _temp6811 >= _temp6809.last_plus_one){
_throw( Null_Exception);}* _temp6811;})),({ struct _tagged_string* _temp6812=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp6812[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6813= yyvs;
struct _xtunion_struct** _temp6815= _temp6813.curr + yyvsp_offset; if( _temp6815
< _temp6813.base? 1: _temp6815 >= _temp6813.last_plus_one){ _throw(
Null_Exception);}* _temp6815;})); _temp6812;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6816= yyls; struct Cyc_Yyltype* _temp6818= _temp6816.curr
+( yylsp_offset - 2); if( _temp6818 < _temp6816.base? 1: _temp6818 >= _temp6816.last_plus_one){
_throw( Null_Exception);}* _temp6818;}).first_line,({ struct _tagged_ptr3
_temp6819= yyls; struct Cyc_Yyltype* _temp6821= _temp6819.curr + yylsp_offset;
if( _temp6821 < _temp6819.base? 1: _temp6821 >= _temp6819.last_plus_one){ _throw(
Null_Exception);}* _temp6821;}).last_line)); _temp6808;}); _temp6807;}); break;
case 364: _LL6806: { struct _tuple1* q= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp6823= yyvs; struct _xtunion_struct** _temp6825= _temp6823.curr
+ yyvsp_offset; if( _temp6825 < _temp6823.base? 1: _temp6825 >= _temp6823.last_plus_one){
_throw( Null_Exception);}* _temp6825;}));{ void* _temp6826=(* q).f1; struct Cyc_List_List*
_temp6836; struct Cyc_List_List* _temp6838; _LL6828: if(( int) _temp6826 == Cyc_Absyn_Loc_n){
goto _LL6829;} else{ goto _LL6830;} _LL6830: if(( unsigned int) _temp6826 > 1u?((
struct _tunion_struct*) _temp6826)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL6837:
_temp6836=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp6826)->f1;
if( _temp6836 == 0){ goto _LL6831;} else{ goto _LL6832;}} else{ goto _LL6832;}
_LL6832: if(( unsigned int) _temp6826 > 1u?(( struct _tunion_struct*) _temp6826)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6839: _temp6838=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6826)->f1; if( _temp6838 == 0){ goto _LL6833;}
else{ goto _LL6834;}} else{ goto _LL6834;} _LL6834: goto _LL6835; _LL6829: goto
_LL6827; _LL6831: goto _LL6827; _LL6833: goto _LL6827; _LL6835: Cyc_Parse_err((
struct _tagged_string)({ char* _temp6840=( char*)"struct field is qualified with module name";
struct _tagged_string _temp6841; _temp6841.curr= _temp6840; _temp6841.base=
_temp6840; _temp6841.last_plus_one= _temp6840 + 43; _temp6841;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6842= yyls; struct Cyc_Yyltype* _temp6844= _temp6842.curr
+ yylsp_offset; if( _temp6844 < _temp6842.base? 1: _temp6844 >= _temp6842.last_plus_one){
_throw( Null_Exception);}* _temp6844;}).first_line,({ struct _tagged_ptr3
_temp6845= yyls; struct Cyc_Yyltype* _temp6847= _temp6845.curr + yylsp_offset;
if( _temp6847 < _temp6845.base? 1: _temp6847 >= _temp6845.last_plus_one){ _throw(
Null_Exception);}* _temp6847;}).last_line)); goto _LL6827; _LL6827:;} yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6848=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6848[ 0]=({ struct Cyc_Exp_tok_struct
_temp6849; _temp6849.tag= Cyc_Exp_tok_tag; _temp6849.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6850= yyvs; struct _xtunion_struct**
_temp6852= _temp6850.curr +( yyvsp_offset - 2); if( _temp6852 < _temp6850.base?
1: _temp6852 >= _temp6850.last_plus_one){ _throw( Null_Exception);}* _temp6852;})),(*
q).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6853= yyls;
struct Cyc_Yyltype* _temp6855= _temp6853.curr +( yylsp_offset - 2); if(
_temp6855 < _temp6853.base? 1: _temp6855 >= _temp6853.last_plus_one){ _throw(
Null_Exception);}* _temp6855;}).first_line,({ struct _tagged_ptr3 _temp6856=
yyls; struct Cyc_Yyltype* _temp6858= _temp6856.curr + yylsp_offset; if(
_temp6858 < _temp6856.base? 1: _temp6858 >= _temp6856.last_plus_one){ _throw(
Null_Exception);}* _temp6858;}).last_line)); _temp6849;}); _temp6848;}); break;}
case 365: _LL6822: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6860=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6860[ 0]=({ struct Cyc_Exp_tok_struct _temp6861; _temp6861.tag= Cyc_Exp_tok_tag;
_temp6861.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6862= yyvs; struct _xtunion_struct** _temp6864= _temp6862.curr +(
yyvsp_offset - 1); if( _temp6864 < _temp6862.base? 1: _temp6864 >= _temp6862.last_plus_one){
_throw( Null_Exception);}* _temp6864;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6865= yyls; struct Cyc_Yyltype* _temp6867= _temp6865.curr +(
yylsp_offset - 1); if( _temp6867 < _temp6865.base? 1: _temp6867 >= _temp6865.last_plus_one){
_throw( Null_Exception);}* _temp6867;}).first_line,({ struct _tagged_ptr3
_temp6868= yyls; struct Cyc_Yyltype* _temp6870= _temp6868.curr + yylsp_offset;
if( _temp6870 < _temp6868.base? 1: _temp6870 >= _temp6868.last_plus_one){ _throw(
Null_Exception);}* _temp6870;}).last_line)); _temp6861;}); _temp6860;}); break;
case 366: _LL6859: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6872=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6872[ 0]=({ struct Cyc_Exp_tok_struct _temp6873; _temp6873.tag= Cyc_Exp_tok_tag;
_temp6873.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6874= yyvs; struct _xtunion_struct** _temp6876= _temp6874.curr +(
yyvsp_offset - 1); if( _temp6876 < _temp6874.base? 1: _temp6876 >= _temp6874.last_plus_one){
_throw( Null_Exception);}* _temp6876;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6877= yyls; struct Cyc_Yyltype* _temp6879= _temp6877.curr +(
yylsp_offset - 1); if( _temp6879 < _temp6877.base? 1: _temp6879 >= _temp6877.last_plus_one){
_throw( Null_Exception);}* _temp6879;}).first_line,({ struct _tagged_ptr3
_temp6880= yyls; struct Cyc_Yyltype* _temp6882= _temp6880.curr + yylsp_offset;
if( _temp6882 < _temp6880.base? 1: _temp6882 >= _temp6880.last_plus_one){ _throw(
Null_Exception);}* _temp6882;}).last_line)); _temp6873;}); _temp6872;}); break;
case 367: _LL6871: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6884=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6884[ 0]=({ struct Cyc_Exp_tok_struct _temp6885; _temp6885.tag= Cyc_Exp_tok_tag;
_temp6885.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_CompoundLit_e_struct*
_temp6886=( struct Cyc_Absyn_CompoundLit_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_CompoundLit_e_struct)); _temp6886[ 0]=({ struct Cyc_Absyn_CompoundLit_e_struct
_temp6887; _temp6887.tag= Cyc_Absyn_CompoundLit_e_tag; _temp6887.f1= Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp6888= yyvs; struct _xtunion_struct** _temp6890=
_temp6888.curr +( yyvsp_offset - 4); if( _temp6890 < _temp6888.base? 1:
_temp6890 >= _temp6888.last_plus_one){ _throw( Null_Exception);}* _temp6890;}));
_temp6887.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2 _temp6891= yyvs; struct
_xtunion_struct** _temp6893= _temp6891.curr +( yyvsp_offset - 1); if( _temp6893
< _temp6891.base? 1: _temp6893 >= _temp6891.last_plus_one){ _throw(
Null_Exception);}* _temp6893;}))); _temp6887;}); _temp6886;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6894= yyls; struct Cyc_Yyltype* _temp6896= _temp6894.curr
+( yylsp_offset - 5); if( _temp6896 < _temp6894.base? 1: _temp6896 >= _temp6894.last_plus_one){
_throw( Null_Exception);}* _temp6896;}).first_line,({ struct _tagged_ptr3
_temp6897= yyls; struct Cyc_Yyltype* _temp6899= _temp6897.curr + yylsp_offset;
if( _temp6899 < _temp6897.base? 1: _temp6899 >= _temp6897.last_plus_one){ _throw(
Null_Exception);}* _temp6899;}).last_line)); _temp6885;}); _temp6884;}); break;
case 368: _LL6883: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6901=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6901[ 0]=({ struct Cyc_Exp_tok_struct _temp6902; _temp6902.tag= Cyc_Exp_tok_tag;
_temp6902.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_CompoundLit_e_struct*
_temp6903=( struct Cyc_Absyn_CompoundLit_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_CompoundLit_e_struct)); _temp6903[ 0]=({ struct Cyc_Absyn_CompoundLit_e_struct
_temp6904; _temp6904.tag= Cyc_Absyn_CompoundLit_e_tag; _temp6904.f1= Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp6905= yyvs; struct _xtunion_struct** _temp6907=
_temp6905.curr +( yyvsp_offset - 5); if( _temp6907 < _temp6905.base? 1:
_temp6907 >= _temp6905.last_plus_one){ _throw( Null_Exception);}* _temp6907;}));
_temp6904.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2 _temp6908= yyvs; struct
_xtunion_struct** _temp6910= _temp6908.curr +( yyvsp_offset - 2); if( _temp6910
< _temp6908.base? 1: _temp6910 >= _temp6908.last_plus_one){ _throw(
Null_Exception);}* _temp6910;}))); _temp6904;}); _temp6903;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6911= yyls; struct Cyc_Yyltype* _temp6913= _temp6911.curr
+( yylsp_offset - 6); if( _temp6913 < _temp6911.base? 1: _temp6913 >= _temp6911.last_plus_one){
_throw( Null_Exception);}* _temp6913;}).first_line,({ struct _tagged_ptr3
_temp6914= yyls; struct Cyc_Yyltype* _temp6916= _temp6914.curr + yylsp_offset;
if( _temp6916 < _temp6914.base? 1: _temp6916 >= _temp6914.last_plus_one){ _throw(
Null_Exception);}* _temp6916;}).last_line)); _temp6902;}); _temp6901;}); break;
case 369: _LL6900: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6918=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6918[ 0]=({ struct Cyc_Exp_tok_struct _temp6919; _temp6919.tag= Cyc_Exp_tok_tag;
_temp6919.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Fill_e_struct*
_temp6920=( struct Cyc_Absyn_Fill_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct));
_temp6920[ 0]=({ struct Cyc_Absyn_Fill_e_struct _temp6921; _temp6921.tag= Cyc_Absyn_Fill_e_tag;
_temp6921.f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6922= yyvs; struct
_xtunion_struct** _temp6924= _temp6922.curr +( yyvsp_offset - 1); if( _temp6924
< _temp6922.base? 1: _temp6924 >= _temp6922.last_plus_one){ _throw(
Null_Exception);}* _temp6924;})); _temp6921;}); _temp6920;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6925= yyls; struct Cyc_Yyltype* _temp6927= _temp6925.curr
+( yylsp_offset - 3); if( _temp6927 < _temp6925.base? 1: _temp6927 >= _temp6925.last_plus_one){
_throw( Null_Exception);}* _temp6927;}).first_line,({ struct _tagged_ptr3
_temp6928= yyls; struct Cyc_Yyltype* _temp6930= _temp6928.curr + yylsp_offset;
if( _temp6930 < _temp6928.base? 1: _temp6930 >= _temp6928.last_plus_one){ _throw(
Null_Exception);}* _temp6930;}).last_line)); _temp6919;}); _temp6918;}); break;
case 370: _LL6917: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6932=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6932[ 0]=({ struct Cyc_Exp_tok_struct _temp6933; _temp6933.tag= Cyc_Exp_tok_tag;
_temp6933.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Codegen_e_struct*
_temp6934=( struct Cyc_Absyn_Codegen_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct));
_temp6934[ 0]=({ struct Cyc_Absyn_Codegen_e_struct _temp6935; _temp6935.tag= Cyc_Absyn_Codegen_e_tag;
_temp6935.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp6936= yyvs;
struct _xtunion_struct** _temp6938= _temp6936.curr +( yyvsp_offset - 1); if(
_temp6938 < _temp6936.base? 1: _temp6938 >= _temp6936.last_plus_one){ _throw(
Null_Exception);}* _temp6938;})); _temp6935;}); _temp6934;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6939= yyls; struct Cyc_Yyltype* _temp6941= _temp6939.curr
+( yylsp_offset - 3); if( _temp6941 < _temp6939.base? 1: _temp6941 >= _temp6939.last_plus_one){
_throw( Null_Exception);}* _temp6941;}).first_line,({ struct _tagged_ptr3
_temp6942= yyls; struct Cyc_Yyltype* _temp6944= _temp6942.curr + yylsp_offset;
if( _temp6944 < _temp6942.base? 1: _temp6944 >= _temp6942.last_plus_one){ _throw(
Null_Exception);}* _temp6944;}).last_line)); _temp6933;}); _temp6932;}); break;
case 371: _LL6931: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6946=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6946[ 0]=({ struct Cyc_Exp_tok_struct _temp6947; _temp6947.tag= Cyc_Exp_tok_tag;
_temp6947.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp6948=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp6948[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp6949; _temp6949.tag=
Cyc_Absyn_UnknownId_e_tag; _temp6949.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp6950= yyvs; struct _xtunion_struct** _temp6952= _temp6950.curr
+ yyvsp_offset; if( _temp6952 < _temp6950.base? 1: _temp6952 >= _temp6950.last_plus_one){
_throw( Null_Exception);}* _temp6952;})); _temp6949;}); _temp6948;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6953= yyls; struct Cyc_Yyltype* _temp6955= _temp6953.curr
+ yylsp_offset; if( _temp6955 < _temp6953.base? 1: _temp6955 >= _temp6953.last_plus_one){
_throw( Null_Exception);}* _temp6955;}).first_line,({ struct _tagged_ptr3
_temp6956= yyls; struct Cyc_Yyltype* _temp6958= _temp6956.curr + yylsp_offset;
if( _temp6958 < _temp6956.base? 1: _temp6958 >= _temp6956.last_plus_one){ _throw(
Null_Exception);}* _temp6958;}).last_line)); _temp6947;}); _temp6946;}); break;
case 372: _LL6945: yyval=({ struct _tagged_ptr2 _temp6960= yyvs; struct
_xtunion_struct** _temp6962= _temp6960.curr + yyvsp_offset; if( _temp6962 <
_temp6960.base? 1: _temp6962 >= _temp6960.last_plus_one){ _throw( Null_Exception);}*
_temp6962;}); break; case 373: _LL6959: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6964=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6964[ 0]=({ struct Cyc_Exp_tok_struct
_temp6965; _temp6965.tag= Cyc_Exp_tok_tag; _temp6965.f1= Cyc_Absyn_string_exp(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6966= yyvs; struct
_xtunion_struct** _temp6968= _temp6966.curr + yyvsp_offset; if( _temp6968 <
_temp6966.base? 1: _temp6968 >= _temp6966.last_plus_one){ _throw( Null_Exception);}*
_temp6968;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6969=
yyls; struct Cyc_Yyltype* _temp6971= _temp6969.curr + yylsp_offset; if(
_temp6971 < _temp6969.base? 1: _temp6971 >= _temp6969.last_plus_one){ _throw(
Null_Exception);}* _temp6971;}).first_line,({ struct _tagged_ptr3 _temp6972=
yyls; struct Cyc_Yyltype* _temp6974= _temp6972.curr + yylsp_offset; if(
_temp6974 < _temp6972.base? 1: _temp6974 >= _temp6972.last_plus_one){ _throw(
Null_Exception);}* _temp6974;}).last_line)); _temp6965;}); _temp6964;}); break;
case 374: _LL6963: yyval=({ struct _tagged_ptr2 _temp6976= yyvs; struct
_xtunion_struct** _temp6978= _temp6976.curr +( yyvsp_offset - 1); if( _temp6978
< _temp6976.base? 1: _temp6978 >= _temp6976.last_plus_one){ _throw(
Null_Exception);}* _temp6978;}); break; case 375: _LL6975: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6980=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6980[ 0]=({ struct Cyc_Exp_tok_struct
_temp6981; _temp6981.tag= Cyc_Exp_tok_tag; _temp6981.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp6982=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp6982[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp6983; _temp6983.tag=
Cyc_Absyn_UnknownId_e_tag; _temp6983.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp6984= yyvs; struct _xtunion_struct** _temp6986= _temp6984.curr
+( yyvsp_offset - 1); if( _temp6986 < _temp6984.base? 1: _temp6986 >= _temp6984.last_plus_one){
_throw( Null_Exception);}* _temp6986;})); _temp6983;}); _temp6982;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6987= yyls; struct Cyc_Yyltype* _temp6989= _temp6987.curr
+( yylsp_offset - 1); if( _temp6989 < _temp6987.base? 1: _temp6989 >= _temp6987.last_plus_one){
_throw( Null_Exception);}* _temp6989;}).first_line,({ struct _tagged_ptr3
_temp6990= yyls; struct Cyc_Yyltype* _temp6992= _temp6990.curr +( yylsp_offset -
1); if( _temp6992 < _temp6990.base? 1: _temp6992 >= _temp6990.last_plus_one){
_throw( Null_Exception);}* _temp6992;}).last_line)), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6993= yyls; struct Cyc_Yyltype* _temp6995= _temp6993.curr
+( yylsp_offset - 1); if( _temp6995 < _temp6993.base? 1: _temp6995 >= _temp6993.last_plus_one){
_throw( Null_Exception);}* _temp6995;}).first_line,({ struct _tagged_ptr3
_temp6996= yyls; struct Cyc_Yyltype* _temp6998= _temp6996.curr + yylsp_offset;
if( _temp6998 < _temp6996.base? 1: _temp6998 >= _temp6996.last_plus_one){ _throw(
Null_Exception);}* _temp6998;}).last_line)); _temp6981;}); _temp6980;}); break;
case 376: _LL6979: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7000=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7000[ 0]=({ struct Cyc_Exp_tok_struct _temp7001; _temp7001.tag= Cyc_Exp_tok_tag;
_temp7001.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp7002=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7002[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7003; _temp7003.tag=
Cyc_Absyn_UnknownId_e_tag; _temp7003.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp7004= yyvs; struct _xtunion_struct** _temp7006= _temp7004.curr
+( yyvsp_offset - 4); if( _temp7006 < _temp7004.base? 1: _temp7006 >= _temp7004.last_plus_one){
_throw( Null_Exception);}* _temp7006;})); _temp7003;}); _temp7002;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7007= yyls; struct Cyc_Yyltype* _temp7009= _temp7007.curr
+( yylsp_offset - 4); if( _temp7009 < _temp7007.base? 1: _temp7009 >= _temp7007.last_plus_one){
_throw( Null_Exception);}* _temp7009;}).first_line,({ struct _tagged_ptr3
_temp7010= yyls; struct Cyc_Yyltype* _temp7012= _temp7010.curr +( yylsp_offset -
4); if( _temp7012 < _temp7010.base? 1: _temp7012 >= _temp7010.last_plus_one){
_throw( Null_Exception);}* _temp7012;}).last_line)),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp7013= yyvs; struct _xtunion_struct** _temp7015= _temp7013.curr
+( yyvsp_offset - 1); if( _temp7015 < _temp7013.base? 1: _temp7015 >= _temp7013.last_plus_one){
_throw( Null_Exception);}* _temp7015;}))), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7016= yyls; struct Cyc_Yyltype* _temp7018= _temp7016.curr +(
yylsp_offset - 4); if( _temp7018 < _temp7016.base? 1: _temp7018 >= _temp7016.last_plus_one){
_throw( Null_Exception);}* _temp7018;}).first_line,({ struct _tagged_ptr3
_temp7019= yyls; struct Cyc_Yyltype* _temp7021= _temp7019.curr + yylsp_offset;
if( _temp7021 < _temp7019.base? 1: _temp7021 >= _temp7019.last_plus_one){ _throw(
Null_Exception);}* _temp7021;}).last_line)); _temp7001;}); _temp7000;}); break;
case 377: _LL6999: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7023=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7023[ 0]=({ struct Cyc_Exp_tok_struct _temp7024; _temp7024.tag= Cyc_Exp_tok_tag;
_temp7024.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2
_temp7025= yyvs; struct _xtunion_struct** _temp7027= _temp7025.curr +(
yyvsp_offset - 1); if( _temp7027 < _temp7025.base? 1: _temp7027 >= _temp7025.last_plus_one){
_throw( Null_Exception);}* _temp7027;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7028= yyls; struct Cyc_Yyltype* _temp7030= _temp7028.curr +(
yylsp_offset - 3); if( _temp7030 < _temp7028.base? 1: _temp7030 >= _temp7028.last_plus_one){
_throw( Null_Exception);}* _temp7030;}).first_line,({ struct _tagged_ptr3
_temp7031= yyls; struct Cyc_Yyltype* _temp7033= _temp7031.curr + yylsp_offset;
if( _temp7033 < _temp7031.base? 1: _temp7033 >= _temp7031.last_plus_one){ _throw(
Null_Exception);}* _temp7033;}).last_line)); _temp7024;}); _temp7023;}); break;
case 378: _LL7022: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7035=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7035[ 0]=({ struct Cyc_Exp_tok_struct _temp7036; _temp7036.tag= Cyc_Exp_tok_tag;
_temp7036.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp7037=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp7037[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp7038; _temp7038.tag= Cyc_Absyn_Struct_e_tag;
_temp7038.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp7039= yyvs;
struct _xtunion_struct** _temp7041= _temp7039.curr +( yyvsp_offset - 3); if(
_temp7041 < _temp7039.base? 1: _temp7041 >= _temp7039.last_plus_one){ _throw(
Null_Exception);}* _temp7041;})); _temp7038.f2= 0; _temp7038.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp7042= yyvs; struct _xtunion_struct** _temp7044=
_temp7042.curr +( yyvsp_offset - 1); if( _temp7044 < _temp7042.base? 1:
_temp7044 >= _temp7042.last_plus_one){ _throw( Null_Exception);}* _temp7044;})));
_temp7038.f4= 0; _temp7038;}); _temp7037;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7045= yyls; struct Cyc_Yyltype* _temp7047= _temp7045.curr
+( yylsp_offset - 3); if( _temp7047 < _temp7045.base? 1: _temp7047 >= _temp7045.last_plus_one){
_throw( Null_Exception);}* _temp7047;}).first_line,({ struct _tagged_ptr3
_temp7048= yyls; struct Cyc_Yyltype* _temp7050= _temp7048.curr + yylsp_offset;
if( _temp7050 < _temp7048.base? 1: _temp7050 >= _temp7048.last_plus_one){ _throw(
Null_Exception);}* _temp7050;}).last_line)); _temp7036;}); _temp7035;}); break;
case 379: _LL7034: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7052=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7052[ 0]=({ struct Cyc_Exp_tok_struct _temp7053; _temp7053.tag= Cyc_Exp_tok_tag;
_temp7053.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp7054= yyvs; struct _xtunion_struct** _temp7056= _temp7054.curr +(
yyvsp_offset - 2); if( _temp7056 < _temp7054.base? 1: _temp7056 >= _temp7054.last_plus_one){
_throw( Null_Exception);}* _temp7056;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7057= yyls; struct Cyc_Yyltype* _temp7059= _temp7057.curr +(
yylsp_offset - 4); if( _temp7059 < _temp7057.base? 1: _temp7059 >= _temp7057.last_plus_one){
_throw( Null_Exception);}* _temp7059;}).first_line,({ struct _tagged_ptr3
_temp7060= yyls; struct Cyc_Yyltype* _temp7062= _temp7060.curr + yylsp_offset;
if( _temp7062 < _temp7060.base? 1: _temp7062 >= _temp7060.last_plus_one){ _throw(
Null_Exception);}* _temp7062;}).last_line)); _temp7053;}); _temp7052;}); break;
case 380: _LL7051: yyval=( struct _xtunion_struct*)({ struct Cyc_ExpList_tok_struct*
_temp7064=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp7064[ 0]=({ struct Cyc_ExpList_tok_struct _temp7065; _temp7065.tag= Cyc_ExpList_tok_tag;
_temp7065.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp7066= yyvs; struct
_xtunion_struct** _temp7068= _temp7066.curr + yyvsp_offset; if( _temp7068 <
_temp7066.base? 1: _temp7068 >= _temp7066.last_plus_one){ _throw( Null_Exception);}*
_temp7068;}))); _temp7065;}); _temp7064;}); break; case 381: _LL7063: yyval=(
struct _xtunion_struct*)({ struct Cyc_ExpList_tok_struct* _temp7070=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp7070[ 0]=({ struct Cyc_ExpList_tok_struct
_temp7071; _temp7071.tag= Cyc_ExpList_tok_tag; _temp7071.f1=({ struct Cyc_List_List*
_temp7072=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp7072->hd=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7073= yyvs;
struct _xtunion_struct** _temp7075= _temp7073.curr + yyvsp_offset; if( _temp7075
< _temp7073.base? 1: _temp7075 >= _temp7073.last_plus_one){ _throw(
Null_Exception);}* _temp7075;})); _temp7072->tl= 0; _temp7072;}); _temp7071;});
_temp7070;}); break; case 382: _LL7069: yyval=( struct _xtunion_struct*)({
struct Cyc_ExpList_tok_struct* _temp7077=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp7077[ 0]=({ struct Cyc_ExpList_tok_struct
_temp7078; _temp7078.tag= Cyc_ExpList_tok_tag; _temp7078.f1=({ struct Cyc_List_List*
_temp7079=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp7079->hd=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7083= yyvs;
struct _xtunion_struct** _temp7085= _temp7083.curr + yyvsp_offset; if( _temp7085
< _temp7083.base? 1: _temp7085 >= _temp7083.last_plus_one){ _throw(
Null_Exception);}* _temp7085;})); _temp7079->tl= Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp7080= yyvs; struct _xtunion_struct** _temp7082= _temp7080.curr
+( yyvsp_offset - 2); if( _temp7082 < _temp7080.base? 1: _temp7082 >= _temp7080.last_plus_one){
_throw( Null_Exception);}* _temp7082;})); _temp7079;}); _temp7078;}); _temp7077;});
break; case 383: _LL7076: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7087=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7087[ 0]=({ struct Cyc_Exp_tok_struct _temp7088; _temp7088.tag= Cyc_Exp_tok_tag;
_temp7088.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(({ struct _tagged_ptr2
_temp7089= yyvs; struct _xtunion_struct** _temp7091= _temp7089.curr +
yyvsp_offset; if( _temp7091 < _temp7089.base? 1: _temp7091 >= _temp7089.last_plus_one){
_throw( Null_Exception);}* _temp7091;}))).f1,(* Cyc_yyget_Int_tok(({ struct
_tagged_ptr2 _temp7092= yyvs; struct _xtunion_struct** _temp7094= _temp7092.curr
+ yyvsp_offset; if( _temp7094 < _temp7092.base? 1: _temp7094 >= _temp7092.last_plus_one){
_throw( Null_Exception);}* _temp7094;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7095= yyls; struct Cyc_Yyltype* _temp7097= _temp7095.curr
+ yylsp_offset; if( _temp7097 < _temp7095.base? 1: _temp7097 >= _temp7095.last_plus_one){
_throw( Null_Exception);}* _temp7097;}).first_line,({ struct _tagged_ptr3
_temp7098= yyls; struct Cyc_Yyltype* _temp7100= _temp7098.curr + yylsp_offset;
if( _temp7100 < _temp7098.base? 1: _temp7100 >= _temp7098.last_plus_one){ _throw(
Null_Exception);}* _temp7100;}).last_line)); _temp7088;}); _temp7087;}); break;
case 384: _LL7086: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7102=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7102[ 0]=({ struct Cyc_Exp_tok_struct _temp7103; _temp7103.tag= Cyc_Exp_tok_tag;
_temp7103.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok(({ struct _tagged_ptr2
_temp7104= yyvs; struct _xtunion_struct** _temp7106= _temp7104.curr +
yyvsp_offset; if( _temp7106 < _temp7104.base? 1: _temp7106 >= _temp7104.last_plus_one){
_throw( Null_Exception);}* _temp7106;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7107= yyls; struct Cyc_Yyltype* _temp7109= _temp7107.curr +
yylsp_offset; if( _temp7109 < _temp7107.base? 1: _temp7109 >= _temp7107.last_plus_one){
_throw( Null_Exception);}* _temp7109;}).first_line,({ struct _tagged_ptr3
_temp7110= yyls; struct Cyc_Yyltype* _temp7112= _temp7110.curr + yylsp_offset;
if( _temp7112 < _temp7110.base? 1: _temp7112 >= _temp7110.last_plus_one){ _throw(
Null_Exception);}* _temp7112;}).last_line)); _temp7103;}); _temp7102;}); break;
case 385: _LL7101: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7114=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7114[ 0]=({ struct Cyc_Exp_tok_struct _temp7115; _temp7115.tag= Cyc_Exp_tok_tag;
_temp7115.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp7116= yyvs; struct _xtunion_struct** _temp7118= _temp7116.curr +
yyvsp_offset; if( _temp7118 < _temp7116.base? 1: _temp7118 >= _temp7116.last_plus_one){
_throw( Null_Exception);}* _temp7118;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7119= yyls; struct Cyc_Yyltype* _temp7121= _temp7119.curr +
yylsp_offset; if( _temp7121 < _temp7119.base? 1: _temp7121 >= _temp7119.last_plus_one){
_throw( Null_Exception);}* _temp7121;}).first_line,({ struct _tagged_ptr3
_temp7122= yyls; struct Cyc_Yyltype* _temp7124= _temp7122.curr + yylsp_offset;
if( _temp7124 < _temp7122.base? 1: _temp7124 >= _temp7122.last_plus_one){ _throw(
Null_Exception);}* _temp7124;}).last_line)); _temp7115;}); _temp7114;}); break;
case 386: _LL7113: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7126=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7126[ 0]=({ struct Cyc_Exp_tok_struct _temp7127; _temp7127.tag= Cyc_Exp_tok_tag;
_temp7127.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7128= yyls; struct Cyc_Yyltype* _temp7130= _temp7128.curr +
yylsp_offset; if( _temp7130 < _temp7128.base? 1: _temp7130 >= _temp7128.last_plus_one){
_throw( Null_Exception);}* _temp7130;}).first_line,({ struct _tagged_ptr3
_temp7131= yyls; struct Cyc_Yyltype* _temp7133= _temp7131.curr + yylsp_offset;
if( _temp7133 < _temp7131.base? 1: _temp7133 >= _temp7131.last_plus_one){ _throw(
Null_Exception);}* _temp7133;}).last_line)); _temp7127;}); _temp7126;}); break;
case 387: _LL7125: yyval=( struct _xtunion_struct*)({ struct Cyc_QualId_tok_struct*
_temp7135=( struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));
_temp7135[ 0]=({ struct Cyc_QualId_tok_struct _temp7136; _temp7136.tag= Cyc_QualId_tok_tag;
_temp7136.f1=({ struct _tuple1* _temp7137=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp7137->f1=( void*)({ struct Cyc_Absyn_Rel_n_struct*
_temp7142=( struct Cyc_Absyn_Rel_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp7142[ 0]=({ struct Cyc_Absyn_Rel_n_struct _temp7143; _temp7143.tag= Cyc_Absyn_Rel_n_tag;
_temp7143.f1= 0; _temp7143;}); _temp7142;}); _temp7137->f2=({ struct
_tagged_string* _temp7138=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp7138[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp7139= yyvs; struct _xtunion_struct** _temp7141= _temp7139.curr +
yyvsp_offset; if( _temp7141 < _temp7139.base? 1: _temp7141 >= _temp7139.last_plus_one){
_throw( Null_Exception);}* _temp7141;})); _temp7138;}); _temp7137;}); _temp7136;});
_temp7135;}); break; case 388: _LL7134: yyval=({ struct _tagged_ptr2 _temp7145=
yyvs; struct _xtunion_struct** _temp7147= _temp7145.curr + yyvsp_offset; if(
_temp7147 < _temp7145.base? 1: _temp7147 >= _temp7145.last_plus_one){ _throw(
Null_Exception);}* _temp7147;}); break; default: _LL7144:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Yyimpossible_struct* _temp7149=( struct Cyc_Yyimpossible_struct*)
GC_malloc( sizeof( struct Cyc_Yyimpossible_struct)); _temp7149[ 0]=({ struct Cyc_Yyimpossible_struct
_temp7150; _temp7150.tag= Cyc_Yyimpossible_tag; _temp7150;}); _temp7149;}));}
yyvsp_offset -= yylen; yyssp_offset -= yylen; yylsp_offset -= yylen;({ struct
_tagged_ptr2 _temp7151= yyvs; struct _xtunion_struct** _temp7153= _temp7151.curr
+( ++ yyvsp_offset); if( _temp7153 < _temp7151.base? 1: _temp7153 >= _temp7151.last_plus_one){
_throw( Null_Exception);}* _temp7153= yyval;}); yylsp_offset ++; if( yylen == 0){({
struct _tagged_ptr3 _temp7154= yyls; struct Cyc_Yyltype* _temp7156= _temp7154.curr
+ yylsp_offset; if( _temp7156 < _temp7154.base? 1: _temp7156 >= _temp7154.last_plus_one){
_throw( Null_Exception);}(* _temp7156).first_line= Cyc_yylloc.first_line;});({
struct _tagged_ptr3 _temp7157= yyls; struct Cyc_Yyltype* _temp7159= _temp7157.curr
+ yylsp_offset; if( _temp7159 < _temp7157.base? 1: _temp7159 >= _temp7157.last_plus_one){
_throw( Null_Exception);}(* _temp7159).first_column= Cyc_yylloc.first_column;});({
struct _tagged_ptr3 _temp7160= yyls; struct Cyc_Yyltype* _temp7162= _temp7160.curr
+ yylsp_offset; if( _temp7162 < _temp7160.base? 1: _temp7162 >= _temp7160.last_plus_one){
_throw( Null_Exception);}(* _temp7162).last_line=({ struct _tagged_ptr3
_temp7163= yyls; struct Cyc_Yyltype* _temp7165= _temp7163.curr +( yylsp_offset -
1); if( _temp7165 < _temp7163.base? 1: _temp7165 >= _temp7163.last_plus_one){
_throw( Null_Exception);}* _temp7165;}).last_line;});({ struct _tagged_ptr3
_temp7166= yyls; struct Cyc_Yyltype* _temp7168= _temp7166.curr + yylsp_offset;
if( _temp7168 < _temp7166.base? 1: _temp7168 >= _temp7166.last_plus_one){ _throw(
Null_Exception);}(* _temp7168).last_column=({ struct _tagged_ptr3 _temp7169=
yyls; struct Cyc_Yyltype* _temp7171= _temp7169.curr +( yylsp_offset - 1); if(
_temp7171 < _temp7169.base? 1: _temp7171 >= _temp7169.last_plus_one){ _throw(
Null_Exception);}* _temp7171;}).last_column;});({ struct _tagged_ptr3 _temp7172=
yyls; struct Cyc_Yyltype* _temp7174= _temp7172.curr + yylsp_offset; if(
_temp7174 < _temp7172.base? 1: _temp7174 >= _temp7172.last_plus_one){ _throw(
Null_Exception);}(* _temp7174).text=( struct _tagged_string)({ char* _temp7175=(
char*)""; struct _tagged_string _temp7176; _temp7176.curr= _temp7175; _temp7176.base=
_temp7175; _temp7176.last_plus_one= _temp7175 + 1; _temp7176;});});} else{({
struct _tagged_ptr3 _temp7177= yyls; struct Cyc_Yyltype* _temp7179= _temp7177.curr
+ yylsp_offset; if( _temp7179 < _temp7177.base? 1: _temp7179 >= _temp7177.last_plus_one){
_throw( Null_Exception);}(* _temp7179).last_line=({ struct _tagged_ptr3
_temp7180= yyls; struct Cyc_Yyltype* _temp7182= _temp7180.curr +(( yylsp_offset
+ yylen) - 1); if( _temp7182 < _temp7180.base? 1: _temp7182 >= _temp7180.last_plus_one){
_throw( Null_Exception);}* _temp7182;}).last_line;});({ struct _tagged_ptr3
_temp7183= yyls; struct Cyc_Yyltype* _temp7185= _temp7183.curr + yylsp_offset;
if( _temp7185 < _temp7183.base? 1: _temp7185 >= _temp7183.last_plus_one){ _throw(
Null_Exception);}(* _temp7185).last_column=({ struct _tagged_ptr3 _temp7186=
yyls; struct Cyc_Yyltype* _temp7188= _temp7186.curr +(( yylsp_offset + yylen) -
1); if( _temp7188 < _temp7186.base? 1: _temp7188 >= _temp7186.last_plus_one){
_throw( Null_Exception);}* _temp7188;}).last_column;});} yyn=( int)({ short*
_temp7189=( short*) Cyc_yyr1; unsigned int _temp7190= yyn; if( _temp7190 >= 389u){
_throw( Null_Exception);} _temp7189[ _temp7190];}); yystate=( int)({ short*
_temp7191=( short*) Cyc_yypgoto; unsigned int _temp7192= yyn - 120; if(
_temp7192 >= 107u){ _throw( Null_Exception);} _temp7191[ _temp7192];}) +( int)({
struct _tagged_ptr1 _temp7193= yyss; short* _temp7195= _temp7193.curr +
yyssp_offset; if( _temp7195 < _temp7193.base? 1: _temp7195 >= _temp7193.last_plus_one){
_throw( Null_Exception);}* _temp7195;}); if(( yystate >= 0? yystate <= 4017: 0)?(
int)({ short* _temp7196=( short*) Cyc_yycheck; unsigned int _temp7197= yystate;
if( _temp7197 >= 4018u){ _throw( Null_Exception);} _temp7196[ _temp7197];}) ==(
int)({ struct _tagged_ptr1 _temp7198= yyss; short* _temp7200= _temp7198.curr +
yyssp_offset; if( _temp7200 < _temp7198.base? 1: _temp7200 >= _temp7198.last_plus_one){
_throw( Null_Exception);}* _temp7200;}): 0){ yystate=( int)({ short* _temp7201=(
short*) Cyc_yytable; unsigned int _temp7202= yystate; if( _temp7202 >= 4018u){
_throw( Null_Exception);} _temp7201[ _temp7202];});} else{ yystate=( int)({
short* _temp7203=( short*) Cyc_yydefgoto; unsigned int _temp7204= yyn - 120; if(
_temp7204 >= 107u){ _throw( Null_Exception);} _temp7203[ _temp7204];});} goto
yynewstate; yyerrlab: if( yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int)({ short*
_temp7205=( short*) Cyc_yypact; unsigned int _temp7206= yystate; if( _temp7206
>= 729u){ _throw( Null_Exception);} _temp7205[ _temp7206];}); if( yyn > - 32768?
yyn < 4017: 0){ int sze= 0; struct _tagged_string msg; int x; int count; count=
0; for( x= yyn < 0? - yyn: 0;( unsigned int) x < 227u / sizeof( char*); x ++){
if(( int)({ short* _temp7207=( short*) Cyc_yycheck; unsigned int _temp7208= x +
yyn; if( _temp7208 >= 4018u){ _throw( Null_Exception);} _temp7207[ _temp7208];})
== x){( unsigned int)( sze += Cyc_String_strlen(({ struct _tagged_string*
_temp7209=( struct _tagged_string*) Cyc_yytname; unsigned int _temp7210= x; if(
_temp7210 >= 227u){ _throw( Null_Exception);} _temp7209[ _temp7210];})) +(
unsigned int) 15), count ++;}} msg=({ unsigned int _temp7211=( unsigned int)(
sze + 15); char* _temp7212=( char*) GC_malloc_atomic( sizeof( char) * _temp7211);
unsigned int i; struct _tagged_string _temp7213={ _temp7212, _temp7212,
_temp7212 + _temp7211}; for( i= 0; i < _temp7211; i ++){ _temp7212[ i]='\000';}
_temp7213;}); Cyc_String_strcpy( msg,( struct _tagged_string)({ char* _temp7214=(
char*)"parse error"; struct _tagged_string _temp7215; _temp7215.curr= _temp7214;
_temp7215.base= _temp7214; _temp7215.last_plus_one= _temp7214 + 12; _temp7215;}));
if( count < 5){ count= 0; for( x= yyn < 0? - yyn: 0;( unsigned int) x < 227u /
sizeof( char*); x ++){ if(( int)({ short* _temp7216=( short*) Cyc_yycheck;
unsigned int _temp7217= x + yyn; if( _temp7217 >= 4018u){ _throw( Null_Exception);}
_temp7216[ _temp7217];}) == x){ Cyc_String_strcat( msg, count == 0?( struct
_tagged_string)({ char* _temp7218=( char*)", expecting `"; struct _tagged_string
_temp7219; _temp7219.curr= _temp7218; _temp7219.base= _temp7218; _temp7219.last_plus_one=
_temp7218 + 14; _temp7219;}):( struct _tagged_string)({ char* _temp7220=( char*)" or `";
struct _tagged_string _temp7221; _temp7221.curr= _temp7220; _temp7221.base=
_temp7220; _temp7221.last_plus_one= _temp7220 + 6; _temp7221;})); Cyc_String_strcat(
msg,({ struct _tagged_string* _temp7222=( struct _tagged_string*) Cyc_yytname;
unsigned int _temp7223= x; if( _temp7223 >= 227u){ _throw( Null_Exception);}
_temp7222[ _temp7223];})); Cyc_String_strcat( msg,( struct _tagged_string)({
char* _temp7224=( char*)"'"; struct _tagged_string _temp7225; _temp7225.curr=
_temp7224; _temp7225.base= _temp7224; _temp7225.last_plus_one= _temp7224 + 2;
_temp7225;})); count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror(( struct
_tagged_string)({ char* _temp7226=( char*)"parse error"; struct _tagged_string
_temp7227; _temp7227.curr= _temp7226; _temp7227.base= _temp7226; _temp7227.last_plus_one=
_temp7226 + 12; _temp7227;}));}} goto yyerrlab1; yyerrlab1: if( yyerrstatus == 3){
if( Cyc_yychar == 0){ return 1;} Cyc_yychar= - 2;} yyerrstatus= 3; goto
yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset == 0){ return 1;}
yyvsp_offset --; yystate=( int)({ struct _tagged_ptr1 _temp7228= yyss; short*
_temp7230= _temp7228.curr +( -- yyssp_offset); if( _temp7230 < _temp7228.base? 1:
_temp7230 >= _temp7228.last_plus_one){ _throw( Null_Exception);}* _temp7230;});
yylsp_offset --; yyerrhandle: yyn=( int)({ short* _temp7231=( short*) Cyc_yypact;
unsigned int _temp7232= yystate; if( _temp7232 >= 729u){ _throw( Null_Exception);}
_temp7231[ _temp7232];}); if( yyn == - 32768){ goto yyerrdefault;} yyn += 1; if((
yyn < 0? 1: yyn > 4017)? 1:( int)({ short* _temp7233=( short*) Cyc_yycheck;
unsigned int _temp7234= yyn; if( _temp7234 >= 4018u){ _throw( Null_Exception);}
_temp7233[ _temp7234];}) != 1){ goto yyerrdefault;} yyn=( int)({ short*
_temp7235=( short*) Cyc_yytable; unsigned int _temp7236= yyn; if( _temp7236 >=
4018u){ _throw( Null_Exception);} _temp7235[ _temp7236];}); if( yyn < 0){ if(
yyn == - 32768){ goto yyerrpop;} yyn= - yyn; goto yyreduce;} else{ if( yyn == 0){
goto yyerrpop;}} if( yyn == 728){ return 0;}({ struct _tagged_ptr2 _temp7237=
yyvs; struct _xtunion_struct** _temp7239= _temp7237.curr +( ++ yyvsp_offset);
if( _temp7239 < _temp7237.base? 1: _temp7239 >= _temp7237.last_plus_one){ _throw(
Null_Exception);}* _temp7239= Cyc_yylval;});({ struct _tagged_ptr3 _temp7240=
yyls; struct Cyc_Yyltype* _temp7242= _temp7240.curr +( ++ yylsp_offset); if(
_temp7242 < _temp7240.base? 1: _temp7242 >= _temp7240.last_plus_one){ _throw(
Null_Exception);}* _temp7242= Cyc_yylloc;}); goto yynewstate;} void Cyc_yyprint(
int i, struct _xtunion_struct* v){ struct _xtunion_struct* _temp7243= v; struct
_tuple12* _temp7263; struct _tuple12 _temp7265; int _temp7266; void* _temp7268;
char _temp7270; short _temp7272; struct _tagged_string _temp7274; struct Cyc_Core_Opt*
_temp7276; struct Cyc_Core_Opt* _temp7278; struct Cyc_Core_Opt _temp7280; struct
_tagged_string* _temp7281; struct _tuple1* _temp7283; struct _tuple1 _temp7285;
struct _tagged_string* _temp7286; void* _temp7288; _LL7245: if( _temp7243->tag
== Cyc_Okay_tok_tag){ goto _LL7246;} else{ goto _LL7247;} _LL7247: if((*((
struct _xtunion_struct*) _temp7243)).tag == Cyc_Int_tok_tag){ _LL7264: _temp7263=((
struct Cyc_Int_tok_struct*) _temp7243)->f1; _temp7265=* _temp7263; _LL7269:
_temp7268= _temp7265.f1; goto _LL7267; _LL7267: _temp7266= _temp7265.f2; goto
_LL7248;} else{ goto _LL7249;} _LL7249: if((*(( struct _xtunion_struct*)
_temp7243)).tag == Cyc_Char_tok_tag){ _LL7271: _temp7270=(( struct Cyc_Char_tok_struct*)
_temp7243)->f1; goto _LL7250;} else{ goto _LL7251;} _LL7251: if((*(( struct
_xtunion_struct*) _temp7243)).tag == Cyc_Short_tok_tag){ _LL7273: _temp7272=((
struct Cyc_Short_tok_struct*) _temp7243)->f1; goto _LL7252;} else{ goto _LL7253;}
_LL7253: if((*(( struct _xtunion_struct*) _temp7243)).tag == Cyc_String_tok_tag){
_LL7275: _temp7274=(( struct Cyc_String_tok_struct*) _temp7243)->f1; goto
_LL7254;} else{ goto _LL7255;} _LL7255: if((*(( struct _xtunion_struct*)
_temp7243)).tag == Cyc_Stringopt_tok_tag){ _LL7277: _temp7276=(( struct Cyc_Stringopt_tok_struct*)
_temp7243)->f1; if( _temp7276 == 0){ goto _LL7256;} else{ goto _LL7257;}} else{
goto _LL7257;} _LL7257: if((*(( struct _xtunion_struct*) _temp7243)).tag == Cyc_Stringopt_tok_tag){
_LL7279: _temp7278=(( struct Cyc_Stringopt_tok_struct*) _temp7243)->f1; if(
_temp7278 == 0){ goto _LL7259;} else{ _temp7280=* _temp7278; _LL7282: _temp7281=(
struct _tagged_string*) _temp7280.v; goto _LL7258;}} else{ goto _LL7259;}
_LL7259: if((*(( struct _xtunion_struct*) _temp7243)).tag == Cyc_QualId_tok_tag){
_LL7284: _temp7283=(( struct Cyc_QualId_tok_struct*) _temp7243)->f1; _temp7285=*
_temp7283; _LL7289: _temp7288= _temp7285.f1; goto _LL7287; _LL7287: _temp7286=
_temp7285.f2; goto _LL7260;} else{ goto _LL7261;} _LL7261: goto _LL7262; _LL7246:
fprintf( Cyc_Stdio_stderr,"ok"); goto _LL7244; _LL7248: fprintf( Cyc_Stdio_stderr,"%d",
_temp7266); goto _LL7244; _LL7250: fprintf( Cyc_Stdio_stderr,"%c",( int)
_temp7270); goto _LL7244; _LL7252: fprintf( Cyc_Stdio_stderr,"%ds",( int)
_temp7272); goto _LL7244; _LL7254:({ struct _tagged_string _temp7290= _temp7274;
fprintf( Cyc_Stdio_stderr,"\"%.*s\"", _temp7290.last_plus_one - _temp7290.curr,
_temp7290.curr);}); goto _LL7244; _LL7256: fprintf( Cyc_Stdio_stderr,"null");
goto _LL7244; _LL7258:({ struct _tagged_string _temp7291=* _temp7281; fprintf(
Cyc_Stdio_stderr,"\"%.*s\"", _temp7291.last_plus_one - _temp7291.curr, _temp7291.curr);});
goto _LL7244; _LL7260: { struct Cyc_List_List* prefix= 0;{ void* _temp7292=
_temp7288; struct Cyc_List_List* _temp7300; struct Cyc_List_List* _temp7302;
_LL7294: if(( unsigned int) _temp7292 > 1u?(( struct _tunion_struct*) _temp7292)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL7301: _temp7300=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp7292)->f1; goto _LL7295;} else{ goto _LL7296;}
_LL7296: if(( unsigned int) _temp7292 > 1u?(( struct _tunion_struct*) _temp7292)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL7303: _temp7302=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp7292)->f1; goto _LL7297;} else{ goto _LL7298;}
_LL7298: if(( int) _temp7292 == Cyc_Absyn_Loc_n){ goto _LL7299;} else{ goto
_LL7293;} _LL7295: prefix= _temp7300; goto _LL7293; _LL7297: prefix= _temp7302;
goto _LL7293; _LL7299: goto _LL7293; _LL7293:;} for( 0; prefix != 0; prefix=
prefix->tl){({ struct _tagged_string _temp7304=*(( struct _tagged_string*)
prefix->hd); fprintf( Cyc_Stdio_stderr,"%.*s::", _temp7304.last_plus_one -
_temp7304.curr, _temp7304.curr);});}({ struct _tagged_string _temp7305=*
_temp7286; fprintf( Cyc_Stdio_stderr,"%.*s::", _temp7305.last_plus_one -
_temp7305.curr, _temp7305.curr);}); goto _LL7244;} _LL7262: fprintf( Cyc_Stdio_stderr,"?");
goto _LL7244; _LL7244:;} struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE*
f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp7306=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp7306->v=(
void*) Cyc_Lexing_from_file( f); _temp7306;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}