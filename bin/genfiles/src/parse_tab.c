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
rgn; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc;
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
extern struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp( struct Cyc_Absyn_Exp* rgn_handle,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_true_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_char_exp( char c, struct Cyc_Position_Segment*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_string f, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp( struct _tagged_string s,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp(
void*, struct Cyc_List_List* es, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp(
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
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp43->v=( void*)(*(( struct _tuple1*)({
struct Cyc_Core_Opt* _temp44= _temp41; if( _temp44 == 0){ _throw( Null_Exception);}
_temp44->v;}))).f2; _temp43;});{ void* _temp46=(*(( struct _tuple1*)({ struct
Cyc_Core_Opt* _temp45= _temp41; if( _temp45 == 0){ _throw( Null_Exception);}
_temp45->v;}))).f1; struct Cyc_List_List* _temp56; struct Cyc_List_List* _temp58;
_LL48: if(( unsigned int) _temp46 > 1u?(( struct _tunion_struct*) _temp46)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL57: _temp56=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp46)->f1; if( _temp56 == 0){ goto _LL49;} else{ goto _LL50;}} else{ goto
_LL50;} _LL50: if(( unsigned int) _temp46 > 1u?(( struct _tunion_struct*)
_temp46)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL59: _temp58=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp46)->f1; if( _temp58 == 0){ goto _LL51;}
else{ goto _LL52;}} else{ goto _LL52;} _LL52: if(( int) _temp46 == Cyc_Absyn_Loc_n){
goto _LL53;} else{ goto _LL54;} _LL54: goto _LL55; _LL49: goto _LL47; _LL51:
goto _LL47; _LL53: goto _LL47; _LL55: Cyc_Parse_err(( struct _tagged_string)({
char* _temp60=( char*)"parameter cannot be qualified with a module name"; struct
_tagged_string _temp61; _temp61.curr= _temp60; _temp61.base= _temp60; _temp61.last_plus_one=
_temp60 + 49; _temp61;}), loc); goto _LL47; _LL47:;}} if( _temp35 != 0){(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp62=( char*)"parameter should have no type parameters";
struct _tagged_string _temp63; _temp63.curr= _temp62; _temp63.base= _temp62;
_temp63.last_plus_one= _temp62 + 41; _temp63;}), loc);} return({ struct _tuple2*
_temp64=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp64->f1=
idopt; _temp64->f2= _temp39; _temp64->f3= _temp37; _temp64;});}} static void*
Cyc_Parse_type_spec( void* t, struct Cyc_Position_Segment* loc){ return( void*)({
struct Cyc_Parse_Type_spec_struct* _temp65=( struct Cyc_Parse_Type_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct)); _temp65[ 0]=({ struct
Cyc_Parse_Type_spec_struct _temp66; _temp66.tag= Cyc_Parse_Type_spec_tag;
_temp66.f1=( void*) t; _temp66.f2= loc; _temp66;}); _temp65;});} static void*
Cyc_Parse_array2ptr( void* t){ void* _temp67= t; struct Cyc_Absyn_Exp* _temp73;
struct Cyc_Absyn_Tqual* _temp75; void* _temp77; _LL69: if(( unsigned int)
_temp67 > 4u?(( struct _tunion_struct*) _temp67)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL78: _temp77=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp67)->f1;
goto _LL76; _LL76: _temp75=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp67)->f2; goto _LL74; _LL74: _temp73=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp67)->f3; goto _LL70;} else{ goto _LL71;} _LL71: goto _LL72; _LL70: return
Cyc_Absyn_starb_typ( _temp77,( void*) Cyc_Absyn_HeapRgn, _temp75, _temp73 == 0?(
void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp79=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp79[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp80; _temp80.tag= Cyc_Absyn_Upper_b_tag;
_temp80.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp81= _temp73;
if( _temp81 == 0){ _throw( Null_Exception);} _temp81;}); _temp80;}); _temp79;}));
_LL72: return t; _LL68:;} static void Cyc_Parse_arg_array2ptr( struct _tuple2* x){(*
x).f3= Cyc_Parse_array2ptr((* x).f3);} static struct _tuple10* Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment* loc, struct _tuple2* t){ return({ struct _tuple10*
_temp82=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp82->f1=(*
t).f2; _temp82->f2=(* t).f3; _temp82;});} static void Cyc_Parse_only_vardecl(
struct Cyc_List_List* params, struct Cyc_Absyn_Decl* x){ struct _tagged_string
decl_kind;{ void* _temp83=( void*) x->r; struct Cyc_Absyn_Vardecl* _temp109; int
_temp111; struct Cyc_Absyn_Exp* _temp113; struct Cyc_Core_Opt* _temp115; struct
Cyc_Core_Opt* _temp117; struct Cyc_Absyn_Pat* _temp119; struct Cyc_Absyn_Fndecl*
_temp121; struct Cyc_Absyn_Structdecl* _temp123; struct Cyc_Absyn_Uniondecl*
_temp125; struct Cyc_Absyn_Tuniondecl* _temp127; struct Cyc_Absyn_Typedefdecl*
_temp129; struct Cyc_Absyn_XTuniondecl* _temp131; struct Cyc_Absyn_Enumdecl*
_temp133; struct Cyc_List_List* _temp135; struct _tagged_string* _temp137;
struct Cyc_List_List* _temp139; struct _tuple1* _temp141; struct Cyc_List_List*
_temp143; _LL85: if((( struct _tunion_struct*) _temp83)->tag == Cyc_Absyn_Var_d_tag){
_LL110: _temp109=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp83)->f1; goto _LL86;} else{ goto _LL87;} _LL87: if((( struct _tunion_struct*)
_temp83)->tag == Cyc_Absyn_Let_d_tag){ _LL120: _temp119=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp83)->f1; goto _LL118; _LL118: _temp117=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp83)->f2; goto
_LL116; _LL116: _temp115=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp83)->f3; goto _LL114; _LL114: _temp113=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Let_d_struct*) _temp83)->f4; goto _LL112; _LL112: _temp111=( int)((
struct Cyc_Absyn_Let_d_struct*) _temp83)->f5; goto _LL88;} else{ goto _LL89;}
_LL89: if((( struct _tunion_struct*) _temp83)->tag == Cyc_Absyn_Fn_d_tag){
_LL122: _temp121=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp83)->f1; goto _LL90;} else{ goto _LL91;} _LL91: if((( struct _tunion_struct*)
_temp83)->tag == Cyc_Absyn_Struct_d_tag){ _LL124: _temp123=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp83)->f1; goto _LL92;} else{ goto _LL93;}
_LL93: if((( struct _tunion_struct*) _temp83)->tag == Cyc_Absyn_Union_d_tag){
_LL126: _temp125=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp83)->f1; goto _LL94;} else{ goto _LL95;} _LL95: if((( struct _tunion_struct*)
_temp83)->tag == Cyc_Absyn_Tunion_d_tag){ _LL128: _temp127=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp83)->f1; goto _LL96;} else{ goto _LL97;}
_LL97: if((( struct _tunion_struct*) _temp83)->tag == Cyc_Absyn_Typedef_d_tag){
_LL130: _temp129=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp83)->f1; goto _LL98;} else{ goto _LL99;} _LL99: if((( struct _tunion_struct*)
_temp83)->tag == Cyc_Absyn_XTunion_d_tag){ _LL132: _temp131=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_d_struct*) _temp83)->f1; goto _LL100;} else{ goto
_LL101;} _LL101: if((( struct _tunion_struct*) _temp83)->tag == Cyc_Absyn_Enum_d_tag){
_LL134: _temp133=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp83)->f1; goto _LL102;} else{ goto _LL103;} _LL103: if((( struct
_tunion_struct*) _temp83)->tag == Cyc_Absyn_Namespace_d_tag){ _LL138: _temp137=(
struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp83)->f1;
goto _LL136; _LL136: _temp135=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp83)->f2; goto _LL104;} else{ goto _LL105;} _LL105: if((( struct
_tunion_struct*) _temp83)->tag == Cyc_Absyn_Using_d_tag){ _LL142: _temp141=(
struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*) _temp83)->f1; goto _LL140;
_LL140: _temp139=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp83)->f2; goto _LL106;} else{ goto _LL107;} _LL107: if((( struct
_tunion_struct*) _temp83)->tag == Cyc_Absyn_ExternC_d_tag){ _LL144: _temp143=(
struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*) _temp83)->f1; goto
_LL108;} else{ goto _LL84;} _LL86: if( _temp109->initializer != 0){(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp145=( char*)"initializers are not allowed in parameter declarations";
struct _tagged_string _temp146; _temp146.curr= _temp145; _temp146.base= _temp145;
_temp146.last_plus_one= _temp145 + 55; _temp146;}), x->loc);}{ void* _temp147=(*
_temp109->name).f1; struct Cyc_List_List* _temp157; struct Cyc_List_List*
_temp159; _LL149: if(( int) _temp147 == Cyc_Absyn_Loc_n){ goto _LL150;} else{
goto _LL151;} _LL151: if(( unsigned int) _temp147 > 1u?(( struct _tunion_struct*)
_temp147)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL158: _temp157=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp147)->f1; if( _temp157 == 0){ goto _LL152;}
else{ goto _LL153;}} else{ goto _LL153;} _LL153: if(( unsigned int) _temp147 > 1u?((
struct _tunion_struct*) _temp147)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL160:
_temp159=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp147)->f1;
if( _temp159 == 0){ goto _LL154;} else{ goto _LL155;}} else{ goto _LL155;}
_LL155: goto _LL156; _LL150: goto _LL148; _LL152: goto _LL148; _LL154: goto
_LL148; _LL156: Cyc_Parse_err(( struct _tagged_string)({ char* _temp161=( char*)"module names not allowed on parameter declarations";
struct _tagged_string _temp162; _temp162.curr= _temp161; _temp162.base= _temp161;
_temp162.last_plus_one= _temp161 + 51; _temp162;}), x->loc); goto _LL148; _LL148:;}{
int found= 0; for( 0; params != 0; params=({ struct Cyc_List_List* _temp163=
params; if( _temp163 == 0){ _throw( Null_Exception);} _temp163->tl;})){ if( Cyc_String_zstrptrcmp((*
_temp109->name).f2,( struct _tagged_string*)({ struct Cyc_List_List* _temp164=
params; if( _temp164 == 0){ _throw( Null_Exception);} _temp164->hd;})) == 0){
found= 1; break;}} if( ! found){(( void(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp165=*(*
_temp109->name).f2; xprintf("%.*s is not listed as a parameter", _temp165.last_plus_one
- _temp165.curr, _temp165.curr);}), x->loc);} return;} _LL88: decl_kind=( struct
_tagged_string)({ char* _temp166=( char*)"let declaration"; struct
_tagged_string _temp167; _temp167.curr= _temp166; _temp167.base= _temp166;
_temp167.last_plus_one= _temp166 + 16; _temp167;}); goto _LL84; _LL90: decl_kind=(
struct _tagged_string)({ char* _temp168=( char*)"function declaration"; struct
_tagged_string _temp169; _temp169.curr= _temp168; _temp169.base= _temp168;
_temp169.last_plus_one= _temp168 + 21; _temp169;}); goto _LL84; _LL92: decl_kind=(
struct _tagged_string)({ char* _temp170=( char*)"struct declaration"; struct
_tagged_string _temp171; _temp171.curr= _temp170; _temp171.base= _temp170;
_temp171.last_plus_one= _temp170 + 19; _temp171;}); goto _LL84; _LL94: decl_kind=(
struct _tagged_string)({ char* _temp172=( char*)"union declaration"; struct
_tagged_string _temp173; _temp173.curr= _temp172; _temp173.base= _temp172;
_temp173.last_plus_one= _temp172 + 18; _temp173;}); goto _LL84; _LL96: decl_kind=(
struct _tagged_string)({ char* _temp174=( char*)"tunion declaration"; struct
_tagged_string _temp175; _temp175.curr= _temp174; _temp175.base= _temp174;
_temp175.last_plus_one= _temp174 + 19; _temp175;}); goto _LL84; _LL98: decl_kind=(
struct _tagged_string)({ char* _temp176=( char*)"typedef"; struct _tagged_string
_temp177; _temp177.curr= _temp176; _temp177.base= _temp176; _temp177.last_plus_one=
_temp176 + 8; _temp177;}); goto _LL84; _LL100: decl_kind=( struct _tagged_string)({
char* _temp178=( char*)"xtunion declaration"; struct _tagged_string _temp179;
_temp179.curr= _temp178; _temp179.base= _temp178; _temp179.last_plus_one=
_temp178 + 20; _temp179;}); goto _LL84; _LL102: decl_kind=( struct
_tagged_string)({ char* _temp180=( char*)"enum declaration"; struct
_tagged_string _temp181; _temp181.curr= _temp180; _temp181.base= _temp180;
_temp181.last_plus_one= _temp180 + 17; _temp181;}); goto _LL84; _LL104:
decl_kind=( struct _tagged_string)({ char* _temp182=( char*)"namespace declaration";
struct _tagged_string _temp183; _temp183.curr= _temp182; _temp183.base= _temp182;
_temp183.last_plus_one= _temp182 + 22; _temp183;}); goto _LL84; _LL106:
decl_kind=( struct _tagged_string)({ char* _temp184=( char*)"using declaration";
struct _tagged_string _temp185; _temp185.curr= _temp184; _temp185.base= _temp184;
_temp185.last_plus_one= _temp184 + 18; _temp185;}); goto _LL84; _LL108:
decl_kind=( struct _tagged_string)({ char* _temp186=( char*)"extern C declaration";
struct _tagged_string _temp187; _temp187.curr= _temp186; _temp187.base= _temp186;
_temp187.last_plus_one= _temp186 + 21; _temp187;}); goto _LL84; _LL84:;}(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp188= decl_kind; xprintf("%.*s appears in parameter type",
_temp188.last_plus_one - _temp188.curr, _temp188.curr);}), x->loc); return;}
static struct _tuple2* Cyc_Parse_get_param_type( struct _tuple11* env, struct
_tagged_string* x){ struct _tuple11 _temp191; struct Cyc_Position_Segment*
_temp192; struct Cyc_List_List* _temp194; struct _tuple11* _temp189= env;
_temp191=* _temp189; _LL195: _temp194= _temp191.f1; goto _LL193; _LL193:
_temp192= _temp191.f2; goto _LL190; _LL190: if( _temp194 == 0){ return(( struct
_tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp196=* x; xprintf("missing type for parameter %.*s",
_temp196.last_plus_one - _temp196.curr, _temp196.curr);}), _temp192);}{ void*
_temp198=( void*)(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List* _temp197=
_temp194; if( _temp197 == 0){ _throw( Null_Exception);} _temp197->hd;}))->r;
struct Cyc_Absyn_Vardecl* _temp204; _LL200: if((( struct _tunion_struct*)
_temp198)->tag == Cyc_Absyn_Var_d_tag){ _LL205: _temp204=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp198)->f1; goto _LL201;} else{ goto _LL202;}
_LL202: goto _LL203; _LL201:{ void* _temp206=(* _temp204->name).f1; struct Cyc_List_List*
_temp216; struct Cyc_List_List* _temp218; _LL208: if(( int) _temp206 == Cyc_Absyn_Loc_n){
goto _LL209;} else{ goto _LL210;} _LL210: if(( unsigned int) _temp206 > 1u?((
struct _tunion_struct*) _temp206)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL217:
_temp216=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp206)->f1;
if( _temp216 == 0){ goto _LL211;} else{ goto _LL212;}} else{ goto _LL212;}
_LL212: if(( unsigned int) _temp206 > 1u?(( struct _tunion_struct*) _temp206)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL219: _temp218=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp206)->f1; if( _temp218 == 0){ goto _LL213;} else{
goto _LL214;}} else{ goto _LL214;} _LL214: goto _LL215; _LL209: goto _LL207;
_LL211: goto _LL207; _LL213: goto _LL207; _LL215: Cyc_Parse_err(( struct
_tagged_string)({ char* _temp220=( char*)"module name not allowed on parameter";
struct _tagged_string _temp221; _temp221.curr= _temp220; _temp221.base= _temp220;
_temp221.last_plus_one= _temp220 + 37; _temp221;}), _temp192); goto _LL207;
_LL207:;} if( Cyc_String_zstrptrcmp((* _temp204->name).f2, x) == 0){ return({
struct _tuple2* _temp222=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2) *
1); _temp222[ 0]=({ struct _tuple2 _temp223; _temp223.f1=({ struct Cyc_Core_Opt*
_temp224=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp224[ 0]=({ struct Cyc_Core_Opt _temp225; _temp225.v=( void*)(* _temp204->name).f2;
_temp225;}); _temp224;}); _temp223.f2= _temp204->tq; _temp223.f3=( void*)
_temp204->type; _temp223;}); _temp222;});} else{ return Cyc_Parse_get_param_type(({
struct _tuple11* _temp226=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp226->f1=({ struct Cyc_List_List* _temp227= _temp194; if( _temp227 == 0){
_throw( Null_Exception);} _temp227->tl;}); _temp226->f2= _temp192; _temp226;}),
x);} _LL203: return(( struct _tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp228=( char*)"non-variable declaration";
struct _tagged_string _temp229; _temp229.curr= _temp228; _temp229.base= _temp228;
_temp229.last_plus_one= _temp228 + 25; _temp229;}),(( struct Cyc_Absyn_Decl*)({
struct Cyc_List_List* _temp230= _temp194; if( _temp230 == 0){ _throw(
Null_Exception);} _temp230->hd;}))->loc); _LL199:;}} static int Cyc_Parse_is_typeparam(
void* tm){ void* _temp231= tm; int _temp237; struct Cyc_Position_Segment*
_temp239; struct Cyc_List_List* _temp241; _LL233: if(( unsigned int) _temp231 >
1u?(( struct _tunion_struct*) _temp231)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){
_LL242: _temp241=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp231)->f1; goto _LL240; _LL240: _temp239=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp231)->f2; goto _LL238; _LL238:
_temp237=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp231)->f3; goto
_LL234;} else{ goto _LL235;} _LL235: goto _LL236; _LL234: return 1; _LL236:
return 0; _LL232:;} static void* Cyc_Parse_id2type( struct _tagged_string s,
struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({
char* _temp243=( char*)"`H"; struct _tagged_string _temp244; _temp244.curr=
_temp243; _temp244.base= _temp243; _temp244.last_plus_one= _temp243 + 3;
_temp244;})) == 0){ return( void*) Cyc_Absyn_HeapRgn;} else{ return( void*)({
struct Cyc_Absyn_VarType_struct* _temp245=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp245[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp246; _temp246.tag= Cyc_Absyn_VarType_tag; _temp246.f1=({ struct Cyc_Absyn_Tvar*
_temp247=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp247->name=({ struct _tagged_string* _temp248=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp248[ 0]= s; _temp248;});
_temp247->kind= k; _temp247;}); _temp246;}); _temp245;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp249=
t; struct Cyc_Absyn_Tvar* _temp255; _LL251: if(( unsigned int) _temp249 > 4u?((
struct _tunion_struct*) _temp249)->tag == Cyc_Absyn_VarType_tag: 0){ _LL256:
_temp255=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp249)->f1;
goto _LL252;} else{ goto _LL253;} _LL253: goto _LL254; _LL252: return _temp255;
_LL254: return(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char*
_temp257=( char*)"expecting a list of type variables, not types"; struct
_tagged_string _temp258; _temp258.curr= _temp257; _temp258.base= _temp257;
_temp258.last_plus_one= _temp257 + 46; _temp258;}), loc); _LL250:;} static void*
Cyc_Parse_tvar2typ( struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp259=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp259[ 0]=({ struct Cyc_Absyn_VarType_struct _temp260; _temp260.tag= Cyc_Absyn_VarType_tag;
_temp260.f1= pr; _temp260;}); _temp259;});} static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List* tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment*
loc){ if( tds == 0){ return tms;} if( tms == 0){ return 0;}{ void* _temp262=(
void*)({ struct Cyc_List_List* _temp261= tms; if( _temp261 == 0){ _throw(
Null_Exception);} _temp261->hd;}); void* _temp268; _LL264: if(( unsigned int)
_temp262 > 1u?(( struct _tunion_struct*) _temp262)->tag == Cyc_Absyn_Function_mod_tag:
0){ _LL269: _temp268=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp262)->f1;
goto _LL265;} else{ goto _LL266;} _LL266: goto _LL267; _LL265: if(({ struct Cyc_List_List*
_temp270= tms; if( _temp270 == 0){ _throw( Null_Exception);} _temp270->tl;}) ==
0? 1:( Cyc_Parse_is_typeparam(( void*)({ struct Cyc_List_List* _temp272=({
struct Cyc_List_List* _temp271= tms; if( _temp271 == 0){ _throw( Null_Exception);}
_temp271->tl;}); if( _temp272 == 0){ _throw( Null_Exception);} _temp272->hd;}))?({
struct Cyc_List_List* _temp274=({ struct Cyc_List_List* _temp273= tms; if(
_temp273 == 0){ _throw( Null_Exception);} _temp273->tl;}); if( _temp274 == 0){
_throw( Null_Exception);} _temp274->tl;}) == 0: 0)){ void* _temp275= _temp268;
struct Cyc_Core_Opt* _temp281; int _temp283; struct Cyc_List_List* _temp285;
struct Cyc_Position_Segment* _temp287; struct Cyc_List_List* _temp289; _LL277:
if((( struct _tunion_struct*) _temp275)->tag == Cyc_Absyn_WithTypes_tag){ _LL286:
_temp285=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*) _temp275)->f1;
goto _LL284; _LL284: _temp283=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp275)->f2; goto _LL282; _LL282: _temp281=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp275)->f3; goto _LL278;} else{ goto _LL279;}
_LL279: if((( struct _tunion_struct*) _temp275)->tag == Cyc_Absyn_NoTypes_tag){
_LL290: _temp289=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp275)->f1; goto _LL288; _LL288: _temp287=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_NoTypes_struct*) _temp275)->f2; goto _LL280;} else{ goto _LL276;}
_LL278: Cyc_Parse_warn(( struct _tagged_string)({ char* _temp291=( char*)"function declaration with both new- and old-style parameterdeclarations; ignoring old-style";
struct _tagged_string _temp292; _temp292.curr= _temp291; _temp292.base= _temp291;
_temp292.last_plus_one= _temp291 + 92; _temp292;}), loc); return tms; _LL280:((
void(*)( void(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl,
_temp289, tds);{ struct _tuple11* env=({ struct _tuple11* _temp293=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp293->f1= tds; _temp293->f2=
loc; _temp293;}); return({ struct Cyc_List_List* _temp294=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp294->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp295=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp295[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp296; _temp296.tag= Cyc_Absyn_Function_mod_tag;
_temp296.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp297=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp297[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp298; _temp298.tag= Cyc_Absyn_WithTypes_tag;
_temp298.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple11*,
struct _tagged_string*), struct _tuple11* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, env, _temp289); _temp298.f2= 0; _temp298.f3= 0;
_temp298;}); _temp297;})); _temp296;}); _temp295;})); _temp294->tl= 0; _temp294;});}
_LL276:;} else{ return({ struct Cyc_List_List* _temp299=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp299->hd=( void*)(( void*)({
struct Cyc_List_List* _temp301= tms; if( _temp301 == 0){ _throw( Null_Exception);}
_temp301->hd;})); _temp299->tl= Cyc_Parse_oldstyle2newstyle(({ struct Cyc_List_List*
_temp300= tms; if( _temp300 == 0){ _throw( Null_Exception);} _temp300->tl;}),
tds, loc); _temp299;});} _LL267: return({ struct Cyc_List_List* _temp302=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp302->hd=(
void*)(( void*)({ struct Cyc_List_List* _temp304= tms; if( _temp304 == 0){
_throw( Null_Exception);} _temp304->hd;})); _temp302->tl= Cyc_Parse_oldstyle2newstyle(({
struct Cyc_List_List* _temp303= tms; if( _temp303 == 0){ _throw( Null_Exception);}
_temp303->tl;}), tds, loc); _temp302;}); _LL263:;}} static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function( struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator*
d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment*
loc){ if( tds != 0){ d=({ struct Cyc_Parse_Declarator* _temp305=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp305->id= d->id; _temp305->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp305;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual* tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct
Cyc_Parse_Declaration_spec*)({ struct Cyc_Core_Opt* _temp306= dso; if( _temp306
== 0){ _throw( Null_Exception);} _temp306->v;}))->type_specs; tq=(( struct Cyc_Parse_Declaration_spec*)({
struct Cyc_Core_Opt* _temp307= dso; if( _temp307 == 0){ _throw( Null_Exception);}
_temp307->v;}))->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*)({ struct
Cyc_Core_Opt* _temp308= dso; if( _temp308 == 0){ _throw( Null_Exception);}
_temp308->v;}))->is_inline; atts=(( struct Cyc_Parse_Declaration_spec*)({ struct
Cyc_Core_Opt* _temp309= dso; if( _temp309 == 0){ _throw( Null_Exception);}
_temp309->v;}))->attributes; if((( struct Cyc_Parse_Declaration_spec*)({ struct
Cyc_Core_Opt* _temp310= dso; if( _temp310 == 0){ _throw( Null_Exception);}
_temp310->v;}))->sc != 0){ void* _temp313=( void*)({ struct Cyc_Core_Opt*
_temp312=(( struct Cyc_Parse_Declaration_spec*)({ struct Cyc_Core_Opt* _temp311=
dso; if( _temp311 == 0){ _throw( Null_Exception);} _temp311->v;}))->sc; if(
_temp312 == 0){ _throw( Null_Exception);} _temp312->v;}); _LL315: if(( int)
_temp313 == Cyc_Parse_Extern_sc){ goto _LL316;} else{ goto _LL317;} _LL317: if((
int) _temp313 == Cyc_Parse_ExternC_sc){ goto _LL318;} else{ goto _LL319;} _LL319:
if(( int) _temp313 == Cyc_Parse_Static_sc){ goto _LL320;} else{ goto _LL321;}
_LL321: goto _LL322; _LL316: sc=( void*) Cyc_Absyn_Extern; goto _LL314; _LL318:
sc=( void*) Cyc_Absyn_ExternC; goto _LL314; _LL320: sc=( void*) Cyc_Absyn_Static;
goto _LL314; _LL322: Cyc_Parse_err(( struct _tagged_string)({ char* _temp323=(
char*)"bad storage class on function"; struct _tagged_string _temp324; _temp324.curr=
_temp323; _temp324.base= _temp323; _temp324.last_plus_one= _temp323 + 30;
_temp324;}), loc); goto _LL314; _LL314:;}}{ struct Cyc_Core_Opt* _temp327; void*
_temp329; struct _tuple5 _temp325= Cyc_Parse_collapse_type_specifiers( tss, loc);
_LL330: _temp329= _temp325.f1; goto _LL328; _LL328: _temp327= _temp325.f2; goto
_LL326; _LL326: { struct Cyc_List_List* _temp333; struct Cyc_List_List* _temp335;
void* _temp337; struct Cyc_Absyn_Tqual* _temp339; struct _tuple6 _temp331= Cyc_Parse_apply_tms(
tq, _temp329, atts, d->tms); _LL340: _temp339= _temp331.f1; goto _LL338; _LL338:
_temp337= _temp331.f2; goto _LL336; _LL336: _temp335= _temp331.f3; goto _LL334;
_LL334: _temp333= _temp331.f4; goto _LL332; _LL332: if( _temp327 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp341=( char*)"nested type declaration within function prototype";
struct _tagged_string _temp342; _temp342.curr= _temp341; _temp342.base= _temp341;
_temp342.last_plus_one= _temp341 + 50; _temp342;}), loc);} if( _temp335 != 0){
Cyc_Parse_warn(( struct _tagged_string)({ char* _temp343=( char*)"bad type params, ignoring";
struct _tagged_string _temp344; _temp344.curr= _temp343; _temp344.base= _temp343;
_temp344.last_plus_one= _temp343 + 26; _temp344;}), loc);}{ void* _temp345=
_temp337; struct Cyc_Absyn_FnInfo _temp351; struct Cyc_List_List* _temp353; int
_temp355; struct Cyc_List_List* _temp357; void* _temp359; struct Cyc_Core_Opt*
_temp361; struct Cyc_List_List* _temp363; _LL347: if(( unsigned int) _temp345 >
4u?(( struct _tunion_struct*) _temp345)->tag == Cyc_Absyn_FnType_tag: 0){ _LL352:
_temp351=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp345)->f1;
_LL364: _temp363=( struct Cyc_List_List*) _temp351.tvars; goto _LL362; _LL362:
_temp361=( struct Cyc_Core_Opt*) _temp351.effect; goto _LL360; _LL360: _temp359=(
void*) _temp351.ret_typ; goto _LL358; _LL358: _temp357=( struct Cyc_List_List*)
_temp351.args; goto _LL356; _LL356: _temp355=( int) _temp351.varargs; goto
_LL354; _LL354: _temp353=( struct Cyc_List_List*) _temp351.attributes; goto
_LL348;} else{ goto _LL349;} _LL349: goto _LL350; _LL348: { struct Cyc_List_List*
args2=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp357); return({ struct Cyc_Absyn_Fndecl*
_temp365=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp365->sc=( void*) sc; _temp365->name= d->id; _temp365->tvs= _temp363;
_temp365->is_inline= is_inline; _temp365->effect= _temp361; _temp365->ret_type=(
void*) _temp359; _temp365->args= args2; _temp365->varargs= _temp355; _temp365->body=
body; _temp365->cached_typ= 0; _temp365->param_vardecls= 0; _temp365->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp353, _temp333); _temp365;});} _LL350: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp366=( char*)"declarator is not a function prototype";
struct _tagged_string _temp367; _temp367.curr= _temp366; _temp367.base= _temp366;
_temp367.last_plus_one= _temp366 + 39; _temp367;}), loc); _LL346:;}}}}} static
struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct Cyc_Position_Segment* loc,
struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp368=( char*)"missing argument variable in function prototype"; struct
_tagged_string _temp369; _temp369.curr= _temp368; _temp369.base= _temp368;
_temp369.last_plus_one= _temp368 + 48; _temp369;}), loc); return({ struct
_tuple4* _temp370=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp370->f1=({ struct _tagged_string* _temp371=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp371[ 0]=( struct
_tagged_string)({ char* _temp372=( char*)"?"; struct _tagged_string _temp373;
_temp373.curr= _temp372; _temp373.base= _temp372; _temp373.last_plus_one=
_temp372 + 2; _temp373;}); _temp371;}); _temp370->f2=(* t).f2; _temp370->f3=(* t).f3;
_temp370;});} else{ return({ struct _tuple4* _temp374=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp374->f1=( struct _tagged_string*)({
struct Cyc_Core_Opt* _temp375=(* t).f1; if( _temp375 == 0){ _throw(
Null_Exception);} _temp375->v;}); _temp374->f2=(* t).f2; _temp374->f3=(* t).f3;
_temp374;});}} static char _temp378[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_string Cyc_Parse_msg1=( struct _tagged_string){ _temp378,
_temp378, _temp378 + 52u}; static char _temp381[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg2=( struct _tagged_string){ _temp381,
_temp381, _temp381 + 63u}; static char _temp384[ 50u]="type specifier includes more than one declaration";
static struct _tagged_string Cyc_Parse_msg3=( struct _tagged_string){ _temp384,
_temp384, _temp384 + 50u}; static char _temp387[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg4=( struct _tagged_string){ _temp387,
_temp387, _temp387 + 60u}; static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
declopt= 0; int seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*)
Cyc_Absyn_VoidType; void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed;
struct Cyc_Position_Segment* last_loc= loc; for( 0; ts != 0; ts=({ struct Cyc_List_List*
_temp388= ts; if( _temp388 == 0){ _throw( Null_Exception);} _temp388->tl;})){
void* _temp390=( void*)({ struct Cyc_List_List* _temp389= ts; if( _temp389 == 0){
_throw( Null_Exception);} _temp389->hd;}); struct Cyc_Position_Segment* _temp404;
void* _temp406; struct Cyc_Position_Segment* _temp408; struct Cyc_Position_Segment*
_temp410; struct Cyc_Position_Segment* _temp412; struct Cyc_Position_Segment*
_temp414; struct Cyc_Absyn_Decl* _temp416; _LL392: if((( struct _tunion_struct*)
_temp390)->tag == Cyc_Parse_Type_spec_tag){ _LL407: _temp406=( void*)(( struct
Cyc_Parse_Type_spec_struct*) _temp390)->f1; goto _LL405; _LL405: _temp404=(
struct Cyc_Position_Segment*)(( struct Cyc_Parse_Type_spec_struct*) _temp390)->f2;
goto _LL393;} else{ goto _LL394;} _LL394: if((( struct _tunion_struct*) _temp390)->tag
== Cyc_Parse_Signed_spec_tag){ _LL409: _temp408=( struct Cyc_Position_Segment*)((
struct Cyc_Parse_Signed_spec_struct*) _temp390)->f1; goto _LL395;} else{ goto
_LL396;} _LL396: if((( struct _tunion_struct*) _temp390)->tag == Cyc_Parse_Unsigned_spec_tag){
_LL411: _temp410=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Unsigned_spec_struct*)
_temp390)->f1; goto _LL397;} else{ goto _LL398;} _LL398: if((( struct
_tunion_struct*) _temp390)->tag == Cyc_Parse_Short_spec_tag){ _LL413: _temp412=(
struct Cyc_Position_Segment*)(( struct Cyc_Parse_Short_spec_struct*) _temp390)->f1;
goto _LL399;} else{ goto _LL400;} _LL400: if((( struct _tunion_struct*) _temp390)->tag
== Cyc_Parse_Long_spec_tag){ _LL415: _temp414=( struct Cyc_Position_Segment*)((
struct Cyc_Parse_Long_spec_struct*) _temp390)->f1; goto _LL401;} else{ goto
_LL402;} _LL402: if((( struct _tunion_struct*) _temp390)->tag == Cyc_Parse_Decl_spec_tag){
_LL417: _temp416=( struct Cyc_Absyn_Decl*)(( struct Cyc_Parse_Decl_spec_struct*)
_temp390)->f1; goto _LL403;} else{ goto _LL391;} _LL393: if( seen_type){ Cyc_Parse_err(
Cyc_Parse_msg1, _temp404);} last_loc= _temp404; seen_type= 1; t= _temp406; goto
_LL391; _LL395: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp408);} if(
seen_type){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp418=( char*)"signed qualifier must come before type";
struct _tagged_string _temp419; _temp419.curr= _temp418; _temp419.base= _temp418;
_temp419.last_plus_one= _temp418 + 39; _temp419;}), _temp408);} last_loc=
_temp408; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed; goto _LL391; _LL397: if(
seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp410);} if( seen_type){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp420=( char*)"signed qualifier must come before type";
struct _tagged_string _temp421; _temp421.curr= _temp420; _temp421.base= _temp420;
_temp421.last_plus_one= _temp420 + 39; _temp421;}), _temp410);} last_loc=
_temp410; seen_sign= 1; sgn=( void*) Cyc_Absyn_Unsigned; goto _LL391; _LL399:
if( seen_size){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp422=( char*)"integral size may appear only once within a type specifier";
struct _tagged_string _temp423; _temp423.curr= _temp422; _temp423.base= _temp422;
_temp423.last_plus_one= _temp422 + 59; _temp423;}), _temp412);} if( seen_type){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp424=( char*)"short modifier must come before base type";
struct _tagged_string _temp425; _temp425.curr= _temp424; _temp425.base= _temp424;
_temp425.last_plus_one= _temp424 + 42; _temp425;}), _temp412);} last_loc=
_temp412; seen_size= 1; sz=( void*) Cyc_Absyn_B2; goto _LL391; _LL401: if(
seen_type){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp426=( char*)"long modifier must come before base type";
struct _tagged_string _temp427; _temp427.curr= _temp426; _temp427.base= _temp426;
_temp427.last_plus_one= _temp426 + 41; _temp427;}), _temp414);} if( seen_size){
void* _temp428= sz; _LL430: if(( int) _temp428 == Cyc_Absyn_B4){ goto _LL431;}
else{ goto _LL432;} _LL432: goto _LL433; _LL431: sz=( void*) Cyc_Absyn_B8; goto
_LL429; _LL433: Cyc_Parse_err(( struct _tagged_string)({ char* _temp434=( char*)"extra long in type specifier";
struct _tagged_string _temp435; _temp435.curr= _temp434; _temp435.base= _temp434;
_temp435.last_plus_one= _temp434 + 29; _temp435;}), _temp414); goto _LL429;
_LL429:;} last_loc= _temp414; seen_size= 1; goto _LL391; _LL403: last_loc=
_temp416->loc; if( declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp436=(
void*) _temp416->r; struct Cyc_Absyn_Structdecl* _temp450; struct Cyc_Absyn_Tuniondecl*
_temp452; struct Cyc_Absyn_XTuniondecl* _temp454; struct Cyc_Absyn_Uniondecl*
_temp456; struct Cyc_Absyn_Enumdecl* _temp458; _LL438: if((( struct
_tunion_struct*) _temp436)->tag == Cyc_Absyn_Struct_d_tag){ _LL451: _temp450=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp436)->f1;
goto _LL439;} else{ goto _LL440;} _LL440: if((( struct _tunion_struct*) _temp436)->tag
== Cyc_Absyn_Tunion_d_tag){ _LL453: _temp452=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp436)->f1; goto _LL441;} else{ goto
_LL442;} _LL442: if((( struct _tunion_struct*) _temp436)->tag == Cyc_Absyn_XTunion_d_tag){
_LL455: _temp454=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_d_struct*)
_temp436)->f1; goto _LL443;} else{ goto _LL444;} _LL444: if((( struct
_tunion_struct*) _temp436)->tag == Cyc_Absyn_Union_d_tag){ _LL457: _temp456=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp436)->f1;
goto _LL445;} else{ goto _LL446;} _LL446: if((( struct _tunion_struct*) _temp436)->tag
== Cyc_Absyn_Enum_d_tag){ _LL459: _temp458=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp436)->f1; goto _LL447;} else{ goto _LL448;}
_LL448: goto _LL449; _LL439: { struct Cyc_List_List* args=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp450->tvs); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp460=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp460[ 0]=({ struct Cyc_Absyn_StructType_struct _temp461; _temp461.tag= Cyc_Absyn_StructType_tag;
_temp461.f1=( struct _tuple1*)(( struct _tuple1*)({ struct Cyc_Core_Opt*
_temp462= _temp450->name; if( _temp462 == 0){ _throw( Null_Exception);} _temp462->v;}));
_temp461.f2= args; _temp461.f3= 0; _temp461;}); _temp460;}); if( _temp450->fields
!= 0){ declopt=({ struct Cyc_Core_Opt* _temp463=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp463->v=( void*) _temp416;
_temp463;});} goto _LL437;} _LL441: { struct Cyc_List_List* args=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp452->tvs); t=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp464=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp464[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp465; _temp465.tag= Cyc_Absyn_TunionType_tag;
_temp465.f1=({ struct Cyc_Absyn_TunionInfo _temp466; _temp466.name=( struct
_tuple1*)({ struct Cyc_Core_Opt* _temp467= _temp452->name; if( _temp467 == 0){
_throw( Null_Exception);} _temp467->v;}); _temp466.targs= args; _temp466.rgn=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp466.tud= 0; _temp466;}); _temp465;});
_temp464;}); if( _temp452->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp468=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp468->v=( void*) _temp416; _temp468;});} goto _LL437;} _LL443: t=( void*)({
struct Cyc_Absyn_XTunionType_struct* _temp469=( struct Cyc_Absyn_XTunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct)); _temp469[ 0]=({ struct
Cyc_Absyn_XTunionType_struct _temp470; _temp470.tag= Cyc_Absyn_XTunionType_tag;
_temp470.f1=({ struct Cyc_Absyn_XTunionInfo _temp471; _temp471.name= _temp454->name;
_temp471.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp471.xtud= 0; _temp471;});
_temp470;}); _temp469;}); if( _temp454->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp472=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp472->v=( void*) _temp416; _temp472;});} goto _LL437; _LL445: { struct Cyc_List_List*
args=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp456->tvs); t=( void*)({
struct Cyc_Absyn_UnionType_struct* _temp473=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp473[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp474; _temp474.tag= Cyc_Absyn_UnionType_tag;
_temp474.f1=( struct _tuple1*)(( struct _tuple1*)({ struct Cyc_Core_Opt*
_temp475= _temp456->name; if( _temp475 == 0){ _throw( Null_Exception);} _temp475->v;}));
_temp474.f2= args; _temp474.f3= 0; _temp474;}); _temp473;}); if( _temp456->fields
!= 0){ declopt=({ struct Cyc_Core_Opt* _temp476=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp476->v=( void*) _temp416;
_temp476;});} goto _LL437;} _LL447: t=( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp477=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp477[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp478; _temp478.tag= Cyc_Absyn_EnumType_tag;
_temp478.f1= _temp458->name; _temp478.f2= 0; _temp478;}); _temp477;}); declopt=({
struct Cyc_Core_Opt* _temp479=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp479->v=( void*) _temp416; _temp479;}); goto _LL437; _LL449:((
void(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp480=( char*)"bad declaration within type specifier";
struct _tagged_string _temp481; _temp481.curr= _temp480; _temp481.base= _temp480;
_temp481.last_plus_one= _temp480 + 38; _temp481;}), _temp416->loc); goto _LL437;
_LL437:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3, _temp416->loc);} goto _LL391;
_LL391:;} if( ! seen_type){ if( ! seen_sign? ! seen_size: 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp482=( char*)"missing type withing specifier";
struct _tagged_string _temp483; _temp483.curr= _temp482; _temp483.base= _temp482;
_temp483.last_plus_one= _temp482 + 31; _temp483;}), last_loc);} t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp484=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp484[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp485; _temp485.tag= Cyc_Absyn_IntType_tag; _temp485.f1=( void*) sgn;
_temp485.f2=( void*) sz; _temp485;}); _temp484;});} else{ if( seen_sign){ void*
_temp486= t; void* _temp492; void* _temp494; _LL488: if(( unsigned int) _temp486
> 4u?(( struct _tunion_struct*) _temp486)->tag == Cyc_Absyn_IntType_tag: 0){
_LL495: _temp494=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp486)->f1;
goto _LL493; _LL493: _temp492=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp486)->f2; goto _LL489;} else{ goto _LL490;} _LL490: goto _LL491; _LL489: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp496=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp496[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp497; _temp497.tag= Cyc_Absyn_IntType_tag; _temp497.f1=( void*) sgn;
_temp497.f2=( void*) _temp492; _temp497;}); _temp496;}); goto _LL487; _LL491:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp498=( char*)"sign specification on non-integral type";
struct _tagged_string _temp499; _temp499.curr= _temp498; _temp499.base= _temp498;
_temp499.last_plus_one= _temp498 + 40; _temp499;}), last_loc); goto _LL487;
_LL487:;} if( seen_size){ void* _temp500= t; void* _temp506; void* _temp508;
_LL502: if(( unsigned int) _temp500 > 4u?(( struct _tunion_struct*) _temp500)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL509: _temp508=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp500)->f1; goto _LL507; _LL507: _temp506=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp500)->f2; goto _LL503;} else{ goto _LL504;} _LL504: goto _LL505; _LL503: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp510=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp510[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp511; _temp511.tag= Cyc_Absyn_IntType_tag; _temp511.f1=( void*) _temp508;
_temp511.f2=( void*) sz; _temp511;}); _temp510;}); goto _LL501; _LL505: Cyc_Parse_err((
struct _tagged_string)({ char* _temp512=( char*)"size qualifier on non-integral type";
struct _tagged_string _temp513; _temp513.curr= _temp512; _temp513.base= _temp512;
_temp513.last_plus_one= _temp512 + 36; _temp513;}), last_loc); goto _LL501;
_LL501:;}} return({ struct _tuple5 _temp514; _temp514.f1= t; _temp514.f2=
declopt; _temp514;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss( struct
Cyc_Absyn_Tqual* tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* d=( struct
Cyc_Parse_Declarator*)({ struct Cyc_List_List* _temp515= ds; if( _temp515 == 0){
_throw( Null_Exception);} _temp515->hd;}); struct _tuple1* q= d->id; struct Cyc_List_List*
_temp518; struct Cyc_List_List* _temp520; void* _temp522; struct Cyc_Absyn_Tqual*
_temp524; struct _tuple6 _temp516= Cyc_Parse_apply_tms( tq, t, shared_atts, d->tms);
_LL525: _temp524= _temp516.f1; goto _LL523; _LL523: _temp522= _temp516.f2; goto
_LL521; _LL521: _temp520= _temp516.f3; goto _LL519; _LL519: _temp518= _temp516.f4;
goto _LL517; _LL517: return({ struct Cyc_List_List* _temp526=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp526->hd=( void*)({ struct
_tuple7* _temp528=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp528->f1= q; _temp528->f2= _temp524; _temp528->f3= _temp522; _temp528->f4=
_temp520; _temp528->f5= _temp518; _temp528;}); _temp526->tl= Cyc_Parse_apply_tmss(
_temp524, t,({ struct Cyc_List_List* _temp527= ds; if( _temp527 == 0){ _throw(
Null_Exception);} _temp527->tl;}), shared_atts); _temp526;});}} static int Cyc_Parse_fn_type_att(
void* a){ void* _temp529= a; int _temp543; _LL531: if(( unsigned int) _temp529 >
15u?(( struct _tunion_struct*) _temp529)->tag == Cyc_Absyn_Regparm_att_tag: 0){
_LL544: _temp543=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp529)->f1;
goto _LL532;} else{ goto _LL533;} _LL533: if(( int) _temp529 == Cyc_Absyn_Stdcall_att){
goto _LL534;} else{ goto _LL535;} _LL535: if(( int) _temp529 == Cyc_Absyn_Cdecl_att){
goto _LL536;} else{ goto _LL537;} _LL537: if(( int) _temp529 == Cyc_Absyn_Noreturn_att){
goto _LL538;} else{ goto _LL539;} _LL539: if(( int) _temp529 == Cyc_Absyn_Const_att){
goto _LL540;} else{ goto _LL541;} _LL541: goto _LL542; _LL532: goto _LL534;
_LL534: goto _LL536; _LL536: goto _LL538; _LL538: goto _LL540; _LL540: return 1;
_LL542: return 0; _LL530:;} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual* tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp545; _temp545.f1= tq; _temp545.f2=
t; _temp545.f3= 0; _temp545.f4= atts; _temp545;});}{ void* _temp547=( void*)({
struct Cyc_List_List* _temp546= tms; if( _temp546 == 0){ _throw( Null_Exception);}
_temp546->hd;}); struct Cyc_Absyn_Exp* _temp561; void* _temp563; int _temp565;
struct Cyc_Position_Segment* _temp567; struct Cyc_List_List* _temp569; struct
Cyc_Absyn_Tqual* _temp571; void* _temp573; void* _temp575; struct Cyc_List_List*
_temp577; struct Cyc_Position_Segment* _temp579; _LL549: if(( int) _temp547 ==
Cyc_Absyn_Carray_mod){ goto _LL550;} else{ goto _LL551;} _LL551: if((
unsigned int) _temp547 > 1u?(( struct _tunion_struct*) _temp547)->tag == Cyc_Absyn_ConstArray_mod_tag:
0){ _LL562: _temp561=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp547)->f1; goto _LL552;} else{ goto _LL553;} _LL553: if(( unsigned int)
_temp547 > 1u?(( struct _tunion_struct*) _temp547)->tag == Cyc_Absyn_Function_mod_tag:
0){ _LL564: _temp563=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp547)->f1;
goto _LL554;} else{ goto _LL555;} _LL555: if(( unsigned int) _temp547 > 1u?((
struct _tunion_struct*) _temp547)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){
_LL570: _temp569=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp547)->f1; goto _LL568; _LL568: _temp567=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp547)->f2; goto _LL566; _LL566:
_temp565=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp547)->f3; goto
_LL556;} else{ goto _LL557;} _LL557: if(( unsigned int) _temp547 > 1u?(( struct
_tunion_struct*) _temp547)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL576:
_temp575=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp547)->f1; goto
_LL574; _LL574: _temp573=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp547)->f2; goto _LL572; _LL572: _temp571=( struct Cyc_Absyn_Tqual*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp547)->f3; goto _LL558;} else{ goto _LL559;}
_LL559: if(( unsigned int) _temp547 > 1u?(( struct _tunion_struct*) _temp547)->tag
== Cyc_Absyn_Attributes_mod_tag: 0){ _LL580: _temp579=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_Attributes_mod_struct*) _temp547)->f1; goto _LL578; _LL578:
_temp577=( struct Cyc_List_List*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp547)->f2; goto _LL560;} else{ goto _LL548;} _LL550: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp581=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp581[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp582; _temp582.tag= Cyc_Absyn_ArrayType_tag;
_temp582.f1=( void*) t; _temp582.f2= tq; _temp582.f3= 0; _temp582;}); _temp581;}),
atts,({ struct Cyc_List_List* _temp583= tms; if( _temp583 == 0){ _throw(
Null_Exception);} _temp583->tl;})); _LL552: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp584=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp584[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp585; _temp585.tag= Cyc_Absyn_ArrayType_tag;
_temp585.f1=( void*) t; _temp585.f2= tq; _temp585.f3=( struct Cyc_Absyn_Exp*)
_temp561; _temp585;}); _temp584;}), atts,({ struct Cyc_List_List* _temp586= tms;
if( _temp586 == 0){ _throw( Null_Exception);} _temp586->tl;})); _LL554: { void*
_temp587= _temp563; struct Cyc_Core_Opt* _temp593; int _temp595; struct Cyc_List_List*
_temp597; struct Cyc_Position_Segment* _temp599; struct Cyc_List_List* _temp601;
_LL589: if((( struct _tunion_struct*) _temp587)->tag == Cyc_Absyn_WithTypes_tag){
_LL598: _temp597=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp587)->f1; goto _LL596; _LL596: _temp595=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp587)->f2; goto _LL594; _LL594: _temp593=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp587)->f3; goto _LL590;} else{ goto _LL591;}
_LL591: if((( struct _tunion_struct*) _temp587)->tag == Cyc_Absyn_NoTypes_tag){
_LL602: _temp601=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp587)->f1; goto _LL600; _LL600: _temp599=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_NoTypes_struct*) _temp587)->f2; goto _LL592;} else{ goto _LL588;}
_LL590: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts; for( 0; as
!= 0; as=({ struct Cyc_List_List* _temp603= as; if( _temp603 == 0){ _throw(
Null_Exception);} _temp603->tl;})){ if( Cyc_Parse_fn_type_att(( void*)({ struct
Cyc_List_List* _temp604= as; if( _temp604 == 0){ _throw( Null_Exception);}
_temp604->hd;}))){ fn_atts=({ struct Cyc_List_List* _temp605=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp605->hd=( void*)(( void*)({
struct Cyc_List_List* _temp606= as; if( _temp606 == 0){ _throw( Null_Exception);}
_temp606->hd;})); _temp605->tl= fn_atts; _temp605;});} else{ new_atts=({ struct
Cyc_List_List* _temp607=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp607->hd=( void*)(( void*)({ struct Cyc_List_List* _temp608= as; if(
_temp608 == 0){ _throw( Null_Exception);} _temp608->hd;})); _temp607->tl=
new_atts; _temp607;});}}} if(({ struct Cyc_List_List* _temp609= tms; if(
_temp609 == 0){ _throw( Null_Exception);} _temp609->tl;}) != 0){ void* _temp612=(
void*)({ struct Cyc_List_List* _temp611=({ struct Cyc_List_List* _temp610= tms;
if( _temp610 == 0){ _throw( Null_Exception);} _temp610->tl;}); if( _temp611 == 0){
_throw( Null_Exception);} _temp611->hd;}); int _temp618; struct Cyc_Position_Segment*
_temp620; struct Cyc_List_List* _temp622; _LL614: if(( unsigned int) _temp612 >
1u?(( struct _tunion_struct*) _temp612)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){
_LL623: _temp622=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp612)->f1; goto _LL621; _LL621: _temp620=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp612)->f2; goto _LL619; _LL619:
_temp618=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp612)->f3; goto
_LL615;} else{ goto _LL616;} _LL616: goto _LL617; _LL615: typvars= _temp622; tms=({
struct Cyc_List_List* _temp624= tms; if( _temp624 == 0){ _throw( Null_Exception);}
_temp624->tl;}); goto _LL613; _LL617: goto _LL613; _LL613:;} if(((( ! _temp595?
_temp597 != 0: 0)?({ struct Cyc_List_List* _temp625= _temp597; if( _temp625 == 0){
_throw( Null_Exception);} _temp625->tl;}) == 0: 0)?(*(( struct _tuple2*)({
struct Cyc_List_List* _temp626= _temp597; if( _temp626 == 0){ _throw(
Null_Exception);} _temp626->hd;}))).f1 == 0: 0)?(*(( struct _tuple2*)({ struct
Cyc_List_List* _temp627= _temp597; if( _temp627 == 0){ _throw( Null_Exception);}
_temp627->hd;}))).f3 ==( void*) Cyc_Absyn_VoidType: 0){ _temp597= 0; _temp595= 0;}
t= Cyc_Parse_array2ptr( t);(( void(*)( void(* f)( struct _tuple2*), struct Cyc_List_List*
x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr, _temp597); return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ( typvars, _temp593, t, _temp597,
_temp595, fn_atts), new_atts,({ struct Cyc_List_List* _temp628= tms; if(
_temp628 == 0){ _throw( Null_Exception);} _temp628->tl;}));} _LL592:( void)
_throw((( struct _xtunion_struct*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp629=( char*)"function declaration without parameter types";
struct _tagged_string _temp630; _temp630.curr= _temp629; _temp630.base= _temp629;
_temp630.last_plus_one= _temp629 + 45; _temp630;}), _temp599)); _LL588:;} _LL556:
if(({ struct Cyc_List_List* _temp631= tms; if( _temp631 == 0){ _throw(
Null_Exception);} _temp631->tl;}) == 0){ return({ struct _tuple6 _temp632;
_temp632.f1= tq; _temp632.f2= t; _temp632.f3= _temp569; _temp632.f4= atts;
_temp632;});}( void) _throw((( struct _xtunion_struct*(*)( struct _tagged_string
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_string)({
char* _temp633=( char*)"type parameters must appear before function arguments in declarator";
struct _tagged_string _temp634; _temp634.curr= _temp633; _temp634.base= _temp633;
_temp634.last_plus_one= _temp633 + 68; _temp634;}), _temp567)); _LL558: { void*
_temp635= _temp575; struct Cyc_Absyn_Exp* _temp643; struct Cyc_Absyn_Exp*
_temp645; _LL637: if(( unsigned int) _temp635 > 1u?(( struct _tunion_struct*)
_temp635)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL644: _temp643=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp635)->f1; goto
_LL638;} else{ goto _LL639;} _LL639: if(( unsigned int) _temp635 > 1u?(( struct
_tunion_struct*) _temp635)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL646:
_temp645=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp635)->f1; goto _LL640;} else{ goto _LL641;} _LL641: if(( int) _temp635 ==
Cyc_Absyn_TaggedArray_ps){ goto _LL642;} else{ goto _LL636;} _LL638: return Cyc_Parse_apply_tms(
_temp571, Cyc_Absyn_atb_typ( t, _temp573, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp647=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp647[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp648; _temp648.tag= Cyc_Absyn_Upper_b_tag;
_temp648.f1= _temp643; _temp648;}); _temp647;})), atts,({ struct Cyc_List_List*
_temp649= tms; if( _temp649 == 0){ _throw( Null_Exception);} _temp649->tl;}));
_LL640: return Cyc_Parse_apply_tms( _temp571, Cyc_Absyn_starb_typ( t, _temp573,
tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp650=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp650[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp651; _temp651.tag= Cyc_Absyn_Upper_b_tag; _temp651.f1= _temp645; _temp651;});
_temp650;})), atts,({ struct Cyc_List_List* _temp652= tms; if( _temp652 == 0){
_throw( Null_Exception);} _temp652->tl;})); _LL642: return Cyc_Parse_apply_tms(
_temp571, Cyc_Absyn_tagged_typ( t, _temp573, tq), atts,({ struct Cyc_List_List*
_temp653= tms; if( _temp653 == 0){ _throw( Null_Exception);} _temp653->tl;}));
_LL636:;} _LL560: return Cyc_Parse_apply_tms( tq, t,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( atts,
_temp577),({ struct Cyc_List_List* _temp654= tms; if( _temp654 == 0){ _throw(
Null_Exception);} _temp654->tl;})); _LL548:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp657; void* _temp659; struct _tuple5 _temp655= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL660: _temp659= _temp655.f1; goto _LL658; _LL658: _temp657=
_temp655.f2; goto _LL656; _LL656: if( _temp657 != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp661=( char*)"ignoring nested type declaration(s) in specifier list";
struct _tagged_string _temp662; _temp662.curr= _temp661; _temp662.base= _temp661;
_temp662.last_plus_one= _temp661 + 54; _temp662;}), loc);} return _temp659;}
static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d,
struct Cyc_Absyn_Stmt* s){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct*
_temp663=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp663[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp664; _temp664.tag= Cyc_Absyn_Decl_s_tag;
_temp664.f1= d; _temp664.f2= s; _temp664;}); _temp663;}), Cyc_Position_segment_join(
d->loc, s->loc));} static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_declarations(
struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt* s){ return(( struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*, struct Cyc_Absyn_Stmt*),
struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum)) Cyc_List_fold_right)(
Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List* Cyc_Parse_make_declarations(
struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List* ids, struct Cyc_Position_Segment*
loc){ struct Cyc_List_List* tss= ds->type_specs; struct Cyc_Absyn_Tqual* tq= ds->tq;
int istypedef= 0; void* s=( void*) Cyc_Absyn_Public; struct Cyc_List_List* atts=
ds->attributes; if( ds->is_inline){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp665=( char*)"inline is only allowed on function definitions"; struct
_tagged_string _temp666; _temp666.curr= _temp665; _temp666.base= _temp665;
_temp666.last_plus_one= _temp665 + 47; _temp666;}), loc);} if( tss == 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp667=( char*)"missing type specifiers in declaration";
struct _tagged_string _temp668; _temp668.curr= _temp667; _temp668.base= _temp667;
_temp668.last_plus_one= _temp667 + 39; _temp668;}), loc); return 0;} if( ds->sc
!= 0){ void* _temp670=( void*)({ struct Cyc_Core_Opt* _temp669= ds->sc; if(
_temp669 == 0){ _throw( Null_Exception);} _temp669->v;}); _LL672: if(( int)
_temp670 == Cyc_Parse_Typedef_sc){ goto _LL673;} else{ goto _LL674;} _LL674: if((
int) _temp670 == Cyc_Parse_Extern_sc){ goto _LL675;} else{ goto _LL676;} _LL676:
if(( int) _temp670 == Cyc_Parse_ExternC_sc){ goto _LL677;} else{ goto _LL678;}
_LL678: if(( int) _temp670 == Cyc_Parse_Static_sc){ goto _LL679;} else{ goto
_LL680;} _LL680: if(( int) _temp670 == Cyc_Parse_Auto_sc){ goto _LL681;} else{
goto _LL682;} _LL682: if(( int) _temp670 == Cyc_Parse_Register_sc){ goto _LL683;}
else{ goto _LL684;} _LL684: if(( int) _temp670 == Cyc_Parse_Abstract_sc){ goto
_LL685;} else{ goto _LL671;} _LL673: istypedef= 1; goto _LL671; _LL675: s=( void*)
Cyc_Absyn_Extern; goto _LL671; _LL677: s=( void*) Cyc_Absyn_ExternC; goto _LL671;
_LL679: s=( void*) Cyc_Absyn_Static; goto _LL671; _LL681: s=( void*) Cyc_Absyn_Public;
goto _LL671; _LL683: s=( void*) Cyc_Absyn_Public; goto _LL671; _LL685: s=( void*)
Cyc_Absyn_Abstract; goto _LL671; _LL671:;}{ struct Cyc_List_List* _temp688;
struct Cyc_List_List* _temp690; struct _tuple0 _temp686=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL691: _temp690= _temp686.f1;
goto _LL689; _LL689: _temp688= _temp686.f2; goto _LL687; _LL687: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp688; for( 0; es != 0; es=({
struct Cyc_List_List* _temp692= es; if( _temp692 == 0){ _throw( Null_Exception);}
_temp692->tl;})){ if(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp693=
es; if( _temp693 == 0){ _throw( Null_Exception);} _temp693->hd;}) != 0){
exps_empty= 0; break;}}}{ struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, loc); if( _temp690 == 0){ struct Cyc_Core_Opt* _temp696; void* _temp698;
struct _tuple5 _temp694= ts_info; _LL699: _temp698= _temp694.f1; goto _LL697;
_LL697: _temp696= _temp694.f2; goto _LL695; _LL695: if( _temp696 != 0){ struct
Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct Cyc_Core_Opt* _temp700=
_temp696; if( _temp700 == 0){ _throw( Null_Exception);} _temp700->v;});{ void*
_temp701=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp715; struct Cyc_Absyn_Structdecl*
_temp717; struct Cyc_Absyn_Uniondecl* _temp719; struct Cyc_Absyn_Tuniondecl*
_temp721; struct Cyc_Absyn_XTuniondecl* _temp723; _LL703: if((( struct
_tunion_struct*) _temp701)->tag == Cyc_Absyn_Enum_d_tag){ _LL716: _temp715=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp701)->f1;
goto _LL704;} else{ goto _LL705;} _LL705: if((( struct _tunion_struct*) _temp701)->tag
== Cyc_Absyn_Struct_d_tag){ _LL718: _temp717=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp701)->f1; goto _LL706;} else{ goto
_LL707;} _LL707: if((( struct _tunion_struct*) _temp701)->tag == Cyc_Absyn_Union_d_tag){
_LL720: _temp719=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp701)->f1; goto _LL708;} else{ goto _LL709;} _LL709: if((( struct
_tunion_struct*) _temp701)->tag == Cyc_Absyn_Tunion_d_tag){ _LL722: _temp721=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*) _temp701)->f1;
goto _LL710;} else{ goto _LL711;} _LL711: if((( struct _tunion_struct*) _temp701)->tag
== Cyc_Absyn_XTunion_d_tag){ _LL724: _temp723=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_d_struct*) _temp701)->f1; goto _LL712;} else{ goto
_LL713;} _LL713: goto _LL714; _LL704:( void*)( _temp715->sc=( void*) s); if(
atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp725=( char*)"bad attributes on enum";
struct _tagged_string _temp726; _temp726.curr= _temp725; _temp726.base= _temp725;
_temp726.last_plus_one= _temp725 + 23; _temp726;}), loc);} goto _LL702; _LL706:(
void*)( _temp717->sc=( void*) s); _temp717->attributes= atts; goto _LL702;
_LL708:( void*)( _temp719->sc=( void*) s); _temp719->attributes= atts; goto
_LL702; _LL710:( void*)( _temp721->sc=( void*) s); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp727=( char*)"bad attributes on tunion";
struct _tagged_string _temp728; _temp728.curr= _temp727; _temp728.base= _temp727;
_temp728.last_plus_one= _temp727 + 25; _temp728;}), loc);} goto _LL702; _LL712:(
void*)( _temp723->sc=( void*) s); if( atts != 0){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp729=( char*)"bad attributes on xtunion"; struct
_tagged_string _temp730; _temp730.curr= _temp729; _temp730.base= _temp729;
_temp730.last_plus_one= _temp729 + 26; _temp730;}), loc);} goto _LL702; _LL714:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp731=( char*)"bad declaration";
struct _tagged_string _temp732; _temp732.curr= _temp731; _temp732.base= _temp731;
_temp732.last_plus_one= _temp731 + 16; _temp732;}), loc); return 0; _LL702:;}
return({ struct Cyc_List_List* _temp733=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp733->hd=( void*) d; _temp733->tl= 0;
_temp733;});} else{ void* _temp734= _temp698; struct Cyc_Absyn_Structdecl**
_temp748; struct Cyc_List_List* _temp750; struct _tuple1* _temp752; struct Cyc_Absyn_TunionInfo
_temp754; struct Cyc_Absyn_Tuniondecl* _temp756; void* _temp758; struct Cyc_List_List*
_temp760; struct _tuple1* _temp762; struct Cyc_Absyn_XTunionInfo _temp764;
struct Cyc_Absyn_XTuniondecl* _temp766; void* _temp768; struct _tuple1* _temp770;
struct Cyc_Absyn_Uniondecl** _temp772; struct Cyc_List_List* _temp774; struct
_tuple1* _temp776; struct Cyc_Absyn_Enumdecl* _temp778; struct _tuple1* _temp780;
_LL736: if(( unsigned int) _temp734 > 4u?(( struct _tunion_struct*) _temp734)->tag
== Cyc_Absyn_StructType_tag: 0){ _LL753: _temp752=( struct _tuple1*)(( struct
Cyc_Absyn_StructType_struct*) _temp734)->f1; goto _LL751; _LL751: _temp750=(
struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*) _temp734)->f2;
goto _LL749; _LL749: _temp748=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp734)->f3; goto _LL737;} else{ goto _LL738;} _LL738: if(( unsigned int)
_temp734 > 4u?(( struct _tunion_struct*) _temp734)->tag == Cyc_Absyn_TunionType_tag:
0){ _LL755: _temp754=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp734)->f1; _LL763: _temp762=( struct _tuple1*) _temp754.name; goto _LL761;
_LL761: _temp760=( struct Cyc_List_List*) _temp754.targs; goto _LL759; _LL759:
_temp758=( void*) _temp754.rgn; goto _LL757; _LL757: _temp756=( struct Cyc_Absyn_Tuniondecl*)
_temp754.tud; goto _LL739;} else{ goto _LL740;} _LL740: if(( unsigned int)
_temp734 > 4u?(( struct _tunion_struct*) _temp734)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL765: _temp764=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp734)->f1; _LL771: _temp770=( struct _tuple1*) _temp764.name; goto _LL769;
_LL769: _temp768=( void*) _temp764.rgn; goto _LL767; _LL767: _temp766=( struct
Cyc_Absyn_XTuniondecl*) _temp764.xtud; goto _LL741;} else{ goto _LL742;} _LL742:
if(( unsigned int) _temp734 > 4u?(( struct _tunion_struct*) _temp734)->tag ==
Cyc_Absyn_UnionType_tag: 0){ _LL777: _temp776=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp734)->f1; goto _LL775; _LL775: _temp774=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp734)->f2; goto _LL773; _LL773: _temp772=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp734)->f3;
goto _LL743;} else{ goto _LL744;} _LL744: if(( unsigned int) _temp734 > 4u?((
struct _tunion_struct*) _temp734)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL781:
_temp780=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp734)->f1;
goto _LL779; _LL779: _temp778=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp734)->f2; goto _LL745;} else{ goto _LL746;} _LL746: goto _LL747; _LL737: {
struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp750);
struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl* _temp782=(
struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp782->sc=( void*) s; _temp782->name=({ struct Cyc_Core_Opt* _temp783=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp783->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp784= _temp752; if( _temp784 ==
0){ _throw( Null_Exception);} _temp784;})); _temp783;}); _temp782->tvs= ts2;
_temp782->fields= 0; _temp782->attributes= 0; _temp782;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp785=( char*)"bad attributes on struct";
struct _tagged_string _temp786; _temp786.curr= _temp785; _temp786.base= _temp785;
_temp786.last_plus_one= _temp785 + 25; _temp786;}), loc);} return({ struct Cyc_List_List*
_temp787=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp787->hd=( void*)({ struct Cyc_Absyn_Decl* _temp788=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp788->r=( void*)(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp789=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp789[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp790; _temp790.tag= Cyc_Absyn_Struct_d_tag;
_temp790.f1= sd; _temp790;}); _temp789;})); _temp788->loc= loc; _temp788;});
_temp787->tl= 0; _temp787;});} _LL739: { struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp760); struct Cyc_Absyn_Decl* tud= Cyc_Absyn_tunion_decl( s,({ struct Cyc_Core_Opt*
_temp791=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp791->v=( void*)(( struct _tuple1*) _temp762); _temp791;}), ts2, 0, loc);
if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp792=( char*)"bad attributes on tunion";
struct _tagged_string _temp793; _temp793.curr= _temp792; _temp793.base= _temp792;
_temp793.last_plus_one= _temp792 + 25; _temp793;}), loc);} return({ struct Cyc_List_List*
_temp794=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp794->hd=( void*) tud; _temp794->tl= 0; _temp794;});} _LL741: { struct Cyc_Absyn_XTuniondecl*
xtud=({ struct Cyc_Absyn_XTuniondecl* _temp795=( struct Cyc_Absyn_XTuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_XTuniondecl)); _temp795->sc=( void*) s;
_temp795->name= _temp770; _temp795->fields= 0; _temp795;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp796=( char*)"bad attributes on xtunion";
struct _tagged_string _temp797; _temp797.curr= _temp796; _temp797.base= _temp796;
_temp797.last_plus_one= _temp796 + 26; _temp797;}), loc);} return({ struct Cyc_List_List*
_temp798=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp798->hd=( void*)({ struct Cyc_Absyn_Decl* _temp799=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp799->r=( void*)(( void*)({
struct Cyc_Absyn_XTunion_d_struct* _temp800=( struct Cyc_Absyn_XTunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunion_d_struct)); _temp800[ 0]=({ struct
Cyc_Absyn_XTunion_d_struct _temp801; _temp801.tag= Cyc_Absyn_XTunion_d_tag;
_temp801.f1= xtud; _temp801;}); _temp800;})); _temp799->loc= loc; _temp799;});
_temp798->tl= 0; _temp798;});} _LL743: { struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp774); struct Cyc_Absyn_Uniondecl* ud=({ struct Cyc_Absyn_Uniondecl*
_temp802=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp802->sc=( void*) s; _temp802->name=({ struct Cyc_Core_Opt* _temp803=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp803->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp804= _temp776; if( _temp804 ==
0){ _throw( Null_Exception);} _temp804;})); _temp803;}); _temp802->tvs= ts2;
_temp802->fields= 0; _temp802->attributes= 0; _temp802;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp805=( char*)"bad attributes on union";
struct _tagged_string _temp806; _temp806.curr= _temp805; _temp806.base= _temp805;
_temp806.last_plus_one= _temp805 + 24; _temp806;}), loc);} return({ struct Cyc_List_List*
_temp807=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp807->hd=( void*)({ struct Cyc_Absyn_Decl* _temp808=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp808->r=( void*)(( void*)({
struct Cyc_Absyn_Union_d_struct* _temp809=( struct Cyc_Absyn_Union_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp809[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp810; _temp810.tag= Cyc_Absyn_Union_d_tag; _temp810.f1= ud; _temp810;});
_temp809;})); _temp808->loc= loc; _temp808;}); _temp807->tl= 0; _temp807;});}
_LL745: { struct Cyc_Absyn_Enumdecl* ed=({ struct Cyc_Absyn_Enumdecl* _temp811=(
struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp811->sc=( void*) s; _temp811->name= _temp780; _temp811->fields= 0; _temp811;});
if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp812=( char*)"bad attributes on enum";
struct _tagged_string _temp813; _temp813.curr= _temp812; _temp813.base= _temp812;
_temp813.last_plus_one= _temp812 + 23; _temp813;}), loc);} return({ struct Cyc_List_List*
_temp814=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp814->hd=( void*)({ struct Cyc_Absyn_Decl* _temp815=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp815->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp816=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp816[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp817; _temp817.tag= Cyc_Absyn_Enum_d_tag; _temp817.f1= ed; _temp817;});
_temp816;})); _temp815->loc= loc; _temp815;}); _temp814->tl= 0; _temp814;});}
_LL747: Cyc_Parse_err(( struct _tagged_string)({ char* _temp818=( char*)"missing declarator";
struct _tagged_string _temp819; _temp819.curr= _temp818; _temp819.base= _temp818;
_temp819.last_plus_one= _temp818 + 19; _temp819;}), loc); return 0; _LL735:;}}
else{ void* t= ts_info.f1; struct Cyc_List_List* fields= Cyc_Parse_apply_tmss(
tq, t, _temp690, atts); if( istypedef){ if( ! exps_empty){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp820=( char*)"initializer in typedef declaration";
struct _tagged_string _temp821; _temp821.curr= _temp820; _temp821.base= _temp820;
_temp821.last_plus_one= _temp820 + 35; _temp821;}), loc);}{ struct Cyc_List_List*
decls=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*,
struct _tuple7*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef, loc, fields); if( ts_info.f2 != 0){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct Cyc_Core_Opt*
_temp822= ts_info.f2; if( _temp822 == 0){ _throw( Null_Exception);} _temp822->v;});{
void* _temp823=( void*) d->r; struct Cyc_Absyn_Structdecl* _temp837; struct Cyc_Absyn_Tuniondecl*
_temp839; struct Cyc_Absyn_XTuniondecl* _temp841; struct Cyc_Absyn_Uniondecl*
_temp843; struct Cyc_Absyn_Enumdecl* _temp845; _LL825: if((( struct
_tunion_struct*) _temp823)->tag == Cyc_Absyn_Struct_d_tag){ _LL838: _temp837=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp823)->f1;
goto _LL826;} else{ goto _LL827;} _LL827: if((( struct _tunion_struct*) _temp823)->tag
== Cyc_Absyn_Tunion_d_tag){ _LL840: _temp839=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp823)->f1; goto _LL828;} else{ goto
_LL829;} _LL829: if((( struct _tunion_struct*) _temp823)->tag == Cyc_Absyn_XTunion_d_tag){
_LL842: _temp841=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_d_struct*)
_temp823)->f1; goto _LL830;} else{ goto _LL831;} _LL831: if((( struct
_tunion_struct*) _temp823)->tag == Cyc_Absyn_Union_d_tag){ _LL844: _temp843=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp823)->f1;
goto _LL832;} else{ goto _LL833;} _LL833: if((( struct _tunion_struct*) _temp823)->tag
== Cyc_Absyn_Enum_d_tag){ _LL846: _temp845=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp823)->f1; goto _LL834;} else{ goto _LL835;}
_LL835: goto _LL836; _LL826:( void*)( _temp837->sc=( void*) s); _temp837->attributes=
atts; atts= 0; goto _LL824; _LL828:( void*)( _temp839->sc=( void*) s); goto
_LL824; _LL830:( void*)( _temp841->sc=( void*) s); goto _LL824; _LL832:( void*)(
_temp843->sc=( void*) s); goto _LL824; _LL834:( void*)( _temp845->sc=( void*) s);
goto _LL824; _LL836: Cyc_Parse_err(( struct _tagged_string)({ char* _temp847=(
char*)"declaration within typedef is not a struct, union, tunion, or xtunion";
struct _tagged_string _temp848; _temp848.curr= _temp847; _temp848.base= _temp847;
_temp848.last_plus_one= _temp847 + 70; _temp848;}), loc); goto _LL824; _LL824:;}
decls=({ struct Cyc_List_List* _temp849=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp849->hd=( void*) d; _temp849->tl= decls;
_temp849;});} if( atts != 0){ Cyc_Parse_err(({ struct _tagged_string _temp851=
Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List* _temp850= atts; if(
_temp850 == 0){ _throw( Null_Exception);} _temp850->hd;})); xprintf("bad attribute %.*s in typedef",
_temp851.last_plus_one - _temp851.curr, _temp851.curr);}), loc);} return decls;}}
else{ if( ts_info.f2 != 0){ Cyc_Parse_unimp2(( struct _tagged_string)({ char*
_temp852=( char*)"nested type declaration within declarator"; struct
_tagged_string _temp853; _temp853.curr= _temp852; _temp853.base= _temp852;
_temp853.last_plus_one= _temp852 + 42; _temp853;}), loc);}{ struct Cyc_List_List*
decls= 0;{ struct Cyc_List_List* ds= fields; for( 0; ds != 0; ds=({ struct Cyc_List_List*
_temp854= ds; if( _temp854 == 0){ _throw( Null_Exception);} _temp854->tl;}),
_temp688=({ struct Cyc_List_List* _temp855= _temp688; if( _temp855 == 0){ _throw(
Null_Exception);} _temp855->tl;})){ struct _tuple7 _temp859; struct Cyc_List_List*
_temp860; struct Cyc_List_List* _temp862; void* _temp864; struct Cyc_Absyn_Tqual*
_temp866; struct _tuple1* _temp868; struct _tuple7* _temp857=( struct _tuple7*)({
struct Cyc_List_List* _temp856= ds; if( _temp856 == 0){ _throw( Null_Exception);}
_temp856->hd;}); _temp859=* _temp857; _LL869: _temp868= _temp859.f1; goto _LL867;
_LL867: _temp866= _temp859.f2; goto _LL865; _LL865: _temp864= _temp859.f3; goto
_LL863; _LL863: _temp862= _temp859.f4; goto _LL861; _LL861: _temp860= _temp859.f5;
goto _LL858; _LL858: if( _temp862 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp870=( char*)"bad type params, ignoring"; struct _tagged_string
_temp871; _temp871.curr= _temp870; _temp871.base= _temp870; _temp871.last_plus_one=
_temp870 + 26; _temp871;}), loc);} if( _temp688 == 0){(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp872=( char*)"unexpected null in parse!"; struct
_tagged_string _temp873; _temp873.curr= _temp872; _temp873.base= _temp872;
_temp873.last_plus_one= _temp872 + 26; _temp873;}), loc);}{ struct Cyc_Absyn_Exp*
eopt=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp874= _temp688; if(
_temp874 == 0){ _throw( Null_Exception);} _temp874->hd;}); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_new_vardecl( _temp868, _temp864, eopt); vd->tq= _temp866;( void*)(
vd->sc=( void*) s); vd->attributes= _temp860;{ struct Cyc_Absyn_Decl* d=({
struct Cyc_Absyn_Decl* _temp875=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof(
struct Cyc_Absyn_Decl)); _temp875->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp876=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp876[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp877; _temp877.tag= Cyc_Absyn_Var_d_tag;
_temp877.f1= vd; _temp877;}); _temp876;})); _temp875->loc= loc; _temp875;});
decls=({ struct Cyc_List_List* _temp878=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp878->hd=( void*) d; _temp878->tl= decls;
_temp878;});}}}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( decls);}}}}}}} static void* Cyc_Parse_id_to_kind( struct
_tagged_string s, struct Cyc_Position_Segment* loc){ if( Cyc_String_strlen( s)
!= 1){ Cyc_Parse_err(({ struct _tagged_string _temp879= s; xprintf("bad kind: %.*s",
_temp879.last_plus_one - _temp879.curr, _temp879.curr);}), loc); return( void*)
Cyc_Absyn_BoxKind;} else{ switch(({ struct _tagged_string _temp880= s; char*
_temp882= _temp880.curr + 0; if( _temp880.base == 0? 1:( _temp882 < _temp880.base?
1: _temp882 >= _temp880.last_plus_one)){ _throw( Null_Exception);}* _temp882;})){
case 'A': _LL883: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL884: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL885: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL886: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL887: return(
void*) Cyc_Absyn_EffKind; default: _LL888: Cyc_Parse_err(({ struct
_tagged_string _temp890= s; xprintf("bad kind: %.*s", _temp890.last_plus_one -
_temp890.curr, _temp890.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;}}}
static struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts == 0){
return tms;} else{ return({ struct Cyc_List_List* _temp891=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp891->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp892=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp892[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp893; _temp893.tag= Cyc_Absyn_Attributes_mod_tag;
_temp893.f1= loc; _temp893.f2= atts; _temp893;}); _temp892;})); _temp891->tl=
tms; _temp891;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1;
Cyc_Lex_register_typedef( x); if((* t).f5 != 0){ Cyc_Parse_err(({ struct
_tagged_string _temp895= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List*
_temp894=(* t).f5; if( _temp894 == 0){ _throw( Null_Exception);} _temp894->hd;}));
xprintf("bad attribute %.*s within typedef", _temp895.last_plus_one - _temp895.curr,
_temp895.curr);}), loc);} return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct*
_temp896=( struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp896[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp897; _temp897.tag= Cyc_Absyn_Typedef_d_tag;
_temp897.f1=({ struct Cyc_Absyn_Typedefdecl* _temp898=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp898->name= x; _temp898->tvs=(*
t).f4; _temp898->defn=( void*)(* t).f3; _temp898;}); _temp897;}); _temp896;}),
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
char* tag; struct Cyc_List_List* f1; } ; static char _temp901[ 8u]="Int_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp901, _temp901, _temp901 + 8u}}; struct _tuple12*
Cyc_yyget_Int_tok( struct _xtunion_struct* yy1){ struct _xtunion_struct*
_temp902= yy1; struct _tuple12* _temp908; _LL904: if((*(( struct _xtunion_struct*)
_temp902)).tag == Cyc_Int_tok_tag){ _LL909: _temp908=(( struct Cyc_Int_tok_struct*)
_temp902)->f1; goto _LL905;} else{ goto _LL906;} _LL906: goto _LL907; _LL905:
return _temp908; _LL907:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Int_tok);
_LL903:;} static char _temp912[ 11u]="String_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp912,
_temp912, _temp912 + 11u}}; struct _tagged_string Cyc_yyget_String_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp913= yy1; struct
_tagged_string _temp919; _LL915: if((*(( struct _xtunion_struct*) _temp913)).tag
== Cyc_String_tok_tag){ _LL920: _temp919=(( struct Cyc_String_tok_struct*)
_temp913)->f1; goto _LL916;} else{ goto _LL917;} _LL917: goto _LL918; _LL916:
return _temp919; _LL918:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_String_tok);
_LL914:;} static char _temp923[ 9u]="Char_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Char_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp923,
_temp923, _temp923 + 9u}}; char Cyc_yyget_Char_tok( struct _xtunion_struct* yy1){
struct _xtunion_struct* _temp924= yy1; char _temp930; _LL926: if((*(( struct
_xtunion_struct*) _temp924)).tag == Cyc_Char_tok_tag){ _LL931: _temp930=((
struct Cyc_Char_tok_struct*) _temp924)->f1; goto _LL927;} else{ goto _LL928;}
_LL928: goto _LL929; _LL927: return _temp930; _LL929:( void) _throw(( struct
_xtunion_struct*)& Cyc_yyfail_Char_tok); _LL925:;} static char _temp934[ 17u]="Pointer_Sort_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp934, _temp934, _temp934 + 17u}}; void* Cyc_yyget_Pointer_Sort_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp935= yy1; void*
_temp941; _LL937: if((*(( struct _xtunion_struct*) _temp935)).tag == Cyc_Pointer_Sort_tok_tag){
_LL942: _temp941=(( struct Cyc_Pointer_Sort_tok_struct*) _temp935)->f1; goto
_LL938;} else{ goto _LL939;} _LL939: goto _LL940; _LL938: return _temp941;
_LL940:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Pointer_Sort_tok);
_LL936:;} static char _temp945[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Exp_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp945,
_temp945, _temp945 + 8u}}; struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp946= yy1; struct Cyc_Absyn_Exp*
_temp952; _LL948: if((*(( struct _xtunion_struct*) _temp946)).tag == Cyc_Exp_tok_tag){
_LL953: _temp952=(( struct Cyc_Exp_tok_struct*) _temp946)->f1; goto _LL949;}
else{ goto _LL950;} _LL950: goto _LL951; _LL949: return _temp952; _LL951:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_Exp_tok); _LL947:;} static char
_temp956[ 12u]="ExpList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp956, _temp956, _temp956 + 12u}};
struct Cyc_List_List* Cyc_yyget_ExpList_tok( struct _xtunion_struct* yy1){
struct _xtunion_struct* _temp957= yy1; struct Cyc_List_List* _temp963; _LL959:
if((*(( struct _xtunion_struct*) _temp957)).tag == Cyc_ExpList_tok_tag){ _LL964:
_temp963=(( struct Cyc_ExpList_tok_struct*) _temp957)->f1; goto _LL960;} else{
goto _LL961;} _LL961: goto _LL962; _LL960: return _temp963; _LL962:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_ExpList_tok); _LL958:;} static
char _temp967[ 20u]="InitializerList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp967, _temp967, _temp967 + 20u}}; struct Cyc_List_List* Cyc_yyget_InitializerList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp968= yy1; struct Cyc_List_List*
_temp974; _LL970: if((*(( struct _xtunion_struct*) _temp968)).tag == Cyc_InitializerList_tok_tag){
_LL975: _temp974=(( struct Cyc_InitializerList_tok_struct*) _temp968)->f1; goto
_LL971;} else{ goto _LL972;} _LL972: goto _LL973; _LL971: return _temp974;
_LL973:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_InitializerList_tok);
_LL969:;} static char _temp978[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp978,
_temp978, _temp978 + 11u}}; void* Cyc_yyget_Primop_tok( struct _xtunion_struct*
yy1){ struct _xtunion_struct* _temp979= yy1; void* _temp985; _LL981: if((*((
struct _xtunion_struct*) _temp979)).tag == Cyc_Primop_tok_tag){ _LL986: _temp985=((
struct Cyc_Primop_tok_struct*) _temp979)->f1; goto _LL982;} else{ goto _LL983;}
_LL983: goto _LL984; _LL982: return _temp985; _LL984:( void) _throw(( struct
_xtunion_struct*)& Cyc_yyfail_Primop_tok); _LL980:;} static char _temp989[ 14u]="Primopopt_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp989, _temp989, _temp989 + 14u}}; struct Cyc_Core_Opt*
Cyc_yyget_Primopopt_tok( struct _xtunion_struct* yy1){ struct _xtunion_struct*
_temp990= yy1; struct Cyc_Core_Opt* _temp996; _LL992: if((*(( struct
_xtunion_struct*) _temp990)).tag == Cyc_Primopopt_tok_tag){ _LL997: _temp996=((
struct Cyc_Primopopt_tok_struct*) _temp990)->f1; goto _LL993;} else{ goto _LL994;}
_LL994: goto _LL995; _LL993: return _temp996; _LL995:( void) _throw(( struct
_xtunion_struct*)& Cyc_yyfail_Primopopt_tok); _LL991:;} static char _temp1000[
11u]="QualId_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1000, _temp1000, _temp1000 +
11u}}; struct _tuple1* Cyc_yyget_QualId_tok( struct _xtunion_struct* yy1){
struct _xtunion_struct* _temp1001= yy1; struct _tuple1* _temp1007; _LL1003: if((*((
struct _xtunion_struct*) _temp1001)).tag == Cyc_QualId_tok_tag){ _LL1008:
_temp1007=(( struct Cyc_QualId_tok_struct*) _temp1001)->f1; goto _LL1004;} else{
goto _LL1005;} _LL1005: goto _LL1006; _LL1004: return _temp1007; _LL1006:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_QualId_tok); _LL1002:;} static
char _temp1011[ 9u]="Stmt_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1011, _temp1011, _temp1011 +
9u}}; struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( struct _xtunion_struct* yy1){
struct _xtunion_struct* _temp1012= yy1; struct Cyc_Absyn_Stmt* _temp1018;
_LL1014: if((*(( struct _xtunion_struct*) _temp1012)).tag == Cyc_Stmt_tok_tag){
_LL1019: _temp1018=(( struct Cyc_Stmt_tok_struct*) _temp1012)->f1; goto _LL1015;}
else{ goto _LL1016;} _LL1016: goto _LL1017; _LL1015: return _temp1018; _LL1017:(
void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Stmt_tok); _LL1013:;} static
char _temp1022[ 14u]="BlockItem_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_BlockItem_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1022, _temp1022, _temp1022 +
14u}}; void* Cyc_yyget_BlockItem_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1023= yy1; void* _temp1029; _LL1025: if((*(( struct
_xtunion_struct*) _temp1023)).tag == Cyc_BlockItem_tok_tag){ _LL1030: _temp1029=((
struct Cyc_BlockItem_tok_struct*) _temp1023)->f1; goto _LL1026;} else{ goto
_LL1027;} _LL1027: goto _LL1028; _LL1026: return _temp1029; _LL1028:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_BlockItem_tok); _LL1024:;} static
char _temp1033[ 21u]="SwitchClauseList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1033, _temp1033, _temp1033 + 21u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1034= yy1; struct Cyc_List_List*
_temp1040; _LL1036: if((*(( struct _xtunion_struct*) _temp1034)).tag == Cyc_SwitchClauseList_tok_tag){
_LL1041: _temp1040=(( struct Cyc_SwitchClauseList_tok_struct*) _temp1034)->f1;
goto _LL1037;} else{ goto _LL1038;} _LL1038: goto _LL1039; _LL1037: return
_temp1040; _LL1039:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_SwitchClauseList_tok);
_LL1035:;} static char _temp1044[ 12u]="Pattern_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1044, _temp1044, _temp1044 + 12u}}; struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1045= yy1; struct Cyc_Absyn_Pat*
_temp1051; _LL1047: if((*(( struct _xtunion_struct*) _temp1045)).tag == Cyc_Pattern_tok_tag){
_LL1052: _temp1051=(( struct Cyc_Pattern_tok_struct*) _temp1045)->f1; goto
_LL1048;} else{ goto _LL1049;} _LL1049: goto _LL1050; _LL1048: return _temp1051;
_LL1050:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Pattern_tok);
_LL1046:;} static char _temp1055[ 16u]="PatternList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1055, _temp1055, _temp1055 + 16u}}; struct Cyc_List_List* Cyc_yyget_PatternList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1056= yy1; struct Cyc_List_List*
_temp1062; _LL1058: if((*(( struct _xtunion_struct*) _temp1056)).tag == Cyc_PatternList_tok_tag){
_LL1063: _temp1062=(( struct Cyc_PatternList_tok_struct*) _temp1056)->f1; goto
_LL1059;} else{ goto _LL1060;} _LL1060: goto _LL1061; _LL1059: return _temp1062;
_LL1061:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_PatternList_tok);
_LL1057:;} static char _temp1066[ 17u]="FieldPattern_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FieldPattern_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1066, _temp1066, _temp1066 + 17u}}; struct _tuple17* Cyc_yyget_FieldPattern_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1067= yy1; struct
_tuple17* _temp1073; _LL1069: if((*(( struct _xtunion_struct*) _temp1067)).tag
== Cyc_FieldPattern_tok_tag){ _LL1074: _temp1073=(( struct Cyc_FieldPattern_tok_struct*)
_temp1067)->f1; goto _LL1070;} else{ goto _LL1071;} _LL1071: goto _LL1072;
_LL1070: return _temp1073; _LL1072:( void) _throw(( struct _xtunion_struct*)&
Cyc_yyfail_FieldPattern_tok); _LL1068:;} static char _temp1077[ 21u]="FieldPatternList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1077, _temp1077, _temp1077 + 21u}}; struct Cyc_List_List*
Cyc_yyget_FieldPatternList_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1078= yy1; struct Cyc_List_List* _temp1084; _LL1080: if((*((
struct _xtunion_struct*) _temp1078)).tag == Cyc_FieldPatternList_tok_tag){
_LL1085: _temp1084=(( struct Cyc_FieldPatternList_tok_struct*) _temp1078)->f1;
goto _LL1081;} else{ goto _LL1082;} _LL1082: goto _LL1083; _LL1081: return
_temp1084; _LL1083:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_FieldPatternList_tok);
_LL1079:;} static char _temp1088[ 11u]="FnDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1088,
_temp1088, _temp1088 + 11u}}; struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1089= yy1; struct Cyc_Absyn_Fndecl*
_temp1095; _LL1091: if((*(( struct _xtunion_struct*) _temp1089)).tag == Cyc_FnDecl_tok_tag){
_LL1096: _temp1095=(( struct Cyc_FnDecl_tok_struct*) _temp1089)->f1; goto
_LL1092;} else{ goto _LL1093;} _LL1093: goto _LL1094; _LL1092: return _temp1095;
_LL1094:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_FnDecl_tok);
_LL1090:;} static char _temp1099[ 13u]="DeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1099, _temp1099, _temp1099 + 13u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1100= yy1; struct Cyc_List_List*
_temp1106; _LL1102: if((*(( struct _xtunion_struct*) _temp1100)).tag == Cyc_DeclList_tok_tag){
_LL1107: _temp1106=(( struct Cyc_DeclList_tok_struct*) _temp1100)->f1; goto
_LL1103;} else{ goto _LL1104;} _LL1104: goto _LL1105; _LL1103: return _temp1106;
_LL1105:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_DeclList_tok);
_LL1101:;} static char _temp1110[ 13u]="DeclSpec_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1110, _temp1110, _temp1110 + 13u}}; struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1111= yy1; struct Cyc_Parse_Declaration_spec*
_temp1117; _LL1113: if((*(( struct _xtunion_struct*) _temp1111)).tag == Cyc_DeclSpec_tok_tag){
_LL1118: _temp1117=(( struct Cyc_DeclSpec_tok_struct*) _temp1111)->f1; goto
_LL1114;} else{ goto _LL1115;} _LL1115: goto _LL1116; _LL1114: return _temp1117;
_LL1116:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_DeclSpec_tok);
_LL1112:;} static char _temp1121[ 13u]="InitDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1121, _temp1121, _temp1121 + 13u}}; struct _tuple13* Cyc_yyget_InitDecl_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1122= yy1; struct
_tuple13* _temp1128; _LL1124: if((*(( struct _xtunion_struct*) _temp1122)).tag
== Cyc_InitDecl_tok_tag){ _LL1129: _temp1128=(( struct Cyc_InitDecl_tok_struct*)
_temp1122)->f1; goto _LL1125;} else{ goto _LL1126;} _LL1126: goto _LL1127;
_LL1125: return _temp1128; _LL1127:( void) _throw(( struct _xtunion_struct*)&
Cyc_yyfail_InitDecl_tok); _LL1123:;} static char _temp1132[ 17u]="InitDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1132, _temp1132, _temp1132 + 17u}}; struct Cyc_List_List*
Cyc_yyget_InitDeclList_tok( struct _xtunion_struct* yy1){ struct _xtunion_struct*
_temp1133= yy1; struct Cyc_List_List* _temp1139; _LL1135: if((*(( struct
_xtunion_struct*) _temp1133)).tag == Cyc_InitDeclList_tok_tag){ _LL1140:
_temp1139=(( struct Cyc_InitDeclList_tok_struct*) _temp1133)->f1; goto _LL1136;}
else{ goto _LL1137;} _LL1137: goto _LL1138; _LL1136: return _temp1139; _LL1138:(
void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_InitDeclList_tok); _LL1134:;}
static char _temp1143[ 17u]="StorageClass_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1143, _temp1143, _temp1143 + 17u}}; void* Cyc_yyget_StorageClass_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1144= yy1; void*
_temp1150; _LL1146: if((*(( struct _xtunion_struct*) _temp1144)).tag == Cyc_StorageClass_tok_tag){
_LL1151: _temp1150=(( struct Cyc_StorageClass_tok_struct*) _temp1144)->f1; goto
_LL1147;} else{ goto _LL1148;} _LL1148: goto _LL1149; _LL1147: return _temp1150;
_LL1149:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_StorageClass_tok);
_LL1145:;} static char _temp1154[ 18u]="TypeSpecifier_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1154, _temp1154, _temp1154 + 18u}}; void* Cyc_yyget_TypeSpecifier_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1155= yy1; void*
_temp1161; _LL1157: if((*(( struct _xtunion_struct*) _temp1155)).tag == Cyc_TypeSpecifier_tok_tag){
_LL1162: _temp1161=(( struct Cyc_TypeSpecifier_tok_struct*) _temp1155)->f1; goto
_LL1158;} else{ goto _LL1159;} _LL1159: goto _LL1160; _LL1158: return _temp1161;
_LL1160:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TypeSpecifier_tok);
_LL1156:;} static char _temp1165[ 18u]="StructOrUnion_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1165, _temp1165, _temp1165 + 18u}}; void* Cyc_yyget_StructOrUnion_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1166= yy1; void*
_temp1172; _LL1168: if((*(( struct _xtunion_struct*) _temp1166)).tag == Cyc_StructOrUnion_tok_tag){
_LL1173: _temp1172=(( struct Cyc_StructOrUnion_tok_struct*) _temp1166)->f1; goto
_LL1169;} else{ goto _LL1170;} _LL1170: goto _LL1171; _LL1169: return _temp1172;
_LL1171:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_StructOrUnion_tok);
_LL1167:;} static char _temp1176[ 13u]="TypeQual_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1176, _temp1176, _temp1176 + 13u}}; struct Cyc_Absyn_Tqual* Cyc_yyget_TypeQual_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1177= yy1; struct Cyc_Absyn_Tqual*
_temp1183; _LL1179: if((*(( struct _xtunion_struct*) _temp1177)).tag == Cyc_TypeQual_tok_tag){
_LL1184: _temp1183=(( struct Cyc_TypeQual_tok_struct*) _temp1177)->f1; goto
_LL1180;} else{ goto _LL1181;} _LL1181: goto _LL1182; _LL1180: return _temp1183;
_LL1182:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TypeQual_tok);
_LL1178:;} static char _temp1187[ 24u]="StructFieldDeclList_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1187, _temp1187, _temp1187 + 24u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclList_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1188= yy1; struct Cyc_List_List* _temp1194; _LL1190: if((*((
struct _xtunion_struct*) _temp1188)).tag == Cyc_StructFieldDeclList_tok_tag){
_LL1195: _temp1194=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp1188)->f1;
goto _LL1191;} else{ goto _LL1192;} _LL1192: goto _LL1193; _LL1191: return
_temp1194; _LL1193:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL1189:;} static char _temp1198[ 28u]="StructFieldDeclListList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclListList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1198, _temp1198, _temp1198 + 28u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclListList_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1199= yy1; struct Cyc_List_List* _temp1205; _LL1201: if((*((
struct _xtunion_struct*) _temp1199)).tag == Cyc_StructFieldDeclListList_tok_tag){
_LL1206: _temp1205=(( struct Cyc_StructFieldDeclListList_tok_struct*) _temp1199)->f1;
goto _LL1202;} else{ goto _LL1203;} _LL1203: goto _LL1204; _LL1202: return
_temp1205; _LL1204:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL1200:;} static char _temp1209[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1209, _temp1209, _temp1209 + 21u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1210= yy1; struct Cyc_List_List*
_temp1216; _LL1212: if((*(( struct _xtunion_struct*) _temp1210)).tag == Cyc_TypeModifierList_tok_tag){
_LL1217: _temp1216=(( struct Cyc_TypeModifierList_tok_struct*) _temp1210)->f1;
goto _LL1213;} else{ goto _LL1214;} _LL1214: goto _LL1215; _LL1213: return
_temp1216; _LL1215:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TypeModifierList_tok);
_LL1211:;} static char _temp1220[ 8u]="Rgn_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgn_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1220,
_temp1220, _temp1220 + 8u}}; void* Cyc_yyget_Rgn_tok( struct _xtunion_struct*
yy1){ struct _xtunion_struct* _temp1221= yy1; void* _temp1227; _LL1223: if((*((
struct _xtunion_struct*) _temp1221)).tag == Cyc_Rgn_tok_tag){ _LL1228: _temp1227=((
struct Cyc_Rgn_tok_struct*) _temp1221)->f1; goto _LL1224;} else{ goto _LL1225;}
_LL1225: goto _LL1226; _LL1224: return _temp1227; _LL1226:( void) _throw((
struct _xtunion_struct*)& Cyc_yyfail_Rgn_tok); _LL1222:;} static char _temp1231[
15u]="Declarator_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1231, _temp1231, _temp1231 +
15u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp1232= yy1; struct Cyc_Parse_Declarator*
_temp1238; _LL1234: if((*(( struct _xtunion_struct*) _temp1232)).tag == Cyc_Declarator_tok_tag){
_LL1239: _temp1238=(( struct Cyc_Declarator_tok_struct*) _temp1232)->f1; goto
_LL1235;} else{ goto _LL1236;} _LL1236: goto _LL1237; _LL1235: return _temp1238;
_LL1237:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Declarator_tok);
_LL1233:;} static char _temp1242[ 21u]="DeclaratorExpopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1242, _temp1242, _temp1242 + 21u}}; struct _tuple15* Cyc_yyget_DeclaratorExpopt_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1243= yy1; struct
_tuple15* _temp1249; _LL1245: if((*(( struct _xtunion_struct*) _temp1243)).tag
== Cyc_DeclaratorExpopt_tok_tag){ _LL1250: _temp1249=(( struct Cyc_DeclaratorExpopt_tok_struct*)
_temp1243)->f1; goto _LL1246;} else{ goto _LL1247;} _LL1247: goto _LL1248;
_LL1246: return _temp1249; _LL1248:( void) _throw(( struct _xtunion_struct*)&
Cyc_yyfail_DeclaratorExpopt_tok); _LL1244:;} static char _temp1253[ 25u]="DeclaratorExpoptList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1253, _temp1253, _temp1253 + 25u}}; struct Cyc_List_List*
Cyc_yyget_DeclaratorExpoptList_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1254= yy1; struct Cyc_List_List* _temp1260; _LL1256: if((*((
struct _xtunion_struct*) _temp1254)).tag == Cyc_DeclaratorExpoptList_tok_tag){
_LL1261: _temp1260=(( struct Cyc_DeclaratorExpoptList_tok_struct*) _temp1254)->f1;
goto _LL1257;} else{ goto _LL1258;} _LL1258: goto _LL1259; _LL1257: return
_temp1260; _LL1259:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_DeclaratorExpoptList_tok);
_LL1255:;} static char _temp1264[ 23u]="AbstractDeclarator_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1264, _temp1264, _temp1264 + 23u}}; struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_AbstractDeclarator_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1265= yy1; struct Cyc_Parse_Abstractdeclarator* _temp1271;
_LL1267: if((*(( struct _xtunion_struct*) _temp1265)).tag == Cyc_AbstractDeclarator_tok_tag){
_LL1272: _temp1271=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1265)->f1;
goto _LL1268;} else{ goto _LL1269;} _LL1269: goto _LL1270; _LL1268: return
_temp1271; _LL1270:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL1266:;} static char _temp1275[ 16u]="TunionField_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1275, _temp1275, _temp1275 + 16u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1276= yy1; struct Cyc_Absyn_Tunionfield*
_temp1282; _LL1278: if((*(( struct _xtunion_struct*) _temp1276)).tag == Cyc_TunionField_tok_tag){
_LL1283: _temp1282=(( struct Cyc_TunionField_tok_struct*) _temp1276)->f1; goto
_LL1279;} else{ goto _LL1280;} _LL1280: goto _LL1281; _LL1279: return _temp1282;
_LL1281:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TunionField_tok);
_LL1277:;} static char _temp1286[ 20u]="TunionFieldList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionFieldList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1286, _temp1286, _temp1286 + 20u}}; struct Cyc_List_List* Cyc_yyget_TunionFieldList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1287= yy1; struct Cyc_List_List*
_temp1293; _LL1289: if((*(( struct _xtunion_struct*) _temp1287)).tag == Cyc_TunionFieldList_tok_tag){
_LL1294: _temp1293=(( struct Cyc_TunionFieldList_tok_struct*) _temp1287)->f1;
goto _LL1290;} else{ goto _LL1291;} _LL1291: goto _LL1292; _LL1290: return
_temp1293; _LL1292:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TunionFieldList_tok);
_LL1288:;} static char _temp1297[ 17u]="QualSpecList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1297, _temp1297, _temp1297 + 17u}}; struct _tuple14* Cyc_yyget_QualSpecList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1298= yy1; struct
_tuple14* _temp1304; _LL1300: if((*(( struct _xtunion_struct*) _temp1298)).tag
== Cyc_QualSpecList_tok_tag){ _LL1305: _temp1304=(( struct Cyc_QualSpecList_tok_struct*)
_temp1298)->f1; goto _LL1301;} else{ goto _LL1302;} _LL1302: goto _LL1303;
_LL1301: return _temp1304; _LL1303:( void) _throw(( struct _xtunion_struct*)&
Cyc_yyfail_QualSpecList_tok); _LL1299:;} static char _temp1308[ 11u]="IdList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1308, _temp1308, _temp1308 + 11u}}; struct Cyc_List_List*
Cyc_yyget_IdList_tok( struct _xtunion_struct* yy1){ struct _xtunion_struct*
_temp1309= yy1; struct Cyc_List_List* _temp1315; _LL1311: if((*(( struct
_xtunion_struct*) _temp1309)).tag == Cyc_IdList_tok_tag){ _LL1316: _temp1315=((
struct Cyc_IdList_tok_struct*) _temp1309)->f1; goto _LL1312;} else{ goto _LL1313;}
_LL1313: goto _LL1314; _LL1312: return _temp1315; _LL1314:( void) _throw((
struct _xtunion_struct*)& Cyc_yyfail_IdList_tok); _LL1310:;} static char
_temp1319[ 14u]="ParamDecl_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1319, _temp1319, _temp1319 +
14u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok( struct _xtunion_struct* yy1){
struct _xtunion_struct* _temp1320= yy1; struct _tuple2* _temp1326; _LL1322: if((*((
struct _xtunion_struct*) _temp1320)).tag == Cyc_ParamDecl_tok_tag){ _LL1327:
_temp1326=(( struct Cyc_ParamDecl_tok_struct*) _temp1320)->f1; goto _LL1323;}
else{ goto _LL1324;} _LL1324: goto _LL1325; _LL1323: return _temp1326; _LL1325:(
void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_ParamDecl_tok); _LL1321:;}
static char _temp1330[ 18u]="ParamDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1330, _temp1330, _temp1330 + 18u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1331= yy1; struct Cyc_List_List*
_temp1337; _LL1333: if((*(( struct _xtunion_struct*) _temp1331)).tag == Cyc_ParamDeclList_tok_tag){
_LL1338: _temp1337=(( struct Cyc_ParamDeclList_tok_struct*) _temp1331)->f1; goto
_LL1334;} else{ goto _LL1335;} _LL1335: goto _LL1336; _LL1334: return _temp1337;
_LL1336:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_ParamDeclList_tok);
_LL1332:;} static char _temp1341[ 22u]="ParamDeclListBool_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1341, _temp1341, _temp1341 + 22u}}; struct _tuple16*
Cyc_yyget_ParamDeclListBool_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1342= yy1; struct _tuple16* _temp1348; _LL1344: if((*((
struct _xtunion_struct*) _temp1342)).tag == Cyc_ParamDeclListBool_tok_tag){
_LL1349: _temp1348=(( struct Cyc_ParamDeclListBool_tok_struct*) _temp1342)->f1;
goto _LL1345;} else{ goto _LL1346;} _LL1346: goto _LL1347; _LL1345: return
_temp1348; _LL1347:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_ParamDeclListBool_tok);
_LL1343:;} static char _temp1352[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1352, _temp1352, _temp1352 + 13u}}; struct Cyc_List_List* Cyc_yyget_TypeList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1353= yy1; struct Cyc_List_List*
_temp1359; _LL1355: if((*(( struct _xtunion_struct*) _temp1353)).tag == Cyc_TypeList_tok_tag){
_LL1360: _temp1359=(( struct Cyc_TypeList_tok_struct*) _temp1353)->f1; goto
_LL1356;} else{ goto _LL1357;} _LL1357: goto _LL1358; _LL1356: return _temp1359;
_LL1358:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TypeList_tok);
_LL1354:;} static char _temp1363[ 19u]="DesignatorList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1363, _temp1363, _temp1363 + 19u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1364= yy1; struct Cyc_List_List*
_temp1370; _LL1366: if((*(( struct _xtunion_struct*) _temp1364)).tag == Cyc_DesignatorList_tok_tag){
_LL1371: _temp1370=(( struct Cyc_DesignatorList_tok_struct*) _temp1364)->f1;
goto _LL1367;} else{ goto _LL1368;} _LL1368: goto _LL1369; _LL1367: return
_temp1370; _LL1369:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_DesignatorList_tok);
_LL1365:;} static char _temp1374[ 15u]="Designator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Designator_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1374, _temp1374, _temp1374 + 15u}}; void* Cyc_yyget_Designator_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp1375= yy1; void* _temp1381;
_LL1377: if((*(( struct _xtunion_struct*) _temp1375)).tag == Cyc_Designator_tok_tag){
_LL1382: _temp1381=(( struct Cyc_Designator_tok_struct*) _temp1375)->f1; goto
_LL1378;} else{ goto _LL1379;} _LL1379: goto _LL1380; _LL1378: return _temp1381;
_LL1380:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Designator_tok);
_LL1376:;} static char _temp1385[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1385,
_temp1385, _temp1385 + 9u}}; void* Cyc_yyget_Kind_tok( struct _xtunion_struct*
yy1){ struct _xtunion_struct* _temp1386= yy1; void* _temp1392; _LL1388: if((*((
struct _xtunion_struct*) _temp1386)).tag == Cyc_Kind_tok_tag){ _LL1393:
_temp1392=(( struct Cyc_Kind_tok_struct*) _temp1386)->f1; goto _LL1389;} else{
goto _LL1390;} _LL1390: goto _LL1391; _LL1389: return _temp1392; _LL1391:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_Kind_tok); _LL1387:;} static char
_temp1396[ 9u]="Type_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1396, _temp1396, _temp1396 +
9u}}; void* Cyc_yyget_Type_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1397= yy1; void* _temp1403; _LL1399: if((*(( struct
_xtunion_struct*) _temp1397)).tag == Cyc_Type_tok_tag){ _LL1404: _temp1403=((
struct Cyc_Type_tok_struct*) _temp1397)->f1; goto _LL1400;} else{ goto _LL1401;}
_LL1401: goto _LL1402; _LL1400: return _temp1403; _LL1402:( void) _throw((
struct _xtunion_struct*)& Cyc_yyfail_Type_tok); _LL1398:;} static char _temp1407[
18u]="AttributeList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1407, _temp1407, _temp1407 +
18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok( struct _xtunion_struct*
yy1){ struct _xtunion_struct* _temp1408= yy1; struct Cyc_List_List* _temp1414;
_LL1410: if((*(( struct _xtunion_struct*) _temp1408)).tag == Cyc_AttributeList_tok_tag){
_LL1415: _temp1414=(( struct Cyc_AttributeList_tok_struct*) _temp1408)->f1; goto
_LL1411;} else{ goto _LL1412;} _LL1412: goto _LL1413; _LL1411: return _temp1414;
_LL1413:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_AttributeList_tok);
_LL1409:;} static char _temp1418[ 14u]="Attribute_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1418, _temp1418, _temp1418 + 14u}}; void* Cyc_yyget_Attribute_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp1419= yy1; void* _temp1425;
_LL1421: if((*(( struct _xtunion_struct*) _temp1419)).tag == Cyc_Attribute_tok_tag){
_LL1426: _temp1425=(( struct Cyc_Attribute_tok_struct*) _temp1419)->f1; goto
_LL1422;} else{ goto _LL1423;} _LL1423: goto _LL1424; _LL1422: return _temp1425;
_LL1424:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Attribute_tok);
_LL1420:;} static char _temp1429[ 14u]="Enumfield_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1429, _temp1429, _temp1429 + 14u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1430= yy1; struct Cyc_Absyn_Enumfield*
_temp1436; _LL1432: if((*(( struct _xtunion_struct*) _temp1430)).tag == Cyc_Enumfield_tok_tag){
_LL1437: _temp1436=(( struct Cyc_Enumfield_tok_struct*) _temp1430)->f1; goto
_LL1433;} else{ goto _LL1434;} _LL1434: goto _LL1435; _LL1433: return _temp1436;
_LL1435:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Enumfield_tok);
_LL1431:;} static char _temp1440[ 18u]="EnumfieldList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1440, _temp1440, _temp1440 + 18u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1441= yy1; struct Cyc_List_List*
_temp1447; _LL1443: if((*(( struct _xtunion_struct*) _temp1441)).tag == Cyc_EnumfieldList_tok_tag){
_LL1448: _temp1447=(( struct Cyc_EnumfieldList_tok_struct*) _temp1441)->f1; goto
_LL1444;} else{ goto _LL1445;} _LL1445: goto _LL1446; _LL1444: return _temp1447;
_LL1446:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_EnumfieldList_tok);
_LL1442:;} struct Cyc_Yyltype{ int timestamp; int first_line; int first_column;
int last_line; int last_column; struct _tagged_string text; } ; typedef struct
Cyc_Yyltype Cyc_yyltype; struct Cyc_Yyltype Cyc_yynewloc(){ return({ struct Cyc_Yyltype
_temp1449; _temp1449.timestamp= 0; _temp1449.first_line= 0; _temp1449.first_column=
0; _temp1449.last_line= 0; _temp1449.last_column= 0; _temp1449.text=( struct
_tagged_string)({ char* _temp1450=( char*)""; struct _tagged_string _temp1451;
_temp1451.curr= _temp1450; _temp1451.base= _temp1450; _temp1451.last_plus_one=
_temp1450 + 1; _temp1451;}); _temp1449;});} static char _temp1454[ 1u]="";
struct Cyc_Yyltype Cyc_yylloc=( struct Cyc_Yyltype){.timestamp= 0,.first_line= 0,.first_column=
0,.last_line= 0,.last_column= 0,.text=( struct _tagged_string){ _temp1454,
_temp1454, _temp1454 + 1u}}; static short Cyc_yytranslate[ 352u]={ (short)0,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)123, (short)2, (short)2, (short)105, (short)121, (short)117, (short)2,
(short)101, (short)102, (short)112, (short)115, (short)103, (short)116, (short)109,
(short)120, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)108, (short)97, (short)106, (short)100,
(short)107, (short)114, (short)113, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)110, (short)2,
(short)111, (short)119, (short)104, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)98,
(short)118, (short)99, (short)122, (short)2, (short)2, (short)2, (short)2,
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
(short)89, (short)90, (short)91, (short)92, (short)93, (short)94, (short)95,
(short)96}; static char _temp1457[ 2u]="$"; static char _temp1460[ 6u]="error";
static char _temp1463[ 12u]="$undefined."; static char _temp1466[ 5u]="AUTO";
static char _temp1469[ 9u]="REGISTER"; static char _temp1472[ 7u]="STATIC";
static char _temp1475[ 7u]="EXTERN"; static char _temp1478[ 8u]="TYPEDEF";
static char _temp1481[ 5u]="VOID"; static char _temp1484[ 5u]="CHAR"; static
char _temp1487[ 6u]="SHORT"; static char _temp1490[ 4u]="INT"; static char
_temp1493[ 5u]="LONG"; static char _temp1496[ 6u]="FLOAT"; static char _temp1499[
7u]="DOUBLE"; static char _temp1502[ 7u]="SIGNED"; static char _temp1505[ 9u]="UNSIGNED";
static char _temp1508[ 6u]="CONST"; static char _temp1511[ 9u]="VOLATILE";
static char _temp1514[ 9u]="RESTRICT"; static char _temp1517[ 7u]="STRUCT";
static char _temp1520[ 6u]="UNION"; static char _temp1523[ 5u]="CASE"; static
char _temp1526[ 8u]="DEFAULT"; static char _temp1529[ 7u]="INLINE"; static char
_temp1532[ 3u]="IF"; static char _temp1535[ 5u]="ELSE"; static char _temp1538[ 7u]="SWITCH";
static char _temp1541[ 6u]="WHILE"; static char _temp1544[ 3u]="DO"; static char
_temp1547[ 4u]="FOR"; static char _temp1550[ 5u]="GOTO"; static char _temp1553[
9u]="CONTINUE"; static char _temp1556[ 6u]="BREAK"; static char _temp1559[ 7u]="RETURN";
static char _temp1562[ 7u]="SIZEOF"; static char _temp1565[ 5u]="ENUM"; static
char _temp1568[ 8u]="NULL_kw"; static char _temp1571[ 4u]="LET"; static char
_temp1574[ 6u]="THROW"; static char _temp1577[ 4u]="TRY"; static char _temp1580[
6u]="CATCH"; static char _temp1583[ 4u]="NEW"; static char _temp1586[ 9u]="ABSTRACT";
static char _temp1589[ 9u]="FALLTHRU"; static char _temp1592[ 6u]="USING";
static char _temp1595[ 10u]="NAMESPACE"; static char _temp1598[ 7u]="TUNION";
static char _temp1601[ 8u]="XTUNION"; static char _temp1604[ 5u]="FILL"; static
char _temp1607[ 8u]="CODEGEN"; static char _temp1610[ 4u]="CUT"; static char
_temp1613[ 7u]="SPLICE"; static char _temp1616[ 7u]="PRINTF"; static char
_temp1619[ 8u]="FPRINTF"; static char _temp1622[ 8u]="XPRINTF"; static char
_temp1625[ 6u]="SCANF"; static char _temp1628[ 7u]="FSCANF"; static char
_temp1631[ 7u]="SSCANF"; static char _temp1634[ 7u]="MALLOC"; static char
_temp1637[ 9u]="REGION_T"; static char _temp1640[ 7u]="REGION"; static char
_temp1643[ 5u]="RNEW"; static char _temp1646[ 8u]="RMALLOC"; static char
_temp1649[ 7u]="PTR_OP"; static char _temp1652[ 7u]="INC_OP"; static char
_temp1655[ 7u]="DEC_OP"; static char _temp1658[ 8u]="LEFT_OP"; static char
_temp1661[ 9u]="RIGHT_OP"; static char _temp1664[ 6u]="LE_OP"; static char
_temp1667[ 6u]="GE_OP"; static char _temp1670[ 6u]="EQ_OP"; static char
_temp1673[ 6u]="NE_OP"; static char _temp1676[ 7u]="AND_OP"; static char
_temp1679[ 6u]="OR_OP"; static char _temp1682[ 11u]="MUL_ASSIGN"; static char
_temp1685[ 11u]="DIV_ASSIGN"; static char _temp1688[ 11u]="MOD_ASSIGN"; static
char _temp1691[ 11u]="ADD_ASSIGN"; static char _temp1694[ 11u]="SUB_ASSIGN";
static char _temp1697[ 12u]="LEFT_ASSIGN"; static char _temp1700[ 13u]="RIGHT_ASSIGN";
static char _temp1703[ 11u]="AND_ASSIGN"; static char _temp1706[ 11u]="XOR_ASSIGN";
static char _temp1709[ 10u]="OR_ASSIGN"; static char _temp1712[ 9u]="ELLIPSIS";
static char _temp1715[ 11u]="LEFT_RIGHT"; static char _temp1718[ 12u]="COLON_COLON";
static char _temp1721[ 11u]="IDENTIFIER"; static char _temp1724[ 17u]="INTEGER_CONSTANT";
static char _temp1727[ 7u]="STRING"; static char _temp1730[ 19u]="CHARACTER_CONSTANT";
static char _temp1733[ 18u]="FLOATING_CONSTANT"; static char _temp1736[ 9u]="TYPE_VAR";
static char _temp1739[ 16u]="QUAL_IDENTIFIER"; static char _temp1742[ 18u]="QUAL_TYPEDEF_NAME";
static char _temp1745[ 10u]="ATTRIBUTE"; static char _temp1748[ 4u]="';'";
static char _temp1751[ 4u]="'{'"; static char _temp1754[ 4u]="'}'"; static char
_temp1757[ 4u]="'='"; static char _temp1760[ 4u]="'('"; static char _temp1763[ 4u]="')'";
static char _temp1766[ 4u]="','"; static char _temp1769[ 4u]="'_'"; static char
_temp1772[ 4u]="'$'"; static char _temp1775[ 4u]="'<'"; static char _temp1778[ 4u]="'>'";
static char _temp1781[ 4u]="':'"; static char _temp1784[ 4u]="'.'"; static char
_temp1787[ 4u]="'['"; static char _temp1790[ 4u]="']'"; static char _temp1793[ 4u]="'*'";
static char _temp1796[ 4u]="'@'"; static char _temp1799[ 4u]="'?'"; static char
_temp1802[ 4u]="'+'"; static char _temp1805[ 4u]="'-'"; static char _temp1808[ 4u]="'&'";
static char _temp1811[ 4u]="'|'"; static char _temp1814[ 4u]="'^'"; static char
_temp1817[ 4u]="'/'"; static char _temp1820[ 4u]="'%'"; static char _temp1823[ 4u]="'~'";
static char _temp1826[ 4u]="'!'"; static char _temp1829[ 5u]="prog"; static char
_temp1832[ 17u]="translation_unit"; static char _temp1835[ 21u]="external_declaration";
static char _temp1838[ 20u]="function_definition"; static char _temp1841[ 21u]="function_definition2";
static char _temp1844[ 13u]="using_action"; static char _temp1847[ 15u]="unusing_action";
static char _temp1850[ 17u]="namespace_action"; static char _temp1853[ 19u]="unnamespace_action";
static char _temp1856[ 12u]="declaration"; static char _temp1859[ 17u]="declaration_list";
static char _temp1862[ 23u]="declaration_specifiers"; static char _temp1865[ 24u]="storage_class_specifier";
static char _temp1868[ 15u]="attributes_opt"; static char _temp1871[ 11u]="attributes";
static char _temp1874[ 15u]="attribute_list"; static char _temp1877[ 10u]="attribute";
static char _temp1880[ 15u]="type_specifier"; static char _temp1883[ 5u]="kind";
static char _temp1886[ 15u]="type_qualifier"; static char _temp1889[ 15u]="enum_specifier";
static char _temp1892[ 11u]="enum_field"; static char _temp1895[ 22u]="enum_declaration_list";
static char _temp1898[ 26u]="struct_or_union_specifier"; static char _temp1901[
16u]="type_params_opt"; static char _temp1904[ 16u]="struct_or_union"; static
char _temp1907[ 24u]="struct_declaration_list"; static char _temp1910[ 25u]="struct_declaration_list0";
static char _temp1913[ 21u]="init_declarator_list"; static char _temp1916[ 22u]="init_declarator_list0";
static char _temp1919[ 16u]="init_declarator"; static char _temp1922[ 19u]="struct_declaration";
static char _temp1925[ 25u]="specifier_qualifier_list"; static char _temp1928[
23u]="struct_declarator_list"; static char _temp1931[ 24u]="struct_declarator_list0";
static char _temp1934[ 18u]="struct_declarator"; static char _temp1937[ 17u]="tunion_specifier";
static char _temp1940[ 17u]="tunionfield_list"; static char _temp1943[ 12u]="tunionfield";
static char _temp1946[ 11u]="declarator"; static char _temp1949[ 18u]="direct_declarator";
static char _temp1952[ 8u]="pointer"; static char _temp1955[ 13u]="pointer_char";
static char _temp1958[ 8u]="rgn_opt"; static char _temp1961[ 4u]="rgn"; static
char _temp1964[ 20u]="type_qualifier_list"; static char _temp1967[ 20u]="parameter_type_list";
static char _temp1970[ 11u]="effect_set"; static char _temp1973[ 14u]="atomic_effect";
static char _temp1976[ 11u]="region_set"; static char _temp1979[ 15u]="parameter_list";
static char _temp1982[ 22u]="parameter_declaration"; static char _temp1985[ 16u]="identifier_list";
static char _temp1988[ 17u]="identifier_list0"; static char _temp1991[ 12u]="initializer";
static char _temp1994[ 18u]="array_initializer"; static char _temp1997[ 17u]="initializer_list";
static char _temp2000[ 12u]="designation"; static char _temp2003[ 16u]="designator_list";
static char _temp2006[ 11u]="designator"; static char _temp2009[ 10u]="type_name";
static char _temp2012[ 14u]="any_type_name"; static char _temp2015[ 15u]="type_name_list";
static char _temp2018[ 20u]="abstract_declarator"; static char _temp2021[ 27u]="direct_abstract_declarator";
static char _temp2024[ 10u]="statement"; static char _temp2027[ 18u]="labeled_statement";
static char _temp2030[ 21u]="expression_statement"; static char _temp2033[ 19u]="compound_statement";
static char _temp2036[ 16u]="block_item_list"; static char _temp2039[ 11u]="block_item";
static char _temp2042[ 20u]="selection_statement"; static char _temp2045[ 15u]="switch_clauses";
static char _temp2048[ 20u]="iteration_statement"; static char _temp2051[ 15u]="jump_statement";
static char _temp2054[ 8u]="pattern"; static char _temp2057[ 19u]="tuple_pattern_list";
static char _temp2060[ 20u]="tuple_pattern_list0"; static char _temp2063[ 14u]="field_pattern";
static char _temp2066[ 19u]="field_pattern_list"; static char _temp2069[ 20u]="field_pattern_list0";
static char _temp2072[ 11u]="expression"; static char _temp2075[ 22u]="assignment_expression";
static char _temp2078[ 20u]="assignment_operator"; static char _temp2081[ 23u]="conditional_expression";
static char _temp2084[ 20u]="constant_expression"; static char _temp2087[ 22u]="logical_or_expression";
static char _temp2090[ 23u]="logical_and_expression"; static char _temp2093[ 24u]="inclusive_or_expression";
static char _temp2096[ 24u]="exclusive_or_expression"; static char _temp2099[ 15u]="and_expression";
static char _temp2102[ 20u]="equality_expression"; static char _temp2105[ 22u]="relational_expression";
static char _temp2108[ 17u]="shift_expression"; static char _temp2111[ 20u]="additive_expression";
static char _temp2114[ 26u]="multiplicative_expression"; static char _temp2117[
16u]="cast_expression"; static char _temp2120[ 17u]="unary_expression"; static
char _temp2123[ 14u]="format_primop"; static char _temp2126[ 15u]="unary_operator";
static char _temp2129[ 19u]="postfix_expression"; static char _temp2132[ 19u]="primary_expression";
static char _temp2135[ 25u]="argument_expression_list"; static char _temp2138[
26u]="argument_expression_list0"; static char _temp2141[ 9u]="constant"; static
char _temp2144[ 20u]="qual_opt_identifier"; static struct _tagged_string Cyc_yytname[
230u]={( struct _tagged_string){ _temp1457, _temp1457, _temp1457 + 2u},( struct
_tagged_string){ _temp1460, _temp1460, _temp1460 + 6u},( struct _tagged_string){
_temp1463, _temp1463, _temp1463 + 12u},( struct _tagged_string){ _temp1466,
_temp1466, _temp1466 + 5u},( struct _tagged_string){ _temp1469, _temp1469,
_temp1469 + 9u},( struct _tagged_string){ _temp1472, _temp1472, _temp1472 + 7u},(
struct _tagged_string){ _temp1475, _temp1475, _temp1475 + 7u},( struct
_tagged_string){ _temp1478, _temp1478, _temp1478 + 8u},( struct _tagged_string){
_temp1481, _temp1481, _temp1481 + 5u},( struct _tagged_string){ _temp1484,
_temp1484, _temp1484 + 5u},( struct _tagged_string){ _temp1487, _temp1487,
_temp1487 + 6u},( struct _tagged_string){ _temp1490, _temp1490, _temp1490 + 4u},(
struct _tagged_string){ _temp1493, _temp1493, _temp1493 + 5u},( struct
_tagged_string){ _temp1496, _temp1496, _temp1496 + 6u},( struct _tagged_string){
_temp1499, _temp1499, _temp1499 + 7u},( struct _tagged_string){ _temp1502,
_temp1502, _temp1502 + 7u},( struct _tagged_string){ _temp1505, _temp1505,
_temp1505 + 9u},( struct _tagged_string){ _temp1508, _temp1508, _temp1508 + 6u},(
struct _tagged_string){ _temp1511, _temp1511, _temp1511 + 9u},( struct
_tagged_string){ _temp1514, _temp1514, _temp1514 + 9u},( struct _tagged_string){
_temp1517, _temp1517, _temp1517 + 7u},( struct _tagged_string){ _temp1520,
_temp1520, _temp1520 + 6u},( struct _tagged_string){ _temp1523, _temp1523,
_temp1523 + 5u},( struct _tagged_string){ _temp1526, _temp1526, _temp1526 + 8u},(
struct _tagged_string){ _temp1529, _temp1529, _temp1529 + 7u},( struct
_tagged_string){ _temp1532, _temp1532, _temp1532 + 3u},( struct _tagged_string){
_temp1535, _temp1535, _temp1535 + 5u},( struct _tagged_string){ _temp1538,
_temp1538, _temp1538 + 7u},( struct _tagged_string){ _temp1541, _temp1541,
_temp1541 + 6u},( struct _tagged_string){ _temp1544, _temp1544, _temp1544 + 3u},(
struct _tagged_string){ _temp1547, _temp1547, _temp1547 + 4u},( struct
_tagged_string){ _temp1550, _temp1550, _temp1550 + 5u},( struct _tagged_string){
_temp1553, _temp1553, _temp1553 + 9u},( struct _tagged_string){ _temp1556,
_temp1556, _temp1556 + 6u},( struct _tagged_string){ _temp1559, _temp1559,
_temp1559 + 7u},( struct _tagged_string){ _temp1562, _temp1562, _temp1562 + 7u},(
struct _tagged_string){ _temp1565, _temp1565, _temp1565 + 5u},( struct
_tagged_string){ _temp1568, _temp1568, _temp1568 + 8u},( struct _tagged_string){
_temp1571, _temp1571, _temp1571 + 4u},( struct _tagged_string){ _temp1574,
_temp1574, _temp1574 + 6u},( struct _tagged_string){ _temp1577, _temp1577,
_temp1577 + 4u},( struct _tagged_string){ _temp1580, _temp1580, _temp1580 + 6u},(
struct _tagged_string){ _temp1583, _temp1583, _temp1583 + 4u},( struct
_tagged_string){ _temp1586, _temp1586, _temp1586 + 9u},( struct _tagged_string){
_temp1589, _temp1589, _temp1589 + 9u},( struct _tagged_string){ _temp1592,
_temp1592, _temp1592 + 6u},( struct _tagged_string){ _temp1595, _temp1595,
_temp1595 + 10u},( struct _tagged_string){ _temp1598, _temp1598, _temp1598 + 7u},(
struct _tagged_string){ _temp1601, _temp1601, _temp1601 + 8u},( struct
_tagged_string){ _temp1604, _temp1604, _temp1604 + 5u},( struct _tagged_string){
_temp1607, _temp1607, _temp1607 + 8u},( struct _tagged_string){ _temp1610,
_temp1610, _temp1610 + 4u},( struct _tagged_string){ _temp1613, _temp1613,
_temp1613 + 7u},( struct _tagged_string){ _temp1616, _temp1616, _temp1616 + 7u},(
struct _tagged_string){ _temp1619, _temp1619, _temp1619 + 8u},( struct
_tagged_string){ _temp1622, _temp1622, _temp1622 + 8u},( struct _tagged_string){
_temp1625, _temp1625, _temp1625 + 6u},( struct _tagged_string){ _temp1628,
_temp1628, _temp1628 + 7u},( struct _tagged_string){ _temp1631, _temp1631,
_temp1631 + 7u},( struct _tagged_string){ _temp1634, _temp1634, _temp1634 + 7u},(
struct _tagged_string){ _temp1637, _temp1637, _temp1637 + 9u},( struct
_tagged_string){ _temp1640, _temp1640, _temp1640 + 7u},( struct _tagged_string){
_temp1643, _temp1643, _temp1643 + 5u},( struct _tagged_string){ _temp1646,
_temp1646, _temp1646 + 8u},( struct _tagged_string){ _temp1649, _temp1649,
_temp1649 + 7u},( struct _tagged_string){ _temp1652, _temp1652, _temp1652 + 7u},(
struct _tagged_string){ _temp1655, _temp1655, _temp1655 + 7u},( struct
_tagged_string){ _temp1658, _temp1658, _temp1658 + 8u},( struct _tagged_string){
_temp1661, _temp1661, _temp1661 + 9u},( struct _tagged_string){ _temp1664,
_temp1664, _temp1664 + 6u},( struct _tagged_string){ _temp1667, _temp1667,
_temp1667 + 6u},( struct _tagged_string){ _temp1670, _temp1670, _temp1670 + 6u},(
struct _tagged_string){ _temp1673, _temp1673, _temp1673 + 6u},( struct
_tagged_string){ _temp1676, _temp1676, _temp1676 + 7u},( struct _tagged_string){
_temp1679, _temp1679, _temp1679 + 6u},( struct _tagged_string){ _temp1682,
_temp1682, _temp1682 + 11u},( struct _tagged_string){ _temp1685, _temp1685,
_temp1685 + 11u},( struct _tagged_string){ _temp1688, _temp1688, _temp1688 + 11u},(
struct _tagged_string){ _temp1691, _temp1691, _temp1691 + 11u},( struct
_tagged_string){ _temp1694, _temp1694, _temp1694 + 11u},( struct _tagged_string){
_temp1697, _temp1697, _temp1697 + 12u},( struct _tagged_string){ _temp1700,
_temp1700, _temp1700 + 13u},( struct _tagged_string){ _temp1703, _temp1703,
_temp1703 + 11u},( struct _tagged_string){ _temp1706, _temp1706, _temp1706 + 11u},(
struct _tagged_string){ _temp1709, _temp1709, _temp1709 + 10u},( struct
_tagged_string){ _temp1712, _temp1712, _temp1712 + 9u},( struct _tagged_string){
_temp1715, _temp1715, _temp1715 + 11u},( struct _tagged_string){ _temp1718,
_temp1718, _temp1718 + 12u},( struct _tagged_string){ _temp1721, _temp1721,
_temp1721 + 11u},( struct _tagged_string){ _temp1724, _temp1724, _temp1724 + 17u},(
struct _tagged_string){ _temp1727, _temp1727, _temp1727 + 7u},( struct
_tagged_string){ _temp1730, _temp1730, _temp1730 + 19u},( struct _tagged_string){
_temp1733, _temp1733, _temp1733 + 18u},( struct _tagged_string){ _temp1736,
_temp1736, _temp1736 + 9u},( struct _tagged_string){ _temp1739, _temp1739,
_temp1739 + 16u},( struct _tagged_string){ _temp1742, _temp1742, _temp1742 + 18u},(
struct _tagged_string){ _temp1745, _temp1745, _temp1745 + 10u},( struct
_tagged_string){ _temp1748, _temp1748, _temp1748 + 4u},( struct _tagged_string){
_temp1751, _temp1751, _temp1751 + 4u},( struct _tagged_string){ _temp1754,
_temp1754, _temp1754 + 4u},( struct _tagged_string){ _temp1757, _temp1757,
_temp1757 + 4u},( struct _tagged_string){ _temp1760, _temp1760, _temp1760 + 4u},(
struct _tagged_string){ _temp1763, _temp1763, _temp1763 + 4u},( struct
_tagged_string){ _temp1766, _temp1766, _temp1766 + 4u},( struct _tagged_string){
_temp1769, _temp1769, _temp1769 + 4u},( struct _tagged_string){ _temp1772,
_temp1772, _temp1772 + 4u},( struct _tagged_string){ _temp1775, _temp1775,
_temp1775 + 4u},( struct _tagged_string){ _temp1778, _temp1778, _temp1778 + 4u},(
struct _tagged_string){ _temp1781, _temp1781, _temp1781 + 4u},( struct
_tagged_string){ _temp1784, _temp1784, _temp1784 + 4u},( struct _tagged_string){
_temp1787, _temp1787, _temp1787 + 4u},( struct _tagged_string){ _temp1790,
_temp1790, _temp1790 + 4u},( struct _tagged_string){ _temp1793, _temp1793,
_temp1793 + 4u},( struct _tagged_string){ _temp1796, _temp1796, _temp1796 + 4u},(
struct _tagged_string){ _temp1799, _temp1799, _temp1799 + 4u},( struct
_tagged_string){ _temp1802, _temp1802, _temp1802 + 4u},( struct _tagged_string){
_temp1805, _temp1805, _temp1805 + 4u},( struct _tagged_string){ _temp1808,
_temp1808, _temp1808 + 4u},( struct _tagged_string){ _temp1811, _temp1811,
_temp1811 + 4u},( struct _tagged_string){ _temp1814, _temp1814, _temp1814 + 4u},(
struct _tagged_string){ _temp1817, _temp1817, _temp1817 + 4u},( struct
_tagged_string){ _temp1820, _temp1820, _temp1820 + 4u},( struct _tagged_string){
_temp1823, _temp1823, _temp1823 + 4u},( struct _tagged_string){ _temp1826,
_temp1826, _temp1826 + 4u},( struct _tagged_string){ _temp1829, _temp1829,
_temp1829 + 5u},( struct _tagged_string){ _temp1832, _temp1832, _temp1832 + 17u},(
struct _tagged_string){ _temp1835, _temp1835, _temp1835 + 21u},( struct
_tagged_string){ _temp1838, _temp1838, _temp1838 + 20u},( struct _tagged_string){
_temp1841, _temp1841, _temp1841 + 21u},( struct _tagged_string){ _temp1844,
_temp1844, _temp1844 + 13u},( struct _tagged_string){ _temp1847, _temp1847,
_temp1847 + 15u},( struct _tagged_string){ _temp1850, _temp1850, _temp1850 + 17u},(
struct _tagged_string){ _temp1853, _temp1853, _temp1853 + 19u},( struct
_tagged_string){ _temp1856, _temp1856, _temp1856 + 12u},( struct _tagged_string){
_temp1859, _temp1859, _temp1859 + 17u},( struct _tagged_string){ _temp1862,
_temp1862, _temp1862 + 23u},( struct _tagged_string){ _temp1865, _temp1865,
_temp1865 + 24u},( struct _tagged_string){ _temp1868, _temp1868, _temp1868 + 15u},(
struct _tagged_string){ _temp1871, _temp1871, _temp1871 + 11u},( struct
_tagged_string){ _temp1874, _temp1874, _temp1874 + 15u},( struct _tagged_string){
_temp1877, _temp1877, _temp1877 + 10u},( struct _tagged_string){ _temp1880,
_temp1880, _temp1880 + 15u},( struct _tagged_string){ _temp1883, _temp1883,
_temp1883 + 5u},( struct _tagged_string){ _temp1886, _temp1886, _temp1886 + 15u},(
struct _tagged_string){ _temp1889, _temp1889, _temp1889 + 15u},( struct
_tagged_string){ _temp1892, _temp1892, _temp1892 + 11u},( struct _tagged_string){
_temp1895, _temp1895, _temp1895 + 22u},( struct _tagged_string){ _temp1898,
_temp1898, _temp1898 + 26u},( struct _tagged_string){ _temp1901, _temp1901,
_temp1901 + 16u},( struct _tagged_string){ _temp1904, _temp1904, _temp1904 + 16u},(
struct _tagged_string){ _temp1907, _temp1907, _temp1907 + 24u},( struct
_tagged_string){ _temp1910, _temp1910, _temp1910 + 25u},( struct _tagged_string){
_temp1913, _temp1913, _temp1913 + 21u},( struct _tagged_string){ _temp1916,
_temp1916, _temp1916 + 22u},( struct _tagged_string){ _temp1919, _temp1919,
_temp1919 + 16u},( struct _tagged_string){ _temp1922, _temp1922, _temp1922 + 19u},(
struct _tagged_string){ _temp1925, _temp1925, _temp1925 + 25u},( struct
_tagged_string){ _temp1928, _temp1928, _temp1928 + 23u},( struct _tagged_string){
_temp1931, _temp1931, _temp1931 + 24u},( struct _tagged_string){ _temp1934,
_temp1934, _temp1934 + 18u},( struct _tagged_string){ _temp1937, _temp1937,
_temp1937 + 17u},( struct _tagged_string){ _temp1940, _temp1940, _temp1940 + 17u},(
struct _tagged_string){ _temp1943, _temp1943, _temp1943 + 12u},( struct
_tagged_string){ _temp1946, _temp1946, _temp1946 + 11u},( struct _tagged_string){
_temp1949, _temp1949, _temp1949 + 18u},( struct _tagged_string){ _temp1952,
_temp1952, _temp1952 + 8u},( struct _tagged_string){ _temp1955, _temp1955,
_temp1955 + 13u},( struct _tagged_string){ _temp1958, _temp1958, _temp1958 + 8u},(
struct _tagged_string){ _temp1961, _temp1961, _temp1961 + 4u},( struct
_tagged_string){ _temp1964, _temp1964, _temp1964 + 20u},( struct _tagged_string){
_temp1967, _temp1967, _temp1967 + 20u},( struct _tagged_string){ _temp1970,
_temp1970, _temp1970 + 11u},( struct _tagged_string){ _temp1973, _temp1973,
_temp1973 + 14u},( struct _tagged_string){ _temp1976, _temp1976, _temp1976 + 11u},(
struct _tagged_string){ _temp1979, _temp1979, _temp1979 + 15u},( struct
_tagged_string){ _temp1982, _temp1982, _temp1982 + 22u},( struct _tagged_string){
_temp1985, _temp1985, _temp1985 + 16u},( struct _tagged_string){ _temp1988,
_temp1988, _temp1988 + 17u},( struct _tagged_string){ _temp1991, _temp1991,
_temp1991 + 12u},( struct _tagged_string){ _temp1994, _temp1994, _temp1994 + 18u},(
struct _tagged_string){ _temp1997, _temp1997, _temp1997 + 17u},( struct
_tagged_string){ _temp2000, _temp2000, _temp2000 + 12u},( struct _tagged_string){
_temp2003, _temp2003, _temp2003 + 16u},( struct _tagged_string){ _temp2006,
_temp2006, _temp2006 + 11u},( struct _tagged_string){ _temp2009, _temp2009,
_temp2009 + 10u},( struct _tagged_string){ _temp2012, _temp2012, _temp2012 + 14u},(
struct _tagged_string){ _temp2015, _temp2015, _temp2015 + 15u},( struct
_tagged_string){ _temp2018, _temp2018, _temp2018 + 20u},( struct _tagged_string){
_temp2021, _temp2021, _temp2021 + 27u},( struct _tagged_string){ _temp2024,
_temp2024, _temp2024 + 10u},( struct _tagged_string){ _temp2027, _temp2027,
_temp2027 + 18u},( struct _tagged_string){ _temp2030, _temp2030, _temp2030 + 21u},(
struct _tagged_string){ _temp2033, _temp2033, _temp2033 + 19u},( struct
_tagged_string){ _temp2036, _temp2036, _temp2036 + 16u},( struct _tagged_string){
_temp2039, _temp2039, _temp2039 + 11u},( struct _tagged_string){ _temp2042,
_temp2042, _temp2042 + 20u},( struct _tagged_string){ _temp2045, _temp2045,
_temp2045 + 15u},( struct _tagged_string){ _temp2048, _temp2048, _temp2048 + 20u},(
struct _tagged_string){ _temp2051, _temp2051, _temp2051 + 15u},( struct
_tagged_string){ _temp2054, _temp2054, _temp2054 + 8u},( struct _tagged_string){
_temp2057, _temp2057, _temp2057 + 19u},( struct _tagged_string){ _temp2060,
_temp2060, _temp2060 + 20u},( struct _tagged_string){ _temp2063, _temp2063,
_temp2063 + 14u},( struct _tagged_string){ _temp2066, _temp2066, _temp2066 + 19u},(
struct _tagged_string){ _temp2069, _temp2069, _temp2069 + 20u},( struct
_tagged_string){ _temp2072, _temp2072, _temp2072 + 11u},( struct _tagged_string){
_temp2075, _temp2075, _temp2075 + 22u},( struct _tagged_string){ _temp2078,
_temp2078, _temp2078 + 20u},( struct _tagged_string){ _temp2081, _temp2081,
_temp2081 + 23u},( struct _tagged_string){ _temp2084, _temp2084, _temp2084 + 20u},(
struct _tagged_string){ _temp2087, _temp2087, _temp2087 + 22u},( struct
_tagged_string){ _temp2090, _temp2090, _temp2090 + 23u},( struct _tagged_string){
_temp2093, _temp2093, _temp2093 + 24u},( struct _tagged_string){ _temp2096,
_temp2096, _temp2096 + 24u},( struct _tagged_string){ _temp2099, _temp2099,
_temp2099 + 15u},( struct _tagged_string){ _temp2102, _temp2102, _temp2102 + 20u},(
struct _tagged_string){ _temp2105, _temp2105, _temp2105 + 22u},( struct
_tagged_string){ _temp2108, _temp2108, _temp2108 + 17u},( struct _tagged_string){
_temp2111, _temp2111, _temp2111 + 20u},( struct _tagged_string){ _temp2114,
_temp2114, _temp2114 + 26u},( struct _tagged_string){ _temp2117, _temp2117,
_temp2117 + 16u},( struct _tagged_string){ _temp2120, _temp2120, _temp2120 + 17u},(
struct _tagged_string){ _temp2123, _temp2123, _temp2123 + 14u},( struct
_tagged_string){ _temp2126, _temp2126, _temp2126 + 15u},( struct _tagged_string){
_temp2129, _temp2129, _temp2129 + 19u},( struct _tagged_string){ _temp2132,
_temp2132, _temp2132 + 19u},( struct _tagged_string){ _temp2135, _temp2135,
_temp2135 + 25u},( struct _tagged_string){ _temp2138, _temp2138, _temp2138 + 26u},(
struct _tagged_string){ _temp2141, _temp2141, _temp2141 + 9u},( struct
_tagged_string){ _temp2144, _temp2144, _temp2144 + 20u}}; static short Cyc_yyr1[
393u]={ (short)0, (short)124, (short)125, (short)125, (short)125, (short)125,
(short)125, (short)125, (short)125, (short)126, (short)126, (short)127, (short)127,
(short)127, (short)127, (short)128, (short)128, (short)129, (short)130, (short)131,
(short)132, (short)133, (short)133, (short)133, (short)134, (short)134, (short)135,
(short)135, (short)135, (short)135, (short)135, (short)135, (short)135, (short)135,
(short)136, (short)136, (short)136, (short)136, (short)136, (short)136, (short)136,
(short)137, (short)137, (short)138, (short)139, (short)139, (short)140, (short)140,
(short)140, (short)140, (short)141, (short)141, (short)141, (short)141, (short)141,
(short)141, (short)141, (short)141, (short)141, (short)141, (short)141, (short)141,
(short)141, (short)141, (short)141, (short)141, (short)141, (short)141, (short)142,
(short)142, (short)143, (short)143, (short)143, (short)144, (short)144, (short)145,
(short)145, (short)146, (short)146, (short)147, (short)147, (short)147, (short)147,
(short)147, (short)148, (short)148, (short)148, (short)149, (short)149, (short)150,
(short)151, (short)151, (short)152, (short)153, (short)153, (short)154, (short)154,
(short)155, (short)156, (short)156, (short)156, (short)156, (short)157, (short)158,
(short)158, (short)159, (short)159, (short)159, (short)160, (short)160, (short)160,
(short)160, (short)160, (short)160, (short)160, (short)160, (short)161, (short)161,
(short)161, (short)161, (short)162, (short)162, (short)163, (short)163, (short)164,
(short)164, (short)164, (short)164, (short)164, (short)164, (short)164, (short)164,
(short)164, (short)164, (short)164, (short)165, (short)165, (short)165, (short)165,
(short)166, (short)166, (short)166, (short)166, (short)166, (short)167, (short)167,
(short)168, (short)168, (short)168, (short)169, (short)169, (short)170, (short)170,
(short)170, (short)171, (short)171, (short)172, (short)172, (short)172, (short)172,
(short)173, (short)173, (short)173, (short)173, (short)174, (short)174, (short)175,
(short)175, (short)175, (short)176, (short)177, (short)177, (short)178, (short)178,
(short)179, (short)179, (short)179, (short)179, (short)180, (short)180, (short)180,
(short)180, (short)181, (short)182, (short)182, (short)183, (short)183, (short)184,
(short)184, (short)185, (short)185, (short)185, (short)185, (short)186, (short)186,
(short)187, (short)187, (short)187, (short)188, (short)188, (short)188, (short)188,
(short)188, (short)188, (short)188, (short)188, (short)188, (short)188, (short)188,
(short)188, (short)188, (short)188, (short)189, (short)189, (short)189, (short)189,
(short)189, (short)189, (short)189, (short)189, (short)189, (short)189, (short)190,
(short)191, (short)191, (short)192, (short)192, (short)193, (short)193, (short)194,
(short)194, (short)194, (short)195, (short)195, (short)195, (short)195, (short)196,
(short)196, (short)196, (short)196, (short)196, (short)196, (short)197, (short)197,
(short)197, (short)197, (short)197, (short)197, (short)197, (short)197, (short)197,
(short)197, (short)197, (short)197, (short)197, (short)197, (short)198, (short)198,
(short)198, (short)198, (short)198, (short)198, (short)198, (short)198, (short)199,
(short)199, (short)199, (short)199, (short)199, (short)199, (short)199, (short)199,
(short)199, (short)199, (short)199, (short)199, (short)199, (short)199, (short)200,
(short)200, (short)201, (short)201, (short)202, (short)202, (short)203, (short)204,
(short)204, (short)205, (short)205, (short)206, (short)206, (short)207, (short)207,
(short)207, (short)207, (short)207, (short)207, (short)207, (short)207, (short)207,
(short)207, (short)207, (short)208, (short)208, (short)208, (short)208, (short)208,
(short)208, (short)208, (short)209, (short)210, (short)210, (short)211, (short)211,
(short)212, (short)212, (short)213, (short)213, (short)214, (short)214, (short)215,
(short)215, (short)215, (short)216, (short)216, (short)216, (short)216, (short)216,
(short)217, (short)217, (short)217, (short)218, (short)218, (short)218, (short)219,
(short)219, (short)219, (short)219, (short)220, (short)220, (short)221, (short)221,
(short)221, (short)221, (short)221, (short)221, (short)221, (short)221, (short)221,
(short)221, (short)221, (short)221, (short)222, (short)222, (short)222, (short)222,
(short)222, (short)222, (short)223, (short)223, (short)223, (short)224, (short)224,
(short)224, (short)224, (short)224, (short)224, (short)224, (short)224, (short)224,
(short)224, (short)224, (short)224, (short)224, (short)224, (short)225, (short)225,
(short)225, (short)225, (short)225, (short)225, (short)225, (short)225, (short)225,
(short)226, (short)227, (short)227, (short)228, (short)228, (short)228, (short)228,
(short)229, (short)229}; static short Cyc_yyr2[ 393u]={ (short)0, (short)1,
(short)2, (short)3, (short)5, (short)3, (short)5, (short)6, (short)0, (short)1,
(short)1, (short)2, (short)3, (short)3, (short)4, (short)3, (short)4, (short)2,
(short)1, (short)2, (short)1, (short)2, (short)3, (short)5, (short)1, (short)2,
(short)2, (short)3, (short)2, (short)3, (short)2, (short)3, (short)2, (short)3,
(short)1, (short)1, (short)1, (short)1, (short)2, (short)1, (short)1, (short)0,
(short)1, (short)6, (short)1, (short)3, (short)1, (short)1, (short)4, (short)4,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)3, (short)2,
(short)4, (short)4, (short)1, (short)1, (short)1, (short)1, (short)1, (short)5,
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
(short)6, (short)3, (short)2, (short)2, (short)3, (short)1, (short)2, (short)2,
(short)3, (short)1, (short)2, (short)1, (short)1, (short)1, (short)5, (short)7,
(short)7, (short)6, (short)0, (short)3, (short)4, (short)5, (short)6, (short)7,
(short)5, (short)7, (short)6, (short)7, (short)7, (short)8, (short)7, (short)8,
(short)8, (short)9, (short)6, (short)7, (short)7, (short)8, (short)3, (short)2,
(short)2, (short)2, (short)3, (short)2, (short)4, (short)5, (short)1, (short)3,
(short)1, (short)2, (short)1, (short)1, (short)1, (short)1, (short)5, (short)4,
(short)4, (short)5, (short)2, (short)2, (short)0, (short)1, (short)1, (short)3,
(short)1, (short)2, (short)1, (short)1, (short)3, (short)1, (short)3, (short)1,
(short)3, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)5, (short)2, (short)2,
(short)2, (short)5, (short)5, (short)1, (short)1, (short)3, (short)1, (short)3,
(short)1, (short)3, (short)1, (short)3, (short)1, (short)3, (short)1, (short)3,
(short)3, (short)1, (short)3, (short)3, (short)3, (short)3, (short)1, (short)3,
(short)3, (short)1, (short)3, (short)3, (short)1, (short)3, (short)3, (short)3,
(short)1, (short)4, (short)1, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)4, (short)2, (short)4, (short)7, (short)9, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)4, (short)3, (short)4, (short)3, (short)3, (short)3, (short)3, (short)2,
(short)2, (short)6, (short)7, (short)4, (short)4, (short)1, (short)1, (short)1,
(short)3, (short)2, (short)5, (short)4, (short)4, (short)5, (short)1, (short)1,
(short)3, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1}; static
short Cyc_yydefact[ 755u]={ (short)8, (short)34, (short)35, (short)36, (short)37,
(short)39, (short)50, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, (short)70, (short)71, (short)72, (short)87,
(short)88, (short)41, (short)0, (short)0, (short)40, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)391, (short)63, (short)392, (short)84, (short)0,
(short)51, (short)0, (short)139, (short)140, (short)143, (short)1, (short)8,
(short)9, (short)0, (short)0, (short)10, (short)0, (short)41, (short)41, (short)41,
(short)60, (short)61, (short)0, (short)62, (short)0, (short)122, (short)0,
(short)144, (short)124, (short)38, (short)0, (short)32, (short)42, (short)74,
(short)270, (short)266, (short)269, (short)268, (short)0, (short)264, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)271, (short)17, (short)19,
(short)146, (short)148, (short)0, (short)84, (short)0, (short)113, (short)0,
(short)0, (short)0, (short)65, (short)0, (short)0, (short)0, (short)0, (short)2,
(short)8, (short)8, (short)8, (short)8, (short)21, (short)0, (short)92, (short)93,
(short)95, (short)26, (short)28, (short)30, (short)84, (short)0, (short)84,
(short)37, (short)0, (short)24, (short)0, (short)0, (short)11, (short)0, (short)0,
(short)0, (short)134, (short)123, (short)41, (short)145, (short)8, (short)0,
(short)33, (short)0, (short)0, (short)278, (short)277, (short)267, (short)276,
(short)0, (short)0, (short)0, (short)84, (short)0, (short)109, (short)114,
(short)0, (short)0, (short)0, (short)41, (short)41, (short)187, (short)189,
(short)0, (short)68, (short)69, (short)64, (short)193, (short)0, (short)125,
(short)167, (short)0, (short)164, (short)0, (short)390, (short)0, (short)0,
(short)0, (short)0, (short)352, (short)353, (short)354, (short)355, (short)356,
(short)357, (short)0, (short)0, (short)0, (short)0, (short)0, (short)387,
(short)377, (short)388, (short)389, (short)0, (short)0, (short)0, (short)0,
(short)360, (short)0, (short)358, (short)359, (short)0, (short)289, (short)302,
(short)310, (short)312, (short)314, (short)316, (short)318, (short)320, (short)323,
(short)328, (short)331, (short)334, (short)338, (short)0, (short)0, (short)340,
(short)361, (short)376, (short)375, (short)0, (short)3, (short)0, (short)5,
(short)0, (short)22, (short)0, (short)0, (short)0, (short)12, (short)27, (short)29,
(short)31, (short)83, (short)0, (short)89, (short)90, (short)0, (short)82,
(short)38, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)391,
(short)223, (short)225, (short)0, (short)231, (short)229, (short)0, (short)230,
(short)212, (short)213, (short)214, (short)0, (short)227, (short)215, (short)216,
(short)217, (short)0, (short)287, (short)25, (short)13, (short)95, (short)170,
(short)0, (short)129, (short)0, (short)151, (short)0, (short)169, (short)0,
(short)126, (short)0, (short)135, (short)0, (short)0, (short)77, (short)0,
(short)75, (short)265, (short)280, (short)0, (short)279, (short)0, (short)0,
(short)278, (short)147, (short)110, (short)84, (short)0, (short)0, (short)116,
(short)120, (short)115, (short)160, (short)190, (short)0, (short)98, (short)100,
(short)0, (short)0, (short)195, (short)188, (short)196, (short)67, (short)0,
(short)86, (short)0, (short)85, (short)0, (short)166, (short)195, (short)168,
(short)66, (short)0, (short)0, (short)348, (short)304, (short)338, (short)0,
(short)305, (short)306, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)341, (short)342, (short)0, (short)0, (short)0, (short)0,
(short)344, (short)345, (short)343, (short)141, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)292, (short)293, (short)294, (short)295, (short)296, (short)297, (short)298,
(short)299, (short)300, (short)301, (short)291, (short)0, (short)0, (short)346,
(short)0, (short)369, (short)370, (short)0, (short)0, (short)0, (short)379,
(short)0, (short)0, (short)142, (short)18, (short)8, (short)20, (short)8,
(short)94, (short)96, (short)173, (short)172, (short)14, (short)0, (short)79,
(short)91, (short)0, (short)0, (short)102, (short)103, (short)105, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)257, (short)258,
(short)259, (short)0, (short)0, (short)261, (short)0, (short)220, (short)221,
(short)0, (short)0, (short)0, (short)0, (short)95, (short)226, (short)228,
(short)224, (short)0, (short)158, (short)0, (short)0, (short)154, (short)128,
(short)0, (short)0, (short)131, (short)0, (short)133, (short)132, (short)127,
(short)149, (short)137, (short)136, (short)8, (short)47, (short)46, (short)0,
(short)44, (short)0, (short)73, (short)0, (short)273, (short)0, (short)23,
(short)274, (short)0, (short)0, (short)0, (short)0, (short)183, (short)282,
(short)285, (short)0, (short)284, (short)0, (short)111, (short)0, (short)112,
(short)117, (short)0, (short)0, (short)0, (short)0, (short)191, (short)99,
(short)101, (short)0, (short)203, (short)0, (short)0, (short)199, (short)0,
(short)197, (short)0, (short)0, (short)0, (short)211, (short)192, (short)194,
(short)165, (short)0, (short)0, (short)174, (short)178, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)378, (short)385, (short)0, (short)384, (short)311, (short)0,
(short)313, (short)315, (short)317, (short)319, (short)321, (short)322, (short)326,
(short)327, (short)324, (short)325, (short)329, (short)330, (short)332, (short)333,
(short)335, (short)336, (short)337, (short)290, (short)0, (short)367, (short)368,
(short)363, (short)0, (short)365, (short)366, (short)0, (short)0, (short)0,
(short)4, (short)6, (short)0, (short)309, (short)106, (short)97, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)256, (short)260, (short)0, (short)0, (short)0, (short)219,
(short)0, (short)222, (short)0, (short)15, (short)288, (short)0, (short)156,
(short)0, (short)130, (short)0, (short)153, (short)152, (short)171, (short)150,
(short)138, (short)7, (short)0, (short)0, (short)0, (short)78, (short)76,
(short)281, (short)186, (short)0, (short)283, (short)182, (short)184, (short)275,
(short)0, (short)272, (short)108, (short)119, (short)118, (short)0, (short)162,
(short)161, (short)0, (short)205, (short)198, (short)201, (short)0, (short)206,
(short)0, (short)0, (short)200, (short)0, (short)347, (short)0, (short)175,
(short)0, (short)179, (short)373, (short)374, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)339, (short)381, (short)0,
(short)0, (short)349, (short)364, (short)362, (short)382, (short)0, (short)0,
(short)81, (short)104, (short)107, (short)80, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)236, (short)262,
(short)0, (short)0, (short)16, (short)159, (short)157, (short)155, (short)0,
(short)0, (short)43, (short)45, (short)185, (short)286, (short)0, (short)0,
(short)204, (short)0, (short)208, (short)210, (short)209, (short)202, (short)0,
(short)176, (short)180, (short)0, (short)0, (short)307, (short)308, (short)0,
(short)383, (short)0, (short)386, (short)303, (short)380, (short)232, (short)236,
(short)242, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)263, (short)0, (short)48,
(short)49, (short)121, (short)163, (short)207, (short)0, (short)181, (short)0,
(short)0, (short)371, (short)0, (short)0, (short)0, (short)0, (short)244,
(short)0, (short)0, (short)0, (short)252, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)235, (short)218, (short)0,
(short)350, (short)0, (short)372, (short)233, (short)234, (short)243, (short)245,
(short)246, (short)0, (short)254, (short)253, (short)0, (short)248, (short)0,
(short)0, (short)0, (short)0, (short)236, (short)237, (short)0, (short)0,
(short)247, (short)255, (short)249, (short)250, (short)0, (short)0, (short)236,
(short)238, (short)177, (short)351, (short)251, (short)236, (short)239, (short)236,
(short)240, (short)241, (short)0, (short)0, (short)0}; static short Cyc_yydefgoto[
106u]={ (short)752, (short)39, (short)40, (short)41, (short)240, (short)42,
(short)376, (short)43, (short)378, (short)44, (short)210, (short)45, (short)46,
(short)60, (short)61, (short)435, (short)436, (short)47, (short)146, (short)48,
(short)49, (short)270, (short)271, (short)50, (short)86, (short)51, (short)216,
(short)217, (short)97, (short)98, (short)99, (short)218, (short)141, (short)388,
(short)389, (short)390, (short)52, (short)284, (short)285, (short)53, (short)54,
(short)55, (short)56, (short)118, (short)79, (short)431, (short)467, (short)419,
(short)420, (short)290, (short)261, (short)152, (short)262, (short)263, (short)482,
(short)381, (short)483, (short)484, (short)447, (short)448, (short)142, (short)147,
(short)148, (short)468, (short)297, (short)243, (short)244, (short)245, (short)246,
(short)247, (short)248, (short)249, (short)682, (short)250, (short)251, (short)274,
(short)275, (short)276, (short)450, (short)451, (short)452, (short)252, (short)253,
(short)362, (short)183, (short)532, (short)184, (short)185, (short)186, (short)187,
(short)188, (short)189, (short)190, (short)191, (short)192, (short)193, (short)194,
(short)195, (short)196, (short)197, (short)198, (short)199, (short)496, (short)497,
(short)200, (short)201}; static short Cyc_yypact[ 755u]={ (short)1548, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)69, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)10, (short)148,
(short)1997, - (short)-32768, (short)148, - (short)35, (short)118, (short)118,
(short)26, - (short)-32768, (short)67, - (short)-32768, (short)33, (short)474, -
(short)-32768, (short)72, (short)89, (short)127, - (short)-32768, - (short)-32768,
(short)1548, - (short)-32768, (short)143, (short)256, - (short)-32768, (short)307,
- (short)10, - (short)10, - (short)10, - (short)-32768, - (short)-32768, (short)311,
- (short)-32768, (short)1846, (short)166, (short)34, - (short)5, - (short)-32768,
(short)163, (short)162, (short)1906, - (short)-32768, (short)176, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)1997, - (short)-32768,
(short)191, (short)214, (short)224, (short)1997, (short)221, (short)120, -
(short)-32768, - (short)-32768, (short)320, - (short)-32768, (short)148, (short)266,
(short)148, (short)44, (short)2158, - (short)4, (short)2158, - (short)-32768,
(short)310, (short)2225, (short)3736, (short)3736, - (short)-32768, (short)1548,
(short)1548, (short)1548, (short)1548, - (short)-32768, (short)327, (short)328,
- (short)-32768, (short)1783, (short)1906, (short)1906, (short)1906, (short)33,
(short)2225, (short)33, (short)393, (short)953, - (short)-32768, (short)1846,
(short)307, - (short)-32768, (short)2046, (short)2158, (short)2600, - (short)-32768,
(short)166, - (short)10, - (short)-32768, (short)1548, (short)335, - (short)-32768,
(short)148, (short)354, (short)1997, - (short)-32768, - (short)-32768, - (short)-32768,
(short)3736, (short)363, - (short)4, (short)33, (short)148, (short)389, -
(short)-32768, (short)148, (short)148, (short)145, - (short)10, - (short)10,
(short)187, - (short)-32768, (short)18, - (short)-32768, - (short)-32768, -
(short)-32768, (short)374, - (short)3, - (short)-32768, (short)226, (short)282,
- (short)-32768, (short)3949, - (short)-32768, (short)3736, (short)3807, (short)391,
(short)394, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)396, (short)399, (short)401, (short)4020,
(short)4020, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)1311, (short)412, (short)4091, (short)4091, - (short)-32768, (short)4091,
- (short)-32768, - (short)-32768, (short)418, - (short)-32768, - (short)31,
(short)446, (short)423, (short)402, (short)422, (short)343, (short)123, (short)411,
(short)366, (short)161, - (short)-32768, (short)626, (short)443, (short)4091,
(short)136, - (short)-32768, - (short)-32768, (short)16, (short)448, - (short)-32768,
(short)450, - (short)-32768, (short)461, - (short)-32768, (short)474, (short)2671,
(short)1846, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)467, (short)468, (short)2225, - (short)-32768, (short)424, (short)476, -
(short)-32768, (short)496, (short)499, (short)502, (short)1930, (short)503,
(short)488, (short)508, (short)512, (short)2742, (short)1930, (short)233,
(short)1930, (short)1930, - (short)52, (short)505, - (short)-32768, - (short)-32768,
(short)509, - (short)-32768, - (short)-32768, (short)307, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)517, (short)1074, -
(short)-32768, - (short)-32768, - (short)-32768, (short)156, - (short)-32768, -
(short)-32768, - (short)-32768, (short)511, - (short)-32768, (short)10, -
(short)-32768, (short)516, (short)193, (short)518, (short)519, (short)37, -
(short)-32768, (short)510, (short)58, (short)520, (short)13, (short)521, (short)524,
(short)525, - (short)-32768, - (short)-32768, (short)528, (short)523, (short)212,
(short)454, (short)1997, - (short)-32768, - (short)-32768, (short)33, (short)148,
(short)532, (short)258, (short)211, - (short)-32768, (short)24, - (short)-32768,
(short)533, (short)2225, (short)2225, (short)1723, (short)2813, (short)103, -
(short)-32768, (short)384, - (short)-32768, (short)10, - (short)-32768, (short)2158,
- (short)-32768, (short)1663, - (short)-32768, (short)264, - (short)-32768, -
(short)-32768, (short)2225, (short)1311, - (short)-32768, - (short)-32768, -
(short)-32768, (short)2244, - (short)-32768, (short)559, (short)3736, (short)1608,
(short)599, (short)3736, (short)3736, (short)1311, - (short)-32768, - (short)-32768,
(short)1074, (short)535, (short)403, (short)3736, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)4091, (short)3736, (short)4091, (short)4091,
(short)4091, (short)4091, (short)4091, (short)4091, (short)4091, (short)4091,
(short)4091, (short)4091, (short)4091, (short)4091, (short)4091, (short)4091,
(short)4091, (short)4091, (short)4091, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)3736,
(short)3736, - (short)-32768, (short)102, - (short)-32768, - (short)-32768,
(short)2884, (short)203, (short)3736, - (short)-32768, (short)2511, (short)534,
- (short)-32768, - (short)-32768, (short)1548, - (short)-32768, (short)1548, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)2225, - (short)-32768, - (short)-32768, (short)3736, (short)544, (short)540,
- (short)-32768, (short)536, (short)2225, (short)3736, (short)3736, (short)3736,
(short)617, (short)1195, (short)549, - (short)-32768, - (short)-32768, - (short)-32768,
(short)259, (short)606, - (short)-32768, (short)2955, - (short)-32768, - (short)-32768,
(short)1930, (short)556, (short)1930, (short)1427, (short)1783, - (short)-32768,
- (short)-32768, - (short)-32768, (short)3736, (short)564, (short)290, (short)550,
(short)538, - (short)-32768, (short)10, (short)2172, - (short)-32768, (short)566,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)58, (short)1548, - (short)-32768, (short)554, (short)555, (short)557,
(short)148, - (short)-32768, (short)3736, - (short)-32768, (short)1997, -
(short)-32768, - (short)-32768, (short)571, (short)3736, (short)1997, (short)68,
- (short)-32768, - (short)-32768, - (short)-32768, (short)562, (short)560,
(short)565, - (short)-32768, (short)567, - (short)-32768, (short)148, (short)148,
(short)563, - (short)4, (short)575, - (short)-32768, - (short)-32768, - (short)-32768,
(short)10, - (short)-32768, (short)568, (short)570, - (short)-32768, (short)558,
(short)384, (short)2060, (short)2158, (short)3026, - (short)-32768, - (short)-32768,
(short)374, - (short)-32768, (short)572, (short)585, - (short)-32768, - (short)-32768,
(short)268, (short)2671, (short)406, (short)573, (short)474, (short)576, (short)408,
(short)577, (short)580, (short)587, (short)3878, - (short)-32768, - (short)-32768,
(short)588, (short)589, (short)446, (short)234, (short)423, (short)402, (short)422,
(short)343, (short)123, (short)123, (short)411, (short)411, (short)411, (short)411,
(short)366, (short)366, (short)161, (short)161, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)591, - (short)-32768, - (short)-32768,
- (short)-32768, (short)594, - (short)-32768, - (short)-32768, (short)55,
(short)340, (short)2158, - (short)-32768, - (short)-32768, (short)592, - (short)-32768,
- (short)-32768, - (short)-32768, (short)424, (short)3736, (short)598, (short)413,
(short)421, (short)431, (short)610, (short)3097, (short)3168, (short)300, -
(short)-32768, - (short)-32768, (short)601, (short)603, (short)611, - (short)-32768,
(short)607, - (short)-32768, (short)1846, - (short)-32768, - (short)-32768, -
(short)4, - (short)-32768, (short)616, - (short)-32768, (short)10, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)437, (short)619, (short)13, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)612, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)628, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)2225, (short)615, - (short)-32768, (short)637, - (short)-32768,
- (short)-32768, - (short)-32768, (short)10, - (short)-32768, (short)639,
(short)45, - (short)-32768, (short)631, (short)630, (short)624, - (short)-32768,
(short)2333, - (short)-32768, - (short)-32768, - (short)-32768, (short)1846,
(short)2225, (short)3807, (short)708, (short)630, (short)644, (short)2511, -
(short)-32768, - (short)-32768, (short)3736, (short)3736, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)2511, (short)341, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)1930,
(short)649, (short)1930, (short)3736, (short)3239, (short)313, (short)3310,
(short)314, (short)3381, (short)529, - (short)-32768, (short)652, (short)662, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)653,
(short)656, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)470, (short)575, - (short)-32768, (short)658, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)3736, - (short)-32768, - (short)-32768,
(short)2671, (short)659, - (short)-32768, (short)559, (short)650, - (short)-32768,
(short)346, - (short)-32768, - (short)-32768, - (short)-32768, (short)728,
(short)529, - (short)-32768, (short)475, (short)1930, (short)478, (short)3452,
(short)1930, (short)481, (short)3523, (short)3594, (short)325, (short)1997,
(short)654, (short)664, - (short)-32768, (short)1930, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)243, - (short)-32768,
(short)663, (short)2225, - (short)-32768, (short)2422, (short)1930, (short)665,
(short)670, - (short)-32768, (short)1930, (short)1930, (short)487, - (short)-32768,
(short)1930, (short)1930, (short)489, (short)1930, (short)491, (short)3665, -
(short)34, (short)1074, - (short)-32768, - (short)-32768, (short)3736, - (short)-32768,
(short)666, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)1930, - (short)-32768, - (short)-32768,
(short)1930, - (short)-32768, (short)1930, (short)1930, (short)493, (short)3736,
(short)820, - (short)-32768, (short)364, (short)667, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)1930, (short)279, (short)529, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)820, - (short)-32768,
(short)529, - (short)-32768, - (short)-32768, (short)770, (short)772, - (short)-32768};
static short Cyc_yypgoto[ 106u]={ - (short)-32768, (short)90, - (short)-32768,
(short)456, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)49, - (short)48, - (short)53, - (short)-32768, (short)117,
- (short)45, (short)206, - (short)-32768, (short)2, - (short)115, - (short)43, -
(short)-32768, - (short)-32768, (short)339, - (short)-32768, (short)43, -
(short)-32768, - (short)137, - (short)-32768, - (short)-32768, - (short)-32768,
(short)569, (short)593, - (short)76, - (short)-32768, - (short)-32768, (short)245,
- (short)-32768, - (short)250, - (short)-32768, - (short)14, - (short)37, -
(short)124, - (short)-32768, - (short)-32768, (short)54, - (short)-32768, -
(short)102, - (short)356, (short)483, - (short)394, - (short)85, - (short)273, -
(short)-32768, - (short)-32768, - (short)165, - (short)150, - (short)358, -
(short)218, - (short)-32768, (short)336, - (short)154, - (short)68, - (short)104,
(short)73, - (short)149, (short)52, - (short)-32768, - (short)-32768, - (short)21,
- (short)246, - (short)-32768, - (short)-32768, - (short)551, - (short)-32768, -
(short)-32768, - (short)9, (short)506, - (short)-32768, (short)208, - (short)-32768,
- (short)-32768, (short)57, (short)30, - (short)-32768, - (short)117, - (short)347,
- (short)148, (short)452, (short)453, (short)455, (short)477, (short)479,
(short)218, (short)129, (short)257, (short)261, - (short)17, (short)459, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)299,
- (short)-32768, - (short)-32768, (short)1}; static short Cyc_yytable[ 4215u]={
(short)111, (short)57, (short)414, (short)151, (short)109, (short)110, (short)314,
(short)122, (short)315, (short)116, (short)264, (short)260, (short)150, (short)73,
(short)526, (short)143, (short)280, (short)295, (short)117, (short)87, (short)325,
(short)58, (short)62, (short)74, (short)557, (short)75, (short)305, (short)80,
(short)82, (short)219, (short)433, (short)100, (short)112, (short)455, (short)57,
(short)478, (short)408, (short)150, (short)311, (short)731, (short)140, (short)57,
(short)140, (short)332, (short)380, (short)140, (short)57, (short)111, (short)212,
(short)213, (short)214, (short)109, (short)106, (short)76, (short)409, (short)242,
(short)57, (short)111, (short)124, (short)241, (short)446, (short)254, (short)140,
(short)128, (short)518, (short)300, (short)560, (short)585, (short)74, (short)522,
(short)140, (short)140, (short)116, (short)74, (short)732, (short)15, (short)16,
(short)17, (short)492, (short)211, (short)132, (short)81, (short)135, (short)333,
(short)144, (short)139, (short)59, (short)139, (short)77, (short)255, (short)139,
(short)145, (short)570, (short)57, (short)57, (short)57, (short)57, (short)256,
(short)573, (short)78, (short)301, (short)434, (short)371, (short)417, (short)302,
(short)426, (short)548, (short)139, (short)418, (short)586, (short)119, (short)460,
(short)57, (short)652, (short)372, (short)139, (short)139, (short)130, (short)697,
(short)182, (short)202, (short)57, (short)29, (short)134, (short)272, (short)298,
(short)74, (short)461, (short)31, (short)373, (short)91, (short)140, (short)83,
(short)299, (short)282, (short)33, (short)304, (short)286, (short)287, (short)85,
(short)301, (short)219, (short)136, (short)430, (short)427, (short)266, (short)471,
(short)215, (short)301, (short)220, (short)478, (short)57, (short)653, (short)137,
(short)84, (short)479, (short)471, (short)111, (short)416, (short)328, (short)329,
(short)254, (short)330, (short)101, (short)102, (short)103, (short)616, (short)491,
(short)575, (short)295, (short)36, (short)37, (short)38, (short)88, (short)140,
(short)281, (short)139, (short)444, (short)445, (short)305, (short)364, (short)743,
(short)203, (short)204, (short)205, (short)206, (short)277, (short)89, (short)622,
(short)383, (short)519, (short)748, (short)340, (short)341, (short)256, (short)242,
(short)750, (short)520, (short)751, (short)241, (short)365, (short)366, (short)367,
(short)640, (short)293, (short)391, (short)29, (short)581, (short)582, (short)57,
(short)268, (short)77, (short)31, (short)294, (short)296, (short)463, (short)464,
(short)150, - (short)84, (short)139, (short)57, - (short)84, (short)78, (short)306,
(short)429, (short)90, (short)85, (short)150, (short)412, (short)342, (short)343,
(short)326, (short)150, (short)477, (short)650, (short)267, (short)29, (short)368,
(short)288, (short)382, (short)92, (short)93, (short)31, (short)57, (short)289,
(short)369, (short)370, (short)530, (short)140, (short)140, (short)140, (short)664,
(short)475, (short)415, (short)688, (short)536, (short)291, (short)292, (short)140,
(short)416, (short)140, (short)120, (short)59, (short)121, (short)487, (short)140,
(short)140, (short)113, (short)117, (short)449, (short)531, (short)242, (short)114,
(short)348, (short)123, (short)241, (short)115, (short)396, (short)140, (short)74,
(short)74, (short)349, (short)350, (short)403, (short)286, (short)406, (short)407,
(short)402, (short)293, (short)87, (short)422, (short)523, (short)125, (short)139,
(short)139, (short)139, (short)423, (short)294, (short)524, (short)36, (short)37,
(short)38, (short)126, (short)139, (short)57, (short)139, (short)57, (short)564,
(short)219, (short)442, (short)139, (short)139, - (short)84, (short)127, (short)29,
(short)416, (short)219, (short)85, (short)57, (short)600, (short)31, (short)129,
(short)531, (short)139, (short)470, (short)454, (short)151, (short)303, (short)531,
(short)459, (short)404, (short)514, (short)515, (short)516, (short)405, (short)150,
(short)294, (short)416, (short)36, (short)37, (short)38, (short)140, (short)613,
(short)382, (short)111, (short)584, (short)416, (short)150, (short)542, (short)140,
(short)490, (short)714, (short)29, (short)94, (short)95, (short)457, (short)545,
(short)495, (short)31, (short)111, (short)446, (short)458, (short)416, (short)109,
(short)552, (short)303, (short)326, (short)598, (short)140, (short)593, (short)592,
(short)599, (short)85, (short)485, (short)294, (short)133, (short)489, (short)57,
(short)326, (short)57, (short)140, (short)658, (short)416, (short)288, (short)307,
(short)308, (short)139, (short)747, (short)563, (short)556, (short)499, (short)553,
(short)517, (short)495, (short)139, (short)29, (short)150, (short)632, (short)495,
(short)29, (short)658, (short)31, (short)382, (short)416, (short)96, (short)31,
(short)104, (short)131, (short)33, (short)105, (short)674, (short)677, (short)149,
(short)139, (short)338, (short)339, (short)416, (short)416, (short)531, (short)36,
(short)37, (short)38, (short)709, (short)619, (short)207, (short)139, (short)475,
(short)525, (short)416, (short)140, (short)140, (short)208, (short)571, (short)57,
(short)657, (short)495, (short)269, (short)574, (short)272, (short)617, (short)638,
(short)495, (short)74, (short)618, (short)301, (short)694, (short)554, (short)74,
(short)667, (short)695, (short)537, (short)538, (short)539, (short)657, (short)543,
(short)660, (short)273, (short)661, (short)286, (short)286, (short)549, (short)278,
(short)551, (short)744, (short)279, (short)733, (short)528, (short)416, (short)529,
(short)506, (short)507, (short)508, (short)509, (short)603, (short)139, (short)139,
(short)610, (short)658, (short)344, (short)345, (short)59, (short)346, (short)347,
(short)221, (short)140, (short)472, (short)742, (short)283, (short)57, (short)299,
(short)473, (short)63, (short)316, (short)691, (short)474, (short)317, (short)666,
(short)318, (short)647, (short)111, (short)319, (short)749, (short)320, (short)254,
(short)595, (short)494, (short)416, (short)150, (short)601, (short)416, (short)605,
(short)416, (short)29, (short)327, (short)382, (short)624, (short)416, (short)331,
(short)31, (short)334, (short)391, (short)336, (short)565, (short)625, (short)416,
(short)33, (short)641, (short)642, (short)659, (short)139, (short)657, (short)637,
(short)387, (short)626, (short)416, (short)57, (short)36, (short)37, (short)38,
(short)337, (short)140, (short)335, (short)29, (short)64, (short)363, (short)65,
(short)66, (short)374, (short)31, (short)375, (short)111, (short)680, (short)681,
(short)443, (short)109, (short)67, (short)504, (short)505, (short)68, (short)69,
(short)377, (short)140, (short)29, (short)444, (short)445, (short)384, (short)70,
(short)385, (short)31, (short)449, (short)71, (short)72, (short)687, (short)308,
(short)392, (short)33, (short)398, (short)698, (short)416, (short)74, (short)700,
(short)416, (short)211, (short)704, (short)416, (short)139, (short)36, (short)37,
(short)38, (short)723, (short)416, (short)726, (short)416, (short)728, (short)416,
(short)740, (short)416, (short)393, (short)629, (short)631, (short)394, (short)510,
(short)511, (short)395, (short)397, (short)399, (short)139, (short)512, (short)513,
(short)400, (short)411, (short)209, (short)310, (short)410, (short)312, (short)312,
(short)413, (short)716, (short)421, (short)432, (short)424, (short)428, (short)425,
(short)438, (short)437, (short)439, (short)441, (short)322, (short)323, (short)382,
(short)440, (short)456, (short)462, (short)332, (short)488, (short)312, (short)312,
(short)493, (short)312, (short)382, (short)527, (short)533, (short)665, (short)534,
(short)535, (short)540, (short)544, (short)546, (short)382, (short)550, (short)140,
(short)555, (short)558, (short)559, (short)562, (short)566, (short)312, (short)567,
(short)242, (short)572, (short)568, (short)577, (short)241, (short)578, (short)583,
(short)63, (short)580, (short)579, (short)288, (short)589, (short)587, (short)710,
(short)588, (short)597, (short)596, (short)602, (short)668, (short)604, (short)670,
(short)242, (short)606, (short)74, (short)607, (short)241, (short)671, (short)673,
(short)608, (short)676, (short)382, (short)679, (short)611, (short)620, (short)612,
(short)614, (short)242, (short)139, (short)615, (short)623, (short)241, (short)633,
(short)634, (short)351, (short)352, (short)353, (short)354, (short)355, (short)356,
(short)357, (short)358, (short)359, (short)360, (short)627, (short)690, (short)635,
(short)636, (short)639, (short)29, (short)64, (short)648, (short)65, (short)66,
(short)643, (short)31, (short)645, (short)699, (short)382, (short)361, (short)703,
(short)609, (short)67, (short)655, (short)702, (short)68, (short)69, (short)706,
(short)708, (short)713, (short)444, (short)445, (short)649, (short)70, (short)651,
(short)654, (short)662, (short)71, (short)72, (short)663, (short)669, (short)718,
(short)683, (short)684, (short)693, (short)721, (short)722, (short)696, (short)685,
(short)724, (short)725, (short)686, (short)727, (short)689, (short)692, (short)711,
(short)712, (short)719, (short)715, (short)730, (short)720, (short)735, (short)745,
(short)753, (short)734, (short)754, (short)486, (short)644, (short)736, (short)569,
(short)379, (short)737, (short)621, (short)738, (short)739, (short)476, (short)576,
(short)498, (short)453, (short)646, (short)500, (short)741, (short)0, (short)501,
(short)312, (short)746, (short)312, (short)312, (short)312, (short)312, (short)312,
(short)312, (short)312, (short)312, (short)312, (short)312, (short)312, (short)312,
(short)312, (short)312, (short)312, (short)312, (short)312, (short)386, (short)0,
(short)0, (short)502, (short)0, (short)0, (short)503, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)1, (short)2, (short)3, (short)107,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)680, (short)681, (short)20, (short)222, (short)312, (short)223, (short)224,
(short)225, (short)226, (short)227, (short)228, (short)229, (short)230, (short)153,
(short)21, (short)154, (short)22, (short)155, (short)231, (short)0, (short)156,
(short)23, (short)232, (short)0, (short)0, (short)26, (short)27, (short)157,
(short)158, (short)233, (short)234, (short)159, (short)160, (short)161, (short)162,
(short)163, (short)164, (short)165, (short)28, (short)235, (short)166, (short)167,
(short)0, (short)168, (short)169, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)312,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)312, (short)0, (short)0,
(short)0, (short)236, (short)170, (short)171, (short)172, (short)173, (short)30,
(short)31, (short)32, (short)0, (short)237, (short)108, (short)0, (short)0,
(short)174, (short)0, (short)0, (short)34, (short)239, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)176, (short)0, (short)0, (short)177,
(short)178, (short)179, (short)0, (short)0, (short)0, (short)0, (short)180,
(short)181, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)312, (short)0, (short)0, (short)0, (short)1, (short)2, (short)3,
(short)107, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)0, (short)0, (short)20, (short)222, (short)0, (short)223,
(short)224, (short)225, (short)226, (short)227, (short)228, (short)229, (short)230,
(short)153, (short)21, (short)154, (short)22, (short)155, (short)231, (short)312,
(short)156, (short)23, (short)232, (short)0, (short)0, (short)26, (short)27,
(short)157, (short)158, (short)233, (short)234, (short)159, (short)160, (short)161,
(short)162, (short)163, (short)164, (short)165, (short)28, (short)235, (short)166,
(short)167, (short)0, (short)168, (short)169, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)236, (short)170, (short)171, (short)172, (short)173,
(short)30, (short)31, (short)32, (short)0, (short)237, (short)108, (short)238,
(short)0, (short)174, (short)0, (short)0, (short)34, (short)239, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)312, (short)176, (short)0,
(short)0, (short)177, (short)178, (short)179, (short)0, (short)312, (short)0,
(short)0, (short)180, (short)181, (short)1, (short)2, (short)3, (short)107,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)0, (short)0, (short)20, (short)222, (short)0, (short)223, (short)224,
(short)225, (short)226, (short)227, (short)228, (short)229, (short)230, (short)153,
(short)21, (short)154, (short)22, (short)155, (short)231, (short)0, (short)156,
(short)23, (short)232, (short)0, (short)0, (short)26, (short)27, (short)157,
(short)158, (short)233, (short)234, (short)159, (short)160, (short)161, (short)162,
(short)163, (short)164, (short)165, (short)28, (short)235, (short)166, (short)167,
(short)0, (short)168, (short)169, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)236, (short)170, (short)171, (short)172, (short)173, (short)30,
(short)31, (short)32, (short)0, (short)237, (short)108, (short)0, (short)0,
(short)174, (short)0, (short)0, (short)34, (short)239, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)176, (short)0, (short)0, (short)177,
(short)178, (short)179, (short)0, (short)0, (short)0, (short)0, (short)180,
(short)181, (short)1, (short)2, (short)3, (short)107, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)20, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)153, (short)21, (short)154,
(short)22, (short)155, (short)0, (short)0, (short)156, (short)23, (short)0,
(short)0, (short)0, (short)26, (short)27, (short)157, (short)158, (short)0,
(short)0, (short)159, (short)160, (short)161, (short)162, (short)163, (short)164,
(short)165, (short)28, (short)0, (short)166, (short)167, (short)0, (short)168,
(short)169, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)170,
(short)171, (short)172, (short)173, (short)30, (short)31, (short)32, (short)0,
(short)541, (short)0, (short)0, (short)0, (short)174, (short)0, (short)0,
(short)34, (short)239, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)176, (short)0, (short)0, (short)177, (short)178, (short)179, (short)0,
(short)0, (short)0, (short)0, (short)180, (short)181, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)153, (short)21, (short)154, (short)0,
(short)155, (short)0, (short)0, (short)156, (short)0, (short)0, (short)0,
(short)0, (short)26, (short)27, (short)157, (short)158, (short)0, (short)0,
(short)159, (short)160, (short)161, (short)162, (short)163, (short)164, (short)165,
(short)28, (short)0, (short)166, (short)167, (short)0, (short)168, (short)169,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)170, (short)171,
(short)172, (short)173, (short)30, (short)31, (short)32, (short)0, (short)0,
(short)324, (short)0, (short)0, (short)174, (short)0, (short)0, (short)34,
(short)239, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)176,
(short)0, (short)0, (short)177, (short)178, (short)179, (short)0, (short)0,
(short)0, (short)0, (short)180, (short)181, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)153, (short)21, (short)154, (short)0, (short)155,
(short)0, (short)0, (short)156, (short)0, (short)0, (short)0, (short)0, (short)26,
(short)27, (short)157, (short)158, (short)0, (short)0, (short)159, (short)160,
(short)161, (short)162, (short)163, (short)164, (short)165, (short)28, (short)0,
(short)166, (short)167, (short)0, (short)168, (short)169, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)170, (short)171, (short)172,
(short)173, (short)30, (short)31, (short)32, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)174, (short)0, (short)0, (short)34, (short)239,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)176, (short)0,
(short)0, (short)177, (short)178, (short)179, (short)0, (short)0, (short)0,
(short)0, (short)180, (short)181, (short)1, (short)2, (short)3, (short)4,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)0, (short)0, (short)20, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)21, (short)0,
(short)22, (short)0, (short)0, (short)0, (short)0, (short)23, (short)0, (short)24,
(short)25, (short)26, (short)27, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)0,
(short)0, (short)1, (short)2, (short)3, (short)107, (short)5, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)0, (short)0,
(short)20, (short)0, (short)0, (short)0, (short)29, (short)0, (short)0, (short)0,
(short)0, (short)30, (short)31, (short)32, (short)21, (short)0, (short)0,
(short)0, (short)0, (short)33, (short)0, (short)23, (short)34, (short)35,
(short)0, (short)26, (short)27, (short)0, (short)0, (short)0, (short)36, (short)37,
(short)38, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)0,
(short)0, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)0, (short)0, (short)21, (short)0,
(short)30, (short)31, (short)32, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)33, (short)26, (short)27, (short)34, (short)35, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)36, (short)37, (short)38, (short)28,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)0, (short)0,
(short)0, (short)0, (short)30, (short)31, (short)32, (short)21, (short)465,
(short)0, (short)0, (short)0, (short)303, (short)466, (short)0, (short)34,
(short)35, (short)0, (short)26, (short)27, (short)0, (short)294, (short)0,
(short)36, (short)37, (short)38, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)28, (short)0, (short)0, (short)1, (short)2, (short)3, (short)107, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)20, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)30, (short)0, (short)32, (short)21, (short)465,
(short)22, (short)0, (short)0, (short)293, (short)466, (short)23, (short)34,
(short)35, (short)0, (short)26, (short)27, (short)0, (short)294, (short)0,
(short)36, (short)37, (short)38, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)28, (short)0, (short)0, (short)0, (short)0, (short)0, (short)1, (short)2,
(short)3, (short)107, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)0, (short)0, (short)20, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)30, (short)0, (short)32, (short)0, (short)0, (short)108,
(short)21, (short)209, (short)22, (short)0, (short)0, (short)34, (short)35,
(short)23, (short)0, (short)0, (short)0, (short)26, (short)27, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)0, (short)0, (short)1, (short)2, (short)3, (short)107,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)0, (short)0, (short)20, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)30, (short)0, (short)32, (short)21, (short)0,
(short)108, (short)0, (short)0, (short)0, (short)0, (short)23, (short)34,
(short)35, (short)0, (short)26, (short)27, (short)222, (short)0, (short)223,
(short)224, (short)225, (short)226, (short)227, (short)228, (short)229, (short)230,
(short)153, (short)28, (short)154, (short)0, (short)155, (short)231, (short)0,
(short)156, (short)0, (short)232, (short)0, (short)0, (short)0, (short)0,
(short)157, (short)158, (short)233, (short)234, (short)159, (short)160, (short)161,
(short)162, (short)163, (short)164, (short)165, (short)0, (short)235, (short)166,
(short)167, (short)0, (short)168, (short)169, (short)0, (short)0, (short)30,
(short)0, (short)32, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)34, (short)35, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)236, (short)170, (short)171, (short)172, (short)173,
(short)0, (short)31, (short)0, (short)0, (short)237, (short)108, (short)0,
(short)0, (short)174, (short)0, (short)0, (short)63, (short)175, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)176, (short)0, (short)0,
(short)177, (short)178, (short)179, (short)0, (short)0, (short)0, (short)0,
(short)180, (short)181, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)21, (short)0, (short)0, (short)29, (short)64, (short)0,
(short)65, (short)66, (short)0, (short)31, (short)0, (short)26, (short)27,
(short)0, (short)21, (short)0, (short)67, (short)0, (short)0, (short)68, (short)69,
(short)0, (short)0, (short)0, (short)28, (short)26, (short)27, (short)70,
(short)0, (short)0, (short)0, (short)71, (short)72, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)28, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)257, (short)0, (short)0, (short)0, (short)0, (short)30, (short)0, (short)32,
(short)0, (short)258, (short)0, (short)0, (short)0, (short)0, (short)259,
(short)0, (short)34, (short)35, (short)0, (short)30, (short)0, (short)32,
(short)0, (short)590, (short)0, (short)0, (short)0, (short)0, (short)591,
(short)0, (short)34, (short)35, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)21, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)26, (short)27, (short)0, (short)21,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)26, (short)27, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)0, (short)0, (short)30, (short)0, (short)32, (short)0, (short)0,
(short)138, (short)561, (short)0, (short)0, (short)0, (short)21, (short)34,
(short)35, (short)0, (short)30, (short)0, (short)32, (short)0, (short)0, (short)0,
(short)0, (short)26, (short)27, (short)480, (short)0, (short)34, (short)35,
(short)0, (short)153, (short)0, (short)154, (short)0, (short)155, (short)0,
(short)28, (short)156, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)157, (short)158, (short)0, (short)0, (short)159, (short)160, (short)161,
(short)162, (short)163, (short)164, (short)165, (short)0, (short)0, (short)166,
(short)167, (short)0, (short)168, (short)169, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)30, (short)0, (short)32, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)34,
(short)35, (short)0, (short)29, (short)170, (short)171, (short)172, (short)173,
(short)0, (short)31, (short)0, (short)0, (short)0, (short)313, (short)481,
(short)0, (short)174, (short)0, (short)0, (short)0, (short)175, (short)0,
(short)0, (short)0, (short)444, (short)445, (short)0, (short)176, (short)0,
(short)0, (short)177, (short)178, (short)179, (short)0, (short)0, (short)0,
(short)0, (short)180, (short)181, (short)153, (short)0, (short)154, (short)0,
(short)155, (short)0, (short)0, (short)156, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)157, (short)158, (short)0, (short)0,
(short)159, (short)160, (short)161, (short)162, (short)163, (short)164, (short)165,
(short)0, (short)0, (short)166, (short)167, (short)0, (short)168, (short)169,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)170, (short)171,
(short)172, (short)173, (short)0, (short)31, (short)0, (short)0, (short)0,
(short)313, (short)656, (short)0, (short)174, (short)0, (short)0, (short)0,
(short)175, (short)0, (short)0, (short)0, (short)444, (short)445, (short)0,
(short)176, (short)0, (short)0, (short)177, (short)178, (short)179, (short)0,
(short)0, (short)0, (short)0, (short)180, (short)181, (short)153, (short)0,
(short)154, (short)0, (short)155, (short)0, (short)0, (short)156, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)157, (short)158,
(short)0, (short)0, (short)159, (short)160, (short)161, (short)162, (short)163,
(short)164, (short)165, (short)0, (short)0, (short)166, (short)167, (short)0,
(short)168, (short)169, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)29, (short)170, (short)171, (short)172, (short)173, (short)0, (short)31,
(short)0, (short)0, (short)0, (short)313, (short)717, (short)0, (short)174,
(short)0, (short)0, (short)0, (short)175, (short)0, (short)0, (short)0, (short)444,
(short)445, (short)0, (short)176, (short)0, (short)0, (short)177, (short)178,
(short)179, (short)0, (short)0, (short)0, (short)0, (short)180, (short)181,
(short)153, (short)0, (short)154, (short)0, (short)155, (short)0, (short)0,
(short)156, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)157,
(short)158, (short)0, (short)0, (short)159, (short)160, (short)161, (short)162,
(short)163, (short)164, (short)165, (short)0, (short)0, (short)166, (short)167,
(short)0, (short)168, (short)169, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)29, (short)170, (short)171, (short)172, (short)173, (short)0,
(short)31, (short)0, (short)0, (short)0, (short)313, (short)0, (short)0, (short)174,
(short)0, (short)0, (short)0, (short)175, (short)0, (short)0, (short)0, (short)444,
(short)445, (short)0, (short)176, (short)0, (short)0, (short)177, (short)178,
(short)179, (short)0, (short)0, (short)0, (short)0, (short)180, (short)181,
(short)153, (short)0, (short)154, (short)0, (short)155, (short)0, (short)0,
(short)156, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)157,
(short)158, (short)0, (short)0, (short)159, (short)160, (short)161, (short)162,
(short)163, (short)164, (short)165, (short)0, (short)0, (short)166, (short)167,
(short)0, (short)168, (short)169, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)29, (short)170, (short)171, (short)172, (short)173, (short)0,
(short)31, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)174,
(short)0, (short)0, (short)0, (short)175, (short)153, (short)0, (short)154,
(short)0, (short)155, (short)265, (short)176, (short)156, (short)0, (short)177,
(short)178, (short)179, (short)0, (short)0, (short)157, (short)158, (short)180,
(short)181, (short)159, (short)160, (short)161, (short)162, (short)163, (short)164,
(short)165, (short)0, (short)0, (short)166, (short)167, (short)0, (short)168,
(short)169, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)170,
(short)171, (short)172, (short)173, (short)0, (short)31, (short)0, (short)0,
(short)0, (short)313, (short)0, (short)0, (short)174, (short)0, (short)0,
(short)0, (short)175, (short)153, (short)0, (short)154, (short)0, (short)155,
(short)0, (short)176, (short)156, (short)0, (short)177, (short)178, (short)179,
(short)0, (short)0, (short)157, (short)158, (short)180, (short)181, (short)159,
(short)160, (short)161, (short)162, (short)163, (short)164, (short)165, (short)0,
(short)0, (short)166, (short)167, (short)0, (short)168, (short)169, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)29, (short)170, (short)171,
(short)172, (short)173, (short)0, (short)31, (short)0, (short)0, (short)401,
(short)0, (short)0, (short)0, (short)174, (short)0, (short)0, (short)0, (short)175,
(short)153, (short)0, (short)154, (short)0, (short)155, (short)0, (short)176,
(short)156, (short)0, (short)177, (short)178, (short)179, (short)0, (short)0,
(short)157, (short)158, (short)180, (short)181, (short)159, (short)160, (short)161,
(short)162, (short)163, (short)164, (short)165, (short)0, (short)0, (short)166,
(short)167, (short)0, (short)168, (short)169, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)29, (short)170, (short)171, (short)172, (short)173,
(short)0, (short)31, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)174, (short)0, (short)0, (short)0, (short)175, (short)153, (short)0,
(short)154, (short)0, (short)155, (short)469, (short)176, (short)156, (short)0,
(short)177, (short)178, (short)179, (short)0, (short)0, (short)157, (short)158,
(short)180, (short)181, (short)159, (short)160, (short)161, (short)162, (short)163,
(short)164, (short)165, (short)0, (short)0, (short)166, (short)167, (short)0,
(short)168, (short)169, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)29, (short)170, (short)171, (short)172, (short)173, (short)0, (short)31,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)174, (short)521,
(short)0, (short)0, (short)175, (short)153, (short)0, (short)154, (short)0,
(short)155, (short)0, (short)176, (short)156, (short)0, (short)177, (short)178,
(short)179, (short)0, (short)0, (short)157, (short)158, (short)180, (short)181,
(short)159, (short)160, (short)161, (short)162, (short)163, (short)164, (short)165,
(short)0, (short)0, (short)166, (short)167, (short)0, (short)168, (short)169,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)170, (short)171,
(short)172, (short)173, (short)0, (short)31, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)174, (short)547, (short)0, (short)0,
(short)175, (short)153, (short)0, (short)154, (short)0, (short)155, (short)0,
(short)176, (short)156, (short)0, (short)177, (short)178, (short)179, (short)0,
(short)0, (short)157, (short)158, (short)180, (short)181, (short)159, (short)160,
(short)161, (short)162, (short)163, (short)164, (short)165, (short)0, (short)0,
(short)166, (short)167, (short)0, (short)168, (short)169, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)170, (short)171, (short)172,
(short)173, (short)0, (short)31, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)174, (short)0, (short)0, (short)0, (short)175, (short)153,
(short)0, (short)154, (short)0, (short)155, (short)594, (short)176, (short)156,
(short)0, (short)177, (short)178, (short)179, (short)0, (short)0, (short)157,
(short)158, (short)180, (short)181, (short)159, (short)160, (short)161, (short)162,
(short)163, (short)164, (short)165, (short)0, (short)0, (short)166, (short)167,
(short)0, (short)168, (short)169, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)29, (short)170, (short)171, (short)172, (short)173, (short)0,
(short)31, (short)0, (short)0, (short)628, (short)0, (short)0, (short)0, (short)174,
(short)0, (short)0, (short)0, (short)175, (short)153, (short)0, (short)154,
(short)0, (short)155, (short)0, (short)176, (short)156, (short)0, (short)177,
(short)178, (short)179, (short)0, (short)0, (short)157, (short)158, (short)180,
(short)181, (short)159, (short)160, (short)161, (short)162, (short)163, (short)164,
(short)165, (short)0, (short)0, (short)166, (short)167, (short)0, (short)168,
(short)169, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)170,
(short)171, (short)172, (short)173, (short)0, (short)31, (short)0, (short)0,
(short)630, (short)0, (short)0, (short)0, (short)174, (short)0, (short)0,
(short)0, (short)175, (short)153, (short)0, (short)154, (short)0, (short)155,
(short)0, (short)176, (short)156, (short)0, (short)177, (short)178, (short)179,
(short)0, (short)0, (short)157, (short)158, (short)180, (short)181, (short)159,
(short)160, (short)161, (short)162, (short)163, (short)164, (short)165, (short)0,
(short)0, (short)166, (short)167, (short)0, (short)168, (short)169, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)29, (short)170, (short)171,
(short)172, (short)173, (short)0, (short)31, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)174, (short)672, (short)0, (short)0,
(short)175, (short)153, (short)0, (short)154, (short)0, (short)155, (short)0,
(short)176, (short)156, (short)0, (short)177, (short)178, (short)179, (short)0,
(short)0, (short)157, (short)158, (short)180, (short)181, (short)159, (short)160,
(short)161, (short)162, (short)163, (short)164, (short)165, (short)0, (short)0,
(short)166, (short)167, (short)0, (short)168, (short)169, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)170, (short)171, (short)172,
(short)173, (short)0, (short)31, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)174, (short)675, (short)0, (short)0, (short)175, (short)153,
(short)0, (short)154, (short)0, (short)155, (short)0, (short)176, (short)156,
(short)0, (short)177, (short)178, (short)179, (short)0, (short)0, (short)157,
(short)158, (short)180, (short)181, (short)159, (short)160, (short)161, (short)162,
(short)163, (short)164, (short)165, (short)0, (short)0, (short)166, (short)167,
(short)0, (short)168, (short)169, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)29, (short)170, (short)171, (short)172, (short)173, (short)0,
(short)31, (short)0, (short)0, (short)678, (short)0, (short)0, (short)0, (short)174,
(short)0, (short)0, (short)0, (short)175, (short)153, (short)0, (short)154,
(short)0, (short)155, (short)0, (short)176, (short)156, (short)0, (short)177,
(short)178, (short)179, (short)0, (short)0, (short)157, (short)158, (short)180,
(short)181, (short)159, (short)160, (short)161, (short)162, (short)163, (short)164,
(short)165, (short)0, (short)0, (short)166, (short)167, (short)0, (short)168,
(short)169, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)170,
(short)171, (short)172, (short)173, (short)0, (short)31, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)174, (short)701, (short)0,
(short)0, (short)175, (short)153, (short)0, (short)154, (short)0, (short)155,
(short)0, (short)176, (short)156, (short)0, (short)177, (short)178, (short)179,
(short)0, (short)0, (short)157, (short)158, (short)180, (short)181, (short)159,
(short)160, (short)161, (short)162, (short)163, (short)164, (short)165, (short)0,
(short)0, (short)166, (short)167, (short)0, (short)168, (short)169, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)29, (short)170, (short)171,
(short)172, (short)173, (short)0, (short)31, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)174, (short)705, (short)0, (short)0,
(short)175, (short)153, (short)0, (short)154, (short)0, (short)155, (short)0,
(short)176, (short)156, (short)0, (short)177, (short)178, (short)179, (short)0,
(short)0, (short)157, (short)158, (short)180, (short)181, (short)159, (short)160,
(short)161, (short)162, (short)163, (short)164, (short)165, (short)0, (short)0,
(short)166, (short)167, (short)0, (short)168, (short)169, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)170, (short)171, (short)172,
(short)173, (short)0, (short)31, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)174, (short)707, (short)0, (short)0, (short)175, (short)153,
(short)0, (short)154, (short)0, (short)155, (short)0, (short)176, (short)156,
(short)0, (short)177, (short)178, (short)179, (short)0, (short)0, (short)157,
(short)158, (short)180, (short)181, (short)159, (short)160, (short)161, (short)162,
(short)163, (short)164, (short)165, (short)0, (short)0, (short)166, (short)167,
(short)0, (short)168, (short)169, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)29, (short)170, (short)171, (short)172, (short)173, (short)0,
(short)31, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)174,
(short)729, (short)0, (short)0, (short)175, (short)153, (short)0, (short)154,
(short)0, (short)155, (short)0, (short)176, (short)156, (short)0, (short)177,
(short)178, (short)179, (short)0, (short)0, (short)157, (short)158, (short)180,
(short)181, (short)159, (short)160, (short)161, (short)162, (short)163, (short)164,
(short)165, (short)0, (short)0, (short)166, (short)167, (short)0, (short)168,
(short)169, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)170,
(short)171, (short)172, (short)173, (short)0, (short)31, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)174, (short)0, (short)0, (short)0,
(short)175, (short)153, (short)0, (short)154, (short)0, (short)0, (short)0,
(short)176, (short)0, (short)0, (short)177, (short)178, (short)179, (short)0,
(short)0, (short)157, (short)158, (short)180, (short)181, (short)159, (short)160,
(short)161, (short)162, (short)163, (short)164, (short)165, (short)0, (short)0,
(short)0, (short)167, (short)0, (short)168, (short)169, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)170, (short)171, (short)172,
(short)173, (short)0, (short)31, (short)0, (short)0, (short)0, (short)313,
(short)0, (short)0, (short)174, (short)0, (short)0, (short)0, (short)175,
(short)153, (short)0, (short)154, (short)0, (short)0, (short)0, (short)176,
(short)0, (short)0, (short)177, (short)178, (short)179, (short)0, (short)0,
(short)157, (short)158, (short)180, (short)181, (short)159, (short)160, (short)161,
(short)162, (short)163, (short)164, (short)165, (short)0, (short)0, (short)0,
(short)167, (short)0, (short)168, (short)169, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)29, (short)170, (short)171, (short)172, (short)173,
(short)0, (short)31, (short)0, (short)0, (short)0, (short)609, (short)0, (short)0,
(short)174, (short)0, (short)0, (short)0, (short)175, (short)153, (short)0,
(short)154, (short)0, (short)0, (short)0, (short)176, (short)0, (short)0,
(short)177, (short)178, (short)179, (short)0, (short)0, (short)157, (short)158,
(short)180, (short)181, (short)159, (short)160, (short)161, (short)162, (short)163,
(short)164, (short)165, (short)0, (short)0, (short)0, (short)167, (short)0,
(short)168, (short)169, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)29, (short)170, (short)171, (short)172, (short)173, (short)0, (short)31,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)309, (short)0,
(short)0, (short)0, (short)175, (short)153, (short)0, (short)154, (short)0,
(short)0, (short)0, (short)176, (short)0, (short)0, (short)177, (short)178,
(short)179, (short)0, (short)0, (short)157, (short)158, (short)180, (short)181,
(short)159, (short)160, (short)161, (short)162, (short)163, (short)164, (short)165,
(short)0, (short)0, (short)0, (short)167, (short)0, (short)168, (short)169,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)170, (short)171,
(short)172, (short)173, (short)0, (short)31, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)321, (short)0, (short)0, (short)0, (short)175,
(short)153, (short)0, (short)154, (short)0, (short)0, (short)0, (short)176,
(short)0, (short)0, (short)177, (short)178, (short)179, (short)0, (short)0,
(short)157, (short)158, (short)180, (short)181, (short)159, (short)160, (short)161,
(short)162, (short)163, (short)164, (short)165, (short)0, (short)0, (short)0,
(short)167, (short)0, (short)168, (short)169, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)29, (short)170, (short)171, (short)172, (short)173,
(short)0, (short)31, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)174, (short)0, (short)0, (short)0, (short)175, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)176, (short)0, (short)0, (short)177,
(short)178, (short)179, (short)0, (short)0, (short)0, (short)0, (short)180,
(short)181}; static short Cyc_yycheck[ 4215u]={ (short)53, (short)0, (short)248,
(short)88, (short)53, (short)53, (short)156, (short)60, (short)156, (short)54,
(short)114, (short)113, (short)88, (short)22, (short)372, (short)83, (short)131,
(short)141, (short)55, (short)33, (short)174, (short)90, (short)21, (short)22,
(short)418, (short)24, (short)150, (short)26, (short)27, (short)105, (short)17,
(short)45, (short)53, (short)283, (short)33, (short)308, (short)88, (short)113,
(short)155, (short)73, (short)83, (short)40, (short)85, (short)74, (short)209,
(short)88, (short)45, (short)100, (short)101, (short)102, (short)103, (short)100,
(short)51, (short)88, (short)106, (short)108, (short)55, (short)110, (short)67,
(short)108, (short)278, (short)110, (short)105, (short)72, (short)363, (short)68,
(short)422, (short)461, (short)67, (short)368, (short)113, (short)114, (short)117,
(short)72, (short)108, (short)17, (short)18, (short)19, (short)324, (short)100,
(short)79, (short)27, (short)81, (short)114, (short)88, (short)83, (short)96,
(short)85, (short)93, (short)110, (short)88, (short)95, (short)439, (short)92,
(short)93, (short)94, (short)95, (short)111, (short)445, (short)104, (short)103,
(short)88, (short)86, (short)93, (short)107, (short)68, (short)405, (short)105,
(short)98, (short)465, (short)56, (short)87, (short)111, (short)68, (short)98,
(short)113, (short)114, (short)74, (short)669, (short)89, (short)90, (short)120,
(short)88, (short)80, (short)123, (short)107, (short)125, (short)103, (short)94,
(short)113, (short)40, (short)174, (short)106, (short)115, (short)133, (short)101,
(short)150, (short)136, (short)137, (short)106, (short)103, (short)217, (short)98,
(short)267, (short)107, (short)115, (short)295, (short)104, (short)103, (short)106,
(short)423, (short)150, (short)107, (short)109, (short)87, (short)309, (short)305,
(short)210, (short)103, (short)176, (short)177, (short)210, (short)179, (short)46,
(short)47, (short)48, (short)111, (short)321, (short)100, (short)293, (short)112,
(short)113, (short)114, (short)101, (short)217, (short)132, (short)174, (short)109,
(short)110, (short)303, (short)197, (short)732, (short)92, (short)93, (short)94,
(short)95, (short)129, (short)98, (short)535, (short)210, (short)88, (short)742,
(short)69, (short)70, (short)208, (short)248, (short)747, (short)95, (short)749,
(short)248, (short)64, (short)65, (short)66, (short)559, (short)101, (short)219,
(short)88, (short)457, (short)458, (short)208, (short)120, (short)93, (short)94,
(short)110, (short)141, (short)291, (short)292, (short)293, (short)98, (short)217,
(short)219, (short)101, (short)104, (short)150, (short)267, (short)98, (short)106,
(short)303, (short)242, (short)106, (short)107, (short)174, (short)308, (short)301,
(short)590, (short)118, (short)88, (short)101, (short)93, (short)209, (short)97,
(short)98, (short)94, (short)242, (short)99, (short)109, (short)110, (short)384,
(short)291, (short)292, (short)293, (short)609, (short)297, (short)97, (short)648,
(short)392, (short)139, (short)140, (short)301, (short)103, (short)303, (short)98,
(short)96, (short)101, (short)317, (short)308, (short)309, (short)101, (short)305,
(short)278, (short)387, (short)324, (short)106, (short)112, (short)98, (short)324,
(short)110, (short)225, (short)321, (short)278, (short)279, (short)120, (short)121,
(short)231, (short)283, (short)233, (short)234, (short)230, (short)101, (short)303,
(short)97, (short)88, (short)101, (short)291, (short)292, (short)293, (short)103,
(short)110, (short)95, (short)112, (short)113, (short)114, (short)88, (short)301,
(short)303, (short)303, (short)305, (short)431, (short)384, (short)97, (short)308,
(short)309, (short)101, (short)89, (short)88, (short)103, (short)392, (short)106,
(short)317, (short)484, (short)94, (short)100, (short)439, (short)321, (short)294,
(short)282, (short)411, (short)101, (short)445, (short)286, (short)97, (short)348,
(short)349, (short)350, (short)101, (short)411, (short)110, (short)103, (short)112,
(short)113, (short)114, (short)384, (short)108, (short)313, (short)397, (short)460,
(short)103, (short)423, (short)397, (short)392, (short)320, (short)108, (short)88,
(short)97, (short)98, (short)97, (short)97, (short)327, (short)94, (short)412,
(short)578, (short)103, (short)103, (short)412, (short)412, (short)101, (short)309,
(short)99, (short)411, (short)473, (short)472, (short)103, (short)106, (short)316,
(short)110, (short)109, (short)319, (short)376, (short)321, (short)378, (short)423,
(short)599, (short)103, (short)93, (short)102, (short)103, (short)384, (short)108,
(short)431, (short)99, (short)333, (short)412, (short)362, (short)363, (short)392,
(short)88, (short)472, (short)97, (short)368, (short)88, (short)618, (short)94,
(short)372, (short)103, (short)97, (short)94, (short)95, (short)87, (short)101,
(short)98, (short)97, (short)97, (short)102, (short)411, (short)71, (short)72,
(short)103, (short)103, (short)535, (short)112, (short)113, (short)114, (short)97,
(short)527, (short)97, (short)423, (short)471, (short)370, (short)103, (short)472,
(short)473, (short)103, (short)441, (short)432, (short)599, (short)405, (short)101,
(short)446, (short)437, (short)99, (short)555, (short)411, (short)441, (short)103,
(short)103, (short)99, (short)416, (short)446, (short)107, (short)103, (short)393,
(short)394, (short)395, (short)618, (short)397, (short)605, (short)102, (short)605,
(short)457, (short)458, (short)408, (short)98, (short)410, (short)99, (short)101,
(short)711, (short)376, (short)103, (short)378, (short)340, (short)341, (short)342,
(short)343, (short)487, (short)472, (short)473, (short)493, (short)695, (short)67,
(short)68, (short)96, (short)115, (short)116, (short)90, (short)527, (short)101,
(short)732, (short)98, (short)487, (short)115, (short)106, (short)37, (short)101,
(short)658, (short)110, (short)101, (short)613, (short)101, (short)583, (short)552,
(short)101, (short)747, (short)101, (short)552, (short)474, (short)102, (short)103,
(short)583, (short)102, (short)103, (short)102, (short)103, (short)88, (short)101,
(short)484, (short)102, (short)103, (short)99, (short)94, (short)73, (short)534,
(short)119, (short)432, (short)102, (short)103, (short)101, (short)89, (short)90,
(short)604, (short)527, (short)695, (short)552, (short)108, (short)102, (short)103,
(short)534, (short)112, (short)113, (short)114, (short)117, (short)583, (short)118,
(short)88, (short)89, (short)101, (short)91, (short)92, (short)99, (short)94,
(short)99, (short)603, (short)22, (short)23, (short)99, (short)603, (short)101,
(short)338, (short)339, (short)104, (short)105, (short)99, (short)604, (short)88,
(short)109, (short)110, (short)98, (short)112, (short)99, (short)94, (short)578,
(short)116, (short)117, (short)102, (short)103, (short)98, (short)101, (short)88,
(short)102, (short)103, (short)578, (short)102, (short)103, (short)603, (short)102,
(short)103, (short)583, (short)112, (short)113, (short)114, (short)102, (short)103,
(short)102, (short)103, (short)102, (short)103, (short)102, (short)103, (short)101,
(short)541, (short)542, (short)101, (short)344, (short)345, (short)101, (short)101,
(short)97, (short)604, (short)346, (short)347, (short)97, (short)101, (short)100,
(short)153, (short)108, (short)155, (short)156, (short)99, (short)693, (short)102,
(short)99, (short)102, (short)111, (short)103, (short)99, (short)103, (short)100,
(short)103, (short)168, (short)169, (short)599, (short)102, (short)99, (short)99,
(short)74, (short)35, (short)176, (short)177, (short)102, (short)179, (short)609,
(short)106, (short)97, (short)612, (short)103, (short)108, (short)28, (short)97,
(short)41, (short)618, (short)93, (short)693, (short)87, (short)102, (short)115,
(short)88, (short)101, (short)197, (short)102, (short)711, (short)88, (short)103,
(short)99, (short)711, (short)103, (short)101, (short)37, (short)99, (short)102,
(short)93, (short)111, (short)102, (short)680, (short)102, (short)88, (short)102,
(short)102, (short)624, (short)101, (short)626, (short)732, (short)103, (short)680,
(short)102, (short)732, (short)627, (short)628, (short)99, (short)630, (short)658,
(short)632, (short)102, (short)99, (short)103, (short)102, (short)747, (short)693,
(short)102, (short)99, (short)747, (short)98, (short)97, (short)75, (short)76,
(short)77, (short)78, (short)79, (short)80, (short)81, (short)82, (short)83,
(short)84, (short)101, (short)655, (short)102, (short)107, (short)99, (short)88,
(short)89, (short)103, (short)91, (short)92, (short)102, (short)94, (short)111,
(short)672, (short)695, (short)100, (short)675, (short)98, (short)101, (short)106,
(short)674, (short)104, (short)105, (short)677, (short)678, (short)684, (short)109,
(short)110, (short)102, (short)112, (short)102, (short)111, (short)35, (short)116,
(short)117, (short)102, (short)98, (short)696, (short)97, (short)88, (short)101,
(short)700, (short)701, (short)26, (short)102, (short)704, (short)705, (short)102,
(short)707, (short)102, (short)102, (short)108, (short)99, (short)99, (short)102,
(short)709, (short)97, (short)102, (short)102, (short)0, (short)714, (short)0,
(short)317, (short)568, (short)723, (short)437, (short)208, (short)726, (short)534,
(short)728, (short)729, (short)299, (short)447, (short)332, (short)279, (short)578,
(short)334, (short)731, - (short)1, (short)335, (short)332, (short)740, (short)334,
(short)335, (short)336, (short)337, (short)338, (short)339, (short)340, (short)341,
(short)342, (short)343, (short)344, (short)345, (short)346, (short)347, (short)348,
(short)349, (short)350, (short)217, - (short)1, - (short)1, (short)336, -
(short)1, - (short)1, (short)337, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)3, (short)4, (short)5, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21,
(short)22, (short)23, (short)24, (short)25, (short)387, (short)27, (short)28,
(short)29, (short)30, (short)31, (short)32, (short)33, (short)34, (short)35,
(short)36, (short)37, (short)38, (short)39, (short)40, - (short)1, (short)42,
(short)43, (short)44, - (short)1, - (short)1, (short)47, (short)48, (short)49,
(short)50, (short)51, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, (short)60, (short)61, (short)62, (short)63, -
(short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)439, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)445, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, (short)93, (short)94, (short)95, - (short)1, (short)97,
(short)98, - (short)1, - (short)1, (short)101, - (short)1, - (short)1, (short)104,
(short)105, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)112, - (short)1, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, - (short)1, - (short)1, (short)122, (short)123, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)493, - (short)1, - (short)1, - (short)1, (short)3, (short)4,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, (short)24, (short)25, - (short)1,
(short)27, (short)28, (short)29, (short)30, (short)31, (short)32, (short)33,
(short)34, (short)35, (short)36, (short)37, (short)38, (short)39, (short)40,
(short)535, (short)42, (short)43, (short)44, - (short)1, - (short)1, (short)47,
(short)48, (short)49, (short)50, (short)51, (short)52, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, (short)59, (short)60, (short)61,
(short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)88,
(short)89, (short)90, (short)91, (short)92, (short)93, (short)94, (short)95, -
(short)1, (short)97, (short)98, (short)99, - (short)1, (short)101, - (short)1, -
(short)1, (short)104, (short)105, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)605, (short)112, - (short)1, - (short)1, (short)115, (short)116,
(short)117, - (short)1, (short)613, - (short)1, - (short)1, (short)122, (short)123,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24,
(short)25, - (short)1, (short)27, (short)28, (short)29, (short)30, (short)31,
(short)32, (short)33, (short)34, (short)35, (short)36, (short)37, (short)38,
(short)39, (short)40, - (short)1, (short)42, (short)43, (short)44, - (short)1, -
(short)1, (short)47, (short)48, (short)49, (short)50, (short)51, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, (short)59,
(short)60, (short)61, (short)62, (short)63, - (short)1, (short)65, (short)66, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, (short)93,
(short)94, (short)95, - (short)1, (short)97, (short)98, - (short)1, - (short)1,
(short)101, - (short)1, - (short)1, (short)104, (short)105, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)112, - (short)1,
- (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1, -
(short)1, - (short)1, (short)122, (short)123, (short)3, (short)4, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)35, (short)36, (short)37, (short)38, (short)39, - (short)1, -
(short)1, (short)42, (short)43, - (short)1, - (short)1, - (short)1, (short)47,
(short)48, (short)49, (short)50, - (short)1, - (short)1, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, (short)59, (short)60, - (short)1,
(short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)88,
(short)89, (short)90, (short)91, (short)92, (short)93, (short)94, (short)95, -
(short)1, (short)97, - (short)1, - (short)1, - (short)1, (short)101, - (short)1,
- (short)1, (short)104, (short)105, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)112, - (short)1, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, - (short)1, - (short)1, (short)122,
(short)123, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)35, (short)36, (short)37, - (short)1, (short)39, - (short)1, -
(short)1, (short)42, - (short)1, - (short)1, - (short)1, - (short)1, (short)47,
(short)48, (short)49, (short)50, - (short)1, - (short)1, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, (short)59, (short)60, - (short)1,
(short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)88,
(short)89, (short)90, (short)91, (short)92, (short)93, (short)94, (short)95, -
(short)1, - (short)1, (short)98, - (short)1, - (short)1, (short)101, - (short)1,
- (short)1, (short)104, (short)105, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)112, - (short)1, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, - (short)1, - (short)1, (short)122,
(short)123, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)35, (short)36, (short)37, - (short)1, (short)39, - (short)1, -
(short)1, (short)42, - (short)1, - (short)1, - (short)1, - (short)1, (short)47,
(short)48, (short)49, (short)50, - (short)1, - (short)1, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, (short)59, (short)60, - (short)1,
(short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)88,
(short)89, (short)90, (short)91, (short)92, (short)93, (short)94, (short)95, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)101, - (short)1,
- (short)1, (short)104, (short)105, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)112, - (short)1, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, - (short)1, - (short)1, (short)122,
(short)123, (short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1,
(short)24, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)36, - (short)1,
(short)38, - (short)1, - (short)1, - (short)1, - (short)1, (short)43, - (short)1,
(short)45, (short)46, (short)47, (short)48, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)60, - (short)1, - (short)1, (short)3, (short)4,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1,
- (short)1, (short)88, - (short)1, - (short)1, - (short)1, - (short)1, (short)93,
(short)94, (short)95, (short)36, - (short)1, - (short)1, - (short)1, - (short)1,
(short)101, - (short)1, (short)43, (short)104, (short)105, - (short)1, (short)47,
(short)48, - (short)1, - (short)1, - (short)1, (short)112, (short)113, (short)114,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)60, - (short)1,
- (short)1, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)88, - (short)1,
- (short)1, (short)36, - (short)1, (short)93, (short)94, (short)95, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)101, (short)47, (short)48,
(short)104, (short)105, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)112, (short)113, (short)114, (short)60, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)88, - (short)1,
- (short)1, - (short)1, - (short)1, (short)93, (short)94, (short)95, (short)36,
(short)97, - (short)1, - (short)1, - (short)1, (short)101, (short)102, - (short)1,
(short)104, (short)105, - (short)1, (short)47, (short)48, - (short)1, (short)110,
- (short)1, (short)112, (short)113, (short)114, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)60, - (short)1, - (short)1, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)93, - (short)1, (short)95, (short)36, (short)97, (short)38, -
(short)1, - (short)1, (short)101, (short)102, (short)43, (short)104, (short)105,
- (short)1, (short)47, (short)48, - (short)1, (short)110, - (short)1, (short)112,
(short)113, (short)114, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)60, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)93, - (short)1,
(short)95, - (short)1, - (short)1, (short)98, (short)36, (short)100, (short)38,
- (short)1, - (short)1, (short)104, (short)105, (short)43, - (short)1, - (short)1,
- (short)1, (short)47, (short)48, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)60, - (short)1, - (short)1, (short)3, (short)4, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)93, - (short)1,
(short)95, (short)36, - (short)1, (short)98, - (short)1, - (short)1, - (short)1,
- (short)1, (short)43, (short)104, (short)105, - (short)1, (short)47, (short)48,
(short)25, - (short)1, (short)27, (short)28, (short)29, (short)30, (short)31,
(short)32, (short)33, (short)34, (short)35, (short)60, (short)37, - (short)1,
(short)39, (short)40, - (short)1, (short)42, - (short)1, (short)44, - (short)1,
- (short)1, - (short)1, - (short)1, (short)49, (short)50, (short)51, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, (short)59, -
(short)1, (short)61, (short)62, (short)63, - (short)1, (short)65, (short)66, -
(short)1, - (short)1, (short)93, - (short)1, (short)95, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
(short)105, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)88, (short)89, (short)90, (short)91, (short)92, - (short)1,
(short)94, - (short)1, - (short)1, (short)97, (short)98, - (short)1, - (short)1,
(short)101, - (short)1, - (short)1, (short)37, (short)105, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)112, - (short)1, -
(short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1, - (short)1,
- (short)1, (short)122, (short)123, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, (short)36, - (short)1, - (short)1, (short)88,
(short)89, - (short)1, (short)91, (short)92, - (short)1, (short)94, - (short)1,
(short)47, (short)48, - (short)1, (short)36, - (short)1, (short)101, - (short)1,
- (short)1, (short)104, (short)105, - (short)1, - (short)1, - (short)1, (short)60,
(short)47, (short)48, (short)112, - (short)1, - (short)1, - (short)1, (short)116,
(short)117, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)60,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, - (short)1, - (short)1, - (short)1, - (short)1, (short)93, - (short)1,
(short)95, - (short)1, (short)97, - (short)1, - (short)1, - (short)1, - (short)1,
(short)102, - (short)1, (short)104, (short)105, - (short)1, (short)93, - (short)1,
(short)95, - (short)1, (short)97, - (short)1, - (short)1, - (short)1, - (short)1,
(short)102, - (short)1, (short)104, (short)105, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, (short)36, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)47, (short)48, - (short)1, (short)36, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)60, (short)47, (short)48, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)60, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1, (short)93,
- (short)1, (short)95, - (short)1, - (short)1, (short)98, (short)85, - (short)1,
- (short)1, - (short)1, (short)36, (short)104, (short)105, - (short)1, (short)93,
- (short)1, (short)95, - (short)1, - (short)1, - (short)1, - (short)1, (short)47,
(short)48, (short)30, - (short)1, (short)104, (short)105, - (short)1, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, (short)60, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)49,
(short)50, - (short)1, - (short)1, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, (short)62, (short)63, -
(short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)93, - (short)1, (short)95, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)104, (short)105, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1,
(short)98, (short)99, - (short)1, (short)101, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, - (short)1, (short)109, (short)110, -
(short)1, (short)112, - (short)1, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, - (short)1, - (short)1, (short)122, (short)123, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)49,
(short)50, - (short)1, - (short)1, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, (short)62, (short)63, -
(short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1,
(short)98, (short)99, - (short)1, (short)101, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, - (short)1, (short)109, (short)110, -
(short)1, (short)112, - (short)1, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, - (short)1, - (short)1, (short)122, (short)123, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)49,
(short)50, - (short)1, - (short)1, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, (short)62, (short)63, -
(short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1,
(short)98, (short)99, - (short)1, (short)101, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, - (short)1, (short)109, (short)110, -
(short)1, (short)112, - (short)1, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, - (short)1, - (short)1, (short)122, (short)123, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)49,
(short)50, - (short)1, - (short)1, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, (short)62, (short)63, -
(short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1,
(short)98, - (short)1, - (short)1, (short)101, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, - (short)1, (short)109, (short)110, -
(short)1, (short)112, - (short)1, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, - (short)1, - (short)1, (short)122, (short)123, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)49,
(short)50, - (short)1, - (short)1, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, (short)62, (short)63, -
(short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)101, - (short)1, - (short)1, -
(short)1, (short)105, (short)35, - (short)1, (short)37, - (short)1, (short)39,
(short)111, (short)112, (short)42, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, (short)49, (short)50, (short)122, (short)123, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, -
(short)1, (short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, (short)89, (short)90, (short)91, (short)92, - (short)1, (short)94, -
(short)1, - (short)1, - (short)1, (short)98, - (short)1, - (short)1, (short)101,
- (short)1, - (short)1, - (short)1, (short)105, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, (short)112, (short)42, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, (short)49, (short)50, (short)122,
(short)123, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, - (short)1, (short)62, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, -
(short)1, (short)94, - (short)1, - (short)1, (short)97, - (short)1, - (short)1,
- (short)1, (short)101, - (short)1, - (short)1, - (short)1, (short)105, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, (short)112, (short)42,
- (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1, (short)49,
(short)50, (short)122, (short)123, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, (short)62, (short)63, -
(short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)101, - (short)1, - (short)1, -
(short)1, (short)105, (short)35, - (short)1, (short)37, - (short)1, (short)39,
(short)111, (short)112, (short)42, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, (short)49, (short)50, (short)122, (short)123, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, -
(short)1, (short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, (short)89, (short)90, (short)91, (short)92, - (short)1, (short)94, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)101,
(short)102, - (short)1, - (short)1, (short)105, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, (short)112, (short)42, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, (short)49, (short)50, (short)122,
(short)123, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, - (short)1, (short)62, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, -
(short)1, (short)94, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)101, (short)102, - (short)1, - (short)1, (short)105, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, (short)112, (short)42,
- (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1, (short)49,
(short)50, (short)122, (short)123, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, (short)62, (short)63, -
(short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)101, - (short)1, - (short)1, -
(short)1, (short)105, (short)35, - (short)1, (short)37, - (short)1, (short)39,
(short)111, (short)112, (short)42, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, (short)49, (short)50, (short)122, (short)123, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, -
(short)1, (short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, (short)89, (short)90, (short)91, (short)92, - (short)1, (short)94, -
(short)1, - (short)1, (short)97, - (short)1, - (short)1, - (short)1, (short)101,
- (short)1, - (short)1, - (short)1, (short)105, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, (short)112, (short)42, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, (short)49, (short)50, (short)122,
(short)123, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, - (short)1, (short)62, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, -
(short)1, (short)94, - (short)1, - (short)1, (short)97, - (short)1, - (short)1,
- (short)1, (short)101, - (short)1, - (short)1, - (short)1, (short)105, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, (short)112, (short)42,
- (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1, (short)49,
(short)50, (short)122, (short)123, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, (short)62, (short)63, -
(short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)101, (short)102, - (short)1, -
(short)1, (short)105, (short)35, - (short)1, (short)37, - (short)1, (short)39, -
(short)1, (short)112, (short)42, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, (short)49, (short)50, (short)122, (short)123, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, -
(short)1, (short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, (short)89, (short)90, (short)91, (short)92, - (short)1, (short)94, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)101,
(short)102, - (short)1, - (short)1, (short)105, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, (short)112, (short)42, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, (short)49, (short)50, (short)122,
(short)123, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, - (short)1, (short)62, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, -
(short)1, (short)94, - (short)1, - (short)1, (short)97, - (short)1, - (short)1,
- (short)1, (short)101, - (short)1, - (short)1, - (short)1, (short)105, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, (short)112, (short)42,
- (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1, (short)49,
(short)50, (short)122, (short)123, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, (short)62, (short)63, -
(short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)101, (short)102, - (short)1, -
(short)1, (short)105, (short)35, - (short)1, (short)37, - (short)1, (short)39, -
(short)1, (short)112, (short)42, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, (short)49, (short)50, (short)122, (short)123, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, -
(short)1, (short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, (short)89, (short)90, (short)91, (short)92, - (short)1, (short)94, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)101,
(short)102, - (short)1, - (short)1, (short)105, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, (short)112, (short)42, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, (short)49, (short)50, (short)122,
(short)123, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, - (short)1, (short)62, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, -
(short)1, (short)94, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)101, (short)102, - (short)1, - (short)1, (short)105, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, (short)112, (short)42,
- (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1, (short)49,
(short)50, (short)122, (short)123, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, (short)62, (short)63, -
(short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)101, (short)102, - (short)1, -
(short)1, (short)105, (short)35, - (short)1, (short)37, - (short)1, (short)39, -
(short)1, (short)112, (short)42, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, (short)49, (short)50, (short)122, (short)123, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, -
(short)1, (short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, (short)89, (short)90, (short)91, (short)92, - (short)1, (short)94, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)101,
- (short)1, - (short)1, - (short)1, (short)105, (short)35, - (short)1, (short)37,
- (short)1, - (short)1, - (short)1, (short)112, - (short)1, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, (short)49, (short)50, (short)122,
(short)123, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, - (short)1, - (short)1, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, -
(short)1, (short)94, - (short)1, - (short)1, - (short)1, (short)98, - (short)1,
- (short)1, (short)101, - (short)1, - (short)1, - (short)1, (short)105, (short)35,
- (short)1, (short)37, - (short)1, - (short)1, - (short)1, (short)112, - (short)1,
- (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1, (short)49,
(short)50, (short)122, (short)123, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, - (short)1, (short)63,
- (short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1,
(short)98, - (short)1, - (short)1, (short)101, - (short)1, - (short)1, - (short)1,
(short)105, (short)35, - (short)1, (short)37, - (short)1, - (short)1, - (short)1,
(short)112, - (short)1, - (short)1, (short)115, (short)116, (short)117, -
(short)1, - (short)1, (short)49, (short)50, (short)122, (short)123, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, -
(short)1, - (short)1, (short)63, - (short)1, (short)65, (short)66, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, (short)89, (short)90, (short)91, (short)92, - (short)1, (short)94, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)101,
- (short)1, - (short)1, - (short)1, (short)105, (short)35, - (short)1, (short)37,
- (short)1, - (short)1, - (short)1, (short)112, - (short)1, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, (short)49, (short)50, (short)122,
(short)123, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, - (short)1, - (short)1, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, -
(short)1, (short)94, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)101, - (short)1, - (short)1, - (short)1, (short)105, (short)35,
- (short)1, (short)37, - (short)1, - (short)1, - (short)1, (short)112, - (short)1,
- (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1, (short)49,
(short)50, (short)122, (short)123, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, - (short)1, (short)63,
- (short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)101, - (short)1, - (short)1, -
(short)1, (short)105, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)112, - (short)1, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, - (short)1, - (short)1, (short)122, (short)123}; char
Cyc_Yyimpossible_tag[ 13u]="Yyimpossible"; struct Cyc_Yyimpossible_struct{ char*
tag; } ; char Cyc_Yystack_overflow_tag[ 17u]="Yystack_overflow"; struct Cyc_Yystack_overflow_struct{
char* tag; } ; extern void Cyc_yyerror( struct _tagged_string); extern int Cyc_yylex();
char Cyc_YYINITIALSVAL_tag[ 14u]="YYINITIALSVAL"; struct Cyc_YYINITIALSVAL_struct{
char* tag; } ; static int Cyc_yychar=( int)'\000'; struct Cyc_YYINITIALSVAL_struct
Cyc_yyinitval={ Cyc_YYINITIALSVAL_tag}; struct _xtunion_struct* Cyc_yylval=(
struct _xtunion_struct*)& Cyc_yyinitval; static int Cyc_yynerrs= 0; int Cyc_yyparse(){
int yystate; int yyn= 0; int yyerrstatus; int yychar1= 0; int yyssp_offset;
struct _tagged_ptr1 yyss=( struct _tagged_ptr1)({ short* _temp2149=( short*)({
unsigned int _temp2145= 200u; short* _temp2146=( short*) GC_malloc_atomic(
sizeof( short) * _temp2145);{ unsigned int _temp2147= _temp2145; unsigned int i;
for( i= 0; i < _temp2147; i ++){ _temp2146[ i]= (short)0;}}; _temp2146;});
struct _tagged_ptr1 _temp2150; _temp2150.curr= _temp2149; _temp2150.base=
_temp2149; _temp2150.last_plus_one= _temp2149 + 200; _temp2150;}); int
yyvsp_offset; struct _tagged_ptr2 yyvs=( struct _tagged_ptr2)({ struct
_xtunion_struct** _temp2155=( struct _xtunion_struct**)({ unsigned int _temp2151=
200u; struct _xtunion_struct** _temp2152=( struct _xtunion_struct**) GC_malloc(
sizeof( struct _xtunion_struct*) * _temp2151);{ unsigned int _temp2153=
_temp2151; unsigned int i; for( i= 0; i < _temp2153; i ++){ _temp2152[ i]= Cyc_yylval;}};
_temp2152;}); struct _tagged_ptr2 _temp2156; _temp2156.curr= _temp2155;
_temp2156.base= _temp2155; _temp2156.last_plus_one= _temp2155 + 200; _temp2156;});
int yylsp_offset; struct _tagged_ptr3 yyls=( struct _tagged_ptr3)({ struct Cyc_Yyltype*
_temp2161=( struct Cyc_Yyltype*)({ unsigned int _temp2157= 200u; struct Cyc_Yyltype*
_temp2158=( struct Cyc_Yyltype*) GC_malloc( sizeof( struct Cyc_Yyltype) *
_temp2157);{ unsigned int _temp2159= _temp2157; unsigned int i; for( i= 0; i <
_temp2159; i ++){ _temp2158[ i]= Cyc_yynewloc();}}; _temp2158;}); struct
_tagged_ptr3 _temp2162; _temp2162.curr= _temp2161; _temp2162.base= _temp2161;
_temp2162.last_plus_one= _temp2161 + 200; _temp2162;}); int yystacksize= 200;
struct _xtunion_struct* yyval= Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0;
Cyc_yynerrs= 0; Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset= 0;
yylsp_offset= 0; yynewstate:({ struct _tagged_ptr1 _temp2163= yyss; short*
_temp2165= _temp2163.curr +( ++ yyssp_offset); if( _temp2163.base == 0? 1:(
_temp2165 < _temp2163.base? 1: _temp2165 >= _temp2163.last_plus_one)){ _throw(
Null_Exception);}* _temp2165=( short) yystate;}); if( yyssp_offset >=
yystacksize - 1){ if( yystacksize >= 10000){ Cyc_yyerror(( struct _tagged_string)({
char* _temp2166=( char*)"parser stack overflow"; struct _tagged_string _temp2167;
_temp2167.curr= _temp2166; _temp2167.base= _temp2166; _temp2167.last_plus_one=
_temp2166 + 22; _temp2167;}));( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_Yystack_overflow_struct* _temp2168=( struct Cyc_Yystack_overflow_struct*)
GC_malloc( sizeof( struct Cyc_Yystack_overflow_struct)); _temp2168[ 0]=({ struct
Cyc_Yystack_overflow_struct _temp2169; _temp2169.tag= Cyc_Yystack_overflow_tag;
_temp2169;}); _temp2168;}));} yystacksize *= 2; if( yystacksize > 10000){
yystacksize= 10000;}{ struct _tagged_ptr1 yyss1=({ unsigned int _temp2170=(
unsigned int) yystacksize; short* _temp2171=( short*) GC_malloc_atomic( sizeof(
short) * _temp2170); struct _tagged_ptr1 _temp2174={ _temp2171, _temp2171,
_temp2171 + _temp2170};{ unsigned int _temp2172= _temp2170; unsigned int i; for(
i= 0; i < _temp2172; i ++){ _temp2171[ i]= (short)0;}}; _temp2174;}); struct
_tagged_ptr2 yyvs1=({ unsigned int _temp2175=( unsigned int) yystacksize; struct
_xtunion_struct** _temp2176=( struct _xtunion_struct**) GC_malloc( sizeof(
struct _xtunion_struct*) * _temp2175); struct _tagged_ptr2 _temp2179={ _temp2176,
_temp2176, _temp2176 + _temp2175};{ unsigned int _temp2177= _temp2175;
unsigned int i; for( i= 0; i < _temp2177; i ++){ _temp2176[ i]= Cyc_yylval;}};
_temp2179;}); struct _tagged_ptr3 yyls1=({ unsigned int _temp2180=( unsigned int)
yystacksize; struct Cyc_Yyltype* _temp2181=( struct Cyc_Yyltype*) GC_malloc(
sizeof( struct Cyc_Yyltype) * _temp2180); struct _tagged_ptr3 _temp2184={
_temp2181, _temp2181, _temp2181 + _temp2180};{ unsigned int _temp2182= _temp2180;
unsigned int i; for( i= 0; i < _temp2182; i ++){ _temp2181[ i]= Cyc_yynewloc();}};
_temp2184;});{ int i= 0; for( 0; i <= yyssp_offset; i ++){({ struct _tagged_ptr1
_temp2185= yyss1; short* _temp2187= _temp2185.curr + i; if( _temp2185.base == 0?
1:( _temp2187 < _temp2185.base? 1: _temp2187 >= _temp2185.last_plus_one)){
_throw( Null_Exception);}* _temp2187=({ struct _tagged_ptr1 _temp2188= yyss;
short* _temp2190= _temp2188.curr + i; if( _temp2188.base == 0? 1:( _temp2190 <
_temp2188.base? 1: _temp2190 >= _temp2188.last_plus_one)){ _throw(
Null_Exception);}* _temp2190;});});({ struct _tagged_ptr2 _temp2191= yyvs1;
struct _xtunion_struct** _temp2193= _temp2191.curr + i; if( _temp2191.base == 0?
1:( _temp2193 < _temp2191.base? 1: _temp2193 >= _temp2191.last_plus_one)){
_throw( Null_Exception);}* _temp2193=({ struct _tagged_ptr2 _temp2194= yyvs;
struct _xtunion_struct** _temp2196= _temp2194.curr + i; if( _temp2194.base == 0?
1:( _temp2196 < _temp2194.base? 1: _temp2196 >= _temp2194.last_plus_one)){
_throw( Null_Exception);}* _temp2196;});});({ struct _tagged_ptr3 _temp2197=
yyls1; struct Cyc_Yyltype* _temp2199= _temp2197.curr + i; if( _temp2197.base ==
0? 1:( _temp2199 < _temp2197.base? 1: _temp2199 >= _temp2197.last_plus_one)){
_throw( Null_Exception);}* _temp2199=({ struct _tagged_ptr3 _temp2200= yyls;
struct Cyc_Yyltype* _temp2202= _temp2200.curr + i; if( _temp2200.base == 0? 1:(
_temp2202 < _temp2200.base? 1: _temp2202 >= _temp2200.last_plus_one)){ _throw(
Null_Exception);}* _temp2202;});});}} yyss= yyss1; yyvs= yyvs1; yyls= yyls1;}}
goto yybackup; yybackup: yyn=( int)({ short* _temp2203=( short*) Cyc_yypact;
unsigned int _temp2204= yystate; if( _temp2204 >= 755u){ _throw( Null_Exception);}
_temp2203[ _temp2204];}); if( yyn == - 32768){ goto yydefault;} if( Cyc_yychar
== - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 0){ yychar1= 0; Cyc_yychar=
0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <= 351: 0)?( int)({ short*
_temp2205=( short*) Cyc_yytranslate; unsigned int _temp2206= Cyc_yychar; if(
_temp2206 >= 352u){ _throw( Null_Exception);} _temp2205[ _temp2206];}): 230;}
yyn += yychar1; if(( yyn < 0? 1: yyn > 4214)? 1:({ short* _temp2207=( short*)
Cyc_yycheck; unsigned int _temp2208= yyn; if( _temp2208 >= 4215u){ _throw(
Null_Exception);} _temp2207[ _temp2208];}) != yychar1){ goto yydefault;} yyn=(
int)({ short* _temp2209=( short*) Cyc_yytable; unsigned int _temp2210= yyn; if(
_temp2210 >= 4215u){ _throw( Null_Exception);} _temp2209[ _temp2210];}); if( yyn
< 0){ if( yyn == - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if(
yyn == 0){ goto yyerrlab;}} if( yyn == 754){ return 0;} if( Cyc_yychar != 0){
Cyc_yychar= - 2;}({ struct _tagged_ptr2 _temp2211= yyvs; struct _xtunion_struct**
_temp2213= _temp2211.curr +( ++ yyvsp_offset); if( _temp2211.base == 0? 1:(
_temp2213 < _temp2211.base? 1: _temp2213 >= _temp2211.last_plus_one)){ _throw(
Null_Exception);}* _temp2213= Cyc_yylval;});({ struct _tagged_ptr3 _temp2214=
yyls; struct Cyc_Yyltype* _temp2216= _temp2214.curr +( ++ yylsp_offset); if(
_temp2214.base == 0? 1:( _temp2216 < _temp2214.base? 1: _temp2216 >= _temp2214.last_plus_one)){
_throw( Null_Exception);}* _temp2216= Cyc_yylloc;}); if( yyerrstatus != 0){
yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int)({ short*
_temp2217=( short*) Cyc_yydefact; unsigned int _temp2218= yystate; if( _temp2218
>= 755u){ _throw( Null_Exception);} _temp2217[ _temp2218];}); if( yyn == 0){
goto yyerrlab;} yyreduce: yylen=( int)({ short* _temp2219=( short*) Cyc_yyr2;
unsigned int _temp2220= yyn; if( _temp2220 >= 393u){ _throw( Null_Exception);}
_temp2219[ _temp2220];}); if( yylen > 0){ yyval=({ struct _tagged_ptr2 _temp2221=
yyvs; struct _xtunion_struct** _temp2223= _temp2221.curr +(( yyvsp_offset + 1) -
yylen); if( _temp2221.base == 0? 1:( _temp2223 < _temp2221.base? 1: _temp2223 >=
_temp2221.last_plus_one)){ _throw( Null_Exception);}* _temp2223;});} switch( yyn){
case 1: _LL2224: yyval=({ struct _tagged_ptr2 _temp2226= yyvs; struct
_xtunion_struct** _temp2228= _temp2226.curr + yyvsp_offset; if( _temp2226.base
== 0? 1:( _temp2228 < _temp2226.base? 1: _temp2228 >= _temp2226.last_plus_one)){
_throw( Null_Exception);}* _temp2228;}); Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2229= yyvs; struct _xtunion_struct** _temp2231=
_temp2229.curr + yyvsp_offset; if( _temp2229.base == 0? 1:( _temp2231 <
_temp2229.base? 1: _temp2231 >= _temp2229.last_plus_one)){ _throw(
Null_Exception);}* _temp2231;})); break; case 2: _LL2225: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2233=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2233[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2234; _temp2234.tag= Cyc_DeclList_tok_tag; _temp2234.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2235= yyvs; struct _xtunion_struct** _temp2237=
_temp2235.curr +( yyvsp_offset - 1); if( _temp2235.base == 0? 1:( _temp2237 <
_temp2235.base? 1: _temp2237 >= _temp2235.last_plus_one)){ _throw(
Null_Exception);}* _temp2237;})), Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2238= yyvs; struct _xtunion_struct** _temp2240= _temp2238.curr +
yyvsp_offset; if( _temp2238.base == 0? 1:( _temp2240 < _temp2238.base? 1:
_temp2240 >= _temp2238.last_plus_one)){ _throw( Null_Exception);}* _temp2240;})));
_temp2234;}); _temp2233;}); break; case 3: _LL2232: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2242=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2242[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2243; _temp2243.tag= Cyc_DeclList_tok_tag; _temp2243.f1=({ struct Cyc_List_List*
_temp2244=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2244->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2245=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2245->r=( void*)(( void*)({
struct Cyc_Absyn_Using_d_struct* _temp2252=( struct Cyc_Absyn_Using_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp2252[ 0]=({ struct
Cyc_Absyn_Using_d_struct _temp2253; _temp2253.tag= Cyc_Absyn_Using_d_tag;
_temp2253.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2254= yyvs;
struct _xtunion_struct** _temp2256= _temp2254.curr +( yyvsp_offset - 2); if(
_temp2254.base == 0? 1:( _temp2256 < _temp2254.base? 1: _temp2256 >= _temp2254.last_plus_one)){
_throw( Null_Exception);}* _temp2256;})); _temp2253.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2257= yyvs; struct _xtunion_struct** _temp2259=
_temp2257.curr + yyvsp_offset; if( _temp2257.base == 0? 1:( _temp2259 <
_temp2257.base? 1: _temp2259 >= _temp2257.last_plus_one)){ _throw(
Null_Exception);}* _temp2259;})); _temp2253;}); _temp2252;})); _temp2245->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2246= yyls; struct Cyc_Yyltype*
_temp2248= _temp2246.curr +( yylsp_offset - 2); if( _temp2246.base == 0? 1:(
_temp2248 < _temp2246.base? 1: _temp2248 >= _temp2246.last_plus_one)){ _throw(
Null_Exception);}* _temp2248;}).first_line,({ struct _tagged_ptr3 _temp2249=
yyls; struct Cyc_Yyltype* _temp2251= _temp2249.curr + yylsp_offset; if(
_temp2249.base == 0? 1:( _temp2251 < _temp2249.base? 1: _temp2251 >= _temp2249.last_plus_one)){
_throw( Null_Exception);}* _temp2251;}).last_line); _temp2245;}); _temp2244->tl=
0; _temp2244;}); _temp2243;}); _temp2242;}); Cyc_Lex_leave_using(); break; case
4: _LL2241: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2261=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2261[ 0]=({ struct Cyc_DeclList_tok_struct _temp2262; _temp2262.tag= Cyc_DeclList_tok_tag;
_temp2262.f1=({ struct Cyc_List_List* _temp2263=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2263->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2267=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2267->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2274=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2274[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2275; _temp2275.tag= Cyc_Absyn_Using_d_tag;
_temp2275.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2276= yyvs;
struct _xtunion_struct** _temp2278= _temp2276.curr +( yyvsp_offset - 4); if(
_temp2276.base == 0? 1:( _temp2278 < _temp2276.base? 1: _temp2278 >= _temp2276.last_plus_one)){
_throw( Null_Exception);}* _temp2278;})); _temp2275.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2279= yyvs; struct _xtunion_struct** _temp2281=
_temp2279.curr +( yyvsp_offset - 2); if( _temp2279.base == 0? 1:( _temp2281 <
_temp2279.base? 1: _temp2281 >= _temp2279.last_plus_one)){ _throw(
Null_Exception);}* _temp2281;})); _temp2275;}); _temp2274;})); _temp2267->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2268= yyls; struct Cyc_Yyltype*
_temp2270= _temp2268.curr +( yylsp_offset - 4); if( _temp2268.base == 0? 1:(
_temp2270 < _temp2268.base? 1: _temp2270 >= _temp2268.last_plus_one)){ _throw(
Null_Exception);}* _temp2270;}).first_line,({ struct _tagged_ptr3 _temp2271=
yyls; struct Cyc_Yyltype* _temp2273= _temp2271.curr +( yylsp_offset - 1); if(
_temp2271.base == 0? 1:( _temp2273 < _temp2271.base? 1: _temp2273 >= _temp2271.last_plus_one)){
_throw( Null_Exception);}* _temp2273;}).last_line); _temp2267;}); _temp2263->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2264= yyvs; struct
_xtunion_struct** _temp2266= _temp2264.curr + yyvsp_offset; if( _temp2264.base
== 0? 1:( _temp2266 < _temp2264.base? 1: _temp2266 >= _temp2264.last_plus_one)){
_throw( Null_Exception);}* _temp2266;})); _temp2263;}); _temp2262;}); _temp2261;});
break; case 5: _LL2260: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2283=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2283[ 0]=({ struct Cyc_DeclList_tok_struct _temp2284; _temp2284.tag= Cyc_DeclList_tok_tag;
_temp2284.f1=({ struct Cyc_List_List* _temp2285=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2285->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2286=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2286->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2293=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2293[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2294; _temp2294.tag=
Cyc_Absyn_Namespace_d_tag; _temp2294.f1=({ struct _tagged_string* _temp2295=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp2295[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2296= yyvs;
struct _xtunion_struct** _temp2298= _temp2296.curr +( yyvsp_offset - 2); if(
_temp2296.base == 0? 1:( _temp2298 < _temp2296.base? 1: _temp2298 >= _temp2296.last_plus_one)){
_throw( Null_Exception);}* _temp2298;})); _temp2295;}); _temp2294.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2299= yyvs; struct _xtunion_struct** _temp2301=
_temp2299.curr + yyvsp_offset; if( _temp2299.base == 0? 1:( _temp2301 <
_temp2299.base? 1: _temp2301 >= _temp2299.last_plus_one)){ _throw(
Null_Exception);}* _temp2301;})); _temp2294;}); _temp2293;})); _temp2286->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2287= yyls; struct Cyc_Yyltype*
_temp2289= _temp2287.curr +( yylsp_offset - 2); if( _temp2287.base == 0? 1:(
_temp2289 < _temp2287.base? 1: _temp2289 >= _temp2287.last_plus_one)){ _throw(
Null_Exception);}* _temp2289;}).first_line,({ struct _tagged_ptr3 _temp2290=
yyls; struct Cyc_Yyltype* _temp2292= _temp2290.curr + yylsp_offset; if(
_temp2290.base == 0? 1:( _temp2292 < _temp2290.base? 1: _temp2292 >= _temp2290.last_plus_one)){
_throw( Null_Exception);}* _temp2292;}).last_line); _temp2286;}); _temp2285->tl=
0; _temp2285;}); _temp2284;}); _temp2283;}); Cyc_Lex_leave_namespace(); break;
case 6: _LL2282: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2303=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2303[ 0]=({ struct Cyc_DeclList_tok_struct _temp2304; _temp2304.tag= Cyc_DeclList_tok_tag;
_temp2304.f1=({ struct Cyc_List_List* _temp2305=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2305->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2309=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2309->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2316=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2316[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2317; _temp2317.tag=
Cyc_Absyn_Namespace_d_tag; _temp2317.f1=({ struct _tagged_string* _temp2318=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp2318[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2319= yyvs;
struct _xtunion_struct** _temp2321= _temp2319.curr +( yyvsp_offset - 4); if(
_temp2319.base == 0? 1:( _temp2321 < _temp2319.base? 1: _temp2321 >= _temp2319.last_plus_one)){
_throw( Null_Exception);}* _temp2321;})); _temp2318;}); _temp2317.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2322= yyvs; struct _xtunion_struct** _temp2324=
_temp2322.curr +( yyvsp_offset - 2); if( _temp2322.base == 0? 1:( _temp2324 <
_temp2322.base? 1: _temp2324 >= _temp2322.last_plus_one)){ _throw(
Null_Exception);}* _temp2324;})); _temp2317;}); _temp2316;})); _temp2309->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2310= yyls; struct Cyc_Yyltype*
_temp2312= _temp2310.curr +( yylsp_offset - 4); if( _temp2310.base == 0? 1:(
_temp2312 < _temp2310.base? 1: _temp2312 >= _temp2310.last_plus_one)){ _throw(
Null_Exception);}* _temp2312;}).first_line,({ struct _tagged_ptr3 _temp2313=
yyls; struct Cyc_Yyltype* _temp2315= _temp2313.curr +( yylsp_offset - 1); if(
_temp2313.base == 0? 1:( _temp2315 < _temp2313.base? 1: _temp2315 >= _temp2313.last_plus_one)){
_throw( Null_Exception);}* _temp2315;}).last_line); _temp2309;}); _temp2305->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2306= yyvs; struct
_xtunion_struct** _temp2308= _temp2306.curr + yyvsp_offset; if( _temp2306.base
== 0? 1:( _temp2308 < _temp2306.base? 1: _temp2308 >= _temp2306.last_plus_one)){
_throw( Null_Exception);}* _temp2308;})); _temp2305;}); _temp2304;}); _temp2303;});
break; case 7: _LL2302: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2326= yyvs; struct _xtunion_struct** _temp2328= _temp2326.curr
+( yyvsp_offset - 4); if( _temp2326.base == 0? 1:( _temp2328 < _temp2326.base? 1:
_temp2328 >= _temp2326.last_plus_one)){ _throw( Null_Exception);}* _temp2328;})),(
struct _tagged_string)({ char* _temp2329=( char*)"C"; struct _tagged_string
_temp2330; _temp2330.curr= _temp2329; _temp2330.base= _temp2329; _temp2330.last_plus_one=
_temp2329 + 2; _temp2330;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp2331=( char*)"only extern \"C\" { ... } is allowed"; struct
_tagged_string _temp2332; _temp2332.curr= _temp2331; _temp2332.base= _temp2331;
_temp2332.last_plus_one= _temp2331 + 35; _temp2332;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2333= yyls; struct Cyc_Yyltype* _temp2335= _temp2333.curr
+( yylsp_offset - 5); if( _temp2333.base == 0? 1:( _temp2335 < _temp2333.base? 1:
_temp2335 >= _temp2333.last_plus_one)){ _throw( Null_Exception);}* _temp2335;}).first_line,({
struct _tagged_ptr3 _temp2336= yyls; struct Cyc_Yyltype* _temp2338= _temp2336.curr
+( yylsp_offset - 4); if( _temp2336.base == 0? 1:( _temp2338 < _temp2336.base? 1:
_temp2338 >= _temp2336.last_plus_one)){ _throw( Null_Exception);}* _temp2338;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2339=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2339[ 0]=({ struct Cyc_DeclList_tok_struct _temp2340; _temp2340.tag= Cyc_DeclList_tok_tag;
_temp2340.f1=({ struct Cyc_List_List* _temp2341=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2341->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2345=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2345->r=( void*)(( void*)({ struct Cyc_Absyn_ExternC_d_struct* _temp2352=(
struct Cyc_Absyn_ExternC_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct));
_temp2352[ 0]=({ struct Cyc_Absyn_ExternC_d_struct _temp2353; _temp2353.tag= Cyc_Absyn_ExternC_d_tag;
_temp2353.f1= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2354= yyvs;
struct _xtunion_struct** _temp2356= _temp2354.curr +( yyvsp_offset - 2); if(
_temp2354.base == 0? 1:( _temp2356 < _temp2354.base? 1: _temp2356 >= _temp2354.last_plus_one)){
_throw( Null_Exception);}* _temp2356;})); _temp2353;}); _temp2352;})); _temp2345->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2346= yyls; struct Cyc_Yyltype*
_temp2348= _temp2346.curr +( yylsp_offset - 5); if( _temp2346.base == 0? 1:(
_temp2348 < _temp2346.base? 1: _temp2348 >= _temp2346.last_plus_one)){ _throw(
Null_Exception);}* _temp2348;}).first_line,({ struct _tagged_ptr3 _temp2349=
yyls; struct Cyc_Yyltype* _temp2351= _temp2349.curr +( yylsp_offset - 1); if(
_temp2349.base == 0? 1:( _temp2351 < _temp2349.base? 1: _temp2351 >= _temp2349.last_plus_one)){
_throw( Null_Exception);}* _temp2351;}).last_line); _temp2345;}); _temp2341->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2342= yyvs; struct
_xtunion_struct** _temp2344= _temp2342.curr + yyvsp_offset; if( _temp2342.base
== 0? 1:( _temp2344 < _temp2342.base? 1: _temp2344 >= _temp2342.last_plus_one)){
_throw( Null_Exception);}* _temp2344;})); _temp2341;}); _temp2340;}); _temp2339;});
break; case 8: _LL2325: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2358=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2358[ 0]=({ struct Cyc_DeclList_tok_struct _temp2359; _temp2359.tag= Cyc_DeclList_tok_tag;
_temp2359.f1= 0; _temp2359;}); _temp2358;}); break; case 9: _LL2357: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2361=( struct
Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2361[ 0]=({ struct Cyc_DeclList_tok_struct _temp2362; _temp2362.tag= Cyc_DeclList_tok_tag;
_temp2362.f1=({ struct Cyc_List_List* _temp2363=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2363->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2364=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp2364[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp2365; _temp2365.tag= Cyc_Absyn_Fn_d_tag; _temp2365.f1= Cyc_yyget_FnDecl_tok(({
struct _tagged_ptr2 _temp2366= yyvs; struct _xtunion_struct** _temp2368=
_temp2366.curr + yyvsp_offset; if( _temp2366.base == 0? 1:( _temp2368 <
_temp2366.base? 1: _temp2368 >= _temp2366.last_plus_one)){ _throw(
Null_Exception);}* _temp2368;})); _temp2365;}); _temp2364;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2369= yyls; struct Cyc_Yyltype* _temp2371= _temp2369.curr
+ yylsp_offset; if( _temp2369.base == 0? 1:( _temp2371 < _temp2369.base? 1:
_temp2371 >= _temp2369.last_plus_one)){ _throw( Null_Exception);}* _temp2371;}).first_line,({
struct _tagged_ptr3 _temp2372= yyls; struct Cyc_Yyltype* _temp2374= _temp2372.curr
+ yylsp_offset; if( _temp2372.base == 0? 1:( _temp2374 < _temp2372.base? 1:
_temp2374 >= _temp2372.last_plus_one)){ _throw( Null_Exception);}* _temp2374;}).last_line));
_temp2363->tl= 0; _temp2363;}); _temp2362;}); _temp2361;}); break; case 10:
_LL2360: yyval=({ struct _tagged_ptr2 _temp2376= yyvs; struct _xtunion_struct**
_temp2378= _temp2376.curr + yyvsp_offset; if( _temp2376.base == 0? 1:( _temp2378
< _temp2376.base? 1: _temp2378 >= _temp2376.last_plus_one)){ _throw(
Null_Exception);}* _temp2378;}); break; case 11: _LL2375: yyval=( struct
_xtunion_struct*)({ struct Cyc_FnDecl_tok_struct* _temp2380=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2380[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2381; _temp2381.tag= Cyc_FnDecl_tok_tag; _temp2381.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp2382= yyvs; struct
_xtunion_struct** _temp2384= _temp2382.curr +( yyvsp_offset - 1); if( _temp2382.base
== 0? 1:( _temp2384 < _temp2382.base? 1: _temp2384 >= _temp2382.last_plus_one)){
_throw( Null_Exception);}* _temp2384;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2385= yyvs; struct _xtunion_struct** _temp2387= _temp2385.curr
+ yyvsp_offset; if( _temp2385.base == 0? 1:( _temp2387 < _temp2385.base? 1:
_temp2387 >= _temp2385.last_plus_one)){ _throw( Null_Exception);}* _temp2387;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2388= yyls; struct Cyc_Yyltype*
_temp2390= _temp2388.curr +( yylsp_offset - 1); if( _temp2388.base == 0? 1:(
_temp2390 < _temp2388.base? 1: _temp2390 >= _temp2388.last_plus_one)){ _throw(
Null_Exception);}* _temp2390;}).first_line,({ struct _tagged_ptr3 _temp2391=
yyls; struct Cyc_Yyltype* _temp2393= _temp2391.curr + yylsp_offset; if(
_temp2391.base == 0? 1:( _temp2393 < _temp2391.base? 1: _temp2393 >= _temp2391.last_plus_one)){
_throw( Null_Exception);}* _temp2393;}).last_line)); _temp2381;}); _temp2380;});
break; case 12: _LL2379: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2395=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2395[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2396; _temp2396.tag= Cyc_FnDecl_tok_tag;
_temp2396.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2397=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2397->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2398= yyvs; struct
_xtunion_struct** _temp2400= _temp2398.curr +( yyvsp_offset - 2); if( _temp2398.base
== 0? 1:( _temp2400 < _temp2398.base? 1: _temp2400 >= _temp2398.last_plus_one)){
_throw( Null_Exception);}* _temp2400;})); _temp2397;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2401= yyvs; struct _xtunion_struct** _temp2403=
_temp2401.curr +( yyvsp_offset - 1); if( _temp2401.base == 0? 1:( _temp2403 <
_temp2401.base? 1: _temp2403 >= _temp2401.last_plus_one)){ _throw(
Null_Exception);}* _temp2403;})), 0, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2404= yyvs; struct _xtunion_struct** _temp2406= _temp2404.curr +
yyvsp_offset; if( _temp2404.base == 0? 1:( _temp2406 < _temp2404.base? 1:
_temp2406 >= _temp2404.last_plus_one)){ _throw( Null_Exception);}* _temp2406;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2407= yyls; struct Cyc_Yyltype*
_temp2409= _temp2407.curr +( yylsp_offset - 2); if( _temp2407.base == 0? 1:(
_temp2409 < _temp2407.base? 1: _temp2409 >= _temp2407.last_plus_one)){ _throw(
Null_Exception);}* _temp2409;}).first_line,({ struct _tagged_ptr3 _temp2410=
yyls; struct Cyc_Yyltype* _temp2412= _temp2410.curr + yylsp_offset; if(
_temp2410.base == 0? 1:( _temp2412 < _temp2410.base? 1: _temp2412 >= _temp2410.last_plus_one)){
_throw( Null_Exception);}* _temp2412;}).last_line)); _temp2396;}); _temp2395;});
break; case 13: _LL2394: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2414=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2414[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2415; _temp2415.tag= Cyc_FnDecl_tok_tag;
_temp2415.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp2416= yyvs; struct _xtunion_struct** _temp2418= _temp2416.curr
+( yyvsp_offset - 2); if( _temp2416.base == 0? 1:( _temp2418 < _temp2416.base? 1:
_temp2418 >= _temp2416.last_plus_one)){ _throw( Null_Exception);}* _temp2418;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2419= yyvs; struct
_xtunion_struct** _temp2421= _temp2419.curr +( yyvsp_offset - 1); if( _temp2419.base
== 0? 1:( _temp2421 < _temp2419.base? 1: _temp2421 >= _temp2419.last_plus_one)){
_throw( Null_Exception);}* _temp2421;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2422= yyvs; struct _xtunion_struct** _temp2424= _temp2422.curr
+ yyvsp_offset; if( _temp2422.base == 0? 1:( _temp2424 < _temp2422.base? 1:
_temp2424 >= _temp2422.last_plus_one)){ _throw( Null_Exception);}* _temp2424;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2425= yyls; struct Cyc_Yyltype*
_temp2427= _temp2425.curr +( yylsp_offset - 2); if( _temp2425.base == 0? 1:(
_temp2427 < _temp2425.base? 1: _temp2427 >= _temp2425.last_plus_one)){ _throw(
Null_Exception);}* _temp2427;}).first_line,({ struct _tagged_ptr3 _temp2428=
yyls; struct Cyc_Yyltype* _temp2430= _temp2428.curr + yylsp_offset; if(
_temp2428.base == 0? 1:( _temp2430 < _temp2428.base? 1: _temp2430 >= _temp2428.last_plus_one)){
_throw( Null_Exception);}* _temp2430;}).last_line)); _temp2415;}); _temp2414;});
break; case 14: _LL2413: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2432=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2432[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2433; _temp2433.tag= Cyc_FnDecl_tok_tag;
_temp2433.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2434=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2434->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2435= yyvs; struct
_xtunion_struct** _temp2437= _temp2435.curr +( yyvsp_offset - 3); if( _temp2435.base
== 0? 1:( _temp2437 < _temp2435.base? 1: _temp2437 >= _temp2435.last_plus_one)){
_throw( Null_Exception);}* _temp2437;})); _temp2434;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2438= yyvs; struct _xtunion_struct** _temp2440=
_temp2438.curr +( yyvsp_offset - 2); if( _temp2438.base == 0? 1:( _temp2440 <
_temp2438.base? 1: _temp2440 >= _temp2438.last_plus_one)){ _throw(
Null_Exception);}* _temp2440;})), Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2441= yyvs; struct _xtunion_struct** _temp2443= _temp2441.curr +(
yyvsp_offset - 1); if( _temp2441.base == 0? 1:( _temp2443 < _temp2441.base? 1:
_temp2443 >= _temp2441.last_plus_one)){ _throw( Null_Exception);}* _temp2443;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2444= yyvs; struct
_xtunion_struct** _temp2446= _temp2444.curr + yyvsp_offset; if( _temp2444.base
== 0? 1:( _temp2446 < _temp2444.base? 1: _temp2446 >= _temp2444.last_plus_one)){
_throw( Null_Exception);}* _temp2446;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2447= yyls; struct Cyc_Yyltype* _temp2449= _temp2447.curr +(
yylsp_offset - 3); if( _temp2447.base == 0? 1:( _temp2449 < _temp2447.base? 1:
_temp2449 >= _temp2447.last_plus_one)){ _throw( Null_Exception);}* _temp2449;}).first_line,({
struct _tagged_ptr3 _temp2450= yyls; struct Cyc_Yyltype* _temp2452= _temp2450.curr
+ yylsp_offset; if( _temp2450.base == 0? 1:( _temp2452 < _temp2450.base? 1:
_temp2452 >= _temp2450.last_plus_one)){ _throw( Null_Exception);}* _temp2452;}).last_line));
_temp2433;}); _temp2432;}); break; case 15: _LL2431: yyval=( struct
_xtunion_struct*)({ struct Cyc_FnDecl_tok_struct* _temp2454=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2454[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2455; _temp2455.tag= Cyc_FnDecl_tok_tag; _temp2455.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2456=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2456->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2457= yyvs; struct _xtunion_struct** _temp2459= _temp2457.curr
+( yyvsp_offset - 2); if( _temp2457.base == 0? 1:( _temp2459 < _temp2457.base? 1:
_temp2459 >= _temp2457.last_plus_one)){ _throw( Null_Exception);}* _temp2459;}));
_temp2456;}), Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp2460= yyvs;
struct _xtunion_struct** _temp2462= _temp2460.curr +( yyvsp_offset - 1); if(
_temp2460.base == 0? 1:( _temp2462 < _temp2460.base? 1: _temp2462 >= _temp2460.last_plus_one)){
_throw( Null_Exception);}* _temp2462;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2463= yyvs; struct _xtunion_struct** _temp2465= _temp2463.curr
+ yyvsp_offset; if( _temp2463.base == 0? 1:( _temp2465 < _temp2463.base? 1:
_temp2465 >= _temp2463.last_plus_one)){ _throw( Null_Exception);}* _temp2465;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2466= yyls; struct Cyc_Yyltype*
_temp2468= _temp2466.curr +( yylsp_offset - 2); if( _temp2466.base == 0? 1:(
_temp2468 < _temp2466.base? 1: _temp2468 >= _temp2466.last_plus_one)){ _throw(
Null_Exception);}* _temp2468;}).first_line,({ struct _tagged_ptr3 _temp2469=
yyls; struct Cyc_Yyltype* _temp2471= _temp2469.curr + yylsp_offset; if(
_temp2469.base == 0? 1:( _temp2471 < _temp2469.base? 1: _temp2471 >= _temp2469.last_plus_one)){
_throw( Null_Exception);}* _temp2471;}).last_line)); _temp2455;}); _temp2454;});
break; case 16: _LL2453: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2473=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2473[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2474; _temp2474.tag= Cyc_FnDecl_tok_tag;
_temp2474.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2475=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2475->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2476= yyvs; struct
_xtunion_struct** _temp2478= _temp2476.curr +( yyvsp_offset - 3); if( _temp2476.base
== 0? 1:( _temp2478 < _temp2476.base? 1: _temp2478 >= _temp2476.last_plus_one)){
_throw( Null_Exception);}* _temp2478;})); _temp2475;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2479= yyvs; struct _xtunion_struct** _temp2481=
_temp2479.curr +( yyvsp_offset - 2); if( _temp2479.base == 0? 1:( _temp2481 <
_temp2479.base? 1: _temp2481 >= _temp2479.last_plus_one)){ _throw(
Null_Exception);}* _temp2481;})), Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2482= yyvs; struct _xtunion_struct** _temp2484= _temp2482.curr +(
yyvsp_offset - 1); if( _temp2482.base == 0? 1:( _temp2484 < _temp2482.base? 1:
_temp2484 >= _temp2482.last_plus_one)){ _throw( Null_Exception);}* _temp2484;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2485= yyvs; struct
_xtunion_struct** _temp2487= _temp2485.curr + yyvsp_offset; if( _temp2485.base
== 0? 1:( _temp2487 < _temp2485.base? 1: _temp2487 >= _temp2485.last_plus_one)){
_throw( Null_Exception);}* _temp2487;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2488= yyls; struct Cyc_Yyltype* _temp2490= _temp2488.curr +(
yylsp_offset - 3); if( _temp2488.base == 0? 1:( _temp2490 < _temp2488.base? 1:
_temp2490 >= _temp2488.last_plus_one)){ _throw( Null_Exception);}* _temp2490;}).first_line,({
struct _tagged_ptr3 _temp2491= yyls; struct Cyc_Yyltype* _temp2493= _temp2491.curr
+ yylsp_offset; if( _temp2491.base == 0? 1:( _temp2493 < _temp2491.base? 1:
_temp2493 >= _temp2491.last_plus_one)){ _throw( Null_Exception);}* _temp2493;}).last_line));
_temp2474;}); _temp2473;}); break; case 17: _LL2472: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2495= yyvs; struct _xtunion_struct** _temp2497=
_temp2495.curr + yyvsp_offset; if( _temp2495.base == 0? 1:( _temp2497 <
_temp2495.base? 1: _temp2497 >= _temp2495.last_plus_one)){ _throw(
Null_Exception);}* _temp2497;}))); yyval=({ struct _tagged_ptr2 _temp2498= yyvs;
struct _xtunion_struct** _temp2500= _temp2498.curr + yyvsp_offset; if( _temp2498.base
== 0? 1:( _temp2500 < _temp2498.base? 1: _temp2500 >= _temp2498.last_plus_one)){
_throw( Null_Exception);}* _temp2500;}); break; case 18: _LL2494: Cyc_Lex_leave_using();
break; case 19: _LL2501: Cyc_Lex_enter_namespace(({ struct _tagged_string*
_temp2503=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2503[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2504= yyvs;
struct _xtunion_struct** _temp2506= _temp2504.curr + yyvsp_offset; if( _temp2504.base
== 0? 1:( _temp2506 < _temp2504.base? 1: _temp2506 >= _temp2504.last_plus_one)){
_throw( Null_Exception);}* _temp2506;})); _temp2503;})); yyval=({ struct
_tagged_ptr2 _temp2507= yyvs; struct _xtunion_struct** _temp2509= _temp2507.curr
+ yyvsp_offset; if( _temp2507.base == 0? 1:( _temp2509 < _temp2507.base? 1:
_temp2509 >= _temp2507.last_plus_one)){ _throw( Null_Exception);}* _temp2509;});
break; case 20: _LL2502: Cyc_Lex_leave_namespace(); break; case 21: _LL2510:
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2512=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2512[ 0]=({ struct Cyc_DeclList_tok_struct _temp2513; _temp2513.tag= Cyc_DeclList_tok_tag;
_temp2513.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2514= yyvs; struct _xtunion_struct** _temp2516= _temp2514.curr
+( yyvsp_offset - 1); if( _temp2514.base == 0? 1:( _temp2516 < _temp2514.base? 1:
_temp2516 >= _temp2514.last_plus_one)){ _throw( Null_Exception);}* _temp2516;})),
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2517= yyls; struct
Cyc_Yyltype* _temp2519= _temp2517.curr +( yylsp_offset - 1); if( _temp2517.base
== 0? 1:( _temp2519 < _temp2517.base? 1: _temp2519 >= _temp2517.last_plus_one)){
_throw( Null_Exception);}* _temp2519;}).first_line,({ struct _tagged_ptr3
_temp2520= yyls; struct Cyc_Yyltype* _temp2522= _temp2520.curr +( yylsp_offset -
1); if( _temp2520.base == 0? 1:( _temp2522 < _temp2520.base? 1: _temp2522 >=
_temp2520.last_plus_one)){ _throw( Null_Exception);}* _temp2522;}).last_line));
_temp2513;}); _temp2512;}); break; case 22: _LL2511: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2524=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2524[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2525; _temp2525.tag= Cyc_DeclList_tok_tag; _temp2525.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2526= yyvs; struct
_xtunion_struct** _temp2528= _temp2526.curr +( yyvsp_offset - 2); if( _temp2526.base
== 0? 1:( _temp2528 < _temp2526.base? 1: _temp2528 >= _temp2526.last_plus_one)){
_throw( Null_Exception);}* _temp2528;})), Cyc_yyget_InitDeclList_tok(({ struct
_tagged_ptr2 _temp2529= yyvs; struct _xtunion_struct** _temp2531= _temp2529.curr
+( yyvsp_offset - 1); if( _temp2529.base == 0? 1:( _temp2531 < _temp2529.base? 1:
_temp2531 >= _temp2529.last_plus_one)){ _throw( Null_Exception);}* _temp2531;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2532= yyls; struct Cyc_Yyltype*
_temp2534= _temp2532.curr +( yylsp_offset - 2); if( _temp2532.base == 0? 1:(
_temp2534 < _temp2532.base? 1: _temp2534 >= _temp2532.last_plus_one)){ _throw(
Null_Exception);}* _temp2534;}).first_line,({ struct _tagged_ptr3 _temp2535=
yyls; struct Cyc_Yyltype* _temp2537= _temp2535.curr + yylsp_offset; if(
_temp2535.base == 0? 1:( _temp2537 < _temp2535.base? 1: _temp2537 >= _temp2535.last_plus_one)){
_throw( Null_Exception);}* _temp2537;}).last_line)); _temp2525;}); _temp2524;});
break; case 23: _LL2523: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2539=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2539[ 0]=({ struct Cyc_DeclList_tok_struct _temp2540; _temp2540.tag= Cyc_DeclList_tok_tag;
_temp2540.f1=({ struct Cyc_List_List* _temp2541=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2541->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp2542= yyvs; struct
_xtunion_struct** _temp2544= _temp2542.curr +( yyvsp_offset - 3); if( _temp2542.base
== 0? 1:( _temp2544 < _temp2542.base? 1: _temp2544 >= _temp2542.last_plus_one)){
_throw( Null_Exception);}* _temp2544;})), 0, Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp2545= yyvs; struct _xtunion_struct** _temp2547= _temp2545.curr
+( yyvsp_offset - 1); if( _temp2545.base == 0? 1:( _temp2547 < _temp2545.base? 1:
_temp2547 >= _temp2545.last_plus_one)){ _throw( Null_Exception);}* _temp2547;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2548= yyls; struct Cyc_Yyltype*
_temp2550= _temp2548.curr +( yylsp_offset - 4); if( _temp2548.base == 0? 1:(
_temp2550 < _temp2548.base? 1: _temp2550 >= _temp2548.last_plus_one)){ _throw(
Null_Exception);}* _temp2550;}).first_line,({ struct _tagged_ptr3 _temp2551=
yyls; struct Cyc_Yyltype* _temp2553= _temp2551.curr + yylsp_offset; if(
_temp2551.base == 0? 1:( _temp2553 < _temp2551.base? 1: _temp2553 >= _temp2551.last_plus_one)){
_throw( Null_Exception);}* _temp2553;}).last_line)); _temp2541->tl= 0; _temp2541;});
_temp2540;}); _temp2539;}); break; case 24: _LL2538: yyval=({ struct
_tagged_ptr2 _temp2555= yyvs; struct _xtunion_struct** _temp2557= _temp2555.curr
+ yyvsp_offset; if( _temp2555.base == 0? 1:( _temp2557 < _temp2555.base? 1:
_temp2557 >= _temp2555.last_plus_one)){ _throw( Null_Exception);}* _temp2557;});
break; case 25: _LL2554: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2559=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2559[ 0]=({ struct Cyc_DeclList_tok_struct _temp2560; _temp2560.tag= Cyc_DeclList_tok_tag;
_temp2560.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2561= yyvs; struct _xtunion_struct** _temp2563= _temp2561.curr +(
yyvsp_offset - 1); if( _temp2561.base == 0? 1:( _temp2563 < _temp2561.base? 1:
_temp2563 >= _temp2561.last_plus_one)){ _throw( Null_Exception);}* _temp2563;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2564= yyvs; struct
_xtunion_struct** _temp2566= _temp2564.curr + yyvsp_offset; if( _temp2564.base
== 0? 1:( _temp2566 < _temp2564.base? 1: _temp2566 >= _temp2564.last_plus_one)){
_throw( Null_Exception);}* _temp2566;}))); _temp2560;}); _temp2559;}); break;
case 26: _LL2558: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct*
_temp2568=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2568[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2569; _temp2569.tag= Cyc_DeclSpec_tok_tag;
_temp2569.f1=({ struct Cyc_Parse_Declaration_spec* _temp2570=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2570->sc=({ struct
Cyc_Core_Opt* _temp2574=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2574->v=( void*) Cyc_yyget_StorageClass_tok(({ struct _tagged_ptr2
_temp2575= yyvs; struct _xtunion_struct** _temp2577= _temp2575.curr +(
yyvsp_offset - 1); if( _temp2575.base == 0? 1:( _temp2577 < _temp2575.base? 1:
_temp2577 >= _temp2575.last_plus_one)){ _throw( Null_Exception);}* _temp2577;}));
_temp2574;}); _temp2570->tq= Cyc_Absyn_empty_tqual(); _temp2570->type_specs= 0;
_temp2570->is_inline= 0; _temp2570->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2571= yyvs; struct _xtunion_struct** _temp2573=
_temp2571.curr + yyvsp_offset; if( _temp2571.base == 0? 1:( _temp2573 <
_temp2571.base? 1: _temp2573 >= _temp2571.last_plus_one)){ _throw(
Null_Exception);}* _temp2573;})); _temp2570;}); _temp2569;}); _temp2568;});
break; case 27: _LL2567: if(( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2579= yyvs; struct _xtunion_struct** _temp2581= _temp2579.curr +
yyvsp_offset; if( _temp2579.base == 0? 1:( _temp2581 < _temp2579.base? 1:
_temp2581 >= _temp2579.last_plus_one)){ _throw( Null_Exception);}* _temp2581;})))->sc
!= 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp2582=( char*)"Only one storage class is allowed in a declaration";
struct _tagged_string _temp2583; _temp2583.curr= _temp2582; _temp2583.base=
_temp2582; _temp2583.last_plus_one= _temp2582 + 51; _temp2583;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2584= yyls; struct Cyc_Yyltype* _temp2586= _temp2584.curr
+( yylsp_offset - 2); if( _temp2584.base == 0? 1:( _temp2586 < _temp2584.base? 1:
_temp2586 >= _temp2584.last_plus_one)){ _throw( Null_Exception);}* _temp2586;}).first_line,({
struct _tagged_ptr3 _temp2587= yyls; struct Cyc_Yyltype* _temp2589= _temp2587.curr
+( yylsp_offset - 1); if( _temp2587.base == 0? 1:( _temp2589 < _temp2587.base? 1:
_temp2589 >= _temp2587.last_plus_one)){ _throw( Null_Exception);}* _temp2589;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2590=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2590[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2591; _temp2591.tag= Cyc_DeclSpec_tok_tag;
_temp2591.f1=({ struct Cyc_Parse_Declaration_spec* _temp2592=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2592->sc=({ struct
Cyc_Core_Opt* _temp2608=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2608->v=( void*) Cyc_yyget_StorageClass_tok(({ struct _tagged_ptr2
_temp2609= yyvs; struct _xtunion_struct** _temp2611= _temp2609.curr +(
yyvsp_offset - 2); if( _temp2609.base == 0? 1:( _temp2611 < _temp2609.base? 1:
_temp2611 >= _temp2609.last_plus_one)){ _throw( Null_Exception);}* _temp2611;}));
_temp2608;}); _temp2592->tq=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2605= yyvs; struct _xtunion_struct** _temp2607= _temp2605.curr +
yyvsp_offset; if( _temp2605.base == 0? 1:( _temp2607 < _temp2605.base? 1:
_temp2607 >= _temp2605.last_plus_one)){ _throw( Null_Exception);}* _temp2607;})))->tq;
_temp2592->type_specs=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2602=
yyvs; struct _xtunion_struct** _temp2604= _temp2602.curr + yyvsp_offset; if(
_temp2602.base == 0? 1:( _temp2604 < _temp2602.base? 1: _temp2604 >= _temp2602.last_plus_one)){
_throw( Null_Exception);}* _temp2604;})))->type_specs; _temp2592->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2599= yyvs; struct
_xtunion_struct** _temp2601= _temp2599.curr + yyvsp_offset; if( _temp2599.base
== 0? 1:( _temp2601 < _temp2599.base? 1: _temp2601 >= _temp2599.last_plus_one)){
_throw( Null_Exception);}* _temp2601;})))->is_inline; _temp2592->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2593= yyvs; struct
_xtunion_struct** _temp2595= _temp2593.curr +( yyvsp_offset - 1); if( _temp2593.base
== 0? 1:( _temp2595 < _temp2593.base? 1: _temp2595 >= _temp2593.last_plus_one)){
_throw( Null_Exception);}* _temp2595;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2596= yyvs; struct _xtunion_struct** _temp2598= _temp2596.curr
+ yyvsp_offset; if( _temp2596.base == 0? 1:( _temp2598 < _temp2596.base? 1:
_temp2598 >= _temp2596.last_plus_one)){ _throw( Null_Exception);}* _temp2598;})))->attributes);
_temp2592;}); _temp2591;}); _temp2590;}); break; case 28: _LL2578: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2613=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2613[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2614; _temp2614.tag= Cyc_DeclSpec_tok_tag;
_temp2614.f1=({ struct Cyc_Parse_Declaration_spec* _temp2615=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2615->sc= 0;
_temp2615->tq= Cyc_Absyn_empty_tqual(); _temp2615->type_specs=({ struct Cyc_List_List*
_temp2619=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2619->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2620= yyvs; struct _xtunion_struct** _temp2622= _temp2620.curr +(
yyvsp_offset - 1); if( _temp2620.base == 0? 1:( _temp2622 < _temp2620.base? 1:
_temp2622 >= _temp2620.last_plus_one)){ _throw( Null_Exception);}* _temp2622;}));
_temp2619->tl= 0; _temp2619;}); _temp2615->is_inline= 0; _temp2615->attributes=
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2616= yyvs; struct
_xtunion_struct** _temp2618= _temp2616.curr + yyvsp_offset; if( _temp2616.base
== 0? 1:( _temp2618 < _temp2616.base? 1: _temp2618 >= _temp2616.last_plus_one)){
_throw( Null_Exception);}* _temp2618;})); _temp2615;}); _temp2614;}); _temp2613;});
break; case 29: _LL2612: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct*
_temp2624=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2624[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2625; _temp2625.tag= Cyc_DeclSpec_tok_tag;
_temp2625.f1=({ struct Cyc_Parse_Declaration_spec* _temp2626=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2626->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2646= yyvs; struct _xtunion_struct** _temp2648=
_temp2646.curr + yyvsp_offset; if( _temp2646.base == 0? 1:( _temp2648 <
_temp2646.base? 1: _temp2648 >= _temp2646.last_plus_one)){ _throw(
Null_Exception);}* _temp2648;})))->sc; _temp2626->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2643= yyvs; struct _xtunion_struct** _temp2645=
_temp2643.curr + yyvsp_offset; if( _temp2643.base == 0? 1:( _temp2645 <
_temp2643.base? 1: _temp2645 >= _temp2643.last_plus_one)){ _throw(
Null_Exception);}* _temp2645;})))->tq; _temp2626->type_specs=({ struct Cyc_List_List*
_temp2636=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2636->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2640= yyvs; struct _xtunion_struct** _temp2642= _temp2640.curr +(
yyvsp_offset - 2); if( _temp2640.base == 0? 1:( _temp2642 < _temp2640.base? 1:
_temp2642 >= _temp2640.last_plus_one)){ _throw( Null_Exception);}* _temp2642;}));
_temp2636->tl=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2637= yyvs;
struct _xtunion_struct** _temp2639= _temp2637.curr + yyvsp_offset; if( _temp2637.base
== 0? 1:( _temp2639 < _temp2637.base? 1: _temp2639 >= _temp2637.last_plus_one)){
_throw( Null_Exception);}* _temp2639;})))->type_specs; _temp2636;}); _temp2626->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2633= yyvs; struct
_xtunion_struct** _temp2635= _temp2633.curr + yyvsp_offset; if( _temp2633.base
== 0? 1:( _temp2635 < _temp2633.base? 1: _temp2635 >= _temp2633.last_plus_one)){
_throw( Null_Exception);}* _temp2635;})))->is_inline; _temp2626->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2627= yyvs; struct
_xtunion_struct** _temp2629= _temp2627.curr +( yyvsp_offset - 1); if( _temp2627.base
== 0? 1:( _temp2629 < _temp2627.base? 1: _temp2629 >= _temp2627.last_plus_one)){
_throw( Null_Exception);}* _temp2629;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2630= yyvs; struct _xtunion_struct** _temp2632= _temp2630.curr
+ yyvsp_offset; if( _temp2630.base == 0? 1:( _temp2632 < _temp2630.base? 1:
_temp2632 >= _temp2630.last_plus_one)){ _throw( Null_Exception);}* _temp2632;})))->attributes);
_temp2626;}); _temp2625;}); _temp2624;}); break; case 30: _LL2623: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2650=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2650[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2651; _temp2651.tag= Cyc_DeclSpec_tok_tag;
_temp2651.f1=({ struct Cyc_Parse_Declaration_spec* _temp2652=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2652->sc= 0;
_temp2652->tq= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp2656= yyvs;
struct _xtunion_struct** _temp2658= _temp2656.curr +( yyvsp_offset - 1); if(
_temp2656.base == 0? 1:( _temp2658 < _temp2656.base? 1: _temp2658 >= _temp2656.last_plus_one)){
_throw( Null_Exception);}* _temp2658;})); _temp2652->type_specs= 0; _temp2652->is_inline=
0; _temp2652->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2653= yyvs; struct _xtunion_struct** _temp2655= _temp2653.curr +
yyvsp_offset; if( _temp2653.base == 0? 1:( _temp2655 < _temp2653.base? 1:
_temp2655 >= _temp2653.last_plus_one)){ _throw( Null_Exception);}* _temp2655;}));
_temp2652;}); _temp2651;}); _temp2650;}); break; case 31: _LL2649: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2660=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2660[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2661; _temp2661.tag= Cyc_DeclSpec_tok_tag;
_temp2661.f1=({ struct Cyc_Parse_Declaration_spec* _temp2662=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2662->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2681= yyvs; struct _xtunion_struct** _temp2683=
_temp2681.curr + yyvsp_offset; if( _temp2681.base == 0? 1:( _temp2683 <
_temp2681.base? 1: _temp2683 >= _temp2681.last_plus_one)){ _throw(
Null_Exception);}* _temp2683;})))->sc; _temp2662->tq= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp2675= yyvs; struct
_xtunion_struct** _temp2677= _temp2675.curr +( yyvsp_offset - 2); if( _temp2675.base
== 0? 1:( _temp2677 < _temp2675.base? 1: _temp2677 >= _temp2675.last_plus_one)){
_throw( Null_Exception);}* _temp2677;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2678= yyvs; struct _xtunion_struct** _temp2680= _temp2678.curr
+ yyvsp_offset; if( _temp2678.base == 0? 1:( _temp2680 < _temp2678.base? 1:
_temp2680 >= _temp2678.last_plus_one)){ _throw( Null_Exception);}* _temp2680;})))->tq);
_temp2662->type_specs=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2672=
yyvs; struct _xtunion_struct** _temp2674= _temp2672.curr + yyvsp_offset; if(
_temp2672.base == 0? 1:( _temp2674 < _temp2672.base? 1: _temp2674 >= _temp2672.last_plus_one)){
_throw( Null_Exception);}* _temp2674;})))->type_specs; _temp2662->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2669= yyvs; struct
_xtunion_struct** _temp2671= _temp2669.curr + yyvsp_offset; if( _temp2669.base
== 0? 1:( _temp2671 < _temp2669.base? 1: _temp2671 >= _temp2669.last_plus_one)){
_throw( Null_Exception);}* _temp2671;})))->is_inline; _temp2662->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2663= yyvs; struct
_xtunion_struct** _temp2665= _temp2663.curr +( yyvsp_offset - 1); if( _temp2663.base
== 0? 1:( _temp2665 < _temp2663.base? 1: _temp2665 >= _temp2663.last_plus_one)){
_throw( Null_Exception);}* _temp2665;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2666= yyvs; struct _xtunion_struct** _temp2668= _temp2666.curr
+ yyvsp_offset; if( _temp2666.base == 0? 1:( _temp2668 < _temp2666.base? 1:
_temp2668 >= _temp2666.last_plus_one)){ _throw( Null_Exception);}* _temp2668;})))->attributes);
_temp2662;}); _temp2661;}); _temp2660;}); break; case 32: _LL2659: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2685=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2685[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2686; _temp2686.tag= Cyc_DeclSpec_tok_tag;
_temp2686.f1=({ struct Cyc_Parse_Declaration_spec* _temp2687=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2687->sc= 0;
_temp2687->tq= Cyc_Absyn_empty_tqual(); _temp2687->type_specs= 0; _temp2687->is_inline=
1; _temp2687->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2688= yyvs; struct _xtunion_struct** _temp2690= _temp2688.curr +
yyvsp_offset; if( _temp2688.base == 0? 1:( _temp2690 < _temp2688.base? 1:
_temp2690 >= _temp2688.last_plus_one)){ _throw( Null_Exception);}* _temp2690;}));
_temp2687;}); _temp2686;}); _temp2685;}); break; case 33: _LL2684: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2692=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2692[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2693; _temp2693.tag= Cyc_DeclSpec_tok_tag;
_temp2693.f1=({ struct Cyc_Parse_Declaration_spec* _temp2694=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2694->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2707= yyvs; struct _xtunion_struct** _temp2709=
_temp2707.curr + yyvsp_offset; if( _temp2707.base == 0? 1:( _temp2709 <
_temp2707.base? 1: _temp2709 >= _temp2707.last_plus_one)){ _throw(
Null_Exception);}* _temp2709;})))->sc; _temp2694->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2704= yyvs; struct _xtunion_struct** _temp2706=
_temp2704.curr + yyvsp_offset; if( _temp2704.base == 0? 1:( _temp2706 <
_temp2704.base? 1: _temp2706 >= _temp2704.last_plus_one)){ _throw(
Null_Exception);}* _temp2706;})))->tq; _temp2694->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2701= yyvs; struct _xtunion_struct** _temp2703=
_temp2701.curr + yyvsp_offset; if( _temp2701.base == 0? 1:( _temp2703 <
_temp2701.base? 1: _temp2703 >= _temp2701.last_plus_one)){ _throw(
Null_Exception);}* _temp2703;})))->type_specs; _temp2694->is_inline= 1;
_temp2694->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2695= yyvs; struct _xtunion_struct** _temp2697=
_temp2695.curr +( yyvsp_offset - 1); if( _temp2695.base == 0? 1:( _temp2697 <
_temp2695.base? 1: _temp2697 >= _temp2695.last_plus_one)){ _throw(
Null_Exception);}* _temp2697;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2698= yyvs; struct _xtunion_struct** _temp2700= _temp2698.curr +
yyvsp_offset; if( _temp2698.base == 0? 1:( _temp2700 < _temp2698.base? 1:
_temp2700 >= _temp2698.last_plus_one)){ _throw( Null_Exception);}* _temp2700;})))->attributes);
_temp2694;}); _temp2693;}); _temp2692;}); break; case 34: _LL2691: yyval=(
struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2711=(
struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2711[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2712; _temp2712.tag=
Cyc_StorageClass_tok_tag; _temp2712.f1=( void*)(( void*) Cyc_Parse_Auto_sc);
_temp2712;}); _temp2711;}); break; case 35: _LL2710: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2714=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2714[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2715; _temp2715.tag= Cyc_StorageClass_tok_tag;
_temp2715.f1=( void*)(( void*) Cyc_Parse_Register_sc); _temp2715;}); _temp2714;});
break; case 36: _LL2713: yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct*
_temp2717=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2717[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2718; _temp2718.tag=
Cyc_StorageClass_tok_tag; _temp2718.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp2718;}); _temp2717;}); break; case 37: _LL2716: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2720=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2720[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2721; _temp2721.tag= Cyc_StorageClass_tok_tag;
_temp2721.f1=( void*)(( void*) Cyc_Parse_Extern_sc); _temp2721;}); _temp2720;});
break; case 38: _LL2719: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2723= yyvs; struct _xtunion_struct** _temp2725= _temp2723.curr
+ yyvsp_offset; if( _temp2723.base == 0? 1:( _temp2725 < _temp2723.base? 1:
_temp2725 >= _temp2723.last_plus_one)){ _throw( Null_Exception);}* _temp2725;})),(
struct _tagged_string)({ char* _temp2726=( char*)"C"; struct _tagged_string
_temp2727; _temp2727.curr= _temp2726; _temp2727.base= _temp2726; _temp2727.last_plus_one=
_temp2726 + 2; _temp2727;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp2728=( char*)"only extern or extern \"C\" is allowed"; struct
_tagged_string _temp2729; _temp2729.curr= _temp2728; _temp2729.base= _temp2728;
_temp2729.last_plus_one= _temp2728 + 37; _temp2729;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2730= yyls; struct Cyc_Yyltype* _temp2732= _temp2730.curr
+( yylsp_offset - 1); if( _temp2730.base == 0? 1:( _temp2732 < _temp2730.base? 1:
_temp2732 >= _temp2730.last_plus_one)){ _throw( Null_Exception);}* _temp2732;}).first_line,({
struct _tagged_ptr3 _temp2733= yyls; struct Cyc_Yyltype* _temp2735= _temp2733.curr
+ yylsp_offset; if( _temp2733.base == 0? 1:( _temp2735 < _temp2733.base? 1:
_temp2735 >= _temp2733.last_plus_one)){ _throw( Null_Exception);}* _temp2735;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2736=(
struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2736[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2737; _temp2737.tag=
Cyc_StorageClass_tok_tag; _temp2737.f1=( void*)(( void*) Cyc_Parse_ExternC_sc);
_temp2737;}); _temp2736;}); break; case 39: _LL2722: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2739=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2739[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2740; _temp2740.tag= Cyc_StorageClass_tok_tag;
_temp2740.f1=( void*)(( void*) Cyc_Parse_Typedef_sc); _temp2740;}); _temp2739;});
break; case 40: _LL2738: yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct*
_temp2742=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2742[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2743; _temp2743.tag=
Cyc_StorageClass_tok_tag; _temp2743.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp2743;}); _temp2742;}); break; case 41: _LL2741: yyval=( struct
_xtunion_struct*)({ struct Cyc_AttributeList_tok_struct* _temp2745=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2745[ 0]=({
struct Cyc_AttributeList_tok_struct _temp2746; _temp2746.tag= Cyc_AttributeList_tok_tag;
_temp2746.f1= 0; _temp2746;}); _temp2745;}); break; case 42: _LL2744: yyval=({
struct _tagged_ptr2 _temp2748= yyvs; struct _xtunion_struct** _temp2750=
_temp2748.curr + yyvsp_offset; if( _temp2748.base == 0? 1:( _temp2750 <
_temp2748.base? 1: _temp2750 >= _temp2748.last_plus_one)){ _throw(
Null_Exception);}* _temp2750;}); break; case 43: _LL2747: yyval=( struct
_xtunion_struct*)({ struct Cyc_AttributeList_tok_struct* _temp2752=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2752[ 0]=({
struct Cyc_AttributeList_tok_struct _temp2753; _temp2753.tag= Cyc_AttributeList_tok_tag;
_temp2753.f1= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2754= yyvs;
struct _xtunion_struct** _temp2756= _temp2754.curr +( yyvsp_offset - 2); if(
_temp2754.base == 0? 1:( _temp2756 < _temp2754.base? 1: _temp2756 >= _temp2754.last_plus_one)){
_throw( Null_Exception);}* _temp2756;})); _temp2753;}); _temp2752;}); break;
case 44: _LL2751: yyval=( struct _xtunion_struct*)({ struct Cyc_AttributeList_tok_struct*
_temp2758=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2758[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2759; _temp2759.tag=
Cyc_AttributeList_tok_tag; _temp2759.f1=({ struct Cyc_List_List* _temp2760=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2760->hd=(
void*) Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2761= yyvs; struct
_xtunion_struct** _temp2763= _temp2761.curr + yyvsp_offset; if( _temp2761.base
== 0? 1:( _temp2763 < _temp2761.base? 1: _temp2763 >= _temp2761.last_plus_one)){
_throw( Null_Exception);}* _temp2763;})); _temp2760->tl= 0; _temp2760;});
_temp2759;}); _temp2758;}); break; case 45: _LL2757: yyval=( struct
_xtunion_struct*)({ struct Cyc_AttributeList_tok_struct* _temp2765=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2765[ 0]=({
struct Cyc_AttributeList_tok_struct _temp2766; _temp2766.tag= Cyc_AttributeList_tok_tag;
_temp2766.f1=({ struct Cyc_List_List* _temp2767=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2767->hd=( void*) Cyc_yyget_Attribute_tok(({
struct _tagged_ptr2 _temp2771= yyvs; struct _xtunion_struct** _temp2773=
_temp2771.curr +( yyvsp_offset - 2); if( _temp2771.base == 0? 1:( _temp2773 <
_temp2771.base? 1: _temp2773 >= _temp2771.last_plus_one)){ _throw(
Null_Exception);}* _temp2773;})); _temp2767->tl= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2768= yyvs; struct _xtunion_struct** _temp2770=
_temp2768.curr + yyvsp_offset; if( _temp2768.base == 0? 1:( _temp2770 <
_temp2768.base? 1: _temp2770 >= _temp2768.last_plus_one)){ _throw(
Null_Exception);}* _temp2770;})); _temp2767;}); _temp2766;}); _temp2765;});
break; case 46: _LL2764: { struct _tagged_string s= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2775= yyvs; struct _xtunion_struct** _temp2777=
_temp2775.curr + yyvsp_offset; if( _temp2775.base == 0? 1:( _temp2777 <
_temp2775.base? 1: _temp2777 >= _temp2775.last_plus_one)){ _throw(
Null_Exception);}* _temp2777;})); void* a; if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2778=( char*)"stdcall"; struct _tagged_string
_temp2779; _temp2779.curr= _temp2778; _temp2779.base= _temp2778; _temp2779.last_plus_one=
_temp2778 + 8; _temp2779;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2780=( char*)"__stdcall__"; struct _tagged_string
_temp2781; _temp2781.curr= _temp2780; _temp2781.base= _temp2780; _temp2781.last_plus_one=
_temp2780 + 12; _temp2781;})) == 0){ a=( void*) Cyc_Absyn_Stdcall_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2782=( char*)"cdecl";
struct _tagged_string _temp2783; _temp2783.curr= _temp2782; _temp2783.base=
_temp2782; _temp2783.last_plus_one= _temp2782 + 6; _temp2783;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2784=( char*)"__cdecl__"; struct
_tagged_string _temp2785; _temp2785.curr= _temp2784; _temp2785.base= _temp2784;
_temp2785.last_plus_one= _temp2784 + 10; _temp2785;})) == 0){ a=( void*) Cyc_Absyn_Cdecl_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2786=(
char*)"noreturn"; struct _tagged_string _temp2787; _temp2787.curr= _temp2786;
_temp2787.base= _temp2786; _temp2787.last_plus_one= _temp2786 + 9; _temp2787;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2788=( char*)"__noreturn__";
struct _tagged_string _temp2789; _temp2789.curr= _temp2788; _temp2789.base=
_temp2788; _temp2789.last_plus_one= _temp2788 + 13; _temp2789;})) == 0){ a=(
void*) Cyc_Absyn_Noreturn_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2790=( char*)"noreturn"; struct _tagged_string
_temp2791; _temp2791.curr= _temp2790; _temp2791.base= _temp2790; _temp2791.last_plus_one=
_temp2790 + 9; _temp2791;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2792=( char*)"__noreturn__"; struct _tagged_string
_temp2793; _temp2793.curr= _temp2792; _temp2793.base= _temp2792; _temp2793.last_plus_one=
_temp2792 + 13; _temp2793;})) == 0){ a=( void*) Cyc_Absyn_Noreturn_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2794=( char*)"__const__";
struct _tagged_string _temp2795; _temp2795.curr= _temp2794; _temp2795.base=
_temp2794; _temp2795.last_plus_one= _temp2794 + 10; _temp2795;})) == 0){ a=(
void*) Cyc_Absyn_Const_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2796=( char*)"aligned"; struct _tagged_string
_temp2797; _temp2797.curr= _temp2796; _temp2797.base= _temp2796; _temp2797.last_plus_one=
_temp2796 + 8; _temp2797;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2798=( char*)"__aligned__"; struct _tagged_string
_temp2799; _temp2799.curr= _temp2798; _temp2799.base= _temp2798; _temp2799.last_plus_one=
_temp2798 + 12; _temp2799;})) == 0){ a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp2800=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct));
_temp2800[ 0]=({ struct Cyc_Absyn_Aligned_att_struct _temp2801; _temp2801.tag=
Cyc_Absyn_Aligned_att_tag; _temp2801.f1= - 1; _temp2801;}); _temp2800;});} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2802=( char*)"packed";
struct _tagged_string _temp2803; _temp2803.curr= _temp2802; _temp2803.base=
_temp2802; _temp2803.last_plus_one= _temp2802 + 7; _temp2803;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2804=( char*)"__packed__"; struct
_tagged_string _temp2805; _temp2805.curr= _temp2804; _temp2805.base= _temp2804;
_temp2805.last_plus_one= _temp2804 + 11; _temp2805;})) == 0){ a=( void*) Cyc_Absyn_Packed_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2806=(
char*)"shared"; struct _tagged_string _temp2807; _temp2807.curr= _temp2806;
_temp2807.base= _temp2806; _temp2807.last_plus_one= _temp2806 + 7; _temp2807;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2808=( char*)"__shared__";
struct _tagged_string _temp2809; _temp2809.curr= _temp2808; _temp2809.base=
_temp2808; _temp2809.last_plus_one= _temp2808 + 11; _temp2809;})) == 0){ a=(
void*) Cyc_Absyn_Shared_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2810=( char*)"unused"; struct _tagged_string
_temp2811; _temp2811.curr= _temp2810; _temp2811.base= _temp2810; _temp2811.last_plus_one=
_temp2810 + 7; _temp2811;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2812=( char*)"__unused__"; struct _tagged_string
_temp2813; _temp2813.curr= _temp2812; _temp2813.base= _temp2812; _temp2813.last_plus_one=
_temp2812 + 11; _temp2813;})) == 0){ a=( void*) Cyc_Absyn_Unused_att;} else{ if(
Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2814=( char*)"weak";
struct _tagged_string _temp2815; _temp2815.curr= _temp2814; _temp2815.base=
_temp2814; _temp2815.last_plus_one= _temp2814 + 5; _temp2815;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2816=( char*)"__weak__"; struct
_tagged_string _temp2817; _temp2817.curr= _temp2816; _temp2817.base= _temp2816;
_temp2817.last_plus_one= _temp2816 + 9; _temp2817;})) == 0){ a=( void*) Cyc_Absyn_Weak_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2818=(
char*)"dllimport"; struct _tagged_string _temp2819; _temp2819.curr= _temp2818;
_temp2819.base= _temp2818; _temp2819.last_plus_one= _temp2818 + 10; _temp2819;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2820=( char*)"__dllimport__";
struct _tagged_string _temp2821; _temp2821.curr= _temp2820; _temp2821.base=
_temp2820; _temp2821.last_plus_one= _temp2820 + 14; _temp2821;})) == 0){ a=(
void*) Cyc_Absyn_Dllimport_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2822=( char*)"dllexport"; struct _tagged_string
_temp2823; _temp2823.curr= _temp2822; _temp2823.base= _temp2822; _temp2823.last_plus_one=
_temp2822 + 10; _temp2823;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2824=( char*)"__dllexport__"; struct _tagged_string
_temp2825; _temp2825.curr= _temp2824; _temp2825.base= _temp2824; _temp2825.last_plus_one=
_temp2824 + 14; _temp2825;})) == 0){ a=( void*) Cyc_Absyn_Dllexport_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2826=( char*)"no_instrument_function";
struct _tagged_string _temp2827; _temp2827.curr= _temp2826; _temp2827.base=
_temp2826; _temp2827.last_plus_one= _temp2826 + 23; _temp2827;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2828=( char*)"__no_instrument_function__";
struct _tagged_string _temp2829; _temp2829.curr= _temp2828; _temp2829.base=
_temp2828; _temp2829.last_plus_one= _temp2828 + 27; _temp2829;})) == 0){ a=(
void*) Cyc_Absyn_No_instrument_function_att;} else{ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2830=( char*)"constructor"; struct
_tagged_string _temp2831; _temp2831.curr= _temp2830; _temp2831.base= _temp2830;
_temp2831.last_plus_one= _temp2830 + 12; _temp2831;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2832=( char*)"__constructor__"; struct
_tagged_string _temp2833; _temp2833.curr= _temp2832; _temp2833.base= _temp2832;
_temp2833.last_plus_one= _temp2832 + 16; _temp2833;})) == 0){ a=( void*) Cyc_Absyn_Constructor_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2834=(
char*)"destructor"; struct _tagged_string _temp2835; _temp2835.curr= _temp2834;
_temp2835.base= _temp2834; _temp2835.last_plus_one= _temp2834 + 11; _temp2835;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2836=( char*)"__destructor__";
struct _tagged_string _temp2837; _temp2837.curr= _temp2836; _temp2837.base=
_temp2836; _temp2837.last_plus_one= _temp2836 + 15; _temp2837;})) == 0){ a=(
void*) Cyc_Absyn_Destructor_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2838=( char*)"no_check_memory_usage"; struct
_tagged_string _temp2839; _temp2839.curr= _temp2838; _temp2839.base= _temp2838;
_temp2839.last_plus_one= _temp2838 + 22; _temp2839;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2840=( char*)"__no_check_memory_usage__";
struct _tagged_string _temp2841; _temp2841.curr= _temp2840; _temp2841.base=
_temp2840; _temp2841.last_plus_one= _temp2840 + 26; _temp2841;})) == 0){ a=(
void*) Cyc_Absyn_No_check_memory_usage_att;} else{ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2842=( char*)"unrecognized attribute"; struct
_tagged_string _temp2843; _temp2843.curr= _temp2842; _temp2843.base= _temp2842;
_temp2843.last_plus_one= _temp2842 + 23; _temp2843;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2844= yyls; struct Cyc_Yyltype* _temp2846= _temp2844.curr
+ yylsp_offset; if( _temp2844.base == 0? 1:( _temp2846 < _temp2844.base? 1:
_temp2846 >= _temp2844.last_plus_one)){ _throw( Null_Exception);}* _temp2846;}).first_line,({
struct _tagged_ptr3 _temp2847= yyls; struct Cyc_Yyltype* _temp2849= _temp2847.curr
+ yylsp_offset; if( _temp2847.base == 0? 1:( _temp2849 < _temp2847.base? 1:
_temp2849 >= _temp2847.last_plus_one)){ _throw( Null_Exception);}* _temp2849;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}}}}}}}}}}}}}}}} yyval=( struct _xtunion_struct*)({
struct Cyc_Attribute_tok_struct* _temp2850=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2850[ 0]=({ struct
Cyc_Attribute_tok_struct _temp2851; _temp2851.tag= Cyc_Attribute_tok_tag;
_temp2851.f1=( void*) a; _temp2851;}); _temp2850;}); break;} case 47: _LL2774:
yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct* _temp2853=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2853[ 0]=({ struct Cyc_Attribute_tok_struct _temp2854; _temp2854.tag= Cyc_Attribute_tok_tag;
_temp2854.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp2854;}); _temp2853;});
break; case 48: _LL2852: { struct _tagged_string s= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2856= yyvs; struct _xtunion_struct** _temp2858=
_temp2856.curr +( yyvsp_offset - 3); if( _temp2856.base == 0? 1:( _temp2858 <
_temp2856.base? 1: _temp2858 >= _temp2856.last_plus_one)){ _throw(
Null_Exception);}* _temp2858;})); struct _tuple12 _temp2864; int _temp2865; void*
_temp2867; struct _tuple12* _temp2862= Cyc_yyget_Int_tok(({ struct _tagged_ptr2
_temp2859= yyvs; struct _xtunion_struct** _temp2861= _temp2859.curr +(
yyvsp_offset - 1); if( _temp2859.base == 0? 1:( _temp2861 < _temp2859.base? 1:
_temp2861 >= _temp2859.last_plus_one)){ _throw( Null_Exception);}* _temp2861;}));
_temp2864=* _temp2862; _LL2868: _temp2867= _temp2864.f1; goto _LL2866; _LL2866:
_temp2865= _temp2864.f2; goto _LL2863; _LL2863: { void* a; if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2869=( char*)"regparm"; struct
_tagged_string _temp2870; _temp2870.curr= _temp2869; _temp2870.base= _temp2869;
_temp2870.last_plus_one= _temp2869 + 8; _temp2870;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2871=( char*)"__regparm__"; struct
_tagged_string _temp2872; _temp2872.curr= _temp2871; _temp2872.base= _temp2871;
_temp2872.last_plus_one= _temp2871 + 12; _temp2872;})) == 0){ if( _temp2865 <= 0?
1: _temp2865 > 3){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2873=(
char*)"regparm requires value between 1 and 3"; struct _tagged_string _temp2874;
_temp2874.curr= _temp2873; _temp2874.base= _temp2873; _temp2874.last_plus_one=
_temp2873 + 39; _temp2874;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2875= yyls; struct Cyc_Yyltype* _temp2877= _temp2875.curr +( yylsp_offset -
1); if( _temp2875.base == 0? 1:( _temp2877 < _temp2875.base? 1: _temp2877 >=
_temp2875.last_plus_one)){ _throw( Null_Exception);}* _temp2877;}).first_line,({
struct _tagged_ptr3 _temp2878= yyls; struct Cyc_Yyltype* _temp2880= _temp2878.curr
+( yylsp_offset - 1); if( _temp2878.base == 0? 1:( _temp2880 < _temp2878.base? 1:
_temp2880 >= _temp2878.last_plus_one)){ _throw( Null_Exception);}* _temp2880;}).last_line));}
a=( void*)({ struct Cyc_Absyn_Regparm_att_struct* _temp2881=( struct Cyc_Absyn_Regparm_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Regparm_att_struct)); _temp2881[ 0]=({
struct Cyc_Absyn_Regparm_att_struct _temp2882; _temp2882.tag= Cyc_Absyn_Regparm_att_tag;
_temp2882.f1= _temp2865; _temp2882;}); _temp2881;});} else{ if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2883=( char*)"aligned"; struct
_tagged_string _temp2884; _temp2884.curr= _temp2883; _temp2884.base= _temp2883;
_temp2884.last_plus_one= _temp2883 + 8; _temp2884;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2885=( char*)"__aligned__"; struct
_tagged_string _temp2886; _temp2886.curr= _temp2885; _temp2886.base= _temp2885;
_temp2886.last_plus_one= _temp2885 + 12; _temp2886;})) == 0){ if( _temp2865 < 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2887=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2888; _temp2888.curr= _temp2887; _temp2888.base=
_temp2887; _temp2888.last_plus_one= _temp2887 + 39; _temp2888;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2889= yyls; struct Cyc_Yyltype* _temp2891= _temp2889.curr
+( yylsp_offset - 1); if( _temp2889.base == 0? 1:( _temp2891 < _temp2889.base? 1:
_temp2891 >= _temp2889.last_plus_one)){ _throw( Null_Exception);}* _temp2891;}).first_line,({
struct _tagged_ptr3 _temp2892= yyls; struct Cyc_Yyltype* _temp2894= _temp2892.curr
+( yylsp_offset - 1); if( _temp2892.base == 0? 1:( _temp2894 < _temp2892.base? 1:
_temp2894 >= _temp2892.last_plus_one)){ _throw( Null_Exception);}* _temp2894;}).last_line));}{
unsigned int j=( unsigned int) _temp2865; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2895=(
char*)"aligned requires positive power of two"; struct _tagged_string _temp2896;
_temp2896.curr= _temp2895; _temp2896.base= _temp2895; _temp2896.last_plus_one=
_temp2895 + 39; _temp2896;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2897= yyls; struct Cyc_Yyltype* _temp2899= _temp2897.curr +( yylsp_offset -
1); if( _temp2897.base == 0? 1:( _temp2899 < _temp2897.base? 1: _temp2899 >=
_temp2897.last_plus_one)){ _throw( Null_Exception);}* _temp2899;}).first_line,({
struct _tagged_ptr3 _temp2900= yyls; struct Cyc_Yyltype* _temp2902= _temp2900.curr
+( yylsp_offset - 1); if( _temp2900.base == 0? 1:( _temp2902 < _temp2900.base? 1:
_temp2902 >= _temp2900.last_plus_one)){ _throw( Null_Exception);}* _temp2902;}).last_line));}
a=( void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp2903=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp2903[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp2904; _temp2904.tag= Cyc_Absyn_Aligned_att_tag;
_temp2904.f1= _temp2865; _temp2904;}); _temp2903;});}} else{ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2905=( char*)"unrecognized attribute";
struct _tagged_string _temp2906; _temp2906.curr= _temp2905; _temp2906.base=
_temp2905; _temp2906.last_plus_one= _temp2905 + 23; _temp2906;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2907= yyls; struct Cyc_Yyltype* _temp2909= _temp2907.curr
+( yylsp_offset - 3); if( _temp2907.base == 0? 1:( _temp2909 < _temp2907.base? 1:
_temp2909 >= _temp2907.last_plus_one)){ _throw( Null_Exception);}* _temp2909;}).first_line,({
struct _tagged_ptr3 _temp2910= yyls; struct Cyc_Yyltype* _temp2912= _temp2910.curr
+( yylsp_offset - 3); if( _temp2910.base == 0? 1:( _temp2912 < _temp2910.base? 1:
_temp2912 >= _temp2910.last_plus_one)){ _throw( Null_Exception);}* _temp2912;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}} yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct*
_temp2913=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2913[ 0]=({ struct Cyc_Attribute_tok_struct _temp2914; _temp2914.tag= Cyc_Attribute_tok_tag;
_temp2914.f1=( void*) a; _temp2914;}); _temp2913;}); break;}} case 49: _LL2855: {
struct _tagged_string s= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2916=
yyvs; struct _xtunion_struct** _temp2918= _temp2916.curr +( yyvsp_offset - 3);
if( _temp2916.base == 0? 1:( _temp2918 < _temp2916.base? 1: _temp2918 >=
_temp2916.last_plus_one)){ _throw( Null_Exception);}* _temp2918;})); struct
_tagged_string str= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2919= yyvs;
struct _xtunion_struct** _temp2921= _temp2919.curr +( yyvsp_offset - 1); if(
_temp2919.base == 0? 1:( _temp2921 < _temp2919.base? 1: _temp2921 >= _temp2919.last_plus_one)){
_throw( Null_Exception);}* _temp2921;})); void* a; if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2922=( char*)"section"; struct
_tagged_string _temp2923; _temp2923.curr= _temp2922; _temp2923.base= _temp2922;
_temp2923.last_plus_one= _temp2922 + 8; _temp2923;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2924=( char*)"__section__"; struct
_tagged_string _temp2925; _temp2925.curr= _temp2924; _temp2925.base= _temp2924;
_temp2925.last_plus_one= _temp2924 + 12; _temp2925;}))){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp2926=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp2926[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp2927; _temp2927.tag=
Cyc_Absyn_Section_att_tag; _temp2927.f1= str; _temp2927;}); _temp2926;});} else{
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2928=( char*)"unrecognized attribute";
struct _tagged_string _temp2929; _temp2929.curr= _temp2928; _temp2929.base=
_temp2928; _temp2929.last_plus_one= _temp2928 + 23; _temp2929;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2930= yyls; struct Cyc_Yyltype* _temp2932= _temp2930.curr
+( yylsp_offset - 3); if( _temp2930.base == 0? 1:( _temp2932 < _temp2930.base? 1:
_temp2932 >= _temp2930.last_plus_one)){ _throw( Null_Exception);}* _temp2932;}).first_line,({
struct _tagged_ptr3 _temp2933= yyls; struct Cyc_Yyltype* _temp2935= _temp2933.curr
+( yylsp_offset - 3); if( _temp2933.base == 0? 1:( _temp2935 < _temp2933.base? 1:
_temp2935 >= _temp2933.last_plus_one)){ _throw( Null_Exception);}* _temp2935;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct*
_temp2936=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2936[ 0]=({ struct Cyc_Attribute_tok_struct _temp2937; _temp2937.tag= Cyc_Attribute_tok_tag;
_temp2937.f1=( void*) a; _temp2937;}); _temp2936;}); break;} case 50: _LL2915:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2939=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2939[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2940; _temp2940.tag=
Cyc_TypeSpecifier_tok_tag; _temp2940.f1=( void*) Cyc_Parse_type_spec(( void*)
Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2941=
yyls; struct Cyc_Yyltype* _temp2943= _temp2941.curr + yylsp_offset; if(
_temp2941.base == 0? 1:( _temp2943 < _temp2941.base? 1: _temp2943 >= _temp2941.last_plus_one)){
_throw( Null_Exception);}* _temp2943;}).first_line,({ struct _tagged_ptr3
_temp2944= yyls; struct Cyc_Yyltype* _temp2946= _temp2944.curr + yylsp_offset;
if( _temp2944.base == 0? 1:( _temp2946 < _temp2944.base? 1: _temp2946 >=
_temp2944.last_plus_one)){ _throw( Null_Exception);}* _temp2946;}).last_line));
_temp2940;}); _temp2939;}); break; case 51: _LL2938: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2948=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2948[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2949; _temp2949.tag= Cyc_TypeSpecifier_tok_tag;
_temp2949.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2950= yyls; struct Cyc_Yyltype*
_temp2952= _temp2950.curr + yylsp_offset; if( _temp2950.base == 0? 1:( _temp2952
< _temp2950.base? 1: _temp2952 >= _temp2950.last_plus_one)){ _throw(
Null_Exception);}* _temp2952;}).first_line,({ struct _tagged_ptr3 _temp2953=
yyls; struct Cyc_Yyltype* _temp2955= _temp2953.curr + yylsp_offset; if(
_temp2953.base == 0? 1:( _temp2955 < _temp2953.base? 1: _temp2955 >= _temp2953.last_plus_one)){
_throw( Null_Exception);}* _temp2955;}).last_line)); _temp2949;}); _temp2948;});
break; case 52: _LL2947: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2957=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2957[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2958; _temp2958.tag=
Cyc_TypeSpecifier_tok_tag; _temp2958.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2959= yyls; struct Cyc_Yyltype*
_temp2961= _temp2959.curr + yylsp_offset; if( _temp2959.base == 0? 1:( _temp2961
< _temp2959.base? 1: _temp2961 >= _temp2959.last_plus_one)){ _throw(
Null_Exception);}* _temp2961;}).first_line,({ struct _tagged_ptr3 _temp2962=
yyls; struct Cyc_Yyltype* _temp2964= _temp2962.curr + yylsp_offset; if(
_temp2962.base == 0? 1:( _temp2964 < _temp2962.base? 1: _temp2964 >= _temp2962.last_plus_one)){
_throw( Null_Exception);}* _temp2964;}).last_line)); _temp2958;}); _temp2957;});
break; case 53: _LL2956: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2966=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2966[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2967; _temp2967.tag=
Cyc_TypeSpecifier_tok_tag; _temp2967.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp2968=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp2968[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp2969; _temp2969.tag=
Cyc_Parse_Short_spec_tag; _temp2969.f1= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2970= yyls; struct Cyc_Yyltype* _temp2972= _temp2970.curr +
yylsp_offset; if( _temp2970.base == 0? 1:( _temp2972 < _temp2970.base? 1:
_temp2972 >= _temp2970.last_plus_one)){ _throw( Null_Exception);}* _temp2972;}).first_line,({
struct _tagged_ptr3 _temp2973= yyls; struct Cyc_Yyltype* _temp2975= _temp2973.curr
+ yylsp_offset; if( _temp2973.base == 0? 1:( _temp2975 < _temp2973.base? 1:
_temp2975 >= _temp2973.last_plus_one)){ _throw( Null_Exception);}* _temp2975;}).last_line);
_temp2969;}); _temp2968;})); _temp2967;}); _temp2966;}); break; case 54: _LL2965:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2977=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2977[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2978; _temp2978.tag=
Cyc_TypeSpecifier_tok_tag; _temp2978.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2979= yyls; struct Cyc_Yyltype*
_temp2981= _temp2979.curr + yylsp_offset; if( _temp2979.base == 0? 1:( _temp2981
< _temp2979.base? 1: _temp2981 >= _temp2979.last_plus_one)){ _throw(
Null_Exception);}* _temp2981;}).first_line,({ struct _tagged_ptr3 _temp2982=
yyls; struct Cyc_Yyltype* _temp2984= _temp2982.curr + yylsp_offset; if(
_temp2982.base == 0? 1:( _temp2984 < _temp2982.base? 1: _temp2984 >= _temp2982.last_plus_one)){
_throw( Null_Exception);}* _temp2984;}).last_line)); _temp2978;}); _temp2977;});
break; case 55: _LL2976: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2986=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2986[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2987; _temp2987.tag=
Cyc_TypeSpecifier_tok_tag; _temp2987.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp2988=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp2988[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp2989; _temp2989.tag= Cyc_Parse_Long_spec_tag;
_temp2989.f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2990= yyls;
struct Cyc_Yyltype* _temp2992= _temp2990.curr + yylsp_offset; if( _temp2990.base
== 0? 1:( _temp2992 < _temp2990.base? 1: _temp2992 >= _temp2990.last_plus_one)){
_throw( Null_Exception);}* _temp2992;}).first_line,({ struct _tagged_ptr3
_temp2993= yyls; struct Cyc_Yyltype* _temp2995= _temp2993.curr + yylsp_offset;
if( _temp2993.base == 0? 1:( _temp2995 < _temp2993.base? 1: _temp2995 >=
_temp2993.last_plus_one)){ _throw( Null_Exception);}* _temp2995;}).last_line);
_temp2989;}); _temp2988;})); _temp2987;}); _temp2986;}); break; case 56: _LL2985:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2997=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2997[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2998; _temp2998.tag=
Cyc_TypeSpecifier_tok_tag; _temp2998.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2999= yyls; struct Cyc_Yyltype*
_temp3001= _temp2999.curr + yylsp_offset; if( _temp2999.base == 0? 1:( _temp3001
< _temp2999.base? 1: _temp3001 >= _temp2999.last_plus_one)){ _throw(
Null_Exception);}* _temp3001;}).first_line,({ struct _tagged_ptr3 _temp3002=
yyls; struct Cyc_Yyltype* _temp3004= _temp3002.curr + yylsp_offset; if(
_temp3002.base == 0? 1:( _temp3004 < _temp3002.base? 1: _temp3004 >= _temp3002.last_plus_one)){
_throw( Null_Exception);}* _temp3004;}).last_line)); _temp2998;}); _temp2997;});
break; case 57: _LL2996: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3006=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3006[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3007; _temp3007.tag=
Cyc_TypeSpecifier_tok_tag; _temp3007.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3008= yyls; struct Cyc_Yyltype*
_temp3010= _temp3008.curr + yylsp_offset; if( _temp3008.base == 0? 1:( _temp3010
< _temp3008.base? 1: _temp3010 >= _temp3008.last_plus_one)){ _throw(
Null_Exception);}* _temp3010;}).first_line,({ struct _tagged_ptr3 _temp3011=
yyls; struct Cyc_Yyltype* _temp3013= _temp3011.curr + yylsp_offset; if(
_temp3011.base == 0? 1:( _temp3013 < _temp3011.base? 1: _temp3013 >= _temp3011.last_plus_one)){
_throw( Null_Exception);}* _temp3013;}).last_line)); _temp3007;}); _temp3006;});
break; case 58: _LL3005: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3015=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3015[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3016; _temp3016.tag=
Cyc_TypeSpecifier_tok_tag; _temp3016.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp3017=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp3017[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp3018; _temp3018.tag=
Cyc_Parse_Signed_spec_tag; _temp3018.f1= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3019= yyls; struct Cyc_Yyltype* _temp3021= _temp3019.curr +
yylsp_offset; if( _temp3019.base == 0? 1:( _temp3021 < _temp3019.base? 1:
_temp3021 >= _temp3019.last_plus_one)){ _throw( Null_Exception);}* _temp3021;}).first_line,({
struct _tagged_ptr3 _temp3022= yyls; struct Cyc_Yyltype* _temp3024= _temp3022.curr
+ yylsp_offset; if( _temp3022.base == 0? 1:( _temp3024 < _temp3022.base? 1:
_temp3024 >= _temp3022.last_plus_one)){ _throw( Null_Exception);}* _temp3024;}).last_line);
_temp3018;}); _temp3017;})); _temp3016;}); _temp3015;}); break; case 59: _LL3014:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3026=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3026[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3027; _temp3027.tag=
Cyc_TypeSpecifier_tok_tag; _temp3027.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp3028=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp3028[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp3029; _temp3029.tag= Cyc_Parse_Unsigned_spec_tag; _temp3029.f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3030= yyls; struct Cyc_Yyltype* _temp3032= _temp3030.curr
+ yylsp_offset; if( _temp3030.base == 0? 1:( _temp3032 < _temp3030.base? 1:
_temp3032 >= _temp3030.last_plus_one)){ _throw( Null_Exception);}* _temp3032;}).first_line,({
struct _tagged_ptr3 _temp3033= yyls; struct Cyc_Yyltype* _temp3035= _temp3033.curr
+ yylsp_offset; if( _temp3033.base == 0? 1:( _temp3035 < _temp3033.base? 1:
_temp3035 >= _temp3033.last_plus_one)){ _throw( Null_Exception);}* _temp3035;}).last_line);
_temp3029;}); _temp3028;})); _temp3027;}); _temp3026;}); break; case 60: _LL3025:
yyval=({ struct _tagged_ptr2 _temp3037= yyvs; struct _xtunion_struct** _temp3039=
_temp3037.curr + yyvsp_offset; if( _temp3037.base == 0? 1:( _temp3039 <
_temp3037.base? 1: _temp3039 >= _temp3037.last_plus_one)){ _throw(
Null_Exception);}* _temp3039;}); break; case 61: _LL3036: yyval=({ struct
_tagged_ptr2 _temp3041= yyvs; struct _xtunion_struct** _temp3043= _temp3041.curr
+ yyvsp_offset; if( _temp3041.base == 0? 1:( _temp3043 < _temp3041.base? 1:
_temp3043 >= _temp3041.last_plus_one)){ _throw( Null_Exception);}* _temp3043;});
break; case 62: _LL3040: yyval=({ struct _tagged_ptr2 _temp3045= yyvs; struct
_xtunion_struct** _temp3047= _temp3045.curr + yyvsp_offset; if( _temp3045.base
== 0? 1:( _temp3047 < _temp3045.base? 1: _temp3047 >= _temp3045.last_plus_one)){
_throw( Null_Exception);}* _temp3047;}); break; case 63: _LL3044: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3049=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3049[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3050; _temp3050.tag= Cyc_TypeSpecifier_tok_tag;
_temp3050.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp3051= yyvs; struct _xtunion_struct** _temp3053=
_temp3051.curr + yyvsp_offset; if( _temp3051.base == 0? 1:( _temp3053 <
_temp3051.base? 1: _temp3053 >= _temp3051.last_plus_one)){ _throw(
Null_Exception);}* _temp3053;})),(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3054= yyls; struct Cyc_Yyltype*
_temp3056= _temp3054.curr + yylsp_offset; if( _temp3054.base == 0? 1:( _temp3056
< _temp3054.base? 1: _temp3056 >= _temp3054.last_plus_one)){ _throw(
Null_Exception);}* _temp3056;}).first_line,({ struct _tagged_ptr3 _temp3057=
yyls; struct Cyc_Yyltype* _temp3059= _temp3057.curr + yylsp_offset; if(
_temp3057.base == 0? 1:( _temp3059 < _temp3057.base? 1: _temp3059 >= _temp3057.last_plus_one)){
_throw( Null_Exception);}* _temp3059;}).last_line)); _temp3050;}); _temp3049;});
break; case 64: _LL3048: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3061=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3061[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3062; _temp3062.tag=
Cyc_TypeSpecifier_tok_tag; _temp3062.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3063= yyvs; struct
_xtunion_struct** _temp3065= _temp3063.curr +( yyvsp_offset - 2); if( _temp3063.base
== 0? 1:( _temp3065 < _temp3063.base? 1: _temp3065 >= _temp3063.last_plus_one)){
_throw( Null_Exception);}* _temp3065;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp3066= yyvs;
struct _xtunion_struct** _temp3068= _temp3066.curr + yyvsp_offset; if( _temp3066.base
== 0? 1:( _temp3068 < _temp3066.base? 1: _temp3068 >= _temp3066.last_plus_one)){
_throw( Null_Exception);}* _temp3068;})))), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3069= yyls; struct Cyc_Yyltype* _temp3071= _temp3069.curr
+( yylsp_offset - 2); if( _temp3069.base == 0? 1:( _temp3071 < _temp3069.base? 1:
_temp3071 >= _temp3069.last_plus_one)){ _throw( Null_Exception);}* _temp3071;}).first_line,({
struct _tagged_ptr3 _temp3072= yyls; struct Cyc_Yyltype* _temp3074= _temp3072.curr
+ yylsp_offset; if( _temp3072.base == 0? 1:( _temp3074 < _temp3072.base? 1:
_temp3074 >= _temp3072.last_plus_one)){ _throw( Null_Exception);}* _temp3074;}).last_line));
_temp3062;}); _temp3061;}); break; case 65: _LL3060: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3076=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3076[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3077; _temp3077.tag= Cyc_TypeSpecifier_tok_tag;
_temp3077.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp3078=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp3078[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp3079; _temp3079.tag=
Cyc_Absyn_TypedefType_tag; _temp3079.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3080= yyvs; struct _xtunion_struct** _temp3082= _temp3080.curr
+( yyvsp_offset - 1); if( _temp3080.base == 0? 1:( _temp3082 < _temp3080.base? 1:
_temp3082 >= _temp3080.last_plus_one)){ _throw( Null_Exception);}* _temp3082;}));
_temp3079.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3083= yyvs;
struct _xtunion_struct** _temp3085= _temp3083.curr + yyvsp_offset; if( _temp3083.base
== 0? 1:( _temp3085 < _temp3083.base? 1: _temp3085 >= _temp3083.last_plus_one)){
_throw( Null_Exception);}* _temp3085;})); _temp3079.f3= 0; _temp3079;});
_temp3078;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3086= yyls;
struct Cyc_Yyltype* _temp3088= _temp3086.curr +( yylsp_offset - 1); if(
_temp3086.base == 0? 1:( _temp3088 < _temp3086.base? 1: _temp3088 >= _temp3086.last_plus_one)){
_throw( Null_Exception);}* _temp3088;}).first_line,({ struct _tagged_ptr3
_temp3089= yyls; struct Cyc_Yyltype* _temp3091= _temp3089.curr + yylsp_offset;
if( _temp3089.base == 0? 1:( _temp3091 < _temp3089.base? 1: _temp3091 >=
_temp3089.last_plus_one)){ _throw( Null_Exception);}* _temp3091;}).last_line));
_temp3077;}); _temp3076;}); break; case 66: _LL3075: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3093=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3093[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3094; _temp3094.tag= Cyc_TypeSpecifier_tok_tag;
_temp3094.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp3095=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp3095[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp3096; _temp3096.tag= Cyc_Absyn_TupleType_tag;
_temp3096.f1=(( struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3097= yyls; struct Cyc_Yyltype* _temp3099= _temp3097.curr +(
yylsp_offset - 1); if( _temp3097.base == 0? 1:( _temp3099 < _temp3097.base? 1:
_temp3099 >= _temp3097.last_plus_one)){ _throw( Null_Exception);}* _temp3099;}).first_line,({
struct _tagged_ptr3 _temp3100= yyls; struct Cyc_Yyltype* _temp3102= _temp3100.curr
+( yylsp_offset - 1); if( _temp3100.base == 0? 1:( _temp3102 < _temp3100.base? 1:
_temp3102 >= _temp3100.last_plus_one)){ _throw( Null_Exception);}* _temp3102;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3103= yyvs; struct _xtunion_struct** _temp3105=
_temp3103.curr +( yyvsp_offset - 1); if( _temp3103.base == 0? 1:( _temp3105 <
_temp3103.base? 1: _temp3105 >= _temp3103.last_plus_one)){ _throw(
Null_Exception);}* _temp3105;})))); _temp3096;}); _temp3095;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3106= yyls; struct Cyc_Yyltype* _temp3108= _temp3106.curr
+( yylsp_offset - 3); if( _temp3106.base == 0? 1:( _temp3108 < _temp3106.base? 1:
_temp3108 >= _temp3106.last_plus_one)){ _throw( Null_Exception);}* _temp3108;}).first_line,({
struct _tagged_ptr3 _temp3109= yyls; struct Cyc_Yyltype* _temp3111= _temp3109.curr
+ yylsp_offset; if( _temp3109.base == 0? 1:( _temp3111 < _temp3109.base? 1:
_temp3111 >= _temp3109.last_plus_one)){ _throw( Null_Exception);}* _temp3111;}).last_line));
_temp3094;}); _temp3093;}); break; case 67: _LL3092: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3113=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3113[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3114; _temp3114.tag= Cyc_TypeSpecifier_tok_tag;
_temp3114.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3115=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3115[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3116; _temp3116.tag= Cyc_Absyn_RgnHandleType_tag; _temp3116.f1=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp3117= yyvs; struct _xtunion_struct** _temp3119=
_temp3117.curr +( yyvsp_offset - 1); if( _temp3117.base == 0? 1:( _temp3119 <
_temp3117.base? 1: _temp3119 >= _temp3117.last_plus_one)){ _throw(
Null_Exception);}* _temp3119;})); _temp3116;}); _temp3115;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3120= yyls; struct Cyc_Yyltype* _temp3122= _temp3120.curr
+( yylsp_offset - 3); if( _temp3120.base == 0? 1:( _temp3122 < _temp3120.base? 1:
_temp3122 >= _temp3120.last_plus_one)){ _throw( Null_Exception);}* _temp3122;}).first_line,({
struct _tagged_ptr3 _temp3123= yyls; struct Cyc_Yyltype* _temp3125= _temp3123.curr
+ yylsp_offset; if( _temp3123.base == 0? 1:( _temp3125 < _temp3123.base? 1:
_temp3125 >= _temp3123.last_plus_one)){ _throw( Null_Exception);}* _temp3125;}).last_line));
_temp3114;}); _temp3113;}); break; case 68: _LL3112: yyval=( struct
_xtunion_struct*)({ struct Cyc_Kind_tok_struct* _temp3127=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp3127[ 0]=({ struct Cyc_Kind_tok_struct
_temp3128; _temp3128.tag= Cyc_Kind_tok_tag; _temp3128.f1=( void*) Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3129= yyvs; struct
_xtunion_struct** _temp3131= _temp3129.curr + yyvsp_offset; if( _temp3129.base
== 0? 1:( _temp3131 < _temp3129.base? 1: _temp3131 >= _temp3129.last_plus_one)){
_throw( Null_Exception);}* _temp3131;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3132= yyls; struct Cyc_Yyltype* _temp3134= _temp3132.curr +
yylsp_offset; if( _temp3132.base == 0? 1:( _temp3134 < _temp3132.base? 1:
_temp3134 >= _temp3132.last_plus_one)){ _throw( Null_Exception);}* _temp3134;}).first_line,({
struct _tagged_ptr3 _temp3135= yyls; struct Cyc_Yyltype* _temp3137= _temp3135.curr
+ yylsp_offset; if( _temp3135.base == 0? 1:( _temp3137 < _temp3135.base? 1:
_temp3137 >= _temp3135.last_plus_one)){ _throw( Null_Exception);}* _temp3137;}).last_line));
_temp3128;}); _temp3127;}); break; case 69: _LL3126: { struct _tagged_string*
_temp3144; void* _temp3146; struct _tuple1 _temp3142=* Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3139= yyvs; struct _xtunion_struct** _temp3141=
_temp3139.curr + yyvsp_offset; if( _temp3139.base == 0? 1:( _temp3141 <
_temp3139.base? 1: _temp3141 >= _temp3139.last_plus_one)){ _throw(
Null_Exception);}* _temp3141;})); _LL3147: _temp3146= _temp3142.f1; goto _LL3145;
_LL3145: _temp3144= _temp3142.f2; goto _LL3143; _LL3143: if( _temp3146 !=( void*)
Cyc_Absyn_Loc_n){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp3148=(
char*)"bad kind in type specifier"; struct _tagged_string _temp3149; _temp3149.curr=
_temp3148; _temp3149.base= _temp3148; _temp3149.last_plus_one= _temp3148 + 27;
_temp3149;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3150= yyls;
struct Cyc_Yyltype* _temp3152= _temp3150.curr + yylsp_offset; if( _temp3150.base
== 0? 1:( _temp3152 < _temp3150.base? 1: _temp3152 >= _temp3150.last_plus_one)){
_throw( Null_Exception);}* _temp3152;}).first_line,({ struct _tagged_ptr3
_temp3153= yyls; struct Cyc_Yyltype* _temp3155= _temp3153.curr + yylsp_offset;
if( _temp3153.base == 0? 1:( _temp3155 < _temp3153.base? 1: _temp3155 >=
_temp3153.last_plus_one)){ _throw( Null_Exception);}* _temp3155;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_Kind_tok_struct* _temp3156=(
struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp3156[ 0]=({ struct Cyc_Kind_tok_struct _temp3157; _temp3157.tag= Cyc_Kind_tok_tag;
_temp3157.f1=( void*) Cyc_Parse_id_to_kind(* _temp3144, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3158= yyls; struct Cyc_Yyltype* _temp3160= _temp3158.curr
+ yylsp_offset; if( _temp3158.base == 0? 1:( _temp3160 < _temp3158.base? 1:
_temp3160 >= _temp3158.last_plus_one)){ _throw( Null_Exception);}* _temp3160;}).first_line,({
struct _tagged_ptr3 _temp3161= yyls; struct Cyc_Yyltype* _temp3163= _temp3161.curr
+ yylsp_offset; if( _temp3161.base == 0? 1:( _temp3163 < _temp3161.base? 1:
_temp3163 >= _temp3161.last_plus_one)){ _throw( Null_Exception);}* _temp3163;}).last_line));
_temp3157;}); _temp3156;}); break;} case 70: _LL3138: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeQual_tok_struct* _temp3165=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp3165[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3166; _temp3166.tag= Cyc_TypeQual_tok_tag; _temp3166.f1=({ struct Cyc_Absyn_Tqual*
_temp3167=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp3167->q_const= 1; _temp3167->q_volatile= 0; _temp3167->q_restrict= 0;
_temp3167;}); _temp3166;}); _temp3165;}); break; case 71: _LL3164: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeQual_tok_struct* _temp3169=( struct
Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp3169[ 0]=({ struct Cyc_TypeQual_tok_struct _temp3170; _temp3170.tag= Cyc_TypeQual_tok_tag;
_temp3170.f1=({ struct Cyc_Absyn_Tqual* _temp3171=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp3171->q_const= 0;
_temp3171->q_volatile= 1; _temp3171->q_restrict= 0; _temp3171;}); _temp3170;});
_temp3169;}); break; case 72: _LL3168: yyval=( struct _xtunion_struct*)({ struct
Cyc_TypeQual_tok_struct* _temp3173=( struct Cyc_TypeQual_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeQual_tok_struct)); _temp3173[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3174; _temp3174.tag= Cyc_TypeQual_tok_tag; _temp3174.f1=({ struct Cyc_Absyn_Tqual*
_temp3175=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp3175->q_const= 0; _temp3175->q_volatile= 0; _temp3175->q_restrict= 1;
_temp3175;}); _temp3174;}); _temp3173;}); break; case 73: _LL3172: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3177=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3177[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3178; _temp3178.tag=
Cyc_TypeSpecifier_tok_tag; _temp3178.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3179=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3179[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3180; _temp3180.tag= Cyc_Parse_Decl_spec_tag;
_temp3180.f1=({ struct Cyc_Absyn_Decl* _temp3181=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp3181->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp3188=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp3188[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp3189; _temp3189.tag= Cyc_Absyn_Enum_d_tag; _temp3189.f1=({ struct Cyc_Absyn_Enumdecl*
_temp3190=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp3190->sc=( void*)(( void*) Cyc_Absyn_Public); _temp3190->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3194= yyvs; struct _xtunion_struct** _temp3196=
_temp3194.curr +( yyvsp_offset - 3); if( _temp3194.base == 0? 1:( _temp3196 <
_temp3194.base? 1: _temp3196 >= _temp3194.last_plus_one)){ _throw(
Null_Exception);}* _temp3196;})); _temp3190->fields= Cyc_yyget_EnumfieldList_tok(({
struct _tagged_ptr2 _temp3191= yyvs; struct _xtunion_struct** _temp3193=
_temp3191.curr +( yyvsp_offset - 1); if( _temp3191.base == 0? 1:( _temp3193 <
_temp3191.base? 1: _temp3193 >= _temp3191.last_plus_one)){ _throw(
Null_Exception);}* _temp3193;})); _temp3190;}); _temp3189;}); _temp3188;}));
_temp3181->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3182=
yyls; struct Cyc_Yyltype* _temp3184= _temp3182.curr +( yylsp_offset - 4); if(
_temp3182.base == 0? 1:( _temp3184 < _temp3182.base? 1: _temp3184 >= _temp3182.last_plus_one)){
_throw( Null_Exception);}* _temp3184;}).first_line,({ struct _tagged_ptr3
_temp3185= yyls; struct Cyc_Yyltype* _temp3187= _temp3185.curr + yylsp_offset;
if( _temp3185.base == 0? 1:( _temp3187 < _temp3185.base? 1: _temp3187 >=
_temp3185.last_plus_one)){ _throw( Null_Exception);}* _temp3187;}).last_line);
_temp3181;}); _temp3180;}); _temp3179;})); _temp3178;}); _temp3177;}); break;
case 74: _LL3176: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3198=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3198[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3199; _temp3199.tag=
Cyc_TypeSpecifier_tok_tag; _temp3199.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp3200=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp3200[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp3201; _temp3201.tag= Cyc_Absyn_EnumType_tag;
_temp3201.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3202= yyvs;
struct _xtunion_struct** _temp3204= _temp3202.curr + yyvsp_offset; if( _temp3202.base
== 0? 1:( _temp3204 < _temp3202.base? 1: _temp3204 >= _temp3202.last_plus_one)){
_throw( Null_Exception);}* _temp3204;})); _temp3201.f2= 0; _temp3201;});
_temp3200;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3205= yyls;
struct Cyc_Yyltype* _temp3207= _temp3205.curr +( yylsp_offset - 1); if(
_temp3205.base == 0? 1:( _temp3207 < _temp3205.base? 1: _temp3207 >= _temp3205.last_plus_one)){
_throw( Null_Exception);}* _temp3207;}).first_line,({ struct _tagged_ptr3
_temp3208= yyls; struct Cyc_Yyltype* _temp3210= _temp3208.curr + yylsp_offset;
if( _temp3208.base == 0? 1:( _temp3210 < _temp3208.base? 1: _temp3210 >=
_temp3208.last_plus_one)){ _throw( Null_Exception);}* _temp3210;}).last_line));
_temp3199;}); _temp3198;}); break; case 75: _LL3197: yyval=( struct
_xtunion_struct*)({ struct Cyc_Enumfield_tok_struct* _temp3212=( struct Cyc_Enumfield_tok_struct*)
GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct)); _temp3212[ 0]=({ struct
Cyc_Enumfield_tok_struct _temp3213; _temp3213.tag= Cyc_Enumfield_tok_tag;
_temp3213.f1=({ struct Cyc_Absyn_Enumfield* _temp3214=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3214->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3221= yyvs; struct _xtunion_struct** _temp3223=
_temp3221.curr + yyvsp_offset; if( _temp3221.base == 0? 1:( _temp3223 <
_temp3221.base? 1: _temp3223 >= _temp3221.last_plus_one)){ _throw(
Null_Exception);}* _temp3223;})); _temp3214->tag= 0; _temp3214->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3215= yyls; struct Cyc_Yyltype* _temp3217= _temp3215.curr
+ yylsp_offset; if( _temp3215.base == 0? 1:( _temp3217 < _temp3215.base? 1:
_temp3217 >= _temp3215.last_plus_one)){ _throw( Null_Exception);}* _temp3217;}).first_line,({
struct _tagged_ptr3 _temp3218= yyls; struct Cyc_Yyltype* _temp3220= _temp3218.curr
+ yylsp_offset; if( _temp3218.base == 0? 1:( _temp3220 < _temp3218.base? 1:
_temp3220 >= _temp3218.last_plus_one)){ _throw( Null_Exception);}* _temp3220;}).last_line);
_temp3214;}); _temp3213;}); _temp3212;}); break; case 76: _LL3211: yyval=(
struct _xtunion_struct*)({ struct Cyc_Enumfield_tok_struct* _temp3225=( struct
Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp3225[ 0]=({ struct Cyc_Enumfield_tok_struct _temp3226; _temp3226.tag= Cyc_Enumfield_tok_tag;
_temp3226.f1=({ struct Cyc_Absyn_Enumfield* _temp3227=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3227->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3237= yyvs; struct _xtunion_struct** _temp3239=
_temp3237.curr +( yyvsp_offset - 2); if( _temp3237.base == 0? 1:( _temp3239 <
_temp3237.base? 1: _temp3239 >= _temp3237.last_plus_one)){ _throw(
Null_Exception);}* _temp3239;})); _temp3227->tag=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3234= yyvs; struct _xtunion_struct** _temp3236=
_temp3234.curr + yyvsp_offset; if( _temp3234.base == 0? 1:( _temp3236 <
_temp3234.base? 1: _temp3236 >= _temp3234.last_plus_one)){ _throw(
Null_Exception);}* _temp3236;})); _temp3227->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3228= yyls; struct Cyc_Yyltype* _temp3230= _temp3228.curr
+( yylsp_offset - 2); if( _temp3228.base == 0? 1:( _temp3230 < _temp3228.base? 1:
_temp3230 >= _temp3228.last_plus_one)){ _throw( Null_Exception);}* _temp3230;}).first_line,({
struct _tagged_ptr3 _temp3231= yyls; struct Cyc_Yyltype* _temp3233= _temp3231.curr
+ yylsp_offset; if( _temp3231.base == 0? 1:( _temp3233 < _temp3231.base? 1:
_temp3233 >= _temp3231.last_plus_one)){ _throw( Null_Exception);}* _temp3233;}).last_line);
_temp3227;}); _temp3226;}); _temp3225;}); break; case 77: _LL3224: yyval=(
struct _xtunion_struct*)({ struct Cyc_EnumfieldList_tok_struct* _temp3241=(
struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp3241[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp3242; _temp3242.tag=
Cyc_EnumfieldList_tok_tag; _temp3242.f1=({ struct Cyc_List_List* _temp3243=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3243->hd=(
void*) Cyc_yyget_Enumfield_tok(({ struct _tagged_ptr2 _temp3244= yyvs; struct
_xtunion_struct** _temp3246= _temp3244.curr + yyvsp_offset; if( _temp3244.base
== 0? 1:( _temp3246 < _temp3244.base? 1: _temp3246 >= _temp3244.last_plus_one)){
_throw( Null_Exception);}* _temp3246;})); _temp3243->tl= 0; _temp3243;});
_temp3242;}); _temp3241;}); break; case 78: _LL3240: yyval=( struct
_xtunion_struct*)({ struct Cyc_EnumfieldList_tok_struct* _temp3248=( struct Cyc_EnumfieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct)); _temp3248[ 0]=({
struct Cyc_EnumfieldList_tok_struct _temp3249; _temp3249.tag= Cyc_EnumfieldList_tok_tag;
_temp3249.f1=({ struct Cyc_List_List* _temp3250=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3250->hd=( void*) Cyc_yyget_Enumfield_tok(({
struct _tagged_ptr2 _temp3254= yyvs; struct _xtunion_struct** _temp3256=
_temp3254.curr +( yyvsp_offset - 2); if( _temp3254.base == 0? 1:( _temp3256 <
_temp3254.base? 1: _temp3256 >= _temp3254.last_plus_one)){ _throw(
Null_Exception);}* _temp3256;})); _temp3250->tl= Cyc_yyget_EnumfieldList_tok(({
struct _tagged_ptr2 _temp3251= yyvs; struct _xtunion_struct** _temp3253=
_temp3251.curr + yyvsp_offset; if( _temp3251.base == 0? 1:( _temp3253 <
_temp3251.base? 1: _temp3253 >= _temp3251.last_plus_one)){ _throw(
Null_Exception);}* _temp3253;})); _temp3250;}); _temp3249;}); _temp3248;});
break; case 79: _LL3247: { struct Cyc_Absyn_Decl* d;{ void* _temp3261= Cyc_yyget_StructOrUnion_tok(({
struct _tagged_ptr2 _temp3258= yyvs; struct _xtunion_struct** _temp3260=
_temp3258.curr +( yyvsp_offset - 3); if( _temp3258.base == 0? 1:( _temp3260 <
_temp3258.base? 1: _temp3260 >= _temp3258.last_plus_one)){ _throw(
Null_Exception);}* _temp3260;})); _LL3263: if(( int) _temp3261 == Cyc_Parse_Struct_su){
goto _LL3264;} else{ goto _LL3265;} _LL3265: if(( int) _temp3261 == Cyc_Parse_Union_su){
goto _LL3266;} else{ goto _LL3262;} _LL3264: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public, 0, 0,({ struct Cyc_Core_Opt* _temp3267=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3267->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3268= yyvs; struct _xtunion_struct** _temp3270=
_temp3268.curr +( yyvsp_offset - 1); if( _temp3268.base == 0? 1:( _temp3270 <
_temp3268.base? 1: _temp3270 >= _temp3268.last_plus_one)){ _throw(
Null_Exception);}* _temp3270;})); _temp3267;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3271= yyls; struct Cyc_Yyltype* _temp3273= _temp3271.curr
+( yylsp_offset - 3); if( _temp3271.base == 0? 1:( _temp3273 < _temp3271.base? 1:
_temp3273 >= _temp3271.last_plus_one)){ _throw( Null_Exception);}* _temp3273;}).first_line,({
struct _tagged_ptr3 _temp3274= yyls; struct Cyc_Yyltype* _temp3276= _temp3274.curr
+ yylsp_offset; if( _temp3274.base == 0? 1:( _temp3276 < _temp3274.base? 1:
_temp3276 >= _temp3274.last_plus_one)){ _throw( Null_Exception);}* _temp3276;}).last_line));
goto _LL3262; _LL3266: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public, 0, 0,({
struct Cyc_Core_Opt* _temp3277=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3277->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3278= yyvs; struct _xtunion_struct** _temp3280=
_temp3278.curr +( yyvsp_offset - 1); if( _temp3278.base == 0? 1:( _temp3280 <
_temp3278.base? 1: _temp3280 >= _temp3278.last_plus_one)){ _throw(
Null_Exception);}* _temp3280;})); _temp3277;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3281= yyls; struct Cyc_Yyltype* _temp3283= _temp3281.curr
+( yylsp_offset - 3); if( _temp3281.base == 0? 1:( _temp3283 < _temp3281.base? 1:
_temp3283 >= _temp3281.last_plus_one)){ _throw( Null_Exception);}* _temp3283;}).first_line,({
struct _tagged_ptr3 _temp3284= yyls; struct Cyc_Yyltype* _temp3286= _temp3284.curr
+ yylsp_offset; if( _temp3284.base == 0? 1:( _temp3286 < _temp3284.base? 1:
_temp3286 >= _temp3284.last_plus_one)){ _throw( Null_Exception);}* _temp3286;}).last_line));
goto _LL3262; _LL3262:;} yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3287=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3287[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3288; _temp3288.tag=
Cyc_TypeSpecifier_tok_tag; _temp3288.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3289=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3289[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3290; _temp3290.tag= Cyc_Parse_Decl_spec_tag;
_temp3290.f1= d; _temp3290;}); _temp3289;})); _temp3288;}); _temp3287;}); Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp3291=( char*)"anonymous structs/unions";
struct _tagged_string _temp3292; _temp3292.curr= _temp3291; _temp3292.base=
_temp3291; _temp3292.last_plus_one= _temp3291 + 25; _temp3292;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3293= yyls; struct Cyc_Yyltype* _temp3295= _temp3293.curr
+( yylsp_offset - 3); if( _temp3293.base == 0? 1:( _temp3295 < _temp3293.base? 1:
_temp3295 >= _temp3293.last_plus_one)){ _throw( Null_Exception);}* _temp3295;}).first_line,({
struct _tagged_ptr3 _temp3296= yyls; struct Cyc_Yyltype* _temp3298= _temp3296.curr
+ yylsp_offset; if( _temp3296.base == 0? 1:( _temp3298 < _temp3296.base? 1:
_temp3298 >= _temp3296.last_plus_one)){ _throw( Null_Exception);}* _temp3298;}).last_line));
break;} case 80: _LL3257: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3300= yyls; struct Cyc_Yyltype* _temp3302= _temp3300.curr
+( yylsp_offset - 3); if( _temp3300.base == 0? 1:( _temp3302 < _temp3300.base? 1:
_temp3302 >= _temp3300.last_plus_one)){ _throw( Null_Exception);}* _temp3302;}).first_line,({
struct _tagged_ptr3 _temp3303= yyls; struct Cyc_Yyltype* _temp3305= _temp3303.curr
+( yylsp_offset - 3); if( _temp3303.base == 0? 1:( _temp3305 < _temp3303.base? 1:
_temp3305 >= _temp3303.last_plus_one)){ _throw( Null_Exception);}* _temp3305;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3306= yyvs; struct
_xtunion_struct** _temp3308= _temp3306.curr +( yyvsp_offset - 3); if( _temp3306.base
== 0? 1:( _temp3308 < _temp3306.base? 1: _temp3308 >= _temp3306.last_plus_one)){
_throw( Null_Exception);}* _temp3308;}))); struct Cyc_Absyn_Decl* d;{ void*
_temp3312= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3309= yyvs;
struct _xtunion_struct** _temp3311= _temp3309.curr +( yyvsp_offset - 5); if(
_temp3309.base == 0? 1:( _temp3311 < _temp3309.base? 1: _temp3311 >= _temp3309.last_plus_one)){
_throw( Null_Exception);}* _temp3311;})); _LL3314: if(( int) _temp3312 == Cyc_Parse_Struct_su){
goto _LL3315;} else{ goto _LL3316;} _LL3316: if(( int) _temp3312 == Cyc_Parse_Union_su){
goto _LL3317;} else{ goto _LL3313;} _LL3315: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp3318=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3318->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3319= yyvs; struct _xtunion_struct** _temp3321=
_temp3319.curr +( yyvsp_offset - 4); if( _temp3319.base == 0? 1:( _temp3321 <
_temp3319.base? 1: _temp3321 >= _temp3319.last_plus_one)){ _throw(
Null_Exception);}* _temp3321;})); _temp3318;}), ts,({ struct Cyc_Core_Opt*
_temp3322=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3322->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp3323= yyvs; struct _xtunion_struct** _temp3325= _temp3323.curr +(
yyvsp_offset - 1); if( _temp3323.base == 0? 1:( _temp3325 < _temp3323.base? 1:
_temp3325 >= _temp3323.last_plus_one)){ _throw( Null_Exception);}* _temp3325;}));
_temp3322;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3326=
yyls; struct Cyc_Yyltype* _temp3328= _temp3326.curr +( yylsp_offset - 5); if(
_temp3326.base == 0? 1:( _temp3328 < _temp3326.base? 1: _temp3328 >= _temp3326.last_plus_one)){
_throw( Null_Exception);}* _temp3328;}).first_line,({ struct _tagged_ptr3
_temp3329= yyls; struct Cyc_Yyltype* _temp3331= _temp3329.curr + yylsp_offset;
if( _temp3329.base == 0? 1:( _temp3331 < _temp3329.base? 1: _temp3331 >=
_temp3329.last_plus_one)){ _throw( Null_Exception);}* _temp3331;}).last_line));
goto _LL3313; _LL3317: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp3332=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3332->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3333= yyvs; struct _xtunion_struct** _temp3335= _temp3333.curr
+( yyvsp_offset - 4); if( _temp3333.base == 0? 1:( _temp3335 < _temp3333.base? 1:
_temp3335 >= _temp3333.last_plus_one)){ _throw( Null_Exception);}* _temp3335;}));
_temp3332;}), ts,({ struct Cyc_Core_Opt* _temp3336=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3336->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3337= yyvs; struct _xtunion_struct** _temp3339=
_temp3337.curr +( yyvsp_offset - 1); if( _temp3337.base == 0? 1:( _temp3339 <
_temp3337.base? 1: _temp3339 >= _temp3337.last_plus_one)){ _throw(
Null_Exception);}* _temp3339;})); _temp3336;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3340= yyls; struct Cyc_Yyltype* _temp3342= _temp3340.curr
+( yylsp_offset - 5); if( _temp3340.base == 0? 1:( _temp3342 < _temp3340.base? 1:
_temp3342 >= _temp3340.last_plus_one)){ _throw( Null_Exception);}* _temp3342;}).first_line,({
struct _tagged_ptr3 _temp3343= yyls; struct Cyc_Yyltype* _temp3345= _temp3343.curr
+ yylsp_offset; if( _temp3343.base == 0? 1:( _temp3345 < _temp3343.base? 1:
_temp3345 >= _temp3343.last_plus_one)){ _throw( Null_Exception);}* _temp3345;}).last_line));
goto _LL3313; _LL3313:;} yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3346=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3346[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3347; _temp3347.tag=
Cyc_TypeSpecifier_tok_tag; _temp3347.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3348=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3348[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3349; _temp3349.tag= Cyc_Parse_Decl_spec_tag;
_temp3349.f1= d; _temp3349;}); _temp3348;})); _temp3347;}); _temp3346;}); break;}
case 81: _LL3299: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3351= yyls; struct Cyc_Yyltype* _temp3353= _temp3351.curr
+( yylsp_offset - 3); if( _temp3351.base == 0? 1:( _temp3353 < _temp3351.base? 1:
_temp3353 >= _temp3351.last_plus_one)){ _throw( Null_Exception);}* _temp3353;}).first_line,({
struct _tagged_ptr3 _temp3354= yyls; struct Cyc_Yyltype* _temp3356= _temp3354.curr
+( yylsp_offset - 3); if( _temp3354.base == 0? 1:( _temp3356 < _temp3354.base? 1:
_temp3356 >= _temp3354.last_plus_one)){ _throw( Null_Exception);}* _temp3356;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3357= yyvs; struct
_xtunion_struct** _temp3359= _temp3357.curr +( yyvsp_offset - 3); if( _temp3357.base
== 0? 1:( _temp3359 < _temp3357.base? 1: _temp3359 >= _temp3357.last_plus_one)){
_throw( Null_Exception);}* _temp3359;}))); struct Cyc_Absyn_Decl* d;{ void*
_temp3363= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3360= yyvs;
struct _xtunion_struct** _temp3362= _temp3360.curr +( yyvsp_offset - 5); if(
_temp3360.base == 0? 1:( _temp3362 < _temp3360.base? 1: _temp3362 >= _temp3360.last_plus_one)){
_throw( Null_Exception);}* _temp3362;})); _LL3365: if(( int) _temp3363 == Cyc_Parse_Struct_su){
goto _LL3366;} else{ goto _LL3367;} _LL3367: if(( int) _temp3363 == Cyc_Parse_Union_su){
goto _LL3368;} else{ goto _LL3364;} _LL3366: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp3369=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3369->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3370= yyvs; struct _xtunion_struct** _temp3372=
_temp3370.curr +( yyvsp_offset - 4); if( _temp3370.base == 0? 1:( _temp3372 <
_temp3370.base? 1: _temp3372 >= _temp3370.last_plus_one)){ _throw(
Null_Exception);}* _temp3372;})); _temp3369;}), ts,({ struct Cyc_Core_Opt*
_temp3373=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3373->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp3374= yyvs; struct _xtunion_struct** _temp3376= _temp3374.curr +(
yyvsp_offset - 1); if( _temp3374.base == 0? 1:( _temp3376 < _temp3374.base? 1:
_temp3376 >= _temp3374.last_plus_one)){ _throw( Null_Exception);}* _temp3376;}));
_temp3373;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3377=
yyls; struct Cyc_Yyltype* _temp3379= _temp3377.curr +( yylsp_offset - 5); if(
_temp3377.base == 0? 1:( _temp3379 < _temp3377.base? 1: _temp3379 >= _temp3377.last_plus_one)){
_throw( Null_Exception);}* _temp3379;}).first_line,({ struct _tagged_ptr3
_temp3380= yyls; struct Cyc_Yyltype* _temp3382= _temp3380.curr + yylsp_offset;
if( _temp3380.base == 0? 1:( _temp3382 < _temp3380.base? 1: _temp3382 >=
_temp3380.last_plus_one)){ _throw( Null_Exception);}* _temp3382;}).last_line));
goto _LL3364; _LL3368: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp3383=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3383->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3384= yyvs; struct _xtunion_struct** _temp3386= _temp3384.curr
+( yyvsp_offset - 4); if( _temp3384.base == 0? 1:( _temp3386 < _temp3384.base? 1:
_temp3386 >= _temp3384.last_plus_one)){ _throw( Null_Exception);}* _temp3386;}));
_temp3383;}), ts,({ struct Cyc_Core_Opt* _temp3387=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3387->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3388= yyvs; struct _xtunion_struct** _temp3390=
_temp3388.curr +( yyvsp_offset - 1); if( _temp3388.base == 0? 1:( _temp3390 <
_temp3388.base? 1: _temp3390 >= _temp3388.last_plus_one)){ _throw(
Null_Exception);}* _temp3390;})); _temp3387;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3391= yyls; struct Cyc_Yyltype* _temp3393= _temp3391.curr
+( yylsp_offset - 5); if( _temp3391.base == 0? 1:( _temp3393 < _temp3391.base? 1:
_temp3393 >= _temp3391.last_plus_one)){ _throw( Null_Exception);}* _temp3393;}).first_line,({
struct _tagged_ptr3 _temp3394= yyls; struct Cyc_Yyltype* _temp3396= _temp3394.curr
+ yylsp_offset; if( _temp3394.base == 0? 1:( _temp3396 < _temp3394.base? 1:
_temp3396 >= _temp3394.last_plus_one)){ _throw( Null_Exception);}* _temp3396;}).last_line));
goto _LL3364; _LL3364:;} yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3397=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3397[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3398; _temp3398.tag=
Cyc_TypeSpecifier_tok_tag; _temp3398.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3399=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3399[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3400; _temp3400.tag= Cyc_Parse_Decl_spec_tag;
_temp3400.f1= d; _temp3400;}); _temp3399;})); _temp3398;}); _temp3397;}); break;}
case 82: _LL3350:{ void* _temp3405= Cyc_yyget_StructOrUnion_tok(({ struct
_tagged_ptr2 _temp3402= yyvs; struct _xtunion_struct** _temp3404= _temp3402.curr
+( yyvsp_offset - 2); if( _temp3402.base == 0? 1:( _temp3404 < _temp3402.base? 1:
_temp3404 >= _temp3402.last_plus_one)){ _throw( Null_Exception);}* _temp3404;}));
_LL3407: if(( int) _temp3405 == Cyc_Parse_Struct_su){ goto _LL3408;} else{ goto
_LL3409;} _LL3409: if(( int) _temp3405 == Cyc_Parse_Union_su){ goto _LL3410;}
else{ goto _LL3406;} _LL3408: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3411=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3411[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3412; _temp3412.tag=
Cyc_TypeSpecifier_tok_tag; _temp3412.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp3413=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3413[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp3414; _temp3414.tag= Cyc_Absyn_StructType_tag;
_temp3414.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3415= yyvs; struct _xtunion_struct** _temp3417= _temp3415.curr +(
yyvsp_offset - 1); if( _temp3415.base == 0? 1:( _temp3417 < _temp3415.base? 1:
_temp3417 >= _temp3415.last_plus_one)){ _throw( Null_Exception);}* _temp3417;}));
_temp3414.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3418= yyvs;
struct _xtunion_struct** _temp3420= _temp3418.curr + yyvsp_offset; if( _temp3418.base
== 0? 1:( _temp3420 < _temp3418.base? 1: _temp3420 >= _temp3418.last_plus_one)){
_throw( Null_Exception);}* _temp3420;})); _temp3414.f3= 0; _temp3414;});
_temp3413;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3421= yyls;
struct Cyc_Yyltype* _temp3423= _temp3421.curr +( yylsp_offset - 2); if(
_temp3421.base == 0? 1:( _temp3423 < _temp3421.base? 1: _temp3423 >= _temp3421.last_plus_one)){
_throw( Null_Exception);}* _temp3423;}).first_line,({ struct _tagged_ptr3
_temp3424= yyls; struct Cyc_Yyltype* _temp3426= _temp3424.curr + yylsp_offset;
if( _temp3424.base == 0? 1:( _temp3426 < _temp3424.base? 1: _temp3426 >=
_temp3424.last_plus_one)){ _throw( Null_Exception);}* _temp3426;}).last_line));
_temp3412;}); _temp3411;}); goto _LL3406; _LL3410: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3427=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3427[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3428; _temp3428.tag= Cyc_TypeSpecifier_tok_tag;
_temp3428.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp3429=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp3429[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp3430; _temp3430.tag= Cyc_Absyn_UnionType_tag;
_temp3430.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3431= yyvs; struct _xtunion_struct** _temp3433= _temp3431.curr +(
yyvsp_offset - 1); if( _temp3431.base == 0? 1:( _temp3433 < _temp3431.base? 1:
_temp3433 >= _temp3431.last_plus_one)){ _throw( Null_Exception);}* _temp3433;}));
_temp3430.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3434= yyvs;
struct _xtunion_struct** _temp3436= _temp3434.curr + yyvsp_offset; if( _temp3434.base
== 0? 1:( _temp3436 < _temp3434.base? 1: _temp3436 >= _temp3434.last_plus_one)){
_throw( Null_Exception);}* _temp3436;})); _temp3430.f3= 0; _temp3430;});
_temp3429;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3437= yyls;
struct Cyc_Yyltype* _temp3439= _temp3437.curr +( yylsp_offset - 2); if(
_temp3437.base == 0? 1:( _temp3439 < _temp3437.base? 1: _temp3439 >= _temp3437.last_plus_one)){
_throw( Null_Exception);}* _temp3439;}).first_line,({ struct _tagged_ptr3
_temp3440= yyls; struct Cyc_Yyltype* _temp3442= _temp3440.curr + yylsp_offset;
if( _temp3440.base == 0? 1:( _temp3442 < _temp3440.base? 1: _temp3442 >=
_temp3440.last_plus_one)){ _throw( Null_Exception);}* _temp3442;}).last_line));
_temp3428;}); _temp3427;}); goto _LL3406; _LL3406:;} break; case 83: _LL3401:{
void* _temp3447= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3444=
yyvs; struct _xtunion_struct** _temp3446= _temp3444.curr +( yyvsp_offset - 2);
if( _temp3444.base == 0? 1:( _temp3446 < _temp3444.base? 1: _temp3446 >=
_temp3444.last_plus_one)){ _throw( Null_Exception);}* _temp3446;})); _LL3449:
if(( int) _temp3447 == Cyc_Parse_Struct_su){ goto _LL3450;} else{ goto _LL3451;}
_LL3451: if(( int) _temp3447 == Cyc_Parse_Union_su){ goto _LL3452;} else{ goto
_LL3448;} _LL3450: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3453=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3453[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3454; _temp3454.tag=
Cyc_TypeSpecifier_tok_tag; _temp3454.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp3455=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3455[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp3456; _temp3456.tag= Cyc_Absyn_StructType_tag;
_temp3456.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3457= yyvs; struct _xtunion_struct** _temp3459= _temp3457.curr +(
yyvsp_offset - 1); if( _temp3457.base == 0? 1:( _temp3459 < _temp3457.base? 1:
_temp3459 >= _temp3457.last_plus_one)){ _throw( Null_Exception);}* _temp3459;}));
_temp3456.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3460= yyvs;
struct _xtunion_struct** _temp3462= _temp3460.curr + yyvsp_offset; if( _temp3460.base
== 0? 1:( _temp3462 < _temp3460.base? 1: _temp3462 >= _temp3460.last_plus_one)){
_throw( Null_Exception);}* _temp3462;})); _temp3456.f3= 0; _temp3456;});
_temp3455;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3463= yyls;
struct Cyc_Yyltype* _temp3465= _temp3463.curr +( yylsp_offset - 2); if(
_temp3463.base == 0? 1:( _temp3465 < _temp3463.base? 1: _temp3465 >= _temp3463.last_plus_one)){
_throw( Null_Exception);}* _temp3465;}).first_line,({ struct _tagged_ptr3
_temp3466= yyls; struct Cyc_Yyltype* _temp3468= _temp3466.curr + yylsp_offset;
if( _temp3466.base == 0? 1:( _temp3468 < _temp3466.base? 1: _temp3468 >=
_temp3466.last_plus_one)){ _throw( Null_Exception);}* _temp3468;}).last_line));
_temp3454;}); _temp3453;}); goto _LL3448; _LL3452: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3469=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3469[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3470; _temp3470.tag= Cyc_TypeSpecifier_tok_tag;
_temp3470.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp3471=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp3471[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp3472; _temp3472.tag= Cyc_Absyn_UnionType_tag;
_temp3472.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3473= yyvs; struct _xtunion_struct** _temp3475= _temp3473.curr +(
yyvsp_offset - 1); if( _temp3473.base == 0? 1:( _temp3475 < _temp3473.base? 1:
_temp3475 >= _temp3473.last_plus_one)){ _throw( Null_Exception);}* _temp3475;}));
_temp3472.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3476= yyvs;
struct _xtunion_struct** _temp3478= _temp3476.curr + yyvsp_offset; if( _temp3476.base
== 0? 1:( _temp3478 < _temp3476.base? 1: _temp3478 >= _temp3476.last_plus_one)){
_throw( Null_Exception);}* _temp3478;})); _temp3472.f3= 0; _temp3472;});
_temp3471;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3479= yyls;
struct Cyc_Yyltype* _temp3481= _temp3479.curr +( yylsp_offset - 2); if(
_temp3479.base == 0? 1:( _temp3481 < _temp3479.base? 1: _temp3481 >= _temp3479.last_plus_one)){
_throw( Null_Exception);}* _temp3481;}).first_line,({ struct _tagged_ptr3
_temp3482= yyls; struct Cyc_Yyltype* _temp3484= _temp3482.curr + yylsp_offset;
if( _temp3482.base == 0? 1:( _temp3484 < _temp3482.base? 1: _temp3484 >=
_temp3482.last_plus_one)){ _throw( Null_Exception);}* _temp3484;}).last_line));
_temp3470;}); _temp3469;}); goto _LL3448; _LL3448:;} break; case 84: _LL3443:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp3486=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3486[ 0]=({ struct Cyc_TypeList_tok_struct _temp3487; _temp3487.tag= Cyc_TypeList_tok_tag;
_temp3487.f1= 0; _temp3487;}); _temp3486;}); break; case 85: _LL3485: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp3489=( struct
Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3489[ 0]=({ struct Cyc_TypeList_tok_struct _temp3490; _temp3490.tag= Cyc_TypeList_tok_tag;
_temp3490.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3491= yyvs; struct
_xtunion_struct** _temp3493= _temp3491.curr +( yyvsp_offset - 1); if( _temp3491.base
== 0? 1:( _temp3493 < _temp3491.base? 1: _temp3493 >= _temp3491.last_plus_one)){
_throw( Null_Exception);}* _temp3493;}))); _temp3490;}); _temp3489;}); break;
case 86: _LL3488:(( struct Cyc_Lexing_lexbuf*)({ struct Cyc_Core_Opt* _temp3495=
Cyc_Parse_lbuf; if( _temp3495 == 0){ _throw( Null_Exception);} _temp3495->v;}))->lex_curr_pos
-= 1; yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct*
_temp3496=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3496[ 0]=({ struct Cyc_TypeList_tok_struct _temp3497; _temp3497.tag= Cyc_TypeList_tok_tag;
_temp3497.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3498= yyvs; struct
_xtunion_struct** _temp3500= _temp3498.curr +( yyvsp_offset - 1); if( _temp3498.base
== 0? 1:( _temp3500 < _temp3498.base? 1: _temp3500 >= _temp3498.last_plus_one)){
_throw( Null_Exception);}* _temp3500;}))); _temp3497;}); _temp3496;}); break;
case 87: _LL3494: yyval=( struct _xtunion_struct*)({ struct Cyc_StructOrUnion_tok_struct*
_temp3502=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp3502[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp3503; _temp3503.tag=
Cyc_StructOrUnion_tok_tag; _temp3503.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp3503;}); _temp3502;}); break; case 88: _LL3501: yyval=( struct
_xtunion_struct*)({ struct Cyc_StructOrUnion_tok_struct* _temp3505=( struct Cyc_StructOrUnion_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct)); _temp3505[ 0]=({
struct Cyc_StructOrUnion_tok_struct _temp3506; _temp3506.tag= Cyc_StructOrUnion_tok_tag;
_temp3506.f1=( void*)(( void*) Cyc_Parse_Union_su); _temp3506;}); _temp3505;});
break; case 89: _LL3504: yyval=( struct _xtunion_struct*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3508=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp3508[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp3509; _temp3509.tag= Cyc_StructFieldDeclList_tok_tag; _temp3509.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3510= yyvs; struct _xtunion_struct** _temp3512=
_temp3510.curr + yyvsp_offset; if( _temp3510.base == 0? 1:( _temp3512 <
_temp3510.base? 1: _temp3512 >= _temp3510.last_plus_one)){ _throw(
Null_Exception);}* _temp3512;})))); _temp3509;}); _temp3508;}); break; case 90:
_LL3507: yyval=( struct _xtunion_struct*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3514=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp3514[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp3515; _temp3515.tag= Cyc_StructFieldDeclListList_tok_tag; _temp3515.f1=({
struct Cyc_List_List* _temp3516=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3516->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3517= yyvs; struct _xtunion_struct** _temp3519=
_temp3517.curr + yyvsp_offset; if( _temp3517.base == 0? 1:( _temp3519 <
_temp3517.base? 1: _temp3519 >= _temp3517.last_plus_one)){ _throw(
Null_Exception);}* _temp3519;})); _temp3516->tl= 0; _temp3516;}); _temp3515;});
_temp3514;}); break; case 91: _LL3513: yyval=( struct _xtunion_struct*)({ struct
Cyc_StructFieldDeclListList_tok_struct* _temp3521=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp3521[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp3522; _temp3522.tag= Cyc_StructFieldDeclListList_tok_tag;
_temp3522.f1=({ struct Cyc_List_List* _temp3523=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3523->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3527= yyvs; struct _xtunion_struct** _temp3529=
_temp3527.curr + yyvsp_offset; if( _temp3527.base == 0? 1:( _temp3529 <
_temp3527.base? 1: _temp3529 >= _temp3527.last_plus_one)){ _throw(
Null_Exception);}* _temp3529;})); _temp3523->tl= Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3524= yyvs; struct _xtunion_struct** _temp3526=
_temp3524.curr +( yyvsp_offset - 1); if( _temp3524.base == 0? 1:( _temp3526 <
_temp3524.base? 1: _temp3526 >= _temp3524.last_plus_one)){ _throw(
Null_Exception);}* _temp3526;})); _temp3523;}); _temp3522;}); _temp3521;});
break; case 92: _LL3520: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDeclList_tok_struct*
_temp3531=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3531[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp3532; _temp3532.tag=
Cyc_InitDeclList_tok_tag; _temp3532.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(({ struct _tagged_ptr2
_temp3533= yyvs; struct _xtunion_struct** _temp3535= _temp3533.curr +
yyvsp_offset; if( _temp3533.base == 0? 1:( _temp3535 < _temp3533.base? 1:
_temp3535 >= _temp3533.last_plus_one)){ _throw( Null_Exception);}* _temp3535;})));
_temp3532;}); _temp3531;}); break; case 93: _LL3530: yyval=( struct
_xtunion_struct*)({ struct Cyc_InitDeclList_tok_struct* _temp3537=( struct Cyc_InitDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp3537[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp3538; _temp3538.tag= Cyc_InitDeclList_tok_tag;
_temp3538.f1=({ struct Cyc_List_List* _temp3539=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3539->hd=( void*) Cyc_yyget_InitDecl_tok(({
struct _tagged_ptr2 _temp3540= yyvs; struct _xtunion_struct** _temp3542=
_temp3540.curr + yyvsp_offset; if( _temp3540.base == 0? 1:( _temp3542 <
_temp3540.base? 1: _temp3542 >= _temp3540.last_plus_one)){ _throw(
Null_Exception);}* _temp3542;})); _temp3539->tl= 0; _temp3539;}); _temp3538;});
_temp3537;}); break; case 94: _LL3536: yyval=( struct _xtunion_struct*)({ struct
Cyc_InitDeclList_tok_struct* _temp3544=( struct Cyc_InitDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp3544[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp3545; _temp3545.tag= Cyc_InitDeclList_tok_tag;
_temp3545.f1=({ struct Cyc_List_List* _temp3546=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3546->hd=( void*) Cyc_yyget_InitDecl_tok(({
struct _tagged_ptr2 _temp3550= yyvs; struct _xtunion_struct** _temp3552=
_temp3550.curr + yyvsp_offset; if( _temp3550.base == 0? 1:( _temp3552 <
_temp3550.base? 1: _temp3552 >= _temp3550.last_plus_one)){ _throw(
Null_Exception);}* _temp3552;})); _temp3546->tl= Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp3547= yyvs; struct _xtunion_struct** _temp3549=
_temp3547.curr +( yyvsp_offset - 2); if( _temp3547.base == 0? 1:( _temp3549 <
_temp3547.base? 1: _temp3549 >= _temp3547.last_plus_one)){ _throw(
Null_Exception);}* _temp3549;})); _temp3546;}); _temp3545;}); _temp3544;});
break; case 95: _LL3543: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDecl_tok_struct*
_temp3554=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp3554[ 0]=({ struct Cyc_InitDecl_tok_struct _temp3555; _temp3555.tag= Cyc_InitDecl_tok_tag;
_temp3555.f1=({ struct _tuple13* _temp3556=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp3556->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3557= yyvs; struct _xtunion_struct** _temp3559= _temp3557.curr
+ yyvsp_offset; if( _temp3557.base == 0? 1:( _temp3559 < _temp3557.base? 1:
_temp3559 >= _temp3557.last_plus_one)){ _throw( Null_Exception);}* _temp3559;}));
_temp3556->f2= 0; _temp3556;}); _temp3555;}); _temp3554;}); break; case 96:
_LL3553: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDecl_tok_struct*
_temp3561=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp3561[ 0]=({ struct Cyc_InitDecl_tok_struct _temp3562; _temp3562.tag= Cyc_InitDecl_tok_tag;
_temp3562.f1=({ struct _tuple13* _temp3563=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp3563->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3567= yyvs; struct _xtunion_struct** _temp3569= _temp3567.curr
+( yyvsp_offset - 2); if( _temp3567.base == 0? 1:( _temp3569 < _temp3567.base? 1:
_temp3569 >= _temp3567.last_plus_one)){ _throw( Null_Exception);}* _temp3569;}));
_temp3563->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp3564= yyvs; struct _xtunion_struct** _temp3566= _temp3564.curr +
yyvsp_offset; if( _temp3564.base == 0? 1:( _temp3566 < _temp3564.base? 1:
_temp3566 >= _temp3564.last_plus_one)){ _throw( Null_Exception);}* _temp3566;}));
_temp3563;}); _temp3562;}); _temp3561;}); break; case 97: _LL3560: { struct Cyc_Absyn_Tqual*
tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3571= yyvs; struct
_xtunion_struct** _temp3573= _temp3571.curr +( yyvsp_offset - 2); if( _temp3571.base
== 0? 1:( _temp3573 < _temp3571.base? 1: _temp3573 >= _temp3571.last_plus_one)){
_throw( Null_Exception);}* _temp3573;}))).f1; struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3574= yyvs; struct _xtunion_struct** _temp3576=
_temp3574.curr +( yyvsp_offset - 2); if( _temp3574.base == 0? 1:( _temp3576 <
_temp3574.base? 1: _temp3576 >= _temp3574.last_plus_one)){ _throw(
Null_Exception);}* _temp3576;}))).f3; void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3577= yyvs; struct _xtunion_struct** _temp3579=
_temp3577.curr +( yyvsp_offset - 2); if( _temp3577.base == 0? 1:( _temp3579 <
_temp3577.base? 1: _temp3579 >= _temp3577.last_plus_one)){ _throw(
Null_Exception);}* _temp3579;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3580= yyls; struct Cyc_Yyltype* _temp3582= _temp3580.curr +(
yylsp_offset - 2); if( _temp3580.base == 0? 1:( _temp3582 < _temp3580.base? 1:
_temp3582 >= _temp3580.last_plus_one)){ _throw( Null_Exception);}* _temp3582;}).first_line,({
struct _tagged_ptr3 _temp3583= yyls; struct Cyc_Yyltype* _temp3585= _temp3583.curr
+( yylsp_offset - 2); if( _temp3583.base == 0? 1:( _temp3585 < _temp3583.base? 1:
_temp3585 >= _temp3583.last_plus_one)){ _throw( Null_Exception);}* _temp3585;}).last_line));
struct Cyc_List_List* _temp3591; struct Cyc_List_List* _temp3593; struct _tuple0
_temp3589=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3586= yyvs; struct _xtunion_struct** _temp3588=
_temp3586.curr +( yyvsp_offset - 1); if( _temp3586.base == 0? 1:( _temp3588 <
_temp3586.base? 1: _temp3588 >= _temp3586.last_plus_one)){ _throw(
Null_Exception);}* _temp3588;}))); _LL3594: _temp3593= _temp3589.f1; goto
_LL3592; _LL3592: _temp3591= _temp3589.f2; goto _LL3590; _LL3590: { struct Cyc_List_List*
info=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( Cyc_Parse_apply_tmss( tq, t, _temp3593, atts), _temp3591);
yyval=( struct _xtunion_struct*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3595=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp3595[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp3596; _temp3596.tag= Cyc_StructFieldDeclList_tok_tag; _temp3596.f1=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple8*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3597= yyls; struct Cyc_Yyltype* _temp3599= _temp3597.curr
+( yylsp_offset - 2); if( _temp3597.base == 0? 1:( _temp3599 < _temp3597.base? 1:
_temp3599 >= _temp3597.last_plus_one)){ _throw( Null_Exception);}* _temp3599;}).first_line,({
struct _tagged_ptr3 _temp3600= yyls; struct Cyc_Yyltype* _temp3602= _temp3600.curr
+( yylsp_offset - 1); if( _temp3600.base == 0? 1:( _temp3602 < _temp3600.base? 1:
_temp3602 >= _temp3600.last_plus_one)){ _throw( Null_Exception);}* _temp3602;}).last_line),
info); _temp3596;}); _temp3595;}); break;}} case 98: _LL3570: yyval=( struct
_xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct* _temp3604=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp3604[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp3605; _temp3605.tag= Cyc_QualSpecList_tok_tag;
_temp3605.f1=({ struct _tuple14* _temp3606=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3606->f1= Cyc_Absyn_empty_tqual(); _temp3606->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp3610=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3610->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3611= yyvs; struct _xtunion_struct** _temp3613=
_temp3611.curr +( yyvsp_offset - 1); if( _temp3611.base == 0? 1:( _temp3613 <
_temp3611.base? 1: _temp3613 >= _temp3611.last_plus_one)){ _throw(
Null_Exception);}* _temp3613;})); _temp3610->tl= 0; _temp3610;}); _temp3606->f3=
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp3607= yyvs; struct
_xtunion_struct** _temp3609= _temp3607.curr + yyvsp_offset; if( _temp3607.base
== 0? 1:( _temp3609 < _temp3607.base? 1: _temp3609 >= _temp3607.last_plus_one)){
_throw( Null_Exception);}* _temp3609;})); _temp3606;}); _temp3605;}); _temp3604;});
break; case 99: _LL3603: yyval=( struct _xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct*
_temp3615=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3615[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3616; _temp3616.tag=
Cyc_QualSpecList_tok_tag; _temp3616.f1=({ struct _tuple14* _temp3617=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3617->f1=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3631= yyvs; struct _xtunion_struct** _temp3633=
_temp3631.curr + yyvsp_offset; if( _temp3631.base == 0? 1:( _temp3633 <
_temp3631.base? 1: _temp3633 >= _temp3631.last_plus_one)){ _throw(
Null_Exception);}* _temp3633;}))).f1; _temp3617->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp3624=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3624->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3628= yyvs; struct _xtunion_struct** _temp3630=
_temp3628.curr +( yyvsp_offset - 2); if( _temp3628.base == 0? 1:( _temp3630 <
_temp3628.base? 1: _temp3630 >= _temp3628.last_plus_one)){ _throw(
Null_Exception);}* _temp3630;})); _temp3624->tl=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3625= yyvs; struct _xtunion_struct** _temp3627=
_temp3625.curr + yyvsp_offset; if( _temp3625.base == 0? 1:( _temp3627 <
_temp3625.base? 1: _temp3627 >= _temp3625.last_plus_one)){ _throw(
Null_Exception);}* _temp3627;}))).f2; _temp3624;}); _temp3617->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3618= yyvs; struct _xtunion_struct** _temp3620=
_temp3618.curr +( yyvsp_offset - 1); if( _temp3618.base == 0? 1:( _temp3620 <
_temp3618.base? 1: _temp3620 >= _temp3618.last_plus_one)){ _throw(
Null_Exception);}* _temp3620;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3621= yyvs; struct _xtunion_struct** _temp3623= _temp3621.curr
+ yyvsp_offset; if( _temp3621.base == 0? 1:( _temp3623 < _temp3621.base? 1:
_temp3623 >= _temp3621.last_plus_one)){ _throw( Null_Exception);}* _temp3623;}))).f3);
_temp3617;}); _temp3616;}); _temp3615;}); break; case 100: _LL3614: yyval=(
struct _xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct* _temp3635=(
struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3635[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3636; _temp3636.tag=
Cyc_QualSpecList_tok_tag; _temp3636.f1=({ struct _tuple14* _temp3637=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3637->f1= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3641= yyvs; struct _xtunion_struct** _temp3643=
_temp3641.curr +( yyvsp_offset - 1); if( _temp3641.base == 0? 1:( _temp3643 <
_temp3641.base? 1: _temp3643 >= _temp3641.last_plus_one)){ _throw(
Null_Exception);}* _temp3643;})); _temp3637->f2= 0; _temp3637->f3= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3638= yyvs; struct _xtunion_struct** _temp3640=
_temp3638.curr + yyvsp_offset; if( _temp3638.base == 0? 1:( _temp3640 <
_temp3638.base? 1: _temp3640 >= _temp3638.last_plus_one)){ _throw(
Null_Exception);}* _temp3640;})); _temp3637;}); _temp3636;}); _temp3635;});
break; case 101: _LL3634: yyval=( struct _xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct*
_temp3645=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3645[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3646; _temp3646.tag=
Cyc_QualSpecList_tok_tag; _temp3646.f1=({ struct _tuple14* _temp3647=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3647->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp3657= yyvs; struct
_xtunion_struct** _temp3659= _temp3657.curr +( yyvsp_offset - 2); if( _temp3657.base
== 0? 1:( _temp3659 < _temp3657.base? 1: _temp3659 >= _temp3657.last_plus_one)){
_throw( Null_Exception);}* _temp3659;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3660= yyvs; struct _xtunion_struct** _temp3662= _temp3660.curr
+ yyvsp_offset; if( _temp3660.base == 0? 1:( _temp3662 < _temp3660.base? 1:
_temp3662 >= _temp3660.last_plus_one)){ _throw( Null_Exception);}* _temp3662;}))).f1);
_temp3647->f2=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3654=
yyvs; struct _xtunion_struct** _temp3656= _temp3654.curr + yyvsp_offset; if(
_temp3654.base == 0? 1:( _temp3656 < _temp3654.base? 1: _temp3656 >= _temp3654.last_plus_one)){
_throw( Null_Exception);}* _temp3656;}))).f2; _temp3647->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3648= yyvs; struct _xtunion_struct** _temp3650=
_temp3648.curr +( yyvsp_offset - 1); if( _temp3648.base == 0? 1:( _temp3650 <
_temp3648.base? 1: _temp3650 >= _temp3648.last_plus_one)){ _throw(
Null_Exception);}* _temp3650;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3651= yyvs; struct _xtunion_struct** _temp3653= _temp3651.curr
+ yyvsp_offset; if( _temp3651.base == 0? 1:( _temp3653 < _temp3651.base? 1:
_temp3653 >= _temp3651.last_plus_one)){ _throw( Null_Exception);}* _temp3653;}))).f3);
_temp3647;}); _temp3646;}); _temp3645;}); break; case 102: _LL3644: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp3664=(
struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct));
_temp3664[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct _temp3665; _temp3665.tag=
Cyc_DeclaratorExpoptList_tok_tag; _temp3665.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3666= yyvs; struct _xtunion_struct** _temp3668=
_temp3666.curr + yyvsp_offset; if( _temp3666.base == 0? 1:( _temp3668 <
_temp3666.base? 1: _temp3668 >= _temp3666.last_plus_one)){ _throw(
Null_Exception);}* _temp3668;}))); _temp3665;}); _temp3664;}); break; case 103:
_LL3663: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclaratorExpoptList_tok_struct*
_temp3670=( struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof(
struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3670[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct
_temp3671; _temp3671.tag= Cyc_DeclaratorExpoptList_tok_tag; _temp3671.f1=({
struct Cyc_List_List* _temp3672=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3672->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(({
struct _tagged_ptr2 _temp3673= yyvs; struct _xtunion_struct** _temp3675=
_temp3673.curr + yyvsp_offset; if( _temp3673.base == 0? 1:( _temp3675 <
_temp3673.base? 1: _temp3675 >= _temp3673.last_plus_one)){ _throw(
Null_Exception);}* _temp3675;})); _temp3672->tl= 0; _temp3672;}); _temp3671;});
_temp3670;}); break; case 104: _LL3669: yyval=( struct _xtunion_struct*)({
struct Cyc_DeclaratorExpoptList_tok_struct* _temp3677=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3677[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp3678; _temp3678.tag= Cyc_DeclaratorExpoptList_tok_tag;
_temp3678.f1=({ struct Cyc_List_List* _temp3679=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3679->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(({
struct _tagged_ptr2 _temp3683= yyvs; struct _xtunion_struct** _temp3685=
_temp3683.curr + yyvsp_offset; if( _temp3683.base == 0? 1:( _temp3685 <
_temp3683.base? 1: _temp3685 >= _temp3683.last_plus_one)){ _throw(
Null_Exception);}* _temp3685;})); _temp3679->tl= Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3680= yyvs; struct _xtunion_struct** _temp3682=
_temp3680.curr +( yyvsp_offset - 2); if( _temp3680.base == 0? 1:( _temp3682 <
_temp3680.base? 1: _temp3682 >= _temp3680.last_plus_one)){ _throw(
Null_Exception);}* _temp3682;})); _temp3679;}); _temp3678;}); _temp3677;});
break; case 105: _LL3676: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclaratorExpopt_tok_struct*
_temp3687=( struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct
Cyc_DeclaratorExpopt_tok_struct)); _temp3687[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct
_temp3688; _temp3688.tag= Cyc_DeclaratorExpopt_tok_tag; _temp3688.f1=({ struct
_tuple15* _temp3689=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15));
_temp3689->f1= Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3690= yyvs;
struct _xtunion_struct** _temp3692= _temp3690.curr + yyvsp_offset; if( _temp3690.base
== 0? 1:( _temp3692 < _temp3690.base? 1: _temp3692 >= _temp3690.last_plus_one)){
_throw( Null_Exception);}* _temp3692;})); _temp3689->f2= 0; _temp3689;});
_temp3688;}); _temp3687;}); break; case 106: _LL3686: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp3694=( struct
Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp3694[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp3695; _temp3695.tag=
Cyc_DeclaratorExpopt_tok_tag; _temp3695.f1=({ struct _tuple15* _temp3696=(
struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp3696->f1=({ struct
Cyc_Parse_Declarator* _temp3701=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3701->id=({ struct _tuple1*
_temp3702=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3702->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp3706=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp3706[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp3707; _temp3707.tag= Cyc_Absyn_Rel_n_tag; _temp3707.f1= 0; _temp3707;});
_temp3706;}); _temp3702->f2=({ struct _tagged_string* _temp3703=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3703[ 0]=(
struct _tagged_string)({ char* _temp3704=( char*)""; struct _tagged_string
_temp3705; _temp3705.curr= _temp3704; _temp3705.base= _temp3704; _temp3705.last_plus_one=
_temp3704 + 1; _temp3705;}); _temp3703;}); _temp3702;}); _temp3701->tms= 0;
_temp3701;}); _temp3696->f2=({ struct Cyc_Core_Opt* _temp3697=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3697->v=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3698= yyvs; struct _xtunion_struct** _temp3700=
_temp3698.curr + yyvsp_offset; if( _temp3698.base == 0? 1:( _temp3700 <
_temp3698.base? 1: _temp3700 >= _temp3698.last_plus_one)){ _throw(
Null_Exception);}* _temp3700;})); _temp3697;}); _temp3696;}); _temp3695;});
_temp3694;}); break; case 107: _LL3693: yyval=( struct _xtunion_struct*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp3709=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp3709[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp3710; _temp3710.tag= Cyc_DeclaratorExpopt_tok_tag;
_temp3710.f1=({ struct _tuple15* _temp3711=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp3711->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3716= yyvs; struct _xtunion_struct** _temp3718= _temp3716.curr
+( yyvsp_offset - 2); if( _temp3716.base == 0? 1:( _temp3718 < _temp3716.base? 1:
_temp3718 >= _temp3716.last_plus_one)){ _throw( Null_Exception);}* _temp3718;}));
_temp3711->f2=({ struct Cyc_Core_Opt* _temp3712=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3712->v=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3713= yyvs; struct _xtunion_struct** _temp3715=
_temp3713.curr + yyvsp_offset; if( _temp3713.base == 0? 1:( _temp3715 <
_temp3713.base? 1: _temp3715 >= _temp3713.last_plus_one)){ _throw(
Null_Exception);}* _temp3715;})); _temp3712;}); _temp3711;}); _temp3710;});
_temp3709;}); break; case 108: _LL3708: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3720= yyls; struct Cyc_Yyltype* _temp3722= _temp3720.curr
+( yylsp_offset - 3); if( _temp3720.base == 0? 1:( _temp3722 < _temp3720.base? 1:
_temp3722 >= _temp3720.last_plus_one)){ _throw( Null_Exception);}* _temp3722;}).first_line,({
struct _tagged_ptr3 _temp3723= yyls; struct Cyc_Yyltype* _temp3725= _temp3723.curr
+( yylsp_offset - 3); if( _temp3723.base == 0? 1:( _temp3725 < _temp3723.base? 1:
_temp3725 >= _temp3723.last_plus_one)){ _throw( Null_Exception);}* _temp3725;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3726= yyvs; struct
_xtunion_struct** _temp3728= _temp3726.curr +( yyvsp_offset - 3); if( _temp3726.base
== 0? 1:( _temp3728 < _temp3726.base? 1: _temp3728 >= _temp3726.last_plus_one)){
_throw( Null_Exception);}* _temp3728;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_TypeSpecifier_tok_struct* _temp3729=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3729[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3730; _temp3730.tag= Cyc_TypeSpecifier_tok_tag;
_temp3730.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp3731=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3731[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3732; _temp3732.tag= Cyc_Parse_Decl_spec_tag;
_temp3732.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp3733=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3733->v=( void*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3734=
yyvs; struct _xtunion_struct** _temp3736= _temp3734.curr +( yyvsp_offset - 4);
if( _temp3734.base == 0? 1:( _temp3736 < _temp3734.base? 1: _temp3736 >=
_temp3734.last_plus_one)){ _throw( Null_Exception);}* _temp3736;})); _temp3733;}),
ts,({ struct Cyc_Core_Opt* _temp3737=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp3737->v=( void*) Cyc_yyget_TunionFieldList_tok(({
struct _tagged_ptr2 _temp3738= yyvs; struct _xtunion_struct** _temp3740=
_temp3738.curr +( yyvsp_offset - 1); if( _temp3738.base == 0? 1:( _temp3740 <
_temp3738.base? 1: _temp3740 >= _temp3738.last_plus_one)){ _throw(
Null_Exception);}* _temp3740;})); _temp3737;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3741= yyls; struct Cyc_Yyltype* _temp3743= _temp3741.curr
+( yylsp_offset - 5); if( _temp3741.base == 0? 1:( _temp3743 < _temp3741.base? 1:
_temp3743 >= _temp3741.last_plus_one)){ _throw( Null_Exception);}* _temp3743;}).first_line,({
struct _tagged_ptr3 _temp3744= yyls; struct Cyc_Yyltype* _temp3746= _temp3744.curr
+ yylsp_offset; if( _temp3744.base == 0? 1:( _temp3746 < _temp3744.base? 1:
_temp3746 >= _temp3744.last_plus_one)){ _throw( Null_Exception);}* _temp3746;}).last_line));
_temp3732;}); _temp3731;})); _temp3730;}); _temp3729;}); break;} case 109:
_LL3719: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3748=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3748[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3749; _temp3749.tag=
Cyc_TypeSpecifier_tok_tag; _temp3749.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp3750=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp3750[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp3751; _temp3751.tag= Cyc_Absyn_TunionType_tag;
_temp3751.f1=({ struct Cyc_Absyn_TunionInfo _temp3752; _temp3752.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3756= yyvs; struct _xtunion_struct** _temp3758=
_temp3756.curr +( yyvsp_offset - 1); if( _temp3756.base == 0? 1:( _temp3758 <
_temp3756.base? 1: _temp3758 >= _temp3756.last_plus_one)){ _throw(
Null_Exception);}* _temp3758;})); _temp3752.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3753= yyvs; struct _xtunion_struct** _temp3755=
_temp3753.curr + yyvsp_offset; if( _temp3753.base == 0? 1:( _temp3755 <
_temp3753.base? 1: _temp3755 >= _temp3753.last_plus_one)){ _throw(
Null_Exception);}* _temp3755;})); _temp3752.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp3752.tud= 0; _temp3752;}); _temp3751;}); _temp3750;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3759= yyls; struct Cyc_Yyltype* _temp3761= _temp3759.curr
+( yylsp_offset - 2); if( _temp3759.base == 0? 1:( _temp3761 < _temp3759.base? 1:
_temp3761 >= _temp3759.last_plus_one)){ _throw( Null_Exception);}* _temp3761;}).first_line,({
struct _tagged_ptr3 _temp3762= yyls; struct Cyc_Yyltype* _temp3764= _temp3762.curr
+ yylsp_offset; if( _temp3762.base == 0? 1:( _temp3764 < _temp3762.base? 1:
_temp3764 >= _temp3762.last_plus_one)){ _throw( Null_Exception);}* _temp3764;}).last_line));
_temp3749;}); _temp3748;}); break; case 110: _LL3747: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3766=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3766[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3767; _temp3767.tag= Cyc_TypeSpecifier_tok_tag;
_temp3767.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp3768=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp3768[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp3769; _temp3769.tag=
Cyc_Absyn_TunionType_tag; _temp3769.f1=({ struct Cyc_Absyn_TunionInfo _temp3770;
_temp3770.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3777= yyvs;
struct _xtunion_struct** _temp3779= _temp3777.curr +( yyvsp_offset - 1); if(
_temp3777.base == 0? 1:( _temp3779 < _temp3777.base? 1: _temp3779 >= _temp3777.last_plus_one)){
_throw( Null_Exception);}* _temp3779;})); _temp3770.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3774= yyvs; struct _xtunion_struct** _temp3776=
_temp3774.curr + yyvsp_offset; if( _temp3774.base == 0? 1:( _temp3776 <
_temp3774.base? 1: _temp3776 >= _temp3774.last_plus_one)){ _throw(
Null_Exception);}* _temp3776;})); _temp3770.rgn=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3771= yyvs; struct _xtunion_struct** _temp3773=
_temp3771.curr +( yyvsp_offset - 2); if( _temp3771.base == 0? 1:( _temp3773 <
_temp3771.base? 1: _temp3773 >= _temp3771.last_plus_one)){ _throw(
Null_Exception);}* _temp3773;})); _temp3770.tud= 0; _temp3770;}); _temp3769;});
_temp3768;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3780= yyls;
struct Cyc_Yyltype* _temp3782= _temp3780.curr +( yylsp_offset - 3); if(
_temp3780.base == 0? 1:( _temp3782 < _temp3780.base? 1: _temp3782 >= _temp3780.last_plus_one)){
_throw( Null_Exception);}* _temp3782;}).first_line,({ struct _tagged_ptr3
_temp3783= yyls; struct Cyc_Yyltype* _temp3785= _temp3783.curr + yylsp_offset;
if( _temp3783.base == 0? 1:( _temp3785 < _temp3783.base? 1: _temp3785 >=
_temp3783.last_plus_one)){ _throw( Null_Exception);}* _temp3785;}).last_line));
_temp3767;}); _temp3766;}); break; case 111: _LL3765: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3787=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3787[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3788; _temp3788.tag= Cyc_TypeSpecifier_tok_tag;
_temp3788.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp3789=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp3789[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp3790; _temp3790.tag= Cyc_Absyn_TunionFieldType_tag; _temp3790.f1=({ struct
Cyc_Absyn_TunionFieldInfo _temp3791; _temp3791.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3798= yyvs; struct _xtunion_struct** _temp3800=
_temp3798.curr +( yyvsp_offset - 3); if( _temp3798.base == 0? 1:( _temp3800 <
_temp3798.base? 1: _temp3800 >= _temp3798.last_plus_one)){ _throw(
Null_Exception);}* _temp3800;})); _temp3791.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3795= yyvs; struct _xtunion_struct** _temp3797=
_temp3795.curr + yyvsp_offset; if( _temp3795.base == 0? 1:( _temp3797 <
_temp3795.base? 1: _temp3797 >= _temp3795.last_plus_one)){ _throw(
Null_Exception);}* _temp3797;})); _temp3791.fname= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3792= yyvs; struct _xtunion_struct** _temp3794=
_temp3792.curr +( yyvsp_offset - 1); if( _temp3792.base == 0? 1:( _temp3794 <
_temp3792.base? 1: _temp3794 >= _temp3792.last_plus_one)){ _throw(
Null_Exception);}* _temp3794;})); _temp3791.tud= 0; _temp3791.tufd= 0; _temp3791;});
_temp3790;}); _temp3789;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3801= yyls; struct Cyc_Yyltype* _temp3803= _temp3801.curr +( yylsp_offset -
4); if( _temp3801.base == 0? 1:( _temp3803 < _temp3801.base? 1: _temp3803 >=
_temp3801.last_plus_one)){ _throw( Null_Exception);}* _temp3803;}).first_line,({
struct _tagged_ptr3 _temp3804= yyls; struct Cyc_Yyltype* _temp3806= _temp3804.curr
+ yylsp_offset; if( _temp3804.base == 0? 1:( _temp3806 < _temp3804.base? 1:
_temp3806 >= _temp3804.last_plus_one)){ _throw( Null_Exception);}* _temp3806;}).last_line));
_temp3788;}); _temp3787;}); break; case 112: _LL3786: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3808=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3808[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3809; _temp3809.tag= Cyc_TypeSpecifier_tok_tag;
_temp3809.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp3810=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3810[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3811; _temp3811.tag= Cyc_Parse_Decl_spec_tag;
_temp3811.f1= Cyc_Absyn_xtunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3812= yyvs; struct _xtunion_struct** _temp3814=
_temp3812.curr +( yyvsp_offset - 3); if( _temp3812.base == 0? 1:( _temp3814 <
_temp3812.base? 1: _temp3814 >= _temp3812.last_plus_one)){ _throw(
Null_Exception);}* _temp3814;})), Cyc_yyget_TunionFieldList_tok(({ struct
_tagged_ptr2 _temp3815= yyvs; struct _xtunion_struct** _temp3817= _temp3815.curr
+( yyvsp_offset - 1); if( _temp3815.base == 0? 1:( _temp3817 < _temp3815.base? 1:
_temp3817 >= _temp3815.last_plus_one)){ _throw( Null_Exception);}* _temp3817;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3818= yyls; struct Cyc_Yyltype*
_temp3820= _temp3818.curr +( yylsp_offset - 4); if( _temp3818.base == 0? 1:(
_temp3820 < _temp3818.base? 1: _temp3820 >= _temp3818.last_plus_one)){ _throw(
Null_Exception);}* _temp3820;}).first_line,({ struct _tagged_ptr3 _temp3821=
yyls; struct Cyc_Yyltype* _temp3823= _temp3821.curr + yylsp_offset; if(
_temp3821.base == 0? 1:( _temp3823 < _temp3821.base? 1: _temp3823 >= _temp3821.last_plus_one)){
_throw( Null_Exception);}* _temp3823;}).last_line)); _temp3811;}); _temp3810;}));
_temp3809;}); _temp3808;}); break; case 113: _LL3807: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3825=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3825[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3826; _temp3826.tag= Cyc_TypeSpecifier_tok_tag;
_temp3826.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_XTunionType_struct*
_temp3827=( struct Cyc_Absyn_XTunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct));
_temp3827[ 0]=({ struct Cyc_Absyn_XTunionType_struct _temp3828; _temp3828.tag=
Cyc_Absyn_XTunionType_tag; _temp3828.f1=({ struct Cyc_Absyn_XTunionInfo
_temp3829; _temp3829.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3830=
yyvs; struct _xtunion_struct** _temp3832= _temp3830.curr + yyvsp_offset; if(
_temp3830.base == 0? 1:( _temp3832 < _temp3830.base? 1: _temp3832 >= _temp3830.last_plus_one)){
_throw( Null_Exception);}* _temp3832;})); _temp3829.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp3829.xtud= 0; _temp3829;}); _temp3828;}); _temp3827;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3833= yyls; struct Cyc_Yyltype* _temp3835= _temp3833.curr
+( yylsp_offset - 1); if( _temp3833.base == 0? 1:( _temp3835 < _temp3833.base? 1:
_temp3835 >= _temp3833.last_plus_one)){ _throw( Null_Exception);}* _temp3835;}).first_line,({
struct _tagged_ptr3 _temp3836= yyls; struct Cyc_Yyltype* _temp3838= _temp3836.curr
+ yylsp_offset; if( _temp3836.base == 0? 1:( _temp3838 < _temp3836.base? 1:
_temp3838 >= _temp3836.last_plus_one)){ _throw( Null_Exception);}* _temp3838;}).last_line));
_temp3826;}); _temp3825;}); break; case 114: _LL3824: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3840=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3840[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3841; _temp3841.tag= Cyc_TypeSpecifier_tok_tag;
_temp3841.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_XTunionType_struct*
_temp3842=( struct Cyc_Absyn_XTunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct));
_temp3842[ 0]=({ struct Cyc_Absyn_XTunionType_struct _temp3843; _temp3843.tag=
Cyc_Absyn_XTunionType_tag; _temp3843.f1=({ struct Cyc_Absyn_XTunionInfo
_temp3844; _temp3844.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3848=
yyvs; struct _xtunion_struct** _temp3850= _temp3848.curr + yyvsp_offset; if(
_temp3848.base == 0? 1:( _temp3850 < _temp3848.base? 1: _temp3850 >= _temp3848.last_plus_one)){
_throw( Null_Exception);}* _temp3850;})); _temp3844.rgn=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3845= yyvs; struct _xtunion_struct** _temp3847=
_temp3845.curr +( yyvsp_offset - 1); if( _temp3845.base == 0? 1:( _temp3847 <
_temp3845.base? 1: _temp3847 >= _temp3845.last_plus_one)){ _throw(
Null_Exception);}* _temp3847;})); _temp3844.xtud= 0; _temp3844;}); _temp3843;});
_temp3842;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3851= yyls;
struct Cyc_Yyltype* _temp3853= _temp3851.curr +( yylsp_offset - 2); if(
_temp3851.base == 0? 1:( _temp3853 < _temp3851.base? 1: _temp3853 >= _temp3851.last_plus_one)){
_throw( Null_Exception);}* _temp3853;}).first_line,({ struct _tagged_ptr3
_temp3854= yyls; struct Cyc_Yyltype* _temp3856= _temp3854.curr + yylsp_offset;
if( _temp3854.base == 0? 1:( _temp3856 < _temp3854.base? 1: _temp3856 >=
_temp3854.last_plus_one)){ _throw( Null_Exception);}* _temp3856;}).last_line));
_temp3841;}); _temp3840;}); break; case 115: _LL3839: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3858=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3858[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3859; _temp3859.tag= Cyc_TypeSpecifier_tok_tag;
_temp3859.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_XTunionFieldType_struct*
_temp3860=( struct Cyc_Absyn_XTunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_XTunionFieldType_struct)); _temp3860[ 0]=({ struct Cyc_Absyn_XTunionFieldType_struct
_temp3861; _temp3861.tag= Cyc_Absyn_XTunionFieldType_tag; _temp3861.f1=({ struct
Cyc_Absyn_XTunionFieldInfo _temp3862; _temp3862.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3866= yyvs; struct _xtunion_struct** _temp3868=
_temp3866.curr +( yyvsp_offset - 2); if( _temp3866.base == 0? 1:( _temp3868 <
_temp3866.base? 1: _temp3868 >= _temp3866.last_plus_one)){ _throw(
Null_Exception);}* _temp3868;})); _temp3862.fname= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3863= yyvs; struct _xtunion_struct** _temp3865=
_temp3863.curr + yyvsp_offset; if( _temp3863.base == 0? 1:( _temp3865 <
_temp3863.base? 1: _temp3865 >= _temp3863.last_plus_one)){ _throw(
Null_Exception);}* _temp3865;})); _temp3862.xtud= 0; _temp3862.xtufd= 0;
_temp3862;}); _temp3861;}); _temp3860;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3869= yyls; struct Cyc_Yyltype* _temp3871= _temp3869.curr +(
yylsp_offset - 3); if( _temp3869.base == 0? 1:( _temp3871 < _temp3869.base? 1:
_temp3871 >= _temp3869.last_plus_one)){ _throw( Null_Exception);}* _temp3871;}).first_line,({
struct _tagged_ptr3 _temp3872= yyls; struct Cyc_Yyltype* _temp3874= _temp3872.curr
+ yylsp_offset; if( _temp3872.base == 0? 1:( _temp3874 < _temp3872.base? 1:
_temp3874 >= _temp3872.last_plus_one)){ _throw( Null_Exception);}* _temp3874;}).last_line));
_temp3859;}); _temp3858;}); break; case 116: _LL3857: yyval=( struct
_xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct* _temp3876=( struct
Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3876[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3877; _temp3877.tag=
Cyc_TunionFieldList_tok_tag; _temp3877.f1=({ struct Cyc_List_List* _temp3878=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3878->hd=(
void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2 _temp3879= yyvs; struct
_xtunion_struct** _temp3881= _temp3879.curr + yyvsp_offset; if( _temp3879.base
== 0? 1:( _temp3881 < _temp3879.base? 1: _temp3881 >= _temp3879.last_plus_one)){
_throw( Null_Exception);}* _temp3881;})); _temp3878->tl= 0; _temp3878;});
_temp3877;}); _temp3876;}); break; case 117: _LL3875: yyval=( struct
_xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct* _temp3883=( struct
Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3883[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3884; _temp3884.tag=
Cyc_TunionFieldList_tok_tag; _temp3884.f1=({ struct Cyc_List_List* _temp3885=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3885->hd=(
void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2 _temp3886= yyvs; struct
_xtunion_struct** _temp3888= _temp3886.curr +( yyvsp_offset - 1); if( _temp3886.base
== 0? 1:( _temp3888 < _temp3886.base? 1: _temp3888 >= _temp3886.last_plus_one)){
_throw( Null_Exception);}* _temp3888;})); _temp3885->tl= 0; _temp3885;});
_temp3884;}); _temp3883;}); break; case 118: _LL3882: yyval=( struct
_xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct* _temp3890=( struct
Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3890[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3891; _temp3891.tag=
Cyc_TunionFieldList_tok_tag; _temp3891.f1=({ struct Cyc_List_List* _temp3892=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3892->hd=(
void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2 _temp3896= yyvs; struct
_xtunion_struct** _temp3898= _temp3896.curr +( yyvsp_offset - 2); if( _temp3896.base
== 0? 1:( _temp3898 < _temp3896.base? 1: _temp3898 >= _temp3896.last_plus_one)){
_throw( Null_Exception);}* _temp3898;})); _temp3892->tl= Cyc_yyget_TunionFieldList_tok(({
struct _tagged_ptr2 _temp3893= yyvs; struct _xtunion_struct** _temp3895=
_temp3893.curr + yyvsp_offset; if( _temp3893.base == 0? 1:( _temp3895 <
_temp3893.base? 1: _temp3895 >= _temp3893.last_plus_one)){ _throw(
Null_Exception);}* _temp3895;})); _temp3892;}); _temp3891;}); _temp3890;});
break; case 119: _LL3889: yyval=( struct _xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct*
_temp3900=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp3900[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp3901; _temp3901.tag= Cyc_TunionFieldList_tok_tag; _temp3901.f1=({ struct
Cyc_List_List* _temp3902=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3902->hd=( void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2
_temp3906= yyvs; struct _xtunion_struct** _temp3908= _temp3906.curr +(
yyvsp_offset - 2); if( _temp3906.base == 0? 1:( _temp3908 < _temp3906.base? 1:
_temp3908 >= _temp3906.last_plus_one)){ _throw( Null_Exception);}* _temp3908;}));
_temp3902->tl= Cyc_yyget_TunionFieldList_tok(({ struct _tagged_ptr2 _temp3903=
yyvs; struct _xtunion_struct** _temp3905= _temp3903.curr + yyvsp_offset; if(
_temp3903.base == 0? 1:( _temp3905 < _temp3903.base? 1: _temp3905 >= _temp3903.last_plus_one)){
_throw( Null_Exception);}* _temp3905;})); _temp3902;}); _temp3901;}); _temp3900;});
break; case 120: _LL3899: yyval=( struct _xtunion_struct*)({ struct Cyc_TunionField_tok_struct*
_temp3910=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp3910[ 0]=({ struct Cyc_TunionField_tok_struct _temp3911; _temp3911.tag= Cyc_TunionField_tok_tag;
_temp3911.f1=({ struct Cyc_Absyn_Tunionfield* _temp3912=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3912->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3919= yyvs; struct _xtunion_struct** _temp3921=
_temp3919.curr + yyvsp_offset; if( _temp3919.base == 0? 1:( _temp3921 <
_temp3919.base? 1: _temp3921 >= _temp3919.last_plus_one)){ _throw(
Null_Exception);}* _temp3921;})); _temp3912->tvs= 0; _temp3912->typs= 0;
_temp3912->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3913=
yyls; struct Cyc_Yyltype* _temp3915= _temp3913.curr + yylsp_offset; if(
_temp3913.base == 0? 1:( _temp3915 < _temp3913.base? 1: _temp3915 >= _temp3913.last_plus_one)){
_throw( Null_Exception);}* _temp3915;}).first_line,({ struct _tagged_ptr3
_temp3916= yyls; struct Cyc_Yyltype* _temp3918= _temp3916.curr + yylsp_offset;
if( _temp3916.base == 0? 1:( _temp3918 < _temp3916.base? 1: _temp3918 >=
_temp3916.last_plus_one)){ _throw( Null_Exception);}* _temp3918;}).last_line);
_temp3912;}); _temp3911;}); _temp3910;}); break; case 121: _LL3909: { struct Cyc_List_List*
typs=(( struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3923= yyls; struct Cyc_Yyltype* _temp3925= _temp3923.curr +(
yylsp_offset - 1); if( _temp3923.base == 0? 1:( _temp3925 < _temp3923.base? 1:
_temp3925 >= _temp3923.last_plus_one)){ _throw( Null_Exception);}* _temp3925;}).first_line,({
struct _tagged_ptr3 _temp3926= yyls; struct Cyc_Yyltype* _temp3928= _temp3926.curr
+( yylsp_offset - 1); if( _temp3926.base == 0? 1:( _temp3928 < _temp3926.base? 1:
_temp3928 >= _temp3926.last_plus_one)){ _throw( Null_Exception);}* _temp3928;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3929= yyvs; struct _xtunion_struct** _temp3931=
_temp3929.curr +( yyvsp_offset - 1); if( _temp3929.base == 0? 1:( _temp3931 <
_temp3929.base? 1: _temp3931 >= _temp3929.last_plus_one)){ _throw(
Null_Exception);}* _temp3931;})))); struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3932= yyls; struct Cyc_Yyltype* _temp3934= _temp3932.curr
+( yylsp_offset - 3); if( _temp3932.base == 0? 1:( _temp3934 < _temp3932.base? 1:
_temp3934 >= _temp3932.last_plus_one)){ _throw( Null_Exception);}* _temp3934;}).first_line,({
struct _tagged_ptr3 _temp3935= yyls; struct Cyc_Yyltype* _temp3937= _temp3935.curr
+( yylsp_offset - 3); if( _temp3935.base == 0? 1:( _temp3937 < _temp3935.base? 1:
_temp3937 >= _temp3935.last_plus_one)){ _throw( Null_Exception);}* _temp3937;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3938= yyvs; struct
_xtunion_struct** _temp3940= _temp3938.curr +( yyvsp_offset - 3); if( _temp3938.base
== 0? 1:( _temp3940 < _temp3938.base? 1: _temp3940 >= _temp3938.last_plus_one)){
_throw( Null_Exception);}* _temp3940;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_TunionField_tok_struct* _temp3941=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp3941[ 0]=({ struct
Cyc_TunionField_tok_struct _temp3942; _temp3942.tag= Cyc_TunionField_tok_tag;
_temp3942.f1=({ struct Cyc_Absyn_Tunionfield* _temp3943=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3943->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3950= yyvs; struct _xtunion_struct** _temp3952=
_temp3950.curr +( yyvsp_offset - 4); if( _temp3950.base == 0? 1:( _temp3952 <
_temp3950.base? 1: _temp3952 >= _temp3950.last_plus_one)){ _throw(
Null_Exception);}* _temp3952;})); _temp3943->tvs= tvs; _temp3943->typs= typs;
_temp3943->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3944=
yyls; struct Cyc_Yyltype* _temp3946= _temp3944.curr +( yylsp_offset - 4); if(
_temp3944.base == 0? 1:( _temp3946 < _temp3944.base? 1: _temp3946 >= _temp3944.last_plus_one)){
_throw( Null_Exception);}* _temp3946;}).first_line,({ struct _tagged_ptr3
_temp3947= yyls; struct Cyc_Yyltype* _temp3949= _temp3947.curr + yylsp_offset;
if( _temp3947.base == 0? 1:( _temp3949 < _temp3947.base? 1: _temp3949 >=
_temp3947.last_plus_one)){ _throw( Null_Exception);}* _temp3949;}).last_line);
_temp3943;}); _temp3942;}); _temp3941;}); break;} case 122: _LL3922: yyval=({
struct _tagged_ptr2 _temp3954= yyvs; struct _xtunion_struct** _temp3956=
_temp3954.curr + yyvsp_offset; if( _temp3954.base == 0? 1:( _temp3956 <
_temp3954.base? 1: _temp3956 >= _temp3954.last_plus_one)){ _throw(
Null_Exception);}* _temp3956;}); break; case 123: _LL3953: yyval=( struct
_xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp3958=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3958[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3959; _temp3959.tag= Cyc_Declarator_tok_tag;
_temp3959.f1=({ struct Cyc_Parse_Declarator* _temp3960=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3960->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3967= yyvs; struct _xtunion_struct** _temp3969=
_temp3967.curr + yyvsp_offset; if( _temp3967.base == 0? 1:( _temp3969 <
_temp3967.base? 1: _temp3969 >= _temp3967.last_plus_one)){ _throw(
Null_Exception);}* _temp3969;})))->id; _temp3960->tms=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp3961= yyvs; struct _xtunion_struct** _temp3963=
_temp3961.curr +( yyvsp_offset - 1); if( _temp3961.base == 0? 1:( _temp3963 <
_temp3961.base? 1: _temp3963 >= _temp3961.last_plus_one)){ _throw(
Null_Exception);}* _temp3963;})),( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3964= yyvs; struct _xtunion_struct** _temp3966= _temp3964.curr
+ yyvsp_offset; if( _temp3964.base == 0? 1:( _temp3966 < _temp3964.base? 1:
_temp3966 >= _temp3964.last_plus_one)){ _throw( Null_Exception);}* _temp3966;})))->tms);
_temp3960;}); _temp3959;}); _temp3958;}); break; case 124: _LL3957: yyval=(
struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp3971=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3971[ 0]=({ struct Cyc_Declarator_tok_struct _temp3972; _temp3972.tag= Cyc_Declarator_tok_tag;
_temp3972.f1=({ struct Cyc_Parse_Declarator* _temp3973=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3973->id= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3974= yyvs; struct _xtunion_struct** _temp3976=
_temp3974.curr + yyvsp_offset; if( _temp3974.base == 0? 1:( _temp3976 <
_temp3974.base? 1: _temp3976 >= _temp3974.last_plus_one)){ _throw(
Null_Exception);}* _temp3976;})); _temp3973->tms= 0; _temp3973;}); _temp3972;});
_temp3971;}); break; case 125: _LL3970: yyval=({ struct _tagged_ptr2 _temp3978=
yyvs; struct _xtunion_struct** _temp3980= _temp3978.curr +( yyvsp_offset - 1);
if( _temp3978.base == 0? 1:( _temp3980 < _temp3978.base? 1: _temp3980 >=
_temp3978.last_plus_one)){ _throw( Null_Exception);}* _temp3980;}); break; case
126: _LL3977: yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct*
_temp3982=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3982[ 0]=({ struct Cyc_Declarator_tok_struct _temp3983; _temp3983.tag= Cyc_Declarator_tok_tag;
_temp3983.f1=({ struct Cyc_Parse_Declarator* _temp3984=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3984->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3989= yyvs; struct _xtunion_struct** _temp3991=
_temp3989.curr +( yyvsp_offset - 2); if( _temp3989.base == 0? 1:( _temp3991 <
_temp3989.base? 1: _temp3991 >= _temp3989.last_plus_one)){ _throw(
Null_Exception);}* _temp3991;})))->id; _temp3984->tms=({ struct Cyc_List_List*
_temp3985=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3985->hd=( void*)(( void*) Cyc_Absyn_Carray_mod); _temp3985->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3986= yyvs; struct _xtunion_struct** _temp3988=
_temp3986.curr +( yyvsp_offset - 2); if( _temp3986.base == 0? 1:( _temp3988 <
_temp3986.base? 1: _temp3988 >= _temp3986.last_plus_one)){ _throw(
Null_Exception);}* _temp3988;})))->tms; _temp3985;}); _temp3984;}); _temp3983;});
_temp3982;}); break; case 127: _LL3981: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp3993=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3993[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3994; _temp3994.tag= Cyc_Declarator_tok_tag;
_temp3994.f1=({ struct Cyc_Parse_Declarator* _temp3995=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3995->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4005= yyvs; struct _xtunion_struct** _temp4007=
_temp4005.curr +( yyvsp_offset - 3); if( _temp4005.base == 0? 1:( _temp4007 <
_temp4005.base? 1: _temp4007 >= _temp4005.last_plus_one)){ _throw(
Null_Exception);}* _temp4007;})))->id; _temp3995->tms=({ struct Cyc_List_List*
_temp3996=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3996->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4000=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4000[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp4001; _temp4001.tag= Cyc_Absyn_ConstArray_mod_tag; _temp4001.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4002= yyvs; struct _xtunion_struct** _temp4004=
_temp4002.curr +( yyvsp_offset - 1); if( _temp4002.base == 0? 1:( _temp4004 <
_temp4002.base? 1: _temp4004 >= _temp4002.last_plus_one)){ _throw(
Null_Exception);}* _temp4004;})); _temp4001;}); _temp4000;})); _temp3996->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3997= yyvs; struct
_xtunion_struct** _temp3999= _temp3997.curr +( yyvsp_offset - 3); if( _temp3997.base
== 0? 1:( _temp3999 < _temp3997.base? 1: _temp3999 >= _temp3997.last_plus_one)){
_throw( Null_Exception);}* _temp3999;})))->tms; _temp3996;}); _temp3995;});
_temp3994;}); _temp3993;}); break; case 128: _LL3992: { struct _tuple16
_temp4014; struct Cyc_Core_Opt* _temp4015; int _temp4017; struct Cyc_List_List*
_temp4019; struct _tuple16* _temp4012= Cyc_yyget_ParamDeclListBool_tok(({ struct
_tagged_ptr2 _temp4009= yyvs; struct _xtunion_struct** _temp4011= _temp4009.curr
+( yyvsp_offset - 1); if( _temp4009.base == 0? 1:( _temp4011 < _temp4009.base? 1:
_temp4011 >= _temp4009.last_plus_one)){ _throw( Null_Exception);}* _temp4011;}));
_temp4014=* _temp4012; _LL4020: _temp4019= _temp4014.f1; goto _LL4018; _LL4018:
_temp4017= _temp4014.f2; goto _LL4016; _LL4016: _temp4015= _temp4014.f3; goto
_LL4013; _LL4013: yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct*
_temp4021=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp4021[ 0]=({ struct Cyc_Declarator_tok_struct _temp4022; _temp4022.tag= Cyc_Declarator_tok_tag;
_temp4022.f1=({ struct Cyc_Parse_Declarator* _temp4023=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4023->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4032= yyvs; struct _xtunion_struct** _temp4034=
_temp4032.curr +( yyvsp_offset - 3); if( _temp4032.base == 0? 1:( _temp4034 <
_temp4032.base? 1: _temp4034 >= _temp4032.last_plus_one)){ _throw(
Null_Exception);}* _temp4034;})))->id; _temp4023->tms=({ struct Cyc_List_List*
_temp4024=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4024->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4028=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4028[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4029; _temp4029.tag=
Cyc_Absyn_Function_mod_tag; _temp4029.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4030=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4030[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4031; _temp4031.tag= Cyc_Absyn_WithTypes_tag;
_temp4031.f1= _temp4019; _temp4031.f2= _temp4017; _temp4031.f3= _temp4015;
_temp4031;}); _temp4030;})); _temp4029;}); _temp4028;})); _temp4024->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4025= yyvs; struct _xtunion_struct** _temp4027=
_temp4025.curr +( yyvsp_offset - 3); if( _temp4025.base == 0? 1:( _temp4027 <
_temp4025.base? 1: _temp4027 >= _temp4025.last_plus_one)){ _throw(
Null_Exception);}* _temp4027;})))->tms; _temp4024;}); _temp4023;}); _temp4022;});
_temp4021;}); break;} case 129: _LL4008: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp4036=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4036[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4037; _temp4037.tag= Cyc_Declarator_tok_tag;
_temp4037.f1=({ struct Cyc_Parse_Declarator* _temp4038=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4038->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4047= yyvs; struct _xtunion_struct** _temp4049=
_temp4047.curr +( yyvsp_offset - 2); if( _temp4047.base == 0? 1:( _temp4049 <
_temp4047.base? 1: _temp4049 >= _temp4047.last_plus_one)){ _throw(
Null_Exception);}* _temp4049;})))->id; _temp4038->tms=({ struct Cyc_List_List*
_temp4039=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4039->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4043=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4043[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4044; _temp4044.tag=
Cyc_Absyn_Function_mod_tag; _temp4044.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4045=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4045[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4046; _temp4046.tag= Cyc_Absyn_WithTypes_tag;
_temp4046.f1= 0; _temp4046.f2= 0; _temp4046.f3= 0; _temp4046;}); _temp4045;}));
_temp4044;}); _temp4043;})); _temp4039->tl=( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp4040= yyvs; struct _xtunion_struct** _temp4042= _temp4040.curr
+( yyvsp_offset - 2); if( _temp4040.base == 0? 1:( _temp4042 < _temp4040.base? 1:
_temp4042 >= _temp4040.last_plus_one)){ _throw( Null_Exception);}* _temp4042;})))->tms;
_temp4039;}); _temp4038;}); _temp4037;}); _temp4036;}); break; case 130: _LL4035:
yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp4051=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp4051[ 0]=({ struct Cyc_Declarator_tok_struct _temp4052; _temp4052.tag= Cyc_Declarator_tok_tag;
_temp4052.f1=({ struct Cyc_Parse_Declarator* _temp4053=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4053->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4068= yyvs; struct _xtunion_struct** _temp4070=
_temp4068.curr +( yyvsp_offset - 4); if( _temp4068.base == 0? 1:( _temp4070 <
_temp4068.base? 1: _temp4070 >= _temp4068.last_plus_one)){ _throw(
Null_Exception);}* _temp4070;})))->id; _temp4053->tms=({ struct Cyc_List_List*
_temp4054=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4054->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4058=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4058[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4059; _temp4059.tag=
Cyc_Absyn_Function_mod_tag; _temp4059.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4060=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4060[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4061; _temp4061.tag= Cyc_Absyn_WithTypes_tag;
_temp4061.f1= 0; _temp4061.f2= 0; _temp4061.f3=({ struct Cyc_Core_Opt* _temp4062=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4062->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4063=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4063[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4064; _temp4064.tag= Cyc_Absyn_JoinEff_tag;
_temp4064.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4065= yyvs;
struct _xtunion_struct** _temp4067= _temp4065.curr +( yyvsp_offset - 1); if(
_temp4065.base == 0? 1:( _temp4067 < _temp4065.base? 1: _temp4067 >= _temp4065.last_plus_one)){
_throw( Null_Exception);}* _temp4067;})); _temp4064;}); _temp4063;})); _temp4062;});
_temp4061;}); _temp4060;})); _temp4059;}); _temp4058;})); _temp4054->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4055= yyvs; struct _xtunion_struct** _temp4057=
_temp4055.curr +( yyvsp_offset - 4); if( _temp4055.base == 0? 1:( _temp4057 <
_temp4055.base? 1: _temp4057 >= _temp4055.last_plus_one)){ _throw(
Null_Exception);}* _temp4057;})))->tms; _temp4054;}); _temp4053;}); _temp4052;});
_temp4051;}); break; case 131: _LL4050: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp4072=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4072[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4073; _temp4073.tag= Cyc_Declarator_tok_tag;
_temp4073.f1=({ struct Cyc_Parse_Declarator* _temp4074=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4074->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4092= yyvs; struct _xtunion_struct** _temp4094=
_temp4092.curr +( yyvsp_offset - 3); if( _temp4092.base == 0? 1:( _temp4094 <
_temp4092.base? 1: _temp4094 >= _temp4092.last_plus_one)){ _throw(
Null_Exception);}* _temp4094;})))->id; _temp4074->tms=({ struct Cyc_List_List*
_temp4075=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4075->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4079=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4079[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4080; _temp4080.tag=
Cyc_Absyn_Function_mod_tag; _temp4080.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct*
_temp4081=( struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp4081[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp4082; _temp4082.tag= Cyc_Absyn_NoTypes_tag;
_temp4082.f1= Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp4083= yyvs;
struct _xtunion_struct** _temp4085= _temp4083.curr +( yyvsp_offset - 1); if(
_temp4083.base == 0? 1:( _temp4085 < _temp4083.base? 1: _temp4085 >= _temp4083.last_plus_one)){
_throw( Null_Exception);}* _temp4085;})); _temp4082.f2= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4086= yyls; struct Cyc_Yyltype* _temp4088= _temp4086.curr
+( yylsp_offset - 3); if( _temp4086.base == 0? 1:( _temp4088 < _temp4086.base? 1:
_temp4088 >= _temp4086.last_plus_one)){ _throw( Null_Exception);}* _temp4088;}).first_line,({
struct _tagged_ptr3 _temp4089= yyls; struct Cyc_Yyltype* _temp4091= _temp4089.curr
+ yylsp_offset; if( _temp4089.base == 0? 1:( _temp4091 < _temp4089.base? 1:
_temp4091 >= _temp4089.last_plus_one)){ _throw( Null_Exception);}* _temp4091;}).last_line);
_temp4082;}); _temp4081;})); _temp4080;}); _temp4079;})); _temp4075->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4076= yyvs; struct _xtunion_struct** _temp4078=
_temp4076.curr +( yyvsp_offset - 3); if( _temp4076.base == 0? 1:( _temp4078 <
_temp4076.base? 1: _temp4078 >= _temp4076.last_plus_one)){ _throw(
Null_Exception);}* _temp4078;})))->tms; _temp4075;}); _temp4074;}); _temp4073;});
_temp4072;}); break; case 132: _LL4071: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4096= yyls; struct Cyc_Yyltype* _temp4098= _temp4096.curr
+( yylsp_offset - 2); if( _temp4096.base == 0? 1:( _temp4098 < _temp4096.base? 1:
_temp4098 >= _temp4096.last_plus_one)){ _throw( Null_Exception);}* _temp4098;}).first_line,({
struct _tagged_ptr3 _temp4099= yyls; struct Cyc_Yyltype* _temp4101= _temp4099.curr
+ yylsp_offset; if( _temp4099.base == 0? 1:( _temp4101 < _temp4099.base? 1:
_temp4101 >= _temp4099.last_plus_one)){ _throw( Null_Exception);}* _temp4101;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4102= yyvs; struct _xtunion_struct** _temp4104=
_temp4102.curr +( yyvsp_offset - 1); if( _temp4102.base == 0? 1:( _temp4104 <
_temp4102.base? 1: _temp4104 >= _temp4102.last_plus_one)){ _throw(
Null_Exception);}* _temp4104;})))); yyval=( struct _xtunion_struct*)({ struct
Cyc_Declarator_tok_struct* _temp4105=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4105[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4106; _temp4106.tag= Cyc_Declarator_tok_tag;
_temp4106.f1=({ struct Cyc_Parse_Declarator* _temp4107=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4107->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4120= yyvs; struct _xtunion_struct** _temp4122=
_temp4120.curr +( yyvsp_offset - 3); if( _temp4120.base == 0? 1:( _temp4122 <
_temp4120.base? 1: _temp4122 >= _temp4120.last_plus_one)){ _throw(
Null_Exception);}* _temp4122;})))->id; _temp4107->tms=({ struct Cyc_List_List*
_temp4108=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4108->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4112=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4112[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp4113; _temp4113.tag= Cyc_Absyn_TypeParams_mod_tag; _temp4113.f1= ts;
_temp4113.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4114= yyls;
struct Cyc_Yyltype* _temp4116= _temp4114.curr +( yylsp_offset - 3); if(
_temp4114.base == 0? 1:( _temp4116 < _temp4114.base? 1: _temp4116 >= _temp4114.last_plus_one)){
_throw( Null_Exception);}* _temp4116;}).first_line,({ struct _tagged_ptr3
_temp4117= yyls; struct Cyc_Yyltype* _temp4119= _temp4117.curr + yylsp_offset;
if( _temp4117.base == 0? 1:( _temp4119 < _temp4117.base? 1: _temp4119 >=
_temp4117.last_plus_one)){ _throw( Null_Exception);}* _temp4119;}).last_line);
_temp4113.f3= 0; _temp4113;}); _temp4112;})); _temp4108->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4109= yyvs; struct _xtunion_struct** _temp4111=
_temp4109.curr +( yyvsp_offset - 3); if( _temp4109.base == 0? 1:( _temp4111 <
_temp4109.base? 1: _temp4111 >= _temp4109.last_plus_one)){ _throw(
Null_Exception);}* _temp4111;})))->tms; _temp4108;}); _temp4107;}); _temp4106;});
_temp4105;}); break;} case 133: _LL4095:(( struct Cyc_Lexing_lexbuf*)({ struct
Cyc_Core_Opt* _temp4124= Cyc_Parse_lbuf; if( _temp4124 == 0){ _throw(
Null_Exception);} _temp4124->v;}))->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4125=
yyls; struct Cyc_Yyltype* _temp4127= _temp4125.curr +( yylsp_offset - 2); if(
_temp4125.base == 0? 1:( _temp4127 < _temp4125.base? 1: _temp4127 >= _temp4125.last_plus_one)){
_throw( Null_Exception);}* _temp4127;}).first_line,({ struct _tagged_ptr3
_temp4128= yyls; struct Cyc_Yyltype* _temp4130= _temp4128.curr + yylsp_offset;
if( _temp4128.base == 0? 1:( _temp4130 < _temp4128.base? 1: _temp4130 >=
_temp4128.last_plus_one)){ _throw( Null_Exception);}* _temp4130;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4131= yyvs; struct _xtunion_struct** _temp4133=
_temp4131.curr +( yyvsp_offset - 1); if( _temp4131.base == 0? 1:( _temp4133 <
_temp4131.base? 1: _temp4133 >= _temp4131.last_plus_one)){ _throw(
Null_Exception);}* _temp4133;})))); yyval=( struct _xtunion_struct*)({ struct
Cyc_Declarator_tok_struct* _temp4134=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4134[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4135; _temp4135.tag= Cyc_Declarator_tok_tag;
_temp4135.f1=({ struct Cyc_Parse_Declarator* _temp4136=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4136->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4149= yyvs; struct _xtunion_struct** _temp4151=
_temp4149.curr +( yyvsp_offset - 3); if( _temp4149.base == 0? 1:( _temp4151 <
_temp4149.base? 1: _temp4151 >= _temp4149.last_plus_one)){ _throw(
Null_Exception);}* _temp4151;})))->id; _temp4136->tms=({ struct Cyc_List_List*
_temp4137=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4137->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4141=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4141[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp4142; _temp4142.tag= Cyc_Absyn_TypeParams_mod_tag; _temp4142.f1= ts;
_temp4142.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4143= yyls;
struct Cyc_Yyltype* _temp4145= _temp4143.curr +( yylsp_offset - 3); if(
_temp4143.base == 0? 1:( _temp4145 < _temp4143.base? 1: _temp4145 >= _temp4143.last_plus_one)){
_throw( Null_Exception);}* _temp4145;}).first_line,({ struct _tagged_ptr3
_temp4146= yyls; struct Cyc_Yyltype* _temp4148= _temp4146.curr + yylsp_offset;
if( _temp4146.base == 0? 1:( _temp4148 < _temp4146.base? 1: _temp4148 >=
_temp4146.last_plus_one)){ _throw( Null_Exception);}* _temp4148;}).last_line);
_temp4142.f3= 0; _temp4142;}); _temp4141;})); _temp4137->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4138= yyvs; struct _xtunion_struct** _temp4140=
_temp4138.curr +( yyvsp_offset - 3); if( _temp4138.base == 0? 1:( _temp4140 <
_temp4138.base? 1: _temp4140 >= _temp4138.last_plus_one)){ _throw(
Null_Exception);}* _temp4140;})))->tms; _temp4137;}); _temp4136;}); _temp4135;});
_temp4134;}); break;} case 134: _LL4123: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp4153=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4153[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4154; _temp4154.tag= Cyc_Declarator_tok_tag;
_temp4154.f1=({ struct Cyc_Parse_Declarator* _temp4155=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4155->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4171= yyvs; struct _xtunion_struct** _temp4173=
_temp4171.curr +( yyvsp_offset - 1); if( _temp4171.base == 0? 1:( _temp4173 <
_temp4171.base? 1: _temp4173 >= _temp4171.last_plus_one)){ _throw(
Null_Exception);}* _temp4173;})))->id; _temp4155->tms=({ struct Cyc_List_List*
_temp4156=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4156->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp4160=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp4160[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp4161; _temp4161.tag= Cyc_Absyn_Attributes_mod_tag; _temp4161.f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4162= yyls; struct Cyc_Yyltype* _temp4164= _temp4162.curr
+ yylsp_offset; if( _temp4162.base == 0? 1:( _temp4164 < _temp4162.base? 1:
_temp4164 >= _temp4162.last_plus_one)){ _throw( Null_Exception);}* _temp4164;}).first_line,({
struct _tagged_ptr3 _temp4165= yyls; struct Cyc_Yyltype* _temp4167= _temp4165.curr
+ yylsp_offset; if( _temp4165.base == 0? 1:( _temp4167 < _temp4165.base? 1:
_temp4167 >= _temp4165.last_plus_one)){ _throw( Null_Exception);}* _temp4167;}).last_line);
_temp4161.f2= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4168= yyvs;
struct _xtunion_struct** _temp4170= _temp4168.curr + yyvsp_offset; if( _temp4168.base
== 0? 1:( _temp4170 < _temp4168.base? 1: _temp4170 >= _temp4168.last_plus_one)){
_throw( Null_Exception);}* _temp4170;})); _temp4161;}); _temp4160;})); _temp4156->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp4157= yyvs; struct
_xtunion_struct** _temp4159= _temp4157.curr +( yyvsp_offset - 1); if( _temp4157.base
== 0? 1:( _temp4159 < _temp4157.base? 1: _temp4159 >= _temp4157.last_plus_one)){
_throw( Null_Exception);}* _temp4159;})))->tms; _temp4156;}); _temp4155;});
_temp4154;}); _temp4153;}); break; case 135: _LL4152: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeModifierList_tok_struct* _temp4175=( struct
Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp4175[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp4176; _temp4176.tag=
Cyc_TypeModifierList_tok_tag; _temp4176.f1=({ struct Cyc_List_List* _temp4177=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4177->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4187=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4187[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4188; _temp4188.tag= Cyc_Absyn_Pointer_mod_tag;
_temp4188.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4189= yyvs; struct _xtunion_struct** _temp4191= _temp4189.curr +(
yyvsp_offset - 2); if( _temp4189.base == 0? 1:( _temp4191 < _temp4189.base? 1:
_temp4191 >= _temp4189.last_plus_one)){ _throw( Null_Exception);}* _temp4191;}));
_temp4188.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4192= yyvs;
struct _xtunion_struct** _temp4194= _temp4192.curr +( yyvsp_offset - 1); if(
_temp4192.base == 0? 1:( _temp4194 < _temp4192.base? 1: _temp4194 >= _temp4192.last_plus_one)){
_throw( Null_Exception);}* _temp4194;})); _temp4188.f3= Cyc_Absyn_empty_tqual();
_temp4188;}); _temp4187;})); _temp4177->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4178= yyls; struct Cyc_Yyltype* _temp4180= _temp4178.curr
+ yylsp_offset; if( _temp4178.base == 0? 1:( _temp4180 < _temp4178.base? 1:
_temp4180 >= _temp4178.last_plus_one)){ _throw( Null_Exception);}* _temp4180;}).first_line,({
struct _tagged_ptr3 _temp4181= yyls; struct Cyc_Yyltype* _temp4183= _temp4181.curr
+ yylsp_offset; if( _temp4181.base == 0? 1:( _temp4183 < _temp4181.base? 1:
_temp4183 >= _temp4181.last_plus_one)){ _throw( Null_Exception);}* _temp4183;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4184= yyvs; struct
_xtunion_struct** _temp4186= _temp4184.curr + yyvsp_offset; if( _temp4184.base
== 0? 1:( _temp4186 < _temp4184.base? 1: _temp4186 >= _temp4184.last_plus_one)){
_throw( Null_Exception);}* _temp4186;})), 0); _temp4177;}); _temp4176;});
_temp4175;}); break; case 136: _LL4174: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeModifierList_tok_struct* _temp4196=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp4196[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp4197; _temp4197.tag= Cyc_TypeModifierList_tok_tag;
_temp4197.f1=({ struct Cyc_List_List* _temp4198=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4198->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp4208=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4208[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4209; _temp4209.tag= Cyc_Absyn_Pointer_mod_tag;
_temp4209.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4210= yyvs; struct _xtunion_struct** _temp4212= _temp4210.curr +(
yyvsp_offset - 3); if( _temp4210.base == 0? 1:( _temp4212 < _temp4210.base? 1:
_temp4212 >= _temp4210.last_plus_one)){ _throw( Null_Exception);}* _temp4212;}));
_temp4209.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4213= yyvs;
struct _xtunion_struct** _temp4215= _temp4213.curr +( yyvsp_offset - 2); if(
_temp4213.base == 0? 1:( _temp4215 < _temp4213.base? 1: _temp4215 >= _temp4213.last_plus_one)){
_throw( Null_Exception);}* _temp4215;})); _temp4209.f3= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp4216= yyvs; struct _xtunion_struct** _temp4218=
_temp4216.curr + yyvsp_offset; if( _temp4216.base == 0? 1:( _temp4218 <
_temp4216.base? 1: _temp4218 >= _temp4216.last_plus_one)){ _throw(
Null_Exception);}* _temp4218;})); _temp4209;}); _temp4208;})); _temp4198->tl=
Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4199= yyls; struct Cyc_Yyltype* _temp4201= _temp4199.curr +( yylsp_offset -
1); if( _temp4199.base == 0? 1:( _temp4201 < _temp4199.base? 1: _temp4201 >=
_temp4199.last_plus_one)){ _throw( Null_Exception);}* _temp4201;}).first_line,({
struct _tagged_ptr3 _temp4202= yyls; struct Cyc_Yyltype* _temp4204= _temp4202.curr
+( yylsp_offset - 1); if( _temp4202.base == 0? 1:( _temp4204 < _temp4202.base? 1:
_temp4204 >= _temp4202.last_plus_one)){ _throw( Null_Exception);}* _temp4204;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4205= yyvs; struct
_xtunion_struct** _temp4207= _temp4205.curr +( yyvsp_offset - 1); if( _temp4205.base
== 0? 1:( _temp4207 < _temp4205.base? 1: _temp4207 >= _temp4205.last_plus_one)){
_throw( Null_Exception);}* _temp4207;})), 0); _temp4198;}); _temp4197;});
_temp4196;}); break; case 137: _LL4195: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeModifierList_tok_struct* _temp4220=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp4220[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp4221; _temp4221.tag= Cyc_TypeModifierList_tok_tag;
_temp4221.f1=({ struct Cyc_List_List* _temp4222=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4222->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp4235=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4235[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4236; _temp4236.tag= Cyc_Absyn_Pointer_mod_tag;
_temp4236.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4237= yyvs; struct _xtunion_struct** _temp4239= _temp4237.curr +(
yyvsp_offset - 3); if( _temp4237.base == 0? 1:( _temp4239 < _temp4237.base? 1:
_temp4239 >= _temp4237.last_plus_one)){ _throw( Null_Exception);}* _temp4239;}));
_temp4236.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4240= yyvs;
struct _xtunion_struct** _temp4242= _temp4240.curr +( yyvsp_offset - 2); if(
_temp4240.base == 0? 1:( _temp4242 < _temp4240.base? 1: _temp4242 >= _temp4240.last_plus_one)){
_throw( Null_Exception);}* _temp4242;})); _temp4236.f3= Cyc_Absyn_empty_tqual();
_temp4236;}); _temp4235;})); _temp4222->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4223= yyls; struct Cyc_Yyltype* _temp4225= _temp4223.curr
+( yylsp_offset - 1); if( _temp4223.base == 0? 1:( _temp4225 < _temp4223.base? 1:
_temp4225 >= _temp4223.last_plus_one)){ _throw( Null_Exception);}* _temp4225;}).first_line,({
struct _tagged_ptr3 _temp4226= yyls; struct Cyc_Yyltype* _temp4228= _temp4226.curr
+( yylsp_offset - 1); if( _temp4226.base == 0? 1:( _temp4228 < _temp4226.base? 1:
_temp4228 >= _temp4226.last_plus_one)){ _throw( Null_Exception);}* _temp4228;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4229= yyvs; struct
_xtunion_struct** _temp4231= _temp4229.curr +( yyvsp_offset - 1); if( _temp4229.base
== 0? 1:( _temp4231 < _temp4229.base? 1: _temp4231 >= _temp4229.last_plus_one)){
_throw( Null_Exception);}* _temp4231;})), Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp4232= yyvs; struct _xtunion_struct** _temp4234=
_temp4232.curr + yyvsp_offset; if( _temp4232.base == 0? 1:( _temp4234 <
_temp4232.base? 1: _temp4234 >= _temp4232.last_plus_one)){ _throw(
Null_Exception);}* _temp4234;}))); _temp4222;}); _temp4221;}); _temp4220;});
break; case 138: _LL4219: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeModifierList_tok_struct*
_temp4244=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp4244[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp4245; _temp4245.tag= Cyc_TypeModifierList_tok_tag; _temp4245.f1=({ struct
Cyc_List_List* _temp4246=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4246->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4259=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp4259[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp4260; _temp4260.tag=
Cyc_Absyn_Pointer_mod_tag; _temp4260.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp4261= yyvs; struct _xtunion_struct** _temp4263=
_temp4261.curr +( yyvsp_offset - 4); if( _temp4261.base == 0? 1:( _temp4263 <
_temp4261.base? 1: _temp4263 >= _temp4261.last_plus_one)){ _throw(
Null_Exception);}* _temp4263;})); _temp4260.f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp4264= yyvs; struct _xtunion_struct** _temp4266=
_temp4264.curr +( yyvsp_offset - 3); if( _temp4264.base == 0? 1:( _temp4266 <
_temp4264.base? 1: _temp4266 >= _temp4264.last_plus_one)){ _throw(
Null_Exception);}* _temp4266;})); _temp4260.f3= Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp4267= yyvs; struct _xtunion_struct** _temp4269= _temp4267.curr
+( yyvsp_offset - 1); if( _temp4267.base == 0? 1:( _temp4269 < _temp4267.base? 1:
_temp4269 >= _temp4267.last_plus_one)){ _throw( Null_Exception);}* _temp4269;}));
_temp4260;}); _temp4259;})); _temp4246->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4247= yyls; struct Cyc_Yyltype* _temp4249= _temp4247.curr
+( yylsp_offset - 2); if( _temp4247.base == 0? 1:( _temp4249 < _temp4247.base? 1:
_temp4249 >= _temp4247.last_plus_one)){ _throw( Null_Exception);}* _temp4249;}).first_line,({
struct _tagged_ptr3 _temp4250= yyls; struct Cyc_Yyltype* _temp4252= _temp4250.curr
+( yylsp_offset - 2); if( _temp4250.base == 0? 1:( _temp4252 < _temp4250.base? 1:
_temp4252 >= _temp4250.last_plus_one)){ _throw( Null_Exception);}* _temp4252;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4253= yyvs; struct
_xtunion_struct** _temp4255= _temp4253.curr +( yyvsp_offset - 2); if( _temp4253.base
== 0? 1:( _temp4255 < _temp4253.base? 1: _temp4255 >= _temp4253.last_plus_one)){
_throw( Null_Exception);}* _temp4255;})), Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp4256= yyvs; struct _xtunion_struct** _temp4258=
_temp4256.curr + yyvsp_offset; if( _temp4256.base == 0? 1:( _temp4258 <
_temp4256.base? 1: _temp4258 >= _temp4256.last_plus_one)){ _throw(
Null_Exception);}* _temp4258;}))); _temp4246;}); _temp4245;}); _temp4244;});
break; case 139: _LL4243: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4271=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4271[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4272; _temp4272.tag=
Cyc_Pointer_Sort_tok_tag; _temp4272.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp4273=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp4273[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp4274; _temp4274.tag=
Cyc_Absyn_Nullable_ps_tag; _temp4274.f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4275= yyls; struct Cyc_Yyltype* _temp4277= _temp4275.curr
+ yylsp_offset; if( _temp4275.base == 0? 1:( _temp4277 < _temp4275.base? 1:
_temp4277 >= _temp4275.last_plus_one)){ _throw( Null_Exception);}* _temp4277;}).first_line,({
struct _tagged_ptr3 _temp4278= yyls; struct Cyc_Yyltype* _temp4280= _temp4278.curr
+ yylsp_offset; if( _temp4278.base == 0? 1:( _temp4280 < _temp4278.base? 1:
_temp4280 >= _temp4278.last_plus_one)){ _throw( Null_Exception);}* _temp4280;}).last_line));
_temp4274;}); _temp4273;})); _temp4272;}); _temp4271;}); break; case 140:
_LL4270: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4282=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4282[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4283; _temp4283.tag=
Cyc_Pointer_Sort_tok_tag; _temp4283.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp4284=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp4284[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp4285; _temp4285.tag= Cyc_Absyn_NonNullable_ps_tag; _temp4285.f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4286= yyls; struct
Cyc_Yyltype* _temp4288= _temp4286.curr + yylsp_offset; if( _temp4286.base == 0?
1:( _temp4288 < _temp4286.base? 1: _temp4288 >= _temp4286.last_plus_one)){
_throw( Null_Exception);}* _temp4288;}).first_line,({ struct _tagged_ptr3
_temp4289= yyls; struct Cyc_Yyltype* _temp4291= _temp4289.curr + yylsp_offset;
if( _temp4289.base == 0? 1:( _temp4291 < _temp4289.base? 1: _temp4291 >=
_temp4289.last_plus_one)){ _throw( Null_Exception);}* _temp4291;}).last_line));
_temp4285;}); _temp4284;})); _temp4283;}); _temp4282;}); break; case 141:
_LL4281: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4293=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4293[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4294; _temp4294.tag=
Cyc_Pointer_Sort_tok_tag; _temp4294.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp4295=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp4295[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp4296; _temp4296.tag=
Cyc_Absyn_Nullable_ps_tag; _temp4296.f1= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4297= yyvs; struct _xtunion_struct** _temp4299= _temp4297.curr
+( yyvsp_offset - 1); if( _temp4297.base == 0? 1:( _temp4299 < _temp4297.base? 1:
_temp4299 >= _temp4297.last_plus_one)){ _throw( Null_Exception);}* _temp4299;}));
_temp4296;}); _temp4295;})); _temp4294;}); _temp4293;}); break; case 142:
_LL4292: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4301=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4301[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4302; _temp4302.tag=
Cyc_Pointer_Sort_tok_tag; _temp4302.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp4303=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp4303[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp4304; _temp4304.tag= Cyc_Absyn_NonNullable_ps_tag; _temp4304.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4305= yyvs; struct _xtunion_struct** _temp4307=
_temp4305.curr +( yyvsp_offset - 1); if( _temp4305.base == 0? 1:( _temp4307 <
_temp4305.base? 1: _temp4307 >= _temp4305.last_plus_one)){ _throw(
Null_Exception);}* _temp4307;})); _temp4304;}); _temp4303;})); _temp4302;});
_temp4301;}); break; case 143: _LL4300: yyval=( struct _xtunion_struct*)({
struct Cyc_Pointer_Sort_tok_struct* _temp4309=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp4309[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp4310; _temp4310.tag= Cyc_Pointer_Sort_tok_tag;
_temp4310.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp4310;});
_temp4309;}); break; case 144: _LL4308: yyval=( struct _xtunion_struct*)({
struct Cyc_Rgn_tok_struct* _temp4312=( struct Cyc_Rgn_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgn_tok_struct)); _temp4312[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4313; _temp4313.tag= Cyc_Rgn_tok_tag; _temp4313.f1=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp4313;}); _temp4312;}); break; case 145: _LL4311: yyval=({ struct
_tagged_ptr2 _temp4315= yyvs; struct _xtunion_struct** _temp4317= _temp4315.curr
+ yyvsp_offset; if( _temp4315.base == 0? 1:( _temp4317 < _temp4315.base? 1:
_temp4317 >= _temp4315.last_plus_one)){ _throw( Null_Exception);}* _temp4317;});
break; case 146: _LL4314: yyval=( struct _xtunion_struct*)({ struct Cyc_Rgn_tok_struct*
_temp4319=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp4319[ 0]=({ struct Cyc_Rgn_tok_struct _temp4320; _temp4320.tag= Cyc_Rgn_tok_tag;
_temp4320.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4321= yyvs; struct _xtunion_struct** _temp4323= _temp4321.curr
+ yyvsp_offset; if( _temp4321.base == 0? 1:( _temp4323 < _temp4321.base? 1:
_temp4323 >= _temp4321.last_plus_one)){ _throw( Null_Exception);}* _temp4323;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4320;}); _temp4319;}); break; case 147: _LL4318: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp4325= yyvs; struct _xtunion_struct** _temp4327=
_temp4325.curr + yyvsp_offset; if( _temp4325.base == 0? 1:( _temp4327 <
_temp4325.base? 1: _temp4327 >= _temp4325.last_plus_one)){ _throw(
Null_Exception);}* _temp4327;})) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4328=( char*)"expecting region kind\n";
struct _tagged_string _temp4329; _temp4329.curr= _temp4328; _temp4329.base=
_temp4328; _temp4329.last_plus_one= _temp4328 + 23; _temp4329;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4330= yyls; struct Cyc_Yyltype* _temp4332= _temp4330.curr
+ yylsp_offset; if( _temp4330.base == 0? 1:( _temp4332 < _temp4330.base? 1:
_temp4332 >= _temp4330.last_plus_one)){ _throw( Null_Exception);}* _temp4332;}).first_line,({
struct _tagged_ptr3 _temp4333= yyls; struct Cyc_Yyltype* _temp4335= _temp4333.curr
+ yylsp_offset; if( _temp4333.base == 0? 1:( _temp4335 < _temp4333.base? 1:
_temp4335 >= _temp4333.last_plus_one)){ _throw( Null_Exception);}* _temp4335;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_Rgn_tok_struct* _temp4336=( struct
Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4336[
0]=({ struct Cyc_Rgn_tok_struct _temp4337; _temp4337.tag= Cyc_Rgn_tok_tag;
_temp4337.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4338= yyvs; struct _xtunion_struct** _temp4340= _temp4338.curr
+( yyvsp_offset - 2); if( _temp4338.base == 0? 1:( _temp4340 < _temp4338.base? 1:
_temp4340 >= _temp4338.last_plus_one)){ _throw( Null_Exception);}* _temp4340;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4337;}); _temp4336;}); break; case 148: _LL4324: yyval=( struct
_xtunion_struct*)({ struct Cyc_Rgn_tok_struct* _temp4342=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4342[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4343; _temp4343.tag= Cyc_Rgn_tok_tag; _temp4343.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp4343;}); _temp4342;}); break; case 149: _LL4341:
yyval=({ struct _tagged_ptr2 _temp4345= yyvs; struct _xtunion_struct** _temp4347=
_temp4345.curr + yyvsp_offset; if( _temp4345.base == 0? 1:( _temp4347 <
_temp4345.base? 1: _temp4347 >= _temp4345.last_plus_one)){ _throw(
Null_Exception);}* _temp4347;}); break; case 150: _LL4344: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeQual_tok_struct* _temp4349=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp4349[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp4350; _temp4350.tag= Cyc_TypeQual_tok_tag; _temp4350.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp4351= yyvs; struct
_xtunion_struct** _temp4353= _temp4351.curr +( yyvsp_offset - 1); if( _temp4351.base
== 0? 1:( _temp4353 < _temp4351.base? 1: _temp4353 >= _temp4351.last_plus_one)){
_throw( Null_Exception);}* _temp4353;})), Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp4354= yyvs; struct _xtunion_struct** _temp4356= _temp4354.curr
+ yyvsp_offset; if( _temp4354.base == 0? 1:( _temp4356 < _temp4354.base? 1:
_temp4356 >= _temp4354.last_plus_one)){ _throw( Null_Exception);}* _temp4356;})));
_temp4350;}); _temp4349;}); break; case 151: _LL4348: yyval=( struct
_xtunion_struct*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp4358=( struct
Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp4358[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp4359; _temp4359.tag=
Cyc_ParamDeclListBool_tok_tag; _temp4359.f1=({ struct _tuple16* _temp4360=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp4360->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4361= yyvs; struct _xtunion_struct** _temp4363=
_temp4361.curr + yyvsp_offset; if( _temp4361.base == 0? 1:( _temp4363 <
_temp4361.base? 1: _temp4363 >= _temp4361.last_plus_one)){ _throw(
Null_Exception);}* _temp4363;}))); _temp4360->f2= 0; _temp4360->f3= 0; _temp4360;});
_temp4359;}); _temp4358;}); break; case 152: _LL4357: yyval=( struct
_xtunion_struct*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp4365=( struct
Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp4365[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp4366; _temp4366.tag=
Cyc_ParamDeclListBool_tok_tag; _temp4366.f1=({ struct _tuple16* _temp4367=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp4367->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4368= yyvs; struct _xtunion_struct** _temp4370=
_temp4368.curr +( yyvsp_offset - 2); if( _temp4368.base == 0? 1:( _temp4370 <
_temp4368.base? 1: _temp4370 >= _temp4368.last_plus_one)){ _throw(
Null_Exception);}* _temp4370;}))); _temp4367->f2= 1; _temp4367->f3= 0; _temp4367;});
_temp4366;}); _temp4365;}); break; case 153: _LL4364: yyval=( struct
_xtunion_struct*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp4372=( struct
Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp4372[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp4373; _temp4373.tag=
Cyc_ParamDeclListBool_tok_tag; _temp4373.f1=({ struct _tuple16* _temp4374=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp4374->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4381= yyvs; struct _xtunion_struct** _temp4383=
_temp4381.curr +( yyvsp_offset - 2); if( _temp4381.base == 0? 1:( _temp4383 <
_temp4381.base? 1: _temp4383 >= _temp4381.last_plus_one)){ _throw(
Null_Exception);}* _temp4383;}))); _temp4374->f2= 0; _temp4374->f3=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp4375=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp4375->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp4376=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4376[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4377; _temp4377.tag= Cyc_Absyn_JoinEff_tag;
_temp4377.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4378= yyvs;
struct _xtunion_struct** _temp4380= _temp4378.curr + yyvsp_offset; if( _temp4378.base
== 0? 1:( _temp4380 < _temp4378.base? 1: _temp4380 >= _temp4378.last_plus_one)){
_throw( Null_Exception);}* _temp4380;})); _temp4377;}); _temp4376;})); _temp4375;});
_temp4374;}); _temp4373;}); _temp4372;}); break; case 154: _LL4371: yyval=({
struct _tagged_ptr2 _temp4385= yyvs; struct _xtunion_struct** _temp4387=
_temp4385.curr + yyvsp_offset; if( _temp4385.base == 0? 1:( _temp4387 <
_temp4385.base? 1: _temp4387 >= _temp4385.last_plus_one)){ _throw(
Null_Exception);}* _temp4387;}); break; case 155: _LL4384: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4389=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4389[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4390; _temp4390.tag= Cyc_TypeList_tok_tag; _temp4390.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4391= yyvs; struct _xtunion_struct** _temp4393=
_temp4391.curr +( yyvsp_offset - 2); if( _temp4391.base == 0? 1:( _temp4393 <
_temp4391.base? 1: _temp4393 >= _temp4391.last_plus_one)){ _throw(
Null_Exception);}* _temp4393;})), Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2
_temp4394= yyvs; struct _xtunion_struct** _temp4396= _temp4394.curr +
yyvsp_offset; if( _temp4394.base == 0? 1:( _temp4396 < _temp4394.base? 1:
_temp4396 >= _temp4394.last_plus_one)){ _throw( Null_Exception);}* _temp4396;})));
_temp4390;}); _temp4389;}); break; case 156: _LL4388: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4398=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4398[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4399; _temp4399.tag= Cyc_TypeList_tok_tag; _temp4399.f1= 0; _temp4399;});
_temp4398;}); break; case 157: _LL4397: yyval=({ struct _tagged_ptr2 _temp4401=
yyvs; struct _xtunion_struct** _temp4403= _temp4401.curr +( yyvsp_offset - 1);
if( _temp4401.base == 0? 1:( _temp4403 < _temp4401.base? 1: _temp4403 >=
_temp4401.last_plus_one)){ _throw( Null_Exception);}* _temp4403;}); break; case
158: _LL4400: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct*
_temp4405=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4405[ 0]=({ struct Cyc_TypeList_tok_struct _temp4406; _temp4406.tag= Cyc_TypeList_tok_tag;
_temp4406.f1=({ struct Cyc_List_List* _temp4407=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4407->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4408= yyvs; struct
_xtunion_struct** _temp4410= _temp4408.curr + yyvsp_offset; if( _temp4408.base
== 0? 1:( _temp4410 < _temp4408.base? 1: _temp4410 >= _temp4408.last_plus_one)){
_throw( Null_Exception);}* _temp4410;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp4407->tl= 0; _temp4407;});
_temp4406;}); _temp4405;}); break; case 159: _LL4404: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp4412= yyvs; struct _xtunion_struct** _temp4414=
_temp4412.curr + yyvsp_offset; if( _temp4412.base == 0? 1:( _temp4414 <
_temp4412.base? 1: _temp4414 >= _temp4412.last_plus_one)){ _throw(
Null_Exception);}* _temp4414;})) !=( void*) Cyc_Absyn_EffKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4415=( char*)"expecing effect kind (E)";
struct _tagged_string _temp4416; _temp4416.curr= _temp4415; _temp4416.base=
_temp4415; _temp4416.last_plus_one= _temp4415 + 25; _temp4416;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4417= yyls; struct Cyc_Yyltype* _temp4419= _temp4417.curr
+ yylsp_offset; if( _temp4417.base == 0? 1:( _temp4419 < _temp4417.base? 1:
_temp4419 >= _temp4417.last_plus_one)){ _throw( Null_Exception);}* _temp4419;}).first_line,({
struct _tagged_ptr3 _temp4420= yyls; struct Cyc_Yyltype* _temp4422= _temp4420.curr
+ yylsp_offset; if( _temp4420.base == 0? 1:( _temp4422 < _temp4420.base? 1:
_temp4422 >= _temp4420.last_plus_one)){ _throw( Null_Exception);}* _temp4422;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4423=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4423[ 0]=({ struct Cyc_TypeList_tok_struct _temp4424; _temp4424.tag= Cyc_TypeList_tok_tag;
_temp4424.f1=({ struct Cyc_List_List* _temp4425=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4425->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4426= yyvs; struct
_xtunion_struct** _temp4428= _temp4426.curr +( yyvsp_offset - 2); if( _temp4426.base
== 0? 1:( _temp4428 < _temp4426.base? 1: _temp4428 >= _temp4426.last_plus_one)){
_throw( Null_Exception);}* _temp4428;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp4425->tl= 0; _temp4425;});
_temp4424;}); _temp4423;}); break; case 160: _LL4411: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4430=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4430[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4431; _temp4431.tag= Cyc_TypeList_tok_tag; _temp4431.f1=({ struct Cyc_List_List*
_temp4432=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4432->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4433=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4433[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4434; _temp4434.tag= Cyc_Absyn_AccessEff_tag;
_temp4434.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4435= yyvs; struct _xtunion_struct** _temp4437= _temp4435.curr
+ yyvsp_offset; if( _temp4435.base == 0? 1:( _temp4437 < _temp4435.base? 1:
_temp4437 >= _temp4435.last_plus_one)){ _throw( Null_Exception);}* _temp4437;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4434;}); _temp4433;})); _temp4432->tl= 0; _temp4432;}); _temp4431;});
_temp4430;}); break; case 161: _LL4429: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeList_tok_struct* _temp4439=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4439[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4440; _temp4440.tag= Cyc_TypeList_tok_tag; _temp4440.f1=({ struct Cyc_List_List*
_temp4441=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4441->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4445=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4445[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4446; _temp4446.tag= Cyc_Absyn_AccessEff_tag;
_temp4446.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4447= yyvs; struct _xtunion_struct** _temp4449= _temp4447.curr
+( yyvsp_offset - 2); if( _temp4447.base == 0? 1:( _temp4449 < _temp4447.base? 1:
_temp4449 >= _temp4447.last_plus_one)){ _throw( Null_Exception);}* _temp4449;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4446;}); _temp4445;})); _temp4441->tl= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4442= yyvs; struct _xtunion_struct** _temp4444= _temp4442.curr
+ yyvsp_offset; if( _temp4442.base == 0? 1:( _temp4444 < _temp4442.base? 1:
_temp4444 >= _temp4442.last_plus_one)){ _throw( Null_Exception);}* _temp4444;}));
_temp4441;}); _temp4440;}); _temp4439;}); break; case 162: _LL4438: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp4451= yyvs; struct _xtunion_struct** _temp4453=
_temp4451.curr + yyvsp_offset; if( _temp4451.base == 0? 1:( _temp4453 <
_temp4451.base? 1: _temp4453 >= _temp4451.last_plus_one)){ _throw(
Null_Exception);}* _temp4453;})) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4454=( char*)"expecting region kind (R)";
struct _tagged_string _temp4455; _temp4455.curr= _temp4454; _temp4455.base=
_temp4454; _temp4455.last_plus_one= _temp4454 + 26; _temp4455;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4456= yyls; struct Cyc_Yyltype* _temp4458= _temp4456.curr
+ yylsp_offset; if( _temp4456.base == 0? 1:( _temp4458 < _temp4456.base? 1:
_temp4458 >= _temp4456.last_plus_one)){ _throw( Null_Exception);}* _temp4458;}).first_line,({
struct _tagged_ptr3 _temp4459= yyls; struct Cyc_Yyltype* _temp4461= _temp4459.curr
+ yylsp_offset; if( _temp4459.base == 0? 1:( _temp4461 < _temp4459.base? 1:
_temp4461 >= _temp4459.last_plus_one)){ _throw( Null_Exception);}* _temp4461;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4462=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4462[ 0]=({ struct Cyc_TypeList_tok_struct _temp4463; _temp4463.tag= Cyc_TypeList_tok_tag;
_temp4463.f1=({ struct Cyc_List_List* _temp4464=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4464->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp4465=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp4465[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp4466; _temp4466.tag= Cyc_Absyn_AccessEff_tag;
_temp4466.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4467= yyvs; struct _xtunion_struct** _temp4469= _temp4467.curr
+( yyvsp_offset - 2); if( _temp4467.base == 0? 1:( _temp4469 < _temp4467.base? 1:
_temp4469 >= _temp4467.last_plus_one)){ _throw( Null_Exception);}* _temp4469;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4466;}); _temp4465;})); _temp4464->tl= 0; _temp4464;}); _temp4463;});
_temp4462;}); break; case 163: _LL4450: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp4471= yyvs; struct _xtunion_struct** _temp4473= _temp4471.curr
+( yyvsp_offset - 2); if( _temp4471.base == 0? 1:( _temp4473 < _temp4471.base? 1:
_temp4473 >= _temp4471.last_plus_one)){ _throw( Null_Exception);}* _temp4473;}))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp4474=( char*)"expecting region kind (R)"; struct _tagged_string _temp4475;
_temp4475.curr= _temp4474; _temp4475.base= _temp4474; _temp4475.last_plus_one=
_temp4474 + 26; _temp4475;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4476= yyls; struct Cyc_Yyltype* _temp4478= _temp4476.curr +( yylsp_offset -
2); if( _temp4476.base == 0? 1:( _temp4478 < _temp4476.base? 1: _temp4478 >=
_temp4476.last_plus_one)){ _throw( Null_Exception);}* _temp4478;}).first_line,({
struct _tagged_ptr3 _temp4479= yyls; struct Cyc_Yyltype* _temp4481= _temp4479.curr
+( yylsp_offset - 2); if( _temp4479.base == 0? 1:( _temp4481 < _temp4479.base? 1:
_temp4481 >= _temp4479.last_plus_one)){ _throw( Null_Exception);}* _temp4481;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4482=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4482[ 0]=({ struct Cyc_TypeList_tok_struct _temp4483; _temp4483.tag= Cyc_TypeList_tok_tag;
_temp4483.f1=({ struct Cyc_List_List* _temp4484=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4484->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp4488=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp4488[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp4489; _temp4489.tag= Cyc_Absyn_AccessEff_tag;
_temp4489.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4490= yyvs; struct _xtunion_struct** _temp4492= _temp4490.curr
+( yyvsp_offset - 4); if( _temp4490.base == 0? 1:( _temp4492 < _temp4490.base? 1:
_temp4492 >= _temp4490.last_plus_one)){ _throw( Null_Exception);}* _temp4492;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4489;}); _temp4488;})); _temp4484->tl= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4485= yyvs; struct _xtunion_struct** _temp4487= _temp4485.curr
+ yyvsp_offset; if( _temp4485.base == 0? 1:( _temp4487 < _temp4485.base? 1:
_temp4487 >= _temp4485.last_plus_one)){ _throw( Null_Exception);}* _temp4487;}));
_temp4484;}); _temp4483;}); _temp4482;}); break; case 164: _LL4470: yyval=(
struct _xtunion_struct*)({ struct Cyc_ParamDeclList_tok_struct* _temp4494=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp4494[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp4495; _temp4495.tag=
Cyc_ParamDeclList_tok_tag; _temp4495.f1=({ struct Cyc_List_List* _temp4496=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4496->hd=(
void*) Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4497= yyvs; struct
_xtunion_struct** _temp4499= _temp4497.curr + yyvsp_offset; if( _temp4497.base
== 0? 1:( _temp4499 < _temp4497.base? 1: _temp4499 >= _temp4497.last_plus_one)){
_throw( Null_Exception);}* _temp4499;})); _temp4496->tl= 0; _temp4496;});
_temp4495;}); _temp4494;}); break; case 165: _LL4493: yyval=( struct
_xtunion_struct*)({ struct Cyc_ParamDeclList_tok_struct* _temp4501=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp4501[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp4502; _temp4502.tag= Cyc_ParamDeclList_tok_tag;
_temp4502.f1=({ struct Cyc_List_List* _temp4503=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4503->hd=( void*) Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp4507= yyvs; struct _xtunion_struct** _temp4509=
_temp4507.curr + yyvsp_offset; if( _temp4507.base == 0? 1:( _temp4509 <
_temp4507.base? 1: _temp4509 >= _temp4507.last_plus_one)){ _throw(
Null_Exception);}* _temp4509;})); _temp4503->tl= Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4504= yyvs; struct _xtunion_struct** _temp4506=
_temp4504.curr +( yyvsp_offset - 2); if( _temp4504.base == 0? 1:( _temp4506 <
_temp4504.base? 1: _temp4506 >= _temp4504.last_plus_one)){ _throw(
Null_Exception);}* _temp4506;})); _temp4503;}); _temp4502;}); _temp4501;});
break; case 166: _LL4500: { void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4511= yyvs; struct _xtunion_struct** _temp4513=
_temp4511.curr +( yyvsp_offset - 1); if( _temp4511.base == 0? 1:( _temp4513 <
_temp4511.base? 1: _temp4513 >= _temp4511.last_plus_one)){ _throw(
Null_Exception);}* _temp4513;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4514= yyls; struct Cyc_Yyltype* _temp4516= _temp4514.curr +(
yylsp_offset - 1); if( _temp4514.base == 0? 1:( _temp4516 < _temp4514.base? 1:
_temp4516 >= _temp4514.last_plus_one)){ _throw( Null_Exception);}* _temp4516;}).first_line,({
struct _tagged_ptr3 _temp4517= yyls; struct Cyc_Yyltype* _temp4519= _temp4517.curr
+( yylsp_offset - 1); if( _temp4517.base == 0? 1:( _temp4519 < _temp4517.base? 1:
_temp4519 >= _temp4517.last_plus_one)){ _throw( Null_Exception);}* _temp4519;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4520= yyvs; struct _xtunion_struct** _temp4522= _temp4520.curr +(
yyvsp_offset - 1); if( _temp4520.base == 0? 1:( _temp4522 < _temp4520.base? 1:
_temp4522 >= _temp4520.last_plus_one)){ _throw( Null_Exception);}* _temp4522;}))).f3;
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4523= yyvs; struct _xtunion_struct** _temp4525= _temp4523.curr +(
yyvsp_offset - 1); if( _temp4523.base == 0? 1:( _temp4525 < _temp4523.base? 1:
_temp4525 >= _temp4523.last_plus_one)){ _throw( Null_Exception);}* _temp4525;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp4526= yyvs; struct _xtunion_struct** _temp4528= _temp4526.curr +
yyvsp_offset; if( _temp4526.base == 0? 1:( _temp4528 < _temp4526.base? 1:
_temp4528 >= _temp4526.last_plus_one)){ _throw( Null_Exception);}* _temp4528;})))->tms;
struct _tuple6 t_info= Cyc_Parse_apply_tms( tq, t, atts, tms); if( t_info.f3 !=
0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp4529=( char*)"parameter with bad type params";
struct _tagged_string _temp4530; _temp4530.curr= _temp4529; _temp4530.base=
_temp4529; _temp4530.last_plus_one= _temp4529 + 31; _temp4530;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4531= yyls; struct Cyc_Yyltype* _temp4533= _temp4531.curr
+ yylsp_offset; if( _temp4531.base == 0? 1:( _temp4533 < _temp4531.base? 1:
_temp4533 >= _temp4531.last_plus_one)){ _throw( Null_Exception);}* _temp4533;}).first_line,({
struct _tagged_ptr3 _temp4534= yyls; struct Cyc_Yyltype* _temp4536= _temp4534.curr
+ yylsp_offset; if( _temp4534.base == 0? 1:( _temp4536 < _temp4534.base? 1:
_temp4536 >= _temp4534.last_plus_one)){ _throw( Null_Exception);}* _temp4536;}).last_line));}{
struct _tuple1* q=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp4537=
yyvs; struct _xtunion_struct** _temp4539= _temp4537.curr + yyvsp_offset; if(
_temp4537.base == 0? 1:( _temp4539 < _temp4537.base? 1: _temp4539 >= _temp4537.last_plus_one)){
_throw( Null_Exception);}* _temp4539;})))->id;{ void* _temp4540=(* q).f1; struct
Cyc_List_List* _temp4550; struct Cyc_List_List* _temp4552; _LL4542: if(( int)
_temp4540 == Cyc_Absyn_Loc_n){ goto _LL4543;} else{ goto _LL4544;} _LL4544: if((
unsigned int) _temp4540 > 1u?(( struct _tunion_struct*) _temp4540)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL4551: _temp4550=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp4540)->f1; if( _temp4550 == 0){ goto _LL4545;} else{ goto _LL4546;}} else{
goto _LL4546;} _LL4546: if(( unsigned int) _temp4540 > 1u?(( struct
_tunion_struct*) _temp4540)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL4553: _temp4552=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp4540)->f1; if(
_temp4552 == 0){ goto _LL4547;} else{ goto _LL4548;}} else{ goto _LL4548;}
_LL4548: goto _LL4549; _LL4543: goto _LL4541; _LL4545: goto _LL4541; _LL4547:
goto _LL4541; _LL4549: Cyc_Parse_err(( struct _tagged_string)({ char* _temp4554=(
char*)"parameter cannot be qualified with a module name"; struct _tagged_string
_temp4555; _temp4555.curr= _temp4554; _temp4555.base= _temp4554; _temp4555.last_plus_one=
_temp4554 + 49; _temp4555;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4556= yyls; struct Cyc_Yyltype* _temp4558= _temp4556.curr +( yylsp_offset -
1); if( _temp4556.base == 0? 1:( _temp4558 < _temp4556.base? 1: _temp4558 >=
_temp4556.last_plus_one)){ _throw( Null_Exception);}* _temp4558;}).first_line,({
struct _tagged_ptr3 _temp4559= yyls; struct Cyc_Yyltype* _temp4561= _temp4559.curr
+( yylsp_offset - 1); if( _temp4559.base == 0? 1:( _temp4561 < _temp4559.base? 1:
_temp4561 >= _temp4559.last_plus_one)){ _throw( Null_Exception);}* _temp4561;}).last_line));
goto _LL4541; _LL4541:;}{ struct Cyc_Core_Opt* idopt=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp4562=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp4562->v=( void*)(* q).f2; _temp4562;}); if( t_info.f4 != 0){
Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4563=( char*)"extra attributes on parameter, ignoring";
struct _tagged_string _temp4564; _temp4564.curr= _temp4563; _temp4564.base=
_temp4563; _temp4564.last_plus_one= _temp4563 + 40; _temp4564;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4565= yyls; struct Cyc_Yyltype* _temp4567= _temp4565.curr
+( yylsp_offset - 1); if( _temp4565.base == 0? 1:( _temp4567 < _temp4565.base? 1:
_temp4567 >= _temp4565.last_plus_one)){ _throw( Null_Exception);}* _temp4567;}).first_line,({
struct _tagged_ptr3 _temp4568= yyls; struct Cyc_Yyltype* _temp4570= _temp4568.curr
+ yylsp_offset; if( _temp4568.base == 0? 1:( _temp4570 < _temp4568.base? 1:
_temp4570 >= _temp4568.last_plus_one)){ _throw( Null_Exception);}* _temp4570;}).last_line));}
if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4571=(
char*)"extra type variables on parameter, ignoring"; struct _tagged_string
_temp4572; _temp4572.curr= _temp4571; _temp4572.base= _temp4571; _temp4572.last_plus_one=
_temp4571 + 44; _temp4572;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4573= yyls; struct Cyc_Yyltype* _temp4575= _temp4573.curr +( yylsp_offset -
1); if( _temp4573.base == 0? 1:( _temp4575 < _temp4573.base? 1: _temp4575 >=
_temp4573.last_plus_one)){ _throw( Null_Exception);}* _temp4575;}).first_line,({
struct _tagged_ptr3 _temp4576= yyls; struct Cyc_Yyltype* _temp4578= _temp4576.curr
+ yylsp_offset; if( _temp4576.base == 0? 1:( _temp4578 < _temp4576.base? 1:
_temp4578 >= _temp4576.last_plus_one)){ _throw( Null_Exception);}* _temp4578;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4579=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4579[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4580; _temp4580.tag= Cyc_ParamDecl_tok_tag;
_temp4580.f1=({ struct _tuple2* _temp4581=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4581->f1= idopt; _temp4581->f2= t_info.f1; _temp4581->f3=
t_info.f2; _temp4581;}); _temp4580;}); _temp4579;}); break;}}} case 167: _LL4510: {
void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp4583= yyvs; struct _xtunion_struct** _temp4585= _temp4583.curr
+ yyvsp_offset; if( _temp4583.base == 0? 1:( _temp4585 < _temp4583.base? 1:
_temp4585 >= _temp4583.last_plus_one)){ _throw( Null_Exception);}* _temp4585;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4586= yyls; struct Cyc_Yyltype*
_temp4588= _temp4586.curr + yylsp_offset; if( _temp4586.base == 0? 1:( _temp4588
< _temp4586.base? 1: _temp4588 >= _temp4586.last_plus_one)){ _throw(
Null_Exception);}* _temp4588;}).first_line,({ struct _tagged_ptr3 _temp4589=
yyls; struct Cyc_Yyltype* _temp4591= _temp4589.curr + yylsp_offset; if(
_temp4589.base == 0? 1:( _temp4591 < _temp4589.base? 1: _temp4591 >= _temp4589.last_plus_one)){
_throw( Null_Exception);}* _temp4591;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4592= yyvs; struct
_xtunion_struct** _temp4594= _temp4592.curr + yyvsp_offset; if( _temp4592.base
== 0? 1:( _temp4594 < _temp4592.base? 1: _temp4594 >= _temp4592.last_plus_one)){
_throw( Null_Exception);}* _temp4594;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4595= yyvs; struct _xtunion_struct** _temp4597=
_temp4595.curr + yyvsp_offset; if( _temp4595.base == 0? 1:( _temp4597 <
_temp4595.base? 1: _temp4597 >= _temp4595.last_plus_one)){ _throw(
Null_Exception);}* _temp4597;}))).f1; if( atts != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp4598=( char*)"bad attributes on parameter, ignoring";
struct _tagged_string _temp4599; _temp4599.curr= _temp4598; _temp4599.base=
_temp4598; _temp4599.last_plus_one= _temp4598 + 38; _temp4599;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4600= yyls; struct Cyc_Yyltype* _temp4602= _temp4600.curr
+ yylsp_offset; if( _temp4600.base == 0? 1:( _temp4602 < _temp4600.base? 1:
_temp4602 >= _temp4600.last_plus_one)){ _throw( Null_Exception);}* _temp4602;}).first_line,({
struct _tagged_ptr3 _temp4603= yyls; struct Cyc_Yyltype* _temp4605= _temp4603.curr
+ yylsp_offset; if( _temp4603.base == 0? 1:( _temp4605 < _temp4603.base? 1:
_temp4605 >= _temp4603.last_plus_one)){ _throw( Null_Exception);}* _temp4605;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4606=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4606[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4607; _temp4607.tag= Cyc_ParamDecl_tok_tag;
_temp4607.f1=({ struct _tuple2* _temp4608=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4608->f1= 0; _temp4608->f2= tq; _temp4608->f3= t;
_temp4608;}); _temp4607;}); _temp4606;}); break;} case 168: _LL4582: { void* t=
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4610= yyvs; struct _xtunion_struct** _temp4612= _temp4610.curr +(
yyvsp_offset - 1); if( _temp4610.base == 0? 1:( _temp4612 < _temp4610.base? 1:
_temp4612 >= _temp4610.last_plus_one)){ _throw( Null_Exception);}* _temp4612;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4613= yyls; struct Cyc_Yyltype*
_temp4615= _temp4613.curr +( yylsp_offset - 1); if( _temp4613.base == 0? 1:(
_temp4615 < _temp4613.base? 1: _temp4615 >= _temp4613.last_plus_one)){ _throw(
Null_Exception);}* _temp4615;}).first_line,({ struct _tagged_ptr3 _temp4616=
yyls; struct Cyc_Yyltype* _temp4618= _temp4616.curr +( yylsp_offset - 1); if(
_temp4616.base == 0? 1:( _temp4618 < _temp4616.base? 1: _temp4618 >= _temp4616.last_plus_one)){
_throw( Null_Exception);}* _temp4618;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4619= yyvs; struct
_xtunion_struct** _temp4621= _temp4619.curr +( yyvsp_offset - 1); if( _temp4619.base
== 0? 1:( _temp4621 < _temp4619.base? 1: _temp4621 >= _temp4619.last_plus_one)){
_throw( Null_Exception);}* _temp4621;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4622= yyvs; struct _xtunion_struct** _temp4624=
_temp4622.curr +( yyvsp_offset - 1); if( _temp4622.base == 0? 1:( _temp4624 <
_temp4622.base? 1: _temp4624 >= _temp4622.last_plus_one)){ _throw(
Null_Exception);}* _temp4624;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4625= yyvs; struct _xtunion_struct** _temp4627=
_temp4625.curr + yyvsp_offset; if( _temp4625.base == 0? 1:( _temp4627 <
_temp4625.base? 1: _temp4627 >= _temp4625.last_plus_one)){ _throw(
Null_Exception);}* _temp4627;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp4628=( char*)"bad type parameters on formal argument, ignoring";
struct _tagged_string _temp4629; _temp4629.curr= _temp4628; _temp4629.base=
_temp4628; _temp4629.last_plus_one= _temp4628 + 49; _temp4629;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4630= yyls; struct Cyc_Yyltype* _temp4632= _temp4630.curr
+( yylsp_offset - 1); if( _temp4630.base == 0? 1:( _temp4632 < _temp4630.base? 1:
_temp4632 >= _temp4630.last_plus_one)){ _throw( Null_Exception);}* _temp4632;}).first_line,({
struct _tagged_ptr3 _temp4633= yyls; struct Cyc_Yyltype* _temp4635= _temp4633.curr
+ yylsp_offset; if( _temp4633.base == 0? 1:( _temp4635 < _temp4633.base? 1:
_temp4635 >= _temp4633.last_plus_one)){ _throw( Null_Exception);}* _temp4635;}).last_line));}
if( t_info.f4 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4636=(
char*)"bad attributes on parameter, ignoring"; struct _tagged_string _temp4637;
_temp4637.curr= _temp4636; _temp4637.base= _temp4636; _temp4637.last_plus_one=
_temp4636 + 38; _temp4637;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4638= yyls; struct Cyc_Yyltype* _temp4640= _temp4638.curr +( yylsp_offset -
1); if( _temp4638.base == 0? 1:( _temp4640 < _temp4638.base? 1: _temp4640 >=
_temp4638.last_plus_one)){ _throw( Null_Exception);}* _temp4640;}).first_line,({
struct _tagged_ptr3 _temp4641= yyls; struct Cyc_Yyltype* _temp4643= _temp4641.curr
+ yylsp_offset; if( _temp4641.base == 0? 1:( _temp4643 < _temp4641.base? 1:
_temp4643 >= _temp4641.last_plus_one)){ _throw( Null_Exception);}* _temp4643;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4644=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4644[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4645; _temp4645.tag= Cyc_ParamDecl_tok_tag;
_temp4645.f1=({ struct _tuple2* _temp4646=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4646->f1= 0; _temp4646->f2= t_info.f1; _temp4646->f3=
t_info.f2; _temp4646;}); _temp4645;}); _temp4644;}); break;} case 169: _LL4609:
yyval=( struct _xtunion_struct*)({ struct Cyc_IdList_tok_struct* _temp4648=(
struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4648[ 0]=({ struct Cyc_IdList_tok_struct _temp4649; _temp4649.tag= Cyc_IdList_tok_tag;
_temp4649.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp4650= yyvs; struct
_xtunion_struct** _temp4652= _temp4650.curr + yyvsp_offset; if( _temp4650.base
== 0? 1:( _temp4652 < _temp4650.base? 1: _temp4652 >= _temp4650.last_plus_one)){
_throw( Null_Exception);}* _temp4652;}))); _temp4649;}); _temp4648;}); break;
case 170: _LL4647: yyval=( struct _xtunion_struct*)({ struct Cyc_IdList_tok_struct*
_temp4654=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4654[ 0]=({ struct Cyc_IdList_tok_struct _temp4655; _temp4655.tag= Cyc_IdList_tok_tag;
_temp4655.f1=({ struct Cyc_List_List* _temp4656=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4656->hd=( void*)({ struct
_tagged_string* _temp4657=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4657[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp4658= yyvs; struct _xtunion_struct** _temp4660= _temp4658.curr +
yyvsp_offset; if( _temp4658.base == 0? 1:( _temp4660 < _temp4658.base? 1:
_temp4660 >= _temp4658.last_plus_one)){ _throw( Null_Exception);}* _temp4660;}));
_temp4657;}); _temp4656->tl= 0; _temp4656;}); _temp4655;}); _temp4654;}); break;
case 171: _LL4653: yyval=( struct _xtunion_struct*)({ struct Cyc_IdList_tok_struct*
_temp4662=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4662[ 0]=({ struct Cyc_IdList_tok_struct _temp4663; _temp4663.tag= Cyc_IdList_tok_tag;
_temp4663.f1=({ struct Cyc_List_List* _temp4664=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4664->hd=( void*)({ struct
_tagged_string* _temp4668=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4668[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp4669= yyvs; struct _xtunion_struct** _temp4671= _temp4669.curr +
yyvsp_offset; if( _temp4669.base == 0? 1:( _temp4671 < _temp4669.base? 1:
_temp4671 >= _temp4669.last_plus_one)){ _throw( Null_Exception);}* _temp4671;}));
_temp4668;}); _temp4664->tl= Cyc_yyget_IdList_tok(({ struct _tagged_ptr2
_temp4665= yyvs; struct _xtunion_struct** _temp4667= _temp4665.curr +(
yyvsp_offset - 2); if( _temp4665.base == 0? 1:( _temp4667 < _temp4665.base? 1:
_temp4667 >= _temp4665.last_plus_one)){ _throw( Null_Exception);}* _temp4667;}));
_temp4664;}); _temp4663;}); _temp4662;}); break; case 172: _LL4661: yyval=({
struct _tagged_ptr2 _temp4673= yyvs; struct _xtunion_struct** _temp4675=
_temp4673.curr + yyvsp_offset; if( _temp4673.base == 0? 1:( _temp4675 <
_temp4673.base? 1: _temp4675 >= _temp4673.last_plus_one)){ _throw(
Null_Exception);}* _temp4675;}); break; case 173: _LL4672: yyval=({ struct
_tagged_ptr2 _temp4677= yyvs; struct _xtunion_struct** _temp4679= _temp4677.curr
+ yyvsp_offset; if( _temp4677.base == 0? 1:( _temp4679 < _temp4677.base? 1:
_temp4679 >= _temp4677.last_plus_one)){ _throw( Null_Exception);}* _temp4679;});
break; case 174: _LL4676: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp4681=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4681[ 0]=({ struct Cyc_Exp_tok_struct _temp4682; _temp4682.tag= Cyc_Exp_tok_tag;
_temp4682.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp4683=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp4683[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp4684; _temp4684.tag= Cyc_Absyn_UnresolvedMem_e_tag; _temp4684.f1= 0;
_temp4684.f2= 0; _temp4684;}); _temp4683;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4685= yyls; struct Cyc_Yyltype* _temp4687= _temp4685.curr
+( yylsp_offset - 1); if( _temp4685.base == 0? 1:( _temp4687 < _temp4685.base? 1:
_temp4687 >= _temp4685.last_plus_one)){ _throw( Null_Exception);}* _temp4687;}).first_line,({
struct _tagged_ptr3 _temp4688= yyls; struct Cyc_Yyltype* _temp4690= _temp4688.curr
+ yylsp_offset; if( _temp4688.base == 0? 1:( _temp4690 < _temp4688.base? 1:
_temp4690 >= _temp4688.last_plus_one)){ _throw( Null_Exception);}* _temp4690;}).last_line));
_temp4682;}); _temp4681;}); break; case 175: _LL4680: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp4692=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4692[ 0]=({ struct Cyc_Exp_tok_struct
_temp4693; _temp4693.tag= Cyc_Exp_tok_tag; _temp4693.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4694=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4694[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4695; _temp4695.tag= Cyc_Absyn_UnresolvedMem_e_tag;
_temp4695.f1= 0; _temp4695.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp4696= yyvs; struct _xtunion_struct** _temp4698= _temp4696.curr +(
yyvsp_offset - 1); if( _temp4696.base == 0? 1:( _temp4698 < _temp4696.base? 1:
_temp4698 >= _temp4696.last_plus_one)){ _throw( Null_Exception);}* _temp4698;})));
_temp4695;}); _temp4694;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4699= yyls; struct Cyc_Yyltype* _temp4701= _temp4699.curr +( yylsp_offset -
2); if( _temp4699.base == 0? 1:( _temp4701 < _temp4699.base? 1: _temp4701 >=
_temp4699.last_plus_one)){ _throw( Null_Exception);}* _temp4701;}).first_line,({
struct _tagged_ptr3 _temp4702= yyls; struct Cyc_Yyltype* _temp4704= _temp4702.curr
+ yylsp_offset; if( _temp4702.base == 0? 1:( _temp4704 < _temp4702.base? 1:
_temp4704 >= _temp4702.last_plus_one)){ _throw( Null_Exception);}* _temp4704;}).last_line));
_temp4693;}); _temp4692;}); break; case 176: _LL4691: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp4706=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4706[ 0]=({ struct Cyc_Exp_tok_struct
_temp4707; _temp4707.tag= Cyc_Exp_tok_tag; _temp4707.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4708=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4708[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4709; _temp4709.tag= Cyc_Absyn_UnresolvedMem_e_tag;
_temp4709.f1= 0; _temp4709.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp4710= yyvs; struct _xtunion_struct** _temp4712= _temp4710.curr +(
yyvsp_offset - 2); if( _temp4710.base == 0? 1:( _temp4712 < _temp4710.base? 1:
_temp4712 >= _temp4710.last_plus_one)){ _throw( Null_Exception);}* _temp4712;})));
_temp4709;}); _temp4708;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4713= yyls; struct Cyc_Yyltype* _temp4715= _temp4713.curr +( yylsp_offset -
3); if( _temp4713.base == 0? 1:( _temp4715 < _temp4713.base? 1: _temp4715 >=
_temp4713.last_plus_one)){ _throw( Null_Exception);}* _temp4715;}).first_line,({
struct _tagged_ptr3 _temp4716= yyls; struct Cyc_Yyltype* _temp4718= _temp4716.curr
+ yylsp_offset; if( _temp4716.base == 0? 1:( _temp4718 < _temp4716.base? 1:
_temp4718 >= _temp4716.last_plus_one)){ _throw( Null_Exception);}* _temp4718;}).last_line));
_temp4707;}); _temp4706;}); break; case 177: _LL4705: { struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp4720=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp4720->f1=( void*) Cyc_Absyn_Loc_n;
_temp4720->f2=({ struct _tagged_string* _temp4721=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4721[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4722= yyvs; struct _xtunion_struct** _temp4724=
_temp4722.curr +( yyvsp_offset - 5); if( _temp4722.base == 0? 1:( _temp4724 <
_temp4722.base? 1: _temp4724 >= _temp4722.last_plus_one)){ _throw(
Null_Exception);}* _temp4724;})); _temp4721;}); _temp4720;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4725= yyls; struct Cyc_Yyltype* _temp4727= _temp4725.curr
+( yylsp_offset - 5); if( _temp4725.base == 0? 1:( _temp4727 < _temp4725.base? 1:
_temp4727 >= _temp4725.last_plus_one)){ _throw( Null_Exception);}* _temp4727;}).first_line,({
struct _tagged_ptr3 _temp4728= yyls; struct Cyc_Yyltype* _temp4730= _temp4728.curr
+( yylsp_offset - 5); if( _temp4728.base == 0? 1:( _temp4730 < _temp4728.base? 1:
_temp4730 >= _temp4728.last_plus_one)){ _throw( Null_Exception);}* _temp4730;}).last_line)));
vd->tq=({ struct Cyc_Absyn_Tqual* _temp4731=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp4731->q_const= 1;
_temp4731->q_volatile= 0; _temp4731->q_restrict= 1; _temp4731;}); yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp4732=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4732[ 0]=({ struct Cyc_Exp_tok_struct
_temp4733; _temp4733.tag= Cyc_Exp_tok_tag; _temp4733.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Comprehension_e_struct* _temp4734=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp4734[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp4735; _temp4735.tag= Cyc_Absyn_Comprehension_e_tag;
_temp4735.f1= vd; _temp4735.f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4736= yyvs; struct _xtunion_struct** _temp4738= _temp4736.curr +(
yyvsp_offset - 3); if( _temp4736.base == 0? 1:( _temp4738 < _temp4736.base? 1:
_temp4738 >= _temp4736.last_plus_one)){ _throw( Null_Exception);}* _temp4738;}));
_temp4735.f3= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4739= yyvs; struct
_xtunion_struct** _temp4741= _temp4739.curr +( yyvsp_offset - 1); if( _temp4739.base
== 0? 1:( _temp4741 < _temp4739.base? 1: _temp4741 >= _temp4739.last_plus_one)){
_throw( Null_Exception);}* _temp4741;})); _temp4735;}); _temp4734;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4742= yyls; struct Cyc_Yyltype* _temp4744= _temp4742.curr
+( yylsp_offset - 7); if( _temp4742.base == 0? 1:( _temp4744 < _temp4742.base? 1:
_temp4744 >= _temp4742.last_plus_one)){ _throw( Null_Exception);}* _temp4744;}).first_line,({
struct _tagged_ptr3 _temp4745= yyls; struct Cyc_Yyltype* _temp4747= _temp4745.curr
+ yylsp_offset; if( _temp4745.base == 0? 1:( _temp4747 < _temp4745.base? 1:
_temp4747 >= _temp4745.last_plus_one)){ _throw( Null_Exception);}* _temp4747;}).last_line));
_temp4733;}); _temp4732;}); break;} case 178: _LL4719: yyval=( struct
_xtunion_struct*)({ struct Cyc_InitializerList_tok_struct* _temp4749=( struct
Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp4749[ 0]=({ struct Cyc_InitializerList_tok_struct _temp4750; _temp4750.tag=
Cyc_InitializerList_tok_tag; _temp4750.f1=({ struct Cyc_List_List* _temp4751=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4751->hd=(
void*)({ struct _tuple18* _temp4752=( struct _tuple18*) GC_malloc( sizeof(
struct _tuple18)); _temp4752->f1= 0; _temp4752->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4753= yyvs; struct _xtunion_struct** _temp4755= _temp4753.curr
+ yyvsp_offset; if( _temp4753.base == 0? 1:( _temp4755 < _temp4753.base? 1:
_temp4755 >= _temp4753.last_plus_one)){ _throw( Null_Exception);}* _temp4755;}));
_temp4752;}); _temp4751->tl= 0; _temp4751;}); _temp4750;}); _temp4749;}); break;
case 179: _LL4748: yyval=( struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct*
_temp4757=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4757[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4758; _temp4758.tag= Cyc_InitializerList_tok_tag; _temp4758.f1=({ struct
Cyc_List_List* _temp4759=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4759->hd=( void*)({ struct _tuple18* _temp4760=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4760->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4764= yyvs; struct _xtunion_struct** _temp4766=
_temp4764.curr +( yyvsp_offset - 1); if( _temp4764.base == 0? 1:( _temp4766 <
_temp4764.base? 1: _temp4766 >= _temp4764.last_plus_one)){ _throw(
Null_Exception);}* _temp4766;})); _temp4760->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4761= yyvs; struct _xtunion_struct** _temp4763= _temp4761.curr
+ yyvsp_offset; if( _temp4761.base == 0? 1:( _temp4763 < _temp4761.base? 1:
_temp4763 >= _temp4761.last_plus_one)){ _throw( Null_Exception);}* _temp4763;}));
_temp4760;}); _temp4759->tl= 0; _temp4759;}); _temp4758;}); _temp4757;}); break;
case 180: _LL4756: yyval=( struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct*
_temp4768=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4768[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4769; _temp4769.tag= Cyc_InitializerList_tok_tag; _temp4769.f1=({ struct
Cyc_List_List* _temp4770=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4770->hd=( void*)({ struct _tuple18* _temp4774=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4774->f1= 0; _temp4774->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4775= yyvs; struct _xtunion_struct** _temp4777=
_temp4775.curr + yyvsp_offset; if( _temp4775.base == 0? 1:( _temp4777 <
_temp4775.base? 1: _temp4777 >= _temp4775.last_plus_one)){ _throw(
Null_Exception);}* _temp4777;})); _temp4774;}); _temp4770->tl= Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4771= yyvs; struct _xtunion_struct** _temp4773=
_temp4771.curr +( yyvsp_offset - 2); if( _temp4771.base == 0? 1:( _temp4773 <
_temp4771.base? 1: _temp4773 >= _temp4771.last_plus_one)){ _throw(
Null_Exception);}* _temp4773;})); _temp4770;}); _temp4769;}); _temp4768;});
break; case 181: _LL4767: yyval=( struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct*
_temp4779=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4779[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4780; _temp4780.tag= Cyc_InitializerList_tok_tag; _temp4780.f1=({ struct
Cyc_List_List* _temp4781=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4781->hd=( void*)({ struct _tuple18* _temp4785=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4785->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4789= yyvs; struct _xtunion_struct** _temp4791=
_temp4789.curr +( yyvsp_offset - 1); if( _temp4789.base == 0? 1:( _temp4791 <
_temp4789.base? 1: _temp4791 >= _temp4789.last_plus_one)){ _throw(
Null_Exception);}* _temp4791;})); _temp4785->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4786= yyvs; struct _xtunion_struct** _temp4788= _temp4786.curr
+ yyvsp_offset; if( _temp4786.base == 0? 1:( _temp4788 < _temp4786.base? 1:
_temp4788 >= _temp4786.last_plus_one)){ _throw( Null_Exception);}* _temp4788;}));
_temp4785;}); _temp4781->tl= Cyc_yyget_InitializerList_tok(({ struct
_tagged_ptr2 _temp4782= yyvs; struct _xtunion_struct** _temp4784= _temp4782.curr
+( yyvsp_offset - 3); if( _temp4782.base == 0? 1:( _temp4784 < _temp4782.base? 1:
_temp4784 >= _temp4782.last_plus_one)){ _throw( Null_Exception);}* _temp4784;}));
_temp4781;}); _temp4780;}); _temp4779;}); break; case 182: _LL4778: yyval=(
struct _xtunion_struct*)({ struct Cyc_DesignatorList_tok_struct* _temp4793=(
struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4793[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4794; _temp4794.tag=
Cyc_DesignatorList_tok_tag; _temp4794.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2
_temp4795= yyvs; struct _xtunion_struct** _temp4797= _temp4795.curr +(
yyvsp_offset - 1); if( _temp4795.base == 0? 1:( _temp4797 < _temp4795.base? 1:
_temp4797 >= _temp4795.last_plus_one)){ _throw( Null_Exception);}* _temp4797;})));
_temp4794;}); _temp4793;}); break; case 183: _LL4792: yyval=( struct
_xtunion_struct*)({ struct Cyc_DesignatorList_tok_struct* _temp4799=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp4799[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp4800; _temp4800.tag= Cyc_DesignatorList_tok_tag;
_temp4800.f1=({ struct Cyc_List_List* _temp4801=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4801->hd=( void*) Cyc_yyget_Designator_tok(({
struct _tagged_ptr2 _temp4802= yyvs; struct _xtunion_struct** _temp4804=
_temp4802.curr + yyvsp_offset; if( _temp4802.base == 0? 1:( _temp4804 <
_temp4802.base? 1: _temp4804 >= _temp4802.last_plus_one)){ _throw(
Null_Exception);}* _temp4804;})); _temp4801->tl= 0; _temp4801;}); _temp4800;});
_temp4799;}); break; case 184: _LL4798: yyval=( struct _xtunion_struct*)({
struct Cyc_DesignatorList_tok_struct* _temp4806=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp4806[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp4807; _temp4807.tag= Cyc_DesignatorList_tok_tag;
_temp4807.f1=({ struct Cyc_List_List* _temp4808=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4808->hd=( void*) Cyc_yyget_Designator_tok(({
struct _tagged_ptr2 _temp4812= yyvs; struct _xtunion_struct** _temp4814=
_temp4812.curr + yyvsp_offset; if( _temp4812.base == 0? 1:( _temp4814 <
_temp4812.base? 1: _temp4814 >= _temp4812.last_plus_one)){ _throw(
Null_Exception);}* _temp4814;})); _temp4808->tl= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4809= yyvs; struct _xtunion_struct** _temp4811=
_temp4809.curr +( yyvsp_offset - 1); if( _temp4809.base == 0? 1:( _temp4811 <
_temp4809.base? 1: _temp4811 >= _temp4809.last_plus_one)){ _throw(
Null_Exception);}* _temp4811;})); _temp4808;}); _temp4807;}); _temp4806;});
break; case 185: _LL4805: yyval=( struct _xtunion_struct*)({ struct Cyc_Designator_tok_struct*
_temp4816=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4816[ 0]=({ struct Cyc_Designator_tok_struct _temp4817; _temp4817.tag= Cyc_Designator_tok_tag;
_temp4817.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp4818=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp4818[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp4819; _temp4819.tag=
Cyc_Absyn_ArrayElement_tag; _temp4819.f1= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4820= yyvs; struct _xtunion_struct** _temp4822= _temp4820.curr
+( yyvsp_offset - 1); if( _temp4820.base == 0? 1:( _temp4822 < _temp4820.base? 1:
_temp4822 >= _temp4820.last_plus_one)){ _throw( Null_Exception);}* _temp4822;}));
_temp4819;}); _temp4818;})); _temp4817;}); _temp4816;}); break; case 186:
_LL4815: yyval=( struct _xtunion_struct*)({ struct Cyc_Designator_tok_struct*
_temp4824=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4824[ 0]=({ struct Cyc_Designator_tok_struct _temp4825; _temp4825.tag= Cyc_Designator_tok_tag;
_temp4825.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp4826=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp4826[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp4827; _temp4827.tag= Cyc_Absyn_FieldName_tag;
_temp4827.f1=({ struct _tagged_string* _temp4828=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4828[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4829= yyvs; struct _xtunion_struct** _temp4831=
_temp4829.curr + yyvsp_offset; if( _temp4829.base == 0? 1:( _temp4831 <
_temp4829.base? 1: _temp4831 >= _temp4829.last_plus_one)){ _throw(
Null_Exception);}* _temp4831;})); _temp4828;}); _temp4827;}); _temp4826;}));
_temp4825;}); _temp4824;}); break; case 187: _LL4823: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4833= yyvs; struct
_xtunion_struct** _temp4835= _temp4833.curr + yyvsp_offset; if( _temp4833.base
== 0? 1:( _temp4835 < _temp4833.base? 1: _temp4835 >= _temp4833.last_plus_one)){
_throw( Null_Exception);}* _temp4835;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4836= yyls; struct Cyc_Yyltype* _temp4838= _temp4836.curr
+ yylsp_offset; if( _temp4836.base == 0? 1:( _temp4838 < _temp4836.base? 1:
_temp4838 >= _temp4836.last_plus_one)){ _throw( Null_Exception);}* _temp4838;}).first_line,({
struct _tagged_ptr3 _temp4839= yyls; struct Cyc_Yyltype* _temp4841= _temp4839.curr
+ yylsp_offset; if( _temp4839.base == 0? 1:( _temp4841 < _temp4839.base? 1:
_temp4841 >= _temp4839.last_plus_one)){ _throw( Null_Exception);}* _temp4841;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4842= yyvs; struct _xtunion_struct** _temp4844= _temp4842.curr +
yyvsp_offset; if( _temp4842.base == 0? 1:( _temp4844 < _temp4842.base? 1:
_temp4844 >= _temp4842.last_plus_one)){ _throw( Null_Exception);}* _temp4844;}))).f3;
if( atts != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4845=( char*)"ignoring attributes in type";
struct _tagged_string _temp4846; _temp4846.curr= _temp4845; _temp4846.base=
_temp4845; _temp4846.last_plus_one= _temp4845 + 28; _temp4846;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4847= yyls; struct Cyc_Yyltype* _temp4849= _temp4847.curr
+ yylsp_offset; if( _temp4847.base == 0? 1:( _temp4849 < _temp4847.base? 1:
_temp4849 >= _temp4847.last_plus_one)){ _throw( Null_Exception);}* _temp4849;}).first_line,({
struct _tagged_ptr3 _temp4850= yyls; struct Cyc_Yyltype* _temp4852= _temp4850.curr
+ yylsp_offset; if( _temp4850.base == 0? 1:( _temp4852 < _temp4850.base? 1:
_temp4852 >= _temp4850.last_plus_one)){ _throw( Null_Exception);}* _temp4852;}).last_line));}{
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4853= yyvs; struct _xtunion_struct** _temp4855= _temp4853.curr +
yyvsp_offset; if( _temp4853.base == 0? 1:( _temp4855 < _temp4853.base? 1:
_temp4855 >= _temp4853.last_plus_one)){ _throw( Null_Exception);}* _temp4855;}))).f1;
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4856=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4856[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4857; _temp4857.tag= Cyc_ParamDecl_tok_tag;
_temp4857.f1=({ struct _tuple2* _temp4858=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2) * 1); _temp4858[ 0]=({ struct _tuple2 _temp4859; _temp4859.f1= 0;
_temp4859.f2= tq; _temp4859.f3= t; _temp4859;}); _temp4858;}); _temp4857;});
_temp4856;}); break;}} case 188: _LL4832: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4861= yyvs; struct
_xtunion_struct** _temp4863= _temp4861.curr +( yyvsp_offset - 1); if( _temp4861.base
== 0? 1:( _temp4863 < _temp4861.base? 1: _temp4863 >= _temp4861.last_plus_one)){
_throw( Null_Exception);}* _temp4863;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4864= yyls; struct Cyc_Yyltype* _temp4866= _temp4864.curr
+( yylsp_offset - 1); if( _temp4864.base == 0? 1:( _temp4866 < _temp4864.base? 1:
_temp4866 >= _temp4864.last_plus_one)){ _throw( Null_Exception);}* _temp4866;}).first_line,({
struct _tagged_ptr3 _temp4867= yyls; struct Cyc_Yyltype* _temp4869= _temp4867.curr
+( yylsp_offset - 1); if( _temp4867.base == 0? 1:( _temp4869 < _temp4867.base? 1:
_temp4869 >= _temp4867.last_plus_one)){ _throw( Null_Exception);}* _temp4869;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4870= yyvs; struct _xtunion_struct** _temp4872= _temp4870.curr +(
yyvsp_offset - 1); if( _temp4870.base == 0? 1:( _temp4872 < _temp4870.base? 1:
_temp4872 >= _temp4870.last_plus_one)){ _throw( Null_Exception);}* _temp4872;}))).f3;
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4873= yyvs; struct _xtunion_struct** _temp4875= _temp4873.curr +(
yyvsp_offset - 1); if( _temp4873.base == 0? 1:( _temp4875 < _temp4873.base? 1:
_temp4875 >= _temp4873.last_plus_one)){ _throw( Null_Exception);}* _temp4875;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4876= yyvs; struct _xtunion_struct** _temp4878= _temp4876.curr
+ yyvsp_offset; if( _temp4876.base == 0? 1:( _temp4878 < _temp4876.base? 1:
_temp4878 >= _temp4876.last_plus_one)){ _throw( Null_Exception);}* _temp4878;})))->tms;
struct _tuple6 t_info= Cyc_Parse_apply_tms( tq, t, atts, tms); if( t_info.f3 !=
0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4879=( char*)"bad type params, ignoring";
struct _tagged_string _temp4880; _temp4880.curr= _temp4879; _temp4880.base=
_temp4879; _temp4880.last_plus_one= _temp4879 + 26; _temp4880;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4881= yyls; struct Cyc_Yyltype* _temp4883= _temp4881.curr
+ yylsp_offset; if( _temp4881.base == 0? 1:( _temp4883 < _temp4881.base? 1:
_temp4883 >= _temp4881.last_plus_one)){ _throw( Null_Exception);}* _temp4883;}).first_line,({
struct _tagged_ptr3 _temp4884= yyls; struct Cyc_Yyltype* _temp4886= _temp4884.curr
+ yylsp_offset; if( _temp4884.base == 0? 1:( _temp4886 < _temp4884.base? 1:
_temp4886 >= _temp4884.last_plus_one)){ _throw( Null_Exception);}* _temp4886;}).last_line));}
if( t_info.f4 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4887=(
char*)"bad specifiers, ignoring"; struct _tagged_string _temp4888; _temp4888.curr=
_temp4887; _temp4888.base= _temp4887; _temp4888.last_plus_one= _temp4887 + 25;
_temp4888;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4889= yyls;
struct Cyc_Yyltype* _temp4891= _temp4889.curr + yylsp_offset; if( _temp4889.base
== 0? 1:( _temp4891 < _temp4889.base? 1: _temp4891 >= _temp4889.last_plus_one)){
_throw( Null_Exception);}* _temp4891;}).first_line,({ struct _tagged_ptr3
_temp4892= yyls; struct Cyc_Yyltype* _temp4894= _temp4892.curr + yylsp_offset;
if( _temp4892.base == 0? 1:( _temp4894 < _temp4892.base? 1: _temp4894 >=
_temp4892.last_plus_one)){ _throw( Null_Exception);}* _temp4894;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4895=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4895[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4896; _temp4896.tag= Cyc_ParamDecl_tok_tag;
_temp4896.f1=({ struct _tuple2* _temp4897=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4897->f1= 0; _temp4897->f2= t_info.f1; _temp4897->f3=
t_info.f2; _temp4897;}); _temp4896;}); _temp4895;}); break;} case 189: _LL4860:
yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct* _temp4899=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4899[ 0]=({ struct Cyc_Type_tok_struct _temp4900; _temp4900.tag= Cyc_Type_tok_tag;
_temp4900.f1=( void*)(* Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4901=
yyvs; struct _xtunion_struct** _temp4903= _temp4901.curr + yyvsp_offset; if(
_temp4901.base == 0? 1:( _temp4903 < _temp4901.base? 1: _temp4903 >= _temp4901.last_plus_one)){
_throw( Null_Exception);}* _temp4903;}))).f3; _temp4900;}); _temp4899;}); break;
case 190: _LL4898: yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct*
_temp4905=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4905[ 0]=({ struct Cyc_Type_tok_struct _temp4906; _temp4906.tag= Cyc_Type_tok_tag;
_temp4906.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4907=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4907[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4908; _temp4908.tag= Cyc_Absyn_JoinEff_tag;
_temp4908.f1= 0; _temp4908;}); _temp4907;})); _temp4906;}); _temp4905;}); break;
case 191: _LL4904: yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct*
_temp4910=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4910[ 0]=({ struct Cyc_Type_tok_struct _temp4911; _temp4911.tag= Cyc_Type_tok_tag;
_temp4911.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4912=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4912[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4913; _temp4913.tag= Cyc_Absyn_JoinEff_tag;
_temp4913.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4914= yyvs;
struct _xtunion_struct** _temp4916= _temp4914.curr +( yyvsp_offset - 1); if(
_temp4914.base == 0? 1:( _temp4916 < _temp4914.base? 1: _temp4916 >= _temp4914.last_plus_one)){
_throw( Null_Exception);}* _temp4916;})); _temp4913;}); _temp4912;})); _temp4911;});
_temp4910;}); break; case 192: _LL4909: yyval=( struct _xtunion_struct*)({
struct Cyc_Type_tok_struct* _temp4918=( struct Cyc_Type_tok_struct*) GC_malloc(
sizeof( struct Cyc_Type_tok_struct)); _temp4918[ 0]=({ struct Cyc_Type_tok_struct
_temp4919; _temp4919.tag= Cyc_Type_tok_tag; _temp4919.f1=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp4920=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4920[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4921; _temp4921.tag= Cyc_Absyn_JoinEff_tag;
_temp4921.f1=({ struct Cyc_List_List* _temp4922=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4922->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4926= yyvs; struct _xtunion_struct** _temp4928=
_temp4926.curr +( yyvsp_offset - 2); if( _temp4926.base == 0? 1:( _temp4928 <
_temp4926.base? 1: _temp4928 >= _temp4926.last_plus_one)){ _throw(
Null_Exception);}* _temp4928;})); _temp4922->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4923= yyvs; struct _xtunion_struct** _temp4925=
_temp4923.curr + yyvsp_offset; if( _temp4923.base == 0? 1:( _temp4925 <
_temp4923.base? 1: _temp4925 >= _temp4923.last_plus_one)){ _throw(
Null_Exception);}* _temp4925;})); _temp4922;}); _temp4921;}); _temp4920;}));
_temp4919;}); _temp4918;}); break; case 193: _LL4917: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4930=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4930[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4931; _temp4931.tag= Cyc_TypeList_tok_tag; _temp4931.f1=({ struct Cyc_List_List*
_temp4932=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4932->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4933= yyvs;
struct _xtunion_struct** _temp4935= _temp4933.curr + yyvsp_offset; if( _temp4933.base
== 0? 1:( _temp4935 < _temp4933.base? 1: _temp4935 >= _temp4933.last_plus_one)){
_throw( Null_Exception);}* _temp4935;})); _temp4932->tl= 0; _temp4932;});
_temp4931;}); _temp4930;}); break; case 194: _LL4929: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4937=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4937[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4938; _temp4938.tag= Cyc_TypeList_tok_tag; _temp4938.f1=({ struct Cyc_List_List*
_temp4939=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4939->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4943= yyvs;
struct _xtunion_struct** _temp4945= _temp4943.curr + yyvsp_offset; if( _temp4943.base
== 0? 1:( _temp4945 < _temp4943.base? 1: _temp4945 >= _temp4943.last_plus_one)){
_throw( Null_Exception);}* _temp4945;})); _temp4939->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4940= yyvs; struct _xtunion_struct** _temp4942=
_temp4940.curr +( yyvsp_offset - 2); if( _temp4940.base == 0? 1:( _temp4942 <
_temp4940.base? 1: _temp4942 >= _temp4940.last_plus_one)){ _throw(
Null_Exception);}* _temp4942;})); _temp4939;}); _temp4938;}); _temp4937;});
break; case 195: _LL4936: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4947=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4947[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4948; _temp4948.tag= Cyc_AbstractDeclarator_tok_tag; _temp4948.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4949=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4949->tms= Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp4950= yyvs; struct _xtunion_struct** _temp4952=
_temp4950.curr + yyvsp_offset; if( _temp4950.base == 0? 1:( _temp4952 <
_temp4950.base? 1: _temp4952 >= _temp4950.last_plus_one)){ _throw(
Null_Exception);}* _temp4952;})); _temp4949;}); _temp4948;}); _temp4947;});
break; case 196: _LL4946: yyval=({ struct _tagged_ptr2 _temp4954= yyvs; struct
_xtunion_struct** _temp4956= _temp4954.curr + yyvsp_offset; if( _temp4954.base
== 0? 1:( _temp4956 < _temp4954.base? 1: _temp4956 >= _temp4954.last_plus_one)){
_throw( Null_Exception);}* _temp4956;}); break; case 197: _LL4953: yyval=(
struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4958=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4958[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4959; _temp4959.tag=
Cyc_AbstractDeclarator_tok_tag; _temp4959.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4960=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4960->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2
_temp4961= yyvs; struct _xtunion_struct** _temp4963= _temp4961.curr +(
yyvsp_offset - 1); if( _temp4961.base == 0? 1:( _temp4963 < _temp4961.base? 1:
_temp4963 >= _temp4961.last_plus_one)){ _throw( Null_Exception);}* _temp4963;})),(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4964= yyvs; struct
_xtunion_struct** _temp4966= _temp4964.curr + yyvsp_offset; if( _temp4964.base
== 0? 1:( _temp4966 < _temp4964.base? 1: _temp4966 >= _temp4964.last_plus_one)){
_throw( Null_Exception);}* _temp4966;})))->tms); _temp4960;}); _temp4959;});
_temp4958;}); break; case 198: _LL4957: yyval=({ struct _tagged_ptr2 _temp4968=
yyvs; struct _xtunion_struct** _temp4970= _temp4968.curr +( yyvsp_offset - 1);
if( _temp4968.base == 0? 1:( _temp4970 < _temp4968.base? 1: _temp4970 >=
_temp4968.last_plus_one)){ _throw( Null_Exception);}* _temp4970;}); break; case
199: _LL4967: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4972=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4972[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4973; _temp4973.tag= Cyc_AbstractDeclarator_tok_tag; _temp4973.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4974=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4974->tms=({
struct Cyc_List_List* _temp4975=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4975->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4975->tl= 0; _temp4975;}); _temp4974;}); _temp4973;}); _temp4972;}); break;
case 200: _LL4971: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4977=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4977[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4978; _temp4978.tag= Cyc_AbstractDeclarator_tok_tag; _temp4978.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4979=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4979->tms=({
struct Cyc_List_List* _temp4980=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4980->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4980->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4981= yyvs; struct _xtunion_struct** _temp4983= _temp4981.curr +(
yyvsp_offset - 2); if( _temp4981.base == 0? 1:( _temp4983 < _temp4981.base? 1:
_temp4983 >= _temp4981.last_plus_one)){ _throw( Null_Exception);}* _temp4983;})))->tms;
_temp4980;}); _temp4979;}); _temp4978;}); _temp4977;}); break; case 201: _LL4976:
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4985=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4985[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4986; _temp4986.tag= Cyc_AbstractDeclarator_tok_tag; _temp4986.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4987=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4987->tms=({
struct Cyc_List_List* _temp4988=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4988->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4989=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4989[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp4990; _temp4990.tag= Cyc_Absyn_ConstArray_mod_tag; _temp4990.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4991= yyvs; struct _xtunion_struct** _temp4993=
_temp4991.curr +( yyvsp_offset - 1); if( _temp4991.base == 0? 1:( _temp4993 <
_temp4991.base? 1: _temp4993 >= _temp4991.last_plus_one)){ _throw(
Null_Exception);}* _temp4993;})); _temp4990;}); _temp4989;})); _temp4988->tl= 0;
_temp4988;}); _temp4987;}); _temp4986;}); _temp4985;}); break; case 202: _LL4984:
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4995=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4995[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4996; _temp4996.tag= Cyc_AbstractDeclarator_tok_tag; _temp4996.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4997=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4997->tms=({
struct Cyc_List_List* _temp4998=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4998->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp5002=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp5002[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp5003; _temp5003.tag= Cyc_Absyn_ConstArray_mod_tag; _temp5003.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5004= yyvs; struct _xtunion_struct** _temp5006=
_temp5004.curr +( yyvsp_offset - 1); if( _temp5004.base == 0? 1:( _temp5006 <
_temp5004.base? 1: _temp5006 >= _temp5004.last_plus_one)){ _throw(
Null_Exception);}* _temp5006;})); _temp5003;}); _temp5002;})); _temp4998->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4999= yyvs; struct
_xtunion_struct** _temp5001= _temp4999.curr +( yyvsp_offset - 3); if( _temp4999.base
== 0? 1:( _temp5001 < _temp4999.base? 1: _temp5001 >= _temp4999.last_plus_one)){
_throw( Null_Exception);}* _temp5001;})))->tms; _temp4998;}); _temp4997;});
_temp4996;}); _temp4995;}); break; case 203: _LL4994: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5008=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5008[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5009; _temp5009.tag=
Cyc_AbstractDeclarator_tok_tag; _temp5009.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5010=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5010->tms=({ struct Cyc_List_List* _temp5011=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5011->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5012=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5012[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5013; _temp5013.tag= Cyc_Absyn_Function_mod_tag;
_temp5013.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5014=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5014[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5015; _temp5015.tag= Cyc_Absyn_WithTypes_tag;
_temp5015.f1= 0; _temp5015.f2= 0; _temp5015.f3= 0; _temp5015;}); _temp5014;}));
_temp5013;}); _temp5012;})); _temp5011->tl= 0; _temp5011;}); _temp5010;});
_temp5009;}); _temp5008;}); break; case 204: _LL5007: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5017=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5017[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5018; _temp5018.tag=
Cyc_AbstractDeclarator_tok_tag; _temp5018.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5019=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5019->tms=({ struct Cyc_List_List* _temp5020=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5020->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5021=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5021[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5022; _temp5022.tag= Cyc_Absyn_Function_mod_tag;
_temp5022.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5023=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5023[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5024; _temp5024.tag= Cyc_Absyn_WithTypes_tag;
_temp5024.f1= 0; _temp5024.f2= 0; _temp5024.f3=({ struct Cyc_Core_Opt* _temp5025=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5025->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp5026=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp5026[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp5027; _temp5027.tag= Cyc_Absyn_JoinEff_tag;
_temp5027.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5028= yyvs;
struct _xtunion_struct** _temp5030= _temp5028.curr +( yyvsp_offset - 1); if(
_temp5028.base == 0? 1:( _temp5030 < _temp5028.base? 1: _temp5030 >= _temp5028.last_plus_one)){
_throw( Null_Exception);}* _temp5030;})); _temp5027;}); _temp5026;})); _temp5025;});
_temp5024;}); _temp5023;})); _temp5022;}); _temp5021;})); _temp5020->tl= 0;
_temp5020;}); _temp5019;}); _temp5018;}); _temp5017;}); break; case 205: _LL5016: {
struct _tuple16 _temp5037; struct Cyc_Core_Opt* _temp5038; int _temp5040; struct
Cyc_List_List* _temp5042; struct _tuple16* _temp5035= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp5032= yyvs; struct _xtunion_struct** _temp5034=
_temp5032.curr +( yyvsp_offset - 1); if( _temp5032.base == 0? 1:( _temp5034 <
_temp5032.base? 1: _temp5034 >= _temp5032.last_plus_one)){ _throw(
Null_Exception);}* _temp5034;})); _temp5037=* _temp5035; _LL5043: _temp5042=
_temp5037.f1; goto _LL5041; _LL5041: _temp5040= _temp5037.f2; goto _LL5039;
_LL5039: _temp5038= _temp5037.f3; goto _LL5036; _LL5036: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5044=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5044[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5045; _temp5045.tag=
Cyc_AbstractDeclarator_tok_tag; _temp5045.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5046=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5046->tms=({ struct Cyc_List_List* _temp5047=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5047->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5048=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5048[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5049; _temp5049.tag= Cyc_Absyn_Function_mod_tag;
_temp5049.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5050=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5050[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5051; _temp5051.tag= Cyc_Absyn_WithTypes_tag;
_temp5051.f1= _temp5042; _temp5051.f2= _temp5040; _temp5051.f3= _temp5038;
_temp5051;}); _temp5050;})); _temp5049;}); _temp5048;})); _temp5047->tl= 0;
_temp5047;}); _temp5046;}); _temp5045;}); _temp5044;}); break;} case 206:
_LL5031: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5053=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5053[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5054; _temp5054.tag= Cyc_AbstractDeclarator_tok_tag; _temp5054.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp5055=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5055->tms=({
struct Cyc_List_List* _temp5056=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5056->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp5060=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp5060[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp5061; _temp5061.tag=
Cyc_Absyn_Function_mod_tag; _temp5061.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp5062=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5062[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5063; _temp5063.tag= Cyc_Absyn_WithTypes_tag;
_temp5063.f1= 0; _temp5063.f2= 0; _temp5063.f3= 0; _temp5063;}); _temp5062;}));
_temp5061;}); _temp5060;})); _temp5056->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5057= yyvs; struct _xtunion_struct** _temp5059=
_temp5057.curr +( yyvsp_offset - 2); if( _temp5057.base == 0? 1:( _temp5059 <
_temp5057.base? 1: _temp5059 >= _temp5057.last_plus_one)){ _throw(
Null_Exception);}* _temp5059;})))->tms; _temp5056;}); _temp5055;}); _temp5054;});
_temp5053;}); break; case 207: _LL5052: { struct Cyc_Core_Opt* eff=({ struct Cyc_Core_Opt*
_temp5065=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5065->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp5066=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp5066[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp5067; _temp5067.tag= Cyc_Absyn_JoinEff_tag;
_temp5067.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5068= yyvs;
struct _xtunion_struct** _temp5070= _temp5068.curr +( yyvsp_offset - 1); if(
_temp5068.base == 0? 1:( _temp5070 < _temp5068.base? 1: _temp5070 >= _temp5068.last_plus_one)){
_throw( Null_Exception);}* _temp5070;})); _temp5067;}); _temp5066;})); _temp5065;});
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5071=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5071[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5072; _temp5072.tag= Cyc_AbstractDeclarator_tok_tag; _temp5072.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp5073=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5073->tms=({
struct Cyc_List_List* _temp5074=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5074->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp5078=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp5078[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp5079; _temp5079.tag=
Cyc_Absyn_Function_mod_tag; _temp5079.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp5080=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5080[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5081; _temp5081.tag= Cyc_Absyn_WithTypes_tag;
_temp5081.f1= 0; _temp5081.f2= 0; _temp5081.f3= eff; _temp5081;}); _temp5080;}));
_temp5079;}); _temp5078;})); _temp5074->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5075= yyvs; struct _xtunion_struct** _temp5077=
_temp5075.curr +( yyvsp_offset - 4); if( _temp5075.base == 0? 1:( _temp5077 <
_temp5075.base? 1: _temp5077 >= _temp5075.last_plus_one)){ _throw(
Null_Exception);}* _temp5077;})))->tms; _temp5074;}); _temp5073;}); _temp5072;});
_temp5071;}); break;} case 208: _LL5064: { struct _tuple16 _temp5088; struct Cyc_Core_Opt*
_temp5089; int _temp5091; struct Cyc_List_List* _temp5093; struct _tuple16*
_temp5086= Cyc_yyget_ParamDeclListBool_tok(({ struct _tagged_ptr2 _temp5083=
yyvs; struct _xtunion_struct** _temp5085= _temp5083.curr +( yyvsp_offset - 1);
if( _temp5083.base == 0? 1:( _temp5085 < _temp5083.base? 1: _temp5085 >=
_temp5083.last_plus_one)){ _throw( Null_Exception);}* _temp5085;})); _temp5088=*
_temp5086; _LL5094: _temp5093= _temp5088.f1; goto _LL5092; _LL5092: _temp5091=
_temp5088.f2; goto _LL5090; _LL5090: _temp5089= _temp5088.f3; goto _LL5087;
_LL5087: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5095=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5095[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5096; _temp5096.tag= Cyc_AbstractDeclarator_tok_tag; _temp5096.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp5097=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5097->tms=({
struct Cyc_List_List* _temp5098=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5098->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp5102=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp5102[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp5103; _temp5103.tag=
Cyc_Absyn_Function_mod_tag; _temp5103.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp5104=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5104[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5105; _temp5105.tag= Cyc_Absyn_WithTypes_tag;
_temp5105.f1= _temp5093; _temp5105.f2= _temp5091; _temp5105.f3= _temp5089;
_temp5105;}); _temp5104;})); _temp5103;}); _temp5102;})); _temp5098->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5099= yyvs; struct _xtunion_struct** _temp5101=
_temp5099.curr +( yyvsp_offset - 3); if( _temp5099.base == 0? 1:( _temp5101 <
_temp5099.base? 1: _temp5101 >= _temp5099.last_plus_one)){ _throw(
Null_Exception);}* _temp5101;})))->tms; _temp5098;}); _temp5097;}); _temp5096;});
_temp5095;}); break;} case 209: _LL5082: { struct Cyc_List_List* ts=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5107=
yyls; struct Cyc_Yyltype* _temp5109= _temp5107.curr +( yylsp_offset - 2); if(
_temp5107.base == 0? 1:( _temp5109 < _temp5107.base? 1: _temp5109 >= _temp5107.last_plus_one)){
_throw( Null_Exception);}* _temp5109;}).first_line,({ struct _tagged_ptr3
_temp5110= yyls; struct Cyc_Yyltype* _temp5112= _temp5110.curr + yylsp_offset;
if( _temp5110.base == 0? 1:( _temp5112 < _temp5110.base? 1: _temp5112 >=
_temp5110.last_plus_one)){ _throw( Null_Exception);}* _temp5112;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5113= yyvs; struct _xtunion_struct** _temp5115=
_temp5113.curr +( yyvsp_offset - 1); if( _temp5113.base == 0? 1:( _temp5115 <
_temp5113.base? 1: _temp5115 >= _temp5113.last_plus_one)){ _throw(
Null_Exception);}* _temp5115;})))); yyval=( struct _xtunion_struct*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp5116=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5116[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5117; _temp5117.tag= Cyc_AbstractDeclarator_tok_tag;
_temp5117.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5118=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5118->tms=({
struct Cyc_List_List* _temp5119=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5119->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp5123=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp5123[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp5124; _temp5124.tag= Cyc_Absyn_TypeParams_mod_tag; _temp5124.f1= ts;
_temp5124.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5125= yyls;
struct Cyc_Yyltype* _temp5127= _temp5125.curr +( yylsp_offset - 2); if(
_temp5125.base == 0? 1:( _temp5127 < _temp5125.base? 1: _temp5127 >= _temp5125.last_plus_one)){
_throw( Null_Exception);}* _temp5127;}).first_line,({ struct _tagged_ptr3
_temp5128= yyls; struct Cyc_Yyltype* _temp5130= _temp5128.curr + yylsp_offset;
if( _temp5128.base == 0? 1:( _temp5130 < _temp5128.base? 1: _temp5130 >=
_temp5128.last_plus_one)){ _throw( Null_Exception);}* _temp5130;}).last_line);
_temp5124.f3= 0; _temp5124;}); _temp5123;})); _temp5119->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5120= yyvs; struct _xtunion_struct** _temp5122=
_temp5120.curr +( yyvsp_offset - 3); if( _temp5120.base == 0? 1:( _temp5122 <
_temp5120.base? 1: _temp5122 >= _temp5120.last_plus_one)){ _throw(
Null_Exception);}* _temp5122;})))->tms; _temp5119;}); _temp5118;}); _temp5117;});
_temp5116;}); break;} case 210: _LL5106:(( struct Cyc_Lexing_lexbuf*)({ struct
Cyc_Core_Opt* _temp5132= Cyc_Parse_lbuf; if( _temp5132 == 0){ _throw(
Null_Exception);} _temp5132->v;}))->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5133=
yyls; struct Cyc_Yyltype* _temp5135= _temp5133.curr +( yylsp_offset - 2); if(
_temp5133.base == 0? 1:( _temp5135 < _temp5133.base? 1: _temp5135 >= _temp5133.last_plus_one)){
_throw( Null_Exception);}* _temp5135;}).first_line,({ struct _tagged_ptr3
_temp5136= yyls; struct Cyc_Yyltype* _temp5138= _temp5136.curr + yylsp_offset;
if( _temp5136.base == 0? 1:( _temp5138 < _temp5136.base? 1: _temp5138 >=
_temp5136.last_plus_one)){ _throw( Null_Exception);}* _temp5138;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5139= yyvs; struct _xtunion_struct** _temp5141=
_temp5139.curr +( yyvsp_offset - 1); if( _temp5139.base == 0? 1:( _temp5141 <
_temp5139.base? 1: _temp5141 >= _temp5139.last_plus_one)){ _throw(
Null_Exception);}* _temp5141;})))); yyval=( struct _xtunion_struct*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp5142=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5142[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5143; _temp5143.tag= Cyc_AbstractDeclarator_tok_tag;
_temp5143.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5144=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5144->tms=({
struct Cyc_List_List* _temp5145=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5145->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp5149=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp5149[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp5150; _temp5150.tag= Cyc_Absyn_TypeParams_mod_tag; _temp5150.f1= ts;
_temp5150.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5151= yyls;
struct Cyc_Yyltype* _temp5153= _temp5151.curr +( yylsp_offset - 2); if(
_temp5151.base == 0? 1:( _temp5153 < _temp5151.base? 1: _temp5153 >= _temp5151.last_plus_one)){
_throw( Null_Exception);}* _temp5153;}).first_line,({ struct _tagged_ptr3
_temp5154= yyls; struct Cyc_Yyltype* _temp5156= _temp5154.curr + yylsp_offset;
if( _temp5154.base == 0? 1:( _temp5156 < _temp5154.base? 1: _temp5156 >=
_temp5154.last_plus_one)){ _throw( Null_Exception);}* _temp5156;}).last_line);
_temp5150.f3= 0; _temp5150;}); _temp5149;})); _temp5145->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5146= yyvs; struct _xtunion_struct** _temp5148=
_temp5146.curr +( yyvsp_offset - 3); if( _temp5146.base == 0? 1:( _temp5148 <
_temp5146.base? 1: _temp5148 >= _temp5146.last_plus_one)){ _throw(
Null_Exception);}* _temp5148;})))->tms; _temp5145;}); _temp5144;}); _temp5143;});
_temp5142;}); break;} case 211: _LL5131: yyval=( struct _xtunion_struct*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp5158=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5158[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5159; _temp5159.tag= Cyc_AbstractDeclarator_tok_tag;
_temp5159.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5160=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5160->tms=({
struct Cyc_List_List* _temp5161=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5161->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp5165=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp5165[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp5166; _temp5166.tag= Cyc_Absyn_Attributes_mod_tag; _temp5166.f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5167= yyls; struct Cyc_Yyltype* _temp5169= _temp5167.curr
+ yylsp_offset; if( _temp5167.base == 0? 1:( _temp5169 < _temp5167.base? 1:
_temp5169 >= _temp5167.last_plus_one)){ _throw( Null_Exception);}* _temp5169;}).first_line,({
struct _tagged_ptr3 _temp5170= yyls; struct Cyc_Yyltype* _temp5172= _temp5170.curr
+ yylsp_offset; if( _temp5170.base == 0? 1:( _temp5172 < _temp5170.base? 1:
_temp5172 >= _temp5170.last_plus_one)){ _throw( Null_Exception);}* _temp5172;}).last_line);
_temp5166.f2= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp5173= yyvs;
struct _xtunion_struct** _temp5175= _temp5173.curr + yyvsp_offset; if( _temp5173.base
== 0? 1:( _temp5175 < _temp5173.base? 1: _temp5175 >= _temp5173.last_plus_one)){
_throw( Null_Exception);}* _temp5175;})); _temp5166;}); _temp5165;})); _temp5161->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp5162= yyvs; struct
_xtunion_struct** _temp5164= _temp5162.curr +( yyvsp_offset - 1); if( _temp5162.base
== 0? 1:( _temp5164 < _temp5162.base? 1: _temp5164 >= _temp5162.last_plus_one)){
_throw( Null_Exception);}* _temp5164;})))->tms; _temp5161;}); _temp5160;});
_temp5159;}); _temp5158;}); break; case 212: _LL5157: yyval=({ struct
_tagged_ptr2 _temp5177= yyvs; struct _xtunion_struct** _temp5179= _temp5177.curr
+ yyvsp_offset; if( _temp5177.base == 0? 1:( _temp5179 < _temp5177.base? 1:
_temp5179 >= _temp5177.last_plus_one)){ _throw( Null_Exception);}* _temp5179;});
break; case 213: _LL5176: yyval=({ struct _tagged_ptr2 _temp5181= yyvs; struct
_xtunion_struct** _temp5183= _temp5181.curr + yyvsp_offset; if( _temp5181.base
== 0? 1:( _temp5183 < _temp5181.base? 1: _temp5183 >= _temp5181.last_plus_one)){
_throw( Null_Exception);}* _temp5183;}); break; case 214: _LL5180: yyval=({
struct _tagged_ptr2 _temp5185= yyvs; struct _xtunion_struct** _temp5187=
_temp5185.curr + yyvsp_offset; if( _temp5185.base == 0? 1:( _temp5187 <
_temp5185.base? 1: _temp5187 >= _temp5185.last_plus_one)){ _throw(
Null_Exception);}* _temp5187;}); break; case 215: _LL5184: yyval=({ struct
_tagged_ptr2 _temp5189= yyvs; struct _xtunion_struct** _temp5191= _temp5189.curr
+ yyvsp_offset; if( _temp5189.base == 0? 1:( _temp5191 < _temp5189.base? 1:
_temp5191 >= _temp5189.last_plus_one)){ _throw( Null_Exception);}* _temp5191;});
break; case 216: _LL5188: yyval=({ struct _tagged_ptr2 _temp5193= yyvs; struct
_xtunion_struct** _temp5195= _temp5193.curr + yyvsp_offset; if( _temp5193.base
== 0? 1:( _temp5195 < _temp5193.base? 1: _temp5195 >= _temp5193.last_plus_one)){
_throw( Null_Exception);}* _temp5195;}); break; case 217: _LL5192: yyval=({
struct _tagged_ptr2 _temp5197= yyvs; struct _xtunion_struct** _temp5199=
_temp5197.curr + yyvsp_offset; if( _temp5197.base == 0? 1:( _temp5199 <
_temp5197.base? 1: _temp5199 >= _temp5197.last_plus_one)){ _throw(
Null_Exception);}* _temp5199;}); break; case 218: _LL5196: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5201= yyvs; struct
_xtunion_struct** _temp5203= _temp5201.curr +( yyvsp_offset - 3); if( _temp5201.base
== 0? 1:( _temp5203 < _temp5201.base? 1: _temp5203 >= _temp5201.last_plus_one)){
_throw( Null_Exception);}* _temp5203;})),( struct _tagged_string)({ char*
_temp5204=( char*)"`H"; struct _tagged_string _temp5205; _temp5205.curr=
_temp5204; _temp5205.base= _temp5204; _temp5205.last_plus_one= _temp5204 + 3;
_temp5205;})) == 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp5206=(
char*)"bad occurrence of heap region `H"; struct _tagged_string _temp5207;
_temp5207.curr= _temp5206; _temp5207.base= _temp5206; _temp5207.last_plus_one=
_temp5206 + 33; _temp5207;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5208= yyls; struct Cyc_Yyltype* _temp5210= _temp5208.curr +( yylsp_offset -
3); if( _temp5208.base == 0? 1:( _temp5210 < _temp5208.base? 1: _temp5210 >=
_temp5208.last_plus_one)){ _throw( Null_Exception);}* _temp5210;}).first_line,({
struct _tagged_ptr3 _temp5211= yyls; struct Cyc_Yyltype* _temp5213= _temp5211.curr
+( yylsp_offset - 3); if( _temp5211.base == 0? 1:( _temp5213 < _temp5211.base? 1:
_temp5213 >= _temp5211.last_plus_one)){ _throw( Null_Exception);}* _temp5213;}).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp5214=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp5214->name=({ struct
_tagged_string* _temp5215=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp5215[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp5216= yyvs; struct _xtunion_struct** _temp5218= _temp5216.curr +(
yyvsp_offset - 3); if( _temp5216.base == 0? 1:( _temp5218 < _temp5216.base? 1:
_temp5218 >= _temp5216.last_plus_one)){ _throw( Null_Exception);}* _temp5218;}));
_temp5215;}); _temp5214->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind); _temp5214;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp5219=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp5219[ 0]=({ struct Cyc_Absyn_VarType_struct _temp5220; _temp5220.tag= Cyc_Absyn_VarType_tag;
_temp5220.f1= tv; _temp5220;}); _temp5219;}); yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5221=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5221[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5222; _temp5222.tag= Cyc_Stmt_tok_tag; _temp5222.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Region_s_struct* _temp5223=( struct Cyc_Absyn_Region_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct)); _temp5223[ 0]=({ struct
Cyc_Absyn_Region_s_struct _temp5224; _temp5224.tag= Cyc_Absyn_Region_s_tag;
_temp5224.f1= tv; _temp5224.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp5225=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp5225->f1=(
void*) Cyc_Absyn_Loc_n; _temp5225->f2=({ struct _tagged_string* _temp5226=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp5226[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5227= yyvs; struct
_xtunion_struct** _temp5229= _temp5227.curr +( yyvsp_offset - 1); if( _temp5227.base
== 0? 1:( _temp5229 < _temp5227.base? 1: _temp5229 >= _temp5227.last_plus_one)){
_throw( Null_Exception);}* _temp5229;})); _temp5226;}); _temp5225;}),( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp5230=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp5230[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp5231; _temp5231.tag= Cyc_Absyn_RgnHandleType_tag;
_temp5231.f1=( void*) t; _temp5231;}); _temp5230;}), 0); _temp5224.f3= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5232= yyvs; struct _xtunion_struct** _temp5234=
_temp5232.curr + yyvsp_offset; if( _temp5232.base == 0? 1:( _temp5234 <
_temp5232.base? 1: _temp5234 >= _temp5232.last_plus_one)){ _throw(
Null_Exception);}* _temp5234;})); _temp5224;}); _temp5223;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5235= yyls; struct Cyc_Yyltype* _temp5237= _temp5235.curr
+( yylsp_offset - 5); if( _temp5235.base == 0? 1:( _temp5237 < _temp5235.base? 1:
_temp5237 >= _temp5235.last_plus_one)){ _throw( Null_Exception);}* _temp5237;}).first_line,({
struct _tagged_ptr3 _temp5238= yyls; struct Cyc_Yyltype* _temp5240= _temp5238.curr
+ yylsp_offset; if( _temp5238.base == 0? 1:( _temp5240 < _temp5238.base? 1:
_temp5240 >= _temp5238.last_plus_one)){ _throw( Null_Exception);}* _temp5240;}).last_line));
_temp5222;}); _temp5221;}); break;} case 219: _LL5200: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5242= yyvs; struct
_xtunion_struct** _temp5244= _temp5242.curr +( yyvsp_offset - 1); if( _temp5242.base
== 0? 1:( _temp5244 < _temp5242.base? 1: _temp5244 >= _temp5242.last_plus_one)){
_throw( Null_Exception);}* _temp5244;})),( struct _tagged_string)({ char*
_temp5245=( char*)"H"; struct _tagged_string _temp5246; _temp5246.curr=
_temp5245; _temp5246.base= _temp5245; _temp5246.last_plus_one= _temp5245 + 2;
_temp5246;})) == 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp5247=(
char*)"bad occurrence of heap region `H"; struct _tagged_string _temp5248;
_temp5248.curr= _temp5247; _temp5248.base= _temp5247; _temp5248.last_plus_one=
_temp5247 + 33; _temp5248;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5249= yyls; struct Cyc_Yyltype* _temp5251= _temp5249.curr + yylsp_offset;
if( _temp5249.base == 0? 1:( _temp5251 < _temp5249.base? 1: _temp5251 >=
_temp5249.last_plus_one)){ _throw( Null_Exception);}* _temp5251;}).first_line,({
struct _tagged_ptr3 _temp5252= yyls; struct Cyc_Yyltype* _temp5254= _temp5252.curr
+ yylsp_offset; if( _temp5252.base == 0? 1:( _temp5254 < _temp5252.base? 1:
_temp5254 >= _temp5252.last_plus_one)){ _throw( Null_Exception);}* _temp5254;}).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp5255=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp5255->name=({ struct
_tagged_string* _temp5256=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp5256[ 0]=({ struct _tagged_string _temp5260= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp5257= yyvs; struct _xtunion_struct** _temp5259=
_temp5257.curr +( yyvsp_offset - 1); if( _temp5257.base == 0? 1:( _temp5259 <
_temp5257.base? 1: _temp5259 >= _temp5257.last_plus_one)){ _throw(
Null_Exception);}* _temp5259;})); xprintf("`%.*s", _temp5260.last_plus_one -
_temp5260.curr, _temp5260.curr);}); _temp5256;}); _temp5255->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp5255;}); void*
t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp5261=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp5261[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp5262; _temp5262.tag= Cyc_Absyn_VarType_tag;
_temp5262.f1= tv; _temp5262;}); _temp5261;}); yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5263=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5263[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5264; _temp5264.tag= Cyc_Stmt_tok_tag; _temp5264.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Region_s_struct* _temp5265=( struct Cyc_Absyn_Region_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct)); _temp5265[ 0]=({ struct
Cyc_Absyn_Region_s_struct _temp5266; _temp5266.tag= Cyc_Absyn_Region_s_tag;
_temp5266.f1= tv; _temp5266.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp5267=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp5267->f1=(
void*) Cyc_Absyn_Loc_n; _temp5267->f2=({ struct _tagged_string* _temp5268=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp5268[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5269= yyvs; struct
_xtunion_struct** _temp5271= _temp5269.curr +( yyvsp_offset - 1); if( _temp5269.base
== 0? 1:( _temp5271 < _temp5269.base? 1: _temp5271 >= _temp5269.last_plus_one)){
_throw( Null_Exception);}* _temp5271;})); _temp5268;}); _temp5267;}),( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp5272=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp5272[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp5273; _temp5273.tag= Cyc_Absyn_RgnHandleType_tag;
_temp5273.f1=( void*) t; _temp5273;}); _temp5272;}), 0); _temp5266.f3= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5274= yyvs; struct _xtunion_struct** _temp5276=
_temp5274.curr + yyvsp_offset; if( _temp5274.base == 0? 1:( _temp5276 <
_temp5274.base? 1: _temp5276 >= _temp5274.last_plus_one)){ _throw(
Null_Exception);}* _temp5276;})); _temp5266;}); _temp5265;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5277= yyls; struct Cyc_Yyltype* _temp5279= _temp5277.curr
+( yylsp_offset - 2); if( _temp5277.base == 0? 1:( _temp5279 < _temp5277.base? 1:
_temp5279 >= _temp5277.last_plus_one)){ _throw( Null_Exception);}* _temp5279;}).first_line,({
struct _tagged_ptr3 _temp5280= yyls; struct Cyc_Yyltype* _temp5282= _temp5280.curr
+ yylsp_offset; if( _temp5280.base == 0? 1:( _temp5282 < _temp5280.base? 1:
_temp5282 >= _temp5280.last_plus_one)){ _throw( Null_Exception);}* _temp5282;}).last_line));
_temp5264;}); _temp5263;}); break;} case 220: _LL5241: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5284=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5284[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5285; _temp5285.tag= Cyc_Stmt_tok_tag; _temp5285.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Cut_s_struct* _temp5286=( struct Cyc_Absyn_Cut_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct)); _temp5286[ 0]=({ struct Cyc_Absyn_Cut_s_struct
_temp5287; _temp5287.tag= Cyc_Absyn_Cut_s_tag; _temp5287.f1= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5288= yyvs; struct _xtunion_struct** _temp5290=
_temp5288.curr + yyvsp_offset; if( _temp5288.base == 0? 1:( _temp5290 <
_temp5288.base? 1: _temp5290 >= _temp5288.last_plus_one)){ _throw(
Null_Exception);}* _temp5290;})); _temp5287;}); _temp5286;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5291= yyls; struct Cyc_Yyltype* _temp5293= _temp5291.curr
+( yylsp_offset - 1); if( _temp5291.base == 0? 1:( _temp5293 < _temp5291.base? 1:
_temp5293 >= _temp5291.last_plus_one)){ _throw( Null_Exception);}* _temp5293;}).first_line,({
struct _tagged_ptr3 _temp5294= yyls; struct Cyc_Yyltype* _temp5296= _temp5294.curr
+ yylsp_offset; if( _temp5294.base == 0? 1:( _temp5296 < _temp5294.base? 1:
_temp5296 >= _temp5294.last_plus_one)){ _throw( Null_Exception);}* _temp5296;}).last_line));
_temp5285;}); _temp5284;}); break; case 221: _LL5283: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5298=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5298[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5299; _temp5299.tag= Cyc_Stmt_tok_tag; _temp5299.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Splice_s_struct* _temp5300=( struct Cyc_Absyn_Splice_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp5300[ 0]=({ struct
Cyc_Absyn_Splice_s_struct _temp5301; _temp5301.tag= Cyc_Absyn_Splice_s_tag;
_temp5301.f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5302= yyvs; struct
_xtunion_struct** _temp5304= _temp5302.curr + yyvsp_offset; if( _temp5302.base
== 0? 1:( _temp5304 < _temp5302.base? 1: _temp5304 >= _temp5302.last_plus_one)){
_throw( Null_Exception);}* _temp5304;})); _temp5301;}); _temp5300;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5305= yyls; struct Cyc_Yyltype* _temp5307= _temp5305.curr
+( yylsp_offset - 1); if( _temp5305.base == 0? 1:( _temp5307 < _temp5305.base? 1:
_temp5307 >= _temp5305.last_plus_one)){ _throw( Null_Exception);}* _temp5307;}).first_line,({
struct _tagged_ptr3 _temp5308= yyls; struct Cyc_Yyltype* _temp5310= _temp5308.curr
+ yylsp_offset; if( _temp5308.base == 0? 1:( _temp5310 < _temp5308.base? 1:
_temp5310 >= _temp5308.last_plus_one)){ _throw( Null_Exception);}* _temp5310;}).last_line));
_temp5299;}); _temp5298;}); break; case 222: _LL5297: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5312=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5312[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5313; _temp5313.tag= Cyc_Stmt_tok_tag; _temp5313.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Label_s_struct* _temp5314=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp5314[ 0]=({ struct
Cyc_Absyn_Label_s_struct _temp5315; _temp5315.tag= Cyc_Absyn_Label_s_tag;
_temp5315.f1=({ struct _tagged_string* _temp5316=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp5316[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp5317= yyvs; struct _xtunion_struct** _temp5319=
_temp5317.curr +( yyvsp_offset - 2); if( _temp5317.base == 0? 1:( _temp5319 <
_temp5317.base? 1: _temp5319 >= _temp5317.last_plus_one)){ _throw(
Null_Exception);}* _temp5319;})); _temp5316;}); _temp5315.f2= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5320= yyvs; struct _xtunion_struct** _temp5322=
_temp5320.curr + yyvsp_offset; if( _temp5320.base == 0? 1:( _temp5322 <
_temp5320.base? 1: _temp5322 >= _temp5320.last_plus_one)){ _throw(
Null_Exception);}* _temp5322;})); _temp5315;}); _temp5314;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5323= yyls; struct Cyc_Yyltype* _temp5325= _temp5323.curr
+( yylsp_offset - 2); if( _temp5323.base == 0? 1:( _temp5325 < _temp5323.base? 1:
_temp5325 >= _temp5323.last_plus_one)){ _throw( Null_Exception);}* _temp5325;}).first_line,({
struct _tagged_ptr3 _temp5326= yyls; struct Cyc_Yyltype* _temp5328= _temp5326.curr
+ yylsp_offset; if( _temp5326.base == 0? 1:( _temp5328 < _temp5326.base? 1:
_temp5328 >= _temp5326.last_plus_one)){ _throw( Null_Exception);}* _temp5328;}).last_line));
_temp5313;}); _temp5312;}); break; case 223: _LL5311: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5330=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5330[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5331; _temp5331.tag= Cyc_Stmt_tok_tag; _temp5331.f1= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5332= yyls; struct Cyc_Yyltype*
_temp5334= _temp5332.curr + yylsp_offset; if( _temp5332.base == 0? 1:( _temp5334
< _temp5332.base? 1: _temp5334 >= _temp5332.last_plus_one)){ _throw(
Null_Exception);}* _temp5334;}).first_line,({ struct _tagged_ptr3 _temp5335=
yyls; struct Cyc_Yyltype* _temp5337= _temp5335.curr + yylsp_offset; if(
_temp5335.base == 0? 1:( _temp5337 < _temp5335.base? 1: _temp5337 >= _temp5335.last_plus_one)){
_throw( Null_Exception);}* _temp5337;}).last_line)); _temp5331;}); _temp5330;});
break; case 224: _LL5329: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5339=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5339[ 0]=({ struct Cyc_Stmt_tok_struct _temp5340; _temp5340.tag= Cyc_Stmt_tok_tag;
_temp5340.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5341= yyvs; struct _xtunion_struct** _temp5343= _temp5341.curr +(
yyvsp_offset - 1); if( _temp5341.base == 0? 1:( _temp5343 < _temp5341.base? 1:
_temp5343 >= _temp5341.last_plus_one)){ _throw( Null_Exception);}* _temp5343;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5344= yyls; struct Cyc_Yyltype*
_temp5346= _temp5344.curr +( yylsp_offset - 1); if( _temp5344.base == 0? 1:(
_temp5346 < _temp5344.base? 1: _temp5346 >= _temp5344.last_plus_one)){ _throw(
Null_Exception);}* _temp5346;}).first_line,({ struct _tagged_ptr3 _temp5347=
yyls; struct Cyc_Yyltype* _temp5349= _temp5347.curr + yylsp_offset; if(
_temp5347.base == 0? 1:( _temp5349 < _temp5347.base? 1: _temp5349 >= _temp5347.last_plus_one)){
_throw( Null_Exception);}* _temp5349;}).last_line)); _temp5340;}); _temp5339;});
break; case 225: _LL5338: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5351=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5351[ 0]=({ struct Cyc_Stmt_tok_struct _temp5352; _temp5352.tag= Cyc_Stmt_tok_tag;
_temp5352.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5353= yyls; struct Cyc_Yyltype* _temp5355= _temp5353.curr +(
yylsp_offset - 1); if( _temp5353.base == 0? 1:( _temp5355 < _temp5353.base? 1:
_temp5355 >= _temp5353.last_plus_one)){ _throw( Null_Exception);}* _temp5355;}).first_line,({
struct _tagged_ptr3 _temp5356= yyls; struct Cyc_Yyltype* _temp5358= _temp5356.curr
+ yylsp_offset; if( _temp5356.base == 0? 1:( _temp5358 < _temp5356.base? 1:
_temp5358 >= _temp5356.last_plus_one)){ _throw( Null_Exception);}* _temp5358;}).last_line));
_temp5352;}); _temp5351;}); break; case 226: _LL5350: yyval=({ struct
_tagged_ptr2 _temp5360= yyvs; struct _xtunion_struct** _temp5362= _temp5360.curr
+( yyvsp_offset - 1); if( _temp5360.base == 0? 1:( _temp5362 < _temp5360.base? 1:
_temp5362 >= _temp5360.last_plus_one)){ _throw( Null_Exception);}* _temp5362;});
break; case 227: _LL5359:{ void* _temp5367= Cyc_yyget_BlockItem_tok(({ struct
_tagged_ptr2 _temp5364= yyvs; struct _xtunion_struct** _temp5366= _temp5364.curr
+ yyvsp_offset; if( _temp5364.base == 0? 1:( _temp5366 < _temp5364.base? 1:
_temp5366 >= _temp5364.last_plus_one)){ _throw( Null_Exception);}* _temp5366;}));
struct Cyc_List_List* _temp5375; struct Cyc_Absyn_Fndecl* _temp5377; struct Cyc_Absyn_Stmt*
_temp5379; _LL5369: if((( struct _tunion_struct*) _temp5367)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL5376: _temp5375=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp5367)->f1; goto _LL5370;} else{ goto _LL5371;} _LL5371: if((( struct
_tunion_struct*) _temp5367)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL5378: _temp5377=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp5367)->f1;
goto _LL5372;} else{ goto _LL5373;} _LL5373: if((( struct _tunion_struct*)
_temp5367)->tag == Cyc_Parse_Stmt_bl_tag){ _LL5380: _temp5379=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp5367)->f1; goto _LL5374;} else{ goto
_LL5368;} _LL5370: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5381=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5381[ 0]=({ struct Cyc_Stmt_tok_struct _temp5382; _temp5382.tag= Cyc_Stmt_tok_tag;
_temp5382.f1= Cyc_Parse_flatten_declarations( _temp5375, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5383= yyls; struct Cyc_Yyltype*
_temp5385= _temp5383.curr + yylsp_offset; if( _temp5383.base == 0? 1:( _temp5385
< _temp5383.base? 1: _temp5385 >= _temp5383.last_plus_one)){ _throw(
Null_Exception);}* _temp5385;}).first_line,({ struct _tagged_ptr3 _temp5386=
yyls; struct Cyc_Yyltype* _temp5388= _temp5386.curr + yylsp_offset; if(
_temp5386.base == 0? 1:( _temp5388 < _temp5386.base? 1: _temp5388 >= _temp5386.last_plus_one)){
_throw( Null_Exception);}* _temp5388;}).last_line))); _temp5382;}); _temp5381;});
goto _LL5368; _LL5372: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5389=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5389[ 0]=({ struct Cyc_Stmt_tok_struct _temp5390; _temp5390.tag= Cyc_Stmt_tok_tag;
_temp5390.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp5391=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5391[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5392; _temp5392.tag= Cyc_Absyn_Fn_d_tag;
_temp5392.f1= _temp5377; _temp5392;}); _temp5391;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5393= yyls; struct Cyc_Yyltype* _temp5395= _temp5393.curr
+ yylsp_offset; if( _temp5393.base == 0? 1:( _temp5395 < _temp5393.base? 1:
_temp5395 >= _temp5393.last_plus_one)){ _throw( Null_Exception);}* _temp5395;}).first_line,({
struct _tagged_ptr3 _temp5396= yyls; struct Cyc_Yyltype* _temp5398= _temp5396.curr
+ yylsp_offset; if( _temp5396.base == 0? 1:( _temp5398 < _temp5396.base? 1:
_temp5398 >= _temp5396.last_plus_one)){ _throw( Null_Exception);}* _temp5398;}).last_line)),
Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5399= yyls; struct Cyc_Yyltype* _temp5401= _temp5399.curr + yylsp_offset;
if( _temp5399.base == 0? 1:( _temp5401 < _temp5399.base? 1: _temp5401 >=
_temp5399.last_plus_one)){ _throw( Null_Exception);}* _temp5401;}).first_line,({
struct _tagged_ptr3 _temp5402= yyls; struct Cyc_Yyltype* _temp5404= _temp5402.curr
+ yylsp_offset; if( _temp5402.base == 0? 1:( _temp5404 < _temp5402.base? 1:
_temp5404 >= _temp5402.last_plus_one)){ _throw( Null_Exception);}* _temp5404;}).last_line)));
_temp5390;}); _temp5389;}); goto _LL5368; _LL5374: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5405=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5405[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5406; _temp5406.tag= Cyc_Stmt_tok_tag; _temp5406.f1= _temp5379; _temp5406;});
_temp5405;}); goto _LL5368; _LL5368:;} break; case 228: _LL5363:{ void*
_temp5411= Cyc_yyget_BlockItem_tok(({ struct _tagged_ptr2 _temp5408= yyvs;
struct _xtunion_struct** _temp5410= _temp5408.curr +( yyvsp_offset - 1); if(
_temp5408.base == 0? 1:( _temp5410 < _temp5408.base? 1: _temp5410 >= _temp5408.last_plus_one)){
_throw( Null_Exception);}* _temp5410;})); struct Cyc_List_List* _temp5419;
struct Cyc_Absyn_Fndecl* _temp5421; struct Cyc_Absyn_Stmt* _temp5423; _LL5413:
if((( struct _tunion_struct*) _temp5411)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL5420: _temp5419=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp5411)->f1; goto _LL5414;} else{ goto _LL5415;} _LL5415: if((( struct
_tunion_struct*) _temp5411)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL5422: _temp5421=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp5411)->f1;
goto _LL5416;} else{ goto _LL5417;} _LL5417: if((( struct _tunion_struct*)
_temp5411)->tag == Cyc_Parse_Stmt_bl_tag){ _LL5424: _temp5423=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp5411)->f1; goto _LL5418;} else{ goto
_LL5412;} _LL5414: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5425=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5425[ 0]=({ struct Cyc_Stmt_tok_struct _temp5426; _temp5426.tag= Cyc_Stmt_tok_tag;
_temp5426.f1= Cyc_Parse_flatten_declarations( _temp5419, Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5427= yyvs; struct _xtunion_struct** _temp5429=
_temp5427.curr + yyvsp_offset; if( _temp5427.base == 0? 1:( _temp5429 <
_temp5427.base? 1: _temp5429 >= _temp5427.last_plus_one)){ _throw(
Null_Exception);}* _temp5429;}))); _temp5426;}); _temp5425;}); goto _LL5412;
_LL5416: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5430=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5430[ 0]=({ struct Cyc_Stmt_tok_struct _temp5431; _temp5431.tag= Cyc_Stmt_tok_tag;
_temp5431.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp5432=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5432[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5433; _temp5433.tag= Cyc_Absyn_Fn_d_tag;
_temp5433.f1= _temp5421; _temp5433;}); _temp5432;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5434= yyls; struct Cyc_Yyltype* _temp5436= _temp5434.curr
+( yylsp_offset - 1); if( _temp5434.base == 0? 1:( _temp5436 < _temp5434.base? 1:
_temp5436 >= _temp5434.last_plus_one)){ _throw( Null_Exception);}* _temp5436;}).first_line,({
struct _tagged_ptr3 _temp5437= yyls; struct Cyc_Yyltype* _temp5439= _temp5437.curr
+( yylsp_offset - 1); if( _temp5437.base == 0? 1:( _temp5439 < _temp5437.base? 1:
_temp5439 >= _temp5437.last_plus_one)){ _throw( Null_Exception);}* _temp5439;}).last_line)),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5440= yyvs; struct
_xtunion_struct** _temp5442= _temp5440.curr + yyvsp_offset; if( _temp5440.base
== 0? 1:( _temp5442 < _temp5440.base? 1: _temp5442 >= _temp5440.last_plus_one)){
_throw( Null_Exception);}* _temp5442;}))); _temp5431;}); _temp5430;}); goto
_LL5412; _LL5418: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5443=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5443[ 0]=({ struct Cyc_Stmt_tok_struct _temp5444; _temp5444.tag= Cyc_Stmt_tok_tag;
_temp5444.f1= Cyc_Absyn_seq_stmt( _temp5423, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5445= yyvs; struct _xtunion_struct** _temp5447= _temp5445.curr
+ yyvsp_offset; if( _temp5445.base == 0? 1:( _temp5447 < _temp5445.base? 1:
_temp5447 >= _temp5445.last_plus_one)){ _throw( Null_Exception);}* _temp5447;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5448= yyls; struct Cyc_Yyltype*
_temp5450= _temp5448.curr +( yylsp_offset - 1); if( _temp5448.base == 0? 1:(
_temp5450 < _temp5448.base? 1: _temp5450 >= _temp5448.last_plus_one)){ _throw(
Null_Exception);}* _temp5450;}).first_line,({ struct _tagged_ptr3 _temp5451=
yyls; struct Cyc_Yyltype* _temp5453= _temp5451.curr + yylsp_offset; if(
_temp5451.base == 0? 1:( _temp5453 < _temp5451.base? 1: _temp5453 >= _temp5451.last_plus_one)){
_throw( Null_Exception);}* _temp5453;}).last_line)); _temp5444;}); _temp5443;});
goto _LL5412; _LL5412:;} break; case 229: _LL5407: yyval=( struct
_xtunion_struct*)({ struct Cyc_BlockItem_tok_struct* _temp5455=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp5455[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp5456; _temp5456.tag= Cyc_BlockItem_tok_tag;
_temp5456.f1=( void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp5457=(
struct Cyc_Parse_TopDecls_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct));
_temp5457[ 0]=({ struct Cyc_Parse_TopDecls_bl_struct _temp5458; _temp5458.tag=
Cyc_Parse_TopDecls_bl_tag; _temp5458.f1= Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp5459= yyvs; struct _xtunion_struct** _temp5461= _temp5459.curr
+ yyvsp_offset; if( _temp5459.base == 0? 1:( _temp5461 < _temp5459.base? 1:
_temp5461 >= _temp5459.last_plus_one)){ _throw( Null_Exception);}* _temp5461;}));
_temp5458;}); _temp5457;})); _temp5456;}); _temp5455;}); break; case 230:
_LL5454: yyval=( struct _xtunion_struct*)({ struct Cyc_BlockItem_tok_struct*
_temp5463=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp5463[ 0]=({ struct Cyc_BlockItem_tok_struct _temp5464; _temp5464.tag= Cyc_BlockItem_tok_tag;
_temp5464.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp5465=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp5465[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp5466; _temp5466.tag= Cyc_Parse_Stmt_bl_tag;
_temp5466.f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5467= yyvs; struct
_xtunion_struct** _temp5469= _temp5467.curr + yyvsp_offset; if( _temp5467.base
== 0? 1:( _temp5469 < _temp5467.base? 1: _temp5469 >= _temp5467.last_plus_one)){
_throw( Null_Exception);}* _temp5469;})); _temp5466;}); _temp5465;})); _temp5464;});
_temp5463;}); break; case 231: _LL5462: yyval=( struct _xtunion_struct*)({
struct Cyc_BlockItem_tok_struct* _temp5471=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp5471[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp5472; _temp5472.tag= Cyc_BlockItem_tok_tag;
_temp5472.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp5473=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp5473[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp5474; _temp5474.tag= Cyc_Parse_FnDecl_bl_tag;
_temp5474.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp5475= yyvs;
struct _xtunion_struct** _temp5477= _temp5475.curr + yyvsp_offset; if( _temp5475.base
== 0? 1:( _temp5477 < _temp5475.base? 1: _temp5477 >= _temp5475.last_plus_one)){
_throw( Null_Exception);}* _temp5477;})); _temp5474;}); _temp5473;})); _temp5472;});
_temp5471;}); break; case 232: _LL5470: yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5479=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5479[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5480; _temp5480.tag= Cyc_Stmt_tok_tag; _temp5480.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5481= yyvs; struct _xtunion_struct**
_temp5483= _temp5481.curr +( yyvsp_offset - 2); if( _temp5481.base == 0? 1:(
_temp5483 < _temp5481.base? 1: _temp5483 >= _temp5481.last_plus_one)){ _throw(
Null_Exception);}* _temp5483;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5484= yyvs; struct _xtunion_struct** _temp5486= _temp5484.curr +
yyvsp_offset; if( _temp5484.base == 0? 1:( _temp5486 < _temp5484.base? 1:
_temp5486 >= _temp5484.last_plus_one)){ _throw( Null_Exception);}* _temp5486;})),
Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5487= yyls; struct Cyc_Yyltype* _temp5489= _temp5487.curr +( yylsp_offset -
4); if( _temp5487.base == 0? 1:( _temp5489 < _temp5487.base? 1: _temp5489 >=
_temp5487.last_plus_one)){ _throw( Null_Exception);}* _temp5489;}).first_line,({
struct _tagged_ptr3 _temp5490= yyls; struct Cyc_Yyltype* _temp5492= _temp5490.curr
+ yylsp_offset; if( _temp5490.base == 0? 1:( _temp5492 < _temp5490.base? 1:
_temp5492 >= _temp5490.last_plus_one)){ _throw( Null_Exception);}* _temp5492;}).last_line));
_temp5480;}); _temp5479;}); break; case 233: _LL5478: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5494=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5494[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5495; _temp5495.tag= Cyc_Stmt_tok_tag; _temp5495.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5496= yyvs; struct _xtunion_struct**
_temp5498= _temp5496.curr +( yyvsp_offset - 4); if( _temp5496.base == 0? 1:(
_temp5498 < _temp5496.base? 1: _temp5498 >= _temp5496.last_plus_one)){ _throw(
Null_Exception);}* _temp5498;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5499= yyvs; struct _xtunion_struct** _temp5501= _temp5499.curr +(
yyvsp_offset - 2); if( _temp5499.base == 0? 1:( _temp5501 < _temp5499.base? 1:
_temp5501 >= _temp5499.last_plus_one)){ _throw( Null_Exception);}* _temp5501;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5502= yyvs; struct
_xtunion_struct** _temp5504= _temp5502.curr + yyvsp_offset; if( _temp5502.base
== 0? 1:( _temp5504 < _temp5502.base? 1: _temp5504 >= _temp5502.last_plus_one)){
_throw( Null_Exception);}* _temp5504;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5505= yyls; struct Cyc_Yyltype* _temp5507= _temp5505.curr +(
yylsp_offset - 6); if( _temp5505.base == 0? 1:( _temp5507 < _temp5505.base? 1:
_temp5507 >= _temp5505.last_plus_one)){ _throw( Null_Exception);}* _temp5507;}).first_line,({
struct _tagged_ptr3 _temp5508= yyls; struct Cyc_Yyltype* _temp5510= _temp5508.curr
+ yylsp_offset; if( _temp5508.base == 0? 1:( _temp5510 < _temp5508.base? 1:
_temp5510 >= _temp5508.last_plus_one)){ _throw( Null_Exception);}* _temp5510;}).last_line));
_temp5495;}); _temp5494;}); break; case 234: _LL5493: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5512=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5512[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5513; _temp5513.tag= Cyc_Stmt_tok_tag; _temp5513.f1= Cyc_Absyn_switch_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5514= yyvs; struct _xtunion_struct**
_temp5516= _temp5514.curr +( yyvsp_offset - 4); if( _temp5514.base == 0? 1:(
_temp5516 < _temp5514.base? 1: _temp5516 >= _temp5514.last_plus_one)){ _throw(
Null_Exception);}* _temp5516;})), Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5517= yyvs; struct _xtunion_struct** _temp5519= _temp5517.curr
+( yyvsp_offset - 1); if( _temp5517.base == 0? 1:( _temp5519 < _temp5517.base? 1:
_temp5519 >= _temp5517.last_plus_one)){ _throw( Null_Exception);}* _temp5519;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5520= yyls; struct Cyc_Yyltype*
_temp5522= _temp5520.curr +( yylsp_offset - 6); if( _temp5520.base == 0? 1:(
_temp5522 < _temp5520.base? 1: _temp5522 >= _temp5520.last_plus_one)){ _throw(
Null_Exception);}* _temp5522;}).first_line,({ struct _tagged_ptr3 _temp5523=
yyls; struct Cyc_Yyltype* _temp5525= _temp5523.curr + yylsp_offset; if(
_temp5523.base == 0? 1:( _temp5525 < _temp5523.base? 1: _temp5525 >= _temp5523.last_plus_one)){
_throw( Null_Exception);}* _temp5525;}).last_line)); _temp5513;}); _temp5512;});
break; case 235: _LL5511: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5527=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5527[ 0]=({ struct Cyc_Stmt_tok_struct _temp5528; _temp5528.tag= Cyc_Stmt_tok_tag;
_temp5528.f1= Cyc_Absyn_trycatch_stmt( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5529= yyvs; struct _xtunion_struct** _temp5531= _temp5529.curr +(
yyvsp_offset - 4); if( _temp5529.base == 0? 1:( _temp5531 < _temp5529.base? 1:
_temp5531 >= _temp5529.last_plus_one)){ _throw( Null_Exception);}* _temp5531;})),
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp5532= yyvs; struct
_xtunion_struct** _temp5534= _temp5532.curr +( yyvsp_offset - 1); if( _temp5532.base
== 0? 1:( _temp5534 < _temp5532.base? 1: _temp5534 >= _temp5532.last_plus_one)){
_throw( Null_Exception);}* _temp5534;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5535= yyls; struct Cyc_Yyltype* _temp5537= _temp5535.curr +(
yylsp_offset - 5); if( _temp5535.base == 0? 1:( _temp5537 < _temp5535.base? 1:
_temp5537 >= _temp5535.last_plus_one)){ _throw( Null_Exception);}* _temp5537;}).first_line,({
struct _tagged_ptr3 _temp5538= yyls; struct Cyc_Yyltype* _temp5540= _temp5538.curr
+ yylsp_offset; if( _temp5538.base == 0? 1:( _temp5540 < _temp5538.base? 1:
_temp5540 >= _temp5538.last_plus_one)){ _throw( Null_Exception);}* _temp5540;}).last_line));
_temp5528;}); _temp5527;}); break; case 236: _LL5526: yyval=( struct
_xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5542=( struct
Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5542[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5543; _temp5543.tag=
Cyc_SwitchClauseList_tok_tag; _temp5543.f1= 0; _temp5543;}); _temp5542;});
break; case 237: _LL5541: yyval=( struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp5545=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp5545[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp5546; _temp5546.tag= Cyc_SwitchClauseList_tok_tag; _temp5546.f1=({ struct
Cyc_List_List* _temp5547=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5547->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp5548=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5548->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5558= yyls; struct Cyc_Yyltype* _temp5560= _temp5558.curr +( yylsp_offset -
2); if( _temp5558.base == 0? 1:( _temp5560 < _temp5558.base? 1: _temp5560 >=
_temp5558.last_plus_one)){ _throw( Null_Exception);}* _temp5560;}).first_line,({
struct _tagged_ptr3 _temp5561= yyls; struct Cyc_Yyltype* _temp5563= _temp5561.curr
+( yylsp_offset - 2); if( _temp5561.base == 0? 1:( _temp5563 < _temp5561.base? 1:
_temp5563 >= _temp5561.last_plus_one)){ _throw( Null_Exception);}* _temp5563;}).last_line));
_temp5548->pat_vars= 0; _temp5548->where_clause= 0; _temp5548->body= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5555= yyvs; struct _xtunion_struct** _temp5557=
_temp5555.curr + yyvsp_offset; if( _temp5555.base == 0? 1:( _temp5557 <
_temp5555.base? 1: _temp5557 >= _temp5555.last_plus_one)){ _throw(
Null_Exception);}* _temp5557;})); _temp5548->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5549= yyls; struct Cyc_Yyltype* _temp5551= _temp5549.curr
+( yylsp_offset - 2); if( _temp5549.base == 0? 1:( _temp5551 < _temp5549.base? 1:
_temp5551 >= _temp5549.last_plus_one)){ _throw( Null_Exception);}* _temp5551;}).first_line,({
struct _tagged_ptr3 _temp5552= yyls; struct Cyc_Yyltype* _temp5554= _temp5552.curr
+ yylsp_offset; if( _temp5552.base == 0? 1:( _temp5554 < _temp5552.base? 1:
_temp5554 >= _temp5552.last_plus_one)){ _throw( Null_Exception);}* _temp5554;}).last_line);
_temp5548;}); _temp5547->tl= 0; _temp5547;}); _temp5546;}); _temp5545;}); break;
case 238: _LL5544: yyval=( struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp5565=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp5565[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp5566; _temp5566.tag= Cyc_SwitchClauseList_tok_tag; _temp5566.f1=({ struct
Cyc_List_List* _temp5567=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5567->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp5571=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5571->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5584= yyvs; struct _xtunion_struct** _temp5586=
_temp5584.curr +( yyvsp_offset - 2); if( _temp5584.base == 0? 1:( _temp5586 <
_temp5584.base? 1: _temp5586 >= _temp5584.last_plus_one)){ _throw(
Null_Exception);}* _temp5586;})); _temp5571->pat_vars= 0; _temp5571->where_clause=
0; _temp5571->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5578= yyls; struct Cyc_Yyltype* _temp5580= _temp5578.curr +(
yylsp_offset - 1); if( _temp5578.base == 0? 1:( _temp5580 < _temp5578.base? 1:
_temp5580 >= _temp5578.last_plus_one)){ _throw( Null_Exception);}* _temp5580;}).first_line,({
struct _tagged_ptr3 _temp5581= yyls; struct Cyc_Yyltype* _temp5583= _temp5581.curr
+( yylsp_offset - 1); if( _temp5581.base == 0? 1:( _temp5583 < _temp5581.base? 1:
_temp5583 >= _temp5581.last_plus_one)){ _throw( Null_Exception);}* _temp5583;}).last_line));
_temp5571->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5572=
yyls; struct Cyc_Yyltype* _temp5574= _temp5572.curr +( yylsp_offset - 3); if(
_temp5572.base == 0? 1:( _temp5574 < _temp5572.base? 1: _temp5574 >= _temp5572.last_plus_one)){
_throw( Null_Exception);}* _temp5574;}).first_line,({ struct _tagged_ptr3
_temp5575= yyls; struct Cyc_Yyltype* _temp5577= _temp5575.curr + yylsp_offset;
if( _temp5575.base == 0? 1:( _temp5577 < _temp5575.base? 1: _temp5577 >=
_temp5575.last_plus_one)){ _throw( Null_Exception);}* _temp5577;}).last_line);
_temp5571;}); _temp5567->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5568= yyvs; struct _xtunion_struct** _temp5570= _temp5568.curr
+ yyvsp_offset; if( _temp5568.base == 0? 1:( _temp5570 < _temp5568.base? 1:
_temp5570 >= _temp5568.last_plus_one)){ _throw( Null_Exception);}* _temp5570;}));
_temp5567;}); _temp5566;}); _temp5565;}); break; case 239: _LL5564: yyval=(
struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5588=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5588[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5589; _temp5589.tag=
Cyc_SwitchClauseList_tok_tag; _temp5589.f1=({ struct Cyc_List_List* _temp5590=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5590->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5594=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5594->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5604= yyvs; struct _xtunion_struct** _temp5606=
_temp5604.curr +( yyvsp_offset - 3); if( _temp5604.base == 0? 1:( _temp5606 <
_temp5604.base? 1: _temp5606 >= _temp5604.last_plus_one)){ _throw(
Null_Exception);}* _temp5606;})); _temp5594->pat_vars= 0; _temp5594->where_clause=
0; _temp5594->body= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5601= yyvs;
struct _xtunion_struct** _temp5603= _temp5601.curr +( yyvsp_offset - 1); if(
_temp5601.base == 0? 1:( _temp5603 < _temp5601.base? 1: _temp5603 >= _temp5601.last_plus_one)){
_throw( Null_Exception);}* _temp5603;})); _temp5594->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5595= yyls; struct Cyc_Yyltype* _temp5597= _temp5595.curr
+( yylsp_offset - 4); if( _temp5595.base == 0? 1:( _temp5597 < _temp5595.base? 1:
_temp5597 >= _temp5595.last_plus_one)){ _throw( Null_Exception);}* _temp5597;}).first_line,({
struct _tagged_ptr3 _temp5598= yyls; struct Cyc_Yyltype* _temp5600= _temp5598.curr
+( yylsp_offset - 1); if( _temp5598.base == 0? 1:( _temp5600 < _temp5598.base? 1:
_temp5600 >= _temp5598.last_plus_one)){ _throw( Null_Exception);}* _temp5600;}).last_line);
_temp5594;}); _temp5590->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5591= yyvs; struct _xtunion_struct** _temp5593= _temp5591.curr
+ yyvsp_offset; if( _temp5591.base == 0? 1:( _temp5593 < _temp5591.base? 1:
_temp5593 >= _temp5591.last_plus_one)){ _throw( Null_Exception);}* _temp5593;}));
_temp5590;}); _temp5589;}); _temp5588;}); break; case 240: _LL5587: yyval=(
struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5608=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5608[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5609; _temp5609.tag=
Cyc_SwitchClauseList_tok_tag; _temp5609.f1=({ struct Cyc_List_List* _temp5610=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5610->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5614=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5614->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5630= yyvs; struct _xtunion_struct** _temp5632=
_temp5630.curr +( yyvsp_offset - 4); if( _temp5630.base == 0? 1:( _temp5632 <
_temp5630.base? 1: _temp5632 >= _temp5630.last_plus_one)){ _throw(
Null_Exception);}* _temp5632;})); _temp5614->pat_vars= 0; _temp5614->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5627= yyvs;
struct _xtunion_struct** _temp5629= _temp5627.curr +( yyvsp_offset - 2); if(
_temp5627.base == 0? 1:( _temp5629 < _temp5627.base? 1: _temp5629 >= _temp5627.last_plus_one)){
_throw( Null_Exception);}* _temp5629;})); _temp5614->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5621= yyls; struct Cyc_Yyltype*
_temp5623= _temp5621.curr +( yylsp_offset - 1); if( _temp5621.base == 0? 1:(
_temp5623 < _temp5621.base? 1: _temp5623 >= _temp5621.last_plus_one)){ _throw(
Null_Exception);}* _temp5623;}).first_line,({ struct _tagged_ptr3 _temp5624=
yyls; struct Cyc_Yyltype* _temp5626= _temp5624.curr +( yylsp_offset - 1); if(
_temp5624.base == 0? 1:( _temp5626 < _temp5624.base? 1: _temp5626 >= _temp5624.last_plus_one)){
_throw( Null_Exception);}* _temp5626;}).last_line)); _temp5614->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5615= yyls; struct Cyc_Yyltype* _temp5617= _temp5615.curr
+( yylsp_offset - 5); if( _temp5615.base == 0? 1:( _temp5617 < _temp5615.base? 1:
_temp5617 >= _temp5615.last_plus_one)){ _throw( Null_Exception);}* _temp5617;}).first_line,({
struct _tagged_ptr3 _temp5618= yyls; struct Cyc_Yyltype* _temp5620= _temp5618.curr
+ yylsp_offset; if( _temp5618.base == 0? 1:( _temp5620 < _temp5618.base? 1:
_temp5620 >= _temp5618.last_plus_one)){ _throw( Null_Exception);}* _temp5620;}).last_line);
_temp5614;}); _temp5610->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5611= yyvs; struct _xtunion_struct** _temp5613= _temp5611.curr
+ yyvsp_offset; if( _temp5611.base == 0? 1:( _temp5613 < _temp5611.base? 1:
_temp5613 >= _temp5611.last_plus_one)){ _throw( Null_Exception);}* _temp5613;}));
_temp5610;}); _temp5609;}); _temp5608;}); break; case 241: _LL5607: yyval=(
struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5634=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5634[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5635; _temp5635.tag=
Cyc_SwitchClauseList_tok_tag; _temp5635.f1=({ struct Cyc_List_List* _temp5636=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5636->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5640=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5640->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5653= yyvs; struct _xtunion_struct** _temp5655=
_temp5653.curr +( yyvsp_offset - 5); if( _temp5653.base == 0? 1:( _temp5655 <
_temp5653.base? 1: _temp5655 >= _temp5653.last_plus_one)){ _throw(
Null_Exception);}* _temp5655;})); _temp5640->pat_vars= 0; _temp5640->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5650= yyvs;
struct _xtunion_struct** _temp5652= _temp5650.curr +( yyvsp_offset - 3); if(
_temp5650.base == 0? 1:( _temp5652 < _temp5650.base? 1: _temp5652 >= _temp5650.last_plus_one)){
_throw( Null_Exception);}* _temp5652;})); _temp5640->body= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5647= yyvs; struct _xtunion_struct** _temp5649=
_temp5647.curr +( yyvsp_offset - 1); if( _temp5647.base == 0? 1:( _temp5649 <
_temp5647.base? 1: _temp5649 >= _temp5647.last_plus_one)){ _throw(
Null_Exception);}* _temp5649;})); _temp5640->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5641= yyls; struct Cyc_Yyltype* _temp5643= _temp5641.curr
+( yylsp_offset - 6); if( _temp5641.base == 0? 1:( _temp5643 < _temp5641.base? 1:
_temp5643 >= _temp5641.last_plus_one)){ _throw( Null_Exception);}* _temp5643;}).first_line,({
struct _tagged_ptr3 _temp5644= yyls; struct Cyc_Yyltype* _temp5646= _temp5644.curr
+ yylsp_offset; if( _temp5644.base == 0? 1:( _temp5646 < _temp5644.base? 1:
_temp5646 >= _temp5644.last_plus_one)){ _throw( Null_Exception);}* _temp5646;}).last_line);
_temp5640;}); _temp5636->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5637= yyvs; struct _xtunion_struct** _temp5639= _temp5637.curr
+ yyvsp_offset; if( _temp5637.base == 0? 1:( _temp5639 < _temp5637.base? 1:
_temp5639 >= _temp5637.last_plus_one)){ _throw( Null_Exception);}* _temp5639;}));
_temp5636;}); _temp5635;}); _temp5634;}); break; case 242: _LL5633: yyval=(
struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5657=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5657[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5658; _temp5658.tag= Cyc_Stmt_tok_tag; _temp5658.f1= Cyc_Absyn_while_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5659= yyvs; struct _xtunion_struct**
_temp5661= _temp5659.curr +( yyvsp_offset - 2); if( _temp5659.base == 0? 1:(
_temp5661 < _temp5659.base? 1: _temp5661 >= _temp5659.last_plus_one)){ _throw(
Null_Exception);}* _temp5661;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5662= yyvs; struct _xtunion_struct** _temp5664= _temp5662.curr +
yyvsp_offset; if( _temp5662.base == 0? 1:( _temp5664 < _temp5662.base? 1:
_temp5664 >= _temp5662.last_plus_one)){ _throw( Null_Exception);}* _temp5664;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5665= yyls; struct Cyc_Yyltype*
_temp5667= _temp5665.curr +( yylsp_offset - 4); if( _temp5665.base == 0? 1:(
_temp5667 < _temp5665.base? 1: _temp5667 >= _temp5665.last_plus_one)){ _throw(
Null_Exception);}* _temp5667;}).first_line,({ struct _tagged_ptr3 _temp5668=
yyls; struct Cyc_Yyltype* _temp5670= _temp5668.curr + yylsp_offset; if(
_temp5668.base == 0? 1:( _temp5670 < _temp5668.base? 1: _temp5670 >= _temp5668.last_plus_one)){
_throw( Null_Exception);}* _temp5670;}).last_line)); _temp5658;}); _temp5657;});
break; case 243: _LL5656: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5672=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5672[ 0]=({ struct Cyc_Stmt_tok_struct _temp5673; _temp5673.tag= Cyc_Stmt_tok_tag;
_temp5673.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5674= yyvs; struct _xtunion_struct** _temp5676= _temp5674.curr +(
yyvsp_offset - 5); if( _temp5674.base == 0? 1:( _temp5676 < _temp5674.base? 1:
_temp5676 >= _temp5674.last_plus_one)){ _throw( Null_Exception);}* _temp5676;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5677= yyvs; struct _xtunion_struct**
_temp5679= _temp5677.curr +( yyvsp_offset - 2); if( _temp5677.base == 0? 1:(
_temp5679 < _temp5677.base? 1: _temp5679 >= _temp5677.last_plus_one)){ _throw(
Null_Exception);}* _temp5679;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5680= yyls; struct Cyc_Yyltype* _temp5682= _temp5680.curr +(
yylsp_offset - 6); if( _temp5680.base == 0? 1:( _temp5682 < _temp5680.base? 1:
_temp5682 >= _temp5680.last_plus_one)){ _throw( Null_Exception);}* _temp5682;}).first_line,({
struct _tagged_ptr3 _temp5683= yyls; struct Cyc_Yyltype* _temp5685= _temp5683.curr
+ yylsp_offset; if( _temp5683.base == 0? 1:( _temp5685 < _temp5683.base? 1:
_temp5685 >= _temp5683.last_plus_one)){ _throw( Null_Exception);}* _temp5685;}).last_line));
_temp5673;}); _temp5672;}); break; case 244: _LL5671: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5687=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5687[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5688; _temp5688.tag= Cyc_Stmt_tok_tag; _temp5688.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5689= yyvs; struct _xtunion_struct** _temp5691=
_temp5689.curr + yyvsp_offset; if( _temp5689.base == 0? 1:( _temp5691 <
_temp5689.base? 1: _temp5691 >= _temp5689.last_plus_one)){ _throw(
Null_Exception);}* _temp5691;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5692= yyls; struct Cyc_Yyltype* _temp5694= _temp5692.curr +(
yylsp_offset - 5); if( _temp5692.base == 0? 1:( _temp5694 < _temp5692.base? 1:
_temp5694 >= _temp5692.last_plus_one)){ _throw( Null_Exception);}* _temp5694;}).first_line,({
struct _tagged_ptr3 _temp5695= yyls; struct Cyc_Yyltype* _temp5697= _temp5695.curr
+ yylsp_offset; if( _temp5695.base == 0? 1:( _temp5697 < _temp5695.base? 1:
_temp5697 >= _temp5695.last_plus_one)){ _throw( Null_Exception);}* _temp5697;}).last_line));
_temp5688;}); _temp5687;}); break; case 245: _LL5686: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5699=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5699[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5700; _temp5700.tag= Cyc_Stmt_tok_tag; _temp5700.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5701= yyvs; struct _xtunion_struct** _temp5703= _temp5701.curr
+( yyvsp_offset - 2); if( _temp5701.base == 0? 1:( _temp5703 < _temp5701.base? 1:
_temp5703 >= _temp5701.last_plus_one)){ _throw( Null_Exception);}* _temp5703;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5704= yyvs; struct
_xtunion_struct** _temp5706= _temp5704.curr + yyvsp_offset; if( _temp5704.base
== 0? 1:( _temp5706 < _temp5704.base? 1: _temp5706 >= _temp5704.last_plus_one)){
_throw( Null_Exception);}* _temp5706;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5707= yyls; struct Cyc_Yyltype* _temp5709= _temp5707.curr +(
yylsp_offset - 6); if( _temp5707.base == 0? 1:( _temp5709 < _temp5707.base? 1:
_temp5709 >= _temp5707.last_plus_one)){ _throw( Null_Exception);}* _temp5709;}).first_line,({
struct _tagged_ptr3 _temp5710= yyls; struct Cyc_Yyltype* _temp5712= _temp5710.curr
+ yylsp_offset; if( _temp5710.base == 0? 1:( _temp5712 < _temp5710.base? 1:
_temp5712 >= _temp5710.last_plus_one)){ _throw( Null_Exception);}* _temp5712;}).last_line));
_temp5700;}); _temp5699;}); break; case 246: _LL5698: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5714=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5714[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5715; _temp5715.tag= Cyc_Stmt_tok_tag; _temp5715.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5716=
yyvs; struct _xtunion_struct** _temp5718= _temp5716.curr +( yyvsp_offset - 3);
if( _temp5716.base == 0? 1:( _temp5718 < _temp5716.base? 1: _temp5718 >=
_temp5716.last_plus_one)){ _throw( Null_Exception);}* _temp5718;})), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5719= yyvs; struct
_xtunion_struct** _temp5721= _temp5719.curr + yyvsp_offset; if( _temp5719.base
== 0? 1:( _temp5721 < _temp5719.base? 1: _temp5721 >= _temp5719.last_plus_one)){
_throw( Null_Exception);}* _temp5721;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5722= yyls; struct Cyc_Yyltype* _temp5724= _temp5722.curr +(
yylsp_offset - 6); if( _temp5722.base == 0? 1:( _temp5724 < _temp5722.base? 1:
_temp5724 >= _temp5722.last_plus_one)){ _throw( Null_Exception);}* _temp5724;}).first_line,({
struct _tagged_ptr3 _temp5725= yyls; struct Cyc_Yyltype* _temp5727= _temp5725.curr
+ yylsp_offset; if( _temp5725.base == 0? 1:( _temp5727 < _temp5725.base? 1:
_temp5727 >= _temp5725.last_plus_one)){ _throw( Null_Exception);}* _temp5727;}).last_line));
_temp5715;}); _temp5714;}); break; case 247: _LL5713: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5729=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5729[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5730; _temp5730.tag= Cyc_Stmt_tok_tag; _temp5730.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5731=
yyvs; struct _xtunion_struct** _temp5733= _temp5731.curr +( yyvsp_offset - 4);
if( _temp5731.base == 0? 1:( _temp5733 < _temp5731.base? 1: _temp5733 >=
_temp5731.last_plus_one)){ _throw( Null_Exception);}* _temp5733;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5734= yyvs; struct _xtunion_struct** _temp5736=
_temp5734.curr +( yyvsp_offset - 2); if( _temp5734.base == 0? 1:( _temp5736 <
_temp5734.base? 1: _temp5736 >= _temp5734.last_plus_one)){ _throw(
Null_Exception);}* _temp5736;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5737= yyvs; struct _xtunion_struct** _temp5739= _temp5737.curr +
yyvsp_offset; if( _temp5737.base == 0? 1:( _temp5739 < _temp5737.base? 1:
_temp5739 >= _temp5737.last_plus_one)){ _throw( Null_Exception);}* _temp5739;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5740= yyls; struct Cyc_Yyltype*
_temp5742= _temp5740.curr +( yylsp_offset - 7); if( _temp5740.base == 0? 1:(
_temp5742 < _temp5740.base? 1: _temp5742 >= _temp5740.last_plus_one)){ _throw(
Null_Exception);}* _temp5742;}).first_line,({ struct _tagged_ptr3 _temp5743=
yyls; struct Cyc_Yyltype* _temp5745= _temp5743.curr +( yylsp_offset - 1); if(
_temp5743.base == 0? 1:( _temp5745 < _temp5743.base? 1: _temp5745 >= _temp5743.last_plus_one)){
_throw( Null_Exception);}* _temp5745;}).last_line)); _temp5730;}); _temp5729;});
break; case 248: _LL5728: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5747=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5747[ 0]=({ struct Cyc_Stmt_tok_struct _temp5748; _temp5748.tag= Cyc_Stmt_tok_tag;
_temp5748.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5749= yyvs; struct _xtunion_struct** _temp5751= _temp5749.curr +(
yyvsp_offset - 4); if( _temp5749.base == 0? 1:( _temp5751 < _temp5749.base? 1:
_temp5751 >= _temp5749.last_plus_one)){ _throw( Null_Exception);}* _temp5751;})),
Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5752= yyvs; struct _xtunion_struct** _temp5754= _temp5752.curr
+ yyvsp_offset; if( _temp5752.base == 0? 1:( _temp5754 < _temp5752.base? 1:
_temp5754 >= _temp5752.last_plus_one)){ _throw( Null_Exception);}* _temp5754;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5755= yyls; struct Cyc_Yyltype*
_temp5757= _temp5755.curr +( yylsp_offset - 6); if( _temp5755.base == 0? 1:(
_temp5757 < _temp5755.base? 1: _temp5757 >= _temp5755.last_plus_one)){ _throw(
Null_Exception);}* _temp5757;}).first_line,({ struct _tagged_ptr3 _temp5758=
yyls; struct Cyc_Yyltype* _temp5760= _temp5758.curr + yylsp_offset; if(
_temp5758.base == 0? 1:( _temp5760 < _temp5758.base? 1: _temp5760 >= _temp5758.last_plus_one)){
_throw( Null_Exception);}* _temp5760;}).last_line)); _temp5748;}); _temp5747;});
break; case 249: _LL5746: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5762=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5762[ 0]=({ struct Cyc_Stmt_tok_struct _temp5763; _temp5763.tag= Cyc_Stmt_tok_tag;
_temp5763.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5764= yyvs; struct _xtunion_struct** _temp5766= _temp5764.curr +(
yyvsp_offset - 5); if( _temp5764.base == 0? 1:( _temp5766 < _temp5764.base? 1:
_temp5766 >= _temp5764.last_plus_one)){ _throw( Null_Exception);}* _temp5766;})),
Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5767= yyvs;
struct _xtunion_struct** _temp5769= _temp5767.curr +( yyvsp_offset - 2); if(
_temp5767.base == 0? 1:( _temp5769 < _temp5767.base? 1: _temp5769 >= _temp5767.last_plus_one)){
_throw( Null_Exception);}* _temp5769;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5770= yyvs; struct _xtunion_struct** _temp5772= _temp5770.curr
+ yyvsp_offset; if( _temp5770.base == 0? 1:( _temp5772 < _temp5770.base? 1:
_temp5772 >= _temp5770.last_plus_one)){ _throw( Null_Exception);}* _temp5772;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5773= yyls; struct Cyc_Yyltype*
_temp5775= _temp5773.curr +( yylsp_offset - 7); if( _temp5773.base == 0? 1:(
_temp5775 < _temp5773.base? 1: _temp5775 >= _temp5773.last_plus_one)){ _throw(
Null_Exception);}* _temp5775;}).first_line,({ struct _tagged_ptr3 _temp5776=
yyls; struct Cyc_Yyltype* _temp5778= _temp5776.curr + yylsp_offset; if(
_temp5776.base == 0? 1:( _temp5778 < _temp5776.base? 1: _temp5778 >= _temp5776.last_plus_one)){
_throw( Null_Exception);}* _temp5778;}).last_line)); _temp5763;}); _temp5762;});
break; case 250: _LL5761: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5780=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5780[ 0]=({ struct Cyc_Stmt_tok_struct _temp5781; _temp5781.tag= Cyc_Stmt_tok_tag;
_temp5781.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5782= yyvs; struct _xtunion_struct** _temp5784= _temp5782.curr +(
yyvsp_offset - 5); if( _temp5782.base == 0? 1:( _temp5784 < _temp5782.base? 1:
_temp5784 >= _temp5782.last_plus_one)){ _throw( Null_Exception);}* _temp5784;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5785= yyvs; struct _xtunion_struct**
_temp5787= _temp5785.curr +( yyvsp_offset - 3); if( _temp5785.base == 0? 1:(
_temp5787 < _temp5785.base? 1: _temp5787 >= _temp5785.last_plus_one)){ _throw(
Null_Exception);}* _temp5787;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5788= yyvs; struct _xtunion_struct** _temp5790=
_temp5788.curr + yyvsp_offset; if( _temp5788.base == 0? 1:( _temp5790 <
_temp5788.base? 1: _temp5790 >= _temp5788.last_plus_one)){ _throw(
Null_Exception);}* _temp5790;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5791= yyls; struct Cyc_Yyltype* _temp5793= _temp5791.curr +(
yylsp_offset - 7); if( _temp5791.base == 0? 1:( _temp5793 < _temp5791.base? 1:
_temp5793 >= _temp5791.last_plus_one)){ _throw( Null_Exception);}* _temp5793;}).first_line,({
struct _tagged_ptr3 _temp5794= yyls; struct Cyc_Yyltype* _temp5796= _temp5794.curr
+ yylsp_offset; if( _temp5794.base == 0? 1:( _temp5796 < _temp5794.base? 1:
_temp5796 >= _temp5794.last_plus_one)){ _throw( Null_Exception);}* _temp5796;}).last_line));
_temp5781;}); _temp5780;}); break; case 251: _LL5779: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5798=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5798[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5799; _temp5799.tag= Cyc_Stmt_tok_tag; _temp5799.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5800= yyvs; struct _xtunion_struct**
_temp5802= _temp5800.curr +( yyvsp_offset - 6); if( _temp5800.base == 0? 1:(
_temp5802 < _temp5800.base? 1: _temp5802 >= _temp5800.last_plus_one)){ _throw(
Null_Exception);}* _temp5802;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5803= yyvs; struct _xtunion_struct** _temp5805= _temp5803.curr +(
yyvsp_offset - 4); if( _temp5803.base == 0? 1:( _temp5805 < _temp5803.base? 1:
_temp5805 >= _temp5803.last_plus_one)){ _throw( Null_Exception);}* _temp5805;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5806= yyvs; struct _xtunion_struct**
_temp5808= _temp5806.curr +( yyvsp_offset - 2); if( _temp5806.base == 0? 1:(
_temp5808 < _temp5806.base? 1: _temp5808 >= _temp5806.last_plus_one)){ _throw(
Null_Exception);}* _temp5808;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5809= yyvs; struct _xtunion_struct** _temp5811= _temp5809.curr +
yyvsp_offset; if( _temp5809.base == 0? 1:( _temp5811 < _temp5809.base? 1:
_temp5811 >= _temp5809.last_plus_one)){ _throw( Null_Exception);}* _temp5811;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5812= yyls; struct Cyc_Yyltype*
_temp5814= _temp5812.curr +( yylsp_offset - 8); if( _temp5812.base == 0? 1:(
_temp5814 < _temp5812.base? 1: _temp5814 >= _temp5812.last_plus_one)){ _throw(
Null_Exception);}* _temp5814;}).first_line,({ struct _tagged_ptr3 _temp5815=
yyls; struct Cyc_Yyltype* _temp5817= _temp5815.curr + yylsp_offset; if(
_temp5815.base == 0? 1:( _temp5817 < _temp5815.base? 1: _temp5817 >= _temp5815.last_plus_one)){
_throw( Null_Exception);}* _temp5817;}).last_line)); _temp5799;}); _temp5798;});
break; case 252: _LL5797: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5819= yyvs; struct _xtunion_struct** _temp5821=
_temp5819.curr +( yyvsp_offset - 3); if( _temp5819.base == 0? 1:( _temp5821 <
_temp5819.base? 1: _temp5821 >= _temp5819.last_plus_one)){ _throw(
Null_Exception);}* _temp5821;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5822= yyvs; struct _xtunion_struct** _temp5824=
_temp5822.curr + yyvsp_offset; if( _temp5822.base == 0? 1:( _temp5824 <
_temp5822.base? 1: _temp5824 >= _temp5822.last_plus_one)){ _throw(
Null_Exception);}* _temp5824;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5825= yyls; struct Cyc_Yyltype* _temp5827= _temp5825.curr +(
yylsp_offset - 5); if( _temp5825.base == 0? 1:( _temp5827 < _temp5825.base? 1:
_temp5827 >= _temp5825.last_plus_one)){ _throw( Null_Exception);}* _temp5827;}).first_line,({
struct _tagged_ptr3 _temp5828= yyls; struct Cyc_Yyltype* _temp5830= _temp5828.curr
+ yylsp_offset; if( _temp5828.base == 0? 1:( _temp5830 < _temp5828.base? 1:
_temp5830 >= _temp5828.last_plus_one)){ _throw( Null_Exception);}* _temp5830;}).last_line));
yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5831=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5831[ 0]=({ struct Cyc_Stmt_tok_struct _temp5832; _temp5832.tag= Cyc_Stmt_tok_tag;
_temp5832.f1= Cyc_Parse_flatten_declarations( decls, s); _temp5832;}); _temp5831;});
break;} case 253: _LL5818: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5834= yyvs; struct _xtunion_struct** _temp5836=
_temp5834.curr +( yyvsp_offset - 4); if( _temp5834.base == 0? 1:( _temp5836 <
_temp5834.base? 1: _temp5836 >= _temp5834.last_plus_one)){ _throw(
Null_Exception);}* _temp5836;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5837=
yyvs; struct _xtunion_struct** _temp5839= _temp5837.curr +( yyvsp_offset - 3);
if( _temp5837.base == 0? 1:( _temp5839 < _temp5837.base? 1: _temp5839 >=
_temp5837.last_plus_one)){ _throw( Null_Exception);}* _temp5839;})), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5840= yyvs; struct
_xtunion_struct** _temp5842= _temp5840.curr + yyvsp_offset; if( _temp5840.base
== 0? 1:( _temp5842 < _temp5840.base? 1: _temp5842 >= _temp5840.last_plus_one)){
_throw( Null_Exception);}* _temp5842;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5843= yyls; struct Cyc_Yyltype* _temp5845= _temp5843.curr +(
yylsp_offset - 6); if( _temp5843.base == 0? 1:( _temp5845 < _temp5843.base? 1:
_temp5845 >= _temp5843.last_plus_one)){ _throw( Null_Exception);}* _temp5845;}).first_line,({
struct _tagged_ptr3 _temp5846= yyls; struct Cyc_Yyltype* _temp5848= _temp5846.curr
+ yylsp_offset; if( _temp5846.base == 0? 1:( _temp5848 < _temp5846.base? 1:
_temp5848 >= _temp5846.last_plus_one)){ _throw( Null_Exception);}* _temp5848;}).last_line));
yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5849=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5849[ 0]=({ struct Cyc_Stmt_tok_struct _temp5850; _temp5850.tag= Cyc_Stmt_tok_tag;
_temp5850.f1= Cyc_Parse_flatten_declarations( decls, s); _temp5850;}); _temp5849;});
break;} case 254: _LL5833: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5852= yyvs; struct _xtunion_struct** _temp5854=
_temp5852.curr +( yyvsp_offset - 4); if( _temp5852.base == 0? 1:( _temp5854 <
_temp5852.base? 1: _temp5854 >= _temp5852.last_plus_one)){ _throw(
Null_Exception);}* _temp5854;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5855= yyvs; struct _xtunion_struct** _temp5857= _temp5855.curr
+( yyvsp_offset - 2); if( _temp5855.base == 0? 1:( _temp5857 < _temp5855.base? 1:
_temp5857 >= _temp5855.last_plus_one)){ _throw( Null_Exception);}* _temp5857;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5858= yyvs; struct
_xtunion_struct** _temp5860= _temp5858.curr + yyvsp_offset; if( _temp5858.base
== 0? 1:( _temp5860 < _temp5858.base? 1: _temp5860 >= _temp5858.last_plus_one)){
_throw( Null_Exception);}* _temp5860;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5861= yyls; struct Cyc_Yyltype* _temp5863= _temp5861.curr +(
yylsp_offset - 6); if( _temp5861.base == 0? 1:( _temp5863 < _temp5861.base? 1:
_temp5863 >= _temp5861.last_plus_one)){ _throw( Null_Exception);}* _temp5863;}).first_line,({
struct _tagged_ptr3 _temp5864= yyls; struct Cyc_Yyltype* _temp5866= _temp5864.curr
+ yylsp_offset; if( _temp5864.base == 0? 1:( _temp5866 < _temp5864.base? 1:
_temp5866 >= _temp5864.last_plus_one)){ _throw( Null_Exception);}* _temp5866;}).last_line));
yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5867=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5867[ 0]=({ struct Cyc_Stmt_tok_struct _temp5868; _temp5868.tag= Cyc_Stmt_tok_tag;
_temp5868.f1= Cyc_Parse_flatten_declarations( decls, s); _temp5868;}); _temp5867;});
break;} case 255: _LL5851: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5870= yyvs; struct _xtunion_struct** _temp5872=
_temp5870.curr +( yyvsp_offset - 5); if( _temp5870.base == 0? 1:( _temp5872 <
_temp5870.base? 1: _temp5872 >= _temp5870.last_plus_one)){ _throw(
Null_Exception);}* _temp5872;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5873=
yyvs; struct _xtunion_struct** _temp5875= _temp5873.curr +( yyvsp_offset - 4);
if( _temp5873.base == 0? 1:( _temp5875 < _temp5873.base? 1: _temp5875 >=
_temp5873.last_plus_one)){ _throw( Null_Exception);}* _temp5875;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5876= yyvs; struct _xtunion_struct** _temp5878=
_temp5876.curr +( yyvsp_offset - 2); if( _temp5876.base == 0? 1:( _temp5878 <
_temp5876.base? 1: _temp5878 >= _temp5876.last_plus_one)){ _throw(
Null_Exception);}* _temp5878;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5879= yyvs; struct _xtunion_struct** _temp5881= _temp5879.curr +
yyvsp_offset; if( _temp5879.base == 0? 1:( _temp5881 < _temp5879.base? 1:
_temp5881 >= _temp5879.last_plus_one)){ _throw( Null_Exception);}* _temp5881;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5882= yyls; struct Cyc_Yyltype*
_temp5884= _temp5882.curr +( yylsp_offset - 7); if( _temp5882.base == 0? 1:(
_temp5884 < _temp5882.base? 1: _temp5884 >= _temp5882.last_plus_one)){ _throw(
Null_Exception);}* _temp5884;}).first_line,({ struct _tagged_ptr3 _temp5885=
yyls; struct Cyc_Yyltype* _temp5887= _temp5885.curr + yylsp_offset; if(
_temp5885.base == 0? 1:( _temp5887 < _temp5885.base? 1: _temp5887 >= _temp5885.last_plus_one)){
_throw( Null_Exception);}* _temp5887;}).last_line)); yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5888=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5888[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5889; _temp5889.tag= Cyc_Stmt_tok_tag; _temp5889.f1= Cyc_Parse_flatten_declarations(
decls, s); _temp5889;}); _temp5888;}); break;} case 256: _LL5869: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5891=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5891[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5892; _temp5892.tag= Cyc_Stmt_tok_tag; _temp5892.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_string* _temp5893=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp5893[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5894= yyvs; struct _xtunion_struct** _temp5896= _temp5894.curr
+( yyvsp_offset - 1); if( _temp5894.base == 0? 1:( _temp5896 < _temp5894.base? 1:
_temp5896 >= _temp5894.last_plus_one)){ _throw( Null_Exception);}* _temp5896;}));
_temp5893;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5897= yyls;
struct Cyc_Yyltype* _temp5899= _temp5897.curr +( yylsp_offset - 2); if(
_temp5897.base == 0? 1:( _temp5899 < _temp5897.base? 1: _temp5899 >= _temp5897.last_plus_one)){
_throw( Null_Exception);}* _temp5899;}).first_line,({ struct _tagged_ptr3
_temp5900= yyls; struct Cyc_Yyltype* _temp5902= _temp5900.curr +( yylsp_offset -
1); if( _temp5900.base == 0? 1:( _temp5902 < _temp5900.base? 1: _temp5902 >=
_temp5900.last_plus_one)){ _throw( Null_Exception);}* _temp5902;}).last_line));
_temp5892;}); _temp5891;}); break; case 257: _LL5890: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5904=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5904[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5905; _temp5905.tag= Cyc_Stmt_tok_tag; _temp5905.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5906= yyls; struct Cyc_Yyltype*
_temp5908= _temp5906.curr +( yylsp_offset - 1); if( _temp5906.base == 0? 1:(
_temp5908 < _temp5906.base? 1: _temp5908 >= _temp5906.last_plus_one)){ _throw(
Null_Exception);}* _temp5908;}).first_line,({ struct _tagged_ptr3 _temp5909=
yyls; struct Cyc_Yyltype* _temp5911= _temp5909.curr +( yylsp_offset - 1); if(
_temp5909.base == 0? 1:( _temp5911 < _temp5909.base? 1: _temp5911 >= _temp5909.last_plus_one)){
_throw( Null_Exception);}* _temp5911;}).last_line)); _temp5905;}); _temp5904;});
break; case 258: _LL5903: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5913=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5913[ 0]=({ struct Cyc_Stmt_tok_struct _temp5914; _temp5914.tag= Cyc_Stmt_tok_tag;
_temp5914.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5915= yyls; struct Cyc_Yyltype* _temp5917= _temp5915.curr +(
yylsp_offset - 1); if( _temp5915.base == 0? 1:( _temp5917 < _temp5915.base? 1:
_temp5917 >= _temp5915.last_plus_one)){ _throw( Null_Exception);}* _temp5917;}).first_line,({
struct _tagged_ptr3 _temp5918= yyls; struct Cyc_Yyltype* _temp5920= _temp5918.curr
+( yylsp_offset - 1); if( _temp5918.base == 0? 1:( _temp5920 < _temp5918.base? 1:
_temp5920 >= _temp5918.last_plus_one)){ _throw( Null_Exception);}* _temp5920;}).last_line));
_temp5914;}); _temp5913;}); break; case 259: _LL5912: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5922=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5922[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5923; _temp5923.tag= Cyc_Stmt_tok_tag; _temp5923.f1= Cyc_Absyn_return_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5924= yyls; struct
Cyc_Yyltype* _temp5926= _temp5924.curr +( yylsp_offset - 1); if( _temp5924.base
== 0? 1:( _temp5926 < _temp5924.base? 1: _temp5926 >= _temp5924.last_plus_one)){
_throw( Null_Exception);}* _temp5926;}).first_line,({ struct _tagged_ptr3
_temp5927= yyls; struct Cyc_Yyltype* _temp5929= _temp5927.curr +( yylsp_offset -
1); if( _temp5927.base == 0? 1:( _temp5929 < _temp5927.base? 1: _temp5929 >=
_temp5927.last_plus_one)){ _throw( Null_Exception);}* _temp5929;}).last_line));
_temp5923;}); _temp5922;}); break; case 260: _LL5921: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5931=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5931[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5932; _temp5932.tag= Cyc_Stmt_tok_tag; _temp5932.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5933= yyvs;
struct _xtunion_struct** _temp5935= _temp5933.curr +( yyvsp_offset - 1); if(
_temp5933.base == 0? 1:( _temp5935 < _temp5933.base? 1: _temp5935 >= _temp5933.last_plus_one)){
_throw( Null_Exception);}* _temp5935;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5936= yyls; struct Cyc_Yyltype* _temp5938= _temp5936.curr +(
yylsp_offset - 2); if( _temp5936.base == 0? 1:( _temp5938 < _temp5936.base? 1:
_temp5938 >= _temp5936.last_plus_one)){ _throw( Null_Exception);}* _temp5938;}).first_line,({
struct _tagged_ptr3 _temp5939= yyls; struct Cyc_Yyltype* _temp5941= _temp5939.curr
+( yylsp_offset - 1); if( _temp5939.base == 0? 1:( _temp5941 < _temp5939.base? 1:
_temp5941 >= _temp5939.last_plus_one)){ _throw( Null_Exception);}* _temp5941;}).last_line));
_temp5932;}); _temp5931;}); break; case 261: _LL5930: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5943=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5943[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5944; _temp5944.tag= Cyc_Stmt_tok_tag; _temp5944.f1= Cyc_Absyn_fallthru_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5945= yyls; struct
Cyc_Yyltype* _temp5947= _temp5945.curr +( yylsp_offset - 1); if( _temp5945.base
== 0? 1:( _temp5947 < _temp5945.base? 1: _temp5947 >= _temp5945.last_plus_one)){
_throw( Null_Exception);}* _temp5947;}).first_line,({ struct _tagged_ptr3
_temp5948= yyls; struct Cyc_Yyltype* _temp5950= _temp5948.curr +( yylsp_offset -
1); if( _temp5948.base == 0? 1:( _temp5950 < _temp5948.base? 1: _temp5950 >=
_temp5948.last_plus_one)){ _throw( Null_Exception);}* _temp5950;}).last_line));
_temp5944;}); _temp5943;}); break; case 262: _LL5942: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5952=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5952[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5953; _temp5953.tag= Cyc_Stmt_tok_tag; _temp5953.f1= Cyc_Absyn_fallthru_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5954= yyls; struct
Cyc_Yyltype* _temp5956= _temp5954.curr +( yylsp_offset - 3); if( _temp5954.base
== 0? 1:( _temp5956 < _temp5954.base? 1: _temp5956 >= _temp5954.last_plus_one)){
_throw( Null_Exception);}* _temp5956;}).first_line,({ struct _tagged_ptr3
_temp5957= yyls; struct Cyc_Yyltype* _temp5959= _temp5957.curr +( yylsp_offset -
3); if( _temp5957.base == 0? 1:( _temp5959 < _temp5957.base? 1: _temp5959 >=
_temp5957.last_plus_one)){ _throw( Null_Exception);}* _temp5959;}).last_line));
_temp5953;}); _temp5952;}); break; case 263: _LL5951: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5961=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5961[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5962; _temp5962.tag= Cyc_Stmt_tok_tag; _temp5962.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp5963= yyvs; struct
_xtunion_struct** _temp5965= _temp5963.curr +( yyvsp_offset - 2); if( _temp5963.base
== 0? 1:( _temp5965 < _temp5963.base? 1: _temp5965 >= _temp5963.last_plus_one)){
_throw( Null_Exception);}* _temp5965;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5966= yyls; struct Cyc_Yyltype* _temp5968= _temp5966.curr +(
yylsp_offset - 4); if( _temp5966.base == 0? 1:( _temp5968 < _temp5966.base? 1:
_temp5968 >= _temp5966.last_plus_one)){ _throw( Null_Exception);}* _temp5968;}).first_line,({
struct _tagged_ptr3 _temp5969= yyls; struct Cyc_Yyltype* _temp5971= _temp5969.curr
+( yylsp_offset - 4); if( _temp5969.base == 0? 1:( _temp5971 < _temp5969.base? 1:
_temp5971 >= _temp5969.last_plus_one)){ _throw( Null_Exception);}* _temp5971;}).last_line));
_temp5962;}); _temp5961;}); break; case 264: _LL5960: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5973=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5973[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5974; _temp5974.tag= Cyc_Pattern_tok_tag; _temp5974.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5975= yyls; struct Cyc_Yyltype* _temp5977= _temp5975.curr + yylsp_offset;
if( _temp5975.base == 0? 1:( _temp5977 < _temp5975.base? 1: _temp5977 >=
_temp5975.last_plus_one)){ _throw( Null_Exception);}* _temp5977;}).first_line,({
struct _tagged_ptr3 _temp5978= yyls; struct Cyc_Yyltype* _temp5980= _temp5978.curr
+ yylsp_offset; if( _temp5978.base == 0? 1:( _temp5980 < _temp5978.base? 1:
_temp5980 >= _temp5978.last_plus_one)){ _throw( Null_Exception);}* _temp5980;}).last_line));
_temp5974;}); _temp5973;}); break; case 265: _LL5972: yyval=({ struct
_tagged_ptr2 _temp5982= yyvs; struct _xtunion_struct** _temp5984= _temp5982.curr
+( yyvsp_offset - 1); if( _temp5982.base == 0? 1:( _temp5984 < _temp5982.base? 1:
_temp5984 >= _temp5982.last_plus_one)){ _throw( Null_Exception);}* _temp5984;});
break; case 266: _LL5981: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5986=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5986[ 0]=({ struct Cyc_Pattern_tok_struct _temp5987; _temp5987.tag= Cyc_Pattern_tok_tag;
_temp5987.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp5988=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp5988[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp5989; _temp5989.tag= Cyc_Absyn_Int_p_tag;
_temp5989.f1=( void*)(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp5990= yyvs;
struct _xtunion_struct** _temp5992= _temp5990.curr + yyvsp_offset; if( _temp5990.base
== 0? 1:( _temp5992 < _temp5990.base? 1: _temp5992 >= _temp5990.last_plus_one)){
_throw( Null_Exception);}* _temp5992;}))).f1; _temp5989.f2=(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5993= yyvs; struct _xtunion_struct** _temp5995=
_temp5993.curr + yyvsp_offset; if( _temp5993.base == 0? 1:( _temp5995 <
_temp5993.base? 1: _temp5995 >= _temp5993.last_plus_one)){ _throw(
Null_Exception);}* _temp5995;}))).f2; _temp5989;}); _temp5988;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5996= yyls; struct Cyc_Yyltype* _temp5998= _temp5996.curr
+ yylsp_offset; if( _temp5996.base == 0? 1:( _temp5998 < _temp5996.base? 1:
_temp5998 >= _temp5996.last_plus_one)){ _throw( Null_Exception);}* _temp5998;}).first_line,({
struct _tagged_ptr3 _temp5999= yyls; struct Cyc_Yyltype* _temp6001= _temp5999.curr
+ yylsp_offset; if( _temp5999.base == 0? 1:( _temp6001 < _temp5999.base? 1:
_temp6001 >= _temp5999.last_plus_one)){ _throw( Null_Exception);}* _temp6001;}).last_line));
_temp5987;}); _temp5986;}); break; case 267: _LL5985: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6003=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6003[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6004; _temp6004.tag= Cyc_Pattern_tok_tag; _temp6004.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp6005=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp6005[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp6006; _temp6006.tag= Cyc_Absyn_Int_p_tag; _temp6006.f1=( void*)(( void*)
Cyc_Absyn_Signed); _temp6006.f2= -(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2
_temp6007= yyvs; struct _xtunion_struct** _temp6009= _temp6007.curr +
yyvsp_offset; if( _temp6007.base == 0? 1:( _temp6009 < _temp6007.base? 1:
_temp6009 >= _temp6007.last_plus_one)){ _throw( Null_Exception);}* _temp6009;}))).f2;
_temp6006;}); _temp6005;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6010= yyls; struct Cyc_Yyltype* _temp6012= _temp6010.curr +( yylsp_offset -
1); if( _temp6010.base == 0? 1:( _temp6012 < _temp6010.base? 1: _temp6012 >=
_temp6010.last_plus_one)){ _throw( Null_Exception);}* _temp6012;}).first_line,({
struct _tagged_ptr3 _temp6013= yyls; struct Cyc_Yyltype* _temp6015= _temp6013.curr
+ yylsp_offset; if( _temp6013.base == 0? 1:( _temp6015 < _temp6013.base? 1:
_temp6015 >= _temp6013.last_plus_one)){ _throw( Null_Exception);}* _temp6015;}).last_line));
_temp6004;}); _temp6003;}); break; case 268: _LL6002: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6017=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6017[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6018; _temp6018.tag= Cyc_Pattern_tok_tag; _temp6018.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp6019=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp6019[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp6020; _temp6020.tag= Cyc_Absyn_Float_p_tag;
_temp6020.f1= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6021= yyvs;
struct _xtunion_struct** _temp6023= _temp6021.curr + yyvsp_offset; if( _temp6021.base
== 0? 1:( _temp6023 < _temp6021.base? 1: _temp6023 >= _temp6021.last_plus_one)){
_throw( Null_Exception);}* _temp6023;})); _temp6020;}); _temp6019;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6024= yyls; struct Cyc_Yyltype* _temp6026= _temp6024.curr
+ yylsp_offset; if( _temp6024.base == 0? 1:( _temp6026 < _temp6024.base? 1:
_temp6026 >= _temp6024.last_plus_one)){ _throw( Null_Exception);}* _temp6026;}).first_line,({
struct _tagged_ptr3 _temp6027= yyls; struct Cyc_Yyltype* _temp6029= _temp6027.curr
+ yylsp_offset; if( _temp6027.base == 0? 1:( _temp6029 < _temp6027.base? 1:
_temp6029 >= _temp6027.last_plus_one)){ _throw( Null_Exception);}* _temp6029;}).last_line));
_temp6018;}); _temp6017;}); break; case 269: _LL6016: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6031=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6031[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6032; _temp6032.tag= Cyc_Pattern_tok_tag; _temp6032.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Char_p_struct* _temp6033=( struct Cyc_Absyn_Char_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_p_struct)); _temp6033[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp6034; _temp6034.tag= Cyc_Absyn_Char_p_tag; _temp6034.f1= Cyc_yyget_Char_tok(({
struct _tagged_ptr2 _temp6035= yyvs; struct _xtunion_struct** _temp6037=
_temp6035.curr + yyvsp_offset; if( _temp6035.base == 0? 1:( _temp6037 <
_temp6035.base? 1: _temp6037 >= _temp6035.last_plus_one)){ _throw(
Null_Exception);}* _temp6037;})); _temp6034;}); _temp6033;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6038= yyls; struct Cyc_Yyltype* _temp6040= _temp6038.curr
+ yylsp_offset; if( _temp6038.base == 0? 1:( _temp6040 < _temp6038.base? 1:
_temp6040 >= _temp6038.last_plus_one)){ _throw( Null_Exception);}* _temp6040;}).first_line,({
struct _tagged_ptr3 _temp6041= yyls; struct Cyc_Yyltype* _temp6043= _temp6041.curr
+ yylsp_offset; if( _temp6041.base == 0? 1:( _temp6043 < _temp6041.base? 1:
_temp6043 >= _temp6041.last_plus_one)){ _throw( Null_Exception);}* _temp6043;}).last_line));
_temp6032;}); _temp6031;}); break; case 270: _LL6030: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6045=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6045[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6046; _temp6046.tag= Cyc_Pattern_tok_tag; _temp6046.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6047= yyls; struct Cyc_Yyltype* _temp6049= _temp6047.curr + yylsp_offset;
if( _temp6047.base == 0? 1:( _temp6049 < _temp6047.base? 1: _temp6049 >=
_temp6047.last_plus_one)){ _throw( Null_Exception);}* _temp6049;}).first_line,({
struct _tagged_ptr3 _temp6050= yyls; struct Cyc_Yyltype* _temp6052= _temp6050.curr
+ yylsp_offset; if( _temp6050.base == 0? 1:( _temp6052 < _temp6050.base? 1:
_temp6052 >= _temp6050.last_plus_one)){ _throw( Null_Exception);}* _temp6052;}).last_line));
_temp6046;}); _temp6045;}); break; case 271: _LL6044: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6054=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6054[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6055; _temp6055.tag= Cyc_Pattern_tok_tag; _temp6055.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownId_p_struct* _temp6056=( struct Cyc_Absyn_UnknownId_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct)); _temp6056[ 0]=({
struct Cyc_Absyn_UnknownId_p_struct _temp6057; _temp6057.tag= Cyc_Absyn_UnknownId_p_tag;
_temp6057.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6058= yyvs;
struct _xtunion_struct** _temp6060= _temp6058.curr + yyvsp_offset; if( _temp6058.base
== 0? 1:( _temp6060 < _temp6058.base? 1: _temp6060 >= _temp6058.last_plus_one)){
_throw( Null_Exception);}* _temp6060;})); _temp6057;}); _temp6056;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6061= yyls; struct Cyc_Yyltype* _temp6063= _temp6061.curr
+ yylsp_offset; if( _temp6061.base == 0? 1:( _temp6063 < _temp6061.base? 1:
_temp6063 >= _temp6061.last_plus_one)){ _throw( Null_Exception);}* _temp6063;}).first_line,({
struct _tagged_ptr3 _temp6064= yyls; struct Cyc_Yyltype* _temp6066= _temp6064.curr
+ yylsp_offset; if( _temp6064.base == 0? 1:( _temp6066 < _temp6064.base? 1:
_temp6066 >= _temp6064.last_plus_one)){ _throw( Null_Exception);}* _temp6066;}).last_line));
_temp6055;}); _temp6054;}); break; case 272: _LL6053: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6068=
yyls; struct Cyc_Yyltype* _temp6070= _temp6068.curr +( yylsp_offset - 3); if(
_temp6068.base == 0? 1:( _temp6070 < _temp6068.base? 1: _temp6070 >= _temp6068.last_plus_one)){
_throw( Null_Exception);}* _temp6070;}).first_line,({ struct _tagged_ptr3
_temp6071= yyls; struct Cyc_Yyltype* _temp6073= _temp6071.curr +( yylsp_offset -
3); if( _temp6071.base == 0? 1:( _temp6073 < _temp6071.base? 1: _temp6073 >=
_temp6071.last_plus_one)){ _throw( Null_Exception);}* _temp6073;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6074= yyvs; struct
_xtunion_struct** _temp6076= _temp6074.curr +( yyvsp_offset - 3); if( _temp6074.base
== 0? 1:( _temp6076 < _temp6074.base? 1: _temp6076 >= _temp6074.last_plus_one)){
_throw( Null_Exception);}* _temp6076;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_Pattern_tok_struct* _temp6077=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6077[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6078; _temp6078.tag= Cyc_Pattern_tok_tag; _temp6078.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownCall_p_struct* _temp6079=( struct Cyc_Absyn_UnknownCall_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownCall_p_struct)); _temp6079[ 0]=({
struct Cyc_Absyn_UnknownCall_p_struct _temp6080; _temp6080.tag= Cyc_Absyn_UnknownCall_p_tag;
_temp6080.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6081= yyvs;
struct _xtunion_struct** _temp6083= _temp6081.curr +( yyvsp_offset - 4); if(
_temp6081.base == 0? 1:( _temp6083 < _temp6081.base? 1: _temp6083 >= _temp6081.last_plus_one)){
_throw( Null_Exception);}* _temp6083;})); _temp6080.f2= tvs; _temp6080.f3= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp6084= yyvs; struct _xtunion_struct** _temp6086=
_temp6084.curr +( yyvsp_offset - 1); if( _temp6084.base == 0? 1:( _temp6086 <
_temp6084.base? 1: _temp6086 >= _temp6084.last_plus_one)){ _throw(
Null_Exception);}* _temp6086;})); _temp6080;}); _temp6079;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6087= yyls; struct Cyc_Yyltype* _temp6089= _temp6087.curr
+( yylsp_offset - 4); if( _temp6087.base == 0? 1:( _temp6089 < _temp6087.base? 1:
_temp6089 >= _temp6087.last_plus_one)){ _throw( Null_Exception);}* _temp6089;}).first_line,({
struct _tagged_ptr3 _temp6090= yyls; struct Cyc_Yyltype* _temp6092= _temp6090.curr
+ yylsp_offset; if( _temp6090.base == 0? 1:( _temp6092 < _temp6090.base? 1:
_temp6092 >= _temp6090.last_plus_one)){ _throw( Null_Exception);}* _temp6092;}).last_line));
_temp6078;}); _temp6077;}); break;} case 273: _LL6067: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6094=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6094[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6095; _temp6095.tag= Cyc_Pattern_tok_tag; _temp6095.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Tuple_p_struct* _temp6096=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp6096[ 0]=({ struct
Cyc_Absyn_Tuple_p_struct _temp6097; _temp6097.tag= Cyc_Absyn_Tuple_p_tag;
_temp6097.f1= Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp6098= yyvs;
struct _xtunion_struct** _temp6100= _temp6098.curr +( yyvsp_offset - 1); if(
_temp6098.base == 0? 1:( _temp6100 < _temp6098.base? 1: _temp6100 >= _temp6098.last_plus_one)){
_throw( Null_Exception);}* _temp6100;})); _temp6097;}); _temp6096;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6101= yyls; struct Cyc_Yyltype* _temp6103= _temp6101.curr
+( yylsp_offset - 3); if( _temp6101.base == 0? 1:( _temp6103 < _temp6101.base? 1:
_temp6103 >= _temp6101.last_plus_one)){ _throw( Null_Exception);}* _temp6103;}).first_line,({
struct _tagged_ptr3 _temp6104= yyls; struct Cyc_Yyltype* _temp6106= _temp6104.curr
+ yylsp_offset; if( _temp6104.base == 0? 1:( _temp6106 < _temp6104.base? 1:
_temp6106 >= _temp6104.last_plus_one)){ _throw( Null_Exception);}* _temp6106;}).last_line));
_temp6095;}); _temp6094;}); break; case 274: _LL6093: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6108=
yyls; struct Cyc_Yyltype* _temp6110= _temp6108.curr +( yylsp_offset - 2); if(
_temp6108.base == 0? 1:( _temp6110 < _temp6108.base? 1: _temp6110 >= _temp6108.last_plus_one)){
_throw( Null_Exception);}* _temp6110;}).first_line,({ struct _tagged_ptr3
_temp6111= yyls; struct Cyc_Yyltype* _temp6113= _temp6111.curr +( yylsp_offset -
2); if( _temp6111.base == 0? 1:( _temp6113 < _temp6111.base? 1: _temp6113 >=
_temp6111.last_plus_one)){ _throw( Null_Exception);}* _temp6113;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6114= yyvs; struct
_xtunion_struct** _temp6116= _temp6114.curr +( yyvsp_offset - 2); if( _temp6114.base
== 0? 1:( _temp6116 < _temp6114.base? 1: _temp6116 >= _temp6114.last_plus_one)){
_throw( Null_Exception);}* _temp6116;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_Pattern_tok_struct* _temp6117=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6117[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6118; _temp6118.tag= Cyc_Pattern_tok_tag; _temp6118.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp6119=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp6119[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp6120; _temp6120.tag= Cyc_Absyn_UnknownFields_p_tag;
_temp6120.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6121= yyvs;
struct _xtunion_struct** _temp6123= _temp6121.curr +( yyvsp_offset - 3); if(
_temp6121.base == 0? 1:( _temp6123 < _temp6121.base? 1: _temp6123 >= _temp6121.last_plus_one)){
_throw( Null_Exception);}* _temp6123;})); _temp6120.f2= tvs; _temp6120.f3= 0;
_temp6120;}); _temp6119;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6124= yyls; struct Cyc_Yyltype* _temp6126= _temp6124.curr +( yylsp_offset -
3); if( _temp6124.base == 0? 1:( _temp6126 < _temp6124.base? 1: _temp6126 >=
_temp6124.last_plus_one)){ _throw( Null_Exception);}* _temp6126;}).first_line,({
struct _tagged_ptr3 _temp6127= yyls; struct Cyc_Yyltype* _temp6129= _temp6127.curr
+ yylsp_offset; if( _temp6127.base == 0? 1:( _temp6129 < _temp6127.base? 1:
_temp6129 >= _temp6127.last_plus_one)){ _throw( Null_Exception);}* _temp6129;}).last_line));
_temp6118;}); _temp6117;}); break;} case 275: _LL6107: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6131=
yyls; struct Cyc_Yyltype* _temp6133= _temp6131.curr +( yylsp_offset - 3); if(
_temp6131.base == 0? 1:( _temp6133 < _temp6131.base? 1: _temp6133 >= _temp6131.last_plus_one)){
_throw( Null_Exception);}* _temp6133;}).first_line,({ struct _tagged_ptr3
_temp6134= yyls; struct Cyc_Yyltype* _temp6136= _temp6134.curr +( yylsp_offset -
3); if( _temp6134.base == 0? 1:( _temp6136 < _temp6134.base? 1: _temp6136 >=
_temp6134.last_plus_one)){ _throw( Null_Exception);}* _temp6136;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6137= yyvs; struct
_xtunion_struct** _temp6139= _temp6137.curr +( yyvsp_offset - 3); if( _temp6137.base
== 0? 1:( _temp6139 < _temp6137.base? 1: _temp6139 >= _temp6137.last_plus_one)){
_throw( Null_Exception);}* _temp6139;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_Pattern_tok_struct* _temp6140=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6140[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6141; _temp6141.tag= Cyc_Pattern_tok_tag; _temp6141.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp6142=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp6142[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp6143; _temp6143.tag= Cyc_Absyn_UnknownFields_p_tag;
_temp6143.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6144= yyvs;
struct _xtunion_struct** _temp6146= _temp6144.curr +( yyvsp_offset - 4); if(
_temp6144.base == 0? 1:( _temp6146 < _temp6144.base? 1: _temp6146 >= _temp6144.last_plus_one)){
_throw( Null_Exception);}* _temp6146;})); _temp6143.f2= tvs; _temp6143.f3= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp6147= yyvs; struct _xtunion_struct** _temp6149=
_temp6147.curr +( yyvsp_offset - 1); if( _temp6147.base == 0? 1:( _temp6149 <
_temp6147.base? 1: _temp6149 >= _temp6147.last_plus_one)){ _throw(
Null_Exception);}* _temp6149;})); _temp6143;}); _temp6142;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6150= yyls; struct Cyc_Yyltype* _temp6152= _temp6150.curr
+( yylsp_offset - 4); if( _temp6150.base == 0? 1:( _temp6152 < _temp6150.base? 1:
_temp6152 >= _temp6150.last_plus_one)){ _throw( Null_Exception);}* _temp6152;}).first_line,({
struct _tagged_ptr3 _temp6153= yyls; struct Cyc_Yyltype* _temp6155= _temp6153.curr
+ yylsp_offset; if( _temp6153.base == 0? 1:( _temp6155 < _temp6153.base? 1:
_temp6155 >= _temp6153.last_plus_one)){ _throw( Null_Exception);}* _temp6155;}).last_line));
_temp6141;}); _temp6140;}); break;} case 276: _LL6130: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6157=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6157[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6158; _temp6158.tag= Cyc_Pattern_tok_tag; _temp6158.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Pointer_p_struct* _temp6159=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp6159[ 0]=({ struct
Cyc_Absyn_Pointer_p_struct _temp6160; _temp6160.tag= Cyc_Absyn_Pointer_p_tag;
_temp6160.f1= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6161= yyvs;
struct _xtunion_struct** _temp6163= _temp6161.curr + yyvsp_offset; if( _temp6161.base
== 0? 1:( _temp6163 < _temp6161.base? 1: _temp6163 >= _temp6161.last_plus_one)){
_throw( Null_Exception);}* _temp6163;})); _temp6160;}); _temp6159;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6164= yyls; struct Cyc_Yyltype* _temp6166= _temp6164.curr
+( yylsp_offset - 1); if( _temp6164.base == 0? 1:( _temp6166 < _temp6164.base? 1:
_temp6166 >= _temp6164.last_plus_one)){ _throw( Null_Exception);}* _temp6166;}).first_line,({
struct _tagged_ptr3 _temp6167= yyls; struct Cyc_Yyltype* _temp6169= _temp6167.curr
+ yylsp_offset; if( _temp6167.base == 0? 1:( _temp6169 < _temp6167.base? 1:
_temp6169 >= _temp6167.last_plus_one)){ _throw( Null_Exception);}* _temp6169;}).last_line));
_temp6158;}); _temp6157;}); break; case 277: _LL6156: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6171=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6171[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6172; _temp6172.tag= Cyc_Pattern_tok_tag; _temp6172.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Reference_p_struct* _temp6173=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp6173[ 0]=({
struct Cyc_Absyn_Reference_p_struct _temp6174; _temp6174.tag= Cyc_Absyn_Reference_p_tag;
_temp6174.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp6175=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp6175->f1=( void*) Cyc_Absyn_Loc_n;
_temp6175->f2=({ struct _tagged_string* _temp6176=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp6176[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp6177= yyvs; struct _xtunion_struct** _temp6179=
_temp6177.curr + yyvsp_offset; if( _temp6177.base == 0? 1:( _temp6179 <
_temp6177.base? 1: _temp6179 >= _temp6177.last_plus_one)){ _throw(
Null_Exception);}* _temp6179;})); _temp6176;}); _temp6175;}),( void*) Cyc_Absyn_VoidType,
0); _temp6174;}); _temp6173;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6180= yyls; struct Cyc_Yyltype* _temp6182= _temp6180.curr +(
yylsp_offset - 1); if( _temp6180.base == 0? 1:( _temp6182 < _temp6180.base? 1:
_temp6182 >= _temp6180.last_plus_one)){ _throw( Null_Exception);}* _temp6182;}).first_line,({
struct _tagged_ptr3 _temp6183= yyls; struct Cyc_Yyltype* _temp6185= _temp6183.curr
+ yylsp_offset; if( _temp6183.base == 0? 1:( _temp6185 < _temp6183.base? 1:
_temp6185 >= _temp6183.last_plus_one)){ _throw( Null_Exception);}* _temp6185;}).last_line));
_temp6172;}); _temp6171;}); break; case 278: _LL6170: yyval=( struct
_xtunion_struct*)({ struct Cyc_PatternList_tok_struct* _temp6187=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp6187[ 0]=({ struct
Cyc_PatternList_tok_struct _temp6188; _temp6188.tag= Cyc_PatternList_tok_tag;
_temp6188.f1= 0; _temp6188;}); _temp6187;}); break; case 279: _LL6186: yyval=(
struct _xtunion_struct*)({ struct Cyc_PatternList_tok_struct* _temp6190=( struct
Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6190[ 0]=({ struct Cyc_PatternList_tok_struct _temp6191; _temp6191.tag= Cyc_PatternList_tok_tag;
_temp6191.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp6192= yyvs; struct
_xtunion_struct** _temp6194= _temp6192.curr + yyvsp_offset; if( _temp6192.base
== 0? 1:( _temp6194 < _temp6192.base? 1: _temp6194 >= _temp6192.last_plus_one)){
_throw( Null_Exception);}* _temp6194;}))); _temp6191;}); _temp6190;}); break;
case 280: _LL6189: yyval=( struct _xtunion_struct*)({ struct Cyc_PatternList_tok_struct*
_temp6196=( struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6196[ 0]=({ struct Cyc_PatternList_tok_struct _temp6197; _temp6197.tag= Cyc_PatternList_tok_tag;
_temp6197.f1=({ struct Cyc_List_List* _temp6198=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6198->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp6199= yyvs; struct _xtunion_struct** _temp6201=
_temp6199.curr + yyvsp_offset; if( _temp6199.base == 0? 1:( _temp6201 <
_temp6199.base? 1: _temp6201 >= _temp6199.last_plus_one)){ _throw(
Null_Exception);}* _temp6201;})); _temp6198->tl= 0; _temp6198;}); _temp6197;});
_temp6196;}); break; case 281: _LL6195: yyval=( struct _xtunion_struct*)({
struct Cyc_PatternList_tok_struct* _temp6203=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp6203[ 0]=({ struct
Cyc_PatternList_tok_struct _temp6204; _temp6204.tag= Cyc_PatternList_tok_tag;
_temp6204.f1=({ struct Cyc_List_List* _temp6205=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6205->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp6209= yyvs; struct _xtunion_struct** _temp6211=
_temp6209.curr + yyvsp_offset; if( _temp6209.base == 0? 1:( _temp6211 <
_temp6209.base? 1: _temp6211 >= _temp6209.last_plus_one)){ _throw(
Null_Exception);}* _temp6211;})); _temp6205->tl= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp6206= yyvs; struct _xtunion_struct** _temp6208=
_temp6206.curr +( yyvsp_offset - 2); if( _temp6206.base == 0? 1:( _temp6208 <
_temp6206.base? 1: _temp6208 >= _temp6206.last_plus_one)){ _throw(
Null_Exception);}* _temp6208;})); _temp6205;}); _temp6204;}); _temp6203;});
break; case 282: _LL6202: yyval=( struct _xtunion_struct*)({ struct Cyc_FieldPattern_tok_struct*
_temp6213=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6213[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6214; _temp6214.tag=
Cyc_FieldPattern_tok_tag; _temp6214.f1=({ struct _tuple17* _temp6215=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp6215->f1= 0; _temp6215->f2=
Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6216= yyvs; struct
_xtunion_struct** _temp6218= _temp6216.curr + yyvsp_offset; if( _temp6216.base
== 0? 1:( _temp6218 < _temp6216.base? 1: _temp6218 >= _temp6216.last_plus_one)){
_throw( Null_Exception);}* _temp6218;})); _temp6215;}); _temp6214;}); _temp6213;});
break; case 283: _LL6212: yyval=( struct _xtunion_struct*)({ struct Cyc_FieldPattern_tok_struct*
_temp6220=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6220[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6221; _temp6221.tag=
Cyc_FieldPattern_tok_tag; _temp6221.f1=({ struct _tuple17* _temp6222=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp6222->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp6226= yyvs; struct _xtunion_struct** _temp6228=
_temp6226.curr +( yyvsp_offset - 1); if( _temp6226.base == 0? 1:( _temp6228 <
_temp6226.base? 1: _temp6228 >= _temp6226.last_plus_one)){ _throw(
Null_Exception);}* _temp6228;})); _temp6222->f2= Cyc_yyget_Pattern_tok(({ struct
_tagged_ptr2 _temp6223= yyvs; struct _xtunion_struct** _temp6225= _temp6223.curr
+ yyvsp_offset; if( _temp6223.base == 0? 1:( _temp6225 < _temp6223.base? 1:
_temp6225 >= _temp6223.last_plus_one)){ _throw( Null_Exception);}* _temp6225;}));
_temp6222;}); _temp6221;}); _temp6220;}); break; case 284: _LL6219: yyval=(
struct _xtunion_struct*)({ struct Cyc_FieldPatternList_tok_struct* _temp6230=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp6230[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp6231; _temp6231.tag=
Cyc_FieldPatternList_tok_tag; _temp6231.f1=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok(({ struct
_tagged_ptr2 _temp6232= yyvs; struct _xtunion_struct** _temp6234= _temp6232.curr
+ yyvsp_offset; if( _temp6232.base == 0? 1:( _temp6234 < _temp6232.base? 1:
_temp6234 >= _temp6232.last_plus_one)){ _throw( Null_Exception);}* _temp6234;})));
_temp6231;}); _temp6230;}); break; case 285: _LL6229: yyval=( struct
_xtunion_struct*)({ struct Cyc_FieldPatternList_tok_struct* _temp6236=( struct
Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp6236[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp6237; _temp6237.tag=
Cyc_FieldPatternList_tok_tag; _temp6237.f1=({ struct Cyc_List_List* _temp6238=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp6238->hd=(
void*) Cyc_yyget_FieldPattern_tok(({ struct _tagged_ptr2 _temp6239= yyvs; struct
_xtunion_struct** _temp6241= _temp6239.curr + yyvsp_offset; if( _temp6239.base
== 0? 1:( _temp6241 < _temp6239.base? 1: _temp6241 >= _temp6239.last_plus_one)){
_throw( Null_Exception);}* _temp6241;})); _temp6238->tl= 0; _temp6238;});
_temp6237;}); _temp6236;}); break; case 286: _LL6235: yyval=( struct
_xtunion_struct*)({ struct Cyc_FieldPatternList_tok_struct* _temp6243=( struct
Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp6243[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp6244; _temp6244.tag=
Cyc_FieldPatternList_tok_tag; _temp6244.f1=({ struct Cyc_List_List* _temp6245=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp6245->hd=(
void*) Cyc_yyget_FieldPattern_tok(({ struct _tagged_ptr2 _temp6249= yyvs; struct
_xtunion_struct** _temp6251= _temp6249.curr + yyvsp_offset; if( _temp6249.base
== 0? 1:( _temp6251 < _temp6249.base? 1: _temp6251 >= _temp6249.last_plus_one)){
_throw( Null_Exception);}* _temp6251;})); _temp6245->tl= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp6246= yyvs; struct _xtunion_struct** _temp6248=
_temp6246.curr +( yyvsp_offset - 2); if( _temp6246.base == 0? 1:( _temp6248 <
_temp6246.base? 1: _temp6248 >= _temp6246.last_plus_one)){ _throw(
Null_Exception);}* _temp6248;})); _temp6245;}); _temp6244;}); _temp6243;});
break; case 287: _LL6242: yyval=({ struct _tagged_ptr2 _temp6253= yyvs; struct
_xtunion_struct** _temp6255= _temp6253.curr + yyvsp_offset; if( _temp6253.base
== 0? 1:( _temp6255 < _temp6253.base? 1: _temp6255 >= _temp6253.last_plus_one)){
_throw( Null_Exception);}* _temp6255;}); break; case 288: _LL6252: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6257=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6257[ 0]=({ struct Cyc_Exp_tok_struct
_temp6258; _temp6258.tag= Cyc_Exp_tok_tag; _temp6258.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6259= yyvs; struct _xtunion_struct** _temp6261=
_temp6259.curr +( yyvsp_offset - 2); if( _temp6259.base == 0? 1:( _temp6261 <
_temp6259.base? 1: _temp6261 >= _temp6259.last_plus_one)){ _throw(
Null_Exception);}* _temp6261;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6262= yyvs; struct _xtunion_struct** _temp6264= _temp6262.curr +
yyvsp_offset; if( _temp6262.base == 0? 1:( _temp6264 < _temp6262.base? 1:
_temp6264 >= _temp6262.last_plus_one)){ _throw( Null_Exception);}* _temp6264;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6265= yyls; struct Cyc_Yyltype*
_temp6267= _temp6265.curr +( yylsp_offset - 2); if( _temp6265.base == 0? 1:(
_temp6267 < _temp6265.base? 1: _temp6267 >= _temp6265.last_plus_one)){ _throw(
Null_Exception);}* _temp6267;}).first_line,({ struct _tagged_ptr3 _temp6268=
yyls; struct Cyc_Yyltype* _temp6270= _temp6268.curr + yylsp_offset; if(
_temp6268.base == 0? 1:( _temp6270 < _temp6268.base? 1: _temp6270 >= _temp6268.last_plus_one)){
_throw( Null_Exception);}* _temp6270;}).last_line)); _temp6258;}); _temp6257;});
break; case 289: _LL6256: yyval=({ struct _tagged_ptr2 _temp6272= yyvs; struct
_xtunion_struct** _temp6274= _temp6272.curr + yyvsp_offset; if( _temp6272.base
== 0? 1:( _temp6274 < _temp6272.base? 1: _temp6274 >= _temp6272.last_plus_one)){
_throw( Null_Exception);}* _temp6274;}); break; case 290: _LL6271: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6276=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6276[ 0]=({ struct Cyc_Exp_tok_struct
_temp6277; _temp6277.tag= Cyc_Exp_tok_tag; _temp6277.f1= Cyc_Absyn_assignop_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6278= yyvs; struct _xtunion_struct**
_temp6280= _temp6278.curr +( yyvsp_offset - 2); if( _temp6278.base == 0? 1:(
_temp6280 < _temp6278.base? 1: _temp6280 >= _temp6278.last_plus_one)){ _throw(
Null_Exception);}* _temp6280;})), Cyc_yyget_Primopopt_tok(({ struct _tagged_ptr2
_temp6281= yyvs; struct _xtunion_struct** _temp6283= _temp6281.curr +(
yyvsp_offset - 1); if( _temp6281.base == 0? 1:( _temp6283 < _temp6281.base? 1:
_temp6283 >= _temp6281.last_plus_one)){ _throw( Null_Exception);}* _temp6283;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6284= yyvs; struct _xtunion_struct**
_temp6286= _temp6284.curr + yyvsp_offset; if( _temp6284.base == 0? 1:( _temp6286
< _temp6284.base? 1: _temp6286 >= _temp6284.last_plus_one)){ _throw(
Null_Exception);}* _temp6286;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6287= yyls; struct Cyc_Yyltype* _temp6289= _temp6287.curr +(
yylsp_offset - 2); if( _temp6287.base == 0? 1:( _temp6289 < _temp6287.base? 1:
_temp6289 >= _temp6287.last_plus_one)){ _throw( Null_Exception);}* _temp6289;}).first_line,({
struct _tagged_ptr3 _temp6290= yyls; struct Cyc_Yyltype* _temp6292= _temp6290.curr
+ yylsp_offset; if( _temp6290.base == 0? 1:( _temp6292 < _temp6290.base? 1:
_temp6292 >= _temp6290.last_plus_one)){ _throw( Null_Exception);}* _temp6292;}).last_line));
_temp6277;}); _temp6276;}); break; case 291: _LL6275: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6294=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6294[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6295; _temp6295.tag= Cyc_Primopopt_tok_tag;
_temp6295.f1= 0; _temp6295;}); _temp6294;}); break; case 292: _LL6293: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6297=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6297[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6298; _temp6298.tag= Cyc_Primopopt_tok_tag;
_temp6298.f1=({ struct Cyc_Core_Opt* _temp6299=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6299->v=( void*)(( void*) Cyc_Absyn_Times);
_temp6299;}); _temp6298;}); _temp6297;}); break; case 293: _LL6296: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6301=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6301[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6302; _temp6302.tag= Cyc_Primopopt_tok_tag;
_temp6302.f1=({ struct Cyc_Core_Opt* _temp6303=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6303->v=( void*)(( void*) Cyc_Absyn_Div);
_temp6303;}); _temp6302;}); _temp6301;}); break; case 294: _LL6300: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6305=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6305[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6306; _temp6306.tag= Cyc_Primopopt_tok_tag;
_temp6306.f1=({ struct Cyc_Core_Opt* _temp6307=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6307->v=( void*)(( void*) Cyc_Absyn_Mod);
_temp6307;}); _temp6306;}); _temp6305;}); break; case 295: _LL6304: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6309=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6309[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6310; _temp6310.tag= Cyc_Primopopt_tok_tag;
_temp6310.f1=({ struct Cyc_Core_Opt* _temp6311=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6311->v=( void*)(( void*) Cyc_Absyn_Plus);
_temp6311;}); _temp6310;}); _temp6309;}); break; case 296: _LL6308: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6313=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6313[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6314; _temp6314.tag= Cyc_Primopopt_tok_tag;
_temp6314.f1=({ struct Cyc_Core_Opt* _temp6315=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6315->v=( void*)(( void*) Cyc_Absyn_Minus);
_temp6315;}); _temp6314;}); _temp6313;}); break; case 297: _LL6312: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6317=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6317[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6318; _temp6318.tag= Cyc_Primopopt_tok_tag;
_temp6318.f1=({ struct Cyc_Core_Opt* _temp6319=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6319->v=( void*)(( void*) Cyc_Absyn_Bitlshift);
_temp6319;}); _temp6318;}); _temp6317;}); break; case 298: _LL6316: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6321=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6321[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6322; _temp6322.tag= Cyc_Primopopt_tok_tag;
_temp6322.f1=({ struct Cyc_Core_Opt* _temp6323=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6323->v=( void*)(( void*) Cyc_Absyn_Bitlrshift);
_temp6323;}); _temp6322;}); _temp6321;}); break; case 299: _LL6320: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6325=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6325[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6326; _temp6326.tag= Cyc_Primopopt_tok_tag;
_temp6326.f1=({ struct Cyc_Core_Opt* _temp6327=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6327->v=( void*)(( void*) Cyc_Absyn_Bitand);
_temp6327;}); _temp6326;}); _temp6325;}); break; case 300: _LL6324: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6329=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6329[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6330; _temp6330.tag= Cyc_Primopopt_tok_tag;
_temp6330.f1=({ struct Cyc_Core_Opt* _temp6331=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6331->v=( void*)(( void*) Cyc_Absyn_Bitxor);
_temp6331;}); _temp6330;}); _temp6329;}); break; case 301: _LL6328: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6333=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6333[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6334; _temp6334.tag= Cyc_Primopopt_tok_tag;
_temp6334.f1=({ struct Cyc_Core_Opt* _temp6335=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6335->v=( void*)(( void*) Cyc_Absyn_Bitor);
_temp6335;}); _temp6334;}); _temp6333;}); break; case 302: _LL6332: yyval=({
struct _tagged_ptr2 _temp6337= yyvs; struct _xtunion_struct** _temp6339=
_temp6337.curr + yyvsp_offset; if( _temp6337.base == 0? 1:( _temp6339 <
_temp6337.base? 1: _temp6339 >= _temp6337.last_plus_one)){ _throw(
Null_Exception);}* _temp6339;}); break; case 303: _LL6336: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6341=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6341[ 0]=({ struct Cyc_Exp_tok_struct
_temp6342; _temp6342.tag= Cyc_Exp_tok_tag; _temp6342.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6343= yyvs; struct _xtunion_struct**
_temp6345= _temp6343.curr +( yyvsp_offset - 4); if( _temp6343.base == 0? 1:(
_temp6345 < _temp6343.base? 1: _temp6345 >= _temp6343.last_plus_one)){ _throw(
Null_Exception);}* _temp6345;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6346= yyvs; struct _xtunion_struct** _temp6348= _temp6346.curr +(
yyvsp_offset - 2); if( _temp6346.base == 0? 1:( _temp6348 < _temp6346.base? 1:
_temp6348 >= _temp6346.last_plus_one)){ _throw( Null_Exception);}* _temp6348;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6349= yyvs; struct _xtunion_struct**
_temp6351= _temp6349.curr + yyvsp_offset; if( _temp6349.base == 0? 1:( _temp6351
< _temp6349.base? 1: _temp6351 >= _temp6349.last_plus_one)){ _throw(
Null_Exception);}* _temp6351;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6352= yyls; struct Cyc_Yyltype* _temp6354= _temp6352.curr +(
yylsp_offset - 4); if( _temp6352.base == 0? 1:( _temp6354 < _temp6352.base? 1:
_temp6354 >= _temp6352.last_plus_one)){ _throw( Null_Exception);}* _temp6354;}).first_line,({
struct _tagged_ptr3 _temp6355= yyls; struct Cyc_Yyltype* _temp6357= _temp6355.curr
+ yylsp_offset; if( _temp6355.base == 0? 1:( _temp6357 < _temp6355.base? 1:
_temp6357 >= _temp6355.last_plus_one)){ _throw( Null_Exception);}* _temp6357;}).last_line));
_temp6342;}); _temp6341;}); break; case 304: _LL6340: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6359=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6359[ 0]=({ struct Cyc_Exp_tok_struct
_temp6360; _temp6360.tag= Cyc_Exp_tok_tag; _temp6360.f1= Cyc_Absyn_throw_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6361= yyvs; struct _xtunion_struct**
_temp6363= _temp6361.curr + yyvsp_offset; if( _temp6361.base == 0? 1:( _temp6363
< _temp6361.base? 1: _temp6363 >= _temp6361.last_plus_one)){ _throw(
Null_Exception);}* _temp6363;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6364= yyls; struct Cyc_Yyltype* _temp6366= _temp6364.curr +(
yylsp_offset - 1); if( _temp6364.base == 0? 1:( _temp6366 < _temp6364.base? 1:
_temp6366 >= _temp6364.last_plus_one)){ _throw( Null_Exception);}* _temp6366;}).first_line,({
struct _tagged_ptr3 _temp6367= yyls; struct Cyc_Yyltype* _temp6369= _temp6367.curr
+ yylsp_offset; if( _temp6367.base == 0? 1:( _temp6369 < _temp6367.base? 1:
_temp6369 >= _temp6367.last_plus_one)){ _throw( Null_Exception);}* _temp6369;}).last_line));
_temp6360;}); _temp6359;}); break; case 305: _LL6358: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6371=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6371[ 0]=({ struct Cyc_Exp_tok_struct
_temp6372; _temp6372.tag= Cyc_Exp_tok_tag; _temp6372.f1= Cyc_Absyn_New_exp( 0,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6373= yyvs; struct _xtunion_struct**
_temp6375= _temp6373.curr + yyvsp_offset; if( _temp6373.base == 0? 1:( _temp6375
< _temp6373.base? 1: _temp6375 >= _temp6373.last_plus_one)){ _throw(
Null_Exception);}* _temp6375;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6376= yyls; struct Cyc_Yyltype* _temp6378= _temp6376.curr +(
yylsp_offset - 1); if( _temp6376.base == 0? 1:( _temp6378 < _temp6376.base? 1:
_temp6378 >= _temp6376.last_plus_one)){ _throw( Null_Exception);}* _temp6378;}).first_line,({
struct _tagged_ptr3 _temp6379= yyls; struct Cyc_Yyltype* _temp6381= _temp6379.curr
+( yylsp_offset + 1); if( _temp6379.base == 0? 1:( _temp6381 < _temp6379.base? 1:
_temp6381 >= _temp6379.last_plus_one)){ _throw( Null_Exception);}* _temp6381;}).last_line));
_temp6372;}); _temp6371;}); break; case 306: _LL6370: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6383=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6383[ 0]=({ struct Cyc_Exp_tok_struct
_temp6384; _temp6384.tag= Cyc_Exp_tok_tag; _temp6384.f1= Cyc_Absyn_New_exp( 0,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6385= yyvs; struct _xtunion_struct**
_temp6387= _temp6385.curr + yyvsp_offset; if( _temp6385.base == 0? 1:( _temp6387
< _temp6385.base? 1: _temp6387 >= _temp6385.last_plus_one)){ _throw(
Null_Exception);}* _temp6387;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6388= yyls; struct Cyc_Yyltype* _temp6390= _temp6388.curr +(
yylsp_offset - 1); if( _temp6388.base == 0? 1:( _temp6390 < _temp6388.base? 1:
_temp6390 >= _temp6388.last_plus_one)){ _throw( Null_Exception);}* _temp6390;}).first_line,({
struct _tagged_ptr3 _temp6391= yyls; struct Cyc_Yyltype* _temp6393= _temp6391.curr
+( yylsp_offset + 1); if( _temp6391.base == 0? 1:( _temp6393 < _temp6391.base? 1:
_temp6393 >= _temp6391.last_plus_one)){ _throw( Null_Exception);}* _temp6393;}).last_line));
_temp6384;}); _temp6383;}); break; case 307: _LL6382: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6395=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6395[ 0]=({ struct Cyc_Exp_tok_struct
_temp6396; _temp6396.tag= Cyc_Exp_tok_tag; _temp6396.f1= Cyc_Absyn_New_exp((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6397= yyvs;
struct _xtunion_struct** _temp6399= _temp6397.curr +( yyvsp_offset - 2); if(
_temp6397.base == 0? 1:( _temp6399 < _temp6397.base? 1: _temp6399 >= _temp6397.last_plus_one)){
_throw( Null_Exception);}* _temp6399;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6400= yyvs; struct _xtunion_struct** _temp6402= _temp6400.curr
+ yyvsp_offset; if( _temp6400.base == 0? 1:( _temp6402 < _temp6400.base? 1:
_temp6402 >= _temp6400.last_plus_one)){ _throw( Null_Exception);}* _temp6402;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6403= yyls; struct Cyc_Yyltype*
_temp6405= _temp6403.curr +( yylsp_offset - 4); if( _temp6403.base == 0? 1:(
_temp6405 < _temp6403.base? 1: _temp6405 >= _temp6403.last_plus_one)){ _throw(
Null_Exception);}* _temp6405;}).first_line,({ struct _tagged_ptr3 _temp6406=
yyls; struct Cyc_Yyltype* _temp6408= _temp6406.curr + yylsp_offset; if(
_temp6406.base == 0? 1:( _temp6408 < _temp6406.base? 1: _temp6408 >= _temp6406.last_plus_one)){
_throw( Null_Exception);}* _temp6408;}).last_line)); _temp6396;}); _temp6395;});
break; case 308: _LL6394: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6410=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6410[ 0]=({ struct Cyc_Exp_tok_struct _temp6411; _temp6411.tag= Cyc_Exp_tok_tag;
_temp6411.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6412= yyvs; struct _xtunion_struct** _temp6414=
_temp6412.curr +( yyvsp_offset - 2); if( _temp6412.base == 0? 1:( _temp6414 <
_temp6412.base? 1: _temp6414 >= _temp6412.last_plus_one)){ _throw(
Null_Exception);}* _temp6414;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6415= yyvs; struct _xtunion_struct** _temp6417= _temp6415.curr +
yyvsp_offset; if( _temp6415.base == 0? 1:( _temp6417 < _temp6415.base? 1:
_temp6417 >= _temp6415.last_plus_one)){ _throw( Null_Exception);}* _temp6417;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6418= yyls; struct Cyc_Yyltype*
_temp6420= _temp6418.curr +( yylsp_offset - 4); if( _temp6418.base == 0? 1:(
_temp6420 < _temp6418.base? 1: _temp6420 >= _temp6418.last_plus_one)){ _throw(
Null_Exception);}* _temp6420;}).first_line,({ struct _tagged_ptr3 _temp6421=
yyls; struct Cyc_Yyltype* _temp6423= _temp6421.curr + yylsp_offset; if(
_temp6421.base == 0? 1:( _temp6423 < _temp6421.base? 1: _temp6423 >= _temp6421.last_plus_one)){
_throw( Null_Exception);}* _temp6423;}).last_line)); _temp6411;}); _temp6410;});
break; case 309: _LL6409: yyval=({ struct _tagged_ptr2 _temp6425= yyvs; struct
_xtunion_struct** _temp6427= _temp6425.curr + yyvsp_offset; if( _temp6425.base
== 0? 1:( _temp6427 < _temp6425.base? 1: _temp6427 >= _temp6425.last_plus_one)){
_throw( Null_Exception);}* _temp6427;}); break; case 310: _LL6424: yyval=({
struct _tagged_ptr2 _temp6429= yyvs; struct _xtunion_struct** _temp6431=
_temp6429.curr + yyvsp_offset; if( _temp6429.base == 0? 1:( _temp6431 <
_temp6429.base? 1: _temp6431 >= _temp6429.last_plus_one)){ _throw(
Null_Exception);}* _temp6431;}); break; case 311: _LL6428: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6433=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6433[ 0]=({ struct Cyc_Exp_tok_struct
_temp6434; _temp6434.tag= Cyc_Exp_tok_tag; _temp6434.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6435= yyvs; struct _xtunion_struct** _temp6437=
_temp6435.curr +( yyvsp_offset - 2); if( _temp6435.base == 0? 1:( _temp6437 <
_temp6435.base? 1: _temp6437 >= _temp6435.last_plus_one)){ _throw(
Null_Exception);}* _temp6437;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6438= yyvs; struct _xtunion_struct** _temp6440= _temp6438.curr +
yyvsp_offset; if( _temp6438.base == 0? 1:( _temp6440 < _temp6438.base? 1:
_temp6440 >= _temp6438.last_plus_one)){ _throw( Null_Exception);}* _temp6440;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6441= yyls; struct Cyc_Yyltype*
_temp6443= _temp6441.curr +( yylsp_offset - 2); if( _temp6441.base == 0? 1:(
_temp6443 < _temp6441.base? 1: _temp6443 >= _temp6441.last_plus_one)){ _throw(
Null_Exception);}* _temp6443;}).first_line,({ struct _tagged_ptr3 _temp6444=
yyls; struct Cyc_Yyltype* _temp6446= _temp6444.curr + yylsp_offset; if(
_temp6444.base == 0? 1:( _temp6446 < _temp6444.base? 1: _temp6446 >= _temp6444.last_plus_one)){
_throw( Null_Exception);}* _temp6446;}).last_line)); _temp6434;}); _temp6433;});
break; case 312: _LL6432: yyval=({ struct _tagged_ptr2 _temp6448= yyvs; struct
_xtunion_struct** _temp6450= _temp6448.curr + yyvsp_offset; if( _temp6448.base
== 0? 1:( _temp6450 < _temp6448.base? 1: _temp6450 >= _temp6448.last_plus_one)){
_throw( Null_Exception);}* _temp6450;}); break; case 313: _LL6447: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6452=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6452[ 0]=({ struct Cyc_Exp_tok_struct
_temp6453; _temp6453.tag= Cyc_Exp_tok_tag; _temp6453.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6454= yyvs; struct _xtunion_struct** _temp6456=
_temp6454.curr +( yyvsp_offset - 2); if( _temp6454.base == 0? 1:( _temp6456 <
_temp6454.base? 1: _temp6456 >= _temp6454.last_plus_one)){ _throw(
Null_Exception);}* _temp6456;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6457= yyvs; struct _xtunion_struct** _temp6459= _temp6457.curr +
yyvsp_offset; if( _temp6457.base == 0? 1:( _temp6459 < _temp6457.base? 1:
_temp6459 >= _temp6457.last_plus_one)){ _throw( Null_Exception);}* _temp6459;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6460= yyls; struct Cyc_Yyltype*
_temp6462= _temp6460.curr +( yylsp_offset - 2); if( _temp6460.base == 0? 1:(
_temp6462 < _temp6460.base? 1: _temp6462 >= _temp6460.last_plus_one)){ _throw(
Null_Exception);}* _temp6462;}).first_line,({ struct _tagged_ptr3 _temp6463=
yyls; struct Cyc_Yyltype* _temp6465= _temp6463.curr + yylsp_offset; if(
_temp6463.base == 0? 1:( _temp6465 < _temp6463.base? 1: _temp6465 >= _temp6463.last_plus_one)){
_throw( Null_Exception);}* _temp6465;}).last_line)); _temp6453;}); _temp6452;});
break; case 314: _LL6451: yyval=({ struct _tagged_ptr2 _temp6467= yyvs; struct
_xtunion_struct** _temp6469= _temp6467.curr + yyvsp_offset; if( _temp6467.base
== 0? 1:( _temp6469 < _temp6467.base? 1: _temp6469 >= _temp6467.last_plus_one)){
_throw( Null_Exception);}* _temp6469;}); break; case 315: _LL6466: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6471=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6471[ 0]=({ struct Cyc_Exp_tok_struct
_temp6472; _temp6472.tag= Cyc_Exp_tok_tag; _temp6472.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6473= yyvs;
struct _xtunion_struct** _temp6475= _temp6473.curr +( yyvsp_offset - 2); if(
_temp6473.base == 0? 1:( _temp6475 < _temp6473.base? 1: _temp6475 >= _temp6473.last_plus_one)){
_throw( Null_Exception);}* _temp6475;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6476= yyvs; struct _xtunion_struct** _temp6478= _temp6476.curr
+ yyvsp_offset; if( _temp6476.base == 0? 1:( _temp6478 < _temp6476.base? 1:
_temp6478 >= _temp6476.last_plus_one)){ _throw( Null_Exception);}* _temp6478;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6479= yyls; struct Cyc_Yyltype*
_temp6481= _temp6479.curr +( yylsp_offset - 2); if( _temp6479.base == 0? 1:(
_temp6481 < _temp6479.base? 1: _temp6481 >= _temp6479.last_plus_one)){ _throw(
Null_Exception);}* _temp6481;}).first_line,({ struct _tagged_ptr3 _temp6482=
yyls; struct Cyc_Yyltype* _temp6484= _temp6482.curr + yylsp_offset; if(
_temp6482.base == 0? 1:( _temp6484 < _temp6482.base? 1: _temp6484 >= _temp6482.last_plus_one)){
_throw( Null_Exception);}* _temp6484;}).last_line)); _temp6472;}); _temp6471;});
break; case 316: _LL6470: yyval=({ struct _tagged_ptr2 _temp6486= yyvs; struct
_xtunion_struct** _temp6488= _temp6486.curr + yyvsp_offset; if( _temp6486.base
== 0? 1:( _temp6488 < _temp6486.base? 1: _temp6488 >= _temp6486.last_plus_one)){
_throw( Null_Exception);}* _temp6488;}); break; case 317: _LL6485: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6490=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6490[ 0]=({ struct Cyc_Exp_tok_struct
_temp6491; _temp6491.tag= Cyc_Exp_tok_tag; _temp6491.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6492=
yyvs; struct _xtunion_struct** _temp6494= _temp6492.curr +( yyvsp_offset - 2);
if( _temp6492.base == 0? 1:( _temp6494 < _temp6492.base? 1: _temp6494 >=
_temp6492.last_plus_one)){ _throw( Null_Exception);}* _temp6494;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6495= yyvs; struct _xtunion_struct** _temp6497=
_temp6495.curr + yyvsp_offset; if( _temp6495.base == 0? 1:( _temp6497 <
_temp6495.base? 1: _temp6497 >= _temp6495.last_plus_one)){ _throw(
Null_Exception);}* _temp6497;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6498= yyls; struct Cyc_Yyltype* _temp6500= _temp6498.curr +(
yylsp_offset - 2); if( _temp6498.base == 0? 1:( _temp6500 < _temp6498.base? 1:
_temp6500 >= _temp6498.last_plus_one)){ _throw( Null_Exception);}* _temp6500;}).first_line,({
struct _tagged_ptr3 _temp6501= yyls; struct Cyc_Yyltype* _temp6503= _temp6501.curr
+ yylsp_offset; if( _temp6501.base == 0? 1:( _temp6503 < _temp6501.base? 1:
_temp6503 >= _temp6501.last_plus_one)){ _throw( Null_Exception);}* _temp6503;}).last_line));
_temp6491;}); _temp6490;}); break; case 318: _LL6489: yyval=({ struct
_tagged_ptr2 _temp6505= yyvs; struct _xtunion_struct** _temp6507= _temp6505.curr
+ yyvsp_offset; if( _temp6505.base == 0? 1:( _temp6507 < _temp6505.base? 1:
_temp6507 >= _temp6505.last_plus_one)){ _throw( Null_Exception);}* _temp6507;});
break; case 319: _LL6504: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6509=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6509[ 0]=({ struct Cyc_Exp_tok_struct _temp6510; _temp6510.tag= Cyc_Exp_tok_tag;
_temp6510.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6511= yyvs; struct _xtunion_struct** _temp6513=
_temp6511.curr +( yyvsp_offset - 2); if( _temp6511.base == 0? 1:( _temp6513 <
_temp6511.base? 1: _temp6513 >= _temp6511.last_plus_one)){ _throw(
Null_Exception);}* _temp6513;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6514= yyvs; struct _xtunion_struct** _temp6516= _temp6514.curr +
yyvsp_offset; if( _temp6514.base == 0? 1:( _temp6516 < _temp6514.base? 1:
_temp6516 >= _temp6514.last_plus_one)){ _throw( Null_Exception);}* _temp6516;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6517= yyls; struct Cyc_Yyltype*
_temp6519= _temp6517.curr +( yylsp_offset - 2); if( _temp6517.base == 0? 1:(
_temp6519 < _temp6517.base? 1: _temp6519 >= _temp6517.last_plus_one)){ _throw(
Null_Exception);}* _temp6519;}).first_line,({ struct _tagged_ptr3 _temp6520=
yyls; struct Cyc_Yyltype* _temp6522= _temp6520.curr + yylsp_offset; if(
_temp6520.base == 0? 1:( _temp6522 < _temp6520.base? 1: _temp6522 >= _temp6520.last_plus_one)){
_throw( Null_Exception);}* _temp6522;}).last_line)); _temp6510;}); _temp6509;});
break; case 320: _LL6508: yyval=({ struct _tagged_ptr2 _temp6524= yyvs; struct
_xtunion_struct** _temp6526= _temp6524.curr + yyvsp_offset; if( _temp6524.base
== 0? 1:( _temp6526 < _temp6524.base? 1: _temp6526 >= _temp6524.last_plus_one)){
_throw( Null_Exception);}* _temp6526;}); break; case 321: _LL6523: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6528=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6528[ 0]=({ struct Cyc_Exp_tok_struct
_temp6529; _temp6529.tag= Cyc_Exp_tok_tag; _temp6529.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6530= yyvs; struct _xtunion_struct** _temp6532=
_temp6530.curr +( yyvsp_offset - 2); if( _temp6530.base == 0? 1:( _temp6532 <
_temp6530.base? 1: _temp6532 >= _temp6530.last_plus_one)){ _throw(
Null_Exception);}* _temp6532;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6533= yyvs; struct _xtunion_struct** _temp6535= _temp6533.curr +
yyvsp_offset; if( _temp6533.base == 0? 1:( _temp6535 < _temp6533.base? 1:
_temp6535 >= _temp6533.last_plus_one)){ _throw( Null_Exception);}* _temp6535;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6536= yyls; struct Cyc_Yyltype*
_temp6538= _temp6536.curr +( yylsp_offset - 2); if( _temp6536.base == 0? 1:(
_temp6538 < _temp6536.base? 1: _temp6538 >= _temp6536.last_plus_one)){ _throw(
Null_Exception);}* _temp6538;}).first_line,({ struct _tagged_ptr3 _temp6539=
yyls; struct Cyc_Yyltype* _temp6541= _temp6539.curr + yylsp_offset; if(
_temp6539.base == 0? 1:( _temp6541 < _temp6539.base? 1: _temp6541 >= _temp6539.last_plus_one)){
_throw( Null_Exception);}* _temp6541;}).last_line)); _temp6529;}); _temp6528;});
break; case 322: _LL6527: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6543=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6543[ 0]=({ struct Cyc_Exp_tok_struct _temp6544; _temp6544.tag= Cyc_Exp_tok_tag;
_temp6544.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6545= yyvs; struct _xtunion_struct** _temp6547= _temp6545.curr +(
yyvsp_offset - 2); if( _temp6545.base == 0? 1:( _temp6547 < _temp6545.base? 1:
_temp6547 >= _temp6545.last_plus_one)){ _throw( Null_Exception);}* _temp6547;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6548= yyvs; struct _xtunion_struct**
_temp6550= _temp6548.curr + yyvsp_offset; if( _temp6548.base == 0? 1:( _temp6550
< _temp6548.base? 1: _temp6550 >= _temp6548.last_plus_one)){ _throw(
Null_Exception);}* _temp6550;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6551= yyls; struct Cyc_Yyltype* _temp6553= _temp6551.curr +(
yylsp_offset - 2); if( _temp6551.base == 0? 1:( _temp6553 < _temp6551.base? 1:
_temp6553 >= _temp6551.last_plus_one)){ _throw( Null_Exception);}* _temp6553;}).first_line,({
struct _tagged_ptr3 _temp6554= yyls; struct Cyc_Yyltype* _temp6556= _temp6554.curr
+ yylsp_offset; if( _temp6554.base == 0? 1:( _temp6556 < _temp6554.base? 1:
_temp6556 >= _temp6554.last_plus_one)){ _throw( Null_Exception);}* _temp6556;}).last_line));
_temp6544;}); _temp6543;}); break; case 323: _LL6542: yyval=({ struct
_tagged_ptr2 _temp6558= yyvs; struct _xtunion_struct** _temp6560= _temp6558.curr
+ yyvsp_offset; if( _temp6558.base == 0? 1:( _temp6560 < _temp6558.base? 1:
_temp6560 >= _temp6558.last_plus_one)){ _throw( Null_Exception);}* _temp6560;});
break; case 324: _LL6557: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6562=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6562[ 0]=({ struct Cyc_Exp_tok_struct _temp6563; _temp6563.tag= Cyc_Exp_tok_tag;
_temp6563.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6564= yyvs; struct _xtunion_struct** _temp6566= _temp6564.curr +(
yyvsp_offset - 2); if( _temp6564.base == 0? 1:( _temp6566 < _temp6564.base? 1:
_temp6566 >= _temp6564.last_plus_one)){ _throw( Null_Exception);}* _temp6566;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6567= yyvs; struct _xtunion_struct**
_temp6569= _temp6567.curr + yyvsp_offset; if( _temp6567.base == 0? 1:( _temp6569
< _temp6567.base? 1: _temp6569 >= _temp6567.last_plus_one)){ _throw(
Null_Exception);}* _temp6569;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6570= yyls; struct Cyc_Yyltype* _temp6572= _temp6570.curr +(
yylsp_offset - 2); if( _temp6570.base == 0? 1:( _temp6572 < _temp6570.base? 1:
_temp6572 >= _temp6570.last_plus_one)){ _throw( Null_Exception);}* _temp6572;}).first_line,({
struct _tagged_ptr3 _temp6573= yyls; struct Cyc_Yyltype* _temp6575= _temp6573.curr
+ yylsp_offset; if( _temp6573.base == 0? 1:( _temp6575 < _temp6573.base? 1:
_temp6575 >= _temp6573.last_plus_one)){ _throw( Null_Exception);}* _temp6575;}).last_line));
_temp6563;}); _temp6562;}); break; case 325: _LL6561: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6577=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6577[ 0]=({ struct Cyc_Exp_tok_struct
_temp6578; _temp6578.tag= Cyc_Exp_tok_tag; _temp6578.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6579= yyvs; struct _xtunion_struct** _temp6581=
_temp6579.curr +( yyvsp_offset - 2); if( _temp6579.base == 0? 1:( _temp6581 <
_temp6579.base? 1: _temp6581 >= _temp6579.last_plus_one)){ _throw(
Null_Exception);}* _temp6581;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6582= yyvs; struct _xtunion_struct** _temp6584= _temp6582.curr +
yyvsp_offset; if( _temp6582.base == 0? 1:( _temp6584 < _temp6582.base? 1:
_temp6584 >= _temp6582.last_plus_one)){ _throw( Null_Exception);}* _temp6584;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6585= yyls; struct Cyc_Yyltype*
_temp6587= _temp6585.curr +( yylsp_offset - 2); if( _temp6585.base == 0? 1:(
_temp6587 < _temp6585.base? 1: _temp6587 >= _temp6585.last_plus_one)){ _throw(
Null_Exception);}* _temp6587;}).first_line,({ struct _tagged_ptr3 _temp6588=
yyls; struct Cyc_Yyltype* _temp6590= _temp6588.curr + yylsp_offset; if(
_temp6588.base == 0? 1:( _temp6590 < _temp6588.base? 1: _temp6590 >= _temp6588.last_plus_one)){
_throw( Null_Exception);}* _temp6590;}).last_line)); _temp6578;}); _temp6577;});
break; case 326: _LL6576: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6592=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6592[ 0]=({ struct Cyc_Exp_tok_struct _temp6593; _temp6593.tag= Cyc_Exp_tok_tag;
_temp6593.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6594= yyvs; struct _xtunion_struct** _temp6596= _temp6594.curr +(
yyvsp_offset - 2); if( _temp6594.base == 0? 1:( _temp6596 < _temp6594.base? 1:
_temp6596 >= _temp6594.last_plus_one)){ _throw( Null_Exception);}* _temp6596;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6597= yyvs; struct _xtunion_struct**
_temp6599= _temp6597.curr + yyvsp_offset; if( _temp6597.base == 0? 1:( _temp6599
< _temp6597.base? 1: _temp6599 >= _temp6597.last_plus_one)){ _throw(
Null_Exception);}* _temp6599;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6600= yyls; struct Cyc_Yyltype* _temp6602= _temp6600.curr +(
yylsp_offset - 2); if( _temp6600.base == 0? 1:( _temp6602 < _temp6600.base? 1:
_temp6602 >= _temp6600.last_plus_one)){ _throw( Null_Exception);}* _temp6602;}).first_line,({
struct _tagged_ptr3 _temp6603= yyls; struct Cyc_Yyltype* _temp6605= _temp6603.curr
+ yylsp_offset; if( _temp6603.base == 0? 1:( _temp6605 < _temp6603.base? 1:
_temp6605 >= _temp6603.last_plus_one)){ _throw( Null_Exception);}* _temp6605;}).last_line));
_temp6593;}); _temp6592;}); break; case 327: _LL6591: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6607=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6607[ 0]=({ struct Cyc_Exp_tok_struct
_temp6608; _temp6608.tag= Cyc_Exp_tok_tag; _temp6608.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6609= yyvs; struct _xtunion_struct** _temp6611=
_temp6609.curr +( yyvsp_offset - 2); if( _temp6609.base == 0? 1:( _temp6611 <
_temp6609.base? 1: _temp6611 >= _temp6609.last_plus_one)){ _throw(
Null_Exception);}* _temp6611;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6612= yyvs; struct _xtunion_struct** _temp6614= _temp6612.curr +
yyvsp_offset; if( _temp6612.base == 0? 1:( _temp6614 < _temp6612.base? 1:
_temp6614 >= _temp6612.last_plus_one)){ _throw( Null_Exception);}* _temp6614;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6615= yyls; struct Cyc_Yyltype*
_temp6617= _temp6615.curr +( yylsp_offset - 2); if( _temp6615.base == 0? 1:(
_temp6617 < _temp6615.base? 1: _temp6617 >= _temp6615.last_plus_one)){ _throw(
Null_Exception);}* _temp6617;}).first_line,({ struct _tagged_ptr3 _temp6618=
yyls; struct Cyc_Yyltype* _temp6620= _temp6618.curr + yylsp_offset; if(
_temp6618.base == 0? 1:( _temp6620 < _temp6618.base? 1: _temp6620 >= _temp6618.last_plus_one)){
_throw( Null_Exception);}* _temp6620;}).last_line)); _temp6608;}); _temp6607;});
break; case 328: _LL6606: yyval=({ struct _tagged_ptr2 _temp6622= yyvs; struct
_xtunion_struct** _temp6624= _temp6622.curr + yyvsp_offset; if( _temp6622.base
== 0? 1:( _temp6624 < _temp6622.base? 1: _temp6624 >= _temp6622.last_plus_one)){
_throw( Null_Exception);}* _temp6624;}); break; case 329: _LL6621: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6626=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6626[ 0]=({ struct Cyc_Exp_tok_struct
_temp6627; _temp6627.tag= Cyc_Exp_tok_tag; _temp6627.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6628=
yyvs; struct _xtunion_struct** _temp6630= _temp6628.curr +( yyvsp_offset - 2);
if( _temp6628.base == 0? 1:( _temp6630 < _temp6628.base? 1: _temp6630 >=
_temp6628.last_plus_one)){ _throw( Null_Exception);}* _temp6630;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6631= yyvs; struct _xtunion_struct** _temp6633=
_temp6631.curr + yyvsp_offset; if( _temp6631.base == 0? 1:( _temp6633 <
_temp6631.base? 1: _temp6633 >= _temp6631.last_plus_one)){ _throw(
Null_Exception);}* _temp6633;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6634= yyls; struct Cyc_Yyltype* _temp6636= _temp6634.curr +(
yylsp_offset - 2); if( _temp6634.base == 0? 1:( _temp6636 < _temp6634.base? 1:
_temp6636 >= _temp6634.last_plus_one)){ _throw( Null_Exception);}* _temp6636;}).first_line,({
struct _tagged_ptr3 _temp6637= yyls; struct Cyc_Yyltype* _temp6639= _temp6637.curr
+ yylsp_offset; if( _temp6637.base == 0? 1:( _temp6639 < _temp6637.base? 1:
_temp6639 >= _temp6637.last_plus_one)){ _throw( Null_Exception);}* _temp6639;}).last_line));
_temp6627;}); _temp6626;}); break; case 330: _LL6625: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6641=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6641[ 0]=({ struct Cyc_Exp_tok_struct
_temp6642; _temp6642.tag= Cyc_Exp_tok_tag; _temp6642.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6643=
yyvs; struct _xtunion_struct** _temp6645= _temp6643.curr +( yyvsp_offset - 2);
if( _temp6643.base == 0? 1:( _temp6645 < _temp6643.base? 1: _temp6645 >=
_temp6643.last_plus_one)){ _throw( Null_Exception);}* _temp6645;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6646= yyvs; struct _xtunion_struct** _temp6648=
_temp6646.curr + yyvsp_offset; if( _temp6646.base == 0? 1:( _temp6648 <
_temp6646.base? 1: _temp6648 >= _temp6646.last_plus_one)){ _throw(
Null_Exception);}* _temp6648;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6649= yyls; struct Cyc_Yyltype* _temp6651= _temp6649.curr +(
yylsp_offset - 2); if( _temp6649.base == 0? 1:( _temp6651 < _temp6649.base? 1:
_temp6651 >= _temp6649.last_plus_one)){ _throw( Null_Exception);}* _temp6651;}).first_line,({
struct _tagged_ptr3 _temp6652= yyls; struct Cyc_Yyltype* _temp6654= _temp6652.curr
+ yylsp_offset; if( _temp6652.base == 0? 1:( _temp6654 < _temp6652.base? 1:
_temp6654 >= _temp6652.last_plus_one)){ _throw( Null_Exception);}* _temp6654;}).last_line));
_temp6642;}); _temp6641;}); break; case 331: _LL6640: yyval=({ struct
_tagged_ptr2 _temp6656= yyvs; struct _xtunion_struct** _temp6658= _temp6656.curr
+ yyvsp_offset; if( _temp6656.base == 0? 1:( _temp6658 < _temp6656.base? 1:
_temp6658 >= _temp6656.last_plus_one)){ _throw( Null_Exception);}* _temp6658;});
break; case 332: _LL6655: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6660=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6660[ 0]=({ struct Cyc_Exp_tok_struct _temp6661; _temp6661.tag= Cyc_Exp_tok_tag;
_temp6661.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6662= yyvs; struct _xtunion_struct** _temp6664=
_temp6662.curr +( yyvsp_offset - 2); if( _temp6662.base == 0? 1:( _temp6664 <
_temp6662.base? 1: _temp6664 >= _temp6662.last_plus_one)){ _throw(
Null_Exception);}* _temp6664;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6665= yyvs; struct _xtunion_struct** _temp6667= _temp6665.curr +
yyvsp_offset; if( _temp6665.base == 0? 1:( _temp6667 < _temp6665.base? 1:
_temp6667 >= _temp6665.last_plus_one)){ _throw( Null_Exception);}* _temp6667;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6668= yyls; struct Cyc_Yyltype*
_temp6670= _temp6668.curr +( yylsp_offset - 2); if( _temp6668.base == 0? 1:(
_temp6670 < _temp6668.base? 1: _temp6670 >= _temp6668.last_plus_one)){ _throw(
Null_Exception);}* _temp6670;}).first_line,({ struct _tagged_ptr3 _temp6671=
yyls; struct Cyc_Yyltype* _temp6673= _temp6671.curr + yylsp_offset; if(
_temp6671.base == 0? 1:( _temp6673 < _temp6671.base? 1: _temp6673 >= _temp6671.last_plus_one)){
_throw( Null_Exception);}* _temp6673;}).last_line)); _temp6661;}); _temp6660;});
break; case 333: _LL6659: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6675=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6675[ 0]=({ struct Cyc_Exp_tok_struct _temp6676; _temp6676.tag= Cyc_Exp_tok_tag;
_temp6676.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6677= yyvs; struct _xtunion_struct** _temp6679=
_temp6677.curr +( yyvsp_offset - 2); if( _temp6677.base == 0? 1:( _temp6679 <
_temp6677.base? 1: _temp6679 >= _temp6677.last_plus_one)){ _throw(
Null_Exception);}* _temp6679;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6680= yyvs; struct _xtunion_struct** _temp6682= _temp6680.curr +
yyvsp_offset; if( _temp6680.base == 0? 1:( _temp6682 < _temp6680.base? 1:
_temp6682 >= _temp6680.last_plus_one)){ _throw( Null_Exception);}* _temp6682;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6683= yyls; struct Cyc_Yyltype*
_temp6685= _temp6683.curr +( yylsp_offset - 2); if( _temp6683.base == 0? 1:(
_temp6685 < _temp6683.base? 1: _temp6685 >= _temp6683.last_plus_one)){ _throw(
Null_Exception);}* _temp6685;}).first_line,({ struct _tagged_ptr3 _temp6686=
yyls; struct Cyc_Yyltype* _temp6688= _temp6686.curr + yylsp_offset; if(
_temp6686.base == 0? 1:( _temp6688 < _temp6686.base? 1: _temp6688 >= _temp6686.last_plus_one)){
_throw( Null_Exception);}* _temp6688;}).last_line)); _temp6676;}); _temp6675;});
break; case 334: _LL6674: yyval=({ struct _tagged_ptr2 _temp6690= yyvs; struct
_xtunion_struct** _temp6692= _temp6690.curr + yyvsp_offset; if( _temp6690.base
== 0? 1:( _temp6692 < _temp6690.base? 1: _temp6692 >= _temp6690.last_plus_one)){
_throw( Null_Exception);}* _temp6692;}); break; case 335: _LL6689: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6694=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6694[ 0]=({ struct Cyc_Exp_tok_struct
_temp6695; _temp6695.tag= Cyc_Exp_tok_tag; _temp6695.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Times, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6696= yyvs;
struct _xtunion_struct** _temp6698= _temp6696.curr +( yyvsp_offset - 2); if(
_temp6696.base == 0? 1:( _temp6698 < _temp6696.base? 1: _temp6698 >= _temp6696.last_plus_one)){
_throw( Null_Exception);}* _temp6698;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6699= yyvs; struct _xtunion_struct** _temp6701= _temp6699.curr
+ yyvsp_offset; if( _temp6699.base == 0? 1:( _temp6701 < _temp6699.base? 1:
_temp6701 >= _temp6699.last_plus_one)){ _throw( Null_Exception);}* _temp6701;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6702= yyls; struct Cyc_Yyltype*
_temp6704= _temp6702.curr +( yylsp_offset - 2); if( _temp6702.base == 0? 1:(
_temp6704 < _temp6702.base? 1: _temp6704 >= _temp6702.last_plus_one)){ _throw(
Null_Exception);}* _temp6704;}).first_line,({ struct _tagged_ptr3 _temp6705=
yyls; struct Cyc_Yyltype* _temp6707= _temp6705.curr + yylsp_offset; if(
_temp6705.base == 0? 1:( _temp6707 < _temp6705.base? 1: _temp6707 >= _temp6705.last_plus_one)){
_throw( Null_Exception);}* _temp6707;}).last_line)); _temp6695;}); _temp6694;});
break; case 336: _LL6693: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6709=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6709[ 0]=({ struct Cyc_Exp_tok_struct _temp6710; _temp6710.tag= Cyc_Exp_tok_tag;
_temp6710.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6711= yyvs; struct _xtunion_struct** _temp6713=
_temp6711.curr +( yyvsp_offset - 2); if( _temp6711.base == 0? 1:( _temp6713 <
_temp6711.base? 1: _temp6713 >= _temp6711.last_plus_one)){ _throw(
Null_Exception);}* _temp6713;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6714= yyvs; struct _xtunion_struct** _temp6716= _temp6714.curr +
yyvsp_offset; if( _temp6714.base == 0? 1:( _temp6716 < _temp6714.base? 1:
_temp6716 >= _temp6714.last_plus_one)){ _throw( Null_Exception);}* _temp6716;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6717= yyls; struct Cyc_Yyltype*
_temp6719= _temp6717.curr +( yylsp_offset - 2); if( _temp6717.base == 0? 1:(
_temp6719 < _temp6717.base? 1: _temp6719 >= _temp6717.last_plus_one)){ _throw(
Null_Exception);}* _temp6719;}).first_line,({ struct _tagged_ptr3 _temp6720=
yyls; struct Cyc_Yyltype* _temp6722= _temp6720.curr + yylsp_offset; if(
_temp6720.base == 0? 1:( _temp6722 < _temp6720.base? 1: _temp6722 >= _temp6720.last_plus_one)){
_throw( Null_Exception);}* _temp6722;}).last_line)); _temp6710;}); _temp6709;});
break; case 337: _LL6708: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6724=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6724[ 0]=({ struct Cyc_Exp_tok_struct _temp6725; _temp6725.tag= Cyc_Exp_tok_tag;
_temp6725.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6726= yyvs; struct _xtunion_struct** _temp6728=
_temp6726.curr +( yyvsp_offset - 2); if( _temp6726.base == 0? 1:( _temp6728 <
_temp6726.base? 1: _temp6728 >= _temp6726.last_plus_one)){ _throw(
Null_Exception);}* _temp6728;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6729= yyvs; struct _xtunion_struct** _temp6731= _temp6729.curr +
yyvsp_offset; if( _temp6729.base == 0? 1:( _temp6731 < _temp6729.base? 1:
_temp6731 >= _temp6729.last_plus_one)){ _throw( Null_Exception);}* _temp6731;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6732= yyls; struct Cyc_Yyltype*
_temp6734= _temp6732.curr +( yylsp_offset - 2); if( _temp6732.base == 0? 1:(
_temp6734 < _temp6732.base? 1: _temp6734 >= _temp6732.last_plus_one)){ _throw(
Null_Exception);}* _temp6734;}).first_line,({ struct _tagged_ptr3 _temp6735=
yyls; struct Cyc_Yyltype* _temp6737= _temp6735.curr + yylsp_offset; if(
_temp6735.base == 0? 1:( _temp6737 < _temp6735.base? 1: _temp6737 >= _temp6735.last_plus_one)){
_throw( Null_Exception);}* _temp6737;}).last_line)); _temp6725;}); _temp6724;});
break; case 338: _LL6723: yyval=({ struct _tagged_ptr2 _temp6739= yyvs; struct
_xtunion_struct** _temp6741= _temp6739.curr + yyvsp_offset; if( _temp6739.base
== 0? 1:( _temp6741 < _temp6739.base? 1: _temp6741 >= _temp6739.last_plus_one)){
_throw( Null_Exception);}* _temp6741;}); break; case 339: _LL6738: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6743=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6743[ 0]=({ struct Cyc_Exp_tok_struct
_temp6744; _temp6744.tag= Cyc_Exp_tok_tag; _temp6744.f1= Cyc_Absyn_cast_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6745= yyvs; struct
_xtunion_struct** _temp6747= _temp6745.curr +( yyvsp_offset - 2); if( _temp6745.base
== 0? 1:( _temp6747 < _temp6745.base? 1: _temp6747 >= _temp6745.last_plus_one)){
_throw( Null_Exception);}* _temp6747;}))).f3, Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6748= yyvs; struct _xtunion_struct** _temp6750= _temp6748.curr
+ yyvsp_offset; if( _temp6748.base == 0? 1:( _temp6750 < _temp6748.base? 1:
_temp6750 >= _temp6748.last_plus_one)){ _throw( Null_Exception);}* _temp6750;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6751= yyls; struct Cyc_Yyltype*
_temp6753= _temp6751.curr +( yylsp_offset - 3); if( _temp6751.base == 0? 1:(
_temp6753 < _temp6751.base? 1: _temp6753 >= _temp6751.last_plus_one)){ _throw(
Null_Exception);}* _temp6753;}).first_line,({ struct _tagged_ptr3 _temp6754=
yyls; struct Cyc_Yyltype* _temp6756= _temp6754.curr + yylsp_offset; if(
_temp6754.base == 0? 1:( _temp6756 < _temp6754.base? 1: _temp6756 >= _temp6754.last_plus_one)){
_throw( Null_Exception);}* _temp6756;}).last_line)); _temp6744;}); _temp6743;});
break; case 340: _LL6742: yyval=({ struct _tagged_ptr2 _temp6758= yyvs; struct
_xtunion_struct** _temp6760= _temp6758.curr + yyvsp_offset; if( _temp6758.base
== 0? 1:( _temp6760 < _temp6758.base? 1: _temp6760 >= _temp6758.last_plus_one)){
_throw( Null_Exception);}* _temp6760;}); break; case 341: _LL6757: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6762=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6762[ 0]=({ struct Cyc_Exp_tok_struct
_temp6763; _temp6763.tag= Cyc_Exp_tok_tag; _temp6763.f1= Cyc_Absyn_pre_inc_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6764= yyvs; struct _xtunion_struct**
_temp6766= _temp6764.curr + yyvsp_offset; if( _temp6764.base == 0? 1:( _temp6766
< _temp6764.base? 1: _temp6766 >= _temp6764.last_plus_one)){ _throw(
Null_Exception);}* _temp6766;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6767= yyls; struct Cyc_Yyltype* _temp6769= _temp6767.curr +(
yylsp_offset - 1); if( _temp6767.base == 0? 1:( _temp6769 < _temp6767.base? 1:
_temp6769 >= _temp6767.last_plus_one)){ _throw( Null_Exception);}* _temp6769;}).first_line,({
struct _tagged_ptr3 _temp6770= yyls; struct Cyc_Yyltype* _temp6772= _temp6770.curr
+ yylsp_offset; if( _temp6770.base == 0? 1:( _temp6772 < _temp6770.base? 1:
_temp6772 >= _temp6770.last_plus_one)){ _throw( Null_Exception);}* _temp6772;}).last_line));
_temp6763;}); _temp6762;}); break; case 342: _LL6761: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6774=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6774[ 0]=({ struct Cyc_Exp_tok_struct
_temp6775; _temp6775.tag= Cyc_Exp_tok_tag; _temp6775.f1= Cyc_Absyn_pre_dec_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6776= yyvs; struct _xtunion_struct**
_temp6778= _temp6776.curr + yyvsp_offset; if( _temp6776.base == 0? 1:( _temp6778
< _temp6776.base? 1: _temp6778 >= _temp6776.last_plus_one)){ _throw(
Null_Exception);}* _temp6778;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6779= yyls; struct Cyc_Yyltype* _temp6781= _temp6779.curr +(
yylsp_offset - 1); if( _temp6779.base == 0? 1:( _temp6781 < _temp6779.base? 1:
_temp6781 >= _temp6779.last_plus_one)){ _throw( Null_Exception);}* _temp6781;}).first_line,({
struct _tagged_ptr3 _temp6782= yyls; struct Cyc_Yyltype* _temp6784= _temp6782.curr
+ yylsp_offset; if( _temp6782.base == 0? 1:( _temp6784 < _temp6782.base? 1:
_temp6784 >= _temp6782.last_plus_one)){ _throw( Null_Exception);}* _temp6784;}).last_line));
_temp6775;}); _temp6774;}); break; case 343: _LL6773: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6786=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6786[ 0]=({ struct Cyc_Exp_tok_struct
_temp6787; _temp6787.tag= Cyc_Exp_tok_tag; _temp6787.f1= Cyc_Absyn_address_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6788= yyvs; struct _xtunion_struct**
_temp6790= _temp6788.curr + yyvsp_offset; if( _temp6788.base == 0? 1:( _temp6790
< _temp6788.base? 1: _temp6790 >= _temp6788.last_plus_one)){ _throw(
Null_Exception);}* _temp6790;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6791= yyls; struct Cyc_Yyltype* _temp6793= _temp6791.curr +(
yylsp_offset - 1); if( _temp6791.base == 0? 1:( _temp6793 < _temp6791.base? 1:
_temp6793 >= _temp6791.last_plus_one)){ _throw( Null_Exception);}* _temp6793;}).first_line,({
struct _tagged_ptr3 _temp6794= yyls; struct Cyc_Yyltype* _temp6796= _temp6794.curr
+ yylsp_offset; if( _temp6794.base == 0? 1:( _temp6796 < _temp6794.base? 1:
_temp6796 >= _temp6794.last_plus_one)){ _throw( Null_Exception);}* _temp6796;}).last_line));
_temp6787;}); _temp6786;}); break; case 344: _LL6785: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6798=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6798[ 0]=({ struct Cyc_Exp_tok_struct
_temp6799; _temp6799.tag= Cyc_Exp_tok_tag; _temp6799.f1= Cyc_Absyn_deref_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6800= yyvs; struct _xtunion_struct**
_temp6802= _temp6800.curr + yyvsp_offset; if( _temp6800.base == 0? 1:( _temp6802
< _temp6800.base? 1: _temp6802 >= _temp6800.last_plus_one)){ _throw(
Null_Exception);}* _temp6802;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6803= yyls; struct Cyc_Yyltype* _temp6805= _temp6803.curr +(
yylsp_offset - 1); if( _temp6803.base == 0? 1:( _temp6805 < _temp6803.base? 1:
_temp6805 >= _temp6803.last_plus_one)){ _throw( Null_Exception);}* _temp6805;}).first_line,({
struct _tagged_ptr3 _temp6806= yyls; struct Cyc_Yyltype* _temp6808= _temp6806.curr
+ yylsp_offset; if( _temp6806.base == 0? 1:( _temp6808 < _temp6806.base? 1:
_temp6808 >= _temp6806.last_plus_one)){ _throw( Null_Exception);}* _temp6808;}).last_line));
_temp6799;}); _temp6798;}); break; case 345: _LL6797: yyval=({ struct
_tagged_ptr2 _temp6810= yyvs; struct _xtunion_struct** _temp6812= _temp6810.curr
+ yyvsp_offset; if( _temp6810.base == 0? 1:( _temp6812 < _temp6810.base? 1:
_temp6812 >= _temp6810.last_plus_one)){ _throw( Null_Exception);}* _temp6812;});
break; case 346: _LL6809: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6814=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6814[ 0]=({ struct Cyc_Exp_tok_struct _temp6815; _temp6815.tag= Cyc_Exp_tok_tag;
_temp6815.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(({ struct _tagged_ptr2
_temp6816= yyvs; struct _xtunion_struct** _temp6818= _temp6816.curr +(
yyvsp_offset - 1); if( _temp6816.base == 0? 1:( _temp6818 < _temp6816.base? 1:
_temp6818 >= _temp6816.last_plus_one)){ _throw( Null_Exception);}* _temp6818;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6819= yyvs; struct _xtunion_struct**
_temp6821= _temp6819.curr + yyvsp_offset; if( _temp6819.base == 0? 1:( _temp6821
< _temp6819.base? 1: _temp6821 >= _temp6819.last_plus_one)){ _throw(
Null_Exception);}* _temp6821;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6822= yyls; struct Cyc_Yyltype* _temp6824= _temp6822.curr +(
yylsp_offset - 1); if( _temp6822.base == 0? 1:( _temp6824 < _temp6822.base? 1:
_temp6824 >= _temp6822.last_plus_one)){ _throw( Null_Exception);}* _temp6824;}).first_line,({
struct _tagged_ptr3 _temp6825= yyls; struct Cyc_Yyltype* _temp6827= _temp6825.curr
+ yylsp_offset; if( _temp6825.base == 0? 1:( _temp6827 < _temp6825.base? 1:
_temp6827 >= _temp6825.last_plus_one)){ _throw( Null_Exception);}* _temp6827;}).last_line));
_temp6815;}); _temp6814;}); break; case 347: _LL6813: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6829=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6829[ 0]=({ struct Cyc_Exp_tok_struct
_temp6830; _temp6830.tag= Cyc_Exp_tok_tag; _temp6830.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6831= yyvs; struct
_xtunion_struct** _temp6833= _temp6831.curr +( yyvsp_offset - 1); if( _temp6831.base
== 0? 1:( _temp6833 < _temp6831.base? 1: _temp6833 >= _temp6831.last_plus_one)){
_throw( Null_Exception);}* _temp6833;}))).f3, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6834= yyls; struct Cyc_Yyltype* _temp6836= _temp6834.curr
+( yylsp_offset - 3); if( _temp6834.base == 0? 1:( _temp6836 < _temp6834.base? 1:
_temp6836 >= _temp6834.last_plus_one)){ _throw( Null_Exception);}* _temp6836;}).first_line,({
struct _tagged_ptr3 _temp6837= yyls; struct Cyc_Yyltype* _temp6839= _temp6837.curr
+ yylsp_offset; if( _temp6837.base == 0? 1:( _temp6839 < _temp6837.base? 1:
_temp6839 >= _temp6837.last_plus_one)){ _throw( Null_Exception);}* _temp6839;}).last_line));
_temp6830;}); _temp6829;}); break; case 348: _LL6828: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6841=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6841[ 0]=({ struct Cyc_Exp_tok_struct
_temp6842; _temp6842.tag= Cyc_Exp_tok_tag; _temp6842.f1= Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6843= yyvs; struct _xtunion_struct**
_temp6845= _temp6843.curr + yyvsp_offset; if( _temp6843.base == 0? 1:( _temp6845
< _temp6843.base? 1: _temp6845 >= _temp6843.last_plus_one)){ _throw(
Null_Exception);}* _temp6845;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6846= yyls; struct Cyc_Yyltype* _temp6848= _temp6846.curr +(
yylsp_offset - 1); if( _temp6846.base == 0? 1:( _temp6848 < _temp6846.base? 1:
_temp6848 >= _temp6846.last_plus_one)){ _throw( Null_Exception);}* _temp6848;}).first_line,({
struct _tagged_ptr3 _temp6849= yyls; struct Cyc_Yyltype* _temp6851= _temp6849.curr
+ yylsp_offset; if( _temp6849.base == 0? 1:( _temp6851 < _temp6849.base? 1:
_temp6851 >= _temp6849.last_plus_one)){ _throw( Null_Exception);}* _temp6851;}).last_line));
_temp6842;}); _temp6841;}); break; case 349: _LL6840: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6853=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6853[ 0]=({ struct Cyc_Exp_tok_struct
_temp6854; _temp6854.tag= Cyc_Exp_tok_tag; _temp6854.f1= Cyc_Absyn_primop_exp(
Cyc_yyget_Primop_tok(({ struct _tagged_ptr2 _temp6855= yyvs; struct
_xtunion_struct** _temp6857= _temp6855.curr +( yyvsp_offset - 3); if( _temp6855.base
== 0? 1:( _temp6857 < _temp6855.base? 1: _temp6857 >= _temp6855.last_plus_one)){
_throw( Null_Exception);}* _temp6857;})), Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp6858= yyvs; struct _xtunion_struct** _temp6860= _temp6858.curr
+( yyvsp_offset - 1); if( _temp6858.base == 0? 1:( _temp6860 < _temp6858.base? 1:
_temp6860 >= _temp6858.last_plus_one)){ _throw( Null_Exception);}* _temp6860;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6861= yyls; struct Cyc_Yyltype*
_temp6863= _temp6861.curr +( yylsp_offset - 3); if( _temp6861.base == 0? 1:(
_temp6863 < _temp6861.base? 1: _temp6863 >= _temp6861.last_plus_one)){ _throw(
Null_Exception);}* _temp6863;}).first_line,({ struct _tagged_ptr3 _temp6864=
yyls; struct Cyc_Yyltype* _temp6866= _temp6864.curr + yylsp_offset; if(
_temp6864.base == 0? 1:( _temp6866 < _temp6864.base? 1: _temp6866 >= _temp6864.last_plus_one)){
_throw( Null_Exception);}* _temp6866;}).last_line)); _temp6854;}); _temp6853;});
break; case 350: _LL6852: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6868=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6868[ 0]=({ struct Cyc_Exp_tok_struct _temp6869; _temp6869.tag= Cyc_Exp_tok_tag;
_temp6869.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp6870=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp6870[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp6871; _temp6871.tag= Cyc_Absyn_Malloc_e_tag;
_temp6871.f1= 0; _temp6871.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp6872= yyvs; struct _xtunion_struct** _temp6874=
_temp6872.curr +( yyvsp_offset - 2); if( _temp6872.base == 0? 1:( _temp6874 <
_temp6872.base? 1: _temp6874 >= _temp6872.last_plus_one)){ _throw(
Null_Exception);}* _temp6874;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6875= yyls; struct Cyc_Yyltype* _temp6877= _temp6875.curr +(
yylsp_offset - 2); if( _temp6875.base == 0? 1:( _temp6877 < _temp6875.base? 1:
_temp6877 >= _temp6875.last_plus_one)){ _throw( Null_Exception);}* _temp6877;}).first_line,({
struct _tagged_ptr3 _temp6878= yyls; struct Cyc_Yyltype* _temp6880= _temp6878.curr
+( yylsp_offset - 2); if( _temp6878.base == 0? 1:( _temp6880 < _temp6878.base? 1:
_temp6880 >= _temp6878.last_plus_one)){ _throw( Null_Exception);}* _temp6880;}).last_line));
_temp6871;}); _temp6870;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6881= yyls; struct Cyc_Yyltype* _temp6883= _temp6881.curr +( yylsp_offset -
6); if( _temp6881.base == 0? 1:( _temp6883 < _temp6881.base? 1: _temp6883 >=
_temp6881.last_plus_one)){ _throw( Null_Exception);}* _temp6883;}).first_line,({
struct _tagged_ptr3 _temp6884= yyls; struct Cyc_Yyltype* _temp6886= _temp6884.curr
+ yylsp_offset; if( _temp6884.base == 0? 1:( _temp6886 < _temp6884.base? 1:
_temp6886 >= _temp6884.last_plus_one)){ _throw( Null_Exception);}* _temp6886;}).last_line));
_temp6869;}); _temp6868;}); break; case 351: _LL6867: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6888=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6888[ 0]=({ struct Cyc_Exp_tok_struct
_temp6889; _temp6889.tag= Cyc_Exp_tok_tag; _temp6889.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Malloc_e_struct* _temp6890=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp6890[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp6891; _temp6891.tag= Cyc_Absyn_Malloc_e_tag;
_temp6891.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6892= yyvs; struct _xtunion_struct** _temp6894= _temp6892.curr +(
yyvsp_offset - 6); if( _temp6892.base == 0? 1:( _temp6894 < _temp6892.base? 1:
_temp6894 >= _temp6892.last_plus_one)){ _throw( Null_Exception);}* _temp6894;}));
_temp6891.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp6895= yyvs; struct _xtunion_struct** _temp6897=
_temp6895.curr +( yyvsp_offset - 2); if( _temp6895.base == 0? 1:( _temp6897 <
_temp6895.base? 1: _temp6897 >= _temp6895.last_plus_one)){ _throw(
Null_Exception);}* _temp6897;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6898= yyls; struct Cyc_Yyltype* _temp6900= _temp6898.curr +(
yylsp_offset - 2); if( _temp6898.base == 0? 1:( _temp6900 < _temp6898.base? 1:
_temp6900 >= _temp6898.last_plus_one)){ _throw( Null_Exception);}* _temp6900;}).first_line,({
struct _tagged_ptr3 _temp6901= yyls; struct Cyc_Yyltype* _temp6903= _temp6901.curr
+( yylsp_offset - 2); if( _temp6901.base == 0? 1:( _temp6903 < _temp6901.base? 1:
_temp6903 >= _temp6901.last_plus_one)){ _throw( Null_Exception);}* _temp6903;}).last_line));
_temp6891;}); _temp6890;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6904= yyls; struct Cyc_Yyltype* _temp6906= _temp6904.curr +( yylsp_offset -
8); if( _temp6904.base == 0? 1:( _temp6906 < _temp6904.base? 1: _temp6906 >=
_temp6904.last_plus_one)){ _throw( Null_Exception);}* _temp6906;}).first_line,({
struct _tagged_ptr3 _temp6907= yyls; struct Cyc_Yyltype* _temp6909= _temp6907.curr
+ yylsp_offset; if( _temp6907.base == 0? 1:( _temp6909 < _temp6907.base? 1:
_temp6909 >= _temp6907.last_plus_one)){ _throw( Null_Exception);}* _temp6909;}).last_line));
_temp6889;}); _temp6888;}); break; case 352: _LL6887: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6911=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6911[ 0]=({ struct Cyc_Primop_tok_struct
_temp6912; _temp6912.tag= Cyc_Primop_tok_tag; _temp6912.f1=( void*)(( void*) Cyc_Absyn_Printf);
_temp6912;}); _temp6911;}); break; case 353: _LL6910: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6914=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6914[ 0]=({ struct Cyc_Primop_tok_struct
_temp6915; _temp6915.tag= Cyc_Primop_tok_tag; _temp6915.f1=( void*)(( void*) Cyc_Absyn_Fprintf);
_temp6915;}); _temp6914;}); break; case 354: _LL6913: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6917=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6917[ 0]=({ struct Cyc_Primop_tok_struct
_temp6918; _temp6918.tag= Cyc_Primop_tok_tag; _temp6918.f1=( void*)(( void*) Cyc_Absyn_Xprintf);
_temp6918;}); _temp6917;}); break; case 355: _LL6916: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6920=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6920[ 0]=({ struct Cyc_Primop_tok_struct
_temp6921; _temp6921.tag= Cyc_Primop_tok_tag; _temp6921.f1=( void*)(( void*) Cyc_Absyn_Scanf);
_temp6921;}); _temp6920;}); break; case 356: _LL6919: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6923=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6923[ 0]=({ struct Cyc_Primop_tok_struct
_temp6924; _temp6924.tag= Cyc_Primop_tok_tag; _temp6924.f1=( void*)(( void*) Cyc_Absyn_Fscanf);
_temp6924;}); _temp6923;}); break; case 357: _LL6922: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6926=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6926[ 0]=({ struct Cyc_Primop_tok_struct
_temp6927; _temp6927.tag= Cyc_Primop_tok_tag; _temp6927.f1=( void*)(( void*) Cyc_Absyn_Sscanf);
_temp6927;}); _temp6926;}); break; case 358: _LL6925: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6929=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6929[ 0]=({ struct Cyc_Primop_tok_struct
_temp6930; _temp6930.tag= Cyc_Primop_tok_tag; _temp6930.f1=( void*)(( void*) Cyc_Absyn_Bitnot);
_temp6930;}); _temp6929;}); break; case 359: _LL6928: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6932=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6932[ 0]=({ struct Cyc_Primop_tok_struct
_temp6933; _temp6933.tag= Cyc_Primop_tok_tag; _temp6933.f1=( void*)(( void*) Cyc_Absyn_Not);
_temp6933;}); _temp6932;}); break; case 360: _LL6931: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6935=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6935[ 0]=({ struct Cyc_Primop_tok_struct
_temp6936; _temp6936.tag= Cyc_Primop_tok_tag; _temp6936.f1=( void*)(( void*) Cyc_Absyn_Minus);
_temp6936;}); _temp6935;}); break; case 361: _LL6934: yyval=({ struct
_tagged_ptr2 _temp6938= yyvs; struct _xtunion_struct** _temp6940= _temp6938.curr
+ yyvsp_offset; if( _temp6938.base == 0? 1:( _temp6940 < _temp6938.base? 1:
_temp6940 >= _temp6938.last_plus_one)){ _throw( Null_Exception);}* _temp6940;});
break; case 362: _LL6937: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6942=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6942[ 0]=({ struct Cyc_Exp_tok_struct _temp6943; _temp6943.tag= Cyc_Exp_tok_tag;
_temp6943.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6944= yyvs; struct _xtunion_struct** _temp6946= _temp6944.curr +(
yyvsp_offset - 3); if( _temp6944.base == 0? 1:( _temp6946 < _temp6944.base? 1:
_temp6946 >= _temp6944.last_plus_one)){ _throw( Null_Exception);}* _temp6946;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6947= yyvs; struct _xtunion_struct**
_temp6949= _temp6947.curr +( yyvsp_offset - 1); if( _temp6947.base == 0? 1:(
_temp6949 < _temp6947.base? 1: _temp6949 >= _temp6947.last_plus_one)){ _throw(
Null_Exception);}* _temp6949;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6950= yyls; struct Cyc_Yyltype* _temp6952= _temp6950.curr +(
yylsp_offset - 3); if( _temp6950.base == 0? 1:( _temp6952 < _temp6950.base? 1:
_temp6952 >= _temp6950.last_plus_one)){ _throw( Null_Exception);}* _temp6952;}).first_line,({
struct _tagged_ptr3 _temp6953= yyls; struct Cyc_Yyltype* _temp6955= _temp6953.curr
+ yylsp_offset; if( _temp6953.base == 0? 1:( _temp6955 < _temp6953.base? 1:
_temp6955 >= _temp6953.last_plus_one)){ _throw( Null_Exception);}* _temp6955;}).last_line));
_temp6943;}); _temp6942;}); break; case 363: _LL6941: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6957=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6957[ 0]=({ struct Cyc_Exp_tok_struct
_temp6958; _temp6958.tag= Cyc_Exp_tok_tag; _temp6958.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6959= yyvs; struct _xtunion_struct**
_temp6961= _temp6959.curr +( yyvsp_offset - 2); if( _temp6959.base == 0? 1:(
_temp6961 < _temp6959.base? 1: _temp6961 >= _temp6959.last_plus_one)){ _throw(
Null_Exception);}* _temp6961;})), 0, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6962= yyls; struct Cyc_Yyltype* _temp6964= _temp6962.curr +(
yylsp_offset - 2); if( _temp6962.base == 0? 1:( _temp6964 < _temp6962.base? 1:
_temp6964 >= _temp6962.last_plus_one)){ _throw( Null_Exception);}* _temp6964;}).first_line,({
struct _tagged_ptr3 _temp6965= yyls; struct Cyc_Yyltype* _temp6967= _temp6965.curr
+ yylsp_offset; if( _temp6965.base == 0? 1:( _temp6967 < _temp6965.base? 1:
_temp6967 >= _temp6965.last_plus_one)){ _throw( Null_Exception);}* _temp6967;}).last_line));
_temp6958;}); _temp6957;}); break; case 364: _LL6956: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6969=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6969[ 0]=({ struct Cyc_Exp_tok_struct
_temp6970; _temp6970.tag= Cyc_Exp_tok_tag; _temp6970.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6971= yyvs; struct _xtunion_struct**
_temp6973= _temp6971.curr +( yyvsp_offset - 3); if( _temp6971.base == 0? 1:(
_temp6973 < _temp6971.base? 1: _temp6973 >= _temp6971.last_plus_one)){ _throw(
Null_Exception);}* _temp6973;})), Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2
_temp6974= yyvs; struct _xtunion_struct** _temp6976= _temp6974.curr +(
yyvsp_offset - 1); if( _temp6974.base == 0? 1:( _temp6976 < _temp6974.base? 1:
_temp6976 >= _temp6974.last_plus_one)){ _throw( Null_Exception);}* _temp6976;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6977= yyls; struct Cyc_Yyltype*
_temp6979= _temp6977.curr +( yylsp_offset - 3); if( _temp6977.base == 0? 1:(
_temp6979 < _temp6977.base? 1: _temp6979 >= _temp6977.last_plus_one)){ _throw(
Null_Exception);}* _temp6979;}).first_line,({ struct _tagged_ptr3 _temp6980=
yyls; struct Cyc_Yyltype* _temp6982= _temp6980.curr + yylsp_offset; if(
_temp6980.base == 0? 1:( _temp6982 < _temp6980.base? 1: _temp6982 >= _temp6980.last_plus_one)){
_throw( Null_Exception);}* _temp6982;}).last_line)); _temp6970;}); _temp6969;});
break; case 365: _LL6968: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6984=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6984[ 0]=({ struct Cyc_Exp_tok_struct _temp6985; _temp6985.tag= Cyc_Exp_tok_tag;
_temp6985.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6986= yyvs; struct _xtunion_struct** _temp6988= _temp6986.curr
+( yyvsp_offset - 2); if( _temp6986.base == 0? 1:( _temp6988 < _temp6986.base? 1:
_temp6988 >= _temp6986.last_plus_one)){ _throw( Null_Exception);}* _temp6988;})),({
struct _tagged_string* _temp6989=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp6989[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp6990= yyvs; struct _xtunion_struct** _temp6992= _temp6990.curr
+ yyvsp_offset; if( _temp6990.base == 0? 1:( _temp6992 < _temp6990.base? 1:
_temp6992 >= _temp6990.last_plus_one)){ _throw( Null_Exception);}* _temp6992;}));
_temp6989;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6993= yyls;
struct Cyc_Yyltype* _temp6995= _temp6993.curr +( yylsp_offset - 2); if(
_temp6993.base == 0? 1:( _temp6995 < _temp6993.base? 1: _temp6995 >= _temp6993.last_plus_one)){
_throw( Null_Exception);}* _temp6995;}).first_line,({ struct _tagged_ptr3
_temp6996= yyls; struct Cyc_Yyltype* _temp6998= _temp6996.curr + yylsp_offset;
if( _temp6996.base == 0? 1:( _temp6998 < _temp6996.base? 1: _temp6998 >=
_temp6996.last_plus_one)){ _throw( Null_Exception);}* _temp6998;}).last_line));
_temp6985;}); _temp6984;}); break; case 366: _LL6983: { struct _tuple1* q= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp7000= yyvs; struct _xtunion_struct** _temp7002=
_temp7000.curr + yyvsp_offset; if( _temp7000.base == 0? 1:( _temp7002 <
_temp7000.base? 1: _temp7002 >= _temp7000.last_plus_one)){ _throw(
Null_Exception);}* _temp7002;}));{ void* _temp7003=(* q).f1; struct Cyc_List_List*
_temp7013; struct Cyc_List_List* _temp7015; _LL7005: if(( int) _temp7003 == Cyc_Absyn_Loc_n){
goto _LL7006;} else{ goto _LL7007;} _LL7007: if(( unsigned int) _temp7003 > 1u?((
struct _tunion_struct*) _temp7003)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL7014:
_temp7013=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp7003)->f1;
if( _temp7013 == 0){ goto _LL7008;} else{ goto _LL7009;}} else{ goto _LL7009;}
_LL7009: if(( unsigned int) _temp7003 > 1u?(( struct _tunion_struct*) _temp7003)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL7016: _temp7015=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp7003)->f1; if( _temp7015 == 0){ goto _LL7010;}
else{ goto _LL7011;}} else{ goto _LL7011;} _LL7011: goto _LL7012; _LL7006: goto
_LL7004; _LL7008: goto _LL7004; _LL7010: goto _LL7004; _LL7012: Cyc_Parse_err((
struct _tagged_string)({ char* _temp7017=( char*)"struct field name is qualified";
struct _tagged_string _temp7018; _temp7018.curr= _temp7017; _temp7018.base=
_temp7017; _temp7018.last_plus_one= _temp7017 + 31; _temp7018;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7019= yyls; struct Cyc_Yyltype* _temp7021= _temp7019.curr
+ yylsp_offset; if( _temp7019.base == 0? 1:( _temp7021 < _temp7019.base? 1:
_temp7021 >= _temp7019.last_plus_one)){ _throw( Null_Exception);}* _temp7021;}).first_line,({
struct _tagged_ptr3 _temp7022= yyls; struct Cyc_Yyltype* _temp7024= _temp7022.curr
+ yylsp_offset; if( _temp7022.base == 0? 1:( _temp7024 < _temp7022.base? 1:
_temp7024 >= _temp7022.last_plus_one)){ _throw( Null_Exception);}* _temp7024;}).last_line));
goto _LL7004; _LL7004:;} yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7025=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7025[ 0]=({ struct Cyc_Exp_tok_struct _temp7026; _temp7026.tag= Cyc_Exp_tok_tag;
_temp7026.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp7027= yyvs; struct _xtunion_struct** _temp7029= _temp7027.curr
+( yyvsp_offset - 2); if( _temp7027.base == 0? 1:( _temp7029 < _temp7027.base? 1:
_temp7029 >= _temp7027.last_plus_one)){ _throw( Null_Exception);}* _temp7029;})),(*
q).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7030= yyls;
struct Cyc_Yyltype* _temp7032= _temp7030.curr +( yylsp_offset - 2); if(
_temp7030.base == 0? 1:( _temp7032 < _temp7030.base? 1: _temp7032 >= _temp7030.last_plus_one)){
_throw( Null_Exception);}* _temp7032;}).first_line,({ struct _tagged_ptr3
_temp7033= yyls; struct Cyc_Yyltype* _temp7035= _temp7033.curr + yylsp_offset;
if( _temp7033.base == 0? 1:( _temp7035 < _temp7033.base? 1: _temp7035 >=
_temp7033.last_plus_one)){ _throw( Null_Exception);}* _temp7035;}).last_line));
_temp7026;}); _temp7025;}); break;} case 367: _LL6999: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7037=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7037[ 0]=({ struct Cyc_Exp_tok_struct
_temp7038; _temp7038.tag= Cyc_Exp_tok_tag; _temp7038.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7039= yyvs; struct _xtunion_struct**
_temp7041= _temp7039.curr +( yyvsp_offset - 2); if( _temp7039.base == 0? 1:(
_temp7041 < _temp7039.base? 1: _temp7041 >= _temp7039.last_plus_one)){ _throw(
Null_Exception);}* _temp7041;})),({ struct _tagged_string* _temp7042=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp7042[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp7043= yyvs; struct
_xtunion_struct** _temp7045= _temp7043.curr + yyvsp_offset; if( _temp7043.base
== 0? 1:( _temp7045 < _temp7043.base? 1: _temp7045 >= _temp7043.last_plus_one)){
_throw( Null_Exception);}* _temp7045;})); _temp7042;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7046= yyls; struct Cyc_Yyltype* _temp7048= _temp7046.curr
+( yylsp_offset - 2); if( _temp7046.base == 0? 1:( _temp7048 < _temp7046.base? 1:
_temp7048 >= _temp7046.last_plus_one)){ _throw( Null_Exception);}* _temp7048;}).first_line,({
struct _tagged_ptr3 _temp7049= yyls; struct Cyc_Yyltype* _temp7051= _temp7049.curr
+ yylsp_offset; if( _temp7049.base == 0? 1:( _temp7051 < _temp7049.base? 1:
_temp7051 >= _temp7049.last_plus_one)){ _throw( Null_Exception);}* _temp7051;}).last_line));
_temp7038;}); _temp7037;}); break; case 368: _LL7036: { struct _tuple1* q= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp7053= yyvs; struct _xtunion_struct** _temp7055=
_temp7053.curr + yyvsp_offset; if( _temp7053.base == 0? 1:( _temp7055 <
_temp7053.base? 1: _temp7055 >= _temp7053.last_plus_one)){ _throw(
Null_Exception);}* _temp7055;}));{ void* _temp7056=(* q).f1; struct Cyc_List_List*
_temp7066; struct Cyc_List_List* _temp7068; _LL7058: if(( int) _temp7056 == Cyc_Absyn_Loc_n){
goto _LL7059;} else{ goto _LL7060;} _LL7060: if(( unsigned int) _temp7056 > 1u?((
struct _tunion_struct*) _temp7056)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL7067:
_temp7066=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp7056)->f1;
if( _temp7066 == 0){ goto _LL7061;} else{ goto _LL7062;}} else{ goto _LL7062;}
_LL7062: if(( unsigned int) _temp7056 > 1u?(( struct _tunion_struct*) _temp7056)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL7069: _temp7068=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp7056)->f1; if( _temp7068 == 0){ goto _LL7063;}
else{ goto _LL7064;}} else{ goto _LL7064;} _LL7064: goto _LL7065; _LL7059: goto
_LL7057; _LL7061: goto _LL7057; _LL7063: goto _LL7057; _LL7065: Cyc_Parse_err((
struct _tagged_string)({ char* _temp7070=( char*)"struct field is qualified with module name";
struct _tagged_string _temp7071; _temp7071.curr= _temp7070; _temp7071.base=
_temp7070; _temp7071.last_plus_one= _temp7070 + 43; _temp7071;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7072= yyls; struct Cyc_Yyltype* _temp7074= _temp7072.curr
+ yylsp_offset; if( _temp7072.base == 0? 1:( _temp7074 < _temp7072.base? 1:
_temp7074 >= _temp7072.last_plus_one)){ _throw( Null_Exception);}* _temp7074;}).first_line,({
struct _tagged_ptr3 _temp7075= yyls; struct Cyc_Yyltype* _temp7077= _temp7075.curr
+ yylsp_offset; if( _temp7075.base == 0? 1:( _temp7077 < _temp7075.base? 1:
_temp7077 >= _temp7075.last_plus_one)){ _throw( Null_Exception);}* _temp7077;}).last_line));
goto _LL7057; _LL7057:;} yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7078=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7078[ 0]=({ struct Cyc_Exp_tok_struct _temp7079; _temp7079.tag= Cyc_Exp_tok_tag;
_temp7079.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp7080= yyvs; struct _xtunion_struct** _temp7082= _temp7080.curr
+( yyvsp_offset - 2); if( _temp7080.base == 0? 1:( _temp7082 < _temp7080.base? 1:
_temp7082 >= _temp7080.last_plus_one)){ _throw( Null_Exception);}* _temp7082;})),(*
q).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7083= yyls;
struct Cyc_Yyltype* _temp7085= _temp7083.curr +( yylsp_offset - 2); if(
_temp7083.base == 0? 1:( _temp7085 < _temp7083.base? 1: _temp7085 >= _temp7083.last_plus_one)){
_throw( Null_Exception);}* _temp7085;}).first_line,({ struct _tagged_ptr3
_temp7086= yyls; struct Cyc_Yyltype* _temp7088= _temp7086.curr + yylsp_offset;
if( _temp7086.base == 0? 1:( _temp7088 < _temp7086.base? 1: _temp7088 >=
_temp7086.last_plus_one)){ _throw( Null_Exception);}* _temp7088;}).last_line));
_temp7079;}); _temp7078;}); break;} case 369: _LL7052: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7090=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7090[ 0]=({ struct Cyc_Exp_tok_struct
_temp7091; _temp7091.tag= Cyc_Exp_tok_tag; _temp7091.f1= Cyc_Absyn_post_inc_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7092= yyvs; struct _xtunion_struct**
_temp7094= _temp7092.curr +( yyvsp_offset - 1); if( _temp7092.base == 0? 1:(
_temp7094 < _temp7092.base? 1: _temp7094 >= _temp7092.last_plus_one)){ _throw(
Null_Exception);}* _temp7094;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7095= yyls; struct Cyc_Yyltype* _temp7097= _temp7095.curr +(
yylsp_offset - 1); if( _temp7095.base == 0? 1:( _temp7097 < _temp7095.base? 1:
_temp7097 >= _temp7095.last_plus_one)){ _throw( Null_Exception);}* _temp7097;}).first_line,({
struct _tagged_ptr3 _temp7098= yyls; struct Cyc_Yyltype* _temp7100= _temp7098.curr
+ yylsp_offset; if( _temp7098.base == 0? 1:( _temp7100 < _temp7098.base? 1:
_temp7100 >= _temp7098.last_plus_one)){ _throw( Null_Exception);}* _temp7100;}).last_line));
_temp7091;}); _temp7090;}); break; case 370: _LL7089: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7102=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7102[ 0]=({ struct Cyc_Exp_tok_struct
_temp7103; _temp7103.tag= Cyc_Exp_tok_tag; _temp7103.f1= Cyc_Absyn_post_dec_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7104= yyvs; struct _xtunion_struct**
_temp7106= _temp7104.curr +( yyvsp_offset - 1); if( _temp7104.base == 0? 1:(
_temp7106 < _temp7104.base? 1: _temp7106 >= _temp7104.last_plus_one)){ _throw(
Null_Exception);}* _temp7106;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7107= yyls; struct Cyc_Yyltype* _temp7109= _temp7107.curr +(
yylsp_offset - 1); if( _temp7107.base == 0? 1:( _temp7109 < _temp7107.base? 1:
_temp7109 >= _temp7107.last_plus_one)){ _throw( Null_Exception);}* _temp7109;}).first_line,({
struct _tagged_ptr3 _temp7110= yyls; struct Cyc_Yyltype* _temp7112= _temp7110.curr
+ yylsp_offset; if( _temp7110.base == 0? 1:( _temp7112 < _temp7110.base? 1:
_temp7112 >= _temp7110.last_plus_one)){ _throw( Null_Exception);}* _temp7112;}).last_line));
_temp7103;}); _temp7102;}); break; case 371: _LL7101: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7114=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7114[ 0]=({ struct Cyc_Exp_tok_struct
_temp7115; _temp7115.tag= Cyc_Exp_tok_tag; _temp7115.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_CompoundLit_e_struct* _temp7116=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp7116[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp7117; _temp7117.tag= Cyc_Absyn_CompoundLit_e_tag;
_temp7117.f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp7118= yyvs;
struct _xtunion_struct** _temp7120= _temp7118.curr +( yyvsp_offset - 4); if(
_temp7118.base == 0? 1:( _temp7120 < _temp7118.base? 1: _temp7120 >= _temp7118.last_plus_one)){
_throw( Null_Exception);}* _temp7120;})); _temp7117.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp7121= yyvs; struct _xtunion_struct** _temp7123=
_temp7121.curr +( yyvsp_offset - 1); if( _temp7121.base == 0? 1:( _temp7123 <
_temp7121.base? 1: _temp7123 >= _temp7121.last_plus_one)){ _throw(
Null_Exception);}* _temp7123;}))); _temp7117;}); _temp7116;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7124= yyls; struct Cyc_Yyltype* _temp7126= _temp7124.curr
+( yylsp_offset - 5); if( _temp7124.base == 0? 1:( _temp7126 < _temp7124.base? 1:
_temp7126 >= _temp7124.last_plus_one)){ _throw( Null_Exception);}* _temp7126;}).first_line,({
struct _tagged_ptr3 _temp7127= yyls; struct Cyc_Yyltype* _temp7129= _temp7127.curr
+ yylsp_offset; if( _temp7127.base == 0? 1:( _temp7129 < _temp7127.base? 1:
_temp7129 >= _temp7127.last_plus_one)){ _throw( Null_Exception);}* _temp7129;}).last_line));
_temp7115;}); _temp7114;}); break; case 372: _LL7113: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7131=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7131[ 0]=({ struct Cyc_Exp_tok_struct
_temp7132; _temp7132.tag= Cyc_Exp_tok_tag; _temp7132.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_CompoundLit_e_struct* _temp7133=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp7133[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp7134; _temp7134.tag= Cyc_Absyn_CompoundLit_e_tag;
_temp7134.f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp7135= yyvs;
struct _xtunion_struct** _temp7137= _temp7135.curr +( yyvsp_offset - 5); if(
_temp7135.base == 0? 1:( _temp7137 < _temp7135.base? 1: _temp7137 >= _temp7135.last_plus_one)){
_throw( Null_Exception);}* _temp7137;})); _temp7134.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp7138= yyvs; struct _xtunion_struct** _temp7140=
_temp7138.curr +( yyvsp_offset - 2); if( _temp7138.base == 0? 1:( _temp7140 <
_temp7138.base? 1: _temp7140 >= _temp7138.last_plus_one)){ _throw(
Null_Exception);}* _temp7140;}))); _temp7134;}); _temp7133;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7141= yyls; struct Cyc_Yyltype* _temp7143= _temp7141.curr
+( yylsp_offset - 6); if( _temp7141.base == 0? 1:( _temp7143 < _temp7141.base? 1:
_temp7143 >= _temp7141.last_plus_one)){ _throw( Null_Exception);}* _temp7143;}).first_line,({
struct _tagged_ptr3 _temp7144= yyls; struct Cyc_Yyltype* _temp7146= _temp7144.curr
+ yylsp_offset; if( _temp7144.base == 0? 1:( _temp7146 < _temp7144.base? 1:
_temp7146 >= _temp7144.last_plus_one)){ _throw( Null_Exception);}* _temp7146;}).last_line));
_temp7132;}); _temp7131;}); break; case 373: _LL7130: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7148=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7148[ 0]=({ struct Cyc_Exp_tok_struct
_temp7149; _temp7149.tag= Cyc_Exp_tok_tag; _temp7149.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Fill_e_struct* _temp7150=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp7150[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp7151; _temp7151.tag= Cyc_Absyn_Fill_e_tag; _temp7151.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp7152= yyvs; struct _xtunion_struct** _temp7154=
_temp7152.curr +( yyvsp_offset - 1); if( _temp7152.base == 0? 1:( _temp7154 <
_temp7152.base? 1: _temp7154 >= _temp7152.last_plus_one)){ _throw(
Null_Exception);}* _temp7154;})); _temp7151;}); _temp7150;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7155= yyls; struct Cyc_Yyltype* _temp7157= _temp7155.curr
+( yylsp_offset - 3); if( _temp7155.base == 0? 1:( _temp7157 < _temp7155.base? 1:
_temp7157 >= _temp7155.last_plus_one)){ _throw( Null_Exception);}* _temp7157;}).first_line,({
struct _tagged_ptr3 _temp7158= yyls; struct Cyc_Yyltype* _temp7160= _temp7158.curr
+ yylsp_offset; if( _temp7158.base == 0? 1:( _temp7160 < _temp7158.base? 1:
_temp7160 >= _temp7158.last_plus_one)){ _throw( Null_Exception);}* _temp7160;}).last_line));
_temp7149;}); _temp7148;}); break; case 374: _LL7147: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7162=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7162[ 0]=({ struct Cyc_Exp_tok_struct
_temp7163; _temp7163.tag= Cyc_Exp_tok_tag; _temp7163.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Codegen_e_struct* _temp7164=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp7164[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp7165; _temp7165.tag= Cyc_Absyn_Codegen_e_tag;
_temp7165.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp7166= yyvs;
struct _xtunion_struct** _temp7168= _temp7166.curr +( yyvsp_offset - 1); if(
_temp7166.base == 0? 1:( _temp7168 < _temp7166.base? 1: _temp7168 >= _temp7166.last_plus_one)){
_throw( Null_Exception);}* _temp7168;})); _temp7165;}); _temp7164;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7169= yyls; struct Cyc_Yyltype* _temp7171= _temp7169.curr
+( yylsp_offset - 3); if( _temp7169.base == 0? 1:( _temp7171 < _temp7169.base? 1:
_temp7171 >= _temp7169.last_plus_one)){ _throw( Null_Exception);}* _temp7171;}).first_line,({
struct _tagged_ptr3 _temp7172= yyls; struct Cyc_Yyltype* _temp7174= _temp7172.curr
+ yylsp_offset; if( _temp7172.base == 0? 1:( _temp7174 < _temp7172.base? 1:
_temp7174 >= _temp7172.last_plus_one)){ _throw( Null_Exception);}* _temp7174;}).last_line));
_temp7163;}); _temp7162;}); break; case 375: _LL7161: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7176=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7176[ 0]=({ struct Cyc_Exp_tok_struct
_temp7177; _temp7177.tag= Cyc_Exp_tok_tag; _temp7177.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp7178=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp7178[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp7179; _temp7179.tag= Cyc_Absyn_UnknownId_e_tag;
_temp7179.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp7180= yyvs;
struct _xtunion_struct** _temp7182= _temp7180.curr + yyvsp_offset; if( _temp7180.base
== 0? 1:( _temp7182 < _temp7180.base? 1: _temp7182 >= _temp7180.last_plus_one)){
_throw( Null_Exception);}* _temp7182;})); _temp7179;}); _temp7178;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7183= yyls; struct Cyc_Yyltype* _temp7185= _temp7183.curr
+ yylsp_offset; if( _temp7183.base == 0? 1:( _temp7185 < _temp7183.base? 1:
_temp7185 >= _temp7183.last_plus_one)){ _throw( Null_Exception);}* _temp7185;}).first_line,({
struct _tagged_ptr3 _temp7186= yyls; struct Cyc_Yyltype* _temp7188= _temp7186.curr
+ yylsp_offset; if( _temp7186.base == 0? 1:( _temp7188 < _temp7186.base? 1:
_temp7188 >= _temp7186.last_plus_one)){ _throw( Null_Exception);}* _temp7188;}).last_line));
_temp7177;}); _temp7176;}); break; case 376: _LL7175: yyval=({ struct
_tagged_ptr2 _temp7190= yyvs; struct _xtunion_struct** _temp7192= _temp7190.curr
+ yyvsp_offset; if( _temp7190.base == 0? 1:( _temp7192 < _temp7190.base? 1:
_temp7192 >= _temp7190.last_plus_one)){ _throw( Null_Exception);}* _temp7192;});
break; case 377: _LL7189: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7194=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7194[ 0]=({ struct Cyc_Exp_tok_struct _temp7195; _temp7195.tag= Cyc_Exp_tok_tag;
_temp7195.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp7196= yyvs; struct _xtunion_struct** _temp7198= _temp7196.curr +
yyvsp_offset; if( _temp7196.base == 0? 1:( _temp7198 < _temp7196.base? 1:
_temp7198 >= _temp7196.last_plus_one)){ _throw( Null_Exception);}* _temp7198;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7199= yyls; struct Cyc_Yyltype*
_temp7201= _temp7199.curr + yylsp_offset; if( _temp7199.base == 0? 1:( _temp7201
< _temp7199.base? 1: _temp7201 >= _temp7199.last_plus_one)){ _throw(
Null_Exception);}* _temp7201;}).first_line,({ struct _tagged_ptr3 _temp7202=
yyls; struct Cyc_Yyltype* _temp7204= _temp7202.curr + yylsp_offset; if(
_temp7202.base == 0? 1:( _temp7204 < _temp7202.base? 1: _temp7204 >= _temp7202.last_plus_one)){
_throw( Null_Exception);}* _temp7204;}).last_line)); _temp7195;}); _temp7194;});
break; case 378: _LL7193: yyval=({ struct _tagged_ptr2 _temp7206= yyvs; struct
_xtunion_struct** _temp7208= _temp7206.curr +( yyvsp_offset - 1); if( _temp7206.base
== 0? 1:( _temp7208 < _temp7206.base? 1: _temp7208 >= _temp7206.last_plus_one)){
_throw( Null_Exception);}* _temp7208;}); break; case 379: _LL7205: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7210=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7210[ 0]=({ struct Cyc_Exp_tok_struct
_temp7211; _temp7211.tag= Cyc_Exp_tok_tag; _temp7211.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp7212=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7212[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7213; _temp7213.tag=
Cyc_Absyn_UnknownId_e_tag; _temp7213.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp7214= yyvs; struct _xtunion_struct** _temp7216= _temp7214.curr
+( yyvsp_offset - 1); if( _temp7214.base == 0? 1:( _temp7216 < _temp7214.base? 1:
_temp7216 >= _temp7214.last_plus_one)){ _throw( Null_Exception);}* _temp7216;}));
_temp7213;}); _temp7212;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7217= yyls; struct Cyc_Yyltype* _temp7219= _temp7217.curr +( yylsp_offset -
1); if( _temp7217.base == 0? 1:( _temp7219 < _temp7217.base? 1: _temp7219 >=
_temp7217.last_plus_one)){ _throw( Null_Exception);}* _temp7219;}).first_line,({
struct _tagged_ptr3 _temp7220= yyls; struct Cyc_Yyltype* _temp7222= _temp7220.curr
+( yylsp_offset - 1); if( _temp7220.base == 0? 1:( _temp7222 < _temp7220.base? 1:
_temp7222 >= _temp7220.last_plus_one)){ _throw( Null_Exception);}* _temp7222;}).last_line)),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7223= yyls; struct Cyc_Yyltype*
_temp7225= _temp7223.curr +( yylsp_offset - 1); if( _temp7223.base == 0? 1:(
_temp7225 < _temp7223.base? 1: _temp7225 >= _temp7223.last_plus_one)){ _throw(
Null_Exception);}* _temp7225;}).first_line,({ struct _tagged_ptr3 _temp7226=
yyls; struct Cyc_Yyltype* _temp7228= _temp7226.curr + yylsp_offset; if(
_temp7226.base == 0? 1:( _temp7228 < _temp7226.base? 1: _temp7228 >= _temp7226.last_plus_one)){
_throw( Null_Exception);}* _temp7228;}).last_line)); _temp7211;}); _temp7210;});
break; case 380: _LL7209: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7230=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7230[ 0]=({ struct Cyc_Exp_tok_struct _temp7231; _temp7231.tag= Cyc_Exp_tok_tag;
_temp7231.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp7232=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7232[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7233; _temp7233.tag=
Cyc_Absyn_UnknownId_e_tag; _temp7233.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp7234= yyvs; struct _xtunion_struct** _temp7236= _temp7234.curr
+( yyvsp_offset - 4); if( _temp7234.base == 0? 1:( _temp7236 < _temp7234.base? 1:
_temp7236 >= _temp7234.last_plus_one)){ _throw( Null_Exception);}* _temp7236;}));
_temp7233;}); _temp7232;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7237= yyls; struct Cyc_Yyltype* _temp7239= _temp7237.curr +( yylsp_offset -
4); if( _temp7237.base == 0? 1:( _temp7239 < _temp7237.base? 1: _temp7239 >=
_temp7237.last_plus_one)){ _throw( Null_Exception);}* _temp7239;}).first_line,({
struct _tagged_ptr3 _temp7240= yyls; struct Cyc_Yyltype* _temp7242= _temp7240.curr
+( yylsp_offset - 4); if( _temp7240.base == 0? 1:( _temp7242 < _temp7240.base? 1:
_temp7242 >= _temp7240.last_plus_one)){ _throw( Null_Exception);}* _temp7242;}).last_line)),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp7243= yyvs; struct _xtunion_struct** _temp7245=
_temp7243.curr +( yyvsp_offset - 1); if( _temp7243.base == 0? 1:( _temp7245 <
_temp7243.base? 1: _temp7245 >= _temp7243.last_plus_one)){ _throw(
Null_Exception);}* _temp7245;}))), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7246= yyls; struct Cyc_Yyltype* _temp7248= _temp7246.curr +(
yylsp_offset - 4); if( _temp7246.base == 0? 1:( _temp7248 < _temp7246.base? 1:
_temp7248 >= _temp7246.last_plus_one)){ _throw( Null_Exception);}* _temp7248;}).first_line,({
struct _tagged_ptr3 _temp7249= yyls; struct Cyc_Yyltype* _temp7251= _temp7249.curr
+ yylsp_offset; if( _temp7249.base == 0? 1:( _temp7251 < _temp7249.base? 1:
_temp7251 >= _temp7249.last_plus_one)){ _throw( Null_Exception);}* _temp7251;}).last_line));
_temp7231;}); _temp7230;}); break; case 381: _LL7229: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7253=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7253[ 0]=({ struct Cyc_Exp_tok_struct
_temp7254; _temp7254.tag= Cyc_Exp_tok_tag; _temp7254.f1= Cyc_Absyn_tuple_exp(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp7255= yyvs; struct
_xtunion_struct** _temp7257= _temp7255.curr +( yyvsp_offset - 1); if( _temp7255.base
== 0? 1:( _temp7257 < _temp7255.base? 1: _temp7257 >= _temp7255.last_plus_one)){
_throw( Null_Exception);}* _temp7257;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7258= yyls; struct Cyc_Yyltype* _temp7260= _temp7258.curr +(
yylsp_offset - 3); if( _temp7258.base == 0? 1:( _temp7260 < _temp7258.base? 1:
_temp7260 >= _temp7258.last_plus_one)){ _throw( Null_Exception);}* _temp7260;}).first_line,({
struct _tagged_ptr3 _temp7261= yyls; struct Cyc_Yyltype* _temp7263= _temp7261.curr
+ yylsp_offset; if( _temp7261.base == 0? 1:( _temp7263 < _temp7261.base? 1:
_temp7263 >= _temp7261.last_plus_one)){ _throw( Null_Exception);}* _temp7263;}).last_line));
_temp7254;}); _temp7253;}); break; case 382: _LL7252: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7265=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7265[ 0]=({ struct Cyc_Exp_tok_struct
_temp7266; _temp7266.tag= Cyc_Exp_tok_tag; _temp7266.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Struct_e_struct* _temp7267=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp7267[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp7268; _temp7268.tag= Cyc_Absyn_Struct_e_tag;
_temp7268.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp7269= yyvs;
struct _xtunion_struct** _temp7271= _temp7269.curr +( yyvsp_offset - 3); if(
_temp7269.base == 0? 1:( _temp7271 < _temp7269.base? 1: _temp7271 >= _temp7269.last_plus_one)){
_throw( Null_Exception);}* _temp7271;})); _temp7268.f2= 0; _temp7268.f3=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp7272= yyvs; struct _xtunion_struct** _temp7274=
_temp7272.curr +( yyvsp_offset - 1); if( _temp7272.base == 0? 1:( _temp7274 <
_temp7272.base? 1: _temp7274 >= _temp7272.last_plus_one)){ _throw(
Null_Exception);}* _temp7274;}))); _temp7268.f4= 0; _temp7268;}); _temp7267;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7275= yyls; struct Cyc_Yyltype*
_temp7277= _temp7275.curr +( yylsp_offset - 3); if( _temp7275.base == 0? 1:(
_temp7277 < _temp7275.base? 1: _temp7277 >= _temp7275.last_plus_one)){ _throw(
Null_Exception);}* _temp7277;}).first_line,({ struct _tagged_ptr3 _temp7278=
yyls; struct Cyc_Yyltype* _temp7280= _temp7278.curr + yylsp_offset; if(
_temp7278.base == 0? 1:( _temp7280 < _temp7278.base? 1: _temp7280 >= _temp7278.last_plus_one)){
_throw( Null_Exception);}* _temp7280;}).last_line)); _temp7266;}); _temp7265;});
break; case 383: _LL7264: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7282=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7282[ 0]=({ struct Cyc_Exp_tok_struct _temp7283; _temp7283.tag= Cyc_Exp_tok_tag;
_temp7283.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp7284= yyvs; struct _xtunion_struct** _temp7286= _temp7284.curr +(
yyvsp_offset - 2); if( _temp7284.base == 0? 1:( _temp7286 < _temp7284.base? 1:
_temp7286 >= _temp7284.last_plus_one)){ _throw( Null_Exception);}* _temp7286;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7287= yyls; struct Cyc_Yyltype*
_temp7289= _temp7287.curr +( yylsp_offset - 4); if( _temp7287.base == 0? 1:(
_temp7289 < _temp7287.base? 1: _temp7289 >= _temp7287.last_plus_one)){ _throw(
Null_Exception);}* _temp7289;}).first_line,({ struct _tagged_ptr3 _temp7290=
yyls; struct Cyc_Yyltype* _temp7292= _temp7290.curr + yylsp_offset; if(
_temp7290.base == 0? 1:( _temp7292 < _temp7290.base? 1: _temp7292 >= _temp7290.last_plus_one)){
_throw( Null_Exception);}* _temp7292;}).last_line)); _temp7283;}); _temp7282;});
break; case 384: _LL7281: yyval=( struct _xtunion_struct*)({ struct Cyc_ExpList_tok_struct*
_temp7294=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp7294[ 0]=({ struct Cyc_ExpList_tok_struct _temp7295; _temp7295.tag= Cyc_ExpList_tok_tag;
_temp7295.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp7296= yyvs; struct
_xtunion_struct** _temp7298= _temp7296.curr + yyvsp_offset; if( _temp7296.base
== 0? 1:( _temp7298 < _temp7296.base? 1: _temp7298 >= _temp7296.last_plus_one)){
_throw( Null_Exception);}* _temp7298;}))); _temp7295;}); _temp7294;}); break;
case 385: _LL7293: yyval=( struct _xtunion_struct*)({ struct Cyc_ExpList_tok_struct*
_temp7300=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp7300[ 0]=({ struct Cyc_ExpList_tok_struct _temp7301; _temp7301.tag= Cyc_ExpList_tok_tag;
_temp7301.f1=({ struct Cyc_List_List* _temp7302=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp7302->hd=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp7303= yyvs; struct _xtunion_struct** _temp7305=
_temp7303.curr + yyvsp_offset; if( _temp7303.base == 0? 1:( _temp7305 <
_temp7303.base? 1: _temp7305 >= _temp7303.last_plus_one)){ _throw(
Null_Exception);}* _temp7305;})); _temp7302->tl= 0; _temp7302;}); _temp7301;});
_temp7300;}); break; case 386: _LL7299: yyval=( struct _xtunion_struct*)({
struct Cyc_ExpList_tok_struct* _temp7307=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp7307[ 0]=({ struct Cyc_ExpList_tok_struct
_temp7308; _temp7308.tag= Cyc_ExpList_tok_tag; _temp7308.f1=({ struct Cyc_List_List*
_temp7309=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp7309->hd=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7313= yyvs;
struct _xtunion_struct** _temp7315= _temp7313.curr + yyvsp_offset; if( _temp7313.base
== 0? 1:( _temp7315 < _temp7313.base? 1: _temp7315 >= _temp7313.last_plus_one)){
_throw( Null_Exception);}* _temp7315;})); _temp7309->tl= Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp7310= yyvs; struct _xtunion_struct** _temp7312=
_temp7310.curr +( yyvsp_offset - 2); if( _temp7310.base == 0? 1:( _temp7312 <
_temp7310.base? 1: _temp7312 >= _temp7310.last_plus_one)){ _throw(
Null_Exception);}* _temp7312;})); _temp7309;}); _temp7308;}); _temp7307;});
break; case 387: _LL7306: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7317=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7317[ 0]=({ struct Cyc_Exp_tok_struct _temp7318; _temp7318.tag= Cyc_Exp_tok_tag;
_temp7318.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(({ struct _tagged_ptr2
_temp7319= yyvs; struct _xtunion_struct** _temp7321= _temp7319.curr +
yyvsp_offset; if( _temp7319.base == 0? 1:( _temp7321 < _temp7319.base? 1:
_temp7321 >= _temp7319.last_plus_one)){ _throw( Null_Exception);}* _temp7321;}))).f1,(*
Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp7322= yyvs; struct _xtunion_struct**
_temp7324= _temp7322.curr + yyvsp_offset; if( _temp7322.base == 0? 1:( _temp7324
< _temp7322.base? 1: _temp7324 >= _temp7322.last_plus_one)){ _throw(
Null_Exception);}* _temp7324;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7325= yyls; struct Cyc_Yyltype* _temp7327= _temp7325.curr +
yylsp_offset; if( _temp7325.base == 0? 1:( _temp7327 < _temp7325.base? 1:
_temp7327 >= _temp7325.last_plus_one)){ _throw( Null_Exception);}* _temp7327;}).first_line,({
struct _tagged_ptr3 _temp7328= yyls; struct Cyc_Yyltype* _temp7330= _temp7328.curr
+ yylsp_offset; if( _temp7328.base == 0? 1:( _temp7330 < _temp7328.base? 1:
_temp7330 >= _temp7328.last_plus_one)){ _throw( Null_Exception);}* _temp7330;}).last_line));
_temp7318;}); _temp7317;}); break; case 388: _LL7316: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7332=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7332[ 0]=({ struct Cyc_Exp_tok_struct
_temp7333; _temp7333.tag= Cyc_Exp_tok_tag; _temp7333.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok(({
struct _tagged_ptr2 _temp7334= yyvs; struct _xtunion_struct** _temp7336=
_temp7334.curr + yyvsp_offset; if( _temp7334.base == 0? 1:( _temp7336 <
_temp7334.base? 1: _temp7336 >= _temp7334.last_plus_one)){ _throw(
Null_Exception);}* _temp7336;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7337= yyls; struct Cyc_Yyltype* _temp7339= _temp7337.curr +
yylsp_offset; if( _temp7337.base == 0? 1:( _temp7339 < _temp7337.base? 1:
_temp7339 >= _temp7337.last_plus_one)){ _throw( Null_Exception);}* _temp7339;}).first_line,({
struct _tagged_ptr3 _temp7340= yyls; struct Cyc_Yyltype* _temp7342= _temp7340.curr
+ yylsp_offset; if( _temp7340.base == 0? 1:( _temp7342 < _temp7340.base? 1:
_temp7342 >= _temp7340.last_plus_one)){ _throw( Null_Exception);}* _temp7342;}).last_line));
_temp7333;}); _temp7332;}); break; case 389: _LL7331: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7344=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7344[ 0]=({ struct Cyc_Exp_tok_struct
_temp7345; _temp7345.tag= Cyc_Exp_tok_tag; _temp7345.f1= Cyc_Absyn_float_exp(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp7346= yyvs; struct
_xtunion_struct** _temp7348= _temp7346.curr + yyvsp_offset; if( _temp7346.base
== 0? 1:( _temp7348 < _temp7346.base? 1: _temp7348 >= _temp7346.last_plus_one)){
_throw( Null_Exception);}* _temp7348;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7349= yyls; struct Cyc_Yyltype* _temp7351= _temp7349.curr +
yylsp_offset; if( _temp7349.base == 0? 1:( _temp7351 < _temp7349.base? 1:
_temp7351 >= _temp7349.last_plus_one)){ _throw( Null_Exception);}* _temp7351;}).first_line,({
struct _tagged_ptr3 _temp7352= yyls; struct Cyc_Yyltype* _temp7354= _temp7352.curr
+ yylsp_offset; if( _temp7352.base == 0? 1:( _temp7354 < _temp7352.base? 1:
_temp7354 >= _temp7352.last_plus_one)){ _throw( Null_Exception);}* _temp7354;}).last_line));
_temp7345;}); _temp7344;}); break; case 390: _LL7343: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7356=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7356[ 0]=({ struct Cyc_Exp_tok_struct
_temp7357; _temp7357.tag= Cyc_Exp_tok_tag; _temp7357.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7358= yyls; struct Cyc_Yyltype* _temp7360= _temp7358.curr
+ yylsp_offset; if( _temp7358.base == 0? 1:( _temp7360 < _temp7358.base? 1:
_temp7360 >= _temp7358.last_plus_one)){ _throw( Null_Exception);}* _temp7360;}).first_line,({
struct _tagged_ptr3 _temp7361= yyls; struct Cyc_Yyltype* _temp7363= _temp7361.curr
+ yylsp_offset; if( _temp7361.base == 0? 1:( _temp7363 < _temp7361.base? 1:
_temp7363 >= _temp7361.last_plus_one)){ _throw( Null_Exception);}* _temp7363;}).last_line));
_temp7357;}); _temp7356;}); break; case 391: _LL7355: yyval=( struct
_xtunion_struct*)({ struct Cyc_QualId_tok_struct* _temp7365=( struct Cyc_QualId_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualId_tok_struct)); _temp7365[ 0]=({ struct Cyc_QualId_tok_struct
_temp7366; _temp7366.tag= Cyc_QualId_tok_tag; _temp7366.f1=({ struct _tuple1*
_temp7367=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp7367->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp7372=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp7372[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp7373; _temp7373.tag= Cyc_Absyn_Rel_n_tag; _temp7373.f1= 0; _temp7373;});
_temp7372;}); _temp7367->f2=({ struct _tagged_string* _temp7368=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp7368[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp7369= yyvs; struct
_xtunion_struct** _temp7371= _temp7369.curr + yyvsp_offset; if( _temp7369.base
== 0? 1:( _temp7371 < _temp7369.base? 1: _temp7371 >= _temp7369.last_plus_one)){
_throw( Null_Exception);}* _temp7371;})); _temp7368;}); _temp7367;}); _temp7366;});
_temp7365;}); break; case 392: _LL7364: yyval=({ struct _tagged_ptr2 _temp7375=
yyvs; struct _xtunion_struct** _temp7377= _temp7375.curr + yyvsp_offset; if(
_temp7375.base == 0? 1:( _temp7377 < _temp7375.base? 1: _temp7377 >= _temp7375.last_plus_one)){
_throw( Null_Exception);}* _temp7377;}); break; default: _LL7374:( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Yyimpossible_struct* _temp7379=( struct
Cyc_Yyimpossible_struct*) GC_malloc( sizeof( struct Cyc_Yyimpossible_struct));
_temp7379[ 0]=({ struct Cyc_Yyimpossible_struct _temp7380; _temp7380.tag= Cyc_Yyimpossible_tag;
_temp7380;}); _temp7379;}));} yyvsp_offset -= yylen; yyssp_offset -= yylen;
yylsp_offset -= yylen;({ struct _tagged_ptr2 _temp7381= yyvs; struct
_xtunion_struct** _temp7383= _temp7381.curr +( ++ yyvsp_offset); if( _temp7381.base
== 0? 1:( _temp7383 < _temp7381.base? 1: _temp7383 >= _temp7381.last_plus_one)){
_throw( Null_Exception);}* _temp7383= yyval;}); yylsp_offset ++; if( yylen == 0){({
struct _tagged_ptr3 _temp7384= yyls; struct Cyc_Yyltype* _temp7386= _temp7384.curr
+ yylsp_offset; if( _temp7384.base == 0? 1:( _temp7386 < _temp7384.base? 1:
_temp7386 >= _temp7384.last_plus_one)){ _throw( Null_Exception);}(* _temp7386).first_line=
Cyc_yylloc.first_line;});({ struct _tagged_ptr3 _temp7387= yyls; struct Cyc_Yyltype*
_temp7389= _temp7387.curr + yylsp_offset; if( _temp7387.base == 0? 1:( _temp7389
< _temp7387.base? 1: _temp7389 >= _temp7387.last_plus_one)){ _throw(
Null_Exception);}(* _temp7389).first_column= Cyc_yylloc.first_column;});({
struct _tagged_ptr3 _temp7390= yyls; struct Cyc_Yyltype* _temp7392= _temp7390.curr
+ yylsp_offset; if( _temp7390.base == 0? 1:( _temp7392 < _temp7390.base? 1:
_temp7392 >= _temp7390.last_plus_one)){ _throw( Null_Exception);}(* _temp7392).last_line=({
struct _tagged_ptr3 _temp7393= yyls; struct Cyc_Yyltype* _temp7395= _temp7393.curr
+( yylsp_offset - 1); if( _temp7393.base == 0? 1:( _temp7395 < _temp7393.base? 1:
_temp7395 >= _temp7393.last_plus_one)){ _throw( Null_Exception);}* _temp7395;}).last_line;});({
struct _tagged_ptr3 _temp7396= yyls; struct Cyc_Yyltype* _temp7398= _temp7396.curr
+ yylsp_offset; if( _temp7396.base == 0? 1:( _temp7398 < _temp7396.base? 1:
_temp7398 >= _temp7396.last_plus_one)){ _throw( Null_Exception);}(* _temp7398).last_column=({
struct _tagged_ptr3 _temp7399= yyls; struct Cyc_Yyltype* _temp7401= _temp7399.curr
+( yylsp_offset - 1); if( _temp7399.base == 0? 1:( _temp7401 < _temp7399.base? 1:
_temp7401 >= _temp7399.last_plus_one)){ _throw( Null_Exception);}* _temp7401;}).last_column;});({
struct _tagged_ptr3 _temp7402= yyls; struct Cyc_Yyltype* _temp7404= _temp7402.curr
+ yylsp_offset; if( _temp7402.base == 0? 1:( _temp7404 < _temp7402.base? 1:
_temp7404 >= _temp7402.last_plus_one)){ _throw( Null_Exception);}(* _temp7404).text=(
struct _tagged_string)({ char* _temp7405=( char*)""; struct _tagged_string
_temp7406; _temp7406.curr= _temp7405; _temp7406.base= _temp7405; _temp7406.last_plus_one=
_temp7405 + 1; _temp7406;});});} else{({ struct _tagged_ptr3 _temp7407= yyls;
struct Cyc_Yyltype* _temp7409= _temp7407.curr + yylsp_offset; if( _temp7407.base
== 0? 1:( _temp7409 < _temp7407.base? 1: _temp7409 >= _temp7407.last_plus_one)){
_throw( Null_Exception);}(* _temp7409).last_line=({ struct _tagged_ptr3
_temp7410= yyls; struct Cyc_Yyltype* _temp7412= _temp7410.curr +(( yylsp_offset
+ yylen) - 1); if( _temp7410.base == 0? 1:( _temp7412 < _temp7410.base? 1:
_temp7412 >= _temp7410.last_plus_one)){ _throw( Null_Exception);}* _temp7412;}).last_line;});({
struct _tagged_ptr3 _temp7413= yyls; struct Cyc_Yyltype* _temp7415= _temp7413.curr
+ yylsp_offset; if( _temp7413.base == 0? 1:( _temp7415 < _temp7413.base? 1:
_temp7415 >= _temp7413.last_plus_one)){ _throw( Null_Exception);}(* _temp7415).last_column=({
struct _tagged_ptr3 _temp7416= yyls; struct Cyc_Yyltype* _temp7418= _temp7416.curr
+(( yylsp_offset + yylen) - 1); if( _temp7416.base == 0? 1:( _temp7418 <
_temp7416.base? 1: _temp7418 >= _temp7416.last_plus_one)){ _throw(
Null_Exception);}* _temp7418;}).last_column;});} yyn=( int)({ short* _temp7419=(
short*) Cyc_yyr1; unsigned int _temp7420= yyn; if( _temp7420 >= 393u){ _throw(
Null_Exception);} _temp7419[ _temp7420];}); yystate=({ short* _temp7421=( short*)
Cyc_yypgoto; unsigned int _temp7422= yyn - 124; if( _temp7422 >= 106u){ _throw(
Null_Exception);} _temp7421[ _temp7422];}) +({ struct _tagged_ptr1 _temp7423=
yyss; short* _temp7425= _temp7423.curr + yyssp_offset; if( _temp7423.base == 0?
1:( _temp7425 < _temp7423.base? 1: _temp7425 >= _temp7423.last_plus_one)){
_throw( Null_Exception);}* _temp7425;}); if(( yystate >= 0? yystate <= 4214: 0)?({
short* _temp7426=( short*) Cyc_yycheck; unsigned int _temp7427= yystate; if(
_temp7427 >= 4215u){ _throw( Null_Exception);} _temp7426[ _temp7427];}) ==({
struct _tagged_ptr1 _temp7428= yyss; short* _temp7430= _temp7428.curr +
yyssp_offset; if( _temp7428.base == 0? 1:( _temp7430 < _temp7428.base? 1:
_temp7430 >= _temp7428.last_plus_one)){ _throw( Null_Exception);}* _temp7430;}):
0){ yystate=( int)({ short* _temp7431=( short*) Cyc_yytable; unsigned int
_temp7432= yystate; if( _temp7432 >= 4215u){ _throw( Null_Exception);} _temp7431[
_temp7432];});} else{ yystate=( int)({ short* _temp7433=( short*) Cyc_yydefgoto;
unsigned int _temp7434= yyn - 124; if( _temp7434 >= 106u){ _throw(
Null_Exception);} _temp7433[ _temp7434];});} goto yynewstate; yyerrlab: if(
yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int)({ short* _temp7435=( short*) Cyc_yypact;
unsigned int _temp7436= yystate; if( _temp7436 >= 755u){ _throw( Null_Exception);}
_temp7435[ _temp7436];}); if( yyn > - 32768? yyn < 4214: 0){ int sze= 0; struct
_tagged_string msg; int x; int count; count= 0; for( x= yyn < 0? - yyn: 0; x <
230u / sizeof( char*); x ++){ if(({ short* _temp7437=( short*) Cyc_yycheck;
unsigned int _temp7438= x + yyn; if( _temp7438 >= 4215u){ _throw( Null_Exception);}
_temp7437[ _temp7438];}) == x){ sze += Cyc_String_strlen(({ struct
_tagged_string* _temp7439=( struct _tagged_string*) Cyc_yytname; unsigned int
_temp7440= x; if( _temp7440 >= 230u){ _throw( Null_Exception);} _temp7439[
_temp7440];})) + 15, count ++;}} msg=({ unsigned int _temp7441=( unsigned int)(
sze + 15); char* _temp7442=( char*) GC_malloc_atomic( sizeof( char) * _temp7441);
struct _tagged_string _temp7445={ _temp7442, _temp7442, _temp7442 + _temp7441};{
unsigned int _temp7443= _temp7441; unsigned int i; for( i= 0; i < _temp7443; i
++){ _temp7442[ i]='\000';}}; _temp7445;}); Cyc_String_strcpy( msg,( struct
_tagged_string)({ char* _temp7446=( char*)"parse error"; struct _tagged_string
_temp7447; _temp7447.curr= _temp7446; _temp7447.base= _temp7446; _temp7447.last_plus_one=
_temp7446 + 12; _temp7447;})); if( count < 5){ count= 0; for( x= yyn < 0? - yyn:
0; x < 230u / sizeof( char*); x ++){ if(({ short* _temp7448=( short*) Cyc_yycheck;
unsigned int _temp7449= x + yyn; if( _temp7449 >= 4215u){ _throw( Null_Exception);}
_temp7448[ _temp7449];}) == x){ Cyc_String_strcat( msg, count == 0?( struct
_tagged_string)({ char* _temp7450=( char*)", expecting `"; struct _tagged_string
_temp7451; _temp7451.curr= _temp7450; _temp7451.base= _temp7450; _temp7451.last_plus_one=
_temp7450 + 14; _temp7451;}):( struct _tagged_string)({ char* _temp7452=( char*)" or `";
struct _tagged_string _temp7453; _temp7453.curr= _temp7452; _temp7453.base=
_temp7452; _temp7453.last_plus_one= _temp7452 + 6; _temp7453;})); Cyc_String_strcat(
msg,({ struct _tagged_string* _temp7454=( struct _tagged_string*) Cyc_yytname;
unsigned int _temp7455= x; if( _temp7455 >= 230u){ _throw( Null_Exception);}
_temp7454[ _temp7455];})); Cyc_String_strcat( msg,( struct _tagged_string)({
char* _temp7456=( char*)"'"; struct _tagged_string _temp7457; _temp7457.curr=
_temp7456; _temp7457.base= _temp7456; _temp7457.last_plus_one= _temp7456 + 2;
_temp7457;})); count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror(( struct
_tagged_string)({ char* _temp7458=( char*)"parse error"; struct _tagged_string
_temp7459; _temp7459.curr= _temp7458; _temp7459.base= _temp7458; _temp7459.last_plus_one=
_temp7458 + 12; _temp7459;}));}} goto yyerrlab1; yyerrlab1: if( yyerrstatus == 3){
if( Cyc_yychar == 0){ return 1;} Cyc_yychar= - 2;} yyerrstatus= 3; goto
yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset == 0){ return 1;}
yyvsp_offset --; yystate=( int)({ struct _tagged_ptr1 _temp7460= yyss; short*
_temp7462= _temp7460.curr +( -- yyssp_offset); if( _temp7460.base == 0? 1:(
_temp7462 < _temp7460.base? 1: _temp7462 >= _temp7460.last_plus_one)){ _throw(
Null_Exception);}* _temp7462;}); yylsp_offset --; yyerrhandle: yyn=( int)({
short* _temp7463=( short*) Cyc_yypact; unsigned int _temp7464= yystate; if(
_temp7464 >= 755u){ _throw( Null_Exception);} _temp7463[ _temp7464];}); if( yyn
== - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn < 0? 1: yyn > 4214)? 1:({
short* _temp7465=( short*) Cyc_yycheck; unsigned int _temp7466= yyn; if(
_temp7466 >= 4215u){ _throw( Null_Exception);} _temp7465[ _temp7466];}) != 1){
goto yyerrdefault;} yyn=( int)({ short* _temp7467=( short*) Cyc_yytable;
unsigned int _temp7468= yyn; if( _temp7468 >= 4215u){ _throw( Null_Exception);}
_temp7467[ _temp7468];}); if( yyn < 0){ if( yyn == - 32768){ goto yyerrpop;} yyn=
- yyn; goto yyreduce;} else{ if( yyn == 0){ goto yyerrpop;}} if( yyn == 754){
return 0;}({ struct _tagged_ptr2 _temp7469= yyvs; struct _xtunion_struct**
_temp7471= _temp7469.curr +( ++ yyvsp_offset); if( _temp7469.base == 0? 1:(
_temp7471 < _temp7469.base? 1: _temp7471 >= _temp7469.last_plus_one)){ _throw(
Null_Exception);}* _temp7471= Cyc_yylval;});({ struct _tagged_ptr3 _temp7472=
yyls; struct Cyc_Yyltype* _temp7474= _temp7472.curr +( ++ yylsp_offset); if(
_temp7472.base == 0? 1:( _temp7474 < _temp7472.base? 1: _temp7474 >= _temp7472.last_plus_one)){
_throw( Null_Exception);}* _temp7474= Cyc_yylloc;}); goto yynewstate;} void Cyc_yyprint(
int i, struct _xtunion_struct* v){ struct _xtunion_struct* _temp7475= v; struct
_tuple12* _temp7495; struct _tuple12 _temp7497; int _temp7498; void* _temp7500;
char _temp7502; short _temp7504; struct _tagged_string _temp7506; struct Cyc_Core_Opt*
_temp7508; struct Cyc_Core_Opt* _temp7510; struct Cyc_Core_Opt _temp7512; struct
_tagged_string* _temp7513; struct _tuple1* _temp7515; struct _tuple1 _temp7517;
struct _tagged_string* _temp7518; void* _temp7520; _LL7477: if( _temp7475->tag
== Cyc_Okay_tok_tag){ goto _LL7478;} else{ goto _LL7479;} _LL7479: if((*((
struct _xtunion_struct*) _temp7475)).tag == Cyc_Int_tok_tag){ _LL7496: _temp7495=((
struct Cyc_Int_tok_struct*) _temp7475)->f1; _temp7497=* _temp7495; _LL7501:
_temp7500= _temp7497.f1; goto _LL7499; _LL7499: _temp7498= _temp7497.f2; goto
_LL7480;} else{ goto _LL7481;} _LL7481: if((*(( struct _xtunion_struct*)
_temp7475)).tag == Cyc_Char_tok_tag){ _LL7503: _temp7502=(( struct Cyc_Char_tok_struct*)
_temp7475)->f1; goto _LL7482;} else{ goto _LL7483;} _LL7483: if((*(( struct
_xtunion_struct*) _temp7475)).tag == Cyc_Short_tok_tag){ _LL7505: _temp7504=((
struct Cyc_Short_tok_struct*) _temp7475)->f1; goto _LL7484;} else{ goto _LL7485;}
_LL7485: if((*(( struct _xtunion_struct*) _temp7475)).tag == Cyc_String_tok_tag){
_LL7507: _temp7506=(( struct Cyc_String_tok_struct*) _temp7475)->f1; goto
_LL7486;} else{ goto _LL7487;} _LL7487: if((*(( struct _xtunion_struct*)
_temp7475)).tag == Cyc_Stringopt_tok_tag){ _LL7509: _temp7508=(( struct Cyc_Stringopt_tok_struct*)
_temp7475)->f1; if( _temp7508 == 0){ goto _LL7488;} else{ goto _LL7489;}} else{
goto _LL7489;} _LL7489: if((*(( struct _xtunion_struct*) _temp7475)).tag == Cyc_Stringopt_tok_tag){
_LL7511: _temp7510=(( struct Cyc_Stringopt_tok_struct*) _temp7475)->f1; if(
_temp7510 == 0){ goto _LL7491;} else{ _temp7512=* _temp7510; _LL7514: _temp7513=(
struct _tagged_string*) _temp7512.v; goto _LL7490;}} else{ goto _LL7491;}
_LL7491: if((*(( struct _xtunion_struct*) _temp7475)).tag == Cyc_QualId_tok_tag){
_LL7516: _temp7515=(( struct Cyc_QualId_tok_struct*) _temp7475)->f1; _temp7517=*
_temp7515; _LL7521: _temp7520= _temp7517.f1; goto _LL7519; _LL7519: _temp7518=
_temp7517.f2; goto _LL7492;} else{ goto _LL7493;} _LL7493: goto _LL7494; _LL7478:
fprintf( Cyc_Stdio_stderr,"ok"); goto _LL7476; _LL7480: fprintf( Cyc_Stdio_stderr,"%d",
_temp7498); goto _LL7476; _LL7482: fprintf( Cyc_Stdio_stderr,"%c",( int)
_temp7502); goto _LL7476; _LL7484: fprintf( Cyc_Stdio_stderr,"%ds",( int)
_temp7504); goto _LL7476; _LL7486:({ struct _tagged_string _temp7522= _temp7506;
fprintf( Cyc_Stdio_stderr,"\"%.*s\"", _temp7522.last_plus_one - _temp7522.curr,
_temp7522.curr);}); goto _LL7476; _LL7488: fprintf( Cyc_Stdio_stderr,"null");
goto _LL7476; _LL7490:({ struct _tagged_string _temp7523=* _temp7513; fprintf(
Cyc_Stdio_stderr,"\"%.*s\"", _temp7523.last_plus_one - _temp7523.curr, _temp7523.curr);});
goto _LL7476; _LL7492: { struct Cyc_List_List* prefix= 0;{ void* _temp7524=
_temp7520; struct Cyc_List_List* _temp7532; struct Cyc_List_List* _temp7534;
_LL7526: if(( unsigned int) _temp7524 > 1u?(( struct _tunion_struct*) _temp7524)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL7533: _temp7532=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp7524)->f1; goto _LL7527;} else{ goto _LL7528;}
_LL7528: if(( unsigned int) _temp7524 > 1u?(( struct _tunion_struct*) _temp7524)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL7535: _temp7534=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp7524)->f1; goto _LL7529;} else{ goto _LL7530;}
_LL7530: if(( int) _temp7524 == Cyc_Absyn_Loc_n){ goto _LL7531;} else{ goto
_LL7525;} _LL7527: prefix= _temp7532; goto _LL7525; _LL7529: prefix= _temp7534;
goto _LL7525; _LL7531: goto _LL7525; _LL7525:;} for( 0; prefix != 0; prefix=({
struct Cyc_List_List* _temp7536= prefix; if( _temp7536 == 0){ _throw(
Null_Exception);} _temp7536->tl;})){({ struct _tagged_string _temp7538=*((
struct _tagged_string*)({ struct Cyc_List_List* _temp7537= prefix; if( _temp7537
== 0){ _throw( Null_Exception);} _temp7537->hd;})); fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp7538.last_plus_one - _temp7538.curr, _temp7538.curr);});}({ struct
_tagged_string _temp7539=* _temp7518; fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp7539.last_plus_one - _temp7539.curr, _temp7539.curr);}); goto _LL7476;}
_LL7494: fprintf( Cyc_Stdio_stderr,"?"); goto _LL7476; _LL7476:;} struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0;
Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp7540=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp7540->v=( void*) Cyc_Lexing_from_file(
f); _temp7540;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}