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
_temp289, tds);{ struct _tuple11* env=({ struct _tuple11* _temp298=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp298->f1= tds; _temp298->f2=
loc; _temp298;}); return({ struct Cyc_List_List* _temp293=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp293->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp294=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp294[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp295; _temp295.tag= Cyc_Absyn_Function_mod_tag;
_temp295.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp296=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp296[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp297; _temp297.tag= Cyc_Absyn_WithTypes_tag;
_temp297.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple11*,
struct _tagged_string*), struct _tuple11* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, env, _temp289); _temp297.f2= 0; _temp297.f3= 0;
_temp297;}); _temp296;})); _temp295;}); _temp294;})); _temp293->tl= 0; _temp293;});}
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
Cyc_Parse_Declarator*)({ struct Cyc_List_List* _temp528= ds; if( _temp528 == 0){
_throw( Null_Exception);} _temp528->hd;}); struct _tuple1* q= d->id; struct Cyc_List_List*
_temp517; struct Cyc_List_List* _temp519; void* _temp521; struct Cyc_Absyn_Tqual*
_temp523; struct _tuple6 _temp515= Cyc_Parse_apply_tms( tq, t, shared_atts, d->tms);
_LL524: _temp523= _temp515.f1; goto _LL522; _LL522: _temp521= _temp515.f2; goto
_LL520; _LL520: _temp519= _temp515.f3; goto _LL518; _LL518: _temp517= _temp515.f4;
goto _LL516; _LL516: return({ struct Cyc_List_List* _temp525=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp525->hd=( void*)({ struct
_tuple7* _temp527=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp527->f1= q; _temp527->f2= _temp523; _temp527->f3= _temp521; _temp527->f4=
_temp519; _temp527->f5= _temp517; _temp527;}); _temp525->tl= Cyc_Parse_apply_tmss(
_temp523, t,({ struct Cyc_List_List* _temp526= ds; if( _temp526 == 0){ _throw(
Null_Exception);} _temp526->tl;}), shared_atts); _temp525;});}} static int Cyc_Parse_fn_type_att(
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
Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct Cyc_Core_Opt* _temp733=
_temp696; if( _temp733 == 0){ _throw( Null_Exception);} _temp733->v;});{ void*
_temp700=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp714; struct Cyc_Absyn_Structdecl*
_temp716; struct Cyc_Absyn_Uniondecl* _temp718; struct Cyc_Absyn_Tuniondecl*
_temp720; struct Cyc_Absyn_XTuniondecl* _temp722; _LL702: if((( struct
_tunion_struct*) _temp700)->tag == Cyc_Absyn_Enum_d_tag){ _LL715: _temp714=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp700)->f1;
goto _LL703;} else{ goto _LL704;} _LL704: if((( struct _tunion_struct*) _temp700)->tag
== Cyc_Absyn_Struct_d_tag){ _LL717: _temp716=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp700)->f1; goto _LL705;} else{ goto
_LL706;} _LL706: if((( struct _tunion_struct*) _temp700)->tag == Cyc_Absyn_Union_d_tag){
_LL719: _temp718=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp700)->f1; goto _LL707;} else{ goto _LL708;} _LL708: if((( struct
_tunion_struct*) _temp700)->tag == Cyc_Absyn_Tunion_d_tag){ _LL721: _temp720=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*) _temp700)->f1;
goto _LL709;} else{ goto _LL710;} _LL710: if((( struct _tunion_struct*) _temp700)->tag
== Cyc_Absyn_XTunion_d_tag){ _LL723: _temp722=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_d_struct*) _temp700)->f1; goto _LL711;} else{ goto
_LL712;} _LL712: goto _LL713; _LL703:( void*)( _temp714->sc=( void*) s); if(
atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp724=( char*)"bad attributes on enum";
struct _tagged_string _temp725; _temp725.curr= _temp724; _temp725.base= _temp724;
_temp725.last_plus_one= _temp724 + 23; _temp725;}), loc);} goto _LL701; _LL705:(
void*)( _temp716->sc=( void*) s); _temp716->attributes= atts; goto _LL701;
_LL707:( void*)( _temp718->sc=( void*) s); _temp718->attributes= atts; goto
_LL701; _LL709:( void*)( _temp720->sc=( void*) s); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp726=( char*)"bad attributes on tunion";
struct _tagged_string _temp727; _temp727.curr= _temp726; _temp727.base= _temp726;
_temp727.last_plus_one= _temp726 + 25; _temp727;}), loc);} goto _LL701; _LL711:(
void*)( _temp722->sc=( void*) s); if( atts != 0){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp728=( char*)"bad attributes on xtunion"; struct
_tagged_string _temp729; _temp729.curr= _temp728; _temp729.base= _temp728;
_temp729.last_plus_one= _temp728 + 26; _temp729;}), loc);} goto _LL701; _LL713:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp730=( char*)"bad declaration";
struct _tagged_string _temp731; _temp731.curr= _temp730; _temp731.base= _temp730;
_temp731.last_plus_one= _temp730 + 16; _temp731;}), loc); return 0; _LL701:;}
return({ struct Cyc_List_List* _temp732=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp732->hd=( void*) d; _temp732->tl= 0;
_temp732;});} else{ void* _temp734= _temp698; struct Cyc_Absyn_Structdecl**
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
struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl* _temp788=(
struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp788->sc=( void*) s; _temp788->name=({ struct Cyc_Core_Opt* _temp789=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp789->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp790= _temp752; if( _temp790 ==
0){ _throw( Null_Exception);} _temp790;})); _temp789;}); _temp788->tvs= ts2;
_temp788->fields= 0; _temp788->attributes= 0; _temp788;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp782=( char*)"bad attributes on struct";
struct _tagged_string _temp783; _temp783.curr= _temp782; _temp783.base= _temp782;
_temp783.last_plus_one= _temp782 + 25; _temp783;}), loc);} return({ struct Cyc_List_List*
_temp784=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp784->hd=( void*)({ struct Cyc_Absyn_Decl* _temp785=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp785->r=( void*)(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp786=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp786[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp787; _temp787.tag= Cyc_Absyn_Struct_d_tag;
_temp787.f1= sd; _temp787;}); _temp786;})); _temp785->loc= loc; _temp785;});
_temp784->tl= 0; _temp784;});} _LL739: { struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp760); struct Cyc_Absyn_Decl* tud= Cyc_Absyn_tunion_decl( s,({ struct Cyc_Core_Opt*
_temp794=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp794->v=( void*)(( struct _tuple1*) _temp762); _temp794;}), ts2, 0, loc);
if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp791=( char*)"bad attributes on tunion";
struct _tagged_string _temp792; _temp792.curr= _temp791; _temp792.base= _temp791;
_temp792.last_plus_one= _temp791 + 25; _temp792;}), loc);} return({ struct Cyc_List_List*
_temp793=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp793->hd=( void*) tud; _temp793->tl= 0; _temp793;});} _LL741: { struct Cyc_Absyn_XTuniondecl*
xtud=({ struct Cyc_Absyn_XTuniondecl* _temp801=( struct Cyc_Absyn_XTuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_XTuniondecl)); _temp801->sc=( void*) s;
_temp801->name= _temp770; _temp801->fields= 0; _temp801;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp795=( char*)"bad attributes on xtunion";
struct _tagged_string _temp796; _temp796.curr= _temp795; _temp796.base= _temp795;
_temp796.last_plus_one= _temp795 + 26; _temp796;}), loc);} return({ struct Cyc_List_List*
_temp797=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp797->hd=( void*)({ struct Cyc_Absyn_Decl* _temp798=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp798->r=( void*)(( void*)({
struct Cyc_Absyn_XTunion_d_struct* _temp799=( struct Cyc_Absyn_XTunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunion_d_struct)); _temp799[ 0]=({ struct
Cyc_Absyn_XTunion_d_struct _temp800; _temp800.tag= Cyc_Absyn_XTunion_d_tag;
_temp800.f1= xtud; _temp800;}); _temp799;})); _temp798->loc= loc; _temp798;});
_temp797->tl= 0; _temp797;});} _LL743: { struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp774); struct Cyc_Absyn_Uniondecl* ud=({ struct Cyc_Absyn_Uniondecl*
_temp808=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp808->sc=( void*) s; _temp808->name=({ struct Cyc_Core_Opt* _temp809=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp809->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp810= _temp776; if( _temp810 ==
0){ _throw( Null_Exception);} _temp810;})); _temp809;}); _temp808->tvs= ts2;
_temp808->fields= 0; _temp808->attributes= 0; _temp808;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp802=( char*)"bad attributes on union";
struct _tagged_string _temp803; _temp803.curr= _temp802; _temp803.base= _temp802;
_temp803.last_plus_one= _temp802 + 24; _temp803;}), loc);} return({ struct Cyc_List_List*
_temp804=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp804->hd=( void*)({ struct Cyc_Absyn_Decl* _temp805=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp805->r=( void*)(( void*)({
struct Cyc_Absyn_Union_d_struct* _temp806=( struct Cyc_Absyn_Union_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp806[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp807; _temp807.tag= Cyc_Absyn_Union_d_tag; _temp807.f1= ud; _temp807;});
_temp806;})); _temp805->loc= loc; _temp805;}); _temp804->tl= 0; _temp804;});}
_LL745: { struct Cyc_Absyn_Enumdecl* ed=({ struct Cyc_Absyn_Enumdecl* _temp817=(
struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp817->sc=( void*) s; _temp817->name= _temp780; _temp817->fields= 0; _temp817;});
if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp811=( char*)"bad attributes on enum";
struct _tagged_string _temp812; _temp812.curr= _temp811; _temp812.base= _temp811;
_temp812.last_plus_one= _temp811 + 23; _temp812;}), loc);} return({ struct Cyc_List_List*
_temp813=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp813->hd=( void*)({ struct Cyc_Absyn_Decl* _temp814=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp814->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp815=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp815[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp816; _temp816.tag= Cyc_Absyn_Enum_d_tag; _temp816.f1= ed; _temp816;});
_temp815;})); _temp814->loc= loc; _temp814;}); _temp813->tl= 0; _temp813;});}
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
_temp849= ts_info.f2; if( _temp849 == 0){ _throw( Null_Exception);} _temp849->v;});{
void* _temp822=( void*) d->r; struct Cyc_Absyn_Structdecl* _temp836; struct Cyc_Absyn_Tuniondecl*
_temp838; struct Cyc_Absyn_XTuniondecl* _temp840; struct Cyc_Absyn_Uniondecl*
_temp842; struct Cyc_Absyn_Enumdecl* _temp844; _LL824: if((( struct
_tunion_struct*) _temp822)->tag == Cyc_Absyn_Struct_d_tag){ _LL837: _temp836=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp822)->f1;
goto _LL825;} else{ goto _LL826;} _LL826: if((( struct _tunion_struct*) _temp822)->tag
== Cyc_Absyn_Tunion_d_tag){ _LL839: _temp838=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp822)->f1; goto _LL827;} else{ goto
_LL828;} _LL828: if((( struct _tunion_struct*) _temp822)->tag == Cyc_Absyn_XTunion_d_tag){
_LL841: _temp840=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_d_struct*)
_temp822)->f1; goto _LL829;} else{ goto _LL830;} _LL830: if((( struct
_tunion_struct*) _temp822)->tag == Cyc_Absyn_Union_d_tag){ _LL843: _temp842=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp822)->f1;
goto _LL831;} else{ goto _LL832;} _LL832: if((( struct _tunion_struct*) _temp822)->tag
== Cyc_Absyn_Enum_d_tag){ _LL845: _temp844=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp822)->f1; goto _LL833;} else{ goto _LL834;}
_LL834: goto _LL835; _LL825:( void*)( _temp836->sc=( void*) s); _temp836->attributes=
atts; atts= 0; goto _LL823; _LL827:( void*)( _temp838->sc=( void*) s); goto
_LL823; _LL829:( void*)( _temp840->sc=( void*) s); goto _LL823; _LL831:( void*)(
_temp842->sc=( void*) s); goto _LL823; _LL833:( void*)( _temp844->sc=( void*) s);
goto _LL823; _LL835: Cyc_Parse_err(( struct _tagged_string)({ char* _temp846=(
char*)"declaration within typedef is not a struct, union, tunion, or xtunion";
struct _tagged_string _temp847; _temp847.curr= _temp846; _temp847.base= _temp846;
_temp847.last_plus_one= _temp846 + 70; _temp847;}), loc); goto _LL823; _LL823:;}
decls=({ struct Cyc_List_List* _temp848=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp848->hd=( void*) d; _temp848->tl= decls;
_temp848;});} if( atts != 0){ Cyc_Parse_err(({ struct _tagged_string _temp851=
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
eopt=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp878= _temp688; if(
_temp878 == 0){ _throw( Null_Exception);} _temp878->hd;}); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_new_vardecl( _temp868, _temp864, eopt); vd->tq= _temp866;( void*)(
vd->sc=( void*) s); vd->attributes= _temp860;{ struct Cyc_Absyn_Decl* d=({
struct Cyc_Absyn_Decl* _temp875=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof(
struct Cyc_Absyn_Decl)); _temp875->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp876=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp876[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp877; _temp877.tag= Cyc_Absyn_Var_d_tag;
_temp877.f1= vd; _temp877;}); _temp876;})); _temp875->loc= loc; _temp875;});
decls=({ struct Cyc_List_List* _temp874=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp874->hd=( void*) d; _temp874->tl= decls;
_temp874;});}}}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
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
struct _tagged_ptr1 yyss=( struct _tagged_ptr1)({ short* _temp7473=( short*)({
unsigned int _temp7469= 200u; short* _temp7470=( short*) GC_malloc_atomic(
sizeof( short) * _temp7469);{ unsigned int _temp7471= _temp7469; unsigned int i;
for( i= 0; i < _temp7471; i ++){ _temp7470[ i]= (short)0;}}; _temp7470;});
struct _tagged_ptr1 _temp7474; _temp7474.curr= _temp7473; _temp7474.base=
_temp7473; _temp7474.last_plus_one= _temp7473 + 200; _temp7474;}); int
yyvsp_offset; struct _tagged_ptr2 yyvs=( struct _tagged_ptr2)({ struct
_xtunion_struct** _temp7467=( struct _xtunion_struct**)({ unsigned int _temp7463=
200u; struct _xtunion_struct** _temp7464=( struct _xtunion_struct**) GC_malloc(
sizeof( struct _xtunion_struct*) * _temp7463);{ unsigned int _temp7465=
_temp7463; unsigned int i; for( i= 0; i < _temp7465; i ++){ _temp7464[ i]= Cyc_yylval;}};
_temp7464;}); struct _tagged_ptr2 _temp7468; _temp7468.curr= _temp7467;
_temp7468.base= _temp7467; _temp7468.last_plus_one= _temp7467 + 200; _temp7468;});
int yylsp_offset; struct _tagged_ptr3 yyls=( struct _tagged_ptr3)({ struct Cyc_Yyltype*
_temp7461=( struct Cyc_Yyltype*)({ unsigned int _temp7457= 200u; struct Cyc_Yyltype*
_temp7458=( struct Cyc_Yyltype*) GC_malloc( sizeof( struct Cyc_Yyltype) *
_temp7457);{ unsigned int _temp7459= _temp7457; unsigned int i; for( i= 0; i <
_temp7459; i ++){ _temp7458[ i]= Cyc_yynewloc();}}; _temp7458;}); struct
_tagged_ptr3 _temp7462; _temp7462.curr= _temp7461; _temp7462.base= _temp7461;
_temp7462.last_plus_one= _temp7461 + 200; _temp7462;}); int yystacksize= 200;
struct _xtunion_struct* yyval= Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0;
Cyc_yynerrs= 0; Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset= 0;
yylsp_offset= 0; yynewstate:({ struct _tagged_ptr1 _temp2145= yyss; short*
_temp2147= _temp2145.curr +( ++ yyssp_offset); if( _temp2145.base == 0? 1:(
_temp2147 < _temp2145.base? 1: _temp2147 >= _temp2145.last_plus_one)){ _throw(
Null_Exception);}* _temp2147=( short) yystate;}); if( yyssp_offset >=
yystacksize - 1){ if( yystacksize >= 10000){ Cyc_yyerror(( struct _tagged_string)({
char* _temp2148=( char*)"parser stack overflow"; struct _tagged_string _temp2149;
_temp2149.curr= _temp2148; _temp2149.base= _temp2148; _temp2149.last_plus_one=
_temp2148 + 22; _temp2149;}));( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_Yystack_overflow_struct* _temp2150=( struct Cyc_Yystack_overflow_struct*)
GC_malloc( sizeof( struct Cyc_Yystack_overflow_struct)); _temp2150[ 0]=({ struct
Cyc_Yystack_overflow_struct _temp2151; _temp2151.tag= Cyc_Yystack_overflow_tag;
_temp2151;}); _temp2150;}));} yystacksize *= 2; if( yystacksize > 10000){
yystacksize= 10000;}{ struct _tagged_ptr1 yyss1=({ unsigned int _temp2180=(
unsigned int) yystacksize; short* _temp2181=( short*) GC_malloc_atomic( sizeof(
short) * _temp2180); struct _tagged_ptr1 _temp2184={ _temp2181, _temp2181,
_temp2181 + _temp2180};{ unsigned int _temp2182= _temp2180; unsigned int i; for(
i= 0; i < _temp2182; i ++){ _temp2181[ i]= (short)0;}}; _temp2184;}); struct
_tagged_ptr2 yyvs1=({ unsigned int _temp2175=( unsigned int) yystacksize; struct
_xtunion_struct** _temp2176=( struct _xtunion_struct**) GC_malloc( sizeof(
struct _xtunion_struct*) * _temp2175); struct _tagged_ptr2 _temp2179={ _temp2176,
_temp2176, _temp2176 + _temp2175};{ unsigned int _temp2177= _temp2175;
unsigned int i; for( i= 0; i < _temp2177; i ++){ _temp2176[ i]= Cyc_yylval;}};
_temp2179;}); struct _tagged_ptr3 yyls1=({ unsigned int _temp2170=( unsigned int)
yystacksize; struct Cyc_Yyltype* _temp2171=( struct Cyc_Yyltype*) GC_malloc(
sizeof( struct Cyc_Yyltype) * _temp2170); struct _tagged_ptr3 _temp2174={
_temp2171, _temp2171, _temp2171 + _temp2170};{ unsigned int _temp2172= _temp2170;
unsigned int i; for( i= 0; i < _temp2172; i ++){ _temp2171[ i]= Cyc_yynewloc();}};
_temp2174;});{ int i= 0; for( 0; i <= yyssp_offset; i ++){({ struct _tagged_ptr1
_temp2152= yyss1; short* _temp2154= _temp2152.curr + i; if( _temp2152.base == 0?
1:( _temp2154 < _temp2152.base? 1: _temp2154 >= _temp2152.last_plus_one)){
_throw( Null_Exception);}* _temp2154=({ struct _tagged_ptr1 _temp2155= yyss;
short* _temp2157= _temp2155.curr + i; if( _temp2155.base == 0? 1:( _temp2157 <
_temp2155.base? 1: _temp2157 >= _temp2155.last_plus_one)){ _throw(
Null_Exception);}* _temp2157;});});({ struct _tagged_ptr2 _temp2158= yyvs1;
struct _xtunion_struct** _temp2160= _temp2158.curr + i; if( _temp2158.base == 0?
1:( _temp2160 < _temp2158.base? 1: _temp2160 >= _temp2158.last_plus_one)){
_throw( Null_Exception);}* _temp2160=({ struct _tagged_ptr2 _temp2161= yyvs;
struct _xtunion_struct** _temp2163= _temp2161.curr + i; if( _temp2161.base == 0?
1:( _temp2163 < _temp2161.base? 1: _temp2163 >= _temp2161.last_plus_one)){
_throw( Null_Exception);}* _temp2163;});});({ struct _tagged_ptr3 _temp2164=
yyls1; struct Cyc_Yyltype* _temp2166= _temp2164.curr + i; if( _temp2164.base ==
0? 1:( _temp2166 < _temp2164.base? 1: _temp2166 >= _temp2164.last_plus_one)){
_throw( Null_Exception);}* _temp2166=({ struct _tagged_ptr3 _temp2167= yyls;
struct Cyc_Yyltype* _temp2169= _temp2167.curr + i; if( _temp2167.base == 0? 1:(
_temp2169 < _temp2167.base? 1: _temp2169 >= _temp2167.last_plus_one)){ _throw(
Null_Exception);}* _temp2169;});});}} yyss= yyss1; yyvs= yyvs1; yyls= yyls1;}}
goto yybackup; yybackup: yyn=( int)({ short* _temp2185=( short*) Cyc_yypact;
unsigned int _temp2186= yystate; if( _temp2186 >= 755u){ _throw( Null_Exception);}
_temp2185[ _temp2186];}); if( yyn == - 32768){ goto yydefault;} if( Cyc_yychar
== - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 0){ yychar1= 0; Cyc_yychar=
0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <= 351: 0)?( int)({ short*
_temp2187=( short*) Cyc_yytranslate; unsigned int _temp2188= Cyc_yychar; if(
_temp2188 >= 352u){ _throw( Null_Exception);} _temp2187[ _temp2188];}): 230;}
yyn += yychar1; if(( yyn < 0? 1: yyn > 4214)? 1:({ short* _temp2189=( short*)
Cyc_yycheck; unsigned int _temp2190= yyn; if( _temp2190 >= 4215u){ _throw(
Null_Exception);} _temp2189[ _temp2190];}) != yychar1){ goto yydefault;} yyn=(
int)({ short* _temp2191=( short*) Cyc_yytable; unsigned int _temp2192= yyn; if(
_temp2192 >= 4215u){ _throw( Null_Exception);} _temp2191[ _temp2192];}); if( yyn
< 0){ if( yyn == - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if(
yyn == 0){ goto yyerrlab;}} if( yyn == 754){ return 0;} if( Cyc_yychar != 0){
Cyc_yychar= - 2;}({ struct _tagged_ptr2 _temp2193= yyvs; struct _xtunion_struct**
_temp2195= _temp2193.curr +( ++ yyvsp_offset); if( _temp2193.base == 0? 1:(
_temp2195 < _temp2193.base? 1: _temp2195 >= _temp2193.last_plus_one)){ _throw(
Null_Exception);}* _temp2195= Cyc_yylval;});({ struct _tagged_ptr3 _temp2196=
yyls; struct Cyc_Yyltype* _temp2198= _temp2196.curr +( ++ yylsp_offset); if(
_temp2196.base == 0? 1:( _temp2198 < _temp2196.base? 1: _temp2198 >= _temp2196.last_plus_one)){
_throw( Null_Exception);}* _temp2198= Cyc_yylloc;}); if( yyerrstatus != 0){
yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int)({ short*
_temp2199=( short*) Cyc_yydefact; unsigned int _temp2200= yystate; if( _temp2200
>= 755u){ _throw( Null_Exception);} _temp2199[ _temp2200];}); if( yyn == 0){
goto yyerrlab;} yyreduce: yylen=( int)({ short* _temp2201=( short*) Cyc_yyr2;
unsigned int _temp2202= yyn; if( _temp2202 >= 393u){ _throw( Null_Exception);}
_temp2201[ _temp2202];}); if( yylen > 0){ yyval=({ struct _tagged_ptr2 _temp2203=
yyvs; struct _xtunion_struct** _temp2205= _temp2203.curr +(( yyvsp_offset + 1) -
yylen); if( _temp2203.base == 0? 1:( _temp2205 < _temp2203.base? 1: _temp2205 >=
_temp2203.last_plus_one)){ _throw( Null_Exception);}* _temp2205;});} switch( yyn){
case 1: _LL2206: yyval=({ struct _tagged_ptr2 _temp2208= yyvs; struct
_xtunion_struct** _temp2210= _temp2208.curr + yyvsp_offset; if( _temp2208.base
== 0? 1:( _temp2210 < _temp2208.base? 1: _temp2210 >= _temp2208.last_plus_one)){
_throw( Null_Exception);}* _temp2210;}); Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2211= yyvs; struct _xtunion_struct** _temp2213=
_temp2211.curr + yyvsp_offset; if( _temp2211.base == 0? 1:( _temp2213 <
_temp2211.base? 1: _temp2213 >= _temp2211.last_plus_one)){ _throw(
Null_Exception);}* _temp2213;})); break; case 2: _LL2207: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2215=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2215[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2216; _temp2216.tag= Cyc_DeclList_tok_tag; _temp2216.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2217= yyvs; struct _xtunion_struct** _temp2219=
_temp2217.curr +( yyvsp_offset - 1); if( _temp2217.base == 0? 1:( _temp2219 <
_temp2217.base? 1: _temp2219 >= _temp2217.last_plus_one)){ _throw(
Null_Exception);}* _temp2219;})), Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2220= yyvs; struct _xtunion_struct** _temp2222= _temp2220.curr +
yyvsp_offset; if( _temp2220.base == 0? 1:( _temp2222 < _temp2220.base? 1:
_temp2222 >= _temp2220.last_plus_one)){ _throw( Null_Exception);}* _temp2222;})));
_temp2216;}); _temp2215;}); break; case 3: _LL2214: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2224=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2224[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2225; _temp2225.tag= Cyc_DeclList_tok_tag; _temp2225.f1=({ struct Cyc_List_List*
_temp2226=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2226->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2227=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2227->r=( void*)(( void*)({
struct Cyc_Absyn_Using_d_struct* _temp2234=( struct Cyc_Absyn_Using_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp2234[ 0]=({ struct
Cyc_Absyn_Using_d_struct _temp2235; _temp2235.tag= Cyc_Absyn_Using_d_tag;
_temp2235.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2236= yyvs;
struct _xtunion_struct** _temp2238= _temp2236.curr +( yyvsp_offset - 2); if(
_temp2236.base == 0? 1:( _temp2238 < _temp2236.base? 1: _temp2238 >= _temp2236.last_plus_one)){
_throw( Null_Exception);}* _temp2238;})); _temp2235.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2239= yyvs; struct _xtunion_struct** _temp2241=
_temp2239.curr + yyvsp_offset; if( _temp2239.base == 0? 1:( _temp2241 <
_temp2239.base? 1: _temp2241 >= _temp2239.last_plus_one)){ _throw(
Null_Exception);}* _temp2241;})); _temp2235;}); _temp2234;})); _temp2227->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2228= yyls; struct Cyc_Yyltype*
_temp2230= _temp2228.curr +( yylsp_offset - 2); if( _temp2228.base == 0? 1:(
_temp2230 < _temp2228.base? 1: _temp2230 >= _temp2228.last_plus_one)){ _throw(
Null_Exception);}* _temp2230;}).first_line,({ struct _tagged_ptr3 _temp2231=
yyls; struct Cyc_Yyltype* _temp2233= _temp2231.curr + yylsp_offset; if(
_temp2231.base == 0? 1:( _temp2233 < _temp2231.base? 1: _temp2233 >= _temp2231.last_plus_one)){
_throw( Null_Exception);}* _temp2233;}).last_line); _temp2227;}); _temp2226->tl=
0; _temp2226;}); _temp2225;}); _temp2224;}); Cyc_Lex_leave_using(); break; case
4: _LL2223: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2243=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2243[ 0]=({ struct Cyc_DeclList_tok_struct _temp2244; _temp2244.tag= Cyc_DeclList_tok_tag;
_temp2244.f1=({ struct Cyc_List_List* _temp2245=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2245->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2249=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2249->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2256=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2256[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2257; _temp2257.tag= Cyc_Absyn_Using_d_tag;
_temp2257.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2258= yyvs;
struct _xtunion_struct** _temp2260= _temp2258.curr +( yyvsp_offset - 4); if(
_temp2258.base == 0? 1:( _temp2260 < _temp2258.base? 1: _temp2260 >= _temp2258.last_plus_one)){
_throw( Null_Exception);}* _temp2260;})); _temp2257.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2261= yyvs; struct _xtunion_struct** _temp2263=
_temp2261.curr +( yyvsp_offset - 2); if( _temp2261.base == 0? 1:( _temp2263 <
_temp2261.base? 1: _temp2263 >= _temp2261.last_plus_one)){ _throw(
Null_Exception);}* _temp2263;})); _temp2257;}); _temp2256;})); _temp2249->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2250= yyls; struct Cyc_Yyltype*
_temp2252= _temp2250.curr +( yylsp_offset - 4); if( _temp2250.base == 0? 1:(
_temp2252 < _temp2250.base? 1: _temp2252 >= _temp2250.last_plus_one)){ _throw(
Null_Exception);}* _temp2252;}).first_line,({ struct _tagged_ptr3 _temp2253=
yyls; struct Cyc_Yyltype* _temp2255= _temp2253.curr +( yylsp_offset - 1); if(
_temp2253.base == 0? 1:( _temp2255 < _temp2253.base? 1: _temp2255 >= _temp2253.last_plus_one)){
_throw( Null_Exception);}* _temp2255;}).last_line); _temp2249;}); _temp2245->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2246= yyvs; struct
_xtunion_struct** _temp2248= _temp2246.curr + yyvsp_offset; if( _temp2246.base
== 0? 1:( _temp2248 < _temp2246.base? 1: _temp2248 >= _temp2246.last_plus_one)){
_throw( Null_Exception);}* _temp2248;})); _temp2245;}); _temp2244;}); _temp2243;});
break; case 5: _LL2242: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2265=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2265[ 0]=({ struct Cyc_DeclList_tok_struct _temp2266; _temp2266.tag= Cyc_DeclList_tok_tag;
_temp2266.f1=({ struct Cyc_List_List* _temp2267=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2267->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2268=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2268->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2275=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2275[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2276; _temp2276.tag=
Cyc_Absyn_Namespace_d_tag; _temp2276.f1=({ struct _tagged_string* _temp2277=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp2277[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2278= yyvs;
struct _xtunion_struct** _temp2280= _temp2278.curr +( yyvsp_offset - 2); if(
_temp2278.base == 0? 1:( _temp2280 < _temp2278.base? 1: _temp2280 >= _temp2278.last_plus_one)){
_throw( Null_Exception);}* _temp2280;})); _temp2277;}); _temp2276.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2281= yyvs; struct _xtunion_struct** _temp2283=
_temp2281.curr + yyvsp_offset; if( _temp2281.base == 0? 1:( _temp2283 <
_temp2281.base? 1: _temp2283 >= _temp2281.last_plus_one)){ _throw(
Null_Exception);}* _temp2283;})); _temp2276;}); _temp2275;})); _temp2268->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2269= yyls; struct Cyc_Yyltype*
_temp2271= _temp2269.curr +( yylsp_offset - 2); if( _temp2269.base == 0? 1:(
_temp2271 < _temp2269.base? 1: _temp2271 >= _temp2269.last_plus_one)){ _throw(
Null_Exception);}* _temp2271;}).first_line,({ struct _tagged_ptr3 _temp2272=
yyls; struct Cyc_Yyltype* _temp2274= _temp2272.curr + yylsp_offset; if(
_temp2272.base == 0? 1:( _temp2274 < _temp2272.base? 1: _temp2274 >= _temp2272.last_plus_one)){
_throw( Null_Exception);}* _temp2274;}).last_line); _temp2268;}); _temp2267->tl=
0; _temp2267;}); _temp2266;}); _temp2265;}); Cyc_Lex_leave_namespace(); break;
case 6: _LL2264: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2285=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2285[ 0]=({ struct Cyc_DeclList_tok_struct _temp2286; _temp2286.tag= Cyc_DeclList_tok_tag;
_temp2286.f1=({ struct Cyc_List_List* _temp2287=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2287->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2291=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2291->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2298=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2298[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2299; _temp2299.tag=
Cyc_Absyn_Namespace_d_tag; _temp2299.f1=({ struct _tagged_string* _temp2300=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp2300[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2301= yyvs;
struct _xtunion_struct** _temp2303= _temp2301.curr +( yyvsp_offset - 4); if(
_temp2301.base == 0? 1:( _temp2303 < _temp2301.base? 1: _temp2303 >= _temp2301.last_plus_one)){
_throw( Null_Exception);}* _temp2303;})); _temp2300;}); _temp2299.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2304= yyvs; struct _xtunion_struct** _temp2306=
_temp2304.curr +( yyvsp_offset - 2); if( _temp2304.base == 0? 1:( _temp2306 <
_temp2304.base? 1: _temp2306 >= _temp2304.last_plus_one)){ _throw(
Null_Exception);}* _temp2306;})); _temp2299;}); _temp2298;})); _temp2291->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2292= yyls; struct Cyc_Yyltype*
_temp2294= _temp2292.curr +( yylsp_offset - 4); if( _temp2292.base == 0? 1:(
_temp2294 < _temp2292.base? 1: _temp2294 >= _temp2292.last_plus_one)){ _throw(
Null_Exception);}* _temp2294;}).first_line,({ struct _tagged_ptr3 _temp2295=
yyls; struct Cyc_Yyltype* _temp2297= _temp2295.curr +( yylsp_offset - 1); if(
_temp2295.base == 0? 1:( _temp2297 < _temp2295.base? 1: _temp2297 >= _temp2295.last_plus_one)){
_throw( Null_Exception);}* _temp2297;}).last_line); _temp2291;}); _temp2287->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2288= yyvs; struct
_xtunion_struct** _temp2290= _temp2288.curr + yyvsp_offset; if( _temp2288.base
== 0? 1:( _temp2290 < _temp2288.base? 1: _temp2290 >= _temp2288.last_plus_one)){
_throw( Null_Exception);}* _temp2290;})); _temp2287;}); _temp2286;}); _temp2285;});
break; case 7: _LL2284: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2308= yyvs; struct _xtunion_struct** _temp2310= _temp2308.curr
+( yyvsp_offset - 4); if( _temp2308.base == 0? 1:( _temp2310 < _temp2308.base? 1:
_temp2310 >= _temp2308.last_plus_one)){ _throw( Null_Exception);}* _temp2310;})),(
struct _tagged_string)({ char* _temp2311=( char*)"C"; struct _tagged_string
_temp2312; _temp2312.curr= _temp2311; _temp2312.base= _temp2311; _temp2312.last_plus_one=
_temp2311 + 2; _temp2312;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp2313=( char*)"only extern \"C\" { ... } is allowed"; struct
_tagged_string _temp2314; _temp2314.curr= _temp2313; _temp2314.base= _temp2313;
_temp2314.last_plus_one= _temp2313 + 35; _temp2314;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2315= yyls; struct Cyc_Yyltype* _temp2317= _temp2315.curr
+( yylsp_offset - 5); if( _temp2315.base == 0? 1:( _temp2317 < _temp2315.base? 1:
_temp2317 >= _temp2315.last_plus_one)){ _throw( Null_Exception);}* _temp2317;}).first_line,({
struct _tagged_ptr3 _temp2318= yyls; struct Cyc_Yyltype* _temp2320= _temp2318.curr
+( yylsp_offset - 4); if( _temp2318.base == 0? 1:( _temp2320 < _temp2318.base? 1:
_temp2320 >= _temp2318.last_plus_one)){ _throw( Null_Exception);}* _temp2320;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2321=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2321[ 0]=({ struct Cyc_DeclList_tok_struct _temp2322; _temp2322.tag= Cyc_DeclList_tok_tag;
_temp2322.f1=({ struct Cyc_List_List* _temp2323=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2323->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2327=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2327->r=( void*)(( void*)({ struct Cyc_Absyn_ExternC_d_struct* _temp2334=(
struct Cyc_Absyn_ExternC_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct));
_temp2334[ 0]=({ struct Cyc_Absyn_ExternC_d_struct _temp2335; _temp2335.tag= Cyc_Absyn_ExternC_d_tag;
_temp2335.f1= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2336= yyvs;
struct _xtunion_struct** _temp2338= _temp2336.curr +( yyvsp_offset - 2); if(
_temp2336.base == 0? 1:( _temp2338 < _temp2336.base? 1: _temp2338 >= _temp2336.last_plus_one)){
_throw( Null_Exception);}* _temp2338;})); _temp2335;}); _temp2334;})); _temp2327->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2328= yyls; struct Cyc_Yyltype*
_temp2330= _temp2328.curr +( yylsp_offset - 5); if( _temp2328.base == 0? 1:(
_temp2330 < _temp2328.base? 1: _temp2330 >= _temp2328.last_plus_one)){ _throw(
Null_Exception);}* _temp2330;}).first_line,({ struct _tagged_ptr3 _temp2331=
yyls; struct Cyc_Yyltype* _temp2333= _temp2331.curr +( yylsp_offset - 1); if(
_temp2331.base == 0? 1:( _temp2333 < _temp2331.base? 1: _temp2333 >= _temp2331.last_plus_one)){
_throw( Null_Exception);}* _temp2333;}).last_line); _temp2327;}); _temp2323->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2324= yyvs; struct
_xtunion_struct** _temp2326= _temp2324.curr + yyvsp_offset; if( _temp2324.base
== 0? 1:( _temp2326 < _temp2324.base? 1: _temp2326 >= _temp2324.last_plus_one)){
_throw( Null_Exception);}* _temp2326;})); _temp2323;}); _temp2322;}); _temp2321;});
break; case 8: _LL2307: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2340=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2340[ 0]=({ struct Cyc_DeclList_tok_struct _temp2341; _temp2341.tag= Cyc_DeclList_tok_tag;
_temp2341.f1= 0; _temp2341;}); _temp2340;}); break; case 9: _LL2339: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2343=( struct
Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2343[ 0]=({ struct Cyc_DeclList_tok_struct _temp2344; _temp2344.tag= Cyc_DeclList_tok_tag;
_temp2344.f1=({ struct Cyc_List_List* _temp2345=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2345->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2346=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp2346[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp2347; _temp2347.tag= Cyc_Absyn_Fn_d_tag; _temp2347.f1= Cyc_yyget_FnDecl_tok(({
struct _tagged_ptr2 _temp2348= yyvs; struct _xtunion_struct** _temp2350=
_temp2348.curr + yyvsp_offset; if( _temp2348.base == 0? 1:( _temp2350 <
_temp2348.base? 1: _temp2350 >= _temp2348.last_plus_one)){ _throw(
Null_Exception);}* _temp2350;})); _temp2347;}); _temp2346;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2351= yyls; struct Cyc_Yyltype* _temp2353= _temp2351.curr
+ yylsp_offset; if( _temp2351.base == 0? 1:( _temp2353 < _temp2351.base? 1:
_temp2353 >= _temp2351.last_plus_one)){ _throw( Null_Exception);}* _temp2353;}).first_line,({
struct _tagged_ptr3 _temp2354= yyls; struct Cyc_Yyltype* _temp2356= _temp2354.curr
+ yylsp_offset; if( _temp2354.base == 0? 1:( _temp2356 < _temp2354.base? 1:
_temp2356 >= _temp2354.last_plus_one)){ _throw( Null_Exception);}* _temp2356;}).last_line));
_temp2345->tl= 0; _temp2345;}); _temp2344;}); _temp2343;}); break; case 10:
_LL2342: yyval=({ struct _tagged_ptr2 _temp2358= yyvs; struct _xtunion_struct**
_temp2360= _temp2358.curr + yyvsp_offset; if( _temp2358.base == 0? 1:( _temp2360
< _temp2358.base? 1: _temp2360 >= _temp2358.last_plus_one)){ _throw(
Null_Exception);}* _temp2360;}); break; case 11: _LL2357: yyval=( struct
_xtunion_struct*)({ struct Cyc_FnDecl_tok_struct* _temp2362=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2362[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2363; _temp2363.tag= Cyc_FnDecl_tok_tag; _temp2363.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp2364= yyvs; struct
_xtunion_struct** _temp2366= _temp2364.curr +( yyvsp_offset - 1); if( _temp2364.base
== 0? 1:( _temp2366 < _temp2364.base? 1: _temp2366 >= _temp2364.last_plus_one)){
_throw( Null_Exception);}* _temp2366;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2367= yyvs; struct _xtunion_struct** _temp2369= _temp2367.curr
+ yyvsp_offset; if( _temp2367.base == 0? 1:( _temp2369 < _temp2367.base? 1:
_temp2369 >= _temp2367.last_plus_one)){ _throw( Null_Exception);}* _temp2369;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2370= yyls; struct Cyc_Yyltype*
_temp2372= _temp2370.curr +( yylsp_offset - 1); if( _temp2370.base == 0? 1:(
_temp2372 < _temp2370.base? 1: _temp2372 >= _temp2370.last_plus_one)){ _throw(
Null_Exception);}* _temp2372;}).first_line,({ struct _tagged_ptr3 _temp2373=
yyls; struct Cyc_Yyltype* _temp2375= _temp2373.curr + yylsp_offset; if(
_temp2373.base == 0? 1:( _temp2375 < _temp2373.base? 1: _temp2375 >= _temp2373.last_plus_one)){
_throw( Null_Exception);}* _temp2375;}).last_line)); _temp2363;}); _temp2362;});
break; case 12: _LL2361: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2377=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2377[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2378; _temp2378.tag= Cyc_FnDecl_tok_tag;
_temp2378.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2379=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2379->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2380= yyvs; struct
_xtunion_struct** _temp2382= _temp2380.curr +( yyvsp_offset - 2); if( _temp2380.base
== 0? 1:( _temp2382 < _temp2380.base? 1: _temp2382 >= _temp2380.last_plus_one)){
_throw( Null_Exception);}* _temp2382;})); _temp2379;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2383= yyvs; struct _xtunion_struct** _temp2385=
_temp2383.curr +( yyvsp_offset - 1); if( _temp2383.base == 0? 1:( _temp2385 <
_temp2383.base? 1: _temp2385 >= _temp2383.last_plus_one)){ _throw(
Null_Exception);}* _temp2385;})), 0, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2386= yyvs; struct _xtunion_struct** _temp2388= _temp2386.curr +
yyvsp_offset; if( _temp2386.base == 0? 1:( _temp2388 < _temp2386.base? 1:
_temp2388 >= _temp2386.last_plus_one)){ _throw( Null_Exception);}* _temp2388;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2389= yyls; struct Cyc_Yyltype*
_temp2391= _temp2389.curr +( yylsp_offset - 2); if( _temp2389.base == 0? 1:(
_temp2391 < _temp2389.base? 1: _temp2391 >= _temp2389.last_plus_one)){ _throw(
Null_Exception);}* _temp2391;}).first_line,({ struct _tagged_ptr3 _temp2392=
yyls; struct Cyc_Yyltype* _temp2394= _temp2392.curr + yylsp_offset; if(
_temp2392.base == 0? 1:( _temp2394 < _temp2392.base? 1: _temp2394 >= _temp2392.last_plus_one)){
_throw( Null_Exception);}* _temp2394;}).last_line)); _temp2378;}); _temp2377;});
break; case 13: _LL2376: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2396=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2396[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2397; _temp2397.tag= Cyc_FnDecl_tok_tag;
_temp2397.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp2398= yyvs; struct _xtunion_struct** _temp2400= _temp2398.curr
+( yyvsp_offset - 2); if( _temp2398.base == 0? 1:( _temp2400 < _temp2398.base? 1:
_temp2400 >= _temp2398.last_plus_one)){ _throw( Null_Exception);}* _temp2400;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2401= yyvs; struct
_xtunion_struct** _temp2403= _temp2401.curr +( yyvsp_offset - 1); if( _temp2401.base
== 0? 1:( _temp2403 < _temp2401.base? 1: _temp2403 >= _temp2401.last_plus_one)){
_throw( Null_Exception);}* _temp2403;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2404= yyvs; struct _xtunion_struct** _temp2406= _temp2404.curr
+ yyvsp_offset; if( _temp2404.base == 0? 1:( _temp2406 < _temp2404.base? 1:
_temp2406 >= _temp2404.last_plus_one)){ _throw( Null_Exception);}* _temp2406;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2407= yyls; struct Cyc_Yyltype*
_temp2409= _temp2407.curr +( yylsp_offset - 2); if( _temp2407.base == 0? 1:(
_temp2409 < _temp2407.base? 1: _temp2409 >= _temp2407.last_plus_one)){ _throw(
Null_Exception);}* _temp2409;}).first_line,({ struct _tagged_ptr3 _temp2410=
yyls; struct Cyc_Yyltype* _temp2412= _temp2410.curr + yylsp_offset; if(
_temp2410.base == 0? 1:( _temp2412 < _temp2410.base? 1: _temp2412 >= _temp2410.last_plus_one)){
_throw( Null_Exception);}* _temp2412;}).last_line)); _temp2397;}); _temp2396;});
break; case 14: _LL2395: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2414=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2414[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2415; _temp2415.tag= Cyc_FnDecl_tok_tag;
_temp2415.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2416=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2416->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2417= yyvs; struct
_xtunion_struct** _temp2419= _temp2417.curr +( yyvsp_offset - 3); if( _temp2417.base
== 0? 1:( _temp2419 < _temp2417.base? 1: _temp2419 >= _temp2417.last_plus_one)){
_throw( Null_Exception);}* _temp2419;})); _temp2416;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2420= yyvs; struct _xtunion_struct** _temp2422=
_temp2420.curr +( yyvsp_offset - 2); if( _temp2420.base == 0? 1:( _temp2422 <
_temp2420.base? 1: _temp2422 >= _temp2420.last_plus_one)){ _throw(
Null_Exception);}* _temp2422;})), Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2423= yyvs; struct _xtunion_struct** _temp2425= _temp2423.curr +(
yyvsp_offset - 1); if( _temp2423.base == 0? 1:( _temp2425 < _temp2423.base? 1:
_temp2425 >= _temp2423.last_plus_one)){ _throw( Null_Exception);}* _temp2425;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2426= yyvs; struct
_xtunion_struct** _temp2428= _temp2426.curr + yyvsp_offset; if( _temp2426.base
== 0? 1:( _temp2428 < _temp2426.base? 1: _temp2428 >= _temp2426.last_plus_one)){
_throw( Null_Exception);}* _temp2428;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2429= yyls; struct Cyc_Yyltype* _temp2431= _temp2429.curr +(
yylsp_offset - 3); if( _temp2429.base == 0? 1:( _temp2431 < _temp2429.base? 1:
_temp2431 >= _temp2429.last_plus_one)){ _throw( Null_Exception);}* _temp2431;}).first_line,({
struct _tagged_ptr3 _temp2432= yyls; struct Cyc_Yyltype* _temp2434= _temp2432.curr
+ yylsp_offset; if( _temp2432.base == 0? 1:( _temp2434 < _temp2432.base? 1:
_temp2434 >= _temp2432.last_plus_one)){ _throw( Null_Exception);}* _temp2434;}).last_line));
_temp2415;}); _temp2414;}); break; case 15: _LL2413: yyval=( struct
_xtunion_struct*)({ struct Cyc_FnDecl_tok_struct* _temp2436=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2436[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2437; _temp2437.tag= Cyc_FnDecl_tok_tag; _temp2437.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2438=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2438->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2439= yyvs; struct _xtunion_struct** _temp2441= _temp2439.curr
+( yyvsp_offset - 2); if( _temp2439.base == 0? 1:( _temp2441 < _temp2439.base? 1:
_temp2441 >= _temp2439.last_plus_one)){ _throw( Null_Exception);}* _temp2441;}));
_temp2438;}), Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp2442= yyvs;
struct _xtunion_struct** _temp2444= _temp2442.curr +( yyvsp_offset - 1); if(
_temp2442.base == 0? 1:( _temp2444 < _temp2442.base? 1: _temp2444 >= _temp2442.last_plus_one)){
_throw( Null_Exception);}* _temp2444;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2445= yyvs; struct _xtunion_struct** _temp2447= _temp2445.curr
+ yyvsp_offset; if( _temp2445.base == 0? 1:( _temp2447 < _temp2445.base? 1:
_temp2447 >= _temp2445.last_plus_one)){ _throw( Null_Exception);}* _temp2447;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2448= yyls; struct Cyc_Yyltype*
_temp2450= _temp2448.curr +( yylsp_offset - 2); if( _temp2448.base == 0? 1:(
_temp2450 < _temp2448.base? 1: _temp2450 >= _temp2448.last_plus_one)){ _throw(
Null_Exception);}* _temp2450;}).first_line,({ struct _tagged_ptr3 _temp2451=
yyls; struct Cyc_Yyltype* _temp2453= _temp2451.curr + yylsp_offset; if(
_temp2451.base == 0? 1:( _temp2453 < _temp2451.base? 1: _temp2453 >= _temp2451.last_plus_one)){
_throw( Null_Exception);}* _temp2453;}).last_line)); _temp2437;}); _temp2436;});
break; case 16: _LL2435: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2455=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2455[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2456; _temp2456.tag= Cyc_FnDecl_tok_tag;
_temp2456.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2457=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2457->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2458= yyvs; struct
_xtunion_struct** _temp2460= _temp2458.curr +( yyvsp_offset - 3); if( _temp2458.base
== 0? 1:( _temp2460 < _temp2458.base? 1: _temp2460 >= _temp2458.last_plus_one)){
_throw( Null_Exception);}* _temp2460;})); _temp2457;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2461= yyvs; struct _xtunion_struct** _temp2463=
_temp2461.curr +( yyvsp_offset - 2); if( _temp2461.base == 0? 1:( _temp2463 <
_temp2461.base? 1: _temp2463 >= _temp2461.last_plus_one)){ _throw(
Null_Exception);}* _temp2463;})), Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2464= yyvs; struct _xtunion_struct** _temp2466= _temp2464.curr +(
yyvsp_offset - 1); if( _temp2464.base == 0? 1:( _temp2466 < _temp2464.base? 1:
_temp2466 >= _temp2464.last_plus_one)){ _throw( Null_Exception);}* _temp2466;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2467= yyvs; struct
_xtunion_struct** _temp2469= _temp2467.curr + yyvsp_offset; if( _temp2467.base
== 0? 1:( _temp2469 < _temp2467.base? 1: _temp2469 >= _temp2467.last_plus_one)){
_throw( Null_Exception);}* _temp2469;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2470= yyls; struct Cyc_Yyltype* _temp2472= _temp2470.curr +(
yylsp_offset - 3); if( _temp2470.base == 0? 1:( _temp2472 < _temp2470.base? 1:
_temp2472 >= _temp2470.last_plus_one)){ _throw( Null_Exception);}* _temp2472;}).first_line,({
struct _tagged_ptr3 _temp2473= yyls; struct Cyc_Yyltype* _temp2475= _temp2473.curr
+ yylsp_offset; if( _temp2473.base == 0? 1:( _temp2475 < _temp2473.base? 1:
_temp2475 >= _temp2473.last_plus_one)){ _throw( Null_Exception);}* _temp2475;}).last_line));
_temp2456;}); _temp2455;}); break; case 17: _LL2454: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2477= yyvs; struct _xtunion_struct** _temp2479=
_temp2477.curr + yyvsp_offset; if( _temp2477.base == 0? 1:( _temp2479 <
_temp2477.base? 1: _temp2479 >= _temp2477.last_plus_one)){ _throw(
Null_Exception);}* _temp2479;}))); yyval=({ struct _tagged_ptr2 _temp2480= yyvs;
struct _xtunion_struct** _temp2482= _temp2480.curr + yyvsp_offset; if( _temp2480.base
== 0? 1:( _temp2482 < _temp2480.base? 1: _temp2482 >= _temp2480.last_plus_one)){
_throw( Null_Exception);}* _temp2482;}); break; case 18: _LL2476: Cyc_Lex_leave_using();
break; case 19: _LL2483: Cyc_Lex_enter_namespace(({ struct _tagged_string*
_temp2485=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2485[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2486= yyvs;
struct _xtunion_struct** _temp2488= _temp2486.curr + yyvsp_offset; if( _temp2486.base
== 0? 1:( _temp2488 < _temp2486.base? 1: _temp2488 >= _temp2486.last_plus_one)){
_throw( Null_Exception);}* _temp2488;})); _temp2485;})); yyval=({ struct
_tagged_ptr2 _temp2489= yyvs; struct _xtunion_struct** _temp2491= _temp2489.curr
+ yyvsp_offset; if( _temp2489.base == 0? 1:( _temp2491 < _temp2489.base? 1:
_temp2491 >= _temp2489.last_plus_one)){ _throw( Null_Exception);}* _temp2491;});
break; case 20: _LL2484: Cyc_Lex_leave_namespace(); break; case 21: _LL2492:
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2494=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2494[ 0]=({ struct Cyc_DeclList_tok_struct _temp2495; _temp2495.tag= Cyc_DeclList_tok_tag;
_temp2495.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2496= yyvs; struct _xtunion_struct** _temp2498= _temp2496.curr
+( yyvsp_offset - 1); if( _temp2496.base == 0? 1:( _temp2498 < _temp2496.base? 1:
_temp2498 >= _temp2496.last_plus_one)){ _throw( Null_Exception);}* _temp2498;})),
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2499= yyls; struct
Cyc_Yyltype* _temp2501= _temp2499.curr +( yylsp_offset - 1); if( _temp2499.base
== 0? 1:( _temp2501 < _temp2499.base? 1: _temp2501 >= _temp2499.last_plus_one)){
_throw( Null_Exception);}* _temp2501;}).first_line,({ struct _tagged_ptr3
_temp2502= yyls; struct Cyc_Yyltype* _temp2504= _temp2502.curr +( yylsp_offset -
1); if( _temp2502.base == 0? 1:( _temp2504 < _temp2502.base? 1: _temp2504 >=
_temp2502.last_plus_one)){ _throw( Null_Exception);}* _temp2504;}).last_line));
_temp2495;}); _temp2494;}); break; case 22: _LL2493: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2506=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2506[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2507; _temp2507.tag= Cyc_DeclList_tok_tag; _temp2507.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2508= yyvs; struct
_xtunion_struct** _temp2510= _temp2508.curr +( yyvsp_offset - 2); if( _temp2508.base
== 0? 1:( _temp2510 < _temp2508.base? 1: _temp2510 >= _temp2508.last_plus_one)){
_throw( Null_Exception);}* _temp2510;})), Cyc_yyget_InitDeclList_tok(({ struct
_tagged_ptr2 _temp2511= yyvs; struct _xtunion_struct** _temp2513= _temp2511.curr
+( yyvsp_offset - 1); if( _temp2511.base == 0? 1:( _temp2513 < _temp2511.base? 1:
_temp2513 >= _temp2511.last_plus_one)){ _throw( Null_Exception);}* _temp2513;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2514= yyls; struct Cyc_Yyltype*
_temp2516= _temp2514.curr +( yylsp_offset - 2); if( _temp2514.base == 0? 1:(
_temp2516 < _temp2514.base? 1: _temp2516 >= _temp2514.last_plus_one)){ _throw(
Null_Exception);}* _temp2516;}).first_line,({ struct _tagged_ptr3 _temp2517=
yyls; struct Cyc_Yyltype* _temp2519= _temp2517.curr + yylsp_offset; if(
_temp2517.base == 0? 1:( _temp2519 < _temp2517.base? 1: _temp2519 >= _temp2517.last_plus_one)){
_throw( Null_Exception);}* _temp2519;}).last_line)); _temp2507;}); _temp2506;});
break; case 23: _LL2505: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2521=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2521[ 0]=({ struct Cyc_DeclList_tok_struct _temp2522; _temp2522.tag= Cyc_DeclList_tok_tag;
_temp2522.f1=({ struct Cyc_List_List* _temp2523=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2523->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp2524= yyvs; struct
_xtunion_struct** _temp2526= _temp2524.curr +( yyvsp_offset - 3); if( _temp2524.base
== 0? 1:( _temp2526 < _temp2524.base? 1: _temp2526 >= _temp2524.last_plus_one)){
_throw( Null_Exception);}* _temp2526;})), 0, Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp2527= yyvs; struct _xtunion_struct** _temp2529= _temp2527.curr
+( yyvsp_offset - 1); if( _temp2527.base == 0? 1:( _temp2529 < _temp2527.base? 1:
_temp2529 >= _temp2527.last_plus_one)){ _throw( Null_Exception);}* _temp2529;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2530= yyls; struct Cyc_Yyltype*
_temp2532= _temp2530.curr +( yylsp_offset - 4); if( _temp2530.base == 0? 1:(
_temp2532 < _temp2530.base? 1: _temp2532 >= _temp2530.last_plus_one)){ _throw(
Null_Exception);}* _temp2532;}).first_line,({ struct _tagged_ptr3 _temp2533=
yyls; struct Cyc_Yyltype* _temp2535= _temp2533.curr + yylsp_offset; if(
_temp2533.base == 0? 1:( _temp2535 < _temp2533.base? 1: _temp2535 >= _temp2533.last_plus_one)){
_throw( Null_Exception);}* _temp2535;}).last_line)); _temp2523->tl= 0; _temp2523;});
_temp2522;}); _temp2521;}); break; case 24: _LL2520: yyval=({ struct
_tagged_ptr2 _temp2537= yyvs; struct _xtunion_struct** _temp2539= _temp2537.curr
+ yyvsp_offset; if( _temp2537.base == 0? 1:( _temp2539 < _temp2537.base? 1:
_temp2539 >= _temp2537.last_plus_one)){ _throw( Null_Exception);}* _temp2539;});
break; case 25: _LL2536: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2541=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2541[ 0]=({ struct Cyc_DeclList_tok_struct _temp2542; _temp2542.tag= Cyc_DeclList_tok_tag;
_temp2542.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2543= yyvs; struct _xtunion_struct** _temp2545= _temp2543.curr +(
yyvsp_offset - 1); if( _temp2543.base == 0? 1:( _temp2545 < _temp2543.base? 1:
_temp2545 >= _temp2543.last_plus_one)){ _throw( Null_Exception);}* _temp2545;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2546= yyvs; struct
_xtunion_struct** _temp2548= _temp2546.curr + yyvsp_offset; if( _temp2546.base
== 0? 1:( _temp2548 < _temp2546.base? 1: _temp2548 >= _temp2546.last_plus_one)){
_throw( Null_Exception);}* _temp2548;}))); _temp2542;}); _temp2541;}); break;
case 26: _LL2540: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct*
_temp2550=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2550[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2551; _temp2551.tag= Cyc_DeclSpec_tok_tag;
_temp2551.f1=({ struct Cyc_Parse_Declaration_spec* _temp2552=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2552->sc=({ struct
Cyc_Core_Opt* _temp2556=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2556->v=( void*) Cyc_yyget_StorageClass_tok(({ struct _tagged_ptr2
_temp2557= yyvs; struct _xtunion_struct** _temp2559= _temp2557.curr +(
yyvsp_offset - 1); if( _temp2557.base == 0? 1:( _temp2559 < _temp2557.base? 1:
_temp2559 >= _temp2557.last_plus_one)){ _throw( Null_Exception);}* _temp2559;}));
_temp2556;}); _temp2552->tq= Cyc_Absyn_empty_tqual(); _temp2552->type_specs= 0;
_temp2552->is_inline= 0; _temp2552->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2553= yyvs; struct _xtunion_struct** _temp2555=
_temp2553.curr + yyvsp_offset; if( _temp2553.base == 0? 1:( _temp2555 <
_temp2553.base? 1: _temp2555 >= _temp2553.last_plus_one)){ _throw(
Null_Exception);}* _temp2555;})); _temp2552;}); _temp2551;}); _temp2550;});
break; case 27: _LL2549: if(( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2561= yyvs; struct _xtunion_struct** _temp2563= _temp2561.curr +
yyvsp_offset; if( _temp2561.base == 0? 1:( _temp2563 < _temp2561.base? 1:
_temp2563 >= _temp2561.last_plus_one)){ _throw( Null_Exception);}* _temp2563;})))->sc
!= 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp2564=( char*)"Only one storage class is allowed in a declaration";
struct _tagged_string _temp2565; _temp2565.curr= _temp2564; _temp2565.base=
_temp2564; _temp2565.last_plus_one= _temp2564 + 51; _temp2565;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2566= yyls; struct Cyc_Yyltype* _temp2568= _temp2566.curr
+( yylsp_offset - 2); if( _temp2566.base == 0? 1:( _temp2568 < _temp2566.base? 1:
_temp2568 >= _temp2566.last_plus_one)){ _throw( Null_Exception);}* _temp2568;}).first_line,({
struct _tagged_ptr3 _temp2569= yyls; struct Cyc_Yyltype* _temp2571= _temp2569.curr
+( yylsp_offset - 1); if( _temp2569.base == 0? 1:( _temp2571 < _temp2569.base? 1:
_temp2571 >= _temp2569.last_plus_one)){ _throw( Null_Exception);}* _temp2571;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2572=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2572[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2573; _temp2573.tag= Cyc_DeclSpec_tok_tag;
_temp2573.f1=({ struct Cyc_Parse_Declaration_spec* _temp2574=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2574->sc=({ struct
Cyc_Core_Opt* _temp2590=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2590->v=( void*) Cyc_yyget_StorageClass_tok(({ struct _tagged_ptr2
_temp2591= yyvs; struct _xtunion_struct** _temp2593= _temp2591.curr +(
yyvsp_offset - 2); if( _temp2591.base == 0? 1:( _temp2593 < _temp2591.base? 1:
_temp2593 >= _temp2591.last_plus_one)){ _throw( Null_Exception);}* _temp2593;}));
_temp2590;}); _temp2574->tq=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2587= yyvs; struct _xtunion_struct** _temp2589= _temp2587.curr +
yyvsp_offset; if( _temp2587.base == 0? 1:( _temp2589 < _temp2587.base? 1:
_temp2589 >= _temp2587.last_plus_one)){ _throw( Null_Exception);}* _temp2589;})))->tq;
_temp2574->type_specs=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2584=
yyvs; struct _xtunion_struct** _temp2586= _temp2584.curr + yyvsp_offset; if(
_temp2584.base == 0? 1:( _temp2586 < _temp2584.base? 1: _temp2586 >= _temp2584.last_plus_one)){
_throw( Null_Exception);}* _temp2586;})))->type_specs; _temp2574->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2581= yyvs; struct
_xtunion_struct** _temp2583= _temp2581.curr + yyvsp_offset; if( _temp2581.base
== 0? 1:( _temp2583 < _temp2581.base? 1: _temp2583 >= _temp2581.last_plus_one)){
_throw( Null_Exception);}* _temp2583;})))->is_inline; _temp2574->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2575= yyvs; struct
_xtunion_struct** _temp2577= _temp2575.curr +( yyvsp_offset - 1); if( _temp2575.base
== 0? 1:( _temp2577 < _temp2575.base? 1: _temp2577 >= _temp2575.last_plus_one)){
_throw( Null_Exception);}* _temp2577;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2578= yyvs; struct _xtunion_struct** _temp2580= _temp2578.curr
+ yyvsp_offset; if( _temp2578.base == 0? 1:( _temp2580 < _temp2578.base? 1:
_temp2580 >= _temp2578.last_plus_one)){ _throw( Null_Exception);}* _temp2580;})))->attributes);
_temp2574;}); _temp2573;}); _temp2572;}); break; case 28: _LL2560: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2595=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2595[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2596; _temp2596.tag= Cyc_DeclSpec_tok_tag;
_temp2596.f1=({ struct Cyc_Parse_Declaration_spec* _temp2597=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2597->sc= 0;
_temp2597->tq= Cyc_Absyn_empty_tqual(); _temp2597->type_specs=({ struct Cyc_List_List*
_temp2601=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2601->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2602= yyvs; struct _xtunion_struct** _temp2604= _temp2602.curr +(
yyvsp_offset - 1); if( _temp2602.base == 0? 1:( _temp2604 < _temp2602.base? 1:
_temp2604 >= _temp2602.last_plus_one)){ _throw( Null_Exception);}* _temp2604;}));
_temp2601->tl= 0; _temp2601;}); _temp2597->is_inline= 0; _temp2597->attributes=
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2598= yyvs; struct
_xtunion_struct** _temp2600= _temp2598.curr + yyvsp_offset; if( _temp2598.base
== 0? 1:( _temp2600 < _temp2598.base? 1: _temp2600 >= _temp2598.last_plus_one)){
_throw( Null_Exception);}* _temp2600;})); _temp2597;}); _temp2596;}); _temp2595;});
break; case 29: _LL2594: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct*
_temp2606=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2606[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2607; _temp2607.tag= Cyc_DeclSpec_tok_tag;
_temp2607.f1=({ struct Cyc_Parse_Declaration_spec* _temp2608=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2608->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2628= yyvs; struct _xtunion_struct** _temp2630=
_temp2628.curr + yyvsp_offset; if( _temp2628.base == 0? 1:( _temp2630 <
_temp2628.base? 1: _temp2630 >= _temp2628.last_plus_one)){ _throw(
Null_Exception);}* _temp2630;})))->sc; _temp2608->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2625= yyvs; struct _xtunion_struct** _temp2627=
_temp2625.curr + yyvsp_offset; if( _temp2625.base == 0? 1:( _temp2627 <
_temp2625.base? 1: _temp2627 >= _temp2625.last_plus_one)){ _throw(
Null_Exception);}* _temp2627;})))->tq; _temp2608->type_specs=({ struct Cyc_List_List*
_temp2618=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2618->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2622= yyvs; struct _xtunion_struct** _temp2624= _temp2622.curr +(
yyvsp_offset - 2); if( _temp2622.base == 0? 1:( _temp2624 < _temp2622.base? 1:
_temp2624 >= _temp2622.last_plus_one)){ _throw( Null_Exception);}* _temp2624;}));
_temp2618->tl=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2619= yyvs;
struct _xtunion_struct** _temp2621= _temp2619.curr + yyvsp_offset; if( _temp2619.base
== 0? 1:( _temp2621 < _temp2619.base? 1: _temp2621 >= _temp2619.last_plus_one)){
_throw( Null_Exception);}* _temp2621;})))->type_specs; _temp2618;}); _temp2608->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2615= yyvs; struct
_xtunion_struct** _temp2617= _temp2615.curr + yyvsp_offset; if( _temp2615.base
== 0? 1:( _temp2617 < _temp2615.base? 1: _temp2617 >= _temp2615.last_plus_one)){
_throw( Null_Exception);}* _temp2617;})))->is_inline; _temp2608->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2609= yyvs; struct
_xtunion_struct** _temp2611= _temp2609.curr +( yyvsp_offset - 1); if( _temp2609.base
== 0? 1:( _temp2611 < _temp2609.base? 1: _temp2611 >= _temp2609.last_plus_one)){
_throw( Null_Exception);}* _temp2611;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2612= yyvs; struct _xtunion_struct** _temp2614= _temp2612.curr
+ yyvsp_offset; if( _temp2612.base == 0? 1:( _temp2614 < _temp2612.base? 1:
_temp2614 >= _temp2612.last_plus_one)){ _throw( Null_Exception);}* _temp2614;})))->attributes);
_temp2608;}); _temp2607;}); _temp2606;}); break; case 30: _LL2605: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2632=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2632[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2633; _temp2633.tag= Cyc_DeclSpec_tok_tag;
_temp2633.f1=({ struct Cyc_Parse_Declaration_spec* _temp2634=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2634->sc= 0;
_temp2634->tq= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp2638= yyvs;
struct _xtunion_struct** _temp2640= _temp2638.curr +( yyvsp_offset - 1); if(
_temp2638.base == 0? 1:( _temp2640 < _temp2638.base? 1: _temp2640 >= _temp2638.last_plus_one)){
_throw( Null_Exception);}* _temp2640;})); _temp2634->type_specs= 0; _temp2634->is_inline=
0; _temp2634->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2635= yyvs; struct _xtunion_struct** _temp2637= _temp2635.curr +
yyvsp_offset; if( _temp2635.base == 0? 1:( _temp2637 < _temp2635.base? 1:
_temp2637 >= _temp2635.last_plus_one)){ _throw( Null_Exception);}* _temp2637;}));
_temp2634;}); _temp2633;}); _temp2632;}); break; case 31: _LL2631: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2642=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2642[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2643; _temp2643.tag= Cyc_DeclSpec_tok_tag;
_temp2643.f1=({ struct Cyc_Parse_Declaration_spec* _temp2644=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2644->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2663= yyvs; struct _xtunion_struct** _temp2665=
_temp2663.curr + yyvsp_offset; if( _temp2663.base == 0? 1:( _temp2665 <
_temp2663.base? 1: _temp2665 >= _temp2663.last_plus_one)){ _throw(
Null_Exception);}* _temp2665;})))->sc; _temp2644->tq= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp2657= yyvs; struct
_xtunion_struct** _temp2659= _temp2657.curr +( yyvsp_offset - 2); if( _temp2657.base
== 0? 1:( _temp2659 < _temp2657.base? 1: _temp2659 >= _temp2657.last_plus_one)){
_throw( Null_Exception);}* _temp2659;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2660= yyvs; struct _xtunion_struct** _temp2662= _temp2660.curr
+ yyvsp_offset; if( _temp2660.base == 0? 1:( _temp2662 < _temp2660.base? 1:
_temp2662 >= _temp2660.last_plus_one)){ _throw( Null_Exception);}* _temp2662;})))->tq);
_temp2644->type_specs=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2654=
yyvs; struct _xtunion_struct** _temp2656= _temp2654.curr + yyvsp_offset; if(
_temp2654.base == 0? 1:( _temp2656 < _temp2654.base? 1: _temp2656 >= _temp2654.last_plus_one)){
_throw( Null_Exception);}* _temp2656;})))->type_specs; _temp2644->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2651= yyvs; struct
_xtunion_struct** _temp2653= _temp2651.curr + yyvsp_offset; if( _temp2651.base
== 0? 1:( _temp2653 < _temp2651.base? 1: _temp2653 >= _temp2651.last_plus_one)){
_throw( Null_Exception);}* _temp2653;})))->is_inline; _temp2644->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2645= yyvs; struct
_xtunion_struct** _temp2647= _temp2645.curr +( yyvsp_offset - 1); if( _temp2645.base
== 0? 1:( _temp2647 < _temp2645.base? 1: _temp2647 >= _temp2645.last_plus_one)){
_throw( Null_Exception);}* _temp2647;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2648= yyvs; struct _xtunion_struct** _temp2650= _temp2648.curr
+ yyvsp_offset; if( _temp2648.base == 0? 1:( _temp2650 < _temp2648.base? 1:
_temp2650 >= _temp2648.last_plus_one)){ _throw( Null_Exception);}* _temp2650;})))->attributes);
_temp2644;}); _temp2643;}); _temp2642;}); break; case 32: _LL2641: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2667=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2667[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2668; _temp2668.tag= Cyc_DeclSpec_tok_tag;
_temp2668.f1=({ struct Cyc_Parse_Declaration_spec* _temp2669=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2669->sc= 0;
_temp2669->tq= Cyc_Absyn_empty_tqual(); _temp2669->type_specs= 0; _temp2669->is_inline=
1; _temp2669->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2670= yyvs; struct _xtunion_struct** _temp2672= _temp2670.curr +
yyvsp_offset; if( _temp2670.base == 0? 1:( _temp2672 < _temp2670.base? 1:
_temp2672 >= _temp2670.last_plus_one)){ _throw( Null_Exception);}* _temp2672;}));
_temp2669;}); _temp2668;}); _temp2667;}); break; case 33: _LL2666: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2674=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2674[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2675; _temp2675.tag= Cyc_DeclSpec_tok_tag;
_temp2675.f1=({ struct Cyc_Parse_Declaration_spec* _temp2676=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2676->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2689= yyvs; struct _xtunion_struct** _temp2691=
_temp2689.curr + yyvsp_offset; if( _temp2689.base == 0? 1:( _temp2691 <
_temp2689.base? 1: _temp2691 >= _temp2689.last_plus_one)){ _throw(
Null_Exception);}* _temp2691;})))->sc; _temp2676->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2686= yyvs; struct _xtunion_struct** _temp2688=
_temp2686.curr + yyvsp_offset; if( _temp2686.base == 0? 1:( _temp2688 <
_temp2686.base? 1: _temp2688 >= _temp2686.last_plus_one)){ _throw(
Null_Exception);}* _temp2688;})))->tq; _temp2676->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2683= yyvs; struct _xtunion_struct** _temp2685=
_temp2683.curr + yyvsp_offset; if( _temp2683.base == 0? 1:( _temp2685 <
_temp2683.base? 1: _temp2685 >= _temp2683.last_plus_one)){ _throw(
Null_Exception);}* _temp2685;})))->type_specs; _temp2676->is_inline= 1;
_temp2676->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2677= yyvs; struct _xtunion_struct** _temp2679=
_temp2677.curr +( yyvsp_offset - 1); if( _temp2677.base == 0? 1:( _temp2679 <
_temp2677.base? 1: _temp2679 >= _temp2677.last_plus_one)){ _throw(
Null_Exception);}* _temp2679;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2680= yyvs; struct _xtunion_struct** _temp2682= _temp2680.curr +
yyvsp_offset; if( _temp2680.base == 0? 1:( _temp2682 < _temp2680.base? 1:
_temp2682 >= _temp2680.last_plus_one)){ _throw( Null_Exception);}* _temp2682;})))->attributes);
_temp2676;}); _temp2675;}); _temp2674;}); break; case 34: _LL2673: yyval=(
struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2693=(
struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2693[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2694; _temp2694.tag=
Cyc_StorageClass_tok_tag; _temp2694.f1=( void*)(( void*) Cyc_Parse_Auto_sc);
_temp2694;}); _temp2693;}); break; case 35: _LL2692: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2696=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2696[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2697; _temp2697.tag= Cyc_StorageClass_tok_tag;
_temp2697.f1=( void*)(( void*) Cyc_Parse_Register_sc); _temp2697;}); _temp2696;});
break; case 36: _LL2695: yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct*
_temp2699=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2699[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2700; _temp2700.tag=
Cyc_StorageClass_tok_tag; _temp2700.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp2700;}); _temp2699;}); break; case 37: _LL2698: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2702=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2702[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2703; _temp2703.tag= Cyc_StorageClass_tok_tag;
_temp2703.f1=( void*)(( void*) Cyc_Parse_Extern_sc); _temp2703;}); _temp2702;});
break; case 38: _LL2701: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2705= yyvs; struct _xtunion_struct** _temp2707= _temp2705.curr
+ yyvsp_offset; if( _temp2705.base == 0? 1:( _temp2707 < _temp2705.base? 1:
_temp2707 >= _temp2705.last_plus_one)){ _throw( Null_Exception);}* _temp2707;})),(
struct _tagged_string)({ char* _temp2708=( char*)"C"; struct _tagged_string
_temp2709; _temp2709.curr= _temp2708; _temp2709.base= _temp2708; _temp2709.last_plus_one=
_temp2708 + 2; _temp2709;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp2710=( char*)"only extern or extern \"C\" is allowed"; struct
_tagged_string _temp2711; _temp2711.curr= _temp2710; _temp2711.base= _temp2710;
_temp2711.last_plus_one= _temp2710 + 37; _temp2711;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2712= yyls; struct Cyc_Yyltype* _temp2714= _temp2712.curr
+( yylsp_offset - 1); if( _temp2712.base == 0? 1:( _temp2714 < _temp2712.base? 1:
_temp2714 >= _temp2712.last_plus_one)){ _throw( Null_Exception);}* _temp2714;}).first_line,({
struct _tagged_ptr3 _temp2715= yyls; struct Cyc_Yyltype* _temp2717= _temp2715.curr
+ yylsp_offset; if( _temp2715.base == 0? 1:( _temp2717 < _temp2715.base? 1:
_temp2717 >= _temp2715.last_plus_one)){ _throw( Null_Exception);}* _temp2717;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2718=(
struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2718[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2719; _temp2719.tag=
Cyc_StorageClass_tok_tag; _temp2719.f1=( void*)(( void*) Cyc_Parse_ExternC_sc);
_temp2719;}); _temp2718;}); break; case 39: _LL2704: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2721=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2721[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2722; _temp2722.tag= Cyc_StorageClass_tok_tag;
_temp2722.f1=( void*)(( void*) Cyc_Parse_Typedef_sc); _temp2722;}); _temp2721;});
break; case 40: _LL2720: yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct*
_temp2724=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2724[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2725; _temp2725.tag=
Cyc_StorageClass_tok_tag; _temp2725.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp2725;}); _temp2724;}); break; case 41: _LL2723: yyval=( struct
_xtunion_struct*)({ struct Cyc_AttributeList_tok_struct* _temp2727=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2727[ 0]=({
struct Cyc_AttributeList_tok_struct _temp2728; _temp2728.tag= Cyc_AttributeList_tok_tag;
_temp2728.f1= 0; _temp2728;}); _temp2727;}); break; case 42: _LL2726: yyval=({
struct _tagged_ptr2 _temp2730= yyvs; struct _xtunion_struct** _temp2732=
_temp2730.curr + yyvsp_offset; if( _temp2730.base == 0? 1:( _temp2732 <
_temp2730.base? 1: _temp2732 >= _temp2730.last_plus_one)){ _throw(
Null_Exception);}* _temp2732;}); break; case 43: _LL2729: yyval=( struct
_xtunion_struct*)({ struct Cyc_AttributeList_tok_struct* _temp2734=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2734[ 0]=({
struct Cyc_AttributeList_tok_struct _temp2735; _temp2735.tag= Cyc_AttributeList_tok_tag;
_temp2735.f1= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2736= yyvs;
struct _xtunion_struct** _temp2738= _temp2736.curr +( yyvsp_offset - 2); if(
_temp2736.base == 0? 1:( _temp2738 < _temp2736.base? 1: _temp2738 >= _temp2736.last_plus_one)){
_throw( Null_Exception);}* _temp2738;})); _temp2735;}); _temp2734;}); break;
case 44: _LL2733: yyval=( struct _xtunion_struct*)({ struct Cyc_AttributeList_tok_struct*
_temp2740=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2740[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2741; _temp2741.tag=
Cyc_AttributeList_tok_tag; _temp2741.f1=({ struct Cyc_List_List* _temp2742=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2742->hd=(
void*) Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2743= yyvs; struct
_xtunion_struct** _temp2745= _temp2743.curr + yyvsp_offset; if( _temp2743.base
== 0? 1:( _temp2745 < _temp2743.base? 1: _temp2745 >= _temp2743.last_plus_one)){
_throw( Null_Exception);}* _temp2745;})); _temp2742->tl= 0; _temp2742;});
_temp2741;}); _temp2740;}); break; case 45: _LL2739: yyval=( struct
_xtunion_struct*)({ struct Cyc_AttributeList_tok_struct* _temp2747=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2747[ 0]=({
struct Cyc_AttributeList_tok_struct _temp2748; _temp2748.tag= Cyc_AttributeList_tok_tag;
_temp2748.f1=({ struct Cyc_List_List* _temp2749=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2749->hd=( void*) Cyc_yyget_Attribute_tok(({
struct _tagged_ptr2 _temp2753= yyvs; struct _xtunion_struct** _temp2755=
_temp2753.curr +( yyvsp_offset - 2); if( _temp2753.base == 0? 1:( _temp2755 <
_temp2753.base? 1: _temp2755 >= _temp2753.last_plus_one)){ _throw(
Null_Exception);}* _temp2755;})); _temp2749->tl= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2750= yyvs; struct _xtunion_struct** _temp2752=
_temp2750.curr + yyvsp_offset; if( _temp2750.base == 0? 1:( _temp2752 <
_temp2750.base? 1: _temp2752 >= _temp2750.last_plus_one)){ _throw(
Null_Exception);}* _temp2752;})); _temp2749;}); _temp2748;}); _temp2747;});
break; case 46: _LL2746: { struct _tagged_string s= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2831= yyvs; struct _xtunion_struct** _temp2833=
_temp2831.curr + yyvsp_offset; if( _temp2831.base == 0? 1:( _temp2833 <
_temp2831.base? 1: _temp2833 >= _temp2831.last_plus_one)){ _throw(
Null_Exception);}* _temp2833;})); void* a; if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2757=( char*)"stdcall"; struct _tagged_string
_temp2758; _temp2758.curr= _temp2757; _temp2758.base= _temp2757; _temp2758.last_plus_one=
_temp2757 + 8; _temp2758;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2759=( char*)"__stdcall__"; struct _tagged_string
_temp2760; _temp2760.curr= _temp2759; _temp2760.base= _temp2759; _temp2760.last_plus_one=
_temp2759 + 12; _temp2760;})) == 0){ a=( void*) Cyc_Absyn_Stdcall_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2761=( char*)"cdecl";
struct _tagged_string _temp2762; _temp2762.curr= _temp2761; _temp2762.base=
_temp2761; _temp2762.last_plus_one= _temp2761 + 6; _temp2762;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2763=( char*)"__cdecl__"; struct
_tagged_string _temp2764; _temp2764.curr= _temp2763; _temp2764.base= _temp2763;
_temp2764.last_plus_one= _temp2763 + 10; _temp2764;})) == 0){ a=( void*) Cyc_Absyn_Cdecl_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2765=(
char*)"noreturn"; struct _tagged_string _temp2766; _temp2766.curr= _temp2765;
_temp2766.base= _temp2765; _temp2766.last_plus_one= _temp2765 + 9; _temp2766;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2767=( char*)"__noreturn__";
struct _tagged_string _temp2768; _temp2768.curr= _temp2767; _temp2768.base=
_temp2767; _temp2768.last_plus_one= _temp2767 + 13; _temp2768;})) == 0){ a=(
void*) Cyc_Absyn_Noreturn_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2769=( char*)"noreturn"; struct _tagged_string
_temp2770; _temp2770.curr= _temp2769; _temp2770.base= _temp2769; _temp2770.last_plus_one=
_temp2769 + 9; _temp2770;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2771=( char*)"__noreturn__"; struct _tagged_string
_temp2772; _temp2772.curr= _temp2771; _temp2772.base= _temp2771; _temp2772.last_plus_one=
_temp2771 + 13; _temp2772;})) == 0){ a=( void*) Cyc_Absyn_Noreturn_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2773=( char*)"__const__";
struct _tagged_string _temp2774; _temp2774.curr= _temp2773; _temp2774.base=
_temp2773; _temp2774.last_plus_one= _temp2773 + 10; _temp2774;})) == 0){ a=(
void*) Cyc_Absyn_Const_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2775=( char*)"aligned"; struct _tagged_string
_temp2776; _temp2776.curr= _temp2775; _temp2776.base= _temp2775; _temp2776.last_plus_one=
_temp2775 + 8; _temp2776;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2777=( char*)"__aligned__"; struct _tagged_string
_temp2778; _temp2778.curr= _temp2777; _temp2778.base= _temp2777; _temp2778.last_plus_one=
_temp2777 + 12; _temp2778;})) == 0){ a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp2779=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct));
_temp2779[ 0]=({ struct Cyc_Absyn_Aligned_att_struct _temp2780; _temp2780.tag=
Cyc_Absyn_Aligned_att_tag; _temp2780.f1= - 1; _temp2780;}); _temp2779;});} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2781=( char*)"packed";
struct _tagged_string _temp2782; _temp2782.curr= _temp2781; _temp2782.base=
_temp2781; _temp2782.last_plus_one= _temp2781 + 7; _temp2782;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2783=( char*)"__packed__"; struct
_tagged_string _temp2784; _temp2784.curr= _temp2783; _temp2784.base= _temp2783;
_temp2784.last_plus_one= _temp2783 + 11; _temp2784;})) == 0){ a=( void*) Cyc_Absyn_Packed_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2785=(
char*)"shared"; struct _tagged_string _temp2786; _temp2786.curr= _temp2785;
_temp2786.base= _temp2785; _temp2786.last_plus_one= _temp2785 + 7; _temp2786;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2787=( char*)"__shared__";
struct _tagged_string _temp2788; _temp2788.curr= _temp2787; _temp2788.base=
_temp2787; _temp2788.last_plus_one= _temp2787 + 11; _temp2788;})) == 0){ a=(
void*) Cyc_Absyn_Shared_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2789=( char*)"unused"; struct _tagged_string
_temp2790; _temp2790.curr= _temp2789; _temp2790.base= _temp2789; _temp2790.last_plus_one=
_temp2789 + 7; _temp2790;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2791=( char*)"__unused__"; struct _tagged_string
_temp2792; _temp2792.curr= _temp2791; _temp2792.base= _temp2791; _temp2792.last_plus_one=
_temp2791 + 11; _temp2792;})) == 0){ a=( void*) Cyc_Absyn_Unused_att;} else{ if(
Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2793=( char*)"weak";
struct _tagged_string _temp2794; _temp2794.curr= _temp2793; _temp2794.base=
_temp2793; _temp2794.last_plus_one= _temp2793 + 5; _temp2794;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2795=( char*)"__weak__"; struct
_tagged_string _temp2796; _temp2796.curr= _temp2795; _temp2796.base= _temp2795;
_temp2796.last_plus_one= _temp2795 + 9; _temp2796;})) == 0){ a=( void*) Cyc_Absyn_Weak_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2797=(
char*)"dllimport"; struct _tagged_string _temp2798; _temp2798.curr= _temp2797;
_temp2798.base= _temp2797; _temp2798.last_plus_one= _temp2797 + 10; _temp2798;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2799=( char*)"__dllimport__";
struct _tagged_string _temp2800; _temp2800.curr= _temp2799; _temp2800.base=
_temp2799; _temp2800.last_plus_one= _temp2799 + 14; _temp2800;})) == 0){ a=(
void*) Cyc_Absyn_Dllimport_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2801=( char*)"dllexport"; struct _tagged_string
_temp2802; _temp2802.curr= _temp2801; _temp2802.base= _temp2801; _temp2802.last_plus_one=
_temp2801 + 10; _temp2802;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2803=( char*)"__dllexport__"; struct _tagged_string
_temp2804; _temp2804.curr= _temp2803; _temp2804.base= _temp2803; _temp2804.last_plus_one=
_temp2803 + 14; _temp2804;})) == 0){ a=( void*) Cyc_Absyn_Dllexport_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2805=( char*)"no_instrument_function";
struct _tagged_string _temp2806; _temp2806.curr= _temp2805; _temp2806.base=
_temp2805; _temp2806.last_plus_one= _temp2805 + 23; _temp2806;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2807=( char*)"__no_instrument_function__";
struct _tagged_string _temp2808; _temp2808.curr= _temp2807; _temp2808.base=
_temp2807; _temp2808.last_plus_one= _temp2807 + 27; _temp2808;})) == 0){ a=(
void*) Cyc_Absyn_No_instrument_function_att;} else{ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2809=( char*)"constructor"; struct
_tagged_string _temp2810; _temp2810.curr= _temp2809; _temp2810.base= _temp2809;
_temp2810.last_plus_one= _temp2809 + 12; _temp2810;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2811=( char*)"__constructor__"; struct
_tagged_string _temp2812; _temp2812.curr= _temp2811; _temp2812.base= _temp2811;
_temp2812.last_plus_one= _temp2811 + 16; _temp2812;})) == 0){ a=( void*) Cyc_Absyn_Constructor_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2813=(
char*)"destructor"; struct _tagged_string _temp2814; _temp2814.curr= _temp2813;
_temp2814.base= _temp2813; _temp2814.last_plus_one= _temp2813 + 11; _temp2814;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2815=( char*)"__destructor__";
struct _tagged_string _temp2816; _temp2816.curr= _temp2815; _temp2816.base=
_temp2815; _temp2816.last_plus_one= _temp2815 + 15; _temp2816;})) == 0){ a=(
void*) Cyc_Absyn_Destructor_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2817=( char*)"no_check_memory_usage"; struct
_tagged_string _temp2818; _temp2818.curr= _temp2817; _temp2818.base= _temp2817;
_temp2818.last_plus_one= _temp2817 + 22; _temp2818;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2819=( char*)"__no_check_memory_usage__";
struct _tagged_string _temp2820; _temp2820.curr= _temp2819; _temp2820.base=
_temp2819; _temp2820.last_plus_one= _temp2819 + 26; _temp2820;})) == 0){ a=(
void*) Cyc_Absyn_No_check_memory_usage_att;} else{ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2821=( char*)"unrecognized attribute"; struct
_tagged_string _temp2822; _temp2822.curr= _temp2821; _temp2822.base= _temp2821;
_temp2822.last_plus_one= _temp2821 + 23; _temp2822;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2823= yyls; struct Cyc_Yyltype* _temp2825= _temp2823.curr
+ yylsp_offset; if( _temp2823.base == 0? 1:( _temp2825 < _temp2823.base? 1:
_temp2825 >= _temp2823.last_plus_one)){ _throw( Null_Exception);}* _temp2825;}).first_line,({
struct _tagged_ptr3 _temp2826= yyls; struct Cyc_Yyltype* _temp2828= _temp2826.curr
+ yylsp_offset; if( _temp2826.base == 0? 1:( _temp2828 < _temp2826.base? 1:
_temp2828 >= _temp2826.last_plus_one)){ _throw( Null_Exception);}* _temp2828;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}}}}}}}}}}}}}}}} yyval=( struct _xtunion_struct*)({
struct Cyc_Attribute_tok_struct* _temp2829=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2829[ 0]=({ struct
Cyc_Attribute_tok_struct _temp2830; _temp2830.tag= Cyc_Attribute_tok_tag;
_temp2830.f1=( void*) a; _temp2830;}); _temp2829;}); break;} case 47: _LL2756:
yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct* _temp2835=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2835[ 0]=({ struct Cyc_Attribute_tok_struct _temp2836; _temp2836.tag= Cyc_Attribute_tok_tag;
_temp2836.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp2836;}); _temp2835;});
break; case 48: _LL2834: { struct _tagged_string s= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2894= yyvs; struct _xtunion_struct** _temp2896=
_temp2894.curr +( yyvsp_offset - 3); if( _temp2894.base == 0? 1:( _temp2896 <
_temp2894.base? 1: _temp2896 >= _temp2894.last_plus_one)){ _throw(
Null_Exception);}* _temp2896;})); struct _tuple12 _temp2843; int _temp2844; void*
_temp2846; struct _tuple12* _temp2841= Cyc_yyget_Int_tok(({ struct _tagged_ptr2
_temp2838= yyvs; struct _xtunion_struct** _temp2840= _temp2838.curr +(
yyvsp_offset - 1); if( _temp2838.base == 0? 1:( _temp2840 < _temp2838.base? 1:
_temp2840 >= _temp2838.last_plus_one)){ _throw( Null_Exception);}* _temp2840;}));
_temp2843=* _temp2841; _LL2847: _temp2846= _temp2843.f1; goto _LL2845; _LL2845:
_temp2844= _temp2843.f2; goto _LL2842; _LL2842: { void* a; if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2848=( char*)"regparm"; struct
_tagged_string _temp2849; _temp2849.curr= _temp2848; _temp2849.base= _temp2848;
_temp2849.last_plus_one= _temp2848 + 8; _temp2849;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2850=( char*)"__regparm__"; struct
_tagged_string _temp2851; _temp2851.curr= _temp2850; _temp2851.base= _temp2850;
_temp2851.last_plus_one= _temp2850 + 12; _temp2851;})) == 0){ if( _temp2844 <= 0?
1: _temp2844 > 3){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2852=(
char*)"regparm requires value between 1 and 3"; struct _tagged_string _temp2853;
_temp2853.curr= _temp2852; _temp2853.base= _temp2852; _temp2853.last_plus_one=
_temp2852 + 39; _temp2853;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2854= yyls; struct Cyc_Yyltype* _temp2856= _temp2854.curr +( yylsp_offset -
1); if( _temp2854.base == 0? 1:( _temp2856 < _temp2854.base? 1: _temp2856 >=
_temp2854.last_plus_one)){ _throw( Null_Exception);}* _temp2856;}).first_line,({
struct _tagged_ptr3 _temp2857= yyls; struct Cyc_Yyltype* _temp2859= _temp2857.curr
+( yylsp_offset - 1); if( _temp2857.base == 0? 1:( _temp2859 < _temp2857.base? 1:
_temp2859 >= _temp2857.last_plus_one)){ _throw( Null_Exception);}* _temp2859;}).last_line));}
a=( void*)({ struct Cyc_Absyn_Regparm_att_struct* _temp2860=( struct Cyc_Absyn_Regparm_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Regparm_att_struct)); _temp2860[ 0]=({
struct Cyc_Absyn_Regparm_att_struct _temp2861; _temp2861.tag= Cyc_Absyn_Regparm_att_tag;
_temp2861.f1= _temp2844; _temp2861;}); _temp2860;});} else{ if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2862=( char*)"aligned"; struct
_tagged_string _temp2863; _temp2863.curr= _temp2862; _temp2863.base= _temp2862;
_temp2863.last_plus_one= _temp2862 + 8; _temp2863;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2864=( char*)"__aligned__"; struct
_tagged_string _temp2865; _temp2865.curr= _temp2864; _temp2865.base= _temp2864;
_temp2865.last_plus_one= _temp2864 + 12; _temp2865;})) == 0){ if( _temp2844 < 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2866=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2867; _temp2867.curr= _temp2866; _temp2867.base=
_temp2866; _temp2867.last_plus_one= _temp2866 + 39; _temp2867;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2868= yyls; struct Cyc_Yyltype* _temp2870= _temp2868.curr
+( yylsp_offset - 1); if( _temp2868.base == 0? 1:( _temp2870 < _temp2868.base? 1:
_temp2870 >= _temp2868.last_plus_one)){ _throw( Null_Exception);}* _temp2870;}).first_line,({
struct _tagged_ptr3 _temp2871= yyls; struct Cyc_Yyltype* _temp2873= _temp2871.curr
+( yylsp_offset - 1); if( _temp2871.base == 0? 1:( _temp2873 < _temp2871.base? 1:
_temp2873 >= _temp2871.last_plus_one)){ _throw( Null_Exception);}* _temp2873;}).last_line));}{
unsigned int j=( unsigned int) _temp2844; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2874=(
char*)"aligned requires positive power of two"; struct _tagged_string _temp2875;
_temp2875.curr= _temp2874; _temp2875.base= _temp2874; _temp2875.last_plus_one=
_temp2874 + 39; _temp2875;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2876= yyls; struct Cyc_Yyltype* _temp2878= _temp2876.curr +( yylsp_offset -
1); if( _temp2876.base == 0? 1:( _temp2878 < _temp2876.base? 1: _temp2878 >=
_temp2876.last_plus_one)){ _throw( Null_Exception);}* _temp2878;}).first_line,({
struct _tagged_ptr3 _temp2879= yyls; struct Cyc_Yyltype* _temp2881= _temp2879.curr
+( yylsp_offset - 1); if( _temp2879.base == 0? 1:( _temp2881 < _temp2879.base? 1:
_temp2881 >= _temp2879.last_plus_one)){ _throw( Null_Exception);}* _temp2881;}).last_line));}
a=( void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp2882=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp2882[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp2883; _temp2883.tag= Cyc_Absyn_Aligned_att_tag;
_temp2883.f1= _temp2844; _temp2883;}); _temp2882;});}} else{ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2884=( char*)"unrecognized attribute";
struct _tagged_string _temp2885; _temp2885.curr= _temp2884; _temp2885.base=
_temp2884; _temp2885.last_plus_one= _temp2884 + 23; _temp2885;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2886= yyls; struct Cyc_Yyltype* _temp2888= _temp2886.curr
+( yylsp_offset - 3); if( _temp2886.base == 0? 1:( _temp2888 < _temp2886.base? 1:
_temp2888 >= _temp2886.last_plus_one)){ _throw( Null_Exception);}* _temp2888;}).first_line,({
struct _tagged_ptr3 _temp2889= yyls; struct Cyc_Yyltype* _temp2891= _temp2889.curr
+( yylsp_offset - 3); if( _temp2889.base == 0? 1:( _temp2891 < _temp2889.base? 1:
_temp2891 >= _temp2889.last_plus_one)){ _throw( Null_Exception);}* _temp2891;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}} yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct*
_temp2892=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2892[ 0]=({ struct Cyc_Attribute_tok_struct _temp2893; _temp2893.tag= Cyc_Attribute_tok_tag;
_temp2893.f1=( void*) a; _temp2893;}); _temp2892;}); break;}} case 49: _LL2837: {
struct _tagged_string s= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2917=
yyvs; struct _xtunion_struct** _temp2919= _temp2917.curr +( yyvsp_offset - 3);
if( _temp2917.base == 0? 1:( _temp2919 < _temp2917.base? 1: _temp2919 >=
_temp2917.last_plus_one)){ _throw( Null_Exception);}* _temp2919;})); struct
_tagged_string str= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2914= yyvs;
struct _xtunion_struct** _temp2916= _temp2914.curr +( yyvsp_offset - 1); if(
_temp2914.base == 0? 1:( _temp2916 < _temp2914.base? 1: _temp2916 >= _temp2914.last_plus_one)){
_throw( Null_Exception);}* _temp2916;})); void* a; if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2898=( char*)"section"; struct
_tagged_string _temp2899; _temp2899.curr= _temp2898; _temp2899.base= _temp2898;
_temp2899.last_plus_one= _temp2898 + 8; _temp2899;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2900=( char*)"__section__"; struct
_tagged_string _temp2901; _temp2901.curr= _temp2900; _temp2901.base= _temp2900;
_temp2901.last_plus_one= _temp2900 + 12; _temp2901;}))){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp2902=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp2902[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp2903; _temp2903.tag=
Cyc_Absyn_Section_att_tag; _temp2903.f1= str; _temp2903;}); _temp2902;});} else{
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2904=( char*)"unrecognized attribute";
struct _tagged_string _temp2905; _temp2905.curr= _temp2904; _temp2905.base=
_temp2904; _temp2905.last_plus_one= _temp2904 + 23; _temp2905;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2906= yyls; struct Cyc_Yyltype* _temp2908= _temp2906.curr
+( yylsp_offset - 3); if( _temp2906.base == 0? 1:( _temp2908 < _temp2906.base? 1:
_temp2908 >= _temp2906.last_plus_one)){ _throw( Null_Exception);}* _temp2908;}).first_line,({
struct _tagged_ptr3 _temp2909= yyls; struct Cyc_Yyltype* _temp2911= _temp2909.curr
+( yylsp_offset - 3); if( _temp2909.base == 0? 1:( _temp2911 < _temp2909.base? 1:
_temp2911 >= _temp2909.last_plus_one)){ _throw( Null_Exception);}* _temp2911;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct*
_temp2912=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2912[ 0]=({ struct Cyc_Attribute_tok_struct _temp2913; _temp2913.tag= Cyc_Attribute_tok_tag;
_temp2913.f1=( void*) a; _temp2913;}); _temp2912;}); break;} case 50: _LL2897:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2921=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2921[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2922; _temp2922.tag=
Cyc_TypeSpecifier_tok_tag; _temp2922.f1=( void*) Cyc_Parse_type_spec(( void*)
Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2923=
yyls; struct Cyc_Yyltype* _temp2925= _temp2923.curr + yylsp_offset; if(
_temp2923.base == 0? 1:( _temp2925 < _temp2923.base? 1: _temp2925 >= _temp2923.last_plus_one)){
_throw( Null_Exception);}* _temp2925;}).first_line,({ struct _tagged_ptr3
_temp2926= yyls; struct Cyc_Yyltype* _temp2928= _temp2926.curr + yylsp_offset;
if( _temp2926.base == 0? 1:( _temp2928 < _temp2926.base? 1: _temp2928 >=
_temp2926.last_plus_one)){ _throw( Null_Exception);}* _temp2928;}).last_line));
_temp2922;}); _temp2921;}); break; case 51: _LL2920: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2930=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2930[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2931; _temp2931.tag= Cyc_TypeSpecifier_tok_tag;
_temp2931.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2932= yyls; struct Cyc_Yyltype*
_temp2934= _temp2932.curr + yylsp_offset; if( _temp2932.base == 0? 1:( _temp2934
< _temp2932.base? 1: _temp2934 >= _temp2932.last_plus_one)){ _throw(
Null_Exception);}* _temp2934;}).first_line,({ struct _tagged_ptr3 _temp2935=
yyls; struct Cyc_Yyltype* _temp2937= _temp2935.curr + yylsp_offset; if(
_temp2935.base == 0? 1:( _temp2937 < _temp2935.base? 1: _temp2937 >= _temp2935.last_plus_one)){
_throw( Null_Exception);}* _temp2937;}).last_line)); _temp2931;}); _temp2930;});
break; case 52: _LL2929: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2939=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2939[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2940; _temp2940.tag=
Cyc_TypeSpecifier_tok_tag; _temp2940.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2941= yyls; struct Cyc_Yyltype*
_temp2943= _temp2941.curr + yylsp_offset; if( _temp2941.base == 0? 1:( _temp2943
< _temp2941.base? 1: _temp2943 >= _temp2941.last_plus_one)){ _throw(
Null_Exception);}* _temp2943;}).first_line,({ struct _tagged_ptr3 _temp2944=
yyls; struct Cyc_Yyltype* _temp2946= _temp2944.curr + yylsp_offset; if(
_temp2944.base == 0? 1:( _temp2946 < _temp2944.base? 1: _temp2946 >= _temp2944.last_plus_one)){
_throw( Null_Exception);}* _temp2946;}).last_line)); _temp2940;}); _temp2939;});
break; case 53: _LL2938: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2948=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2948[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2949; _temp2949.tag=
Cyc_TypeSpecifier_tok_tag; _temp2949.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp2950=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp2950[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp2951; _temp2951.tag=
Cyc_Parse_Short_spec_tag; _temp2951.f1= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2952= yyls; struct Cyc_Yyltype* _temp2954= _temp2952.curr +
yylsp_offset; if( _temp2952.base == 0? 1:( _temp2954 < _temp2952.base? 1:
_temp2954 >= _temp2952.last_plus_one)){ _throw( Null_Exception);}* _temp2954;}).first_line,({
struct _tagged_ptr3 _temp2955= yyls; struct Cyc_Yyltype* _temp2957= _temp2955.curr
+ yylsp_offset; if( _temp2955.base == 0? 1:( _temp2957 < _temp2955.base? 1:
_temp2957 >= _temp2955.last_plus_one)){ _throw( Null_Exception);}* _temp2957;}).last_line);
_temp2951;}); _temp2950;})); _temp2949;}); _temp2948;}); break; case 54: _LL2947:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2959=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2959[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2960; _temp2960.tag=
Cyc_TypeSpecifier_tok_tag; _temp2960.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2961= yyls; struct Cyc_Yyltype*
_temp2963= _temp2961.curr + yylsp_offset; if( _temp2961.base == 0? 1:( _temp2963
< _temp2961.base? 1: _temp2963 >= _temp2961.last_plus_one)){ _throw(
Null_Exception);}* _temp2963;}).first_line,({ struct _tagged_ptr3 _temp2964=
yyls; struct Cyc_Yyltype* _temp2966= _temp2964.curr + yylsp_offset; if(
_temp2964.base == 0? 1:( _temp2966 < _temp2964.base? 1: _temp2966 >= _temp2964.last_plus_one)){
_throw( Null_Exception);}* _temp2966;}).last_line)); _temp2960;}); _temp2959;});
break; case 55: _LL2958: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2968=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2968[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2969; _temp2969.tag=
Cyc_TypeSpecifier_tok_tag; _temp2969.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp2970=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp2970[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp2971; _temp2971.tag= Cyc_Parse_Long_spec_tag;
_temp2971.f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2972= yyls;
struct Cyc_Yyltype* _temp2974= _temp2972.curr + yylsp_offset; if( _temp2972.base
== 0? 1:( _temp2974 < _temp2972.base? 1: _temp2974 >= _temp2972.last_plus_one)){
_throw( Null_Exception);}* _temp2974;}).first_line,({ struct _tagged_ptr3
_temp2975= yyls; struct Cyc_Yyltype* _temp2977= _temp2975.curr + yylsp_offset;
if( _temp2975.base == 0? 1:( _temp2977 < _temp2975.base? 1: _temp2977 >=
_temp2975.last_plus_one)){ _throw( Null_Exception);}* _temp2977;}).last_line);
_temp2971;}); _temp2970;})); _temp2969;}); _temp2968;}); break; case 56: _LL2967:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2979=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2979[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2980; _temp2980.tag=
Cyc_TypeSpecifier_tok_tag; _temp2980.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2981= yyls; struct Cyc_Yyltype*
_temp2983= _temp2981.curr + yylsp_offset; if( _temp2981.base == 0? 1:( _temp2983
< _temp2981.base? 1: _temp2983 >= _temp2981.last_plus_one)){ _throw(
Null_Exception);}* _temp2983;}).first_line,({ struct _tagged_ptr3 _temp2984=
yyls; struct Cyc_Yyltype* _temp2986= _temp2984.curr + yylsp_offset; if(
_temp2984.base == 0? 1:( _temp2986 < _temp2984.base? 1: _temp2986 >= _temp2984.last_plus_one)){
_throw( Null_Exception);}* _temp2986;}).last_line)); _temp2980;}); _temp2979;});
break; case 57: _LL2978: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2988=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2988[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2989; _temp2989.tag=
Cyc_TypeSpecifier_tok_tag; _temp2989.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2990= yyls; struct Cyc_Yyltype*
_temp2992= _temp2990.curr + yylsp_offset; if( _temp2990.base == 0? 1:( _temp2992
< _temp2990.base? 1: _temp2992 >= _temp2990.last_plus_one)){ _throw(
Null_Exception);}* _temp2992;}).first_line,({ struct _tagged_ptr3 _temp2993=
yyls; struct Cyc_Yyltype* _temp2995= _temp2993.curr + yylsp_offset; if(
_temp2993.base == 0? 1:( _temp2995 < _temp2993.base? 1: _temp2995 >= _temp2993.last_plus_one)){
_throw( Null_Exception);}* _temp2995;}).last_line)); _temp2989;}); _temp2988;});
break; case 58: _LL2987: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2997=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2997[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2998; _temp2998.tag=
Cyc_TypeSpecifier_tok_tag; _temp2998.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp2999=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp2999[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp3000; _temp3000.tag=
Cyc_Parse_Signed_spec_tag; _temp3000.f1= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3001= yyls; struct Cyc_Yyltype* _temp3003= _temp3001.curr +
yylsp_offset; if( _temp3001.base == 0? 1:( _temp3003 < _temp3001.base? 1:
_temp3003 >= _temp3001.last_plus_one)){ _throw( Null_Exception);}* _temp3003;}).first_line,({
struct _tagged_ptr3 _temp3004= yyls; struct Cyc_Yyltype* _temp3006= _temp3004.curr
+ yylsp_offset; if( _temp3004.base == 0? 1:( _temp3006 < _temp3004.base? 1:
_temp3006 >= _temp3004.last_plus_one)){ _throw( Null_Exception);}* _temp3006;}).last_line);
_temp3000;}); _temp2999;})); _temp2998;}); _temp2997;}); break; case 59: _LL2996:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3008=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3008[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3009; _temp3009.tag=
Cyc_TypeSpecifier_tok_tag; _temp3009.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp3010=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp3010[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp3011; _temp3011.tag= Cyc_Parse_Unsigned_spec_tag; _temp3011.f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3012= yyls; struct Cyc_Yyltype* _temp3014= _temp3012.curr
+ yylsp_offset; if( _temp3012.base == 0? 1:( _temp3014 < _temp3012.base? 1:
_temp3014 >= _temp3012.last_plus_one)){ _throw( Null_Exception);}* _temp3014;}).first_line,({
struct _tagged_ptr3 _temp3015= yyls; struct Cyc_Yyltype* _temp3017= _temp3015.curr
+ yylsp_offset; if( _temp3015.base == 0? 1:( _temp3017 < _temp3015.base? 1:
_temp3017 >= _temp3015.last_plus_one)){ _throw( Null_Exception);}* _temp3017;}).last_line);
_temp3011;}); _temp3010;})); _temp3009;}); _temp3008;}); break; case 60: _LL3007:
yyval=({ struct _tagged_ptr2 _temp3019= yyvs; struct _xtunion_struct** _temp3021=
_temp3019.curr + yyvsp_offset; if( _temp3019.base == 0? 1:( _temp3021 <
_temp3019.base? 1: _temp3021 >= _temp3019.last_plus_one)){ _throw(
Null_Exception);}* _temp3021;}); break; case 61: _LL3018: yyval=({ struct
_tagged_ptr2 _temp3023= yyvs; struct _xtunion_struct** _temp3025= _temp3023.curr
+ yyvsp_offset; if( _temp3023.base == 0? 1:( _temp3025 < _temp3023.base? 1:
_temp3025 >= _temp3023.last_plus_one)){ _throw( Null_Exception);}* _temp3025;});
break; case 62: _LL3022: yyval=({ struct _tagged_ptr2 _temp3027= yyvs; struct
_xtunion_struct** _temp3029= _temp3027.curr + yyvsp_offset; if( _temp3027.base
== 0? 1:( _temp3029 < _temp3027.base? 1: _temp3029 >= _temp3027.last_plus_one)){
_throw( Null_Exception);}* _temp3029;}); break; case 63: _LL3026: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3031=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3031[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3032; _temp3032.tag= Cyc_TypeSpecifier_tok_tag;
_temp3032.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp3033= yyvs; struct _xtunion_struct** _temp3035=
_temp3033.curr + yyvsp_offset; if( _temp3033.base == 0? 1:( _temp3035 <
_temp3033.base? 1: _temp3035 >= _temp3033.last_plus_one)){ _throw(
Null_Exception);}* _temp3035;})),(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3036= yyls; struct Cyc_Yyltype*
_temp3038= _temp3036.curr + yylsp_offset; if( _temp3036.base == 0? 1:( _temp3038
< _temp3036.base? 1: _temp3038 >= _temp3036.last_plus_one)){ _throw(
Null_Exception);}* _temp3038;}).first_line,({ struct _tagged_ptr3 _temp3039=
yyls; struct Cyc_Yyltype* _temp3041= _temp3039.curr + yylsp_offset; if(
_temp3039.base == 0? 1:( _temp3041 < _temp3039.base? 1: _temp3041 >= _temp3039.last_plus_one)){
_throw( Null_Exception);}* _temp3041;}).last_line)); _temp3032;}); _temp3031;});
break; case 64: _LL3030: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3043=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3043[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3044; _temp3044.tag=
Cyc_TypeSpecifier_tok_tag; _temp3044.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3045= yyvs; struct
_xtunion_struct** _temp3047= _temp3045.curr +( yyvsp_offset - 2); if( _temp3045.base
== 0? 1:( _temp3047 < _temp3045.base? 1: _temp3047 >= _temp3045.last_plus_one)){
_throw( Null_Exception);}* _temp3047;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp3048= yyvs;
struct _xtunion_struct** _temp3050= _temp3048.curr + yyvsp_offset; if( _temp3048.base
== 0? 1:( _temp3050 < _temp3048.base? 1: _temp3050 >= _temp3048.last_plus_one)){
_throw( Null_Exception);}* _temp3050;})))), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3051= yyls; struct Cyc_Yyltype* _temp3053= _temp3051.curr
+( yylsp_offset - 2); if( _temp3051.base == 0? 1:( _temp3053 < _temp3051.base? 1:
_temp3053 >= _temp3051.last_plus_one)){ _throw( Null_Exception);}* _temp3053;}).first_line,({
struct _tagged_ptr3 _temp3054= yyls; struct Cyc_Yyltype* _temp3056= _temp3054.curr
+ yylsp_offset; if( _temp3054.base == 0? 1:( _temp3056 < _temp3054.base? 1:
_temp3056 >= _temp3054.last_plus_one)){ _throw( Null_Exception);}* _temp3056;}).last_line));
_temp3044;}); _temp3043;}); break; case 65: _LL3042: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3058=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3058[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3059; _temp3059.tag= Cyc_TypeSpecifier_tok_tag;
_temp3059.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp3060=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp3060[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp3061; _temp3061.tag=
Cyc_Absyn_TypedefType_tag; _temp3061.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3062= yyvs; struct _xtunion_struct** _temp3064= _temp3062.curr
+( yyvsp_offset - 1); if( _temp3062.base == 0? 1:( _temp3064 < _temp3062.base? 1:
_temp3064 >= _temp3062.last_plus_one)){ _throw( Null_Exception);}* _temp3064;}));
_temp3061.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3065= yyvs;
struct _xtunion_struct** _temp3067= _temp3065.curr + yyvsp_offset; if( _temp3065.base
== 0? 1:( _temp3067 < _temp3065.base? 1: _temp3067 >= _temp3065.last_plus_one)){
_throw( Null_Exception);}* _temp3067;})); _temp3061.f3= 0; _temp3061;});
_temp3060;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3068= yyls;
struct Cyc_Yyltype* _temp3070= _temp3068.curr +( yylsp_offset - 1); if(
_temp3068.base == 0? 1:( _temp3070 < _temp3068.base? 1: _temp3070 >= _temp3068.last_plus_one)){
_throw( Null_Exception);}* _temp3070;}).first_line,({ struct _tagged_ptr3
_temp3071= yyls; struct Cyc_Yyltype* _temp3073= _temp3071.curr + yylsp_offset;
if( _temp3071.base == 0? 1:( _temp3073 < _temp3071.base? 1: _temp3073 >=
_temp3071.last_plus_one)){ _throw( Null_Exception);}* _temp3073;}).last_line));
_temp3059;}); _temp3058;}); break; case 66: _LL3057: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3075=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3075[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3076; _temp3076.tag= Cyc_TypeSpecifier_tok_tag;
_temp3076.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp3077=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp3077[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp3078; _temp3078.tag= Cyc_Absyn_TupleType_tag;
_temp3078.f1=(( struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3079= yyls; struct Cyc_Yyltype* _temp3081= _temp3079.curr +(
yylsp_offset - 1); if( _temp3079.base == 0? 1:( _temp3081 < _temp3079.base? 1:
_temp3081 >= _temp3079.last_plus_one)){ _throw( Null_Exception);}* _temp3081;}).first_line,({
struct _tagged_ptr3 _temp3082= yyls; struct Cyc_Yyltype* _temp3084= _temp3082.curr
+( yylsp_offset - 1); if( _temp3082.base == 0? 1:( _temp3084 < _temp3082.base? 1:
_temp3084 >= _temp3082.last_plus_one)){ _throw( Null_Exception);}* _temp3084;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3085= yyvs; struct _xtunion_struct** _temp3087=
_temp3085.curr +( yyvsp_offset - 1); if( _temp3085.base == 0? 1:( _temp3087 <
_temp3085.base? 1: _temp3087 >= _temp3085.last_plus_one)){ _throw(
Null_Exception);}* _temp3087;})))); _temp3078;}); _temp3077;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3088= yyls; struct Cyc_Yyltype* _temp3090= _temp3088.curr
+( yylsp_offset - 3); if( _temp3088.base == 0? 1:( _temp3090 < _temp3088.base? 1:
_temp3090 >= _temp3088.last_plus_one)){ _throw( Null_Exception);}* _temp3090;}).first_line,({
struct _tagged_ptr3 _temp3091= yyls; struct Cyc_Yyltype* _temp3093= _temp3091.curr
+ yylsp_offset; if( _temp3091.base == 0? 1:( _temp3093 < _temp3091.base? 1:
_temp3093 >= _temp3091.last_plus_one)){ _throw( Null_Exception);}* _temp3093;}).last_line));
_temp3076;}); _temp3075;}); break; case 67: _LL3074: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3095=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3095[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3096; _temp3096.tag= Cyc_TypeSpecifier_tok_tag;
_temp3096.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3097=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3097[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3098; _temp3098.tag= Cyc_Absyn_RgnHandleType_tag; _temp3098.f1=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp3099= yyvs; struct _xtunion_struct** _temp3101=
_temp3099.curr +( yyvsp_offset - 1); if( _temp3099.base == 0? 1:( _temp3101 <
_temp3099.base? 1: _temp3101 >= _temp3099.last_plus_one)){ _throw(
Null_Exception);}* _temp3101;})); _temp3098;}); _temp3097;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3102= yyls; struct Cyc_Yyltype* _temp3104= _temp3102.curr
+( yylsp_offset - 3); if( _temp3102.base == 0? 1:( _temp3104 < _temp3102.base? 1:
_temp3104 >= _temp3102.last_plus_one)){ _throw( Null_Exception);}* _temp3104;}).first_line,({
struct _tagged_ptr3 _temp3105= yyls; struct Cyc_Yyltype* _temp3107= _temp3105.curr
+ yylsp_offset; if( _temp3105.base == 0? 1:( _temp3107 < _temp3105.base? 1:
_temp3107 >= _temp3105.last_plus_one)){ _throw( Null_Exception);}* _temp3107;}).last_line));
_temp3096;}); _temp3095;}); break; case 68: _LL3094: yyval=( struct
_xtunion_struct*)({ struct Cyc_Kind_tok_struct* _temp3109=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp3109[ 0]=({ struct Cyc_Kind_tok_struct
_temp3110; _temp3110.tag= Cyc_Kind_tok_tag; _temp3110.f1=( void*) Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3111= yyvs; struct
_xtunion_struct** _temp3113= _temp3111.curr + yyvsp_offset; if( _temp3111.base
== 0? 1:( _temp3113 < _temp3111.base? 1: _temp3113 >= _temp3111.last_plus_one)){
_throw( Null_Exception);}* _temp3113;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3114= yyls; struct Cyc_Yyltype* _temp3116= _temp3114.curr +
yylsp_offset; if( _temp3114.base == 0? 1:( _temp3116 < _temp3114.base? 1:
_temp3116 >= _temp3114.last_plus_one)){ _throw( Null_Exception);}* _temp3116;}).first_line,({
struct _tagged_ptr3 _temp3117= yyls; struct Cyc_Yyltype* _temp3119= _temp3117.curr
+ yylsp_offset; if( _temp3117.base == 0? 1:( _temp3119 < _temp3117.base? 1:
_temp3119 >= _temp3117.last_plus_one)){ _throw( Null_Exception);}* _temp3119;}).last_line));
_temp3110;}); _temp3109;}); break; case 69: _LL3108: { struct _tagged_string*
_temp3126; void* _temp3128; struct _tuple1 _temp3124=* Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3121= yyvs; struct _xtunion_struct** _temp3123=
_temp3121.curr + yyvsp_offset; if( _temp3121.base == 0? 1:( _temp3123 <
_temp3121.base? 1: _temp3123 >= _temp3121.last_plus_one)){ _throw(
Null_Exception);}* _temp3123;})); _LL3129: _temp3128= _temp3124.f1; goto _LL3127;
_LL3127: _temp3126= _temp3124.f2; goto _LL3125; _LL3125: if( _temp3128 !=( void*)
Cyc_Absyn_Loc_n){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp3130=(
char*)"bad kind in type specifier"; struct _tagged_string _temp3131; _temp3131.curr=
_temp3130; _temp3131.base= _temp3130; _temp3131.last_plus_one= _temp3130 + 27;
_temp3131;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3132= yyls;
struct Cyc_Yyltype* _temp3134= _temp3132.curr + yylsp_offset; if( _temp3132.base
== 0? 1:( _temp3134 < _temp3132.base? 1: _temp3134 >= _temp3132.last_plus_one)){
_throw( Null_Exception);}* _temp3134;}).first_line,({ struct _tagged_ptr3
_temp3135= yyls; struct Cyc_Yyltype* _temp3137= _temp3135.curr + yylsp_offset;
if( _temp3135.base == 0? 1:( _temp3137 < _temp3135.base? 1: _temp3137 >=
_temp3135.last_plus_one)){ _throw( Null_Exception);}* _temp3137;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_Kind_tok_struct* _temp3138=(
struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp3138[ 0]=({ struct Cyc_Kind_tok_struct _temp3139; _temp3139.tag= Cyc_Kind_tok_tag;
_temp3139.f1=( void*) Cyc_Parse_id_to_kind(* _temp3126, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3140= yyls; struct Cyc_Yyltype* _temp3142= _temp3140.curr
+ yylsp_offset; if( _temp3140.base == 0? 1:( _temp3142 < _temp3140.base? 1:
_temp3142 >= _temp3140.last_plus_one)){ _throw( Null_Exception);}* _temp3142;}).first_line,({
struct _tagged_ptr3 _temp3143= yyls; struct Cyc_Yyltype* _temp3145= _temp3143.curr
+ yylsp_offset; if( _temp3143.base == 0? 1:( _temp3145 < _temp3143.base? 1:
_temp3145 >= _temp3143.last_plus_one)){ _throw( Null_Exception);}* _temp3145;}).last_line));
_temp3139;}); _temp3138;}); break;} case 70: _LL3120: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeQual_tok_struct* _temp3147=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp3147[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3148; _temp3148.tag= Cyc_TypeQual_tok_tag; _temp3148.f1=({ struct Cyc_Absyn_Tqual*
_temp3149=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp3149->q_const= 1; _temp3149->q_volatile= 0; _temp3149->q_restrict= 0;
_temp3149;}); _temp3148;}); _temp3147;}); break; case 71: _LL3146: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeQual_tok_struct* _temp3151=( struct
Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp3151[ 0]=({ struct Cyc_TypeQual_tok_struct _temp3152; _temp3152.tag= Cyc_TypeQual_tok_tag;
_temp3152.f1=({ struct Cyc_Absyn_Tqual* _temp3153=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp3153->q_const= 0;
_temp3153->q_volatile= 1; _temp3153->q_restrict= 0; _temp3153;}); _temp3152;});
_temp3151;}); break; case 72: _LL3150: yyval=( struct _xtunion_struct*)({ struct
Cyc_TypeQual_tok_struct* _temp3155=( struct Cyc_TypeQual_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeQual_tok_struct)); _temp3155[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3156; _temp3156.tag= Cyc_TypeQual_tok_tag; _temp3156.f1=({ struct Cyc_Absyn_Tqual*
_temp3157=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp3157->q_const= 0; _temp3157->q_volatile= 0; _temp3157->q_restrict= 1;
_temp3157;}); _temp3156;}); _temp3155;}); break; case 73: _LL3154: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3159=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3159[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3160; _temp3160.tag=
Cyc_TypeSpecifier_tok_tag; _temp3160.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3161=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3161[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3162; _temp3162.tag= Cyc_Parse_Decl_spec_tag;
_temp3162.f1=({ struct Cyc_Absyn_Decl* _temp3163=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp3163->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp3170=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp3170[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp3171; _temp3171.tag= Cyc_Absyn_Enum_d_tag; _temp3171.f1=({ struct Cyc_Absyn_Enumdecl*
_temp3172=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp3172->sc=( void*)(( void*) Cyc_Absyn_Public); _temp3172->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3176= yyvs; struct _xtunion_struct** _temp3178=
_temp3176.curr +( yyvsp_offset - 3); if( _temp3176.base == 0? 1:( _temp3178 <
_temp3176.base? 1: _temp3178 >= _temp3176.last_plus_one)){ _throw(
Null_Exception);}* _temp3178;})); _temp3172->fields= Cyc_yyget_EnumfieldList_tok(({
struct _tagged_ptr2 _temp3173= yyvs; struct _xtunion_struct** _temp3175=
_temp3173.curr +( yyvsp_offset - 1); if( _temp3173.base == 0? 1:( _temp3175 <
_temp3173.base? 1: _temp3175 >= _temp3173.last_plus_one)){ _throw(
Null_Exception);}* _temp3175;})); _temp3172;}); _temp3171;}); _temp3170;}));
_temp3163->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3164=
yyls; struct Cyc_Yyltype* _temp3166= _temp3164.curr +( yylsp_offset - 4); if(
_temp3164.base == 0? 1:( _temp3166 < _temp3164.base? 1: _temp3166 >= _temp3164.last_plus_one)){
_throw( Null_Exception);}* _temp3166;}).first_line,({ struct _tagged_ptr3
_temp3167= yyls; struct Cyc_Yyltype* _temp3169= _temp3167.curr + yylsp_offset;
if( _temp3167.base == 0? 1:( _temp3169 < _temp3167.base? 1: _temp3169 >=
_temp3167.last_plus_one)){ _throw( Null_Exception);}* _temp3169;}).last_line);
_temp3163;}); _temp3162;}); _temp3161;})); _temp3160;}); _temp3159;}); break;
case 74: _LL3158: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3180=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3180[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3181; _temp3181.tag=
Cyc_TypeSpecifier_tok_tag; _temp3181.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp3182=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp3182[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp3183; _temp3183.tag= Cyc_Absyn_EnumType_tag;
_temp3183.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3184= yyvs;
struct _xtunion_struct** _temp3186= _temp3184.curr + yyvsp_offset; if( _temp3184.base
== 0? 1:( _temp3186 < _temp3184.base? 1: _temp3186 >= _temp3184.last_plus_one)){
_throw( Null_Exception);}* _temp3186;})); _temp3183.f2= 0; _temp3183;});
_temp3182;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3187= yyls;
struct Cyc_Yyltype* _temp3189= _temp3187.curr +( yylsp_offset - 1); if(
_temp3187.base == 0? 1:( _temp3189 < _temp3187.base? 1: _temp3189 >= _temp3187.last_plus_one)){
_throw( Null_Exception);}* _temp3189;}).first_line,({ struct _tagged_ptr3
_temp3190= yyls; struct Cyc_Yyltype* _temp3192= _temp3190.curr + yylsp_offset;
if( _temp3190.base == 0? 1:( _temp3192 < _temp3190.base? 1: _temp3192 >=
_temp3190.last_plus_one)){ _throw( Null_Exception);}* _temp3192;}).last_line));
_temp3181;}); _temp3180;}); break; case 75: _LL3179: yyval=( struct
_xtunion_struct*)({ struct Cyc_Enumfield_tok_struct* _temp3194=( struct Cyc_Enumfield_tok_struct*)
GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct)); _temp3194[ 0]=({ struct
Cyc_Enumfield_tok_struct _temp3195; _temp3195.tag= Cyc_Enumfield_tok_tag;
_temp3195.f1=({ struct Cyc_Absyn_Enumfield* _temp3196=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3196->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3203= yyvs; struct _xtunion_struct** _temp3205=
_temp3203.curr + yyvsp_offset; if( _temp3203.base == 0? 1:( _temp3205 <
_temp3203.base? 1: _temp3205 >= _temp3203.last_plus_one)){ _throw(
Null_Exception);}* _temp3205;})); _temp3196->tag= 0; _temp3196->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3197= yyls; struct Cyc_Yyltype* _temp3199= _temp3197.curr
+ yylsp_offset; if( _temp3197.base == 0? 1:( _temp3199 < _temp3197.base? 1:
_temp3199 >= _temp3197.last_plus_one)){ _throw( Null_Exception);}* _temp3199;}).first_line,({
struct _tagged_ptr3 _temp3200= yyls; struct Cyc_Yyltype* _temp3202= _temp3200.curr
+ yylsp_offset; if( _temp3200.base == 0? 1:( _temp3202 < _temp3200.base? 1:
_temp3202 >= _temp3200.last_plus_one)){ _throw( Null_Exception);}* _temp3202;}).last_line);
_temp3196;}); _temp3195;}); _temp3194;}); break; case 76: _LL3193: yyval=(
struct _xtunion_struct*)({ struct Cyc_Enumfield_tok_struct* _temp3207=( struct
Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp3207[ 0]=({ struct Cyc_Enumfield_tok_struct _temp3208; _temp3208.tag= Cyc_Enumfield_tok_tag;
_temp3208.f1=({ struct Cyc_Absyn_Enumfield* _temp3209=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3209->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3219= yyvs; struct _xtunion_struct** _temp3221=
_temp3219.curr +( yyvsp_offset - 2); if( _temp3219.base == 0? 1:( _temp3221 <
_temp3219.base? 1: _temp3221 >= _temp3219.last_plus_one)){ _throw(
Null_Exception);}* _temp3221;})); _temp3209->tag=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3216= yyvs; struct _xtunion_struct** _temp3218=
_temp3216.curr + yyvsp_offset; if( _temp3216.base == 0? 1:( _temp3218 <
_temp3216.base? 1: _temp3218 >= _temp3216.last_plus_one)){ _throw(
Null_Exception);}* _temp3218;})); _temp3209->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3210= yyls; struct Cyc_Yyltype* _temp3212= _temp3210.curr
+( yylsp_offset - 2); if( _temp3210.base == 0? 1:( _temp3212 < _temp3210.base? 1:
_temp3212 >= _temp3210.last_plus_one)){ _throw( Null_Exception);}* _temp3212;}).first_line,({
struct _tagged_ptr3 _temp3213= yyls; struct Cyc_Yyltype* _temp3215= _temp3213.curr
+ yylsp_offset; if( _temp3213.base == 0? 1:( _temp3215 < _temp3213.base? 1:
_temp3215 >= _temp3213.last_plus_one)){ _throw( Null_Exception);}* _temp3215;}).last_line);
_temp3209;}); _temp3208;}); _temp3207;}); break; case 77: _LL3206: yyval=(
struct _xtunion_struct*)({ struct Cyc_EnumfieldList_tok_struct* _temp3223=(
struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp3223[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp3224; _temp3224.tag=
Cyc_EnumfieldList_tok_tag; _temp3224.f1=({ struct Cyc_List_List* _temp3225=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3225->hd=(
void*) Cyc_yyget_Enumfield_tok(({ struct _tagged_ptr2 _temp3226= yyvs; struct
_xtunion_struct** _temp3228= _temp3226.curr + yyvsp_offset; if( _temp3226.base
== 0? 1:( _temp3228 < _temp3226.base? 1: _temp3228 >= _temp3226.last_plus_one)){
_throw( Null_Exception);}* _temp3228;})); _temp3225->tl= 0; _temp3225;});
_temp3224;}); _temp3223;}); break; case 78: _LL3222: yyval=( struct
_xtunion_struct*)({ struct Cyc_EnumfieldList_tok_struct* _temp3230=( struct Cyc_EnumfieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct)); _temp3230[ 0]=({
struct Cyc_EnumfieldList_tok_struct _temp3231; _temp3231.tag= Cyc_EnumfieldList_tok_tag;
_temp3231.f1=({ struct Cyc_List_List* _temp3232=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3232->hd=( void*) Cyc_yyget_Enumfield_tok(({
struct _tagged_ptr2 _temp3236= yyvs; struct _xtunion_struct** _temp3238=
_temp3236.curr +( yyvsp_offset - 2); if( _temp3236.base == 0? 1:( _temp3238 <
_temp3236.base? 1: _temp3238 >= _temp3236.last_plus_one)){ _throw(
Null_Exception);}* _temp3238;})); _temp3232->tl= Cyc_yyget_EnumfieldList_tok(({
struct _tagged_ptr2 _temp3233= yyvs; struct _xtunion_struct** _temp3235=
_temp3233.curr + yyvsp_offset; if( _temp3233.base == 0? 1:( _temp3235 <
_temp3233.base? 1: _temp3235 >= _temp3233.last_plus_one)){ _throw(
Null_Exception);}* _temp3235;})); _temp3232;}); _temp3231;}); _temp3230;});
break; case 79: _LL3229: { struct Cyc_Absyn_Decl* d;{ void* _temp3243= Cyc_yyget_StructOrUnion_tok(({
struct _tagged_ptr2 _temp3240= yyvs; struct _xtunion_struct** _temp3242=
_temp3240.curr +( yyvsp_offset - 3); if( _temp3240.base == 0? 1:( _temp3242 <
_temp3240.base? 1: _temp3242 >= _temp3240.last_plus_one)){ _throw(
Null_Exception);}* _temp3242;})); _LL3245: if(( int) _temp3243 == Cyc_Parse_Struct_su){
goto _LL3246;} else{ goto _LL3247;} _LL3247: if(( int) _temp3243 == Cyc_Parse_Union_su){
goto _LL3248;} else{ goto _LL3244;} _LL3246: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public, 0, 0,({ struct Cyc_Core_Opt* _temp3249=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3249->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3250= yyvs; struct _xtunion_struct** _temp3252=
_temp3250.curr +( yyvsp_offset - 1); if( _temp3250.base == 0? 1:( _temp3252 <
_temp3250.base? 1: _temp3252 >= _temp3250.last_plus_one)){ _throw(
Null_Exception);}* _temp3252;})); _temp3249;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3253= yyls; struct Cyc_Yyltype* _temp3255= _temp3253.curr
+( yylsp_offset - 3); if( _temp3253.base == 0? 1:( _temp3255 < _temp3253.base? 1:
_temp3255 >= _temp3253.last_plus_one)){ _throw( Null_Exception);}* _temp3255;}).first_line,({
struct _tagged_ptr3 _temp3256= yyls; struct Cyc_Yyltype* _temp3258= _temp3256.curr
+ yylsp_offset; if( _temp3256.base == 0? 1:( _temp3258 < _temp3256.base? 1:
_temp3258 >= _temp3256.last_plus_one)){ _throw( Null_Exception);}* _temp3258;}).last_line));
goto _LL3244; _LL3248: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public, 0, 0,({
struct Cyc_Core_Opt* _temp3259=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3259->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3260= yyvs; struct _xtunion_struct** _temp3262=
_temp3260.curr +( yyvsp_offset - 1); if( _temp3260.base == 0? 1:( _temp3262 <
_temp3260.base? 1: _temp3262 >= _temp3260.last_plus_one)){ _throw(
Null_Exception);}* _temp3262;})); _temp3259;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3263= yyls; struct Cyc_Yyltype* _temp3265= _temp3263.curr
+( yylsp_offset - 3); if( _temp3263.base == 0? 1:( _temp3265 < _temp3263.base? 1:
_temp3265 >= _temp3263.last_plus_one)){ _throw( Null_Exception);}* _temp3265;}).first_line,({
struct _tagged_ptr3 _temp3266= yyls; struct Cyc_Yyltype* _temp3268= _temp3266.curr
+ yylsp_offset; if( _temp3266.base == 0? 1:( _temp3268 < _temp3266.base? 1:
_temp3268 >= _temp3266.last_plus_one)){ _throw( Null_Exception);}* _temp3268;}).last_line));
goto _LL3244; _LL3244:;} yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3269=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3269[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3270; _temp3270.tag=
Cyc_TypeSpecifier_tok_tag; _temp3270.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3271=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3271[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3272; _temp3272.tag= Cyc_Parse_Decl_spec_tag;
_temp3272.f1= d; _temp3272;}); _temp3271;})); _temp3270;}); _temp3269;}); Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp3273=( char*)"anonymous structs/unions";
struct _tagged_string _temp3274; _temp3274.curr= _temp3273; _temp3274.base=
_temp3273; _temp3274.last_plus_one= _temp3273 + 25; _temp3274;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3275= yyls; struct Cyc_Yyltype* _temp3277= _temp3275.curr
+( yylsp_offset - 3); if( _temp3275.base == 0? 1:( _temp3277 < _temp3275.base? 1:
_temp3277 >= _temp3275.last_plus_one)){ _throw( Null_Exception);}* _temp3277;}).first_line,({
struct _tagged_ptr3 _temp3278= yyls; struct Cyc_Yyltype* _temp3280= _temp3278.curr
+ yylsp_offset; if( _temp3278.base == 0? 1:( _temp3280 < _temp3278.base? 1:
_temp3280 >= _temp3278.last_plus_one)){ _throw( Null_Exception);}* _temp3280;}).last_line));
break;} case 80: _LL3239: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3323= yyls; struct Cyc_Yyltype* _temp3325= _temp3323.curr
+( yylsp_offset - 3); if( _temp3323.base == 0? 1:( _temp3325 < _temp3323.base? 1:
_temp3325 >= _temp3323.last_plus_one)){ _throw( Null_Exception);}* _temp3325;}).first_line,({
struct _tagged_ptr3 _temp3326= yyls; struct Cyc_Yyltype* _temp3328= _temp3326.curr
+( yylsp_offset - 3); if( _temp3326.base == 0? 1:( _temp3328 < _temp3326.base? 1:
_temp3328 >= _temp3326.last_plus_one)){ _throw( Null_Exception);}* _temp3328;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3329= yyvs; struct
_xtunion_struct** _temp3331= _temp3329.curr +( yyvsp_offset - 3); if( _temp3329.base
== 0? 1:( _temp3331 < _temp3329.base? 1: _temp3331 >= _temp3329.last_plus_one)){
_throw( Null_Exception);}* _temp3331;}))); struct Cyc_Absyn_Decl* d;{ void*
_temp3285= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3282= yyvs;
struct _xtunion_struct** _temp3284= _temp3282.curr +( yyvsp_offset - 5); if(
_temp3282.base == 0? 1:( _temp3284 < _temp3282.base? 1: _temp3284 >= _temp3282.last_plus_one)){
_throw( Null_Exception);}* _temp3284;})); _LL3287: if(( int) _temp3285 == Cyc_Parse_Struct_su){
goto _LL3288;} else{ goto _LL3289;} _LL3289: if(( int) _temp3285 == Cyc_Parse_Union_su){
goto _LL3290;} else{ goto _LL3286;} _LL3288: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp3291=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3291->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3292= yyvs; struct _xtunion_struct** _temp3294=
_temp3292.curr +( yyvsp_offset - 4); if( _temp3292.base == 0? 1:( _temp3294 <
_temp3292.base? 1: _temp3294 >= _temp3292.last_plus_one)){ _throw(
Null_Exception);}* _temp3294;})); _temp3291;}), ts,({ struct Cyc_Core_Opt*
_temp3295=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3295->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp3296= yyvs; struct _xtunion_struct** _temp3298= _temp3296.curr +(
yyvsp_offset - 1); if( _temp3296.base == 0? 1:( _temp3298 < _temp3296.base? 1:
_temp3298 >= _temp3296.last_plus_one)){ _throw( Null_Exception);}* _temp3298;}));
_temp3295;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3299=
yyls; struct Cyc_Yyltype* _temp3301= _temp3299.curr +( yylsp_offset - 5); if(
_temp3299.base == 0? 1:( _temp3301 < _temp3299.base? 1: _temp3301 >= _temp3299.last_plus_one)){
_throw( Null_Exception);}* _temp3301;}).first_line,({ struct _tagged_ptr3
_temp3302= yyls; struct Cyc_Yyltype* _temp3304= _temp3302.curr + yylsp_offset;
if( _temp3302.base == 0? 1:( _temp3304 < _temp3302.base? 1: _temp3304 >=
_temp3302.last_plus_one)){ _throw( Null_Exception);}* _temp3304;}).last_line));
goto _LL3286; _LL3290: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp3305=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3305->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3306= yyvs; struct _xtunion_struct** _temp3308= _temp3306.curr
+( yyvsp_offset - 4); if( _temp3306.base == 0? 1:( _temp3308 < _temp3306.base? 1:
_temp3308 >= _temp3306.last_plus_one)){ _throw( Null_Exception);}* _temp3308;}));
_temp3305;}), ts,({ struct Cyc_Core_Opt* _temp3309=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3309->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3310= yyvs; struct _xtunion_struct** _temp3312=
_temp3310.curr +( yyvsp_offset - 1); if( _temp3310.base == 0? 1:( _temp3312 <
_temp3310.base? 1: _temp3312 >= _temp3310.last_plus_one)){ _throw(
Null_Exception);}* _temp3312;})); _temp3309;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3313= yyls; struct Cyc_Yyltype* _temp3315= _temp3313.curr
+( yylsp_offset - 5); if( _temp3313.base == 0? 1:( _temp3315 < _temp3313.base? 1:
_temp3315 >= _temp3313.last_plus_one)){ _throw( Null_Exception);}* _temp3315;}).first_line,({
struct _tagged_ptr3 _temp3316= yyls; struct Cyc_Yyltype* _temp3318= _temp3316.curr
+ yylsp_offset; if( _temp3316.base == 0? 1:( _temp3318 < _temp3316.base? 1:
_temp3318 >= _temp3316.last_plus_one)){ _throw( Null_Exception);}* _temp3318;}).last_line));
goto _LL3286; _LL3286:;} yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3319=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3319[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3320; _temp3320.tag=
Cyc_TypeSpecifier_tok_tag; _temp3320.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3321=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3321[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3322; _temp3322.tag= Cyc_Parse_Decl_spec_tag;
_temp3322.f1= d; _temp3322;}); _temp3321;})); _temp3320;}); _temp3319;}); break;}
case 81: _LL3281: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3374= yyls; struct Cyc_Yyltype* _temp3376= _temp3374.curr
+( yylsp_offset - 3); if( _temp3374.base == 0? 1:( _temp3376 < _temp3374.base? 1:
_temp3376 >= _temp3374.last_plus_one)){ _throw( Null_Exception);}* _temp3376;}).first_line,({
struct _tagged_ptr3 _temp3377= yyls; struct Cyc_Yyltype* _temp3379= _temp3377.curr
+( yylsp_offset - 3); if( _temp3377.base == 0? 1:( _temp3379 < _temp3377.base? 1:
_temp3379 >= _temp3377.last_plus_one)){ _throw( Null_Exception);}* _temp3379;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3380= yyvs; struct
_xtunion_struct** _temp3382= _temp3380.curr +( yyvsp_offset - 3); if( _temp3380.base
== 0? 1:( _temp3382 < _temp3380.base? 1: _temp3382 >= _temp3380.last_plus_one)){
_throw( Null_Exception);}* _temp3382;}))); struct Cyc_Absyn_Decl* d;{ void*
_temp3336= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3333= yyvs;
struct _xtunion_struct** _temp3335= _temp3333.curr +( yyvsp_offset - 5); if(
_temp3333.base == 0? 1:( _temp3335 < _temp3333.base? 1: _temp3335 >= _temp3333.last_plus_one)){
_throw( Null_Exception);}* _temp3335;})); _LL3338: if(( int) _temp3336 == Cyc_Parse_Struct_su){
goto _LL3339;} else{ goto _LL3340;} _LL3340: if(( int) _temp3336 == Cyc_Parse_Union_su){
goto _LL3341;} else{ goto _LL3337;} _LL3339: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp3342=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3342->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3343= yyvs; struct _xtunion_struct** _temp3345=
_temp3343.curr +( yyvsp_offset - 4); if( _temp3343.base == 0? 1:( _temp3345 <
_temp3343.base? 1: _temp3345 >= _temp3343.last_plus_one)){ _throw(
Null_Exception);}* _temp3345;})); _temp3342;}), ts,({ struct Cyc_Core_Opt*
_temp3346=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3346->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp3347= yyvs; struct _xtunion_struct** _temp3349= _temp3347.curr +(
yyvsp_offset - 1); if( _temp3347.base == 0? 1:( _temp3349 < _temp3347.base? 1:
_temp3349 >= _temp3347.last_plus_one)){ _throw( Null_Exception);}* _temp3349;}));
_temp3346;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3350=
yyls; struct Cyc_Yyltype* _temp3352= _temp3350.curr +( yylsp_offset - 5); if(
_temp3350.base == 0? 1:( _temp3352 < _temp3350.base? 1: _temp3352 >= _temp3350.last_plus_one)){
_throw( Null_Exception);}* _temp3352;}).first_line,({ struct _tagged_ptr3
_temp3353= yyls; struct Cyc_Yyltype* _temp3355= _temp3353.curr + yylsp_offset;
if( _temp3353.base == 0? 1:( _temp3355 < _temp3353.base? 1: _temp3355 >=
_temp3353.last_plus_one)){ _throw( Null_Exception);}* _temp3355;}).last_line));
goto _LL3337; _LL3341: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp3356=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3356->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3357= yyvs; struct _xtunion_struct** _temp3359= _temp3357.curr
+( yyvsp_offset - 4); if( _temp3357.base == 0? 1:( _temp3359 < _temp3357.base? 1:
_temp3359 >= _temp3357.last_plus_one)){ _throw( Null_Exception);}* _temp3359;}));
_temp3356;}), ts,({ struct Cyc_Core_Opt* _temp3360=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3360->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3361= yyvs; struct _xtunion_struct** _temp3363=
_temp3361.curr +( yyvsp_offset - 1); if( _temp3361.base == 0? 1:( _temp3363 <
_temp3361.base? 1: _temp3363 >= _temp3361.last_plus_one)){ _throw(
Null_Exception);}* _temp3363;})); _temp3360;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3364= yyls; struct Cyc_Yyltype* _temp3366= _temp3364.curr
+( yylsp_offset - 5); if( _temp3364.base == 0? 1:( _temp3366 < _temp3364.base? 1:
_temp3366 >= _temp3364.last_plus_one)){ _throw( Null_Exception);}* _temp3366;}).first_line,({
struct _tagged_ptr3 _temp3367= yyls; struct Cyc_Yyltype* _temp3369= _temp3367.curr
+ yylsp_offset; if( _temp3367.base == 0? 1:( _temp3369 < _temp3367.base? 1:
_temp3369 >= _temp3367.last_plus_one)){ _throw( Null_Exception);}* _temp3369;}).last_line));
goto _LL3337; _LL3337:;} yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3370=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3370[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3371; _temp3371.tag=
Cyc_TypeSpecifier_tok_tag; _temp3371.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3372=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3372[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3373; _temp3373.tag= Cyc_Parse_Decl_spec_tag;
_temp3373.f1= d; _temp3373;}); _temp3372;})); _temp3371;}); _temp3370;}); break;}
case 82: _LL3332:{ void* _temp3387= Cyc_yyget_StructOrUnion_tok(({ struct
_tagged_ptr2 _temp3384= yyvs; struct _xtunion_struct** _temp3386= _temp3384.curr
+( yyvsp_offset - 2); if( _temp3384.base == 0? 1:( _temp3386 < _temp3384.base? 1:
_temp3386 >= _temp3384.last_plus_one)){ _throw( Null_Exception);}* _temp3386;}));
_LL3389: if(( int) _temp3387 == Cyc_Parse_Struct_su){ goto _LL3390;} else{ goto
_LL3391;} _LL3391: if(( int) _temp3387 == Cyc_Parse_Union_su){ goto _LL3392;}
else{ goto _LL3388;} _LL3390: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3393=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3393[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3394; _temp3394.tag=
Cyc_TypeSpecifier_tok_tag; _temp3394.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp3395=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3395[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp3396; _temp3396.tag= Cyc_Absyn_StructType_tag;
_temp3396.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3397= yyvs; struct _xtunion_struct** _temp3399= _temp3397.curr +(
yyvsp_offset - 1); if( _temp3397.base == 0? 1:( _temp3399 < _temp3397.base? 1:
_temp3399 >= _temp3397.last_plus_one)){ _throw( Null_Exception);}* _temp3399;}));
_temp3396.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3400= yyvs;
struct _xtunion_struct** _temp3402= _temp3400.curr + yyvsp_offset; if( _temp3400.base
== 0? 1:( _temp3402 < _temp3400.base? 1: _temp3402 >= _temp3400.last_plus_one)){
_throw( Null_Exception);}* _temp3402;})); _temp3396.f3= 0; _temp3396;});
_temp3395;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3403= yyls;
struct Cyc_Yyltype* _temp3405= _temp3403.curr +( yylsp_offset - 2); if(
_temp3403.base == 0? 1:( _temp3405 < _temp3403.base? 1: _temp3405 >= _temp3403.last_plus_one)){
_throw( Null_Exception);}* _temp3405;}).first_line,({ struct _tagged_ptr3
_temp3406= yyls; struct Cyc_Yyltype* _temp3408= _temp3406.curr + yylsp_offset;
if( _temp3406.base == 0? 1:( _temp3408 < _temp3406.base? 1: _temp3408 >=
_temp3406.last_plus_one)){ _throw( Null_Exception);}* _temp3408;}).last_line));
_temp3394;}); _temp3393;}); goto _LL3388; _LL3392: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3409=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3409[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3410; _temp3410.tag= Cyc_TypeSpecifier_tok_tag;
_temp3410.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp3411=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp3411[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp3412; _temp3412.tag= Cyc_Absyn_UnionType_tag;
_temp3412.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3413= yyvs; struct _xtunion_struct** _temp3415= _temp3413.curr +(
yyvsp_offset - 1); if( _temp3413.base == 0? 1:( _temp3415 < _temp3413.base? 1:
_temp3415 >= _temp3413.last_plus_one)){ _throw( Null_Exception);}* _temp3415;}));
_temp3412.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3416= yyvs;
struct _xtunion_struct** _temp3418= _temp3416.curr + yyvsp_offset; if( _temp3416.base
== 0? 1:( _temp3418 < _temp3416.base? 1: _temp3418 >= _temp3416.last_plus_one)){
_throw( Null_Exception);}* _temp3418;})); _temp3412.f3= 0; _temp3412;});
_temp3411;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3419= yyls;
struct Cyc_Yyltype* _temp3421= _temp3419.curr +( yylsp_offset - 2); if(
_temp3419.base == 0? 1:( _temp3421 < _temp3419.base? 1: _temp3421 >= _temp3419.last_plus_one)){
_throw( Null_Exception);}* _temp3421;}).first_line,({ struct _tagged_ptr3
_temp3422= yyls; struct Cyc_Yyltype* _temp3424= _temp3422.curr + yylsp_offset;
if( _temp3422.base == 0? 1:( _temp3424 < _temp3422.base? 1: _temp3424 >=
_temp3422.last_plus_one)){ _throw( Null_Exception);}* _temp3424;}).last_line));
_temp3410;}); _temp3409;}); goto _LL3388; _LL3388:;} break; case 83: _LL3383:{
void* _temp3429= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3426=
yyvs; struct _xtunion_struct** _temp3428= _temp3426.curr +( yyvsp_offset - 2);
if( _temp3426.base == 0? 1:( _temp3428 < _temp3426.base? 1: _temp3428 >=
_temp3426.last_plus_one)){ _throw( Null_Exception);}* _temp3428;})); _LL3431:
if(( int) _temp3429 == Cyc_Parse_Struct_su){ goto _LL3432;} else{ goto _LL3433;}
_LL3433: if(( int) _temp3429 == Cyc_Parse_Union_su){ goto _LL3434;} else{ goto
_LL3430;} _LL3432: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3435=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3435[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3436; _temp3436.tag=
Cyc_TypeSpecifier_tok_tag; _temp3436.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp3437=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3437[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp3438; _temp3438.tag= Cyc_Absyn_StructType_tag;
_temp3438.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3439= yyvs; struct _xtunion_struct** _temp3441= _temp3439.curr +(
yyvsp_offset - 1); if( _temp3439.base == 0? 1:( _temp3441 < _temp3439.base? 1:
_temp3441 >= _temp3439.last_plus_one)){ _throw( Null_Exception);}* _temp3441;}));
_temp3438.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3442= yyvs;
struct _xtunion_struct** _temp3444= _temp3442.curr + yyvsp_offset; if( _temp3442.base
== 0? 1:( _temp3444 < _temp3442.base? 1: _temp3444 >= _temp3442.last_plus_one)){
_throw( Null_Exception);}* _temp3444;})); _temp3438.f3= 0; _temp3438;});
_temp3437;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3445= yyls;
struct Cyc_Yyltype* _temp3447= _temp3445.curr +( yylsp_offset - 2); if(
_temp3445.base == 0? 1:( _temp3447 < _temp3445.base? 1: _temp3447 >= _temp3445.last_plus_one)){
_throw( Null_Exception);}* _temp3447;}).first_line,({ struct _tagged_ptr3
_temp3448= yyls; struct Cyc_Yyltype* _temp3450= _temp3448.curr + yylsp_offset;
if( _temp3448.base == 0? 1:( _temp3450 < _temp3448.base? 1: _temp3450 >=
_temp3448.last_plus_one)){ _throw( Null_Exception);}* _temp3450;}).last_line));
_temp3436;}); _temp3435;}); goto _LL3430; _LL3434: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3451=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3451[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3452; _temp3452.tag= Cyc_TypeSpecifier_tok_tag;
_temp3452.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp3453=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp3453[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp3454; _temp3454.tag= Cyc_Absyn_UnionType_tag;
_temp3454.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3455= yyvs; struct _xtunion_struct** _temp3457= _temp3455.curr +(
yyvsp_offset - 1); if( _temp3455.base == 0? 1:( _temp3457 < _temp3455.base? 1:
_temp3457 >= _temp3455.last_plus_one)){ _throw( Null_Exception);}* _temp3457;}));
_temp3454.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3458= yyvs;
struct _xtunion_struct** _temp3460= _temp3458.curr + yyvsp_offset; if( _temp3458.base
== 0? 1:( _temp3460 < _temp3458.base? 1: _temp3460 >= _temp3458.last_plus_one)){
_throw( Null_Exception);}* _temp3460;})); _temp3454.f3= 0; _temp3454;});
_temp3453;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3461= yyls;
struct Cyc_Yyltype* _temp3463= _temp3461.curr +( yylsp_offset - 2); if(
_temp3461.base == 0? 1:( _temp3463 < _temp3461.base? 1: _temp3463 >= _temp3461.last_plus_one)){
_throw( Null_Exception);}* _temp3463;}).first_line,({ struct _tagged_ptr3
_temp3464= yyls; struct Cyc_Yyltype* _temp3466= _temp3464.curr + yylsp_offset;
if( _temp3464.base == 0? 1:( _temp3466 < _temp3464.base? 1: _temp3466 >=
_temp3464.last_plus_one)){ _throw( Null_Exception);}* _temp3466;}).last_line));
_temp3452;}); _temp3451;}); goto _LL3430; _LL3430:;} break; case 84: _LL3425:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp3468=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3468[ 0]=({ struct Cyc_TypeList_tok_struct _temp3469; _temp3469.tag= Cyc_TypeList_tok_tag;
_temp3469.f1= 0; _temp3469;}); _temp3468;}); break; case 85: _LL3467: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp3471=( struct
Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3471[ 0]=({ struct Cyc_TypeList_tok_struct _temp3472; _temp3472.tag= Cyc_TypeList_tok_tag;
_temp3472.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3473= yyvs; struct
_xtunion_struct** _temp3475= _temp3473.curr +( yyvsp_offset - 1); if( _temp3473.base
== 0? 1:( _temp3475 < _temp3473.base? 1: _temp3475 >= _temp3473.last_plus_one)){
_throw( Null_Exception);}* _temp3475;}))); _temp3472;}); _temp3471;}); break;
case 86: _LL3470:(( struct Cyc_Lexing_lexbuf*)({ struct Cyc_Core_Opt* _temp3477=
Cyc_Parse_lbuf; if( _temp3477 == 0){ _throw( Null_Exception);} _temp3477->v;}))->lex_curr_pos
-= 1; yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct*
_temp3478=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3478[ 0]=({ struct Cyc_TypeList_tok_struct _temp3479; _temp3479.tag= Cyc_TypeList_tok_tag;
_temp3479.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3480= yyvs; struct
_xtunion_struct** _temp3482= _temp3480.curr +( yyvsp_offset - 1); if( _temp3480.base
== 0? 1:( _temp3482 < _temp3480.base? 1: _temp3482 >= _temp3480.last_plus_one)){
_throw( Null_Exception);}* _temp3482;}))); _temp3479;}); _temp3478;}); break;
case 87: _LL3476: yyval=( struct _xtunion_struct*)({ struct Cyc_StructOrUnion_tok_struct*
_temp3484=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp3484[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp3485; _temp3485.tag=
Cyc_StructOrUnion_tok_tag; _temp3485.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp3485;}); _temp3484;}); break; case 88: _LL3483: yyval=( struct
_xtunion_struct*)({ struct Cyc_StructOrUnion_tok_struct* _temp3487=( struct Cyc_StructOrUnion_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct)); _temp3487[ 0]=({
struct Cyc_StructOrUnion_tok_struct _temp3488; _temp3488.tag= Cyc_StructOrUnion_tok_tag;
_temp3488.f1=( void*)(( void*) Cyc_Parse_Union_su); _temp3488;}); _temp3487;});
break; case 89: _LL3486: yyval=( struct _xtunion_struct*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3490=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp3490[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp3491; _temp3491.tag= Cyc_StructFieldDeclList_tok_tag; _temp3491.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3492= yyvs; struct _xtunion_struct** _temp3494=
_temp3492.curr + yyvsp_offset; if( _temp3492.base == 0? 1:( _temp3494 <
_temp3492.base? 1: _temp3494 >= _temp3492.last_plus_one)){ _throw(
Null_Exception);}* _temp3494;})))); _temp3491;}); _temp3490;}); break; case 90:
_LL3489: yyval=( struct _xtunion_struct*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3496=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp3496[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp3497; _temp3497.tag= Cyc_StructFieldDeclListList_tok_tag; _temp3497.f1=({
struct Cyc_List_List* _temp3498=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3498->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3499= yyvs; struct _xtunion_struct** _temp3501=
_temp3499.curr + yyvsp_offset; if( _temp3499.base == 0? 1:( _temp3501 <
_temp3499.base? 1: _temp3501 >= _temp3499.last_plus_one)){ _throw(
Null_Exception);}* _temp3501;})); _temp3498->tl= 0; _temp3498;}); _temp3497;});
_temp3496;}); break; case 91: _LL3495: yyval=( struct _xtunion_struct*)({ struct
Cyc_StructFieldDeclListList_tok_struct* _temp3503=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp3503[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp3504; _temp3504.tag= Cyc_StructFieldDeclListList_tok_tag;
_temp3504.f1=({ struct Cyc_List_List* _temp3505=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3505->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3509= yyvs; struct _xtunion_struct** _temp3511=
_temp3509.curr + yyvsp_offset; if( _temp3509.base == 0? 1:( _temp3511 <
_temp3509.base? 1: _temp3511 >= _temp3509.last_plus_one)){ _throw(
Null_Exception);}* _temp3511;})); _temp3505->tl= Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3506= yyvs; struct _xtunion_struct** _temp3508=
_temp3506.curr +( yyvsp_offset - 1); if( _temp3506.base == 0? 1:( _temp3508 <
_temp3506.base? 1: _temp3508 >= _temp3506.last_plus_one)){ _throw(
Null_Exception);}* _temp3508;})); _temp3505;}); _temp3504;}); _temp3503;});
break; case 92: _LL3502: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDeclList_tok_struct*
_temp3513=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3513[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp3514; _temp3514.tag=
Cyc_InitDeclList_tok_tag; _temp3514.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(({ struct _tagged_ptr2
_temp3515= yyvs; struct _xtunion_struct** _temp3517= _temp3515.curr +
yyvsp_offset; if( _temp3515.base == 0? 1:( _temp3517 < _temp3515.base? 1:
_temp3517 >= _temp3515.last_plus_one)){ _throw( Null_Exception);}* _temp3517;})));
_temp3514;}); _temp3513;}); break; case 93: _LL3512: yyval=( struct
_xtunion_struct*)({ struct Cyc_InitDeclList_tok_struct* _temp3519=( struct Cyc_InitDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp3519[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp3520; _temp3520.tag= Cyc_InitDeclList_tok_tag;
_temp3520.f1=({ struct Cyc_List_List* _temp3521=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3521->hd=( void*) Cyc_yyget_InitDecl_tok(({
struct _tagged_ptr2 _temp3522= yyvs; struct _xtunion_struct** _temp3524=
_temp3522.curr + yyvsp_offset; if( _temp3522.base == 0? 1:( _temp3524 <
_temp3522.base? 1: _temp3524 >= _temp3522.last_plus_one)){ _throw(
Null_Exception);}* _temp3524;})); _temp3521->tl= 0; _temp3521;}); _temp3520;});
_temp3519;}); break; case 94: _LL3518: yyval=( struct _xtunion_struct*)({ struct
Cyc_InitDeclList_tok_struct* _temp3526=( struct Cyc_InitDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp3526[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp3527; _temp3527.tag= Cyc_InitDeclList_tok_tag;
_temp3527.f1=({ struct Cyc_List_List* _temp3528=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3528->hd=( void*) Cyc_yyget_InitDecl_tok(({
struct _tagged_ptr2 _temp3532= yyvs; struct _xtunion_struct** _temp3534=
_temp3532.curr + yyvsp_offset; if( _temp3532.base == 0? 1:( _temp3534 <
_temp3532.base? 1: _temp3534 >= _temp3532.last_plus_one)){ _throw(
Null_Exception);}* _temp3534;})); _temp3528->tl= Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp3529= yyvs; struct _xtunion_struct** _temp3531=
_temp3529.curr +( yyvsp_offset - 2); if( _temp3529.base == 0? 1:( _temp3531 <
_temp3529.base? 1: _temp3531 >= _temp3529.last_plus_one)){ _throw(
Null_Exception);}* _temp3531;})); _temp3528;}); _temp3527;}); _temp3526;});
break; case 95: _LL3525: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDecl_tok_struct*
_temp3536=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp3536[ 0]=({ struct Cyc_InitDecl_tok_struct _temp3537; _temp3537.tag= Cyc_InitDecl_tok_tag;
_temp3537.f1=({ struct _tuple13* _temp3538=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp3538->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3539= yyvs; struct _xtunion_struct** _temp3541= _temp3539.curr
+ yyvsp_offset; if( _temp3539.base == 0? 1:( _temp3541 < _temp3539.base? 1:
_temp3541 >= _temp3539.last_plus_one)){ _throw( Null_Exception);}* _temp3541;}));
_temp3538->f2= 0; _temp3538;}); _temp3537;}); _temp3536;}); break; case 96:
_LL3535: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDecl_tok_struct*
_temp3543=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp3543[ 0]=({ struct Cyc_InitDecl_tok_struct _temp3544; _temp3544.tag= Cyc_InitDecl_tok_tag;
_temp3544.f1=({ struct _tuple13* _temp3545=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp3545->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3549= yyvs; struct _xtunion_struct** _temp3551= _temp3549.curr
+( yyvsp_offset - 2); if( _temp3549.base == 0? 1:( _temp3551 < _temp3549.base? 1:
_temp3551 >= _temp3549.last_plus_one)){ _throw( Null_Exception);}* _temp3551;}));
_temp3545->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp3546= yyvs; struct _xtunion_struct** _temp3548= _temp3546.curr +
yyvsp_offset; if( _temp3546.base == 0? 1:( _temp3548 < _temp3546.base? 1:
_temp3548 >= _temp3546.last_plus_one)){ _throw( Null_Exception);}* _temp3548;}));
_temp3545;}); _temp3544;}); _temp3543;}); break; case 97: _LL3542: { struct Cyc_Absyn_Tqual*
tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3582= yyvs; struct
_xtunion_struct** _temp3584= _temp3582.curr +( yyvsp_offset - 2); if( _temp3582.base
== 0? 1:( _temp3584 < _temp3582.base? 1: _temp3584 >= _temp3582.last_plus_one)){
_throw( Null_Exception);}* _temp3584;}))).f1; struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3579= yyvs; struct _xtunion_struct** _temp3581=
_temp3579.curr +( yyvsp_offset - 2); if( _temp3579.base == 0? 1:( _temp3581 <
_temp3579.base? 1: _temp3581 >= _temp3579.last_plus_one)){ _throw(
Null_Exception);}* _temp3581;}))).f3; void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3570= yyvs; struct _xtunion_struct** _temp3572=
_temp3570.curr +( yyvsp_offset - 2); if( _temp3570.base == 0? 1:( _temp3572 <
_temp3570.base? 1: _temp3572 >= _temp3570.last_plus_one)){ _throw(
Null_Exception);}* _temp3572;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3573= yyls; struct Cyc_Yyltype* _temp3575= _temp3573.curr +(
yylsp_offset - 2); if( _temp3573.base == 0? 1:( _temp3575 < _temp3573.base? 1:
_temp3575 >= _temp3573.last_plus_one)){ _throw( Null_Exception);}* _temp3575;}).first_line,({
struct _tagged_ptr3 _temp3576= yyls; struct Cyc_Yyltype* _temp3578= _temp3576.curr
+( yylsp_offset - 2); if( _temp3576.base == 0? 1:( _temp3578 < _temp3576.base? 1:
_temp3578 >= _temp3576.last_plus_one)){ _throw( Null_Exception);}* _temp3578;}).last_line));
struct Cyc_List_List* _temp3558; struct Cyc_List_List* _temp3560; struct _tuple0
_temp3556=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3553= yyvs; struct _xtunion_struct** _temp3555=
_temp3553.curr +( yyvsp_offset - 1); if( _temp3553.base == 0? 1:( _temp3555 <
_temp3553.base? 1: _temp3555 >= _temp3553.last_plus_one)){ _throw(
Null_Exception);}* _temp3555;}))); _LL3561: _temp3560= _temp3556.f1; goto
_LL3559; _LL3559: _temp3558= _temp3556.f2; goto _LL3557; _LL3557: { struct Cyc_List_List*
info=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( Cyc_Parse_apply_tmss( tq, t, _temp3560, atts), _temp3558);
yyval=( struct _xtunion_struct*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3562=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp3562[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp3563; _temp3563.tag= Cyc_StructFieldDeclList_tok_tag; _temp3563.f1=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple8*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3564= yyls; struct Cyc_Yyltype* _temp3566= _temp3564.curr
+( yylsp_offset - 2); if( _temp3564.base == 0? 1:( _temp3566 < _temp3564.base? 1:
_temp3566 >= _temp3564.last_plus_one)){ _throw( Null_Exception);}* _temp3566;}).first_line,({
struct _tagged_ptr3 _temp3567= yyls; struct Cyc_Yyltype* _temp3569= _temp3567.curr
+( yylsp_offset - 1); if( _temp3567.base == 0? 1:( _temp3569 < _temp3567.base? 1:
_temp3569 >= _temp3567.last_plus_one)){ _throw( Null_Exception);}* _temp3569;}).last_line),
info); _temp3563;}); _temp3562;}); break;}} case 98: _LL3552: yyval=( struct
_xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct* _temp3586=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp3586[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp3587; _temp3587.tag= Cyc_QualSpecList_tok_tag;
_temp3587.f1=({ struct _tuple14* _temp3588=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3588->f1= Cyc_Absyn_empty_tqual(); _temp3588->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp3592=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3592->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3593= yyvs; struct _xtunion_struct** _temp3595=
_temp3593.curr +( yyvsp_offset - 1); if( _temp3593.base == 0? 1:( _temp3595 <
_temp3593.base? 1: _temp3595 >= _temp3593.last_plus_one)){ _throw(
Null_Exception);}* _temp3595;})); _temp3592->tl= 0; _temp3592;}); _temp3588->f3=
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp3589= yyvs; struct
_xtunion_struct** _temp3591= _temp3589.curr + yyvsp_offset; if( _temp3589.base
== 0? 1:( _temp3591 < _temp3589.base? 1: _temp3591 >= _temp3589.last_plus_one)){
_throw( Null_Exception);}* _temp3591;})); _temp3588;}); _temp3587;}); _temp3586;});
break; case 99: _LL3585: yyval=( struct _xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct*
_temp3597=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3597[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3598; _temp3598.tag=
Cyc_QualSpecList_tok_tag; _temp3598.f1=({ struct _tuple14* _temp3599=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3599->f1=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3613= yyvs; struct _xtunion_struct** _temp3615=
_temp3613.curr + yyvsp_offset; if( _temp3613.base == 0? 1:( _temp3615 <
_temp3613.base? 1: _temp3615 >= _temp3613.last_plus_one)){ _throw(
Null_Exception);}* _temp3615;}))).f1; _temp3599->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp3606=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3606->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3610= yyvs; struct _xtunion_struct** _temp3612=
_temp3610.curr +( yyvsp_offset - 2); if( _temp3610.base == 0? 1:( _temp3612 <
_temp3610.base? 1: _temp3612 >= _temp3610.last_plus_one)){ _throw(
Null_Exception);}* _temp3612;})); _temp3606->tl=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3607= yyvs; struct _xtunion_struct** _temp3609=
_temp3607.curr + yyvsp_offset; if( _temp3607.base == 0? 1:( _temp3609 <
_temp3607.base? 1: _temp3609 >= _temp3607.last_plus_one)){ _throw(
Null_Exception);}* _temp3609;}))).f2; _temp3606;}); _temp3599->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3600= yyvs; struct _xtunion_struct** _temp3602=
_temp3600.curr +( yyvsp_offset - 1); if( _temp3600.base == 0? 1:( _temp3602 <
_temp3600.base? 1: _temp3602 >= _temp3600.last_plus_one)){ _throw(
Null_Exception);}* _temp3602;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3603= yyvs; struct _xtunion_struct** _temp3605= _temp3603.curr
+ yyvsp_offset; if( _temp3603.base == 0? 1:( _temp3605 < _temp3603.base? 1:
_temp3605 >= _temp3603.last_plus_one)){ _throw( Null_Exception);}* _temp3605;}))).f3);
_temp3599;}); _temp3598;}); _temp3597;}); break; case 100: _LL3596: yyval=(
struct _xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct* _temp3617=(
struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3617[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3618; _temp3618.tag=
Cyc_QualSpecList_tok_tag; _temp3618.f1=({ struct _tuple14* _temp3619=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3619->f1= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3623= yyvs; struct _xtunion_struct** _temp3625=
_temp3623.curr +( yyvsp_offset - 1); if( _temp3623.base == 0? 1:( _temp3625 <
_temp3623.base? 1: _temp3625 >= _temp3623.last_plus_one)){ _throw(
Null_Exception);}* _temp3625;})); _temp3619->f2= 0; _temp3619->f3= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3620= yyvs; struct _xtunion_struct** _temp3622=
_temp3620.curr + yyvsp_offset; if( _temp3620.base == 0? 1:( _temp3622 <
_temp3620.base? 1: _temp3622 >= _temp3620.last_plus_one)){ _throw(
Null_Exception);}* _temp3622;})); _temp3619;}); _temp3618;}); _temp3617;});
break; case 101: _LL3616: yyval=( struct _xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct*
_temp3627=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3627[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3628; _temp3628.tag=
Cyc_QualSpecList_tok_tag; _temp3628.f1=({ struct _tuple14* _temp3629=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3629->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp3639= yyvs; struct
_xtunion_struct** _temp3641= _temp3639.curr +( yyvsp_offset - 2); if( _temp3639.base
== 0? 1:( _temp3641 < _temp3639.base? 1: _temp3641 >= _temp3639.last_plus_one)){
_throw( Null_Exception);}* _temp3641;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3642= yyvs; struct _xtunion_struct** _temp3644= _temp3642.curr
+ yyvsp_offset; if( _temp3642.base == 0? 1:( _temp3644 < _temp3642.base? 1:
_temp3644 >= _temp3642.last_plus_one)){ _throw( Null_Exception);}* _temp3644;}))).f1);
_temp3629->f2=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3636=
yyvs; struct _xtunion_struct** _temp3638= _temp3636.curr + yyvsp_offset; if(
_temp3636.base == 0? 1:( _temp3638 < _temp3636.base? 1: _temp3638 >= _temp3636.last_plus_one)){
_throw( Null_Exception);}* _temp3638;}))).f2; _temp3629->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3630= yyvs; struct _xtunion_struct** _temp3632=
_temp3630.curr +( yyvsp_offset - 1); if( _temp3630.base == 0? 1:( _temp3632 <
_temp3630.base? 1: _temp3632 >= _temp3630.last_plus_one)){ _throw(
Null_Exception);}* _temp3632;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3633= yyvs; struct _xtunion_struct** _temp3635= _temp3633.curr
+ yyvsp_offset; if( _temp3633.base == 0? 1:( _temp3635 < _temp3633.base? 1:
_temp3635 >= _temp3633.last_plus_one)){ _throw( Null_Exception);}* _temp3635;}))).f3);
_temp3629;}); _temp3628;}); _temp3627;}); break; case 102: _LL3626: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp3646=(
struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct));
_temp3646[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct _temp3647; _temp3647.tag=
Cyc_DeclaratorExpoptList_tok_tag; _temp3647.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3648= yyvs; struct _xtunion_struct** _temp3650=
_temp3648.curr + yyvsp_offset; if( _temp3648.base == 0? 1:( _temp3650 <
_temp3648.base? 1: _temp3650 >= _temp3648.last_plus_one)){ _throw(
Null_Exception);}* _temp3650;}))); _temp3647;}); _temp3646;}); break; case 103:
_LL3645: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclaratorExpoptList_tok_struct*
_temp3652=( struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof(
struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3652[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct
_temp3653; _temp3653.tag= Cyc_DeclaratorExpoptList_tok_tag; _temp3653.f1=({
struct Cyc_List_List* _temp3654=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3654->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(({
struct _tagged_ptr2 _temp3655= yyvs; struct _xtunion_struct** _temp3657=
_temp3655.curr + yyvsp_offset; if( _temp3655.base == 0? 1:( _temp3657 <
_temp3655.base? 1: _temp3657 >= _temp3655.last_plus_one)){ _throw(
Null_Exception);}* _temp3657;})); _temp3654->tl= 0; _temp3654;}); _temp3653;});
_temp3652;}); break; case 104: _LL3651: yyval=( struct _xtunion_struct*)({
struct Cyc_DeclaratorExpoptList_tok_struct* _temp3659=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3659[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp3660; _temp3660.tag= Cyc_DeclaratorExpoptList_tok_tag;
_temp3660.f1=({ struct Cyc_List_List* _temp3661=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3661->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(({
struct _tagged_ptr2 _temp3665= yyvs; struct _xtunion_struct** _temp3667=
_temp3665.curr + yyvsp_offset; if( _temp3665.base == 0? 1:( _temp3667 <
_temp3665.base? 1: _temp3667 >= _temp3665.last_plus_one)){ _throw(
Null_Exception);}* _temp3667;})); _temp3661->tl= Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3662= yyvs; struct _xtunion_struct** _temp3664=
_temp3662.curr +( yyvsp_offset - 2); if( _temp3662.base == 0? 1:( _temp3664 <
_temp3662.base? 1: _temp3664 >= _temp3662.last_plus_one)){ _throw(
Null_Exception);}* _temp3664;})); _temp3661;}); _temp3660;}); _temp3659;});
break; case 105: _LL3658: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclaratorExpopt_tok_struct*
_temp3669=( struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct
Cyc_DeclaratorExpopt_tok_struct)); _temp3669[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct
_temp3670; _temp3670.tag= Cyc_DeclaratorExpopt_tok_tag; _temp3670.f1=({ struct
_tuple15* _temp3671=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15));
_temp3671->f1= Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3672= yyvs;
struct _xtunion_struct** _temp3674= _temp3672.curr + yyvsp_offset; if( _temp3672.base
== 0? 1:( _temp3674 < _temp3672.base? 1: _temp3674 >= _temp3672.last_plus_one)){
_throw( Null_Exception);}* _temp3674;})); _temp3671->f2= 0; _temp3671;});
_temp3670;}); _temp3669;}); break; case 106: _LL3668: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp3676=( struct
Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp3676[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp3677; _temp3677.tag=
Cyc_DeclaratorExpopt_tok_tag; _temp3677.f1=({ struct _tuple15* _temp3678=(
struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp3678->f1=({ struct
Cyc_Parse_Declarator* _temp3683=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3683->id=({ struct _tuple1*
_temp3684=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3684->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp3688=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp3688[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp3689; _temp3689.tag= Cyc_Absyn_Rel_n_tag; _temp3689.f1= 0; _temp3689;});
_temp3688;}); _temp3684->f2=({ struct _tagged_string* _temp3685=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3685[ 0]=(
struct _tagged_string)({ char* _temp3686=( char*)""; struct _tagged_string
_temp3687; _temp3687.curr= _temp3686; _temp3687.base= _temp3686; _temp3687.last_plus_one=
_temp3686 + 1; _temp3687;}); _temp3685;}); _temp3684;}); _temp3683->tms= 0;
_temp3683;}); _temp3678->f2=({ struct Cyc_Core_Opt* _temp3679=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3679->v=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3680= yyvs; struct _xtunion_struct** _temp3682=
_temp3680.curr + yyvsp_offset; if( _temp3680.base == 0? 1:( _temp3682 <
_temp3680.base? 1: _temp3682 >= _temp3680.last_plus_one)){ _throw(
Null_Exception);}* _temp3682;})); _temp3679;}); _temp3678;}); _temp3677;});
_temp3676;}); break; case 107: _LL3675: yyval=( struct _xtunion_struct*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp3691=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp3691[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp3692; _temp3692.tag= Cyc_DeclaratorExpopt_tok_tag;
_temp3692.f1=({ struct _tuple15* _temp3693=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp3693->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3698= yyvs; struct _xtunion_struct** _temp3700= _temp3698.curr
+( yyvsp_offset - 2); if( _temp3698.base == 0? 1:( _temp3700 < _temp3698.base? 1:
_temp3700 >= _temp3698.last_plus_one)){ _throw( Null_Exception);}* _temp3700;}));
_temp3693->f2=({ struct Cyc_Core_Opt* _temp3694=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3694->v=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3695= yyvs; struct _xtunion_struct** _temp3697=
_temp3695.curr + yyvsp_offset; if( _temp3695.base == 0? 1:( _temp3697 <
_temp3695.base? 1: _temp3697 >= _temp3695.last_plus_one)){ _throw(
Null_Exception);}* _temp3697;})); _temp3694;}); _temp3693;}); _temp3692;});
_temp3691;}); break; case 108: _LL3690: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
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
struct Cyc_TypeSpecifier_tok_struct* _temp3702=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3702[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3703; _temp3703.tag= Cyc_TypeSpecifier_tok_tag;
_temp3703.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp3704=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3704[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3705; _temp3705.tag= Cyc_Parse_Decl_spec_tag;
_temp3705.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp3706=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3706->v=( void*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3707=
yyvs; struct _xtunion_struct** _temp3709= _temp3707.curr +( yyvsp_offset - 4);
if( _temp3707.base == 0? 1:( _temp3709 < _temp3707.base? 1: _temp3709 >=
_temp3707.last_plus_one)){ _throw( Null_Exception);}* _temp3709;})); _temp3706;}),
ts,({ struct Cyc_Core_Opt* _temp3710=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp3710->v=( void*) Cyc_yyget_TunionFieldList_tok(({
struct _tagged_ptr2 _temp3711= yyvs; struct _xtunion_struct** _temp3713=
_temp3711.curr +( yyvsp_offset - 1); if( _temp3711.base == 0? 1:( _temp3713 <
_temp3711.base? 1: _temp3713 >= _temp3711.last_plus_one)){ _throw(
Null_Exception);}* _temp3713;})); _temp3710;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3714= yyls; struct Cyc_Yyltype* _temp3716= _temp3714.curr
+( yylsp_offset - 5); if( _temp3714.base == 0? 1:( _temp3716 < _temp3714.base? 1:
_temp3716 >= _temp3714.last_plus_one)){ _throw( Null_Exception);}* _temp3716;}).first_line,({
struct _tagged_ptr3 _temp3717= yyls; struct Cyc_Yyltype* _temp3719= _temp3717.curr
+ yylsp_offset; if( _temp3717.base == 0? 1:( _temp3719 < _temp3717.base? 1:
_temp3719 >= _temp3717.last_plus_one)){ _throw( Null_Exception);}* _temp3719;}).last_line));
_temp3705;}); _temp3704;})); _temp3703;}); _temp3702;}); break;} case 109:
_LL3701: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3730=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3730[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3731; _temp3731.tag=
Cyc_TypeSpecifier_tok_tag; _temp3731.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp3732=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp3732[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp3733; _temp3733.tag= Cyc_Absyn_TunionType_tag;
_temp3733.f1=({ struct Cyc_Absyn_TunionInfo _temp3734; _temp3734.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3738= yyvs; struct _xtunion_struct** _temp3740=
_temp3738.curr +( yyvsp_offset - 1); if( _temp3738.base == 0? 1:( _temp3740 <
_temp3738.base? 1: _temp3740 >= _temp3738.last_plus_one)){ _throw(
Null_Exception);}* _temp3740;})); _temp3734.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3735= yyvs; struct _xtunion_struct** _temp3737=
_temp3735.curr + yyvsp_offset; if( _temp3735.base == 0? 1:( _temp3737 <
_temp3735.base? 1: _temp3737 >= _temp3735.last_plus_one)){ _throw(
Null_Exception);}* _temp3737;})); _temp3734.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp3734.tud= 0; _temp3734;}); _temp3733;}); _temp3732;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3741= yyls; struct Cyc_Yyltype* _temp3743= _temp3741.curr
+( yylsp_offset - 2); if( _temp3741.base == 0? 1:( _temp3743 < _temp3741.base? 1:
_temp3743 >= _temp3741.last_plus_one)){ _throw( Null_Exception);}* _temp3743;}).first_line,({
struct _tagged_ptr3 _temp3744= yyls; struct Cyc_Yyltype* _temp3746= _temp3744.curr
+ yylsp_offset; if( _temp3744.base == 0? 1:( _temp3746 < _temp3744.base? 1:
_temp3746 >= _temp3744.last_plus_one)){ _throw( Null_Exception);}* _temp3746;}).last_line));
_temp3731;}); _temp3730;}); break; case 110: _LL3729: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3748=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3748[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3749; _temp3749.tag= Cyc_TypeSpecifier_tok_tag;
_temp3749.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp3750=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp3750[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp3751; _temp3751.tag=
Cyc_Absyn_TunionType_tag; _temp3751.f1=({ struct Cyc_Absyn_TunionInfo _temp3752;
_temp3752.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3759= yyvs;
struct _xtunion_struct** _temp3761= _temp3759.curr +( yyvsp_offset - 1); if(
_temp3759.base == 0? 1:( _temp3761 < _temp3759.base? 1: _temp3761 >= _temp3759.last_plus_one)){
_throw( Null_Exception);}* _temp3761;})); _temp3752.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3756= yyvs; struct _xtunion_struct** _temp3758=
_temp3756.curr + yyvsp_offset; if( _temp3756.base == 0? 1:( _temp3758 <
_temp3756.base? 1: _temp3758 >= _temp3756.last_plus_one)){ _throw(
Null_Exception);}* _temp3758;})); _temp3752.rgn=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3753= yyvs; struct _xtunion_struct** _temp3755=
_temp3753.curr +( yyvsp_offset - 2); if( _temp3753.base == 0? 1:( _temp3755 <
_temp3753.base? 1: _temp3755 >= _temp3753.last_plus_one)){ _throw(
Null_Exception);}* _temp3755;})); _temp3752.tud= 0; _temp3752;}); _temp3751;});
_temp3750;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3762= yyls;
struct Cyc_Yyltype* _temp3764= _temp3762.curr +( yylsp_offset - 3); if(
_temp3762.base == 0? 1:( _temp3764 < _temp3762.base? 1: _temp3764 >= _temp3762.last_plus_one)){
_throw( Null_Exception);}* _temp3764;}).first_line,({ struct _tagged_ptr3
_temp3765= yyls; struct Cyc_Yyltype* _temp3767= _temp3765.curr + yylsp_offset;
if( _temp3765.base == 0? 1:( _temp3767 < _temp3765.base? 1: _temp3767 >=
_temp3765.last_plus_one)){ _throw( Null_Exception);}* _temp3767;}).last_line));
_temp3749;}); _temp3748;}); break; case 111: _LL3747: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3769=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3769[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3770; _temp3770.tag= Cyc_TypeSpecifier_tok_tag;
_temp3770.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp3771=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp3771[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp3772; _temp3772.tag= Cyc_Absyn_TunionFieldType_tag; _temp3772.f1=({ struct
Cyc_Absyn_TunionFieldInfo _temp3773; _temp3773.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3780= yyvs; struct _xtunion_struct** _temp3782=
_temp3780.curr +( yyvsp_offset - 3); if( _temp3780.base == 0? 1:( _temp3782 <
_temp3780.base? 1: _temp3782 >= _temp3780.last_plus_one)){ _throw(
Null_Exception);}* _temp3782;})); _temp3773.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3777= yyvs; struct _xtunion_struct** _temp3779=
_temp3777.curr + yyvsp_offset; if( _temp3777.base == 0? 1:( _temp3779 <
_temp3777.base? 1: _temp3779 >= _temp3777.last_plus_one)){ _throw(
Null_Exception);}* _temp3779;})); _temp3773.fname= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3774= yyvs; struct _xtunion_struct** _temp3776=
_temp3774.curr +( yyvsp_offset - 1); if( _temp3774.base == 0? 1:( _temp3776 <
_temp3774.base? 1: _temp3776 >= _temp3774.last_plus_one)){ _throw(
Null_Exception);}* _temp3776;})); _temp3773.tud= 0; _temp3773.tufd= 0; _temp3773;});
_temp3772;}); _temp3771;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3783= yyls; struct Cyc_Yyltype* _temp3785= _temp3783.curr +( yylsp_offset -
4); if( _temp3783.base == 0? 1:( _temp3785 < _temp3783.base? 1: _temp3785 >=
_temp3783.last_plus_one)){ _throw( Null_Exception);}* _temp3785;}).first_line,({
struct _tagged_ptr3 _temp3786= yyls; struct Cyc_Yyltype* _temp3788= _temp3786.curr
+ yylsp_offset; if( _temp3786.base == 0? 1:( _temp3788 < _temp3786.base? 1:
_temp3788 >= _temp3786.last_plus_one)){ _throw( Null_Exception);}* _temp3788;}).last_line));
_temp3770;}); _temp3769;}); break; case 112: _LL3768: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3790=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3790[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3791; _temp3791.tag= Cyc_TypeSpecifier_tok_tag;
_temp3791.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp3792=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3792[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3793; _temp3793.tag= Cyc_Parse_Decl_spec_tag;
_temp3793.f1= Cyc_Absyn_xtunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3794= yyvs; struct _xtunion_struct** _temp3796=
_temp3794.curr +( yyvsp_offset - 3); if( _temp3794.base == 0? 1:( _temp3796 <
_temp3794.base? 1: _temp3796 >= _temp3794.last_plus_one)){ _throw(
Null_Exception);}* _temp3796;})), Cyc_yyget_TunionFieldList_tok(({ struct
_tagged_ptr2 _temp3797= yyvs; struct _xtunion_struct** _temp3799= _temp3797.curr
+( yyvsp_offset - 1); if( _temp3797.base == 0? 1:( _temp3799 < _temp3797.base? 1:
_temp3799 >= _temp3797.last_plus_one)){ _throw( Null_Exception);}* _temp3799;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3800= yyls; struct Cyc_Yyltype*
_temp3802= _temp3800.curr +( yylsp_offset - 4); if( _temp3800.base == 0? 1:(
_temp3802 < _temp3800.base? 1: _temp3802 >= _temp3800.last_plus_one)){ _throw(
Null_Exception);}* _temp3802;}).first_line,({ struct _tagged_ptr3 _temp3803=
yyls; struct Cyc_Yyltype* _temp3805= _temp3803.curr + yylsp_offset; if(
_temp3803.base == 0? 1:( _temp3805 < _temp3803.base? 1: _temp3805 >= _temp3803.last_plus_one)){
_throw( Null_Exception);}* _temp3805;}).last_line)); _temp3793;}); _temp3792;}));
_temp3791;}); _temp3790;}); break; case 113: _LL3789: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3807=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3807[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3808; _temp3808.tag= Cyc_TypeSpecifier_tok_tag;
_temp3808.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_XTunionType_struct*
_temp3809=( struct Cyc_Absyn_XTunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct));
_temp3809[ 0]=({ struct Cyc_Absyn_XTunionType_struct _temp3810; _temp3810.tag=
Cyc_Absyn_XTunionType_tag; _temp3810.f1=({ struct Cyc_Absyn_XTunionInfo
_temp3811; _temp3811.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3812=
yyvs; struct _xtunion_struct** _temp3814= _temp3812.curr + yyvsp_offset; if(
_temp3812.base == 0? 1:( _temp3814 < _temp3812.base? 1: _temp3814 >= _temp3812.last_plus_one)){
_throw( Null_Exception);}* _temp3814;})); _temp3811.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp3811.xtud= 0; _temp3811;}); _temp3810;}); _temp3809;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3815= yyls; struct Cyc_Yyltype* _temp3817= _temp3815.curr
+( yylsp_offset - 1); if( _temp3815.base == 0? 1:( _temp3817 < _temp3815.base? 1:
_temp3817 >= _temp3815.last_plus_one)){ _throw( Null_Exception);}* _temp3817;}).first_line,({
struct _tagged_ptr3 _temp3818= yyls; struct Cyc_Yyltype* _temp3820= _temp3818.curr
+ yylsp_offset; if( _temp3818.base == 0? 1:( _temp3820 < _temp3818.base? 1:
_temp3820 >= _temp3818.last_plus_one)){ _throw( Null_Exception);}* _temp3820;}).last_line));
_temp3808;}); _temp3807;}); break; case 114: _LL3806: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3822=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3822[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3823; _temp3823.tag= Cyc_TypeSpecifier_tok_tag;
_temp3823.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_XTunionType_struct*
_temp3824=( struct Cyc_Absyn_XTunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct));
_temp3824[ 0]=({ struct Cyc_Absyn_XTunionType_struct _temp3825; _temp3825.tag=
Cyc_Absyn_XTunionType_tag; _temp3825.f1=({ struct Cyc_Absyn_XTunionInfo
_temp3826; _temp3826.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3830=
yyvs; struct _xtunion_struct** _temp3832= _temp3830.curr + yyvsp_offset; if(
_temp3830.base == 0? 1:( _temp3832 < _temp3830.base? 1: _temp3832 >= _temp3830.last_plus_one)){
_throw( Null_Exception);}* _temp3832;})); _temp3826.rgn=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3827= yyvs; struct _xtunion_struct** _temp3829=
_temp3827.curr +( yyvsp_offset - 1); if( _temp3827.base == 0? 1:( _temp3829 <
_temp3827.base? 1: _temp3829 >= _temp3827.last_plus_one)){ _throw(
Null_Exception);}* _temp3829;})); _temp3826.xtud= 0; _temp3826;}); _temp3825;});
_temp3824;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3833= yyls;
struct Cyc_Yyltype* _temp3835= _temp3833.curr +( yylsp_offset - 2); if(
_temp3833.base == 0? 1:( _temp3835 < _temp3833.base? 1: _temp3835 >= _temp3833.last_plus_one)){
_throw( Null_Exception);}* _temp3835;}).first_line,({ struct _tagged_ptr3
_temp3836= yyls; struct Cyc_Yyltype* _temp3838= _temp3836.curr + yylsp_offset;
if( _temp3836.base == 0? 1:( _temp3838 < _temp3836.base? 1: _temp3838 >=
_temp3836.last_plus_one)){ _throw( Null_Exception);}* _temp3838;}).last_line));
_temp3823;}); _temp3822;}); break; case 115: _LL3821: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3840=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3840[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3841; _temp3841.tag= Cyc_TypeSpecifier_tok_tag;
_temp3841.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_XTunionFieldType_struct*
_temp3842=( struct Cyc_Absyn_XTunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_XTunionFieldType_struct)); _temp3842[ 0]=({ struct Cyc_Absyn_XTunionFieldType_struct
_temp3843; _temp3843.tag= Cyc_Absyn_XTunionFieldType_tag; _temp3843.f1=({ struct
Cyc_Absyn_XTunionFieldInfo _temp3844; _temp3844.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3848= yyvs; struct _xtunion_struct** _temp3850=
_temp3848.curr +( yyvsp_offset - 2); if( _temp3848.base == 0? 1:( _temp3850 <
_temp3848.base? 1: _temp3850 >= _temp3848.last_plus_one)){ _throw(
Null_Exception);}* _temp3850;})); _temp3844.fname= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3845= yyvs; struct _xtunion_struct** _temp3847=
_temp3845.curr + yyvsp_offset; if( _temp3845.base == 0? 1:( _temp3847 <
_temp3845.base? 1: _temp3847 >= _temp3845.last_plus_one)){ _throw(
Null_Exception);}* _temp3847;})); _temp3844.xtud= 0; _temp3844.xtufd= 0;
_temp3844;}); _temp3843;}); _temp3842;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3851= yyls; struct Cyc_Yyltype* _temp3853= _temp3851.curr +(
yylsp_offset - 3); if( _temp3851.base == 0? 1:( _temp3853 < _temp3851.base? 1:
_temp3853 >= _temp3851.last_plus_one)){ _throw( Null_Exception);}* _temp3853;}).first_line,({
struct _tagged_ptr3 _temp3854= yyls; struct Cyc_Yyltype* _temp3856= _temp3854.curr
+ yylsp_offset; if( _temp3854.base == 0? 1:( _temp3856 < _temp3854.base? 1:
_temp3856 >= _temp3854.last_plus_one)){ _throw( Null_Exception);}* _temp3856;}).last_line));
_temp3841;}); _temp3840;}); break; case 116: _LL3839: yyval=( struct
_xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct* _temp3858=( struct
Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3858[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3859; _temp3859.tag=
Cyc_TunionFieldList_tok_tag; _temp3859.f1=({ struct Cyc_List_List* _temp3860=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3860->hd=(
void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2 _temp3861= yyvs; struct
_xtunion_struct** _temp3863= _temp3861.curr + yyvsp_offset; if( _temp3861.base
== 0? 1:( _temp3863 < _temp3861.base? 1: _temp3863 >= _temp3861.last_plus_one)){
_throw( Null_Exception);}* _temp3863;})); _temp3860->tl= 0; _temp3860;});
_temp3859;}); _temp3858;}); break; case 117: _LL3857: yyval=( struct
_xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct* _temp3865=( struct
Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3865[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3866; _temp3866.tag=
Cyc_TunionFieldList_tok_tag; _temp3866.f1=({ struct Cyc_List_List* _temp3867=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3867->hd=(
void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2 _temp3868= yyvs; struct
_xtunion_struct** _temp3870= _temp3868.curr +( yyvsp_offset - 1); if( _temp3868.base
== 0? 1:( _temp3870 < _temp3868.base? 1: _temp3870 >= _temp3868.last_plus_one)){
_throw( Null_Exception);}* _temp3870;})); _temp3867->tl= 0; _temp3867;});
_temp3866;}); _temp3865;}); break; case 118: _LL3864: yyval=( struct
_xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct* _temp3872=( struct
Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3872[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3873; _temp3873.tag=
Cyc_TunionFieldList_tok_tag; _temp3873.f1=({ struct Cyc_List_List* _temp3874=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3874->hd=(
void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2 _temp3878= yyvs; struct
_xtunion_struct** _temp3880= _temp3878.curr +( yyvsp_offset - 2); if( _temp3878.base
== 0? 1:( _temp3880 < _temp3878.base? 1: _temp3880 >= _temp3878.last_plus_one)){
_throw( Null_Exception);}* _temp3880;})); _temp3874->tl= Cyc_yyget_TunionFieldList_tok(({
struct _tagged_ptr2 _temp3875= yyvs; struct _xtunion_struct** _temp3877=
_temp3875.curr + yyvsp_offset; if( _temp3875.base == 0? 1:( _temp3877 <
_temp3875.base? 1: _temp3877 >= _temp3875.last_plus_one)){ _throw(
Null_Exception);}* _temp3877;})); _temp3874;}); _temp3873;}); _temp3872;});
break; case 119: _LL3871: yyval=( struct _xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct*
_temp3882=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp3882[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp3883; _temp3883.tag= Cyc_TunionFieldList_tok_tag; _temp3883.f1=({ struct
Cyc_List_List* _temp3884=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3884->hd=( void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2
_temp3888= yyvs; struct _xtunion_struct** _temp3890= _temp3888.curr +(
yyvsp_offset - 2); if( _temp3888.base == 0? 1:( _temp3890 < _temp3888.base? 1:
_temp3890 >= _temp3888.last_plus_one)){ _throw( Null_Exception);}* _temp3890;}));
_temp3884->tl= Cyc_yyget_TunionFieldList_tok(({ struct _tagged_ptr2 _temp3885=
yyvs; struct _xtunion_struct** _temp3887= _temp3885.curr + yyvsp_offset; if(
_temp3885.base == 0? 1:( _temp3887 < _temp3885.base? 1: _temp3887 >= _temp3885.last_plus_one)){
_throw( Null_Exception);}* _temp3887;})); _temp3884;}); _temp3883;}); _temp3882;});
break; case 120: _LL3881: yyval=( struct _xtunion_struct*)({ struct Cyc_TunionField_tok_struct*
_temp3892=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp3892[ 0]=({ struct Cyc_TunionField_tok_struct _temp3893; _temp3893.tag= Cyc_TunionField_tok_tag;
_temp3893.f1=({ struct Cyc_Absyn_Tunionfield* _temp3894=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3894->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3901= yyvs; struct _xtunion_struct** _temp3903=
_temp3901.curr + yyvsp_offset; if( _temp3901.base == 0? 1:( _temp3903 <
_temp3901.base? 1: _temp3903 >= _temp3901.last_plus_one)){ _throw(
Null_Exception);}* _temp3903;})); _temp3894->tvs= 0; _temp3894->typs= 0;
_temp3894->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3895=
yyls; struct Cyc_Yyltype* _temp3897= _temp3895.curr + yylsp_offset; if(
_temp3895.base == 0? 1:( _temp3897 < _temp3895.base? 1: _temp3897 >= _temp3895.last_plus_one)){
_throw( Null_Exception);}* _temp3897;}).first_line,({ struct _tagged_ptr3
_temp3898= yyls; struct Cyc_Yyltype* _temp3900= _temp3898.curr + yylsp_offset;
if( _temp3898.base == 0? 1:( _temp3900 < _temp3898.base? 1: _temp3900 >=
_temp3898.last_plus_one)){ _throw( Null_Exception);}* _temp3900;}).last_line);
_temp3894;}); _temp3893;}); _temp3892;}); break; case 121: _LL3891: { struct Cyc_List_List*
typs=(( struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3926= yyls; struct Cyc_Yyltype* _temp3928= _temp3926.curr +(
yylsp_offset - 1); if( _temp3926.base == 0? 1:( _temp3928 < _temp3926.base? 1:
_temp3928 >= _temp3926.last_plus_one)){ _throw( Null_Exception);}* _temp3928;}).first_line,({
struct _tagged_ptr3 _temp3929= yyls; struct Cyc_Yyltype* _temp3931= _temp3929.curr
+( yylsp_offset - 1); if( _temp3929.base == 0? 1:( _temp3931 < _temp3929.base? 1:
_temp3931 >= _temp3929.last_plus_one)){ _throw( Null_Exception);}* _temp3931;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3932= yyvs; struct _xtunion_struct** _temp3934=
_temp3932.curr +( yyvsp_offset - 1); if( _temp3932.base == 0? 1:( _temp3934 <
_temp3932.base? 1: _temp3934 >= _temp3932.last_plus_one)){ _throw(
Null_Exception);}* _temp3934;})))); struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3917= yyls; struct Cyc_Yyltype* _temp3919= _temp3917.curr
+( yylsp_offset - 3); if( _temp3917.base == 0? 1:( _temp3919 < _temp3917.base? 1:
_temp3919 >= _temp3917.last_plus_one)){ _throw( Null_Exception);}* _temp3919;}).first_line,({
struct _tagged_ptr3 _temp3920= yyls; struct Cyc_Yyltype* _temp3922= _temp3920.curr
+( yylsp_offset - 3); if( _temp3920.base == 0? 1:( _temp3922 < _temp3920.base? 1:
_temp3922 >= _temp3920.last_plus_one)){ _throw( Null_Exception);}* _temp3922;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3923= yyvs; struct
_xtunion_struct** _temp3925= _temp3923.curr +( yyvsp_offset - 3); if( _temp3923.base
== 0? 1:( _temp3925 < _temp3923.base? 1: _temp3925 >= _temp3923.last_plus_one)){
_throw( Null_Exception);}* _temp3925;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_TunionField_tok_struct* _temp3905=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp3905[ 0]=({ struct
Cyc_TunionField_tok_struct _temp3906; _temp3906.tag= Cyc_TunionField_tok_tag;
_temp3906.f1=({ struct Cyc_Absyn_Tunionfield* _temp3907=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3907->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3914= yyvs; struct _xtunion_struct** _temp3916=
_temp3914.curr +( yyvsp_offset - 4); if( _temp3914.base == 0? 1:( _temp3916 <
_temp3914.base? 1: _temp3916 >= _temp3914.last_plus_one)){ _throw(
Null_Exception);}* _temp3916;})); _temp3907->tvs= tvs; _temp3907->typs= typs;
_temp3907->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3908=
yyls; struct Cyc_Yyltype* _temp3910= _temp3908.curr +( yylsp_offset - 4); if(
_temp3908.base == 0? 1:( _temp3910 < _temp3908.base? 1: _temp3910 >= _temp3908.last_plus_one)){
_throw( Null_Exception);}* _temp3910;}).first_line,({ struct _tagged_ptr3
_temp3911= yyls; struct Cyc_Yyltype* _temp3913= _temp3911.curr + yylsp_offset;
if( _temp3911.base == 0? 1:( _temp3913 < _temp3911.base? 1: _temp3913 >=
_temp3911.last_plus_one)){ _throw( Null_Exception);}* _temp3913;}).last_line);
_temp3907;}); _temp3906;}); _temp3905;}); break;} case 122: _LL3904: yyval=({
struct _tagged_ptr2 _temp3936= yyvs; struct _xtunion_struct** _temp3938=
_temp3936.curr + yyvsp_offset; if( _temp3936.base == 0? 1:( _temp3938 <
_temp3936.base? 1: _temp3938 >= _temp3936.last_plus_one)){ _throw(
Null_Exception);}* _temp3938;}); break; case 123: _LL3935: yyval=( struct
_xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp3940=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3940[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3941; _temp3941.tag= Cyc_Declarator_tok_tag;
_temp3941.f1=({ struct Cyc_Parse_Declarator* _temp3942=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3942->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3949= yyvs; struct _xtunion_struct** _temp3951=
_temp3949.curr + yyvsp_offset; if( _temp3949.base == 0? 1:( _temp3951 <
_temp3949.base? 1: _temp3951 >= _temp3949.last_plus_one)){ _throw(
Null_Exception);}* _temp3951;})))->id; _temp3942->tms=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp3943= yyvs; struct _xtunion_struct** _temp3945=
_temp3943.curr +( yyvsp_offset - 1); if( _temp3943.base == 0? 1:( _temp3945 <
_temp3943.base? 1: _temp3945 >= _temp3943.last_plus_one)){ _throw(
Null_Exception);}* _temp3945;})),( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3946= yyvs; struct _xtunion_struct** _temp3948= _temp3946.curr
+ yyvsp_offset; if( _temp3946.base == 0? 1:( _temp3948 < _temp3946.base? 1:
_temp3948 >= _temp3946.last_plus_one)){ _throw( Null_Exception);}* _temp3948;})))->tms);
_temp3942;}); _temp3941;}); _temp3940;}); break; case 124: _LL3939: yyval=(
struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp3953=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3953[ 0]=({ struct Cyc_Declarator_tok_struct _temp3954; _temp3954.tag= Cyc_Declarator_tok_tag;
_temp3954.f1=({ struct Cyc_Parse_Declarator* _temp3955=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3955->id= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3956= yyvs; struct _xtunion_struct** _temp3958=
_temp3956.curr + yyvsp_offset; if( _temp3956.base == 0? 1:( _temp3958 <
_temp3956.base? 1: _temp3958 >= _temp3956.last_plus_one)){ _throw(
Null_Exception);}* _temp3958;})); _temp3955->tms= 0; _temp3955;}); _temp3954;});
_temp3953;}); break; case 125: _LL3952: yyval=({ struct _tagged_ptr2 _temp3960=
yyvs; struct _xtunion_struct** _temp3962= _temp3960.curr +( yyvsp_offset - 1);
if( _temp3960.base == 0? 1:( _temp3962 < _temp3960.base? 1: _temp3962 >=
_temp3960.last_plus_one)){ _throw( Null_Exception);}* _temp3962;}); break; case
126: _LL3959: yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct*
_temp3964=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3964[ 0]=({ struct Cyc_Declarator_tok_struct _temp3965; _temp3965.tag= Cyc_Declarator_tok_tag;
_temp3965.f1=({ struct Cyc_Parse_Declarator* _temp3966=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3966->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3971= yyvs; struct _xtunion_struct** _temp3973=
_temp3971.curr +( yyvsp_offset - 2); if( _temp3971.base == 0? 1:( _temp3973 <
_temp3971.base? 1: _temp3973 >= _temp3971.last_plus_one)){ _throw(
Null_Exception);}* _temp3973;})))->id; _temp3966->tms=({ struct Cyc_List_List*
_temp3967=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3967->hd=( void*)(( void*) Cyc_Absyn_Carray_mod); _temp3967->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3968= yyvs; struct _xtunion_struct** _temp3970=
_temp3968.curr +( yyvsp_offset - 2); if( _temp3968.base == 0? 1:( _temp3970 <
_temp3968.base? 1: _temp3970 >= _temp3968.last_plus_one)){ _throw(
Null_Exception);}* _temp3970;})))->tms; _temp3967;}); _temp3966;}); _temp3965;});
_temp3964;}); break; case 127: _LL3963: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp3975=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3975[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3976; _temp3976.tag= Cyc_Declarator_tok_tag;
_temp3976.f1=({ struct Cyc_Parse_Declarator* _temp3977=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3977->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3987= yyvs; struct _xtunion_struct** _temp3989=
_temp3987.curr +( yyvsp_offset - 3); if( _temp3987.base == 0? 1:( _temp3989 <
_temp3987.base? 1: _temp3989 >= _temp3987.last_plus_one)){ _throw(
Null_Exception);}* _temp3989;})))->id; _temp3977->tms=({ struct Cyc_List_List*
_temp3978=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3978->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp3982=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp3982[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp3983; _temp3983.tag= Cyc_Absyn_ConstArray_mod_tag; _temp3983.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3984= yyvs; struct _xtunion_struct** _temp3986=
_temp3984.curr +( yyvsp_offset - 1); if( _temp3984.base == 0? 1:( _temp3986 <
_temp3984.base? 1: _temp3986 >= _temp3984.last_plus_one)){ _throw(
Null_Exception);}* _temp3986;})); _temp3983;}); _temp3982;})); _temp3978->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3979= yyvs; struct
_xtunion_struct** _temp3981= _temp3979.curr +( yyvsp_offset - 3); if( _temp3979.base
== 0? 1:( _temp3981 < _temp3979.base? 1: _temp3981 >= _temp3979.last_plus_one)){
_throw( Null_Exception);}* _temp3981;})))->tms; _temp3978;}); _temp3977;});
_temp3976;}); _temp3975;}); break; case 128: _LL3974: { struct _tuple16
_temp3996; struct Cyc_Core_Opt* _temp3997; int _temp3999; struct Cyc_List_List*
_temp4001; struct _tuple16* _temp3994= Cyc_yyget_ParamDeclListBool_tok(({ struct
_tagged_ptr2 _temp3991= yyvs; struct _xtunion_struct** _temp3993= _temp3991.curr
+( yyvsp_offset - 1); if( _temp3991.base == 0? 1:( _temp3993 < _temp3991.base? 1:
_temp3993 >= _temp3991.last_plus_one)){ _throw( Null_Exception);}* _temp3993;}));
_temp3996=* _temp3994; _LL4002: _temp4001= _temp3996.f1; goto _LL4000; _LL4000:
_temp3999= _temp3996.f2; goto _LL3998; _LL3998: _temp3997= _temp3996.f3; goto
_LL3995; _LL3995: yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct*
_temp4003=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp4003[ 0]=({ struct Cyc_Declarator_tok_struct _temp4004; _temp4004.tag= Cyc_Declarator_tok_tag;
_temp4004.f1=({ struct Cyc_Parse_Declarator* _temp4005=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4005->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4014= yyvs; struct _xtunion_struct** _temp4016=
_temp4014.curr +( yyvsp_offset - 3); if( _temp4014.base == 0? 1:( _temp4016 <
_temp4014.base? 1: _temp4016 >= _temp4014.last_plus_one)){ _throw(
Null_Exception);}* _temp4016;})))->id; _temp4005->tms=({ struct Cyc_List_List*
_temp4006=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4006->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4010=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4010[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4011; _temp4011.tag=
Cyc_Absyn_Function_mod_tag; _temp4011.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4012=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4012[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4013; _temp4013.tag= Cyc_Absyn_WithTypes_tag;
_temp4013.f1= _temp4001; _temp4013.f2= _temp3999; _temp4013.f3= _temp3997;
_temp4013;}); _temp4012;})); _temp4011;}); _temp4010;})); _temp4006->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4007= yyvs; struct _xtunion_struct** _temp4009=
_temp4007.curr +( yyvsp_offset - 3); if( _temp4007.base == 0? 1:( _temp4009 <
_temp4007.base? 1: _temp4009 >= _temp4007.last_plus_one)){ _throw(
Null_Exception);}* _temp4009;})))->tms; _temp4006;}); _temp4005;}); _temp4004;});
_temp4003;}); break;} case 129: _LL3990: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp4018=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4018[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4019; _temp4019.tag= Cyc_Declarator_tok_tag;
_temp4019.f1=({ struct Cyc_Parse_Declarator* _temp4020=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4020->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4029= yyvs; struct _xtunion_struct** _temp4031=
_temp4029.curr +( yyvsp_offset - 2); if( _temp4029.base == 0? 1:( _temp4031 <
_temp4029.base? 1: _temp4031 >= _temp4029.last_plus_one)){ _throw(
Null_Exception);}* _temp4031;})))->id; _temp4020->tms=({ struct Cyc_List_List*
_temp4021=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4021->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4025=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4025[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4026; _temp4026.tag=
Cyc_Absyn_Function_mod_tag; _temp4026.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4027=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4027[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4028; _temp4028.tag= Cyc_Absyn_WithTypes_tag;
_temp4028.f1= 0; _temp4028.f2= 0; _temp4028.f3= 0; _temp4028;}); _temp4027;}));
_temp4026;}); _temp4025;})); _temp4021->tl=( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp4022= yyvs; struct _xtunion_struct** _temp4024= _temp4022.curr
+( yyvsp_offset - 2); if( _temp4022.base == 0? 1:( _temp4024 < _temp4022.base? 1:
_temp4024 >= _temp4022.last_plus_one)){ _throw( Null_Exception);}* _temp4024;})))->tms;
_temp4021;}); _temp4020;}); _temp4019;}); _temp4018;}); break; case 130: _LL4017:
yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp4033=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp4033[ 0]=({ struct Cyc_Declarator_tok_struct _temp4034; _temp4034.tag= Cyc_Declarator_tok_tag;
_temp4034.f1=({ struct Cyc_Parse_Declarator* _temp4035=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4035->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4050= yyvs; struct _xtunion_struct** _temp4052=
_temp4050.curr +( yyvsp_offset - 4); if( _temp4050.base == 0? 1:( _temp4052 <
_temp4050.base? 1: _temp4052 >= _temp4050.last_plus_one)){ _throw(
Null_Exception);}* _temp4052;})))->id; _temp4035->tms=({ struct Cyc_List_List*
_temp4036=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4036->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4040=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4040[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4041; _temp4041.tag=
Cyc_Absyn_Function_mod_tag; _temp4041.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4042=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4042[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4043; _temp4043.tag= Cyc_Absyn_WithTypes_tag;
_temp4043.f1= 0; _temp4043.f2= 0; _temp4043.f3=({ struct Cyc_Core_Opt* _temp4044=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4044->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4045=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4045[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4046; _temp4046.tag= Cyc_Absyn_JoinEff_tag;
_temp4046.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4047= yyvs;
struct _xtunion_struct** _temp4049= _temp4047.curr +( yyvsp_offset - 1); if(
_temp4047.base == 0? 1:( _temp4049 < _temp4047.base? 1: _temp4049 >= _temp4047.last_plus_one)){
_throw( Null_Exception);}* _temp4049;})); _temp4046;}); _temp4045;})); _temp4044;});
_temp4043;}); _temp4042;})); _temp4041;}); _temp4040;})); _temp4036->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4037= yyvs; struct _xtunion_struct** _temp4039=
_temp4037.curr +( yyvsp_offset - 4); if( _temp4037.base == 0? 1:( _temp4039 <
_temp4037.base? 1: _temp4039 >= _temp4037.last_plus_one)){ _throw(
Null_Exception);}* _temp4039;})))->tms; _temp4036;}); _temp4035;}); _temp4034;});
_temp4033;}); break; case 131: _LL4032: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp4054=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4054[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4055; _temp4055.tag= Cyc_Declarator_tok_tag;
_temp4055.f1=({ struct Cyc_Parse_Declarator* _temp4056=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4056->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4074= yyvs; struct _xtunion_struct** _temp4076=
_temp4074.curr +( yyvsp_offset - 3); if( _temp4074.base == 0? 1:( _temp4076 <
_temp4074.base? 1: _temp4076 >= _temp4074.last_plus_one)){ _throw(
Null_Exception);}* _temp4076;})))->id; _temp4056->tms=({ struct Cyc_List_List*
_temp4057=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4057->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4061=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4061[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4062; _temp4062.tag=
Cyc_Absyn_Function_mod_tag; _temp4062.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct*
_temp4063=( struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp4063[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp4064; _temp4064.tag= Cyc_Absyn_NoTypes_tag;
_temp4064.f1= Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp4065= yyvs;
struct _xtunion_struct** _temp4067= _temp4065.curr +( yyvsp_offset - 1); if(
_temp4065.base == 0? 1:( _temp4067 < _temp4065.base? 1: _temp4067 >= _temp4065.last_plus_one)){
_throw( Null_Exception);}* _temp4067;})); _temp4064.f2= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4068= yyls; struct Cyc_Yyltype* _temp4070= _temp4068.curr
+( yylsp_offset - 3); if( _temp4068.base == 0? 1:( _temp4070 < _temp4068.base? 1:
_temp4070 >= _temp4068.last_plus_one)){ _throw( Null_Exception);}* _temp4070;}).first_line,({
struct _tagged_ptr3 _temp4071= yyls; struct Cyc_Yyltype* _temp4073= _temp4071.curr
+ yylsp_offset; if( _temp4071.base == 0? 1:( _temp4073 < _temp4071.base? 1:
_temp4073 >= _temp4071.last_plus_one)){ _throw( Null_Exception);}* _temp4073;}).last_line);
_temp4064;}); _temp4063;})); _temp4062;}); _temp4061;})); _temp4057->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4058= yyvs; struct _xtunion_struct** _temp4060=
_temp4058.curr +( yyvsp_offset - 3); if( _temp4058.base == 0? 1:( _temp4060 <
_temp4058.base? 1: _temp4060 >= _temp4058.last_plus_one)){ _throw(
Null_Exception);}* _temp4060;})))->tms; _temp4057;}); _temp4056;}); _temp4055;});
_temp4054;}); break; case 132: _LL4053: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
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
Cyc_Declarator_tok_struct* _temp4078=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4078[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4079; _temp4079.tag= Cyc_Declarator_tok_tag;
_temp4079.f1=({ struct Cyc_Parse_Declarator* _temp4080=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4080->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4093= yyvs; struct _xtunion_struct** _temp4095=
_temp4093.curr +( yyvsp_offset - 3); if( _temp4093.base == 0? 1:( _temp4095 <
_temp4093.base? 1: _temp4095 >= _temp4093.last_plus_one)){ _throw(
Null_Exception);}* _temp4095;})))->id; _temp4080->tms=({ struct Cyc_List_List*
_temp4081=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4081->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4085=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4085[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp4086; _temp4086.tag= Cyc_Absyn_TypeParams_mod_tag; _temp4086.f1= ts;
_temp4086.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4087= yyls;
struct Cyc_Yyltype* _temp4089= _temp4087.curr +( yylsp_offset - 3); if(
_temp4087.base == 0? 1:( _temp4089 < _temp4087.base? 1: _temp4089 >= _temp4087.last_plus_one)){
_throw( Null_Exception);}* _temp4089;}).first_line,({ struct _tagged_ptr3
_temp4090= yyls; struct Cyc_Yyltype* _temp4092= _temp4090.curr + yylsp_offset;
if( _temp4090.base == 0? 1:( _temp4092 < _temp4090.base? 1: _temp4092 >=
_temp4090.last_plus_one)){ _throw( Null_Exception);}* _temp4092;}).last_line);
_temp4086.f3= 0; _temp4086;}); _temp4085;})); _temp4081->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4082= yyvs; struct _xtunion_struct** _temp4084=
_temp4082.curr +( yyvsp_offset - 3); if( _temp4082.base == 0? 1:( _temp4084 <
_temp4082.base? 1: _temp4084 >= _temp4082.last_plus_one)){ _throw(
Null_Exception);}* _temp4084;})))->tms; _temp4081;}); _temp4080;}); _temp4079;});
_temp4078;}); break;} case 133: _LL4077:(( struct Cyc_Lexing_lexbuf*)({ struct
Cyc_Core_Opt* _temp4106= Cyc_Parse_lbuf; if( _temp4106 == 0){ _throw(
Null_Exception);} _temp4106->v;}))->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=((
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
Cyc_Declarator_tok_struct* _temp4107=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4107[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4108; _temp4108.tag= Cyc_Declarator_tok_tag;
_temp4108.f1=({ struct Cyc_Parse_Declarator* _temp4109=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4109->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4122= yyvs; struct _xtunion_struct** _temp4124=
_temp4122.curr +( yyvsp_offset - 3); if( _temp4122.base == 0? 1:( _temp4124 <
_temp4122.base? 1: _temp4124 >= _temp4122.last_plus_one)){ _throw(
Null_Exception);}* _temp4124;})))->id; _temp4109->tms=({ struct Cyc_List_List*
_temp4110=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4110->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4114=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4114[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp4115; _temp4115.tag= Cyc_Absyn_TypeParams_mod_tag; _temp4115.f1= ts;
_temp4115.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4116= yyls;
struct Cyc_Yyltype* _temp4118= _temp4116.curr +( yylsp_offset - 3); if(
_temp4116.base == 0? 1:( _temp4118 < _temp4116.base? 1: _temp4118 >= _temp4116.last_plus_one)){
_throw( Null_Exception);}* _temp4118;}).first_line,({ struct _tagged_ptr3
_temp4119= yyls; struct Cyc_Yyltype* _temp4121= _temp4119.curr + yylsp_offset;
if( _temp4119.base == 0? 1:( _temp4121 < _temp4119.base? 1: _temp4121 >=
_temp4119.last_plus_one)){ _throw( Null_Exception);}* _temp4121;}).last_line);
_temp4115.f3= 0; _temp4115;}); _temp4114;})); _temp4110->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4111= yyvs; struct _xtunion_struct** _temp4113=
_temp4111.curr +( yyvsp_offset - 3); if( _temp4111.base == 0? 1:( _temp4113 <
_temp4111.base? 1: _temp4113 >= _temp4111.last_plus_one)){ _throw(
Null_Exception);}* _temp4113;})))->tms; _temp4110;}); _temp4109;}); _temp4108;});
_temp4107;}); break;} case 134: _LL4105: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp4135=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4135[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4136; _temp4136.tag= Cyc_Declarator_tok_tag;
_temp4136.f1=({ struct Cyc_Parse_Declarator* _temp4137=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4137->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4153= yyvs; struct _xtunion_struct** _temp4155=
_temp4153.curr +( yyvsp_offset - 1); if( _temp4153.base == 0? 1:( _temp4155 <
_temp4153.base? 1: _temp4155 >= _temp4153.last_plus_one)){ _throw(
Null_Exception);}* _temp4155;})))->id; _temp4137->tms=({ struct Cyc_List_List*
_temp4138=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4138->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp4142=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp4142[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp4143; _temp4143.tag= Cyc_Absyn_Attributes_mod_tag; _temp4143.f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4144= yyls; struct Cyc_Yyltype* _temp4146= _temp4144.curr
+ yylsp_offset; if( _temp4144.base == 0? 1:( _temp4146 < _temp4144.base? 1:
_temp4146 >= _temp4144.last_plus_one)){ _throw( Null_Exception);}* _temp4146;}).first_line,({
struct _tagged_ptr3 _temp4147= yyls; struct Cyc_Yyltype* _temp4149= _temp4147.curr
+ yylsp_offset; if( _temp4147.base == 0? 1:( _temp4149 < _temp4147.base? 1:
_temp4149 >= _temp4147.last_plus_one)){ _throw( Null_Exception);}* _temp4149;}).last_line);
_temp4143.f2= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4150= yyvs;
struct _xtunion_struct** _temp4152= _temp4150.curr + yyvsp_offset; if( _temp4150.base
== 0? 1:( _temp4152 < _temp4150.base? 1: _temp4152 >= _temp4150.last_plus_one)){
_throw( Null_Exception);}* _temp4152;})); _temp4143;}); _temp4142;})); _temp4138->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp4139= yyvs; struct
_xtunion_struct** _temp4141= _temp4139.curr +( yyvsp_offset - 1); if( _temp4139.base
== 0? 1:( _temp4141 < _temp4139.base? 1: _temp4141 >= _temp4139.last_plus_one)){
_throw( Null_Exception);}* _temp4141;})))->tms; _temp4138;}); _temp4137;});
_temp4136;}); _temp4135;}); break; case 135: _LL4134: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeModifierList_tok_struct* _temp4157=( struct
Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp4157[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp4158; _temp4158.tag=
Cyc_TypeModifierList_tok_tag; _temp4158.f1=({ struct Cyc_List_List* _temp4159=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4159->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4169=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4169[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4170; _temp4170.tag= Cyc_Absyn_Pointer_mod_tag;
_temp4170.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4171= yyvs; struct _xtunion_struct** _temp4173= _temp4171.curr +(
yyvsp_offset - 2); if( _temp4171.base == 0? 1:( _temp4173 < _temp4171.base? 1:
_temp4173 >= _temp4171.last_plus_one)){ _throw( Null_Exception);}* _temp4173;}));
_temp4170.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4174= yyvs;
struct _xtunion_struct** _temp4176= _temp4174.curr +( yyvsp_offset - 1); if(
_temp4174.base == 0? 1:( _temp4176 < _temp4174.base? 1: _temp4176 >= _temp4174.last_plus_one)){
_throw( Null_Exception);}* _temp4176;})); _temp4170.f3= Cyc_Absyn_empty_tqual();
_temp4170;}); _temp4169;})); _temp4159->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4160= yyls; struct Cyc_Yyltype* _temp4162= _temp4160.curr
+ yylsp_offset; if( _temp4160.base == 0? 1:( _temp4162 < _temp4160.base? 1:
_temp4162 >= _temp4160.last_plus_one)){ _throw( Null_Exception);}* _temp4162;}).first_line,({
struct _tagged_ptr3 _temp4163= yyls; struct Cyc_Yyltype* _temp4165= _temp4163.curr
+ yylsp_offset; if( _temp4163.base == 0? 1:( _temp4165 < _temp4163.base? 1:
_temp4165 >= _temp4163.last_plus_one)){ _throw( Null_Exception);}* _temp4165;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4166= yyvs; struct
_xtunion_struct** _temp4168= _temp4166.curr + yyvsp_offset; if( _temp4166.base
== 0? 1:( _temp4168 < _temp4166.base? 1: _temp4168 >= _temp4166.last_plus_one)){
_throw( Null_Exception);}* _temp4168;})), 0); _temp4159;}); _temp4158;});
_temp4157;}); break; case 136: _LL4156: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeModifierList_tok_struct* _temp4178=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp4178[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp4179; _temp4179.tag= Cyc_TypeModifierList_tok_tag;
_temp4179.f1=({ struct Cyc_List_List* _temp4180=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4180->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp4190=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4190[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4191; _temp4191.tag= Cyc_Absyn_Pointer_mod_tag;
_temp4191.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4192= yyvs; struct _xtunion_struct** _temp4194= _temp4192.curr +(
yyvsp_offset - 3); if( _temp4192.base == 0? 1:( _temp4194 < _temp4192.base? 1:
_temp4194 >= _temp4192.last_plus_one)){ _throw( Null_Exception);}* _temp4194;}));
_temp4191.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4195= yyvs;
struct _xtunion_struct** _temp4197= _temp4195.curr +( yyvsp_offset - 2); if(
_temp4195.base == 0? 1:( _temp4197 < _temp4195.base? 1: _temp4197 >= _temp4195.last_plus_one)){
_throw( Null_Exception);}* _temp4197;})); _temp4191.f3= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp4198= yyvs; struct _xtunion_struct** _temp4200=
_temp4198.curr + yyvsp_offset; if( _temp4198.base == 0? 1:( _temp4200 <
_temp4198.base? 1: _temp4200 >= _temp4198.last_plus_one)){ _throw(
Null_Exception);}* _temp4200;})); _temp4191;}); _temp4190;})); _temp4180->tl=
Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4181= yyls; struct Cyc_Yyltype* _temp4183= _temp4181.curr +( yylsp_offset -
1); if( _temp4181.base == 0? 1:( _temp4183 < _temp4181.base? 1: _temp4183 >=
_temp4181.last_plus_one)){ _throw( Null_Exception);}* _temp4183;}).first_line,({
struct _tagged_ptr3 _temp4184= yyls; struct Cyc_Yyltype* _temp4186= _temp4184.curr
+( yylsp_offset - 1); if( _temp4184.base == 0? 1:( _temp4186 < _temp4184.base? 1:
_temp4186 >= _temp4184.last_plus_one)){ _throw( Null_Exception);}* _temp4186;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4187= yyvs; struct
_xtunion_struct** _temp4189= _temp4187.curr +( yyvsp_offset - 1); if( _temp4187.base
== 0? 1:( _temp4189 < _temp4187.base? 1: _temp4189 >= _temp4187.last_plus_one)){
_throw( Null_Exception);}* _temp4189;})), 0); _temp4180;}); _temp4179;});
_temp4178;}); break; case 137: _LL4177: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeModifierList_tok_struct* _temp4202=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp4202[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp4203; _temp4203.tag= Cyc_TypeModifierList_tok_tag;
_temp4203.f1=({ struct Cyc_List_List* _temp4204=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4204->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp4217=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4217[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4218; _temp4218.tag= Cyc_Absyn_Pointer_mod_tag;
_temp4218.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4219= yyvs; struct _xtunion_struct** _temp4221= _temp4219.curr +(
yyvsp_offset - 3); if( _temp4219.base == 0? 1:( _temp4221 < _temp4219.base? 1:
_temp4221 >= _temp4219.last_plus_one)){ _throw( Null_Exception);}* _temp4221;}));
_temp4218.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4222= yyvs;
struct _xtunion_struct** _temp4224= _temp4222.curr +( yyvsp_offset - 2); if(
_temp4222.base == 0? 1:( _temp4224 < _temp4222.base? 1: _temp4224 >= _temp4222.last_plus_one)){
_throw( Null_Exception);}* _temp4224;})); _temp4218.f3= Cyc_Absyn_empty_tqual();
_temp4218;}); _temp4217;})); _temp4204->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4205= yyls; struct Cyc_Yyltype* _temp4207= _temp4205.curr
+( yylsp_offset - 1); if( _temp4205.base == 0? 1:( _temp4207 < _temp4205.base? 1:
_temp4207 >= _temp4205.last_plus_one)){ _throw( Null_Exception);}* _temp4207;}).first_line,({
struct _tagged_ptr3 _temp4208= yyls; struct Cyc_Yyltype* _temp4210= _temp4208.curr
+( yylsp_offset - 1); if( _temp4208.base == 0? 1:( _temp4210 < _temp4208.base? 1:
_temp4210 >= _temp4208.last_plus_one)){ _throw( Null_Exception);}* _temp4210;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4211= yyvs; struct
_xtunion_struct** _temp4213= _temp4211.curr +( yyvsp_offset - 1); if( _temp4211.base
== 0? 1:( _temp4213 < _temp4211.base? 1: _temp4213 >= _temp4211.last_plus_one)){
_throw( Null_Exception);}* _temp4213;})), Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp4214= yyvs; struct _xtunion_struct** _temp4216=
_temp4214.curr + yyvsp_offset; if( _temp4214.base == 0? 1:( _temp4216 <
_temp4214.base? 1: _temp4216 >= _temp4214.last_plus_one)){ _throw(
Null_Exception);}* _temp4216;}))); _temp4204;}); _temp4203;}); _temp4202;});
break; case 138: _LL4201: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeModifierList_tok_struct*
_temp4226=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp4226[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp4227; _temp4227.tag= Cyc_TypeModifierList_tok_tag; _temp4227.f1=({ struct
Cyc_List_List* _temp4228=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4228->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4241=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp4241[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp4242; _temp4242.tag=
Cyc_Absyn_Pointer_mod_tag; _temp4242.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp4243= yyvs; struct _xtunion_struct** _temp4245=
_temp4243.curr +( yyvsp_offset - 4); if( _temp4243.base == 0? 1:( _temp4245 <
_temp4243.base? 1: _temp4245 >= _temp4243.last_plus_one)){ _throw(
Null_Exception);}* _temp4245;})); _temp4242.f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp4246= yyvs; struct _xtunion_struct** _temp4248=
_temp4246.curr +( yyvsp_offset - 3); if( _temp4246.base == 0? 1:( _temp4248 <
_temp4246.base? 1: _temp4248 >= _temp4246.last_plus_one)){ _throw(
Null_Exception);}* _temp4248;})); _temp4242.f3= Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp4249= yyvs; struct _xtunion_struct** _temp4251= _temp4249.curr
+( yyvsp_offset - 1); if( _temp4249.base == 0? 1:( _temp4251 < _temp4249.base? 1:
_temp4251 >= _temp4249.last_plus_one)){ _throw( Null_Exception);}* _temp4251;}));
_temp4242;}); _temp4241;})); _temp4228->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4229= yyls; struct Cyc_Yyltype* _temp4231= _temp4229.curr
+( yylsp_offset - 2); if( _temp4229.base == 0? 1:( _temp4231 < _temp4229.base? 1:
_temp4231 >= _temp4229.last_plus_one)){ _throw( Null_Exception);}* _temp4231;}).first_line,({
struct _tagged_ptr3 _temp4232= yyls; struct Cyc_Yyltype* _temp4234= _temp4232.curr
+( yylsp_offset - 2); if( _temp4232.base == 0? 1:( _temp4234 < _temp4232.base? 1:
_temp4234 >= _temp4232.last_plus_one)){ _throw( Null_Exception);}* _temp4234;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4235= yyvs; struct
_xtunion_struct** _temp4237= _temp4235.curr +( yyvsp_offset - 2); if( _temp4235.base
== 0? 1:( _temp4237 < _temp4235.base? 1: _temp4237 >= _temp4235.last_plus_one)){
_throw( Null_Exception);}* _temp4237;})), Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp4238= yyvs; struct _xtunion_struct** _temp4240=
_temp4238.curr + yyvsp_offset; if( _temp4238.base == 0? 1:( _temp4240 <
_temp4238.base? 1: _temp4240 >= _temp4238.last_plus_one)){ _throw(
Null_Exception);}* _temp4240;}))); _temp4228;}); _temp4227;}); _temp4226;});
break; case 139: _LL4225: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4253=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4253[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4254; _temp4254.tag=
Cyc_Pointer_Sort_tok_tag; _temp4254.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp4255=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp4255[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp4256; _temp4256.tag=
Cyc_Absyn_Nullable_ps_tag; _temp4256.f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4257= yyls; struct Cyc_Yyltype* _temp4259= _temp4257.curr
+ yylsp_offset; if( _temp4257.base == 0? 1:( _temp4259 < _temp4257.base? 1:
_temp4259 >= _temp4257.last_plus_one)){ _throw( Null_Exception);}* _temp4259;}).first_line,({
struct _tagged_ptr3 _temp4260= yyls; struct Cyc_Yyltype* _temp4262= _temp4260.curr
+ yylsp_offset; if( _temp4260.base == 0? 1:( _temp4262 < _temp4260.base? 1:
_temp4262 >= _temp4260.last_plus_one)){ _throw( Null_Exception);}* _temp4262;}).last_line));
_temp4256;}); _temp4255;})); _temp4254;}); _temp4253;}); break; case 140:
_LL4252: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4264=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4264[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4265; _temp4265.tag=
Cyc_Pointer_Sort_tok_tag; _temp4265.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp4266=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp4266[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp4267; _temp4267.tag= Cyc_Absyn_NonNullable_ps_tag; _temp4267.f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4268= yyls; struct
Cyc_Yyltype* _temp4270= _temp4268.curr + yylsp_offset; if( _temp4268.base == 0?
1:( _temp4270 < _temp4268.base? 1: _temp4270 >= _temp4268.last_plus_one)){
_throw( Null_Exception);}* _temp4270;}).first_line,({ struct _tagged_ptr3
_temp4271= yyls; struct Cyc_Yyltype* _temp4273= _temp4271.curr + yylsp_offset;
if( _temp4271.base == 0? 1:( _temp4273 < _temp4271.base? 1: _temp4273 >=
_temp4271.last_plus_one)){ _throw( Null_Exception);}* _temp4273;}).last_line));
_temp4267;}); _temp4266;})); _temp4265;}); _temp4264;}); break; case 141:
_LL4263: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4275=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4275[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4276; _temp4276.tag=
Cyc_Pointer_Sort_tok_tag; _temp4276.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp4277=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp4277[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp4278; _temp4278.tag=
Cyc_Absyn_Nullable_ps_tag; _temp4278.f1= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4279= yyvs; struct _xtunion_struct** _temp4281= _temp4279.curr
+( yyvsp_offset - 1); if( _temp4279.base == 0? 1:( _temp4281 < _temp4279.base? 1:
_temp4281 >= _temp4279.last_plus_one)){ _throw( Null_Exception);}* _temp4281;}));
_temp4278;}); _temp4277;})); _temp4276;}); _temp4275;}); break; case 142:
_LL4274: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4283=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4283[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4284; _temp4284.tag=
Cyc_Pointer_Sort_tok_tag; _temp4284.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp4285=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp4285[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp4286; _temp4286.tag= Cyc_Absyn_NonNullable_ps_tag; _temp4286.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4287= yyvs; struct _xtunion_struct** _temp4289=
_temp4287.curr +( yyvsp_offset - 1); if( _temp4287.base == 0? 1:( _temp4289 <
_temp4287.base? 1: _temp4289 >= _temp4287.last_plus_one)){ _throw(
Null_Exception);}* _temp4289;})); _temp4286;}); _temp4285;})); _temp4284;});
_temp4283;}); break; case 143: _LL4282: yyval=( struct _xtunion_struct*)({
struct Cyc_Pointer_Sort_tok_struct* _temp4291=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp4291[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp4292; _temp4292.tag= Cyc_Pointer_Sort_tok_tag;
_temp4292.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp4292;});
_temp4291;}); break; case 144: _LL4290: yyval=( struct _xtunion_struct*)({
struct Cyc_Rgn_tok_struct* _temp4294=( struct Cyc_Rgn_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgn_tok_struct)); _temp4294[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4295; _temp4295.tag= Cyc_Rgn_tok_tag; _temp4295.f1=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp4295;}); _temp4294;}); break; case 145: _LL4293: yyval=({ struct
_tagged_ptr2 _temp4297= yyvs; struct _xtunion_struct** _temp4299= _temp4297.curr
+ yyvsp_offset; if( _temp4297.base == 0? 1:( _temp4299 < _temp4297.base? 1:
_temp4299 >= _temp4297.last_plus_one)){ _throw( Null_Exception);}* _temp4299;});
break; case 146: _LL4296: yyval=( struct _xtunion_struct*)({ struct Cyc_Rgn_tok_struct*
_temp4301=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp4301[ 0]=({ struct Cyc_Rgn_tok_struct _temp4302; _temp4302.tag= Cyc_Rgn_tok_tag;
_temp4302.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4303= yyvs; struct _xtunion_struct** _temp4305= _temp4303.curr
+ yyvsp_offset; if( _temp4303.base == 0? 1:( _temp4305 < _temp4303.base? 1:
_temp4305 >= _temp4303.last_plus_one)){ _throw( Null_Exception);}* _temp4305;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4302;}); _temp4301;}); break; case 147: _LL4300: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp4307= yyvs; struct _xtunion_struct** _temp4309=
_temp4307.curr + yyvsp_offset; if( _temp4307.base == 0? 1:( _temp4309 <
_temp4307.base? 1: _temp4309 >= _temp4307.last_plus_one)){ _throw(
Null_Exception);}* _temp4309;})) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4310=( char*)"expecting region kind\n";
struct _tagged_string _temp4311; _temp4311.curr= _temp4310; _temp4311.base=
_temp4310; _temp4311.last_plus_one= _temp4310 + 23; _temp4311;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4312= yyls; struct Cyc_Yyltype* _temp4314= _temp4312.curr
+ yylsp_offset; if( _temp4312.base == 0? 1:( _temp4314 < _temp4312.base? 1:
_temp4314 >= _temp4312.last_plus_one)){ _throw( Null_Exception);}* _temp4314;}).first_line,({
struct _tagged_ptr3 _temp4315= yyls; struct Cyc_Yyltype* _temp4317= _temp4315.curr
+ yylsp_offset; if( _temp4315.base == 0? 1:( _temp4317 < _temp4315.base? 1:
_temp4317 >= _temp4315.last_plus_one)){ _throw( Null_Exception);}* _temp4317;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_Rgn_tok_struct* _temp4318=( struct
Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4318[
0]=({ struct Cyc_Rgn_tok_struct _temp4319; _temp4319.tag= Cyc_Rgn_tok_tag;
_temp4319.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4320= yyvs; struct _xtunion_struct** _temp4322= _temp4320.curr
+( yyvsp_offset - 2); if( _temp4320.base == 0? 1:( _temp4322 < _temp4320.base? 1:
_temp4322 >= _temp4320.last_plus_one)){ _throw( Null_Exception);}* _temp4322;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4319;}); _temp4318;}); break; case 148: _LL4306: yyval=( struct
_xtunion_struct*)({ struct Cyc_Rgn_tok_struct* _temp4324=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4324[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4325; _temp4325.tag= Cyc_Rgn_tok_tag; _temp4325.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp4325;}); _temp4324;}); break; case 149: _LL4323:
yyval=({ struct _tagged_ptr2 _temp4327= yyvs; struct _xtunion_struct** _temp4329=
_temp4327.curr + yyvsp_offset; if( _temp4327.base == 0? 1:( _temp4329 <
_temp4327.base? 1: _temp4329 >= _temp4327.last_plus_one)){ _throw(
Null_Exception);}* _temp4329;}); break; case 150: _LL4326: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeQual_tok_struct* _temp4331=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp4331[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp4332; _temp4332.tag= Cyc_TypeQual_tok_tag; _temp4332.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp4333= yyvs; struct
_xtunion_struct** _temp4335= _temp4333.curr +( yyvsp_offset - 1); if( _temp4333.base
== 0? 1:( _temp4335 < _temp4333.base? 1: _temp4335 >= _temp4333.last_plus_one)){
_throw( Null_Exception);}* _temp4335;})), Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp4336= yyvs; struct _xtunion_struct** _temp4338= _temp4336.curr
+ yyvsp_offset; if( _temp4336.base == 0? 1:( _temp4338 < _temp4336.base? 1:
_temp4338 >= _temp4336.last_plus_one)){ _throw( Null_Exception);}* _temp4338;})));
_temp4332;}); _temp4331;}); break; case 151: _LL4330: yyval=( struct
_xtunion_struct*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp4340=( struct
Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp4340[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp4341; _temp4341.tag=
Cyc_ParamDeclListBool_tok_tag; _temp4341.f1=({ struct _tuple16* _temp4342=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp4342->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4343= yyvs; struct _xtunion_struct** _temp4345=
_temp4343.curr + yyvsp_offset; if( _temp4343.base == 0? 1:( _temp4345 <
_temp4343.base? 1: _temp4345 >= _temp4343.last_plus_one)){ _throw(
Null_Exception);}* _temp4345;}))); _temp4342->f2= 0; _temp4342->f3= 0; _temp4342;});
_temp4341;}); _temp4340;}); break; case 152: _LL4339: yyval=( struct
_xtunion_struct*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp4347=( struct
Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp4347[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp4348; _temp4348.tag=
Cyc_ParamDeclListBool_tok_tag; _temp4348.f1=({ struct _tuple16* _temp4349=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp4349->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4350= yyvs; struct _xtunion_struct** _temp4352=
_temp4350.curr +( yyvsp_offset - 2); if( _temp4350.base == 0? 1:( _temp4352 <
_temp4350.base? 1: _temp4352 >= _temp4350.last_plus_one)){ _throw(
Null_Exception);}* _temp4352;}))); _temp4349->f2= 1; _temp4349->f3= 0; _temp4349;});
_temp4348;}); _temp4347;}); break; case 153: _LL4346: yyval=( struct
_xtunion_struct*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp4354=( struct
Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp4354[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp4355; _temp4355.tag=
Cyc_ParamDeclListBool_tok_tag; _temp4355.f1=({ struct _tuple16* _temp4356=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp4356->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4363= yyvs; struct _xtunion_struct** _temp4365=
_temp4363.curr +( yyvsp_offset - 2); if( _temp4363.base == 0? 1:( _temp4365 <
_temp4363.base? 1: _temp4365 >= _temp4363.last_plus_one)){ _throw(
Null_Exception);}* _temp4365;}))); _temp4356->f2= 0; _temp4356->f3=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp4357=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp4357->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp4358=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4358[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4359; _temp4359.tag= Cyc_Absyn_JoinEff_tag;
_temp4359.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4360= yyvs;
struct _xtunion_struct** _temp4362= _temp4360.curr + yyvsp_offset; if( _temp4360.base
== 0? 1:( _temp4362 < _temp4360.base? 1: _temp4362 >= _temp4360.last_plus_one)){
_throw( Null_Exception);}* _temp4362;})); _temp4359;}); _temp4358;})); _temp4357;});
_temp4356;}); _temp4355;}); _temp4354;}); break; case 154: _LL4353: yyval=({
struct _tagged_ptr2 _temp4367= yyvs; struct _xtunion_struct** _temp4369=
_temp4367.curr + yyvsp_offset; if( _temp4367.base == 0? 1:( _temp4369 <
_temp4367.base? 1: _temp4369 >= _temp4367.last_plus_one)){ _throw(
Null_Exception);}* _temp4369;}); break; case 155: _LL4366: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4371=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4371[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4372; _temp4372.tag= Cyc_TypeList_tok_tag; _temp4372.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4373= yyvs; struct _xtunion_struct** _temp4375=
_temp4373.curr +( yyvsp_offset - 2); if( _temp4373.base == 0? 1:( _temp4375 <
_temp4373.base? 1: _temp4375 >= _temp4373.last_plus_one)){ _throw(
Null_Exception);}* _temp4375;})), Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2
_temp4376= yyvs; struct _xtunion_struct** _temp4378= _temp4376.curr +
yyvsp_offset; if( _temp4376.base == 0? 1:( _temp4378 < _temp4376.base? 1:
_temp4378 >= _temp4376.last_plus_one)){ _throw( Null_Exception);}* _temp4378;})));
_temp4372;}); _temp4371;}); break; case 156: _LL4370: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4380=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4380[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4381; _temp4381.tag= Cyc_TypeList_tok_tag; _temp4381.f1= 0; _temp4381;});
_temp4380;}); break; case 157: _LL4379: yyval=({ struct _tagged_ptr2 _temp4383=
yyvs; struct _xtunion_struct** _temp4385= _temp4383.curr +( yyvsp_offset - 1);
if( _temp4383.base == 0? 1:( _temp4385 < _temp4383.base? 1: _temp4385 >=
_temp4383.last_plus_one)){ _throw( Null_Exception);}* _temp4385;}); break; case
158: _LL4382: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct*
_temp4387=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4387[ 0]=({ struct Cyc_TypeList_tok_struct _temp4388; _temp4388.tag= Cyc_TypeList_tok_tag;
_temp4388.f1=({ struct Cyc_List_List* _temp4389=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4389->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4390= yyvs; struct
_xtunion_struct** _temp4392= _temp4390.curr + yyvsp_offset; if( _temp4390.base
== 0? 1:( _temp4392 < _temp4390.base? 1: _temp4392 >= _temp4390.last_plus_one)){
_throw( Null_Exception);}* _temp4392;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp4389->tl= 0; _temp4389;});
_temp4388;}); _temp4387;}); break; case 159: _LL4386: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp4394= yyvs; struct _xtunion_struct** _temp4396=
_temp4394.curr + yyvsp_offset; if( _temp4394.base == 0? 1:( _temp4396 <
_temp4394.base? 1: _temp4396 >= _temp4394.last_plus_one)){ _throw(
Null_Exception);}* _temp4396;})) !=( void*) Cyc_Absyn_EffKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4397=( char*)"expecing effect kind (E)";
struct _tagged_string _temp4398; _temp4398.curr= _temp4397; _temp4398.base=
_temp4397; _temp4398.last_plus_one= _temp4397 + 25; _temp4398;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4399= yyls; struct Cyc_Yyltype* _temp4401= _temp4399.curr
+ yylsp_offset; if( _temp4399.base == 0? 1:( _temp4401 < _temp4399.base? 1:
_temp4401 >= _temp4399.last_plus_one)){ _throw( Null_Exception);}* _temp4401;}).first_line,({
struct _tagged_ptr3 _temp4402= yyls; struct Cyc_Yyltype* _temp4404= _temp4402.curr
+ yylsp_offset; if( _temp4402.base == 0? 1:( _temp4404 < _temp4402.base? 1:
_temp4404 >= _temp4402.last_plus_one)){ _throw( Null_Exception);}* _temp4404;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4405=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4405[ 0]=({ struct Cyc_TypeList_tok_struct _temp4406; _temp4406.tag= Cyc_TypeList_tok_tag;
_temp4406.f1=({ struct Cyc_List_List* _temp4407=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4407->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4408= yyvs; struct
_xtunion_struct** _temp4410= _temp4408.curr +( yyvsp_offset - 2); if( _temp4408.base
== 0? 1:( _temp4410 < _temp4408.base? 1: _temp4410 >= _temp4408.last_plus_one)){
_throw( Null_Exception);}* _temp4410;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp4407->tl= 0; _temp4407;});
_temp4406;}); _temp4405;}); break; case 160: _LL4393: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4412=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4412[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4413; _temp4413.tag= Cyc_TypeList_tok_tag; _temp4413.f1=({ struct Cyc_List_List*
_temp4414=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4414->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4415=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4415[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4416; _temp4416.tag= Cyc_Absyn_AccessEff_tag;
_temp4416.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4417= yyvs; struct _xtunion_struct** _temp4419= _temp4417.curr
+ yyvsp_offset; if( _temp4417.base == 0? 1:( _temp4419 < _temp4417.base? 1:
_temp4419 >= _temp4417.last_plus_one)){ _throw( Null_Exception);}* _temp4419;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4416;}); _temp4415;})); _temp4414->tl= 0; _temp4414;}); _temp4413;});
_temp4412;}); break; case 161: _LL4411: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeList_tok_struct* _temp4421=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4421[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4422; _temp4422.tag= Cyc_TypeList_tok_tag; _temp4422.f1=({ struct Cyc_List_List*
_temp4423=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4423->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4427=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4427[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4428; _temp4428.tag= Cyc_Absyn_AccessEff_tag;
_temp4428.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4429= yyvs; struct _xtunion_struct** _temp4431= _temp4429.curr
+( yyvsp_offset - 2); if( _temp4429.base == 0? 1:( _temp4431 < _temp4429.base? 1:
_temp4431 >= _temp4429.last_plus_one)){ _throw( Null_Exception);}* _temp4431;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4428;}); _temp4427;})); _temp4423->tl= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4424= yyvs; struct _xtunion_struct** _temp4426= _temp4424.curr
+ yyvsp_offset; if( _temp4424.base == 0? 1:( _temp4426 < _temp4424.base? 1:
_temp4426 >= _temp4424.last_plus_one)){ _throw( Null_Exception);}* _temp4426;}));
_temp4423;}); _temp4422;}); _temp4421;}); break; case 162: _LL4420: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp4433= yyvs; struct _xtunion_struct** _temp4435=
_temp4433.curr + yyvsp_offset; if( _temp4433.base == 0? 1:( _temp4435 <
_temp4433.base? 1: _temp4435 >= _temp4433.last_plus_one)){ _throw(
Null_Exception);}* _temp4435;})) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4436=( char*)"expecting region kind (R)";
struct _tagged_string _temp4437; _temp4437.curr= _temp4436; _temp4437.base=
_temp4436; _temp4437.last_plus_one= _temp4436 + 26; _temp4437;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4438= yyls; struct Cyc_Yyltype* _temp4440= _temp4438.curr
+ yylsp_offset; if( _temp4438.base == 0? 1:( _temp4440 < _temp4438.base? 1:
_temp4440 >= _temp4438.last_plus_one)){ _throw( Null_Exception);}* _temp4440;}).first_line,({
struct _tagged_ptr3 _temp4441= yyls; struct Cyc_Yyltype* _temp4443= _temp4441.curr
+ yylsp_offset; if( _temp4441.base == 0? 1:( _temp4443 < _temp4441.base? 1:
_temp4443 >= _temp4441.last_plus_one)){ _throw( Null_Exception);}* _temp4443;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4444=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4444[ 0]=({ struct Cyc_TypeList_tok_struct _temp4445; _temp4445.tag= Cyc_TypeList_tok_tag;
_temp4445.f1=({ struct Cyc_List_List* _temp4446=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4446->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp4447=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp4447[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp4448; _temp4448.tag= Cyc_Absyn_AccessEff_tag;
_temp4448.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4449= yyvs; struct _xtunion_struct** _temp4451= _temp4449.curr
+( yyvsp_offset - 2); if( _temp4449.base == 0? 1:( _temp4451 < _temp4449.base? 1:
_temp4451 >= _temp4449.last_plus_one)){ _throw( Null_Exception);}* _temp4451;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4448;}); _temp4447;})); _temp4446->tl= 0; _temp4446;}); _temp4445;});
_temp4444;}); break; case 163: _LL4432: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp4453= yyvs; struct _xtunion_struct** _temp4455= _temp4453.curr
+( yyvsp_offset - 2); if( _temp4453.base == 0? 1:( _temp4455 < _temp4453.base? 1:
_temp4455 >= _temp4453.last_plus_one)){ _throw( Null_Exception);}* _temp4455;}))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp4456=( char*)"expecting region kind (R)"; struct _tagged_string _temp4457;
_temp4457.curr= _temp4456; _temp4457.base= _temp4456; _temp4457.last_plus_one=
_temp4456 + 26; _temp4457;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4458= yyls; struct Cyc_Yyltype* _temp4460= _temp4458.curr +( yylsp_offset -
2); if( _temp4458.base == 0? 1:( _temp4460 < _temp4458.base? 1: _temp4460 >=
_temp4458.last_plus_one)){ _throw( Null_Exception);}* _temp4460;}).first_line,({
struct _tagged_ptr3 _temp4461= yyls; struct Cyc_Yyltype* _temp4463= _temp4461.curr
+( yylsp_offset - 2); if( _temp4461.base == 0? 1:( _temp4463 < _temp4461.base? 1:
_temp4463 >= _temp4461.last_plus_one)){ _throw( Null_Exception);}* _temp4463;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4464=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4464[ 0]=({ struct Cyc_TypeList_tok_struct _temp4465; _temp4465.tag= Cyc_TypeList_tok_tag;
_temp4465.f1=({ struct Cyc_List_List* _temp4466=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4466->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp4470=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp4470[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp4471; _temp4471.tag= Cyc_Absyn_AccessEff_tag;
_temp4471.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4472= yyvs; struct _xtunion_struct** _temp4474= _temp4472.curr
+( yyvsp_offset - 4); if( _temp4472.base == 0? 1:( _temp4474 < _temp4472.base? 1:
_temp4474 >= _temp4472.last_plus_one)){ _throw( Null_Exception);}* _temp4474;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4471;}); _temp4470;})); _temp4466->tl= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4467= yyvs; struct _xtunion_struct** _temp4469= _temp4467.curr
+ yyvsp_offset; if( _temp4467.base == 0? 1:( _temp4469 < _temp4467.base? 1:
_temp4469 >= _temp4467.last_plus_one)){ _throw( Null_Exception);}* _temp4469;}));
_temp4466;}); _temp4465;}); _temp4464;}); break; case 164: _LL4452: yyval=(
struct _xtunion_struct*)({ struct Cyc_ParamDeclList_tok_struct* _temp4476=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp4476[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp4477; _temp4477.tag=
Cyc_ParamDeclList_tok_tag; _temp4477.f1=({ struct Cyc_List_List* _temp4478=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4478->hd=(
void*) Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4479= yyvs; struct
_xtunion_struct** _temp4481= _temp4479.curr + yyvsp_offset; if( _temp4479.base
== 0? 1:( _temp4481 < _temp4479.base? 1: _temp4481 >= _temp4479.last_plus_one)){
_throw( Null_Exception);}* _temp4481;})); _temp4478->tl= 0; _temp4478;});
_temp4477;}); _temp4476;}); break; case 165: _LL4475: yyval=( struct
_xtunion_struct*)({ struct Cyc_ParamDeclList_tok_struct* _temp4483=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp4483[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp4484; _temp4484.tag= Cyc_ParamDeclList_tok_tag;
_temp4484.f1=({ struct Cyc_List_List* _temp4485=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4485->hd=( void*) Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp4489= yyvs; struct _xtunion_struct** _temp4491=
_temp4489.curr + yyvsp_offset; if( _temp4489.base == 0? 1:( _temp4491 <
_temp4489.base? 1: _temp4491 >= _temp4489.last_plus_one)){ _throw(
Null_Exception);}* _temp4491;})); _temp4485->tl= Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4486= yyvs; struct _xtunion_struct** _temp4488=
_temp4486.curr +( yyvsp_offset - 2); if( _temp4486.base == 0? 1:( _temp4488 <
_temp4486.base? 1: _temp4488 >= _temp4486.last_plus_one)){ _throw(
Null_Exception);}* _temp4488;})); _temp4485;}); _temp4484;}); _temp4483;});
break; case 166: _LL4482: { void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4555= yyvs; struct _xtunion_struct** _temp4557=
_temp4555.curr +( yyvsp_offset - 1); if( _temp4555.base == 0? 1:( _temp4557 <
_temp4555.base? 1: _temp4557 >= _temp4555.last_plus_one)){ _throw(
Null_Exception);}* _temp4557;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4558= yyls; struct Cyc_Yyltype* _temp4560= _temp4558.curr +(
yylsp_offset - 1); if( _temp4558.base == 0? 1:( _temp4560 < _temp4558.base? 1:
_temp4560 >= _temp4558.last_plus_one)){ _throw( Null_Exception);}* _temp4560;}).first_line,({
struct _tagged_ptr3 _temp4561= yyls; struct Cyc_Yyltype* _temp4563= _temp4561.curr
+( yylsp_offset - 1); if( _temp4561.base == 0? 1:( _temp4563 < _temp4561.base? 1:
_temp4563 >= _temp4561.last_plus_one)){ _throw( Null_Exception);}* _temp4563;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4552= yyvs; struct _xtunion_struct** _temp4554= _temp4552.curr +(
yyvsp_offset - 1); if( _temp4552.base == 0? 1:( _temp4554 < _temp4552.base? 1:
_temp4554 >= _temp4552.last_plus_one)){ _throw( Null_Exception);}* _temp4554;}))).f3;
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4549= yyvs; struct _xtunion_struct** _temp4551= _temp4549.curr +(
yyvsp_offset - 1); if( _temp4549.base == 0? 1:( _temp4551 < _temp4549.base? 1:
_temp4551 >= _temp4549.last_plus_one)){ _throw( Null_Exception);}* _temp4551;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp4546= yyvs; struct _xtunion_struct** _temp4548= _temp4546.curr +
yyvsp_offset; if( _temp4546.base == 0? 1:( _temp4548 < _temp4546.base? 1:
_temp4548 >= _temp4546.last_plus_one)){ _throw( Null_Exception);}* _temp4548;})))->tms;
struct _tuple6 t_info= Cyc_Parse_apply_tms( tq, t, atts, tms); if( t_info.f3 !=
0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp4493=( char*)"parameter with bad type params";
struct _tagged_string _temp4494; _temp4494.curr= _temp4493; _temp4494.base=
_temp4493; _temp4494.last_plus_one= _temp4493 + 31; _temp4494;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4495= yyls; struct Cyc_Yyltype* _temp4497= _temp4495.curr
+ yylsp_offset; if( _temp4495.base == 0? 1:( _temp4497 < _temp4495.base? 1:
_temp4497 >= _temp4495.last_plus_one)){ _throw( Null_Exception);}* _temp4497;}).first_line,({
struct _tagged_ptr3 _temp4498= yyls; struct Cyc_Yyltype* _temp4500= _temp4498.curr
+ yylsp_offset; if( _temp4498.base == 0? 1:( _temp4500 < _temp4498.base? 1:
_temp4500 >= _temp4498.last_plus_one)){ _throw( Null_Exception);}* _temp4500;}).last_line));}{
struct _tuple1* q=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp4543=
yyvs; struct _xtunion_struct** _temp4545= _temp4543.curr + yyvsp_offset; if(
_temp4543.base == 0? 1:( _temp4545 < _temp4543.base? 1: _temp4545 >= _temp4543.last_plus_one)){
_throw( Null_Exception);}* _temp4545;})))->id;{ void* _temp4501=(* q).f1; struct
Cyc_List_List* _temp4511; struct Cyc_List_List* _temp4513; _LL4503: if(( int)
_temp4501 == Cyc_Absyn_Loc_n){ goto _LL4504;} else{ goto _LL4505;} _LL4505: if((
unsigned int) _temp4501 > 1u?(( struct _tunion_struct*) _temp4501)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL4512: _temp4511=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp4501)->f1; if( _temp4511 == 0){ goto _LL4506;} else{ goto _LL4507;}} else{
goto _LL4507;} _LL4507: if(( unsigned int) _temp4501 > 1u?(( struct
_tunion_struct*) _temp4501)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL4514: _temp4513=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp4501)->f1; if(
_temp4513 == 0){ goto _LL4508;} else{ goto _LL4509;}} else{ goto _LL4509;}
_LL4509: goto _LL4510; _LL4504: goto _LL4502; _LL4506: goto _LL4502; _LL4508:
goto _LL4502; _LL4510: Cyc_Parse_err(( struct _tagged_string)({ char* _temp4515=(
char*)"parameter cannot be qualified with a module name"; struct _tagged_string
_temp4516; _temp4516.curr= _temp4515; _temp4516.base= _temp4515; _temp4516.last_plus_one=
_temp4515 + 49; _temp4516;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4517= yyls; struct Cyc_Yyltype* _temp4519= _temp4517.curr +( yylsp_offset -
1); if( _temp4517.base == 0? 1:( _temp4519 < _temp4517.base? 1: _temp4519 >=
_temp4517.last_plus_one)){ _throw( Null_Exception);}* _temp4519;}).first_line,({
struct _tagged_ptr3 _temp4520= yyls; struct Cyc_Yyltype* _temp4522= _temp4520.curr
+( yylsp_offset - 1); if( _temp4520.base == 0? 1:( _temp4522 < _temp4520.base? 1:
_temp4522 >= _temp4520.last_plus_one)){ _throw( Null_Exception);}* _temp4522;}).last_line));
goto _LL4502; _LL4502:;}{ struct Cyc_Core_Opt* idopt=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp4542=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp4542->v=( void*)(* q).f2; _temp4542;}); if( t_info.f4 != 0){
Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4523=( char*)"extra attributes on parameter, ignoring";
struct _tagged_string _temp4524; _temp4524.curr= _temp4523; _temp4524.base=
_temp4523; _temp4524.last_plus_one= _temp4523 + 40; _temp4524;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4525= yyls; struct Cyc_Yyltype* _temp4527= _temp4525.curr
+( yylsp_offset - 1); if( _temp4525.base == 0? 1:( _temp4527 < _temp4525.base? 1:
_temp4527 >= _temp4525.last_plus_one)){ _throw( Null_Exception);}* _temp4527;}).first_line,({
struct _tagged_ptr3 _temp4528= yyls; struct Cyc_Yyltype* _temp4530= _temp4528.curr
+ yylsp_offset; if( _temp4528.base == 0? 1:( _temp4530 < _temp4528.base? 1:
_temp4530 >= _temp4528.last_plus_one)){ _throw( Null_Exception);}* _temp4530;}).last_line));}
if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4531=(
char*)"extra type variables on parameter, ignoring"; struct _tagged_string
_temp4532; _temp4532.curr= _temp4531; _temp4532.base= _temp4531; _temp4532.last_plus_one=
_temp4531 + 44; _temp4532;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4533= yyls; struct Cyc_Yyltype* _temp4535= _temp4533.curr +( yylsp_offset -
1); if( _temp4533.base == 0? 1:( _temp4535 < _temp4533.base? 1: _temp4535 >=
_temp4533.last_plus_one)){ _throw( Null_Exception);}* _temp4535;}).first_line,({
struct _tagged_ptr3 _temp4536= yyls; struct Cyc_Yyltype* _temp4538= _temp4536.curr
+ yylsp_offset; if( _temp4536.base == 0? 1:( _temp4538 < _temp4536.base? 1:
_temp4538 >= _temp4536.last_plus_one)){ _throw( Null_Exception);}* _temp4538;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4539=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4539[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4540; _temp4540.tag= Cyc_ParamDecl_tok_tag;
_temp4540.f1=({ struct _tuple2* _temp4541=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4541->f1= idopt; _temp4541->f2= t_info.f1; _temp4541->f3=
t_info.f2; _temp4541;}); _temp4540;}); _temp4539;}); break;}}} case 167: _LL4492: {
void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp4582= yyvs; struct _xtunion_struct** _temp4584= _temp4582.curr
+ yyvsp_offset; if( _temp4582.base == 0? 1:( _temp4584 < _temp4582.base? 1:
_temp4584 >= _temp4582.last_plus_one)){ _throw( Null_Exception);}* _temp4584;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4585= yyls; struct Cyc_Yyltype*
_temp4587= _temp4585.curr + yylsp_offset; if( _temp4585.base == 0? 1:( _temp4587
< _temp4585.base? 1: _temp4587 >= _temp4585.last_plus_one)){ _throw(
Null_Exception);}* _temp4587;}).first_line,({ struct _tagged_ptr3 _temp4588=
yyls; struct Cyc_Yyltype* _temp4590= _temp4588.curr + yylsp_offset; if(
_temp4588.base == 0? 1:( _temp4590 < _temp4588.base? 1: _temp4590 >= _temp4588.last_plus_one)){
_throw( Null_Exception);}* _temp4590;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4579= yyvs; struct
_xtunion_struct** _temp4581= _temp4579.curr + yyvsp_offset; if( _temp4579.base
== 0? 1:( _temp4581 < _temp4579.base? 1: _temp4581 >= _temp4579.last_plus_one)){
_throw( Null_Exception);}* _temp4581;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4576= yyvs; struct _xtunion_struct** _temp4578=
_temp4576.curr + yyvsp_offset; if( _temp4576.base == 0? 1:( _temp4578 <
_temp4576.base? 1: _temp4578 >= _temp4576.last_plus_one)){ _throw(
Null_Exception);}* _temp4578;}))).f1; if( atts != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp4565=( char*)"bad attributes on parameter, ignoring";
struct _tagged_string _temp4566; _temp4566.curr= _temp4565; _temp4566.base=
_temp4565; _temp4566.last_plus_one= _temp4565 + 38; _temp4566;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4567= yyls; struct Cyc_Yyltype* _temp4569= _temp4567.curr
+ yylsp_offset; if( _temp4567.base == 0? 1:( _temp4569 < _temp4567.base? 1:
_temp4569 >= _temp4567.last_plus_one)){ _throw( Null_Exception);}* _temp4569;}).first_line,({
struct _tagged_ptr3 _temp4570= yyls; struct Cyc_Yyltype* _temp4572= _temp4570.curr
+ yylsp_offset; if( _temp4570.base == 0? 1:( _temp4572 < _temp4570.base? 1:
_temp4572 >= _temp4570.last_plus_one)){ _throw( Null_Exception);}* _temp4572;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4573=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4573[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4574; _temp4574.tag= Cyc_ParamDecl_tok_tag;
_temp4574.f1=({ struct _tuple2* _temp4575=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4575->f1= 0; _temp4575->f2= tq; _temp4575->f3= t;
_temp4575;}); _temp4574;}); _temp4573;}); break;} case 168: _LL4564: { void* t=
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4620= yyvs; struct _xtunion_struct** _temp4622= _temp4620.curr +(
yyvsp_offset - 1); if( _temp4620.base == 0? 1:( _temp4622 < _temp4620.base? 1:
_temp4622 >= _temp4620.last_plus_one)){ _throw( Null_Exception);}* _temp4622;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4623= yyls; struct Cyc_Yyltype*
_temp4625= _temp4623.curr +( yylsp_offset - 1); if( _temp4623.base == 0? 1:(
_temp4625 < _temp4623.base? 1: _temp4625 >= _temp4623.last_plus_one)){ _throw(
Null_Exception);}* _temp4625;}).first_line,({ struct _tagged_ptr3 _temp4626=
yyls; struct Cyc_Yyltype* _temp4628= _temp4626.curr +( yylsp_offset - 1); if(
_temp4626.base == 0? 1:( _temp4628 < _temp4626.base? 1: _temp4628 >= _temp4626.last_plus_one)){
_throw( Null_Exception);}* _temp4628;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4617= yyvs; struct
_xtunion_struct** _temp4619= _temp4617.curr +( yyvsp_offset - 1); if( _temp4617.base
== 0? 1:( _temp4619 < _temp4617.base? 1: _temp4619 >= _temp4617.last_plus_one)){
_throw( Null_Exception);}* _temp4619;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4614= yyvs; struct _xtunion_struct** _temp4616=
_temp4614.curr +( yyvsp_offset - 1); if( _temp4614.base == 0? 1:( _temp4616 <
_temp4614.base? 1: _temp4616 >= _temp4614.last_plus_one)){ _throw(
Null_Exception);}* _temp4616;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4611= yyvs; struct _xtunion_struct** _temp4613=
_temp4611.curr + yyvsp_offset; if( _temp4611.base == 0? 1:( _temp4613 <
_temp4611.base? 1: _temp4613 >= _temp4611.last_plus_one)){ _throw(
Null_Exception);}* _temp4613;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp4592=( char*)"bad type parameters on formal argument, ignoring";
struct _tagged_string _temp4593; _temp4593.curr= _temp4592; _temp4593.base=
_temp4592; _temp4593.last_plus_one= _temp4592 + 49; _temp4593;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4594= yyls; struct Cyc_Yyltype* _temp4596= _temp4594.curr
+( yylsp_offset - 1); if( _temp4594.base == 0? 1:( _temp4596 < _temp4594.base? 1:
_temp4596 >= _temp4594.last_plus_one)){ _throw( Null_Exception);}* _temp4596;}).first_line,({
struct _tagged_ptr3 _temp4597= yyls; struct Cyc_Yyltype* _temp4599= _temp4597.curr
+ yylsp_offset; if( _temp4597.base == 0? 1:( _temp4599 < _temp4597.base? 1:
_temp4599 >= _temp4597.last_plus_one)){ _throw( Null_Exception);}* _temp4599;}).last_line));}
if( t_info.f4 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4600=(
char*)"bad attributes on parameter, ignoring"; struct _tagged_string _temp4601;
_temp4601.curr= _temp4600; _temp4601.base= _temp4600; _temp4601.last_plus_one=
_temp4600 + 38; _temp4601;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4602= yyls; struct Cyc_Yyltype* _temp4604= _temp4602.curr +( yylsp_offset -
1); if( _temp4602.base == 0? 1:( _temp4604 < _temp4602.base? 1: _temp4604 >=
_temp4602.last_plus_one)){ _throw( Null_Exception);}* _temp4604;}).first_line,({
struct _tagged_ptr3 _temp4605= yyls; struct Cyc_Yyltype* _temp4607= _temp4605.curr
+ yylsp_offset; if( _temp4605.base == 0? 1:( _temp4607 < _temp4605.base? 1:
_temp4607 >= _temp4605.last_plus_one)){ _throw( Null_Exception);}* _temp4607;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4608=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4608[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4609; _temp4609.tag= Cyc_ParamDecl_tok_tag;
_temp4609.f1=({ struct _tuple2* _temp4610=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4610->f1= 0; _temp4610->f2= t_info.f1; _temp4610->f3=
t_info.f2; _temp4610;}); _temp4609;}); _temp4608;}); break;} case 169: _LL4591:
yyval=( struct _xtunion_struct*)({ struct Cyc_IdList_tok_struct* _temp4630=(
struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4630[ 0]=({ struct Cyc_IdList_tok_struct _temp4631; _temp4631.tag= Cyc_IdList_tok_tag;
_temp4631.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp4632= yyvs; struct
_xtunion_struct** _temp4634= _temp4632.curr + yyvsp_offset; if( _temp4632.base
== 0? 1:( _temp4634 < _temp4632.base? 1: _temp4634 >= _temp4632.last_plus_one)){
_throw( Null_Exception);}* _temp4634;}))); _temp4631;}); _temp4630;}); break;
case 170: _LL4629: yyval=( struct _xtunion_struct*)({ struct Cyc_IdList_tok_struct*
_temp4636=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4636[ 0]=({ struct Cyc_IdList_tok_struct _temp4637; _temp4637.tag= Cyc_IdList_tok_tag;
_temp4637.f1=({ struct Cyc_List_List* _temp4638=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4638->hd=( void*)({ struct
_tagged_string* _temp4639=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4639[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp4640= yyvs; struct _xtunion_struct** _temp4642= _temp4640.curr +
yyvsp_offset; if( _temp4640.base == 0? 1:( _temp4642 < _temp4640.base? 1:
_temp4642 >= _temp4640.last_plus_one)){ _throw( Null_Exception);}* _temp4642;}));
_temp4639;}); _temp4638->tl= 0; _temp4638;}); _temp4637;}); _temp4636;}); break;
case 171: _LL4635: yyval=( struct _xtunion_struct*)({ struct Cyc_IdList_tok_struct*
_temp4644=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4644[ 0]=({ struct Cyc_IdList_tok_struct _temp4645; _temp4645.tag= Cyc_IdList_tok_tag;
_temp4645.f1=({ struct Cyc_List_List* _temp4646=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4646->hd=( void*)({ struct
_tagged_string* _temp4650=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4650[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp4651= yyvs; struct _xtunion_struct** _temp4653= _temp4651.curr +
yyvsp_offset; if( _temp4651.base == 0? 1:( _temp4653 < _temp4651.base? 1:
_temp4653 >= _temp4651.last_plus_one)){ _throw( Null_Exception);}* _temp4653;}));
_temp4650;}); _temp4646->tl= Cyc_yyget_IdList_tok(({ struct _tagged_ptr2
_temp4647= yyvs; struct _xtunion_struct** _temp4649= _temp4647.curr +(
yyvsp_offset - 2); if( _temp4647.base == 0? 1:( _temp4649 < _temp4647.base? 1:
_temp4649 >= _temp4647.last_plus_one)){ _throw( Null_Exception);}* _temp4649;}));
_temp4646;}); _temp4645;}); _temp4644;}); break; case 172: _LL4643: yyval=({
struct _tagged_ptr2 _temp4655= yyvs; struct _xtunion_struct** _temp4657=
_temp4655.curr + yyvsp_offset; if( _temp4655.base == 0? 1:( _temp4657 <
_temp4655.base? 1: _temp4657 >= _temp4655.last_plus_one)){ _throw(
Null_Exception);}* _temp4657;}); break; case 173: _LL4654: yyval=({ struct
_tagged_ptr2 _temp4659= yyvs; struct _xtunion_struct** _temp4661= _temp4659.curr
+ yyvsp_offset; if( _temp4659.base == 0? 1:( _temp4661 < _temp4659.base? 1:
_temp4661 >= _temp4659.last_plus_one)){ _throw( Null_Exception);}* _temp4661;});
break; case 174: _LL4658: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp4663=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4663[ 0]=({ struct Cyc_Exp_tok_struct _temp4664; _temp4664.tag= Cyc_Exp_tok_tag;
_temp4664.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp4665=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp4665[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp4666; _temp4666.tag= Cyc_Absyn_UnresolvedMem_e_tag; _temp4666.f1= 0;
_temp4666.f2= 0; _temp4666;}); _temp4665;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4667= yyls; struct Cyc_Yyltype* _temp4669= _temp4667.curr
+( yylsp_offset - 1); if( _temp4667.base == 0? 1:( _temp4669 < _temp4667.base? 1:
_temp4669 >= _temp4667.last_plus_one)){ _throw( Null_Exception);}* _temp4669;}).first_line,({
struct _tagged_ptr3 _temp4670= yyls; struct Cyc_Yyltype* _temp4672= _temp4670.curr
+ yylsp_offset; if( _temp4670.base == 0? 1:( _temp4672 < _temp4670.base? 1:
_temp4672 >= _temp4670.last_plus_one)){ _throw( Null_Exception);}* _temp4672;}).last_line));
_temp4664;}); _temp4663;}); break; case 175: _LL4662: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp4674=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4674[ 0]=({ struct Cyc_Exp_tok_struct
_temp4675; _temp4675.tag= Cyc_Exp_tok_tag; _temp4675.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4676=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4676[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4677; _temp4677.tag= Cyc_Absyn_UnresolvedMem_e_tag;
_temp4677.f1= 0; _temp4677.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp4678= yyvs; struct _xtunion_struct** _temp4680= _temp4678.curr +(
yyvsp_offset - 1); if( _temp4678.base == 0? 1:( _temp4680 < _temp4678.base? 1:
_temp4680 >= _temp4678.last_plus_one)){ _throw( Null_Exception);}* _temp4680;})));
_temp4677;}); _temp4676;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4681= yyls; struct Cyc_Yyltype* _temp4683= _temp4681.curr +( yylsp_offset -
2); if( _temp4681.base == 0? 1:( _temp4683 < _temp4681.base? 1: _temp4683 >=
_temp4681.last_plus_one)){ _throw( Null_Exception);}* _temp4683;}).first_line,({
struct _tagged_ptr3 _temp4684= yyls; struct Cyc_Yyltype* _temp4686= _temp4684.curr
+ yylsp_offset; if( _temp4684.base == 0? 1:( _temp4686 < _temp4684.base? 1:
_temp4686 >= _temp4684.last_plus_one)){ _throw( Null_Exception);}* _temp4686;}).last_line));
_temp4675;}); _temp4674;}); break; case 176: _LL4673: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp4688=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4688[ 0]=({ struct Cyc_Exp_tok_struct
_temp4689; _temp4689.tag= Cyc_Exp_tok_tag; _temp4689.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4690=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4690[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4691; _temp4691.tag= Cyc_Absyn_UnresolvedMem_e_tag;
_temp4691.f1= 0; _temp4691.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp4692= yyvs; struct _xtunion_struct** _temp4694= _temp4692.curr +(
yyvsp_offset - 2); if( _temp4692.base == 0? 1:( _temp4694 < _temp4692.base? 1:
_temp4694 >= _temp4692.last_plus_one)){ _throw( Null_Exception);}* _temp4694;})));
_temp4691;}); _temp4690;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4695= yyls; struct Cyc_Yyltype* _temp4697= _temp4695.curr +( yylsp_offset -
3); if( _temp4695.base == 0? 1:( _temp4697 < _temp4695.base? 1: _temp4697 >=
_temp4695.last_plus_one)){ _throw( Null_Exception);}* _temp4697;}).first_line,({
struct _tagged_ptr3 _temp4698= yyls; struct Cyc_Yyltype* _temp4700= _temp4698.curr
+ yylsp_offset; if( _temp4698.base == 0? 1:( _temp4700 < _temp4698.base? 1:
_temp4700 >= _temp4698.last_plus_one)){ _throw( Null_Exception);}* _temp4700;}).last_line));
_temp4689;}); _temp4688;}); break; case 177: _LL4687: { struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp4719=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp4719->f1=( void*) Cyc_Absyn_Loc_n;
_temp4719->f2=({ struct _tagged_string* _temp4720=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4720[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4721= yyvs; struct _xtunion_struct** _temp4723=
_temp4721.curr +( yyvsp_offset - 5); if( _temp4721.base == 0? 1:( _temp4723 <
_temp4721.base? 1: _temp4723 >= _temp4721.last_plus_one)){ _throw(
Null_Exception);}* _temp4723;})); _temp4720;}); _temp4719;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4724= yyls; struct Cyc_Yyltype* _temp4726= _temp4724.curr
+( yylsp_offset - 5); if( _temp4724.base == 0? 1:( _temp4726 < _temp4724.base? 1:
_temp4726 >= _temp4724.last_plus_one)){ _throw( Null_Exception);}* _temp4726;}).first_line,({
struct _tagged_ptr3 _temp4727= yyls; struct Cyc_Yyltype* _temp4729= _temp4727.curr
+( yylsp_offset - 5); if( _temp4727.base == 0? 1:( _temp4729 < _temp4727.base? 1:
_temp4729 >= _temp4727.last_plus_one)){ _throw( Null_Exception);}* _temp4729;}).last_line)));
vd->tq=({ struct Cyc_Absyn_Tqual* _temp4702=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp4702->q_const= 1;
_temp4702->q_volatile= 0; _temp4702->q_restrict= 1; _temp4702;}); yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp4703=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4703[ 0]=({ struct Cyc_Exp_tok_struct
_temp4704; _temp4704.tag= Cyc_Exp_tok_tag; _temp4704.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Comprehension_e_struct* _temp4705=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp4705[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp4706; _temp4706.tag= Cyc_Absyn_Comprehension_e_tag;
_temp4706.f1= vd; _temp4706.f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4707= yyvs; struct _xtunion_struct** _temp4709= _temp4707.curr +(
yyvsp_offset - 3); if( _temp4707.base == 0? 1:( _temp4709 < _temp4707.base? 1:
_temp4709 >= _temp4707.last_plus_one)){ _throw( Null_Exception);}* _temp4709;}));
_temp4706.f3= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4710= yyvs; struct
_xtunion_struct** _temp4712= _temp4710.curr +( yyvsp_offset - 1); if( _temp4710.base
== 0? 1:( _temp4712 < _temp4710.base? 1: _temp4712 >= _temp4710.last_plus_one)){
_throw( Null_Exception);}* _temp4712;})); _temp4706;}); _temp4705;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4713= yyls; struct Cyc_Yyltype* _temp4715= _temp4713.curr
+( yylsp_offset - 7); if( _temp4713.base == 0? 1:( _temp4715 < _temp4713.base? 1:
_temp4715 >= _temp4713.last_plus_one)){ _throw( Null_Exception);}* _temp4715;}).first_line,({
struct _tagged_ptr3 _temp4716= yyls; struct Cyc_Yyltype* _temp4718= _temp4716.curr
+ yylsp_offset; if( _temp4716.base == 0? 1:( _temp4718 < _temp4716.base? 1:
_temp4718 >= _temp4716.last_plus_one)){ _throw( Null_Exception);}* _temp4718;}).last_line));
_temp4704;}); _temp4703;}); break;} case 178: _LL4701: yyval=( struct
_xtunion_struct*)({ struct Cyc_InitializerList_tok_struct* _temp4731=( struct
Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp4731[ 0]=({ struct Cyc_InitializerList_tok_struct _temp4732; _temp4732.tag=
Cyc_InitializerList_tok_tag; _temp4732.f1=({ struct Cyc_List_List* _temp4733=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4733->hd=(
void*)({ struct _tuple18* _temp4734=( struct _tuple18*) GC_malloc( sizeof(
struct _tuple18)); _temp4734->f1= 0; _temp4734->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4735= yyvs; struct _xtunion_struct** _temp4737= _temp4735.curr
+ yyvsp_offset; if( _temp4735.base == 0? 1:( _temp4737 < _temp4735.base? 1:
_temp4737 >= _temp4735.last_plus_one)){ _throw( Null_Exception);}* _temp4737;}));
_temp4734;}); _temp4733->tl= 0; _temp4733;}); _temp4732;}); _temp4731;}); break;
case 179: _LL4730: yyval=( struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct*
_temp4739=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4739[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4740; _temp4740.tag= Cyc_InitializerList_tok_tag; _temp4740.f1=({ struct
Cyc_List_List* _temp4741=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4741->hd=( void*)({ struct _tuple18* _temp4742=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4742->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4746= yyvs; struct _xtunion_struct** _temp4748=
_temp4746.curr +( yyvsp_offset - 1); if( _temp4746.base == 0? 1:( _temp4748 <
_temp4746.base? 1: _temp4748 >= _temp4746.last_plus_one)){ _throw(
Null_Exception);}* _temp4748;})); _temp4742->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4743= yyvs; struct _xtunion_struct** _temp4745= _temp4743.curr
+ yyvsp_offset; if( _temp4743.base == 0? 1:( _temp4745 < _temp4743.base? 1:
_temp4745 >= _temp4743.last_plus_one)){ _throw( Null_Exception);}* _temp4745;}));
_temp4742;}); _temp4741->tl= 0; _temp4741;}); _temp4740;}); _temp4739;}); break;
case 180: _LL4738: yyval=( struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct*
_temp4750=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4750[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4751; _temp4751.tag= Cyc_InitializerList_tok_tag; _temp4751.f1=({ struct
Cyc_List_List* _temp4752=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4752->hd=( void*)({ struct _tuple18* _temp4756=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4756->f1= 0; _temp4756->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4757= yyvs; struct _xtunion_struct** _temp4759=
_temp4757.curr + yyvsp_offset; if( _temp4757.base == 0? 1:( _temp4759 <
_temp4757.base? 1: _temp4759 >= _temp4757.last_plus_one)){ _throw(
Null_Exception);}* _temp4759;})); _temp4756;}); _temp4752->tl= Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4753= yyvs; struct _xtunion_struct** _temp4755=
_temp4753.curr +( yyvsp_offset - 2); if( _temp4753.base == 0? 1:( _temp4755 <
_temp4753.base? 1: _temp4755 >= _temp4753.last_plus_one)){ _throw(
Null_Exception);}* _temp4755;})); _temp4752;}); _temp4751;}); _temp4750;});
break; case 181: _LL4749: yyval=( struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct*
_temp4761=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4761[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4762; _temp4762.tag= Cyc_InitializerList_tok_tag; _temp4762.f1=({ struct
Cyc_List_List* _temp4763=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4763->hd=( void*)({ struct _tuple18* _temp4767=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4767->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4771= yyvs; struct _xtunion_struct** _temp4773=
_temp4771.curr +( yyvsp_offset - 1); if( _temp4771.base == 0? 1:( _temp4773 <
_temp4771.base? 1: _temp4773 >= _temp4771.last_plus_one)){ _throw(
Null_Exception);}* _temp4773;})); _temp4767->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4768= yyvs; struct _xtunion_struct** _temp4770= _temp4768.curr
+ yyvsp_offset; if( _temp4768.base == 0? 1:( _temp4770 < _temp4768.base? 1:
_temp4770 >= _temp4768.last_plus_one)){ _throw( Null_Exception);}* _temp4770;}));
_temp4767;}); _temp4763->tl= Cyc_yyget_InitializerList_tok(({ struct
_tagged_ptr2 _temp4764= yyvs; struct _xtunion_struct** _temp4766= _temp4764.curr
+( yyvsp_offset - 3); if( _temp4764.base == 0? 1:( _temp4766 < _temp4764.base? 1:
_temp4766 >= _temp4764.last_plus_one)){ _throw( Null_Exception);}* _temp4766;}));
_temp4763;}); _temp4762;}); _temp4761;}); break; case 182: _LL4760: yyval=(
struct _xtunion_struct*)({ struct Cyc_DesignatorList_tok_struct* _temp4775=(
struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4775[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4776; _temp4776.tag=
Cyc_DesignatorList_tok_tag; _temp4776.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2
_temp4777= yyvs; struct _xtunion_struct** _temp4779= _temp4777.curr +(
yyvsp_offset - 1); if( _temp4777.base == 0? 1:( _temp4779 < _temp4777.base? 1:
_temp4779 >= _temp4777.last_plus_one)){ _throw( Null_Exception);}* _temp4779;})));
_temp4776;}); _temp4775;}); break; case 183: _LL4774: yyval=( struct
_xtunion_struct*)({ struct Cyc_DesignatorList_tok_struct* _temp4781=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp4781[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp4782; _temp4782.tag= Cyc_DesignatorList_tok_tag;
_temp4782.f1=({ struct Cyc_List_List* _temp4783=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4783->hd=( void*) Cyc_yyget_Designator_tok(({
struct _tagged_ptr2 _temp4784= yyvs; struct _xtunion_struct** _temp4786=
_temp4784.curr + yyvsp_offset; if( _temp4784.base == 0? 1:( _temp4786 <
_temp4784.base? 1: _temp4786 >= _temp4784.last_plus_one)){ _throw(
Null_Exception);}* _temp4786;})); _temp4783->tl= 0; _temp4783;}); _temp4782;});
_temp4781;}); break; case 184: _LL4780: yyval=( struct _xtunion_struct*)({
struct Cyc_DesignatorList_tok_struct* _temp4788=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp4788[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp4789; _temp4789.tag= Cyc_DesignatorList_tok_tag;
_temp4789.f1=({ struct Cyc_List_List* _temp4790=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4790->hd=( void*) Cyc_yyget_Designator_tok(({
struct _tagged_ptr2 _temp4794= yyvs; struct _xtunion_struct** _temp4796=
_temp4794.curr + yyvsp_offset; if( _temp4794.base == 0? 1:( _temp4796 <
_temp4794.base? 1: _temp4796 >= _temp4794.last_plus_one)){ _throw(
Null_Exception);}* _temp4796;})); _temp4790->tl= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4791= yyvs; struct _xtunion_struct** _temp4793=
_temp4791.curr +( yyvsp_offset - 1); if( _temp4791.base == 0? 1:( _temp4793 <
_temp4791.base? 1: _temp4793 >= _temp4791.last_plus_one)){ _throw(
Null_Exception);}* _temp4793;})); _temp4790;}); _temp4789;}); _temp4788;});
break; case 185: _LL4787: yyval=( struct _xtunion_struct*)({ struct Cyc_Designator_tok_struct*
_temp4798=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4798[ 0]=({ struct Cyc_Designator_tok_struct _temp4799; _temp4799.tag= Cyc_Designator_tok_tag;
_temp4799.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp4800=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp4800[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp4801; _temp4801.tag=
Cyc_Absyn_ArrayElement_tag; _temp4801.f1= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4802= yyvs; struct _xtunion_struct** _temp4804= _temp4802.curr
+( yyvsp_offset - 1); if( _temp4802.base == 0? 1:( _temp4804 < _temp4802.base? 1:
_temp4804 >= _temp4802.last_plus_one)){ _throw( Null_Exception);}* _temp4804;}));
_temp4801;}); _temp4800;})); _temp4799;}); _temp4798;}); break; case 186:
_LL4797: yyval=( struct _xtunion_struct*)({ struct Cyc_Designator_tok_struct*
_temp4806=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4806[ 0]=({ struct Cyc_Designator_tok_struct _temp4807; _temp4807.tag= Cyc_Designator_tok_tag;
_temp4807.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp4808=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp4808[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp4809; _temp4809.tag= Cyc_Absyn_FieldName_tag;
_temp4809.f1=({ struct _tagged_string* _temp4810=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4810[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4811= yyvs; struct _xtunion_struct** _temp4813=
_temp4811.curr + yyvsp_offset; if( _temp4811.base == 0? 1:( _temp4813 <
_temp4811.base? 1: _temp4813 >= _temp4811.last_plus_one)){ _throw(
Null_Exception);}* _temp4813;})); _temp4810;}); _temp4809;}); _temp4808;}));
_temp4807;}); _temp4806;}); break; case 187: _LL4805: { void* t= Cyc_Parse_speclist2typ((*
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
_temp4830= yyvs; struct _xtunion_struct** _temp4832= _temp4830.curr +
yyvsp_offset; if( _temp4830.base == 0? 1:( _temp4832 < _temp4830.base? 1:
_temp4832 >= _temp4830.last_plus_one)){ _throw( Null_Exception);}* _temp4832;}))).f3;
if( atts != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4815=( char*)"ignoring attributes in type";
struct _tagged_string _temp4816; _temp4816.curr= _temp4815; _temp4816.base=
_temp4815; _temp4816.last_plus_one= _temp4815 + 28; _temp4816;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4817= yyls; struct Cyc_Yyltype* _temp4819= _temp4817.curr
+ yylsp_offset; if( _temp4817.base == 0? 1:( _temp4819 < _temp4817.base? 1:
_temp4819 >= _temp4817.last_plus_one)){ _throw( Null_Exception);}* _temp4819;}).first_line,({
struct _tagged_ptr3 _temp4820= yyls; struct Cyc_Yyltype* _temp4822= _temp4820.curr
+ yylsp_offset; if( _temp4820.base == 0? 1:( _temp4822 < _temp4820.base? 1:
_temp4822 >= _temp4820.last_plus_one)){ _throw( Null_Exception);}* _temp4822;}).last_line));}{
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4827= yyvs; struct _xtunion_struct** _temp4829= _temp4827.curr +
yyvsp_offset; if( _temp4827.base == 0? 1:( _temp4829 < _temp4827.base? 1:
_temp4829 >= _temp4827.last_plus_one)){ _throw( Null_Exception);}* _temp4829;}))).f1;
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4823=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4823[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4824; _temp4824.tag= Cyc_ParamDecl_tok_tag;
_temp4824.f1=({ struct _tuple2* _temp4825=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2) * 1); _temp4825[ 0]=({ struct _tuple2 _temp4826; _temp4826.f1= 0;
_temp4826.f2= tq; _temp4826.f3= t; _temp4826;}); _temp4825;}); _temp4824;});
_temp4823;}); break;}} case 188: _LL4814: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4871= yyvs; struct
_xtunion_struct** _temp4873= _temp4871.curr +( yyvsp_offset - 1); if( _temp4871.base
== 0? 1:( _temp4873 < _temp4871.base? 1: _temp4873 >= _temp4871.last_plus_one)){
_throw( Null_Exception);}* _temp4873;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4874= yyls; struct Cyc_Yyltype* _temp4876= _temp4874.curr
+( yylsp_offset - 1); if( _temp4874.base == 0? 1:( _temp4876 < _temp4874.base? 1:
_temp4876 >= _temp4874.last_plus_one)){ _throw( Null_Exception);}* _temp4876;}).first_line,({
struct _tagged_ptr3 _temp4877= yyls; struct Cyc_Yyltype* _temp4879= _temp4877.curr
+( yylsp_offset - 1); if( _temp4877.base == 0? 1:( _temp4879 < _temp4877.base? 1:
_temp4879 >= _temp4877.last_plus_one)){ _throw( Null_Exception);}* _temp4879;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4868= yyvs; struct _xtunion_struct** _temp4870= _temp4868.curr +(
yyvsp_offset - 1); if( _temp4868.base == 0? 1:( _temp4870 < _temp4868.base? 1:
_temp4870 >= _temp4868.last_plus_one)){ _throw( Null_Exception);}* _temp4870;}))).f3;
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4865= yyvs; struct _xtunion_struct** _temp4867= _temp4865.curr +(
yyvsp_offset - 1); if( _temp4865.base == 0? 1:( _temp4867 < _temp4865.base? 1:
_temp4867 >= _temp4865.last_plus_one)){ _throw( Null_Exception);}* _temp4867;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4862= yyvs; struct _xtunion_struct** _temp4864= _temp4862.curr
+ yyvsp_offset; if( _temp4862.base == 0? 1:( _temp4864 < _temp4862.base? 1:
_temp4864 >= _temp4862.last_plus_one)){ _throw( Null_Exception);}* _temp4864;})))->tms;
struct _tuple6 t_info= Cyc_Parse_apply_tms( tq, t, atts, tms); if( t_info.f3 !=
0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4843=( char*)"bad type params, ignoring";
struct _tagged_string _temp4844; _temp4844.curr= _temp4843; _temp4844.base=
_temp4843; _temp4844.last_plus_one= _temp4843 + 26; _temp4844;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4845= yyls; struct Cyc_Yyltype* _temp4847= _temp4845.curr
+ yylsp_offset; if( _temp4845.base == 0? 1:( _temp4847 < _temp4845.base? 1:
_temp4847 >= _temp4845.last_plus_one)){ _throw( Null_Exception);}* _temp4847;}).first_line,({
struct _tagged_ptr3 _temp4848= yyls; struct Cyc_Yyltype* _temp4850= _temp4848.curr
+ yylsp_offset; if( _temp4848.base == 0? 1:( _temp4850 < _temp4848.base? 1:
_temp4850 >= _temp4848.last_plus_one)){ _throw( Null_Exception);}* _temp4850;}).last_line));}
if( t_info.f4 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4851=(
char*)"bad specifiers, ignoring"; struct _tagged_string _temp4852; _temp4852.curr=
_temp4851; _temp4852.base= _temp4851; _temp4852.last_plus_one= _temp4851 + 25;
_temp4852;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4853= yyls;
struct Cyc_Yyltype* _temp4855= _temp4853.curr + yylsp_offset; if( _temp4853.base
== 0? 1:( _temp4855 < _temp4853.base? 1: _temp4855 >= _temp4853.last_plus_one)){
_throw( Null_Exception);}* _temp4855;}).first_line,({ struct _tagged_ptr3
_temp4856= yyls; struct Cyc_Yyltype* _temp4858= _temp4856.curr + yylsp_offset;
if( _temp4856.base == 0? 1:( _temp4858 < _temp4856.base? 1: _temp4858 >=
_temp4856.last_plus_one)){ _throw( Null_Exception);}* _temp4858;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4859=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4859[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4860; _temp4860.tag= Cyc_ParamDecl_tok_tag;
_temp4860.f1=({ struct _tuple2* _temp4861=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4861->f1= 0; _temp4861->f2= t_info.f1; _temp4861->f3=
t_info.f2; _temp4861;}); _temp4860;}); _temp4859;}); break;} case 189: _LL4842:
yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct* _temp4881=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4881[ 0]=({ struct Cyc_Type_tok_struct _temp4882; _temp4882.tag= Cyc_Type_tok_tag;
_temp4882.f1=( void*)(* Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4883=
yyvs; struct _xtunion_struct** _temp4885= _temp4883.curr + yyvsp_offset; if(
_temp4883.base == 0? 1:( _temp4885 < _temp4883.base? 1: _temp4885 >= _temp4883.last_plus_one)){
_throw( Null_Exception);}* _temp4885;}))).f3; _temp4882;}); _temp4881;}); break;
case 190: _LL4880: yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct*
_temp4887=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4887[ 0]=({ struct Cyc_Type_tok_struct _temp4888; _temp4888.tag= Cyc_Type_tok_tag;
_temp4888.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4889=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4889[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4890; _temp4890.tag= Cyc_Absyn_JoinEff_tag;
_temp4890.f1= 0; _temp4890;}); _temp4889;})); _temp4888;}); _temp4887;}); break;
case 191: _LL4886: yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct*
_temp4892=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4892[ 0]=({ struct Cyc_Type_tok_struct _temp4893; _temp4893.tag= Cyc_Type_tok_tag;
_temp4893.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4894=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4894[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4895; _temp4895.tag= Cyc_Absyn_JoinEff_tag;
_temp4895.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4896= yyvs;
struct _xtunion_struct** _temp4898= _temp4896.curr +( yyvsp_offset - 1); if(
_temp4896.base == 0? 1:( _temp4898 < _temp4896.base? 1: _temp4898 >= _temp4896.last_plus_one)){
_throw( Null_Exception);}* _temp4898;})); _temp4895;}); _temp4894;})); _temp4893;});
_temp4892;}); break; case 192: _LL4891: yyval=( struct _xtunion_struct*)({
struct Cyc_Type_tok_struct* _temp4900=( struct Cyc_Type_tok_struct*) GC_malloc(
sizeof( struct Cyc_Type_tok_struct)); _temp4900[ 0]=({ struct Cyc_Type_tok_struct
_temp4901; _temp4901.tag= Cyc_Type_tok_tag; _temp4901.f1=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp4902=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4902[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4903; _temp4903.tag= Cyc_Absyn_JoinEff_tag;
_temp4903.f1=({ struct Cyc_List_List* _temp4904=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4904->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4908= yyvs; struct _xtunion_struct** _temp4910=
_temp4908.curr +( yyvsp_offset - 2); if( _temp4908.base == 0? 1:( _temp4910 <
_temp4908.base? 1: _temp4910 >= _temp4908.last_plus_one)){ _throw(
Null_Exception);}* _temp4910;})); _temp4904->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4905= yyvs; struct _xtunion_struct** _temp4907=
_temp4905.curr + yyvsp_offset; if( _temp4905.base == 0? 1:( _temp4907 <
_temp4905.base? 1: _temp4907 >= _temp4905.last_plus_one)){ _throw(
Null_Exception);}* _temp4907;})); _temp4904;}); _temp4903;}); _temp4902;}));
_temp4901;}); _temp4900;}); break; case 193: _LL4899: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4912=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4912[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4913; _temp4913.tag= Cyc_TypeList_tok_tag; _temp4913.f1=({ struct Cyc_List_List*
_temp4914=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4914->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4915= yyvs;
struct _xtunion_struct** _temp4917= _temp4915.curr + yyvsp_offset; if( _temp4915.base
== 0? 1:( _temp4917 < _temp4915.base? 1: _temp4917 >= _temp4915.last_plus_one)){
_throw( Null_Exception);}* _temp4917;})); _temp4914->tl= 0; _temp4914;});
_temp4913;}); _temp4912;}); break; case 194: _LL4911: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4919=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4919[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4920; _temp4920.tag= Cyc_TypeList_tok_tag; _temp4920.f1=({ struct Cyc_List_List*
_temp4921=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4921->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4925= yyvs;
struct _xtunion_struct** _temp4927= _temp4925.curr + yyvsp_offset; if( _temp4925.base
== 0? 1:( _temp4927 < _temp4925.base? 1: _temp4927 >= _temp4925.last_plus_one)){
_throw( Null_Exception);}* _temp4927;})); _temp4921->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4922= yyvs; struct _xtunion_struct** _temp4924=
_temp4922.curr +( yyvsp_offset - 2); if( _temp4922.base == 0? 1:( _temp4924 <
_temp4922.base? 1: _temp4924 >= _temp4922.last_plus_one)){ _throw(
Null_Exception);}* _temp4924;})); _temp4921;}); _temp4920;}); _temp4919;});
break; case 195: _LL4918: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4929=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4929[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4930; _temp4930.tag= Cyc_AbstractDeclarator_tok_tag; _temp4930.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4931=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4931->tms= Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp4932= yyvs; struct _xtunion_struct** _temp4934=
_temp4932.curr + yyvsp_offset; if( _temp4932.base == 0? 1:( _temp4934 <
_temp4932.base? 1: _temp4934 >= _temp4932.last_plus_one)){ _throw(
Null_Exception);}* _temp4934;})); _temp4931;}); _temp4930;}); _temp4929;});
break; case 196: _LL4928: yyval=({ struct _tagged_ptr2 _temp4936= yyvs; struct
_xtunion_struct** _temp4938= _temp4936.curr + yyvsp_offset; if( _temp4936.base
== 0? 1:( _temp4938 < _temp4936.base? 1: _temp4938 >= _temp4936.last_plus_one)){
_throw( Null_Exception);}* _temp4938;}); break; case 197: _LL4935: yyval=(
struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4940=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4940[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4941; _temp4941.tag=
Cyc_AbstractDeclarator_tok_tag; _temp4941.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4942=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4942->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2
_temp4943= yyvs; struct _xtunion_struct** _temp4945= _temp4943.curr +(
yyvsp_offset - 1); if( _temp4943.base == 0? 1:( _temp4945 < _temp4943.base? 1:
_temp4945 >= _temp4943.last_plus_one)){ _throw( Null_Exception);}* _temp4945;})),(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4946= yyvs; struct
_xtunion_struct** _temp4948= _temp4946.curr + yyvsp_offset; if( _temp4946.base
== 0? 1:( _temp4948 < _temp4946.base? 1: _temp4948 >= _temp4946.last_plus_one)){
_throw( Null_Exception);}* _temp4948;})))->tms); _temp4942;}); _temp4941;});
_temp4940;}); break; case 198: _LL4939: yyval=({ struct _tagged_ptr2 _temp4950=
yyvs; struct _xtunion_struct** _temp4952= _temp4950.curr +( yyvsp_offset - 1);
if( _temp4950.base == 0? 1:( _temp4952 < _temp4950.base? 1: _temp4952 >=
_temp4950.last_plus_one)){ _throw( Null_Exception);}* _temp4952;}); break; case
199: _LL4949: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4954=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4954[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4955; _temp4955.tag= Cyc_AbstractDeclarator_tok_tag; _temp4955.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4956=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4956->tms=({
struct Cyc_List_List* _temp4957=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4957->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4957->tl= 0; _temp4957;}); _temp4956;}); _temp4955;}); _temp4954;}); break;
case 200: _LL4953: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4959=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4959[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4960; _temp4960.tag= Cyc_AbstractDeclarator_tok_tag; _temp4960.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4961=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4961->tms=({
struct Cyc_List_List* _temp4962=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4962->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4962->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4963= yyvs; struct _xtunion_struct** _temp4965= _temp4963.curr +(
yyvsp_offset - 2); if( _temp4963.base == 0? 1:( _temp4965 < _temp4963.base? 1:
_temp4965 >= _temp4963.last_plus_one)){ _throw( Null_Exception);}* _temp4965;})))->tms;
_temp4962;}); _temp4961;}); _temp4960;}); _temp4959;}); break; case 201: _LL4958:
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4967=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4967[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4968; _temp4968.tag= Cyc_AbstractDeclarator_tok_tag; _temp4968.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4969=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4969->tms=({
struct Cyc_List_List* _temp4970=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4970->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4971=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4971[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp4972; _temp4972.tag= Cyc_Absyn_ConstArray_mod_tag; _temp4972.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4973= yyvs; struct _xtunion_struct** _temp4975=
_temp4973.curr +( yyvsp_offset - 1); if( _temp4973.base == 0? 1:( _temp4975 <
_temp4973.base? 1: _temp4975 >= _temp4973.last_plus_one)){ _throw(
Null_Exception);}* _temp4975;})); _temp4972;}); _temp4971;})); _temp4970->tl= 0;
_temp4970;}); _temp4969;}); _temp4968;}); _temp4967;}); break; case 202: _LL4966:
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4977=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4977[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4978; _temp4978.tag= Cyc_AbstractDeclarator_tok_tag; _temp4978.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4979=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4979->tms=({
struct Cyc_List_List* _temp4980=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4980->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4984=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4984[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp4985; _temp4985.tag= Cyc_Absyn_ConstArray_mod_tag; _temp4985.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4986= yyvs; struct _xtunion_struct** _temp4988=
_temp4986.curr +( yyvsp_offset - 1); if( _temp4986.base == 0? 1:( _temp4988 <
_temp4986.base? 1: _temp4988 >= _temp4986.last_plus_one)){ _throw(
Null_Exception);}* _temp4988;})); _temp4985;}); _temp4984;})); _temp4980->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4981= yyvs; struct
_xtunion_struct** _temp4983= _temp4981.curr +( yyvsp_offset - 3); if( _temp4981.base
== 0? 1:( _temp4983 < _temp4981.base? 1: _temp4983 >= _temp4981.last_plus_one)){
_throw( Null_Exception);}* _temp4983;})))->tms; _temp4980;}); _temp4979;});
_temp4978;}); _temp4977;}); break; case 203: _LL4976: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4990=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4990[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4991; _temp4991.tag=
Cyc_AbstractDeclarator_tok_tag; _temp4991.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4992=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4992->tms=({ struct Cyc_List_List* _temp4993=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4993->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4994=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4994[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4995; _temp4995.tag= Cyc_Absyn_Function_mod_tag;
_temp4995.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp4996=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4996[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4997; _temp4997.tag= Cyc_Absyn_WithTypes_tag;
_temp4997.f1= 0; _temp4997.f2= 0; _temp4997.f3= 0; _temp4997;}); _temp4996;}));
_temp4995;}); _temp4994;})); _temp4993->tl= 0; _temp4993;}); _temp4992;});
_temp4991;}); _temp4990;}); break; case 204: _LL4989: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4999=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4999[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5000; _temp5000.tag=
Cyc_AbstractDeclarator_tok_tag; _temp5000.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5001=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5001->tms=({ struct Cyc_List_List* _temp5002=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5002->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5003=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5003[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5004; _temp5004.tag= Cyc_Absyn_Function_mod_tag;
_temp5004.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5005=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5005[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5006; _temp5006.tag= Cyc_Absyn_WithTypes_tag;
_temp5006.f1= 0; _temp5006.f2= 0; _temp5006.f3=({ struct Cyc_Core_Opt* _temp5007=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5007->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp5008=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp5008[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp5009; _temp5009.tag= Cyc_Absyn_JoinEff_tag;
_temp5009.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5010= yyvs;
struct _xtunion_struct** _temp5012= _temp5010.curr +( yyvsp_offset - 1); if(
_temp5010.base == 0? 1:( _temp5012 < _temp5010.base? 1: _temp5012 >= _temp5010.last_plus_one)){
_throw( Null_Exception);}* _temp5012;})); _temp5009;}); _temp5008;})); _temp5007;});
_temp5006;}); _temp5005;})); _temp5004;}); _temp5003;})); _temp5002->tl= 0;
_temp5002;}); _temp5001;}); _temp5000;}); _temp4999;}); break; case 205: _LL4998: {
struct _tuple16 _temp5019; struct Cyc_Core_Opt* _temp5020; int _temp5022; struct
Cyc_List_List* _temp5024; struct _tuple16* _temp5017= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp5014= yyvs; struct _xtunion_struct** _temp5016=
_temp5014.curr +( yyvsp_offset - 1); if( _temp5014.base == 0? 1:( _temp5016 <
_temp5014.base? 1: _temp5016 >= _temp5014.last_plus_one)){ _throw(
Null_Exception);}* _temp5016;})); _temp5019=* _temp5017; _LL5025: _temp5024=
_temp5019.f1; goto _LL5023; _LL5023: _temp5022= _temp5019.f2; goto _LL5021;
_LL5021: _temp5020= _temp5019.f3; goto _LL5018; _LL5018: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5026=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5026[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5027; _temp5027.tag=
Cyc_AbstractDeclarator_tok_tag; _temp5027.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5028=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5028->tms=({ struct Cyc_List_List* _temp5029=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5029->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5030=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5030[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5031; _temp5031.tag= Cyc_Absyn_Function_mod_tag;
_temp5031.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5032=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5032[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5033; _temp5033.tag= Cyc_Absyn_WithTypes_tag;
_temp5033.f1= _temp5024; _temp5033.f2= _temp5022; _temp5033.f3= _temp5020;
_temp5033;}); _temp5032;})); _temp5031;}); _temp5030;})); _temp5029->tl= 0;
_temp5029;}); _temp5028;}); _temp5027;}); _temp5026;}); break;} case 206:
_LL5013: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5035=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5035[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5036; _temp5036.tag= Cyc_AbstractDeclarator_tok_tag; _temp5036.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp5037=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5037->tms=({
struct Cyc_List_List* _temp5038=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5038->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp5042=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp5042[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp5043; _temp5043.tag=
Cyc_Absyn_Function_mod_tag; _temp5043.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp5044=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5044[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5045; _temp5045.tag= Cyc_Absyn_WithTypes_tag;
_temp5045.f1= 0; _temp5045.f2= 0; _temp5045.f3= 0; _temp5045;}); _temp5044;}));
_temp5043;}); _temp5042;})); _temp5038->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5039= yyvs; struct _xtunion_struct** _temp5041=
_temp5039.curr +( yyvsp_offset - 2); if( _temp5039.base == 0? 1:( _temp5041 <
_temp5039.base? 1: _temp5041 >= _temp5039.last_plus_one)){ _throw(
Null_Exception);}* _temp5041;})))->tms; _temp5038;}); _temp5037;}); _temp5036;});
_temp5035;}); break; case 207: _LL5034: { struct Cyc_Core_Opt* eff=({ struct Cyc_Core_Opt*
_temp5058=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5058->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp5059=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp5059[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp5060; _temp5060.tag= Cyc_Absyn_JoinEff_tag;
_temp5060.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5061= yyvs;
struct _xtunion_struct** _temp5063= _temp5061.curr +( yyvsp_offset - 1); if(
_temp5061.base == 0? 1:( _temp5063 < _temp5061.base? 1: _temp5063 >= _temp5061.last_plus_one)){
_throw( Null_Exception);}* _temp5063;})); _temp5060;}); _temp5059;})); _temp5058;});
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5047=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5047[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5048; _temp5048.tag= Cyc_AbstractDeclarator_tok_tag; _temp5048.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp5049=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5049->tms=({
struct Cyc_List_List* _temp5050=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5050->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp5054=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp5054[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp5055; _temp5055.tag=
Cyc_Absyn_Function_mod_tag; _temp5055.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp5056=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5056[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5057; _temp5057.tag= Cyc_Absyn_WithTypes_tag;
_temp5057.f1= 0; _temp5057.f2= 0; _temp5057.f3= eff; _temp5057;}); _temp5056;}));
_temp5055;}); _temp5054;})); _temp5050->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5051= yyvs; struct _xtunion_struct** _temp5053=
_temp5051.curr +( yyvsp_offset - 4); if( _temp5051.base == 0? 1:( _temp5053 <
_temp5051.base? 1: _temp5053 >= _temp5051.last_plus_one)){ _throw(
Null_Exception);}* _temp5053;})))->tms; _temp5050;}); _temp5049;}); _temp5048;});
_temp5047;}); break;} case 208: _LL5046: { struct _tuple16 _temp5070; struct Cyc_Core_Opt*
_temp5071; int _temp5073; struct Cyc_List_List* _temp5075; struct _tuple16*
_temp5068= Cyc_yyget_ParamDeclListBool_tok(({ struct _tagged_ptr2 _temp5065=
yyvs; struct _xtunion_struct** _temp5067= _temp5065.curr +( yyvsp_offset - 1);
if( _temp5065.base == 0? 1:( _temp5067 < _temp5065.base? 1: _temp5067 >=
_temp5065.last_plus_one)){ _throw( Null_Exception);}* _temp5067;})); _temp5070=*
_temp5068; _LL5076: _temp5075= _temp5070.f1; goto _LL5074; _LL5074: _temp5073=
_temp5070.f2; goto _LL5072; _LL5072: _temp5071= _temp5070.f3; goto _LL5069;
_LL5069: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5077=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5077[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5078; _temp5078.tag= Cyc_AbstractDeclarator_tok_tag; _temp5078.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp5079=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5079->tms=({
struct Cyc_List_List* _temp5080=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5080->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp5084=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp5084[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp5085; _temp5085.tag=
Cyc_Absyn_Function_mod_tag; _temp5085.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp5086=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5086[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5087; _temp5087.tag= Cyc_Absyn_WithTypes_tag;
_temp5087.f1= _temp5075; _temp5087.f2= _temp5073; _temp5087.f3= _temp5071;
_temp5087;}); _temp5086;})); _temp5085;}); _temp5084;})); _temp5080->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5081= yyvs; struct _xtunion_struct** _temp5083=
_temp5081.curr +( yyvsp_offset - 3); if( _temp5081.base == 0? 1:( _temp5083 <
_temp5081.base? 1: _temp5083 >= _temp5081.last_plus_one)){ _throw(
Null_Exception);}* _temp5083;})))->tms; _temp5080;}); _temp5079;}); _temp5078;});
_temp5077;}); break;} case 209: _LL5064: { struct Cyc_List_List* ts=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5104=
yyls; struct Cyc_Yyltype* _temp5106= _temp5104.curr +( yylsp_offset - 2); if(
_temp5104.base == 0? 1:( _temp5106 < _temp5104.base? 1: _temp5106 >= _temp5104.last_plus_one)){
_throw( Null_Exception);}* _temp5106;}).first_line,({ struct _tagged_ptr3
_temp5107= yyls; struct Cyc_Yyltype* _temp5109= _temp5107.curr + yylsp_offset;
if( _temp5107.base == 0? 1:( _temp5109 < _temp5107.base? 1: _temp5109 >=
_temp5107.last_plus_one)){ _throw( Null_Exception);}* _temp5109;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5110= yyvs; struct _xtunion_struct** _temp5112=
_temp5110.curr +( yyvsp_offset - 1); if( _temp5110.base == 0? 1:( _temp5112 <
_temp5110.base? 1: _temp5112 >= _temp5110.last_plus_one)){ _throw(
Null_Exception);}* _temp5112;})))); yyval=( struct _xtunion_struct*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp5089=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5089[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5090; _temp5090.tag= Cyc_AbstractDeclarator_tok_tag;
_temp5090.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5091=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5091->tms=({
struct Cyc_List_List* _temp5092=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5092->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp5096=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp5096[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp5097; _temp5097.tag= Cyc_Absyn_TypeParams_mod_tag; _temp5097.f1= ts;
_temp5097.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5098= yyls;
struct Cyc_Yyltype* _temp5100= _temp5098.curr +( yylsp_offset - 2); if(
_temp5098.base == 0? 1:( _temp5100 < _temp5098.base? 1: _temp5100 >= _temp5098.last_plus_one)){
_throw( Null_Exception);}* _temp5100;}).first_line,({ struct _tagged_ptr3
_temp5101= yyls; struct Cyc_Yyltype* _temp5103= _temp5101.curr + yylsp_offset;
if( _temp5101.base == 0? 1:( _temp5103 < _temp5101.base? 1: _temp5103 >=
_temp5101.last_plus_one)){ _throw( Null_Exception);}* _temp5103;}).last_line);
_temp5097.f3= 0; _temp5097;}); _temp5096;})); _temp5092->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5093= yyvs; struct _xtunion_struct** _temp5095=
_temp5093.curr +( yyvsp_offset - 3); if( _temp5093.base == 0? 1:( _temp5095 <
_temp5093.base? 1: _temp5095 >= _temp5093.last_plus_one)){ _throw(
Null_Exception);}* _temp5095;})))->tms; _temp5092;}); _temp5091;}); _temp5090;});
_temp5089;}); break;} case 210: _LL5088:(( struct Cyc_Lexing_lexbuf*)({ struct
Cyc_Core_Opt* _temp5114= Cyc_Parse_lbuf; if( _temp5114 == 0){ _throw(
Null_Exception);} _temp5114->v;}))->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5130=
yyls; struct Cyc_Yyltype* _temp5132= _temp5130.curr +( yylsp_offset - 2); if(
_temp5130.base == 0? 1:( _temp5132 < _temp5130.base? 1: _temp5132 >= _temp5130.last_plus_one)){
_throw( Null_Exception);}* _temp5132;}).first_line,({ struct _tagged_ptr3
_temp5133= yyls; struct Cyc_Yyltype* _temp5135= _temp5133.curr + yylsp_offset;
if( _temp5133.base == 0? 1:( _temp5135 < _temp5133.base? 1: _temp5135 >=
_temp5133.last_plus_one)){ _throw( Null_Exception);}* _temp5135;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5136= yyvs; struct _xtunion_struct** _temp5138=
_temp5136.curr +( yyvsp_offset - 1); if( _temp5136.base == 0? 1:( _temp5138 <
_temp5136.base? 1: _temp5138 >= _temp5136.last_plus_one)){ _throw(
Null_Exception);}* _temp5138;})))); yyval=( struct _xtunion_struct*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp5115=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5115[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5116; _temp5116.tag= Cyc_AbstractDeclarator_tok_tag;
_temp5116.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5117=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5117->tms=({
struct Cyc_List_List* _temp5118=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5118->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp5122=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp5122[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp5123; _temp5123.tag= Cyc_Absyn_TypeParams_mod_tag; _temp5123.f1= ts;
_temp5123.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5124= yyls;
struct Cyc_Yyltype* _temp5126= _temp5124.curr +( yylsp_offset - 2); if(
_temp5124.base == 0? 1:( _temp5126 < _temp5124.base? 1: _temp5126 >= _temp5124.last_plus_one)){
_throw( Null_Exception);}* _temp5126;}).first_line,({ struct _tagged_ptr3
_temp5127= yyls; struct Cyc_Yyltype* _temp5129= _temp5127.curr + yylsp_offset;
if( _temp5127.base == 0? 1:( _temp5129 < _temp5127.base? 1: _temp5129 >=
_temp5127.last_plus_one)){ _throw( Null_Exception);}* _temp5129;}).last_line);
_temp5123.f3= 0; _temp5123;}); _temp5122;})); _temp5118->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5119= yyvs; struct _xtunion_struct** _temp5121=
_temp5119.curr +( yyvsp_offset - 3); if( _temp5119.base == 0? 1:( _temp5121 <
_temp5119.base? 1: _temp5121 >= _temp5119.last_plus_one)){ _throw(
Null_Exception);}* _temp5121;})))->tms; _temp5118;}); _temp5117;}); _temp5116;});
_temp5115;}); break;} case 211: _LL5113: yyval=( struct _xtunion_struct*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp5140=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5140[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5141; _temp5141.tag= Cyc_AbstractDeclarator_tok_tag;
_temp5141.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5142=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5142->tms=({
struct Cyc_List_List* _temp5143=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5143->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp5147=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp5147[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp5148; _temp5148.tag= Cyc_Absyn_Attributes_mod_tag; _temp5148.f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5149= yyls; struct Cyc_Yyltype* _temp5151= _temp5149.curr
+ yylsp_offset; if( _temp5149.base == 0? 1:( _temp5151 < _temp5149.base? 1:
_temp5151 >= _temp5149.last_plus_one)){ _throw( Null_Exception);}* _temp5151;}).first_line,({
struct _tagged_ptr3 _temp5152= yyls; struct Cyc_Yyltype* _temp5154= _temp5152.curr
+ yylsp_offset; if( _temp5152.base == 0? 1:( _temp5154 < _temp5152.base? 1:
_temp5154 >= _temp5152.last_plus_one)){ _throw( Null_Exception);}* _temp5154;}).last_line);
_temp5148.f2= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp5155= yyvs;
struct _xtunion_struct** _temp5157= _temp5155.curr + yyvsp_offset; if( _temp5155.base
== 0? 1:( _temp5157 < _temp5155.base? 1: _temp5157 >= _temp5155.last_plus_one)){
_throw( Null_Exception);}* _temp5157;})); _temp5148;}); _temp5147;})); _temp5143->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp5144= yyvs; struct
_xtunion_struct** _temp5146= _temp5144.curr +( yyvsp_offset - 1); if( _temp5144.base
== 0? 1:( _temp5146 < _temp5144.base? 1: _temp5146 >= _temp5144.last_plus_one)){
_throw( Null_Exception);}* _temp5146;})))->tms; _temp5143;}); _temp5142;});
_temp5141;}); _temp5140;}); break; case 212: _LL5139: yyval=({ struct
_tagged_ptr2 _temp5159= yyvs; struct _xtunion_struct** _temp5161= _temp5159.curr
+ yyvsp_offset; if( _temp5159.base == 0? 1:( _temp5161 < _temp5159.base? 1:
_temp5161 >= _temp5159.last_plus_one)){ _throw( Null_Exception);}* _temp5161;});
break; case 213: _LL5158: yyval=({ struct _tagged_ptr2 _temp5163= yyvs; struct
_xtunion_struct** _temp5165= _temp5163.curr + yyvsp_offset; if( _temp5163.base
== 0? 1:( _temp5165 < _temp5163.base? 1: _temp5165 >= _temp5163.last_plus_one)){
_throw( Null_Exception);}* _temp5165;}); break; case 214: _LL5162: yyval=({
struct _tagged_ptr2 _temp5167= yyvs; struct _xtunion_struct** _temp5169=
_temp5167.curr + yyvsp_offset; if( _temp5167.base == 0? 1:( _temp5169 <
_temp5167.base? 1: _temp5169 >= _temp5167.last_plus_one)){ _throw(
Null_Exception);}* _temp5169;}); break; case 215: _LL5166: yyval=({ struct
_tagged_ptr2 _temp5171= yyvs; struct _xtunion_struct** _temp5173= _temp5171.curr
+ yyvsp_offset; if( _temp5171.base == 0? 1:( _temp5173 < _temp5171.base? 1:
_temp5173 >= _temp5171.last_plus_one)){ _throw( Null_Exception);}* _temp5173;});
break; case 216: _LL5170: yyval=({ struct _tagged_ptr2 _temp5175= yyvs; struct
_xtunion_struct** _temp5177= _temp5175.curr + yyvsp_offset; if( _temp5175.base
== 0? 1:( _temp5177 < _temp5175.base? 1: _temp5177 >= _temp5175.last_plus_one)){
_throw( Null_Exception);}* _temp5177;}); break; case 217: _LL5174: yyval=({
struct _tagged_ptr2 _temp5179= yyvs; struct _xtunion_struct** _temp5181=
_temp5179.curr + yyvsp_offset; if( _temp5179.base == 0? 1:( _temp5181 <
_temp5179.base? 1: _temp5181 >= _temp5179.last_plus_one)){ _throw(
Null_Exception);}* _temp5181;}); break; case 218: _LL5178: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5183= yyvs; struct
_xtunion_struct** _temp5185= _temp5183.curr +( yyvsp_offset - 3); if( _temp5183.base
== 0? 1:( _temp5185 < _temp5183.base? 1: _temp5185 >= _temp5183.last_plus_one)){
_throw( Null_Exception);}* _temp5185;})),( struct _tagged_string)({ char*
_temp5186=( char*)"`H"; struct _tagged_string _temp5187; _temp5187.curr=
_temp5186; _temp5187.base= _temp5186; _temp5187.last_plus_one= _temp5186 + 3;
_temp5187;})) == 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp5188=(
char*)"bad occurrence of heap region `H"; struct _tagged_string _temp5189;
_temp5189.curr= _temp5188; _temp5189.base= _temp5188; _temp5189.last_plus_one=
_temp5188 + 33; _temp5189;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5190= yyls; struct Cyc_Yyltype* _temp5192= _temp5190.curr +( yylsp_offset -
3); if( _temp5190.base == 0? 1:( _temp5192 < _temp5190.base? 1: _temp5192 >=
_temp5190.last_plus_one)){ _throw( Null_Exception);}* _temp5192;}).first_line,({
struct _tagged_ptr3 _temp5193= yyls; struct Cyc_Yyltype* _temp5195= _temp5193.curr
+( yylsp_offset - 3); if( _temp5193.base == 0? 1:( _temp5195 < _temp5193.base? 1:
_temp5195 >= _temp5193.last_plus_one)){ _throw( Null_Exception);}* _temp5195;}).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp5218=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp5218->name=({ struct
_tagged_string* _temp5219=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp5219[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp5220= yyvs; struct _xtunion_struct** _temp5222= _temp5220.curr +(
yyvsp_offset - 3); if( _temp5220.base == 0? 1:( _temp5222 < _temp5220.base? 1:
_temp5222 >= _temp5220.last_plus_one)){ _throw( Null_Exception);}* _temp5222;}));
_temp5219;}); _temp5218->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind); _temp5218;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp5216=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp5216[ 0]=({ struct Cyc_Absyn_VarType_struct _temp5217; _temp5217.tag= Cyc_Absyn_VarType_tag;
_temp5217.f1= tv; _temp5217;}); _temp5216;}); yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5196=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5196[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5197; _temp5197.tag= Cyc_Stmt_tok_tag; _temp5197.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Region_s_struct* _temp5198=( struct Cyc_Absyn_Region_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct)); _temp5198[ 0]=({ struct
Cyc_Absyn_Region_s_struct _temp5199; _temp5199.tag= Cyc_Absyn_Region_s_tag;
_temp5199.f1= tv; _temp5199.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp5200=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp5200->f1=(
void*) Cyc_Absyn_Loc_n; _temp5200->f2=({ struct _tagged_string* _temp5201=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp5201[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5202= yyvs; struct
_xtunion_struct** _temp5204= _temp5202.curr +( yyvsp_offset - 1); if( _temp5202.base
== 0? 1:( _temp5204 < _temp5202.base? 1: _temp5204 >= _temp5202.last_plus_one)){
_throw( Null_Exception);}* _temp5204;})); _temp5201;}); _temp5200;}),( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp5205=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp5205[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp5206; _temp5206.tag= Cyc_Absyn_RgnHandleType_tag;
_temp5206.f1=( void*) t; _temp5206;}); _temp5205;}), 0); _temp5199.f3= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5207= yyvs; struct _xtunion_struct** _temp5209=
_temp5207.curr + yyvsp_offset; if( _temp5207.base == 0? 1:( _temp5209 <
_temp5207.base? 1: _temp5209 >= _temp5207.last_plus_one)){ _throw(
Null_Exception);}* _temp5209;})); _temp5199;}); _temp5198;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5210= yyls; struct Cyc_Yyltype* _temp5212= _temp5210.curr
+( yylsp_offset - 5); if( _temp5210.base == 0? 1:( _temp5212 < _temp5210.base? 1:
_temp5212 >= _temp5210.last_plus_one)){ _throw( Null_Exception);}* _temp5212;}).first_line,({
struct _tagged_ptr3 _temp5213= yyls; struct Cyc_Yyltype* _temp5215= _temp5213.curr
+ yylsp_offset; if( _temp5213.base == 0? 1:( _temp5215 < _temp5213.base? 1:
_temp5215 >= _temp5213.last_plus_one)){ _throw( Null_Exception);}* _temp5215;}).last_line));
_temp5197;}); _temp5196;}); break;} case 219: _LL5182: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5224= yyvs; struct
_xtunion_struct** _temp5226= _temp5224.curr +( yyvsp_offset - 1); if( _temp5224.base
== 0? 1:( _temp5226 < _temp5224.base? 1: _temp5226 >= _temp5224.last_plus_one)){
_throw( Null_Exception);}* _temp5226;})),( struct _tagged_string)({ char*
_temp5227=( char*)"H"; struct _tagged_string _temp5228; _temp5228.curr=
_temp5227; _temp5228.base= _temp5227; _temp5228.last_plus_one= _temp5227 + 2;
_temp5228;})) == 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp5229=(
char*)"bad occurrence of heap region `H"; struct _tagged_string _temp5230;
_temp5230.curr= _temp5229; _temp5230.base= _temp5229; _temp5230.last_plus_one=
_temp5229 + 33; _temp5230;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5231= yyls; struct Cyc_Yyltype* _temp5233= _temp5231.curr + yylsp_offset;
if( _temp5231.base == 0? 1:( _temp5233 < _temp5231.base? 1: _temp5233 >=
_temp5231.last_plus_one)){ _throw( Null_Exception);}* _temp5233;}).first_line,({
struct _tagged_ptr3 _temp5234= yyls; struct Cyc_Yyltype* _temp5236= _temp5234.curr
+ yylsp_offset; if( _temp5234.base == 0? 1:( _temp5236 < _temp5234.base? 1:
_temp5236 >= _temp5234.last_plus_one)){ _throw( Null_Exception);}* _temp5236;}).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp5259=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp5259->name=({ struct
_tagged_string* _temp5260=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp5260[ 0]=({ struct _tagged_string _temp5264= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp5261= yyvs; struct _xtunion_struct** _temp5263=
_temp5261.curr +( yyvsp_offset - 1); if( _temp5261.base == 0? 1:( _temp5263 <
_temp5261.base? 1: _temp5263 >= _temp5261.last_plus_one)){ _throw(
Null_Exception);}* _temp5263;})); xprintf("`%.*s", _temp5264.last_plus_one -
_temp5264.curr, _temp5264.curr);}); _temp5260;}); _temp5259->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp5259;}); void*
t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp5257=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp5257[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp5258; _temp5258.tag= Cyc_Absyn_VarType_tag;
_temp5258.f1= tv; _temp5258;}); _temp5257;}); yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5237=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5237[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5238; _temp5238.tag= Cyc_Stmt_tok_tag; _temp5238.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Region_s_struct* _temp5239=( struct Cyc_Absyn_Region_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct)); _temp5239[ 0]=({ struct
Cyc_Absyn_Region_s_struct _temp5240; _temp5240.tag= Cyc_Absyn_Region_s_tag;
_temp5240.f1= tv; _temp5240.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp5241=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp5241->f1=(
void*) Cyc_Absyn_Loc_n; _temp5241->f2=({ struct _tagged_string* _temp5242=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp5242[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5243= yyvs; struct
_xtunion_struct** _temp5245= _temp5243.curr +( yyvsp_offset - 1); if( _temp5243.base
== 0? 1:( _temp5245 < _temp5243.base? 1: _temp5245 >= _temp5243.last_plus_one)){
_throw( Null_Exception);}* _temp5245;})); _temp5242;}); _temp5241;}),( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp5246=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp5246[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp5247; _temp5247.tag= Cyc_Absyn_RgnHandleType_tag;
_temp5247.f1=( void*) t; _temp5247;}); _temp5246;}), 0); _temp5240.f3= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5248= yyvs; struct _xtunion_struct** _temp5250=
_temp5248.curr + yyvsp_offset; if( _temp5248.base == 0? 1:( _temp5250 <
_temp5248.base? 1: _temp5250 >= _temp5248.last_plus_one)){ _throw(
Null_Exception);}* _temp5250;})); _temp5240;}); _temp5239;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5251= yyls; struct Cyc_Yyltype* _temp5253= _temp5251.curr
+( yylsp_offset - 2); if( _temp5251.base == 0? 1:( _temp5253 < _temp5251.base? 1:
_temp5253 >= _temp5251.last_plus_one)){ _throw( Null_Exception);}* _temp5253;}).first_line,({
struct _tagged_ptr3 _temp5254= yyls; struct Cyc_Yyltype* _temp5256= _temp5254.curr
+ yylsp_offset; if( _temp5254.base == 0? 1:( _temp5256 < _temp5254.base? 1:
_temp5256 >= _temp5254.last_plus_one)){ _throw( Null_Exception);}* _temp5256;}).last_line));
_temp5238;}); _temp5237;}); break;} case 220: _LL5223: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5266=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5266[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5267; _temp5267.tag= Cyc_Stmt_tok_tag; _temp5267.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Cut_s_struct* _temp5268=( struct Cyc_Absyn_Cut_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct)); _temp5268[ 0]=({ struct Cyc_Absyn_Cut_s_struct
_temp5269; _temp5269.tag= Cyc_Absyn_Cut_s_tag; _temp5269.f1= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5270= yyvs; struct _xtunion_struct** _temp5272=
_temp5270.curr + yyvsp_offset; if( _temp5270.base == 0? 1:( _temp5272 <
_temp5270.base? 1: _temp5272 >= _temp5270.last_plus_one)){ _throw(
Null_Exception);}* _temp5272;})); _temp5269;}); _temp5268;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5273= yyls; struct Cyc_Yyltype* _temp5275= _temp5273.curr
+( yylsp_offset - 1); if( _temp5273.base == 0? 1:( _temp5275 < _temp5273.base? 1:
_temp5275 >= _temp5273.last_plus_one)){ _throw( Null_Exception);}* _temp5275;}).first_line,({
struct _tagged_ptr3 _temp5276= yyls; struct Cyc_Yyltype* _temp5278= _temp5276.curr
+ yylsp_offset; if( _temp5276.base == 0? 1:( _temp5278 < _temp5276.base? 1:
_temp5278 >= _temp5276.last_plus_one)){ _throw( Null_Exception);}* _temp5278;}).last_line));
_temp5267;}); _temp5266;}); break; case 221: _LL5265: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5280=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5280[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5281; _temp5281.tag= Cyc_Stmt_tok_tag; _temp5281.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Splice_s_struct* _temp5282=( struct Cyc_Absyn_Splice_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp5282[ 0]=({ struct
Cyc_Absyn_Splice_s_struct _temp5283; _temp5283.tag= Cyc_Absyn_Splice_s_tag;
_temp5283.f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5284= yyvs; struct
_xtunion_struct** _temp5286= _temp5284.curr + yyvsp_offset; if( _temp5284.base
== 0? 1:( _temp5286 < _temp5284.base? 1: _temp5286 >= _temp5284.last_plus_one)){
_throw( Null_Exception);}* _temp5286;})); _temp5283;}); _temp5282;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5287= yyls; struct Cyc_Yyltype* _temp5289= _temp5287.curr
+( yylsp_offset - 1); if( _temp5287.base == 0? 1:( _temp5289 < _temp5287.base? 1:
_temp5289 >= _temp5287.last_plus_one)){ _throw( Null_Exception);}* _temp5289;}).first_line,({
struct _tagged_ptr3 _temp5290= yyls; struct Cyc_Yyltype* _temp5292= _temp5290.curr
+ yylsp_offset; if( _temp5290.base == 0? 1:( _temp5292 < _temp5290.base? 1:
_temp5292 >= _temp5290.last_plus_one)){ _throw( Null_Exception);}* _temp5292;}).last_line));
_temp5281;}); _temp5280;}); break; case 222: _LL5279: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5294=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5294[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5295; _temp5295.tag= Cyc_Stmt_tok_tag; _temp5295.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Label_s_struct* _temp5296=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp5296[ 0]=({ struct
Cyc_Absyn_Label_s_struct _temp5297; _temp5297.tag= Cyc_Absyn_Label_s_tag;
_temp5297.f1=({ struct _tagged_string* _temp5298=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp5298[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp5299= yyvs; struct _xtunion_struct** _temp5301=
_temp5299.curr +( yyvsp_offset - 2); if( _temp5299.base == 0? 1:( _temp5301 <
_temp5299.base? 1: _temp5301 >= _temp5299.last_plus_one)){ _throw(
Null_Exception);}* _temp5301;})); _temp5298;}); _temp5297.f2= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5302= yyvs; struct _xtunion_struct** _temp5304=
_temp5302.curr + yyvsp_offset; if( _temp5302.base == 0? 1:( _temp5304 <
_temp5302.base? 1: _temp5304 >= _temp5302.last_plus_one)){ _throw(
Null_Exception);}* _temp5304;})); _temp5297;}); _temp5296;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5305= yyls; struct Cyc_Yyltype* _temp5307= _temp5305.curr
+( yylsp_offset - 2); if( _temp5305.base == 0? 1:( _temp5307 < _temp5305.base? 1:
_temp5307 >= _temp5305.last_plus_one)){ _throw( Null_Exception);}* _temp5307;}).first_line,({
struct _tagged_ptr3 _temp5308= yyls; struct Cyc_Yyltype* _temp5310= _temp5308.curr
+ yylsp_offset; if( _temp5308.base == 0? 1:( _temp5310 < _temp5308.base? 1:
_temp5310 >= _temp5308.last_plus_one)){ _throw( Null_Exception);}* _temp5310;}).last_line));
_temp5295;}); _temp5294;}); break; case 223: _LL5293: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5312=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5312[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5313; _temp5313.tag= Cyc_Stmt_tok_tag; _temp5313.f1= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5314= yyls; struct Cyc_Yyltype*
_temp5316= _temp5314.curr + yylsp_offset; if( _temp5314.base == 0? 1:( _temp5316
< _temp5314.base? 1: _temp5316 >= _temp5314.last_plus_one)){ _throw(
Null_Exception);}* _temp5316;}).first_line,({ struct _tagged_ptr3 _temp5317=
yyls; struct Cyc_Yyltype* _temp5319= _temp5317.curr + yylsp_offset; if(
_temp5317.base == 0? 1:( _temp5319 < _temp5317.base? 1: _temp5319 >= _temp5317.last_plus_one)){
_throw( Null_Exception);}* _temp5319;}).last_line)); _temp5313;}); _temp5312;});
break; case 224: _LL5311: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5321=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5321[ 0]=({ struct Cyc_Stmt_tok_struct _temp5322; _temp5322.tag= Cyc_Stmt_tok_tag;
_temp5322.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5323= yyvs; struct _xtunion_struct** _temp5325= _temp5323.curr +(
yyvsp_offset - 1); if( _temp5323.base == 0? 1:( _temp5325 < _temp5323.base? 1:
_temp5325 >= _temp5323.last_plus_one)){ _throw( Null_Exception);}* _temp5325;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5326= yyls; struct Cyc_Yyltype*
_temp5328= _temp5326.curr +( yylsp_offset - 1); if( _temp5326.base == 0? 1:(
_temp5328 < _temp5326.base? 1: _temp5328 >= _temp5326.last_plus_one)){ _throw(
Null_Exception);}* _temp5328;}).first_line,({ struct _tagged_ptr3 _temp5329=
yyls; struct Cyc_Yyltype* _temp5331= _temp5329.curr + yylsp_offset; if(
_temp5329.base == 0? 1:( _temp5331 < _temp5329.base? 1: _temp5331 >= _temp5329.last_plus_one)){
_throw( Null_Exception);}* _temp5331;}).last_line)); _temp5322;}); _temp5321;});
break; case 225: _LL5320: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5333=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5333[ 0]=({ struct Cyc_Stmt_tok_struct _temp5334; _temp5334.tag= Cyc_Stmt_tok_tag;
_temp5334.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5335= yyls; struct Cyc_Yyltype* _temp5337= _temp5335.curr +(
yylsp_offset - 1); if( _temp5335.base == 0? 1:( _temp5337 < _temp5335.base? 1:
_temp5337 >= _temp5335.last_plus_one)){ _throw( Null_Exception);}* _temp5337;}).first_line,({
struct _tagged_ptr3 _temp5338= yyls; struct Cyc_Yyltype* _temp5340= _temp5338.curr
+ yylsp_offset; if( _temp5338.base == 0? 1:( _temp5340 < _temp5338.base? 1:
_temp5340 >= _temp5338.last_plus_one)){ _throw( Null_Exception);}* _temp5340;}).last_line));
_temp5334;}); _temp5333;}); break; case 226: _LL5332: yyval=({ struct
_tagged_ptr2 _temp5342= yyvs; struct _xtunion_struct** _temp5344= _temp5342.curr
+( yyvsp_offset - 1); if( _temp5342.base == 0? 1:( _temp5344 < _temp5342.base? 1:
_temp5344 >= _temp5342.last_plus_one)){ _throw( Null_Exception);}* _temp5344;});
break; case 227: _LL5341:{ void* _temp5349= Cyc_yyget_BlockItem_tok(({ struct
_tagged_ptr2 _temp5346= yyvs; struct _xtunion_struct** _temp5348= _temp5346.curr
+ yyvsp_offset; if( _temp5346.base == 0? 1:( _temp5348 < _temp5346.base? 1:
_temp5348 >= _temp5346.last_plus_one)){ _throw( Null_Exception);}* _temp5348;}));
struct Cyc_List_List* _temp5357; struct Cyc_Absyn_Fndecl* _temp5359; struct Cyc_Absyn_Stmt*
_temp5361; _LL5351: if((( struct _tunion_struct*) _temp5349)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL5358: _temp5357=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp5349)->f1; goto _LL5352;} else{ goto _LL5353;} _LL5353: if((( struct
_tunion_struct*) _temp5349)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL5360: _temp5359=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp5349)->f1;
goto _LL5354;} else{ goto _LL5355;} _LL5355: if((( struct _tunion_struct*)
_temp5349)->tag == Cyc_Parse_Stmt_bl_tag){ _LL5362: _temp5361=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp5349)->f1; goto _LL5356;} else{ goto
_LL5350;} _LL5352: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5363=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5363[ 0]=({ struct Cyc_Stmt_tok_struct _temp5364; _temp5364.tag= Cyc_Stmt_tok_tag;
_temp5364.f1= Cyc_Parse_flatten_declarations( _temp5357, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5365= yyls; struct Cyc_Yyltype*
_temp5367= _temp5365.curr + yylsp_offset; if( _temp5365.base == 0? 1:( _temp5367
< _temp5365.base? 1: _temp5367 >= _temp5365.last_plus_one)){ _throw(
Null_Exception);}* _temp5367;}).first_line,({ struct _tagged_ptr3 _temp5368=
yyls; struct Cyc_Yyltype* _temp5370= _temp5368.curr + yylsp_offset; if(
_temp5368.base == 0? 1:( _temp5370 < _temp5368.base? 1: _temp5370 >= _temp5368.last_plus_one)){
_throw( Null_Exception);}* _temp5370;}).last_line))); _temp5364;}); _temp5363;});
goto _LL5350; _LL5354: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5371=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5371[ 0]=({ struct Cyc_Stmt_tok_struct _temp5372; _temp5372.tag= Cyc_Stmt_tok_tag;
_temp5372.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp5373=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5373[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5374; _temp5374.tag= Cyc_Absyn_Fn_d_tag;
_temp5374.f1= _temp5359; _temp5374;}); _temp5373;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5375= yyls; struct Cyc_Yyltype* _temp5377= _temp5375.curr
+ yylsp_offset; if( _temp5375.base == 0? 1:( _temp5377 < _temp5375.base? 1:
_temp5377 >= _temp5375.last_plus_one)){ _throw( Null_Exception);}* _temp5377;}).first_line,({
struct _tagged_ptr3 _temp5378= yyls; struct Cyc_Yyltype* _temp5380= _temp5378.curr
+ yylsp_offset; if( _temp5378.base == 0? 1:( _temp5380 < _temp5378.base? 1:
_temp5380 >= _temp5378.last_plus_one)){ _throw( Null_Exception);}* _temp5380;}).last_line)),
Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5381= yyls; struct Cyc_Yyltype* _temp5383= _temp5381.curr + yylsp_offset;
if( _temp5381.base == 0? 1:( _temp5383 < _temp5381.base? 1: _temp5383 >=
_temp5381.last_plus_one)){ _throw( Null_Exception);}* _temp5383;}).first_line,({
struct _tagged_ptr3 _temp5384= yyls; struct Cyc_Yyltype* _temp5386= _temp5384.curr
+ yylsp_offset; if( _temp5384.base == 0? 1:( _temp5386 < _temp5384.base? 1:
_temp5386 >= _temp5384.last_plus_one)){ _throw( Null_Exception);}* _temp5386;}).last_line)));
_temp5372;}); _temp5371;}); goto _LL5350; _LL5356: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5387=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5387[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5388; _temp5388.tag= Cyc_Stmt_tok_tag; _temp5388.f1= _temp5361; _temp5388;});
_temp5387;}); goto _LL5350; _LL5350:;} break; case 228: _LL5345:{ void*
_temp5393= Cyc_yyget_BlockItem_tok(({ struct _tagged_ptr2 _temp5390= yyvs;
struct _xtunion_struct** _temp5392= _temp5390.curr +( yyvsp_offset - 1); if(
_temp5390.base == 0? 1:( _temp5392 < _temp5390.base? 1: _temp5392 >= _temp5390.last_plus_one)){
_throw( Null_Exception);}* _temp5392;})); struct Cyc_List_List* _temp5401;
struct Cyc_Absyn_Fndecl* _temp5403; struct Cyc_Absyn_Stmt* _temp5405; _LL5395:
if((( struct _tunion_struct*) _temp5393)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL5402: _temp5401=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp5393)->f1; goto _LL5396;} else{ goto _LL5397;} _LL5397: if((( struct
_tunion_struct*) _temp5393)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL5404: _temp5403=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp5393)->f1;
goto _LL5398;} else{ goto _LL5399;} _LL5399: if((( struct _tunion_struct*)
_temp5393)->tag == Cyc_Parse_Stmt_bl_tag){ _LL5406: _temp5405=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp5393)->f1; goto _LL5400;} else{ goto
_LL5394;} _LL5396: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5407=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5407[ 0]=({ struct Cyc_Stmt_tok_struct _temp5408; _temp5408.tag= Cyc_Stmt_tok_tag;
_temp5408.f1= Cyc_Parse_flatten_declarations( _temp5401, Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5409= yyvs; struct _xtunion_struct** _temp5411=
_temp5409.curr + yyvsp_offset; if( _temp5409.base == 0? 1:( _temp5411 <
_temp5409.base? 1: _temp5411 >= _temp5409.last_plus_one)){ _throw(
Null_Exception);}* _temp5411;}))); _temp5408;}); _temp5407;}); goto _LL5394;
_LL5398: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5412=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5412[ 0]=({ struct Cyc_Stmt_tok_struct _temp5413; _temp5413.tag= Cyc_Stmt_tok_tag;
_temp5413.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp5414=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5414[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5415; _temp5415.tag= Cyc_Absyn_Fn_d_tag;
_temp5415.f1= _temp5403; _temp5415;}); _temp5414;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5416= yyls; struct Cyc_Yyltype* _temp5418= _temp5416.curr
+( yylsp_offset - 1); if( _temp5416.base == 0? 1:( _temp5418 < _temp5416.base? 1:
_temp5418 >= _temp5416.last_plus_one)){ _throw( Null_Exception);}* _temp5418;}).first_line,({
struct _tagged_ptr3 _temp5419= yyls; struct Cyc_Yyltype* _temp5421= _temp5419.curr
+( yylsp_offset - 1); if( _temp5419.base == 0? 1:( _temp5421 < _temp5419.base? 1:
_temp5421 >= _temp5419.last_plus_one)){ _throw( Null_Exception);}* _temp5421;}).last_line)),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5422= yyvs; struct
_xtunion_struct** _temp5424= _temp5422.curr + yyvsp_offset; if( _temp5422.base
== 0? 1:( _temp5424 < _temp5422.base? 1: _temp5424 >= _temp5422.last_plus_one)){
_throw( Null_Exception);}* _temp5424;}))); _temp5413;}); _temp5412;}); goto
_LL5394; _LL5400: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5425=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5425[ 0]=({ struct Cyc_Stmt_tok_struct _temp5426; _temp5426.tag= Cyc_Stmt_tok_tag;
_temp5426.f1= Cyc_Absyn_seq_stmt( _temp5405, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5427= yyvs; struct _xtunion_struct** _temp5429= _temp5427.curr
+ yyvsp_offset; if( _temp5427.base == 0? 1:( _temp5429 < _temp5427.base? 1:
_temp5429 >= _temp5427.last_plus_one)){ _throw( Null_Exception);}* _temp5429;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5430= yyls; struct Cyc_Yyltype*
_temp5432= _temp5430.curr +( yylsp_offset - 1); if( _temp5430.base == 0? 1:(
_temp5432 < _temp5430.base? 1: _temp5432 >= _temp5430.last_plus_one)){ _throw(
Null_Exception);}* _temp5432;}).first_line,({ struct _tagged_ptr3 _temp5433=
yyls; struct Cyc_Yyltype* _temp5435= _temp5433.curr + yylsp_offset; if(
_temp5433.base == 0? 1:( _temp5435 < _temp5433.base? 1: _temp5435 >= _temp5433.last_plus_one)){
_throw( Null_Exception);}* _temp5435;}).last_line)); _temp5426;}); _temp5425;});
goto _LL5394; _LL5394:;} break; case 229: _LL5389: yyval=( struct
_xtunion_struct*)({ struct Cyc_BlockItem_tok_struct* _temp5437=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp5437[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp5438; _temp5438.tag= Cyc_BlockItem_tok_tag;
_temp5438.f1=( void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp5439=(
struct Cyc_Parse_TopDecls_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct));
_temp5439[ 0]=({ struct Cyc_Parse_TopDecls_bl_struct _temp5440; _temp5440.tag=
Cyc_Parse_TopDecls_bl_tag; _temp5440.f1= Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp5441= yyvs; struct _xtunion_struct** _temp5443= _temp5441.curr
+ yyvsp_offset; if( _temp5441.base == 0? 1:( _temp5443 < _temp5441.base? 1:
_temp5443 >= _temp5441.last_plus_one)){ _throw( Null_Exception);}* _temp5443;}));
_temp5440;}); _temp5439;})); _temp5438;}); _temp5437;}); break; case 230:
_LL5436: yyval=( struct _xtunion_struct*)({ struct Cyc_BlockItem_tok_struct*
_temp5445=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp5445[ 0]=({ struct Cyc_BlockItem_tok_struct _temp5446; _temp5446.tag= Cyc_BlockItem_tok_tag;
_temp5446.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp5447=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp5447[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp5448; _temp5448.tag= Cyc_Parse_Stmt_bl_tag;
_temp5448.f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5449= yyvs; struct
_xtunion_struct** _temp5451= _temp5449.curr + yyvsp_offset; if( _temp5449.base
== 0? 1:( _temp5451 < _temp5449.base? 1: _temp5451 >= _temp5449.last_plus_one)){
_throw( Null_Exception);}* _temp5451;})); _temp5448;}); _temp5447;})); _temp5446;});
_temp5445;}); break; case 231: _LL5444: yyval=( struct _xtunion_struct*)({
struct Cyc_BlockItem_tok_struct* _temp5453=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp5453[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp5454; _temp5454.tag= Cyc_BlockItem_tok_tag;
_temp5454.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp5455=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp5455[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp5456; _temp5456.tag= Cyc_Parse_FnDecl_bl_tag;
_temp5456.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp5457= yyvs;
struct _xtunion_struct** _temp5459= _temp5457.curr + yyvsp_offset; if( _temp5457.base
== 0? 1:( _temp5459 < _temp5457.base? 1: _temp5459 >= _temp5457.last_plus_one)){
_throw( Null_Exception);}* _temp5459;})); _temp5456;}); _temp5455;})); _temp5454;});
_temp5453;}); break; case 232: _LL5452: yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5461=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5461[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5462; _temp5462.tag= Cyc_Stmt_tok_tag; _temp5462.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5463= yyvs; struct _xtunion_struct**
_temp5465= _temp5463.curr +( yyvsp_offset - 2); if( _temp5463.base == 0? 1:(
_temp5465 < _temp5463.base? 1: _temp5465 >= _temp5463.last_plus_one)){ _throw(
Null_Exception);}* _temp5465;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5466= yyvs; struct _xtunion_struct** _temp5468= _temp5466.curr +
yyvsp_offset; if( _temp5466.base == 0? 1:( _temp5468 < _temp5466.base? 1:
_temp5468 >= _temp5466.last_plus_one)){ _throw( Null_Exception);}* _temp5468;})),
Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5469= yyls; struct Cyc_Yyltype* _temp5471= _temp5469.curr +( yylsp_offset -
4); if( _temp5469.base == 0? 1:( _temp5471 < _temp5469.base? 1: _temp5471 >=
_temp5469.last_plus_one)){ _throw( Null_Exception);}* _temp5471;}).first_line,({
struct _tagged_ptr3 _temp5472= yyls; struct Cyc_Yyltype* _temp5474= _temp5472.curr
+ yylsp_offset; if( _temp5472.base == 0? 1:( _temp5474 < _temp5472.base? 1:
_temp5474 >= _temp5472.last_plus_one)){ _throw( Null_Exception);}* _temp5474;}).last_line));
_temp5462;}); _temp5461;}); break; case 233: _LL5460: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5476=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5476[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5477; _temp5477.tag= Cyc_Stmt_tok_tag; _temp5477.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5478= yyvs; struct _xtunion_struct**
_temp5480= _temp5478.curr +( yyvsp_offset - 4); if( _temp5478.base == 0? 1:(
_temp5480 < _temp5478.base? 1: _temp5480 >= _temp5478.last_plus_one)){ _throw(
Null_Exception);}* _temp5480;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5481= yyvs; struct _xtunion_struct** _temp5483= _temp5481.curr +(
yyvsp_offset - 2); if( _temp5481.base == 0? 1:( _temp5483 < _temp5481.base? 1:
_temp5483 >= _temp5481.last_plus_one)){ _throw( Null_Exception);}* _temp5483;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5484= yyvs; struct
_xtunion_struct** _temp5486= _temp5484.curr + yyvsp_offset; if( _temp5484.base
== 0? 1:( _temp5486 < _temp5484.base? 1: _temp5486 >= _temp5484.last_plus_one)){
_throw( Null_Exception);}* _temp5486;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5487= yyls; struct Cyc_Yyltype* _temp5489= _temp5487.curr +(
yylsp_offset - 6); if( _temp5487.base == 0? 1:( _temp5489 < _temp5487.base? 1:
_temp5489 >= _temp5487.last_plus_one)){ _throw( Null_Exception);}* _temp5489;}).first_line,({
struct _tagged_ptr3 _temp5490= yyls; struct Cyc_Yyltype* _temp5492= _temp5490.curr
+ yylsp_offset; if( _temp5490.base == 0? 1:( _temp5492 < _temp5490.base? 1:
_temp5492 >= _temp5490.last_plus_one)){ _throw( Null_Exception);}* _temp5492;}).last_line));
_temp5477;}); _temp5476;}); break; case 234: _LL5475: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5494=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5494[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5495; _temp5495.tag= Cyc_Stmt_tok_tag; _temp5495.f1= Cyc_Absyn_switch_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5496= yyvs; struct _xtunion_struct**
_temp5498= _temp5496.curr +( yyvsp_offset - 4); if( _temp5496.base == 0? 1:(
_temp5498 < _temp5496.base? 1: _temp5498 >= _temp5496.last_plus_one)){ _throw(
Null_Exception);}* _temp5498;})), Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5499= yyvs; struct _xtunion_struct** _temp5501= _temp5499.curr
+( yyvsp_offset - 1); if( _temp5499.base == 0? 1:( _temp5501 < _temp5499.base? 1:
_temp5501 >= _temp5499.last_plus_one)){ _throw( Null_Exception);}* _temp5501;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5502= yyls; struct Cyc_Yyltype*
_temp5504= _temp5502.curr +( yylsp_offset - 6); if( _temp5502.base == 0? 1:(
_temp5504 < _temp5502.base? 1: _temp5504 >= _temp5502.last_plus_one)){ _throw(
Null_Exception);}* _temp5504;}).first_line,({ struct _tagged_ptr3 _temp5505=
yyls; struct Cyc_Yyltype* _temp5507= _temp5505.curr + yylsp_offset; if(
_temp5505.base == 0? 1:( _temp5507 < _temp5505.base? 1: _temp5507 >= _temp5505.last_plus_one)){
_throw( Null_Exception);}* _temp5507;}).last_line)); _temp5495;}); _temp5494;});
break; case 235: _LL5493: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5509=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5509[ 0]=({ struct Cyc_Stmt_tok_struct _temp5510; _temp5510.tag= Cyc_Stmt_tok_tag;
_temp5510.f1= Cyc_Absyn_trycatch_stmt( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5511= yyvs; struct _xtunion_struct** _temp5513= _temp5511.curr +(
yyvsp_offset - 4); if( _temp5511.base == 0? 1:( _temp5513 < _temp5511.base? 1:
_temp5513 >= _temp5511.last_plus_one)){ _throw( Null_Exception);}* _temp5513;})),
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp5514= yyvs; struct
_xtunion_struct** _temp5516= _temp5514.curr +( yyvsp_offset - 1); if( _temp5514.base
== 0? 1:( _temp5516 < _temp5514.base? 1: _temp5516 >= _temp5514.last_plus_one)){
_throw( Null_Exception);}* _temp5516;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5517= yyls; struct Cyc_Yyltype* _temp5519= _temp5517.curr +(
yylsp_offset - 5); if( _temp5517.base == 0? 1:( _temp5519 < _temp5517.base? 1:
_temp5519 >= _temp5517.last_plus_one)){ _throw( Null_Exception);}* _temp5519;}).first_line,({
struct _tagged_ptr3 _temp5520= yyls; struct Cyc_Yyltype* _temp5522= _temp5520.curr
+ yylsp_offset; if( _temp5520.base == 0? 1:( _temp5522 < _temp5520.base? 1:
_temp5522 >= _temp5520.last_plus_one)){ _throw( Null_Exception);}* _temp5522;}).last_line));
_temp5510;}); _temp5509;}); break; case 236: _LL5508: yyval=( struct
_xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5524=( struct
Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5524[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5525; _temp5525.tag=
Cyc_SwitchClauseList_tok_tag; _temp5525.f1= 0; _temp5525;}); _temp5524;});
break; case 237: _LL5523: yyval=( struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp5527=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp5527[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp5528; _temp5528.tag= Cyc_SwitchClauseList_tok_tag; _temp5528.f1=({ struct
Cyc_List_List* _temp5529=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5529->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp5530=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5530->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5540= yyls; struct Cyc_Yyltype* _temp5542= _temp5540.curr +( yylsp_offset -
2); if( _temp5540.base == 0? 1:( _temp5542 < _temp5540.base? 1: _temp5542 >=
_temp5540.last_plus_one)){ _throw( Null_Exception);}* _temp5542;}).first_line,({
struct _tagged_ptr3 _temp5543= yyls; struct Cyc_Yyltype* _temp5545= _temp5543.curr
+( yylsp_offset - 2); if( _temp5543.base == 0? 1:( _temp5545 < _temp5543.base? 1:
_temp5545 >= _temp5543.last_plus_one)){ _throw( Null_Exception);}* _temp5545;}).last_line));
_temp5530->pat_vars= 0; _temp5530->where_clause= 0; _temp5530->body= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5537= yyvs; struct _xtunion_struct** _temp5539=
_temp5537.curr + yyvsp_offset; if( _temp5537.base == 0? 1:( _temp5539 <
_temp5537.base? 1: _temp5539 >= _temp5537.last_plus_one)){ _throw(
Null_Exception);}* _temp5539;})); _temp5530->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5531= yyls; struct Cyc_Yyltype* _temp5533= _temp5531.curr
+( yylsp_offset - 2); if( _temp5531.base == 0? 1:( _temp5533 < _temp5531.base? 1:
_temp5533 >= _temp5531.last_plus_one)){ _throw( Null_Exception);}* _temp5533;}).first_line,({
struct _tagged_ptr3 _temp5534= yyls; struct Cyc_Yyltype* _temp5536= _temp5534.curr
+ yylsp_offset; if( _temp5534.base == 0? 1:( _temp5536 < _temp5534.base? 1:
_temp5536 >= _temp5534.last_plus_one)){ _throw( Null_Exception);}* _temp5536;}).last_line);
_temp5530;}); _temp5529->tl= 0; _temp5529;}); _temp5528;}); _temp5527;}); break;
case 238: _LL5526: yyval=( struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp5547=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp5547[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp5548; _temp5548.tag= Cyc_SwitchClauseList_tok_tag; _temp5548.f1=({ struct
Cyc_List_List* _temp5549=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5549->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp5553=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5553->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5566= yyvs; struct _xtunion_struct** _temp5568=
_temp5566.curr +( yyvsp_offset - 2); if( _temp5566.base == 0? 1:( _temp5568 <
_temp5566.base? 1: _temp5568 >= _temp5566.last_plus_one)){ _throw(
Null_Exception);}* _temp5568;})); _temp5553->pat_vars= 0; _temp5553->where_clause=
0; _temp5553->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5560= yyls; struct Cyc_Yyltype* _temp5562= _temp5560.curr +(
yylsp_offset - 1); if( _temp5560.base == 0? 1:( _temp5562 < _temp5560.base? 1:
_temp5562 >= _temp5560.last_plus_one)){ _throw( Null_Exception);}* _temp5562;}).first_line,({
struct _tagged_ptr3 _temp5563= yyls; struct Cyc_Yyltype* _temp5565= _temp5563.curr
+( yylsp_offset - 1); if( _temp5563.base == 0? 1:( _temp5565 < _temp5563.base? 1:
_temp5565 >= _temp5563.last_plus_one)){ _throw( Null_Exception);}* _temp5565;}).last_line));
_temp5553->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5554=
yyls; struct Cyc_Yyltype* _temp5556= _temp5554.curr +( yylsp_offset - 3); if(
_temp5554.base == 0? 1:( _temp5556 < _temp5554.base? 1: _temp5556 >= _temp5554.last_plus_one)){
_throw( Null_Exception);}* _temp5556;}).first_line,({ struct _tagged_ptr3
_temp5557= yyls; struct Cyc_Yyltype* _temp5559= _temp5557.curr + yylsp_offset;
if( _temp5557.base == 0? 1:( _temp5559 < _temp5557.base? 1: _temp5559 >=
_temp5557.last_plus_one)){ _throw( Null_Exception);}* _temp5559;}).last_line);
_temp5553;}); _temp5549->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5550= yyvs; struct _xtunion_struct** _temp5552= _temp5550.curr
+ yyvsp_offset; if( _temp5550.base == 0? 1:( _temp5552 < _temp5550.base? 1:
_temp5552 >= _temp5550.last_plus_one)){ _throw( Null_Exception);}* _temp5552;}));
_temp5549;}); _temp5548;}); _temp5547;}); break; case 239: _LL5546: yyval=(
struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5570=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5570[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5571; _temp5571.tag=
Cyc_SwitchClauseList_tok_tag; _temp5571.f1=({ struct Cyc_List_List* _temp5572=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5572->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5576=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5576->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5586= yyvs; struct _xtunion_struct** _temp5588=
_temp5586.curr +( yyvsp_offset - 3); if( _temp5586.base == 0? 1:( _temp5588 <
_temp5586.base? 1: _temp5588 >= _temp5586.last_plus_one)){ _throw(
Null_Exception);}* _temp5588;})); _temp5576->pat_vars= 0; _temp5576->where_clause=
0; _temp5576->body= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5583= yyvs;
struct _xtunion_struct** _temp5585= _temp5583.curr +( yyvsp_offset - 1); if(
_temp5583.base == 0? 1:( _temp5585 < _temp5583.base? 1: _temp5585 >= _temp5583.last_plus_one)){
_throw( Null_Exception);}* _temp5585;})); _temp5576->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5577= yyls; struct Cyc_Yyltype* _temp5579= _temp5577.curr
+( yylsp_offset - 4); if( _temp5577.base == 0? 1:( _temp5579 < _temp5577.base? 1:
_temp5579 >= _temp5577.last_plus_one)){ _throw( Null_Exception);}* _temp5579;}).first_line,({
struct _tagged_ptr3 _temp5580= yyls; struct Cyc_Yyltype* _temp5582= _temp5580.curr
+( yylsp_offset - 1); if( _temp5580.base == 0? 1:( _temp5582 < _temp5580.base? 1:
_temp5582 >= _temp5580.last_plus_one)){ _throw( Null_Exception);}* _temp5582;}).last_line);
_temp5576;}); _temp5572->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5573= yyvs; struct _xtunion_struct** _temp5575= _temp5573.curr
+ yyvsp_offset; if( _temp5573.base == 0? 1:( _temp5575 < _temp5573.base? 1:
_temp5575 >= _temp5573.last_plus_one)){ _throw( Null_Exception);}* _temp5575;}));
_temp5572;}); _temp5571;}); _temp5570;}); break; case 240: _LL5569: yyval=(
struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5590=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5590[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5591; _temp5591.tag=
Cyc_SwitchClauseList_tok_tag; _temp5591.f1=({ struct Cyc_List_List* _temp5592=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5592->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5596=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5596->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5612= yyvs; struct _xtunion_struct** _temp5614=
_temp5612.curr +( yyvsp_offset - 4); if( _temp5612.base == 0? 1:( _temp5614 <
_temp5612.base? 1: _temp5614 >= _temp5612.last_plus_one)){ _throw(
Null_Exception);}* _temp5614;})); _temp5596->pat_vars= 0; _temp5596->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5609= yyvs;
struct _xtunion_struct** _temp5611= _temp5609.curr +( yyvsp_offset - 2); if(
_temp5609.base == 0? 1:( _temp5611 < _temp5609.base? 1: _temp5611 >= _temp5609.last_plus_one)){
_throw( Null_Exception);}* _temp5611;})); _temp5596->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5603= yyls; struct Cyc_Yyltype*
_temp5605= _temp5603.curr +( yylsp_offset - 1); if( _temp5603.base == 0? 1:(
_temp5605 < _temp5603.base? 1: _temp5605 >= _temp5603.last_plus_one)){ _throw(
Null_Exception);}* _temp5605;}).first_line,({ struct _tagged_ptr3 _temp5606=
yyls; struct Cyc_Yyltype* _temp5608= _temp5606.curr +( yylsp_offset - 1); if(
_temp5606.base == 0? 1:( _temp5608 < _temp5606.base? 1: _temp5608 >= _temp5606.last_plus_one)){
_throw( Null_Exception);}* _temp5608;}).last_line)); _temp5596->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5597= yyls; struct Cyc_Yyltype* _temp5599= _temp5597.curr
+( yylsp_offset - 5); if( _temp5597.base == 0? 1:( _temp5599 < _temp5597.base? 1:
_temp5599 >= _temp5597.last_plus_one)){ _throw( Null_Exception);}* _temp5599;}).first_line,({
struct _tagged_ptr3 _temp5600= yyls; struct Cyc_Yyltype* _temp5602= _temp5600.curr
+ yylsp_offset; if( _temp5600.base == 0? 1:( _temp5602 < _temp5600.base? 1:
_temp5602 >= _temp5600.last_plus_one)){ _throw( Null_Exception);}* _temp5602;}).last_line);
_temp5596;}); _temp5592->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5593= yyvs; struct _xtunion_struct** _temp5595= _temp5593.curr
+ yyvsp_offset; if( _temp5593.base == 0? 1:( _temp5595 < _temp5593.base? 1:
_temp5595 >= _temp5593.last_plus_one)){ _throw( Null_Exception);}* _temp5595;}));
_temp5592;}); _temp5591;}); _temp5590;}); break; case 241: _LL5589: yyval=(
struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5616=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5616[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5617; _temp5617.tag=
Cyc_SwitchClauseList_tok_tag; _temp5617.f1=({ struct Cyc_List_List* _temp5618=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5618->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5622=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5622->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5635= yyvs; struct _xtunion_struct** _temp5637=
_temp5635.curr +( yyvsp_offset - 5); if( _temp5635.base == 0? 1:( _temp5637 <
_temp5635.base? 1: _temp5637 >= _temp5635.last_plus_one)){ _throw(
Null_Exception);}* _temp5637;})); _temp5622->pat_vars= 0; _temp5622->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5632= yyvs;
struct _xtunion_struct** _temp5634= _temp5632.curr +( yyvsp_offset - 3); if(
_temp5632.base == 0? 1:( _temp5634 < _temp5632.base? 1: _temp5634 >= _temp5632.last_plus_one)){
_throw( Null_Exception);}* _temp5634;})); _temp5622->body= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5629= yyvs; struct _xtunion_struct** _temp5631=
_temp5629.curr +( yyvsp_offset - 1); if( _temp5629.base == 0? 1:( _temp5631 <
_temp5629.base? 1: _temp5631 >= _temp5629.last_plus_one)){ _throw(
Null_Exception);}* _temp5631;})); _temp5622->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5623= yyls; struct Cyc_Yyltype* _temp5625= _temp5623.curr
+( yylsp_offset - 6); if( _temp5623.base == 0? 1:( _temp5625 < _temp5623.base? 1:
_temp5625 >= _temp5623.last_plus_one)){ _throw( Null_Exception);}* _temp5625;}).first_line,({
struct _tagged_ptr3 _temp5626= yyls; struct Cyc_Yyltype* _temp5628= _temp5626.curr
+ yylsp_offset; if( _temp5626.base == 0? 1:( _temp5628 < _temp5626.base? 1:
_temp5628 >= _temp5626.last_plus_one)){ _throw( Null_Exception);}* _temp5628;}).last_line);
_temp5622;}); _temp5618->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5619= yyvs; struct _xtunion_struct** _temp5621= _temp5619.curr
+ yyvsp_offset; if( _temp5619.base == 0? 1:( _temp5621 < _temp5619.base? 1:
_temp5621 >= _temp5619.last_plus_one)){ _throw( Null_Exception);}* _temp5621;}));
_temp5618;}); _temp5617;}); _temp5616;}); break; case 242: _LL5615: yyval=(
struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5639=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5639[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5640; _temp5640.tag= Cyc_Stmt_tok_tag; _temp5640.f1= Cyc_Absyn_while_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5641= yyvs; struct _xtunion_struct**
_temp5643= _temp5641.curr +( yyvsp_offset - 2); if( _temp5641.base == 0? 1:(
_temp5643 < _temp5641.base? 1: _temp5643 >= _temp5641.last_plus_one)){ _throw(
Null_Exception);}* _temp5643;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5644= yyvs; struct _xtunion_struct** _temp5646= _temp5644.curr +
yyvsp_offset; if( _temp5644.base == 0? 1:( _temp5646 < _temp5644.base? 1:
_temp5646 >= _temp5644.last_plus_one)){ _throw( Null_Exception);}* _temp5646;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5647= yyls; struct Cyc_Yyltype*
_temp5649= _temp5647.curr +( yylsp_offset - 4); if( _temp5647.base == 0? 1:(
_temp5649 < _temp5647.base? 1: _temp5649 >= _temp5647.last_plus_one)){ _throw(
Null_Exception);}* _temp5649;}).first_line,({ struct _tagged_ptr3 _temp5650=
yyls; struct Cyc_Yyltype* _temp5652= _temp5650.curr + yylsp_offset; if(
_temp5650.base == 0? 1:( _temp5652 < _temp5650.base? 1: _temp5652 >= _temp5650.last_plus_one)){
_throw( Null_Exception);}* _temp5652;}).last_line)); _temp5640;}); _temp5639;});
break; case 243: _LL5638: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5654=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5654[ 0]=({ struct Cyc_Stmt_tok_struct _temp5655; _temp5655.tag= Cyc_Stmt_tok_tag;
_temp5655.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5656= yyvs; struct _xtunion_struct** _temp5658= _temp5656.curr +(
yyvsp_offset - 5); if( _temp5656.base == 0? 1:( _temp5658 < _temp5656.base? 1:
_temp5658 >= _temp5656.last_plus_one)){ _throw( Null_Exception);}* _temp5658;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5659= yyvs; struct _xtunion_struct**
_temp5661= _temp5659.curr +( yyvsp_offset - 2); if( _temp5659.base == 0? 1:(
_temp5661 < _temp5659.base? 1: _temp5661 >= _temp5659.last_plus_one)){ _throw(
Null_Exception);}* _temp5661;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5662= yyls; struct Cyc_Yyltype* _temp5664= _temp5662.curr +(
yylsp_offset - 6); if( _temp5662.base == 0? 1:( _temp5664 < _temp5662.base? 1:
_temp5664 >= _temp5662.last_plus_one)){ _throw( Null_Exception);}* _temp5664;}).first_line,({
struct _tagged_ptr3 _temp5665= yyls; struct Cyc_Yyltype* _temp5667= _temp5665.curr
+ yylsp_offset; if( _temp5665.base == 0? 1:( _temp5667 < _temp5665.base? 1:
_temp5667 >= _temp5665.last_plus_one)){ _throw( Null_Exception);}* _temp5667;}).last_line));
_temp5655;}); _temp5654;}); break; case 244: _LL5653: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5669=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5669[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5670; _temp5670.tag= Cyc_Stmt_tok_tag; _temp5670.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5671= yyvs; struct _xtunion_struct** _temp5673=
_temp5671.curr + yyvsp_offset; if( _temp5671.base == 0? 1:( _temp5673 <
_temp5671.base? 1: _temp5673 >= _temp5671.last_plus_one)){ _throw(
Null_Exception);}* _temp5673;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5674= yyls; struct Cyc_Yyltype* _temp5676= _temp5674.curr +(
yylsp_offset - 5); if( _temp5674.base == 0? 1:( _temp5676 < _temp5674.base? 1:
_temp5676 >= _temp5674.last_plus_one)){ _throw( Null_Exception);}* _temp5676;}).first_line,({
struct _tagged_ptr3 _temp5677= yyls; struct Cyc_Yyltype* _temp5679= _temp5677.curr
+ yylsp_offset; if( _temp5677.base == 0? 1:( _temp5679 < _temp5677.base? 1:
_temp5679 >= _temp5677.last_plus_one)){ _throw( Null_Exception);}* _temp5679;}).last_line));
_temp5670;}); _temp5669;}); break; case 245: _LL5668: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5681=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5681[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5682; _temp5682.tag= Cyc_Stmt_tok_tag; _temp5682.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5683= yyvs; struct _xtunion_struct** _temp5685= _temp5683.curr
+( yyvsp_offset - 2); if( _temp5683.base == 0? 1:( _temp5685 < _temp5683.base? 1:
_temp5685 >= _temp5683.last_plus_one)){ _throw( Null_Exception);}* _temp5685;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5686= yyvs; struct
_xtunion_struct** _temp5688= _temp5686.curr + yyvsp_offset; if( _temp5686.base
== 0? 1:( _temp5688 < _temp5686.base? 1: _temp5688 >= _temp5686.last_plus_one)){
_throw( Null_Exception);}* _temp5688;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5689= yyls; struct Cyc_Yyltype* _temp5691= _temp5689.curr +(
yylsp_offset - 6); if( _temp5689.base == 0? 1:( _temp5691 < _temp5689.base? 1:
_temp5691 >= _temp5689.last_plus_one)){ _throw( Null_Exception);}* _temp5691;}).first_line,({
struct _tagged_ptr3 _temp5692= yyls; struct Cyc_Yyltype* _temp5694= _temp5692.curr
+ yylsp_offset; if( _temp5692.base == 0? 1:( _temp5694 < _temp5692.base? 1:
_temp5694 >= _temp5692.last_plus_one)){ _throw( Null_Exception);}* _temp5694;}).last_line));
_temp5682;}); _temp5681;}); break; case 246: _LL5680: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5696=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5696[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5697; _temp5697.tag= Cyc_Stmt_tok_tag; _temp5697.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5698=
yyvs; struct _xtunion_struct** _temp5700= _temp5698.curr +( yyvsp_offset - 3);
if( _temp5698.base == 0? 1:( _temp5700 < _temp5698.base? 1: _temp5700 >=
_temp5698.last_plus_one)){ _throw( Null_Exception);}* _temp5700;})), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5701= yyvs; struct
_xtunion_struct** _temp5703= _temp5701.curr + yyvsp_offset; if( _temp5701.base
== 0? 1:( _temp5703 < _temp5701.base? 1: _temp5703 >= _temp5701.last_plus_one)){
_throw( Null_Exception);}* _temp5703;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5704= yyls; struct Cyc_Yyltype* _temp5706= _temp5704.curr +(
yylsp_offset - 6); if( _temp5704.base == 0? 1:( _temp5706 < _temp5704.base? 1:
_temp5706 >= _temp5704.last_plus_one)){ _throw( Null_Exception);}* _temp5706;}).first_line,({
struct _tagged_ptr3 _temp5707= yyls; struct Cyc_Yyltype* _temp5709= _temp5707.curr
+ yylsp_offset; if( _temp5707.base == 0? 1:( _temp5709 < _temp5707.base? 1:
_temp5709 >= _temp5707.last_plus_one)){ _throw( Null_Exception);}* _temp5709;}).last_line));
_temp5697;}); _temp5696;}); break; case 247: _LL5695: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5711=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5711[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5712; _temp5712.tag= Cyc_Stmt_tok_tag; _temp5712.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5713=
yyvs; struct _xtunion_struct** _temp5715= _temp5713.curr +( yyvsp_offset - 4);
if( _temp5713.base == 0? 1:( _temp5715 < _temp5713.base? 1: _temp5715 >=
_temp5713.last_plus_one)){ _throw( Null_Exception);}* _temp5715;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5716= yyvs; struct _xtunion_struct** _temp5718=
_temp5716.curr +( yyvsp_offset - 2); if( _temp5716.base == 0? 1:( _temp5718 <
_temp5716.base? 1: _temp5718 >= _temp5716.last_plus_one)){ _throw(
Null_Exception);}* _temp5718;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5719= yyvs; struct _xtunion_struct** _temp5721= _temp5719.curr +
yyvsp_offset; if( _temp5719.base == 0? 1:( _temp5721 < _temp5719.base? 1:
_temp5721 >= _temp5719.last_plus_one)){ _throw( Null_Exception);}* _temp5721;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5722= yyls; struct Cyc_Yyltype*
_temp5724= _temp5722.curr +( yylsp_offset - 7); if( _temp5722.base == 0? 1:(
_temp5724 < _temp5722.base? 1: _temp5724 >= _temp5722.last_plus_one)){ _throw(
Null_Exception);}* _temp5724;}).first_line,({ struct _tagged_ptr3 _temp5725=
yyls; struct Cyc_Yyltype* _temp5727= _temp5725.curr +( yylsp_offset - 1); if(
_temp5725.base == 0? 1:( _temp5727 < _temp5725.base? 1: _temp5727 >= _temp5725.last_plus_one)){
_throw( Null_Exception);}* _temp5727;}).last_line)); _temp5712;}); _temp5711;});
break; case 248: _LL5710: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5729=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5729[ 0]=({ struct Cyc_Stmt_tok_struct _temp5730; _temp5730.tag= Cyc_Stmt_tok_tag;
_temp5730.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5731= yyvs; struct _xtunion_struct** _temp5733= _temp5731.curr +(
yyvsp_offset - 4); if( _temp5731.base == 0? 1:( _temp5733 < _temp5731.base? 1:
_temp5733 >= _temp5731.last_plus_one)){ _throw( Null_Exception);}* _temp5733;})),
Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5734= yyvs; struct _xtunion_struct** _temp5736= _temp5734.curr
+ yyvsp_offset; if( _temp5734.base == 0? 1:( _temp5736 < _temp5734.base? 1:
_temp5736 >= _temp5734.last_plus_one)){ _throw( Null_Exception);}* _temp5736;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5737= yyls; struct Cyc_Yyltype*
_temp5739= _temp5737.curr +( yylsp_offset - 6); if( _temp5737.base == 0? 1:(
_temp5739 < _temp5737.base? 1: _temp5739 >= _temp5737.last_plus_one)){ _throw(
Null_Exception);}* _temp5739;}).first_line,({ struct _tagged_ptr3 _temp5740=
yyls; struct Cyc_Yyltype* _temp5742= _temp5740.curr + yylsp_offset; if(
_temp5740.base == 0? 1:( _temp5742 < _temp5740.base? 1: _temp5742 >= _temp5740.last_plus_one)){
_throw( Null_Exception);}* _temp5742;}).last_line)); _temp5730;}); _temp5729;});
break; case 249: _LL5728: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5744=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5744[ 0]=({ struct Cyc_Stmt_tok_struct _temp5745; _temp5745.tag= Cyc_Stmt_tok_tag;
_temp5745.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5746= yyvs; struct _xtunion_struct** _temp5748= _temp5746.curr +(
yyvsp_offset - 5); if( _temp5746.base == 0? 1:( _temp5748 < _temp5746.base? 1:
_temp5748 >= _temp5746.last_plus_one)){ _throw( Null_Exception);}* _temp5748;})),
Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5749= yyvs;
struct _xtunion_struct** _temp5751= _temp5749.curr +( yyvsp_offset - 2); if(
_temp5749.base == 0? 1:( _temp5751 < _temp5749.base? 1: _temp5751 >= _temp5749.last_plus_one)){
_throw( Null_Exception);}* _temp5751;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5752= yyvs; struct _xtunion_struct** _temp5754= _temp5752.curr
+ yyvsp_offset; if( _temp5752.base == 0? 1:( _temp5754 < _temp5752.base? 1:
_temp5754 >= _temp5752.last_plus_one)){ _throw( Null_Exception);}* _temp5754;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5755= yyls; struct Cyc_Yyltype*
_temp5757= _temp5755.curr +( yylsp_offset - 7); if( _temp5755.base == 0? 1:(
_temp5757 < _temp5755.base? 1: _temp5757 >= _temp5755.last_plus_one)){ _throw(
Null_Exception);}* _temp5757;}).first_line,({ struct _tagged_ptr3 _temp5758=
yyls; struct Cyc_Yyltype* _temp5760= _temp5758.curr + yylsp_offset; if(
_temp5758.base == 0? 1:( _temp5760 < _temp5758.base? 1: _temp5760 >= _temp5758.last_plus_one)){
_throw( Null_Exception);}* _temp5760;}).last_line)); _temp5745;}); _temp5744;});
break; case 250: _LL5743: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5762=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5762[ 0]=({ struct Cyc_Stmt_tok_struct _temp5763; _temp5763.tag= Cyc_Stmt_tok_tag;
_temp5763.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5764= yyvs; struct _xtunion_struct** _temp5766= _temp5764.curr +(
yyvsp_offset - 5); if( _temp5764.base == 0? 1:( _temp5766 < _temp5764.base? 1:
_temp5766 >= _temp5764.last_plus_one)){ _throw( Null_Exception);}* _temp5766;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5767= yyvs; struct _xtunion_struct**
_temp5769= _temp5767.curr +( yyvsp_offset - 3); if( _temp5767.base == 0? 1:(
_temp5769 < _temp5767.base? 1: _temp5769 >= _temp5767.last_plus_one)){ _throw(
Null_Exception);}* _temp5769;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5770= yyvs; struct _xtunion_struct** _temp5772=
_temp5770.curr + yyvsp_offset; if( _temp5770.base == 0? 1:( _temp5772 <
_temp5770.base? 1: _temp5772 >= _temp5770.last_plus_one)){ _throw(
Null_Exception);}* _temp5772;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5773= yyls; struct Cyc_Yyltype* _temp5775= _temp5773.curr +(
yylsp_offset - 7); if( _temp5773.base == 0? 1:( _temp5775 < _temp5773.base? 1:
_temp5775 >= _temp5773.last_plus_one)){ _throw( Null_Exception);}* _temp5775;}).first_line,({
struct _tagged_ptr3 _temp5776= yyls; struct Cyc_Yyltype* _temp5778= _temp5776.curr
+ yylsp_offset; if( _temp5776.base == 0? 1:( _temp5778 < _temp5776.base? 1:
_temp5778 >= _temp5776.last_plus_one)){ _throw( Null_Exception);}* _temp5778;}).last_line));
_temp5763;}); _temp5762;}); break; case 251: _LL5761: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5780=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5780[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5781; _temp5781.tag= Cyc_Stmt_tok_tag; _temp5781.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5782= yyvs; struct _xtunion_struct**
_temp5784= _temp5782.curr +( yyvsp_offset - 6); if( _temp5782.base == 0? 1:(
_temp5784 < _temp5782.base? 1: _temp5784 >= _temp5782.last_plus_one)){ _throw(
Null_Exception);}* _temp5784;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5785= yyvs; struct _xtunion_struct** _temp5787= _temp5785.curr +(
yyvsp_offset - 4); if( _temp5785.base == 0? 1:( _temp5787 < _temp5785.base? 1:
_temp5787 >= _temp5785.last_plus_one)){ _throw( Null_Exception);}* _temp5787;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5788= yyvs; struct _xtunion_struct**
_temp5790= _temp5788.curr +( yyvsp_offset - 2); if( _temp5788.base == 0? 1:(
_temp5790 < _temp5788.base? 1: _temp5790 >= _temp5788.last_plus_one)){ _throw(
Null_Exception);}* _temp5790;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5791= yyvs; struct _xtunion_struct** _temp5793= _temp5791.curr +
yyvsp_offset; if( _temp5791.base == 0? 1:( _temp5793 < _temp5791.base? 1:
_temp5793 >= _temp5791.last_plus_one)){ _throw( Null_Exception);}* _temp5793;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5794= yyls; struct Cyc_Yyltype*
_temp5796= _temp5794.curr +( yylsp_offset - 8); if( _temp5794.base == 0? 1:(
_temp5796 < _temp5794.base? 1: _temp5796 >= _temp5794.last_plus_one)){ _throw(
Null_Exception);}* _temp5796;}).first_line,({ struct _tagged_ptr3 _temp5797=
yyls; struct Cyc_Yyltype* _temp5799= _temp5797.curr + yylsp_offset; if(
_temp5797.base == 0? 1:( _temp5799 < _temp5797.base? 1: _temp5799 >= _temp5797.last_plus_one)){
_throw( Null_Exception);}* _temp5799;}).last_line)); _temp5781;}); _temp5780;});
break; case 252: _LL5779: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5812= yyvs; struct _xtunion_struct** _temp5814=
_temp5812.curr +( yyvsp_offset - 3); if( _temp5812.base == 0? 1:( _temp5814 <
_temp5812.base? 1: _temp5814 >= _temp5812.last_plus_one)){ _throw(
Null_Exception);}* _temp5814;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5803= yyvs; struct _xtunion_struct** _temp5805=
_temp5803.curr + yyvsp_offset; if( _temp5803.base == 0? 1:( _temp5805 <
_temp5803.base? 1: _temp5805 >= _temp5803.last_plus_one)){ _throw(
Null_Exception);}* _temp5805;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5806= yyls; struct Cyc_Yyltype* _temp5808= _temp5806.curr +(
yylsp_offset - 5); if( _temp5806.base == 0? 1:( _temp5808 < _temp5806.base? 1:
_temp5808 >= _temp5806.last_plus_one)){ _throw( Null_Exception);}* _temp5808;}).first_line,({
struct _tagged_ptr3 _temp5809= yyls; struct Cyc_Yyltype* _temp5811= _temp5809.curr
+ yylsp_offset; if( _temp5809.base == 0? 1:( _temp5811 < _temp5809.base? 1:
_temp5811 >= _temp5809.last_plus_one)){ _throw( Null_Exception);}* _temp5811;}).last_line));
yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5801=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5801[ 0]=({ struct Cyc_Stmt_tok_struct _temp5802; _temp5802.tag= Cyc_Stmt_tok_tag;
_temp5802.f1= Cyc_Parse_flatten_declarations( decls, s); _temp5802;}); _temp5801;});
break;} case 253: _LL5800: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5830= yyvs; struct _xtunion_struct** _temp5832=
_temp5830.curr +( yyvsp_offset - 4); if( _temp5830.base == 0? 1:( _temp5832 <
_temp5830.base? 1: _temp5832 >= _temp5830.last_plus_one)){ _throw(
Null_Exception);}* _temp5832;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5818=
yyvs; struct _xtunion_struct** _temp5820= _temp5818.curr +( yyvsp_offset - 3);
if( _temp5818.base == 0? 1:( _temp5820 < _temp5818.base? 1: _temp5820 >=
_temp5818.last_plus_one)){ _throw( Null_Exception);}* _temp5820;})), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5821= yyvs; struct
_xtunion_struct** _temp5823= _temp5821.curr + yyvsp_offset; if( _temp5821.base
== 0? 1:( _temp5823 < _temp5821.base? 1: _temp5823 >= _temp5821.last_plus_one)){
_throw( Null_Exception);}* _temp5823;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5824= yyls; struct Cyc_Yyltype* _temp5826= _temp5824.curr +(
yylsp_offset - 6); if( _temp5824.base == 0? 1:( _temp5826 < _temp5824.base? 1:
_temp5826 >= _temp5824.last_plus_one)){ _throw( Null_Exception);}* _temp5826;}).first_line,({
struct _tagged_ptr3 _temp5827= yyls; struct Cyc_Yyltype* _temp5829= _temp5827.curr
+ yylsp_offset; if( _temp5827.base == 0? 1:( _temp5829 < _temp5827.base? 1:
_temp5829 >= _temp5827.last_plus_one)){ _throw( Null_Exception);}* _temp5829;}).last_line));
yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5816=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5816[ 0]=({ struct Cyc_Stmt_tok_struct _temp5817; _temp5817.tag= Cyc_Stmt_tok_tag;
_temp5817.f1= Cyc_Parse_flatten_declarations( decls, s); _temp5817;}); _temp5816;});
break;} case 254: _LL5815: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5848= yyvs; struct _xtunion_struct** _temp5850=
_temp5848.curr +( yyvsp_offset - 4); if( _temp5848.base == 0? 1:( _temp5850 <
_temp5848.base? 1: _temp5850 >= _temp5848.last_plus_one)){ _throw(
Null_Exception);}* _temp5850;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5836= yyvs; struct _xtunion_struct** _temp5838= _temp5836.curr
+( yyvsp_offset - 2); if( _temp5836.base == 0? 1:( _temp5838 < _temp5836.base? 1:
_temp5838 >= _temp5836.last_plus_one)){ _throw( Null_Exception);}* _temp5838;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5839= yyvs; struct
_xtunion_struct** _temp5841= _temp5839.curr + yyvsp_offset; if( _temp5839.base
== 0? 1:( _temp5841 < _temp5839.base? 1: _temp5841 >= _temp5839.last_plus_one)){
_throw( Null_Exception);}* _temp5841;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5842= yyls; struct Cyc_Yyltype* _temp5844= _temp5842.curr +(
yylsp_offset - 6); if( _temp5842.base == 0? 1:( _temp5844 < _temp5842.base? 1:
_temp5844 >= _temp5842.last_plus_one)){ _throw( Null_Exception);}* _temp5844;}).first_line,({
struct _tagged_ptr3 _temp5845= yyls; struct Cyc_Yyltype* _temp5847= _temp5845.curr
+ yylsp_offset; if( _temp5845.base == 0? 1:( _temp5847 < _temp5845.base? 1:
_temp5847 >= _temp5845.last_plus_one)){ _throw( Null_Exception);}* _temp5847;}).last_line));
yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5834=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5834[ 0]=({ struct Cyc_Stmt_tok_struct _temp5835; _temp5835.tag= Cyc_Stmt_tok_tag;
_temp5835.f1= Cyc_Parse_flatten_declarations( decls, s); _temp5835;}); _temp5834;});
break;} case 255: _LL5833: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5869= yyvs; struct _xtunion_struct** _temp5871=
_temp5869.curr +( yyvsp_offset - 5); if( _temp5869.base == 0? 1:( _temp5871 <
_temp5869.base? 1: _temp5871 >= _temp5869.last_plus_one)){ _throw(
Null_Exception);}* _temp5871;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5854=
yyvs; struct _xtunion_struct** _temp5856= _temp5854.curr +( yyvsp_offset - 4);
if( _temp5854.base == 0? 1:( _temp5856 < _temp5854.base? 1: _temp5856 >=
_temp5854.last_plus_one)){ _throw( Null_Exception);}* _temp5856;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5857= yyvs; struct _xtunion_struct** _temp5859=
_temp5857.curr +( yyvsp_offset - 2); if( _temp5857.base == 0? 1:( _temp5859 <
_temp5857.base? 1: _temp5859 >= _temp5857.last_plus_one)){ _throw(
Null_Exception);}* _temp5859;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5860= yyvs; struct _xtunion_struct** _temp5862= _temp5860.curr +
yyvsp_offset; if( _temp5860.base == 0? 1:( _temp5862 < _temp5860.base? 1:
_temp5862 >= _temp5860.last_plus_one)){ _throw( Null_Exception);}* _temp5862;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5863= yyls; struct Cyc_Yyltype*
_temp5865= _temp5863.curr +( yylsp_offset - 7); if( _temp5863.base == 0? 1:(
_temp5865 < _temp5863.base? 1: _temp5865 >= _temp5863.last_plus_one)){ _throw(
Null_Exception);}* _temp5865;}).first_line,({ struct _tagged_ptr3 _temp5866=
yyls; struct Cyc_Yyltype* _temp5868= _temp5866.curr + yylsp_offset; if(
_temp5866.base == 0? 1:( _temp5868 < _temp5866.base? 1: _temp5868 >= _temp5866.last_plus_one)){
_throw( Null_Exception);}* _temp5868;}).last_line)); yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5852=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5852[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5853; _temp5853.tag= Cyc_Stmt_tok_tag; _temp5853.f1= Cyc_Parse_flatten_declarations(
decls, s); _temp5853;}); _temp5852;}); break;} case 256: _LL5851: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5873=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5873[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5874; _temp5874.tag= Cyc_Stmt_tok_tag; _temp5874.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_string* _temp5875=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp5875[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5876= yyvs; struct _xtunion_struct** _temp5878= _temp5876.curr
+( yyvsp_offset - 1); if( _temp5876.base == 0? 1:( _temp5878 < _temp5876.base? 1:
_temp5878 >= _temp5876.last_plus_one)){ _throw( Null_Exception);}* _temp5878;}));
_temp5875;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5879= yyls;
struct Cyc_Yyltype* _temp5881= _temp5879.curr +( yylsp_offset - 2); if(
_temp5879.base == 0? 1:( _temp5881 < _temp5879.base? 1: _temp5881 >= _temp5879.last_plus_one)){
_throw( Null_Exception);}* _temp5881;}).first_line,({ struct _tagged_ptr3
_temp5882= yyls; struct Cyc_Yyltype* _temp5884= _temp5882.curr +( yylsp_offset -
1); if( _temp5882.base == 0? 1:( _temp5884 < _temp5882.base? 1: _temp5884 >=
_temp5882.last_plus_one)){ _throw( Null_Exception);}* _temp5884;}).last_line));
_temp5874;}); _temp5873;}); break; case 257: _LL5872: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5886=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5886[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5887; _temp5887.tag= Cyc_Stmt_tok_tag; _temp5887.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5888= yyls; struct Cyc_Yyltype*
_temp5890= _temp5888.curr +( yylsp_offset - 1); if( _temp5888.base == 0? 1:(
_temp5890 < _temp5888.base? 1: _temp5890 >= _temp5888.last_plus_one)){ _throw(
Null_Exception);}* _temp5890;}).first_line,({ struct _tagged_ptr3 _temp5891=
yyls; struct Cyc_Yyltype* _temp5893= _temp5891.curr +( yylsp_offset - 1); if(
_temp5891.base == 0? 1:( _temp5893 < _temp5891.base? 1: _temp5893 >= _temp5891.last_plus_one)){
_throw( Null_Exception);}* _temp5893;}).last_line)); _temp5887;}); _temp5886;});
break; case 258: _LL5885: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5895=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5895[ 0]=({ struct Cyc_Stmt_tok_struct _temp5896; _temp5896.tag= Cyc_Stmt_tok_tag;
_temp5896.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5897= yyls; struct Cyc_Yyltype* _temp5899= _temp5897.curr +(
yylsp_offset - 1); if( _temp5897.base == 0? 1:( _temp5899 < _temp5897.base? 1:
_temp5899 >= _temp5897.last_plus_one)){ _throw( Null_Exception);}* _temp5899;}).first_line,({
struct _tagged_ptr3 _temp5900= yyls; struct Cyc_Yyltype* _temp5902= _temp5900.curr
+( yylsp_offset - 1); if( _temp5900.base == 0? 1:( _temp5902 < _temp5900.base? 1:
_temp5902 >= _temp5900.last_plus_one)){ _throw( Null_Exception);}* _temp5902;}).last_line));
_temp5896;}); _temp5895;}); break; case 259: _LL5894: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5904=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5904[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5905; _temp5905.tag= Cyc_Stmt_tok_tag; _temp5905.f1= Cyc_Absyn_return_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5906= yyls; struct
Cyc_Yyltype* _temp5908= _temp5906.curr +( yylsp_offset - 1); if( _temp5906.base
== 0? 1:( _temp5908 < _temp5906.base? 1: _temp5908 >= _temp5906.last_plus_one)){
_throw( Null_Exception);}* _temp5908;}).first_line,({ struct _tagged_ptr3
_temp5909= yyls; struct Cyc_Yyltype* _temp5911= _temp5909.curr +( yylsp_offset -
1); if( _temp5909.base == 0? 1:( _temp5911 < _temp5909.base? 1: _temp5911 >=
_temp5909.last_plus_one)){ _throw( Null_Exception);}* _temp5911;}).last_line));
_temp5905;}); _temp5904;}); break; case 260: _LL5903: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5913=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5913[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5914; _temp5914.tag= Cyc_Stmt_tok_tag; _temp5914.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5915= yyvs;
struct _xtunion_struct** _temp5917= _temp5915.curr +( yyvsp_offset - 1); if(
_temp5915.base == 0? 1:( _temp5917 < _temp5915.base? 1: _temp5917 >= _temp5915.last_plus_one)){
_throw( Null_Exception);}* _temp5917;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5918= yyls; struct Cyc_Yyltype* _temp5920= _temp5918.curr +(
yylsp_offset - 2); if( _temp5918.base == 0? 1:( _temp5920 < _temp5918.base? 1:
_temp5920 >= _temp5918.last_plus_one)){ _throw( Null_Exception);}* _temp5920;}).first_line,({
struct _tagged_ptr3 _temp5921= yyls; struct Cyc_Yyltype* _temp5923= _temp5921.curr
+( yylsp_offset - 1); if( _temp5921.base == 0? 1:( _temp5923 < _temp5921.base? 1:
_temp5923 >= _temp5921.last_plus_one)){ _throw( Null_Exception);}* _temp5923;}).last_line));
_temp5914;}); _temp5913;}); break; case 261: _LL5912: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5925=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5925[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5926; _temp5926.tag= Cyc_Stmt_tok_tag; _temp5926.f1= Cyc_Absyn_fallthru_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5927= yyls; struct
Cyc_Yyltype* _temp5929= _temp5927.curr +( yylsp_offset - 1); if( _temp5927.base
== 0? 1:( _temp5929 < _temp5927.base? 1: _temp5929 >= _temp5927.last_plus_one)){
_throw( Null_Exception);}* _temp5929;}).first_line,({ struct _tagged_ptr3
_temp5930= yyls; struct Cyc_Yyltype* _temp5932= _temp5930.curr +( yylsp_offset -
1); if( _temp5930.base == 0? 1:( _temp5932 < _temp5930.base? 1: _temp5932 >=
_temp5930.last_plus_one)){ _throw( Null_Exception);}* _temp5932;}).last_line));
_temp5926;}); _temp5925;}); break; case 262: _LL5924: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5934=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5934[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5935; _temp5935.tag= Cyc_Stmt_tok_tag; _temp5935.f1= Cyc_Absyn_fallthru_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5936= yyls; struct
Cyc_Yyltype* _temp5938= _temp5936.curr +( yylsp_offset - 3); if( _temp5936.base
== 0? 1:( _temp5938 < _temp5936.base? 1: _temp5938 >= _temp5936.last_plus_one)){
_throw( Null_Exception);}* _temp5938;}).first_line,({ struct _tagged_ptr3
_temp5939= yyls; struct Cyc_Yyltype* _temp5941= _temp5939.curr +( yylsp_offset -
3); if( _temp5939.base == 0? 1:( _temp5941 < _temp5939.base? 1: _temp5941 >=
_temp5939.last_plus_one)){ _throw( Null_Exception);}* _temp5941;}).last_line));
_temp5935;}); _temp5934;}); break; case 263: _LL5933: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5943=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5943[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5944; _temp5944.tag= Cyc_Stmt_tok_tag; _temp5944.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp5945= yyvs; struct
_xtunion_struct** _temp5947= _temp5945.curr +( yyvsp_offset - 2); if( _temp5945.base
== 0? 1:( _temp5947 < _temp5945.base? 1: _temp5947 >= _temp5945.last_plus_one)){
_throw( Null_Exception);}* _temp5947;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5948= yyls; struct Cyc_Yyltype* _temp5950= _temp5948.curr +(
yylsp_offset - 4); if( _temp5948.base == 0? 1:( _temp5950 < _temp5948.base? 1:
_temp5950 >= _temp5948.last_plus_one)){ _throw( Null_Exception);}* _temp5950;}).first_line,({
struct _tagged_ptr3 _temp5951= yyls; struct Cyc_Yyltype* _temp5953= _temp5951.curr
+( yylsp_offset - 4); if( _temp5951.base == 0? 1:( _temp5953 < _temp5951.base? 1:
_temp5953 >= _temp5951.last_plus_one)){ _throw( Null_Exception);}* _temp5953;}).last_line));
_temp5944;}); _temp5943;}); break; case 264: _LL5942: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5955=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5955[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5956; _temp5956.tag= Cyc_Pattern_tok_tag; _temp5956.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5957= yyls; struct Cyc_Yyltype* _temp5959= _temp5957.curr + yylsp_offset;
if( _temp5957.base == 0? 1:( _temp5959 < _temp5957.base? 1: _temp5959 >=
_temp5957.last_plus_one)){ _throw( Null_Exception);}* _temp5959;}).first_line,({
struct _tagged_ptr3 _temp5960= yyls; struct Cyc_Yyltype* _temp5962= _temp5960.curr
+ yylsp_offset; if( _temp5960.base == 0? 1:( _temp5962 < _temp5960.base? 1:
_temp5962 >= _temp5960.last_plus_one)){ _throw( Null_Exception);}* _temp5962;}).last_line));
_temp5956;}); _temp5955;}); break; case 265: _LL5954: yyval=({ struct
_tagged_ptr2 _temp5964= yyvs; struct _xtunion_struct** _temp5966= _temp5964.curr
+( yyvsp_offset - 1); if( _temp5964.base == 0? 1:( _temp5966 < _temp5964.base? 1:
_temp5966 >= _temp5964.last_plus_one)){ _throw( Null_Exception);}* _temp5966;});
break; case 266: _LL5963: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5968=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5968[ 0]=({ struct Cyc_Pattern_tok_struct _temp5969; _temp5969.tag= Cyc_Pattern_tok_tag;
_temp5969.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp5970=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp5970[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp5971; _temp5971.tag= Cyc_Absyn_Int_p_tag;
_temp5971.f1=( void*)(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp5972= yyvs;
struct _xtunion_struct** _temp5974= _temp5972.curr + yyvsp_offset; if( _temp5972.base
== 0? 1:( _temp5974 < _temp5972.base? 1: _temp5974 >= _temp5972.last_plus_one)){
_throw( Null_Exception);}* _temp5974;}))).f1; _temp5971.f2=(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5975= yyvs; struct _xtunion_struct** _temp5977=
_temp5975.curr + yyvsp_offset; if( _temp5975.base == 0? 1:( _temp5977 <
_temp5975.base? 1: _temp5977 >= _temp5975.last_plus_one)){ _throw(
Null_Exception);}* _temp5977;}))).f2; _temp5971;}); _temp5970;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5978= yyls; struct Cyc_Yyltype* _temp5980= _temp5978.curr
+ yylsp_offset; if( _temp5978.base == 0? 1:( _temp5980 < _temp5978.base? 1:
_temp5980 >= _temp5978.last_plus_one)){ _throw( Null_Exception);}* _temp5980;}).first_line,({
struct _tagged_ptr3 _temp5981= yyls; struct Cyc_Yyltype* _temp5983= _temp5981.curr
+ yylsp_offset; if( _temp5981.base == 0? 1:( _temp5983 < _temp5981.base? 1:
_temp5983 >= _temp5981.last_plus_one)){ _throw( Null_Exception);}* _temp5983;}).last_line));
_temp5969;}); _temp5968;}); break; case 267: _LL5967: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5985=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5985[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5986; _temp5986.tag= Cyc_Pattern_tok_tag; _temp5986.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp5987=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp5987[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp5988; _temp5988.tag= Cyc_Absyn_Int_p_tag; _temp5988.f1=( void*)(( void*)
Cyc_Absyn_Signed); _temp5988.f2= -(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2
_temp5989= yyvs; struct _xtunion_struct** _temp5991= _temp5989.curr +
yyvsp_offset; if( _temp5989.base == 0? 1:( _temp5991 < _temp5989.base? 1:
_temp5991 >= _temp5989.last_plus_one)){ _throw( Null_Exception);}* _temp5991;}))).f2;
_temp5988;}); _temp5987;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5992= yyls; struct Cyc_Yyltype* _temp5994= _temp5992.curr +( yylsp_offset -
1); if( _temp5992.base == 0? 1:( _temp5994 < _temp5992.base? 1: _temp5994 >=
_temp5992.last_plus_one)){ _throw( Null_Exception);}* _temp5994;}).first_line,({
struct _tagged_ptr3 _temp5995= yyls; struct Cyc_Yyltype* _temp5997= _temp5995.curr
+ yylsp_offset; if( _temp5995.base == 0? 1:( _temp5997 < _temp5995.base? 1:
_temp5997 >= _temp5995.last_plus_one)){ _throw( Null_Exception);}* _temp5997;}).last_line));
_temp5986;}); _temp5985;}); break; case 268: _LL5984: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5999=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5999[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6000; _temp6000.tag= Cyc_Pattern_tok_tag; _temp6000.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp6001=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp6001[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp6002; _temp6002.tag= Cyc_Absyn_Float_p_tag;
_temp6002.f1= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6003= yyvs;
struct _xtunion_struct** _temp6005= _temp6003.curr + yyvsp_offset; if( _temp6003.base
== 0? 1:( _temp6005 < _temp6003.base? 1: _temp6005 >= _temp6003.last_plus_one)){
_throw( Null_Exception);}* _temp6005;})); _temp6002;}); _temp6001;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6006= yyls; struct Cyc_Yyltype* _temp6008= _temp6006.curr
+ yylsp_offset; if( _temp6006.base == 0? 1:( _temp6008 < _temp6006.base? 1:
_temp6008 >= _temp6006.last_plus_one)){ _throw( Null_Exception);}* _temp6008;}).first_line,({
struct _tagged_ptr3 _temp6009= yyls; struct Cyc_Yyltype* _temp6011= _temp6009.curr
+ yylsp_offset; if( _temp6009.base == 0? 1:( _temp6011 < _temp6009.base? 1:
_temp6011 >= _temp6009.last_plus_one)){ _throw( Null_Exception);}* _temp6011;}).last_line));
_temp6000;}); _temp5999;}); break; case 269: _LL5998: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6013=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6013[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6014; _temp6014.tag= Cyc_Pattern_tok_tag; _temp6014.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Char_p_struct* _temp6015=( struct Cyc_Absyn_Char_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_p_struct)); _temp6015[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp6016; _temp6016.tag= Cyc_Absyn_Char_p_tag; _temp6016.f1= Cyc_yyget_Char_tok(({
struct _tagged_ptr2 _temp6017= yyvs; struct _xtunion_struct** _temp6019=
_temp6017.curr + yyvsp_offset; if( _temp6017.base == 0? 1:( _temp6019 <
_temp6017.base? 1: _temp6019 >= _temp6017.last_plus_one)){ _throw(
Null_Exception);}* _temp6019;})); _temp6016;}); _temp6015;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6020= yyls; struct Cyc_Yyltype* _temp6022= _temp6020.curr
+ yylsp_offset; if( _temp6020.base == 0? 1:( _temp6022 < _temp6020.base? 1:
_temp6022 >= _temp6020.last_plus_one)){ _throw( Null_Exception);}* _temp6022;}).first_line,({
struct _tagged_ptr3 _temp6023= yyls; struct Cyc_Yyltype* _temp6025= _temp6023.curr
+ yylsp_offset; if( _temp6023.base == 0? 1:( _temp6025 < _temp6023.base? 1:
_temp6025 >= _temp6023.last_plus_one)){ _throw( Null_Exception);}* _temp6025;}).last_line));
_temp6014;}); _temp6013;}); break; case 270: _LL6012: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6027=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6027[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6028; _temp6028.tag= Cyc_Pattern_tok_tag; _temp6028.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6029= yyls; struct Cyc_Yyltype* _temp6031= _temp6029.curr + yylsp_offset;
if( _temp6029.base == 0? 1:( _temp6031 < _temp6029.base? 1: _temp6031 >=
_temp6029.last_plus_one)){ _throw( Null_Exception);}* _temp6031;}).first_line,({
struct _tagged_ptr3 _temp6032= yyls; struct Cyc_Yyltype* _temp6034= _temp6032.curr
+ yylsp_offset; if( _temp6032.base == 0? 1:( _temp6034 < _temp6032.base? 1:
_temp6034 >= _temp6032.last_plus_one)){ _throw( Null_Exception);}* _temp6034;}).last_line));
_temp6028;}); _temp6027;}); break; case 271: _LL6026: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6036=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6036[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6037; _temp6037.tag= Cyc_Pattern_tok_tag; _temp6037.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownId_p_struct* _temp6038=( struct Cyc_Absyn_UnknownId_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct)); _temp6038[ 0]=({
struct Cyc_Absyn_UnknownId_p_struct _temp6039; _temp6039.tag= Cyc_Absyn_UnknownId_p_tag;
_temp6039.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6040= yyvs;
struct _xtunion_struct** _temp6042= _temp6040.curr + yyvsp_offset; if( _temp6040.base
== 0? 1:( _temp6042 < _temp6040.base? 1: _temp6042 >= _temp6040.last_plus_one)){
_throw( Null_Exception);}* _temp6042;})); _temp6039;}); _temp6038;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6043= yyls; struct Cyc_Yyltype* _temp6045= _temp6043.curr
+ yylsp_offset; if( _temp6043.base == 0? 1:( _temp6045 < _temp6043.base? 1:
_temp6045 >= _temp6043.last_plus_one)){ _throw( Null_Exception);}* _temp6045;}).first_line,({
struct _tagged_ptr3 _temp6046= yyls; struct Cyc_Yyltype* _temp6048= _temp6046.curr
+ yylsp_offset; if( _temp6046.base == 0? 1:( _temp6048 < _temp6046.base? 1:
_temp6048 >= _temp6046.last_plus_one)){ _throw( Null_Exception);}* _temp6048;}).last_line));
_temp6037;}); _temp6036;}); break; case 272: _LL6035: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6066=
yyls; struct Cyc_Yyltype* _temp6068= _temp6066.curr +( yylsp_offset - 3); if(
_temp6066.base == 0? 1:( _temp6068 < _temp6066.base? 1: _temp6068 >= _temp6066.last_plus_one)){
_throw( Null_Exception);}* _temp6068;}).first_line,({ struct _tagged_ptr3
_temp6069= yyls; struct Cyc_Yyltype* _temp6071= _temp6069.curr +( yylsp_offset -
3); if( _temp6069.base == 0? 1:( _temp6071 < _temp6069.base? 1: _temp6071 >=
_temp6069.last_plus_one)){ _throw( Null_Exception);}* _temp6071;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6072= yyvs; struct
_xtunion_struct** _temp6074= _temp6072.curr +( yyvsp_offset - 3); if( _temp6072.base
== 0? 1:( _temp6074 < _temp6072.base? 1: _temp6074 >= _temp6072.last_plus_one)){
_throw( Null_Exception);}* _temp6074;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_Pattern_tok_struct* _temp6050=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6050[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6051; _temp6051.tag= Cyc_Pattern_tok_tag; _temp6051.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownCall_p_struct* _temp6052=( struct Cyc_Absyn_UnknownCall_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownCall_p_struct)); _temp6052[ 0]=({
struct Cyc_Absyn_UnknownCall_p_struct _temp6053; _temp6053.tag= Cyc_Absyn_UnknownCall_p_tag;
_temp6053.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6054= yyvs;
struct _xtunion_struct** _temp6056= _temp6054.curr +( yyvsp_offset - 4); if(
_temp6054.base == 0? 1:( _temp6056 < _temp6054.base? 1: _temp6056 >= _temp6054.last_plus_one)){
_throw( Null_Exception);}* _temp6056;})); _temp6053.f2= tvs; _temp6053.f3= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp6057= yyvs; struct _xtunion_struct** _temp6059=
_temp6057.curr +( yyvsp_offset - 1); if( _temp6057.base == 0? 1:( _temp6059 <
_temp6057.base? 1: _temp6059 >= _temp6057.last_plus_one)){ _throw(
Null_Exception);}* _temp6059;})); _temp6053;}); _temp6052;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6060= yyls; struct Cyc_Yyltype* _temp6062= _temp6060.curr
+( yylsp_offset - 4); if( _temp6060.base == 0? 1:( _temp6062 < _temp6060.base? 1:
_temp6062 >= _temp6060.last_plus_one)){ _throw( Null_Exception);}* _temp6062;}).first_line,({
struct _tagged_ptr3 _temp6063= yyls; struct Cyc_Yyltype* _temp6065= _temp6063.curr
+ yylsp_offset; if( _temp6063.base == 0? 1:( _temp6065 < _temp6063.base? 1:
_temp6065 >= _temp6063.last_plus_one)){ _throw( Null_Exception);}* _temp6065;}).last_line));
_temp6051;}); _temp6050;}); break;} case 273: _LL6049: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6076=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6076[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6077; _temp6077.tag= Cyc_Pattern_tok_tag; _temp6077.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Tuple_p_struct* _temp6078=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp6078[ 0]=({ struct
Cyc_Absyn_Tuple_p_struct _temp6079; _temp6079.tag= Cyc_Absyn_Tuple_p_tag;
_temp6079.f1= Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp6080= yyvs;
struct _xtunion_struct** _temp6082= _temp6080.curr +( yyvsp_offset - 1); if(
_temp6080.base == 0? 1:( _temp6082 < _temp6080.base? 1: _temp6082 >= _temp6080.last_plus_one)){
_throw( Null_Exception);}* _temp6082;})); _temp6079;}); _temp6078;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6083= yyls; struct Cyc_Yyltype* _temp6085= _temp6083.curr
+( yylsp_offset - 3); if( _temp6083.base == 0? 1:( _temp6085 < _temp6083.base? 1:
_temp6085 >= _temp6083.last_plus_one)){ _throw( Null_Exception);}* _temp6085;}).first_line,({
struct _tagged_ptr3 _temp6086= yyls; struct Cyc_Yyltype* _temp6088= _temp6086.curr
+ yylsp_offset; if( _temp6086.base == 0? 1:( _temp6088 < _temp6086.base? 1:
_temp6088 >= _temp6086.last_plus_one)){ _throw( Null_Exception);}* _temp6088;}).last_line));
_temp6077;}); _temp6076;}); break; case 274: _LL6075: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6103=
yyls; struct Cyc_Yyltype* _temp6105= _temp6103.curr +( yylsp_offset - 2); if(
_temp6103.base == 0? 1:( _temp6105 < _temp6103.base? 1: _temp6105 >= _temp6103.last_plus_one)){
_throw( Null_Exception);}* _temp6105;}).first_line,({ struct _tagged_ptr3
_temp6106= yyls; struct Cyc_Yyltype* _temp6108= _temp6106.curr +( yylsp_offset -
2); if( _temp6106.base == 0? 1:( _temp6108 < _temp6106.base? 1: _temp6108 >=
_temp6106.last_plus_one)){ _throw( Null_Exception);}* _temp6108;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6109= yyvs; struct
_xtunion_struct** _temp6111= _temp6109.curr +( yyvsp_offset - 2); if( _temp6109.base
== 0? 1:( _temp6111 < _temp6109.base? 1: _temp6111 >= _temp6109.last_plus_one)){
_throw( Null_Exception);}* _temp6111;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_Pattern_tok_struct* _temp6090=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6090[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6091; _temp6091.tag= Cyc_Pattern_tok_tag; _temp6091.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp6092=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp6092[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp6093; _temp6093.tag= Cyc_Absyn_UnknownFields_p_tag;
_temp6093.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6094= yyvs;
struct _xtunion_struct** _temp6096= _temp6094.curr +( yyvsp_offset - 3); if(
_temp6094.base == 0? 1:( _temp6096 < _temp6094.base? 1: _temp6096 >= _temp6094.last_plus_one)){
_throw( Null_Exception);}* _temp6096;})); _temp6093.f2= tvs; _temp6093.f3= 0;
_temp6093;}); _temp6092;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6097= yyls; struct Cyc_Yyltype* _temp6099= _temp6097.curr +( yylsp_offset -
3); if( _temp6097.base == 0? 1:( _temp6099 < _temp6097.base? 1: _temp6099 >=
_temp6097.last_plus_one)){ _throw( Null_Exception);}* _temp6099;}).first_line,({
struct _tagged_ptr3 _temp6100= yyls; struct Cyc_Yyltype* _temp6102= _temp6100.curr
+ yylsp_offset; if( _temp6100.base == 0? 1:( _temp6102 < _temp6100.base? 1:
_temp6102 >= _temp6100.last_plus_one)){ _throw( Null_Exception);}* _temp6102;}).last_line));
_temp6091;}); _temp6090;}); break;} case 275: _LL6089: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6129=
yyls; struct Cyc_Yyltype* _temp6131= _temp6129.curr +( yylsp_offset - 3); if(
_temp6129.base == 0? 1:( _temp6131 < _temp6129.base? 1: _temp6131 >= _temp6129.last_plus_one)){
_throw( Null_Exception);}* _temp6131;}).first_line,({ struct _tagged_ptr3
_temp6132= yyls; struct Cyc_Yyltype* _temp6134= _temp6132.curr +( yylsp_offset -
3); if( _temp6132.base == 0? 1:( _temp6134 < _temp6132.base? 1: _temp6134 >=
_temp6132.last_plus_one)){ _throw( Null_Exception);}* _temp6134;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6135= yyvs; struct
_xtunion_struct** _temp6137= _temp6135.curr +( yyvsp_offset - 3); if( _temp6135.base
== 0? 1:( _temp6137 < _temp6135.base? 1: _temp6137 >= _temp6135.last_plus_one)){
_throw( Null_Exception);}* _temp6137;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_Pattern_tok_struct* _temp6113=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6113[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6114; _temp6114.tag= Cyc_Pattern_tok_tag; _temp6114.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp6115=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp6115[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp6116; _temp6116.tag= Cyc_Absyn_UnknownFields_p_tag;
_temp6116.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6117= yyvs;
struct _xtunion_struct** _temp6119= _temp6117.curr +( yyvsp_offset - 4); if(
_temp6117.base == 0? 1:( _temp6119 < _temp6117.base? 1: _temp6119 >= _temp6117.last_plus_one)){
_throw( Null_Exception);}* _temp6119;})); _temp6116.f2= tvs; _temp6116.f3= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp6120= yyvs; struct _xtunion_struct** _temp6122=
_temp6120.curr +( yyvsp_offset - 1); if( _temp6120.base == 0? 1:( _temp6122 <
_temp6120.base? 1: _temp6122 >= _temp6120.last_plus_one)){ _throw(
Null_Exception);}* _temp6122;})); _temp6116;}); _temp6115;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6123= yyls; struct Cyc_Yyltype* _temp6125= _temp6123.curr
+( yylsp_offset - 4); if( _temp6123.base == 0? 1:( _temp6125 < _temp6123.base? 1:
_temp6125 >= _temp6123.last_plus_one)){ _throw( Null_Exception);}* _temp6125;}).first_line,({
struct _tagged_ptr3 _temp6126= yyls; struct Cyc_Yyltype* _temp6128= _temp6126.curr
+ yylsp_offset; if( _temp6126.base == 0? 1:( _temp6128 < _temp6126.base? 1:
_temp6128 >= _temp6126.last_plus_one)){ _throw( Null_Exception);}* _temp6128;}).last_line));
_temp6114;}); _temp6113;}); break;} case 276: _LL6112: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6139=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6139[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6140; _temp6140.tag= Cyc_Pattern_tok_tag; _temp6140.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Pointer_p_struct* _temp6141=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp6141[ 0]=({ struct
Cyc_Absyn_Pointer_p_struct _temp6142; _temp6142.tag= Cyc_Absyn_Pointer_p_tag;
_temp6142.f1= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6143= yyvs;
struct _xtunion_struct** _temp6145= _temp6143.curr + yyvsp_offset; if( _temp6143.base
== 0? 1:( _temp6145 < _temp6143.base? 1: _temp6145 >= _temp6143.last_plus_one)){
_throw( Null_Exception);}* _temp6145;})); _temp6142;}); _temp6141;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6146= yyls; struct Cyc_Yyltype* _temp6148= _temp6146.curr
+( yylsp_offset - 1); if( _temp6146.base == 0? 1:( _temp6148 < _temp6146.base? 1:
_temp6148 >= _temp6146.last_plus_one)){ _throw( Null_Exception);}* _temp6148;}).first_line,({
struct _tagged_ptr3 _temp6149= yyls; struct Cyc_Yyltype* _temp6151= _temp6149.curr
+ yylsp_offset; if( _temp6149.base == 0? 1:( _temp6151 < _temp6149.base? 1:
_temp6151 >= _temp6149.last_plus_one)){ _throw( Null_Exception);}* _temp6151;}).last_line));
_temp6140;}); _temp6139;}); break; case 277: _LL6138: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6153=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6153[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6154; _temp6154.tag= Cyc_Pattern_tok_tag; _temp6154.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Reference_p_struct* _temp6155=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp6155[ 0]=({
struct Cyc_Absyn_Reference_p_struct _temp6156; _temp6156.tag= Cyc_Absyn_Reference_p_tag;
_temp6156.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp6157=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp6157->f1=( void*) Cyc_Absyn_Loc_n;
_temp6157->f2=({ struct _tagged_string* _temp6158=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp6158[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp6159= yyvs; struct _xtunion_struct** _temp6161=
_temp6159.curr + yyvsp_offset; if( _temp6159.base == 0? 1:( _temp6161 <
_temp6159.base? 1: _temp6161 >= _temp6159.last_plus_one)){ _throw(
Null_Exception);}* _temp6161;})); _temp6158;}); _temp6157;}),( void*) Cyc_Absyn_VoidType,
0); _temp6156;}); _temp6155;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6162= yyls; struct Cyc_Yyltype* _temp6164= _temp6162.curr +(
yylsp_offset - 1); if( _temp6162.base == 0? 1:( _temp6164 < _temp6162.base? 1:
_temp6164 >= _temp6162.last_plus_one)){ _throw( Null_Exception);}* _temp6164;}).first_line,({
struct _tagged_ptr3 _temp6165= yyls; struct Cyc_Yyltype* _temp6167= _temp6165.curr
+ yylsp_offset; if( _temp6165.base == 0? 1:( _temp6167 < _temp6165.base? 1:
_temp6167 >= _temp6165.last_plus_one)){ _throw( Null_Exception);}* _temp6167;}).last_line));
_temp6154;}); _temp6153;}); break; case 278: _LL6152: yyval=( struct
_xtunion_struct*)({ struct Cyc_PatternList_tok_struct* _temp6169=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp6169[ 0]=({ struct
Cyc_PatternList_tok_struct _temp6170; _temp6170.tag= Cyc_PatternList_tok_tag;
_temp6170.f1= 0; _temp6170;}); _temp6169;}); break; case 279: _LL6168: yyval=(
struct _xtunion_struct*)({ struct Cyc_PatternList_tok_struct* _temp6172=( struct
Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6172[ 0]=({ struct Cyc_PatternList_tok_struct _temp6173; _temp6173.tag= Cyc_PatternList_tok_tag;
_temp6173.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp6174= yyvs; struct
_xtunion_struct** _temp6176= _temp6174.curr + yyvsp_offset; if( _temp6174.base
== 0? 1:( _temp6176 < _temp6174.base? 1: _temp6176 >= _temp6174.last_plus_one)){
_throw( Null_Exception);}* _temp6176;}))); _temp6173;}); _temp6172;}); break;
case 280: _LL6171: yyval=( struct _xtunion_struct*)({ struct Cyc_PatternList_tok_struct*
_temp6178=( struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6178[ 0]=({ struct Cyc_PatternList_tok_struct _temp6179; _temp6179.tag= Cyc_PatternList_tok_tag;
_temp6179.f1=({ struct Cyc_List_List* _temp6180=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6180->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp6181= yyvs; struct _xtunion_struct** _temp6183=
_temp6181.curr + yyvsp_offset; if( _temp6181.base == 0? 1:( _temp6183 <
_temp6181.base? 1: _temp6183 >= _temp6181.last_plus_one)){ _throw(
Null_Exception);}* _temp6183;})); _temp6180->tl= 0; _temp6180;}); _temp6179;});
_temp6178;}); break; case 281: _LL6177: yyval=( struct _xtunion_struct*)({
struct Cyc_PatternList_tok_struct* _temp6185=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp6185[ 0]=({ struct
Cyc_PatternList_tok_struct _temp6186; _temp6186.tag= Cyc_PatternList_tok_tag;
_temp6186.f1=({ struct Cyc_List_List* _temp6187=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6187->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp6191= yyvs; struct _xtunion_struct** _temp6193=
_temp6191.curr + yyvsp_offset; if( _temp6191.base == 0? 1:( _temp6193 <
_temp6191.base? 1: _temp6193 >= _temp6191.last_plus_one)){ _throw(
Null_Exception);}* _temp6193;})); _temp6187->tl= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp6188= yyvs; struct _xtunion_struct** _temp6190=
_temp6188.curr +( yyvsp_offset - 2); if( _temp6188.base == 0? 1:( _temp6190 <
_temp6188.base? 1: _temp6190 >= _temp6188.last_plus_one)){ _throw(
Null_Exception);}* _temp6190;})); _temp6187;}); _temp6186;}); _temp6185;});
break; case 282: _LL6184: yyval=( struct _xtunion_struct*)({ struct Cyc_FieldPattern_tok_struct*
_temp6195=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6195[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6196; _temp6196.tag=
Cyc_FieldPattern_tok_tag; _temp6196.f1=({ struct _tuple17* _temp6197=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp6197->f1= 0; _temp6197->f2=
Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6198= yyvs; struct
_xtunion_struct** _temp6200= _temp6198.curr + yyvsp_offset; if( _temp6198.base
== 0? 1:( _temp6200 < _temp6198.base? 1: _temp6200 >= _temp6198.last_plus_one)){
_throw( Null_Exception);}* _temp6200;})); _temp6197;}); _temp6196;}); _temp6195;});
break; case 283: _LL6194: yyval=( struct _xtunion_struct*)({ struct Cyc_FieldPattern_tok_struct*
_temp6202=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6202[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6203; _temp6203.tag=
Cyc_FieldPattern_tok_tag; _temp6203.f1=({ struct _tuple17* _temp6204=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp6204->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp6208= yyvs; struct _xtunion_struct** _temp6210=
_temp6208.curr +( yyvsp_offset - 1); if( _temp6208.base == 0? 1:( _temp6210 <
_temp6208.base? 1: _temp6210 >= _temp6208.last_plus_one)){ _throw(
Null_Exception);}* _temp6210;})); _temp6204->f2= Cyc_yyget_Pattern_tok(({ struct
_tagged_ptr2 _temp6205= yyvs; struct _xtunion_struct** _temp6207= _temp6205.curr
+ yyvsp_offset; if( _temp6205.base == 0? 1:( _temp6207 < _temp6205.base? 1:
_temp6207 >= _temp6205.last_plus_one)){ _throw( Null_Exception);}* _temp6207;}));
_temp6204;}); _temp6203;}); _temp6202;}); break; case 284: _LL6201: yyval=(
struct _xtunion_struct*)({ struct Cyc_FieldPatternList_tok_struct* _temp6212=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp6212[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp6213; _temp6213.tag=
Cyc_FieldPatternList_tok_tag; _temp6213.f1=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok(({ struct
_tagged_ptr2 _temp6214= yyvs; struct _xtunion_struct** _temp6216= _temp6214.curr
+ yyvsp_offset; if( _temp6214.base == 0? 1:( _temp6216 < _temp6214.base? 1:
_temp6216 >= _temp6214.last_plus_one)){ _throw( Null_Exception);}* _temp6216;})));
_temp6213;}); _temp6212;}); break; case 285: _LL6211: yyval=( struct
_xtunion_struct*)({ struct Cyc_FieldPatternList_tok_struct* _temp6218=( struct
Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp6218[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp6219; _temp6219.tag=
Cyc_FieldPatternList_tok_tag; _temp6219.f1=({ struct Cyc_List_List* _temp6220=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp6220->hd=(
void*) Cyc_yyget_FieldPattern_tok(({ struct _tagged_ptr2 _temp6221= yyvs; struct
_xtunion_struct** _temp6223= _temp6221.curr + yyvsp_offset; if( _temp6221.base
== 0? 1:( _temp6223 < _temp6221.base? 1: _temp6223 >= _temp6221.last_plus_one)){
_throw( Null_Exception);}* _temp6223;})); _temp6220->tl= 0; _temp6220;});
_temp6219;}); _temp6218;}); break; case 286: _LL6217: yyval=( struct
_xtunion_struct*)({ struct Cyc_FieldPatternList_tok_struct* _temp6225=( struct
Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp6225[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp6226; _temp6226.tag=
Cyc_FieldPatternList_tok_tag; _temp6226.f1=({ struct Cyc_List_List* _temp6227=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp6227->hd=(
void*) Cyc_yyget_FieldPattern_tok(({ struct _tagged_ptr2 _temp6231= yyvs; struct
_xtunion_struct** _temp6233= _temp6231.curr + yyvsp_offset; if( _temp6231.base
== 0? 1:( _temp6233 < _temp6231.base? 1: _temp6233 >= _temp6231.last_plus_one)){
_throw( Null_Exception);}* _temp6233;})); _temp6227->tl= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp6228= yyvs; struct _xtunion_struct** _temp6230=
_temp6228.curr +( yyvsp_offset - 2); if( _temp6228.base == 0? 1:( _temp6230 <
_temp6228.base? 1: _temp6230 >= _temp6228.last_plus_one)){ _throw(
Null_Exception);}* _temp6230;})); _temp6227;}); _temp6226;}); _temp6225;});
break; case 287: _LL6224: yyval=({ struct _tagged_ptr2 _temp6235= yyvs; struct
_xtunion_struct** _temp6237= _temp6235.curr + yyvsp_offset; if( _temp6235.base
== 0? 1:( _temp6237 < _temp6235.base? 1: _temp6237 >= _temp6235.last_plus_one)){
_throw( Null_Exception);}* _temp6237;}); break; case 288: _LL6234: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6239=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6239[ 0]=({ struct Cyc_Exp_tok_struct
_temp6240; _temp6240.tag= Cyc_Exp_tok_tag; _temp6240.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6241= yyvs; struct _xtunion_struct** _temp6243=
_temp6241.curr +( yyvsp_offset - 2); if( _temp6241.base == 0? 1:( _temp6243 <
_temp6241.base? 1: _temp6243 >= _temp6241.last_plus_one)){ _throw(
Null_Exception);}* _temp6243;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6244= yyvs; struct _xtunion_struct** _temp6246= _temp6244.curr +
yyvsp_offset; if( _temp6244.base == 0? 1:( _temp6246 < _temp6244.base? 1:
_temp6246 >= _temp6244.last_plus_one)){ _throw( Null_Exception);}* _temp6246;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6247= yyls; struct Cyc_Yyltype*
_temp6249= _temp6247.curr +( yylsp_offset - 2); if( _temp6247.base == 0? 1:(
_temp6249 < _temp6247.base? 1: _temp6249 >= _temp6247.last_plus_one)){ _throw(
Null_Exception);}* _temp6249;}).first_line,({ struct _tagged_ptr3 _temp6250=
yyls; struct Cyc_Yyltype* _temp6252= _temp6250.curr + yylsp_offset; if(
_temp6250.base == 0? 1:( _temp6252 < _temp6250.base? 1: _temp6252 >= _temp6250.last_plus_one)){
_throw( Null_Exception);}* _temp6252;}).last_line)); _temp6240;}); _temp6239;});
break; case 289: _LL6238: yyval=({ struct _tagged_ptr2 _temp6254= yyvs; struct
_xtunion_struct** _temp6256= _temp6254.curr + yyvsp_offset; if( _temp6254.base
== 0? 1:( _temp6256 < _temp6254.base? 1: _temp6256 >= _temp6254.last_plus_one)){
_throw( Null_Exception);}* _temp6256;}); break; case 290: _LL6253: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6258=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6258[ 0]=({ struct Cyc_Exp_tok_struct
_temp6259; _temp6259.tag= Cyc_Exp_tok_tag; _temp6259.f1= Cyc_Absyn_assignop_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6260= yyvs; struct _xtunion_struct**
_temp6262= _temp6260.curr +( yyvsp_offset - 2); if( _temp6260.base == 0? 1:(
_temp6262 < _temp6260.base? 1: _temp6262 >= _temp6260.last_plus_one)){ _throw(
Null_Exception);}* _temp6262;})), Cyc_yyget_Primopopt_tok(({ struct _tagged_ptr2
_temp6263= yyvs; struct _xtunion_struct** _temp6265= _temp6263.curr +(
yyvsp_offset - 1); if( _temp6263.base == 0? 1:( _temp6265 < _temp6263.base? 1:
_temp6265 >= _temp6263.last_plus_one)){ _throw( Null_Exception);}* _temp6265;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6266= yyvs; struct _xtunion_struct**
_temp6268= _temp6266.curr + yyvsp_offset; if( _temp6266.base == 0? 1:( _temp6268
< _temp6266.base? 1: _temp6268 >= _temp6266.last_plus_one)){ _throw(
Null_Exception);}* _temp6268;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6269= yyls; struct Cyc_Yyltype* _temp6271= _temp6269.curr +(
yylsp_offset - 2); if( _temp6269.base == 0? 1:( _temp6271 < _temp6269.base? 1:
_temp6271 >= _temp6269.last_plus_one)){ _throw( Null_Exception);}* _temp6271;}).first_line,({
struct _tagged_ptr3 _temp6272= yyls; struct Cyc_Yyltype* _temp6274= _temp6272.curr
+ yylsp_offset; if( _temp6272.base == 0? 1:( _temp6274 < _temp6272.base? 1:
_temp6274 >= _temp6272.last_plus_one)){ _throw( Null_Exception);}* _temp6274;}).last_line));
_temp6259;}); _temp6258;}); break; case 291: _LL6257: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6276=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6276[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6277; _temp6277.tag= Cyc_Primopopt_tok_tag;
_temp6277.f1= 0; _temp6277;}); _temp6276;}); break; case 292: _LL6275: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6279=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6279[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6280; _temp6280.tag= Cyc_Primopopt_tok_tag;
_temp6280.f1=({ struct Cyc_Core_Opt* _temp6281=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6281->v=( void*)(( void*) Cyc_Absyn_Times);
_temp6281;}); _temp6280;}); _temp6279;}); break; case 293: _LL6278: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6283=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6283[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6284; _temp6284.tag= Cyc_Primopopt_tok_tag;
_temp6284.f1=({ struct Cyc_Core_Opt* _temp6285=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6285->v=( void*)(( void*) Cyc_Absyn_Div);
_temp6285;}); _temp6284;}); _temp6283;}); break; case 294: _LL6282: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6287=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6287[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6288; _temp6288.tag= Cyc_Primopopt_tok_tag;
_temp6288.f1=({ struct Cyc_Core_Opt* _temp6289=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6289->v=( void*)(( void*) Cyc_Absyn_Mod);
_temp6289;}); _temp6288;}); _temp6287;}); break; case 295: _LL6286: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6291=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6291[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6292; _temp6292.tag= Cyc_Primopopt_tok_tag;
_temp6292.f1=({ struct Cyc_Core_Opt* _temp6293=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6293->v=( void*)(( void*) Cyc_Absyn_Plus);
_temp6293;}); _temp6292;}); _temp6291;}); break; case 296: _LL6290: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6295=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6295[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6296; _temp6296.tag= Cyc_Primopopt_tok_tag;
_temp6296.f1=({ struct Cyc_Core_Opt* _temp6297=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6297->v=( void*)(( void*) Cyc_Absyn_Minus);
_temp6297;}); _temp6296;}); _temp6295;}); break; case 297: _LL6294: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6299=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6299[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6300; _temp6300.tag= Cyc_Primopopt_tok_tag;
_temp6300.f1=({ struct Cyc_Core_Opt* _temp6301=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6301->v=( void*)(( void*) Cyc_Absyn_Bitlshift);
_temp6301;}); _temp6300;}); _temp6299;}); break; case 298: _LL6298: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6303=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6303[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6304; _temp6304.tag= Cyc_Primopopt_tok_tag;
_temp6304.f1=({ struct Cyc_Core_Opt* _temp6305=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6305->v=( void*)(( void*) Cyc_Absyn_Bitlrshift);
_temp6305;}); _temp6304;}); _temp6303;}); break; case 299: _LL6302: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6307=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6307[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6308; _temp6308.tag= Cyc_Primopopt_tok_tag;
_temp6308.f1=({ struct Cyc_Core_Opt* _temp6309=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6309->v=( void*)(( void*) Cyc_Absyn_Bitand);
_temp6309;}); _temp6308;}); _temp6307;}); break; case 300: _LL6306: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6311=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6311[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6312; _temp6312.tag= Cyc_Primopopt_tok_tag;
_temp6312.f1=({ struct Cyc_Core_Opt* _temp6313=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6313->v=( void*)(( void*) Cyc_Absyn_Bitxor);
_temp6313;}); _temp6312;}); _temp6311;}); break; case 301: _LL6310: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6315=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6315[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6316; _temp6316.tag= Cyc_Primopopt_tok_tag;
_temp6316.f1=({ struct Cyc_Core_Opt* _temp6317=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6317->v=( void*)(( void*) Cyc_Absyn_Bitor);
_temp6317;}); _temp6316;}); _temp6315;}); break; case 302: _LL6314: yyval=({
struct _tagged_ptr2 _temp6319= yyvs; struct _xtunion_struct** _temp6321=
_temp6319.curr + yyvsp_offset; if( _temp6319.base == 0? 1:( _temp6321 <
_temp6319.base? 1: _temp6321 >= _temp6319.last_plus_one)){ _throw(
Null_Exception);}* _temp6321;}); break; case 303: _LL6318: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6323=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6323[ 0]=({ struct Cyc_Exp_tok_struct
_temp6324; _temp6324.tag= Cyc_Exp_tok_tag; _temp6324.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6325= yyvs; struct _xtunion_struct**
_temp6327= _temp6325.curr +( yyvsp_offset - 4); if( _temp6325.base == 0? 1:(
_temp6327 < _temp6325.base? 1: _temp6327 >= _temp6325.last_plus_one)){ _throw(
Null_Exception);}* _temp6327;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6328= yyvs; struct _xtunion_struct** _temp6330= _temp6328.curr +(
yyvsp_offset - 2); if( _temp6328.base == 0? 1:( _temp6330 < _temp6328.base? 1:
_temp6330 >= _temp6328.last_plus_one)){ _throw( Null_Exception);}* _temp6330;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6331= yyvs; struct _xtunion_struct**
_temp6333= _temp6331.curr + yyvsp_offset; if( _temp6331.base == 0? 1:( _temp6333
< _temp6331.base? 1: _temp6333 >= _temp6331.last_plus_one)){ _throw(
Null_Exception);}* _temp6333;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6334= yyls; struct Cyc_Yyltype* _temp6336= _temp6334.curr +(
yylsp_offset - 4); if( _temp6334.base == 0? 1:( _temp6336 < _temp6334.base? 1:
_temp6336 >= _temp6334.last_plus_one)){ _throw( Null_Exception);}* _temp6336;}).first_line,({
struct _tagged_ptr3 _temp6337= yyls; struct Cyc_Yyltype* _temp6339= _temp6337.curr
+ yylsp_offset; if( _temp6337.base == 0? 1:( _temp6339 < _temp6337.base? 1:
_temp6339 >= _temp6337.last_plus_one)){ _throw( Null_Exception);}* _temp6339;}).last_line));
_temp6324;}); _temp6323;}); break; case 304: _LL6322: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6341=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6341[ 0]=({ struct Cyc_Exp_tok_struct
_temp6342; _temp6342.tag= Cyc_Exp_tok_tag; _temp6342.f1= Cyc_Absyn_throw_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6343= yyvs; struct _xtunion_struct**
_temp6345= _temp6343.curr + yyvsp_offset; if( _temp6343.base == 0? 1:( _temp6345
< _temp6343.base? 1: _temp6345 >= _temp6343.last_plus_one)){ _throw(
Null_Exception);}* _temp6345;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6346= yyls; struct Cyc_Yyltype* _temp6348= _temp6346.curr +(
yylsp_offset - 1); if( _temp6346.base == 0? 1:( _temp6348 < _temp6346.base? 1:
_temp6348 >= _temp6346.last_plus_one)){ _throw( Null_Exception);}* _temp6348;}).first_line,({
struct _tagged_ptr3 _temp6349= yyls; struct Cyc_Yyltype* _temp6351= _temp6349.curr
+ yylsp_offset; if( _temp6349.base == 0? 1:( _temp6351 < _temp6349.base? 1:
_temp6351 >= _temp6349.last_plus_one)){ _throw( Null_Exception);}* _temp6351;}).last_line));
_temp6342;}); _temp6341;}); break; case 305: _LL6340: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6353=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6353[ 0]=({ struct Cyc_Exp_tok_struct
_temp6354; _temp6354.tag= Cyc_Exp_tok_tag; _temp6354.f1= Cyc_Absyn_New_exp( 0,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6355= yyvs; struct _xtunion_struct**
_temp6357= _temp6355.curr + yyvsp_offset; if( _temp6355.base == 0? 1:( _temp6357
< _temp6355.base? 1: _temp6357 >= _temp6355.last_plus_one)){ _throw(
Null_Exception);}* _temp6357;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6358= yyls; struct Cyc_Yyltype* _temp6360= _temp6358.curr +(
yylsp_offset - 1); if( _temp6358.base == 0? 1:( _temp6360 < _temp6358.base? 1:
_temp6360 >= _temp6358.last_plus_one)){ _throw( Null_Exception);}* _temp6360;}).first_line,({
struct _tagged_ptr3 _temp6361= yyls; struct Cyc_Yyltype* _temp6363= _temp6361.curr
+( yylsp_offset + 1); if( _temp6361.base == 0? 1:( _temp6363 < _temp6361.base? 1:
_temp6363 >= _temp6361.last_plus_one)){ _throw( Null_Exception);}* _temp6363;}).last_line));
_temp6354;}); _temp6353;}); break; case 306: _LL6352: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6365=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6365[ 0]=({ struct Cyc_Exp_tok_struct
_temp6366; _temp6366.tag= Cyc_Exp_tok_tag; _temp6366.f1= Cyc_Absyn_New_exp( 0,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6367= yyvs; struct _xtunion_struct**
_temp6369= _temp6367.curr + yyvsp_offset; if( _temp6367.base == 0? 1:( _temp6369
< _temp6367.base? 1: _temp6369 >= _temp6367.last_plus_one)){ _throw(
Null_Exception);}* _temp6369;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6370= yyls; struct Cyc_Yyltype* _temp6372= _temp6370.curr +(
yylsp_offset - 1); if( _temp6370.base == 0? 1:( _temp6372 < _temp6370.base? 1:
_temp6372 >= _temp6370.last_plus_one)){ _throw( Null_Exception);}* _temp6372;}).first_line,({
struct _tagged_ptr3 _temp6373= yyls; struct Cyc_Yyltype* _temp6375= _temp6373.curr
+( yylsp_offset + 1); if( _temp6373.base == 0? 1:( _temp6375 < _temp6373.base? 1:
_temp6375 >= _temp6373.last_plus_one)){ _throw( Null_Exception);}* _temp6375;}).last_line));
_temp6366;}); _temp6365;}); break; case 307: _LL6364: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6377=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6377[ 0]=({ struct Cyc_Exp_tok_struct
_temp6378; _temp6378.tag= Cyc_Exp_tok_tag; _temp6378.f1= Cyc_Absyn_New_exp((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6379= yyvs;
struct _xtunion_struct** _temp6381= _temp6379.curr +( yyvsp_offset - 2); if(
_temp6379.base == 0? 1:( _temp6381 < _temp6379.base? 1: _temp6381 >= _temp6379.last_plus_one)){
_throw( Null_Exception);}* _temp6381;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6382= yyvs; struct _xtunion_struct** _temp6384= _temp6382.curr
+ yyvsp_offset; if( _temp6382.base == 0? 1:( _temp6384 < _temp6382.base? 1:
_temp6384 >= _temp6382.last_plus_one)){ _throw( Null_Exception);}* _temp6384;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6385= yyls; struct Cyc_Yyltype*
_temp6387= _temp6385.curr +( yylsp_offset - 4); if( _temp6385.base == 0? 1:(
_temp6387 < _temp6385.base? 1: _temp6387 >= _temp6385.last_plus_one)){ _throw(
Null_Exception);}* _temp6387;}).first_line,({ struct _tagged_ptr3 _temp6388=
yyls; struct Cyc_Yyltype* _temp6390= _temp6388.curr + yylsp_offset; if(
_temp6388.base == 0? 1:( _temp6390 < _temp6388.base? 1: _temp6390 >= _temp6388.last_plus_one)){
_throw( Null_Exception);}* _temp6390;}).last_line)); _temp6378;}); _temp6377;});
break; case 308: _LL6376: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6392=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6392[ 0]=({ struct Cyc_Exp_tok_struct _temp6393; _temp6393.tag= Cyc_Exp_tok_tag;
_temp6393.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6394= yyvs; struct _xtunion_struct** _temp6396=
_temp6394.curr +( yyvsp_offset - 2); if( _temp6394.base == 0? 1:( _temp6396 <
_temp6394.base? 1: _temp6396 >= _temp6394.last_plus_one)){ _throw(
Null_Exception);}* _temp6396;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6397= yyvs; struct _xtunion_struct** _temp6399= _temp6397.curr +
yyvsp_offset; if( _temp6397.base == 0? 1:( _temp6399 < _temp6397.base? 1:
_temp6399 >= _temp6397.last_plus_one)){ _throw( Null_Exception);}* _temp6399;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6400= yyls; struct Cyc_Yyltype*
_temp6402= _temp6400.curr +( yylsp_offset - 4); if( _temp6400.base == 0? 1:(
_temp6402 < _temp6400.base? 1: _temp6402 >= _temp6400.last_plus_one)){ _throw(
Null_Exception);}* _temp6402;}).first_line,({ struct _tagged_ptr3 _temp6403=
yyls; struct Cyc_Yyltype* _temp6405= _temp6403.curr + yylsp_offset; if(
_temp6403.base == 0? 1:( _temp6405 < _temp6403.base? 1: _temp6405 >= _temp6403.last_plus_one)){
_throw( Null_Exception);}* _temp6405;}).last_line)); _temp6393;}); _temp6392;});
break; case 309: _LL6391: yyval=({ struct _tagged_ptr2 _temp6407= yyvs; struct
_xtunion_struct** _temp6409= _temp6407.curr + yyvsp_offset; if( _temp6407.base
== 0? 1:( _temp6409 < _temp6407.base? 1: _temp6409 >= _temp6407.last_plus_one)){
_throw( Null_Exception);}* _temp6409;}); break; case 310: _LL6406: yyval=({
struct _tagged_ptr2 _temp6411= yyvs; struct _xtunion_struct** _temp6413=
_temp6411.curr + yyvsp_offset; if( _temp6411.base == 0? 1:( _temp6413 <
_temp6411.base? 1: _temp6413 >= _temp6411.last_plus_one)){ _throw(
Null_Exception);}* _temp6413;}); break; case 311: _LL6410: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6415=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6415[ 0]=({ struct Cyc_Exp_tok_struct
_temp6416; _temp6416.tag= Cyc_Exp_tok_tag; _temp6416.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6417= yyvs; struct _xtunion_struct** _temp6419=
_temp6417.curr +( yyvsp_offset - 2); if( _temp6417.base == 0? 1:( _temp6419 <
_temp6417.base? 1: _temp6419 >= _temp6417.last_plus_one)){ _throw(
Null_Exception);}* _temp6419;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6420= yyvs; struct _xtunion_struct** _temp6422= _temp6420.curr +
yyvsp_offset; if( _temp6420.base == 0? 1:( _temp6422 < _temp6420.base? 1:
_temp6422 >= _temp6420.last_plus_one)){ _throw( Null_Exception);}* _temp6422;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6423= yyls; struct Cyc_Yyltype*
_temp6425= _temp6423.curr +( yylsp_offset - 2); if( _temp6423.base == 0? 1:(
_temp6425 < _temp6423.base? 1: _temp6425 >= _temp6423.last_plus_one)){ _throw(
Null_Exception);}* _temp6425;}).first_line,({ struct _tagged_ptr3 _temp6426=
yyls; struct Cyc_Yyltype* _temp6428= _temp6426.curr + yylsp_offset; if(
_temp6426.base == 0? 1:( _temp6428 < _temp6426.base? 1: _temp6428 >= _temp6426.last_plus_one)){
_throw( Null_Exception);}* _temp6428;}).last_line)); _temp6416;}); _temp6415;});
break; case 312: _LL6414: yyval=({ struct _tagged_ptr2 _temp6430= yyvs; struct
_xtunion_struct** _temp6432= _temp6430.curr + yyvsp_offset; if( _temp6430.base
== 0? 1:( _temp6432 < _temp6430.base? 1: _temp6432 >= _temp6430.last_plus_one)){
_throw( Null_Exception);}* _temp6432;}); break; case 313: _LL6429: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6434=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6434[ 0]=({ struct Cyc_Exp_tok_struct
_temp6435; _temp6435.tag= Cyc_Exp_tok_tag; _temp6435.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6436= yyvs; struct _xtunion_struct** _temp6438=
_temp6436.curr +( yyvsp_offset - 2); if( _temp6436.base == 0? 1:( _temp6438 <
_temp6436.base? 1: _temp6438 >= _temp6436.last_plus_one)){ _throw(
Null_Exception);}* _temp6438;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6439= yyvs; struct _xtunion_struct** _temp6441= _temp6439.curr +
yyvsp_offset; if( _temp6439.base == 0? 1:( _temp6441 < _temp6439.base? 1:
_temp6441 >= _temp6439.last_plus_one)){ _throw( Null_Exception);}* _temp6441;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6442= yyls; struct Cyc_Yyltype*
_temp6444= _temp6442.curr +( yylsp_offset - 2); if( _temp6442.base == 0? 1:(
_temp6444 < _temp6442.base? 1: _temp6444 >= _temp6442.last_plus_one)){ _throw(
Null_Exception);}* _temp6444;}).first_line,({ struct _tagged_ptr3 _temp6445=
yyls; struct Cyc_Yyltype* _temp6447= _temp6445.curr + yylsp_offset; if(
_temp6445.base == 0? 1:( _temp6447 < _temp6445.base? 1: _temp6447 >= _temp6445.last_plus_one)){
_throw( Null_Exception);}* _temp6447;}).last_line)); _temp6435;}); _temp6434;});
break; case 314: _LL6433: yyval=({ struct _tagged_ptr2 _temp6449= yyvs; struct
_xtunion_struct** _temp6451= _temp6449.curr + yyvsp_offset; if( _temp6449.base
== 0? 1:( _temp6451 < _temp6449.base? 1: _temp6451 >= _temp6449.last_plus_one)){
_throw( Null_Exception);}* _temp6451;}); break; case 315: _LL6448: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6453=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6453[ 0]=({ struct Cyc_Exp_tok_struct
_temp6454; _temp6454.tag= Cyc_Exp_tok_tag; _temp6454.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6455= yyvs;
struct _xtunion_struct** _temp6457= _temp6455.curr +( yyvsp_offset - 2); if(
_temp6455.base == 0? 1:( _temp6457 < _temp6455.base? 1: _temp6457 >= _temp6455.last_plus_one)){
_throw( Null_Exception);}* _temp6457;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6458= yyvs; struct _xtunion_struct** _temp6460= _temp6458.curr
+ yyvsp_offset; if( _temp6458.base == 0? 1:( _temp6460 < _temp6458.base? 1:
_temp6460 >= _temp6458.last_plus_one)){ _throw( Null_Exception);}* _temp6460;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6461= yyls; struct Cyc_Yyltype*
_temp6463= _temp6461.curr +( yylsp_offset - 2); if( _temp6461.base == 0? 1:(
_temp6463 < _temp6461.base? 1: _temp6463 >= _temp6461.last_plus_one)){ _throw(
Null_Exception);}* _temp6463;}).first_line,({ struct _tagged_ptr3 _temp6464=
yyls; struct Cyc_Yyltype* _temp6466= _temp6464.curr + yylsp_offset; if(
_temp6464.base == 0? 1:( _temp6466 < _temp6464.base? 1: _temp6466 >= _temp6464.last_plus_one)){
_throw( Null_Exception);}* _temp6466;}).last_line)); _temp6454;}); _temp6453;});
break; case 316: _LL6452: yyval=({ struct _tagged_ptr2 _temp6468= yyvs; struct
_xtunion_struct** _temp6470= _temp6468.curr + yyvsp_offset; if( _temp6468.base
== 0? 1:( _temp6470 < _temp6468.base? 1: _temp6470 >= _temp6468.last_plus_one)){
_throw( Null_Exception);}* _temp6470;}); break; case 317: _LL6467: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6472=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6472[ 0]=({ struct Cyc_Exp_tok_struct
_temp6473; _temp6473.tag= Cyc_Exp_tok_tag; _temp6473.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6474=
yyvs; struct _xtunion_struct** _temp6476= _temp6474.curr +( yyvsp_offset - 2);
if( _temp6474.base == 0? 1:( _temp6476 < _temp6474.base? 1: _temp6476 >=
_temp6474.last_plus_one)){ _throw( Null_Exception);}* _temp6476;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6477= yyvs; struct _xtunion_struct** _temp6479=
_temp6477.curr + yyvsp_offset; if( _temp6477.base == 0? 1:( _temp6479 <
_temp6477.base? 1: _temp6479 >= _temp6477.last_plus_one)){ _throw(
Null_Exception);}* _temp6479;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6480= yyls; struct Cyc_Yyltype* _temp6482= _temp6480.curr +(
yylsp_offset - 2); if( _temp6480.base == 0? 1:( _temp6482 < _temp6480.base? 1:
_temp6482 >= _temp6480.last_plus_one)){ _throw( Null_Exception);}* _temp6482;}).first_line,({
struct _tagged_ptr3 _temp6483= yyls; struct Cyc_Yyltype* _temp6485= _temp6483.curr
+ yylsp_offset; if( _temp6483.base == 0? 1:( _temp6485 < _temp6483.base? 1:
_temp6485 >= _temp6483.last_plus_one)){ _throw( Null_Exception);}* _temp6485;}).last_line));
_temp6473;}); _temp6472;}); break; case 318: _LL6471: yyval=({ struct
_tagged_ptr2 _temp6487= yyvs; struct _xtunion_struct** _temp6489= _temp6487.curr
+ yyvsp_offset; if( _temp6487.base == 0? 1:( _temp6489 < _temp6487.base? 1:
_temp6489 >= _temp6487.last_plus_one)){ _throw( Null_Exception);}* _temp6489;});
break; case 319: _LL6486: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6491=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6491[ 0]=({ struct Cyc_Exp_tok_struct _temp6492; _temp6492.tag= Cyc_Exp_tok_tag;
_temp6492.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6493= yyvs; struct _xtunion_struct** _temp6495=
_temp6493.curr +( yyvsp_offset - 2); if( _temp6493.base == 0? 1:( _temp6495 <
_temp6493.base? 1: _temp6495 >= _temp6493.last_plus_one)){ _throw(
Null_Exception);}* _temp6495;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6496= yyvs; struct _xtunion_struct** _temp6498= _temp6496.curr +
yyvsp_offset; if( _temp6496.base == 0? 1:( _temp6498 < _temp6496.base? 1:
_temp6498 >= _temp6496.last_plus_one)){ _throw( Null_Exception);}* _temp6498;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6499= yyls; struct Cyc_Yyltype*
_temp6501= _temp6499.curr +( yylsp_offset - 2); if( _temp6499.base == 0? 1:(
_temp6501 < _temp6499.base? 1: _temp6501 >= _temp6499.last_plus_one)){ _throw(
Null_Exception);}* _temp6501;}).first_line,({ struct _tagged_ptr3 _temp6502=
yyls; struct Cyc_Yyltype* _temp6504= _temp6502.curr + yylsp_offset; if(
_temp6502.base == 0? 1:( _temp6504 < _temp6502.base? 1: _temp6504 >= _temp6502.last_plus_one)){
_throw( Null_Exception);}* _temp6504;}).last_line)); _temp6492;}); _temp6491;});
break; case 320: _LL6490: yyval=({ struct _tagged_ptr2 _temp6506= yyvs; struct
_xtunion_struct** _temp6508= _temp6506.curr + yyvsp_offset; if( _temp6506.base
== 0? 1:( _temp6508 < _temp6506.base? 1: _temp6508 >= _temp6506.last_plus_one)){
_throw( Null_Exception);}* _temp6508;}); break; case 321: _LL6505: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6510=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6510[ 0]=({ struct Cyc_Exp_tok_struct
_temp6511; _temp6511.tag= Cyc_Exp_tok_tag; _temp6511.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6512= yyvs; struct _xtunion_struct** _temp6514=
_temp6512.curr +( yyvsp_offset - 2); if( _temp6512.base == 0? 1:( _temp6514 <
_temp6512.base? 1: _temp6514 >= _temp6512.last_plus_one)){ _throw(
Null_Exception);}* _temp6514;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6515= yyvs; struct _xtunion_struct** _temp6517= _temp6515.curr +
yyvsp_offset; if( _temp6515.base == 0? 1:( _temp6517 < _temp6515.base? 1:
_temp6517 >= _temp6515.last_plus_one)){ _throw( Null_Exception);}* _temp6517;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6518= yyls; struct Cyc_Yyltype*
_temp6520= _temp6518.curr +( yylsp_offset - 2); if( _temp6518.base == 0? 1:(
_temp6520 < _temp6518.base? 1: _temp6520 >= _temp6518.last_plus_one)){ _throw(
Null_Exception);}* _temp6520;}).first_line,({ struct _tagged_ptr3 _temp6521=
yyls; struct Cyc_Yyltype* _temp6523= _temp6521.curr + yylsp_offset; if(
_temp6521.base == 0? 1:( _temp6523 < _temp6521.base? 1: _temp6523 >= _temp6521.last_plus_one)){
_throw( Null_Exception);}* _temp6523;}).last_line)); _temp6511;}); _temp6510;});
break; case 322: _LL6509: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6525=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6525[ 0]=({ struct Cyc_Exp_tok_struct _temp6526; _temp6526.tag= Cyc_Exp_tok_tag;
_temp6526.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6527= yyvs; struct _xtunion_struct** _temp6529= _temp6527.curr +(
yyvsp_offset - 2); if( _temp6527.base == 0? 1:( _temp6529 < _temp6527.base? 1:
_temp6529 >= _temp6527.last_plus_one)){ _throw( Null_Exception);}* _temp6529;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6530= yyvs; struct _xtunion_struct**
_temp6532= _temp6530.curr + yyvsp_offset; if( _temp6530.base == 0? 1:( _temp6532
< _temp6530.base? 1: _temp6532 >= _temp6530.last_plus_one)){ _throw(
Null_Exception);}* _temp6532;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6533= yyls; struct Cyc_Yyltype* _temp6535= _temp6533.curr +(
yylsp_offset - 2); if( _temp6533.base == 0? 1:( _temp6535 < _temp6533.base? 1:
_temp6535 >= _temp6533.last_plus_one)){ _throw( Null_Exception);}* _temp6535;}).first_line,({
struct _tagged_ptr3 _temp6536= yyls; struct Cyc_Yyltype* _temp6538= _temp6536.curr
+ yylsp_offset; if( _temp6536.base == 0? 1:( _temp6538 < _temp6536.base? 1:
_temp6538 >= _temp6536.last_plus_one)){ _throw( Null_Exception);}* _temp6538;}).last_line));
_temp6526;}); _temp6525;}); break; case 323: _LL6524: yyval=({ struct
_tagged_ptr2 _temp6540= yyvs; struct _xtunion_struct** _temp6542= _temp6540.curr
+ yyvsp_offset; if( _temp6540.base == 0? 1:( _temp6542 < _temp6540.base? 1:
_temp6542 >= _temp6540.last_plus_one)){ _throw( Null_Exception);}* _temp6542;});
break; case 324: _LL6539: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6544=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6544[ 0]=({ struct Cyc_Exp_tok_struct _temp6545; _temp6545.tag= Cyc_Exp_tok_tag;
_temp6545.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6546= yyvs; struct _xtunion_struct** _temp6548= _temp6546.curr +(
yyvsp_offset - 2); if( _temp6546.base == 0? 1:( _temp6548 < _temp6546.base? 1:
_temp6548 >= _temp6546.last_plus_one)){ _throw( Null_Exception);}* _temp6548;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6549= yyvs; struct _xtunion_struct**
_temp6551= _temp6549.curr + yyvsp_offset; if( _temp6549.base == 0? 1:( _temp6551
< _temp6549.base? 1: _temp6551 >= _temp6549.last_plus_one)){ _throw(
Null_Exception);}* _temp6551;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6552= yyls; struct Cyc_Yyltype* _temp6554= _temp6552.curr +(
yylsp_offset - 2); if( _temp6552.base == 0? 1:( _temp6554 < _temp6552.base? 1:
_temp6554 >= _temp6552.last_plus_one)){ _throw( Null_Exception);}* _temp6554;}).first_line,({
struct _tagged_ptr3 _temp6555= yyls; struct Cyc_Yyltype* _temp6557= _temp6555.curr
+ yylsp_offset; if( _temp6555.base == 0? 1:( _temp6557 < _temp6555.base? 1:
_temp6557 >= _temp6555.last_plus_one)){ _throw( Null_Exception);}* _temp6557;}).last_line));
_temp6545;}); _temp6544;}); break; case 325: _LL6543: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6559=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6559[ 0]=({ struct Cyc_Exp_tok_struct
_temp6560; _temp6560.tag= Cyc_Exp_tok_tag; _temp6560.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6561= yyvs; struct _xtunion_struct** _temp6563=
_temp6561.curr +( yyvsp_offset - 2); if( _temp6561.base == 0? 1:( _temp6563 <
_temp6561.base? 1: _temp6563 >= _temp6561.last_plus_one)){ _throw(
Null_Exception);}* _temp6563;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6564= yyvs; struct _xtunion_struct** _temp6566= _temp6564.curr +
yyvsp_offset; if( _temp6564.base == 0? 1:( _temp6566 < _temp6564.base? 1:
_temp6566 >= _temp6564.last_plus_one)){ _throw( Null_Exception);}* _temp6566;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6567= yyls; struct Cyc_Yyltype*
_temp6569= _temp6567.curr +( yylsp_offset - 2); if( _temp6567.base == 0? 1:(
_temp6569 < _temp6567.base? 1: _temp6569 >= _temp6567.last_plus_one)){ _throw(
Null_Exception);}* _temp6569;}).first_line,({ struct _tagged_ptr3 _temp6570=
yyls; struct Cyc_Yyltype* _temp6572= _temp6570.curr + yylsp_offset; if(
_temp6570.base == 0? 1:( _temp6572 < _temp6570.base? 1: _temp6572 >= _temp6570.last_plus_one)){
_throw( Null_Exception);}* _temp6572;}).last_line)); _temp6560;}); _temp6559;});
break; case 326: _LL6558: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6574=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6574[ 0]=({ struct Cyc_Exp_tok_struct _temp6575; _temp6575.tag= Cyc_Exp_tok_tag;
_temp6575.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6576= yyvs; struct _xtunion_struct** _temp6578= _temp6576.curr +(
yyvsp_offset - 2); if( _temp6576.base == 0? 1:( _temp6578 < _temp6576.base? 1:
_temp6578 >= _temp6576.last_plus_one)){ _throw( Null_Exception);}* _temp6578;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6579= yyvs; struct _xtunion_struct**
_temp6581= _temp6579.curr + yyvsp_offset; if( _temp6579.base == 0? 1:( _temp6581
< _temp6579.base? 1: _temp6581 >= _temp6579.last_plus_one)){ _throw(
Null_Exception);}* _temp6581;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6582= yyls; struct Cyc_Yyltype* _temp6584= _temp6582.curr +(
yylsp_offset - 2); if( _temp6582.base == 0? 1:( _temp6584 < _temp6582.base? 1:
_temp6584 >= _temp6582.last_plus_one)){ _throw( Null_Exception);}* _temp6584;}).first_line,({
struct _tagged_ptr3 _temp6585= yyls; struct Cyc_Yyltype* _temp6587= _temp6585.curr
+ yylsp_offset; if( _temp6585.base == 0? 1:( _temp6587 < _temp6585.base? 1:
_temp6587 >= _temp6585.last_plus_one)){ _throw( Null_Exception);}* _temp6587;}).last_line));
_temp6575;}); _temp6574;}); break; case 327: _LL6573: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6589=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6589[ 0]=({ struct Cyc_Exp_tok_struct
_temp6590; _temp6590.tag= Cyc_Exp_tok_tag; _temp6590.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6591= yyvs; struct _xtunion_struct** _temp6593=
_temp6591.curr +( yyvsp_offset - 2); if( _temp6591.base == 0? 1:( _temp6593 <
_temp6591.base? 1: _temp6593 >= _temp6591.last_plus_one)){ _throw(
Null_Exception);}* _temp6593;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6594= yyvs; struct _xtunion_struct** _temp6596= _temp6594.curr +
yyvsp_offset; if( _temp6594.base == 0? 1:( _temp6596 < _temp6594.base? 1:
_temp6596 >= _temp6594.last_plus_one)){ _throw( Null_Exception);}* _temp6596;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6597= yyls; struct Cyc_Yyltype*
_temp6599= _temp6597.curr +( yylsp_offset - 2); if( _temp6597.base == 0? 1:(
_temp6599 < _temp6597.base? 1: _temp6599 >= _temp6597.last_plus_one)){ _throw(
Null_Exception);}* _temp6599;}).first_line,({ struct _tagged_ptr3 _temp6600=
yyls; struct Cyc_Yyltype* _temp6602= _temp6600.curr + yylsp_offset; if(
_temp6600.base == 0? 1:( _temp6602 < _temp6600.base? 1: _temp6602 >= _temp6600.last_plus_one)){
_throw( Null_Exception);}* _temp6602;}).last_line)); _temp6590;}); _temp6589;});
break; case 328: _LL6588: yyval=({ struct _tagged_ptr2 _temp6604= yyvs; struct
_xtunion_struct** _temp6606= _temp6604.curr + yyvsp_offset; if( _temp6604.base
== 0? 1:( _temp6606 < _temp6604.base? 1: _temp6606 >= _temp6604.last_plus_one)){
_throw( Null_Exception);}* _temp6606;}); break; case 329: _LL6603: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6608=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6608[ 0]=({ struct Cyc_Exp_tok_struct
_temp6609; _temp6609.tag= Cyc_Exp_tok_tag; _temp6609.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6610=
yyvs; struct _xtunion_struct** _temp6612= _temp6610.curr +( yyvsp_offset - 2);
if( _temp6610.base == 0? 1:( _temp6612 < _temp6610.base? 1: _temp6612 >=
_temp6610.last_plus_one)){ _throw( Null_Exception);}* _temp6612;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6613= yyvs; struct _xtunion_struct** _temp6615=
_temp6613.curr + yyvsp_offset; if( _temp6613.base == 0? 1:( _temp6615 <
_temp6613.base? 1: _temp6615 >= _temp6613.last_plus_one)){ _throw(
Null_Exception);}* _temp6615;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6616= yyls; struct Cyc_Yyltype* _temp6618= _temp6616.curr +(
yylsp_offset - 2); if( _temp6616.base == 0? 1:( _temp6618 < _temp6616.base? 1:
_temp6618 >= _temp6616.last_plus_one)){ _throw( Null_Exception);}* _temp6618;}).first_line,({
struct _tagged_ptr3 _temp6619= yyls; struct Cyc_Yyltype* _temp6621= _temp6619.curr
+ yylsp_offset; if( _temp6619.base == 0? 1:( _temp6621 < _temp6619.base? 1:
_temp6621 >= _temp6619.last_plus_one)){ _throw( Null_Exception);}* _temp6621;}).last_line));
_temp6609;}); _temp6608;}); break; case 330: _LL6607: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6623=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6623[ 0]=({ struct Cyc_Exp_tok_struct
_temp6624; _temp6624.tag= Cyc_Exp_tok_tag; _temp6624.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6625=
yyvs; struct _xtunion_struct** _temp6627= _temp6625.curr +( yyvsp_offset - 2);
if( _temp6625.base == 0? 1:( _temp6627 < _temp6625.base? 1: _temp6627 >=
_temp6625.last_plus_one)){ _throw( Null_Exception);}* _temp6627;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6628= yyvs; struct _xtunion_struct** _temp6630=
_temp6628.curr + yyvsp_offset; if( _temp6628.base == 0? 1:( _temp6630 <
_temp6628.base? 1: _temp6630 >= _temp6628.last_plus_one)){ _throw(
Null_Exception);}* _temp6630;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6631= yyls; struct Cyc_Yyltype* _temp6633= _temp6631.curr +(
yylsp_offset - 2); if( _temp6631.base == 0? 1:( _temp6633 < _temp6631.base? 1:
_temp6633 >= _temp6631.last_plus_one)){ _throw( Null_Exception);}* _temp6633;}).first_line,({
struct _tagged_ptr3 _temp6634= yyls; struct Cyc_Yyltype* _temp6636= _temp6634.curr
+ yylsp_offset; if( _temp6634.base == 0? 1:( _temp6636 < _temp6634.base? 1:
_temp6636 >= _temp6634.last_plus_one)){ _throw( Null_Exception);}* _temp6636;}).last_line));
_temp6624;}); _temp6623;}); break; case 331: _LL6622: yyval=({ struct
_tagged_ptr2 _temp6638= yyvs; struct _xtunion_struct** _temp6640= _temp6638.curr
+ yyvsp_offset; if( _temp6638.base == 0? 1:( _temp6640 < _temp6638.base? 1:
_temp6640 >= _temp6638.last_plus_one)){ _throw( Null_Exception);}* _temp6640;});
break; case 332: _LL6637: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6642=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6642[ 0]=({ struct Cyc_Exp_tok_struct _temp6643; _temp6643.tag= Cyc_Exp_tok_tag;
_temp6643.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6644= yyvs; struct _xtunion_struct** _temp6646=
_temp6644.curr +( yyvsp_offset - 2); if( _temp6644.base == 0? 1:( _temp6646 <
_temp6644.base? 1: _temp6646 >= _temp6644.last_plus_one)){ _throw(
Null_Exception);}* _temp6646;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6647= yyvs; struct _xtunion_struct** _temp6649= _temp6647.curr +
yyvsp_offset; if( _temp6647.base == 0? 1:( _temp6649 < _temp6647.base? 1:
_temp6649 >= _temp6647.last_plus_one)){ _throw( Null_Exception);}* _temp6649;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6650= yyls; struct Cyc_Yyltype*
_temp6652= _temp6650.curr +( yylsp_offset - 2); if( _temp6650.base == 0? 1:(
_temp6652 < _temp6650.base? 1: _temp6652 >= _temp6650.last_plus_one)){ _throw(
Null_Exception);}* _temp6652;}).first_line,({ struct _tagged_ptr3 _temp6653=
yyls; struct Cyc_Yyltype* _temp6655= _temp6653.curr + yylsp_offset; if(
_temp6653.base == 0? 1:( _temp6655 < _temp6653.base? 1: _temp6655 >= _temp6653.last_plus_one)){
_throw( Null_Exception);}* _temp6655;}).last_line)); _temp6643;}); _temp6642;});
break; case 333: _LL6641: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6657=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6657[ 0]=({ struct Cyc_Exp_tok_struct _temp6658; _temp6658.tag= Cyc_Exp_tok_tag;
_temp6658.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6659= yyvs; struct _xtunion_struct** _temp6661=
_temp6659.curr +( yyvsp_offset - 2); if( _temp6659.base == 0? 1:( _temp6661 <
_temp6659.base? 1: _temp6661 >= _temp6659.last_plus_one)){ _throw(
Null_Exception);}* _temp6661;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6662= yyvs; struct _xtunion_struct** _temp6664= _temp6662.curr +
yyvsp_offset; if( _temp6662.base == 0? 1:( _temp6664 < _temp6662.base? 1:
_temp6664 >= _temp6662.last_plus_one)){ _throw( Null_Exception);}* _temp6664;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6665= yyls; struct Cyc_Yyltype*
_temp6667= _temp6665.curr +( yylsp_offset - 2); if( _temp6665.base == 0? 1:(
_temp6667 < _temp6665.base? 1: _temp6667 >= _temp6665.last_plus_one)){ _throw(
Null_Exception);}* _temp6667;}).first_line,({ struct _tagged_ptr3 _temp6668=
yyls; struct Cyc_Yyltype* _temp6670= _temp6668.curr + yylsp_offset; if(
_temp6668.base == 0? 1:( _temp6670 < _temp6668.base? 1: _temp6670 >= _temp6668.last_plus_one)){
_throw( Null_Exception);}* _temp6670;}).last_line)); _temp6658;}); _temp6657;});
break; case 334: _LL6656: yyval=({ struct _tagged_ptr2 _temp6672= yyvs; struct
_xtunion_struct** _temp6674= _temp6672.curr + yyvsp_offset; if( _temp6672.base
== 0? 1:( _temp6674 < _temp6672.base? 1: _temp6674 >= _temp6672.last_plus_one)){
_throw( Null_Exception);}* _temp6674;}); break; case 335: _LL6671: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6676=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6676[ 0]=({ struct Cyc_Exp_tok_struct
_temp6677; _temp6677.tag= Cyc_Exp_tok_tag; _temp6677.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Times, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6678= yyvs;
struct _xtunion_struct** _temp6680= _temp6678.curr +( yyvsp_offset - 2); if(
_temp6678.base == 0? 1:( _temp6680 < _temp6678.base? 1: _temp6680 >= _temp6678.last_plus_one)){
_throw( Null_Exception);}* _temp6680;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6681= yyvs; struct _xtunion_struct** _temp6683= _temp6681.curr
+ yyvsp_offset; if( _temp6681.base == 0? 1:( _temp6683 < _temp6681.base? 1:
_temp6683 >= _temp6681.last_plus_one)){ _throw( Null_Exception);}* _temp6683;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6684= yyls; struct Cyc_Yyltype*
_temp6686= _temp6684.curr +( yylsp_offset - 2); if( _temp6684.base == 0? 1:(
_temp6686 < _temp6684.base? 1: _temp6686 >= _temp6684.last_plus_one)){ _throw(
Null_Exception);}* _temp6686;}).first_line,({ struct _tagged_ptr3 _temp6687=
yyls; struct Cyc_Yyltype* _temp6689= _temp6687.curr + yylsp_offset; if(
_temp6687.base == 0? 1:( _temp6689 < _temp6687.base? 1: _temp6689 >= _temp6687.last_plus_one)){
_throw( Null_Exception);}* _temp6689;}).last_line)); _temp6677;}); _temp6676;});
break; case 336: _LL6675: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6691=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6691[ 0]=({ struct Cyc_Exp_tok_struct _temp6692; _temp6692.tag= Cyc_Exp_tok_tag;
_temp6692.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6693= yyvs; struct _xtunion_struct** _temp6695=
_temp6693.curr +( yyvsp_offset - 2); if( _temp6693.base == 0? 1:( _temp6695 <
_temp6693.base? 1: _temp6695 >= _temp6693.last_plus_one)){ _throw(
Null_Exception);}* _temp6695;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6696= yyvs; struct _xtunion_struct** _temp6698= _temp6696.curr +
yyvsp_offset; if( _temp6696.base == 0? 1:( _temp6698 < _temp6696.base? 1:
_temp6698 >= _temp6696.last_plus_one)){ _throw( Null_Exception);}* _temp6698;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6699= yyls; struct Cyc_Yyltype*
_temp6701= _temp6699.curr +( yylsp_offset - 2); if( _temp6699.base == 0? 1:(
_temp6701 < _temp6699.base? 1: _temp6701 >= _temp6699.last_plus_one)){ _throw(
Null_Exception);}* _temp6701;}).first_line,({ struct _tagged_ptr3 _temp6702=
yyls; struct Cyc_Yyltype* _temp6704= _temp6702.curr + yylsp_offset; if(
_temp6702.base == 0? 1:( _temp6704 < _temp6702.base? 1: _temp6704 >= _temp6702.last_plus_one)){
_throw( Null_Exception);}* _temp6704;}).last_line)); _temp6692;}); _temp6691;});
break; case 337: _LL6690: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6706=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6706[ 0]=({ struct Cyc_Exp_tok_struct _temp6707; _temp6707.tag= Cyc_Exp_tok_tag;
_temp6707.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6708= yyvs; struct _xtunion_struct** _temp6710=
_temp6708.curr +( yyvsp_offset - 2); if( _temp6708.base == 0? 1:( _temp6710 <
_temp6708.base? 1: _temp6710 >= _temp6708.last_plus_one)){ _throw(
Null_Exception);}* _temp6710;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6711= yyvs; struct _xtunion_struct** _temp6713= _temp6711.curr +
yyvsp_offset; if( _temp6711.base == 0? 1:( _temp6713 < _temp6711.base? 1:
_temp6713 >= _temp6711.last_plus_one)){ _throw( Null_Exception);}* _temp6713;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6714= yyls; struct Cyc_Yyltype*
_temp6716= _temp6714.curr +( yylsp_offset - 2); if( _temp6714.base == 0? 1:(
_temp6716 < _temp6714.base? 1: _temp6716 >= _temp6714.last_plus_one)){ _throw(
Null_Exception);}* _temp6716;}).first_line,({ struct _tagged_ptr3 _temp6717=
yyls; struct Cyc_Yyltype* _temp6719= _temp6717.curr + yylsp_offset; if(
_temp6717.base == 0? 1:( _temp6719 < _temp6717.base? 1: _temp6719 >= _temp6717.last_plus_one)){
_throw( Null_Exception);}* _temp6719;}).last_line)); _temp6707;}); _temp6706;});
break; case 338: _LL6705: yyval=({ struct _tagged_ptr2 _temp6721= yyvs; struct
_xtunion_struct** _temp6723= _temp6721.curr + yyvsp_offset; if( _temp6721.base
== 0? 1:( _temp6723 < _temp6721.base? 1: _temp6723 >= _temp6721.last_plus_one)){
_throw( Null_Exception);}* _temp6723;}); break; case 339: _LL6720: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6725=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6725[ 0]=({ struct Cyc_Exp_tok_struct
_temp6726; _temp6726.tag= Cyc_Exp_tok_tag; _temp6726.f1= Cyc_Absyn_cast_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6727= yyvs; struct
_xtunion_struct** _temp6729= _temp6727.curr +( yyvsp_offset - 2); if( _temp6727.base
== 0? 1:( _temp6729 < _temp6727.base? 1: _temp6729 >= _temp6727.last_plus_one)){
_throw( Null_Exception);}* _temp6729;}))).f3, Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6730= yyvs; struct _xtunion_struct** _temp6732= _temp6730.curr
+ yyvsp_offset; if( _temp6730.base == 0? 1:( _temp6732 < _temp6730.base? 1:
_temp6732 >= _temp6730.last_plus_one)){ _throw( Null_Exception);}* _temp6732;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6733= yyls; struct Cyc_Yyltype*
_temp6735= _temp6733.curr +( yylsp_offset - 3); if( _temp6733.base == 0? 1:(
_temp6735 < _temp6733.base? 1: _temp6735 >= _temp6733.last_plus_one)){ _throw(
Null_Exception);}* _temp6735;}).first_line,({ struct _tagged_ptr3 _temp6736=
yyls; struct Cyc_Yyltype* _temp6738= _temp6736.curr + yylsp_offset; if(
_temp6736.base == 0? 1:( _temp6738 < _temp6736.base? 1: _temp6738 >= _temp6736.last_plus_one)){
_throw( Null_Exception);}* _temp6738;}).last_line)); _temp6726;}); _temp6725;});
break; case 340: _LL6724: yyval=({ struct _tagged_ptr2 _temp6740= yyvs; struct
_xtunion_struct** _temp6742= _temp6740.curr + yyvsp_offset; if( _temp6740.base
== 0? 1:( _temp6742 < _temp6740.base? 1: _temp6742 >= _temp6740.last_plus_one)){
_throw( Null_Exception);}* _temp6742;}); break; case 341: _LL6739: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6744=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6744[ 0]=({ struct Cyc_Exp_tok_struct
_temp6745; _temp6745.tag= Cyc_Exp_tok_tag; _temp6745.f1= Cyc_Absyn_pre_inc_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6746= yyvs; struct _xtunion_struct**
_temp6748= _temp6746.curr + yyvsp_offset; if( _temp6746.base == 0? 1:( _temp6748
< _temp6746.base? 1: _temp6748 >= _temp6746.last_plus_one)){ _throw(
Null_Exception);}* _temp6748;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6749= yyls; struct Cyc_Yyltype* _temp6751= _temp6749.curr +(
yylsp_offset - 1); if( _temp6749.base == 0? 1:( _temp6751 < _temp6749.base? 1:
_temp6751 >= _temp6749.last_plus_one)){ _throw( Null_Exception);}* _temp6751;}).first_line,({
struct _tagged_ptr3 _temp6752= yyls; struct Cyc_Yyltype* _temp6754= _temp6752.curr
+ yylsp_offset; if( _temp6752.base == 0? 1:( _temp6754 < _temp6752.base? 1:
_temp6754 >= _temp6752.last_plus_one)){ _throw( Null_Exception);}* _temp6754;}).last_line));
_temp6745;}); _temp6744;}); break; case 342: _LL6743: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6756=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6756[ 0]=({ struct Cyc_Exp_tok_struct
_temp6757; _temp6757.tag= Cyc_Exp_tok_tag; _temp6757.f1= Cyc_Absyn_pre_dec_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6758= yyvs; struct _xtunion_struct**
_temp6760= _temp6758.curr + yyvsp_offset; if( _temp6758.base == 0? 1:( _temp6760
< _temp6758.base? 1: _temp6760 >= _temp6758.last_plus_one)){ _throw(
Null_Exception);}* _temp6760;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6761= yyls; struct Cyc_Yyltype* _temp6763= _temp6761.curr +(
yylsp_offset - 1); if( _temp6761.base == 0? 1:( _temp6763 < _temp6761.base? 1:
_temp6763 >= _temp6761.last_plus_one)){ _throw( Null_Exception);}* _temp6763;}).first_line,({
struct _tagged_ptr3 _temp6764= yyls; struct Cyc_Yyltype* _temp6766= _temp6764.curr
+ yylsp_offset; if( _temp6764.base == 0? 1:( _temp6766 < _temp6764.base? 1:
_temp6766 >= _temp6764.last_plus_one)){ _throw( Null_Exception);}* _temp6766;}).last_line));
_temp6757;}); _temp6756;}); break; case 343: _LL6755: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6768=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6768[ 0]=({ struct Cyc_Exp_tok_struct
_temp6769; _temp6769.tag= Cyc_Exp_tok_tag; _temp6769.f1= Cyc_Absyn_address_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6770= yyvs; struct _xtunion_struct**
_temp6772= _temp6770.curr + yyvsp_offset; if( _temp6770.base == 0? 1:( _temp6772
< _temp6770.base? 1: _temp6772 >= _temp6770.last_plus_one)){ _throw(
Null_Exception);}* _temp6772;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6773= yyls; struct Cyc_Yyltype* _temp6775= _temp6773.curr +(
yylsp_offset - 1); if( _temp6773.base == 0? 1:( _temp6775 < _temp6773.base? 1:
_temp6775 >= _temp6773.last_plus_one)){ _throw( Null_Exception);}* _temp6775;}).first_line,({
struct _tagged_ptr3 _temp6776= yyls; struct Cyc_Yyltype* _temp6778= _temp6776.curr
+ yylsp_offset; if( _temp6776.base == 0? 1:( _temp6778 < _temp6776.base? 1:
_temp6778 >= _temp6776.last_plus_one)){ _throw( Null_Exception);}* _temp6778;}).last_line));
_temp6769;}); _temp6768;}); break; case 344: _LL6767: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6780=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6780[ 0]=({ struct Cyc_Exp_tok_struct
_temp6781; _temp6781.tag= Cyc_Exp_tok_tag; _temp6781.f1= Cyc_Absyn_deref_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6782= yyvs; struct _xtunion_struct**
_temp6784= _temp6782.curr + yyvsp_offset; if( _temp6782.base == 0? 1:( _temp6784
< _temp6782.base? 1: _temp6784 >= _temp6782.last_plus_one)){ _throw(
Null_Exception);}* _temp6784;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6785= yyls; struct Cyc_Yyltype* _temp6787= _temp6785.curr +(
yylsp_offset - 1); if( _temp6785.base == 0? 1:( _temp6787 < _temp6785.base? 1:
_temp6787 >= _temp6785.last_plus_one)){ _throw( Null_Exception);}* _temp6787;}).first_line,({
struct _tagged_ptr3 _temp6788= yyls; struct Cyc_Yyltype* _temp6790= _temp6788.curr
+ yylsp_offset; if( _temp6788.base == 0? 1:( _temp6790 < _temp6788.base? 1:
_temp6790 >= _temp6788.last_plus_one)){ _throw( Null_Exception);}* _temp6790;}).last_line));
_temp6781;}); _temp6780;}); break; case 345: _LL6779: yyval=({ struct
_tagged_ptr2 _temp6792= yyvs; struct _xtunion_struct** _temp6794= _temp6792.curr
+ yyvsp_offset; if( _temp6792.base == 0? 1:( _temp6794 < _temp6792.base? 1:
_temp6794 >= _temp6792.last_plus_one)){ _throw( Null_Exception);}* _temp6794;});
break; case 346: _LL6791: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6796=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6796[ 0]=({ struct Cyc_Exp_tok_struct _temp6797; _temp6797.tag= Cyc_Exp_tok_tag;
_temp6797.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(({ struct _tagged_ptr2
_temp6798= yyvs; struct _xtunion_struct** _temp6800= _temp6798.curr +(
yyvsp_offset - 1); if( _temp6798.base == 0? 1:( _temp6800 < _temp6798.base? 1:
_temp6800 >= _temp6798.last_plus_one)){ _throw( Null_Exception);}* _temp6800;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6801= yyvs; struct _xtunion_struct**
_temp6803= _temp6801.curr + yyvsp_offset; if( _temp6801.base == 0? 1:( _temp6803
< _temp6801.base? 1: _temp6803 >= _temp6801.last_plus_one)){ _throw(
Null_Exception);}* _temp6803;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6804= yyls; struct Cyc_Yyltype* _temp6806= _temp6804.curr +(
yylsp_offset - 1); if( _temp6804.base == 0? 1:( _temp6806 < _temp6804.base? 1:
_temp6806 >= _temp6804.last_plus_one)){ _throw( Null_Exception);}* _temp6806;}).first_line,({
struct _tagged_ptr3 _temp6807= yyls; struct Cyc_Yyltype* _temp6809= _temp6807.curr
+ yylsp_offset; if( _temp6807.base == 0? 1:( _temp6809 < _temp6807.base? 1:
_temp6809 >= _temp6807.last_plus_one)){ _throw( Null_Exception);}* _temp6809;}).last_line));
_temp6797;}); _temp6796;}); break; case 347: _LL6795: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6811=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6811[ 0]=({ struct Cyc_Exp_tok_struct
_temp6812; _temp6812.tag= Cyc_Exp_tok_tag; _temp6812.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6813= yyvs; struct
_xtunion_struct** _temp6815= _temp6813.curr +( yyvsp_offset - 1); if( _temp6813.base
== 0? 1:( _temp6815 < _temp6813.base? 1: _temp6815 >= _temp6813.last_plus_one)){
_throw( Null_Exception);}* _temp6815;}))).f3, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6816= yyls; struct Cyc_Yyltype* _temp6818= _temp6816.curr
+( yylsp_offset - 3); if( _temp6816.base == 0? 1:( _temp6818 < _temp6816.base? 1:
_temp6818 >= _temp6816.last_plus_one)){ _throw( Null_Exception);}* _temp6818;}).first_line,({
struct _tagged_ptr3 _temp6819= yyls; struct Cyc_Yyltype* _temp6821= _temp6819.curr
+ yylsp_offset; if( _temp6819.base == 0? 1:( _temp6821 < _temp6819.base? 1:
_temp6821 >= _temp6819.last_plus_one)){ _throw( Null_Exception);}* _temp6821;}).last_line));
_temp6812;}); _temp6811;}); break; case 348: _LL6810: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6823=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6823[ 0]=({ struct Cyc_Exp_tok_struct
_temp6824; _temp6824.tag= Cyc_Exp_tok_tag; _temp6824.f1= Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6825= yyvs; struct _xtunion_struct**
_temp6827= _temp6825.curr + yyvsp_offset; if( _temp6825.base == 0? 1:( _temp6827
< _temp6825.base? 1: _temp6827 >= _temp6825.last_plus_one)){ _throw(
Null_Exception);}* _temp6827;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6828= yyls; struct Cyc_Yyltype* _temp6830= _temp6828.curr +(
yylsp_offset - 1); if( _temp6828.base == 0? 1:( _temp6830 < _temp6828.base? 1:
_temp6830 >= _temp6828.last_plus_one)){ _throw( Null_Exception);}* _temp6830;}).first_line,({
struct _tagged_ptr3 _temp6831= yyls; struct Cyc_Yyltype* _temp6833= _temp6831.curr
+ yylsp_offset; if( _temp6831.base == 0? 1:( _temp6833 < _temp6831.base? 1:
_temp6833 >= _temp6831.last_plus_one)){ _throw( Null_Exception);}* _temp6833;}).last_line));
_temp6824;}); _temp6823;}); break; case 349: _LL6822: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6835=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6835[ 0]=({ struct Cyc_Exp_tok_struct
_temp6836; _temp6836.tag= Cyc_Exp_tok_tag; _temp6836.f1= Cyc_Absyn_primop_exp(
Cyc_yyget_Primop_tok(({ struct _tagged_ptr2 _temp6837= yyvs; struct
_xtunion_struct** _temp6839= _temp6837.curr +( yyvsp_offset - 3); if( _temp6837.base
== 0? 1:( _temp6839 < _temp6837.base? 1: _temp6839 >= _temp6837.last_plus_one)){
_throw( Null_Exception);}* _temp6839;})), Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp6840= yyvs; struct _xtunion_struct** _temp6842= _temp6840.curr
+( yyvsp_offset - 1); if( _temp6840.base == 0? 1:( _temp6842 < _temp6840.base? 1:
_temp6842 >= _temp6840.last_plus_one)){ _throw( Null_Exception);}* _temp6842;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6843= yyls; struct Cyc_Yyltype*
_temp6845= _temp6843.curr +( yylsp_offset - 3); if( _temp6843.base == 0? 1:(
_temp6845 < _temp6843.base? 1: _temp6845 >= _temp6843.last_plus_one)){ _throw(
Null_Exception);}* _temp6845;}).first_line,({ struct _tagged_ptr3 _temp6846=
yyls; struct Cyc_Yyltype* _temp6848= _temp6846.curr + yylsp_offset; if(
_temp6846.base == 0? 1:( _temp6848 < _temp6846.base? 1: _temp6848 >= _temp6846.last_plus_one)){
_throw( Null_Exception);}* _temp6848;}).last_line)); _temp6836;}); _temp6835;});
break; case 350: _LL6834: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6850=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6850[ 0]=({ struct Cyc_Exp_tok_struct _temp6851; _temp6851.tag= Cyc_Exp_tok_tag;
_temp6851.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp6852=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp6852[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp6853; _temp6853.tag= Cyc_Absyn_Malloc_e_tag;
_temp6853.f1= 0; _temp6853.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp6854= yyvs; struct _xtunion_struct** _temp6856=
_temp6854.curr +( yyvsp_offset - 2); if( _temp6854.base == 0? 1:( _temp6856 <
_temp6854.base? 1: _temp6856 >= _temp6854.last_plus_one)){ _throw(
Null_Exception);}* _temp6856;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6857= yyls; struct Cyc_Yyltype* _temp6859= _temp6857.curr +(
yylsp_offset - 2); if( _temp6857.base == 0? 1:( _temp6859 < _temp6857.base? 1:
_temp6859 >= _temp6857.last_plus_one)){ _throw( Null_Exception);}* _temp6859;}).first_line,({
struct _tagged_ptr3 _temp6860= yyls; struct Cyc_Yyltype* _temp6862= _temp6860.curr
+( yylsp_offset - 2); if( _temp6860.base == 0? 1:( _temp6862 < _temp6860.base? 1:
_temp6862 >= _temp6860.last_plus_one)){ _throw( Null_Exception);}* _temp6862;}).last_line));
_temp6853;}); _temp6852;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6863= yyls; struct Cyc_Yyltype* _temp6865= _temp6863.curr +( yylsp_offset -
6); if( _temp6863.base == 0? 1:( _temp6865 < _temp6863.base? 1: _temp6865 >=
_temp6863.last_plus_one)){ _throw( Null_Exception);}* _temp6865;}).first_line,({
struct _tagged_ptr3 _temp6866= yyls; struct Cyc_Yyltype* _temp6868= _temp6866.curr
+ yylsp_offset; if( _temp6866.base == 0? 1:( _temp6868 < _temp6866.base? 1:
_temp6868 >= _temp6866.last_plus_one)){ _throw( Null_Exception);}* _temp6868;}).last_line));
_temp6851;}); _temp6850;}); break; case 351: _LL6849: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6870=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6870[ 0]=({ struct Cyc_Exp_tok_struct
_temp6871; _temp6871.tag= Cyc_Exp_tok_tag; _temp6871.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Malloc_e_struct* _temp6872=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp6872[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp6873; _temp6873.tag= Cyc_Absyn_Malloc_e_tag;
_temp6873.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6874= yyvs; struct _xtunion_struct** _temp6876= _temp6874.curr +(
yyvsp_offset - 6); if( _temp6874.base == 0? 1:( _temp6876 < _temp6874.base? 1:
_temp6876 >= _temp6874.last_plus_one)){ _throw( Null_Exception);}* _temp6876;}));
_temp6873.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp6877= yyvs; struct _xtunion_struct** _temp6879=
_temp6877.curr +( yyvsp_offset - 2); if( _temp6877.base == 0? 1:( _temp6879 <
_temp6877.base? 1: _temp6879 >= _temp6877.last_plus_one)){ _throw(
Null_Exception);}* _temp6879;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6880= yyls; struct Cyc_Yyltype* _temp6882= _temp6880.curr +(
yylsp_offset - 2); if( _temp6880.base == 0? 1:( _temp6882 < _temp6880.base? 1:
_temp6882 >= _temp6880.last_plus_one)){ _throw( Null_Exception);}* _temp6882;}).first_line,({
struct _tagged_ptr3 _temp6883= yyls; struct Cyc_Yyltype* _temp6885= _temp6883.curr
+( yylsp_offset - 2); if( _temp6883.base == 0? 1:( _temp6885 < _temp6883.base? 1:
_temp6885 >= _temp6883.last_plus_one)){ _throw( Null_Exception);}* _temp6885;}).last_line));
_temp6873;}); _temp6872;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6886= yyls; struct Cyc_Yyltype* _temp6888= _temp6886.curr +( yylsp_offset -
8); if( _temp6886.base == 0? 1:( _temp6888 < _temp6886.base? 1: _temp6888 >=
_temp6886.last_plus_one)){ _throw( Null_Exception);}* _temp6888;}).first_line,({
struct _tagged_ptr3 _temp6889= yyls; struct Cyc_Yyltype* _temp6891= _temp6889.curr
+ yylsp_offset; if( _temp6889.base == 0? 1:( _temp6891 < _temp6889.base? 1:
_temp6891 >= _temp6889.last_plus_one)){ _throw( Null_Exception);}* _temp6891;}).last_line));
_temp6871;}); _temp6870;}); break; case 352: _LL6869: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6893=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6893[ 0]=({ struct Cyc_Primop_tok_struct
_temp6894; _temp6894.tag= Cyc_Primop_tok_tag; _temp6894.f1=( void*)(( void*) Cyc_Absyn_Printf);
_temp6894;}); _temp6893;}); break; case 353: _LL6892: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6896=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6896[ 0]=({ struct Cyc_Primop_tok_struct
_temp6897; _temp6897.tag= Cyc_Primop_tok_tag; _temp6897.f1=( void*)(( void*) Cyc_Absyn_Fprintf);
_temp6897;}); _temp6896;}); break; case 354: _LL6895: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6899=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6899[ 0]=({ struct Cyc_Primop_tok_struct
_temp6900; _temp6900.tag= Cyc_Primop_tok_tag; _temp6900.f1=( void*)(( void*) Cyc_Absyn_Xprintf);
_temp6900;}); _temp6899;}); break; case 355: _LL6898: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6902=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6902[ 0]=({ struct Cyc_Primop_tok_struct
_temp6903; _temp6903.tag= Cyc_Primop_tok_tag; _temp6903.f1=( void*)(( void*) Cyc_Absyn_Scanf);
_temp6903;}); _temp6902;}); break; case 356: _LL6901: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6905=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6905[ 0]=({ struct Cyc_Primop_tok_struct
_temp6906; _temp6906.tag= Cyc_Primop_tok_tag; _temp6906.f1=( void*)(( void*) Cyc_Absyn_Fscanf);
_temp6906;}); _temp6905;}); break; case 357: _LL6904: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6908=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6908[ 0]=({ struct Cyc_Primop_tok_struct
_temp6909; _temp6909.tag= Cyc_Primop_tok_tag; _temp6909.f1=( void*)(( void*) Cyc_Absyn_Sscanf);
_temp6909;}); _temp6908;}); break; case 358: _LL6907: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6911=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6911[ 0]=({ struct Cyc_Primop_tok_struct
_temp6912; _temp6912.tag= Cyc_Primop_tok_tag; _temp6912.f1=( void*)(( void*) Cyc_Absyn_Bitnot);
_temp6912;}); _temp6911;}); break; case 359: _LL6910: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6914=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6914[ 0]=({ struct Cyc_Primop_tok_struct
_temp6915; _temp6915.tag= Cyc_Primop_tok_tag; _temp6915.f1=( void*)(( void*) Cyc_Absyn_Not);
_temp6915;}); _temp6914;}); break; case 360: _LL6913: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6917=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6917[ 0]=({ struct Cyc_Primop_tok_struct
_temp6918; _temp6918.tag= Cyc_Primop_tok_tag; _temp6918.f1=( void*)(( void*) Cyc_Absyn_Minus);
_temp6918;}); _temp6917;}); break; case 361: _LL6916: yyval=({ struct
_tagged_ptr2 _temp6920= yyvs; struct _xtunion_struct** _temp6922= _temp6920.curr
+ yyvsp_offset; if( _temp6920.base == 0? 1:( _temp6922 < _temp6920.base? 1:
_temp6922 >= _temp6920.last_plus_one)){ _throw( Null_Exception);}* _temp6922;});
break; case 362: _LL6919: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6924=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6924[ 0]=({ struct Cyc_Exp_tok_struct _temp6925; _temp6925.tag= Cyc_Exp_tok_tag;
_temp6925.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6926= yyvs; struct _xtunion_struct** _temp6928= _temp6926.curr +(
yyvsp_offset - 3); if( _temp6926.base == 0? 1:( _temp6928 < _temp6926.base? 1:
_temp6928 >= _temp6926.last_plus_one)){ _throw( Null_Exception);}* _temp6928;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6929= yyvs; struct _xtunion_struct**
_temp6931= _temp6929.curr +( yyvsp_offset - 1); if( _temp6929.base == 0? 1:(
_temp6931 < _temp6929.base? 1: _temp6931 >= _temp6929.last_plus_one)){ _throw(
Null_Exception);}* _temp6931;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6932= yyls; struct Cyc_Yyltype* _temp6934= _temp6932.curr +(
yylsp_offset - 3); if( _temp6932.base == 0? 1:( _temp6934 < _temp6932.base? 1:
_temp6934 >= _temp6932.last_plus_one)){ _throw( Null_Exception);}* _temp6934;}).first_line,({
struct _tagged_ptr3 _temp6935= yyls; struct Cyc_Yyltype* _temp6937= _temp6935.curr
+ yylsp_offset; if( _temp6935.base == 0? 1:( _temp6937 < _temp6935.base? 1:
_temp6937 >= _temp6935.last_plus_one)){ _throw( Null_Exception);}* _temp6937;}).last_line));
_temp6925;}); _temp6924;}); break; case 363: _LL6923: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6939=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6939[ 0]=({ struct Cyc_Exp_tok_struct
_temp6940; _temp6940.tag= Cyc_Exp_tok_tag; _temp6940.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6941= yyvs; struct _xtunion_struct**
_temp6943= _temp6941.curr +( yyvsp_offset - 2); if( _temp6941.base == 0? 1:(
_temp6943 < _temp6941.base? 1: _temp6943 >= _temp6941.last_plus_one)){ _throw(
Null_Exception);}* _temp6943;})), 0, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6944= yyls; struct Cyc_Yyltype* _temp6946= _temp6944.curr +(
yylsp_offset - 2); if( _temp6944.base == 0? 1:( _temp6946 < _temp6944.base? 1:
_temp6946 >= _temp6944.last_plus_one)){ _throw( Null_Exception);}* _temp6946;}).first_line,({
struct _tagged_ptr3 _temp6947= yyls; struct Cyc_Yyltype* _temp6949= _temp6947.curr
+ yylsp_offset; if( _temp6947.base == 0? 1:( _temp6949 < _temp6947.base? 1:
_temp6949 >= _temp6947.last_plus_one)){ _throw( Null_Exception);}* _temp6949;}).last_line));
_temp6940;}); _temp6939;}); break; case 364: _LL6938: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6951=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6951[ 0]=({ struct Cyc_Exp_tok_struct
_temp6952; _temp6952.tag= Cyc_Exp_tok_tag; _temp6952.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6953= yyvs; struct _xtunion_struct**
_temp6955= _temp6953.curr +( yyvsp_offset - 3); if( _temp6953.base == 0? 1:(
_temp6955 < _temp6953.base? 1: _temp6955 >= _temp6953.last_plus_one)){ _throw(
Null_Exception);}* _temp6955;})), Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2
_temp6956= yyvs; struct _xtunion_struct** _temp6958= _temp6956.curr +(
yyvsp_offset - 1); if( _temp6956.base == 0? 1:( _temp6958 < _temp6956.base? 1:
_temp6958 >= _temp6956.last_plus_one)){ _throw( Null_Exception);}* _temp6958;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6959= yyls; struct Cyc_Yyltype*
_temp6961= _temp6959.curr +( yylsp_offset - 3); if( _temp6959.base == 0? 1:(
_temp6961 < _temp6959.base? 1: _temp6961 >= _temp6959.last_plus_one)){ _throw(
Null_Exception);}* _temp6961;}).first_line,({ struct _tagged_ptr3 _temp6962=
yyls; struct Cyc_Yyltype* _temp6964= _temp6962.curr + yylsp_offset; if(
_temp6962.base == 0? 1:( _temp6964 < _temp6962.base? 1: _temp6964 >= _temp6962.last_plus_one)){
_throw( Null_Exception);}* _temp6964;}).last_line)); _temp6952;}); _temp6951;});
break; case 365: _LL6950: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6966=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6966[ 0]=({ struct Cyc_Exp_tok_struct _temp6967; _temp6967.tag= Cyc_Exp_tok_tag;
_temp6967.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6968= yyvs; struct _xtunion_struct** _temp6970= _temp6968.curr
+( yyvsp_offset - 2); if( _temp6968.base == 0? 1:( _temp6970 < _temp6968.base? 1:
_temp6970 >= _temp6968.last_plus_one)){ _throw( Null_Exception);}* _temp6970;})),({
struct _tagged_string* _temp6971=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp6971[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp6972= yyvs; struct _xtunion_struct** _temp6974= _temp6972.curr
+ yyvsp_offset; if( _temp6972.base == 0? 1:( _temp6974 < _temp6972.base? 1:
_temp6974 >= _temp6972.last_plus_one)){ _throw( Null_Exception);}* _temp6974;}));
_temp6971;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6975= yyls;
struct Cyc_Yyltype* _temp6977= _temp6975.curr +( yylsp_offset - 2); if(
_temp6975.base == 0? 1:( _temp6977 < _temp6975.base? 1: _temp6977 >= _temp6975.last_plus_one)){
_throw( Null_Exception);}* _temp6977;}).first_line,({ struct _tagged_ptr3
_temp6978= yyls; struct Cyc_Yyltype* _temp6980= _temp6978.curr + yylsp_offset;
if( _temp6978.base == 0? 1:( _temp6980 < _temp6978.base? 1: _temp6980 >=
_temp6978.last_plus_one)){ _throw( Null_Exception);}* _temp6980;}).last_line));
_temp6967;}); _temp6966;}); break; case 366: _LL6965: { struct _tuple1* q= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp7015= yyvs; struct _xtunion_struct** _temp7017=
_temp7015.curr + yyvsp_offset; if( _temp7015.base == 0? 1:( _temp7017 <
_temp7015.base? 1: _temp7017 >= _temp7015.last_plus_one)){ _throw(
Null_Exception);}* _temp7017;}));{ void* _temp6982=(* q).f1; struct Cyc_List_List*
_temp6992; struct Cyc_List_List* _temp6994; _LL6984: if(( int) _temp6982 == Cyc_Absyn_Loc_n){
goto _LL6985;} else{ goto _LL6986;} _LL6986: if(( unsigned int) _temp6982 > 1u?((
struct _tunion_struct*) _temp6982)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL6993:
_temp6992=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp6982)->f1;
if( _temp6992 == 0){ goto _LL6987;} else{ goto _LL6988;}} else{ goto _LL6988;}
_LL6988: if(( unsigned int) _temp6982 > 1u?(( struct _tunion_struct*) _temp6982)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6995: _temp6994=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6982)->f1; if( _temp6994 == 0){ goto _LL6989;}
else{ goto _LL6990;}} else{ goto _LL6990;} _LL6990: goto _LL6991; _LL6985: goto
_LL6983; _LL6987: goto _LL6983; _LL6989: goto _LL6983; _LL6991: Cyc_Parse_err((
struct _tagged_string)({ char* _temp6996=( char*)"struct field name is qualified";
struct _tagged_string _temp6997; _temp6997.curr= _temp6996; _temp6997.base=
_temp6996; _temp6997.last_plus_one= _temp6996 + 31; _temp6997;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6998= yyls; struct Cyc_Yyltype* _temp7000= _temp6998.curr
+ yylsp_offset; if( _temp6998.base == 0? 1:( _temp7000 < _temp6998.base? 1:
_temp7000 >= _temp6998.last_plus_one)){ _throw( Null_Exception);}* _temp7000;}).first_line,({
struct _tagged_ptr3 _temp7001= yyls; struct Cyc_Yyltype* _temp7003= _temp7001.curr
+ yylsp_offset; if( _temp7001.base == 0? 1:( _temp7003 < _temp7001.base? 1:
_temp7003 >= _temp7001.last_plus_one)){ _throw( Null_Exception);}* _temp7003;}).last_line));
goto _LL6983; _LL6983:;} yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7004=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7004[ 0]=({ struct Cyc_Exp_tok_struct _temp7005; _temp7005.tag= Cyc_Exp_tok_tag;
_temp7005.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp7006= yyvs; struct _xtunion_struct** _temp7008= _temp7006.curr
+( yyvsp_offset - 2); if( _temp7006.base == 0? 1:( _temp7008 < _temp7006.base? 1:
_temp7008 >= _temp7006.last_plus_one)){ _throw( Null_Exception);}* _temp7008;})),(*
q).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7009= yyls;
struct Cyc_Yyltype* _temp7011= _temp7009.curr +( yylsp_offset - 2); if(
_temp7009.base == 0? 1:( _temp7011 < _temp7009.base? 1: _temp7011 >= _temp7009.last_plus_one)){
_throw( Null_Exception);}* _temp7011;}).first_line,({ struct _tagged_ptr3
_temp7012= yyls; struct Cyc_Yyltype* _temp7014= _temp7012.curr + yylsp_offset;
if( _temp7012.base == 0? 1:( _temp7014 < _temp7012.base? 1: _temp7014 >=
_temp7012.last_plus_one)){ _throw( Null_Exception);}* _temp7014;}).last_line));
_temp7005;}); _temp7004;}); break;} case 367: _LL6981: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7019=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7019[ 0]=({ struct Cyc_Exp_tok_struct
_temp7020; _temp7020.tag= Cyc_Exp_tok_tag; _temp7020.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7021= yyvs; struct _xtunion_struct**
_temp7023= _temp7021.curr +( yyvsp_offset - 2); if( _temp7021.base == 0? 1:(
_temp7023 < _temp7021.base? 1: _temp7023 >= _temp7021.last_plus_one)){ _throw(
Null_Exception);}* _temp7023;})),({ struct _tagged_string* _temp7024=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp7024[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp7025= yyvs; struct
_xtunion_struct** _temp7027= _temp7025.curr + yyvsp_offset; if( _temp7025.base
== 0? 1:( _temp7027 < _temp7025.base? 1: _temp7027 >= _temp7025.last_plus_one)){
_throw( Null_Exception);}* _temp7027;})); _temp7024;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7028= yyls; struct Cyc_Yyltype* _temp7030= _temp7028.curr
+( yylsp_offset - 2); if( _temp7028.base == 0? 1:( _temp7030 < _temp7028.base? 1:
_temp7030 >= _temp7028.last_plus_one)){ _throw( Null_Exception);}* _temp7030;}).first_line,({
struct _tagged_ptr3 _temp7031= yyls; struct Cyc_Yyltype* _temp7033= _temp7031.curr
+ yylsp_offset; if( _temp7031.base == 0? 1:( _temp7033 < _temp7031.base? 1:
_temp7033 >= _temp7031.last_plus_one)){ _throw( Null_Exception);}* _temp7033;}).last_line));
_temp7020;}); _temp7019;}); break; case 368: _LL7018: { struct _tuple1* q= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp7068= yyvs; struct _xtunion_struct** _temp7070=
_temp7068.curr + yyvsp_offset; if( _temp7068.base == 0? 1:( _temp7070 <
_temp7068.base? 1: _temp7070 >= _temp7068.last_plus_one)){ _throw(
Null_Exception);}* _temp7070;}));{ void* _temp7035=(* q).f1; struct Cyc_List_List*
_temp7045; struct Cyc_List_List* _temp7047; _LL7037: if(( int) _temp7035 == Cyc_Absyn_Loc_n){
goto _LL7038;} else{ goto _LL7039;} _LL7039: if(( unsigned int) _temp7035 > 1u?((
struct _tunion_struct*) _temp7035)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL7046:
_temp7045=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp7035)->f1;
if( _temp7045 == 0){ goto _LL7040;} else{ goto _LL7041;}} else{ goto _LL7041;}
_LL7041: if(( unsigned int) _temp7035 > 1u?(( struct _tunion_struct*) _temp7035)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL7048: _temp7047=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp7035)->f1; if( _temp7047 == 0){ goto _LL7042;}
else{ goto _LL7043;}} else{ goto _LL7043;} _LL7043: goto _LL7044; _LL7038: goto
_LL7036; _LL7040: goto _LL7036; _LL7042: goto _LL7036; _LL7044: Cyc_Parse_err((
struct _tagged_string)({ char* _temp7049=( char*)"struct field is qualified with module name";
struct _tagged_string _temp7050; _temp7050.curr= _temp7049; _temp7050.base=
_temp7049; _temp7050.last_plus_one= _temp7049 + 43; _temp7050;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7051= yyls; struct Cyc_Yyltype* _temp7053= _temp7051.curr
+ yylsp_offset; if( _temp7051.base == 0? 1:( _temp7053 < _temp7051.base? 1:
_temp7053 >= _temp7051.last_plus_one)){ _throw( Null_Exception);}* _temp7053;}).first_line,({
struct _tagged_ptr3 _temp7054= yyls; struct Cyc_Yyltype* _temp7056= _temp7054.curr
+ yylsp_offset; if( _temp7054.base == 0? 1:( _temp7056 < _temp7054.base? 1:
_temp7056 >= _temp7054.last_plus_one)){ _throw( Null_Exception);}* _temp7056;}).last_line));
goto _LL7036; _LL7036:;} yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7057=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7057[ 0]=({ struct Cyc_Exp_tok_struct _temp7058; _temp7058.tag= Cyc_Exp_tok_tag;
_temp7058.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp7059= yyvs; struct _xtunion_struct** _temp7061= _temp7059.curr
+( yyvsp_offset - 2); if( _temp7059.base == 0? 1:( _temp7061 < _temp7059.base? 1:
_temp7061 >= _temp7059.last_plus_one)){ _throw( Null_Exception);}* _temp7061;})),(*
q).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7062= yyls;
struct Cyc_Yyltype* _temp7064= _temp7062.curr +( yylsp_offset - 2); if(
_temp7062.base == 0? 1:( _temp7064 < _temp7062.base? 1: _temp7064 >= _temp7062.last_plus_one)){
_throw( Null_Exception);}* _temp7064;}).first_line,({ struct _tagged_ptr3
_temp7065= yyls; struct Cyc_Yyltype* _temp7067= _temp7065.curr + yylsp_offset;
if( _temp7065.base == 0? 1:( _temp7067 < _temp7065.base? 1: _temp7067 >=
_temp7065.last_plus_one)){ _throw( Null_Exception);}* _temp7067;}).last_line));
_temp7058;}); _temp7057;}); break;} case 369: _LL7034: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7072=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7072[ 0]=({ struct Cyc_Exp_tok_struct
_temp7073; _temp7073.tag= Cyc_Exp_tok_tag; _temp7073.f1= Cyc_Absyn_post_inc_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7074= yyvs; struct _xtunion_struct**
_temp7076= _temp7074.curr +( yyvsp_offset - 1); if( _temp7074.base == 0? 1:(
_temp7076 < _temp7074.base? 1: _temp7076 >= _temp7074.last_plus_one)){ _throw(
Null_Exception);}* _temp7076;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7077= yyls; struct Cyc_Yyltype* _temp7079= _temp7077.curr +(
yylsp_offset - 1); if( _temp7077.base == 0? 1:( _temp7079 < _temp7077.base? 1:
_temp7079 >= _temp7077.last_plus_one)){ _throw( Null_Exception);}* _temp7079;}).first_line,({
struct _tagged_ptr3 _temp7080= yyls; struct Cyc_Yyltype* _temp7082= _temp7080.curr
+ yylsp_offset; if( _temp7080.base == 0? 1:( _temp7082 < _temp7080.base? 1:
_temp7082 >= _temp7080.last_plus_one)){ _throw( Null_Exception);}* _temp7082;}).last_line));
_temp7073;}); _temp7072;}); break; case 370: _LL7071: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7084=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7084[ 0]=({ struct Cyc_Exp_tok_struct
_temp7085; _temp7085.tag= Cyc_Exp_tok_tag; _temp7085.f1= Cyc_Absyn_post_dec_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7086= yyvs; struct _xtunion_struct**
_temp7088= _temp7086.curr +( yyvsp_offset - 1); if( _temp7086.base == 0? 1:(
_temp7088 < _temp7086.base? 1: _temp7088 >= _temp7086.last_plus_one)){ _throw(
Null_Exception);}* _temp7088;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7089= yyls; struct Cyc_Yyltype* _temp7091= _temp7089.curr +(
yylsp_offset - 1); if( _temp7089.base == 0? 1:( _temp7091 < _temp7089.base? 1:
_temp7091 >= _temp7089.last_plus_one)){ _throw( Null_Exception);}* _temp7091;}).first_line,({
struct _tagged_ptr3 _temp7092= yyls; struct Cyc_Yyltype* _temp7094= _temp7092.curr
+ yylsp_offset; if( _temp7092.base == 0? 1:( _temp7094 < _temp7092.base? 1:
_temp7094 >= _temp7092.last_plus_one)){ _throw( Null_Exception);}* _temp7094;}).last_line));
_temp7085;}); _temp7084;}); break; case 371: _LL7083: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7096=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7096[ 0]=({ struct Cyc_Exp_tok_struct
_temp7097; _temp7097.tag= Cyc_Exp_tok_tag; _temp7097.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_CompoundLit_e_struct* _temp7098=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp7098[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp7099; _temp7099.tag= Cyc_Absyn_CompoundLit_e_tag;
_temp7099.f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp7100= yyvs;
struct _xtunion_struct** _temp7102= _temp7100.curr +( yyvsp_offset - 4); if(
_temp7100.base == 0? 1:( _temp7102 < _temp7100.base? 1: _temp7102 >= _temp7100.last_plus_one)){
_throw( Null_Exception);}* _temp7102;})); _temp7099.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp7103= yyvs; struct _xtunion_struct** _temp7105=
_temp7103.curr +( yyvsp_offset - 1); if( _temp7103.base == 0? 1:( _temp7105 <
_temp7103.base? 1: _temp7105 >= _temp7103.last_plus_one)){ _throw(
Null_Exception);}* _temp7105;}))); _temp7099;}); _temp7098;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7106= yyls; struct Cyc_Yyltype* _temp7108= _temp7106.curr
+( yylsp_offset - 5); if( _temp7106.base == 0? 1:( _temp7108 < _temp7106.base? 1:
_temp7108 >= _temp7106.last_plus_one)){ _throw( Null_Exception);}* _temp7108;}).first_line,({
struct _tagged_ptr3 _temp7109= yyls; struct Cyc_Yyltype* _temp7111= _temp7109.curr
+ yylsp_offset; if( _temp7109.base == 0? 1:( _temp7111 < _temp7109.base? 1:
_temp7111 >= _temp7109.last_plus_one)){ _throw( Null_Exception);}* _temp7111;}).last_line));
_temp7097;}); _temp7096;}); break; case 372: _LL7095: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7113=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7113[ 0]=({ struct Cyc_Exp_tok_struct
_temp7114; _temp7114.tag= Cyc_Exp_tok_tag; _temp7114.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_CompoundLit_e_struct* _temp7115=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp7115[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp7116; _temp7116.tag= Cyc_Absyn_CompoundLit_e_tag;
_temp7116.f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp7117= yyvs;
struct _xtunion_struct** _temp7119= _temp7117.curr +( yyvsp_offset - 5); if(
_temp7117.base == 0? 1:( _temp7119 < _temp7117.base? 1: _temp7119 >= _temp7117.last_plus_one)){
_throw( Null_Exception);}* _temp7119;})); _temp7116.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp7120= yyvs; struct _xtunion_struct** _temp7122=
_temp7120.curr +( yyvsp_offset - 2); if( _temp7120.base == 0? 1:( _temp7122 <
_temp7120.base? 1: _temp7122 >= _temp7120.last_plus_one)){ _throw(
Null_Exception);}* _temp7122;}))); _temp7116;}); _temp7115;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7123= yyls; struct Cyc_Yyltype* _temp7125= _temp7123.curr
+( yylsp_offset - 6); if( _temp7123.base == 0? 1:( _temp7125 < _temp7123.base? 1:
_temp7125 >= _temp7123.last_plus_one)){ _throw( Null_Exception);}* _temp7125;}).first_line,({
struct _tagged_ptr3 _temp7126= yyls; struct Cyc_Yyltype* _temp7128= _temp7126.curr
+ yylsp_offset; if( _temp7126.base == 0? 1:( _temp7128 < _temp7126.base? 1:
_temp7128 >= _temp7126.last_plus_one)){ _throw( Null_Exception);}* _temp7128;}).last_line));
_temp7114;}); _temp7113;}); break; case 373: _LL7112: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7130=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7130[ 0]=({ struct Cyc_Exp_tok_struct
_temp7131; _temp7131.tag= Cyc_Exp_tok_tag; _temp7131.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Fill_e_struct* _temp7132=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp7132[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp7133; _temp7133.tag= Cyc_Absyn_Fill_e_tag; _temp7133.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp7134= yyvs; struct _xtunion_struct** _temp7136=
_temp7134.curr +( yyvsp_offset - 1); if( _temp7134.base == 0? 1:( _temp7136 <
_temp7134.base? 1: _temp7136 >= _temp7134.last_plus_one)){ _throw(
Null_Exception);}* _temp7136;})); _temp7133;}); _temp7132;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7137= yyls; struct Cyc_Yyltype* _temp7139= _temp7137.curr
+( yylsp_offset - 3); if( _temp7137.base == 0? 1:( _temp7139 < _temp7137.base? 1:
_temp7139 >= _temp7137.last_plus_one)){ _throw( Null_Exception);}* _temp7139;}).first_line,({
struct _tagged_ptr3 _temp7140= yyls; struct Cyc_Yyltype* _temp7142= _temp7140.curr
+ yylsp_offset; if( _temp7140.base == 0? 1:( _temp7142 < _temp7140.base? 1:
_temp7142 >= _temp7140.last_plus_one)){ _throw( Null_Exception);}* _temp7142;}).last_line));
_temp7131;}); _temp7130;}); break; case 374: _LL7129: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7144=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7144[ 0]=({ struct Cyc_Exp_tok_struct
_temp7145; _temp7145.tag= Cyc_Exp_tok_tag; _temp7145.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Codegen_e_struct* _temp7146=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp7146[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp7147; _temp7147.tag= Cyc_Absyn_Codegen_e_tag;
_temp7147.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp7148= yyvs;
struct _xtunion_struct** _temp7150= _temp7148.curr +( yyvsp_offset - 1); if(
_temp7148.base == 0? 1:( _temp7150 < _temp7148.base? 1: _temp7150 >= _temp7148.last_plus_one)){
_throw( Null_Exception);}* _temp7150;})); _temp7147;}); _temp7146;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7151= yyls; struct Cyc_Yyltype* _temp7153= _temp7151.curr
+( yylsp_offset - 3); if( _temp7151.base == 0? 1:( _temp7153 < _temp7151.base? 1:
_temp7153 >= _temp7151.last_plus_one)){ _throw( Null_Exception);}* _temp7153;}).first_line,({
struct _tagged_ptr3 _temp7154= yyls; struct Cyc_Yyltype* _temp7156= _temp7154.curr
+ yylsp_offset; if( _temp7154.base == 0? 1:( _temp7156 < _temp7154.base? 1:
_temp7156 >= _temp7154.last_plus_one)){ _throw( Null_Exception);}* _temp7156;}).last_line));
_temp7145;}); _temp7144;}); break; case 375: _LL7143: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7158=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7158[ 0]=({ struct Cyc_Exp_tok_struct
_temp7159; _temp7159.tag= Cyc_Exp_tok_tag; _temp7159.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp7160=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp7160[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp7161; _temp7161.tag= Cyc_Absyn_UnknownId_e_tag;
_temp7161.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp7162= yyvs;
struct _xtunion_struct** _temp7164= _temp7162.curr + yyvsp_offset; if( _temp7162.base
== 0? 1:( _temp7164 < _temp7162.base? 1: _temp7164 >= _temp7162.last_plus_one)){
_throw( Null_Exception);}* _temp7164;})); _temp7161;}); _temp7160;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7165= yyls; struct Cyc_Yyltype* _temp7167= _temp7165.curr
+ yylsp_offset; if( _temp7165.base == 0? 1:( _temp7167 < _temp7165.base? 1:
_temp7167 >= _temp7165.last_plus_one)){ _throw( Null_Exception);}* _temp7167;}).first_line,({
struct _tagged_ptr3 _temp7168= yyls; struct Cyc_Yyltype* _temp7170= _temp7168.curr
+ yylsp_offset; if( _temp7168.base == 0? 1:( _temp7170 < _temp7168.base? 1:
_temp7170 >= _temp7168.last_plus_one)){ _throw( Null_Exception);}* _temp7170;}).last_line));
_temp7159;}); _temp7158;}); break; case 376: _LL7157: yyval=({ struct
_tagged_ptr2 _temp7172= yyvs; struct _xtunion_struct** _temp7174= _temp7172.curr
+ yyvsp_offset; if( _temp7172.base == 0? 1:( _temp7174 < _temp7172.base? 1:
_temp7174 >= _temp7172.last_plus_one)){ _throw( Null_Exception);}* _temp7174;});
break; case 377: _LL7171: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7176=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7176[ 0]=({ struct Cyc_Exp_tok_struct _temp7177; _temp7177.tag= Cyc_Exp_tok_tag;
_temp7177.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp7178= yyvs; struct _xtunion_struct** _temp7180= _temp7178.curr +
yyvsp_offset; if( _temp7178.base == 0? 1:( _temp7180 < _temp7178.base? 1:
_temp7180 >= _temp7178.last_plus_one)){ _throw( Null_Exception);}* _temp7180;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7181= yyls; struct Cyc_Yyltype*
_temp7183= _temp7181.curr + yylsp_offset; if( _temp7181.base == 0? 1:( _temp7183
< _temp7181.base? 1: _temp7183 >= _temp7181.last_plus_one)){ _throw(
Null_Exception);}* _temp7183;}).first_line,({ struct _tagged_ptr3 _temp7184=
yyls; struct Cyc_Yyltype* _temp7186= _temp7184.curr + yylsp_offset; if(
_temp7184.base == 0? 1:( _temp7186 < _temp7184.base? 1: _temp7186 >= _temp7184.last_plus_one)){
_throw( Null_Exception);}* _temp7186;}).last_line)); _temp7177;}); _temp7176;});
break; case 378: _LL7175: yyval=({ struct _tagged_ptr2 _temp7188= yyvs; struct
_xtunion_struct** _temp7190= _temp7188.curr +( yyvsp_offset - 1); if( _temp7188.base
== 0? 1:( _temp7190 < _temp7188.base? 1: _temp7190 >= _temp7188.last_plus_one)){
_throw( Null_Exception);}* _temp7190;}); break; case 379: _LL7187: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7192=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7192[ 0]=({ struct Cyc_Exp_tok_struct
_temp7193; _temp7193.tag= Cyc_Exp_tok_tag; _temp7193.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp7194=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7194[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7195; _temp7195.tag=
Cyc_Absyn_UnknownId_e_tag; _temp7195.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp7196= yyvs; struct _xtunion_struct** _temp7198= _temp7196.curr
+( yyvsp_offset - 1); if( _temp7196.base == 0? 1:( _temp7198 < _temp7196.base? 1:
_temp7198 >= _temp7196.last_plus_one)){ _throw( Null_Exception);}* _temp7198;}));
_temp7195;}); _temp7194;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7199= yyls; struct Cyc_Yyltype* _temp7201= _temp7199.curr +( yylsp_offset -
1); if( _temp7199.base == 0? 1:( _temp7201 < _temp7199.base? 1: _temp7201 >=
_temp7199.last_plus_one)){ _throw( Null_Exception);}* _temp7201;}).first_line,({
struct _tagged_ptr3 _temp7202= yyls; struct Cyc_Yyltype* _temp7204= _temp7202.curr
+( yylsp_offset - 1); if( _temp7202.base == 0? 1:( _temp7204 < _temp7202.base? 1:
_temp7204 >= _temp7202.last_plus_one)){ _throw( Null_Exception);}* _temp7204;}).last_line)),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7205= yyls; struct Cyc_Yyltype*
_temp7207= _temp7205.curr +( yylsp_offset - 1); if( _temp7205.base == 0? 1:(
_temp7207 < _temp7205.base? 1: _temp7207 >= _temp7205.last_plus_one)){ _throw(
Null_Exception);}* _temp7207;}).first_line,({ struct _tagged_ptr3 _temp7208=
yyls; struct Cyc_Yyltype* _temp7210= _temp7208.curr + yylsp_offset; if(
_temp7208.base == 0? 1:( _temp7210 < _temp7208.base? 1: _temp7210 >= _temp7208.last_plus_one)){
_throw( Null_Exception);}* _temp7210;}).last_line)); _temp7193;}); _temp7192;});
break; case 380: _LL7191: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7212=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7212[ 0]=({ struct Cyc_Exp_tok_struct _temp7213; _temp7213.tag= Cyc_Exp_tok_tag;
_temp7213.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp7214=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7214[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7215; _temp7215.tag=
Cyc_Absyn_UnknownId_e_tag; _temp7215.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp7216= yyvs; struct _xtunion_struct** _temp7218= _temp7216.curr
+( yyvsp_offset - 4); if( _temp7216.base == 0? 1:( _temp7218 < _temp7216.base? 1:
_temp7218 >= _temp7216.last_plus_one)){ _throw( Null_Exception);}* _temp7218;}));
_temp7215;}); _temp7214;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7219= yyls; struct Cyc_Yyltype* _temp7221= _temp7219.curr +( yylsp_offset -
4); if( _temp7219.base == 0? 1:( _temp7221 < _temp7219.base? 1: _temp7221 >=
_temp7219.last_plus_one)){ _throw( Null_Exception);}* _temp7221;}).first_line,({
struct _tagged_ptr3 _temp7222= yyls; struct Cyc_Yyltype* _temp7224= _temp7222.curr
+( yylsp_offset - 4); if( _temp7222.base == 0? 1:( _temp7224 < _temp7222.base? 1:
_temp7224 >= _temp7222.last_plus_one)){ _throw( Null_Exception);}* _temp7224;}).last_line)),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp7225= yyvs; struct _xtunion_struct** _temp7227=
_temp7225.curr +( yyvsp_offset - 1); if( _temp7225.base == 0? 1:( _temp7227 <
_temp7225.base? 1: _temp7227 >= _temp7225.last_plus_one)){ _throw(
Null_Exception);}* _temp7227;}))), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7228= yyls; struct Cyc_Yyltype* _temp7230= _temp7228.curr +(
yylsp_offset - 4); if( _temp7228.base == 0? 1:( _temp7230 < _temp7228.base? 1:
_temp7230 >= _temp7228.last_plus_one)){ _throw( Null_Exception);}* _temp7230;}).first_line,({
struct _tagged_ptr3 _temp7231= yyls; struct Cyc_Yyltype* _temp7233= _temp7231.curr
+ yylsp_offset; if( _temp7231.base == 0? 1:( _temp7233 < _temp7231.base? 1:
_temp7233 >= _temp7231.last_plus_one)){ _throw( Null_Exception);}* _temp7233;}).last_line));
_temp7213;}); _temp7212;}); break; case 381: _LL7211: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7235=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7235[ 0]=({ struct Cyc_Exp_tok_struct
_temp7236; _temp7236.tag= Cyc_Exp_tok_tag; _temp7236.f1= Cyc_Absyn_tuple_exp(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp7237= yyvs; struct
_xtunion_struct** _temp7239= _temp7237.curr +( yyvsp_offset - 1); if( _temp7237.base
== 0? 1:( _temp7239 < _temp7237.base? 1: _temp7239 >= _temp7237.last_plus_one)){
_throw( Null_Exception);}* _temp7239;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7240= yyls; struct Cyc_Yyltype* _temp7242= _temp7240.curr +(
yylsp_offset - 3); if( _temp7240.base == 0? 1:( _temp7242 < _temp7240.base? 1:
_temp7242 >= _temp7240.last_plus_one)){ _throw( Null_Exception);}* _temp7242;}).first_line,({
struct _tagged_ptr3 _temp7243= yyls; struct Cyc_Yyltype* _temp7245= _temp7243.curr
+ yylsp_offset; if( _temp7243.base == 0? 1:( _temp7245 < _temp7243.base? 1:
_temp7245 >= _temp7243.last_plus_one)){ _throw( Null_Exception);}* _temp7245;}).last_line));
_temp7236;}); _temp7235;}); break; case 382: _LL7234: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7247=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7247[ 0]=({ struct Cyc_Exp_tok_struct
_temp7248; _temp7248.tag= Cyc_Exp_tok_tag; _temp7248.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Struct_e_struct* _temp7249=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp7249[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp7250; _temp7250.tag= Cyc_Absyn_Struct_e_tag;
_temp7250.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp7251= yyvs;
struct _xtunion_struct** _temp7253= _temp7251.curr +( yyvsp_offset - 3); if(
_temp7251.base == 0? 1:( _temp7253 < _temp7251.base? 1: _temp7253 >= _temp7251.last_plus_one)){
_throw( Null_Exception);}* _temp7253;})); _temp7250.f2= 0; _temp7250.f3=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp7254= yyvs; struct _xtunion_struct** _temp7256=
_temp7254.curr +( yyvsp_offset - 1); if( _temp7254.base == 0? 1:( _temp7256 <
_temp7254.base? 1: _temp7256 >= _temp7254.last_plus_one)){ _throw(
Null_Exception);}* _temp7256;}))); _temp7250.f4= 0; _temp7250;}); _temp7249;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7257= yyls; struct Cyc_Yyltype*
_temp7259= _temp7257.curr +( yylsp_offset - 3); if( _temp7257.base == 0? 1:(
_temp7259 < _temp7257.base? 1: _temp7259 >= _temp7257.last_plus_one)){ _throw(
Null_Exception);}* _temp7259;}).first_line,({ struct _tagged_ptr3 _temp7260=
yyls; struct Cyc_Yyltype* _temp7262= _temp7260.curr + yylsp_offset; if(
_temp7260.base == 0? 1:( _temp7262 < _temp7260.base? 1: _temp7262 >= _temp7260.last_plus_one)){
_throw( Null_Exception);}* _temp7262;}).last_line)); _temp7248;}); _temp7247;});
break; case 383: _LL7246: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7264=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7264[ 0]=({ struct Cyc_Exp_tok_struct _temp7265; _temp7265.tag= Cyc_Exp_tok_tag;
_temp7265.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp7266= yyvs; struct _xtunion_struct** _temp7268= _temp7266.curr +(
yyvsp_offset - 2); if( _temp7266.base == 0? 1:( _temp7268 < _temp7266.base? 1:
_temp7268 >= _temp7266.last_plus_one)){ _throw( Null_Exception);}* _temp7268;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7269= yyls; struct Cyc_Yyltype*
_temp7271= _temp7269.curr +( yylsp_offset - 4); if( _temp7269.base == 0? 1:(
_temp7271 < _temp7269.base? 1: _temp7271 >= _temp7269.last_plus_one)){ _throw(
Null_Exception);}* _temp7271;}).first_line,({ struct _tagged_ptr3 _temp7272=
yyls; struct Cyc_Yyltype* _temp7274= _temp7272.curr + yylsp_offset; if(
_temp7272.base == 0? 1:( _temp7274 < _temp7272.base? 1: _temp7274 >= _temp7272.last_plus_one)){
_throw( Null_Exception);}* _temp7274;}).last_line)); _temp7265;}); _temp7264;});
break; case 384: _LL7263: yyval=( struct _xtunion_struct*)({ struct Cyc_ExpList_tok_struct*
_temp7276=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp7276[ 0]=({ struct Cyc_ExpList_tok_struct _temp7277; _temp7277.tag= Cyc_ExpList_tok_tag;
_temp7277.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp7278= yyvs; struct
_xtunion_struct** _temp7280= _temp7278.curr + yyvsp_offset; if( _temp7278.base
== 0? 1:( _temp7280 < _temp7278.base? 1: _temp7280 >= _temp7278.last_plus_one)){
_throw( Null_Exception);}* _temp7280;}))); _temp7277;}); _temp7276;}); break;
case 385: _LL7275: yyval=( struct _xtunion_struct*)({ struct Cyc_ExpList_tok_struct*
_temp7282=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp7282[ 0]=({ struct Cyc_ExpList_tok_struct _temp7283; _temp7283.tag= Cyc_ExpList_tok_tag;
_temp7283.f1=({ struct Cyc_List_List* _temp7284=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp7284->hd=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp7285= yyvs; struct _xtunion_struct** _temp7287=
_temp7285.curr + yyvsp_offset; if( _temp7285.base == 0? 1:( _temp7287 <
_temp7285.base? 1: _temp7287 >= _temp7285.last_plus_one)){ _throw(
Null_Exception);}* _temp7287;})); _temp7284->tl= 0; _temp7284;}); _temp7283;});
_temp7282;}); break; case 386: _LL7281: yyval=( struct _xtunion_struct*)({
struct Cyc_ExpList_tok_struct* _temp7289=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp7289[ 0]=({ struct Cyc_ExpList_tok_struct
_temp7290; _temp7290.tag= Cyc_ExpList_tok_tag; _temp7290.f1=({ struct Cyc_List_List*
_temp7291=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp7291->hd=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7295= yyvs;
struct _xtunion_struct** _temp7297= _temp7295.curr + yyvsp_offset; if( _temp7295.base
== 0? 1:( _temp7297 < _temp7295.base? 1: _temp7297 >= _temp7295.last_plus_one)){
_throw( Null_Exception);}* _temp7297;})); _temp7291->tl= Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp7292= yyvs; struct _xtunion_struct** _temp7294=
_temp7292.curr +( yyvsp_offset - 2); if( _temp7292.base == 0? 1:( _temp7294 <
_temp7292.base? 1: _temp7294 >= _temp7292.last_plus_one)){ _throw(
Null_Exception);}* _temp7294;})); _temp7291;}); _temp7290;}); _temp7289;});
break; case 387: _LL7288: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7299=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7299[ 0]=({ struct Cyc_Exp_tok_struct _temp7300; _temp7300.tag= Cyc_Exp_tok_tag;
_temp7300.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(({ struct _tagged_ptr2
_temp7301= yyvs; struct _xtunion_struct** _temp7303= _temp7301.curr +
yyvsp_offset; if( _temp7301.base == 0? 1:( _temp7303 < _temp7301.base? 1:
_temp7303 >= _temp7301.last_plus_one)){ _throw( Null_Exception);}* _temp7303;}))).f1,(*
Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp7304= yyvs; struct _xtunion_struct**
_temp7306= _temp7304.curr + yyvsp_offset; if( _temp7304.base == 0? 1:( _temp7306
< _temp7304.base? 1: _temp7306 >= _temp7304.last_plus_one)){ _throw(
Null_Exception);}* _temp7306;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7307= yyls; struct Cyc_Yyltype* _temp7309= _temp7307.curr +
yylsp_offset; if( _temp7307.base == 0? 1:( _temp7309 < _temp7307.base? 1:
_temp7309 >= _temp7307.last_plus_one)){ _throw( Null_Exception);}* _temp7309;}).first_line,({
struct _tagged_ptr3 _temp7310= yyls; struct Cyc_Yyltype* _temp7312= _temp7310.curr
+ yylsp_offset; if( _temp7310.base == 0? 1:( _temp7312 < _temp7310.base? 1:
_temp7312 >= _temp7310.last_plus_one)){ _throw( Null_Exception);}* _temp7312;}).last_line));
_temp7300;}); _temp7299;}); break; case 388: _LL7298: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7314=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7314[ 0]=({ struct Cyc_Exp_tok_struct
_temp7315; _temp7315.tag= Cyc_Exp_tok_tag; _temp7315.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok(({
struct _tagged_ptr2 _temp7316= yyvs; struct _xtunion_struct** _temp7318=
_temp7316.curr + yyvsp_offset; if( _temp7316.base == 0? 1:( _temp7318 <
_temp7316.base? 1: _temp7318 >= _temp7316.last_plus_one)){ _throw(
Null_Exception);}* _temp7318;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7319= yyls; struct Cyc_Yyltype* _temp7321= _temp7319.curr +
yylsp_offset; if( _temp7319.base == 0? 1:( _temp7321 < _temp7319.base? 1:
_temp7321 >= _temp7319.last_plus_one)){ _throw( Null_Exception);}* _temp7321;}).first_line,({
struct _tagged_ptr3 _temp7322= yyls; struct Cyc_Yyltype* _temp7324= _temp7322.curr
+ yylsp_offset; if( _temp7322.base == 0? 1:( _temp7324 < _temp7322.base? 1:
_temp7324 >= _temp7322.last_plus_one)){ _throw( Null_Exception);}* _temp7324;}).last_line));
_temp7315;}); _temp7314;}); break; case 389: _LL7313: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7326=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7326[ 0]=({ struct Cyc_Exp_tok_struct
_temp7327; _temp7327.tag= Cyc_Exp_tok_tag; _temp7327.f1= Cyc_Absyn_float_exp(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp7328= yyvs; struct
_xtunion_struct** _temp7330= _temp7328.curr + yyvsp_offset; if( _temp7328.base
== 0? 1:( _temp7330 < _temp7328.base? 1: _temp7330 >= _temp7328.last_plus_one)){
_throw( Null_Exception);}* _temp7330;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7331= yyls; struct Cyc_Yyltype* _temp7333= _temp7331.curr +
yylsp_offset; if( _temp7331.base == 0? 1:( _temp7333 < _temp7331.base? 1:
_temp7333 >= _temp7331.last_plus_one)){ _throw( Null_Exception);}* _temp7333;}).first_line,({
struct _tagged_ptr3 _temp7334= yyls; struct Cyc_Yyltype* _temp7336= _temp7334.curr
+ yylsp_offset; if( _temp7334.base == 0? 1:( _temp7336 < _temp7334.base? 1:
_temp7336 >= _temp7334.last_plus_one)){ _throw( Null_Exception);}* _temp7336;}).last_line));
_temp7327;}); _temp7326;}); break; case 390: _LL7325: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7338=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7338[ 0]=({ struct Cyc_Exp_tok_struct
_temp7339; _temp7339.tag= Cyc_Exp_tok_tag; _temp7339.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7340= yyls; struct Cyc_Yyltype* _temp7342= _temp7340.curr
+ yylsp_offset; if( _temp7340.base == 0? 1:( _temp7342 < _temp7340.base? 1:
_temp7342 >= _temp7340.last_plus_one)){ _throw( Null_Exception);}* _temp7342;}).first_line,({
struct _tagged_ptr3 _temp7343= yyls; struct Cyc_Yyltype* _temp7345= _temp7343.curr
+ yylsp_offset; if( _temp7343.base == 0? 1:( _temp7345 < _temp7343.base? 1:
_temp7345 >= _temp7343.last_plus_one)){ _throw( Null_Exception);}* _temp7345;}).last_line));
_temp7339;}); _temp7338;}); break; case 391: _LL7337: yyval=( struct
_xtunion_struct*)({ struct Cyc_QualId_tok_struct* _temp7347=( struct Cyc_QualId_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualId_tok_struct)); _temp7347[ 0]=({ struct Cyc_QualId_tok_struct
_temp7348; _temp7348.tag= Cyc_QualId_tok_tag; _temp7348.f1=({ struct _tuple1*
_temp7349=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp7349->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp7354=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp7354[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp7355; _temp7355.tag= Cyc_Absyn_Rel_n_tag; _temp7355.f1= 0; _temp7355;});
_temp7354;}); _temp7349->f2=({ struct _tagged_string* _temp7350=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp7350[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp7351= yyvs; struct
_xtunion_struct** _temp7353= _temp7351.curr + yyvsp_offset; if( _temp7351.base
== 0? 1:( _temp7353 < _temp7351.base? 1: _temp7353 >= _temp7351.last_plus_one)){
_throw( Null_Exception);}* _temp7353;})); _temp7350;}); _temp7349;}); _temp7348;});
_temp7347;}); break; case 392: _LL7346: yyval=({ struct _tagged_ptr2 _temp7357=
yyvs; struct _xtunion_struct** _temp7359= _temp7357.curr + yyvsp_offset; if(
_temp7357.base == 0? 1:( _temp7359 < _temp7357.base? 1: _temp7359 >= _temp7357.last_plus_one)){
_throw( Null_Exception);}* _temp7359;}); break; default: _LL7356:( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Yyimpossible_struct* _temp7361=( struct
Cyc_Yyimpossible_struct*) GC_malloc( sizeof( struct Cyc_Yyimpossible_struct));
_temp7361[ 0]=({ struct Cyc_Yyimpossible_struct _temp7362; _temp7362.tag= Cyc_Yyimpossible_tag;
_temp7362;}); _temp7361;}));} yyvsp_offset -= yylen; yyssp_offset -= yylen;
yylsp_offset -= yylen;({ struct _tagged_ptr2 _temp7363= yyvs; struct
_xtunion_struct** _temp7365= _temp7363.curr +( ++ yyvsp_offset); if( _temp7363.base
== 0? 1:( _temp7365 < _temp7363.base? 1: _temp7365 >= _temp7363.last_plus_one)){
_throw( Null_Exception);}* _temp7365= yyval;}); yylsp_offset ++; if( yylen == 0){({
struct _tagged_ptr3 _temp7366= yyls; struct Cyc_Yyltype* _temp7368= _temp7366.curr
+ yylsp_offset; if( _temp7366.base == 0? 1:( _temp7368 < _temp7366.base? 1:
_temp7368 >= _temp7366.last_plus_one)){ _throw( Null_Exception);}(* _temp7368).first_line=
Cyc_yylloc.first_line;});({ struct _tagged_ptr3 _temp7369= yyls; struct Cyc_Yyltype*
_temp7371= _temp7369.curr + yylsp_offset; if( _temp7369.base == 0? 1:( _temp7371
< _temp7369.base? 1: _temp7371 >= _temp7369.last_plus_one)){ _throw(
Null_Exception);}(* _temp7371).first_column= Cyc_yylloc.first_column;});({
struct _tagged_ptr3 _temp7372= yyls; struct Cyc_Yyltype* _temp7374= _temp7372.curr
+ yylsp_offset; if( _temp7372.base == 0? 1:( _temp7374 < _temp7372.base? 1:
_temp7374 >= _temp7372.last_plus_one)){ _throw( Null_Exception);}(* _temp7374).last_line=({
struct _tagged_ptr3 _temp7375= yyls; struct Cyc_Yyltype* _temp7377= _temp7375.curr
+( yylsp_offset - 1); if( _temp7375.base == 0? 1:( _temp7377 < _temp7375.base? 1:
_temp7377 >= _temp7375.last_plus_one)){ _throw( Null_Exception);}* _temp7377;}).last_line;});({
struct _tagged_ptr3 _temp7378= yyls; struct Cyc_Yyltype* _temp7380= _temp7378.curr
+ yylsp_offset; if( _temp7378.base == 0? 1:( _temp7380 < _temp7378.base? 1:
_temp7380 >= _temp7378.last_plus_one)){ _throw( Null_Exception);}(* _temp7380).last_column=({
struct _tagged_ptr3 _temp7381= yyls; struct Cyc_Yyltype* _temp7383= _temp7381.curr
+( yylsp_offset - 1); if( _temp7381.base == 0? 1:( _temp7383 < _temp7381.base? 1:
_temp7383 >= _temp7381.last_plus_one)){ _throw( Null_Exception);}* _temp7383;}).last_column;});({
struct _tagged_ptr3 _temp7384= yyls; struct Cyc_Yyltype* _temp7386= _temp7384.curr
+ yylsp_offset; if( _temp7384.base == 0? 1:( _temp7386 < _temp7384.base? 1:
_temp7386 >= _temp7384.last_plus_one)){ _throw( Null_Exception);}(* _temp7386).text=(
struct _tagged_string)({ char* _temp7387=( char*)""; struct _tagged_string
_temp7388; _temp7388.curr= _temp7387; _temp7388.base= _temp7387; _temp7388.last_plus_one=
_temp7387 + 1; _temp7388;});});} else{({ struct _tagged_ptr3 _temp7389= yyls;
struct Cyc_Yyltype* _temp7391= _temp7389.curr + yylsp_offset; if( _temp7389.base
== 0? 1:( _temp7391 < _temp7389.base? 1: _temp7391 >= _temp7389.last_plus_one)){
_throw( Null_Exception);}(* _temp7391).last_line=({ struct _tagged_ptr3
_temp7392= yyls; struct Cyc_Yyltype* _temp7394= _temp7392.curr +(( yylsp_offset
+ yylen) - 1); if( _temp7392.base == 0? 1:( _temp7394 < _temp7392.base? 1:
_temp7394 >= _temp7392.last_plus_one)){ _throw( Null_Exception);}* _temp7394;}).last_line;});({
struct _tagged_ptr3 _temp7395= yyls; struct Cyc_Yyltype* _temp7397= _temp7395.curr
+ yylsp_offset; if( _temp7395.base == 0? 1:( _temp7397 < _temp7395.base? 1:
_temp7397 >= _temp7395.last_plus_one)){ _throw( Null_Exception);}(* _temp7397).last_column=({
struct _tagged_ptr3 _temp7398= yyls; struct Cyc_Yyltype* _temp7400= _temp7398.curr
+(( yylsp_offset + yylen) - 1); if( _temp7398.base == 0? 1:( _temp7400 <
_temp7398.base? 1: _temp7400 >= _temp7398.last_plus_one)){ _throw(
Null_Exception);}* _temp7400;}).last_column;});} yyn=( int)({ short* _temp7401=(
short*) Cyc_yyr1; unsigned int _temp7402= yyn; if( _temp7402 >= 393u){ _throw(
Null_Exception);} _temp7401[ _temp7402];}); yystate=({ short* _temp7403=( short*)
Cyc_yypgoto; unsigned int _temp7404= yyn - 124; if( _temp7404 >= 106u){ _throw(
Null_Exception);} _temp7403[ _temp7404];}) +({ struct _tagged_ptr1 _temp7405=
yyss; short* _temp7407= _temp7405.curr + yyssp_offset; if( _temp7405.base == 0?
1:( _temp7407 < _temp7405.base? 1: _temp7407 >= _temp7405.last_plus_one)){
_throw( Null_Exception);}* _temp7407;}); if(( yystate >= 0? yystate <= 4214: 0)?({
short* _temp7408=( short*) Cyc_yycheck; unsigned int _temp7409= yystate; if(
_temp7409 >= 4215u){ _throw( Null_Exception);} _temp7408[ _temp7409];}) ==({
struct _tagged_ptr1 _temp7410= yyss; short* _temp7412= _temp7410.curr +
yyssp_offset; if( _temp7410.base == 0? 1:( _temp7412 < _temp7410.base? 1:
_temp7412 >= _temp7410.last_plus_one)){ _throw( Null_Exception);}* _temp7412;}):
0){ yystate=( int)({ short* _temp7413=( short*) Cyc_yytable; unsigned int
_temp7414= yystate; if( _temp7414 >= 4215u){ _throw( Null_Exception);} _temp7413[
_temp7414];});} else{ yystate=( int)({ short* _temp7415=( short*) Cyc_yydefgoto;
unsigned int _temp7416= yyn - 124; if( _temp7416 >= 106u){ _throw(
Null_Exception);} _temp7415[ _temp7416];});} goto yynewstate; yyerrlab: if(
yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int)({ short* _temp7417=( short*) Cyc_yypact;
unsigned int _temp7418= yystate; if( _temp7418 >= 755u){ _throw( Null_Exception);}
_temp7417[ _temp7418];}); if( yyn > - 32768? yyn < 4214: 0){ int sze= 0; struct
_tagged_string msg; int x; int count; count= 0; for( x= yyn < 0? - yyn: 0; x <
230u / sizeof( char*); x ++){ if(({ short* _temp7419=( short*) Cyc_yycheck;
unsigned int _temp7420= x + yyn; if( _temp7420 >= 4215u){ _throw( Null_Exception);}
_temp7419[ _temp7420];}) == x){ sze += Cyc_String_strlen(({ struct
_tagged_string* _temp7421=( struct _tagged_string*) Cyc_yytname; unsigned int
_temp7422= x; if( _temp7422 >= 230u){ _throw( Null_Exception);} _temp7421[
_temp7422];})) + 15, count ++;}} msg=({ unsigned int _temp7423=( unsigned int)(
sze + 15); char* _temp7424=( char*) GC_malloc_atomic( sizeof( char) * _temp7423);
struct _tagged_string _temp7427={ _temp7424, _temp7424, _temp7424 + _temp7423};{
unsigned int _temp7425= _temp7423; unsigned int i; for( i= 0; i < _temp7425; i
++){ _temp7424[ i]='\000';}}; _temp7427;}); Cyc_String_strcpy( msg,( struct
_tagged_string)({ char* _temp7428=( char*)"parse error"; struct _tagged_string
_temp7429; _temp7429.curr= _temp7428; _temp7429.base= _temp7428; _temp7429.last_plus_one=
_temp7428 + 12; _temp7429;})); if( count < 5){ count= 0; for( x= yyn < 0? - yyn:
0; x < 230u / sizeof( char*); x ++){ if(({ short* _temp7430=( short*) Cyc_yycheck;
unsigned int _temp7431= x + yyn; if( _temp7431 >= 4215u){ _throw( Null_Exception);}
_temp7430[ _temp7431];}) == x){ Cyc_String_strcat( msg, count == 0?( struct
_tagged_string)({ char* _temp7432=( char*)", expecting `"; struct _tagged_string
_temp7433; _temp7433.curr= _temp7432; _temp7433.base= _temp7432; _temp7433.last_plus_one=
_temp7432 + 14; _temp7433;}):( struct _tagged_string)({ char* _temp7434=( char*)" or `";
struct _tagged_string _temp7435; _temp7435.curr= _temp7434; _temp7435.base=
_temp7434; _temp7435.last_plus_one= _temp7434 + 6; _temp7435;})); Cyc_String_strcat(
msg,({ struct _tagged_string* _temp7436=( struct _tagged_string*) Cyc_yytname;
unsigned int _temp7437= x; if( _temp7437 >= 230u){ _throw( Null_Exception);}
_temp7436[ _temp7437];})); Cyc_String_strcat( msg,( struct _tagged_string)({
char* _temp7438=( char*)"'"; struct _tagged_string _temp7439; _temp7439.curr=
_temp7438; _temp7439.base= _temp7438; _temp7439.last_plus_one= _temp7438 + 2;
_temp7439;})); count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror(( struct
_tagged_string)({ char* _temp7440=( char*)"parse error"; struct _tagged_string
_temp7441; _temp7441.curr= _temp7440; _temp7441.base= _temp7440; _temp7441.last_plus_one=
_temp7440 + 12; _temp7441;}));}} goto yyerrlab1; yyerrlab1: if( yyerrstatus == 3){
if( Cyc_yychar == 0){ return 1;} Cyc_yychar= - 2;} yyerrstatus= 3; goto
yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset == 0){ return 1;}
yyvsp_offset --; yystate=( int)({ struct _tagged_ptr1 _temp7442= yyss; short*
_temp7444= _temp7442.curr +( -- yyssp_offset); if( _temp7442.base == 0? 1:(
_temp7444 < _temp7442.base? 1: _temp7444 >= _temp7442.last_plus_one)){ _throw(
Null_Exception);}* _temp7444;}); yylsp_offset --; yyerrhandle: yyn=( int)({
short* _temp7445=( short*) Cyc_yypact; unsigned int _temp7446= yystate; if(
_temp7446 >= 755u){ _throw( Null_Exception);} _temp7445[ _temp7446];}); if( yyn
== - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn < 0? 1: yyn > 4214)? 1:({
short* _temp7447=( short*) Cyc_yycheck; unsigned int _temp7448= yyn; if(
_temp7448 >= 4215u){ _throw( Null_Exception);} _temp7447[ _temp7448];}) != 1){
goto yyerrdefault;} yyn=( int)({ short* _temp7449=( short*) Cyc_yytable;
unsigned int _temp7450= yyn; if( _temp7450 >= 4215u){ _throw( Null_Exception);}
_temp7449[ _temp7450];}); if( yyn < 0){ if( yyn == - 32768){ goto yyerrpop;} yyn=
- yyn; goto yyreduce;} else{ if( yyn == 0){ goto yyerrpop;}} if( yyn == 754){
return 0;}({ struct _tagged_ptr2 _temp7451= yyvs; struct _xtunion_struct**
_temp7453= _temp7451.curr +( ++ yyvsp_offset); if( _temp7451.base == 0? 1:(
_temp7453 < _temp7451.base? 1: _temp7453 >= _temp7451.last_plus_one)){ _throw(
Null_Exception);}* _temp7453= Cyc_yylval;});({ struct _tagged_ptr3 _temp7454=
yyls; struct Cyc_Yyltype* _temp7456= _temp7454.curr +( ++ yylsp_offset); if(
_temp7454.base == 0? 1:( _temp7456 < _temp7454.base? 1: _temp7456 >= _temp7454.last_plus_one)){
_throw( Null_Exception);}* _temp7456= Cyc_yylloc;}); goto yynewstate;} void Cyc_yyprint(
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