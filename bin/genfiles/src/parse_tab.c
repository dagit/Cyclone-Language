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
struct _tagged_ptr1 yyss=( struct _tagged_ptr1)({ short* _temp2147=( short*)({
unsigned int _temp2145= 200u; short* _temp2146=( short*) GC_malloc_atomic(
sizeof( short) * _temp2145); unsigned int i; for( i= 0; i < _temp2145; i ++){
_temp2146[ i]= (short)0;} _temp2146;}); struct _tagged_ptr1 _temp2148; _temp2148.curr=
_temp2147; _temp2148.base= _temp2147; _temp2148.last_plus_one= _temp2147 + 200;
_temp2148;}); int yyvsp_offset; struct _tagged_ptr2 yyvs=( struct _tagged_ptr2)({
struct _xtunion_struct** _temp2151=( struct _xtunion_struct**)({ unsigned int
_temp2149= 200u; struct _xtunion_struct** _temp2150=( struct _xtunion_struct**)
GC_malloc( sizeof( struct _xtunion_struct*) * _temp2149); unsigned int i; for( i=
0; i < _temp2149; i ++){ _temp2150[ i]= Cyc_yylval;} _temp2150;}); struct
_tagged_ptr2 _temp2152; _temp2152.curr= _temp2151; _temp2152.base= _temp2151;
_temp2152.last_plus_one= _temp2151 + 200; _temp2152;}); int yylsp_offset; struct
_tagged_ptr3 yyls=( struct _tagged_ptr3)({ struct Cyc_Yyltype* _temp2155=(
struct Cyc_Yyltype*)({ unsigned int _temp2153= 200u; struct Cyc_Yyltype*
_temp2154=( struct Cyc_Yyltype*) GC_malloc( sizeof( struct Cyc_Yyltype) *
_temp2153); unsigned int i; for( i= 0; i < _temp2153; i ++){ _temp2154[ i]= Cyc_yynewloc();}
_temp2154;}); struct _tagged_ptr3 _temp2156; _temp2156.curr= _temp2155;
_temp2156.base= _temp2155; _temp2156.last_plus_one= _temp2155 + 200; _temp2156;});
int yystacksize= 200; struct _xtunion_struct* yyval= Cyc_yylval; int yylen;
yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar= - 2; yyssp_offset= - 1;
yyvsp_offset= 0; yylsp_offset= 0; yynewstate:({ struct _tagged_ptr1 _temp2157=
yyss; short* _temp2159= _temp2157.curr +( ++ yyssp_offset); if( _temp2157.base
== 0? 1:( _temp2159 < _temp2157.base? 1: _temp2159 >= _temp2157.last_plus_one)){
_throw( Null_Exception);}* _temp2159=( short) yystate;}); if( yyssp_offset >=
yystacksize - 1){ if( yystacksize >= 10000){ Cyc_yyerror(( struct _tagged_string)({
char* _temp2160=( char*)"parser stack overflow"; struct _tagged_string _temp2161;
_temp2161.curr= _temp2160; _temp2161.base= _temp2160; _temp2161.last_plus_one=
_temp2160 + 22; _temp2161;}));( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_Yystack_overflow_struct* _temp2162=( struct Cyc_Yystack_overflow_struct*)
GC_malloc( sizeof( struct Cyc_Yystack_overflow_struct)); _temp2162[ 0]=({ struct
Cyc_Yystack_overflow_struct _temp2163; _temp2163.tag= Cyc_Yystack_overflow_tag;
_temp2163;}); _temp2162;}));} yystacksize *= 2; if( yystacksize > 10000){
yystacksize= 10000;}{ struct _tagged_ptr1 yyss1=({ unsigned int _temp2164=(
unsigned int) yystacksize; short* _temp2165=( short*) GC_malloc_atomic( sizeof(
short) * _temp2164); unsigned int i; struct _tagged_ptr1 _temp2166={ _temp2165,
_temp2165, _temp2165 + _temp2164}; for( i= 0; i < _temp2164; i ++){ _temp2165[ i]=
(short)0;} _temp2166;}); struct _tagged_ptr2 yyvs1=({ unsigned int _temp2167=(
unsigned int) yystacksize; struct _xtunion_struct** _temp2168=( struct
_xtunion_struct**) GC_malloc( sizeof( struct _xtunion_struct*) * _temp2167);
unsigned int i; struct _tagged_ptr2 _temp2169={ _temp2168, _temp2168, _temp2168
+ _temp2167}; for( i= 0; i < _temp2167; i ++){ _temp2168[ i]= Cyc_yylval;}
_temp2169;}); struct _tagged_ptr3 yyls1=({ unsigned int _temp2170=( unsigned int)
yystacksize; struct Cyc_Yyltype* _temp2171=( struct Cyc_Yyltype*) GC_malloc(
sizeof( struct Cyc_Yyltype) * _temp2170); unsigned int i; struct _tagged_ptr3
_temp2172={ _temp2171, _temp2171, _temp2171 + _temp2170}; for( i= 0; i <
_temp2170; i ++){ _temp2171[ i]= Cyc_yynewloc();} _temp2172;});{ int i= 0; for(
0; i <= yyssp_offset; i ++){({ struct _tagged_ptr1 _temp2173= yyss1; short*
_temp2175= _temp2173.curr + i; if( _temp2173.base == 0? 1:( _temp2175 <
_temp2173.base? 1: _temp2175 >= _temp2173.last_plus_one)){ _throw(
Null_Exception);}* _temp2175=({ struct _tagged_ptr1 _temp2176= yyss; short*
_temp2178= _temp2176.curr + i; if( _temp2176.base == 0? 1:( _temp2178 <
_temp2176.base? 1: _temp2178 >= _temp2176.last_plus_one)){ _throw(
Null_Exception);}* _temp2178;});});({ struct _tagged_ptr2 _temp2179= yyvs1;
struct _xtunion_struct** _temp2181= _temp2179.curr + i; if( _temp2179.base == 0?
1:( _temp2181 < _temp2179.base? 1: _temp2181 >= _temp2179.last_plus_one)){
_throw( Null_Exception);}* _temp2181=({ struct _tagged_ptr2 _temp2182= yyvs;
struct _xtunion_struct** _temp2184= _temp2182.curr + i; if( _temp2182.base == 0?
1:( _temp2184 < _temp2182.base? 1: _temp2184 >= _temp2182.last_plus_one)){
_throw( Null_Exception);}* _temp2184;});});({ struct _tagged_ptr3 _temp2185=
yyls1; struct Cyc_Yyltype* _temp2187= _temp2185.curr + i; if( _temp2185.base ==
0? 1:( _temp2187 < _temp2185.base? 1: _temp2187 >= _temp2185.last_plus_one)){
_throw( Null_Exception);}* _temp2187=({ struct _tagged_ptr3 _temp2188= yyls;
struct Cyc_Yyltype* _temp2190= _temp2188.curr + i; if( _temp2188.base == 0? 1:(
_temp2190 < _temp2188.base? 1: _temp2190 >= _temp2188.last_plus_one)){ _throw(
Null_Exception);}* _temp2190;});});}} yyss= yyss1; yyvs= yyvs1; yyls= yyls1;}}
goto yybackup; yybackup: yyn=( int)({ short* _temp2191=( short*) Cyc_yypact;
unsigned int _temp2192= yystate; if( _temp2192 >= 755u){ _throw( Null_Exception);}
_temp2191[ _temp2192];}); if( yyn == - 32768){ goto yydefault;} if( Cyc_yychar
== - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 0){ yychar1= 0; Cyc_yychar=
0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <= 351: 0)?( int)({ short*
_temp2193=( short*) Cyc_yytranslate; unsigned int _temp2194= Cyc_yychar; if(
_temp2194 >= 352u){ _throw( Null_Exception);} _temp2193[ _temp2194];}): 230;}
yyn += yychar1; if(( yyn < 0? 1: yyn > 4214)? 1:({ short* _temp2195=( short*)
Cyc_yycheck; unsigned int _temp2196= yyn; if( _temp2196 >= 4215u){ _throw(
Null_Exception);} _temp2195[ _temp2196];}) != yychar1){ goto yydefault;} yyn=(
int)({ short* _temp2197=( short*) Cyc_yytable; unsigned int _temp2198= yyn; if(
_temp2198 >= 4215u){ _throw( Null_Exception);} _temp2197[ _temp2198];}); if( yyn
< 0){ if( yyn == - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if(
yyn == 0){ goto yyerrlab;}} if( yyn == 754){ return 0;} if( Cyc_yychar != 0){
Cyc_yychar= - 2;}({ struct _tagged_ptr2 _temp2199= yyvs; struct _xtunion_struct**
_temp2201= _temp2199.curr +( ++ yyvsp_offset); if( _temp2199.base == 0? 1:(
_temp2201 < _temp2199.base? 1: _temp2201 >= _temp2199.last_plus_one)){ _throw(
Null_Exception);}* _temp2201= Cyc_yylval;});({ struct _tagged_ptr3 _temp2202=
yyls; struct Cyc_Yyltype* _temp2204= _temp2202.curr +( ++ yylsp_offset); if(
_temp2202.base == 0? 1:( _temp2204 < _temp2202.base? 1: _temp2204 >= _temp2202.last_plus_one)){
_throw( Null_Exception);}* _temp2204= Cyc_yylloc;}); if( yyerrstatus != 0){
yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int)({ short*
_temp2205=( short*) Cyc_yydefact; unsigned int _temp2206= yystate; if( _temp2206
>= 755u){ _throw( Null_Exception);} _temp2205[ _temp2206];}); if( yyn == 0){
goto yyerrlab;} yyreduce: yylen=( int)({ short* _temp2207=( short*) Cyc_yyr2;
unsigned int _temp2208= yyn; if( _temp2208 >= 393u){ _throw( Null_Exception);}
_temp2207[ _temp2208];}); if( yylen > 0){ yyval=({ struct _tagged_ptr2 _temp2209=
yyvs; struct _xtunion_struct** _temp2211= _temp2209.curr +(( yyvsp_offset + 1) -
yylen); if( _temp2209.base == 0? 1:( _temp2211 < _temp2209.base? 1: _temp2211 >=
_temp2209.last_plus_one)){ _throw( Null_Exception);}* _temp2211;});} switch( yyn){
case 1: _LL2212: yyval=({ struct _tagged_ptr2 _temp2214= yyvs; struct
_xtunion_struct** _temp2216= _temp2214.curr + yyvsp_offset; if( _temp2214.base
== 0? 1:( _temp2216 < _temp2214.base? 1: _temp2216 >= _temp2214.last_plus_one)){
_throw( Null_Exception);}* _temp2216;}); Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2217= yyvs; struct _xtunion_struct** _temp2219=
_temp2217.curr + yyvsp_offset; if( _temp2217.base == 0? 1:( _temp2219 <
_temp2217.base? 1: _temp2219 >= _temp2217.last_plus_one)){ _throw(
Null_Exception);}* _temp2219;})); break; case 2: _LL2213: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2221=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2221[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2222; _temp2222.tag= Cyc_DeclList_tok_tag; _temp2222.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2223= yyvs; struct _xtunion_struct** _temp2225=
_temp2223.curr +( yyvsp_offset - 1); if( _temp2223.base == 0? 1:( _temp2225 <
_temp2223.base? 1: _temp2225 >= _temp2223.last_plus_one)){ _throw(
Null_Exception);}* _temp2225;})), Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2226= yyvs; struct _xtunion_struct** _temp2228= _temp2226.curr +
yyvsp_offset; if( _temp2226.base == 0? 1:( _temp2228 < _temp2226.base? 1:
_temp2228 >= _temp2226.last_plus_one)){ _throw( Null_Exception);}* _temp2228;})));
_temp2222;}); _temp2221;}); break; case 3: _LL2220: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2230=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2230[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2231; _temp2231.tag= Cyc_DeclList_tok_tag; _temp2231.f1=({ struct Cyc_List_List*
_temp2232=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2232->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2233=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2233->r=( void*)(( void*)({
struct Cyc_Absyn_Using_d_struct* _temp2240=( struct Cyc_Absyn_Using_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp2240[ 0]=({ struct
Cyc_Absyn_Using_d_struct _temp2241; _temp2241.tag= Cyc_Absyn_Using_d_tag;
_temp2241.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2242= yyvs;
struct _xtunion_struct** _temp2244= _temp2242.curr +( yyvsp_offset - 2); if(
_temp2242.base == 0? 1:( _temp2244 < _temp2242.base? 1: _temp2244 >= _temp2242.last_plus_one)){
_throw( Null_Exception);}* _temp2244;})); _temp2241.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2245= yyvs; struct _xtunion_struct** _temp2247=
_temp2245.curr + yyvsp_offset; if( _temp2245.base == 0? 1:( _temp2247 <
_temp2245.base? 1: _temp2247 >= _temp2245.last_plus_one)){ _throw(
Null_Exception);}* _temp2247;})); _temp2241;}); _temp2240;})); _temp2233->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2234= yyls; struct Cyc_Yyltype*
_temp2236= _temp2234.curr +( yylsp_offset - 2); if( _temp2234.base == 0? 1:(
_temp2236 < _temp2234.base? 1: _temp2236 >= _temp2234.last_plus_one)){ _throw(
Null_Exception);}* _temp2236;}).first_line,({ struct _tagged_ptr3 _temp2237=
yyls; struct Cyc_Yyltype* _temp2239= _temp2237.curr + yylsp_offset; if(
_temp2237.base == 0? 1:( _temp2239 < _temp2237.base? 1: _temp2239 >= _temp2237.last_plus_one)){
_throw( Null_Exception);}* _temp2239;}).last_line); _temp2233;}); _temp2232->tl=
0; _temp2232;}); _temp2231;}); _temp2230;}); Cyc_Lex_leave_using(); break; case
4: _LL2229: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2249=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2249[ 0]=({ struct Cyc_DeclList_tok_struct _temp2250; _temp2250.tag= Cyc_DeclList_tok_tag;
_temp2250.f1=({ struct Cyc_List_List* _temp2251=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2251->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2255=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2255->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2262=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2262[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2263; _temp2263.tag= Cyc_Absyn_Using_d_tag;
_temp2263.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2264= yyvs;
struct _xtunion_struct** _temp2266= _temp2264.curr +( yyvsp_offset - 4); if(
_temp2264.base == 0? 1:( _temp2266 < _temp2264.base? 1: _temp2266 >= _temp2264.last_plus_one)){
_throw( Null_Exception);}* _temp2266;})); _temp2263.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2267= yyvs; struct _xtunion_struct** _temp2269=
_temp2267.curr +( yyvsp_offset - 2); if( _temp2267.base == 0? 1:( _temp2269 <
_temp2267.base? 1: _temp2269 >= _temp2267.last_plus_one)){ _throw(
Null_Exception);}* _temp2269;})); _temp2263;}); _temp2262;})); _temp2255->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2256= yyls; struct Cyc_Yyltype*
_temp2258= _temp2256.curr +( yylsp_offset - 4); if( _temp2256.base == 0? 1:(
_temp2258 < _temp2256.base? 1: _temp2258 >= _temp2256.last_plus_one)){ _throw(
Null_Exception);}* _temp2258;}).first_line,({ struct _tagged_ptr3 _temp2259=
yyls; struct Cyc_Yyltype* _temp2261= _temp2259.curr +( yylsp_offset - 1); if(
_temp2259.base == 0? 1:( _temp2261 < _temp2259.base? 1: _temp2261 >= _temp2259.last_plus_one)){
_throw( Null_Exception);}* _temp2261;}).last_line); _temp2255;}); _temp2251->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2252= yyvs; struct
_xtunion_struct** _temp2254= _temp2252.curr + yyvsp_offset; if( _temp2252.base
== 0? 1:( _temp2254 < _temp2252.base? 1: _temp2254 >= _temp2252.last_plus_one)){
_throw( Null_Exception);}* _temp2254;})); _temp2251;}); _temp2250;}); _temp2249;});
break; case 5: _LL2248: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2271=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2271[ 0]=({ struct Cyc_DeclList_tok_struct _temp2272; _temp2272.tag= Cyc_DeclList_tok_tag;
_temp2272.f1=({ struct Cyc_List_List* _temp2273=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2273->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2274=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2274->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2281=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2281[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2282; _temp2282.tag=
Cyc_Absyn_Namespace_d_tag; _temp2282.f1=({ struct _tagged_string* _temp2283=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp2283[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2284= yyvs;
struct _xtunion_struct** _temp2286= _temp2284.curr +( yyvsp_offset - 2); if(
_temp2284.base == 0? 1:( _temp2286 < _temp2284.base? 1: _temp2286 >= _temp2284.last_plus_one)){
_throw( Null_Exception);}* _temp2286;})); _temp2283;}); _temp2282.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2287= yyvs; struct _xtunion_struct** _temp2289=
_temp2287.curr + yyvsp_offset; if( _temp2287.base == 0? 1:( _temp2289 <
_temp2287.base? 1: _temp2289 >= _temp2287.last_plus_one)){ _throw(
Null_Exception);}* _temp2289;})); _temp2282;}); _temp2281;})); _temp2274->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2275= yyls; struct Cyc_Yyltype*
_temp2277= _temp2275.curr +( yylsp_offset - 2); if( _temp2275.base == 0? 1:(
_temp2277 < _temp2275.base? 1: _temp2277 >= _temp2275.last_plus_one)){ _throw(
Null_Exception);}* _temp2277;}).first_line,({ struct _tagged_ptr3 _temp2278=
yyls; struct Cyc_Yyltype* _temp2280= _temp2278.curr + yylsp_offset; if(
_temp2278.base == 0? 1:( _temp2280 < _temp2278.base? 1: _temp2280 >= _temp2278.last_plus_one)){
_throw( Null_Exception);}* _temp2280;}).last_line); _temp2274;}); _temp2273->tl=
0; _temp2273;}); _temp2272;}); _temp2271;}); Cyc_Lex_leave_namespace(); break;
case 6: _LL2270: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2291=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2291[ 0]=({ struct Cyc_DeclList_tok_struct _temp2292; _temp2292.tag= Cyc_DeclList_tok_tag;
_temp2292.f1=({ struct Cyc_List_List* _temp2293=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2293->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2297=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2297->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2304=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2304[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2305; _temp2305.tag=
Cyc_Absyn_Namespace_d_tag; _temp2305.f1=({ struct _tagged_string* _temp2306=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp2306[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2307= yyvs;
struct _xtunion_struct** _temp2309= _temp2307.curr +( yyvsp_offset - 4); if(
_temp2307.base == 0? 1:( _temp2309 < _temp2307.base? 1: _temp2309 >= _temp2307.last_plus_one)){
_throw( Null_Exception);}* _temp2309;})); _temp2306;}); _temp2305.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2310= yyvs; struct _xtunion_struct** _temp2312=
_temp2310.curr +( yyvsp_offset - 2); if( _temp2310.base == 0? 1:( _temp2312 <
_temp2310.base? 1: _temp2312 >= _temp2310.last_plus_one)){ _throw(
Null_Exception);}* _temp2312;})); _temp2305;}); _temp2304;})); _temp2297->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2298= yyls; struct Cyc_Yyltype*
_temp2300= _temp2298.curr +( yylsp_offset - 4); if( _temp2298.base == 0? 1:(
_temp2300 < _temp2298.base? 1: _temp2300 >= _temp2298.last_plus_one)){ _throw(
Null_Exception);}* _temp2300;}).first_line,({ struct _tagged_ptr3 _temp2301=
yyls; struct Cyc_Yyltype* _temp2303= _temp2301.curr +( yylsp_offset - 1); if(
_temp2301.base == 0? 1:( _temp2303 < _temp2301.base? 1: _temp2303 >= _temp2301.last_plus_one)){
_throw( Null_Exception);}* _temp2303;}).last_line); _temp2297;}); _temp2293->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2294= yyvs; struct
_xtunion_struct** _temp2296= _temp2294.curr + yyvsp_offset; if( _temp2294.base
== 0? 1:( _temp2296 < _temp2294.base? 1: _temp2296 >= _temp2294.last_plus_one)){
_throw( Null_Exception);}* _temp2296;})); _temp2293;}); _temp2292;}); _temp2291;});
break; case 7: _LL2290: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2314= yyvs; struct _xtunion_struct** _temp2316= _temp2314.curr
+( yyvsp_offset - 4); if( _temp2314.base == 0? 1:( _temp2316 < _temp2314.base? 1:
_temp2316 >= _temp2314.last_plus_one)){ _throw( Null_Exception);}* _temp2316;})),(
struct _tagged_string)({ char* _temp2317=( char*)"C"; struct _tagged_string
_temp2318; _temp2318.curr= _temp2317; _temp2318.base= _temp2317; _temp2318.last_plus_one=
_temp2317 + 2; _temp2318;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp2319=( char*)"only extern \"C\" { ... } is allowed"; struct
_tagged_string _temp2320; _temp2320.curr= _temp2319; _temp2320.base= _temp2319;
_temp2320.last_plus_one= _temp2319 + 35; _temp2320;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2321= yyls; struct Cyc_Yyltype* _temp2323= _temp2321.curr
+( yylsp_offset - 5); if( _temp2321.base == 0? 1:( _temp2323 < _temp2321.base? 1:
_temp2323 >= _temp2321.last_plus_one)){ _throw( Null_Exception);}* _temp2323;}).first_line,({
struct _tagged_ptr3 _temp2324= yyls; struct Cyc_Yyltype* _temp2326= _temp2324.curr
+( yylsp_offset - 4); if( _temp2324.base == 0? 1:( _temp2326 < _temp2324.base? 1:
_temp2326 >= _temp2324.last_plus_one)){ _throw( Null_Exception);}* _temp2326;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2327=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2327[ 0]=({ struct Cyc_DeclList_tok_struct _temp2328; _temp2328.tag= Cyc_DeclList_tok_tag;
_temp2328.f1=({ struct Cyc_List_List* _temp2329=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2329->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2333=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2333->r=( void*)(( void*)({ struct Cyc_Absyn_ExternC_d_struct* _temp2340=(
struct Cyc_Absyn_ExternC_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct));
_temp2340[ 0]=({ struct Cyc_Absyn_ExternC_d_struct _temp2341; _temp2341.tag= Cyc_Absyn_ExternC_d_tag;
_temp2341.f1= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2342= yyvs;
struct _xtunion_struct** _temp2344= _temp2342.curr +( yyvsp_offset - 2); if(
_temp2342.base == 0? 1:( _temp2344 < _temp2342.base? 1: _temp2344 >= _temp2342.last_plus_one)){
_throw( Null_Exception);}* _temp2344;})); _temp2341;}); _temp2340;})); _temp2333->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2334= yyls; struct Cyc_Yyltype*
_temp2336= _temp2334.curr +( yylsp_offset - 5); if( _temp2334.base == 0? 1:(
_temp2336 < _temp2334.base? 1: _temp2336 >= _temp2334.last_plus_one)){ _throw(
Null_Exception);}* _temp2336;}).first_line,({ struct _tagged_ptr3 _temp2337=
yyls; struct Cyc_Yyltype* _temp2339= _temp2337.curr +( yylsp_offset - 1); if(
_temp2337.base == 0? 1:( _temp2339 < _temp2337.base? 1: _temp2339 >= _temp2337.last_plus_one)){
_throw( Null_Exception);}* _temp2339;}).last_line); _temp2333;}); _temp2329->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2330= yyvs; struct
_xtunion_struct** _temp2332= _temp2330.curr + yyvsp_offset; if( _temp2330.base
== 0? 1:( _temp2332 < _temp2330.base? 1: _temp2332 >= _temp2330.last_plus_one)){
_throw( Null_Exception);}* _temp2332;})); _temp2329;}); _temp2328;}); _temp2327;});
break; case 8: _LL2313: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2346=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2346[ 0]=({ struct Cyc_DeclList_tok_struct _temp2347; _temp2347.tag= Cyc_DeclList_tok_tag;
_temp2347.f1= 0; _temp2347;}); _temp2346;}); break; case 9: _LL2345: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2349=( struct
Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2349[ 0]=({ struct Cyc_DeclList_tok_struct _temp2350; _temp2350.tag= Cyc_DeclList_tok_tag;
_temp2350.f1=({ struct Cyc_List_List* _temp2351=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2351->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2352=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp2352[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp2353; _temp2353.tag= Cyc_Absyn_Fn_d_tag; _temp2353.f1= Cyc_yyget_FnDecl_tok(({
struct _tagged_ptr2 _temp2354= yyvs; struct _xtunion_struct** _temp2356=
_temp2354.curr + yyvsp_offset; if( _temp2354.base == 0? 1:( _temp2356 <
_temp2354.base? 1: _temp2356 >= _temp2354.last_plus_one)){ _throw(
Null_Exception);}* _temp2356;})); _temp2353;}); _temp2352;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2357= yyls; struct Cyc_Yyltype* _temp2359= _temp2357.curr
+ yylsp_offset; if( _temp2357.base == 0? 1:( _temp2359 < _temp2357.base? 1:
_temp2359 >= _temp2357.last_plus_one)){ _throw( Null_Exception);}* _temp2359;}).first_line,({
struct _tagged_ptr3 _temp2360= yyls; struct Cyc_Yyltype* _temp2362= _temp2360.curr
+ yylsp_offset; if( _temp2360.base == 0? 1:( _temp2362 < _temp2360.base? 1:
_temp2362 >= _temp2360.last_plus_one)){ _throw( Null_Exception);}* _temp2362;}).last_line));
_temp2351->tl= 0; _temp2351;}); _temp2350;}); _temp2349;}); break; case 10:
_LL2348: yyval=({ struct _tagged_ptr2 _temp2364= yyvs; struct _xtunion_struct**
_temp2366= _temp2364.curr + yyvsp_offset; if( _temp2364.base == 0? 1:( _temp2366
< _temp2364.base? 1: _temp2366 >= _temp2364.last_plus_one)){ _throw(
Null_Exception);}* _temp2366;}); break; case 11: _LL2363: yyval=( struct
_xtunion_struct*)({ struct Cyc_FnDecl_tok_struct* _temp2368=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2368[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2369; _temp2369.tag= Cyc_FnDecl_tok_tag; _temp2369.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp2370= yyvs; struct
_xtunion_struct** _temp2372= _temp2370.curr +( yyvsp_offset - 1); if( _temp2370.base
== 0? 1:( _temp2372 < _temp2370.base? 1: _temp2372 >= _temp2370.last_plus_one)){
_throw( Null_Exception);}* _temp2372;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2373= yyvs; struct _xtunion_struct** _temp2375= _temp2373.curr
+ yyvsp_offset; if( _temp2373.base == 0? 1:( _temp2375 < _temp2373.base? 1:
_temp2375 >= _temp2373.last_plus_one)){ _throw( Null_Exception);}* _temp2375;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2376= yyls; struct Cyc_Yyltype*
_temp2378= _temp2376.curr +( yylsp_offset - 1); if( _temp2376.base == 0? 1:(
_temp2378 < _temp2376.base? 1: _temp2378 >= _temp2376.last_plus_one)){ _throw(
Null_Exception);}* _temp2378;}).first_line,({ struct _tagged_ptr3 _temp2379=
yyls; struct Cyc_Yyltype* _temp2381= _temp2379.curr + yylsp_offset; if(
_temp2379.base == 0? 1:( _temp2381 < _temp2379.base? 1: _temp2381 >= _temp2379.last_plus_one)){
_throw( Null_Exception);}* _temp2381;}).last_line)); _temp2369;}); _temp2368;});
break; case 12: _LL2367: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2383=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2383[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2384; _temp2384.tag= Cyc_FnDecl_tok_tag;
_temp2384.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2385=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2385->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2386= yyvs; struct
_xtunion_struct** _temp2388= _temp2386.curr +( yyvsp_offset - 2); if( _temp2386.base
== 0? 1:( _temp2388 < _temp2386.base? 1: _temp2388 >= _temp2386.last_plus_one)){
_throw( Null_Exception);}* _temp2388;})); _temp2385;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2389= yyvs; struct _xtunion_struct** _temp2391=
_temp2389.curr +( yyvsp_offset - 1); if( _temp2389.base == 0? 1:( _temp2391 <
_temp2389.base? 1: _temp2391 >= _temp2389.last_plus_one)){ _throw(
Null_Exception);}* _temp2391;})), 0, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2392= yyvs; struct _xtunion_struct** _temp2394= _temp2392.curr +
yyvsp_offset; if( _temp2392.base == 0? 1:( _temp2394 < _temp2392.base? 1:
_temp2394 >= _temp2392.last_plus_one)){ _throw( Null_Exception);}* _temp2394;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2395= yyls; struct Cyc_Yyltype*
_temp2397= _temp2395.curr +( yylsp_offset - 2); if( _temp2395.base == 0? 1:(
_temp2397 < _temp2395.base? 1: _temp2397 >= _temp2395.last_plus_one)){ _throw(
Null_Exception);}* _temp2397;}).first_line,({ struct _tagged_ptr3 _temp2398=
yyls; struct Cyc_Yyltype* _temp2400= _temp2398.curr + yylsp_offset; if(
_temp2398.base == 0? 1:( _temp2400 < _temp2398.base? 1: _temp2400 >= _temp2398.last_plus_one)){
_throw( Null_Exception);}* _temp2400;}).last_line)); _temp2384;}); _temp2383;});
break; case 13: _LL2382: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2402=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2402[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2403; _temp2403.tag= Cyc_FnDecl_tok_tag;
_temp2403.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp2404= yyvs; struct _xtunion_struct** _temp2406= _temp2404.curr
+( yyvsp_offset - 2); if( _temp2404.base == 0? 1:( _temp2406 < _temp2404.base? 1:
_temp2406 >= _temp2404.last_plus_one)){ _throw( Null_Exception);}* _temp2406;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2407= yyvs; struct
_xtunion_struct** _temp2409= _temp2407.curr +( yyvsp_offset - 1); if( _temp2407.base
== 0? 1:( _temp2409 < _temp2407.base? 1: _temp2409 >= _temp2407.last_plus_one)){
_throw( Null_Exception);}* _temp2409;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2410= yyvs; struct _xtunion_struct** _temp2412= _temp2410.curr
+ yyvsp_offset; if( _temp2410.base == 0? 1:( _temp2412 < _temp2410.base? 1:
_temp2412 >= _temp2410.last_plus_one)){ _throw( Null_Exception);}* _temp2412;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2413= yyls; struct Cyc_Yyltype*
_temp2415= _temp2413.curr +( yylsp_offset - 2); if( _temp2413.base == 0? 1:(
_temp2415 < _temp2413.base? 1: _temp2415 >= _temp2413.last_plus_one)){ _throw(
Null_Exception);}* _temp2415;}).first_line,({ struct _tagged_ptr3 _temp2416=
yyls; struct Cyc_Yyltype* _temp2418= _temp2416.curr + yylsp_offset; if(
_temp2416.base == 0? 1:( _temp2418 < _temp2416.base? 1: _temp2418 >= _temp2416.last_plus_one)){
_throw( Null_Exception);}* _temp2418;}).last_line)); _temp2403;}); _temp2402;});
break; case 14: _LL2401: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2420=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2420[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2421; _temp2421.tag= Cyc_FnDecl_tok_tag;
_temp2421.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2422=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2422->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2423= yyvs; struct
_xtunion_struct** _temp2425= _temp2423.curr +( yyvsp_offset - 3); if( _temp2423.base
== 0? 1:( _temp2425 < _temp2423.base? 1: _temp2425 >= _temp2423.last_plus_one)){
_throw( Null_Exception);}* _temp2425;})); _temp2422;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2426= yyvs; struct _xtunion_struct** _temp2428=
_temp2426.curr +( yyvsp_offset - 2); if( _temp2426.base == 0? 1:( _temp2428 <
_temp2426.base? 1: _temp2428 >= _temp2426.last_plus_one)){ _throw(
Null_Exception);}* _temp2428;})), Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2429= yyvs; struct _xtunion_struct** _temp2431= _temp2429.curr +(
yyvsp_offset - 1); if( _temp2429.base == 0? 1:( _temp2431 < _temp2429.base? 1:
_temp2431 >= _temp2429.last_plus_one)){ _throw( Null_Exception);}* _temp2431;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2432= yyvs; struct
_xtunion_struct** _temp2434= _temp2432.curr + yyvsp_offset; if( _temp2432.base
== 0? 1:( _temp2434 < _temp2432.base? 1: _temp2434 >= _temp2432.last_plus_one)){
_throw( Null_Exception);}* _temp2434;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2435= yyls; struct Cyc_Yyltype* _temp2437= _temp2435.curr +(
yylsp_offset - 3); if( _temp2435.base == 0? 1:( _temp2437 < _temp2435.base? 1:
_temp2437 >= _temp2435.last_plus_one)){ _throw( Null_Exception);}* _temp2437;}).first_line,({
struct _tagged_ptr3 _temp2438= yyls; struct Cyc_Yyltype* _temp2440= _temp2438.curr
+ yylsp_offset; if( _temp2438.base == 0? 1:( _temp2440 < _temp2438.base? 1:
_temp2440 >= _temp2438.last_plus_one)){ _throw( Null_Exception);}* _temp2440;}).last_line));
_temp2421;}); _temp2420;}); break; case 15: _LL2419: yyval=( struct
_xtunion_struct*)({ struct Cyc_FnDecl_tok_struct* _temp2442=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2442[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2443; _temp2443.tag= Cyc_FnDecl_tok_tag; _temp2443.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2444=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2444->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2445= yyvs; struct _xtunion_struct** _temp2447= _temp2445.curr
+( yyvsp_offset - 2); if( _temp2445.base == 0? 1:( _temp2447 < _temp2445.base? 1:
_temp2447 >= _temp2445.last_plus_one)){ _throw( Null_Exception);}* _temp2447;}));
_temp2444;}), Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp2448= yyvs;
struct _xtunion_struct** _temp2450= _temp2448.curr +( yyvsp_offset - 1); if(
_temp2448.base == 0? 1:( _temp2450 < _temp2448.base? 1: _temp2450 >= _temp2448.last_plus_one)){
_throw( Null_Exception);}* _temp2450;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2451= yyvs; struct _xtunion_struct** _temp2453= _temp2451.curr
+ yyvsp_offset; if( _temp2451.base == 0? 1:( _temp2453 < _temp2451.base? 1:
_temp2453 >= _temp2451.last_plus_one)){ _throw( Null_Exception);}* _temp2453;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2454= yyls; struct Cyc_Yyltype*
_temp2456= _temp2454.curr +( yylsp_offset - 2); if( _temp2454.base == 0? 1:(
_temp2456 < _temp2454.base? 1: _temp2456 >= _temp2454.last_plus_one)){ _throw(
Null_Exception);}* _temp2456;}).first_line,({ struct _tagged_ptr3 _temp2457=
yyls; struct Cyc_Yyltype* _temp2459= _temp2457.curr + yylsp_offset; if(
_temp2457.base == 0? 1:( _temp2459 < _temp2457.base? 1: _temp2459 >= _temp2457.last_plus_one)){
_throw( Null_Exception);}* _temp2459;}).last_line)); _temp2443;}); _temp2442;});
break; case 16: _LL2441: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2461=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2461[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2462; _temp2462.tag= Cyc_FnDecl_tok_tag;
_temp2462.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2463=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2463->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2464= yyvs; struct
_xtunion_struct** _temp2466= _temp2464.curr +( yyvsp_offset - 3); if( _temp2464.base
== 0? 1:( _temp2466 < _temp2464.base? 1: _temp2466 >= _temp2464.last_plus_one)){
_throw( Null_Exception);}* _temp2466;})); _temp2463;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2467= yyvs; struct _xtunion_struct** _temp2469=
_temp2467.curr +( yyvsp_offset - 2); if( _temp2467.base == 0? 1:( _temp2469 <
_temp2467.base? 1: _temp2469 >= _temp2467.last_plus_one)){ _throw(
Null_Exception);}* _temp2469;})), Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2470= yyvs; struct _xtunion_struct** _temp2472= _temp2470.curr +(
yyvsp_offset - 1); if( _temp2470.base == 0? 1:( _temp2472 < _temp2470.base? 1:
_temp2472 >= _temp2470.last_plus_one)){ _throw( Null_Exception);}* _temp2472;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2473= yyvs; struct
_xtunion_struct** _temp2475= _temp2473.curr + yyvsp_offset; if( _temp2473.base
== 0? 1:( _temp2475 < _temp2473.base? 1: _temp2475 >= _temp2473.last_plus_one)){
_throw( Null_Exception);}* _temp2475;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2476= yyls; struct Cyc_Yyltype* _temp2478= _temp2476.curr +(
yylsp_offset - 3); if( _temp2476.base == 0? 1:( _temp2478 < _temp2476.base? 1:
_temp2478 >= _temp2476.last_plus_one)){ _throw( Null_Exception);}* _temp2478;}).first_line,({
struct _tagged_ptr3 _temp2479= yyls; struct Cyc_Yyltype* _temp2481= _temp2479.curr
+ yylsp_offset; if( _temp2479.base == 0? 1:( _temp2481 < _temp2479.base? 1:
_temp2481 >= _temp2479.last_plus_one)){ _throw( Null_Exception);}* _temp2481;}).last_line));
_temp2462;}); _temp2461;}); break; case 17: _LL2460: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2483= yyvs; struct _xtunion_struct** _temp2485=
_temp2483.curr + yyvsp_offset; if( _temp2483.base == 0? 1:( _temp2485 <
_temp2483.base? 1: _temp2485 >= _temp2483.last_plus_one)){ _throw(
Null_Exception);}* _temp2485;}))); yyval=({ struct _tagged_ptr2 _temp2486= yyvs;
struct _xtunion_struct** _temp2488= _temp2486.curr + yyvsp_offset; if( _temp2486.base
== 0? 1:( _temp2488 < _temp2486.base? 1: _temp2488 >= _temp2486.last_plus_one)){
_throw( Null_Exception);}* _temp2488;}); break; case 18: _LL2482: Cyc_Lex_leave_using();
break; case 19: _LL2489: Cyc_Lex_enter_namespace(({ struct _tagged_string*
_temp2491=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2491[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2492= yyvs;
struct _xtunion_struct** _temp2494= _temp2492.curr + yyvsp_offset; if( _temp2492.base
== 0? 1:( _temp2494 < _temp2492.base? 1: _temp2494 >= _temp2492.last_plus_one)){
_throw( Null_Exception);}* _temp2494;})); _temp2491;})); yyval=({ struct
_tagged_ptr2 _temp2495= yyvs; struct _xtunion_struct** _temp2497= _temp2495.curr
+ yyvsp_offset; if( _temp2495.base == 0? 1:( _temp2497 < _temp2495.base? 1:
_temp2497 >= _temp2495.last_plus_one)){ _throw( Null_Exception);}* _temp2497;});
break; case 20: _LL2490: Cyc_Lex_leave_namespace(); break; case 21: _LL2498:
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2500=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2500[ 0]=({ struct Cyc_DeclList_tok_struct _temp2501; _temp2501.tag= Cyc_DeclList_tok_tag;
_temp2501.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2502= yyvs; struct _xtunion_struct** _temp2504= _temp2502.curr
+( yyvsp_offset - 1); if( _temp2502.base == 0? 1:( _temp2504 < _temp2502.base? 1:
_temp2504 >= _temp2502.last_plus_one)){ _throw( Null_Exception);}* _temp2504;})),
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2505= yyls; struct
Cyc_Yyltype* _temp2507= _temp2505.curr +( yylsp_offset - 1); if( _temp2505.base
== 0? 1:( _temp2507 < _temp2505.base? 1: _temp2507 >= _temp2505.last_plus_one)){
_throw( Null_Exception);}* _temp2507;}).first_line,({ struct _tagged_ptr3
_temp2508= yyls; struct Cyc_Yyltype* _temp2510= _temp2508.curr +( yylsp_offset -
1); if( _temp2508.base == 0? 1:( _temp2510 < _temp2508.base? 1: _temp2510 >=
_temp2508.last_plus_one)){ _throw( Null_Exception);}* _temp2510;}).last_line));
_temp2501;}); _temp2500;}); break; case 22: _LL2499: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2512=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2512[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2513; _temp2513.tag= Cyc_DeclList_tok_tag; _temp2513.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2514= yyvs; struct
_xtunion_struct** _temp2516= _temp2514.curr +( yyvsp_offset - 2); if( _temp2514.base
== 0? 1:( _temp2516 < _temp2514.base? 1: _temp2516 >= _temp2514.last_plus_one)){
_throw( Null_Exception);}* _temp2516;})), Cyc_yyget_InitDeclList_tok(({ struct
_tagged_ptr2 _temp2517= yyvs; struct _xtunion_struct** _temp2519= _temp2517.curr
+( yyvsp_offset - 1); if( _temp2517.base == 0? 1:( _temp2519 < _temp2517.base? 1:
_temp2519 >= _temp2517.last_plus_one)){ _throw( Null_Exception);}* _temp2519;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2520= yyls; struct Cyc_Yyltype*
_temp2522= _temp2520.curr +( yylsp_offset - 2); if( _temp2520.base == 0? 1:(
_temp2522 < _temp2520.base? 1: _temp2522 >= _temp2520.last_plus_one)){ _throw(
Null_Exception);}* _temp2522;}).first_line,({ struct _tagged_ptr3 _temp2523=
yyls; struct Cyc_Yyltype* _temp2525= _temp2523.curr + yylsp_offset; if(
_temp2523.base == 0? 1:( _temp2525 < _temp2523.base? 1: _temp2525 >= _temp2523.last_plus_one)){
_throw( Null_Exception);}* _temp2525;}).last_line)); _temp2513;}); _temp2512;});
break; case 23: _LL2511: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2527=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2527[ 0]=({ struct Cyc_DeclList_tok_struct _temp2528; _temp2528.tag= Cyc_DeclList_tok_tag;
_temp2528.f1=({ struct Cyc_List_List* _temp2529=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2529->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp2530= yyvs; struct
_xtunion_struct** _temp2532= _temp2530.curr +( yyvsp_offset - 3); if( _temp2530.base
== 0? 1:( _temp2532 < _temp2530.base? 1: _temp2532 >= _temp2530.last_plus_one)){
_throw( Null_Exception);}* _temp2532;})), 0, Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp2533= yyvs; struct _xtunion_struct** _temp2535= _temp2533.curr
+( yyvsp_offset - 1); if( _temp2533.base == 0? 1:( _temp2535 < _temp2533.base? 1:
_temp2535 >= _temp2533.last_plus_one)){ _throw( Null_Exception);}* _temp2535;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2536= yyls; struct Cyc_Yyltype*
_temp2538= _temp2536.curr +( yylsp_offset - 4); if( _temp2536.base == 0? 1:(
_temp2538 < _temp2536.base? 1: _temp2538 >= _temp2536.last_plus_one)){ _throw(
Null_Exception);}* _temp2538;}).first_line,({ struct _tagged_ptr3 _temp2539=
yyls; struct Cyc_Yyltype* _temp2541= _temp2539.curr + yylsp_offset; if(
_temp2539.base == 0? 1:( _temp2541 < _temp2539.base? 1: _temp2541 >= _temp2539.last_plus_one)){
_throw( Null_Exception);}* _temp2541;}).last_line)); _temp2529->tl= 0; _temp2529;});
_temp2528;}); _temp2527;}); break; case 24: _LL2526: yyval=({ struct
_tagged_ptr2 _temp2543= yyvs; struct _xtunion_struct** _temp2545= _temp2543.curr
+ yyvsp_offset; if( _temp2543.base == 0? 1:( _temp2545 < _temp2543.base? 1:
_temp2545 >= _temp2543.last_plus_one)){ _throw( Null_Exception);}* _temp2545;});
break; case 25: _LL2542: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2547=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2547[ 0]=({ struct Cyc_DeclList_tok_struct _temp2548; _temp2548.tag= Cyc_DeclList_tok_tag;
_temp2548.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2549= yyvs; struct _xtunion_struct** _temp2551= _temp2549.curr +(
yyvsp_offset - 1); if( _temp2549.base == 0? 1:( _temp2551 < _temp2549.base? 1:
_temp2551 >= _temp2549.last_plus_one)){ _throw( Null_Exception);}* _temp2551;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2552= yyvs; struct
_xtunion_struct** _temp2554= _temp2552.curr + yyvsp_offset; if( _temp2552.base
== 0? 1:( _temp2554 < _temp2552.base? 1: _temp2554 >= _temp2552.last_plus_one)){
_throw( Null_Exception);}* _temp2554;}))); _temp2548;}); _temp2547;}); break;
case 26: _LL2546: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct*
_temp2556=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2556[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2557; _temp2557.tag= Cyc_DeclSpec_tok_tag;
_temp2557.f1=({ struct Cyc_Parse_Declaration_spec* _temp2558=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2558->sc=({ struct
Cyc_Core_Opt* _temp2562=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2562->v=( void*) Cyc_yyget_StorageClass_tok(({ struct _tagged_ptr2
_temp2563= yyvs; struct _xtunion_struct** _temp2565= _temp2563.curr +(
yyvsp_offset - 1); if( _temp2563.base == 0? 1:( _temp2565 < _temp2563.base? 1:
_temp2565 >= _temp2563.last_plus_one)){ _throw( Null_Exception);}* _temp2565;}));
_temp2562;}); _temp2558->tq= Cyc_Absyn_empty_tqual(); _temp2558->type_specs= 0;
_temp2558->is_inline= 0; _temp2558->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2559= yyvs; struct _xtunion_struct** _temp2561=
_temp2559.curr + yyvsp_offset; if( _temp2559.base == 0? 1:( _temp2561 <
_temp2559.base? 1: _temp2561 >= _temp2559.last_plus_one)){ _throw(
Null_Exception);}* _temp2561;})); _temp2558;}); _temp2557;}); _temp2556;});
break; case 27: _LL2555: if(( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2567= yyvs; struct _xtunion_struct** _temp2569= _temp2567.curr +
yyvsp_offset; if( _temp2567.base == 0? 1:( _temp2569 < _temp2567.base? 1:
_temp2569 >= _temp2567.last_plus_one)){ _throw( Null_Exception);}* _temp2569;})))->sc
!= 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp2570=( char*)"Only one storage class is allowed in a declaration";
struct _tagged_string _temp2571; _temp2571.curr= _temp2570; _temp2571.base=
_temp2570; _temp2571.last_plus_one= _temp2570 + 51; _temp2571;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2572= yyls; struct Cyc_Yyltype* _temp2574= _temp2572.curr
+( yylsp_offset - 2); if( _temp2572.base == 0? 1:( _temp2574 < _temp2572.base? 1:
_temp2574 >= _temp2572.last_plus_one)){ _throw( Null_Exception);}* _temp2574;}).first_line,({
struct _tagged_ptr3 _temp2575= yyls; struct Cyc_Yyltype* _temp2577= _temp2575.curr
+( yylsp_offset - 1); if( _temp2575.base == 0? 1:( _temp2577 < _temp2575.base? 1:
_temp2577 >= _temp2575.last_plus_one)){ _throw( Null_Exception);}* _temp2577;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2578=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2578[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2579; _temp2579.tag= Cyc_DeclSpec_tok_tag;
_temp2579.f1=({ struct Cyc_Parse_Declaration_spec* _temp2580=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2580->sc=({ struct
Cyc_Core_Opt* _temp2596=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2596->v=( void*) Cyc_yyget_StorageClass_tok(({ struct _tagged_ptr2
_temp2597= yyvs; struct _xtunion_struct** _temp2599= _temp2597.curr +(
yyvsp_offset - 2); if( _temp2597.base == 0? 1:( _temp2599 < _temp2597.base? 1:
_temp2599 >= _temp2597.last_plus_one)){ _throw( Null_Exception);}* _temp2599;}));
_temp2596;}); _temp2580->tq=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2593= yyvs; struct _xtunion_struct** _temp2595= _temp2593.curr +
yyvsp_offset; if( _temp2593.base == 0? 1:( _temp2595 < _temp2593.base? 1:
_temp2595 >= _temp2593.last_plus_one)){ _throw( Null_Exception);}* _temp2595;})))->tq;
_temp2580->type_specs=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2590=
yyvs; struct _xtunion_struct** _temp2592= _temp2590.curr + yyvsp_offset; if(
_temp2590.base == 0? 1:( _temp2592 < _temp2590.base? 1: _temp2592 >= _temp2590.last_plus_one)){
_throw( Null_Exception);}* _temp2592;})))->type_specs; _temp2580->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2587= yyvs; struct
_xtunion_struct** _temp2589= _temp2587.curr + yyvsp_offset; if( _temp2587.base
== 0? 1:( _temp2589 < _temp2587.base? 1: _temp2589 >= _temp2587.last_plus_one)){
_throw( Null_Exception);}* _temp2589;})))->is_inline; _temp2580->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2581= yyvs; struct
_xtunion_struct** _temp2583= _temp2581.curr +( yyvsp_offset - 1); if( _temp2581.base
== 0? 1:( _temp2583 < _temp2581.base? 1: _temp2583 >= _temp2581.last_plus_one)){
_throw( Null_Exception);}* _temp2583;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2584= yyvs; struct _xtunion_struct** _temp2586= _temp2584.curr
+ yyvsp_offset; if( _temp2584.base == 0? 1:( _temp2586 < _temp2584.base? 1:
_temp2586 >= _temp2584.last_plus_one)){ _throw( Null_Exception);}* _temp2586;})))->attributes);
_temp2580;}); _temp2579;}); _temp2578;}); break; case 28: _LL2566: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2601=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2601[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2602; _temp2602.tag= Cyc_DeclSpec_tok_tag;
_temp2602.f1=({ struct Cyc_Parse_Declaration_spec* _temp2603=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2603->sc= 0;
_temp2603->tq= Cyc_Absyn_empty_tqual(); _temp2603->type_specs=({ struct Cyc_List_List*
_temp2607=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2607->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2608= yyvs; struct _xtunion_struct** _temp2610= _temp2608.curr +(
yyvsp_offset - 1); if( _temp2608.base == 0? 1:( _temp2610 < _temp2608.base? 1:
_temp2610 >= _temp2608.last_plus_one)){ _throw( Null_Exception);}* _temp2610;}));
_temp2607->tl= 0; _temp2607;}); _temp2603->is_inline= 0; _temp2603->attributes=
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2604= yyvs; struct
_xtunion_struct** _temp2606= _temp2604.curr + yyvsp_offset; if( _temp2604.base
== 0? 1:( _temp2606 < _temp2604.base? 1: _temp2606 >= _temp2604.last_plus_one)){
_throw( Null_Exception);}* _temp2606;})); _temp2603;}); _temp2602;}); _temp2601;});
break; case 29: _LL2600: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct*
_temp2612=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2612[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2613; _temp2613.tag= Cyc_DeclSpec_tok_tag;
_temp2613.f1=({ struct Cyc_Parse_Declaration_spec* _temp2614=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2614->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2634= yyvs; struct _xtunion_struct** _temp2636=
_temp2634.curr + yyvsp_offset; if( _temp2634.base == 0? 1:( _temp2636 <
_temp2634.base? 1: _temp2636 >= _temp2634.last_plus_one)){ _throw(
Null_Exception);}* _temp2636;})))->sc; _temp2614->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2631= yyvs; struct _xtunion_struct** _temp2633=
_temp2631.curr + yyvsp_offset; if( _temp2631.base == 0? 1:( _temp2633 <
_temp2631.base? 1: _temp2633 >= _temp2631.last_plus_one)){ _throw(
Null_Exception);}* _temp2633;})))->tq; _temp2614->type_specs=({ struct Cyc_List_List*
_temp2624=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2624->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2628= yyvs; struct _xtunion_struct** _temp2630= _temp2628.curr +(
yyvsp_offset - 2); if( _temp2628.base == 0? 1:( _temp2630 < _temp2628.base? 1:
_temp2630 >= _temp2628.last_plus_one)){ _throw( Null_Exception);}* _temp2630;}));
_temp2624->tl=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2625= yyvs;
struct _xtunion_struct** _temp2627= _temp2625.curr + yyvsp_offset; if( _temp2625.base
== 0? 1:( _temp2627 < _temp2625.base? 1: _temp2627 >= _temp2625.last_plus_one)){
_throw( Null_Exception);}* _temp2627;})))->type_specs; _temp2624;}); _temp2614->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2621= yyvs; struct
_xtunion_struct** _temp2623= _temp2621.curr + yyvsp_offset; if( _temp2621.base
== 0? 1:( _temp2623 < _temp2621.base? 1: _temp2623 >= _temp2621.last_plus_one)){
_throw( Null_Exception);}* _temp2623;})))->is_inline; _temp2614->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2615= yyvs; struct
_xtunion_struct** _temp2617= _temp2615.curr +( yyvsp_offset - 1); if( _temp2615.base
== 0? 1:( _temp2617 < _temp2615.base? 1: _temp2617 >= _temp2615.last_plus_one)){
_throw( Null_Exception);}* _temp2617;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2618= yyvs; struct _xtunion_struct** _temp2620= _temp2618.curr
+ yyvsp_offset; if( _temp2618.base == 0? 1:( _temp2620 < _temp2618.base? 1:
_temp2620 >= _temp2618.last_plus_one)){ _throw( Null_Exception);}* _temp2620;})))->attributes);
_temp2614;}); _temp2613;}); _temp2612;}); break; case 30: _LL2611: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2638=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2638[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2639; _temp2639.tag= Cyc_DeclSpec_tok_tag;
_temp2639.f1=({ struct Cyc_Parse_Declaration_spec* _temp2640=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2640->sc= 0;
_temp2640->tq= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp2644= yyvs;
struct _xtunion_struct** _temp2646= _temp2644.curr +( yyvsp_offset - 1); if(
_temp2644.base == 0? 1:( _temp2646 < _temp2644.base? 1: _temp2646 >= _temp2644.last_plus_one)){
_throw( Null_Exception);}* _temp2646;})); _temp2640->type_specs= 0; _temp2640->is_inline=
0; _temp2640->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2641= yyvs; struct _xtunion_struct** _temp2643= _temp2641.curr +
yyvsp_offset; if( _temp2641.base == 0? 1:( _temp2643 < _temp2641.base? 1:
_temp2643 >= _temp2641.last_plus_one)){ _throw( Null_Exception);}* _temp2643;}));
_temp2640;}); _temp2639;}); _temp2638;}); break; case 31: _LL2637: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2648=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2648[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2649; _temp2649.tag= Cyc_DeclSpec_tok_tag;
_temp2649.f1=({ struct Cyc_Parse_Declaration_spec* _temp2650=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2650->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2669= yyvs; struct _xtunion_struct** _temp2671=
_temp2669.curr + yyvsp_offset; if( _temp2669.base == 0? 1:( _temp2671 <
_temp2669.base? 1: _temp2671 >= _temp2669.last_plus_one)){ _throw(
Null_Exception);}* _temp2671;})))->sc; _temp2650->tq= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp2663= yyvs; struct
_xtunion_struct** _temp2665= _temp2663.curr +( yyvsp_offset - 2); if( _temp2663.base
== 0? 1:( _temp2665 < _temp2663.base? 1: _temp2665 >= _temp2663.last_plus_one)){
_throw( Null_Exception);}* _temp2665;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2666= yyvs; struct _xtunion_struct** _temp2668= _temp2666.curr
+ yyvsp_offset; if( _temp2666.base == 0? 1:( _temp2668 < _temp2666.base? 1:
_temp2668 >= _temp2666.last_plus_one)){ _throw( Null_Exception);}* _temp2668;})))->tq);
_temp2650->type_specs=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2660=
yyvs; struct _xtunion_struct** _temp2662= _temp2660.curr + yyvsp_offset; if(
_temp2660.base == 0? 1:( _temp2662 < _temp2660.base? 1: _temp2662 >= _temp2660.last_plus_one)){
_throw( Null_Exception);}* _temp2662;})))->type_specs; _temp2650->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2657= yyvs; struct
_xtunion_struct** _temp2659= _temp2657.curr + yyvsp_offset; if( _temp2657.base
== 0? 1:( _temp2659 < _temp2657.base? 1: _temp2659 >= _temp2657.last_plus_one)){
_throw( Null_Exception);}* _temp2659;})))->is_inline; _temp2650->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2651= yyvs; struct
_xtunion_struct** _temp2653= _temp2651.curr +( yyvsp_offset - 1); if( _temp2651.base
== 0? 1:( _temp2653 < _temp2651.base? 1: _temp2653 >= _temp2651.last_plus_one)){
_throw( Null_Exception);}* _temp2653;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2654= yyvs; struct _xtunion_struct** _temp2656= _temp2654.curr
+ yyvsp_offset; if( _temp2654.base == 0? 1:( _temp2656 < _temp2654.base? 1:
_temp2656 >= _temp2654.last_plus_one)){ _throw( Null_Exception);}* _temp2656;})))->attributes);
_temp2650;}); _temp2649;}); _temp2648;}); break; case 32: _LL2647: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2673=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2673[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2674; _temp2674.tag= Cyc_DeclSpec_tok_tag;
_temp2674.f1=({ struct Cyc_Parse_Declaration_spec* _temp2675=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2675->sc= 0;
_temp2675->tq= Cyc_Absyn_empty_tqual(); _temp2675->type_specs= 0; _temp2675->is_inline=
1; _temp2675->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2676= yyvs; struct _xtunion_struct** _temp2678= _temp2676.curr +
yyvsp_offset; if( _temp2676.base == 0? 1:( _temp2678 < _temp2676.base? 1:
_temp2678 >= _temp2676.last_plus_one)){ _throw( Null_Exception);}* _temp2678;}));
_temp2675;}); _temp2674;}); _temp2673;}); break; case 33: _LL2672: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2680=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2680[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2681; _temp2681.tag= Cyc_DeclSpec_tok_tag;
_temp2681.f1=({ struct Cyc_Parse_Declaration_spec* _temp2682=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2682->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2695= yyvs; struct _xtunion_struct** _temp2697=
_temp2695.curr + yyvsp_offset; if( _temp2695.base == 0? 1:( _temp2697 <
_temp2695.base? 1: _temp2697 >= _temp2695.last_plus_one)){ _throw(
Null_Exception);}* _temp2697;})))->sc; _temp2682->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2692= yyvs; struct _xtunion_struct** _temp2694=
_temp2692.curr + yyvsp_offset; if( _temp2692.base == 0? 1:( _temp2694 <
_temp2692.base? 1: _temp2694 >= _temp2692.last_plus_one)){ _throw(
Null_Exception);}* _temp2694;})))->tq; _temp2682->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2689= yyvs; struct _xtunion_struct** _temp2691=
_temp2689.curr + yyvsp_offset; if( _temp2689.base == 0? 1:( _temp2691 <
_temp2689.base? 1: _temp2691 >= _temp2689.last_plus_one)){ _throw(
Null_Exception);}* _temp2691;})))->type_specs; _temp2682->is_inline= 1;
_temp2682->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2683= yyvs; struct _xtunion_struct** _temp2685=
_temp2683.curr +( yyvsp_offset - 1); if( _temp2683.base == 0? 1:( _temp2685 <
_temp2683.base? 1: _temp2685 >= _temp2683.last_plus_one)){ _throw(
Null_Exception);}* _temp2685;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2686= yyvs; struct _xtunion_struct** _temp2688= _temp2686.curr +
yyvsp_offset; if( _temp2686.base == 0? 1:( _temp2688 < _temp2686.base? 1:
_temp2688 >= _temp2686.last_plus_one)){ _throw( Null_Exception);}* _temp2688;})))->attributes);
_temp2682;}); _temp2681;}); _temp2680;}); break; case 34: _LL2679: yyval=(
struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2699=(
struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2699[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2700; _temp2700.tag=
Cyc_StorageClass_tok_tag; _temp2700.f1=( void*)(( void*) Cyc_Parse_Auto_sc);
_temp2700;}); _temp2699;}); break; case 35: _LL2698: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2702=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2702[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2703; _temp2703.tag= Cyc_StorageClass_tok_tag;
_temp2703.f1=( void*)(( void*) Cyc_Parse_Register_sc); _temp2703;}); _temp2702;});
break; case 36: _LL2701: yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct*
_temp2705=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2705[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2706; _temp2706.tag=
Cyc_StorageClass_tok_tag; _temp2706.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp2706;}); _temp2705;}); break; case 37: _LL2704: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2708=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2708[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2709; _temp2709.tag= Cyc_StorageClass_tok_tag;
_temp2709.f1=( void*)(( void*) Cyc_Parse_Extern_sc); _temp2709;}); _temp2708;});
break; case 38: _LL2707: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2711= yyvs; struct _xtunion_struct** _temp2713= _temp2711.curr
+ yyvsp_offset; if( _temp2711.base == 0? 1:( _temp2713 < _temp2711.base? 1:
_temp2713 >= _temp2711.last_plus_one)){ _throw( Null_Exception);}* _temp2713;})),(
struct _tagged_string)({ char* _temp2714=( char*)"C"; struct _tagged_string
_temp2715; _temp2715.curr= _temp2714; _temp2715.base= _temp2714; _temp2715.last_plus_one=
_temp2714 + 2; _temp2715;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp2716=( char*)"only extern or extern \"C\" is allowed"; struct
_tagged_string _temp2717; _temp2717.curr= _temp2716; _temp2717.base= _temp2716;
_temp2717.last_plus_one= _temp2716 + 37; _temp2717;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2718= yyls; struct Cyc_Yyltype* _temp2720= _temp2718.curr
+( yylsp_offset - 1); if( _temp2718.base == 0? 1:( _temp2720 < _temp2718.base? 1:
_temp2720 >= _temp2718.last_plus_one)){ _throw( Null_Exception);}* _temp2720;}).first_line,({
struct _tagged_ptr3 _temp2721= yyls; struct Cyc_Yyltype* _temp2723= _temp2721.curr
+ yylsp_offset; if( _temp2721.base == 0? 1:( _temp2723 < _temp2721.base? 1:
_temp2723 >= _temp2721.last_plus_one)){ _throw( Null_Exception);}* _temp2723;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2724=(
struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2724[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2725; _temp2725.tag=
Cyc_StorageClass_tok_tag; _temp2725.f1=( void*)(( void*) Cyc_Parse_ExternC_sc);
_temp2725;}); _temp2724;}); break; case 39: _LL2710: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2727=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2727[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2728; _temp2728.tag= Cyc_StorageClass_tok_tag;
_temp2728.f1=( void*)(( void*) Cyc_Parse_Typedef_sc); _temp2728;}); _temp2727;});
break; case 40: _LL2726: yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct*
_temp2730=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2730[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2731; _temp2731.tag=
Cyc_StorageClass_tok_tag; _temp2731.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp2731;}); _temp2730;}); break; case 41: _LL2729: yyval=( struct
_xtunion_struct*)({ struct Cyc_AttributeList_tok_struct* _temp2733=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2733[ 0]=({
struct Cyc_AttributeList_tok_struct _temp2734; _temp2734.tag= Cyc_AttributeList_tok_tag;
_temp2734.f1= 0; _temp2734;}); _temp2733;}); break; case 42: _LL2732: yyval=({
struct _tagged_ptr2 _temp2736= yyvs; struct _xtunion_struct** _temp2738=
_temp2736.curr + yyvsp_offset; if( _temp2736.base == 0? 1:( _temp2738 <
_temp2736.base? 1: _temp2738 >= _temp2736.last_plus_one)){ _throw(
Null_Exception);}* _temp2738;}); break; case 43: _LL2735: yyval=( struct
_xtunion_struct*)({ struct Cyc_AttributeList_tok_struct* _temp2740=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2740[ 0]=({
struct Cyc_AttributeList_tok_struct _temp2741; _temp2741.tag= Cyc_AttributeList_tok_tag;
_temp2741.f1= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2742= yyvs;
struct _xtunion_struct** _temp2744= _temp2742.curr +( yyvsp_offset - 2); if(
_temp2742.base == 0? 1:( _temp2744 < _temp2742.base? 1: _temp2744 >= _temp2742.last_plus_one)){
_throw( Null_Exception);}* _temp2744;})); _temp2741;}); _temp2740;}); break;
case 44: _LL2739: yyval=( struct _xtunion_struct*)({ struct Cyc_AttributeList_tok_struct*
_temp2746=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2746[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2747; _temp2747.tag=
Cyc_AttributeList_tok_tag; _temp2747.f1=({ struct Cyc_List_List* _temp2748=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2748->hd=(
void*) Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2749= yyvs; struct
_xtunion_struct** _temp2751= _temp2749.curr + yyvsp_offset; if( _temp2749.base
== 0? 1:( _temp2751 < _temp2749.base? 1: _temp2751 >= _temp2749.last_plus_one)){
_throw( Null_Exception);}* _temp2751;})); _temp2748->tl= 0; _temp2748;});
_temp2747;}); _temp2746;}); break; case 45: _LL2745: yyval=( struct
_xtunion_struct*)({ struct Cyc_AttributeList_tok_struct* _temp2753=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2753[ 0]=({
struct Cyc_AttributeList_tok_struct _temp2754; _temp2754.tag= Cyc_AttributeList_tok_tag;
_temp2754.f1=({ struct Cyc_List_List* _temp2755=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2755->hd=( void*) Cyc_yyget_Attribute_tok(({
struct _tagged_ptr2 _temp2759= yyvs; struct _xtunion_struct** _temp2761=
_temp2759.curr +( yyvsp_offset - 2); if( _temp2759.base == 0? 1:( _temp2761 <
_temp2759.base? 1: _temp2761 >= _temp2759.last_plus_one)){ _throw(
Null_Exception);}* _temp2761;})); _temp2755->tl= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2756= yyvs; struct _xtunion_struct** _temp2758=
_temp2756.curr + yyvsp_offset; if( _temp2756.base == 0? 1:( _temp2758 <
_temp2756.base? 1: _temp2758 >= _temp2756.last_plus_one)){ _throw(
Null_Exception);}* _temp2758;})); _temp2755;}); _temp2754;}); _temp2753;});
break; case 46: _LL2752: { struct _tagged_string s= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2763= yyvs; struct _xtunion_struct** _temp2765=
_temp2763.curr + yyvsp_offset; if( _temp2763.base == 0? 1:( _temp2765 <
_temp2763.base? 1: _temp2765 >= _temp2763.last_plus_one)){ _throw(
Null_Exception);}* _temp2765;})); void* a; if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2766=( char*)"stdcall"; struct _tagged_string
_temp2767; _temp2767.curr= _temp2766; _temp2767.base= _temp2766; _temp2767.last_plus_one=
_temp2766 + 8; _temp2767;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2768=( char*)"__stdcall__"; struct _tagged_string
_temp2769; _temp2769.curr= _temp2768; _temp2769.base= _temp2768; _temp2769.last_plus_one=
_temp2768 + 12; _temp2769;})) == 0){ a=( void*) Cyc_Absyn_Stdcall_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2770=( char*)"cdecl";
struct _tagged_string _temp2771; _temp2771.curr= _temp2770; _temp2771.base=
_temp2770; _temp2771.last_plus_one= _temp2770 + 6; _temp2771;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2772=( char*)"__cdecl__"; struct
_tagged_string _temp2773; _temp2773.curr= _temp2772; _temp2773.base= _temp2772;
_temp2773.last_plus_one= _temp2772 + 10; _temp2773;})) == 0){ a=( void*) Cyc_Absyn_Cdecl_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2774=(
char*)"noreturn"; struct _tagged_string _temp2775; _temp2775.curr= _temp2774;
_temp2775.base= _temp2774; _temp2775.last_plus_one= _temp2774 + 9; _temp2775;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2776=( char*)"__noreturn__";
struct _tagged_string _temp2777; _temp2777.curr= _temp2776; _temp2777.base=
_temp2776; _temp2777.last_plus_one= _temp2776 + 13; _temp2777;})) == 0){ a=(
void*) Cyc_Absyn_Noreturn_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2778=( char*)"noreturn"; struct _tagged_string
_temp2779; _temp2779.curr= _temp2778; _temp2779.base= _temp2778; _temp2779.last_plus_one=
_temp2778 + 9; _temp2779;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2780=( char*)"__noreturn__"; struct _tagged_string
_temp2781; _temp2781.curr= _temp2780; _temp2781.base= _temp2780; _temp2781.last_plus_one=
_temp2780 + 13; _temp2781;})) == 0){ a=( void*) Cyc_Absyn_Noreturn_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2782=( char*)"__const__";
struct _tagged_string _temp2783; _temp2783.curr= _temp2782; _temp2783.base=
_temp2782; _temp2783.last_plus_one= _temp2782 + 10; _temp2783;})) == 0){ a=(
void*) Cyc_Absyn_Const_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2784=( char*)"aligned"; struct _tagged_string
_temp2785; _temp2785.curr= _temp2784; _temp2785.base= _temp2784; _temp2785.last_plus_one=
_temp2784 + 8; _temp2785;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2786=( char*)"__aligned__"; struct _tagged_string
_temp2787; _temp2787.curr= _temp2786; _temp2787.base= _temp2786; _temp2787.last_plus_one=
_temp2786 + 12; _temp2787;})) == 0){ a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp2788=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct));
_temp2788[ 0]=({ struct Cyc_Absyn_Aligned_att_struct _temp2789; _temp2789.tag=
Cyc_Absyn_Aligned_att_tag; _temp2789.f1= - 1; _temp2789;}); _temp2788;});} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2790=( char*)"packed";
struct _tagged_string _temp2791; _temp2791.curr= _temp2790; _temp2791.base=
_temp2790; _temp2791.last_plus_one= _temp2790 + 7; _temp2791;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2792=( char*)"__packed__"; struct
_tagged_string _temp2793; _temp2793.curr= _temp2792; _temp2793.base= _temp2792;
_temp2793.last_plus_one= _temp2792 + 11; _temp2793;})) == 0){ a=( void*) Cyc_Absyn_Packed_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2794=(
char*)"shared"; struct _tagged_string _temp2795; _temp2795.curr= _temp2794;
_temp2795.base= _temp2794; _temp2795.last_plus_one= _temp2794 + 7; _temp2795;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2796=( char*)"__shared__";
struct _tagged_string _temp2797; _temp2797.curr= _temp2796; _temp2797.base=
_temp2796; _temp2797.last_plus_one= _temp2796 + 11; _temp2797;})) == 0){ a=(
void*) Cyc_Absyn_Shared_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2798=( char*)"unused"; struct _tagged_string
_temp2799; _temp2799.curr= _temp2798; _temp2799.base= _temp2798; _temp2799.last_plus_one=
_temp2798 + 7; _temp2799;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2800=( char*)"__unused__"; struct _tagged_string
_temp2801; _temp2801.curr= _temp2800; _temp2801.base= _temp2800; _temp2801.last_plus_one=
_temp2800 + 11; _temp2801;})) == 0){ a=( void*) Cyc_Absyn_Unused_att;} else{ if(
Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2802=( char*)"weak";
struct _tagged_string _temp2803; _temp2803.curr= _temp2802; _temp2803.base=
_temp2802; _temp2803.last_plus_one= _temp2802 + 5; _temp2803;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2804=( char*)"__weak__"; struct
_tagged_string _temp2805; _temp2805.curr= _temp2804; _temp2805.base= _temp2804;
_temp2805.last_plus_one= _temp2804 + 9; _temp2805;})) == 0){ a=( void*) Cyc_Absyn_Weak_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2806=(
char*)"dllimport"; struct _tagged_string _temp2807; _temp2807.curr= _temp2806;
_temp2807.base= _temp2806; _temp2807.last_plus_one= _temp2806 + 10; _temp2807;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2808=( char*)"__dllimport__";
struct _tagged_string _temp2809; _temp2809.curr= _temp2808; _temp2809.base=
_temp2808; _temp2809.last_plus_one= _temp2808 + 14; _temp2809;})) == 0){ a=(
void*) Cyc_Absyn_Dllimport_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2810=( char*)"dllexport"; struct _tagged_string
_temp2811; _temp2811.curr= _temp2810; _temp2811.base= _temp2810; _temp2811.last_plus_one=
_temp2810 + 10; _temp2811;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2812=( char*)"__dllexport__"; struct _tagged_string
_temp2813; _temp2813.curr= _temp2812; _temp2813.base= _temp2812; _temp2813.last_plus_one=
_temp2812 + 14; _temp2813;})) == 0){ a=( void*) Cyc_Absyn_Dllexport_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2814=( char*)"no_instrument_function";
struct _tagged_string _temp2815; _temp2815.curr= _temp2814; _temp2815.base=
_temp2814; _temp2815.last_plus_one= _temp2814 + 23; _temp2815;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2816=( char*)"__no_instrument_function__";
struct _tagged_string _temp2817; _temp2817.curr= _temp2816; _temp2817.base=
_temp2816; _temp2817.last_plus_one= _temp2816 + 27; _temp2817;})) == 0){ a=(
void*) Cyc_Absyn_No_instrument_function_att;} else{ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2818=( char*)"constructor"; struct
_tagged_string _temp2819; _temp2819.curr= _temp2818; _temp2819.base= _temp2818;
_temp2819.last_plus_one= _temp2818 + 12; _temp2819;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2820=( char*)"__constructor__"; struct
_tagged_string _temp2821; _temp2821.curr= _temp2820; _temp2821.base= _temp2820;
_temp2821.last_plus_one= _temp2820 + 16; _temp2821;})) == 0){ a=( void*) Cyc_Absyn_Constructor_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2822=(
char*)"destructor"; struct _tagged_string _temp2823; _temp2823.curr= _temp2822;
_temp2823.base= _temp2822; _temp2823.last_plus_one= _temp2822 + 11; _temp2823;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2824=( char*)"__destructor__";
struct _tagged_string _temp2825; _temp2825.curr= _temp2824; _temp2825.base=
_temp2824; _temp2825.last_plus_one= _temp2824 + 15; _temp2825;})) == 0){ a=(
void*) Cyc_Absyn_Destructor_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2826=( char*)"no_check_memory_usage"; struct
_tagged_string _temp2827; _temp2827.curr= _temp2826; _temp2827.base= _temp2826;
_temp2827.last_plus_one= _temp2826 + 22; _temp2827;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2828=( char*)"__no_check_memory_usage__";
struct _tagged_string _temp2829; _temp2829.curr= _temp2828; _temp2829.base=
_temp2828; _temp2829.last_plus_one= _temp2828 + 26; _temp2829;})) == 0){ a=(
void*) Cyc_Absyn_No_check_memory_usage_att;} else{ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2830=( char*)"unrecognized attribute"; struct
_tagged_string _temp2831; _temp2831.curr= _temp2830; _temp2831.base= _temp2830;
_temp2831.last_plus_one= _temp2830 + 23; _temp2831;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2832= yyls; struct Cyc_Yyltype* _temp2834= _temp2832.curr
+ yylsp_offset; if( _temp2832.base == 0? 1:( _temp2834 < _temp2832.base? 1:
_temp2834 >= _temp2832.last_plus_one)){ _throw( Null_Exception);}* _temp2834;}).first_line,({
struct _tagged_ptr3 _temp2835= yyls; struct Cyc_Yyltype* _temp2837= _temp2835.curr
+ yylsp_offset; if( _temp2835.base == 0? 1:( _temp2837 < _temp2835.base? 1:
_temp2837 >= _temp2835.last_plus_one)){ _throw( Null_Exception);}* _temp2837;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}}}}}}}}}}}}}}}} yyval=( struct _xtunion_struct*)({
struct Cyc_Attribute_tok_struct* _temp2838=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2838[ 0]=({ struct
Cyc_Attribute_tok_struct _temp2839; _temp2839.tag= Cyc_Attribute_tok_tag;
_temp2839.f1=( void*) a; _temp2839;}); _temp2838;}); break;} case 47: _LL2762:
yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct* _temp2841=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2841[ 0]=({ struct Cyc_Attribute_tok_struct _temp2842; _temp2842.tag= Cyc_Attribute_tok_tag;
_temp2842.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp2842;}); _temp2841;});
break; case 48: _LL2840: { struct _tagged_string s= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2844= yyvs; struct _xtunion_struct** _temp2846=
_temp2844.curr +( yyvsp_offset - 3); if( _temp2844.base == 0? 1:( _temp2846 <
_temp2844.base? 1: _temp2846 >= _temp2844.last_plus_one)){ _throw(
Null_Exception);}* _temp2846;})); struct _tuple12 _temp2852; int _temp2853; void*
_temp2855; struct _tuple12* _temp2850= Cyc_yyget_Int_tok(({ struct _tagged_ptr2
_temp2847= yyvs; struct _xtunion_struct** _temp2849= _temp2847.curr +(
yyvsp_offset - 1); if( _temp2847.base == 0? 1:( _temp2849 < _temp2847.base? 1:
_temp2849 >= _temp2847.last_plus_one)){ _throw( Null_Exception);}* _temp2849;}));
_temp2852=* _temp2850; _LL2856: _temp2855= _temp2852.f1; goto _LL2854; _LL2854:
_temp2853= _temp2852.f2; goto _LL2851; _LL2851: { void* a; if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2857=( char*)"regparm"; struct
_tagged_string _temp2858; _temp2858.curr= _temp2857; _temp2858.base= _temp2857;
_temp2858.last_plus_one= _temp2857 + 8; _temp2858;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2859=( char*)"__regparm__"; struct
_tagged_string _temp2860; _temp2860.curr= _temp2859; _temp2860.base= _temp2859;
_temp2860.last_plus_one= _temp2859 + 12; _temp2860;})) == 0){ if( _temp2853 <= 0?
1: _temp2853 > 3){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2861=(
char*)"regparm requires value between 1 and 3"; struct _tagged_string _temp2862;
_temp2862.curr= _temp2861; _temp2862.base= _temp2861; _temp2862.last_plus_one=
_temp2861 + 39; _temp2862;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2863= yyls; struct Cyc_Yyltype* _temp2865= _temp2863.curr +( yylsp_offset -
1); if( _temp2863.base == 0? 1:( _temp2865 < _temp2863.base? 1: _temp2865 >=
_temp2863.last_plus_one)){ _throw( Null_Exception);}* _temp2865;}).first_line,({
struct _tagged_ptr3 _temp2866= yyls; struct Cyc_Yyltype* _temp2868= _temp2866.curr
+( yylsp_offset - 1); if( _temp2866.base == 0? 1:( _temp2868 < _temp2866.base? 1:
_temp2868 >= _temp2866.last_plus_one)){ _throw( Null_Exception);}* _temp2868;}).last_line));}
a=( void*)({ struct Cyc_Absyn_Regparm_att_struct* _temp2869=( struct Cyc_Absyn_Regparm_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Regparm_att_struct)); _temp2869[ 0]=({
struct Cyc_Absyn_Regparm_att_struct _temp2870; _temp2870.tag= Cyc_Absyn_Regparm_att_tag;
_temp2870.f1= _temp2853; _temp2870;}); _temp2869;});} else{ if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2871=( char*)"aligned"; struct
_tagged_string _temp2872; _temp2872.curr= _temp2871; _temp2872.base= _temp2871;
_temp2872.last_plus_one= _temp2871 + 8; _temp2872;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2873=( char*)"__aligned__"; struct
_tagged_string _temp2874; _temp2874.curr= _temp2873; _temp2874.base= _temp2873;
_temp2874.last_plus_one= _temp2873 + 12; _temp2874;})) == 0){ if( _temp2853 < 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2875=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2876; _temp2876.curr= _temp2875; _temp2876.base=
_temp2875; _temp2876.last_plus_one= _temp2875 + 39; _temp2876;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2877= yyls; struct Cyc_Yyltype* _temp2879= _temp2877.curr
+( yylsp_offset - 1); if( _temp2877.base == 0? 1:( _temp2879 < _temp2877.base? 1:
_temp2879 >= _temp2877.last_plus_one)){ _throw( Null_Exception);}* _temp2879;}).first_line,({
struct _tagged_ptr3 _temp2880= yyls; struct Cyc_Yyltype* _temp2882= _temp2880.curr
+( yylsp_offset - 1); if( _temp2880.base == 0? 1:( _temp2882 < _temp2880.base? 1:
_temp2882 >= _temp2880.last_plus_one)){ _throw( Null_Exception);}* _temp2882;}).last_line));}{
unsigned int j=( unsigned int) _temp2853; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2883=(
char*)"aligned requires positive power of two"; struct _tagged_string _temp2884;
_temp2884.curr= _temp2883; _temp2884.base= _temp2883; _temp2884.last_plus_one=
_temp2883 + 39; _temp2884;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2885= yyls; struct Cyc_Yyltype* _temp2887= _temp2885.curr +( yylsp_offset -
1); if( _temp2885.base == 0? 1:( _temp2887 < _temp2885.base? 1: _temp2887 >=
_temp2885.last_plus_one)){ _throw( Null_Exception);}* _temp2887;}).first_line,({
struct _tagged_ptr3 _temp2888= yyls; struct Cyc_Yyltype* _temp2890= _temp2888.curr
+( yylsp_offset - 1); if( _temp2888.base == 0? 1:( _temp2890 < _temp2888.base? 1:
_temp2890 >= _temp2888.last_plus_one)){ _throw( Null_Exception);}* _temp2890;}).last_line));}
a=( void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp2891=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp2891[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp2892; _temp2892.tag= Cyc_Absyn_Aligned_att_tag;
_temp2892.f1= _temp2853; _temp2892;}); _temp2891;});}} else{ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2893=( char*)"unrecognized attribute";
struct _tagged_string _temp2894; _temp2894.curr= _temp2893; _temp2894.base=
_temp2893; _temp2894.last_plus_one= _temp2893 + 23; _temp2894;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2895= yyls; struct Cyc_Yyltype* _temp2897= _temp2895.curr
+( yylsp_offset - 3); if( _temp2895.base == 0? 1:( _temp2897 < _temp2895.base? 1:
_temp2897 >= _temp2895.last_plus_one)){ _throw( Null_Exception);}* _temp2897;}).first_line,({
struct _tagged_ptr3 _temp2898= yyls; struct Cyc_Yyltype* _temp2900= _temp2898.curr
+( yylsp_offset - 3); if( _temp2898.base == 0? 1:( _temp2900 < _temp2898.base? 1:
_temp2900 >= _temp2898.last_plus_one)){ _throw( Null_Exception);}* _temp2900;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}} yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct*
_temp2901=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2901[ 0]=({ struct Cyc_Attribute_tok_struct _temp2902; _temp2902.tag= Cyc_Attribute_tok_tag;
_temp2902.f1=( void*) a; _temp2902;}); _temp2901;}); break;}} case 49: _LL2843: {
struct _tagged_string s= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2904=
yyvs; struct _xtunion_struct** _temp2906= _temp2904.curr +( yyvsp_offset - 3);
if( _temp2904.base == 0? 1:( _temp2906 < _temp2904.base? 1: _temp2906 >=
_temp2904.last_plus_one)){ _throw( Null_Exception);}* _temp2906;})); struct
_tagged_string str= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2907= yyvs;
struct _xtunion_struct** _temp2909= _temp2907.curr +( yyvsp_offset - 1); if(
_temp2907.base == 0? 1:( _temp2909 < _temp2907.base? 1: _temp2909 >= _temp2907.last_plus_one)){
_throw( Null_Exception);}* _temp2909;})); void* a; if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2910=( char*)"section"; struct
_tagged_string _temp2911; _temp2911.curr= _temp2910; _temp2911.base= _temp2910;
_temp2911.last_plus_one= _temp2910 + 8; _temp2911;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2912=( char*)"__section__"; struct
_tagged_string _temp2913; _temp2913.curr= _temp2912; _temp2913.base= _temp2912;
_temp2913.last_plus_one= _temp2912 + 12; _temp2913;}))){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp2914=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp2914[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp2915; _temp2915.tag=
Cyc_Absyn_Section_att_tag; _temp2915.f1= str; _temp2915;}); _temp2914;});} else{
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2916=( char*)"unrecognized attribute";
struct _tagged_string _temp2917; _temp2917.curr= _temp2916; _temp2917.base=
_temp2916; _temp2917.last_plus_one= _temp2916 + 23; _temp2917;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2918= yyls; struct Cyc_Yyltype* _temp2920= _temp2918.curr
+( yylsp_offset - 3); if( _temp2918.base == 0? 1:( _temp2920 < _temp2918.base? 1:
_temp2920 >= _temp2918.last_plus_one)){ _throw( Null_Exception);}* _temp2920;}).first_line,({
struct _tagged_ptr3 _temp2921= yyls; struct Cyc_Yyltype* _temp2923= _temp2921.curr
+( yylsp_offset - 3); if( _temp2921.base == 0? 1:( _temp2923 < _temp2921.base? 1:
_temp2923 >= _temp2921.last_plus_one)){ _throw( Null_Exception);}* _temp2923;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct*
_temp2924=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2924[ 0]=({ struct Cyc_Attribute_tok_struct _temp2925; _temp2925.tag= Cyc_Attribute_tok_tag;
_temp2925.f1=( void*) a; _temp2925;}); _temp2924;}); break;} case 50: _LL2903:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2927=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2927[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2928; _temp2928.tag=
Cyc_TypeSpecifier_tok_tag; _temp2928.f1=( void*) Cyc_Parse_type_spec(( void*)
Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2929=
yyls; struct Cyc_Yyltype* _temp2931= _temp2929.curr + yylsp_offset; if(
_temp2929.base == 0? 1:( _temp2931 < _temp2929.base? 1: _temp2931 >= _temp2929.last_plus_one)){
_throw( Null_Exception);}* _temp2931;}).first_line,({ struct _tagged_ptr3
_temp2932= yyls; struct Cyc_Yyltype* _temp2934= _temp2932.curr + yylsp_offset;
if( _temp2932.base == 0? 1:( _temp2934 < _temp2932.base? 1: _temp2934 >=
_temp2932.last_plus_one)){ _throw( Null_Exception);}* _temp2934;}).last_line));
_temp2928;}); _temp2927;}); break; case 51: _LL2926: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2936=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2936[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2937; _temp2937.tag= Cyc_TypeSpecifier_tok_tag;
_temp2937.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2938= yyls; struct Cyc_Yyltype*
_temp2940= _temp2938.curr + yylsp_offset; if( _temp2938.base == 0? 1:( _temp2940
< _temp2938.base? 1: _temp2940 >= _temp2938.last_plus_one)){ _throw(
Null_Exception);}* _temp2940;}).first_line,({ struct _tagged_ptr3 _temp2941=
yyls; struct Cyc_Yyltype* _temp2943= _temp2941.curr + yylsp_offset; if(
_temp2941.base == 0? 1:( _temp2943 < _temp2941.base? 1: _temp2943 >= _temp2941.last_plus_one)){
_throw( Null_Exception);}* _temp2943;}).last_line)); _temp2937;}); _temp2936;});
break; case 52: _LL2935: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2945=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2945[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2946; _temp2946.tag=
Cyc_TypeSpecifier_tok_tag; _temp2946.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2947= yyls; struct Cyc_Yyltype*
_temp2949= _temp2947.curr + yylsp_offset; if( _temp2947.base == 0? 1:( _temp2949
< _temp2947.base? 1: _temp2949 >= _temp2947.last_plus_one)){ _throw(
Null_Exception);}* _temp2949;}).first_line,({ struct _tagged_ptr3 _temp2950=
yyls; struct Cyc_Yyltype* _temp2952= _temp2950.curr + yylsp_offset; if(
_temp2950.base == 0? 1:( _temp2952 < _temp2950.base? 1: _temp2952 >= _temp2950.last_plus_one)){
_throw( Null_Exception);}* _temp2952;}).last_line)); _temp2946;}); _temp2945;});
break; case 53: _LL2944: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2954=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2954[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2955; _temp2955.tag=
Cyc_TypeSpecifier_tok_tag; _temp2955.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp2956=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp2956[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp2957; _temp2957.tag=
Cyc_Parse_Short_spec_tag; _temp2957.f1= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2958= yyls; struct Cyc_Yyltype* _temp2960= _temp2958.curr +
yylsp_offset; if( _temp2958.base == 0? 1:( _temp2960 < _temp2958.base? 1:
_temp2960 >= _temp2958.last_plus_one)){ _throw( Null_Exception);}* _temp2960;}).first_line,({
struct _tagged_ptr3 _temp2961= yyls; struct Cyc_Yyltype* _temp2963= _temp2961.curr
+ yylsp_offset; if( _temp2961.base == 0? 1:( _temp2963 < _temp2961.base? 1:
_temp2963 >= _temp2961.last_plus_one)){ _throw( Null_Exception);}* _temp2963;}).last_line);
_temp2957;}); _temp2956;})); _temp2955;}); _temp2954;}); break; case 54: _LL2953:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2965=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2965[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2966; _temp2966.tag=
Cyc_TypeSpecifier_tok_tag; _temp2966.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2967= yyls; struct Cyc_Yyltype*
_temp2969= _temp2967.curr + yylsp_offset; if( _temp2967.base == 0? 1:( _temp2969
< _temp2967.base? 1: _temp2969 >= _temp2967.last_plus_one)){ _throw(
Null_Exception);}* _temp2969;}).first_line,({ struct _tagged_ptr3 _temp2970=
yyls; struct Cyc_Yyltype* _temp2972= _temp2970.curr + yylsp_offset; if(
_temp2970.base == 0? 1:( _temp2972 < _temp2970.base? 1: _temp2972 >= _temp2970.last_plus_one)){
_throw( Null_Exception);}* _temp2972;}).last_line)); _temp2966;}); _temp2965;});
break; case 55: _LL2964: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2974=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2974[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2975; _temp2975.tag=
Cyc_TypeSpecifier_tok_tag; _temp2975.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp2976=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp2976[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp2977; _temp2977.tag= Cyc_Parse_Long_spec_tag;
_temp2977.f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2978= yyls;
struct Cyc_Yyltype* _temp2980= _temp2978.curr + yylsp_offset; if( _temp2978.base
== 0? 1:( _temp2980 < _temp2978.base? 1: _temp2980 >= _temp2978.last_plus_one)){
_throw( Null_Exception);}* _temp2980;}).first_line,({ struct _tagged_ptr3
_temp2981= yyls; struct Cyc_Yyltype* _temp2983= _temp2981.curr + yylsp_offset;
if( _temp2981.base == 0? 1:( _temp2983 < _temp2981.base? 1: _temp2983 >=
_temp2981.last_plus_one)){ _throw( Null_Exception);}* _temp2983;}).last_line);
_temp2977;}); _temp2976;})); _temp2975;}); _temp2974;}); break; case 56: _LL2973:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2985=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2985[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2986; _temp2986.tag=
Cyc_TypeSpecifier_tok_tag; _temp2986.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2987= yyls; struct Cyc_Yyltype*
_temp2989= _temp2987.curr + yylsp_offset; if( _temp2987.base == 0? 1:( _temp2989
< _temp2987.base? 1: _temp2989 >= _temp2987.last_plus_one)){ _throw(
Null_Exception);}* _temp2989;}).first_line,({ struct _tagged_ptr3 _temp2990=
yyls; struct Cyc_Yyltype* _temp2992= _temp2990.curr + yylsp_offset; if(
_temp2990.base == 0? 1:( _temp2992 < _temp2990.base? 1: _temp2992 >= _temp2990.last_plus_one)){
_throw( Null_Exception);}* _temp2992;}).last_line)); _temp2986;}); _temp2985;});
break; case 57: _LL2984: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2994=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2994[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2995; _temp2995.tag=
Cyc_TypeSpecifier_tok_tag; _temp2995.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2996= yyls; struct Cyc_Yyltype*
_temp2998= _temp2996.curr + yylsp_offset; if( _temp2996.base == 0? 1:( _temp2998
< _temp2996.base? 1: _temp2998 >= _temp2996.last_plus_one)){ _throw(
Null_Exception);}* _temp2998;}).first_line,({ struct _tagged_ptr3 _temp2999=
yyls; struct Cyc_Yyltype* _temp3001= _temp2999.curr + yylsp_offset; if(
_temp2999.base == 0? 1:( _temp3001 < _temp2999.base? 1: _temp3001 >= _temp2999.last_plus_one)){
_throw( Null_Exception);}* _temp3001;}).last_line)); _temp2995;}); _temp2994;});
break; case 58: _LL2993: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3003=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3003[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3004; _temp3004.tag=
Cyc_TypeSpecifier_tok_tag; _temp3004.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp3005=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp3005[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp3006; _temp3006.tag=
Cyc_Parse_Signed_spec_tag; _temp3006.f1= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3007= yyls; struct Cyc_Yyltype* _temp3009= _temp3007.curr +
yylsp_offset; if( _temp3007.base == 0? 1:( _temp3009 < _temp3007.base? 1:
_temp3009 >= _temp3007.last_plus_one)){ _throw( Null_Exception);}* _temp3009;}).first_line,({
struct _tagged_ptr3 _temp3010= yyls; struct Cyc_Yyltype* _temp3012= _temp3010.curr
+ yylsp_offset; if( _temp3010.base == 0? 1:( _temp3012 < _temp3010.base? 1:
_temp3012 >= _temp3010.last_plus_one)){ _throw( Null_Exception);}* _temp3012;}).last_line);
_temp3006;}); _temp3005;})); _temp3004;}); _temp3003;}); break; case 59: _LL3002:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3014=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3014[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3015; _temp3015.tag=
Cyc_TypeSpecifier_tok_tag; _temp3015.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp3016=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp3016[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp3017; _temp3017.tag= Cyc_Parse_Unsigned_spec_tag; _temp3017.f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3018= yyls; struct Cyc_Yyltype* _temp3020= _temp3018.curr
+ yylsp_offset; if( _temp3018.base == 0? 1:( _temp3020 < _temp3018.base? 1:
_temp3020 >= _temp3018.last_plus_one)){ _throw( Null_Exception);}* _temp3020;}).first_line,({
struct _tagged_ptr3 _temp3021= yyls; struct Cyc_Yyltype* _temp3023= _temp3021.curr
+ yylsp_offset; if( _temp3021.base == 0? 1:( _temp3023 < _temp3021.base? 1:
_temp3023 >= _temp3021.last_plus_one)){ _throw( Null_Exception);}* _temp3023;}).last_line);
_temp3017;}); _temp3016;})); _temp3015;}); _temp3014;}); break; case 60: _LL3013:
yyval=({ struct _tagged_ptr2 _temp3025= yyvs; struct _xtunion_struct** _temp3027=
_temp3025.curr + yyvsp_offset; if( _temp3025.base == 0? 1:( _temp3027 <
_temp3025.base? 1: _temp3027 >= _temp3025.last_plus_one)){ _throw(
Null_Exception);}* _temp3027;}); break; case 61: _LL3024: yyval=({ struct
_tagged_ptr2 _temp3029= yyvs; struct _xtunion_struct** _temp3031= _temp3029.curr
+ yyvsp_offset; if( _temp3029.base == 0? 1:( _temp3031 < _temp3029.base? 1:
_temp3031 >= _temp3029.last_plus_one)){ _throw( Null_Exception);}* _temp3031;});
break; case 62: _LL3028: yyval=({ struct _tagged_ptr2 _temp3033= yyvs; struct
_xtunion_struct** _temp3035= _temp3033.curr + yyvsp_offset; if( _temp3033.base
== 0? 1:( _temp3035 < _temp3033.base? 1: _temp3035 >= _temp3033.last_plus_one)){
_throw( Null_Exception);}* _temp3035;}); break; case 63: _LL3032: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3037=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3037[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3038; _temp3038.tag= Cyc_TypeSpecifier_tok_tag;
_temp3038.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp3039= yyvs; struct _xtunion_struct** _temp3041=
_temp3039.curr + yyvsp_offset; if( _temp3039.base == 0? 1:( _temp3041 <
_temp3039.base? 1: _temp3041 >= _temp3039.last_plus_one)){ _throw(
Null_Exception);}* _temp3041;})),(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3042= yyls; struct Cyc_Yyltype*
_temp3044= _temp3042.curr + yylsp_offset; if( _temp3042.base == 0? 1:( _temp3044
< _temp3042.base? 1: _temp3044 >= _temp3042.last_plus_one)){ _throw(
Null_Exception);}* _temp3044;}).first_line,({ struct _tagged_ptr3 _temp3045=
yyls; struct Cyc_Yyltype* _temp3047= _temp3045.curr + yylsp_offset; if(
_temp3045.base == 0? 1:( _temp3047 < _temp3045.base? 1: _temp3047 >= _temp3045.last_plus_one)){
_throw( Null_Exception);}* _temp3047;}).last_line)); _temp3038;}); _temp3037;});
break; case 64: _LL3036: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3049=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3049[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3050; _temp3050.tag=
Cyc_TypeSpecifier_tok_tag; _temp3050.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3051= yyvs; struct
_xtunion_struct** _temp3053= _temp3051.curr +( yyvsp_offset - 2); if( _temp3051.base
== 0? 1:( _temp3053 < _temp3051.base? 1: _temp3053 >= _temp3051.last_plus_one)){
_throw( Null_Exception);}* _temp3053;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp3054= yyvs;
struct _xtunion_struct** _temp3056= _temp3054.curr + yyvsp_offset; if( _temp3054.base
== 0? 1:( _temp3056 < _temp3054.base? 1: _temp3056 >= _temp3054.last_plus_one)){
_throw( Null_Exception);}* _temp3056;})))), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3057= yyls; struct Cyc_Yyltype* _temp3059= _temp3057.curr
+( yylsp_offset - 2); if( _temp3057.base == 0? 1:( _temp3059 < _temp3057.base? 1:
_temp3059 >= _temp3057.last_plus_one)){ _throw( Null_Exception);}* _temp3059;}).first_line,({
struct _tagged_ptr3 _temp3060= yyls; struct Cyc_Yyltype* _temp3062= _temp3060.curr
+ yylsp_offset; if( _temp3060.base == 0? 1:( _temp3062 < _temp3060.base? 1:
_temp3062 >= _temp3060.last_plus_one)){ _throw( Null_Exception);}* _temp3062;}).last_line));
_temp3050;}); _temp3049;}); break; case 65: _LL3048: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3064=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3064[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3065; _temp3065.tag= Cyc_TypeSpecifier_tok_tag;
_temp3065.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp3066=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp3066[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp3067; _temp3067.tag=
Cyc_Absyn_TypedefType_tag; _temp3067.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3068= yyvs; struct _xtunion_struct** _temp3070= _temp3068.curr
+( yyvsp_offset - 1); if( _temp3068.base == 0? 1:( _temp3070 < _temp3068.base? 1:
_temp3070 >= _temp3068.last_plus_one)){ _throw( Null_Exception);}* _temp3070;}));
_temp3067.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3071= yyvs;
struct _xtunion_struct** _temp3073= _temp3071.curr + yyvsp_offset; if( _temp3071.base
== 0? 1:( _temp3073 < _temp3071.base? 1: _temp3073 >= _temp3071.last_plus_one)){
_throw( Null_Exception);}* _temp3073;})); _temp3067.f3= 0; _temp3067;});
_temp3066;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3074= yyls;
struct Cyc_Yyltype* _temp3076= _temp3074.curr +( yylsp_offset - 1); if(
_temp3074.base == 0? 1:( _temp3076 < _temp3074.base? 1: _temp3076 >= _temp3074.last_plus_one)){
_throw( Null_Exception);}* _temp3076;}).first_line,({ struct _tagged_ptr3
_temp3077= yyls; struct Cyc_Yyltype* _temp3079= _temp3077.curr + yylsp_offset;
if( _temp3077.base == 0? 1:( _temp3079 < _temp3077.base? 1: _temp3079 >=
_temp3077.last_plus_one)){ _throw( Null_Exception);}* _temp3079;}).last_line));
_temp3065;}); _temp3064;}); break; case 66: _LL3063: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3081=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3081[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3082; _temp3082.tag= Cyc_TypeSpecifier_tok_tag;
_temp3082.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp3083=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp3083[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp3084; _temp3084.tag= Cyc_Absyn_TupleType_tag;
_temp3084.f1=(( struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3085= yyls; struct Cyc_Yyltype* _temp3087= _temp3085.curr +(
yylsp_offset - 1); if( _temp3085.base == 0? 1:( _temp3087 < _temp3085.base? 1:
_temp3087 >= _temp3085.last_plus_one)){ _throw( Null_Exception);}* _temp3087;}).first_line,({
struct _tagged_ptr3 _temp3088= yyls; struct Cyc_Yyltype* _temp3090= _temp3088.curr
+( yylsp_offset - 1); if( _temp3088.base == 0? 1:( _temp3090 < _temp3088.base? 1:
_temp3090 >= _temp3088.last_plus_one)){ _throw( Null_Exception);}* _temp3090;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3091= yyvs; struct _xtunion_struct** _temp3093=
_temp3091.curr +( yyvsp_offset - 1); if( _temp3091.base == 0? 1:( _temp3093 <
_temp3091.base? 1: _temp3093 >= _temp3091.last_plus_one)){ _throw(
Null_Exception);}* _temp3093;})))); _temp3084;}); _temp3083;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3094= yyls; struct Cyc_Yyltype* _temp3096= _temp3094.curr
+( yylsp_offset - 3); if( _temp3094.base == 0? 1:( _temp3096 < _temp3094.base? 1:
_temp3096 >= _temp3094.last_plus_one)){ _throw( Null_Exception);}* _temp3096;}).first_line,({
struct _tagged_ptr3 _temp3097= yyls; struct Cyc_Yyltype* _temp3099= _temp3097.curr
+ yylsp_offset; if( _temp3097.base == 0? 1:( _temp3099 < _temp3097.base? 1:
_temp3099 >= _temp3097.last_plus_one)){ _throw( Null_Exception);}* _temp3099;}).last_line));
_temp3082;}); _temp3081;}); break; case 67: _LL3080: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3101=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3101[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3102; _temp3102.tag= Cyc_TypeSpecifier_tok_tag;
_temp3102.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3103=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3103[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3104; _temp3104.tag= Cyc_Absyn_RgnHandleType_tag; _temp3104.f1=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp3105= yyvs; struct _xtunion_struct** _temp3107=
_temp3105.curr +( yyvsp_offset - 1); if( _temp3105.base == 0? 1:( _temp3107 <
_temp3105.base? 1: _temp3107 >= _temp3105.last_plus_one)){ _throw(
Null_Exception);}* _temp3107;})); _temp3104;}); _temp3103;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3108= yyls; struct Cyc_Yyltype* _temp3110= _temp3108.curr
+( yylsp_offset - 3); if( _temp3108.base == 0? 1:( _temp3110 < _temp3108.base? 1:
_temp3110 >= _temp3108.last_plus_one)){ _throw( Null_Exception);}* _temp3110;}).first_line,({
struct _tagged_ptr3 _temp3111= yyls; struct Cyc_Yyltype* _temp3113= _temp3111.curr
+ yylsp_offset; if( _temp3111.base == 0? 1:( _temp3113 < _temp3111.base? 1:
_temp3113 >= _temp3111.last_plus_one)){ _throw( Null_Exception);}* _temp3113;}).last_line));
_temp3102;}); _temp3101;}); break; case 68: _LL3100: yyval=( struct
_xtunion_struct*)({ struct Cyc_Kind_tok_struct* _temp3115=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp3115[ 0]=({ struct Cyc_Kind_tok_struct
_temp3116; _temp3116.tag= Cyc_Kind_tok_tag; _temp3116.f1=( void*) Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3117= yyvs; struct
_xtunion_struct** _temp3119= _temp3117.curr + yyvsp_offset; if( _temp3117.base
== 0? 1:( _temp3119 < _temp3117.base? 1: _temp3119 >= _temp3117.last_plus_one)){
_throw( Null_Exception);}* _temp3119;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3120= yyls; struct Cyc_Yyltype* _temp3122= _temp3120.curr +
yylsp_offset; if( _temp3120.base == 0? 1:( _temp3122 < _temp3120.base? 1:
_temp3122 >= _temp3120.last_plus_one)){ _throw( Null_Exception);}* _temp3122;}).first_line,({
struct _tagged_ptr3 _temp3123= yyls; struct Cyc_Yyltype* _temp3125= _temp3123.curr
+ yylsp_offset; if( _temp3123.base == 0? 1:( _temp3125 < _temp3123.base? 1:
_temp3125 >= _temp3123.last_plus_one)){ _throw( Null_Exception);}* _temp3125;}).last_line));
_temp3116;}); _temp3115;}); break; case 69: _LL3114: { struct _tagged_string*
_temp3132; void* _temp3134; struct _tuple1 _temp3130=* Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3127= yyvs; struct _xtunion_struct** _temp3129=
_temp3127.curr + yyvsp_offset; if( _temp3127.base == 0? 1:( _temp3129 <
_temp3127.base? 1: _temp3129 >= _temp3127.last_plus_one)){ _throw(
Null_Exception);}* _temp3129;})); _LL3135: _temp3134= _temp3130.f1; goto _LL3133;
_LL3133: _temp3132= _temp3130.f2; goto _LL3131; _LL3131: if( _temp3134 !=( void*)
Cyc_Absyn_Loc_n){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp3136=(
char*)"bad kind in type specifier"; struct _tagged_string _temp3137; _temp3137.curr=
_temp3136; _temp3137.base= _temp3136; _temp3137.last_plus_one= _temp3136 + 27;
_temp3137;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3138= yyls;
struct Cyc_Yyltype* _temp3140= _temp3138.curr + yylsp_offset; if( _temp3138.base
== 0? 1:( _temp3140 < _temp3138.base? 1: _temp3140 >= _temp3138.last_plus_one)){
_throw( Null_Exception);}* _temp3140;}).first_line,({ struct _tagged_ptr3
_temp3141= yyls; struct Cyc_Yyltype* _temp3143= _temp3141.curr + yylsp_offset;
if( _temp3141.base == 0? 1:( _temp3143 < _temp3141.base? 1: _temp3143 >=
_temp3141.last_plus_one)){ _throw( Null_Exception);}* _temp3143;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_Kind_tok_struct* _temp3144=(
struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp3144[ 0]=({ struct Cyc_Kind_tok_struct _temp3145; _temp3145.tag= Cyc_Kind_tok_tag;
_temp3145.f1=( void*) Cyc_Parse_id_to_kind(* _temp3132, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3146= yyls; struct Cyc_Yyltype* _temp3148= _temp3146.curr
+ yylsp_offset; if( _temp3146.base == 0? 1:( _temp3148 < _temp3146.base? 1:
_temp3148 >= _temp3146.last_plus_one)){ _throw( Null_Exception);}* _temp3148;}).first_line,({
struct _tagged_ptr3 _temp3149= yyls; struct Cyc_Yyltype* _temp3151= _temp3149.curr
+ yylsp_offset; if( _temp3149.base == 0? 1:( _temp3151 < _temp3149.base? 1:
_temp3151 >= _temp3149.last_plus_one)){ _throw( Null_Exception);}* _temp3151;}).last_line));
_temp3145;}); _temp3144;}); break;} case 70: _LL3126: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeQual_tok_struct* _temp3153=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp3153[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3154; _temp3154.tag= Cyc_TypeQual_tok_tag; _temp3154.f1=({ struct Cyc_Absyn_Tqual*
_temp3155=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp3155->q_const= 1; _temp3155->q_volatile= 0; _temp3155->q_restrict= 0;
_temp3155;}); _temp3154;}); _temp3153;}); break; case 71: _LL3152: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeQual_tok_struct* _temp3157=( struct
Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp3157[ 0]=({ struct Cyc_TypeQual_tok_struct _temp3158; _temp3158.tag= Cyc_TypeQual_tok_tag;
_temp3158.f1=({ struct Cyc_Absyn_Tqual* _temp3159=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp3159->q_const= 0;
_temp3159->q_volatile= 1; _temp3159->q_restrict= 0; _temp3159;}); _temp3158;});
_temp3157;}); break; case 72: _LL3156: yyval=( struct _xtunion_struct*)({ struct
Cyc_TypeQual_tok_struct* _temp3161=( struct Cyc_TypeQual_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeQual_tok_struct)); _temp3161[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3162; _temp3162.tag= Cyc_TypeQual_tok_tag; _temp3162.f1=({ struct Cyc_Absyn_Tqual*
_temp3163=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp3163->q_const= 0; _temp3163->q_volatile= 0; _temp3163->q_restrict= 1;
_temp3163;}); _temp3162;}); _temp3161;}); break; case 73: _LL3160: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3165=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3165[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3166; _temp3166.tag=
Cyc_TypeSpecifier_tok_tag; _temp3166.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3167=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3167[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3168; _temp3168.tag= Cyc_Parse_Decl_spec_tag;
_temp3168.f1=({ struct Cyc_Absyn_Decl* _temp3169=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp3169->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp3176=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp3176[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp3177; _temp3177.tag= Cyc_Absyn_Enum_d_tag; _temp3177.f1=({ struct Cyc_Absyn_Enumdecl*
_temp3178=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp3178->sc=( void*)(( void*) Cyc_Absyn_Public); _temp3178->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3182= yyvs; struct _xtunion_struct** _temp3184=
_temp3182.curr +( yyvsp_offset - 3); if( _temp3182.base == 0? 1:( _temp3184 <
_temp3182.base? 1: _temp3184 >= _temp3182.last_plus_one)){ _throw(
Null_Exception);}* _temp3184;})); _temp3178->fields= Cyc_yyget_EnumfieldList_tok(({
struct _tagged_ptr2 _temp3179= yyvs; struct _xtunion_struct** _temp3181=
_temp3179.curr +( yyvsp_offset - 1); if( _temp3179.base == 0? 1:( _temp3181 <
_temp3179.base? 1: _temp3181 >= _temp3179.last_plus_one)){ _throw(
Null_Exception);}* _temp3181;})); _temp3178;}); _temp3177;}); _temp3176;}));
_temp3169->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3170=
yyls; struct Cyc_Yyltype* _temp3172= _temp3170.curr +( yylsp_offset - 4); if(
_temp3170.base == 0? 1:( _temp3172 < _temp3170.base? 1: _temp3172 >= _temp3170.last_plus_one)){
_throw( Null_Exception);}* _temp3172;}).first_line,({ struct _tagged_ptr3
_temp3173= yyls; struct Cyc_Yyltype* _temp3175= _temp3173.curr + yylsp_offset;
if( _temp3173.base == 0? 1:( _temp3175 < _temp3173.base? 1: _temp3175 >=
_temp3173.last_plus_one)){ _throw( Null_Exception);}* _temp3175;}).last_line);
_temp3169;}); _temp3168;}); _temp3167;})); _temp3166;}); _temp3165;}); break;
case 74: _LL3164: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3186=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3186[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3187; _temp3187.tag=
Cyc_TypeSpecifier_tok_tag; _temp3187.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp3188=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp3188[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp3189; _temp3189.tag= Cyc_Absyn_EnumType_tag;
_temp3189.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3190= yyvs;
struct _xtunion_struct** _temp3192= _temp3190.curr + yyvsp_offset; if( _temp3190.base
== 0? 1:( _temp3192 < _temp3190.base? 1: _temp3192 >= _temp3190.last_plus_one)){
_throw( Null_Exception);}* _temp3192;})); _temp3189.f2= 0; _temp3189;});
_temp3188;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3193= yyls;
struct Cyc_Yyltype* _temp3195= _temp3193.curr +( yylsp_offset - 1); if(
_temp3193.base == 0? 1:( _temp3195 < _temp3193.base? 1: _temp3195 >= _temp3193.last_plus_one)){
_throw( Null_Exception);}* _temp3195;}).first_line,({ struct _tagged_ptr3
_temp3196= yyls; struct Cyc_Yyltype* _temp3198= _temp3196.curr + yylsp_offset;
if( _temp3196.base == 0? 1:( _temp3198 < _temp3196.base? 1: _temp3198 >=
_temp3196.last_plus_one)){ _throw( Null_Exception);}* _temp3198;}).last_line));
_temp3187;}); _temp3186;}); break; case 75: _LL3185: yyval=( struct
_xtunion_struct*)({ struct Cyc_Enumfield_tok_struct* _temp3200=( struct Cyc_Enumfield_tok_struct*)
GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct)); _temp3200[ 0]=({ struct
Cyc_Enumfield_tok_struct _temp3201; _temp3201.tag= Cyc_Enumfield_tok_tag;
_temp3201.f1=({ struct Cyc_Absyn_Enumfield* _temp3202=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3202->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3209= yyvs; struct _xtunion_struct** _temp3211=
_temp3209.curr + yyvsp_offset; if( _temp3209.base == 0? 1:( _temp3211 <
_temp3209.base? 1: _temp3211 >= _temp3209.last_plus_one)){ _throw(
Null_Exception);}* _temp3211;})); _temp3202->tag= 0; _temp3202->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3203= yyls; struct Cyc_Yyltype* _temp3205= _temp3203.curr
+ yylsp_offset; if( _temp3203.base == 0? 1:( _temp3205 < _temp3203.base? 1:
_temp3205 >= _temp3203.last_plus_one)){ _throw( Null_Exception);}* _temp3205;}).first_line,({
struct _tagged_ptr3 _temp3206= yyls; struct Cyc_Yyltype* _temp3208= _temp3206.curr
+ yylsp_offset; if( _temp3206.base == 0? 1:( _temp3208 < _temp3206.base? 1:
_temp3208 >= _temp3206.last_plus_one)){ _throw( Null_Exception);}* _temp3208;}).last_line);
_temp3202;}); _temp3201;}); _temp3200;}); break; case 76: _LL3199: yyval=(
struct _xtunion_struct*)({ struct Cyc_Enumfield_tok_struct* _temp3213=( struct
Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp3213[ 0]=({ struct Cyc_Enumfield_tok_struct _temp3214; _temp3214.tag= Cyc_Enumfield_tok_tag;
_temp3214.f1=({ struct Cyc_Absyn_Enumfield* _temp3215=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3215->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3225= yyvs; struct _xtunion_struct** _temp3227=
_temp3225.curr +( yyvsp_offset - 2); if( _temp3225.base == 0? 1:( _temp3227 <
_temp3225.base? 1: _temp3227 >= _temp3225.last_plus_one)){ _throw(
Null_Exception);}* _temp3227;})); _temp3215->tag=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3222= yyvs; struct _xtunion_struct** _temp3224=
_temp3222.curr + yyvsp_offset; if( _temp3222.base == 0? 1:( _temp3224 <
_temp3222.base? 1: _temp3224 >= _temp3222.last_plus_one)){ _throw(
Null_Exception);}* _temp3224;})); _temp3215->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3216= yyls; struct Cyc_Yyltype* _temp3218= _temp3216.curr
+( yylsp_offset - 2); if( _temp3216.base == 0? 1:( _temp3218 < _temp3216.base? 1:
_temp3218 >= _temp3216.last_plus_one)){ _throw( Null_Exception);}* _temp3218;}).first_line,({
struct _tagged_ptr3 _temp3219= yyls; struct Cyc_Yyltype* _temp3221= _temp3219.curr
+ yylsp_offset; if( _temp3219.base == 0? 1:( _temp3221 < _temp3219.base? 1:
_temp3221 >= _temp3219.last_plus_one)){ _throw( Null_Exception);}* _temp3221;}).last_line);
_temp3215;}); _temp3214;}); _temp3213;}); break; case 77: _LL3212: yyval=(
struct _xtunion_struct*)({ struct Cyc_EnumfieldList_tok_struct* _temp3229=(
struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp3229[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp3230; _temp3230.tag=
Cyc_EnumfieldList_tok_tag; _temp3230.f1=({ struct Cyc_List_List* _temp3231=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3231->hd=(
void*) Cyc_yyget_Enumfield_tok(({ struct _tagged_ptr2 _temp3232= yyvs; struct
_xtunion_struct** _temp3234= _temp3232.curr + yyvsp_offset; if( _temp3232.base
== 0? 1:( _temp3234 < _temp3232.base? 1: _temp3234 >= _temp3232.last_plus_one)){
_throw( Null_Exception);}* _temp3234;})); _temp3231->tl= 0; _temp3231;});
_temp3230;}); _temp3229;}); break; case 78: _LL3228: yyval=( struct
_xtunion_struct*)({ struct Cyc_EnumfieldList_tok_struct* _temp3236=( struct Cyc_EnumfieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct)); _temp3236[ 0]=({
struct Cyc_EnumfieldList_tok_struct _temp3237; _temp3237.tag= Cyc_EnumfieldList_tok_tag;
_temp3237.f1=({ struct Cyc_List_List* _temp3238=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3238->hd=( void*) Cyc_yyget_Enumfield_tok(({
struct _tagged_ptr2 _temp3242= yyvs; struct _xtunion_struct** _temp3244=
_temp3242.curr +( yyvsp_offset - 2); if( _temp3242.base == 0? 1:( _temp3244 <
_temp3242.base? 1: _temp3244 >= _temp3242.last_plus_one)){ _throw(
Null_Exception);}* _temp3244;})); _temp3238->tl= Cyc_yyget_EnumfieldList_tok(({
struct _tagged_ptr2 _temp3239= yyvs; struct _xtunion_struct** _temp3241=
_temp3239.curr + yyvsp_offset; if( _temp3239.base == 0? 1:( _temp3241 <
_temp3239.base? 1: _temp3241 >= _temp3239.last_plus_one)){ _throw(
Null_Exception);}* _temp3241;})); _temp3238;}); _temp3237;}); _temp3236;});
break; case 79: _LL3235: { struct Cyc_Absyn_Decl* d;{ void* _temp3249= Cyc_yyget_StructOrUnion_tok(({
struct _tagged_ptr2 _temp3246= yyvs; struct _xtunion_struct** _temp3248=
_temp3246.curr +( yyvsp_offset - 3); if( _temp3246.base == 0? 1:( _temp3248 <
_temp3246.base? 1: _temp3248 >= _temp3246.last_plus_one)){ _throw(
Null_Exception);}* _temp3248;})); _LL3251: if(( int) _temp3249 == Cyc_Parse_Struct_su){
goto _LL3252;} else{ goto _LL3253;} _LL3253: if(( int) _temp3249 == Cyc_Parse_Union_su){
goto _LL3254;} else{ goto _LL3250;} _LL3252: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public, 0, 0,({ struct Cyc_Core_Opt* _temp3255=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3255->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3256= yyvs; struct _xtunion_struct** _temp3258=
_temp3256.curr +( yyvsp_offset - 1); if( _temp3256.base == 0? 1:( _temp3258 <
_temp3256.base? 1: _temp3258 >= _temp3256.last_plus_one)){ _throw(
Null_Exception);}* _temp3258;})); _temp3255;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3259= yyls; struct Cyc_Yyltype* _temp3261= _temp3259.curr
+( yylsp_offset - 3); if( _temp3259.base == 0? 1:( _temp3261 < _temp3259.base? 1:
_temp3261 >= _temp3259.last_plus_one)){ _throw( Null_Exception);}* _temp3261;}).first_line,({
struct _tagged_ptr3 _temp3262= yyls; struct Cyc_Yyltype* _temp3264= _temp3262.curr
+ yylsp_offset; if( _temp3262.base == 0? 1:( _temp3264 < _temp3262.base? 1:
_temp3264 >= _temp3262.last_plus_one)){ _throw( Null_Exception);}* _temp3264;}).last_line));
goto _LL3250; _LL3254: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public, 0, 0,({
struct Cyc_Core_Opt* _temp3265=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3265->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3266= yyvs; struct _xtunion_struct** _temp3268=
_temp3266.curr +( yyvsp_offset - 1); if( _temp3266.base == 0? 1:( _temp3268 <
_temp3266.base? 1: _temp3268 >= _temp3266.last_plus_one)){ _throw(
Null_Exception);}* _temp3268;})); _temp3265;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3269= yyls; struct Cyc_Yyltype* _temp3271= _temp3269.curr
+( yylsp_offset - 3); if( _temp3269.base == 0? 1:( _temp3271 < _temp3269.base? 1:
_temp3271 >= _temp3269.last_plus_one)){ _throw( Null_Exception);}* _temp3271;}).first_line,({
struct _tagged_ptr3 _temp3272= yyls; struct Cyc_Yyltype* _temp3274= _temp3272.curr
+ yylsp_offset; if( _temp3272.base == 0? 1:( _temp3274 < _temp3272.base? 1:
_temp3274 >= _temp3272.last_plus_one)){ _throw( Null_Exception);}* _temp3274;}).last_line));
goto _LL3250; _LL3250:;} yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3275=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3275[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3276; _temp3276.tag=
Cyc_TypeSpecifier_tok_tag; _temp3276.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3277=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3277[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3278; _temp3278.tag= Cyc_Parse_Decl_spec_tag;
_temp3278.f1= d; _temp3278;}); _temp3277;})); _temp3276;}); _temp3275;}); Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp3279=( char*)"anonymous structs/unions";
struct _tagged_string _temp3280; _temp3280.curr= _temp3279; _temp3280.base=
_temp3279; _temp3280.last_plus_one= _temp3279 + 25; _temp3280;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3281= yyls; struct Cyc_Yyltype* _temp3283= _temp3281.curr
+( yylsp_offset - 3); if( _temp3281.base == 0? 1:( _temp3283 < _temp3281.base? 1:
_temp3283 >= _temp3281.last_plus_one)){ _throw( Null_Exception);}* _temp3283;}).first_line,({
struct _tagged_ptr3 _temp3284= yyls; struct Cyc_Yyltype* _temp3286= _temp3284.curr
+ yylsp_offset; if( _temp3284.base == 0? 1:( _temp3286 < _temp3284.base? 1:
_temp3286 >= _temp3284.last_plus_one)){ _throw( Null_Exception);}* _temp3286;}).last_line));
break;} case 80: _LL3245: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3288= yyls; struct Cyc_Yyltype* _temp3290= _temp3288.curr
+( yylsp_offset - 3); if( _temp3288.base == 0? 1:( _temp3290 < _temp3288.base? 1:
_temp3290 >= _temp3288.last_plus_one)){ _throw( Null_Exception);}* _temp3290;}).first_line,({
struct _tagged_ptr3 _temp3291= yyls; struct Cyc_Yyltype* _temp3293= _temp3291.curr
+( yylsp_offset - 3); if( _temp3291.base == 0? 1:( _temp3293 < _temp3291.base? 1:
_temp3293 >= _temp3291.last_plus_one)){ _throw( Null_Exception);}* _temp3293;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3294= yyvs; struct
_xtunion_struct** _temp3296= _temp3294.curr +( yyvsp_offset - 3); if( _temp3294.base
== 0? 1:( _temp3296 < _temp3294.base? 1: _temp3296 >= _temp3294.last_plus_one)){
_throw( Null_Exception);}* _temp3296;}))); struct Cyc_Absyn_Decl* d;{ void*
_temp3300= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3297= yyvs;
struct _xtunion_struct** _temp3299= _temp3297.curr +( yyvsp_offset - 5); if(
_temp3297.base == 0? 1:( _temp3299 < _temp3297.base? 1: _temp3299 >= _temp3297.last_plus_one)){
_throw( Null_Exception);}* _temp3299;})); _LL3302: if(( int) _temp3300 == Cyc_Parse_Struct_su){
goto _LL3303;} else{ goto _LL3304;} _LL3304: if(( int) _temp3300 == Cyc_Parse_Union_su){
goto _LL3305;} else{ goto _LL3301;} _LL3303: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp3306=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3306->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3307= yyvs; struct _xtunion_struct** _temp3309=
_temp3307.curr +( yyvsp_offset - 4); if( _temp3307.base == 0? 1:( _temp3309 <
_temp3307.base? 1: _temp3309 >= _temp3307.last_plus_one)){ _throw(
Null_Exception);}* _temp3309;})); _temp3306;}), ts,({ struct Cyc_Core_Opt*
_temp3310=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3310->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp3311= yyvs; struct _xtunion_struct** _temp3313= _temp3311.curr +(
yyvsp_offset - 1); if( _temp3311.base == 0? 1:( _temp3313 < _temp3311.base? 1:
_temp3313 >= _temp3311.last_plus_one)){ _throw( Null_Exception);}* _temp3313;}));
_temp3310;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3314=
yyls; struct Cyc_Yyltype* _temp3316= _temp3314.curr +( yylsp_offset - 5); if(
_temp3314.base == 0? 1:( _temp3316 < _temp3314.base? 1: _temp3316 >= _temp3314.last_plus_one)){
_throw( Null_Exception);}* _temp3316;}).first_line,({ struct _tagged_ptr3
_temp3317= yyls; struct Cyc_Yyltype* _temp3319= _temp3317.curr + yylsp_offset;
if( _temp3317.base == 0? 1:( _temp3319 < _temp3317.base? 1: _temp3319 >=
_temp3317.last_plus_one)){ _throw( Null_Exception);}* _temp3319;}).last_line));
goto _LL3301; _LL3305: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp3320=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3320->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3321= yyvs; struct _xtunion_struct** _temp3323= _temp3321.curr
+( yyvsp_offset - 4); if( _temp3321.base == 0? 1:( _temp3323 < _temp3321.base? 1:
_temp3323 >= _temp3321.last_plus_one)){ _throw( Null_Exception);}* _temp3323;}));
_temp3320;}), ts,({ struct Cyc_Core_Opt* _temp3324=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3324->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3325= yyvs; struct _xtunion_struct** _temp3327=
_temp3325.curr +( yyvsp_offset - 1); if( _temp3325.base == 0? 1:( _temp3327 <
_temp3325.base? 1: _temp3327 >= _temp3325.last_plus_one)){ _throw(
Null_Exception);}* _temp3327;})); _temp3324;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3328= yyls; struct Cyc_Yyltype* _temp3330= _temp3328.curr
+( yylsp_offset - 5); if( _temp3328.base == 0? 1:( _temp3330 < _temp3328.base? 1:
_temp3330 >= _temp3328.last_plus_one)){ _throw( Null_Exception);}* _temp3330;}).first_line,({
struct _tagged_ptr3 _temp3331= yyls; struct Cyc_Yyltype* _temp3333= _temp3331.curr
+ yylsp_offset; if( _temp3331.base == 0? 1:( _temp3333 < _temp3331.base? 1:
_temp3333 >= _temp3331.last_plus_one)){ _throw( Null_Exception);}* _temp3333;}).last_line));
goto _LL3301; _LL3301:;} yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3334=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3334[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3335; _temp3335.tag=
Cyc_TypeSpecifier_tok_tag; _temp3335.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3336=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3336[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3337; _temp3337.tag= Cyc_Parse_Decl_spec_tag;
_temp3337.f1= d; _temp3337;}); _temp3336;})); _temp3335;}); _temp3334;}); break;}
case 81: _LL3287: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3339= yyls; struct Cyc_Yyltype* _temp3341= _temp3339.curr
+( yylsp_offset - 3); if( _temp3339.base == 0? 1:( _temp3341 < _temp3339.base? 1:
_temp3341 >= _temp3339.last_plus_one)){ _throw( Null_Exception);}* _temp3341;}).first_line,({
struct _tagged_ptr3 _temp3342= yyls; struct Cyc_Yyltype* _temp3344= _temp3342.curr
+( yylsp_offset - 3); if( _temp3342.base == 0? 1:( _temp3344 < _temp3342.base? 1:
_temp3344 >= _temp3342.last_plus_one)){ _throw( Null_Exception);}* _temp3344;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3345= yyvs; struct
_xtunion_struct** _temp3347= _temp3345.curr +( yyvsp_offset - 3); if( _temp3345.base
== 0? 1:( _temp3347 < _temp3345.base? 1: _temp3347 >= _temp3345.last_plus_one)){
_throw( Null_Exception);}* _temp3347;}))); struct Cyc_Absyn_Decl* d;{ void*
_temp3351= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3348= yyvs;
struct _xtunion_struct** _temp3350= _temp3348.curr +( yyvsp_offset - 5); if(
_temp3348.base == 0? 1:( _temp3350 < _temp3348.base? 1: _temp3350 >= _temp3348.last_plus_one)){
_throw( Null_Exception);}* _temp3350;})); _LL3353: if(( int) _temp3351 == Cyc_Parse_Struct_su){
goto _LL3354;} else{ goto _LL3355;} _LL3355: if(( int) _temp3351 == Cyc_Parse_Union_su){
goto _LL3356;} else{ goto _LL3352;} _LL3354: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp3357=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3357->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3358= yyvs; struct _xtunion_struct** _temp3360=
_temp3358.curr +( yyvsp_offset - 4); if( _temp3358.base == 0? 1:( _temp3360 <
_temp3358.base? 1: _temp3360 >= _temp3358.last_plus_one)){ _throw(
Null_Exception);}* _temp3360;})); _temp3357;}), ts,({ struct Cyc_Core_Opt*
_temp3361=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3361->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp3362= yyvs; struct _xtunion_struct** _temp3364= _temp3362.curr +(
yyvsp_offset - 1); if( _temp3362.base == 0? 1:( _temp3364 < _temp3362.base? 1:
_temp3364 >= _temp3362.last_plus_one)){ _throw( Null_Exception);}* _temp3364;}));
_temp3361;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3365=
yyls; struct Cyc_Yyltype* _temp3367= _temp3365.curr +( yylsp_offset - 5); if(
_temp3365.base == 0? 1:( _temp3367 < _temp3365.base? 1: _temp3367 >= _temp3365.last_plus_one)){
_throw( Null_Exception);}* _temp3367;}).first_line,({ struct _tagged_ptr3
_temp3368= yyls; struct Cyc_Yyltype* _temp3370= _temp3368.curr + yylsp_offset;
if( _temp3368.base == 0? 1:( _temp3370 < _temp3368.base? 1: _temp3370 >=
_temp3368.last_plus_one)){ _throw( Null_Exception);}* _temp3370;}).last_line));
goto _LL3352; _LL3356: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp3371=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3371->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3372= yyvs; struct _xtunion_struct** _temp3374= _temp3372.curr
+( yyvsp_offset - 4); if( _temp3372.base == 0? 1:( _temp3374 < _temp3372.base? 1:
_temp3374 >= _temp3372.last_plus_one)){ _throw( Null_Exception);}* _temp3374;}));
_temp3371;}), ts,({ struct Cyc_Core_Opt* _temp3375=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3375->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3376= yyvs; struct _xtunion_struct** _temp3378=
_temp3376.curr +( yyvsp_offset - 1); if( _temp3376.base == 0? 1:( _temp3378 <
_temp3376.base? 1: _temp3378 >= _temp3376.last_plus_one)){ _throw(
Null_Exception);}* _temp3378;})); _temp3375;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3379= yyls; struct Cyc_Yyltype* _temp3381= _temp3379.curr
+( yylsp_offset - 5); if( _temp3379.base == 0? 1:( _temp3381 < _temp3379.base? 1:
_temp3381 >= _temp3379.last_plus_one)){ _throw( Null_Exception);}* _temp3381;}).first_line,({
struct _tagged_ptr3 _temp3382= yyls; struct Cyc_Yyltype* _temp3384= _temp3382.curr
+ yylsp_offset; if( _temp3382.base == 0? 1:( _temp3384 < _temp3382.base? 1:
_temp3384 >= _temp3382.last_plus_one)){ _throw( Null_Exception);}* _temp3384;}).last_line));
goto _LL3352; _LL3352:;} yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3385=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3385[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3386; _temp3386.tag=
Cyc_TypeSpecifier_tok_tag; _temp3386.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3387=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3387[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3388; _temp3388.tag= Cyc_Parse_Decl_spec_tag;
_temp3388.f1= d; _temp3388;}); _temp3387;})); _temp3386;}); _temp3385;}); break;}
case 82: _LL3338:{ void* _temp3393= Cyc_yyget_StructOrUnion_tok(({ struct
_tagged_ptr2 _temp3390= yyvs; struct _xtunion_struct** _temp3392= _temp3390.curr
+( yyvsp_offset - 2); if( _temp3390.base == 0? 1:( _temp3392 < _temp3390.base? 1:
_temp3392 >= _temp3390.last_plus_one)){ _throw( Null_Exception);}* _temp3392;}));
_LL3395: if(( int) _temp3393 == Cyc_Parse_Struct_su){ goto _LL3396;} else{ goto
_LL3397;} _LL3397: if(( int) _temp3393 == Cyc_Parse_Union_su){ goto _LL3398;}
else{ goto _LL3394;} _LL3396: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3399=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3399[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3400; _temp3400.tag=
Cyc_TypeSpecifier_tok_tag; _temp3400.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp3401=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3401[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp3402; _temp3402.tag= Cyc_Absyn_StructType_tag;
_temp3402.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3403= yyvs; struct _xtunion_struct** _temp3405= _temp3403.curr +(
yyvsp_offset - 1); if( _temp3403.base == 0? 1:( _temp3405 < _temp3403.base? 1:
_temp3405 >= _temp3403.last_plus_one)){ _throw( Null_Exception);}* _temp3405;}));
_temp3402.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3406= yyvs;
struct _xtunion_struct** _temp3408= _temp3406.curr + yyvsp_offset; if( _temp3406.base
== 0? 1:( _temp3408 < _temp3406.base? 1: _temp3408 >= _temp3406.last_plus_one)){
_throw( Null_Exception);}* _temp3408;})); _temp3402.f3= 0; _temp3402;});
_temp3401;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3409= yyls;
struct Cyc_Yyltype* _temp3411= _temp3409.curr +( yylsp_offset - 2); if(
_temp3409.base == 0? 1:( _temp3411 < _temp3409.base? 1: _temp3411 >= _temp3409.last_plus_one)){
_throw( Null_Exception);}* _temp3411;}).first_line,({ struct _tagged_ptr3
_temp3412= yyls; struct Cyc_Yyltype* _temp3414= _temp3412.curr + yylsp_offset;
if( _temp3412.base == 0? 1:( _temp3414 < _temp3412.base? 1: _temp3414 >=
_temp3412.last_plus_one)){ _throw( Null_Exception);}* _temp3414;}).last_line));
_temp3400;}); _temp3399;}); goto _LL3394; _LL3398: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3415=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3415[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3416; _temp3416.tag= Cyc_TypeSpecifier_tok_tag;
_temp3416.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp3417=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp3417[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp3418; _temp3418.tag= Cyc_Absyn_UnionType_tag;
_temp3418.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3419= yyvs; struct _xtunion_struct** _temp3421= _temp3419.curr +(
yyvsp_offset - 1); if( _temp3419.base == 0? 1:( _temp3421 < _temp3419.base? 1:
_temp3421 >= _temp3419.last_plus_one)){ _throw( Null_Exception);}* _temp3421;}));
_temp3418.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3422= yyvs;
struct _xtunion_struct** _temp3424= _temp3422.curr + yyvsp_offset; if( _temp3422.base
== 0? 1:( _temp3424 < _temp3422.base? 1: _temp3424 >= _temp3422.last_plus_one)){
_throw( Null_Exception);}* _temp3424;})); _temp3418.f3= 0; _temp3418;});
_temp3417;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3425= yyls;
struct Cyc_Yyltype* _temp3427= _temp3425.curr +( yylsp_offset - 2); if(
_temp3425.base == 0? 1:( _temp3427 < _temp3425.base? 1: _temp3427 >= _temp3425.last_plus_one)){
_throw( Null_Exception);}* _temp3427;}).first_line,({ struct _tagged_ptr3
_temp3428= yyls; struct Cyc_Yyltype* _temp3430= _temp3428.curr + yylsp_offset;
if( _temp3428.base == 0? 1:( _temp3430 < _temp3428.base? 1: _temp3430 >=
_temp3428.last_plus_one)){ _throw( Null_Exception);}* _temp3430;}).last_line));
_temp3416;}); _temp3415;}); goto _LL3394; _LL3394:;} break; case 83: _LL3389:{
void* _temp3435= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3432=
yyvs; struct _xtunion_struct** _temp3434= _temp3432.curr +( yyvsp_offset - 2);
if( _temp3432.base == 0? 1:( _temp3434 < _temp3432.base? 1: _temp3434 >=
_temp3432.last_plus_one)){ _throw( Null_Exception);}* _temp3434;})); _LL3437:
if(( int) _temp3435 == Cyc_Parse_Struct_su){ goto _LL3438;} else{ goto _LL3439;}
_LL3439: if(( int) _temp3435 == Cyc_Parse_Union_su){ goto _LL3440;} else{ goto
_LL3436;} _LL3438: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3441=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3441[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3442; _temp3442.tag=
Cyc_TypeSpecifier_tok_tag; _temp3442.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp3443=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3443[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp3444; _temp3444.tag= Cyc_Absyn_StructType_tag;
_temp3444.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3445= yyvs; struct _xtunion_struct** _temp3447= _temp3445.curr +(
yyvsp_offset - 1); if( _temp3445.base == 0? 1:( _temp3447 < _temp3445.base? 1:
_temp3447 >= _temp3445.last_plus_one)){ _throw( Null_Exception);}* _temp3447;}));
_temp3444.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3448= yyvs;
struct _xtunion_struct** _temp3450= _temp3448.curr + yyvsp_offset; if( _temp3448.base
== 0? 1:( _temp3450 < _temp3448.base? 1: _temp3450 >= _temp3448.last_plus_one)){
_throw( Null_Exception);}* _temp3450;})); _temp3444.f3= 0; _temp3444;});
_temp3443;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3451= yyls;
struct Cyc_Yyltype* _temp3453= _temp3451.curr +( yylsp_offset - 2); if(
_temp3451.base == 0? 1:( _temp3453 < _temp3451.base? 1: _temp3453 >= _temp3451.last_plus_one)){
_throw( Null_Exception);}* _temp3453;}).first_line,({ struct _tagged_ptr3
_temp3454= yyls; struct Cyc_Yyltype* _temp3456= _temp3454.curr + yylsp_offset;
if( _temp3454.base == 0? 1:( _temp3456 < _temp3454.base? 1: _temp3456 >=
_temp3454.last_plus_one)){ _throw( Null_Exception);}* _temp3456;}).last_line));
_temp3442;}); _temp3441;}); goto _LL3436; _LL3440: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3457=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3457[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3458; _temp3458.tag= Cyc_TypeSpecifier_tok_tag;
_temp3458.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp3459=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp3459[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp3460; _temp3460.tag= Cyc_Absyn_UnionType_tag;
_temp3460.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3461= yyvs; struct _xtunion_struct** _temp3463= _temp3461.curr +(
yyvsp_offset - 1); if( _temp3461.base == 0? 1:( _temp3463 < _temp3461.base? 1:
_temp3463 >= _temp3461.last_plus_one)){ _throw( Null_Exception);}* _temp3463;}));
_temp3460.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3464= yyvs;
struct _xtunion_struct** _temp3466= _temp3464.curr + yyvsp_offset; if( _temp3464.base
== 0? 1:( _temp3466 < _temp3464.base? 1: _temp3466 >= _temp3464.last_plus_one)){
_throw( Null_Exception);}* _temp3466;})); _temp3460.f3= 0; _temp3460;});
_temp3459;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3467= yyls;
struct Cyc_Yyltype* _temp3469= _temp3467.curr +( yylsp_offset - 2); if(
_temp3467.base == 0? 1:( _temp3469 < _temp3467.base? 1: _temp3469 >= _temp3467.last_plus_one)){
_throw( Null_Exception);}* _temp3469;}).first_line,({ struct _tagged_ptr3
_temp3470= yyls; struct Cyc_Yyltype* _temp3472= _temp3470.curr + yylsp_offset;
if( _temp3470.base == 0? 1:( _temp3472 < _temp3470.base? 1: _temp3472 >=
_temp3470.last_plus_one)){ _throw( Null_Exception);}* _temp3472;}).last_line));
_temp3458;}); _temp3457;}); goto _LL3436; _LL3436:;} break; case 84: _LL3431:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp3474=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3474[ 0]=({ struct Cyc_TypeList_tok_struct _temp3475; _temp3475.tag= Cyc_TypeList_tok_tag;
_temp3475.f1= 0; _temp3475;}); _temp3474;}); break; case 85: _LL3473: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp3477=( struct
Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3477[ 0]=({ struct Cyc_TypeList_tok_struct _temp3478; _temp3478.tag= Cyc_TypeList_tok_tag;
_temp3478.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3479= yyvs; struct
_xtunion_struct** _temp3481= _temp3479.curr +( yyvsp_offset - 1); if( _temp3479.base
== 0? 1:( _temp3481 < _temp3479.base? 1: _temp3481 >= _temp3479.last_plus_one)){
_throw( Null_Exception);}* _temp3481;}))); _temp3478;}); _temp3477;}); break;
case 86: _LL3476:(( struct Cyc_Lexing_lexbuf*)({ struct Cyc_Core_Opt* _temp3483=
Cyc_Parse_lbuf; if( _temp3483 == 0){ _throw( Null_Exception);} _temp3483->v;}))->lex_curr_pos
-= 1; yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct*
_temp3484=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3484[ 0]=({ struct Cyc_TypeList_tok_struct _temp3485; _temp3485.tag= Cyc_TypeList_tok_tag;
_temp3485.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3486= yyvs; struct
_xtunion_struct** _temp3488= _temp3486.curr +( yyvsp_offset - 1); if( _temp3486.base
== 0? 1:( _temp3488 < _temp3486.base? 1: _temp3488 >= _temp3486.last_plus_one)){
_throw( Null_Exception);}* _temp3488;}))); _temp3485;}); _temp3484;}); break;
case 87: _LL3482: yyval=( struct _xtunion_struct*)({ struct Cyc_StructOrUnion_tok_struct*
_temp3490=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp3490[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp3491; _temp3491.tag=
Cyc_StructOrUnion_tok_tag; _temp3491.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp3491;}); _temp3490;}); break; case 88: _LL3489: yyval=( struct
_xtunion_struct*)({ struct Cyc_StructOrUnion_tok_struct* _temp3493=( struct Cyc_StructOrUnion_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct)); _temp3493[ 0]=({
struct Cyc_StructOrUnion_tok_struct _temp3494; _temp3494.tag= Cyc_StructOrUnion_tok_tag;
_temp3494.f1=( void*)(( void*) Cyc_Parse_Union_su); _temp3494;}); _temp3493;});
break; case 89: _LL3492: yyval=( struct _xtunion_struct*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3496=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp3496[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp3497; _temp3497.tag= Cyc_StructFieldDeclList_tok_tag; _temp3497.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3498= yyvs; struct _xtunion_struct** _temp3500=
_temp3498.curr + yyvsp_offset; if( _temp3498.base == 0? 1:( _temp3500 <
_temp3498.base? 1: _temp3500 >= _temp3498.last_plus_one)){ _throw(
Null_Exception);}* _temp3500;})))); _temp3497;}); _temp3496;}); break; case 90:
_LL3495: yyval=( struct _xtunion_struct*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3502=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp3502[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp3503; _temp3503.tag= Cyc_StructFieldDeclListList_tok_tag; _temp3503.f1=({
struct Cyc_List_List* _temp3504=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3504->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3505= yyvs; struct _xtunion_struct** _temp3507=
_temp3505.curr + yyvsp_offset; if( _temp3505.base == 0? 1:( _temp3507 <
_temp3505.base? 1: _temp3507 >= _temp3505.last_plus_one)){ _throw(
Null_Exception);}* _temp3507;})); _temp3504->tl= 0; _temp3504;}); _temp3503;});
_temp3502;}); break; case 91: _LL3501: yyval=( struct _xtunion_struct*)({ struct
Cyc_StructFieldDeclListList_tok_struct* _temp3509=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp3509[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp3510; _temp3510.tag= Cyc_StructFieldDeclListList_tok_tag;
_temp3510.f1=({ struct Cyc_List_List* _temp3511=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3511->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3515= yyvs; struct _xtunion_struct** _temp3517=
_temp3515.curr + yyvsp_offset; if( _temp3515.base == 0? 1:( _temp3517 <
_temp3515.base? 1: _temp3517 >= _temp3515.last_plus_one)){ _throw(
Null_Exception);}* _temp3517;})); _temp3511->tl= Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3512= yyvs; struct _xtunion_struct** _temp3514=
_temp3512.curr +( yyvsp_offset - 1); if( _temp3512.base == 0? 1:( _temp3514 <
_temp3512.base? 1: _temp3514 >= _temp3512.last_plus_one)){ _throw(
Null_Exception);}* _temp3514;})); _temp3511;}); _temp3510;}); _temp3509;});
break; case 92: _LL3508: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDeclList_tok_struct*
_temp3519=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3519[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp3520; _temp3520.tag=
Cyc_InitDeclList_tok_tag; _temp3520.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(({ struct _tagged_ptr2
_temp3521= yyvs; struct _xtunion_struct** _temp3523= _temp3521.curr +
yyvsp_offset; if( _temp3521.base == 0? 1:( _temp3523 < _temp3521.base? 1:
_temp3523 >= _temp3521.last_plus_one)){ _throw( Null_Exception);}* _temp3523;})));
_temp3520;}); _temp3519;}); break; case 93: _LL3518: yyval=( struct
_xtunion_struct*)({ struct Cyc_InitDeclList_tok_struct* _temp3525=( struct Cyc_InitDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp3525[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp3526; _temp3526.tag= Cyc_InitDeclList_tok_tag;
_temp3526.f1=({ struct Cyc_List_List* _temp3527=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3527->hd=( void*) Cyc_yyget_InitDecl_tok(({
struct _tagged_ptr2 _temp3528= yyvs; struct _xtunion_struct** _temp3530=
_temp3528.curr + yyvsp_offset; if( _temp3528.base == 0? 1:( _temp3530 <
_temp3528.base? 1: _temp3530 >= _temp3528.last_plus_one)){ _throw(
Null_Exception);}* _temp3530;})); _temp3527->tl= 0; _temp3527;}); _temp3526;});
_temp3525;}); break; case 94: _LL3524: yyval=( struct _xtunion_struct*)({ struct
Cyc_InitDeclList_tok_struct* _temp3532=( struct Cyc_InitDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp3532[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp3533; _temp3533.tag= Cyc_InitDeclList_tok_tag;
_temp3533.f1=({ struct Cyc_List_List* _temp3534=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3534->hd=( void*) Cyc_yyget_InitDecl_tok(({
struct _tagged_ptr2 _temp3538= yyvs; struct _xtunion_struct** _temp3540=
_temp3538.curr + yyvsp_offset; if( _temp3538.base == 0? 1:( _temp3540 <
_temp3538.base? 1: _temp3540 >= _temp3538.last_plus_one)){ _throw(
Null_Exception);}* _temp3540;})); _temp3534->tl= Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp3535= yyvs; struct _xtunion_struct** _temp3537=
_temp3535.curr +( yyvsp_offset - 2); if( _temp3535.base == 0? 1:( _temp3537 <
_temp3535.base? 1: _temp3537 >= _temp3535.last_plus_one)){ _throw(
Null_Exception);}* _temp3537;})); _temp3534;}); _temp3533;}); _temp3532;});
break; case 95: _LL3531: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDecl_tok_struct*
_temp3542=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp3542[ 0]=({ struct Cyc_InitDecl_tok_struct _temp3543; _temp3543.tag= Cyc_InitDecl_tok_tag;
_temp3543.f1=({ struct _tuple13* _temp3544=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp3544->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3545= yyvs; struct _xtunion_struct** _temp3547= _temp3545.curr
+ yyvsp_offset; if( _temp3545.base == 0? 1:( _temp3547 < _temp3545.base? 1:
_temp3547 >= _temp3545.last_plus_one)){ _throw( Null_Exception);}* _temp3547;}));
_temp3544->f2= 0; _temp3544;}); _temp3543;}); _temp3542;}); break; case 96:
_LL3541: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDecl_tok_struct*
_temp3549=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp3549[ 0]=({ struct Cyc_InitDecl_tok_struct _temp3550; _temp3550.tag= Cyc_InitDecl_tok_tag;
_temp3550.f1=({ struct _tuple13* _temp3551=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp3551->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3555= yyvs; struct _xtunion_struct** _temp3557= _temp3555.curr
+( yyvsp_offset - 2); if( _temp3555.base == 0? 1:( _temp3557 < _temp3555.base? 1:
_temp3557 >= _temp3555.last_plus_one)){ _throw( Null_Exception);}* _temp3557;}));
_temp3551->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp3552= yyvs; struct _xtunion_struct** _temp3554= _temp3552.curr +
yyvsp_offset; if( _temp3552.base == 0? 1:( _temp3554 < _temp3552.base? 1:
_temp3554 >= _temp3552.last_plus_one)){ _throw( Null_Exception);}* _temp3554;}));
_temp3551;}); _temp3550;}); _temp3549;}); break; case 97: _LL3548: { struct Cyc_Absyn_Tqual*
tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3559= yyvs; struct
_xtunion_struct** _temp3561= _temp3559.curr +( yyvsp_offset - 2); if( _temp3559.base
== 0? 1:( _temp3561 < _temp3559.base? 1: _temp3561 >= _temp3559.last_plus_one)){
_throw( Null_Exception);}* _temp3561;}))).f1; struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3562= yyvs; struct _xtunion_struct** _temp3564=
_temp3562.curr +( yyvsp_offset - 2); if( _temp3562.base == 0? 1:( _temp3564 <
_temp3562.base? 1: _temp3564 >= _temp3562.last_plus_one)){ _throw(
Null_Exception);}* _temp3564;}))).f3; void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3565= yyvs; struct _xtunion_struct** _temp3567=
_temp3565.curr +( yyvsp_offset - 2); if( _temp3565.base == 0? 1:( _temp3567 <
_temp3565.base? 1: _temp3567 >= _temp3565.last_plus_one)){ _throw(
Null_Exception);}* _temp3567;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3568= yyls; struct Cyc_Yyltype* _temp3570= _temp3568.curr +(
yylsp_offset - 2); if( _temp3568.base == 0? 1:( _temp3570 < _temp3568.base? 1:
_temp3570 >= _temp3568.last_plus_one)){ _throw( Null_Exception);}* _temp3570;}).first_line,({
struct _tagged_ptr3 _temp3571= yyls; struct Cyc_Yyltype* _temp3573= _temp3571.curr
+( yylsp_offset - 2); if( _temp3571.base == 0? 1:( _temp3573 < _temp3571.base? 1:
_temp3573 >= _temp3571.last_plus_one)){ _throw( Null_Exception);}* _temp3573;}).last_line));
struct Cyc_List_List* _temp3579; struct Cyc_List_List* _temp3581; struct _tuple0
_temp3577=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3574= yyvs; struct _xtunion_struct** _temp3576=
_temp3574.curr +( yyvsp_offset - 1); if( _temp3574.base == 0? 1:( _temp3576 <
_temp3574.base? 1: _temp3576 >= _temp3574.last_plus_one)){ _throw(
Null_Exception);}* _temp3576;}))); _LL3582: _temp3581= _temp3577.f1; goto
_LL3580; _LL3580: _temp3579= _temp3577.f2; goto _LL3578; _LL3578: { struct Cyc_List_List*
info=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( Cyc_Parse_apply_tmss( tq, t, _temp3581, atts), _temp3579);
yyval=( struct _xtunion_struct*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3583=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp3583[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp3584; _temp3584.tag= Cyc_StructFieldDeclList_tok_tag; _temp3584.f1=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple8*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3585= yyls; struct Cyc_Yyltype* _temp3587= _temp3585.curr
+( yylsp_offset - 2); if( _temp3585.base == 0? 1:( _temp3587 < _temp3585.base? 1:
_temp3587 >= _temp3585.last_plus_one)){ _throw( Null_Exception);}* _temp3587;}).first_line,({
struct _tagged_ptr3 _temp3588= yyls; struct Cyc_Yyltype* _temp3590= _temp3588.curr
+( yylsp_offset - 1); if( _temp3588.base == 0? 1:( _temp3590 < _temp3588.base? 1:
_temp3590 >= _temp3588.last_plus_one)){ _throw( Null_Exception);}* _temp3590;}).last_line),
info); _temp3584;}); _temp3583;}); break;}} case 98: _LL3558: yyval=( struct
_xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct* _temp3592=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp3592[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp3593; _temp3593.tag= Cyc_QualSpecList_tok_tag;
_temp3593.f1=({ struct _tuple14* _temp3594=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3594->f1= Cyc_Absyn_empty_tqual(); _temp3594->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp3598=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3598->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3599= yyvs; struct _xtunion_struct** _temp3601=
_temp3599.curr +( yyvsp_offset - 1); if( _temp3599.base == 0? 1:( _temp3601 <
_temp3599.base? 1: _temp3601 >= _temp3599.last_plus_one)){ _throw(
Null_Exception);}* _temp3601;})); _temp3598->tl= 0; _temp3598;}); _temp3594->f3=
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp3595= yyvs; struct
_xtunion_struct** _temp3597= _temp3595.curr + yyvsp_offset; if( _temp3595.base
== 0? 1:( _temp3597 < _temp3595.base? 1: _temp3597 >= _temp3595.last_plus_one)){
_throw( Null_Exception);}* _temp3597;})); _temp3594;}); _temp3593;}); _temp3592;});
break; case 99: _LL3591: yyval=( struct _xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct*
_temp3603=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3603[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3604; _temp3604.tag=
Cyc_QualSpecList_tok_tag; _temp3604.f1=({ struct _tuple14* _temp3605=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3605->f1=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3619= yyvs; struct _xtunion_struct** _temp3621=
_temp3619.curr + yyvsp_offset; if( _temp3619.base == 0? 1:( _temp3621 <
_temp3619.base? 1: _temp3621 >= _temp3619.last_plus_one)){ _throw(
Null_Exception);}* _temp3621;}))).f1; _temp3605->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp3612=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3612->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3616= yyvs; struct _xtunion_struct** _temp3618=
_temp3616.curr +( yyvsp_offset - 2); if( _temp3616.base == 0? 1:( _temp3618 <
_temp3616.base? 1: _temp3618 >= _temp3616.last_plus_one)){ _throw(
Null_Exception);}* _temp3618;})); _temp3612->tl=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3613= yyvs; struct _xtunion_struct** _temp3615=
_temp3613.curr + yyvsp_offset; if( _temp3613.base == 0? 1:( _temp3615 <
_temp3613.base? 1: _temp3615 >= _temp3613.last_plus_one)){ _throw(
Null_Exception);}* _temp3615;}))).f2; _temp3612;}); _temp3605->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3606= yyvs; struct _xtunion_struct** _temp3608=
_temp3606.curr +( yyvsp_offset - 1); if( _temp3606.base == 0? 1:( _temp3608 <
_temp3606.base? 1: _temp3608 >= _temp3606.last_plus_one)){ _throw(
Null_Exception);}* _temp3608;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3609= yyvs; struct _xtunion_struct** _temp3611= _temp3609.curr
+ yyvsp_offset; if( _temp3609.base == 0? 1:( _temp3611 < _temp3609.base? 1:
_temp3611 >= _temp3609.last_plus_one)){ _throw( Null_Exception);}* _temp3611;}))).f3);
_temp3605;}); _temp3604;}); _temp3603;}); break; case 100: _LL3602: yyval=(
struct _xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct* _temp3623=(
struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3623[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3624; _temp3624.tag=
Cyc_QualSpecList_tok_tag; _temp3624.f1=({ struct _tuple14* _temp3625=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3625->f1= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3629= yyvs; struct _xtunion_struct** _temp3631=
_temp3629.curr +( yyvsp_offset - 1); if( _temp3629.base == 0? 1:( _temp3631 <
_temp3629.base? 1: _temp3631 >= _temp3629.last_plus_one)){ _throw(
Null_Exception);}* _temp3631;})); _temp3625->f2= 0; _temp3625->f3= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3626= yyvs; struct _xtunion_struct** _temp3628=
_temp3626.curr + yyvsp_offset; if( _temp3626.base == 0? 1:( _temp3628 <
_temp3626.base? 1: _temp3628 >= _temp3626.last_plus_one)){ _throw(
Null_Exception);}* _temp3628;})); _temp3625;}); _temp3624;}); _temp3623;});
break; case 101: _LL3622: yyval=( struct _xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct*
_temp3633=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3633[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3634; _temp3634.tag=
Cyc_QualSpecList_tok_tag; _temp3634.f1=({ struct _tuple14* _temp3635=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3635->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp3645= yyvs; struct
_xtunion_struct** _temp3647= _temp3645.curr +( yyvsp_offset - 2); if( _temp3645.base
== 0? 1:( _temp3647 < _temp3645.base? 1: _temp3647 >= _temp3645.last_plus_one)){
_throw( Null_Exception);}* _temp3647;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3648= yyvs; struct _xtunion_struct** _temp3650= _temp3648.curr
+ yyvsp_offset; if( _temp3648.base == 0? 1:( _temp3650 < _temp3648.base? 1:
_temp3650 >= _temp3648.last_plus_one)){ _throw( Null_Exception);}* _temp3650;}))).f1);
_temp3635->f2=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3642=
yyvs; struct _xtunion_struct** _temp3644= _temp3642.curr + yyvsp_offset; if(
_temp3642.base == 0? 1:( _temp3644 < _temp3642.base? 1: _temp3644 >= _temp3642.last_plus_one)){
_throw( Null_Exception);}* _temp3644;}))).f2; _temp3635->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3636= yyvs; struct _xtunion_struct** _temp3638=
_temp3636.curr +( yyvsp_offset - 1); if( _temp3636.base == 0? 1:( _temp3638 <
_temp3636.base? 1: _temp3638 >= _temp3636.last_plus_one)){ _throw(
Null_Exception);}* _temp3638;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3639= yyvs; struct _xtunion_struct** _temp3641= _temp3639.curr
+ yyvsp_offset; if( _temp3639.base == 0? 1:( _temp3641 < _temp3639.base? 1:
_temp3641 >= _temp3639.last_plus_one)){ _throw( Null_Exception);}* _temp3641;}))).f3);
_temp3635;}); _temp3634;}); _temp3633;}); break; case 102: _LL3632: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp3652=(
struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct));
_temp3652[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct _temp3653; _temp3653.tag=
Cyc_DeclaratorExpoptList_tok_tag; _temp3653.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3654= yyvs; struct _xtunion_struct** _temp3656=
_temp3654.curr + yyvsp_offset; if( _temp3654.base == 0? 1:( _temp3656 <
_temp3654.base? 1: _temp3656 >= _temp3654.last_plus_one)){ _throw(
Null_Exception);}* _temp3656;}))); _temp3653;}); _temp3652;}); break; case 103:
_LL3651: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclaratorExpoptList_tok_struct*
_temp3658=( struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof(
struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3658[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct
_temp3659; _temp3659.tag= Cyc_DeclaratorExpoptList_tok_tag; _temp3659.f1=({
struct Cyc_List_List* _temp3660=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3660->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(({
struct _tagged_ptr2 _temp3661= yyvs; struct _xtunion_struct** _temp3663=
_temp3661.curr + yyvsp_offset; if( _temp3661.base == 0? 1:( _temp3663 <
_temp3661.base? 1: _temp3663 >= _temp3661.last_plus_one)){ _throw(
Null_Exception);}* _temp3663;})); _temp3660->tl= 0; _temp3660;}); _temp3659;});
_temp3658;}); break; case 104: _LL3657: yyval=( struct _xtunion_struct*)({
struct Cyc_DeclaratorExpoptList_tok_struct* _temp3665=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3665[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp3666; _temp3666.tag= Cyc_DeclaratorExpoptList_tok_tag;
_temp3666.f1=({ struct Cyc_List_List* _temp3667=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3667->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(({
struct _tagged_ptr2 _temp3671= yyvs; struct _xtunion_struct** _temp3673=
_temp3671.curr + yyvsp_offset; if( _temp3671.base == 0? 1:( _temp3673 <
_temp3671.base? 1: _temp3673 >= _temp3671.last_plus_one)){ _throw(
Null_Exception);}* _temp3673;})); _temp3667->tl= Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3668= yyvs; struct _xtunion_struct** _temp3670=
_temp3668.curr +( yyvsp_offset - 2); if( _temp3668.base == 0? 1:( _temp3670 <
_temp3668.base? 1: _temp3670 >= _temp3668.last_plus_one)){ _throw(
Null_Exception);}* _temp3670;})); _temp3667;}); _temp3666;}); _temp3665;});
break; case 105: _LL3664: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclaratorExpopt_tok_struct*
_temp3675=( struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct
Cyc_DeclaratorExpopt_tok_struct)); _temp3675[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct
_temp3676; _temp3676.tag= Cyc_DeclaratorExpopt_tok_tag; _temp3676.f1=({ struct
_tuple15* _temp3677=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15));
_temp3677->f1= Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3678= yyvs;
struct _xtunion_struct** _temp3680= _temp3678.curr + yyvsp_offset; if( _temp3678.base
== 0? 1:( _temp3680 < _temp3678.base? 1: _temp3680 >= _temp3678.last_plus_one)){
_throw( Null_Exception);}* _temp3680;})); _temp3677->f2= 0; _temp3677;});
_temp3676;}); _temp3675;}); break; case 106: _LL3674: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp3682=( struct
Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp3682[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp3683; _temp3683.tag=
Cyc_DeclaratorExpopt_tok_tag; _temp3683.f1=({ struct _tuple15* _temp3684=(
struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp3684->f1=({ struct
Cyc_Parse_Declarator* _temp3689=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3689->id=({ struct _tuple1*
_temp3690=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3690->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp3694=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp3694[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp3695; _temp3695.tag= Cyc_Absyn_Rel_n_tag; _temp3695.f1= 0; _temp3695;});
_temp3694;}); _temp3690->f2=({ struct _tagged_string* _temp3691=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3691[ 0]=(
struct _tagged_string)({ char* _temp3692=( char*)""; struct _tagged_string
_temp3693; _temp3693.curr= _temp3692; _temp3693.base= _temp3692; _temp3693.last_plus_one=
_temp3692 + 1; _temp3693;}); _temp3691;}); _temp3690;}); _temp3689->tms= 0;
_temp3689;}); _temp3684->f2=({ struct Cyc_Core_Opt* _temp3685=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3685->v=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3686= yyvs; struct _xtunion_struct** _temp3688=
_temp3686.curr + yyvsp_offset; if( _temp3686.base == 0? 1:( _temp3688 <
_temp3686.base? 1: _temp3688 >= _temp3686.last_plus_one)){ _throw(
Null_Exception);}* _temp3688;})); _temp3685;}); _temp3684;}); _temp3683;});
_temp3682;}); break; case 107: _LL3681: yyval=( struct _xtunion_struct*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp3697=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp3697[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp3698; _temp3698.tag= Cyc_DeclaratorExpopt_tok_tag;
_temp3698.f1=({ struct _tuple15* _temp3699=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp3699->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3704= yyvs; struct _xtunion_struct** _temp3706= _temp3704.curr
+( yyvsp_offset - 2); if( _temp3704.base == 0? 1:( _temp3706 < _temp3704.base? 1:
_temp3706 >= _temp3704.last_plus_one)){ _throw( Null_Exception);}* _temp3706;}));
_temp3699->f2=({ struct Cyc_Core_Opt* _temp3700=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3700->v=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3701= yyvs; struct _xtunion_struct** _temp3703=
_temp3701.curr + yyvsp_offset; if( _temp3701.base == 0? 1:( _temp3703 <
_temp3701.base? 1: _temp3703 >= _temp3701.last_plus_one)){ _throw(
Null_Exception);}* _temp3703;})); _temp3700;}); _temp3699;}); _temp3698;});
_temp3697;}); break; case 108: _LL3696: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3708= yyls; struct Cyc_Yyltype* _temp3710= _temp3708.curr
+( yylsp_offset - 3); if( _temp3708.base == 0? 1:( _temp3710 < _temp3708.base? 1:
_temp3710 >= _temp3708.last_plus_one)){ _throw( Null_Exception);}* _temp3710;}).first_line,({
struct _tagged_ptr3 _temp3711= yyls; struct Cyc_Yyltype* _temp3713= _temp3711.curr
+( yylsp_offset - 3); if( _temp3711.base == 0? 1:( _temp3713 < _temp3711.base? 1:
_temp3713 >= _temp3711.last_plus_one)){ _throw( Null_Exception);}* _temp3713;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3714= yyvs; struct
_xtunion_struct** _temp3716= _temp3714.curr +( yyvsp_offset - 3); if( _temp3714.base
== 0? 1:( _temp3716 < _temp3714.base? 1: _temp3716 >= _temp3714.last_plus_one)){
_throw( Null_Exception);}* _temp3716;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_TypeSpecifier_tok_struct* _temp3717=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3717[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3718; _temp3718.tag= Cyc_TypeSpecifier_tok_tag;
_temp3718.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp3719=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3719[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3720; _temp3720.tag= Cyc_Parse_Decl_spec_tag;
_temp3720.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp3721=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3721->v=( void*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3722=
yyvs; struct _xtunion_struct** _temp3724= _temp3722.curr +( yyvsp_offset - 4);
if( _temp3722.base == 0? 1:( _temp3724 < _temp3722.base? 1: _temp3724 >=
_temp3722.last_plus_one)){ _throw( Null_Exception);}* _temp3724;})); _temp3721;}),
ts,({ struct Cyc_Core_Opt* _temp3725=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp3725->v=( void*) Cyc_yyget_TunionFieldList_tok(({
struct _tagged_ptr2 _temp3726= yyvs; struct _xtunion_struct** _temp3728=
_temp3726.curr +( yyvsp_offset - 1); if( _temp3726.base == 0? 1:( _temp3728 <
_temp3726.base? 1: _temp3728 >= _temp3726.last_plus_one)){ _throw(
Null_Exception);}* _temp3728;})); _temp3725;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3729= yyls; struct Cyc_Yyltype* _temp3731= _temp3729.curr
+( yylsp_offset - 5); if( _temp3729.base == 0? 1:( _temp3731 < _temp3729.base? 1:
_temp3731 >= _temp3729.last_plus_one)){ _throw( Null_Exception);}* _temp3731;}).first_line,({
struct _tagged_ptr3 _temp3732= yyls; struct Cyc_Yyltype* _temp3734= _temp3732.curr
+ yylsp_offset; if( _temp3732.base == 0? 1:( _temp3734 < _temp3732.base? 1:
_temp3734 >= _temp3732.last_plus_one)){ _throw( Null_Exception);}* _temp3734;}).last_line));
_temp3720;}); _temp3719;})); _temp3718;}); _temp3717;}); break;} case 109:
_LL3707: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3736=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3736[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3737; _temp3737.tag=
Cyc_TypeSpecifier_tok_tag; _temp3737.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp3738=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp3738[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp3739; _temp3739.tag= Cyc_Absyn_TunionType_tag;
_temp3739.f1=({ struct Cyc_Absyn_TunionInfo _temp3740; _temp3740.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3744= yyvs; struct _xtunion_struct** _temp3746=
_temp3744.curr +( yyvsp_offset - 1); if( _temp3744.base == 0? 1:( _temp3746 <
_temp3744.base? 1: _temp3746 >= _temp3744.last_plus_one)){ _throw(
Null_Exception);}* _temp3746;})); _temp3740.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3741= yyvs; struct _xtunion_struct** _temp3743=
_temp3741.curr + yyvsp_offset; if( _temp3741.base == 0? 1:( _temp3743 <
_temp3741.base? 1: _temp3743 >= _temp3741.last_plus_one)){ _throw(
Null_Exception);}* _temp3743;})); _temp3740.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp3740.tud= 0; _temp3740;}); _temp3739;}); _temp3738;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3747= yyls; struct Cyc_Yyltype* _temp3749= _temp3747.curr
+( yylsp_offset - 2); if( _temp3747.base == 0? 1:( _temp3749 < _temp3747.base? 1:
_temp3749 >= _temp3747.last_plus_one)){ _throw( Null_Exception);}* _temp3749;}).first_line,({
struct _tagged_ptr3 _temp3750= yyls; struct Cyc_Yyltype* _temp3752= _temp3750.curr
+ yylsp_offset; if( _temp3750.base == 0? 1:( _temp3752 < _temp3750.base? 1:
_temp3752 >= _temp3750.last_plus_one)){ _throw( Null_Exception);}* _temp3752;}).last_line));
_temp3737;}); _temp3736;}); break; case 110: _LL3735: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3754=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3754[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3755; _temp3755.tag= Cyc_TypeSpecifier_tok_tag;
_temp3755.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp3756=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp3756[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp3757; _temp3757.tag=
Cyc_Absyn_TunionType_tag; _temp3757.f1=({ struct Cyc_Absyn_TunionInfo _temp3758;
_temp3758.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3765= yyvs;
struct _xtunion_struct** _temp3767= _temp3765.curr +( yyvsp_offset - 1); if(
_temp3765.base == 0? 1:( _temp3767 < _temp3765.base? 1: _temp3767 >= _temp3765.last_plus_one)){
_throw( Null_Exception);}* _temp3767;})); _temp3758.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3762= yyvs; struct _xtunion_struct** _temp3764=
_temp3762.curr + yyvsp_offset; if( _temp3762.base == 0? 1:( _temp3764 <
_temp3762.base? 1: _temp3764 >= _temp3762.last_plus_one)){ _throw(
Null_Exception);}* _temp3764;})); _temp3758.rgn=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3759= yyvs; struct _xtunion_struct** _temp3761=
_temp3759.curr +( yyvsp_offset - 2); if( _temp3759.base == 0? 1:( _temp3761 <
_temp3759.base? 1: _temp3761 >= _temp3759.last_plus_one)){ _throw(
Null_Exception);}* _temp3761;})); _temp3758.tud= 0; _temp3758;}); _temp3757;});
_temp3756;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3768= yyls;
struct Cyc_Yyltype* _temp3770= _temp3768.curr +( yylsp_offset - 3); if(
_temp3768.base == 0? 1:( _temp3770 < _temp3768.base? 1: _temp3770 >= _temp3768.last_plus_one)){
_throw( Null_Exception);}* _temp3770;}).first_line,({ struct _tagged_ptr3
_temp3771= yyls; struct Cyc_Yyltype* _temp3773= _temp3771.curr + yylsp_offset;
if( _temp3771.base == 0? 1:( _temp3773 < _temp3771.base? 1: _temp3773 >=
_temp3771.last_plus_one)){ _throw( Null_Exception);}* _temp3773;}).last_line));
_temp3755;}); _temp3754;}); break; case 111: _LL3753: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3775=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3775[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3776; _temp3776.tag= Cyc_TypeSpecifier_tok_tag;
_temp3776.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp3777=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp3777[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp3778; _temp3778.tag= Cyc_Absyn_TunionFieldType_tag; _temp3778.f1=({ struct
Cyc_Absyn_TunionFieldInfo _temp3779; _temp3779.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3786= yyvs; struct _xtunion_struct** _temp3788=
_temp3786.curr +( yyvsp_offset - 3); if( _temp3786.base == 0? 1:( _temp3788 <
_temp3786.base? 1: _temp3788 >= _temp3786.last_plus_one)){ _throw(
Null_Exception);}* _temp3788;})); _temp3779.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3783= yyvs; struct _xtunion_struct** _temp3785=
_temp3783.curr + yyvsp_offset; if( _temp3783.base == 0? 1:( _temp3785 <
_temp3783.base? 1: _temp3785 >= _temp3783.last_plus_one)){ _throw(
Null_Exception);}* _temp3785;})); _temp3779.fname= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3780= yyvs; struct _xtunion_struct** _temp3782=
_temp3780.curr +( yyvsp_offset - 1); if( _temp3780.base == 0? 1:( _temp3782 <
_temp3780.base? 1: _temp3782 >= _temp3780.last_plus_one)){ _throw(
Null_Exception);}* _temp3782;})); _temp3779.tud= 0; _temp3779.tufd= 0; _temp3779;});
_temp3778;}); _temp3777;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3789= yyls; struct Cyc_Yyltype* _temp3791= _temp3789.curr +( yylsp_offset -
4); if( _temp3789.base == 0? 1:( _temp3791 < _temp3789.base? 1: _temp3791 >=
_temp3789.last_plus_one)){ _throw( Null_Exception);}* _temp3791;}).first_line,({
struct _tagged_ptr3 _temp3792= yyls; struct Cyc_Yyltype* _temp3794= _temp3792.curr
+ yylsp_offset; if( _temp3792.base == 0? 1:( _temp3794 < _temp3792.base? 1:
_temp3794 >= _temp3792.last_plus_one)){ _throw( Null_Exception);}* _temp3794;}).last_line));
_temp3776;}); _temp3775;}); break; case 112: _LL3774: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3796=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3796[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3797; _temp3797.tag= Cyc_TypeSpecifier_tok_tag;
_temp3797.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp3798=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3798[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3799; _temp3799.tag= Cyc_Parse_Decl_spec_tag;
_temp3799.f1= Cyc_Absyn_xtunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3800= yyvs; struct _xtunion_struct** _temp3802=
_temp3800.curr +( yyvsp_offset - 3); if( _temp3800.base == 0? 1:( _temp3802 <
_temp3800.base? 1: _temp3802 >= _temp3800.last_plus_one)){ _throw(
Null_Exception);}* _temp3802;})), Cyc_yyget_TunionFieldList_tok(({ struct
_tagged_ptr2 _temp3803= yyvs; struct _xtunion_struct** _temp3805= _temp3803.curr
+( yyvsp_offset - 1); if( _temp3803.base == 0? 1:( _temp3805 < _temp3803.base? 1:
_temp3805 >= _temp3803.last_plus_one)){ _throw( Null_Exception);}* _temp3805;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3806= yyls; struct Cyc_Yyltype*
_temp3808= _temp3806.curr +( yylsp_offset - 4); if( _temp3806.base == 0? 1:(
_temp3808 < _temp3806.base? 1: _temp3808 >= _temp3806.last_plus_one)){ _throw(
Null_Exception);}* _temp3808;}).first_line,({ struct _tagged_ptr3 _temp3809=
yyls; struct Cyc_Yyltype* _temp3811= _temp3809.curr + yylsp_offset; if(
_temp3809.base == 0? 1:( _temp3811 < _temp3809.base? 1: _temp3811 >= _temp3809.last_plus_one)){
_throw( Null_Exception);}* _temp3811;}).last_line)); _temp3799;}); _temp3798;}));
_temp3797;}); _temp3796;}); break; case 113: _LL3795: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3813=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3813[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3814; _temp3814.tag= Cyc_TypeSpecifier_tok_tag;
_temp3814.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_XTunionType_struct*
_temp3815=( struct Cyc_Absyn_XTunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct));
_temp3815[ 0]=({ struct Cyc_Absyn_XTunionType_struct _temp3816; _temp3816.tag=
Cyc_Absyn_XTunionType_tag; _temp3816.f1=({ struct Cyc_Absyn_XTunionInfo
_temp3817; _temp3817.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3818=
yyvs; struct _xtunion_struct** _temp3820= _temp3818.curr + yyvsp_offset; if(
_temp3818.base == 0? 1:( _temp3820 < _temp3818.base? 1: _temp3820 >= _temp3818.last_plus_one)){
_throw( Null_Exception);}* _temp3820;})); _temp3817.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp3817.xtud= 0; _temp3817;}); _temp3816;}); _temp3815;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3821= yyls; struct Cyc_Yyltype* _temp3823= _temp3821.curr
+( yylsp_offset - 1); if( _temp3821.base == 0? 1:( _temp3823 < _temp3821.base? 1:
_temp3823 >= _temp3821.last_plus_one)){ _throw( Null_Exception);}* _temp3823;}).first_line,({
struct _tagged_ptr3 _temp3824= yyls; struct Cyc_Yyltype* _temp3826= _temp3824.curr
+ yylsp_offset; if( _temp3824.base == 0? 1:( _temp3826 < _temp3824.base? 1:
_temp3826 >= _temp3824.last_plus_one)){ _throw( Null_Exception);}* _temp3826;}).last_line));
_temp3814;}); _temp3813;}); break; case 114: _LL3812: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3828=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3828[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3829; _temp3829.tag= Cyc_TypeSpecifier_tok_tag;
_temp3829.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_XTunionType_struct*
_temp3830=( struct Cyc_Absyn_XTunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct));
_temp3830[ 0]=({ struct Cyc_Absyn_XTunionType_struct _temp3831; _temp3831.tag=
Cyc_Absyn_XTunionType_tag; _temp3831.f1=({ struct Cyc_Absyn_XTunionInfo
_temp3832; _temp3832.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3836=
yyvs; struct _xtunion_struct** _temp3838= _temp3836.curr + yyvsp_offset; if(
_temp3836.base == 0? 1:( _temp3838 < _temp3836.base? 1: _temp3838 >= _temp3836.last_plus_one)){
_throw( Null_Exception);}* _temp3838;})); _temp3832.rgn=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3833= yyvs; struct _xtunion_struct** _temp3835=
_temp3833.curr +( yyvsp_offset - 1); if( _temp3833.base == 0? 1:( _temp3835 <
_temp3833.base? 1: _temp3835 >= _temp3833.last_plus_one)){ _throw(
Null_Exception);}* _temp3835;})); _temp3832.xtud= 0; _temp3832;}); _temp3831;});
_temp3830;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3839= yyls;
struct Cyc_Yyltype* _temp3841= _temp3839.curr +( yylsp_offset - 2); if(
_temp3839.base == 0? 1:( _temp3841 < _temp3839.base? 1: _temp3841 >= _temp3839.last_plus_one)){
_throw( Null_Exception);}* _temp3841;}).first_line,({ struct _tagged_ptr3
_temp3842= yyls; struct Cyc_Yyltype* _temp3844= _temp3842.curr + yylsp_offset;
if( _temp3842.base == 0? 1:( _temp3844 < _temp3842.base? 1: _temp3844 >=
_temp3842.last_plus_one)){ _throw( Null_Exception);}* _temp3844;}).last_line));
_temp3829;}); _temp3828;}); break; case 115: _LL3827: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3846=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3846[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3847; _temp3847.tag= Cyc_TypeSpecifier_tok_tag;
_temp3847.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_XTunionFieldType_struct*
_temp3848=( struct Cyc_Absyn_XTunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_XTunionFieldType_struct)); _temp3848[ 0]=({ struct Cyc_Absyn_XTunionFieldType_struct
_temp3849; _temp3849.tag= Cyc_Absyn_XTunionFieldType_tag; _temp3849.f1=({ struct
Cyc_Absyn_XTunionFieldInfo _temp3850; _temp3850.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3854= yyvs; struct _xtunion_struct** _temp3856=
_temp3854.curr +( yyvsp_offset - 2); if( _temp3854.base == 0? 1:( _temp3856 <
_temp3854.base? 1: _temp3856 >= _temp3854.last_plus_one)){ _throw(
Null_Exception);}* _temp3856;})); _temp3850.fname= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3851= yyvs; struct _xtunion_struct** _temp3853=
_temp3851.curr + yyvsp_offset; if( _temp3851.base == 0? 1:( _temp3853 <
_temp3851.base? 1: _temp3853 >= _temp3851.last_plus_one)){ _throw(
Null_Exception);}* _temp3853;})); _temp3850.xtud= 0; _temp3850.xtufd= 0;
_temp3850;}); _temp3849;}); _temp3848;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3857= yyls; struct Cyc_Yyltype* _temp3859= _temp3857.curr +(
yylsp_offset - 3); if( _temp3857.base == 0? 1:( _temp3859 < _temp3857.base? 1:
_temp3859 >= _temp3857.last_plus_one)){ _throw( Null_Exception);}* _temp3859;}).first_line,({
struct _tagged_ptr3 _temp3860= yyls; struct Cyc_Yyltype* _temp3862= _temp3860.curr
+ yylsp_offset; if( _temp3860.base == 0? 1:( _temp3862 < _temp3860.base? 1:
_temp3862 >= _temp3860.last_plus_one)){ _throw( Null_Exception);}* _temp3862;}).last_line));
_temp3847;}); _temp3846;}); break; case 116: _LL3845: yyval=( struct
_xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct* _temp3864=( struct
Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3864[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3865; _temp3865.tag=
Cyc_TunionFieldList_tok_tag; _temp3865.f1=({ struct Cyc_List_List* _temp3866=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3866->hd=(
void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2 _temp3867= yyvs; struct
_xtunion_struct** _temp3869= _temp3867.curr + yyvsp_offset; if( _temp3867.base
== 0? 1:( _temp3869 < _temp3867.base? 1: _temp3869 >= _temp3867.last_plus_one)){
_throw( Null_Exception);}* _temp3869;})); _temp3866->tl= 0; _temp3866;});
_temp3865;}); _temp3864;}); break; case 117: _LL3863: yyval=( struct
_xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct* _temp3871=( struct
Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3871[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3872; _temp3872.tag=
Cyc_TunionFieldList_tok_tag; _temp3872.f1=({ struct Cyc_List_List* _temp3873=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3873->hd=(
void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2 _temp3874= yyvs; struct
_xtunion_struct** _temp3876= _temp3874.curr +( yyvsp_offset - 1); if( _temp3874.base
== 0? 1:( _temp3876 < _temp3874.base? 1: _temp3876 >= _temp3874.last_plus_one)){
_throw( Null_Exception);}* _temp3876;})); _temp3873->tl= 0; _temp3873;});
_temp3872;}); _temp3871;}); break; case 118: _LL3870: yyval=( struct
_xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct* _temp3878=( struct
Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3878[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3879; _temp3879.tag=
Cyc_TunionFieldList_tok_tag; _temp3879.f1=({ struct Cyc_List_List* _temp3880=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3880->hd=(
void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2 _temp3884= yyvs; struct
_xtunion_struct** _temp3886= _temp3884.curr +( yyvsp_offset - 2); if( _temp3884.base
== 0? 1:( _temp3886 < _temp3884.base? 1: _temp3886 >= _temp3884.last_plus_one)){
_throw( Null_Exception);}* _temp3886;})); _temp3880->tl= Cyc_yyget_TunionFieldList_tok(({
struct _tagged_ptr2 _temp3881= yyvs; struct _xtunion_struct** _temp3883=
_temp3881.curr + yyvsp_offset; if( _temp3881.base == 0? 1:( _temp3883 <
_temp3881.base? 1: _temp3883 >= _temp3881.last_plus_one)){ _throw(
Null_Exception);}* _temp3883;})); _temp3880;}); _temp3879;}); _temp3878;});
break; case 119: _LL3877: yyval=( struct _xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct*
_temp3888=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp3888[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp3889; _temp3889.tag= Cyc_TunionFieldList_tok_tag; _temp3889.f1=({ struct
Cyc_List_List* _temp3890=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3890->hd=( void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2
_temp3894= yyvs; struct _xtunion_struct** _temp3896= _temp3894.curr +(
yyvsp_offset - 2); if( _temp3894.base == 0? 1:( _temp3896 < _temp3894.base? 1:
_temp3896 >= _temp3894.last_plus_one)){ _throw( Null_Exception);}* _temp3896;}));
_temp3890->tl= Cyc_yyget_TunionFieldList_tok(({ struct _tagged_ptr2 _temp3891=
yyvs; struct _xtunion_struct** _temp3893= _temp3891.curr + yyvsp_offset; if(
_temp3891.base == 0? 1:( _temp3893 < _temp3891.base? 1: _temp3893 >= _temp3891.last_plus_one)){
_throw( Null_Exception);}* _temp3893;})); _temp3890;}); _temp3889;}); _temp3888;});
break; case 120: _LL3887: yyval=( struct _xtunion_struct*)({ struct Cyc_TunionField_tok_struct*
_temp3898=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp3898[ 0]=({ struct Cyc_TunionField_tok_struct _temp3899; _temp3899.tag= Cyc_TunionField_tok_tag;
_temp3899.f1=({ struct Cyc_Absyn_Tunionfield* _temp3900=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3900->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3907= yyvs; struct _xtunion_struct** _temp3909=
_temp3907.curr + yyvsp_offset; if( _temp3907.base == 0? 1:( _temp3909 <
_temp3907.base? 1: _temp3909 >= _temp3907.last_plus_one)){ _throw(
Null_Exception);}* _temp3909;})); _temp3900->tvs= 0; _temp3900->typs= 0;
_temp3900->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3901=
yyls; struct Cyc_Yyltype* _temp3903= _temp3901.curr + yylsp_offset; if(
_temp3901.base == 0? 1:( _temp3903 < _temp3901.base? 1: _temp3903 >= _temp3901.last_plus_one)){
_throw( Null_Exception);}* _temp3903;}).first_line,({ struct _tagged_ptr3
_temp3904= yyls; struct Cyc_Yyltype* _temp3906= _temp3904.curr + yylsp_offset;
if( _temp3904.base == 0? 1:( _temp3906 < _temp3904.base? 1: _temp3906 >=
_temp3904.last_plus_one)){ _throw( Null_Exception);}* _temp3906;}).last_line);
_temp3900;}); _temp3899;}); _temp3898;}); break; case 121: _LL3897: { struct Cyc_List_List*
typs=(( struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3911= yyls; struct Cyc_Yyltype* _temp3913= _temp3911.curr +(
yylsp_offset - 1); if( _temp3911.base == 0? 1:( _temp3913 < _temp3911.base? 1:
_temp3913 >= _temp3911.last_plus_one)){ _throw( Null_Exception);}* _temp3913;}).first_line,({
struct _tagged_ptr3 _temp3914= yyls; struct Cyc_Yyltype* _temp3916= _temp3914.curr
+( yylsp_offset - 1); if( _temp3914.base == 0? 1:( _temp3916 < _temp3914.base? 1:
_temp3916 >= _temp3914.last_plus_one)){ _throw( Null_Exception);}* _temp3916;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3917= yyvs; struct _xtunion_struct** _temp3919=
_temp3917.curr +( yyvsp_offset - 1); if( _temp3917.base == 0? 1:( _temp3919 <
_temp3917.base? 1: _temp3919 >= _temp3917.last_plus_one)){ _throw(
Null_Exception);}* _temp3919;})))); struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3920= yyls; struct Cyc_Yyltype* _temp3922= _temp3920.curr
+( yylsp_offset - 3); if( _temp3920.base == 0? 1:( _temp3922 < _temp3920.base? 1:
_temp3922 >= _temp3920.last_plus_one)){ _throw( Null_Exception);}* _temp3922;}).first_line,({
struct _tagged_ptr3 _temp3923= yyls; struct Cyc_Yyltype* _temp3925= _temp3923.curr
+( yylsp_offset - 3); if( _temp3923.base == 0? 1:( _temp3925 < _temp3923.base? 1:
_temp3925 >= _temp3923.last_plus_one)){ _throw( Null_Exception);}* _temp3925;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3926= yyvs; struct
_xtunion_struct** _temp3928= _temp3926.curr +( yyvsp_offset - 3); if( _temp3926.base
== 0? 1:( _temp3928 < _temp3926.base? 1: _temp3928 >= _temp3926.last_plus_one)){
_throw( Null_Exception);}* _temp3928;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_TunionField_tok_struct* _temp3929=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp3929[ 0]=({ struct
Cyc_TunionField_tok_struct _temp3930; _temp3930.tag= Cyc_TunionField_tok_tag;
_temp3930.f1=({ struct Cyc_Absyn_Tunionfield* _temp3931=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3931->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3938= yyvs; struct _xtunion_struct** _temp3940=
_temp3938.curr +( yyvsp_offset - 4); if( _temp3938.base == 0? 1:( _temp3940 <
_temp3938.base? 1: _temp3940 >= _temp3938.last_plus_one)){ _throw(
Null_Exception);}* _temp3940;})); _temp3931->tvs= tvs; _temp3931->typs= typs;
_temp3931->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3932=
yyls; struct Cyc_Yyltype* _temp3934= _temp3932.curr +( yylsp_offset - 4); if(
_temp3932.base == 0? 1:( _temp3934 < _temp3932.base? 1: _temp3934 >= _temp3932.last_plus_one)){
_throw( Null_Exception);}* _temp3934;}).first_line,({ struct _tagged_ptr3
_temp3935= yyls; struct Cyc_Yyltype* _temp3937= _temp3935.curr + yylsp_offset;
if( _temp3935.base == 0? 1:( _temp3937 < _temp3935.base? 1: _temp3937 >=
_temp3935.last_plus_one)){ _throw( Null_Exception);}* _temp3937;}).last_line);
_temp3931;}); _temp3930;}); _temp3929;}); break;} case 122: _LL3910: yyval=({
struct _tagged_ptr2 _temp3942= yyvs; struct _xtunion_struct** _temp3944=
_temp3942.curr + yyvsp_offset; if( _temp3942.base == 0? 1:( _temp3944 <
_temp3942.base? 1: _temp3944 >= _temp3942.last_plus_one)){ _throw(
Null_Exception);}* _temp3944;}); break; case 123: _LL3941: yyval=( struct
_xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp3946=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3946[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3947; _temp3947.tag= Cyc_Declarator_tok_tag;
_temp3947.f1=({ struct Cyc_Parse_Declarator* _temp3948=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3948->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3955= yyvs; struct _xtunion_struct** _temp3957=
_temp3955.curr + yyvsp_offset; if( _temp3955.base == 0? 1:( _temp3957 <
_temp3955.base? 1: _temp3957 >= _temp3955.last_plus_one)){ _throw(
Null_Exception);}* _temp3957;})))->id; _temp3948->tms=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp3949= yyvs; struct _xtunion_struct** _temp3951=
_temp3949.curr +( yyvsp_offset - 1); if( _temp3949.base == 0? 1:( _temp3951 <
_temp3949.base? 1: _temp3951 >= _temp3949.last_plus_one)){ _throw(
Null_Exception);}* _temp3951;})),( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3952= yyvs; struct _xtunion_struct** _temp3954= _temp3952.curr
+ yyvsp_offset; if( _temp3952.base == 0? 1:( _temp3954 < _temp3952.base? 1:
_temp3954 >= _temp3952.last_plus_one)){ _throw( Null_Exception);}* _temp3954;})))->tms);
_temp3948;}); _temp3947;}); _temp3946;}); break; case 124: _LL3945: yyval=(
struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp3959=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3959[ 0]=({ struct Cyc_Declarator_tok_struct _temp3960; _temp3960.tag= Cyc_Declarator_tok_tag;
_temp3960.f1=({ struct Cyc_Parse_Declarator* _temp3961=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3961->id= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3962= yyvs; struct _xtunion_struct** _temp3964=
_temp3962.curr + yyvsp_offset; if( _temp3962.base == 0? 1:( _temp3964 <
_temp3962.base? 1: _temp3964 >= _temp3962.last_plus_one)){ _throw(
Null_Exception);}* _temp3964;})); _temp3961->tms= 0; _temp3961;}); _temp3960;});
_temp3959;}); break; case 125: _LL3958: yyval=({ struct _tagged_ptr2 _temp3966=
yyvs; struct _xtunion_struct** _temp3968= _temp3966.curr +( yyvsp_offset - 1);
if( _temp3966.base == 0? 1:( _temp3968 < _temp3966.base? 1: _temp3968 >=
_temp3966.last_plus_one)){ _throw( Null_Exception);}* _temp3968;}); break; case
126: _LL3965: yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct*
_temp3970=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3970[ 0]=({ struct Cyc_Declarator_tok_struct _temp3971; _temp3971.tag= Cyc_Declarator_tok_tag;
_temp3971.f1=({ struct Cyc_Parse_Declarator* _temp3972=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3972->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3977= yyvs; struct _xtunion_struct** _temp3979=
_temp3977.curr +( yyvsp_offset - 2); if( _temp3977.base == 0? 1:( _temp3979 <
_temp3977.base? 1: _temp3979 >= _temp3977.last_plus_one)){ _throw(
Null_Exception);}* _temp3979;})))->id; _temp3972->tms=({ struct Cyc_List_List*
_temp3973=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3973->hd=( void*)(( void*) Cyc_Absyn_Carray_mod); _temp3973->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3974= yyvs; struct _xtunion_struct** _temp3976=
_temp3974.curr +( yyvsp_offset - 2); if( _temp3974.base == 0? 1:( _temp3976 <
_temp3974.base? 1: _temp3976 >= _temp3974.last_plus_one)){ _throw(
Null_Exception);}* _temp3976;})))->tms; _temp3973;}); _temp3972;}); _temp3971;});
_temp3970;}); break; case 127: _LL3969: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp3981=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3981[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3982; _temp3982.tag= Cyc_Declarator_tok_tag;
_temp3982.f1=({ struct Cyc_Parse_Declarator* _temp3983=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3983->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3993= yyvs; struct _xtunion_struct** _temp3995=
_temp3993.curr +( yyvsp_offset - 3); if( _temp3993.base == 0? 1:( _temp3995 <
_temp3993.base? 1: _temp3995 >= _temp3993.last_plus_one)){ _throw(
Null_Exception);}* _temp3995;})))->id; _temp3983->tms=({ struct Cyc_List_List*
_temp3984=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3984->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp3988=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp3988[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp3989; _temp3989.tag= Cyc_Absyn_ConstArray_mod_tag; _temp3989.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3990= yyvs; struct _xtunion_struct** _temp3992=
_temp3990.curr +( yyvsp_offset - 1); if( _temp3990.base == 0? 1:( _temp3992 <
_temp3990.base? 1: _temp3992 >= _temp3990.last_plus_one)){ _throw(
Null_Exception);}* _temp3992;})); _temp3989;}); _temp3988;})); _temp3984->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3985= yyvs; struct
_xtunion_struct** _temp3987= _temp3985.curr +( yyvsp_offset - 3); if( _temp3985.base
== 0? 1:( _temp3987 < _temp3985.base? 1: _temp3987 >= _temp3985.last_plus_one)){
_throw( Null_Exception);}* _temp3987;})))->tms; _temp3984;}); _temp3983;});
_temp3982;}); _temp3981;}); break; case 128: _LL3980: { struct _tuple16
_temp4002; struct Cyc_Core_Opt* _temp4003; int _temp4005; struct Cyc_List_List*
_temp4007; struct _tuple16* _temp4000= Cyc_yyget_ParamDeclListBool_tok(({ struct
_tagged_ptr2 _temp3997= yyvs; struct _xtunion_struct** _temp3999= _temp3997.curr
+( yyvsp_offset - 1); if( _temp3997.base == 0? 1:( _temp3999 < _temp3997.base? 1:
_temp3999 >= _temp3997.last_plus_one)){ _throw( Null_Exception);}* _temp3999;}));
_temp4002=* _temp4000; _LL4008: _temp4007= _temp4002.f1; goto _LL4006; _LL4006:
_temp4005= _temp4002.f2; goto _LL4004; _LL4004: _temp4003= _temp4002.f3; goto
_LL4001; _LL4001: yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct*
_temp4009=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp4009[ 0]=({ struct Cyc_Declarator_tok_struct _temp4010; _temp4010.tag= Cyc_Declarator_tok_tag;
_temp4010.f1=({ struct Cyc_Parse_Declarator* _temp4011=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4011->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4020= yyvs; struct _xtunion_struct** _temp4022=
_temp4020.curr +( yyvsp_offset - 3); if( _temp4020.base == 0? 1:( _temp4022 <
_temp4020.base? 1: _temp4022 >= _temp4020.last_plus_one)){ _throw(
Null_Exception);}* _temp4022;})))->id; _temp4011->tms=({ struct Cyc_List_List*
_temp4012=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4012->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4016=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4016[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4017; _temp4017.tag=
Cyc_Absyn_Function_mod_tag; _temp4017.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4018=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4018[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4019; _temp4019.tag= Cyc_Absyn_WithTypes_tag;
_temp4019.f1= _temp4007; _temp4019.f2= _temp4005; _temp4019.f3= _temp4003;
_temp4019;}); _temp4018;})); _temp4017;}); _temp4016;})); _temp4012->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4013= yyvs; struct _xtunion_struct** _temp4015=
_temp4013.curr +( yyvsp_offset - 3); if( _temp4013.base == 0? 1:( _temp4015 <
_temp4013.base? 1: _temp4015 >= _temp4013.last_plus_one)){ _throw(
Null_Exception);}* _temp4015;})))->tms; _temp4012;}); _temp4011;}); _temp4010;});
_temp4009;}); break;} case 129: _LL3996: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp4024=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4024[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4025; _temp4025.tag= Cyc_Declarator_tok_tag;
_temp4025.f1=({ struct Cyc_Parse_Declarator* _temp4026=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4026->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4035= yyvs; struct _xtunion_struct** _temp4037=
_temp4035.curr +( yyvsp_offset - 2); if( _temp4035.base == 0? 1:( _temp4037 <
_temp4035.base? 1: _temp4037 >= _temp4035.last_plus_one)){ _throw(
Null_Exception);}* _temp4037;})))->id; _temp4026->tms=({ struct Cyc_List_List*
_temp4027=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4027->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4031=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4031[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4032; _temp4032.tag=
Cyc_Absyn_Function_mod_tag; _temp4032.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4033=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4033[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4034; _temp4034.tag= Cyc_Absyn_WithTypes_tag;
_temp4034.f1= 0; _temp4034.f2= 0; _temp4034.f3= 0; _temp4034;}); _temp4033;}));
_temp4032;}); _temp4031;})); _temp4027->tl=( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp4028= yyvs; struct _xtunion_struct** _temp4030= _temp4028.curr
+( yyvsp_offset - 2); if( _temp4028.base == 0? 1:( _temp4030 < _temp4028.base? 1:
_temp4030 >= _temp4028.last_plus_one)){ _throw( Null_Exception);}* _temp4030;})))->tms;
_temp4027;}); _temp4026;}); _temp4025;}); _temp4024;}); break; case 130: _LL4023:
yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp4039=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp4039[ 0]=({ struct Cyc_Declarator_tok_struct _temp4040; _temp4040.tag= Cyc_Declarator_tok_tag;
_temp4040.f1=({ struct Cyc_Parse_Declarator* _temp4041=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4041->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4056= yyvs; struct _xtunion_struct** _temp4058=
_temp4056.curr +( yyvsp_offset - 4); if( _temp4056.base == 0? 1:( _temp4058 <
_temp4056.base? 1: _temp4058 >= _temp4056.last_plus_one)){ _throw(
Null_Exception);}* _temp4058;})))->id; _temp4041->tms=({ struct Cyc_List_List*
_temp4042=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4042->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4046=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4046[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4047; _temp4047.tag=
Cyc_Absyn_Function_mod_tag; _temp4047.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4048=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4048[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4049; _temp4049.tag= Cyc_Absyn_WithTypes_tag;
_temp4049.f1= 0; _temp4049.f2= 0; _temp4049.f3=({ struct Cyc_Core_Opt* _temp4050=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4050->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4051=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4051[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4052; _temp4052.tag= Cyc_Absyn_JoinEff_tag;
_temp4052.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4053= yyvs;
struct _xtunion_struct** _temp4055= _temp4053.curr +( yyvsp_offset - 1); if(
_temp4053.base == 0? 1:( _temp4055 < _temp4053.base? 1: _temp4055 >= _temp4053.last_plus_one)){
_throw( Null_Exception);}* _temp4055;})); _temp4052;}); _temp4051;})); _temp4050;});
_temp4049;}); _temp4048;})); _temp4047;}); _temp4046;})); _temp4042->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4043= yyvs; struct _xtunion_struct** _temp4045=
_temp4043.curr +( yyvsp_offset - 4); if( _temp4043.base == 0? 1:( _temp4045 <
_temp4043.base? 1: _temp4045 >= _temp4043.last_plus_one)){ _throw(
Null_Exception);}* _temp4045;})))->tms; _temp4042;}); _temp4041;}); _temp4040;});
_temp4039;}); break; case 131: _LL4038: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp4060=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4060[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4061; _temp4061.tag= Cyc_Declarator_tok_tag;
_temp4061.f1=({ struct Cyc_Parse_Declarator* _temp4062=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4062->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4080= yyvs; struct _xtunion_struct** _temp4082=
_temp4080.curr +( yyvsp_offset - 3); if( _temp4080.base == 0? 1:( _temp4082 <
_temp4080.base? 1: _temp4082 >= _temp4080.last_plus_one)){ _throw(
Null_Exception);}* _temp4082;})))->id; _temp4062->tms=({ struct Cyc_List_List*
_temp4063=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4063->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4067=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4067[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4068; _temp4068.tag=
Cyc_Absyn_Function_mod_tag; _temp4068.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct*
_temp4069=( struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp4069[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp4070; _temp4070.tag= Cyc_Absyn_NoTypes_tag;
_temp4070.f1= Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp4071= yyvs;
struct _xtunion_struct** _temp4073= _temp4071.curr +( yyvsp_offset - 1); if(
_temp4071.base == 0? 1:( _temp4073 < _temp4071.base? 1: _temp4073 >= _temp4071.last_plus_one)){
_throw( Null_Exception);}* _temp4073;})); _temp4070.f2= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4074= yyls; struct Cyc_Yyltype* _temp4076= _temp4074.curr
+( yylsp_offset - 3); if( _temp4074.base == 0? 1:( _temp4076 < _temp4074.base? 1:
_temp4076 >= _temp4074.last_plus_one)){ _throw( Null_Exception);}* _temp4076;}).first_line,({
struct _tagged_ptr3 _temp4077= yyls; struct Cyc_Yyltype* _temp4079= _temp4077.curr
+ yylsp_offset; if( _temp4077.base == 0? 1:( _temp4079 < _temp4077.base? 1:
_temp4079 >= _temp4077.last_plus_one)){ _throw( Null_Exception);}* _temp4079;}).last_line);
_temp4070;}); _temp4069;})); _temp4068;}); _temp4067;})); _temp4063->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4064= yyvs; struct _xtunion_struct** _temp4066=
_temp4064.curr +( yyvsp_offset - 3); if( _temp4064.base == 0? 1:( _temp4066 <
_temp4064.base? 1: _temp4066 >= _temp4064.last_plus_one)){ _throw(
Null_Exception);}* _temp4066;})))->tms; _temp4063;}); _temp4062;}); _temp4061;});
_temp4060;}); break; case 132: _LL4059: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4084= yyls; struct Cyc_Yyltype* _temp4086= _temp4084.curr
+( yylsp_offset - 2); if( _temp4084.base == 0? 1:( _temp4086 < _temp4084.base? 1:
_temp4086 >= _temp4084.last_plus_one)){ _throw( Null_Exception);}* _temp4086;}).first_line,({
struct _tagged_ptr3 _temp4087= yyls; struct Cyc_Yyltype* _temp4089= _temp4087.curr
+ yylsp_offset; if( _temp4087.base == 0? 1:( _temp4089 < _temp4087.base? 1:
_temp4089 >= _temp4087.last_plus_one)){ _throw( Null_Exception);}* _temp4089;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4090= yyvs; struct _xtunion_struct** _temp4092=
_temp4090.curr +( yyvsp_offset - 1); if( _temp4090.base == 0? 1:( _temp4092 <
_temp4090.base? 1: _temp4092 >= _temp4090.last_plus_one)){ _throw(
Null_Exception);}* _temp4092;})))); yyval=( struct _xtunion_struct*)({ struct
Cyc_Declarator_tok_struct* _temp4093=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4093[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4094; _temp4094.tag= Cyc_Declarator_tok_tag;
_temp4094.f1=({ struct Cyc_Parse_Declarator* _temp4095=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4095->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4108= yyvs; struct _xtunion_struct** _temp4110=
_temp4108.curr +( yyvsp_offset - 3); if( _temp4108.base == 0? 1:( _temp4110 <
_temp4108.base? 1: _temp4110 >= _temp4108.last_plus_one)){ _throw(
Null_Exception);}* _temp4110;})))->id; _temp4095->tms=({ struct Cyc_List_List*
_temp4096=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4096->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4100=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4100[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp4101; _temp4101.tag= Cyc_Absyn_TypeParams_mod_tag; _temp4101.f1= ts;
_temp4101.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4102= yyls;
struct Cyc_Yyltype* _temp4104= _temp4102.curr +( yylsp_offset - 3); if(
_temp4102.base == 0? 1:( _temp4104 < _temp4102.base? 1: _temp4104 >= _temp4102.last_plus_one)){
_throw( Null_Exception);}* _temp4104;}).first_line,({ struct _tagged_ptr3
_temp4105= yyls; struct Cyc_Yyltype* _temp4107= _temp4105.curr + yylsp_offset;
if( _temp4105.base == 0? 1:( _temp4107 < _temp4105.base? 1: _temp4107 >=
_temp4105.last_plus_one)){ _throw( Null_Exception);}* _temp4107;}).last_line);
_temp4101.f3= 0; _temp4101;}); _temp4100;})); _temp4096->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4097= yyvs; struct _xtunion_struct** _temp4099=
_temp4097.curr +( yyvsp_offset - 3); if( _temp4097.base == 0? 1:( _temp4099 <
_temp4097.base? 1: _temp4099 >= _temp4097.last_plus_one)){ _throw(
Null_Exception);}* _temp4099;})))->tms; _temp4096;}); _temp4095;}); _temp4094;});
_temp4093;}); break;} case 133: _LL4083:(( struct Cyc_Lexing_lexbuf*)({ struct
Cyc_Core_Opt* _temp4112= Cyc_Parse_lbuf; if( _temp4112 == 0){ _throw(
Null_Exception);} _temp4112->v;}))->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4113=
yyls; struct Cyc_Yyltype* _temp4115= _temp4113.curr +( yylsp_offset - 2); if(
_temp4113.base == 0? 1:( _temp4115 < _temp4113.base? 1: _temp4115 >= _temp4113.last_plus_one)){
_throw( Null_Exception);}* _temp4115;}).first_line,({ struct _tagged_ptr3
_temp4116= yyls; struct Cyc_Yyltype* _temp4118= _temp4116.curr + yylsp_offset;
if( _temp4116.base == 0? 1:( _temp4118 < _temp4116.base? 1: _temp4118 >=
_temp4116.last_plus_one)){ _throw( Null_Exception);}* _temp4118;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4119= yyvs; struct _xtunion_struct** _temp4121=
_temp4119.curr +( yyvsp_offset - 1); if( _temp4119.base == 0? 1:( _temp4121 <
_temp4119.base? 1: _temp4121 >= _temp4119.last_plus_one)){ _throw(
Null_Exception);}* _temp4121;})))); yyval=( struct _xtunion_struct*)({ struct
Cyc_Declarator_tok_struct* _temp4122=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4122[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4123; _temp4123.tag= Cyc_Declarator_tok_tag;
_temp4123.f1=({ struct Cyc_Parse_Declarator* _temp4124=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4124->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4137= yyvs; struct _xtunion_struct** _temp4139=
_temp4137.curr +( yyvsp_offset - 3); if( _temp4137.base == 0? 1:( _temp4139 <
_temp4137.base? 1: _temp4139 >= _temp4137.last_plus_one)){ _throw(
Null_Exception);}* _temp4139;})))->id; _temp4124->tms=({ struct Cyc_List_List*
_temp4125=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4125->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4129=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4129[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp4130; _temp4130.tag= Cyc_Absyn_TypeParams_mod_tag; _temp4130.f1= ts;
_temp4130.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4131= yyls;
struct Cyc_Yyltype* _temp4133= _temp4131.curr +( yylsp_offset - 3); if(
_temp4131.base == 0? 1:( _temp4133 < _temp4131.base? 1: _temp4133 >= _temp4131.last_plus_one)){
_throw( Null_Exception);}* _temp4133;}).first_line,({ struct _tagged_ptr3
_temp4134= yyls; struct Cyc_Yyltype* _temp4136= _temp4134.curr + yylsp_offset;
if( _temp4134.base == 0? 1:( _temp4136 < _temp4134.base? 1: _temp4136 >=
_temp4134.last_plus_one)){ _throw( Null_Exception);}* _temp4136;}).last_line);
_temp4130.f3= 0; _temp4130;}); _temp4129;})); _temp4125->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4126= yyvs; struct _xtunion_struct** _temp4128=
_temp4126.curr +( yyvsp_offset - 3); if( _temp4126.base == 0? 1:( _temp4128 <
_temp4126.base? 1: _temp4128 >= _temp4126.last_plus_one)){ _throw(
Null_Exception);}* _temp4128;})))->tms; _temp4125;}); _temp4124;}); _temp4123;});
_temp4122;}); break;} case 134: _LL4111: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp4141=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4141[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4142; _temp4142.tag= Cyc_Declarator_tok_tag;
_temp4142.f1=({ struct Cyc_Parse_Declarator* _temp4143=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4143->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4159= yyvs; struct _xtunion_struct** _temp4161=
_temp4159.curr +( yyvsp_offset - 1); if( _temp4159.base == 0? 1:( _temp4161 <
_temp4159.base? 1: _temp4161 >= _temp4159.last_plus_one)){ _throw(
Null_Exception);}* _temp4161;})))->id; _temp4143->tms=({ struct Cyc_List_List*
_temp4144=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4144->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp4148=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp4148[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp4149; _temp4149.tag= Cyc_Absyn_Attributes_mod_tag; _temp4149.f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4150= yyls; struct Cyc_Yyltype* _temp4152= _temp4150.curr
+ yylsp_offset; if( _temp4150.base == 0? 1:( _temp4152 < _temp4150.base? 1:
_temp4152 >= _temp4150.last_plus_one)){ _throw( Null_Exception);}* _temp4152;}).first_line,({
struct _tagged_ptr3 _temp4153= yyls; struct Cyc_Yyltype* _temp4155= _temp4153.curr
+ yylsp_offset; if( _temp4153.base == 0? 1:( _temp4155 < _temp4153.base? 1:
_temp4155 >= _temp4153.last_plus_one)){ _throw( Null_Exception);}* _temp4155;}).last_line);
_temp4149.f2= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4156= yyvs;
struct _xtunion_struct** _temp4158= _temp4156.curr + yyvsp_offset; if( _temp4156.base
== 0? 1:( _temp4158 < _temp4156.base? 1: _temp4158 >= _temp4156.last_plus_one)){
_throw( Null_Exception);}* _temp4158;})); _temp4149;}); _temp4148;})); _temp4144->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp4145= yyvs; struct
_xtunion_struct** _temp4147= _temp4145.curr +( yyvsp_offset - 1); if( _temp4145.base
== 0? 1:( _temp4147 < _temp4145.base? 1: _temp4147 >= _temp4145.last_plus_one)){
_throw( Null_Exception);}* _temp4147;})))->tms; _temp4144;}); _temp4143;});
_temp4142;}); _temp4141;}); break; case 135: _LL4140: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeModifierList_tok_struct* _temp4163=( struct
Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp4163[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp4164; _temp4164.tag=
Cyc_TypeModifierList_tok_tag; _temp4164.f1=({ struct Cyc_List_List* _temp4165=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4165->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4175=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4175[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4176; _temp4176.tag= Cyc_Absyn_Pointer_mod_tag;
_temp4176.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4177= yyvs; struct _xtunion_struct** _temp4179= _temp4177.curr +(
yyvsp_offset - 2); if( _temp4177.base == 0? 1:( _temp4179 < _temp4177.base? 1:
_temp4179 >= _temp4177.last_plus_one)){ _throw( Null_Exception);}* _temp4179;}));
_temp4176.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4180= yyvs;
struct _xtunion_struct** _temp4182= _temp4180.curr +( yyvsp_offset - 1); if(
_temp4180.base == 0? 1:( _temp4182 < _temp4180.base? 1: _temp4182 >= _temp4180.last_plus_one)){
_throw( Null_Exception);}* _temp4182;})); _temp4176.f3= Cyc_Absyn_empty_tqual();
_temp4176;}); _temp4175;})); _temp4165->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4166= yyls; struct Cyc_Yyltype* _temp4168= _temp4166.curr
+ yylsp_offset; if( _temp4166.base == 0? 1:( _temp4168 < _temp4166.base? 1:
_temp4168 >= _temp4166.last_plus_one)){ _throw( Null_Exception);}* _temp4168;}).first_line,({
struct _tagged_ptr3 _temp4169= yyls; struct Cyc_Yyltype* _temp4171= _temp4169.curr
+ yylsp_offset; if( _temp4169.base == 0? 1:( _temp4171 < _temp4169.base? 1:
_temp4171 >= _temp4169.last_plus_one)){ _throw( Null_Exception);}* _temp4171;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4172= yyvs; struct
_xtunion_struct** _temp4174= _temp4172.curr + yyvsp_offset; if( _temp4172.base
== 0? 1:( _temp4174 < _temp4172.base? 1: _temp4174 >= _temp4172.last_plus_one)){
_throw( Null_Exception);}* _temp4174;})), 0); _temp4165;}); _temp4164;});
_temp4163;}); break; case 136: _LL4162: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeModifierList_tok_struct* _temp4184=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp4184[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp4185; _temp4185.tag= Cyc_TypeModifierList_tok_tag;
_temp4185.f1=({ struct Cyc_List_List* _temp4186=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4186->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp4196=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4196[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4197; _temp4197.tag= Cyc_Absyn_Pointer_mod_tag;
_temp4197.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4198= yyvs; struct _xtunion_struct** _temp4200= _temp4198.curr +(
yyvsp_offset - 3); if( _temp4198.base == 0? 1:( _temp4200 < _temp4198.base? 1:
_temp4200 >= _temp4198.last_plus_one)){ _throw( Null_Exception);}* _temp4200;}));
_temp4197.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4201= yyvs;
struct _xtunion_struct** _temp4203= _temp4201.curr +( yyvsp_offset - 2); if(
_temp4201.base == 0? 1:( _temp4203 < _temp4201.base? 1: _temp4203 >= _temp4201.last_plus_one)){
_throw( Null_Exception);}* _temp4203;})); _temp4197.f3= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp4204= yyvs; struct _xtunion_struct** _temp4206=
_temp4204.curr + yyvsp_offset; if( _temp4204.base == 0? 1:( _temp4206 <
_temp4204.base? 1: _temp4206 >= _temp4204.last_plus_one)){ _throw(
Null_Exception);}* _temp4206;})); _temp4197;}); _temp4196;})); _temp4186->tl=
Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4187= yyls; struct Cyc_Yyltype* _temp4189= _temp4187.curr +( yylsp_offset -
1); if( _temp4187.base == 0? 1:( _temp4189 < _temp4187.base? 1: _temp4189 >=
_temp4187.last_plus_one)){ _throw( Null_Exception);}* _temp4189;}).first_line,({
struct _tagged_ptr3 _temp4190= yyls; struct Cyc_Yyltype* _temp4192= _temp4190.curr
+( yylsp_offset - 1); if( _temp4190.base == 0? 1:( _temp4192 < _temp4190.base? 1:
_temp4192 >= _temp4190.last_plus_one)){ _throw( Null_Exception);}* _temp4192;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4193= yyvs; struct
_xtunion_struct** _temp4195= _temp4193.curr +( yyvsp_offset - 1); if( _temp4193.base
== 0? 1:( _temp4195 < _temp4193.base? 1: _temp4195 >= _temp4193.last_plus_one)){
_throw( Null_Exception);}* _temp4195;})), 0); _temp4186;}); _temp4185;});
_temp4184;}); break; case 137: _LL4183: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeModifierList_tok_struct* _temp4208=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp4208[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp4209; _temp4209.tag= Cyc_TypeModifierList_tok_tag;
_temp4209.f1=({ struct Cyc_List_List* _temp4210=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4210->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp4223=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4223[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4224; _temp4224.tag= Cyc_Absyn_Pointer_mod_tag;
_temp4224.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4225= yyvs; struct _xtunion_struct** _temp4227= _temp4225.curr +(
yyvsp_offset - 3); if( _temp4225.base == 0? 1:( _temp4227 < _temp4225.base? 1:
_temp4227 >= _temp4225.last_plus_one)){ _throw( Null_Exception);}* _temp4227;}));
_temp4224.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4228= yyvs;
struct _xtunion_struct** _temp4230= _temp4228.curr +( yyvsp_offset - 2); if(
_temp4228.base == 0? 1:( _temp4230 < _temp4228.base? 1: _temp4230 >= _temp4228.last_plus_one)){
_throw( Null_Exception);}* _temp4230;})); _temp4224.f3= Cyc_Absyn_empty_tqual();
_temp4224;}); _temp4223;})); _temp4210->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4211= yyls; struct Cyc_Yyltype* _temp4213= _temp4211.curr
+( yylsp_offset - 1); if( _temp4211.base == 0? 1:( _temp4213 < _temp4211.base? 1:
_temp4213 >= _temp4211.last_plus_one)){ _throw( Null_Exception);}* _temp4213;}).first_line,({
struct _tagged_ptr3 _temp4214= yyls; struct Cyc_Yyltype* _temp4216= _temp4214.curr
+( yylsp_offset - 1); if( _temp4214.base == 0? 1:( _temp4216 < _temp4214.base? 1:
_temp4216 >= _temp4214.last_plus_one)){ _throw( Null_Exception);}* _temp4216;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4217= yyvs; struct
_xtunion_struct** _temp4219= _temp4217.curr +( yyvsp_offset - 1); if( _temp4217.base
== 0? 1:( _temp4219 < _temp4217.base? 1: _temp4219 >= _temp4217.last_plus_one)){
_throw( Null_Exception);}* _temp4219;})), Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp4220= yyvs; struct _xtunion_struct** _temp4222=
_temp4220.curr + yyvsp_offset; if( _temp4220.base == 0? 1:( _temp4222 <
_temp4220.base? 1: _temp4222 >= _temp4220.last_plus_one)){ _throw(
Null_Exception);}* _temp4222;}))); _temp4210;}); _temp4209;}); _temp4208;});
break; case 138: _LL4207: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeModifierList_tok_struct*
_temp4232=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp4232[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp4233; _temp4233.tag= Cyc_TypeModifierList_tok_tag; _temp4233.f1=({ struct
Cyc_List_List* _temp4234=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4234->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4247=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp4247[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp4248; _temp4248.tag=
Cyc_Absyn_Pointer_mod_tag; _temp4248.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp4249= yyvs; struct _xtunion_struct** _temp4251=
_temp4249.curr +( yyvsp_offset - 4); if( _temp4249.base == 0? 1:( _temp4251 <
_temp4249.base? 1: _temp4251 >= _temp4249.last_plus_one)){ _throw(
Null_Exception);}* _temp4251;})); _temp4248.f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp4252= yyvs; struct _xtunion_struct** _temp4254=
_temp4252.curr +( yyvsp_offset - 3); if( _temp4252.base == 0? 1:( _temp4254 <
_temp4252.base? 1: _temp4254 >= _temp4252.last_plus_one)){ _throw(
Null_Exception);}* _temp4254;})); _temp4248.f3= Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp4255= yyvs; struct _xtunion_struct** _temp4257= _temp4255.curr
+( yyvsp_offset - 1); if( _temp4255.base == 0? 1:( _temp4257 < _temp4255.base? 1:
_temp4257 >= _temp4255.last_plus_one)){ _throw( Null_Exception);}* _temp4257;}));
_temp4248;}); _temp4247;})); _temp4234->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4235= yyls; struct Cyc_Yyltype* _temp4237= _temp4235.curr
+( yylsp_offset - 2); if( _temp4235.base == 0? 1:( _temp4237 < _temp4235.base? 1:
_temp4237 >= _temp4235.last_plus_one)){ _throw( Null_Exception);}* _temp4237;}).first_line,({
struct _tagged_ptr3 _temp4238= yyls; struct Cyc_Yyltype* _temp4240= _temp4238.curr
+( yylsp_offset - 2); if( _temp4238.base == 0? 1:( _temp4240 < _temp4238.base? 1:
_temp4240 >= _temp4238.last_plus_one)){ _throw( Null_Exception);}* _temp4240;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4241= yyvs; struct
_xtunion_struct** _temp4243= _temp4241.curr +( yyvsp_offset - 2); if( _temp4241.base
== 0? 1:( _temp4243 < _temp4241.base? 1: _temp4243 >= _temp4241.last_plus_one)){
_throw( Null_Exception);}* _temp4243;})), Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp4244= yyvs; struct _xtunion_struct** _temp4246=
_temp4244.curr + yyvsp_offset; if( _temp4244.base == 0? 1:( _temp4246 <
_temp4244.base? 1: _temp4246 >= _temp4244.last_plus_one)){ _throw(
Null_Exception);}* _temp4246;}))); _temp4234;}); _temp4233;}); _temp4232;});
break; case 139: _LL4231: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4259=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4259[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4260; _temp4260.tag=
Cyc_Pointer_Sort_tok_tag; _temp4260.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp4261=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp4261[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp4262; _temp4262.tag=
Cyc_Absyn_Nullable_ps_tag; _temp4262.f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4263= yyls; struct Cyc_Yyltype* _temp4265= _temp4263.curr
+ yylsp_offset; if( _temp4263.base == 0? 1:( _temp4265 < _temp4263.base? 1:
_temp4265 >= _temp4263.last_plus_one)){ _throw( Null_Exception);}* _temp4265;}).first_line,({
struct _tagged_ptr3 _temp4266= yyls; struct Cyc_Yyltype* _temp4268= _temp4266.curr
+ yylsp_offset; if( _temp4266.base == 0? 1:( _temp4268 < _temp4266.base? 1:
_temp4268 >= _temp4266.last_plus_one)){ _throw( Null_Exception);}* _temp4268;}).last_line));
_temp4262;}); _temp4261;})); _temp4260;}); _temp4259;}); break; case 140:
_LL4258: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4270=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4270[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4271; _temp4271.tag=
Cyc_Pointer_Sort_tok_tag; _temp4271.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp4272=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp4272[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp4273; _temp4273.tag= Cyc_Absyn_NonNullable_ps_tag; _temp4273.f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4274= yyls; struct
Cyc_Yyltype* _temp4276= _temp4274.curr + yylsp_offset; if( _temp4274.base == 0?
1:( _temp4276 < _temp4274.base? 1: _temp4276 >= _temp4274.last_plus_one)){
_throw( Null_Exception);}* _temp4276;}).first_line,({ struct _tagged_ptr3
_temp4277= yyls; struct Cyc_Yyltype* _temp4279= _temp4277.curr + yylsp_offset;
if( _temp4277.base == 0? 1:( _temp4279 < _temp4277.base? 1: _temp4279 >=
_temp4277.last_plus_one)){ _throw( Null_Exception);}* _temp4279;}).last_line));
_temp4273;}); _temp4272;})); _temp4271;}); _temp4270;}); break; case 141:
_LL4269: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4281=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4281[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4282; _temp4282.tag=
Cyc_Pointer_Sort_tok_tag; _temp4282.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp4283=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp4283[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp4284; _temp4284.tag=
Cyc_Absyn_Nullable_ps_tag; _temp4284.f1= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4285= yyvs; struct _xtunion_struct** _temp4287= _temp4285.curr
+( yyvsp_offset - 1); if( _temp4285.base == 0? 1:( _temp4287 < _temp4285.base? 1:
_temp4287 >= _temp4285.last_plus_one)){ _throw( Null_Exception);}* _temp4287;}));
_temp4284;}); _temp4283;})); _temp4282;}); _temp4281;}); break; case 142:
_LL4280: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4289=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4289[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4290; _temp4290.tag=
Cyc_Pointer_Sort_tok_tag; _temp4290.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp4291=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp4291[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp4292; _temp4292.tag= Cyc_Absyn_NonNullable_ps_tag; _temp4292.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4293= yyvs; struct _xtunion_struct** _temp4295=
_temp4293.curr +( yyvsp_offset - 1); if( _temp4293.base == 0? 1:( _temp4295 <
_temp4293.base? 1: _temp4295 >= _temp4293.last_plus_one)){ _throw(
Null_Exception);}* _temp4295;})); _temp4292;}); _temp4291;})); _temp4290;});
_temp4289;}); break; case 143: _LL4288: yyval=( struct _xtunion_struct*)({
struct Cyc_Pointer_Sort_tok_struct* _temp4297=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp4297[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp4298; _temp4298.tag= Cyc_Pointer_Sort_tok_tag;
_temp4298.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp4298;});
_temp4297;}); break; case 144: _LL4296: yyval=( struct _xtunion_struct*)({
struct Cyc_Rgn_tok_struct* _temp4300=( struct Cyc_Rgn_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgn_tok_struct)); _temp4300[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4301; _temp4301.tag= Cyc_Rgn_tok_tag; _temp4301.f1=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp4301;}); _temp4300;}); break; case 145: _LL4299: yyval=({ struct
_tagged_ptr2 _temp4303= yyvs; struct _xtunion_struct** _temp4305= _temp4303.curr
+ yyvsp_offset; if( _temp4303.base == 0? 1:( _temp4305 < _temp4303.base? 1:
_temp4305 >= _temp4303.last_plus_one)){ _throw( Null_Exception);}* _temp4305;});
break; case 146: _LL4302: yyval=( struct _xtunion_struct*)({ struct Cyc_Rgn_tok_struct*
_temp4307=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp4307[ 0]=({ struct Cyc_Rgn_tok_struct _temp4308; _temp4308.tag= Cyc_Rgn_tok_tag;
_temp4308.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4309= yyvs; struct _xtunion_struct** _temp4311= _temp4309.curr
+ yyvsp_offset; if( _temp4309.base == 0? 1:( _temp4311 < _temp4309.base? 1:
_temp4311 >= _temp4309.last_plus_one)){ _throw( Null_Exception);}* _temp4311;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4308;}); _temp4307;}); break; case 147: _LL4306: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp4313= yyvs; struct _xtunion_struct** _temp4315=
_temp4313.curr + yyvsp_offset; if( _temp4313.base == 0? 1:( _temp4315 <
_temp4313.base? 1: _temp4315 >= _temp4313.last_plus_one)){ _throw(
Null_Exception);}* _temp4315;})) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4316=( char*)"expecting region kind\n";
struct _tagged_string _temp4317; _temp4317.curr= _temp4316; _temp4317.base=
_temp4316; _temp4317.last_plus_one= _temp4316 + 23; _temp4317;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4318= yyls; struct Cyc_Yyltype* _temp4320= _temp4318.curr
+ yylsp_offset; if( _temp4318.base == 0? 1:( _temp4320 < _temp4318.base? 1:
_temp4320 >= _temp4318.last_plus_one)){ _throw( Null_Exception);}* _temp4320;}).first_line,({
struct _tagged_ptr3 _temp4321= yyls; struct Cyc_Yyltype* _temp4323= _temp4321.curr
+ yylsp_offset; if( _temp4321.base == 0? 1:( _temp4323 < _temp4321.base? 1:
_temp4323 >= _temp4321.last_plus_one)){ _throw( Null_Exception);}* _temp4323;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_Rgn_tok_struct* _temp4324=( struct
Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4324[
0]=({ struct Cyc_Rgn_tok_struct _temp4325; _temp4325.tag= Cyc_Rgn_tok_tag;
_temp4325.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4326= yyvs; struct _xtunion_struct** _temp4328= _temp4326.curr
+( yyvsp_offset - 2); if( _temp4326.base == 0? 1:( _temp4328 < _temp4326.base? 1:
_temp4328 >= _temp4326.last_plus_one)){ _throw( Null_Exception);}* _temp4328;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4325;}); _temp4324;}); break; case 148: _LL4312: yyval=( struct
_xtunion_struct*)({ struct Cyc_Rgn_tok_struct* _temp4330=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4330[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4331; _temp4331.tag= Cyc_Rgn_tok_tag; _temp4331.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp4331;}); _temp4330;}); break; case 149: _LL4329:
yyval=({ struct _tagged_ptr2 _temp4333= yyvs; struct _xtunion_struct** _temp4335=
_temp4333.curr + yyvsp_offset; if( _temp4333.base == 0? 1:( _temp4335 <
_temp4333.base? 1: _temp4335 >= _temp4333.last_plus_one)){ _throw(
Null_Exception);}* _temp4335;}); break; case 150: _LL4332: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeQual_tok_struct* _temp4337=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp4337[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp4338; _temp4338.tag= Cyc_TypeQual_tok_tag; _temp4338.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp4339= yyvs; struct
_xtunion_struct** _temp4341= _temp4339.curr +( yyvsp_offset - 1); if( _temp4339.base
== 0? 1:( _temp4341 < _temp4339.base? 1: _temp4341 >= _temp4339.last_plus_one)){
_throw( Null_Exception);}* _temp4341;})), Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp4342= yyvs; struct _xtunion_struct** _temp4344= _temp4342.curr
+ yyvsp_offset; if( _temp4342.base == 0? 1:( _temp4344 < _temp4342.base? 1:
_temp4344 >= _temp4342.last_plus_one)){ _throw( Null_Exception);}* _temp4344;})));
_temp4338;}); _temp4337;}); break; case 151: _LL4336: yyval=( struct
_xtunion_struct*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp4346=( struct
Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp4346[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp4347; _temp4347.tag=
Cyc_ParamDeclListBool_tok_tag; _temp4347.f1=({ struct _tuple16* _temp4348=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp4348->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4349= yyvs; struct _xtunion_struct** _temp4351=
_temp4349.curr + yyvsp_offset; if( _temp4349.base == 0? 1:( _temp4351 <
_temp4349.base? 1: _temp4351 >= _temp4349.last_plus_one)){ _throw(
Null_Exception);}* _temp4351;}))); _temp4348->f2= 0; _temp4348->f3= 0; _temp4348;});
_temp4347;}); _temp4346;}); break; case 152: _LL4345: yyval=( struct
_xtunion_struct*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp4353=( struct
Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp4353[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp4354; _temp4354.tag=
Cyc_ParamDeclListBool_tok_tag; _temp4354.f1=({ struct _tuple16* _temp4355=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp4355->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4356= yyvs; struct _xtunion_struct** _temp4358=
_temp4356.curr +( yyvsp_offset - 2); if( _temp4356.base == 0? 1:( _temp4358 <
_temp4356.base? 1: _temp4358 >= _temp4356.last_plus_one)){ _throw(
Null_Exception);}* _temp4358;}))); _temp4355->f2= 1; _temp4355->f3= 0; _temp4355;});
_temp4354;}); _temp4353;}); break; case 153: _LL4352: yyval=( struct
_xtunion_struct*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp4360=( struct
Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp4360[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp4361; _temp4361.tag=
Cyc_ParamDeclListBool_tok_tag; _temp4361.f1=({ struct _tuple16* _temp4362=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp4362->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4369= yyvs; struct _xtunion_struct** _temp4371=
_temp4369.curr +( yyvsp_offset - 2); if( _temp4369.base == 0? 1:( _temp4371 <
_temp4369.base? 1: _temp4371 >= _temp4369.last_plus_one)){ _throw(
Null_Exception);}* _temp4371;}))); _temp4362->f2= 0; _temp4362->f3=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp4363=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp4363->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp4364=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4364[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4365; _temp4365.tag= Cyc_Absyn_JoinEff_tag;
_temp4365.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4366= yyvs;
struct _xtunion_struct** _temp4368= _temp4366.curr + yyvsp_offset; if( _temp4366.base
== 0? 1:( _temp4368 < _temp4366.base? 1: _temp4368 >= _temp4366.last_plus_one)){
_throw( Null_Exception);}* _temp4368;})); _temp4365;}); _temp4364;})); _temp4363;});
_temp4362;}); _temp4361;}); _temp4360;}); break; case 154: _LL4359: yyval=({
struct _tagged_ptr2 _temp4373= yyvs; struct _xtunion_struct** _temp4375=
_temp4373.curr + yyvsp_offset; if( _temp4373.base == 0? 1:( _temp4375 <
_temp4373.base? 1: _temp4375 >= _temp4373.last_plus_one)){ _throw(
Null_Exception);}* _temp4375;}); break; case 155: _LL4372: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4377=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4377[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4378; _temp4378.tag= Cyc_TypeList_tok_tag; _temp4378.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4379= yyvs; struct _xtunion_struct** _temp4381=
_temp4379.curr +( yyvsp_offset - 2); if( _temp4379.base == 0? 1:( _temp4381 <
_temp4379.base? 1: _temp4381 >= _temp4379.last_plus_one)){ _throw(
Null_Exception);}* _temp4381;})), Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2
_temp4382= yyvs; struct _xtunion_struct** _temp4384= _temp4382.curr +
yyvsp_offset; if( _temp4382.base == 0? 1:( _temp4384 < _temp4382.base? 1:
_temp4384 >= _temp4382.last_plus_one)){ _throw( Null_Exception);}* _temp4384;})));
_temp4378;}); _temp4377;}); break; case 156: _LL4376: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4386=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4386[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4387; _temp4387.tag= Cyc_TypeList_tok_tag; _temp4387.f1= 0; _temp4387;});
_temp4386;}); break; case 157: _LL4385: yyval=({ struct _tagged_ptr2 _temp4389=
yyvs; struct _xtunion_struct** _temp4391= _temp4389.curr +( yyvsp_offset - 1);
if( _temp4389.base == 0? 1:( _temp4391 < _temp4389.base? 1: _temp4391 >=
_temp4389.last_plus_one)){ _throw( Null_Exception);}* _temp4391;}); break; case
158: _LL4388: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct*
_temp4393=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4393[ 0]=({ struct Cyc_TypeList_tok_struct _temp4394; _temp4394.tag= Cyc_TypeList_tok_tag;
_temp4394.f1=({ struct Cyc_List_List* _temp4395=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4395->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4396= yyvs; struct
_xtunion_struct** _temp4398= _temp4396.curr + yyvsp_offset; if( _temp4396.base
== 0? 1:( _temp4398 < _temp4396.base? 1: _temp4398 >= _temp4396.last_plus_one)){
_throw( Null_Exception);}* _temp4398;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp4395->tl= 0; _temp4395;});
_temp4394;}); _temp4393;}); break; case 159: _LL4392: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp4400= yyvs; struct _xtunion_struct** _temp4402=
_temp4400.curr + yyvsp_offset; if( _temp4400.base == 0? 1:( _temp4402 <
_temp4400.base? 1: _temp4402 >= _temp4400.last_plus_one)){ _throw(
Null_Exception);}* _temp4402;})) !=( void*) Cyc_Absyn_EffKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4403=( char*)"expecing effect kind (E)";
struct _tagged_string _temp4404; _temp4404.curr= _temp4403; _temp4404.base=
_temp4403; _temp4404.last_plus_one= _temp4403 + 25; _temp4404;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4405= yyls; struct Cyc_Yyltype* _temp4407= _temp4405.curr
+ yylsp_offset; if( _temp4405.base == 0? 1:( _temp4407 < _temp4405.base? 1:
_temp4407 >= _temp4405.last_plus_one)){ _throw( Null_Exception);}* _temp4407;}).first_line,({
struct _tagged_ptr3 _temp4408= yyls; struct Cyc_Yyltype* _temp4410= _temp4408.curr
+ yylsp_offset; if( _temp4408.base == 0? 1:( _temp4410 < _temp4408.base? 1:
_temp4410 >= _temp4408.last_plus_one)){ _throw( Null_Exception);}* _temp4410;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4411=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4411[ 0]=({ struct Cyc_TypeList_tok_struct _temp4412; _temp4412.tag= Cyc_TypeList_tok_tag;
_temp4412.f1=({ struct Cyc_List_List* _temp4413=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4413->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4414= yyvs; struct
_xtunion_struct** _temp4416= _temp4414.curr +( yyvsp_offset - 2); if( _temp4414.base
== 0? 1:( _temp4416 < _temp4414.base? 1: _temp4416 >= _temp4414.last_plus_one)){
_throw( Null_Exception);}* _temp4416;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp4413->tl= 0; _temp4413;});
_temp4412;}); _temp4411;}); break; case 160: _LL4399: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4418=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4418[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4419; _temp4419.tag= Cyc_TypeList_tok_tag; _temp4419.f1=({ struct Cyc_List_List*
_temp4420=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4420->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4421=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4421[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4422; _temp4422.tag= Cyc_Absyn_AccessEff_tag;
_temp4422.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4423= yyvs; struct _xtunion_struct** _temp4425= _temp4423.curr
+ yyvsp_offset; if( _temp4423.base == 0? 1:( _temp4425 < _temp4423.base? 1:
_temp4425 >= _temp4423.last_plus_one)){ _throw( Null_Exception);}* _temp4425;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4422;}); _temp4421;})); _temp4420->tl= 0; _temp4420;}); _temp4419;});
_temp4418;}); break; case 161: _LL4417: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeList_tok_struct* _temp4427=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4427[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4428; _temp4428.tag= Cyc_TypeList_tok_tag; _temp4428.f1=({ struct Cyc_List_List*
_temp4429=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4429->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4433=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4433[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4434; _temp4434.tag= Cyc_Absyn_AccessEff_tag;
_temp4434.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4435= yyvs; struct _xtunion_struct** _temp4437= _temp4435.curr
+( yyvsp_offset - 2); if( _temp4435.base == 0? 1:( _temp4437 < _temp4435.base? 1:
_temp4437 >= _temp4435.last_plus_one)){ _throw( Null_Exception);}* _temp4437;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4434;}); _temp4433;})); _temp4429->tl= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4430= yyvs; struct _xtunion_struct** _temp4432= _temp4430.curr
+ yyvsp_offset; if( _temp4430.base == 0? 1:( _temp4432 < _temp4430.base? 1:
_temp4432 >= _temp4430.last_plus_one)){ _throw( Null_Exception);}* _temp4432;}));
_temp4429;}); _temp4428;}); _temp4427;}); break; case 162: _LL4426: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp4439= yyvs; struct _xtunion_struct** _temp4441=
_temp4439.curr + yyvsp_offset; if( _temp4439.base == 0? 1:( _temp4441 <
_temp4439.base? 1: _temp4441 >= _temp4439.last_plus_one)){ _throw(
Null_Exception);}* _temp4441;})) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4442=( char*)"expecting region kind (R)";
struct _tagged_string _temp4443; _temp4443.curr= _temp4442; _temp4443.base=
_temp4442; _temp4443.last_plus_one= _temp4442 + 26; _temp4443;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4444= yyls; struct Cyc_Yyltype* _temp4446= _temp4444.curr
+ yylsp_offset; if( _temp4444.base == 0? 1:( _temp4446 < _temp4444.base? 1:
_temp4446 >= _temp4444.last_plus_one)){ _throw( Null_Exception);}* _temp4446;}).first_line,({
struct _tagged_ptr3 _temp4447= yyls; struct Cyc_Yyltype* _temp4449= _temp4447.curr
+ yylsp_offset; if( _temp4447.base == 0? 1:( _temp4449 < _temp4447.base? 1:
_temp4449 >= _temp4447.last_plus_one)){ _throw( Null_Exception);}* _temp4449;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4450=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4450[ 0]=({ struct Cyc_TypeList_tok_struct _temp4451; _temp4451.tag= Cyc_TypeList_tok_tag;
_temp4451.f1=({ struct Cyc_List_List* _temp4452=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4452->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp4453=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp4453[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp4454; _temp4454.tag= Cyc_Absyn_AccessEff_tag;
_temp4454.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4455= yyvs; struct _xtunion_struct** _temp4457= _temp4455.curr
+( yyvsp_offset - 2); if( _temp4455.base == 0? 1:( _temp4457 < _temp4455.base? 1:
_temp4457 >= _temp4455.last_plus_one)){ _throw( Null_Exception);}* _temp4457;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4454;}); _temp4453;})); _temp4452->tl= 0; _temp4452;}); _temp4451;});
_temp4450;}); break; case 163: _LL4438: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp4459= yyvs; struct _xtunion_struct** _temp4461= _temp4459.curr
+( yyvsp_offset - 2); if( _temp4459.base == 0? 1:( _temp4461 < _temp4459.base? 1:
_temp4461 >= _temp4459.last_plus_one)){ _throw( Null_Exception);}* _temp4461;}))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp4462=( char*)"expecting region kind (R)"; struct _tagged_string _temp4463;
_temp4463.curr= _temp4462; _temp4463.base= _temp4462; _temp4463.last_plus_one=
_temp4462 + 26; _temp4463;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4464= yyls; struct Cyc_Yyltype* _temp4466= _temp4464.curr +( yylsp_offset -
2); if( _temp4464.base == 0? 1:( _temp4466 < _temp4464.base? 1: _temp4466 >=
_temp4464.last_plus_one)){ _throw( Null_Exception);}* _temp4466;}).first_line,({
struct _tagged_ptr3 _temp4467= yyls; struct Cyc_Yyltype* _temp4469= _temp4467.curr
+( yylsp_offset - 2); if( _temp4467.base == 0? 1:( _temp4469 < _temp4467.base? 1:
_temp4469 >= _temp4467.last_plus_one)){ _throw( Null_Exception);}* _temp4469;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4470=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4470[ 0]=({ struct Cyc_TypeList_tok_struct _temp4471; _temp4471.tag= Cyc_TypeList_tok_tag;
_temp4471.f1=({ struct Cyc_List_List* _temp4472=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4472->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp4476=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp4476[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp4477; _temp4477.tag= Cyc_Absyn_AccessEff_tag;
_temp4477.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4478= yyvs; struct _xtunion_struct** _temp4480= _temp4478.curr
+( yyvsp_offset - 4); if( _temp4478.base == 0? 1:( _temp4480 < _temp4478.base? 1:
_temp4480 >= _temp4478.last_plus_one)){ _throw( Null_Exception);}* _temp4480;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4477;}); _temp4476;})); _temp4472->tl= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4473= yyvs; struct _xtunion_struct** _temp4475= _temp4473.curr
+ yyvsp_offset; if( _temp4473.base == 0? 1:( _temp4475 < _temp4473.base? 1:
_temp4475 >= _temp4473.last_plus_one)){ _throw( Null_Exception);}* _temp4475;}));
_temp4472;}); _temp4471;}); _temp4470;}); break; case 164: _LL4458: yyval=(
struct _xtunion_struct*)({ struct Cyc_ParamDeclList_tok_struct* _temp4482=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp4482[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp4483; _temp4483.tag=
Cyc_ParamDeclList_tok_tag; _temp4483.f1=({ struct Cyc_List_List* _temp4484=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4484->hd=(
void*) Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4485= yyvs; struct
_xtunion_struct** _temp4487= _temp4485.curr + yyvsp_offset; if( _temp4485.base
== 0? 1:( _temp4487 < _temp4485.base? 1: _temp4487 >= _temp4485.last_plus_one)){
_throw( Null_Exception);}* _temp4487;})); _temp4484->tl= 0; _temp4484;});
_temp4483;}); _temp4482;}); break; case 165: _LL4481: yyval=( struct
_xtunion_struct*)({ struct Cyc_ParamDeclList_tok_struct* _temp4489=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp4489[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp4490; _temp4490.tag= Cyc_ParamDeclList_tok_tag;
_temp4490.f1=({ struct Cyc_List_List* _temp4491=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4491->hd=( void*) Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp4495= yyvs; struct _xtunion_struct** _temp4497=
_temp4495.curr + yyvsp_offset; if( _temp4495.base == 0? 1:( _temp4497 <
_temp4495.base? 1: _temp4497 >= _temp4495.last_plus_one)){ _throw(
Null_Exception);}* _temp4497;})); _temp4491->tl= Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4492= yyvs; struct _xtunion_struct** _temp4494=
_temp4492.curr +( yyvsp_offset - 2); if( _temp4492.base == 0? 1:( _temp4494 <
_temp4492.base? 1: _temp4494 >= _temp4492.last_plus_one)){ _throw(
Null_Exception);}* _temp4494;})); _temp4491;}); _temp4490;}); _temp4489;});
break; case 166: _LL4488: { void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4499= yyvs; struct _xtunion_struct** _temp4501=
_temp4499.curr +( yyvsp_offset - 1); if( _temp4499.base == 0? 1:( _temp4501 <
_temp4499.base? 1: _temp4501 >= _temp4499.last_plus_one)){ _throw(
Null_Exception);}* _temp4501;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4502= yyls; struct Cyc_Yyltype* _temp4504= _temp4502.curr +(
yylsp_offset - 1); if( _temp4502.base == 0? 1:( _temp4504 < _temp4502.base? 1:
_temp4504 >= _temp4502.last_plus_one)){ _throw( Null_Exception);}* _temp4504;}).first_line,({
struct _tagged_ptr3 _temp4505= yyls; struct Cyc_Yyltype* _temp4507= _temp4505.curr
+( yylsp_offset - 1); if( _temp4505.base == 0? 1:( _temp4507 < _temp4505.base? 1:
_temp4507 >= _temp4505.last_plus_one)){ _throw( Null_Exception);}* _temp4507;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4508= yyvs; struct _xtunion_struct** _temp4510= _temp4508.curr +(
yyvsp_offset - 1); if( _temp4508.base == 0? 1:( _temp4510 < _temp4508.base? 1:
_temp4510 >= _temp4508.last_plus_one)){ _throw( Null_Exception);}* _temp4510;}))).f3;
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4511= yyvs; struct _xtunion_struct** _temp4513= _temp4511.curr +(
yyvsp_offset - 1); if( _temp4511.base == 0? 1:( _temp4513 < _temp4511.base? 1:
_temp4513 >= _temp4511.last_plus_one)){ _throw( Null_Exception);}* _temp4513;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp4514= yyvs; struct _xtunion_struct** _temp4516= _temp4514.curr +
yyvsp_offset; if( _temp4514.base == 0? 1:( _temp4516 < _temp4514.base? 1:
_temp4516 >= _temp4514.last_plus_one)){ _throw( Null_Exception);}* _temp4516;})))->tms;
struct _tuple6 t_info= Cyc_Parse_apply_tms( tq, t, atts, tms); if( t_info.f3 !=
0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp4517=( char*)"parameter with bad type params";
struct _tagged_string _temp4518; _temp4518.curr= _temp4517; _temp4518.base=
_temp4517; _temp4518.last_plus_one= _temp4517 + 31; _temp4518;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4519= yyls; struct Cyc_Yyltype* _temp4521= _temp4519.curr
+ yylsp_offset; if( _temp4519.base == 0? 1:( _temp4521 < _temp4519.base? 1:
_temp4521 >= _temp4519.last_plus_one)){ _throw( Null_Exception);}* _temp4521;}).first_line,({
struct _tagged_ptr3 _temp4522= yyls; struct Cyc_Yyltype* _temp4524= _temp4522.curr
+ yylsp_offset; if( _temp4522.base == 0? 1:( _temp4524 < _temp4522.base? 1:
_temp4524 >= _temp4522.last_plus_one)){ _throw( Null_Exception);}* _temp4524;}).last_line));}{
struct _tuple1* q=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp4525=
yyvs; struct _xtunion_struct** _temp4527= _temp4525.curr + yyvsp_offset; if(
_temp4525.base == 0? 1:( _temp4527 < _temp4525.base? 1: _temp4527 >= _temp4525.last_plus_one)){
_throw( Null_Exception);}* _temp4527;})))->id;{ void* _temp4528=(* q).f1; struct
Cyc_List_List* _temp4538; struct Cyc_List_List* _temp4540; _LL4530: if(( int)
_temp4528 == Cyc_Absyn_Loc_n){ goto _LL4531;} else{ goto _LL4532;} _LL4532: if((
unsigned int) _temp4528 > 1u?(( struct _tunion_struct*) _temp4528)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL4539: _temp4538=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp4528)->f1; if( _temp4538 == 0){ goto _LL4533;} else{ goto _LL4534;}} else{
goto _LL4534;} _LL4534: if(( unsigned int) _temp4528 > 1u?(( struct
_tunion_struct*) _temp4528)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL4541: _temp4540=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp4528)->f1; if(
_temp4540 == 0){ goto _LL4535;} else{ goto _LL4536;}} else{ goto _LL4536;}
_LL4536: goto _LL4537; _LL4531: goto _LL4529; _LL4533: goto _LL4529; _LL4535:
goto _LL4529; _LL4537: Cyc_Parse_err(( struct _tagged_string)({ char* _temp4542=(
char*)"parameter cannot be qualified with a module name"; struct _tagged_string
_temp4543; _temp4543.curr= _temp4542; _temp4543.base= _temp4542; _temp4543.last_plus_one=
_temp4542 + 49; _temp4543;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4544= yyls; struct Cyc_Yyltype* _temp4546= _temp4544.curr +( yylsp_offset -
1); if( _temp4544.base == 0? 1:( _temp4546 < _temp4544.base? 1: _temp4546 >=
_temp4544.last_plus_one)){ _throw( Null_Exception);}* _temp4546;}).first_line,({
struct _tagged_ptr3 _temp4547= yyls; struct Cyc_Yyltype* _temp4549= _temp4547.curr
+( yylsp_offset - 1); if( _temp4547.base == 0? 1:( _temp4549 < _temp4547.base? 1:
_temp4549 >= _temp4547.last_plus_one)){ _throw( Null_Exception);}* _temp4549;}).last_line));
goto _LL4529; _LL4529:;}{ struct Cyc_Core_Opt* idopt=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp4550=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp4550->v=( void*)(* q).f2; _temp4550;}); if( t_info.f4 != 0){
Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4551=( char*)"extra attributes on parameter, ignoring";
struct _tagged_string _temp4552; _temp4552.curr= _temp4551; _temp4552.base=
_temp4551; _temp4552.last_plus_one= _temp4551 + 40; _temp4552;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4553= yyls; struct Cyc_Yyltype* _temp4555= _temp4553.curr
+( yylsp_offset - 1); if( _temp4553.base == 0? 1:( _temp4555 < _temp4553.base? 1:
_temp4555 >= _temp4553.last_plus_one)){ _throw( Null_Exception);}* _temp4555;}).first_line,({
struct _tagged_ptr3 _temp4556= yyls; struct Cyc_Yyltype* _temp4558= _temp4556.curr
+ yylsp_offset; if( _temp4556.base == 0? 1:( _temp4558 < _temp4556.base? 1:
_temp4558 >= _temp4556.last_plus_one)){ _throw( Null_Exception);}* _temp4558;}).last_line));}
if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4559=(
char*)"extra type variables on parameter, ignoring"; struct _tagged_string
_temp4560; _temp4560.curr= _temp4559; _temp4560.base= _temp4559; _temp4560.last_plus_one=
_temp4559 + 44; _temp4560;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4561= yyls; struct Cyc_Yyltype* _temp4563= _temp4561.curr +( yylsp_offset -
1); if( _temp4561.base == 0? 1:( _temp4563 < _temp4561.base? 1: _temp4563 >=
_temp4561.last_plus_one)){ _throw( Null_Exception);}* _temp4563;}).first_line,({
struct _tagged_ptr3 _temp4564= yyls; struct Cyc_Yyltype* _temp4566= _temp4564.curr
+ yylsp_offset; if( _temp4564.base == 0? 1:( _temp4566 < _temp4564.base? 1:
_temp4566 >= _temp4564.last_plus_one)){ _throw( Null_Exception);}* _temp4566;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4567=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4567[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4568; _temp4568.tag= Cyc_ParamDecl_tok_tag;
_temp4568.f1=({ struct _tuple2* _temp4569=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4569->f1= idopt; _temp4569->f2= t_info.f1; _temp4569->f3=
t_info.f2; _temp4569;}); _temp4568;}); _temp4567;}); break;}}} case 167: _LL4498: {
void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp4571= yyvs; struct _xtunion_struct** _temp4573= _temp4571.curr
+ yyvsp_offset; if( _temp4571.base == 0? 1:( _temp4573 < _temp4571.base? 1:
_temp4573 >= _temp4571.last_plus_one)){ _throw( Null_Exception);}* _temp4573;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4574= yyls; struct Cyc_Yyltype*
_temp4576= _temp4574.curr + yylsp_offset; if( _temp4574.base == 0? 1:( _temp4576
< _temp4574.base? 1: _temp4576 >= _temp4574.last_plus_one)){ _throw(
Null_Exception);}* _temp4576;}).first_line,({ struct _tagged_ptr3 _temp4577=
yyls; struct Cyc_Yyltype* _temp4579= _temp4577.curr + yylsp_offset; if(
_temp4577.base == 0? 1:( _temp4579 < _temp4577.base? 1: _temp4579 >= _temp4577.last_plus_one)){
_throw( Null_Exception);}* _temp4579;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4580= yyvs; struct
_xtunion_struct** _temp4582= _temp4580.curr + yyvsp_offset; if( _temp4580.base
== 0? 1:( _temp4582 < _temp4580.base? 1: _temp4582 >= _temp4580.last_plus_one)){
_throw( Null_Exception);}* _temp4582;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4583= yyvs; struct _xtunion_struct** _temp4585=
_temp4583.curr + yyvsp_offset; if( _temp4583.base == 0? 1:( _temp4585 <
_temp4583.base? 1: _temp4585 >= _temp4583.last_plus_one)){ _throw(
Null_Exception);}* _temp4585;}))).f1; if( atts != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp4586=( char*)"bad attributes on parameter, ignoring";
struct _tagged_string _temp4587; _temp4587.curr= _temp4586; _temp4587.base=
_temp4586; _temp4587.last_plus_one= _temp4586 + 38; _temp4587;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4588= yyls; struct Cyc_Yyltype* _temp4590= _temp4588.curr
+ yylsp_offset; if( _temp4588.base == 0? 1:( _temp4590 < _temp4588.base? 1:
_temp4590 >= _temp4588.last_plus_one)){ _throw( Null_Exception);}* _temp4590;}).first_line,({
struct _tagged_ptr3 _temp4591= yyls; struct Cyc_Yyltype* _temp4593= _temp4591.curr
+ yylsp_offset; if( _temp4591.base == 0? 1:( _temp4593 < _temp4591.base? 1:
_temp4593 >= _temp4591.last_plus_one)){ _throw( Null_Exception);}* _temp4593;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4594=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4594[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4595; _temp4595.tag= Cyc_ParamDecl_tok_tag;
_temp4595.f1=({ struct _tuple2* _temp4596=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4596->f1= 0; _temp4596->f2= tq; _temp4596->f3= t;
_temp4596;}); _temp4595;}); _temp4594;}); break;} case 168: _LL4570: { void* t=
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4598= yyvs; struct _xtunion_struct** _temp4600= _temp4598.curr +(
yyvsp_offset - 1); if( _temp4598.base == 0? 1:( _temp4600 < _temp4598.base? 1:
_temp4600 >= _temp4598.last_plus_one)){ _throw( Null_Exception);}* _temp4600;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4601= yyls; struct Cyc_Yyltype*
_temp4603= _temp4601.curr +( yylsp_offset - 1); if( _temp4601.base == 0? 1:(
_temp4603 < _temp4601.base? 1: _temp4603 >= _temp4601.last_plus_one)){ _throw(
Null_Exception);}* _temp4603;}).first_line,({ struct _tagged_ptr3 _temp4604=
yyls; struct Cyc_Yyltype* _temp4606= _temp4604.curr +( yylsp_offset - 1); if(
_temp4604.base == 0? 1:( _temp4606 < _temp4604.base? 1: _temp4606 >= _temp4604.last_plus_one)){
_throw( Null_Exception);}* _temp4606;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4607= yyvs; struct
_xtunion_struct** _temp4609= _temp4607.curr +( yyvsp_offset - 1); if( _temp4607.base
== 0? 1:( _temp4609 < _temp4607.base? 1: _temp4609 >= _temp4607.last_plus_one)){
_throw( Null_Exception);}* _temp4609;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4610= yyvs; struct _xtunion_struct** _temp4612=
_temp4610.curr +( yyvsp_offset - 1); if( _temp4610.base == 0? 1:( _temp4612 <
_temp4610.base? 1: _temp4612 >= _temp4610.last_plus_one)){ _throw(
Null_Exception);}* _temp4612;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4613= yyvs; struct _xtunion_struct** _temp4615=
_temp4613.curr + yyvsp_offset; if( _temp4613.base == 0? 1:( _temp4615 <
_temp4613.base? 1: _temp4615 >= _temp4613.last_plus_one)){ _throw(
Null_Exception);}* _temp4615;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp4616=( char*)"bad type parameters on formal argument, ignoring";
struct _tagged_string _temp4617; _temp4617.curr= _temp4616; _temp4617.base=
_temp4616; _temp4617.last_plus_one= _temp4616 + 49; _temp4617;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4618= yyls; struct Cyc_Yyltype* _temp4620= _temp4618.curr
+( yylsp_offset - 1); if( _temp4618.base == 0? 1:( _temp4620 < _temp4618.base? 1:
_temp4620 >= _temp4618.last_plus_one)){ _throw( Null_Exception);}* _temp4620;}).first_line,({
struct _tagged_ptr3 _temp4621= yyls; struct Cyc_Yyltype* _temp4623= _temp4621.curr
+ yylsp_offset; if( _temp4621.base == 0? 1:( _temp4623 < _temp4621.base? 1:
_temp4623 >= _temp4621.last_plus_one)){ _throw( Null_Exception);}* _temp4623;}).last_line));}
if( t_info.f4 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4624=(
char*)"bad attributes on parameter, ignoring"; struct _tagged_string _temp4625;
_temp4625.curr= _temp4624; _temp4625.base= _temp4624; _temp4625.last_plus_one=
_temp4624 + 38; _temp4625;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4626= yyls; struct Cyc_Yyltype* _temp4628= _temp4626.curr +( yylsp_offset -
1); if( _temp4626.base == 0? 1:( _temp4628 < _temp4626.base? 1: _temp4628 >=
_temp4626.last_plus_one)){ _throw( Null_Exception);}* _temp4628;}).first_line,({
struct _tagged_ptr3 _temp4629= yyls; struct Cyc_Yyltype* _temp4631= _temp4629.curr
+ yylsp_offset; if( _temp4629.base == 0? 1:( _temp4631 < _temp4629.base? 1:
_temp4631 >= _temp4629.last_plus_one)){ _throw( Null_Exception);}* _temp4631;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4632=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4632[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4633; _temp4633.tag= Cyc_ParamDecl_tok_tag;
_temp4633.f1=({ struct _tuple2* _temp4634=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4634->f1= 0; _temp4634->f2= t_info.f1; _temp4634->f3=
t_info.f2; _temp4634;}); _temp4633;}); _temp4632;}); break;} case 169: _LL4597:
yyval=( struct _xtunion_struct*)({ struct Cyc_IdList_tok_struct* _temp4636=(
struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4636[ 0]=({ struct Cyc_IdList_tok_struct _temp4637; _temp4637.tag= Cyc_IdList_tok_tag;
_temp4637.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp4638= yyvs; struct
_xtunion_struct** _temp4640= _temp4638.curr + yyvsp_offset; if( _temp4638.base
== 0? 1:( _temp4640 < _temp4638.base? 1: _temp4640 >= _temp4638.last_plus_one)){
_throw( Null_Exception);}* _temp4640;}))); _temp4637;}); _temp4636;}); break;
case 170: _LL4635: yyval=( struct _xtunion_struct*)({ struct Cyc_IdList_tok_struct*
_temp4642=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4642[ 0]=({ struct Cyc_IdList_tok_struct _temp4643; _temp4643.tag= Cyc_IdList_tok_tag;
_temp4643.f1=({ struct Cyc_List_List* _temp4644=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4644->hd=( void*)({ struct
_tagged_string* _temp4645=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4645[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp4646= yyvs; struct _xtunion_struct** _temp4648= _temp4646.curr +
yyvsp_offset; if( _temp4646.base == 0? 1:( _temp4648 < _temp4646.base? 1:
_temp4648 >= _temp4646.last_plus_one)){ _throw( Null_Exception);}* _temp4648;}));
_temp4645;}); _temp4644->tl= 0; _temp4644;}); _temp4643;}); _temp4642;}); break;
case 171: _LL4641: yyval=( struct _xtunion_struct*)({ struct Cyc_IdList_tok_struct*
_temp4650=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4650[ 0]=({ struct Cyc_IdList_tok_struct _temp4651; _temp4651.tag= Cyc_IdList_tok_tag;
_temp4651.f1=({ struct Cyc_List_List* _temp4652=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4652->hd=( void*)({ struct
_tagged_string* _temp4656=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4656[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp4657= yyvs; struct _xtunion_struct** _temp4659= _temp4657.curr +
yyvsp_offset; if( _temp4657.base == 0? 1:( _temp4659 < _temp4657.base? 1:
_temp4659 >= _temp4657.last_plus_one)){ _throw( Null_Exception);}* _temp4659;}));
_temp4656;}); _temp4652->tl= Cyc_yyget_IdList_tok(({ struct _tagged_ptr2
_temp4653= yyvs; struct _xtunion_struct** _temp4655= _temp4653.curr +(
yyvsp_offset - 2); if( _temp4653.base == 0? 1:( _temp4655 < _temp4653.base? 1:
_temp4655 >= _temp4653.last_plus_one)){ _throw( Null_Exception);}* _temp4655;}));
_temp4652;}); _temp4651;}); _temp4650;}); break; case 172: _LL4649: yyval=({
struct _tagged_ptr2 _temp4661= yyvs; struct _xtunion_struct** _temp4663=
_temp4661.curr + yyvsp_offset; if( _temp4661.base == 0? 1:( _temp4663 <
_temp4661.base? 1: _temp4663 >= _temp4661.last_plus_one)){ _throw(
Null_Exception);}* _temp4663;}); break; case 173: _LL4660: yyval=({ struct
_tagged_ptr2 _temp4665= yyvs; struct _xtunion_struct** _temp4667= _temp4665.curr
+ yyvsp_offset; if( _temp4665.base == 0? 1:( _temp4667 < _temp4665.base? 1:
_temp4667 >= _temp4665.last_plus_one)){ _throw( Null_Exception);}* _temp4667;});
break; case 174: _LL4664: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp4669=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4669[ 0]=({ struct Cyc_Exp_tok_struct _temp4670; _temp4670.tag= Cyc_Exp_tok_tag;
_temp4670.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp4671=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp4671[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp4672; _temp4672.tag= Cyc_Absyn_UnresolvedMem_e_tag; _temp4672.f1= 0;
_temp4672.f2= 0; _temp4672;}); _temp4671;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4673= yyls; struct Cyc_Yyltype* _temp4675= _temp4673.curr
+( yylsp_offset - 1); if( _temp4673.base == 0? 1:( _temp4675 < _temp4673.base? 1:
_temp4675 >= _temp4673.last_plus_one)){ _throw( Null_Exception);}* _temp4675;}).first_line,({
struct _tagged_ptr3 _temp4676= yyls; struct Cyc_Yyltype* _temp4678= _temp4676.curr
+ yylsp_offset; if( _temp4676.base == 0? 1:( _temp4678 < _temp4676.base? 1:
_temp4678 >= _temp4676.last_plus_one)){ _throw( Null_Exception);}* _temp4678;}).last_line));
_temp4670;}); _temp4669;}); break; case 175: _LL4668: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp4680=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4680[ 0]=({ struct Cyc_Exp_tok_struct
_temp4681; _temp4681.tag= Cyc_Exp_tok_tag; _temp4681.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4682=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4682[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4683; _temp4683.tag= Cyc_Absyn_UnresolvedMem_e_tag;
_temp4683.f1= 0; _temp4683.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp4684= yyvs; struct _xtunion_struct** _temp4686= _temp4684.curr +(
yyvsp_offset - 1); if( _temp4684.base == 0? 1:( _temp4686 < _temp4684.base? 1:
_temp4686 >= _temp4684.last_plus_one)){ _throw( Null_Exception);}* _temp4686;})));
_temp4683;}); _temp4682;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4687= yyls; struct Cyc_Yyltype* _temp4689= _temp4687.curr +( yylsp_offset -
2); if( _temp4687.base == 0? 1:( _temp4689 < _temp4687.base? 1: _temp4689 >=
_temp4687.last_plus_one)){ _throw( Null_Exception);}* _temp4689;}).first_line,({
struct _tagged_ptr3 _temp4690= yyls; struct Cyc_Yyltype* _temp4692= _temp4690.curr
+ yylsp_offset; if( _temp4690.base == 0? 1:( _temp4692 < _temp4690.base? 1:
_temp4692 >= _temp4690.last_plus_one)){ _throw( Null_Exception);}* _temp4692;}).last_line));
_temp4681;}); _temp4680;}); break; case 176: _LL4679: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp4694=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4694[ 0]=({ struct Cyc_Exp_tok_struct
_temp4695; _temp4695.tag= Cyc_Exp_tok_tag; _temp4695.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4696=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4696[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4697; _temp4697.tag= Cyc_Absyn_UnresolvedMem_e_tag;
_temp4697.f1= 0; _temp4697.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp4698= yyvs; struct _xtunion_struct** _temp4700= _temp4698.curr +(
yyvsp_offset - 2); if( _temp4698.base == 0? 1:( _temp4700 < _temp4698.base? 1:
_temp4700 >= _temp4698.last_plus_one)){ _throw( Null_Exception);}* _temp4700;})));
_temp4697;}); _temp4696;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4701= yyls; struct Cyc_Yyltype* _temp4703= _temp4701.curr +( yylsp_offset -
3); if( _temp4701.base == 0? 1:( _temp4703 < _temp4701.base? 1: _temp4703 >=
_temp4701.last_plus_one)){ _throw( Null_Exception);}* _temp4703;}).first_line,({
struct _tagged_ptr3 _temp4704= yyls; struct Cyc_Yyltype* _temp4706= _temp4704.curr
+ yylsp_offset; if( _temp4704.base == 0? 1:( _temp4706 < _temp4704.base? 1:
_temp4706 >= _temp4704.last_plus_one)){ _throw( Null_Exception);}* _temp4706;}).last_line));
_temp4695;}); _temp4694;}); break; case 177: _LL4693: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp4708=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4708[ 0]=({ struct Cyc_Exp_tok_struct
_temp4709; _temp4709.tag= Cyc_Exp_tok_tag; _temp4709.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Comprehension_e_struct* _temp4710=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp4710[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp4711; _temp4711.tag= Cyc_Absyn_Comprehension_e_tag;
_temp4711.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp4712=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp4712->f1=( void*) Cyc_Absyn_Loc_n;
_temp4712->f2=({ struct _tagged_string* _temp4713=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4713[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4714= yyvs; struct _xtunion_struct** _temp4716=
_temp4714.curr +( yyvsp_offset - 5); if( _temp4714.base == 0? 1:( _temp4716 <
_temp4714.base? 1: _temp4716 >= _temp4714.last_plus_one)){ _throw(
Null_Exception);}* _temp4716;})); _temp4713;}); _temp4712;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4717= yyls; struct Cyc_Yyltype* _temp4719= _temp4717.curr
+( yylsp_offset - 5); if( _temp4717.base == 0? 1:( _temp4719 < _temp4717.base? 1:
_temp4719 >= _temp4717.last_plus_one)){ _throw( Null_Exception);}* _temp4719;}).first_line,({
struct _tagged_ptr3 _temp4720= yyls; struct Cyc_Yyltype* _temp4722= _temp4720.curr
+( yylsp_offset - 5); if( _temp4720.base == 0? 1:( _temp4722 < _temp4720.base? 1:
_temp4722 >= _temp4720.last_plus_one)){ _throw( Null_Exception);}* _temp4722;}).last_line)));
_temp4711.f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4723= yyvs; struct
_xtunion_struct** _temp4725= _temp4723.curr +( yyvsp_offset - 3); if( _temp4723.base
== 0? 1:( _temp4725 < _temp4723.base? 1: _temp4725 >= _temp4723.last_plus_one)){
_throw( Null_Exception);}* _temp4725;})); _temp4711.f3= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4726= yyvs; struct _xtunion_struct** _temp4728=
_temp4726.curr +( yyvsp_offset - 1); if( _temp4726.base == 0? 1:( _temp4728 <
_temp4726.base? 1: _temp4728 >= _temp4726.last_plus_one)){ _throw(
Null_Exception);}* _temp4728;})); _temp4711;}); _temp4710;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4729= yyls; struct Cyc_Yyltype* _temp4731= _temp4729.curr
+( yylsp_offset - 7); if( _temp4729.base == 0? 1:( _temp4731 < _temp4729.base? 1:
_temp4731 >= _temp4729.last_plus_one)){ _throw( Null_Exception);}* _temp4731;}).first_line,({
struct _tagged_ptr3 _temp4732= yyls; struct Cyc_Yyltype* _temp4734= _temp4732.curr
+ yylsp_offset; if( _temp4732.base == 0? 1:( _temp4734 < _temp4732.base? 1:
_temp4734 >= _temp4732.last_plus_one)){ _throw( Null_Exception);}* _temp4734;}).last_line));
_temp4709;}); _temp4708;}); break; case 178: _LL4707: yyval=( struct
_xtunion_struct*)({ struct Cyc_InitializerList_tok_struct* _temp4736=( struct
Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp4736[ 0]=({ struct Cyc_InitializerList_tok_struct _temp4737; _temp4737.tag=
Cyc_InitializerList_tok_tag; _temp4737.f1=({ struct Cyc_List_List* _temp4738=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4738->hd=(
void*)({ struct _tuple18* _temp4739=( struct _tuple18*) GC_malloc( sizeof(
struct _tuple18)); _temp4739->f1= 0; _temp4739->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4740= yyvs; struct _xtunion_struct** _temp4742= _temp4740.curr
+ yyvsp_offset; if( _temp4740.base == 0? 1:( _temp4742 < _temp4740.base? 1:
_temp4742 >= _temp4740.last_plus_one)){ _throw( Null_Exception);}* _temp4742;}));
_temp4739;}); _temp4738->tl= 0; _temp4738;}); _temp4737;}); _temp4736;}); break;
case 179: _LL4735: yyval=( struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct*
_temp4744=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4744[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4745; _temp4745.tag= Cyc_InitializerList_tok_tag; _temp4745.f1=({ struct
Cyc_List_List* _temp4746=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4746->hd=( void*)({ struct _tuple18* _temp4747=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4747->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4751= yyvs; struct _xtunion_struct** _temp4753=
_temp4751.curr +( yyvsp_offset - 1); if( _temp4751.base == 0? 1:( _temp4753 <
_temp4751.base? 1: _temp4753 >= _temp4751.last_plus_one)){ _throw(
Null_Exception);}* _temp4753;})); _temp4747->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4748= yyvs; struct _xtunion_struct** _temp4750= _temp4748.curr
+ yyvsp_offset; if( _temp4748.base == 0? 1:( _temp4750 < _temp4748.base? 1:
_temp4750 >= _temp4748.last_plus_one)){ _throw( Null_Exception);}* _temp4750;}));
_temp4747;}); _temp4746->tl= 0; _temp4746;}); _temp4745;}); _temp4744;}); break;
case 180: _LL4743: yyval=( struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct*
_temp4755=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4755[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4756; _temp4756.tag= Cyc_InitializerList_tok_tag; _temp4756.f1=({ struct
Cyc_List_List* _temp4757=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4757->hd=( void*)({ struct _tuple18* _temp4761=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4761->f1= 0; _temp4761->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4762= yyvs; struct _xtunion_struct** _temp4764=
_temp4762.curr + yyvsp_offset; if( _temp4762.base == 0? 1:( _temp4764 <
_temp4762.base? 1: _temp4764 >= _temp4762.last_plus_one)){ _throw(
Null_Exception);}* _temp4764;})); _temp4761;}); _temp4757->tl= Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4758= yyvs; struct _xtunion_struct** _temp4760=
_temp4758.curr +( yyvsp_offset - 2); if( _temp4758.base == 0? 1:( _temp4760 <
_temp4758.base? 1: _temp4760 >= _temp4758.last_plus_one)){ _throw(
Null_Exception);}* _temp4760;})); _temp4757;}); _temp4756;}); _temp4755;});
break; case 181: _LL4754: yyval=( struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct*
_temp4766=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4766[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4767; _temp4767.tag= Cyc_InitializerList_tok_tag; _temp4767.f1=({ struct
Cyc_List_List* _temp4768=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4768->hd=( void*)({ struct _tuple18* _temp4772=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4772->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4776= yyvs; struct _xtunion_struct** _temp4778=
_temp4776.curr +( yyvsp_offset - 1); if( _temp4776.base == 0? 1:( _temp4778 <
_temp4776.base? 1: _temp4778 >= _temp4776.last_plus_one)){ _throw(
Null_Exception);}* _temp4778;})); _temp4772->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4773= yyvs; struct _xtunion_struct** _temp4775= _temp4773.curr
+ yyvsp_offset; if( _temp4773.base == 0? 1:( _temp4775 < _temp4773.base? 1:
_temp4775 >= _temp4773.last_plus_one)){ _throw( Null_Exception);}* _temp4775;}));
_temp4772;}); _temp4768->tl= Cyc_yyget_InitializerList_tok(({ struct
_tagged_ptr2 _temp4769= yyvs; struct _xtunion_struct** _temp4771= _temp4769.curr
+( yyvsp_offset - 3); if( _temp4769.base == 0? 1:( _temp4771 < _temp4769.base? 1:
_temp4771 >= _temp4769.last_plus_one)){ _throw( Null_Exception);}* _temp4771;}));
_temp4768;}); _temp4767;}); _temp4766;}); break; case 182: _LL4765: yyval=(
struct _xtunion_struct*)({ struct Cyc_DesignatorList_tok_struct* _temp4780=(
struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4780[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4781; _temp4781.tag=
Cyc_DesignatorList_tok_tag; _temp4781.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2
_temp4782= yyvs; struct _xtunion_struct** _temp4784= _temp4782.curr +(
yyvsp_offset - 1); if( _temp4782.base == 0? 1:( _temp4784 < _temp4782.base? 1:
_temp4784 >= _temp4782.last_plus_one)){ _throw( Null_Exception);}* _temp4784;})));
_temp4781;}); _temp4780;}); break; case 183: _LL4779: yyval=( struct
_xtunion_struct*)({ struct Cyc_DesignatorList_tok_struct* _temp4786=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp4786[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp4787; _temp4787.tag= Cyc_DesignatorList_tok_tag;
_temp4787.f1=({ struct Cyc_List_List* _temp4788=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4788->hd=( void*) Cyc_yyget_Designator_tok(({
struct _tagged_ptr2 _temp4789= yyvs; struct _xtunion_struct** _temp4791=
_temp4789.curr + yyvsp_offset; if( _temp4789.base == 0? 1:( _temp4791 <
_temp4789.base? 1: _temp4791 >= _temp4789.last_plus_one)){ _throw(
Null_Exception);}* _temp4791;})); _temp4788->tl= 0; _temp4788;}); _temp4787;});
_temp4786;}); break; case 184: _LL4785: yyval=( struct _xtunion_struct*)({
struct Cyc_DesignatorList_tok_struct* _temp4793=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp4793[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp4794; _temp4794.tag= Cyc_DesignatorList_tok_tag;
_temp4794.f1=({ struct Cyc_List_List* _temp4795=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4795->hd=( void*) Cyc_yyget_Designator_tok(({
struct _tagged_ptr2 _temp4799= yyvs; struct _xtunion_struct** _temp4801=
_temp4799.curr + yyvsp_offset; if( _temp4799.base == 0? 1:( _temp4801 <
_temp4799.base? 1: _temp4801 >= _temp4799.last_plus_one)){ _throw(
Null_Exception);}* _temp4801;})); _temp4795->tl= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4796= yyvs; struct _xtunion_struct** _temp4798=
_temp4796.curr +( yyvsp_offset - 1); if( _temp4796.base == 0? 1:( _temp4798 <
_temp4796.base? 1: _temp4798 >= _temp4796.last_plus_one)){ _throw(
Null_Exception);}* _temp4798;})); _temp4795;}); _temp4794;}); _temp4793;});
break; case 185: _LL4792: yyval=( struct _xtunion_struct*)({ struct Cyc_Designator_tok_struct*
_temp4803=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4803[ 0]=({ struct Cyc_Designator_tok_struct _temp4804; _temp4804.tag= Cyc_Designator_tok_tag;
_temp4804.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp4805=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp4805[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp4806; _temp4806.tag=
Cyc_Absyn_ArrayElement_tag; _temp4806.f1= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4807= yyvs; struct _xtunion_struct** _temp4809= _temp4807.curr
+( yyvsp_offset - 1); if( _temp4807.base == 0? 1:( _temp4809 < _temp4807.base? 1:
_temp4809 >= _temp4807.last_plus_one)){ _throw( Null_Exception);}* _temp4809;}));
_temp4806;}); _temp4805;})); _temp4804;}); _temp4803;}); break; case 186:
_LL4802: yyval=( struct _xtunion_struct*)({ struct Cyc_Designator_tok_struct*
_temp4811=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4811[ 0]=({ struct Cyc_Designator_tok_struct _temp4812; _temp4812.tag= Cyc_Designator_tok_tag;
_temp4812.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp4813=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp4813[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp4814; _temp4814.tag= Cyc_Absyn_FieldName_tag;
_temp4814.f1=({ struct _tagged_string* _temp4815=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4815[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4816= yyvs; struct _xtunion_struct** _temp4818=
_temp4816.curr + yyvsp_offset; if( _temp4816.base == 0? 1:( _temp4818 <
_temp4816.base? 1: _temp4818 >= _temp4816.last_plus_one)){ _throw(
Null_Exception);}* _temp4818;})); _temp4815;}); _temp4814;}); _temp4813;}));
_temp4812;}); _temp4811;}); break; case 187: _LL4810: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4820= yyvs; struct
_xtunion_struct** _temp4822= _temp4820.curr + yyvsp_offset; if( _temp4820.base
== 0? 1:( _temp4822 < _temp4820.base? 1: _temp4822 >= _temp4820.last_plus_one)){
_throw( Null_Exception);}* _temp4822;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4823= yyls; struct Cyc_Yyltype* _temp4825= _temp4823.curr
+ yylsp_offset; if( _temp4823.base == 0? 1:( _temp4825 < _temp4823.base? 1:
_temp4825 >= _temp4823.last_plus_one)){ _throw( Null_Exception);}* _temp4825;}).first_line,({
struct _tagged_ptr3 _temp4826= yyls; struct Cyc_Yyltype* _temp4828= _temp4826.curr
+ yylsp_offset; if( _temp4826.base == 0? 1:( _temp4828 < _temp4826.base? 1:
_temp4828 >= _temp4826.last_plus_one)){ _throw( Null_Exception);}* _temp4828;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4829= yyvs; struct _xtunion_struct** _temp4831= _temp4829.curr +
yyvsp_offset; if( _temp4829.base == 0? 1:( _temp4831 < _temp4829.base? 1:
_temp4831 >= _temp4829.last_plus_one)){ _throw( Null_Exception);}* _temp4831;}))).f3;
if( atts != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4832=( char*)"ignoring attributes in type";
struct _tagged_string _temp4833; _temp4833.curr= _temp4832; _temp4833.base=
_temp4832; _temp4833.last_plus_one= _temp4832 + 28; _temp4833;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4834= yyls; struct Cyc_Yyltype* _temp4836= _temp4834.curr
+ yylsp_offset; if( _temp4834.base == 0? 1:( _temp4836 < _temp4834.base? 1:
_temp4836 >= _temp4834.last_plus_one)){ _throw( Null_Exception);}* _temp4836;}).first_line,({
struct _tagged_ptr3 _temp4837= yyls; struct Cyc_Yyltype* _temp4839= _temp4837.curr
+ yylsp_offset; if( _temp4837.base == 0? 1:( _temp4839 < _temp4837.base? 1:
_temp4839 >= _temp4837.last_plus_one)){ _throw( Null_Exception);}* _temp4839;}).last_line));}{
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4840= yyvs; struct _xtunion_struct** _temp4842= _temp4840.curr +
yyvsp_offset; if( _temp4840.base == 0? 1:( _temp4842 < _temp4840.base? 1:
_temp4842 >= _temp4840.last_plus_one)){ _throw( Null_Exception);}* _temp4842;}))).f1;
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4843=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4843[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4844; _temp4844.tag= Cyc_ParamDecl_tok_tag;
_temp4844.f1=({ struct _tuple2* _temp4845=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2) * 1); _temp4845[ 0]=({ struct _tuple2 _temp4846; _temp4846.f1= 0;
_temp4846.f2= tq; _temp4846.f3= t; _temp4846;}); _temp4845;}); _temp4844;});
_temp4843;}); break;}} case 188: _LL4819: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4848= yyvs; struct
_xtunion_struct** _temp4850= _temp4848.curr +( yyvsp_offset - 1); if( _temp4848.base
== 0? 1:( _temp4850 < _temp4848.base? 1: _temp4850 >= _temp4848.last_plus_one)){
_throw( Null_Exception);}* _temp4850;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4851= yyls; struct Cyc_Yyltype* _temp4853= _temp4851.curr
+( yylsp_offset - 1); if( _temp4851.base == 0? 1:( _temp4853 < _temp4851.base? 1:
_temp4853 >= _temp4851.last_plus_one)){ _throw( Null_Exception);}* _temp4853;}).first_line,({
struct _tagged_ptr3 _temp4854= yyls; struct Cyc_Yyltype* _temp4856= _temp4854.curr
+( yylsp_offset - 1); if( _temp4854.base == 0? 1:( _temp4856 < _temp4854.base? 1:
_temp4856 >= _temp4854.last_plus_one)){ _throw( Null_Exception);}* _temp4856;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4857= yyvs; struct _xtunion_struct** _temp4859= _temp4857.curr +(
yyvsp_offset - 1); if( _temp4857.base == 0? 1:( _temp4859 < _temp4857.base? 1:
_temp4859 >= _temp4857.last_plus_one)){ _throw( Null_Exception);}* _temp4859;}))).f3;
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4860= yyvs; struct _xtunion_struct** _temp4862= _temp4860.curr +(
yyvsp_offset - 1); if( _temp4860.base == 0? 1:( _temp4862 < _temp4860.base? 1:
_temp4862 >= _temp4860.last_plus_one)){ _throw( Null_Exception);}* _temp4862;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4863= yyvs; struct _xtunion_struct** _temp4865= _temp4863.curr
+ yyvsp_offset; if( _temp4863.base == 0? 1:( _temp4865 < _temp4863.base? 1:
_temp4865 >= _temp4863.last_plus_one)){ _throw( Null_Exception);}* _temp4865;})))->tms;
struct _tuple6 t_info= Cyc_Parse_apply_tms( tq, t, atts, tms); if( t_info.f3 !=
0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4866=( char*)"bad type params, ignoring";
struct _tagged_string _temp4867; _temp4867.curr= _temp4866; _temp4867.base=
_temp4866; _temp4867.last_plus_one= _temp4866 + 26; _temp4867;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4868= yyls; struct Cyc_Yyltype* _temp4870= _temp4868.curr
+ yylsp_offset; if( _temp4868.base == 0? 1:( _temp4870 < _temp4868.base? 1:
_temp4870 >= _temp4868.last_plus_one)){ _throw( Null_Exception);}* _temp4870;}).first_line,({
struct _tagged_ptr3 _temp4871= yyls; struct Cyc_Yyltype* _temp4873= _temp4871.curr
+ yylsp_offset; if( _temp4871.base == 0? 1:( _temp4873 < _temp4871.base? 1:
_temp4873 >= _temp4871.last_plus_one)){ _throw( Null_Exception);}* _temp4873;}).last_line));}
if( t_info.f4 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4874=(
char*)"bad specifiers, ignoring"; struct _tagged_string _temp4875; _temp4875.curr=
_temp4874; _temp4875.base= _temp4874; _temp4875.last_plus_one= _temp4874 + 25;
_temp4875;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4876= yyls;
struct Cyc_Yyltype* _temp4878= _temp4876.curr + yylsp_offset; if( _temp4876.base
== 0? 1:( _temp4878 < _temp4876.base? 1: _temp4878 >= _temp4876.last_plus_one)){
_throw( Null_Exception);}* _temp4878;}).first_line,({ struct _tagged_ptr3
_temp4879= yyls; struct Cyc_Yyltype* _temp4881= _temp4879.curr + yylsp_offset;
if( _temp4879.base == 0? 1:( _temp4881 < _temp4879.base? 1: _temp4881 >=
_temp4879.last_plus_one)){ _throw( Null_Exception);}* _temp4881;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4882=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4882[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4883; _temp4883.tag= Cyc_ParamDecl_tok_tag;
_temp4883.f1=({ struct _tuple2* _temp4884=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4884->f1= 0; _temp4884->f2= t_info.f1; _temp4884->f3=
t_info.f2; _temp4884;}); _temp4883;}); _temp4882;}); break;} case 189: _LL4847:
yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct* _temp4886=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4886[ 0]=({ struct Cyc_Type_tok_struct _temp4887; _temp4887.tag= Cyc_Type_tok_tag;
_temp4887.f1=( void*)(* Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4888=
yyvs; struct _xtunion_struct** _temp4890= _temp4888.curr + yyvsp_offset; if(
_temp4888.base == 0? 1:( _temp4890 < _temp4888.base? 1: _temp4890 >= _temp4888.last_plus_one)){
_throw( Null_Exception);}* _temp4890;}))).f3; _temp4887;}); _temp4886;}); break;
case 190: _LL4885: yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct*
_temp4892=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4892[ 0]=({ struct Cyc_Type_tok_struct _temp4893; _temp4893.tag= Cyc_Type_tok_tag;
_temp4893.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4894=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4894[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4895; _temp4895.tag= Cyc_Absyn_JoinEff_tag;
_temp4895.f1= 0; _temp4895;}); _temp4894;})); _temp4893;}); _temp4892;}); break;
case 191: _LL4891: yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct*
_temp4897=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4897[ 0]=({ struct Cyc_Type_tok_struct _temp4898; _temp4898.tag= Cyc_Type_tok_tag;
_temp4898.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4899=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4899[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4900; _temp4900.tag= Cyc_Absyn_JoinEff_tag;
_temp4900.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4901= yyvs;
struct _xtunion_struct** _temp4903= _temp4901.curr +( yyvsp_offset - 1); if(
_temp4901.base == 0? 1:( _temp4903 < _temp4901.base? 1: _temp4903 >= _temp4901.last_plus_one)){
_throw( Null_Exception);}* _temp4903;})); _temp4900;}); _temp4899;})); _temp4898;});
_temp4897;}); break; case 192: _LL4896: yyval=( struct _xtunion_struct*)({
struct Cyc_Type_tok_struct* _temp4905=( struct Cyc_Type_tok_struct*) GC_malloc(
sizeof( struct Cyc_Type_tok_struct)); _temp4905[ 0]=({ struct Cyc_Type_tok_struct
_temp4906; _temp4906.tag= Cyc_Type_tok_tag; _temp4906.f1=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp4907=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4907[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4908; _temp4908.tag= Cyc_Absyn_JoinEff_tag;
_temp4908.f1=({ struct Cyc_List_List* _temp4909=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4909->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4913= yyvs; struct _xtunion_struct** _temp4915=
_temp4913.curr +( yyvsp_offset - 2); if( _temp4913.base == 0? 1:( _temp4915 <
_temp4913.base? 1: _temp4915 >= _temp4913.last_plus_one)){ _throw(
Null_Exception);}* _temp4915;})); _temp4909->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4910= yyvs; struct _xtunion_struct** _temp4912=
_temp4910.curr + yyvsp_offset; if( _temp4910.base == 0? 1:( _temp4912 <
_temp4910.base? 1: _temp4912 >= _temp4910.last_plus_one)){ _throw(
Null_Exception);}* _temp4912;})); _temp4909;}); _temp4908;}); _temp4907;}));
_temp4906;}); _temp4905;}); break; case 193: _LL4904: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4917=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4917[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4918; _temp4918.tag= Cyc_TypeList_tok_tag; _temp4918.f1=({ struct Cyc_List_List*
_temp4919=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4919->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4920= yyvs;
struct _xtunion_struct** _temp4922= _temp4920.curr + yyvsp_offset; if( _temp4920.base
== 0? 1:( _temp4922 < _temp4920.base? 1: _temp4922 >= _temp4920.last_plus_one)){
_throw( Null_Exception);}* _temp4922;})); _temp4919->tl= 0; _temp4919;});
_temp4918;}); _temp4917;}); break; case 194: _LL4916: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4924=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4924[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4925; _temp4925.tag= Cyc_TypeList_tok_tag; _temp4925.f1=({ struct Cyc_List_List*
_temp4926=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4926->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4930= yyvs;
struct _xtunion_struct** _temp4932= _temp4930.curr + yyvsp_offset; if( _temp4930.base
== 0? 1:( _temp4932 < _temp4930.base? 1: _temp4932 >= _temp4930.last_plus_one)){
_throw( Null_Exception);}* _temp4932;})); _temp4926->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4927= yyvs; struct _xtunion_struct** _temp4929=
_temp4927.curr +( yyvsp_offset - 2); if( _temp4927.base == 0? 1:( _temp4929 <
_temp4927.base? 1: _temp4929 >= _temp4927.last_plus_one)){ _throw(
Null_Exception);}* _temp4929;})); _temp4926;}); _temp4925;}); _temp4924;});
break; case 195: _LL4923: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4934=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4934[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4935; _temp4935.tag= Cyc_AbstractDeclarator_tok_tag; _temp4935.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4936=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4936->tms= Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp4937= yyvs; struct _xtunion_struct** _temp4939=
_temp4937.curr + yyvsp_offset; if( _temp4937.base == 0? 1:( _temp4939 <
_temp4937.base? 1: _temp4939 >= _temp4937.last_plus_one)){ _throw(
Null_Exception);}* _temp4939;})); _temp4936;}); _temp4935;}); _temp4934;});
break; case 196: _LL4933: yyval=({ struct _tagged_ptr2 _temp4941= yyvs; struct
_xtunion_struct** _temp4943= _temp4941.curr + yyvsp_offset; if( _temp4941.base
== 0? 1:( _temp4943 < _temp4941.base? 1: _temp4943 >= _temp4941.last_plus_one)){
_throw( Null_Exception);}* _temp4943;}); break; case 197: _LL4940: yyval=(
struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4945=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4945[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4946; _temp4946.tag=
Cyc_AbstractDeclarator_tok_tag; _temp4946.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4947=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4947->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2
_temp4948= yyvs; struct _xtunion_struct** _temp4950= _temp4948.curr +(
yyvsp_offset - 1); if( _temp4948.base == 0? 1:( _temp4950 < _temp4948.base? 1:
_temp4950 >= _temp4948.last_plus_one)){ _throw( Null_Exception);}* _temp4950;})),(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4951= yyvs; struct
_xtunion_struct** _temp4953= _temp4951.curr + yyvsp_offset; if( _temp4951.base
== 0? 1:( _temp4953 < _temp4951.base? 1: _temp4953 >= _temp4951.last_plus_one)){
_throw( Null_Exception);}* _temp4953;})))->tms); _temp4947;}); _temp4946;});
_temp4945;}); break; case 198: _LL4944: yyval=({ struct _tagged_ptr2 _temp4955=
yyvs; struct _xtunion_struct** _temp4957= _temp4955.curr +( yyvsp_offset - 1);
if( _temp4955.base == 0? 1:( _temp4957 < _temp4955.base? 1: _temp4957 >=
_temp4955.last_plus_one)){ _throw( Null_Exception);}* _temp4957;}); break; case
199: _LL4954: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4959=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4959[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4960; _temp4960.tag= Cyc_AbstractDeclarator_tok_tag; _temp4960.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4961=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4961->tms=({
struct Cyc_List_List* _temp4962=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4962->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4962->tl= 0; _temp4962;}); _temp4961;}); _temp4960;}); _temp4959;}); break;
case 200: _LL4958: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4964=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4964[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4965; _temp4965.tag= Cyc_AbstractDeclarator_tok_tag; _temp4965.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4966=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4966->tms=({
struct Cyc_List_List* _temp4967=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4967->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4967->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4968= yyvs; struct _xtunion_struct** _temp4970= _temp4968.curr +(
yyvsp_offset - 2); if( _temp4968.base == 0? 1:( _temp4970 < _temp4968.base? 1:
_temp4970 >= _temp4968.last_plus_one)){ _throw( Null_Exception);}* _temp4970;})))->tms;
_temp4967;}); _temp4966;}); _temp4965;}); _temp4964;}); break; case 201: _LL4963:
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4972=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4972[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4973; _temp4973.tag= Cyc_AbstractDeclarator_tok_tag; _temp4973.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4974=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4974->tms=({
struct Cyc_List_List* _temp4975=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4975->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4976=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4976[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp4977; _temp4977.tag= Cyc_Absyn_ConstArray_mod_tag; _temp4977.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4978= yyvs; struct _xtunion_struct** _temp4980=
_temp4978.curr +( yyvsp_offset - 1); if( _temp4978.base == 0? 1:( _temp4980 <
_temp4978.base? 1: _temp4980 >= _temp4978.last_plus_one)){ _throw(
Null_Exception);}* _temp4980;})); _temp4977;}); _temp4976;})); _temp4975->tl= 0;
_temp4975;}); _temp4974;}); _temp4973;}); _temp4972;}); break; case 202: _LL4971:
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4982=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4982[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4983; _temp4983.tag= Cyc_AbstractDeclarator_tok_tag; _temp4983.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4984=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4984->tms=({
struct Cyc_List_List* _temp4985=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4985->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4989=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4989[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp4990; _temp4990.tag= Cyc_Absyn_ConstArray_mod_tag; _temp4990.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4991= yyvs; struct _xtunion_struct** _temp4993=
_temp4991.curr +( yyvsp_offset - 1); if( _temp4991.base == 0? 1:( _temp4993 <
_temp4991.base? 1: _temp4993 >= _temp4991.last_plus_one)){ _throw(
Null_Exception);}* _temp4993;})); _temp4990;}); _temp4989;})); _temp4985->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4986= yyvs; struct
_xtunion_struct** _temp4988= _temp4986.curr +( yyvsp_offset - 3); if( _temp4986.base
== 0? 1:( _temp4988 < _temp4986.base? 1: _temp4988 >= _temp4986.last_plus_one)){
_throw( Null_Exception);}* _temp4988;})))->tms; _temp4985;}); _temp4984;});
_temp4983;}); _temp4982;}); break; case 203: _LL4981: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4995=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4995[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4996; _temp4996.tag=
Cyc_AbstractDeclarator_tok_tag; _temp4996.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4997=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4997->tms=({ struct Cyc_List_List* _temp4998=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4998->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4999=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4999[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5000; _temp5000.tag= Cyc_Absyn_Function_mod_tag;
_temp5000.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5001=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5001[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5002; _temp5002.tag= Cyc_Absyn_WithTypes_tag;
_temp5002.f1= 0; _temp5002.f2= 0; _temp5002.f3= 0; _temp5002;}); _temp5001;}));
_temp5000;}); _temp4999;})); _temp4998->tl= 0; _temp4998;}); _temp4997;});
_temp4996;}); _temp4995;}); break; case 204: _LL4994: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5004=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5004[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5005; _temp5005.tag=
Cyc_AbstractDeclarator_tok_tag; _temp5005.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5006=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5006->tms=({ struct Cyc_List_List* _temp5007=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5007->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5008=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5008[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5009; _temp5009.tag= Cyc_Absyn_Function_mod_tag;
_temp5009.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5010=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5010[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5011; _temp5011.tag= Cyc_Absyn_WithTypes_tag;
_temp5011.f1= 0; _temp5011.f2= 0; _temp5011.f3=({ struct Cyc_Core_Opt* _temp5012=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5012->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp5013=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp5013[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp5014; _temp5014.tag= Cyc_Absyn_JoinEff_tag;
_temp5014.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5015= yyvs;
struct _xtunion_struct** _temp5017= _temp5015.curr +( yyvsp_offset - 1); if(
_temp5015.base == 0? 1:( _temp5017 < _temp5015.base? 1: _temp5017 >= _temp5015.last_plus_one)){
_throw( Null_Exception);}* _temp5017;})); _temp5014;}); _temp5013;})); _temp5012;});
_temp5011;}); _temp5010;})); _temp5009;}); _temp5008;})); _temp5007->tl= 0;
_temp5007;}); _temp5006;}); _temp5005;}); _temp5004;}); break; case 205: _LL5003: {
struct _tuple16 _temp5024; struct Cyc_Core_Opt* _temp5025; int _temp5027; struct
Cyc_List_List* _temp5029; struct _tuple16* _temp5022= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp5019= yyvs; struct _xtunion_struct** _temp5021=
_temp5019.curr +( yyvsp_offset - 1); if( _temp5019.base == 0? 1:( _temp5021 <
_temp5019.base? 1: _temp5021 >= _temp5019.last_plus_one)){ _throw(
Null_Exception);}* _temp5021;})); _temp5024=* _temp5022; _LL5030: _temp5029=
_temp5024.f1; goto _LL5028; _LL5028: _temp5027= _temp5024.f2; goto _LL5026;
_LL5026: _temp5025= _temp5024.f3; goto _LL5023; _LL5023: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5031=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5031[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5032; _temp5032.tag=
Cyc_AbstractDeclarator_tok_tag; _temp5032.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5033=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5033->tms=({ struct Cyc_List_List* _temp5034=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5034->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5035=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5035[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5036; _temp5036.tag= Cyc_Absyn_Function_mod_tag;
_temp5036.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5037=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5037[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5038; _temp5038.tag= Cyc_Absyn_WithTypes_tag;
_temp5038.f1= _temp5029; _temp5038.f2= _temp5027; _temp5038.f3= _temp5025;
_temp5038;}); _temp5037;})); _temp5036;}); _temp5035;})); _temp5034->tl= 0;
_temp5034;}); _temp5033;}); _temp5032;}); _temp5031;}); break;} case 206:
_LL5018: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5040=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5040[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5041; _temp5041.tag= Cyc_AbstractDeclarator_tok_tag; _temp5041.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp5042=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5042->tms=({
struct Cyc_List_List* _temp5043=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5043->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp5047=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp5047[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp5048; _temp5048.tag=
Cyc_Absyn_Function_mod_tag; _temp5048.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp5049=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5049[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5050; _temp5050.tag= Cyc_Absyn_WithTypes_tag;
_temp5050.f1= 0; _temp5050.f2= 0; _temp5050.f3= 0; _temp5050;}); _temp5049;}));
_temp5048;}); _temp5047;})); _temp5043->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5044= yyvs; struct _xtunion_struct** _temp5046=
_temp5044.curr +( yyvsp_offset - 2); if( _temp5044.base == 0? 1:( _temp5046 <
_temp5044.base? 1: _temp5046 >= _temp5044.last_plus_one)){ _throw(
Null_Exception);}* _temp5046;})))->tms; _temp5043;}); _temp5042;}); _temp5041;});
_temp5040;}); break; case 207: _LL5039: { struct Cyc_Core_Opt* eff=({ struct Cyc_Core_Opt*
_temp5052=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5052->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp5053=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp5053[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp5054; _temp5054.tag= Cyc_Absyn_JoinEff_tag;
_temp5054.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5055= yyvs;
struct _xtunion_struct** _temp5057= _temp5055.curr +( yyvsp_offset - 1); if(
_temp5055.base == 0? 1:( _temp5057 < _temp5055.base? 1: _temp5057 >= _temp5055.last_plus_one)){
_throw( Null_Exception);}* _temp5057;})); _temp5054;}); _temp5053;})); _temp5052;});
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5058=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5058[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5059; _temp5059.tag= Cyc_AbstractDeclarator_tok_tag; _temp5059.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp5060=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5060->tms=({
struct Cyc_List_List* _temp5061=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5061->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp5065=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp5065[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp5066; _temp5066.tag=
Cyc_Absyn_Function_mod_tag; _temp5066.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp5067=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5067[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5068; _temp5068.tag= Cyc_Absyn_WithTypes_tag;
_temp5068.f1= 0; _temp5068.f2= 0; _temp5068.f3= eff; _temp5068;}); _temp5067;}));
_temp5066;}); _temp5065;})); _temp5061->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5062= yyvs; struct _xtunion_struct** _temp5064=
_temp5062.curr +( yyvsp_offset - 4); if( _temp5062.base == 0? 1:( _temp5064 <
_temp5062.base? 1: _temp5064 >= _temp5062.last_plus_one)){ _throw(
Null_Exception);}* _temp5064;})))->tms; _temp5061;}); _temp5060;}); _temp5059;});
_temp5058;}); break;} case 208: _LL5051: { struct _tuple16 _temp5075; struct Cyc_Core_Opt*
_temp5076; int _temp5078; struct Cyc_List_List* _temp5080; struct _tuple16*
_temp5073= Cyc_yyget_ParamDeclListBool_tok(({ struct _tagged_ptr2 _temp5070=
yyvs; struct _xtunion_struct** _temp5072= _temp5070.curr +( yyvsp_offset - 1);
if( _temp5070.base == 0? 1:( _temp5072 < _temp5070.base? 1: _temp5072 >=
_temp5070.last_plus_one)){ _throw( Null_Exception);}* _temp5072;})); _temp5075=*
_temp5073; _LL5081: _temp5080= _temp5075.f1; goto _LL5079; _LL5079: _temp5078=
_temp5075.f2; goto _LL5077; _LL5077: _temp5076= _temp5075.f3; goto _LL5074;
_LL5074: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5082=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5082[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5083; _temp5083.tag= Cyc_AbstractDeclarator_tok_tag; _temp5083.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp5084=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5084->tms=({
struct Cyc_List_List* _temp5085=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5085->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp5089=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp5089[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp5090; _temp5090.tag=
Cyc_Absyn_Function_mod_tag; _temp5090.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp5091=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5091[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5092; _temp5092.tag= Cyc_Absyn_WithTypes_tag;
_temp5092.f1= _temp5080; _temp5092.f2= _temp5078; _temp5092.f3= _temp5076;
_temp5092;}); _temp5091;})); _temp5090;}); _temp5089;})); _temp5085->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5086= yyvs; struct _xtunion_struct** _temp5088=
_temp5086.curr +( yyvsp_offset - 3); if( _temp5086.base == 0? 1:( _temp5088 <
_temp5086.base? 1: _temp5088 >= _temp5086.last_plus_one)){ _throw(
Null_Exception);}* _temp5088;})))->tms; _temp5085;}); _temp5084;}); _temp5083;});
_temp5082;}); break;} case 209: _LL5069: { struct Cyc_List_List* ts=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5094=
yyls; struct Cyc_Yyltype* _temp5096= _temp5094.curr +( yylsp_offset - 2); if(
_temp5094.base == 0? 1:( _temp5096 < _temp5094.base? 1: _temp5096 >= _temp5094.last_plus_one)){
_throw( Null_Exception);}* _temp5096;}).first_line,({ struct _tagged_ptr3
_temp5097= yyls; struct Cyc_Yyltype* _temp5099= _temp5097.curr + yylsp_offset;
if( _temp5097.base == 0? 1:( _temp5099 < _temp5097.base? 1: _temp5099 >=
_temp5097.last_plus_one)){ _throw( Null_Exception);}* _temp5099;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5100= yyvs; struct _xtunion_struct** _temp5102=
_temp5100.curr +( yyvsp_offset - 1); if( _temp5100.base == 0? 1:( _temp5102 <
_temp5100.base? 1: _temp5102 >= _temp5100.last_plus_one)){ _throw(
Null_Exception);}* _temp5102;})))); yyval=( struct _xtunion_struct*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp5103=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5103[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5104; _temp5104.tag= Cyc_AbstractDeclarator_tok_tag;
_temp5104.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5105=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5105->tms=({
struct Cyc_List_List* _temp5106=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5106->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp5110=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp5110[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp5111; _temp5111.tag= Cyc_Absyn_TypeParams_mod_tag; _temp5111.f1= ts;
_temp5111.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5112= yyls;
struct Cyc_Yyltype* _temp5114= _temp5112.curr +( yylsp_offset - 2); if(
_temp5112.base == 0? 1:( _temp5114 < _temp5112.base? 1: _temp5114 >= _temp5112.last_plus_one)){
_throw( Null_Exception);}* _temp5114;}).first_line,({ struct _tagged_ptr3
_temp5115= yyls; struct Cyc_Yyltype* _temp5117= _temp5115.curr + yylsp_offset;
if( _temp5115.base == 0? 1:( _temp5117 < _temp5115.base? 1: _temp5117 >=
_temp5115.last_plus_one)){ _throw( Null_Exception);}* _temp5117;}).last_line);
_temp5111.f3= 0; _temp5111;}); _temp5110;})); _temp5106->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5107= yyvs; struct _xtunion_struct** _temp5109=
_temp5107.curr +( yyvsp_offset - 3); if( _temp5107.base == 0? 1:( _temp5109 <
_temp5107.base? 1: _temp5109 >= _temp5107.last_plus_one)){ _throw(
Null_Exception);}* _temp5109;})))->tms; _temp5106;}); _temp5105;}); _temp5104;});
_temp5103;}); break;} case 210: _LL5093:(( struct Cyc_Lexing_lexbuf*)({ struct
Cyc_Core_Opt* _temp5119= Cyc_Parse_lbuf; if( _temp5119 == 0){ _throw(
Null_Exception);} _temp5119->v;}))->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5120=
yyls; struct Cyc_Yyltype* _temp5122= _temp5120.curr +( yylsp_offset - 2); if(
_temp5120.base == 0? 1:( _temp5122 < _temp5120.base? 1: _temp5122 >= _temp5120.last_plus_one)){
_throw( Null_Exception);}* _temp5122;}).first_line,({ struct _tagged_ptr3
_temp5123= yyls; struct Cyc_Yyltype* _temp5125= _temp5123.curr + yylsp_offset;
if( _temp5123.base == 0? 1:( _temp5125 < _temp5123.base? 1: _temp5125 >=
_temp5123.last_plus_one)){ _throw( Null_Exception);}* _temp5125;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5126= yyvs; struct _xtunion_struct** _temp5128=
_temp5126.curr +( yyvsp_offset - 1); if( _temp5126.base == 0? 1:( _temp5128 <
_temp5126.base? 1: _temp5128 >= _temp5126.last_plus_one)){ _throw(
Null_Exception);}* _temp5128;})))); yyval=( struct _xtunion_struct*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp5129=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5129[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5130; _temp5130.tag= Cyc_AbstractDeclarator_tok_tag;
_temp5130.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5131=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5131->tms=({
struct Cyc_List_List* _temp5132=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5132->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp5136=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp5136[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp5137; _temp5137.tag= Cyc_Absyn_TypeParams_mod_tag; _temp5137.f1= ts;
_temp5137.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5138= yyls;
struct Cyc_Yyltype* _temp5140= _temp5138.curr +( yylsp_offset - 2); if(
_temp5138.base == 0? 1:( _temp5140 < _temp5138.base? 1: _temp5140 >= _temp5138.last_plus_one)){
_throw( Null_Exception);}* _temp5140;}).first_line,({ struct _tagged_ptr3
_temp5141= yyls; struct Cyc_Yyltype* _temp5143= _temp5141.curr + yylsp_offset;
if( _temp5141.base == 0? 1:( _temp5143 < _temp5141.base? 1: _temp5143 >=
_temp5141.last_plus_one)){ _throw( Null_Exception);}* _temp5143;}).last_line);
_temp5137.f3= 0; _temp5137;}); _temp5136;})); _temp5132->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5133= yyvs; struct _xtunion_struct** _temp5135=
_temp5133.curr +( yyvsp_offset - 3); if( _temp5133.base == 0? 1:( _temp5135 <
_temp5133.base? 1: _temp5135 >= _temp5133.last_plus_one)){ _throw(
Null_Exception);}* _temp5135;})))->tms; _temp5132;}); _temp5131;}); _temp5130;});
_temp5129;}); break;} case 211: _LL5118: yyval=( struct _xtunion_struct*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp5145=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5145[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5146; _temp5146.tag= Cyc_AbstractDeclarator_tok_tag;
_temp5146.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5147=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5147->tms=({
struct Cyc_List_List* _temp5148=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5148->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp5152=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp5152[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp5153; _temp5153.tag= Cyc_Absyn_Attributes_mod_tag; _temp5153.f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5154= yyls; struct Cyc_Yyltype* _temp5156= _temp5154.curr
+ yylsp_offset; if( _temp5154.base == 0? 1:( _temp5156 < _temp5154.base? 1:
_temp5156 >= _temp5154.last_plus_one)){ _throw( Null_Exception);}* _temp5156;}).first_line,({
struct _tagged_ptr3 _temp5157= yyls; struct Cyc_Yyltype* _temp5159= _temp5157.curr
+ yylsp_offset; if( _temp5157.base == 0? 1:( _temp5159 < _temp5157.base? 1:
_temp5159 >= _temp5157.last_plus_one)){ _throw( Null_Exception);}* _temp5159;}).last_line);
_temp5153.f2= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp5160= yyvs;
struct _xtunion_struct** _temp5162= _temp5160.curr + yyvsp_offset; if( _temp5160.base
== 0? 1:( _temp5162 < _temp5160.base? 1: _temp5162 >= _temp5160.last_plus_one)){
_throw( Null_Exception);}* _temp5162;})); _temp5153;}); _temp5152;})); _temp5148->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp5149= yyvs; struct
_xtunion_struct** _temp5151= _temp5149.curr +( yyvsp_offset - 1); if( _temp5149.base
== 0? 1:( _temp5151 < _temp5149.base? 1: _temp5151 >= _temp5149.last_plus_one)){
_throw( Null_Exception);}* _temp5151;})))->tms; _temp5148;}); _temp5147;});
_temp5146;}); _temp5145;}); break; case 212: _LL5144: yyval=({ struct
_tagged_ptr2 _temp5164= yyvs; struct _xtunion_struct** _temp5166= _temp5164.curr
+ yyvsp_offset; if( _temp5164.base == 0? 1:( _temp5166 < _temp5164.base? 1:
_temp5166 >= _temp5164.last_plus_one)){ _throw( Null_Exception);}* _temp5166;});
break; case 213: _LL5163: yyval=({ struct _tagged_ptr2 _temp5168= yyvs; struct
_xtunion_struct** _temp5170= _temp5168.curr + yyvsp_offset; if( _temp5168.base
== 0? 1:( _temp5170 < _temp5168.base? 1: _temp5170 >= _temp5168.last_plus_one)){
_throw( Null_Exception);}* _temp5170;}); break; case 214: _LL5167: yyval=({
struct _tagged_ptr2 _temp5172= yyvs; struct _xtunion_struct** _temp5174=
_temp5172.curr + yyvsp_offset; if( _temp5172.base == 0? 1:( _temp5174 <
_temp5172.base? 1: _temp5174 >= _temp5172.last_plus_one)){ _throw(
Null_Exception);}* _temp5174;}); break; case 215: _LL5171: yyval=({ struct
_tagged_ptr2 _temp5176= yyvs; struct _xtunion_struct** _temp5178= _temp5176.curr
+ yyvsp_offset; if( _temp5176.base == 0? 1:( _temp5178 < _temp5176.base? 1:
_temp5178 >= _temp5176.last_plus_one)){ _throw( Null_Exception);}* _temp5178;});
break; case 216: _LL5175: yyval=({ struct _tagged_ptr2 _temp5180= yyvs; struct
_xtunion_struct** _temp5182= _temp5180.curr + yyvsp_offset; if( _temp5180.base
== 0? 1:( _temp5182 < _temp5180.base? 1: _temp5182 >= _temp5180.last_plus_one)){
_throw( Null_Exception);}* _temp5182;}); break; case 217: _LL5179: yyval=({
struct _tagged_ptr2 _temp5184= yyvs; struct _xtunion_struct** _temp5186=
_temp5184.curr + yyvsp_offset; if( _temp5184.base == 0? 1:( _temp5186 <
_temp5184.base? 1: _temp5186 >= _temp5184.last_plus_one)){ _throw(
Null_Exception);}* _temp5186;}); break; case 218: _LL5183: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5188= yyvs; struct
_xtunion_struct** _temp5190= _temp5188.curr +( yyvsp_offset - 3); if( _temp5188.base
== 0? 1:( _temp5190 < _temp5188.base? 1: _temp5190 >= _temp5188.last_plus_one)){
_throw( Null_Exception);}* _temp5190;})),( struct _tagged_string)({ char*
_temp5191=( char*)"`H"; struct _tagged_string _temp5192; _temp5192.curr=
_temp5191; _temp5192.base= _temp5191; _temp5192.last_plus_one= _temp5191 + 3;
_temp5192;})) == 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp5193=(
char*)"bad occurrence of heap region `H"; struct _tagged_string _temp5194;
_temp5194.curr= _temp5193; _temp5194.base= _temp5193; _temp5194.last_plus_one=
_temp5193 + 33; _temp5194;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5195= yyls; struct Cyc_Yyltype* _temp5197= _temp5195.curr +( yylsp_offset -
3); if( _temp5195.base == 0? 1:( _temp5197 < _temp5195.base? 1: _temp5197 >=
_temp5195.last_plus_one)){ _throw( Null_Exception);}* _temp5197;}).first_line,({
struct _tagged_ptr3 _temp5198= yyls; struct Cyc_Yyltype* _temp5200= _temp5198.curr
+( yylsp_offset - 3); if( _temp5198.base == 0? 1:( _temp5200 < _temp5198.base? 1:
_temp5200 >= _temp5198.last_plus_one)){ _throw( Null_Exception);}* _temp5200;}).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp5201=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp5201->name=({ struct
_tagged_string* _temp5202=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp5202[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp5203= yyvs; struct _xtunion_struct** _temp5205= _temp5203.curr +(
yyvsp_offset - 3); if( _temp5203.base == 0? 1:( _temp5205 < _temp5203.base? 1:
_temp5205 >= _temp5203.last_plus_one)){ _throw( Null_Exception);}* _temp5205;}));
_temp5202;}); _temp5201->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind); _temp5201;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp5206=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp5206[ 0]=({ struct Cyc_Absyn_VarType_struct _temp5207; _temp5207.tag= Cyc_Absyn_VarType_tag;
_temp5207.f1= tv; _temp5207;}); _temp5206;}); yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5208=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5208[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5209; _temp5209.tag= Cyc_Stmt_tok_tag; _temp5209.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Region_s_struct* _temp5210=( struct Cyc_Absyn_Region_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct)); _temp5210[ 0]=({ struct
Cyc_Absyn_Region_s_struct _temp5211; _temp5211.tag= Cyc_Absyn_Region_s_tag;
_temp5211.f1= tv; _temp5211.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp5212=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp5212->f1=(
void*) Cyc_Absyn_Loc_n; _temp5212->f2=({ struct _tagged_string* _temp5213=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp5213[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5214= yyvs; struct
_xtunion_struct** _temp5216= _temp5214.curr +( yyvsp_offset - 1); if( _temp5214.base
== 0? 1:( _temp5216 < _temp5214.base? 1: _temp5216 >= _temp5214.last_plus_one)){
_throw( Null_Exception);}* _temp5216;})); _temp5213;}); _temp5212;}),( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp5217=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp5217[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp5218; _temp5218.tag= Cyc_Absyn_RgnHandleType_tag;
_temp5218.f1=( void*) t; _temp5218;}); _temp5217;}), 0); _temp5211.f3= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5219= yyvs; struct _xtunion_struct** _temp5221=
_temp5219.curr + yyvsp_offset; if( _temp5219.base == 0? 1:( _temp5221 <
_temp5219.base? 1: _temp5221 >= _temp5219.last_plus_one)){ _throw(
Null_Exception);}* _temp5221;})); _temp5211;}); _temp5210;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5222= yyls; struct Cyc_Yyltype* _temp5224= _temp5222.curr
+( yylsp_offset - 5); if( _temp5222.base == 0? 1:( _temp5224 < _temp5222.base? 1:
_temp5224 >= _temp5222.last_plus_one)){ _throw( Null_Exception);}* _temp5224;}).first_line,({
struct _tagged_ptr3 _temp5225= yyls; struct Cyc_Yyltype* _temp5227= _temp5225.curr
+ yylsp_offset; if( _temp5225.base == 0? 1:( _temp5227 < _temp5225.base? 1:
_temp5227 >= _temp5225.last_plus_one)){ _throw( Null_Exception);}* _temp5227;}).last_line));
_temp5209;}); _temp5208;}); break;} case 219: _LL5187: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5229= yyvs; struct
_xtunion_struct** _temp5231= _temp5229.curr +( yyvsp_offset - 1); if( _temp5229.base
== 0? 1:( _temp5231 < _temp5229.base? 1: _temp5231 >= _temp5229.last_plus_one)){
_throw( Null_Exception);}* _temp5231;})),( struct _tagged_string)({ char*
_temp5232=( char*)"H"; struct _tagged_string _temp5233; _temp5233.curr=
_temp5232; _temp5233.base= _temp5232; _temp5233.last_plus_one= _temp5232 + 2;
_temp5233;})) == 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp5234=(
char*)"bad occurrence of heap region `H"; struct _tagged_string _temp5235;
_temp5235.curr= _temp5234; _temp5235.base= _temp5234; _temp5235.last_plus_one=
_temp5234 + 33; _temp5235;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5236= yyls; struct Cyc_Yyltype* _temp5238= _temp5236.curr + yylsp_offset;
if( _temp5236.base == 0? 1:( _temp5238 < _temp5236.base? 1: _temp5238 >=
_temp5236.last_plus_one)){ _throw( Null_Exception);}* _temp5238;}).first_line,({
struct _tagged_ptr3 _temp5239= yyls; struct Cyc_Yyltype* _temp5241= _temp5239.curr
+ yylsp_offset; if( _temp5239.base == 0? 1:( _temp5241 < _temp5239.base? 1:
_temp5241 >= _temp5239.last_plus_one)){ _throw( Null_Exception);}* _temp5241;}).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp5242=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp5242->name=({ struct
_tagged_string* _temp5243=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp5243[ 0]=({ struct _tagged_string _temp5247= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp5244= yyvs; struct _xtunion_struct** _temp5246=
_temp5244.curr +( yyvsp_offset - 1); if( _temp5244.base == 0? 1:( _temp5246 <
_temp5244.base? 1: _temp5246 >= _temp5244.last_plus_one)){ _throw(
Null_Exception);}* _temp5246;})); xprintf("`%.*s", _temp5247.last_plus_one -
_temp5247.curr, _temp5247.curr);}); _temp5243;}); _temp5242->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp5242;}); void*
t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp5248=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp5248[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp5249; _temp5249.tag= Cyc_Absyn_VarType_tag;
_temp5249.f1= tv; _temp5249;}); _temp5248;}); yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5250=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5250[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5251; _temp5251.tag= Cyc_Stmt_tok_tag; _temp5251.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Region_s_struct* _temp5252=( struct Cyc_Absyn_Region_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct)); _temp5252[ 0]=({ struct
Cyc_Absyn_Region_s_struct _temp5253; _temp5253.tag= Cyc_Absyn_Region_s_tag;
_temp5253.f1= tv; _temp5253.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp5254=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp5254->f1=(
void*) Cyc_Absyn_Loc_n; _temp5254->f2=({ struct _tagged_string* _temp5255=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp5255[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5256= yyvs; struct
_xtunion_struct** _temp5258= _temp5256.curr +( yyvsp_offset - 1); if( _temp5256.base
== 0? 1:( _temp5258 < _temp5256.base? 1: _temp5258 >= _temp5256.last_plus_one)){
_throw( Null_Exception);}* _temp5258;})); _temp5255;}); _temp5254;}),( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp5259=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp5259[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp5260; _temp5260.tag= Cyc_Absyn_RgnHandleType_tag;
_temp5260.f1=( void*) t; _temp5260;}); _temp5259;}), 0); _temp5253.f3= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5261= yyvs; struct _xtunion_struct** _temp5263=
_temp5261.curr + yyvsp_offset; if( _temp5261.base == 0? 1:( _temp5263 <
_temp5261.base? 1: _temp5263 >= _temp5261.last_plus_one)){ _throw(
Null_Exception);}* _temp5263;})); _temp5253;}); _temp5252;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5264= yyls; struct Cyc_Yyltype* _temp5266= _temp5264.curr
+( yylsp_offset - 2); if( _temp5264.base == 0? 1:( _temp5266 < _temp5264.base? 1:
_temp5266 >= _temp5264.last_plus_one)){ _throw( Null_Exception);}* _temp5266;}).first_line,({
struct _tagged_ptr3 _temp5267= yyls; struct Cyc_Yyltype* _temp5269= _temp5267.curr
+ yylsp_offset; if( _temp5267.base == 0? 1:( _temp5269 < _temp5267.base? 1:
_temp5269 >= _temp5267.last_plus_one)){ _throw( Null_Exception);}* _temp5269;}).last_line));
_temp5251;}); _temp5250;}); break;} case 220: _LL5228: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5271=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5271[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5272; _temp5272.tag= Cyc_Stmt_tok_tag; _temp5272.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Cut_s_struct* _temp5273=( struct Cyc_Absyn_Cut_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct)); _temp5273[ 0]=({ struct Cyc_Absyn_Cut_s_struct
_temp5274; _temp5274.tag= Cyc_Absyn_Cut_s_tag; _temp5274.f1= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5275= yyvs; struct _xtunion_struct** _temp5277=
_temp5275.curr + yyvsp_offset; if( _temp5275.base == 0? 1:( _temp5277 <
_temp5275.base? 1: _temp5277 >= _temp5275.last_plus_one)){ _throw(
Null_Exception);}* _temp5277;})); _temp5274;}); _temp5273;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5278= yyls; struct Cyc_Yyltype* _temp5280= _temp5278.curr
+( yylsp_offset - 1); if( _temp5278.base == 0? 1:( _temp5280 < _temp5278.base? 1:
_temp5280 >= _temp5278.last_plus_one)){ _throw( Null_Exception);}* _temp5280;}).first_line,({
struct _tagged_ptr3 _temp5281= yyls; struct Cyc_Yyltype* _temp5283= _temp5281.curr
+ yylsp_offset; if( _temp5281.base == 0? 1:( _temp5283 < _temp5281.base? 1:
_temp5283 >= _temp5281.last_plus_one)){ _throw( Null_Exception);}* _temp5283;}).last_line));
_temp5272;}); _temp5271;}); break; case 221: _LL5270: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5285=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5285[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5286; _temp5286.tag= Cyc_Stmt_tok_tag; _temp5286.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Splice_s_struct* _temp5287=( struct Cyc_Absyn_Splice_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp5287[ 0]=({ struct
Cyc_Absyn_Splice_s_struct _temp5288; _temp5288.tag= Cyc_Absyn_Splice_s_tag;
_temp5288.f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5289= yyvs; struct
_xtunion_struct** _temp5291= _temp5289.curr + yyvsp_offset; if( _temp5289.base
== 0? 1:( _temp5291 < _temp5289.base? 1: _temp5291 >= _temp5289.last_plus_one)){
_throw( Null_Exception);}* _temp5291;})); _temp5288;}); _temp5287;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5292= yyls; struct Cyc_Yyltype* _temp5294= _temp5292.curr
+( yylsp_offset - 1); if( _temp5292.base == 0? 1:( _temp5294 < _temp5292.base? 1:
_temp5294 >= _temp5292.last_plus_one)){ _throw( Null_Exception);}* _temp5294;}).first_line,({
struct _tagged_ptr3 _temp5295= yyls; struct Cyc_Yyltype* _temp5297= _temp5295.curr
+ yylsp_offset; if( _temp5295.base == 0? 1:( _temp5297 < _temp5295.base? 1:
_temp5297 >= _temp5295.last_plus_one)){ _throw( Null_Exception);}* _temp5297;}).last_line));
_temp5286;}); _temp5285;}); break; case 222: _LL5284: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5299=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5299[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5300; _temp5300.tag= Cyc_Stmt_tok_tag; _temp5300.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Label_s_struct* _temp5301=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp5301[ 0]=({ struct
Cyc_Absyn_Label_s_struct _temp5302; _temp5302.tag= Cyc_Absyn_Label_s_tag;
_temp5302.f1=({ struct _tagged_string* _temp5303=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp5303[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp5304= yyvs; struct _xtunion_struct** _temp5306=
_temp5304.curr +( yyvsp_offset - 2); if( _temp5304.base == 0? 1:( _temp5306 <
_temp5304.base? 1: _temp5306 >= _temp5304.last_plus_one)){ _throw(
Null_Exception);}* _temp5306;})); _temp5303;}); _temp5302.f2= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5307= yyvs; struct _xtunion_struct** _temp5309=
_temp5307.curr + yyvsp_offset; if( _temp5307.base == 0? 1:( _temp5309 <
_temp5307.base? 1: _temp5309 >= _temp5307.last_plus_one)){ _throw(
Null_Exception);}* _temp5309;})); _temp5302;}); _temp5301;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5310= yyls; struct Cyc_Yyltype* _temp5312= _temp5310.curr
+( yylsp_offset - 2); if( _temp5310.base == 0? 1:( _temp5312 < _temp5310.base? 1:
_temp5312 >= _temp5310.last_plus_one)){ _throw( Null_Exception);}* _temp5312;}).first_line,({
struct _tagged_ptr3 _temp5313= yyls; struct Cyc_Yyltype* _temp5315= _temp5313.curr
+ yylsp_offset; if( _temp5313.base == 0? 1:( _temp5315 < _temp5313.base? 1:
_temp5315 >= _temp5313.last_plus_one)){ _throw( Null_Exception);}* _temp5315;}).last_line));
_temp5300;}); _temp5299;}); break; case 223: _LL5298: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5317=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5317[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5318; _temp5318.tag= Cyc_Stmt_tok_tag; _temp5318.f1= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5319= yyls; struct Cyc_Yyltype*
_temp5321= _temp5319.curr + yylsp_offset; if( _temp5319.base == 0? 1:( _temp5321
< _temp5319.base? 1: _temp5321 >= _temp5319.last_plus_one)){ _throw(
Null_Exception);}* _temp5321;}).first_line,({ struct _tagged_ptr3 _temp5322=
yyls; struct Cyc_Yyltype* _temp5324= _temp5322.curr + yylsp_offset; if(
_temp5322.base == 0? 1:( _temp5324 < _temp5322.base? 1: _temp5324 >= _temp5322.last_plus_one)){
_throw( Null_Exception);}* _temp5324;}).last_line)); _temp5318;}); _temp5317;});
break; case 224: _LL5316: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5326=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5326[ 0]=({ struct Cyc_Stmt_tok_struct _temp5327; _temp5327.tag= Cyc_Stmt_tok_tag;
_temp5327.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5328= yyvs; struct _xtunion_struct** _temp5330= _temp5328.curr +(
yyvsp_offset - 1); if( _temp5328.base == 0? 1:( _temp5330 < _temp5328.base? 1:
_temp5330 >= _temp5328.last_plus_one)){ _throw( Null_Exception);}* _temp5330;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5331= yyls; struct Cyc_Yyltype*
_temp5333= _temp5331.curr +( yylsp_offset - 1); if( _temp5331.base == 0? 1:(
_temp5333 < _temp5331.base? 1: _temp5333 >= _temp5331.last_plus_one)){ _throw(
Null_Exception);}* _temp5333;}).first_line,({ struct _tagged_ptr3 _temp5334=
yyls; struct Cyc_Yyltype* _temp5336= _temp5334.curr + yylsp_offset; if(
_temp5334.base == 0? 1:( _temp5336 < _temp5334.base? 1: _temp5336 >= _temp5334.last_plus_one)){
_throw( Null_Exception);}* _temp5336;}).last_line)); _temp5327;}); _temp5326;});
break; case 225: _LL5325: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5338=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5338[ 0]=({ struct Cyc_Stmt_tok_struct _temp5339; _temp5339.tag= Cyc_Stmt_tok_tag;
_temp5339.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5340= yyls; struct Cyc_Yyltype* _temp5342= _temp5340.curr +(
yylsp_offset - 1); if( _temp5340.base == 0? 1:( _temp5342 < _temp5340.base? 1:
_temp5342 >= _temp5340.last_plus_one)){ _throw( Null_Exception);}* _temp5342;}).first_line,({
struct _tagged_ptr3 _temp5343= yyls; struct Cyc_Yyltype* _temp5345= _temp5343.curr
+ yylsp_offset; if( _temp5343.base == 0? 1:( _temp5345 < _temp5343.base? 1:
_temp5345 >= _temp5343.last_plus_one)){ _throw( Null_Exception);}* _temp5345;}).last_line));
_temp5339;}); _temp5338;}); break; case 226: _LL5337: yyval=({ struct
_tagged_ptr2 _temp5347= yyvs; struct _xtunion_struct** _temp5349= _temp5347.curr
+( yyvsp_offset - 1); if( _temp5347.base == 0? 1:( _temp5349 < _temp5347.base? 1:
_temp5349 >= _temp5347.last_plus_one)){ _throw( Null_Exception);}* _temp5349;});
break; case 227: _LL5346:{ void* _temp5354= Cyc_yyget_BlockItem_tok(({ struct
_tagged_ptr2 _temp5351= yyvs; struct _xtunion_struct** _temp5353= _temp5351.curr
+ yyvsp_offset; if( _temp5351.base == 0? 1:( _temp5353 < _temp5351.base? 1:
_temp5353 >= _temp5351.last_plus_one)){ _throw( Null_Exception);}* _temp5353;}));
struct Cyc_List_List* _temp5362; struct Cyc_Absyn_Fndecl* _temp5364; struct Cyc_Absyn_Stmt*
_temp5366; _LL5356: if((( struct _tunion_struct*) _temp5354)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL5363: _temp5362=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp5354)->f1; goto _LL5357;} else{ goto _LL5358;} _LL5358: if((( struct
_tunion_struct*) _temp5354)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL5365: _temp5364=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp5354)->f1;
goto _LL5359;} else{ goto _LL5360;} _LL5360: if((( struct _tunion_struct*)
_temp5354)->tag == Cyc_Parse_Stmt_bl_tag){ _LL5367: _temp5366=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp5354)->f1; goto _LL5361;} else{ goto
_LL5355;} _LL5357: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5368=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5368[ 0]=({ struct Cyc_Stmt_tok_struct _temp5369; _temp5369.tag= Cyc_Stmt_tok_tag;
_temp5369.f1= Cyc_Parse_flatten_declarations( _temp5362, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5370= yyls; struct Cyc_Yyltype*
_temp5372= _temp5370.curr + yylsp_offset; if( _temp5370.base == 0? 1:( _temp5372
< _temp5370.base? 1: _temp5372 >= _temp5370.last_plus_one)){ _throw(
Null_Exception);}* _temp5372;}).first_line,({ struct _tagged_ptr3 _temp5373=
yyls; struct Cyc_Yyltype* _temp5375= _temp5373.curr + yylsp_offset; if(
_temp5373.base == 0? 1:( _temp5375 < _temp5373.base? 1: _temp5375 >= _temp5373.last_plus_one)){
_throw( Null_Exception);}* _temp5375;}).last_line))); _temp5369;}); _temp5368;});
goto _LL5355; _LL5359: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5376=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5376[ 0]=({ struct Cyc_Stmt_tok_struct _temp5377; _temp5377.tag= Cyc_Stmt_tok_tag;
_temp5377.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp5378=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5378[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5379; _temp5379.tag= Cyc_Absyn_Fn_d_tag;
_temp5379.f1= _temp5364; _temp5379;}); _temp5378;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5380= yyls; struct Cyc_Yyltype* _temp5382= _temp5380.curr
+ yylsp_offset; if( _temp5380.base == 0? 1:( _temp5382 < _temp5380.base? 1:
_temp5382 >= _temp5380.last_plus_one)){ _throw( Null_Exception);}* _temp5382;}).first_line,({
struct _tagged_ptr3 _temp5383= yyls; struct Cyc_Yyltype* _temp5385= _temp5383.curr
+ yylsp_offset; if( _temp5383.base == 0? 1:( _temp5385 < _temp5383.base? 1:
_temp5385 >= _temp5383.last_plus_one)){ _throw( Null_Exception);}* _temp5385;}).last_line)),
Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5386= yyls; struct Cyc_Yyltype* _temp5388= _temp5386.curr + yylsp_offset;
if( _temp5386.base == 0? 1:( _temp5388 < _temp5386.base? 1: _temp5388 >=
_temp5386.last_plus_one)){ _throw( Null_Exception);}* _temp5388;}).first_line,({
struct _tagged_ptr3 _temp5389= yyls; struct Cyc_Yyltype* _temp5391= _temp5389.curr
+ yylsp_offset; if( _temp5389.base == 0? 1:( _temp5391 < _temp5389.base? 1:
_temp5391 >= _temp5389.last_plus_one)){ _throw( Null_Exception);}* _temp5391;}).last_line)));
_temp5377;}); _temp5376;}); goto _LL5355; _LL5361: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5392=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5392[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5393; _temp5393.tag= Cyc_Stmt_tok_tag; _temp5393.f1= _temp5366; _temp5393;});
_temp5392;}); goto _LL5355; _LL5355:;} break; case 228: _LL5350:{ void*
_temp5398= Cyc_yyget_BlockItem_tok(({ struct _tagged_ptr2 _temp5395= yyvs;
struct _xtunion_struct** _temp5397= _temp5395.curr +( yyvsp_offset - 1); if(
_temp5395.base == 0? 1:( _temp5397 < _temp5395.base? 1: _temp5397 >= _temp5395.last_plus_one)){
_throw( Null_Exception);}* _temp5397;})); struct Cyc_List_List* _temp5406;
struct Cyc_Absyn_Fndecl* _temp5408; struct Cyc_Absyn_Stmt* _temp5410; _LL5400:
if((( struct _tunion_struct*) _temp5398)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL5407: _temp5406=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp5398)->f1; goto _LL5401;} else{ goto _LL5402;} _LL5402: if((( struct
_tunion_struct*) _temp5398)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL5409: _temp5408=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp5398)->f1;
goto _LL5403;} else{ goto _LL5404;} _LL5404: if((( struct _tunion_struct*)
_temp5398)->tag == Cyc_Parse_Stmt_bl_tag){ _LL5411: _temp5410=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp5398)->f1; goto _LL5405;} else{ goto
_LL5399;} _LL5401: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5412=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5412[ 0]=({ struct Cyc_Stmt_tok_struct _temp5413; _temp5413.tag= Cyc_Stmt_tok_tag;
_temp5413.f1= Cyc_Parse_flatten_declarations( _temp5406, Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5414= yyvs; struct _xtunion_struct** _temp5416=
_temp5414.curr + yyvsp_offset; if( _temp5414.base == 0? 1:( _temp5416 <
_temp5414.base? 1: _temp5416 >= _temp5414.last_plus_one)){ _throw(
Null_Exception);}* _temp5416;}))); _temp5413;}); _temp5412;}); goto _LL5399;
_LL5403: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5417=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5417[ 0]=({ struct Cyc_Stmt_tok_struct _temp5418; _temp5418.tag= Cyc_Stmt_tok_tag;
_temp5418.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp5419=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5419[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5420; _temp5420.tag= Cyc_Absyn_Fn_d_tag;
_temp5420.f1= _temp5408; _temp5420;}); _temp5419;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5421= yyls; struct Cyc_Yyltype* _temp5423= _temp5421.curr
+( yylsp_offset - 1); if( _temp5421.base == 0? 1:( _temp5423 < _temp5421.base? 1:
_temp5423 >= _temp5421.last_plus_one)){ _throw( Null_Exception);}* _temp5423;}).first_line,({
struct _tagged_ptr3 _temp5424= yyls; struct Cyc_Yyltype* _temp5426= _temp5424.curr
+( yylsp_offset - 1); if( _temp5424.base == 0? 1:( _temp5426 < _temp5424.base? 1:
_temp5426 >= _temp5424.last_plus_one)){ _throw( Null_Exception);}* _temp5426;}).last_line)),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5427= yyvs; struct
_xtunion_struct** _temp5429= _temp5427.curr + yyvsp_offset; if( _temp5427.base
== 0? 1:( _temp5429 < _temp5427.base? 1: _temp5429 >= _temp5427.last_plus_one)){
_throw( Null_Exception);}* _temp5429;}))); _temp5418;}); _temp5417;}); goto
_LL5399; _LL5405: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5430=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5430[ 0]=({ struct Cyc_Stmt_tok_struct _temp5431; _temp5431.tag= Cyc_Stmt_tok_tag;
_temp5431.f1= Cyc_Absyn_seq_stmt( _temp5410, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5432= yyvs; struct _xtunion_struct** _temp5434= _temp5432.curr
+ yyvsp_offset; if( _temp5432.base == 0? 1:( _temp5434 < _temp5432.base? 1:
_temp5434 >= _temp5432.last_plus_one)){ _throw( Null_Exception);}* _temp5434;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5435= yyls; struct Cyc_Yyltype*
_temp5437= _temp5435.curr +( yylsp_offset - 1); if( _temp5435.base == 0? 1:(
_temp5437 < _temp5435.base? 1: _temp5437 >= _temp5435.last_plus_one)){ _throw(
Null_Exception);}* _temp5437;}).first_line,({ struct _tagged_ptr3 _temp5438=
yyls; struct Cyc_Yyltype* _temp5440= _temp5438.curr + yylsp_offset; if(
_temp5438.base == 0? 1:( _temp5440 < _temp5438.base? 1: _temp5440 >= _temp5438.last_plus_one)){
_throw( Null_Exception);}* _temp5440;}).last_line)); _temp5431;}); _temp5430;});
goto _LL5399; _LL5399:;} break; case 229: _LL5394: yyval=( struct
_xtunion_struct*)({ struct Cyc_BlockItem_tok_struct* _temp5442=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp5442[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp5443; _temp5443.tag= Cyc_BlockItem_tok_tag;
_temp5443.f1=( void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp5444=(
struct Cyc_Parse_TopDecls_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct));
_temp5444[ 0]=({ struct Cyc_Parse_TopDecls_bl_struct _temp5445; _temp5445.tag=
Cyc_Parse_TopDecls_bl_tag; _temp5445.f1= Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp5446= yyvs; struct _xtunion_struct** _temp5448= _temp5446.curr
+ yyvsp_offset; if( _temp5446.base == 0? 1:( _temp5448 < _temp5446.base? 1:
_temp5448 >= _temp5446.last_plus_one)){ _throw( Null_Exception);}* _temp5448;}));
_temp5445;}); _temp5444;})); _temp5443;}); _temp5442;}); break; case 230:
_LL5441: yyval=( struct _xtunion_struct*)({ struct Cyc_BlockItem_tok_struct*
_temp5450=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp5450[ 0]=({ struct Cyc_BlockItem_tok_struct _temp5451; _temp5451.tag= Cyc_BlockItem_tok_tag;
_temp5451.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp5452=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp5452[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp5453; _temp5453.tag= Cyc_Parse_Stmt_bl_tag;
_temp5453.f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5454= yyvs; struct
_xtunion_struct** _temp5456= _temp5454.curr + yyvsp_offset; if( _temp5454.base
== 0? 1:( _temp5456 < _temp5454.base? 1: _temp5456 >= _temp5454.last_plus_one)){
_throw( Null_Exception);}* _temp5456;})); _temp5453;}); _temp5452;})); _temp5451;});
_temp5450;}); break; case 231: _LL5449: yyval=( struct _xtunion_struct*)({
struct Cyc_BlockItem_tok_struct* _temp5458=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp5458[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp5459; _temp5459.tag= Cyc_BlockItem_tok_tag;
_temp5459.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp5460=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp5460[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp5461; _temp5461.tag= Cyc_Parse_FnDecl_bl_tag;
_temp5461.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp5462= yyvs;
struct _xtunion_struct** _temp5464= _temp5462.curr + yyvsp_offset; if( _temp5462.base
== 0? 1:( _temp5464 < _temp5462.base? 1: _temp5464 >= _temp5462.last_plus_one)){
_throw( Null_Exception);}* _temp5464;})); _temp5461;}); _temp5460;})); _temp5459;});
_temp5458;}); break; case 232: _LL5457: yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5466=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5466[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5467; _temp5467.tag= Cyc_Stmt_tok_tag; _temp5467.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5468= yyvs; struct _xtunion_struct**
_temp5470= _temp5468.curr +( yyvsp_offset - 2); if( _temp5468.base == 0? 1:(
_temp5470 < _temp5468.base? 1: _temp5470 >= _temp5468.last_plus_one)){ _throw(
Null_Exception);}* _temp5470;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5471= yyvs; struct _xtunion_struct** _temp5473= _temp5471.curr +
yyvsp_offset; if( _temp5471.base == 0? 1:( _temp5473 < _temp5471.base? 1:
_temp5473 >= _temp5471.last_plus_one)){ _throw( Null_Exception);}* _temp5473;})),
Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5474= yyls; struct Cyc_Yyltype* _temp5476= _temp5474.curr +( yylsp_offset -
4); if( _temp5474.base == 0? 1:( _temp5476 < _temp5474.base? 1: _temp5476 >=
_temp5474.last_plus_one)){ _throw( Null_Exception);}* _temp5476;}).first_line,({
struct _tagged_ptr3 _temp5477= yyls; struct Cyc_Yyltype* _temp5479= _temp5477.curr
+ yylsp_offset; if( _temp5477.base == 0? 1:( _temp5479 < _temp5477.base? 1:
_temp5479 >= _temp5477.last_plus_one)){ _throw( Null_Exception);}* _temp5479;}).last_line));
_temp5467;}); _temp5466;}); break; case 233: _LL5465: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5481=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5481[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5482; _temp5482.tag= Cyc_Stmt_tok_tag; _temp5482.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5483= yyvs; struct _xtunion_struct**
_temp5485= _temp5483.curr +( yyvsp_offset - 4); if( _temp5483.base == 0? 1:(
_temp5485 < _temp5483.base? 1: _temp5485 >= _temp5483.last_plus_one)){ _throw(
Null_Exception);}* _temp5485;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5486= yyvs; struct _xtunion_struct** _temp5488= _temp5486.curr +(
yyvsp_offset - 2); if( _temp5486.base == 0? 1:( _temp5488 < _temp5486.base? 1:
_temp5488 >= _temp5486.last_plus_one)){ _throw( Null_Exception);}* _temp5488;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5489= yyvs; struct
_xtunion_struct** _temp5491= _temp5489.curr + yyvsp_offset; if( _temp5489.base
== 0? 1:( _temp5491 < _temp5489.base? 1: _temp5491 >= _temp5489.last_plus_one)){
_throw( Null_Exception);}* _temp5491;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5492= yyls; struct Cyc_Yyltype* _temp5494= _temp5492.curr +(
yylsp_offset - 6); if( _temp5492.base == 0? 1:( _temp5494 < _temp5492.base? 1:
_temp5494 >= _temp5492.last_plus_one)){ _throw( Null_Exception);}* _temp5494;}).first_line,({
struct _tagged_ptr3 _temp5495= yyls; struct Cyc_Yyltype* _temp5497= _temp5495.curr
+ yylsp_offset; if( _temp5495.base == 0? 1:( _temp5497 < _temp5495.base? 1:
_temp5497 >= _temp5495.last_plus_one)){ _throw( Null_Exception);}* _temp5497;}).last_line));
_temp5482;}); _temp5481;}); break; case 234: _LL5480: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5499=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5499[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5500; _temp5500.tag= Cyc_Stmt_tok_tag; _temp5500.f1= Cyc_Absyn_switch_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5501= yyvs; struct _xtunion_struct**
_temp5503= _temp5501.curr +( yyvsp_offset - 4); if( _temp5501.base == 0? 1:(
_temp5503 < _temp5501.base? 1: _temp5503 >= _temp5501.last_plus_one)){ _throw(
Null_Exception);}* _temp5503;})), Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5504= yyvs; struct _xtunion_struct** _temp5506= _temp5504.curr
+( yyvsp_offset - 1); if( _temp5504.base == 0? 1:( _temp5506 < _temp5504.base? 1:
_temp5506 >= _temp5504.last_plus_one)){ _throw( Null_Exception);}* _temp5506;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5507= yyls; struct Cyc_Yyltype*
_temp5509= _temp5507.curr +( yylsp_offset - 6); if( _temp5507.base == 0? 1:(
_temp5509 < _temp5507.base? 1: _temp5509 >= _temp5507.last_plus_one)){ _throw(
Null_Exception);}* _temp5509;}).first_line,({ struct _tagged_ptr3 _temp5510=
yyls; struct Cyc_Yyltype* _temp5512= _temp5510.curr + yylsp_offset; if(
_temp5510.base == 0? 1:( _temp5512 < _temp5510.base? 1: _temp5512 >= _temp5510.last_plus_one)){
_throw( Null_Exception);}* _temp5512;}).last_line)); _temp5500;}); _temp5499;});
break; case 235: _LL5498: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5514=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5514[ 0]=({ struct Cyc_Stmt_tok_struct _temp5515; _temp5515.tag= Cyc_Stmt_tok_tag;
_temp5515.f1= Cyc_Absyn_trycatch_stmt( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5516= yyvs; struct _xtunion_struct** _temp5518= _temp5516.curr +(
yyvsp_offset - 4); if( _temp5516.base == 0? 1:( _temp5518 < _temp5516.base? 1:
_temp5518 >= _temp5516.last_plus_one)){ _throw( Null_Exception);}* _temp5518;})),
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp5519= yyvs; struct
_xtunion_struct** _temp5521= _temp5519.curr +( yyvsp_offset - 1); if( _temp5519.base
== 0? 1:( _temp5521 < _temp5519.base? 1: _temp5521 >= _temp5519.last_plus_one)){
_throw( Null_Exception);}* _temp5521;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5522= yyls; struct Cyc_Yyltype* _temp5524= _temp5522.curr +(
yylsp_offset - 5); if( _temp5522.base == 0? 1:( _temp5524 < _temp5522.base? 1:
_temp5524 >= _temp5522.last_plus_one)){ _throw( Null_Exception);}* _temp5524;}).first_line,({
struct _tagged_ptr3 _temp5525= yyls; struct Cyc_Yyltype* _temp5527= _temp5525.curr
+ yylsp_offset; if( _temp5525.base == 0? 1:( _temp5527 < _temp5525.base? 1:
_temp5527 >= _temp5525.last_plus_one)){ _throw( Null_Exception);}* _temp5527;}).last_line));
_temp5515;}); _temp5514;}); break; case 236: _LL5513: yyval=( struct
_xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5529=( struct
Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5529[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5530; _temp5530.tag=
Cyc_SwitchClauseList_tok_tag; _temp5530.f1= 0; _temp5530;}); _temp5529;});
break; case 237: _LL5528: yyval=( struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp5532=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp5532[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp5533; _temp5533.tag= Cyc_SwitchClauseList_tok_tag; _temp5533.f1=({ struct
Cyc_List_List* _temp5534=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5534->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp5535=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5535->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5545= yyls; struct Cyc_Yyltype* _temp5547= _temp5545.curr +( yylsp_offset -
2); if( _temp5545.base == 0? 1:( _temp5547 < _temp5545.base? 1: _temp5547 >=
_temp5545.last_plus_one)){ _throw( Null_Exception);}* _temp5547;}).first_line,({
struct _tagged_ptr3 _temp5548= yyls; struct Cyc_Yyltype* _temp5550= _temp5548.curr
+( yylsp_offset - 2); if( _temp5548.base == 0? 1:( _temp5550 < _temp5548.base? 1:
_temp5550 >= _temp5548.last_plus_one)){ _throw( Null_Exception);}* _temp5550;}).last_line));
_temp5535->pat_vars= 0; _temp5535->where_clause= 0; _temp5535->body= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5542= yyvs; struct _xtunion_struct** _temp5544=
_temp5542.curr + yyvsp_offset; if( _temp5542.base == 0? 1:( _temp5544 <
_temp5542.base? 1: _temp5544 >= _temp5542.last_plus_one)){ _throw(
Null_Exception);}* _temp5544;})); _temp5535->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5536= yyls; struct Cyc_Yyltype* _temp5538= _temp5536.curr
+( yylsp_offset - 2); if( _temp5536.base == 0? 1:( _temp5538 < _temp5536.base? 1:
_temp5538 >= _temp5536.last_plus_one)){ _throw( Null_Exception);}* _temp5538;}).first_line,({
struct _tagged_ptr3 _temp5539= yyls; struct Cyc_Yyltype* _temp5541= _temp5539.curr
+ yylsp_offset; if( _temp5539.base == 0? 1:( _temp5541 < _temp5539.base? 1:
_temp5541 >= _temp5539.last_plus_one)){ _throw( Null_Exception);}* _temp5541;}).last_line);
_temp5535;}); _temp5534->tl= 0; _temp5534;}); _temp5533;}); _temp5532;}); break;
case 238: _LL5531: yyval=( struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp5552=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp5552[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp5553; _temp5553.tag= Cyc_SwitchClauseList_tok_tag; _temp5553.f1=({ struct
Cyc_List_List* _temp5554=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5554->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp5558=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5558->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5571= yyvs; struct _xtunion_struct** _temp5573=
_temp5571.curr +( yyvsp_offset - 2); if( _temp5571.base == 0? 1:( _temp5573 <
_temp5571.base? 1: _temp5573 >= _temp5571.last_plus_one)){ _throw(
Null_Exception);}* _temp5573;})); _temp5558->pat_vars= 0; _temp5558->where_clause=
0; _temp5558->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5565= yyls; struct Cyc_Yyltype* _temp5567= _temp5565.curr +(
yylsp_offset - 1); if( _temp5565.base == 0? 1:( _temp5567 < _temp5565.base? 1:
_temp5567 >= _temp5565.last_plus_one)){ _throw( Null_Exception);}* _temp5567;}).first_line,({
struct _tagged_ptr3 _temp5568= yyls; struct Cyc_Yyltype* _temp5570= _temp5568.curr
+( yylsp_offset - 1); if( _temp5568.base == 0? 1:( _temp5570 < _temp5568.base? 1:
_temp5570 >= _temp5568.last_plus_one)){ _throw( Null_Exception);}* _temp5570;}).last_line));
_temp5558->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5559=
yyls; struct Cyc_Yyltype* _temp5561= _temp5559.curr +( yylsp_offset - 3); if(
_temp5559.base == 0? 1:( _temp5561 < _temp5559.base? 1: _temp5561 >= _temp5559.last_plus_one)){
_throw( Null_Exception);}* _temp5561;}).first_line,({ struct _tagged_ptr3
_temp5562= yyls; struct Cyc_Yyltype* _temp5564= _temp5562.curr + yylsp_offset;
if( _temp5562.base == 0? 1:( _temp5564 < _temp5562.base? 1: _temp5564 >=
_temp5562.last_plus_one)){ _throw( Null_Exception);}* _temp5564;}).last_line);
_temp5558;}); _temp5554->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5555= yyvs; struct _xtunion_struct** _temp5557= _temp5555.curr
+ yyvsp_offset; if( _temp5555.base == 0? 1:( _temp5557 < _temp5555.base? 1:
_temp5557 >= _temp5555.last_plus_one)){ _throw( Null_Exception);}* _temp5557;}));
_temp5554;}); _temp5553;}); _temp5552;}); break; case 239: _LL5551: yyval=(
struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5575=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5575[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5576; _temp5576.tag=
Cyc_SwitchClauseList_tok_tag; _temp5576.f1=({ struct Cyc_List_List* _temp5577=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5577->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5581=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5581->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5591= yyvs; struct _xtunion_struct** _temp5593=
_temp5591.curr +( yyvsp_offset - 3); if( _temp5591.base == 0? 1:( _temp5593 <
_temp5591.base? 1: _temp5593 >= _temp5591.last_plus_one)){ _throw(
Null_Exception);}* _temp5593;})); _temp5581->pat_vars= 0; _temp5581->where_clause=
0; _temp5581->body= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5588= yyvs;
struct _xtunion_struct** _temp5590= _temp5588.curr +( yyvsp_offset - 1); if(
_temp5588.base == 0? 1:( _temp5590 < _temp5588.base? 1: _temp5590 >= _temp5588.last_plus_one)){
_throw( Null_Exception);}* _temp5590;})); _temp5581->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5582= yyls; struct Cyc_Yyltype* _temp5584= _temp5582.curr
+( yylsp_offset - 4); if( _temp5582.base == 0? 1:( _temp5584 < _temp5582.base? 1:
_temp5584 >= _temp5582.last_plus_one)){ _throw( Null_Exception);}* _temp5584;}).first_line,({
struct _tagged_ptr3 _temp5585= yyls; struct Cyc_Yyltype* _temp5587= _temp5585.curr
+( yylsp_offset - 1); if( _temp5585.base == 0? 1:( _temp5587 < _temp5585.base? 1:
_temp5587 >= _temp5585.last_plus_one)){ _throw( Null_Exception);}* _temp5587;}).last_line);
_temp5581;}); _temp5577->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5578= yyvs; struct _xtunion_struct** _temp5580= _temp5578.curr
+ yyvsp_offset; if( _temp5578.base == 0? 1:( _temp5580 < _temp5578.base? 1:
_temp5580 >= _temp5578.last_plus_one)){ _throw( Null_Exception);}* _temp5580;}));
_temp5577;}); _temp5576;}); _temp5575;}); break; case 240: _LL5574: yyval=(
struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5595=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5595[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5596; _temp5596.tag=
Cyc_SwitchClauseList_tok_tag; _temp5596.f1=({ struct Cyc_List_List* _temp5597=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5597->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5601=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5601->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5617= yyvs; struct _xtunion_struct** _temp5619=
_temp5617.curr +( yyvsp_offset - 4); if( _temp5617.base == 0? 1:( _temp5619 <
_temp5617.base? 1: _temp5619 >= _temp5617.last_plus_one)){ _throw(
Null_Exception);}* _temp5619;})); _temp5601->pat_vars= 0; _temp5601->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5614= yyvs;
struct _xtunion_struct** _temp5616= _temp5614.curr +( yyvsp_offset - 2); if(
_temp5614.base == 0? 1:( _temp5616 < _temp5614.base? 1: _temp5616 >= _temp5614.last_plus_one)){
_throw( Null_Exception);}* _temp5616;})); _temp5601->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5608= yyls; struct Cyc_Yyltype*
_temp5610= _temp5608.curr +( yylsp_offset - 1); if( _temp5608.base == 0? 1:(
_temp5610 < _temp5608.base? 1: _temp5610 >= _temp5608.last_plus_one)){ _throw(
Null_Exception);}* _temp5610;}).first_line,({ struct _tagged_ptr3 _temp5611=
yyls; struct Cyc_Yyltype* _temp5613= _temp5611.curr +( yylsp_offset - 1); if(
_temp5611.base == 0? 1:( _temp5613 < _temp5611.base? 1: _temp5613 >= _temp5611.last_plus_one)){
_throw( Null_Exception);}* _temp5613;}).last_line)); _temp5601->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5602= yyls; struct Cyc_Yyltype* _temp5604= _temp5602.curr
+( yylsp_offset - 5); if( _temp5602.base == 0? 1:( _temp5604 < _temp5602.base? 1:
_temp5604 >= _temp5602.last_plus_one)){ _throw( Null_Exception);}* _temp5604;}).first_line,({
struct _tagged_ptr3 _temp5605= yyls; struct Cyc_Yyltype* _temp5607= _temp5605.curr
+ yylsp_offset; if( _temp5605.base == 0? 1:( _temp5607 < _temp5605.base? 1:
_temp5607 >= _temp5605.last_plus_one)){ _throw( Null_Exception);}* _temp5607;}).last_line);
_temp5601;}); _temp5597->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5598= yyvs; struct _xtunion_struct** _temp5600= _temp5598.curr
+ yyvsp_offset; if( _temp5598.base == 0? 1:( _temp5600 < _temp5598.base? 1:
_temp5600 >= _temp5598.last_plus_one)){ _throw( Null_Exception);}* _temp5600;}));
_temp5597;}); _temp5596;}); _temp5595;}); break; case 241: _LL5594: yyval=(
struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5621=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5621[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5622; _temp5622.tag=
Cyc_SwitchClauseList_tok_tag; _temp5622.f1=({ struct Cyc_List_List* _temp5623=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5623->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5627=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5627->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5640= yyvs; struct _xtunion_struct** _temp5642=
_temp5640.curr +( yyvsp_offset - 5); if( _temp5640.base == 0? 1:( _temp5642 <
_temp5640.base? 1: _temp5642 >= _temp5640.last_plus_one)){ _throw(
Null_Exception);}* _temp5642;})); _temp5627->pat_vars= 0; _temp5627->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5637= yyvs;
struct _xtunion_struct** _temp5639= _temp5637.curr +( yyvsp_offset - 3); if(
_temp5637.base == 0? 1:( _temp5639 < _temp5637.base? 1: _temp5639 >= _temp5637.last_plus_one)){
_throw( Null_Exception);}* _temp5639;})); _temp5627->body= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5634= yyvs; struct _xtunion_struct** _temp5636=
_temp5634.curr +( yyvsp_offset - 1); if( _temp5634.base == 0? 1:( _temp5636 <
_temp5634.base? 1: _temp5636 >= _temp5634.last_plus_one)){ _throw(
Null_Exception);}* _temp5636;})); _temp5627->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5628= yyls; struct Cyc_Yyltype* _temp5630= _temp5628.curr
+( yylsp_offset - 6); if( _temp5628.base == 0? 1:( _temp5630 < _temp5628.base? 1:
_temp5630 >= _temp5628.last_plus_one)){ _throw( Null_Exception);}* _temp5630;}).first_line,({
struct _tagged_ptr3 _temp5631= yyls; struct Cyc_Yyltype* _temp5633= _temp5631.curr
+ yylsp_offset; if( _temp5631.base == 0? 1:( _temp5633 < _temp5631.base? 1:
_temp5633 >= _temp5631.last_plus_one)){ _throw( Null_Exception);}* _temp5633;}).last_line);
_temp5627;}); _temp5623->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5624= yyvs; struct _xtunion_struct** _temp5626= _temp5624.curr
+ yyvsp_offset; if( _temp5624.base == 0? 1:( _temp5626 < _temp5624.base? 1:
_temp5626 >= _temp5624.last_plus_one)){ _throw( Null_Exception);}* _temp5626;}));
_temp5623;}); _temp5622;}); _temp5621;}); break; case 242: _LL5620: yyval=(
struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5644=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5644[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5645; _temp5645.tag= Cyc_Stmt_tok_tag; _temp5645.f1= Cyc_Absyn_while_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5646= yyvs; struct _xtunion_struct**
_temp5648= _temp5646.curr +( yyvsp_offset - 2); if( _temp5646.base == 0? 1:(
_temp5648 < _temp5646.base? 1: _temp5648 >= _temp5646.last_plus_one)){ _throw(
Null_Exception);}* _temp5648;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5649= yyvs; struct _xtunion_struct** _temp5651= _temp5649.curr +
yyvsp_offset; if( _temp5649.base == 0? 1:( _temp5651 < _temp5649.base? 1:
_temp5651 >= _temp5649.last_plus_one)){ _throw( Null_Exception);}* _temp5651;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5652= yyls; struct Cyc_Yyltype*
_temp5654= _temp5652.curr +( yylsp_offset - 4); if( _temp5652.base == 0? 1:(
_temp5654 < _temp5652.base? 1: _temp5654 >= _temp5652.last_plus_one)){ _throw(
Null_Exception);}* _temp5654;}).first_line,({ struct _tagged_ptr3 _temp5655=
yyls; struct Cyc_Yyltype* _temp5657= _temp5655.curr + yylsp_offset; if(
_temp5655.base == 0? 1:( _temp5657 < _temp5655.base? 1: _temp5657 >= _temp5655.last_plus_one)){
_throw( Null_Exception);}* _temp5657;}).last_line)); _temp5645;}); _temp5644;});
break; case 243: _LL5643: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5659=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5659[ 0]=({ struct Cyc_Stmt_tok_struct _temp5660; _temp5660.tag= Cyc_Stmt_tok_tag;
_temp5660.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5661= yyvs; struct _xtunion_struct** _temp5663= _temp5661.curr +(
yyvsp_offset - 5); if( _temp5661.base == 0? 1:( _temp5663 < _temp5661.base? 1:
_temp5663 >= _temp5661.last_plus_one)){ _throw( Null_Exception);}* _temp5663;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5664= yyvs; struct _xtunion_struct**
_temp5666= _temp5664.curr +( yyvsp_offset - 2); if( _temp5664.base == 0? 1:(
_temp5666 < _temp5664.base? 1: _temp5666 >= _temp5664.last_plus_one)){ _throw(
Null_Exception);}* _temp5666;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5667= yyls; struct Cyc_Yyltype* _temp5669= _temp5667.curr +(
yylsp_offset - 6); if( _temp5667.base == 0? 1:( _temp5669 < _temp5667.base? 1:
_temp5669 >= _temp5667.last_plus_one)){ _throw( Null_Exception);}* _temp5669;}).first_line,({
struct _tagged_ptr3 _temp5670= yyls; struct Cyc_Yyltype* _temp5672= _temp5670.curr
+ yylsp_offset; if( _temp5670.base == 0? 1:( _temp5672 < _temp5670.base? 1:
_temp5672 >= _temp5670.last_plus_one)){ _throw( Null_Exception);}* _temp5672;}).last_line));
_temp5660;}); _temp5659;}); break; case 244: _LL5658: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5674=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5674[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5675; _temp5675.tag= Cyc_Stmt_tok_tag; _temp5675.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5676= yyvs; struct _xtunion_struct** _temp5678=
_temp5676.curr + yyvsp_offset; if( _temp5676.base == 0? 1:( _temp5678 <
_temp5676.base? 1: _temp5678 >= _temp5676.last_plus_one)){ _throw(
Null_Exception);}* _temp5678;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5679= yyls; struct Cyc_Yyltype* _temp5681= _temp5679.curr +(
yylsp_offset - 5); if( _temp5679.base == 0? 1:( _temp5681 < _temp5679.base? 1:
_temp5681 >= _temp5679.last_plus_one)){ _throw( Null_Exception);}* _temp5681;}).first_line,({
struct _tagged_ptr3 _temp5682= yyls; struct Cyc_Yyltype* _temp5684= _temp5682.curr
+ yylsp_offset; if( _temp5682.base == 0? 1:( _temp5684 < _temp5682.base? 1:
_temp5684 >= _temp5682.last_plus_one)){ _throw( Null_Exception);}* _temp5684;}).last_line));
_temp5675;}); _temp5674;}); break; case 245: _LL5673: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5686=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5686[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5687; _temp5687.tag= Cyc_Stmt_tok_tag; _temp5687.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5688= yyvs; struct _xtunion_struct** _temp5690= _temp5688.curr
+( yyvsp_offset - 2); if( _temp5688.base == 0? 1:( _temp5690 < _temp5688.base? 1:
_temp5690 >= _temp5688.last_plus_one)){ _throw( Null_Exception);}* _temp5690;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5691= yyvs; struct
_xtunion_struct** _temp5693= _temp5691.curr + yyvsp_offset; if( _temp5691.base
== 0? 1:( _temp5693 < _temp5691.base? 1: _temp5693 >= _temp5691.last_plus_one)){
_throw( Null_Exception);}* _temp5693;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5694= yyls; struct Cyc_Yyltype* _temp5696= _temp5694.curr +(
yylsp_offset - 6); if( _temp5694.base == 0? 1:( _temp5696 < _temp5694.base? 1:
_temp5696 >= _temp5694.last_plus_one)){ _throw( Null_Exception);}* _temp5696;}).first_line,({
struct _tagged_ptr3 _temp5697= yyls; struct Cyc_Yyltype* _temp5699= _temp5697.curr
+ yylsp_offset; if( _temp5697.base == 0? 1:( _temp5699 < _temp5697.base? 1:
_temp5699 >= _temp5697.last_plus_one)){ _throw( Null_Exception);}* _temp5699;}).last_line));
_temp5687;}); _temp5686;}); break; case 246: _LL5685: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5701=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5701[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5702; _temp5702.tag= Cyc_Stmt_tok_tag; _temp5702.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5703=
yyvs; struct _xtunion_struct** _temp5705= _temp5703.curr +( yyvsp_offset - 3);
if( _temp5703.base == 0? 1:( _temp5705 < _temp5703.base? 1: _temp5705 >=
_temp5703.last_plus_one)){ _throw( Null_Exception);}* _temp5705;})), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5706= yyvs; struct
_xtunion_struct** _temp5708= _temp5706.curr + yyvsp_offset; if( _temp5706.base
== 0? 1:( _temp5708 < _temp5706.base? 1: _temp5708 >= _temp5706.last_plus_one)){
_throw( Null_Exception);}* _temp5708;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5709= yyls; struct Cyc_Yyltype* _temp5711= _temp5709.curr +(
yylsp_offset - 6); if( _temp5709.base == 0? 1:( _temp5711 < _temp5709.base? 1:
_temp5711 >= _temp5709.last_plus_one)){ _throw( Null_Exception);}* _temp5711;}).first_line,({
struct _tagged_ptr3 _temp5712= yyls; struct Cyc_Yyltype* _temp5714= _temp5712.curr
+ yylsp_offset; if( _temp5712.base == 0? 1:( _temp5714 < _temp5712.base? 1:
_temp5714 >= _temp5712.last_plus_one)){ _throw( Null_Exception);}* _temp5714;}).last_line));
_temp5702;}); _temp5701;}); break; case 247: _LL5700: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5716=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5716[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5717; _temp5717.tag= Cyc_Stmt_tok_tag; _temp5717.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5718=
yyvs; struct _xtunion_struct** _temp5720= _temp5718.curr +( yyvsp_offset - 4);
if( _temp5718.base == 0? 1:( _temp5720 < _temp5718.base? 1: _temp5720 >=
_temp5718.last_plus_one)){ _throw( Null_Exception);}* _temp5720;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5721= yyvs; struct _xtunion_struct** _temp5723=
_temp5721.curr +( yyvsp_offset - 2); if( _temp5721.base == 0? 1:( _temp5723 <
_temp5721.base? 1: _temp5723 >= _temp5721.last_plus_one)){ _throw(
Null_Exception);}* _temp5723;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5724= yyvs; struct _xtunion_struct** _temp5726= _temp5724.curr +
yyvsp_offset; if( _temp5724.base == 0? 1:( _temp5726 < _temp5724.base? 1:
_temp5726 >= _temp5724.last_plus_one)){ _throw( Null_Exception);}* _temp5726;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5727= yyls; struct Cyc_Yyltype*
_temp5729= _temp5727.curr +( yylsp_offset - 7); if( _temp5727.base == 0? 1:(
_temp5729 < _temp5727.base? 1: _temp5729 >= _temp5727.last_plus_one)){ _throw(
Null_Exception);}* _temp5729;}).first_line,({ struct _tagged_ptr3 _temp5730=
yyls; struct Cyc_Yyltype* _temp5732= _temp5730.curr +( yylsp_offset - 1); if(
_temp5730.base == 0? 1:( _temp5732 < _temp5730.base? 1: _temp5732 >= _temp5730.last_plus_one)){
_throw( Null_Exception);}* _temp5732;}).last_line)); _temp5717;}); _temp5716;});
break; case 248: _LL5715: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5734=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5734[ 0]=({ struct Cyc_Stmt_tok_struct _temp5735; _temp5735.tag= Cyc_Stmt_tok_tag;
_temp5735.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5736= yyvs; struct _xtunion_struct** _temp5738= _temp5736.curr +(
yyvsp_offset - 4); if( _temp5736.base == 0? 1:( _temp5738 < _temp5736.base? 1:
_temp5738 >= _temp5736.last_plus_one)){ _throw( Null_Exception);}* _temp5738;})),
Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5739= yyvs; struct _xtunion_struct** _temp5741= _temp5739.curr
+ yyvsp_offset; if( _temp5739.base == 0? 1:( _temp5741 < _temp5739.base? 1:
_temp5741 >= _temp5739.last_plus_one)){ _throw( Null_Exception);}* _temp5741;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5742= yyls; struct Cyc_Yyltype*
_temp5744= _temp5742.curr +( yylsp_offset - 6); if( _temp5742.base == 0? 1:(
_temp5744 < _temp5742.base? 1: _temp5744 >= _temp5742.last_plus_one)){ _throw(
Null_Exception);}* _temp5744;}).first_line,({ struct _tagged_ptr3 _temp5745=
yyls; struct Cyc_Yyltype* _temp5747= _temp5745.curr + yylsp_offset; if(
_temp5745.base == 0? 1:( _temp5747 < _temp5745.base? 1: _temp5747 >= _temp5745.last_plus_one)){
_throw( Null_Exception);}* _temp5747;}).last_line)); _temp5735;}); _temp5734;});
break; case 249: _LL5733: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5749=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5749[ 0]=({ struct Cyc_Stmt_tok_struct _temp5750; _temp5750.tag= Cyc_Stmt_tok_tag;
_temp5750.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5751= yyvs; struct _xtunion_struct** _temp5753= _temp5751.curr +(
yyvsp_offset - 5); if( _temp5751.base == 0? 1:( _temp5753 < _temp5751.base? 1:
_temp5753 >= _temp5751.last_plus_one)){ _throw( Null_Exception);}* _temp5753;})),
Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5754= yyvs;
struct _xtunion_struct** _temp5756= _temp5754.curr +( yyvsp_offset - 2); if(
_temp5754.base == 0? 1:( _temp5756 < _temp5754.base? 1: _temp5756 >= _temp5754.last_plus_one)){
_throw( Null_Exception);}* _temp5756;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5757= yyvs; struct _xtunion_struct** _temp5759= _temp5757.curr
+ yyvsp_offset; if( _temp5757.base == 0? 1:( _temp5759 < _temp5757.base? 1:
_temp5759 >= _temp5757.last_plus_one)){ _throw( Null_Exception);}* _temp5759;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5760= yyls; struct Cyc_Yyltype*
_temp5762= _temp5760.curr +( yylsp_offset - 7); if( _temp5760.base == 0? 1:(
_temp5762 < _temp5760.base? 1: _temp5762 >= _temp5760.last_plus_one)){ _throw(
Null_Exception);}* _temp5762;}).first_line,({ struct _tagged_ptr3 _temp5763=
yyls; struct Cyc_Yyltype* _temp5765= _temp5763.curr + yylsp_offset; if(
_temp5763.base == 0? 1:( _temp5765 < _temp5763.base? 1: _temp5765 >= _temp5763.last_plus_one)){
_throw( Null_Exception);}* _temp5765;}).last_line)); _temp5750;}); _temp5749;});
break; case 250: _LL5748: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5767=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5767[ 0]=({ struct Cyc_Stmt_tok_struct _temp5768; _temp5768.tag= Cyc_Stmt_tok_tag;
_temp5768.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5769= yyvs; struct _xtunion_struct** _temp5771= _temp5769.curr +(
yyvsp_offset - 5); if( _temp5769.base == 0? 1:( _temp5771 < _temp5769.base? 1:
_temp5771 >= _temp5769.last_plus_one)){ _throw( Null_Exception);}* _temp5771;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5772= yyvs; struct _xtunion_struct**
_temp5774= _temp5772.curr +( yyvsp_offset - 3); if( _temp5772.base == 0? 1:(
_temp5774 < _temp5772.base? 1: _temp5774 >= _temp5772.last_plus_one)){ _throw(
Null_Exception);}* _temp5774;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5775= yyvs; struct _xtunion_struct** _temp5777=
_temp5775.curr + yyvsp_offset; if( _temp5775.base == 0? 1:( _temp5777 <
_temp5775.base? 1: _temp5777 >= _temp5775.last_plus_one)){ _throw(
Null_Exception);}* _temp5777;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5778= yyls; struct Cyc_Yyltype* _temp5780= _temp5778.curr +(
yylsp_offset - 7); if( _temp5778.base == 0? 1:( _temp5780 < _temp5778.base? 1:
_temp5780 >= _temp5778.last_plus_one)){ _throw( Null_Exception);}* _temp5780;}).first_line,({
struct _tagged_ptr3 _temp5781= yyls; struct Cyc_Yyltype* _temp5783= _temp5781.curr
+ yylsp_offset; if( _temp5781.base == 0? 1:( _temp5783 < _temp5781.base? 1:
_temp5783 >= _temp5781.last_plus_one)){ _throw( Null_Exception);}* _temp5783;}).last_line));
_temp5768;}); _temp5767;}); break; case 251: _LL5766: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5785=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5785[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5786; _temp5786.tag= Cyc_Stmt_tok_tag; _temp5786.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5787= yyvs; struct _xtunion_struct**
_temp5789= _temp5787.curr +( yyvsp_offset - 6); if( _temp5787.base == 0? 1:(
_temp5789 < _temp5787.base? 1: _temp5789 >= _temp5787.last_plus_one)){ _throw(
Null_Exception);}* _temp5789;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5790= yyvs; struct _xtunion_struct** _temp5792= _temp5790.curr +(
yyvsp_offset - 4); if( _temp5790.base == 0? 1:( _temp5792 < _temp5790.base? 1:
_temp5792 >= _temp5790.last_plus_one)){ _throw( Null_Exception);}* _temp5792;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5793= yyvs; struct _xtunion_struct**
_temp5795= _temp5793.curr +( yyvsp_offset - 2); if( _temp5793.base == 0? 1:(
_temp5795 < _temp5793.base? 1: _temp5795 >= _temp5793.last_plus_one)){ _throw(
Null_Exception);}* _temp5795;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5796= yyvs; struct _xtunion_struct** _temp5798= _temp5796.curr +
yyvsp_offset; if( _temp5796.base == 0? 1:( _temp5798 < _temp5796.base? 1:
_temp5798 >= _temp5796.last_plus_one)){ _throw( Null_Exception);}* _temp5798;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5799= yyls; struct Cyc_Yyltype*
_temp5801= _temp5799.curr +( yylsp_offset - 8); if( _temp5799.base == 0? 1:(
_temp5801 < _temp5799.base? 1: _temp5801 >= _temp5799.last_plus_one)){ _throw(
Null_Exception);}* _temp5801;}).first_line,({ struct _tagged_ptr3 _temp5802=
yyls; struct Cyc_Yyltype* _temp5804= _temp5802.curr + yylsp_offset; if(
_temp5802.base == 0? 1:( _temp5804 < _temp5802.base? 1: _temp5804 >= _temp5802.last_plus_one)){
_throw( Null_Exception);}* _temp5804;}).last_line)); _temp5786;}); _temp5785;});
break; case 252: _LL5784: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5806= yyvs; struct _xtunion_struct** _temp5808=
_temp5806.curr +( yyvsp_offset - 3); if( _temp5806.base == 0? 1:( _temp5808 <
_temp5806.base? 1: _temp5808 >= _temp5806.last_plus_one)){ _throw(
Null_Exception);}* _temp5808;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5809= yyvs; struct _xtunion_struct** _temp5811=
_temp5809.curr + yyvsp_offset; if( _temp5809.base == 0? 1:( _temp5811 <
_temp5809.base? 1: _temp5811 >= _temp5809.last_plus_one)){ _throw(
Null_Exception);}* _temp5811;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5812= yyls; struct Cyc_Yyltype* _temp5814= _temp5812.curr +(
yylsp_offset - 5); if( _temp5812.base == 0? 1:( _temp5814 < _temp5812.base? 1:
_temp5814 >= _temp5812.last_plus_one)){ _throw( Null_Exception);}* _temp5814;}).first_line,({
struct _tagged_ptr3 _temp5815= yyls; struct Cyc_Yyltype* _temp5817= _temp5815.curr
+ yylsp_offset; if( _temp5815.base == 0? 1:( _temp5817 < _temp5815.base? 1:
_temp5817 >= _temp5815.last_plus_one)){ _throw( Null_Exception);}* _temp5817;}).last_line));
yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5818=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5818[ 0]=({ struct Cyc_Stmt_tok_struct _temp5819; _temp5819.tag= Cyc_Stmt_tok_tag;
_temp5819.f1= Cyc_Parse_flatten_declarations( decls, s); _temp5819;}); _temp5818;});
break;} case 253: _LL5805: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5821= yyvs; struct _xtunion_struct** _temp5823=
_temp5821.curr +( yyvsp_offset - 4); if( _temp5821.base == 0? 1:( _temp5823 <
_temp5821.base? 1: _temp5823 >= _temp5821.last_plus_one)){ _throw(
Null_Exception);}* _temp5823;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5824=
yyvs; struct _xtunion_struct** _temp5826= _temp5824.curr +( yyvsp_offset - 3);
if( _temp5824.base == 0? 1:( _temp5826 < _temp5824.base? 1: _temp5826 >=
_temp5824.last_plus_one)){ _throw( Null_Exception);}* _temp5826;})), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5827= yyvs; struct
_xtunion_struct** _temp5829= _temp5827.curr + yyvsp_offset; if( _temp5827.base
== 0? 1:( _temp5829 < _temp5827.base? 1: _temp5829 >= _temp5827.last_plus_one)){
_throw( Null_Exception);}* _temp5829;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5830= yyls; struct Cyc_Yyltype* _temp5832= _temp5830.curr +(
yylsp_offset - 6); if( _temp5830.base == 0? 1:( _temp5832 < _temp5830.base? 1:
_temp5832 >= _temp5830.last_plus_one)){ _throw( Null_Exception);}* _temp5832;}).first_line,({
struct _tagged_ptr3 _temp5833= yyls; struct Cyc_Yyltype* _temp5835= _temp5833.curr
+ yylsp_offset; if( _temp5833.base == 0? 1:( _temp5835 < _temp5833.base? 1:
_temp5835 >= _temp5833.last_plus_one)){ _throw( Null_Exception);}* _temp5835;}).last_line));
yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5836=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5836[ 0]=({ struct Cyc_Stmt_tok_struct _temp5837; _temp5837.tag= Cyc_Stmt_tok_tag;
_temp5837.f1= Cyc_Parse_flatten_declarations( decls, s); _temp5837;}); _temp5836;});
break;} case 254: _LL5820: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5839= yyvs; struct _xtunion_struct** _temp5841=
_temp5839.curr +( yyvsp_offset - 4); if( _temp5839.base == 0? 1:( _temp5841 <
_temp5839.base? 1: _temp5841 >= _temp5839.last_plus_one)){ _throw(
Null_Exception);}* _temp5841;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5842= yyvs; struct _xtunion_struct** _temp5844= _temp5842.curr
+( yyvsp_offset - 2); if( _temp5842.base == 0? 1:( _temp5844 < _temp5842.base? 1:
_temp5844 >= _temp5842.last_plus_one)){ _throw( Null_Exception);}* _temp5844;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5845= yyvs; struct
_xtunion_struct** _temp5847= _temp5845.curr + yyvsp_offset; if( _temp5845.base
== 0? 1:( _temp5847 < _temp5845.base? 1: _temp5847 >= _temp5845.last_plus_one)){
_throw( Null_Exception);}* _temp5847;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5848= yyls; struct Cyc_Yyltype* _temp5850= _temp5848.curr +(
yylsp_offset - 6); if( _temp5848.base == 0? 1:( _temp5850 < _temp5848.base? 1:
_temp5850 >= _temp5848.last_plus_one)){ _throw( Null_Exception);}* _temp5850;}).first_line,({
struct _tagged_ptr3 _temp5851= yyls; struct Cyc_Yyltype* _temp5853= _temp5851.curr
+ yylsp_offset; if( _temp5851.base == 0? 1:( _temp5853 < _temp5851.base? 1:
_temp5853 >= _temp5851.last_plus_one)){ _throw( Null_Exception);}* _temp5853;}).last_line));
yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5854=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5854[ 0]=({ struct Cyc_Stmt_tok_struct _temp5855; _temp5855.tag= Cyc_Stmt_tok_tag;
_temp5855.f1= Cyc_Parse_flatten_declarations( decls, s); _temp5855;}); _temp5854;});
break;} case 255: _LL5838: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5857= yyvs; struct _xtunion_struct** _temp5859=
_temp5857.curr +( yyvsp_offset - 5); if( _temp5857.base == 0? 1:( _temp5859 <
_temp5857.base? 1: _temp5859 >= _temp5857.last_plus_one)){ _throw(
Null_Exception);}* _temp5859;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5860=
yyvs; struct _xtunion_struct** _temp5862= _temp5860.curr +( yyvsp_offset - 4);
if( _temp5860.base == 0? 1:( _temp5862 < _temp5860.base? 1: _temp5862 >=
_temp5860.last_plus_one)){ _throw( Null_Exception);}* _temp5862;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5863= yyvs; struct _xtunion_struct** _temp5865=
_temp5863.curr +( yyvsp_offset - 2); if( _temp5863.base == 0? 1:( _temp5865 <
_temp5863.base? 1: _temp5865 >= _temp5863.last_plus_one)){ _throw(
Null_Exception);}* _temp5865;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5866= yyvs; struct _xtunion_struct** _temp5868= _temp5866.curr +
yyvsp_offset; if( _temp5866.base == 0? 1:( _temp5868 < _temp5866.base? 1:
_temp5868 >= _temp5866.last_plus_one)){ _throw( Null_Exception);}* _temp5868;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5869= yyls; struct Cyc_Yyltype*
_temp5871= _temp5869.curr +( yylsp_offset - 7); if( _temp5869.base == 0? 1:(
_temp5871 < _temp5869.base? 1: _temp5871 >= _temp5869.last_plus_one)){ _throw(
Null_Exception);}* _temp5871;}).first_line,({ struct _tagged_ptr3 _temp5872=
yyls; struct Cyc_Yyltype* _temp5874= _temp5872.curr + yylsp_offset; if(
_temp5872.base == 0? 1:( _temp5874 < _temp5872.base? 1: _temp5874 >= _temp5872.last_plus_one)){
_throw( Null_Exception);}* _temp5874;}).last_line)); yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5875=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5875[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5876; _temp5876.tag= Cyc_Stmt_tok_tag; _temp5876.f1= Cyc_Parse_flatten_declarations(
decls, s); _temp5876;}); _temp5875;}); break;} case 256: _LL5856: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5878=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5878[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5879; _temp5879.tag= Cyc_Stmt_tok_tag; _temp5879.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_string* _temp5880=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp5880[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5881= yyvs; struct _xtunion_struct** _temp5883= _temp5881.curr
+( yyvsp_offset - 1); if( _temp5881.base == 0? 1:( _temp5883 < _temp5881.base? 1:
_temp5883 >= _temp5881.last_plus_one)){ _throw( Null_Exception);}* _temp5883;}));
_temp5880;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5884= yyls;
struct Cyc_Yyltype* _temp5886= _temp5884.curr +( yylsp_offset - 2); if(
_temp5884.base == 0? 1:( _temp5886 < _temp5884.base? 1: _temp5886 >= _temp5884.last_plus_one)){
_throw( Null_Exception);}* _temp5886;}).first_line,({ struct _tagged_ptr3
_temp5887= yyls; struct Cyc_Yyltype* _temp5889= _temp5887.curr +( yylsp_offset -
1); if( _temp5887.base == 0? 1:( _temp5889 < _temp5887.base? 1: _temp5889 >=
_temp5887.last_plus_one)){ _throw( Null_Exception);}* _temp5889;}).last_line));
_temp5879;}); _temp5878;}); break; case 257: _LL5877: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5891=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5891[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5892; _temp5892.tag= Cyc_Stmt_tok_tag; _temp5892.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5893= yyls; struct Cyc_Yyltype*
_temp5895= _temp5893.curr +( yylsp_offset - 1); if( _temp5893.base == 0? 1:(
_temp5895 < _temp5893.base? 1: _temp5895 >= _temp5893.last_plus_one)){ _throw(
Null_Exception);}* _temp5895;}).first_line,({ struct _tagged_ptr3 _temp5896=
yyls; struct Cyc_Yyltype* _temp5898= _temp5896.curr +( yylsp_offset - 1); if(
_temp5896.base == 0? 1:( _temp5898 < _temp5896.base? 1: _temp5898 >= _temp5896.last_plus_one)){
_throw( Null_Exception);}* _temp5898;}).last_line)); _temp5892;}); _temp5891;});
break; case 258: _LL5890: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5900=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5900[ 0]=({ struct Cyc_Stmt_tok_struct _temp5901; _temp5901.tag= Cyc_Stmt_tok_tag;
_temp5901.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5902= yyls; struct Cyc_Yyltype* _temp5904= _temp5902.curr +(
yylsp_offset - 1); if( _temp5902.base == 0? 1:( _temp5904 < _temp5902.base? 1:
_temp5904 >= _temp5902.last_plus_one)){ _throw( Null_Exception);}* _temp5904;}).first_line,({
struct _tagged_ptr3 _temp5905= yyls; struct Cyc_Yyltype* _temp5907= _temp5905.curr
+( yylsp_offset - 1); if( _temp5905.base == 0? 1:( _temp5907 < _temp5905.base? 1:
_temp5907 >= _temp5905.last_plus_one)){ _throw( Null_Exception);}* _temp5907;}).last_line));
_temp5901;}); _temp5900;}); break; case 259: _LL5899: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5909=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5909[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5910; _temp5910.tag= Cyc_Stmt_tok_tag; _temp5910.f1= Cyc_Absyn_return_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5911= yyls; struct
Cyc_Yyltype* _temp5913= _temp5911.curr +( yylsp_offset - 1); if( _temp5911.base
== 0? 1:( _temp5913 < _temp5911.base? 1: _temp5913 >= _temp5911.last_plus_one)){
_throw( Null_Exception);}* _temp5913;}).first_line,({ struct _tagged_ptr3
_temp5914= yyls; struct Cyc_Yyltype* _temp5916= _temp5914.curr +( yylsp_offset -
1); if( _temp5914.base == 0? 1:( _temp5916 < _temp5914.base? 1: _temp5916 >=
_temp5914.last_plus_one)){ _throw( Null_Exception);}* _temp5916;}).last_line));
_temp5910;}); _temp5909;}); break; case 260: _LL5908: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5918=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5918[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5919; _temp5919.tag= Cyc_Stmt_tok_tag; _temp5919.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5920= yyvs;
struct _xtunion_struct** _temp5922= _temp5920.curr +( yyvsp_offset - 1); if(
_temp5920.base == 0? 1:( _temp5922 < _temp5920.base? 1: _temp5922 >= _temp5920.last_plus_one)){
_throw( Null_Exception);}* _temp5922;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5923= yyls; struct Cyc_Yyltype* _temp5925= _temp5923.curr +(
yylsp_offset - 2); if( _temp5923.base == 0? 1:( _temp5925 < _temp5923.base? 1:
_temp5925 >= _temp5923.last_plus_one)){ _throw( Null_Exception);}* _temp5925;}).first_line,({
struct _tagged_ptr3 _temp5926= yyls; struct Cyc_Yyltype* _temp5928= _temp5926.curr
+( yylsp_offset - 1); if( _temp5926.base == 0? 1:( _temp5928 < _temp5926.base? 1:
_temp5928 >= _temp5926.last_plus_one)){ _throw( Null_Exception);}* _temp5928;}).last_line));
_temp5919;}); _temp5918;}); break; case 261: _LL5917: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5930=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5930[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5931; _temp5931.tag= Cyc_Stmt_tok_tag; _temp5931.f1= Cyc_Absyn_fallthru_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5932= yyls; struct
Cyc_Yyltype* _temp5934= _temp5932.curr +( yylsp_offset - 1); if( _temp5932.base
== 0? 1:( _temp5934 < _temp5932.base? 1: _temp5934 >= _temp5932.last_plus_one)){
_throw( Null_Exception);}* _temp5934;}).first_line,({ struct _tagged_ptr3
_temp5935= yyls; struct Cyc_Yyltype* _temp5937= _temp5935.curr +( yylsp_offset -
1); if( _temp5935.base == 0? 1:( _temp5937 < _temp5935.base? 1: _temp5937 >=
_temp5935.last_plus_one)){ _throw( Null_Exception);}* _temp5937;}).last_line));
_temp5931;}); _temp5930;}); break; case 262: _LL5929: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5939=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5939[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5940; _temp5940.tag= Cyc_Stmt_tok_tag; _temp5940.f1= Cyc_Absyn_fallthru_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5941= yyls; struct
Cyc_Yyltype* _temp5943= _temp5941.curr +( yylsp_offset - 3); if( _temp5941.base
== 0? 1:( _temp5943 < _temp5941.base? 1: _temp5943 >= _temp5941.last_plus_one)){
_throw( Null_Exception);}* _temp5943;}).first_line,({ struct _tagged_ptr3
_temp5944= yyls; struct Cyc_Yyltype* _temp5946= _temp5944.curr +( yylsp_offset -
3); if( _temp5944.base == 0? 1:( _temp5946 < _temp5944.base? 1: _temp5946 >=
_temp5944.last_plus_one)){ _throw( Null_Exception);}* _temp5946;}).last_line));
_temp5940;}); _temp5939;}); break; case 263: _LL5938: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5948=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5948[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5949; _temp5949.tag= Cyc_Stmt_tok_tag; _temp5949.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp5950= yyvs; struct
_xtunion_struct** _temp5952= _temp5950.curr +( yyvsp_offset - 2); if( _temp5950.base
== 0? 1:( _temp5952 < _temp5950.base? 1: _temp5952 >= _temp5950.last_plus_one)){
_throw( Null_Exception);}* _temp5952;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5953= yyls; struct Cyc_Yyltype* _temp5955= _temp5953.curr +(
yylsp_offset - 4); if( _temp5953.base == 0? 1:( _temp5955 < _temp5953.base? 1:
_temp5955 >= _temp5953.last_plus_one)){ _throw( Null_Exception);}* _temp5955;}).first_line,({
struct _tagged_ptr3 _temp5956= yyls; struct Cyc_Yyltype* _temp5958= _temp5956.curr
+( yylsp_offset - 4); if( _temp5956.base == 0? 1:( _temp5958 < _temp5956.base? 1:
_temp5958 >= _temp5956.last_plus_one)){ _throw( Null_Exception);}* _temp5958;}).last_line));
_temp5949;}); _temp5948;}); break; case 264: _LL5947: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5960=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5960[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5961; _temp5961.tag= Cyc_Pattern_tok_tag; _temp5961.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5962= yyls; struct Cyc_Yyltype* _temp5964= _temp5962.curr + yylsp_offset;
if( _temp5962.base == 0? 1:( _temp5964 < _temp5962.base? 1: _temp5964 >=
_temp5962.last_plus_one)){ _throw( Null_Exception);}* _temp5964;}).first_line,({
struct _tagged_ptr3 _temp5965= yyls; struct Cyc_Yyltype* _temp5967= _temp5965.curr
+ yylsp_offset; if( _temp5965.base == 0? 1:( _temp5967 < _temp5965.base? 1:
_temp5967 >= _temp5965.last_plus_one)){ _throw( Null_Exception);}* _temp5967;}).last_line));
_temp5961;}); _temp5960;}); break; case 265: _LL5959: yyval=({ struct
_tagged_ptr2 _temp5969= yyvs; struct _xtunion_struct** _temp5971= _temp5969.curr
+( yyvsp_offset - 1); if( _temp5969.base == 0? 1:( _temp5971 < _temp5969.base? 1:
_temp5971 >= _temp5969.last_plus_one)){ _throw( Null_Exception);}* _temp5971;});
break; case 266: _LL5968: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5973=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5973[ 0]=({ struct Cyc_Pattern_tok_struct _temp5974; _temp5974.tag= Cyc_Pattern_tok_tag;
_temp5974.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp5975=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp5975[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp5976; _temp5976.tag= Cyc_Absyn_Int_p_tag;
_temp5976.f1=( void*)(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp5977= yyvs;
struct _xtunion_struct** _temp5979= _temp5977.curr + yyvsp_offset; if( _temp5977.base
== 0? 1:( _temp5979 < _temp5977.base? 1: _temp5979 >= _temp5977.last_plus_one)){
_throw( Null_Exception);}* _temp5979;}))).f1; _temp5976.f2=(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5980= yyvs; struct _xtunion_struct** _temp5982=
_temp5980.curr + yyvsp_offset; if( _temp5980.base == 0? 1:( _temp5982 <
_temp5980.base? 1: _temp5982 >= _temp5980.last_plus_one)){ _throw(
Null_Exception);}* _temp5982;}))).f2; _temp5976;}); _temp5975;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5983= yyls; struct Cyc_Yyltype* _temp5985= _temp5983.curr
+ yylsp_offset; if( _temp5983.base == 0? 1:( _temp5985 < _temp5983.base? 1:
_temp5985 >= _temp5983.last_plus_one)){ _throw( Null_Exception);}* _temp5985;}).first_line,({
struct _tagged_ptr3 _temp5986= yyls; struct Cyc_Yyltype* _temp5988= _temp5986.curr
+ yylsp_offset; if( _temp5986.base == 0? 1:( _temp5988 < _temp5986.base? 1:
_temp5988 >= _temp5986.last_plus_one)){ _throw( Null_Exception);}* _temp5988;}).last_line));
_temp5974;}); _temp5973;}); break; case 267: _LL5972: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5990=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5990[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5991; _temp5991.tag= Cyc_Pattern_tok_tag; _temp5991.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp5992=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp5992[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp5993; _temp5993.tag= Cyc_Absyn_Int_p_tag; _temp5993.f1=( void*)(( void*)
Cyc_Absyn_Signed); _temp5993.f2= -(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2
_temp5994= yyvs; struct _xtunion_struct** _temp5996= _temp5994.curr +
yyvsp_offset; if( _temp5994.base == 0? 1:( _temp5996 < _temp5994.base? 1:
_temp5996 >= _temp5994.last_plus_one)){ _throw( Null_Exception);}* _temp5996;}))).f2;
_temp5993;}); _temp5992;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5997= yyls; struct Cyc_Yyltype* _temp5999= _temp5997.curr +( yylsp_offset -
1); if( _temp5997.base == 0? 1:( _temp5999 < _temp5997.base? 1: _temp5999 >=
_temp5997.last_plus_one)){ _throw( Null_Exception);}* _temp5999;}).first_line,({
struct _tagged_ptr3 _temp6000= yyls; struct Cyc_Yyltype* _temp6002= _temp6000.curr
+ yylsp_offset; if( _temp6000.base == 0? 1:( _temp6002 < _temp6000.base? 1:
_temp6002 >= _temp6000.last_plus_one)){ _throw( Null_Exception);}* _temp6002;}).last_line));
_temp5991;}); _temp5990;}); break; case 268: _LL5989: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6004=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6004[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6005; _temp6005.tag= Cyc_Pattern_tok_tag; _temp6005.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp6006=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp6006[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp6007; _temp6007.tag= Cyc_Absyn_Float_p_tag;
_temp6007.f1= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6008= yyvs;
struct _xtunion_struct** _temp6010= _temp6008.curr + yyvsp_offset; if( _temp6008.base
== 0? 1:( _temp6010 < _temp6008.base? 1: _temp6010 >= _temp6008.last_plus_one)){
_throw( Null_Exception);}* _temp6010;})); _temp6007;}); _temp6006;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6011= yyls; struct Cyc_Yyltype* _temp6013= _temp6011.curr
+ yylsp_offset; if( _temp6011.base == 0? 1:( _temp6013 < _temp6011.base? 1:
_temp6013 >= _temp6011.last_plus_one)){ _throw( Null_Exception);}* _temp6013;}).first_line,({
struct _tagged_ptr3 _temp6014= yyls; struct Cyc_Yyltype* _temp6016= _temp6014.curr
+ yylsp_offset; if( _temp6014.base == 0? 1:( _temp6016 < _temp6014.base? 1:
_temp6016 >= _temp6014.last_plus_one)){ _throw( Null_Exception);}* _temp6016;}).last_line));
_temp6005;}); _temp6004;}); break; case 269: _LL6003: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6018=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6018[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6019; _temp6019.tag= Cyc_Pattern_tok_tag; _temp6019.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Char_p_struct* _temp6020=( struct Cyc_Absyn_Char_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_p_struct)); _temp6020[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp6021; _temp6021.tag= Cyc_Absyn_Char_p_tag; _temp6021.f1= Cyc_yyget_Char_tok(({
struct _tagged_ptr2 _temp6022= yyvs; struct _xtunion_struct** _temp6024=
_temp6022.curr + yyvsp_offset; if( _temp6022.base == 0? 1:( _temp6024 <
_temp6022.base? 1: _temp6024 >= _temp6022.last_plus_one)){ _throw(
Null_Exception);}* _temp6024;})); _temp6021;}); _temp6020;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6025= yyls; struct Cyc_Yyltype* _temp6027= _temp6025.curr
+ yylsp_offset; if( _temp6025.base == 0? 1:( _temp6027 < _temp6025.base? 1:
_temp6027 >= _temp6025.last_plus_one)){ _throw( Null_Exception);}* _temp6027;}).first_line,({
struct _tagged_ptr3 _temp6028= yyls; struct Cyc_Yyltype* _temp6030= _temp6028.curr
+ yylsp_offset; if( _temp6028.base == 0? 1:( _temp6030 < _temp6028.base? 1:
_temp6030 >= _temp6028.last_plus_one)){ _throw( Null_Exception);}* _temp6030;}).last_line));
_temp6019;}); _temp6018;}); break; case 270: _LL6017: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6032=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6032[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6033; _temp6033.tag= Cyc_Pattern_tok_tag; _temp6033.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6034= yyls; struct Cyc_Yyltype* _temp6036= _temp6034.curr + yylsp_offset;
if( _temp6034.base == 0? 1:( _temp6036 < _temp6034.base? 1: _temp6036 >=
_temp6034.last_plus_one)){ _throw( Null_Exception);}* _temp6036;}).first_line,({
struct _tagged_ptr3 _temp6037= yyls; struct Cyc_Yyltype* _temp6039= _temp6037.curr
+ yylsp_offset; if( _temp6037.base == 0? 1:( _temp6039 < _temp6037.base? 1:
_temp6039 >= _temp6037.last_plus_one)){ _throw( Null_Exception);}* _temp6039;}).last_line));
_temp6033;}); _temp6032;}); break; case 271: _LL6031: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6041=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6041[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6042; _temp6042.tag= Cyc_Pattern_tok_tag; _temp6042.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownId_p_struct* _temp6043=( struct Cyc_Absyn_UnknownId_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct)); _temp6043[ 0]=({
struct Cyc_Absyn_UnknownId_p_struct _temp6044; _temp6044.tag= Cyc_Absyn_UnknownId_p_tag;
_temp6044.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6045= yyvs;
struct _xtunion_struct** _temp6047= _temp6045.curr + yyvsp_offset; if( _temp6045.base
== 0? 1:( _temp6047 < _temp6045.base? 1: _temp6047 >= _temp6045.last_plus_one)){
_throw( Null_Exception);}* _temp6047;})); _temp6044;}); _temp6043;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6048= yyls; struct Cyc_Yyltype* _temp6050= _temp6048.curr
+ yylsp_offset; if( _temp6048.base == 0? 1:( _temp6050 < _temp6048.base? 1:
_temp6050 >= _temp6048.last_plus_one)){ _throw( Null_Exception);}* _temp6050;}).first_line,({
struct _tagged_ptr3 _temp6051= yyls; struct Cyc_Yyltype* _temp6053= _temp6051.curr
+ yylsp_offset; if( _temp6051.base == 0? 1:( _temp6053 < _temp6051.base? 1:
_temp6053 >= _temp6051.last_plus_one)){ _throw( Null_Exception);}* _temp6053;}).last_line));
_temp6042;}); _temp6041;}); break; case 272: _LL6040: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6055=
yyls; struct Cyc_Yyltype* _temp6057= _temp6055.curr +( yylsp_offset - 3); if(
_temp6055.base == 0? 1:( _temp6057 < _temp6055.base? 1: _temp6057 >= _temp6055.last_plus_one)){
_throw( Null_Exception);}* _temp6057;}).first_line,({ struct _tagged_ptr3
_temp6058= yyls; struct Cyc_Yyltype* _temp6060= _temp6058.curr +( yylsp_offset -
3); if( _temp6058.base == 0? 1:( _temp6060 < _temp6058.base? 1: _temp6060 >=
_temp6058.last_plus_one)){ _throw( Null_Exception);}* _temp6060;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6061= yyvs; struct
_xtunion_struct** _temp6063= _temp6061.curr +( yyvsp_offset - 3); if( _temp6061.base
== 0? 1:( _temp6063 < _temp6061.base? 1: _temp6063 >= _temp6061.last_plus_one)){
_throw( Null_Exception);}* _temp6063;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_Pattern_tok_struct* _temp6064=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6064[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6065; _temp6065.tag= Cyc_Pattern_tok_tag; _temp6065.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownCall_p_struct* _temp6066=( struct Cyc_Absyn_UnknownCall_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownCall_p_struct)); _temp6066[ 0]=({
struct Cyc_Absyn_UnknownCall_p_struct _temp6067; _temp6067.tag= Cyc_Absyn_UnknownCall_p_tag;
_temp6067.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6068= yyvs;
struct _xtunion_struct** _temp6070= _temp6068.curr +( yyvsp_offset - 4); if(
_temp6068.base == 0? 1:( _temp6070 < _temp6068.base? 1: _temp6070 >= _temp6068.last_plus_one)){
_throw( Null_Exception);}* _temp6070;})); _temp6067.f2= tvs; _temp6067.f3= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp6071= yyvs; struct _xtunion_struct** _temp6073=
_temp6071.curr +( yyvsp_offset - 1); if( _temp6071.base == 0? 1:( _temp6073 <
_temp6071.base? 1: _temp6073 >= _temp6071.last_plus_one)){ _throw(
Null_Exception);}* _temp6073;})); _temp6067;}); _temp6066;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6074= yyls; struct Cyc_Yyltype* _temp6076= _temp6074.curr
+( yylsp_offset - 4); if( _temp6074.base == 0? 1:( _temp6076 < _temp6074.base? 1:
_temp6076 >= _temp6074.last_plus_one)){ _throw( Null_Exception);}* _temp6076;}).first_line,({
struct _tagged_ptr3 _temp6077= yyls; struct Cyc_Yyltype* _temp6079= _temp6077.curr
+ yylsp_offset; if( _temp6077.base == 0? 1:( _temp6079 < _temp6077.base? 1:
_temp6079 >= _temp6077.last_plus_one)){ _throw( Null_Exception);}* _temp6079;}).last_line));
_temp6065;}); _temp6064;}); break;} case 273: _LL6054: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6081=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6081[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6082; _temp6082.tag= Cyc_Pattern_tok_tag; _temp6082.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Tuple_p_struct* _temp6083=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp6083[ 0]=({ struct
Cyc_Absyn_Tuple_p_struct _temp6084; _temp6084.tag= Cyc_Absyn_Tuple_p_tag;
_temp6084.f1= Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp6085= yyvs;
struct _xtunion_struct** _temp6087= _temp6085.curr +( yyvsp_offset - 1); if(
_temp6085.base == 0? 1:( _temp6087 < _temp6085.base? 1: _temp6087 >= _temp6085.last_plus_one)){
_throw( Null_Exception);}* _temp6087;})); _temp6084;}); _temp6083;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6088= yyls; struct Cyc_Yyltype* _temp6090= _temp6088.curr
+( yylsp_offset - 3); if( _temp6088.base == 0? 1:( _temp6090 < _temp6088.base? 1:
_temp6090 >= _temp6088.last_plus_one)){ _throw( Null_Exception);}* _temp6090;}).first_line,({
struct _tagged_ptr3 _temp6091= yyls; struct Cyc_Yyltype* _temp6093= _temp6091.curr
+ yylsp_offset; if( _temp6091.base == 0? 1:( _temp6093 < _temp6091.base? 1:
_temp6093 >= _temp6091.last_plus_one)){ _throw( Null_Exception);}* _temp6093;}).last_line));
_temp6082;}); _temp6081;}); break; case 274: _LL6080: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6095=
yyls; struct Cyc_Yyltype* _temp6097= _temp6095.curr +( yylsp_offset - 2); if(
_temp6095.base == 0? 1:( _temp6097 < _temp6095.base? 1: _temp6097 >= _temp6095.last_plus_one)){
_throw( Null_Exception);}* _temp6097;}).first_line,({ struct _tagged_ptr3
_temp6098= yyls; struct Cyc_Yyltype* _temp6100= _temp6098.curr +( yylsp_offset -
2); if( _temp6098.base == 0? 1:( _temp6100 < _temp6098.base? 1: _temp6100 >=
_temp6098.last_plus_one)){ _throw( Null_Exception);}* _temp6100;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6101= yyvs; struct
_xtunion_struct** _temp6103= _temp6101.curr +( yyvsp_offset - 2); if( _temp6101.base
== 0? 1:( _temp6103 < _temp6101.base? 1: _temp6103 >= _temp6101.last_plus_one)){
_throw( Null_Exception);}* _temp6103;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_Pattern_tok_struct* _temp6104=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6104[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6105; _temp6105.tag= Cyc_Pattern_tok_tag; _temp6105.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp6106=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp6106[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp6107; _temp6107.tag= Cyc_Absyn_UnknownFields_p_tag;
_temp6107.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6108= yyvs;
struct _xtunion_struct** _temp6110= _temp6108.curr +( yyvsp_offset - 3); if(
_temp6108.base == 0? 1:( _temp6110 < _temp6108.base? 1: _temp6110 >= _temp6108.last_plus_one)){
_throw( Null_Exception);}* _temp6110;})); _temp6107.f2= tvs; _temp6107.f3= 0;
_temp6107;}); _temp6106;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6111= yyls; struct Cyc_Yyltype* _temp6113= _temp6111.curr +( yylsp_offset -
3); if( _temp6111.base == 0? 1:( _temp6113 < _temp6111.base? 1: _temp6113 >=
_temp6111.last_plus_one)){ _throw( Null_Exception);}* _temp6113;}).first_line,({
struct _tagged_ptr3 _temp6114= yyls; struct Cyc_Yyltype* _temp6116= _temp6114.curr
+ yylsp_offset; if( _temp6114.base == 0? 1:( _temp6116 < _temp6114.base? 1:
_temp6116 >= _temp6114.last_plus_one)){ _throw( Null_Exception);}* _temp6116;}).last_line));
_temp6105;}); _temp6104;}); break;} case 275: _LL6094: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6118=
yyls; struct Cyc_Yyltype* _temp6120= _temp6118.curr +( yylsp_offset - 3); if(
_temp6118.base == 0? 1:( _temp6120 < _temp6118.base? 1: _temp6120 >= _temp6118.last_plus_one)){
_throw( Null_Exception);}* _temp6120;}).first_line,({ struct _tagged_ptr3
_temp6121= yyls; struct Cyc_Yyltype* _temp6123= _temp6121.curr +( yylsp_offset -
3); if( _temp6121.base == 0? 1:( _temp6123 < _temp6121.base? 1: _temp6123 >=
_temp6121.last_plus_one)){ _throw( Null_Exception);}* _temp6123;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6124= yyvs; struct
_xtunion_struct** _temp6126= _temp6124.curr +( yyvsp_offset - 3); if( _temp6124.base
== 0? 1:( _temp6126 < _temp6124.base? 1: _temp6126 >= _temp6124.last_plus_one)){
_throw( Null_Exception);}* _temp6126;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_Pattern_tok_struct* _temp6127=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6127[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6128; _temp6128.tag= Cyc_Pattern_tok_tag; _temp6128.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp6129=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp6129[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp6130; _temp6130.tag= Cyc_Absyn_UnknownFields_p_tag;
_temp6130.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6131= yyvs;
struct _xtunion_struct** _temp6133= _temp6131.curr +( yyvsp_offset - 4); if(
_temp6131.base == 0? 1:( _temp6133 < _temp6131.base? 1: _temp6133 >= _temp6131.last_plus_one)){
_throw( Null_Exception);}* _temp6133;})); _temp6130.f2= tvs; _temp6130.f3= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp6134= yyvs; struct _xtunion_struct** _temp6136=
_temp6134.curr +( yyvsp_offset - 1); if( _temp6134.base == 0? 1:( _temp6136 <
_temp6134.base? 1: _temp6136 >= _temp6134.last_plus_one)){ _throw(
Null_Exception);}* _temp6136;})); _temp6130;}); _temp6129;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6137= yyls; struct Cyc_Yyltype* _temp6139= _temp6137.curr
+( yylsp_offset - 4); if( _temp6137.base == 0? 1:( _temp6139 < _temp6137.base? 1:
_temp6139 >= _temp6137.last_plus_one)){ _throw( Null_Exception);}* _temp6139;}).first_line,({
struct _tagged_ptr3 _temp6140= yyls; struct Cyc_Yyltype* _temp6142= _temp6140.curr
+ yylsp_offset; if( _temp6140.base == 0? 1:( _temp6142 < _temp6140.base? 1:
_temp6142 >= _temp6140.last_plus_one)){ _throw( Null_Exception);}* _temp6142;}).last_line));
_temp6128;}); _temp6127;}); break;} case 276: _LL6117: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6144=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6144[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6145; _temp6145.tag= Cyc_Pattern_tok_tag; _temp6145.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Pointer_p_struct* _temp6146=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp6146[ 0]=({ struct
Cyc_Absyn_Pointer_p_struct _temp6147; _temp6147.tag= Cyc_Absyn_Pointer_p_tag;
_temp6147.f1= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6148= yyvs;
struct _xtunion_struct** _temp6150= _temp6148.curr + yyvsp_offset; if( _temp6148.base
== 0? 1:( _temp6150 < _temp6148.base? 1: _temp6150 >= _temp6148.last_plus_one)){
_throw( Null_Exception);}* _temp6150;})); _temp6147;}); _temp6146;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6151= yyls; struct Cyc_Yyltype* _temp6153= _temp6151.curr
+( yylsp_offset - 1); if( _temp6151.base == 0? 1:( _temp6153 < _temp6151.base? 1:
_temp6153 >= _temp6151.last_plus_one)){ _throw( Null_Exception);}* _temp6153;}).first_line,({
struct _tagged_ptr3 _temp6154= yyls; struct Cyc_Yyltype* _temp6156= _temp6154.curr
+ yylsp_offset; if( _temp6154.base == 0? 1:( _temp6156 < _temp6154.base? 1:
_temp6156 >= _temp6154.last_plus_one)){ _throw( Null_Exception);}* _temp6156;}).last_line));
_temp6145;}); _temp6144;}); break; case 277: _LL6143: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6158=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6158[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6159; _temp6159.tag= Cyc_Pattern_tok_tag; _temp6159.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Reference_p_struct* _temp6160=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp6160[ 0]=({
struct Cyc_Absyn_Reference_p_struct _temp6161; _temp6161.tag= Cyc_Absyn_Reference_p_tag;
_temp6161.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp6162=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp6162->f1=( void*) Cyc_Absyn_Loc_n;
_temp6162->f2=({ struct _tagged_string* _temp6163=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp6163[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp6164= yyvs; struct _xtunion_struct** _temp6166=
_temp6164.curr + yyvsp_offset; if( _temp6164.base == 0? 1:( _temp6166 <
_temp6164.base? 1: _temp6166 >= _temp6164.last_plus_one)){ _throw(
Null_Exception);}* _temp6166;})); _temp6163;}); _temp6162;}),( void*) Cyc_Absyn_VoidType,
0); _temp6161;}); _temp6160;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6167= yyls; struct Cyc_Yyltype* _temp6169= _temp6167.curr +(
yylsp_offset - 1); if( _temp6167.base == 0? 1:( _temp6169 < _temp6167.base? 1:
_temp6169 >= _temp6167.last_plus_one)){ _throw( Null_Exception);}* _temp6169;}).first_line,({
struct _tagged_ptr3 _temp6170= yyls; struct Cyc_Yyltype* _temp6172= _temp6170.curr
+ yylsp_offset; if( _temp6170.base == 0? 1:( _temp6172 < _temp6170.base? 1:
_temp6172 >= _temp6170.last_plus_one)){ _throw( Null_Exception);}* _temp6172;}).last_line));
_temp6159;}); _temp6158;}); break; case 278: _LL6157: yyval=( struct
_xtunion_struct*)({ struct Cyc_PatternList_tok_struct* _temp6174=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp6174[ 0]=({ struct
Cyc_PatternList_tok_struct _temp6175; _temp6175.tag= Cyc_PatternList_tok_tag;
_temp6175.f1= 0; _temp6175;}); _temp6174;}); break; case 279: _LL6173: yyval=(
struct _xtunion_struct*)({ struct Cyc_PatternList_tok_struct* _temp6177=( struct
Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6177[ 0]=({ struct Cyc_PatternList_tok_struct _temp6178; _temp6178.tag= Cyc_PatternList_tok_tag;
_temp6178.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp6179= yyvs; struct
_xtunion_struct** _temp6181= _temp6179.curr + yyvsp_offset; if( _temp6179.base
== 0? 1:( _temp6181 < _temp6179.base? 1: _temp6181 >= _temp6179.last_plus_one)){
_throw( Null_Exception);}* _temp6181;}))); _temp6178;}); _temp6177;}); break;
case 280: _LL6176: yyval=( struct _xtunion_struct*)({ struct Cyc_PatternList_tok_struct*
_temp6183=( struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6183[ 0]=({ struct Cyc_PatternList_tok_struct _temp6184; _temp6184.tag= Cyc_PatternList_tok_tag;
_temp6184.f1=({ struct Cyc_List_List* _temp6185=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6185->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp6186= yyvs; struct _xtunion_struct** _temp6188=
_temp6186.curr + yyvsp_offset; if( _temp6186.base == 0? 1:( _temp6188 <
_temp6186.base? 1: _temp6188 >= _temp6186.last_plus_one)){ _throw(
Null_Exception);}* _temp6188;})); _temp6185->tl= 0; _temp6185;}); _temp6184;});
_temp6183;}); break; case 281: _LL6182: yyval=( struct _xtunion_struct*)({
struct Cyc_PatternList_tok_struct* _temp6190=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp6190[ 0]=({ struct
Cyc_PatternList_tok_struct _temp6191; _temp6191.tag= Cyc_PatternList_tok_tag;
_temp6191.f1=({ struct Cyc_List_List* _temp6192=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6192->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp6196= yyvs; struct _xtunion_struct** _temp6198=
_temp6196.curr + yyvsp_offset; if( _temp6196.base == 0? 1:( _temp6198 <
_temp6196.base? 1: _temp6198 >= _temp6196.last_plus_one)){ _throw(
Null_Exception);}* _temp6198;})); _temp6192->tl= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp6193= yyvs; struct _xtunion_struct** _temp6195=
_temp6193.curr +( yyvsp_offset - 2); if( _temp6193.base == 0? 1:( _temp6195 <
_temp6193.base? 1: _temp6195 >= _temp6193.last_plus_one)){ _throw(
Null_Exception);}* _temp6195;})); _temp6192;}); _temp6191;}); _temp6190;});
break; case 282: _LL6189: yyval=( struct _xtunion_struct*)({ struct Cyc_FieldPattern_tok_struct*
_temp6200=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6200[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6201; _temp6201.tag=
Cyc_FieldPattern_tok_tag; _temp6201.f1=({ struct _tuple17* _temp6202=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp6202->f1= 0; _temp6202->f2=
Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6203= yyvs; struct
_xtunion_struct** _temp6205= _temp6203.curr + yyvsp_offset; if( _temp6203.base
== 0? 1:( _temp6205 < _temp6203.base? 1: _temp6205 >= _temp6203.last_plus_one)){
_throw( Null_Exception);}* _temp6205;})); _temp6202;}); _temp6201;}); _temp6200;});
break; case 283: _LL6199: yyval=( struct _xtunion_struct*)({ struct Cyc_FieldPattern_tok_struct*
_temp6207=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6207[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6208; _temp6208.tag=
Cyc_FieldPattern_tok_tag; _temp6208.f1=({ struct _tuple17* _temp6209=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp6209->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp6213= yyvs; struct _xtunion_struct** _temp6215=
_temp6213.curr +( yyvsp_offset - 1); if( _temp6213.base == 0? 1:( _temp6215 <
_temp6213.base? 1: _temp6215 >= _temp6213.last_plus_one)){ _throw(
Null_Exception);}* _temp6215;})); _temp6209->f2= Cyc_yyget_Pattern_tok(({ struct
_tagged_ptr2 _temp6210= yyvs; struct _xtunion_struct** _temp6212= _temp6210.curr
+ yyvsp_offset; if( _temp6210.base == 0? 1:( _temp6212 < _temp6210.base? 1:
_temp6212 >= _temp6210.last_plus_one)){ _throw( Null_Exception);}* _temp6212;}));
_temp6209;}); _temp6208;}); _temp6207;}); break; case 284: _LL6206: yyval=(
struct _xtunion_struct*)({ struct Cyc_FieldPatternList_tok_struct* _temp6217=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp6217[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp6218; _temp6218.tag=
Cyc_FieldPatternList_tok_tag; _temp6218.f1=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok(({ struct
_tagged_ptr2 _temp6219= yyvs; struct _xtunion_struct** _temp6221= _temp6219.curr
+ yyvsp_offset; if( _temp6219.base == 0? 1:( _temp6221 < _temp6219.base? 1:
_temp6221 >= _temp6219.last_plus_one)){ _throw( Null_Exception);}* _temp6221;})));
_temp6218;}); _temp6217;}); break; case 285: _LL6216: yyval=( struct
_xtunion_struct*)({ struct Cyc_FieldPatternList_tok_struct* _temp6223=( struct
Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp6223[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp6224; _temp6224.tag=
Cyc_FieldPatternList_tok_tag; _temp6224.f1=({ struct Cyc_List_List* _temp6225=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp6225->hd=(
void*) Cyc_yyget_FieldPattern_tok(({ struct _tagged_ptr2 _temp6226= yyvs; struct
_xtunion_struct** _temp6228= _temp6226.curr + yyvsp_offset; if( _temp6226.base
== 0? 1:( _temp6228 < _temp6226.base? 1: _temp6228 >= _temp6226.last_plus_one)){
_throw( Null_Exception);}* _temp6228;})); _temp6225->tl= 0; _temp6225;});
_temp6224;}); _temp6223;}); break; case 286: _LL6222: yyval=( struct
_xtunion_struct*)({ struct Cyc_FieldPatternList_tok_struct* _temp6230=( struct
Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp6230[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp6231; _temp6231.tag=
Cyc_FieldPatternList_tok_tag; _temp6231.f1=({ struct Cyc_List_List* _temp6232=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp6232->hd=(
void*) Cyc_yyget_FieldPattern_tok(({ struct _tagged_ptr2 _temp6236= yyvs; struct
_xtunion_struct** _temp6238= _temp6236.curr + yyvsp_offset; if( _temp6236.base
== 0? 1:( _temp6238 < _temp6236.base? 1: _temp6238 >= _temp6236.last_plus_one)){
_throw( Null_Exception);}* _temp6238;})); _temp6232->tl= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp6233= yyvs; struct _xtunion_struct** _temp6235=
_temp6233.curr +( yyvsp_offset - 2); if( _temp6233.base == 0? 1:( _temp6235 <
_temp6233.base? 1: _temp6235 >= _temp6233.last_plus_one)){ _throw(
Null_Exception);}* _temp6235;})); _temp6232;}); _temp6231;}); _temp6230;});
break; case 287: _LL6229: yyval=({ struct _tagged_ptr2 _temp6240= yyvs; struct
_xtunion_struct** _temp6242= _temp6240.curr + yyvsp_offset; if( _temp6240.base
== 0? 1:( _temp6242 < _temp6240.base? 1: _temp6242 >= _temp6240.last_plus_one)){
_throw( Null_Exception);}* _temp6242;}); break; case 288: _LL6239: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6244=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6244[ 0]=({ struct Cyc_Exp_tok_struct
_temp6245; _temp6245.tag= Cyc_Exp_tok_tag; _temp6245.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6246= yyvs; struct _xtunion_struct** _temp6248=
_temp6246.curr +( yyvsp_offset - 2); if( _temp6246.base == 0? 1:( _temp6248 <
_temp6246.base? 1: _temp6248 >= _temp6246.last_plus_one)){ _throw(
Null_Exception);}* _temp6248;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6249= yyvs; struct _xtunion_struct** _temp6251= _temp6249.curr +
yyvsp_offset; if( _temp6249.base == 0? 1:( _temp6251 < _temp6249.base? 1:
_temp6251 >= _temp6249.last_plus_one)){ _throw( Null_Exception);}* _temp6251;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6252= yyls; struct Cyc_Yyltype*
_temp6254= _temp6252.curr +( yylsp_offset - 2); if( _temp6252.base == 0? 1:(
_temp6254 < _temp6252.base? 1: _temp6254 >= _temp6252.last_plus_one)){ _throw(
Null_Exception);}* _temp6254;}).first_line,({ struct _tagged_ptr3 _temp6255=
yyls; struct Cyc_Yyltype* _temp6257= _temp6255.curr + yylsp_offset; if(
_temp6255.base == 0? 1:( _temp6257 < _temp6255.base? 1: _temp6257 >= _temp6255.last_plus_one)){
_throw( Null_Exception);}* _temp6257;}).last_line)); _temp6245;}); _temp6244;});
break; case 289: _LL6243: yyval=({ struct _tagged_ptr2 _temp6259= yyvs; struct
_xtunion_struct** _temp6261= _temp6259.curr + yyvsp_offset; if( _temp6259.base
== 0? 1:( _temp6261 < _temp6259.base? 1: _temp6261 >= _temp6259.last_plus_one)){
_throw( Null_Exception);}* _temp6261;}); break; case 290: _LL6258: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6263=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6263[ 0]=({ struct Cyc_Exp_tok_struct
_temp6264; _temp6264.tag= Cyc_Exp_tok_tag; _temp6264.f1= Cyc_Absyn_assignop_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6265= yyvs; struct _xtunion_struct**
_temp6267= _temp6265.curr +( yyvsp_offset - 2); if( _temp6265.base == 0? 1:(
_temp6267 < _temp6265.base? 1: _temp6267 >= _temp6265.last_plus_one)){ _throw(
Null_Exception);}* _temp6267;})), Cyc_yyget_Primopopt_tok(({ struct _tagged_ptr2
_temp6268= yyvs; struct _xtunion_struct** _temp6270= _temp6268.curr +(
yyvsp_offset - 1); if( _temp6268.base == 0? 1:( _temp6270 < _temp6268.base? 1:
_temp6270 >= _temp6268.last_plus_one)){ _throw( Null_Exception);}* _temp6270;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6271= yyvs; struct _xtunion_struct**
_temp6273= _temp6271.curr + yyvsp_offset; if( _temp6271.base == 0? 1:( _temp6273
< _temp6271.base? 1: _temp6273 >= _temp6271.last_plus_one)){ _throw(
Null_Exception);}* _temp6273;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6274= yyls; struct Cyc_Yyltype* _temp6276= _temp6274.curr +(
yylsp_offset - 2); if( _temp6274.base == 0? 1:( _temp6276 < _temp6274.base? 1:
_temp6276 >= _temp6274.last_plus_one)){ _throw( Null_Exception);}* _temp6276;}).first_line,({
struct _tagged_ptr3 _temp6277= yyls; struct Cyc_Yyltype* _temp6279= _temp6277.curr
+ yylsp_offset; if( _temp6277.base == 0? 1:( _temp6279 < _temp6277.base? 1:
_temp6279 >= _temp6277.last_plus_one)){ _throw( Null_Exception);}* _temp6279;}).last_line));
_temp6264;}); _temp6263;}); break; case 291: _LL6262: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6281=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6281[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6282; _temp6282.tag= Cyc_Primopopt_tok_tag;
_temp6282.f1= 0; _temp6282;}); _temp6281;}); break; case 292: _LL6280: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6284=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6284[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6285; _temp6285.tag= Cyc_Primopopt_tok_tag;
_temp6285.f1=({ struct Cyc_Core_Opt* _temp6286=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6286->v=( void*)(( void*) Cyc_Absyn_Times);
_temp6286;}); _temp6285;}); _temp6284;}); break; case 293: _LL6283: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6288=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6288[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6289; _temp6289.tag= Cyc_Primopopt_tok_tag;
_temp6289.f1=({ struct Cyc_Core_Opt* _temp6290=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6290->v=( void*)(( void*) Cyc_Absyn_Div);
_temp6290;}); _temp6289;}); _temp6288;}); break; case 294: _LL6287: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6292=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6292[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6293; _temp6293.tag= Cyc_Primopopt_tok_tag;
_temp6293.f1=({ struct Cyc_Core_Opt* _temp6294=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6294->v=( void*)(( void*) Cyc_Absyn_Mod);
_temp6294;}); _temp6293;}); _temp6292;}); break; case 295: _LL6291: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6296=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6296[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6297; _temp6297.tag= Cyc_Primopopt_tok_tag;
_temp6297.f1=({ struct Cyc_Core_Opt* _temp6298=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6298->v=( void*)(( void*) Cyc_Absyn_Plus);
_temp6298;}); _temp6297;}); _temp6296;}); break; case 296: _LL6295: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6300=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6300[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6301; _temp6301.tag= Cyc_Primopopt_tok_tag;
_temp6301.f1=({ struct Cyc_Core_Opt* _temp6302=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6302->v=( void*)(( void*) Cyc_Absyn_Minus);
_temp6302;}); _temp6301;}); _temp6300;}); break; case 297: _LL6299: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6304=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6304[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6305; _temp6305.tag= Cyc_Primopopt_tok_tag;
_temp6305.f1=({ struct Cyc_Core_Opt* _temp6306=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6306->v=( void*)(( void*) Cyc_Absyn_Bitlshift);
_temp6306;}); _temp6305;}); _temp6304;}); break; case 298: _LL6303: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6308=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6308[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6309; _temp6309.tag= Cyc_Primopopt_tok_tag;
_temp6309.f1=({ struct Cyc_Core_Opt* _temp6310=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6310->v=( void*)(( void*) Cyc_Absyn_Bitlrshift);
_temp6310;}); _temp6309;}); _temp6308;}); break; case 299: _LL6307: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6312=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6312[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6313; _temp6313.tag= Cyc_Primopopt_tok_tag;
_temp6313.f1=({ struct Cyc_Core_Opt* _temp6314=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6314->v=( void*)(( void*) Cyc_Absyn_Bitand);
_temp6314;}); _temp6313;}); _temp6312;}); break; case 300: _LL6311: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6316=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6316[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6317; _temp6317.tag= Cyc_Primopopt_tok_tag;
_temp6317.f1=({ struct Cyc_Core_Opt* _temp6318=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6318->v=( void*)(( void*) Cyc_Absyn_Bitxor);
_temp6318;}); _temp6317;}); _temp6316;}); break; case 301: _LL6315: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6320=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6320[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6321; _temp6321.tag= Cyc_Primopopt_tok_tag;
_temp6321.f1=({ struct Cyc_Core_Opt* _temp6322=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6322->v=( void*)(( void*) Cyc_Absyn_Bitor);
_temp6322;}); _temp6321;}); _temp6320;}); break; case 302: _LL6319: yyval=({
struct _tagged_ptr2 _temp6324= yyvs; struct _xtunion_struct** _temp6326=
_temp6324.curr + yyvsp_offset; if( _temp6324.base == 0? 1:( _temp6326 <
_temp6324.base? 1: _temp6326 >= _temp6324.last_plus_one)){ _throw(
Null_Exception);}* _temp6326;}); break; case 303: _LL6323: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6328=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6328[ 0]=({ struct Cyc_Exp_tok_struct
_temp6329; _temp6329.tag= Cyc_Exp_tok_tag; _temp6329.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6330= yyvs; struct _xtunion_struct**
_temp6332= _temp6330.curr +( yyvsp_offset - 4); if( _temp6330.base == 0? 1:(
_temp6332 < _temp6330.base? 1: _temp6332 >= _temp6330.last_plus_one)){ _throw(
Null_Exception);}* _temp6332;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6333= yyvs; struct _xtunion_struct** _temp6335= _temp6333.curr +(
yyvsp_offset - 2); if( _temp6333.base == 0? 1:( _temp6335 < _temp6333.base? 1:
_temp6335 >= _temp6333.last_plus_one)){ _throw( Null_Exception);}* _temp6335;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6336= yyvs; struct _xtunion_struct**
_temp6338= _temp6336.curr + yyvsp_offset; if( _temp6336.base == 0? 1:( _temp6338
< _temp6336.base? 1: _temp6338 >= _temp6336.last_plus_one)){ _throw(
Null_Exception);}* _temp6338;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6339= yyls; struct Cyc_Yyltype* _temp6341= _temp6339.curr +(
yylsp_offset - 4); if( _temp6339.base == 0? 1:( _temp6341 < _temp6339.base? 1:
_temp6341 >= _temp6339.last_plus_one)){ _throw( Null_Exception);}* _temp6341;}).first_line,({
struct _tagged_ptr3 _temp6342= yyls; struct Cyc_Yyltype* _temp6344= _temp6342.curr
+ yylsp_offset; if( _temp6342.base == 0? 1:( _temp6344 < _temp6342.base? 1:
_temp6344 >= _temp6342.last_plus_one)){ _throw( Null_Exception);}* _temp6344;}).last_line));
_temp6329;}); _temp6328;}); break; case 304: _LL6327: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6346=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6346[ 0]=({ struct Cyc_Exp_tok_struct
_temp6347; _temp6347.tag= Cyc_Exp_tok_tag; _temp6347.f1= Cyc_Absyn_throw_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6348= yyvs; struct _xtunion_struct**
_temp6350= _temp6348.curr + yyvsp_offset; if( _temp6348.base == 0? 1:( _temp6350
< _temp6348.base? 1: _temp6350 >= _temp6348.last_plus_one)){ _throw(
Null_Exception);}* _temp6350;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6351= yyls; struct Cyc_Yyltype* _temp6353= _temp6351.curr +(
yylsp_offset - 1); if( _temp6351.base == 0? 1:( _temp6353 < _temp6351.base? 1:
_temp6353 >= _temp6351.last_plus_one)){ _throw( Null_Exception);}* _temp6353;}).first_line,({
struct _tagged_ptr3 _temp6354= yyls; struct Cyc_Yyltype* _temp6356= _temp6354.curr
+ yylsp_offset; if( _temp6354.base == 0? 1:( _temp6356 < _temp6354.base? 1:
_temp6356 >= _temp6354.last_plus_one)){ _throw( Null_Exception);}* _temp6356;}).last_line));
_temp6347;}); _temp6346;}); break; case 305: _LL6345: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6358=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6358[ 0]=({ struct Cyc_Exp_tok_struct
_temp6359; _temp6359.tag= Cyc_Exp_tok_tag; _temp6359.f1= Cyc_Absyn_New_exp( 0,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6360= yyvs; struct _xtunion_struct**
_temp6362= _temp6360.curr + yyvsp_offset; if( _temp6360.base == 0? 1:( _temp6362
< _temp6360.base? 1: _temp6362 >= _temp6360.last_plus_one)){ _throw(
Null_Exception);}* _temp6362;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6363= yyls; struct Cyc_Yyltype* _temp6365= _temp6363.curr +(
yylsp_offset - 1); if( _temp6363.base == 0? 1:( _temp6365 < _temp6363.base? 1:
_temp6365 >= _temp6363.last_plus_one)){ _throw( Null_Exception);}* _temp6365;}).first_line,({
struct _tagged_ptr3 _temp6366= yyls; struct Cyc_Yyltype* _temp6368= _temp6366.curr
+( yylsp_offset + 1); if( _temp6366.base == 0? 1:( _temp6368 < _temp6366.base? 1:
_temp6368 >= _temp6366.last_plus_one)){ _throw( Null_Exception);}* _temp6368;}).last_line));
_temp6359;}); _temp6358;}); break; case 306: _LL6357: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6370=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6370[ 0]=({ struct Cyc_Exp_tok_struct
_temp6371; _temp6371.tag= Cyc_Exp_tok_tag; _temp6371.f1= Cyc_Absyn_New_exp( 0,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6372= yyvs; struct _xtunion_struct**
_temp6374= _temp6372.curr + yyvsp_offset; if( _temp6372.base == 0? 1:( _temp6374
< _temp6372.base? 1: _temp6374 >= _temp6372.last_plus_one)){ _throw(
Null_Exception);}* _temp6374;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6375= yyls; struct Cyc_Yyltype* _temp6377= _temp6375.curr +(
yylsp_offset - 1); if( _temp6375.base == 0? 1:( _temp6377 < _temp6375.base? 1:
_temp6377 >= _temp6375.last_plus_one)){ _throw( Null_Exception);}* _temp6377;}).first_line,({
struct _tagged_ptr3 _temp6378= yyls; struct Cyc_Yyltype* _temp6380= _temp6378.curr
+( yylsp_offset + 1); if( _temp6378.base == 0? 1:( _temp6380 < _temp6378.base? 1:
_temp6380 >= _temp6378.last_plus_one)){ _throw( Null_Exception);}* _temp6380;}).last_line));
_temp6371;}); _temp6370;}); break; case 307: _LL6369: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6382=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6382[ 0]=({ struct Cyc_Exp_tok_struct
_temp6383; _temp6383.tag= Cyc_Exp_tok_tag; _temp6383.f1= Cyc_Absyn_New_exp((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6384= yyvs;
struct _xtunion_struct** _temp6386= _temp6384.curr +( yyvsp_offset - 2); if(
_temp6384.base == 0? 1:( _temp6386 < _temp6384.base? 1: _temp6386 >= _temp6384.last_plus_one)){
_throw( Null_Exception);}* _temp6386;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6387= yyvs; struct _xtunion_struct** _temp6389= _temp6387.curr
+ yyvsp_offset; if( _temp6387.base == 0? 1:( _temp6389 < _temp6387.base? 1:
_temp6389 >= _temp6387.last_plus_one)){ _throw( Null_Exception);}* _temp6389;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6390= yyls; struct Cyc_Yyltype*
_temp6392= _temp6390.curr +( yylsp_offset - 4); if( _temp6390.base == 0? 1:(
_temp6392 < _temp6390.base? 1: _temp6392 >= _temp6390.last_plus_one)){ _throw(
Null_Exception);}* _temp6392;}).first_line,({ struct _tagged_ptr3 _temp6393=
yyls; struct Cyc_Yyltype* _temp6395= _temp6393.curr + yylsp_offset; if(
_temp6393.base == 0? 1:( _temp6395 < _temp6393.base? 1: _temp6395 >= _temp6393.last_plus_one)){
_throw( Null_Exception);}* _temp6395;}).last_line)); _temp6383;}); _temp6382;});
break; case 308: _LL6381: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6397=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6397[ 0]=({ struct Cyc_Exp_tok_struct _temp6398; _temp6398.tag= Cyc_Exp_tok_tag;
_temp6398.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6399= yyvs; struct _xtunion_struct** _temp6401=
_temp6399.curr +( yyvsp_offset - 2); if( _temp6399.base == 0? 1:( _temp6401 <
_temp6399.base? 1: _temp6401 >= _temp6399.last_plus_one)){ _throw(
Null_Exception);}* _temp6401;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6402= yyvs; struct _xtunion_struct** _temp6404= _temp6402.curr +
yyvsp_offset; if( _temp6402.base == 0? 1:( _temp6404 < _temp6402.base? 1:
_temp6404 >= _temp6402.last_plus_one)){ _throw( Null_Exception);}* _temp6404;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6405= yyls; struct Cyc_Yyltype*
_temp6407= _temp6405.curr +( yylsp_offset - 4); if( _temp6405.base == 0? 1:(
_temp6407 < _temp6405.base? 1: _temp6407 >= _temp6405.last_plus_one)){ _throw(
Null_Exception);}* _temp6407;}).first_line,({ struct _tagged_ptr3 _temp6408=
yyls; struct Cyc_Yyltype* _temp6410= _temp6408.curr + yylsp_offset; if(
_temp6408.base == 0? 1:( _temp6410 < _temp6408.base? 1: _temp6410 >= _temp6408.last_plus_one)){
_throw( Null_Exception);}* _temp6410;}).last_line)); _temp6398;}); _temp6397;});
break; case 309: _LL6396: yyval=({ struct _tagged_ptr2 _temp6412= yyvs; struct
_xtunion_struct** _temp6414= _temp6412.curr + yyvsp_offset; if( _temp6412.base
== 0? 1:( _temp6414 < _temp6412.base? 1: _temp6414 >= _temp6412.last_plus_one)){
_throw( Null_Exception);}* _temp6414;}); break; case 310: _LL6411: yyval=({
struct _tagged_ptr2 _temp6416= yyvs; struct _xtunion_struct** _temp6418=
_temp6416.curr + yyvsp_offset; if( _temp6416.base == 0? 1:( _temp6418 <
_temp6416.base? 1: _temp6418 >= _temp6416.last_plus_one)){ _throw(
Null_Exception);}* _temp6418;}); break; case 311: _LL6415: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6420=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6420[ 0]=({ struct Cyc_Exp_tok_struct
_temp6421; _temp6421.tag= Cyc_Exp_tok_tag; _temp6421.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6422= yyvs; struct _xtunion_struct** _temp6424=
_temp6422.curr +( yyvsp_offset - 2); if( _temp6422.base == 0? 1:( _temp6424 <
_temp6422.base? 1: _temp6424 >= _temp6422.last_plus_one)){ _throw(
Null_Exception);}* _temp6424;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6425= yyvs; struct _xtunion_struct** _temp6427= _temp6425.curr +
yyvsp_offset; if( _temp6425.base == 0? 1:( _temp6427 < _temp6425.base? 1:
_temp6427 >= _temp6425.last_plus_one)){ _throw( Null_Exception);}* _temp6427;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6428= yyls; struct Cyc_Yyltype*
_temp6430= _temp6428.curr +( yylsp_offset - 2); if( _temp6428.base == 0? 1:(
_temp6430 < _temp6428.base? 1: _temp6430 >= _temp6428.last_plus_one)){ _throw(
Null_Exception);}* _temp6430;}).first_line,({ struct _tagged_ptr3 _temp6431=
yyls; struct Cyc_Yyltype* _temp6433= _temp6431.curr + yylsp_offset; if(
_temp6431.base == 0? 1:( _temp6433 < _temp6431.base? 1: _temp6433 >= _temp6431.last_plus_one)){
_throw( Null_Exception);}* _temp6433;}).last_line)); _temp6421;}); _temp6420;});
break; case 312: _LL6419: yyval=({ struct _tagged_ptr2 _temp6435= yyvs; struct
_xtunion_struct** _temp6437= _temp6435.curr + yyvsp_offset; if( _temp6435.base
== 0? 1:( _temp6437 < _temp6435.base? 1: _temp6437 >= _temp6435.last_plus_one)){
_throw( Null_Exception);}* _temp6437;}); break; case 313: _LL6434: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6439=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6439[ 0]=({ struct Cyc_Exp_tok_struct
_temp6440; _temp6440.tag= Cyc_Exp_tok_tag; _temp6440.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6441= yyvs; struct _xtunion_struct** _temp6443=
_temp6441.curr +( yyvsp_offset - 2); if( _temp6441.base == 0? 1:( _temp6443 <
_temp6441.base? 1: _temp6443 >= _temp6441.last_plus_one)){ _throw(
Null_Exception);}* _temp6443;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6444= yyvs; struct _xtunion_struct** _temp6446= _temp6444.curr +
yyvsp_offset; if( _temp6444.base == 0? 1:( _temp6446 < _temp6444.base? 1:
_temp6446 >= _temp6444.last_plus_one)){ _throw( Null_Exception);}* _temp6446;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6447= yyls; struct Cyc_Yyltype*
_temp6449= _temp6447.curr +( yylsp_offset - 2); if( _temp6447.base == 0? 1:(
_temp6449 < _temp6447.base? 1: _temp6449 >= _temp6447.last_plus_one)){ _throw(
Null_Exception);}* _temp6449;}).first_line,({ struct _tagged_ptr3 _temp6450=
yyls; struct Cyc_Yyltype* _temp6452= _temp6450.curr + yylsp_offset; if(
_temp6450.base == 0? 1:( _temp6452 < _temp6450.base? 1: _temp6452 >= _temp6450.last_plus_one)){
_throw( Null_Exception);}* _temp6452;}).last_line)); _temp6440;}); _temp6439;});
break; case 314: _LL6438: yyval=({ struct _tagged_ptr2 _temp6454= yyvs; struct
_xtunion_struct** _temp6456= _temp6454.curr + yyvsp_offset; if( _temp6454.base
== 0? 1:( _temp6456 < _temp6454.base? 1: _temp6456 >= _temp6454.last_plus_one)){
_throw( Null_Exception);}* _temp6456;}); break; case 315: _LL6453: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6458=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6458[ 0]=({ struct Cyc_Exp_tok_struct
_temp6459; _temp6459.tag= Cyc_Exp_tok_tag; _temp6459.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6460= yyvs;
struct _xtunion_struct** _temp6462= _temp6460.curr +( yyvsp_offset - 2); if(
_temp6460.base == 0? 1:( _temp6462 < _temp6460.base? 1: _temp6462 >= _temp6460.last_plus_one)){
_throw( Null_Exception);}* _temp6462;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6463= yyvs; struct _xtunion_struct** _temp6465= _temp6463.curr
+ yyvsp_offset; if( _temp6463.base == 0? 1:( _temp6465 < _temp6463.base? 1:
_temp6465 >= _temp6463.last_plus_one)){ _throw( Null_Exception);}* _temp6465;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6466= yyls; struct Cyc_Yyltype*
_temp6468= _temp6466.curr +( yylsp_offset - 2); if( _temp6466.base == 0? 1:(
_temp6468 < _temp6466.base? 1: _temp6468 >= _temp6466.last_plus_one)){ _throw(
Null_Exception);}* _temp6468;}).first_line,({ struct _tagged_ptr3 _temp6469=
yyls; struct Cyc_Yyltype* _temp6471= _temp6469.curr + yylsp_offset; if(
_temp6469.base == 0? 1:( _temp6471 < _temp6469.base? 1: _temp6471 >= _temp6469.last_plus_one)){
_throw( Null_Exception);}* _temp6471;}).last_line)); _temp6459;}); _temp6458;});
break; case 316: _LL6457: yyval=({ struct _tagged_ptr2 _temp6473= yyvs; struct
_xtunion_struct** _temp6475= _temp6473.curr + yyvsp_offset; if( _temp6473.base
== 0? 1:( _temp6475 < _temp6473.base? 1: _temp6475 >= _temp6473.last_plus_one)){
_throw( Null_Exception);}* _temp6475;}); break; case 317: _LL6472: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6477=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6477[ 0]=({ struct Cyc_Exp_tok_struct
_temp6478; _temp6478.tag= Cyc_Exp_tok_tag; _temp6478.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6479=
yyvs; struct _xtunion_struct** _temp6481= _temp6479.curr +( yyvsp_offset - 2);
if( _temp6479.base == 0? 1:( _temp6481 < _temp6479.base? 1: _temp6481 >=
_temp6479.last_plus_one)){ _throw( Null_Exception);}* _temp6481;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6482= yyvs; struct _xtunion_struct** _temp6484=
_temp6482.curr + yyvsp_offset; if( _temp6482.base == 0? 1:( _temp6484 <
_temp6482.base? 1: _temp6484 >= _temp6482.last_plus_one)){ _throw(
Null_Exception);}* _temp6484;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6485= yyls; struct Cyc_Yyltype* _temp6487= _temp6485.curr +(
yylsp_offset - 2); if( _temp6485.base == 0? 1:( _temp6487 < _temp6485.base? 1:
_temp6487 >= _temp6485.last_plus_one)){ _throw( Null_Exception);}* _temp6487;}).first_line,({
struct _tagged_ptr3 _temp6488= yyls; struct Cyc_Yyltype* _temp6490= _temp6488.curr
+ yylsp_offset; if( _temp6488.base == 0? 1:( _temp6490 < _temp6488.base? 1:
_temp6490 >= _temp6488.last_plus_one)){ _throw( Null_Exception);}* _temp6490;}).last_line));
_temp6478;}); _temp6477;}); break; case 318: _LL6476: yyval=({ struct
_tagged_ptr2 _temp6492= yyvs; struct _xtunion_struct** _temp6494= _temp6492.curr
+ yyvsp_offset; if( _temp6492.base == 0? 1:( _temp6494 < _temp6492.base? 1:
_temp6494 >= _temp6492.last_plus_one)){ _throw( Null_Exception);}* _temp6494;});
break; case 319: _LL6491: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6496=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6496[ 0]=({ struct Cyc_Exp_tok_struct _temp6497; _temp6497.tag= Cyc_Exp_tok_tag;
_temp6497.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6498= yyvs; struct _xtunion_struct** _temp6500=
_temp6498.curr +( yyvsp_offset - 2); if( _temp6498.base == 0? 1:( _temp6500 <
_temp6498.base? 1: _temp6500 >= _temp6498.last_plus_one)){ _throw(
Null_Exception);}* _temp6500;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6501= yyvs; struct _xtunion_struct** _temp6503= _temp6501.curr +
yyvsp_offset; if( _temp6501.base == 0? 1:( _temp6503 < _temp6501.base? 1:
_temp6503 >= _temp6501.last_plus_one)){ _throw( Null_Exception);}* _temp6503;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6504= yyls; struct Cyc_Yyltype*
_temp6506= _temp6504.curr +( yylsp_offset - 2); if( _temp6504.base == 0? 1:(
_temp6506 < _temp6504.base? 1: _temp6506 >= _temp6504.last_plus_one)){ _throw(
Null_Exception);}* _temp6506;}).first_line,({ struct _tagged_ptr3 _temp6507=
yyls; struct Cyc_Yyltype* _temp6509= _temp6507.curr + yylsp_offset; if(
_temp6507.base == 0? 1:( _temp6509 < _temp6507.base? 1: _temp6509 >= _temp6507.last_plus_one)){
_throw( Null_Exception);}* _temp6509;}).last_line)); _temp6497;}); _temp6496;});
break; case 320: _LL6495: yyval=({ struct _tagged_ptr2 _temp6511= yyvs; struct
_xtunion_struct** _temp6513= _temp6511.curr + yyvsp_offset; if( _temp6511.base
== 0? 1:( _temp6513 < _temp6511.base? 1: _temp6513 >= _temp6511.last_plus_one)){
_throw( Null_Exception);}* _temp6513;}); break; case 321: _LL6510: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6515=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6515[ 0]=({ struct Cyc_Exp_tok_struct
_temp6516; _temp6516.tag= Cyc_Exp_tok_tag; _temp6516.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6517= yyvs; struct _xtunion_struct** _temp6519=
_temp6517.curr +( yyvsp_offset - 2); if( _temp6517.base == 0? 1:( _temp6519 <
_temp6517.base? 1: _temp6519 >= _temp6517.last_plus_one)){ _throw(
Null_Exception);}* _temp6519;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6520= yyvs; struct _xtunion_struct** _temp6522= _temp6520.curr +
yyvsp_offset; if( _temp6520.base == 0? 1:( _temp6522 < _temp6520.base? 1:
_temp6522 >= _temp6520.last_plus_one)){ _throw( Null_Exception);}* _temp6522;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6523= yyls; struct Cyc_Yyltype*
_temp6525= _temp6523.curr +( yylsp_offset - 2); if( _temp6523.base == 0? 1:(
_temp6525 < _temp6523.base? 1: _temp6525 >= _temp6523.last_plus_one)){ _throw(
Null_Exception);}* _temp6525;}).first_line,({ struct _tagged_ptr3 _temp6526=
yyls; struct Cyc_Yyltype* _temp6528= _temp6526.curr + yylsp_offset; if(
_temp6526.base == 0? 1:( _temp6528 < _temp6526.base? 1: _temp6528 >= _temp6526.last_plus_one)){
_throw( Null_Exception);}* _temp6528;}).last_line)); _temp6516;}); _temp6515;});
break; case 322: _LL6514: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6530=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6530[ 0]=({ struct Cyc_Exp_tok_struct _temp6531; _temp6531.tag= Cyc_Exp_tok_tag;
_temp6531.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6532= yyvs; struct _xtunion_struct** _temp6534= _temp6532.curr +(
yyvsp_offset - 2); if( _temp6532.base == 0? 1:( _temp6534 < _temp6532.base? 1:
_temp6534 >= _temp6532.last_plus_one)){ _throw( Null_Exception);}* _temp6534;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6535= yyvs; struct _xtunion_struct**
_temp6537= _temp6535.curr + yyvsp_offset; if( _temp6535.base == 0? 1:( _temp6537
< _temp6535.base? 1: _temp6537 >= _temp6535.last_plus_one)){ _throw(
Null_Exception);}* _temp6537;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6538= yyls; struct Cyc_Yyltype* _temp6540= _temp6538.curr +(
yylsp_offset - 2); if( _temp6538.base == 0? 1:( _temp6540 < _temp6538.base? 1:
_temp6540 >= _temp6538.last_plus_one)){ _throw( Null_Exception);}* _temp6540;}).first_line,({
struct _tagged_ptr3 _temp6541= yyls; struct Cyc_Yyltype* _temp6543= _temp6541.curr
+ yylsp_offset; if( _temp6541.base == 0? 1:( _temp6543 < _temp6541.base? 1:
_temp6543 >= _temp6541.last_plus_one)){ _throw( Null_Exception);}* _temp6543;}).last_line));
_temp6531;}); _temp6530;}); break; case 323: _LL6529: yyval=({ struct
_tagged_ptr2 _temp6545= yyvs; struct _xtunion_struct** _temp6547= _temp6545.curr
+ yyvsp_offset; if( _temp6545.base == 0? 1:( _temp6547 < _temp6545.base? 1:
_temp6547 >= _temp6545.last_plus_one)){ _throw( Null_Exception);}* _temp6547;});
break; case 324: _LL6544: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6549=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6549[ 0]=({ struct Cyc_Exp_tok_struct _temp6550; _temp6550.tag= Cyc_Exp_tok_tag;
_temp6550.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6551= yyvs; struct _xtunion_struct** _temp6553= _temp6551.curr +(
yyvsp_offset - 2); if( _temp6551.base == 0? 1:( _temp6553 < _temp6551.base? 1:
_temp6553 >= _temp6551.last_plus_one)){ _throw( Null_Exception);}* _temp6553;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6554= yyvs; struct _xtunion_struct**
_temp6556= _temp6554.curr + yyvsp_offset; if( _temp6554.base == 0? 1:( _temp6556
< _temp6554.base? 1: _temp6556 >= _temp6554.last_plus_one)){ _throw(
Null_Exception);}* _temp6556;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6557= yyls; struct Cyc_Yyltype* _temp6559= _temp6557.curr +(
yylsp_offset - 2); if( _temp6557.base == 0? 1:( _temp6559 < _temp6557.base? 1:
_temp6559 >= _temp6557.last_plus_one)){ _throw( Null_Exception);}* _temp6559;}).first_line,({
struct _tagged_ptr3 _temp6560= yyls; struct Cyc_Yyltype* _temp6562= _temp6560.curr
+ yylsp_offset; if( _temp6560.base == 0? 1:( _temp6562 < _temp6560.base? 1:
_temp6562 >= _temp6560.last_plus_one)){ _throw( Null_Exception);}* _temp6562;}).last_line));
_temp6550;}); _temp6549;}); break; case 325: _LL6548: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6564=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6564[ 0]=({ struct Cyc_Exp_tok_struct
_temp6565; _temp6565.tag= Cyc_Exp_tok_tag; _temp6565.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6566= yyvs; struct _xtunion_struct** _temp6568=
_temp6566.curr +( yyvsp_offset - 2); if( _temp6566.base == 0? 1:( _temp6568 <
_temp6566.base? 1: _temp6568 >= _temp6566.last_plus_one)){ _throw(
Null_Exception);}* _temp6568;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6569= yyvs; struct _xtunion_struct** _temp6571= _temp6569.curr +
yyvsp_offset; if( _temp6569.base == 0? 1:( _temp6571 < _temp6569.base? 1:
_temp6571 >= _temp6569.last_plus_one)){ _throw( Null_Exception);}* _temp6571;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6572= yyls; struct Cyc_Yyltype*
_temp6574= _temp6572.curr +( yylsp_offset - 2); if( _temp6572.base == 0? 1:(
_temp6574 < _temp6572.base? 1: _temp6574 >= _temp6572.last_plus_one)){ _throw(
Null_Exception);}* _temp6574;}).first_line,({ struct _tagged_ptr3 _temp6575=
yyls; struct Cyc_Yyltype* _temp6577= _temp6575.curr + yylsp_offset; if(
_temp6575.base == 0? 1:( _temp6577 < _temp6575.base? 1: _temp6577 >= _temp6575.last_plus_one)){
_throw( Null_Exception);}* _temp6577;}).last_line)); _temp6565;}); _temp6564;});
break; case 326: _LL6563: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6579=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6579[ 0]=({ struct Cyc_Exp_tok_struct _temp6580; _temp6580.tag= Cyc_Exp_tok_tag;
_temp6580.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6581= yyvs; struct _xtunion_struct** _temp6583= _temp6581.curr +(
yyvsp_offset - 2); if( _temp6581.base == 0? 1:( _temp6583 < _temp6581.base? 1:
_temp6583 >= _temp6581.last_plus_one)){ _throw( Null_Exception);}* _temp6583;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6584= yyvs; struct _xtunion_struct**
_temp6586= _temp6584.curr + yyvsp_offset; if( _temp6584.base == 0? 1:( _temp6586
< _temp6584.base? 1: _temp6586 >= _temp6584.last_plus_one)){ _throw(
Null_Exception);}* _temp6586;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6587= yyls; struct Cyc_Yyltype* _temp6589= _temp6587.curr +(
yylsp_offset - 2); if( _temp6587.base == 0? 1:( _temp6589 < _temp6587.base? 1:
_temp6589 >= _temp6587.last_plus_one)){ _throw( Null_Exception);}* _temp6589;}).first_line,({
struct _tagged_ptr3 _temp6590= yyls; struct Cyc_Yyltype* _temp6592= _temp6590.curr
+ yylsp_offset; if( _temp6590.base == 0? 1:( _temp6592 < _temp6590.base? 1:
_temp6592 >= _temp6590.last_plus_one)){ _throw( Null_Exception);}* _temp6592;}).last_line));
_temp6580;}); _temp6579;}); break; case 327: _LL6578: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6594=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6594[ 0]=({ struct Cyc_Exp_tok_struct
_temp6595; _temp6595.tag= Cyc_Exp_tok_tag; _temp6595.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6596= yyvs; struct _xtunion_struct** _temp6598=
_temp6596.curr +( yyvsp_offset - 2); if( _temp6596.base == 0? 1:( _temp6598 <
_temp6596.base? 1: _temp6598 >= _temp6596.last_plus_one)){ _throw(
Null_Exception);}* _temp6598;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6599= yyvs; struct _xtunion_struct** _temp6601= _temp6599.curr +
yyvsp_offset; if( _temp6599.base == 0? 1:( _temp6601 < _temp6599.base? 1:
_temp6601 >= _temp6599.last_plus_one)){ _throw( Null_Exception);}* _temp6601;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6602= yyls; struct Cyc_Yyltype*
_temp6604= _temp6602.curr +( yylsp_offset - 2); if( _temp6602.base == 0? 1:(
_temp6604 < _temp6602.base? 1: _temp6604 >= _temp6602.last_plus_one)){ _throw(
Null_Exception);}* _temp6604;}).first_line,({ struct _tagged_ptr3 _temp6605=
yyls; struct Cyc_Yyltype* _temp6607= _temp6605.curr + yylsp_offset; if(
_temp6605.base == 0? 1:( _temp6607 < _temp6605.base? 1: _temp6607 >= _temp6605.last_plus_one)){
_throw( Null_Exception);}* _temp6607;}).last_line)); _temp6595;}); _temp6594;});
break; case 328: _LL6593: yyval=({ struct _tagged_ptr2 _temp6609= yyvs; struct
_xtunion_struct** _temp6611= _temp6609.curr + yyvsp_offset; if( _temp6609.base
== 0? 1:( _temp6611 < _temp6609.base? 1: _temp6611 >= _temp6609.last_plus_one)){
_throw( Null_Exception);}* _temp6611;}); break; case 329: _LL6608: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6613=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6613[ 0]=({ struct Cyc_Exp_tok_struct
_temp6614; _temp6614.tag= Cyc_Exp_tok_tag; _temp6614.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6615=
yyvs; struct _xtunion_struct** _temp6617= _temp6615.curr +( yyvsp_offset - 2);
if( _temp6615.base == 0? 1:( _temp6617 < _temp6615.base? 1: _temp6617 >=
_temp6615.last_plus_one)){ _throw( Null_Exception);}* _temp6617;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6618= yyvs; struct _xtunion_struct** _temp6620=
_temp6618.curr + yyvsp_offset; if( _temp6618.base == 0? 1:( _temp6620 <
_temp6618.base? 1: _temp6620 >= _temp6618.last_plus_one)){ _throw(
Null_Exception);}* _temp6620;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6621= yyls; struct Cyc_Yyltype* _temp6623= _temp6621.curr +(
yylsp_offset - 2); if( _temp6621.base == 0? 1:( _temp6623 < _temp6621.base? 1:
_temp6623 >= _temp6621.last_plus_one)){ _throw( Null_Exception);}* _temp6623;}).first_line,({
struct _tagged_ptr3 _temp6624= yyls; struct Cyc_Yyltype* _temp6626= _temp6624.curr
+ yylsp_offset; if( _temp6624.base == 0? 1:( _temp6626 < _temp6624.base? 1:
_temp6626 >= _temp6624.last_plus_one)){ _throw( Null_Exception);}* _temp6626;}).last_line));
_temp6614;}); _temp6613;}); break; case 330: _LL6612: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6628=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6628[ 0]=({ struct Cyc_Exp_tok_struct
_temp6629; _temp6629.tag= Cyc_Exp_tok_tag; _temp6629.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6630=
yyvs; struct _xtunion_struct** _temp6632= _temp6630.curr +( yyvsp_offset - 2);
if( _temp6630.base == 0? 1:( _temp6632 < _temp6630.base? 1: _temp6632 >=
_temp6630.last_plus_one)){ _throw( Null_Exception);}* _temp6632;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6633= yyvs; struct _xtunion_struct** _temp6635=
_temp6633.curr + yyvsp_offset; if( _temp6633.base == 0? 1:( _temp6635 <
_temp6633.base? 1: _temp6635 >= _temp6633.last_plus_one)){ _throw(
Null_Exception);}* _temp6635;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6636= yyls; struct Cyc_Yyltype* _temp6638= _temp6636.curr +(
yylsp_offset - 2); if( _temp6636.base == 0? 1:( _temp6638 < _temp6636.base? 1:
_temp6638 >= _temp6636.last_plus_one)){ _throw( Null_Exception);}* _temp6638;}).first_line,({
struct _tagged_ptr3 _temp6639= yyls; struct Cyc_Yyltype* _temp6641= _temp6639.curr
+ yylsp_offset; if( _temp6639.base == 0? 1:( _temp6641 < _temp6639.base? 1:
_temp6641 >= _temp6639.last_plus_one)){ _throw( Null_Exception);}* _temp6641;}).last_line));
_temp6629;}); _temp6628;}); break; case 331: _LL6627: yyval=({ struct
_tagged_ptr2 _temp6643= yyvs; struct _xtunion_struct** _temp6645= _temp6643.curr
+ yyvsp_offset; if( _temp6643.base == 0? 1:( _temp6645 < _temp6643.base? 1:
_temp6645 >= _temp6643.last_plus_one)){ _throw( Null_Exception);}* _temp6645;});
break; case 332: _LL6642: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6647=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6647[ 0]=({ struct Cyc_Exp_tok_struct _temp6648; _temp6648.tag= Cyc_Exp_tok_tag;
_temp6648.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6649= yyvs; struct _xtunion_struct** _temp6651=
_temp6649.curr +( yyvsp_offset - 2); if( _temp6649.base == 0? 1:( _temp6651 <
_temp6649.base? 1: _temp6651 >= _temp6649.last_plus_one)){ _throw(
Null_Exception);}* _temp6651;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6652= yyvs; struct _xtunion_struct** _temp6654= _temp6652.curr +
yyvsp_offset; if( _temp6652.base == 0? 1:( _temp6654 < _temp6652.base? 1:
_temp6654 >= _temp6652.last_plus_one)){ _throw( Null_Exception);}* _temp6654;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6655= yyls; struct Cyc_Yyltype*
_temp6657= _temp6655.curr +( yylsp_offset - 2); if( _temp6655.base == 0? 1:(
_temp6657 < _temp6655.base? 1: _temp6657 >= _temp6655.last_plus_one)){ _throw(
Null_Exception);}* _temp6657;}).first_line,({ struct _tagged_ptr3 _temp6658=
yyls; struct Cyc_Yyltype* _temp6660= _temp6658.curr + yylsp_offset; if(
_temp6658.base == 0? 1:( _temp6660 < _temp6658.base? 1: _temp6660 >= _temp6658.last_plus_one)){
_throw( Null_Exception);}* _temp6660;}).last_line)); _temp6648;}); _temp6647;});
break; case 333: _LL6646: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6662=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6662[ 0]=({ struct Cyc_Exp_tok_struct _temp6663; _temp6663.tag= Cyc_Exp_tok_tag;
_temp6663.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6664= yyvs; struct _xtunion_struct** _temp6666=
_temp6664.curr +( yyvsp_offset - 2); if( _temp6664.base == 0? 1:( _temp6666 <
_temp6664.base? 1: _temp6666 >= _temp6664.last_plus_one)){ _throw(
Null_Exception);}* _temp6666;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6667= yyvs; struct _xtunion_struct** _temp6669= _temp6667.curr +
yyvsp_offset; if( _temp6667.base == 0? 1:( _temp6669 < _temp6667.base? 1:
_temp6669 >= _temp6667.last_plus_one)){ _throw( Null_Exception);}* _temp6669;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6670= yyls; struct Cyc_Yyltype*
_temp6672= _temp6670.curr +( yylsp_offset - 2); if( _temp6670.base == 0? 1:(
_temp6672 < _temp6670.base? 1: _temp6672 >= _temp6670.last_plus_one)){ _throw(
Null_Exception);}* _temp6672;}).first_line,({ struct _tagged_ptr3 _temp6673=
yyls; struct Cyc_Yyltype* _temp6675= _temp6673.curr + yylsp_offset; if(
_temp6673.base == 0? 1:( _temp6675 < _temp6673.base? 1: _temp6675 >= _temp6673.last_plus_one)){
_throw( Null_Exception);}* _temp6675;}).last_line)); _temp6663;}); _temp6662;});
break; case 334: _LL6661: yyval=({ struct _tagged_ptr2 _temp6677= yyvs; struct
_xtunion_struct** _temp6679= _temp6677.curr + yyvsp_offset; if( _temp6677.base
== 0? 1:( _temp6679 < _temp6677.base? 1: _temp6679 >= _temp6677.last_plus_one)){
_throw( Null_Exception);}* _temp6679;}); break; case 335: _LL6676: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6681=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6681[ 0]=({ struct Cyc_Exp_tok_struct
_temp6682; _temp6682.tag= Cyc_Exp_tok_tag; _temp6682.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Times, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6683= yyvs;
struct _xtunion_struct** _temp6685= _temp6683.curr +( yyvsp_offset - 2); if(
_temp6683.base == 0? 1:( _temp6685 < _temp6683.base? 1: _temp6685 >= _temp6683.last_plus_one)){
_throw( Null_Exception);}* _temp6685;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6686= yyvs; struct _xtunion_struct** _temp6688= _temp6686.curr
+ yyvsp_offset; if( _temp6686.base == 0? 1:( _temp6688 < _temp6686.base? 1:
_temp6688 >= _temp6686.last_plus_one)){ _throw( Null_Exception);}* _temp6688;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6689= yyls; struct Cyc_Yyltype*
_temp6691= _temp6689.curr +( yylsp_offset - 2); if( _temp6689.base == 0? 1:(
_temp6691 < _temp6689.base? 1: _temp6691 >= _temp6689.last_plus_one)){ _throw(
Null_Exception);}* _temp6691;}).first_line,({ struct _tagged_ptr3 _temp6692=
yyls; struct Cyc_Yyltype* _temp6694= _temp6692.curr + yylsp_offset; if(
_temp6692.base == 0? 1:( _temp6694 < _temp6692.base? 1: _temp6694 >= _temp6692.last_plus_one)){
_throw( Null_Exception);}* _temp6694;}).last_line)); _temp6682;}); _temp6681;});
break; case 336: _LL6680: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6696=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6696[ 0]=({ struct Cyc_Exp_tok_struct _temp6697; _temp6697.tag= Cyc_Exp_tok_tag;
_temp6697.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6698= yyvs; struct _xtunion_struct** _temp6700=
_temp6698.curr +( yyvsp_offset - 2); if( _temp6698.base == 0? 1:( _temp6700 <
_temp6698.base? 1: _temp6700 >= _temp6698.last_plus_one)){ _throw(
Null_Exception);}* _temp6700;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6701= yyvs; struct _xtunion_struct** _temp6703= _temp6701.curr +
yyvsp_offset; if( _temp6701.base == 0? 1:( _temp6703 < _temp6701.base? 1:
_temp6703 >= _temp6701.last_plus_one)){ _throw( Null_Exception);}* _temp6703;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6704= yyls; struct Cyc_Yyltype*
_temp6706= _temp6704.curr +( yylsp_offset - 2); if( _temp6704.base == 0? 1:(
_temp6706 < _temp6704.base? 1: _temp6706 >= _temp6704.last_plus_one)){ _throw(
Null_Exception);}* _temp6706;}).first_line,({ struct _tagged_ptr3 _temp6707=
yyls; struct Cyc_Yyltype* _temp6709= _temp6707.curr + yylsp_offset; if(
_temp6707.base == 0? 1:( _temp6709 < _temp6707.base? 1: _temp6709 >= _temp6707.last_plus_one)){
_throw( Null_Exception);}* _temp6709;}).last_line)); _temp6697;}); _temp6696;});
break; case 337: _LL6695: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6711=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6711[ 0]=({ struct Cyc_Exp_tok_struct _temp6712; _temp6712.tag= Cyc_Exp_tok_tag;
_temp6712.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6713= yyvs; struct _xtunion_struct** _temp6715=
_temp6713.curr +( yyvsp_offset - 2); if( _temp6713.base == 0? 1:( _temp6715 <
_temp6713.base? 1: _temp6715 >= _temp6713.last_plus_one)){ _throw(
Null_Exception);}* _temp6715;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6716= yyvs; struct _xtunion_struct** _temp6718= _temp6716.curr +
yyvsp_offset; if( _temp6716.base == 0? 1:( _temp6718 < _temp6716.base? 1:
_temp6718 >= _temp6716.last_plus_one)){ _throw( Null_Exception);}* _temp6718;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6719= yyls; struct Cyc_Yyltype*
_temp6721= _temp6719.curr +( yylsp_offset - 2); if( _temp6719.base == 0? 1:(
_temp6721 < _temp6719.base? 1: _temp6721 >= _temp6719.last_plus_one)){ _throw(
Null_Exception);}* _temp6721;}).first_line,({ struct _tagged_ptr3 _temp6722=
yyls; struct Cyc_Yyltype* _temp6724= _temp6722.curr + yylsp_offset; if(
_temp6722.base == 0? 1:( _temp6724 < _temp6722.base? 1: _temp6724 >= _temp6722.last_plus_one)){
_throw( Null_Exception);}* _temp6724;}).last_line)); _temp6712;}); _temp6711;});
break; case 338: _LL6710: yyval=({ struct _tagged_ptr2 _temp6726= yyvs; struct
_xtunion_struct** _temp6728= _temp6726.curr + yyvsp_offset; if( _temp6726.base
== 0? 1:( _temp6728 < _temp6726.base? 1: _temp6728 >= _temp6726.last_plus_one)){
_throw( Null_Exception);}* _temp6728;}); break; case 339: _LL6725: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6730=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6730[ 0]=({ struct Cyc_Exp_tok_struct
_temp6731; _temp6731.tag= Cyc_Exp_tok_tag; _temp6731.f1= Cyc_Absyn_cast_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6732= yyvs; struct
_xtunion_struct** _temp6734= _temp6732.curr +( yyvsp_offset - 2); if( _temp6732.base
== 0? 1:( _temp6734 < _temp6732.base? 1: _temp6734 >= _temp6732.last_plus_one)){
_throw( Null_Exception);}* _temp6734;}))).f3, Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6735= yyvs; struct _xtunion_struct** _temp6737= _temp6735.curr
+ yyvsp_offset; if( _temp6735.base == 0? 1:( _temp6737 < _temp6735.base? 1:
_temp6737 >= _temp6735.last_plus_one)){ _throw( Null_Exception);}* _temp6737;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6738= yyls; struct Cyc_Yyltype*
_temp6740= _temp6738.curr +( yylsp_offset - 3); if( _temp6738.base == 0? 1:(
_temp6740 < _temp6738.base? 1: _temp6740 >= _temp6738.last_plus_one)){ _throw(
Null_Exception);}* _temp6740;}).first_line,({ struct _tagged_ptr3 _temp6741=
yyls; struct Cyc_Yyltype* _temp6743= _temp6741.curr + yylsp_offset; if(
_temp6741.base == 0? 1:( _temp6743 < _temp6741.base? 1: _temp6743 >= _temp6741.last_plus_one)){
_throw( Null_Exception);}* _temp6743;}).last_line)); _temp6731;}); _temp6730;});
break; case 340: _LL6729: yyval=({ struct _tagged_ptr2 _temp6745= yyvs; struct
_xtunion_struct** _temp6747= _temp6745.curr + yyvsp_offset; if( _temp6745.base
== 0? 1:( _temp6747 < _temp6745.base? 1: _temp6747 >= _temp6745.last_plus_one)){
_throw( Null_Exception);}* _temp6747;}); break; case 341: _LL6744: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6749=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6749[ 0]=({ struct Cyc_Exp_tok_struct
_temp6750; _temp6750.tag= Cyc_Exp_tok_tag; _temp6750.f1= Cyc_Absyn_pre_inc_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6751= yyvs; struct _xtunion_struct**
_temp6753= _temp6751.curr + yyvsp_offset; if( _temp6751.base == 0? 1:( _temp6753
< _temp6751.base? 1: _temp6753 >= _temp6751.last_plus_one)){ _throw(
Null_Exception);}* _temp6753;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6754= yyls; struct Cyc_Yyltype* _temp6756= _temp6754.curr +(
yylsp_offset - 1); if( _temp6754.base == 0? 1:( _temp6756 < _temp6754.base? 1:
_temp6756 >= _temp6754.last_plus_one)){ _throw( Null_Exception);}* _temp6756;}).first_line,({
struct _tagged_ptr3 _temp6757= yyls; struct Cyc_Yyltype* _temp6759= _temp6757.curr
+ yylsp_offset; if( _temp6757.base == 0? 1:( _temp6759 < _temp6757.base? 1:
_temp6759 >= _temp6757.last_plus_one)){ _throw( Null_Exception);}* _temp6759;}).last_line));
_temp6750;}); _temp6749;}); break; case 342: _LL6748: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6761=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6761[ 0]=({ struct Cyc_Exp_tok_struct
_temp6762; _temp6762.tag= Cyc_Exp_tok_tag; _temp6762.f1= Cyc_Absyn_pre_dec_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6763= yyvs; struct _xtunion_struct**
_temp6765= _temp6763.curr + yyvsp_offset; if( _temp6763.base == 0? 1:( _temp6765
< _temp6763.base? 1: _temp6765 >= _temp6763.last_plus_one)){ _throw(
Null_Exception);}* _temp6765;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6766= yyls; struct Cyc_Yyltype* _temp6768= _temp6766.curr +(
yylsp_offset - 1); if( _temp6766.base == 0? 1:( _temp6768 < _temp6766.base? 1:
_temp6768 >= _temp6766.last_plus_one)){ _throw( Null_Exception);}* _temp6768;}).first_line,({
struct _tagged_ptr3 _temp6769= yyls; struct Cyc_Yyltype* _temp6771= _temp6769.curr
+ yylsp_offset; if( _temp6769.base == 0? 1:( _temp6771 < _temp6769.base? 1:
_temp6771 >= _temp6769.last_plus_one)){ _throw( Null_Exception);}* _temp6771;}).last_line));
_temp6762;}); _temp6761;}); break; case 343: _LL6760: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6773=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6773[ 0]=({ struct Cyc_Exp_tok_struct
_temp6774; _temp6774.tag= Cyc_Exp_tok_tag; _temp6774.f1= Cyc_Absyn_address_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6775= yyvs; struct _xtunion_struct**
_temp6777= _temp6775.curr + yyvsp_offset; if( _temp6775.base == 0? 1:( _temp6777
< _temp6775.base? 1: _temp6777 >= _temp6775.last_plus_one)){ _throw(
Null_Exception);}* _temp6777;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6778= yyls; struct Cyc_Yyltype* _temp6780= _temp6778.curr +(
yylsp_offset - 1); if( _temp6778.base == 0? 1:( _temp6780 < _temp6778.base? 1:
_temp6780 >= _temp6778.last_plus_one)){ _throw( Null_Exception);}* _temp6780;}).first_line,({
struct _tagged_ptr3 _temp6781= yyls; struct Cyc_Yyltype* _temp6783= _temp6781.curr
+ yylsp_offset; if( _temp6781.base == 0? 1:( _temp6783 < _temp6781.base? 1:
_temp6783 >= _temp6781.last_plus_one)){ _throw( Null_Exception);}* _temp6783;}).last_line));
_temp6774;}); _temp6773;}); break; case 344: _LL6772: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6785=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6785[ 0]=({ struct Cyc_Exp_tok_struct
_temp6786; _temp6786.tag= Cyc_Exp_tok_tag; _temp6786.f1= Cyc_Absyn_deref_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6787= yyvs; struct _xtunion_struct**
_temp6789= _temp6787.curr + yyvsp_offset; if( _temp6787.base == 0? 1:( _temp6789
< _temp6787.base? 1: _temp6789 >= _temp6787.last_plus_one)){ _throw(
Null_Exception);}* _temp6789;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6790= yyls; struct Cyc_Yyltype* _temp6792= _temp6790.curr +(
yylsp_offset - 1); if( _temp6790.base == 0? 1:( _temp6792 < _temp6790.base? 1:
_temp6792 >= _temp6790.last_plus_one)){ _throw( Null_Exception);}* _temp6792;}).first_line,({
struct _tagged_ptr3 _temp6793= yyls; struct Cyc_Yyltype* _temp6795= _temp6793.curr
+ yylsp_offset; if( _temp6793.base == 0? 1:( _temp6795 < _temp6793.base? 1:
_temp6795 >= _temp6793.last_plus_one)){ _throw( Null_Exception);}* _temp6795;}).last_line));
_temp6786;}); _temp6785;}); break; case 345: _LL6784: yyval=({ struct
_tagged_ptr2 _temp6797= yyvs; struct _xtunion_struct** _temp6799= _temp6797.curr
+ yyvsp_offset; if( _temp6797.base == 0? 1:( _temp6799 < _temp6797.base? 1:
_temp6799 >= _temp6797.last_plus_one)){ _throw( Null_Exception);}* _temp6799;});
break; case 346: _LL6796: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6801=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6801[ 0]=({ struct Cyc_Exp_tok_struct _temp6802; _temp6802.tag= Cyc_Exp_tok_tag;
_temp6802.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(({ struct _tagged_ptr2
_temp6803= yyvs; struct _xtunion_struct** _temp6805= _temp6803.curr +(
yyvsp_offset - 1); if( _temp6803.base == 0? 1:( _temp6805 < _temp6803.base? 1:
_temp6805 >= _temp6803.last_plus_one)){ _throw( Null_Exception);}* _temp6805;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6806= yyvs; struct _xtunion_struct**
_temp6808= _temp6806.curr + yyvsp_offset; if( _temp6806.base == 0? 1:( _temp6808
< _temp6806.base? 1: _temp6808 >= _temp6806.last_plus_one)){ _throw(
Null_Exception);}* _temp6808;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6809= yyls; struct Cyc_Yyltype* _temp6811= _temp6809.curr +(
yylsp_offset - 1); if( _temp6809.base == 0? 1:( _temp6811 < _temp6809.base? 1:
_temp6811 >= _temp6809.last_plus_one)){ _throw( Null_Exception);}* _temp6811;}).first_line,({
struct _tagged_ptr3 _temp6812= yyls; struct Cyc_Yyltype* _temp6814= _temp6812.curr
+ yylsp_offset; if( _temp6812.base == 0? 1:( _temp6814 < _temp6812.base? 1:
_temp6814 >= _temp6812.last_plus_one)){ _throw( Null_Exception);}* _temp6814;}).last_line));
_temp6802;}); _temp6801;}); break; case 347: _LL6800: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6816=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6816[ 0]=({ struct Cyc_Exp_tok_struct
_temp6817; _temp6817.tag= Cyc_Exp_tok_tag; _temp6817.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6818= yyvs; struct
_xtunion_struct** _temp6820= _temp6818.curr +( yyvsp_offset - 1); if( _temp6818.base
== 0? 1:( _temp6820 < _temp6818.base? 1: _temp6820 >= _temp6818.last_plus_one)){
_throw( Null_Exception);}* _temp6820;}))).f3, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6821= yyls; struct Cyc_Yyltype* _temp6823= _temp6821.curr
+( yylsp_offset - 3); if( _temp6821.base == 0? 1:( _temp6823 < _temp6821.base? 1:
_temp6823 >= _temp6821.last_plus_one)){ _throw( Null_Exception);}* _temp6823;}).first_line,({
struct _tagged_ptr3 _temp6824= yyls; struct Cyc_Yyltype* _temp6826= _temp6824.curr
+ yylsp_offset; if( _temp6824.base == 0? 1:( _temp6826 < _temp6824.base? 1:
_temp6826 >= _temp6824.last_plus_one)){ _throw( Null_Exception);}* _temp6826;}).last_line));
_temp6817;}); _temp6816;}); break; case 348: _LL6815: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6828=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6828[ 0]=({ struct Cyc_Exp_tok_struct
_temp6829; _temp6829.tag= Cyc_Exp_tok_tag; _temp6829.f1= Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6830= yyvs; struct _xtunion_struct**
_temp6832= _temp6830.curr + yyvsp_offset; if( _temp6830.base == 0? 1:( _temp6832
< _temp6830.base? 1: _temp6832 >= _temp6830.last_plus_one)){ _throw(
Null_Exception);}* _temp6832;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6833= yyls; struct Cyc_Yyltype* _temp6835= _temp6833.curr +(
yylsp_offset - 1); if( _temp6833.base == 0? 1:( _temp6835 < _temp6833.base? 1:
_temp6835 >= _temp6833.last_plus_one)){ _throw( Null_Exception);}* _temp6835;}).first_line,({
struct _tagged_ptr3 _temp6836= yyls; struct Cyc_Yyltype* _temp6838= _temp6836.curr
+ yylsp_offset; if( _temp6836.base == 0? 1:( _temp6838 < _temp6836.base? 1:
_temp6838 >= _temp6836.last_plus_one)){ _throw( Null_Exception);}* _temp6838;}).last_line));
_temp6829;}); _temp6828;}); break; case 349: _LL6827: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6840=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6840[ 0]=({ struct Cyc_Exp_tok_struct
_temp6841; _temp6841.tag= Cyc_Exp_tok_tag; _temp6841.f1= Cyc_Absyn_primop_exp(
Cyc_yyget_Primop_tok(({ struct _tagged_ptr2 _temp6842= yyvs; struct
_xtunion_struct** _temp6844= _temp6842.curr +( yyvsp_offset - 3); if( _temp6842.base
== 0? 1:( _temp6844 < _temp6842.base? 1: _temp6844 >= _temp6842.last_plus_one)){
_throw( Null_Exception);}* _temp6844;})), Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp6845= yyvs; struct _xtunion_struct** _temp6847= _temp6845.curr
+( yyvsp_offset - 1); if( _temp6845.base == 0? 1:( _temp6847 < _temp6845.base? 1:
_temp6847 >= _temp6845.last_plus_one)){ _throw( Null_Exception);}* _temp6847;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6848= yyls; struct Cyc_Yyltype*
_temp6850= _temp6848.curr +( yylsp_offset - 3); if( _temp6848.base == 0? 1:(
_temp6850 < _temp6848.base? 1: _temp6850 >= _temp6848.last_plus_one)){ _throw(
Null_Exception);}* _temp6850;}).first_line,({ struct _tagged_ptr3 _temp6851=
yyls; struct Cyc_Yyltype* _temp6853= _temp6851.curr + yylsp_offset; if(
_temp6851.base == 0? 1:( _temp6853 < _temp6851.base? 1: _temp6853 >= _temp6851.last_plus_one)){
_throw( Null_Exception);}* _temp6853;}).last_line)); _temp6841;}); _temp6840;});
break; case 350: _LL6839: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6855=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6855[ 0]=({ struct Cyc_Exp_tok_struct _temp6856; _temp6856.tag= Cyc_Exp_tok_tag;
_temp6856.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp6857=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp6857[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp6858; _temp6858.tag= Cyc_Absyn_Malloc_e_tag;
_temp6858.f1= 0; _temp6858.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp6859= yyvs; struct _xtunion_struct** _temp6861=
_temp6859.curr +( yyvsp_offset - 2); if( _temp6859.base == 0? 1:( _temp6861 <
_temp6859.base? 1: _temp6861 >= _temp6859.last_plus_one)){ _throw(
Null_Exception);}* _temp6861;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6862= yyls; struct Cyc_Yyltype* _temp6864= _temp6862.curr +(
yylsp_offset - 2); if( _temp6862.base == 0? 1:( _temp6864 < _temp6862.base? 1:
_temp6864 >= _temp6862.last_plus_one)){ _throw( Null_Exception);}* _temp6864;}).first_line,({
struct _tagged_ptr3 _temp6865= yyls; struct Cyc_Yyltype* _temp6867= _temp6865.curr
+( yylsp_offset - 2); if( _temp6865.base == 0? 1:( _temp6867 < _temp6865.base? 1:
_temp6867 >= _temp6865.last_plus_one)){ _throw( Null_Exception);}* _temp6867;}).last_line));
_temp6858;}); _temp6857;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6868= yyls; struct Cyc_Yyltype* _temp6870= _temp6868.curr +( yylsp_offset -
6); if( _temp6868.base == 0? 1:( _temp6870 < _temp6868.base? 1: _temp6870 >=
_temp6868.last_plus_one)){ _throw( Null_Exception);}* _temp6870;}).first_line,({
struct _tagged_ptr3 _temp6871= yyls; struct Cyc_Yyltype* _temp6873= _temp6871.curr
+ yylsp_offset; if( _temp6871.base == 0? 1:( _temp6873 < _temp6871.base? 1:
_temp6873 >= _temp6871.last_plus_one)){ _throw( Null_Exception);}* _temp6873;}).last_line));
_temp6856;}); _temp6855;}); break; case 351: _LL6854: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6875=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6875[ 0]=({ struct Cyc_Exp_tok_struct
_temp6876; _temp6876.tag= Cyc_Exp_tok_tag; _temp6876.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Malloc_e_struct* _temp6877=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp6877[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp6878; _temp6878.tag= Cyc_Absyn_Malloc_e_tag;
_temp6878.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6879= yyvs; struct _xtunion_struct** _temp6881= _temp6879.curr +(
yyvsp_offset - 6); if( _temp6879.base == 0? 1:( _temp6881 < _temp6879.base? 1:
_temp6881 >= _temp6879.last_plus_one)){ _throw( Null_Exception);}* _temp6881;}));
_temp6878.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp6882= yyvs; struct _xtunion_struct** _temp6884=
_temp6882.curr +( yyvsp_offset - 2); if( _temp6882.base == 0? 1:( _temp6884 <
_temp6882.base? 1: _temp6884 >= _temp6882.last_plus_one)){ _throw(
Null_Exception);}* _temp6884;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6885= yyls; struct Cyc_Yyltype* _temp6887= _temp6885.curr +(
yylsp_offset - 2); if( _temp6885.base == 0? 1:( _temp6887 < _temp6885.base? 1:
_temp6887 >= _temp6885.last_plus_one)){ _throw( Null_Exception);}* _temp6887;}).first_line,({
struct _tagged_ptr3 _temp6888= yyls; struct Cyc_Yyltype* _temp6890= _temp6888.curr
+( yylsp_offset - 2); if( _temp6888.base == 0? 1:( _temp6890 < _temp6888.base? 1:
_temp6890 >= _temp6888.last_plus_one)){ _throw( Null_Exception);}* _temp6890;}).last_line));
_temp6878;}); _temp6877;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6891= yyls; struct Cyc_Yyltype* _temp6893= _temp6891.curr +( yylsp_offset -
8); if( _temp6891.base == 0? 1:( _temp6893 < _temp6891.base? 1: _temp6893 >=
_temp6891.last_plus_one)){ _throw( Null_Exception);}* _temp6893;}).first_line,({
struct _tagged_ptr3 _temp6894= yyls; struct Cyc_Yyltype* _temp6896= _temp6894.curr
+ yylsp_offset; if( _temp6894.base == 0? 1:( _temp6896 < _temp6894.base? 1:
_temp6896 >= _temp6894.last_plus_one)){ _throw( Null_Exception);}* _temp6896;}).last_line));
_temp6876;}); _temp6875;}); break; case 352: _LL6874: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6898=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6898[ 0]=({ struct Cyc_Primop_tok_struct
_temp6899; _temp6899.tag= Cyc_Primop_tok_tag; _temp6899.f1=( void*)(( void*) Cyc_Absyn_Printf);
_temp6899;}); _temp6898;}); break; case 353: _LL6897: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6901=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6901[ 0]=({ struct Cyc_Primop_tok_struct
_temp6902; _temp6902.tag= Cyc_Primop_tok_tag; _temp6902.f1=( void*)(( void*) Cyc_Absyn_Fprintf);
_temp6902;}); _temp6901;}); break; case 354: _LL6900: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6904=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6904[ 0]=({ struct Cyc_Primop_tok_struct
_temp6905; _temp6905.tag= Cyc_Primop_tok_tag; _temp6905.f1=( void*)(( void*) Cyc_Absyn_Xprintf);
_temp6905;}); _temp6904;}); break; case 355: _LL6903: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6907=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6907[ 0]=({ struct Cyc_Primop_tok_struct
_temp6908; _temp6908.tag= Cyc_Primop_tok_tag; _temp6908.f1=( void*)(( void*) Cyc_Absyn_Scanf);
_temp6908;}); _temp6907;}); break; case 356: _LL6906: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6910=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6910[ 0]=({ struct Cyc_Primop_tok_struct
_temp6911; _temp6911.tag= Cyc_Primop_tok_tag; _temp6911.f1=( void*)(( void*) Cyc_Absyn_Fscanf);
_temp6911;}); _temp6910;}); break; case 357: _LL6909: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6913=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6913[ 0]=({ struct Cyc_Primop_tok_struct
_temp6914; _temp6914.tag= Cyc_Primop_tok_tag; _temp6914.f1=( void*)(( void*) Cyc_Absyn_Sscanf);
_temp6914;}); _temp6913;}); break; case 358: _LL6912: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6916=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6916[ 0]=({ struct Cyc_Primop_tok_struct
_temp6917; _temp6917.tag= Cyc_Primop_tok_tag; _temp6917.f1=( void*)(( void*) Cyc_Absyn_Bitnot);
_temp6917;}); _temp6916;}); break; case 359: _LL6915: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6919=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6919[ 0]=({ struct Cyc_Primop_tok_struct
_temp6920; _temp6920.tag= Cyc_Primop_tok_tag; _temp6920.f1=( void*)(( void*) Cyc_Absyn_Not);
_temp6920;}); _temp6919;}); break; case 360: _LL6918: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6922=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6922[ 0]=({ struct Cyc_Primop_tok_struct
_temp6923; _temp6923.tag= Cyc_Primop_tok_tag; _temp6923.f1=( void*)(( void*) Cyc_Absyn_Minus);
_temp6923;}); _temp6922;}); break; case 361: _LL6921: yyval=({ struct
_tagged_ptr2 _temp6925= yyvs; struct _xtunion_struct** _temp6927= _temp6925.curr
+ yyvsp_offset; if( _temp6925.base == 0? 1:( _temp6927 < _temp6925.base? 1:
_temp6927 >= _temp6925.last_plus_one)){ _throw( Null_Exception);}* _temp6927;});
break; case 362: _LL6924: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6929=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6929[ 0]=({ struct Cyc_Exp_tok_struct _temp6930; _temp6930.tag= Cyc_Exp_tok_tag;
_temp6930.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6931= yyvs; struct _xtunion_struct** _temp6933= _temp6931.curr +(
yyvsp_offset - 3); if( _temp6931.base == 0? 1:( _temp6933 < _temp6931.base? 1:
_temp6933 >= _temp6931.last_plus_one)){ _throw( Null_Exception);}* _temp6933;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6934= yyvs; struct _xtunion_struct**
_temp6936= _temp6934.curr +( yyvsp_offset - 1); if( _temp6934.base == 0? 1:(
_temp6936 < _temp6934.base? 1: _temp6936 >= _temp6934.last_plus_one)){ _throw(
Null_Exception);}* _temp6936;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6937= yyls; struct Cyc_Yyltype* _temp6939= _temp6937.curr +(
yylsp_offset - 3); if( _temp6937.base == 0? 1:( _temp6939 < _temp6937.base? 1:
_temp6939 >= _temp6937.last_plus_one)){ _throw( Null_Exception);}* _temp6939;}).first_line,({
struct _tagged_ptr3 _temp6940= yyls; struct Cyc_Yyltype* _temp6942= _temp6940.curr
+ yylsp_offset; if( _temp6940.base == 0? 1:( _temp6942 < _temp6940.base? 1:
_temp6942 >= _temp6940.last_plus_one)){ _throw( Null_Exception);}* _temp6942;}).last_line));
_temp6930;}); _temp6929;}); break; case 363: _LL6928: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6944=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6944[ 0]=({ struct Cyc_Exp_tok_struct
_temp6945; _temp6945.tag= Cyc_Exp_tok_tag; _temp6945.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6946= yyvs; struct _xtunion_struct**
_temp6948= _temp6946.curr +( yyvsp_offset - 2); if( _temp6946.base == 0? 1:(
_temp6948 < _temp6946.base? 1: _temp6948 >= _temp6946.last_plus_one)){ _throw(
Null_Exception);}* _temp6948;})), 0, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6949= yyls; struct Cyc_Yyltype* _temp6951= _temp6949.curr +(
yylsp_offset - 2); if( _temp6949.base == 0? 1:( _temp6951 < _temp6949.base? 1:
_temp6951 >= _temp6949.last_plus_one)){ _throw( Null_Exception);}* _temp6951;}).first_line,({
struct _tagged_ptr3 _temp6952= yyls; struct Cyc_Yyltype* _temp6954= _temp6952.curr
+ yylsp_offset; if( _temp6952.base == 0? 1:( _temp6954 < _temp6952.base? 1:
_temp6954 >= _temp6952.last_plus_one)){ _throw( Null_Exception);}* _temp6954;}).last_line));
_temp6945;}); _temp6944;}); break; case 364: _LL6943: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6956=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6956[ 0]=({ struct Cyc_Exp_tok_struct
_temp6957; _temp6957.tag= Cyc_Exp_tok_tag; _temp6957.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6958= yyvs; struct _xtunion_struct**
_temp6960= _temp6958.curr +( yyvsp_offset - 3); if( _temp6958.base == 0? 1:(
_temp6960 < _temp6958.base? 1: _temp6960 >= _temp6958.last_plus_one)){ _throw(
Null_Exception);}* _temp6960;})), Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2
_temp6961= yyvs; struct _xtunion_struct** _temp6963= _temp6961.curr +(
yyvsp_offset - 1); if( _temp6961.base == 0? 1:( _temp6963 < _temp6961.base? 1:
_temp6963 >= _temp6961.last_plus_one)){ _throw( Null_Exception);}* _temp6963;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6964= yyls; struct Cyc_Yyltype*
_temp6966= _temp6964.curr +( yylsp_offset - 3); if( _temp6964.base == 0? 1:(
_temp6966 < _temp6964.base? 1: _temp6966 >= _temp6964.last_plus_one)){ _throw(
Null_Exception);}* _temp6966;}).first_line,({ struct _tagged_ptr3 _temp6967=
yyls; struct Cyc_Yyltype* _temp6969= _temp6967.curr + yylsp_offset; if(
_temp6967.base == 0? 1:( _temp6969 < _temp6967.base? 1: _temp6969 >= _temp6967.last_plus_one)){
_throw( Null_Exception);}* _temp6969;}).last_line)); _temp6957;}); _temp6956;});
break; case 365: _LL6955: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6971=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6971[ 0]=({ struct Cyc_Exp_tok_struct _temp6972; _temp6972.tag= Cyc_Exp_tok_tag;
_temp6972.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6973= yyvs; struct _xtunion_struct** _temp6975= _temp6973.curr
+( yyvsp_offset - 2); if( _temp6973.base == 0? 1:( _temp6975 < _temp6973.base? 1:
_temp6975 >= _temp6973.last_plus_one)){ _throw( Null_Exception);}* _temp6975;})),({
struct _tagged_string* _temp6976=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp6976[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp6977= yyvs; struct _xtunion_struct** _temp6979= _temp6977.curr
+ yyvsp_offset; if( _temp6977.base == 0? 1:( _temp6979 < _temp6977.base? 1:
_temp6979 >= _temp6977.last_plus_one)){ _throw( Null_Exception);}* _temp6979;}));
_temp6976;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6980= yyls;
struct Cyc_Yyltype* _temp6982= _temp6980.curr +( yylsp_offset - 2); if(
_temp6980.base == 0? 1:( _temp6982 < _temp6980.base? 1: _temp6982 >= _temp6980.last_plus_one)){
_throw( Null_Exception);}* _temp6982;}).first_line,({ struct _tagged_ptr3
_temp6983= yyls; struct Cyc_Yyltype* _temp6985= _temp6983.curr + yylsp_offset;
if( _temp6983.base == 0? 1:( _temp6985 < _temp6983.base? 1: _temp6985 >=
_temp6983.last_plus_one)){ _throw( Null_Exception);}* _temp6985;}).last_line));
_temp6972;}); _temp6971;}); break; case 366: _LL6970: { struct _tuple1* q= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp6987= yyvs; struct _xtunion_struct** _temp6989=
_temp6987.curr + yyvsp_offset; if( _temp6987.base == 0? 1:( _temp6989 <
_temp6987.base? 1: _temp6989 >= _temp6987.last_plus_one)){ _throw(
Null_Exception);}* _temp6989;}));{ void* _temp6990=(* q).f1; struct Cyc_List_List*
_temp7000; struct Cyc_List_List* _temp7002; _LL6992: if(( int) _temp6990 == Cyc_Absyn_Loc_n){
goto _LL6993;} else{ goto _LL6994;} _LL6994: if(( unsigned int) _temp6990 > 1u?((
struct _tunion_struct*) _temp6990)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL7001:
_temp7000=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp6990)->f1;
if( _temp7000 == 0){ goto _LL6995;} else{ goto _LL6996;}} else{ goto _LL6996;}
_LL6996: if(( unsigned int) _temp6990 > 1u?(( struct _tunion_struct*) _temp6990)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL7003: _temp7002=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6990)->f1; if( _temp7002 == 0){ goto _LL6997;}
else{ goto _LL6998;}} else{ goto _LL6998;} _LL6998: goto _LL6999; _LL6993: goto
_LL6991; _LL6995: goto _LL6991; _LL6997: goto _LL6991; _LL6999: Cyc_Parse_err((
struct _tagged_string)({ char* _temp7004=( char*)"struct field name is qualified";
struct _tagged_string _temp7005; _temp7005.curr= _temp7004; _temp7005.base=
_temp7004; _temp7005.last_plus_one= _temp7004 + 31; _temp7005;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7006= yyls; struct Cyc_Yyltype* _temp7008= _temp7006.curr
+ yylsp_offset; if( _temp7006.base == 0? 1:( _temp7008 < _temp7006.base? 1:
_temp7008 >= _temp7006.last_plus_one)){ _throw( Null_Exception);}* _temp7008;}).first_line,({
struct _tagged_ptr3 _temp7009= yyls; struct Cyc_Yyltype* _temp7011= _temp7009.curr
+ yylsp_offset; if( _temp7009.base == 0? 1:( _temp7011 < _temp7009.base? 1:
_temp7011 >= _temp7009.last_plus_one)){ _throw( Null_Exception);}* _temp7011;}).last_line));
goto _LL6991; _LL6991:;} yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7012=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7012[ 0]=({ struct Cyc_Exp_tok_struct _temp7013; _temp7013.tag= Cyc_Exp_tok_tag;
_temp7013.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp7014= yyvs; struct _xtunion_struct** _temp7016= _temp7014.curr
+( yyvsp_offset - 2); if( _temp7014.base == 0? 1:( _temp7016 < _temp7014.base? 1:
_temp7016 >= _temp7014.last_plus_one)){ _throw( Null_Exception);}* _temp7016;})),(*
q).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7017= yyls;
struct Cyc_Yyltype* _temp7019= _temp7017.curr +( yylsp_offset - 2); if(
_temp7017.base == 0? 1:( _temp7019 < _temp7017.base? 1: _temp7019 >= _temp7017.last_plus_one)){
_throw( Null_Exception);}* _temp7019;}).first_line,({ struct _tagged_ptr3
_temp7020= yyls; struct Cyc_Yyltype* _temp7022= _temp7020.curr + yylsp_offset;
if( _temp7020.base == 0? 1:( _temp7022 < _temp7020.base? 1: _temp7022 >=
_temp7020.last_plus_one)){ _throw( Null_Exception);}* _temp7022;}).last_line));
_temp7013;}); _temp7012;}); break;} case 367: _LL6986: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7024=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7024[ 0]=({ struct Cyc_Exp_tok_struct
_temp7025; _temp7025.tag= Cyc_Exp_tok_tag; _temp7025.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7026= yyvs; struct _xtunion_struct**
_temp7028= _temp7026.curr +( yyvsp_offset - 2); if( _temp7026.base == 0? 1:(
_temp7028 < _temp7026.base? 1: _temp7028 >= _temp7026.last_plus_one)){ _throw(
Null_Exception);}* _temp7028;})),({ struct _tagged_string* _temp7029=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp7029[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp7030= yyvs; struct
_xtunion_struct** _temp7032= _temp7030.curr + yyvsp_offset; if( _temp7030.base
== 0? 1:( _temp7032 < _temp7030.base? 1: _temp7032 >= _temp7030.last_plus_one)){
_throw( Null_Exception);}* _temp7032;})); _temp7029;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7033= yyls; struct Cyc_Yyltype* _temp7035= _temp7033.curr
+( yylsp_offset - 2); if( _temp7033.base == 0? 1:( _temp7035 < _temp7033.base? 1:
_temp7035 >= _temp7033.last_plus_one)){ _throw( Null_Exception);}* _temp7035;}).first_line,({
struct _tagged_ptr3 _temp7036= yyls; struct Cyc_Yyltype* _temp7038= _temp7036.curr
+ yylsp_offset; if( _temp7036.base == 0? 1:( _temp7038 < _temp7036.base? 1:
_temp7038 >= _temp7036.last_plus_one)){ _throw( Null_Exception);}* _temp7038;}).last_line));
_temp7025;}); _temp7024;}); break; case 368: _LL7023: { struct _tuple1* q= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp7040= yyvs; struct _xtunion_struct** _temp7042=
_temp7040.curr + yyvsp_offset; if( _temp7040.base == 0? 1:( _temp7042 <
_temp7040.base? 1: _temp7042 >= _temp7040.last_plus_one)){ _throw(
Null_Exception);}* _temp7042;}));{ void* _temp7043=(* q).f1; struct Cyc_List_List*
_temp7053; struct Cyc_List_List* _temp7055; _LL7045: if(( int) _temp7043 == Cyc_Absyn_Loc_n){
goto _LL7046;} else{ goto _LL7047;} _LL7047: if(( unsigned int) _temp7043 > 1u?((
struct _tunion_struct*) _temp7043)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL7054:
_temp7053=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp7043)->f1;
if( _temp7053 == 0){ goto _LL7048;} else{ goto _LL7049;}} else{ goto _LL7049;}
_LL7049: if(( unsigned int) _temp7043 > 1u?(( struct _tunion_struct*) _temp7043)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL7056: _temp7055=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp7043)->f1; if( _temp7055 == 0){ goto _LL7050;}
else{ goto _LL7051;}} else{ goto _LL7051;} _LL7051: goto _LL7052; _LL7046: goto
_LL7044; _LL7048: goto _LL7044; _LL7050: goto _LL7044; _LL7052: Cyc_Parse_err((
struct _tagged_string)({ char* _temp7057=( char*)"struct field is qualified with module name";
struct _tagged_string _temp7058; _temp7058.curr= _temp7057; _temp7058.base=
_temp7057; _temp7058.last_plus_one= _temp7057 + 43; _temp7058;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7059= yyls; struct Cyc_Yyltype* _temp7061= _temp7059.curr
+ yylsp_offset; if( _temp7059.base == 0? 1:( _temp7061 < _temp7059.base? 1:
_temp7061 >= _temp7059.last_plus_one)){ _throw( Null_Exception);}* _temp7061;}).first_line,({
struct _tagged_ptr3 _temp7062= yyls; struct Cyc_Yyltype* _temp7064= _temp7062.curr
+ yylsp_offset; if( _temp7062.base == 0? 1:( _temp7064 < _temp7062.base? 1:
_temp7064 >= _temp7062.last_plus_one)){ _throw( Null_Exception);}* _temp7064;}).last_line));
goto _LL7044; _LL7044:;} yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7065=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7065[ 0]=({ struct Cyc_Exp_tok_struct _temp7066; _temp7066.tag= Cyc_Exp_tok_tag;
_temp7066.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp7067= yyvs; struct _xtunion_struct** _temp7069= _temp7067.curr
+( yyvsp_offset - 2); if( _temp7067.base == 0? 1:( _temp7069 < _temp7067.base? 1:
_temp7069 >= _temp7067.last_plus_one)){ _throw( Null_Exception);}* _temp7069;})),(*
q).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7070= yyls;
struct Cyc_Yyltype* _temp7072= _temp7070.curr +( yylsp_offset - 2); if(
_temp7070.base == 0? 1:( _temp7072 < _temp7070.base? 1: _temp7072 >= _temp7070.last_plus_one)){
_throw( Null_Exception);}* _temp7072;}).first_line,({ struct _tagged_ptr3
_temp7073= yyls; struct Cyc_Yyltype* _temp7075= _temp7073.curr + yylsp_offset;
if( _temp7073.base == 0? 1:( _temp7075 < _temp7073.base? 1: _temp7075 >=
_temp7073.last_plus_one)){ _throw( Null_Exception);}* _temp7075;}).last_line));
_temp7066;}); _temp7065;}); break;} case 369: _LL7039: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7077=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7077[ 0]=({ struct Cyc_Exp_tok_struct
_temp7078; _temp7078.tag= Cyc_Exp_tok_tag; _temp7078.f1= Cyc_Absyn_post_inc_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7079= yyvs; struct _xtunion_struct**
_temp7081= _temp7079.curr +( yyvsp_offset - 1); if( _temp7079.base == 0? 1:(
_temp7081 < _temp7079.base? 1: _temp7081 >= _temp7079.last_plus_one)){ _throw(
Null_Exception);}* _temp7081;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7082= yyls; struct Cyc_Yyltype* _temp7084= _temp7082.curr +(
yylsp_offset - 1); if( _temp7082.base == 0? 1:( _temp7084 < _temp7082.base? 1:
_temp7084 >= _temp7082.last_plus_one)){ _throw( Null_Exception);}* _temp7084;}).first_line,({
struct _tagged_ptr3 _temp7085= yyls; struct Cyc_Yyltype* _temp7087= _temp7085.curr
+ yylsp_offset; if( _temp7085.base == 0? 1:( _temp7087 < _temp7085.base? 1:
_temp7087 >= _temp7085.last_plus_one)){ _throw( Null_Exception);}* _temp7087;}).last_line));
_temp7078;}); _temp7077;}); break; case 370: _LL7076: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7089=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7089[ 0]=({ struct Cyc_Exp_tok_struct
_temp7090; _temp7090.tag= Cyc_Exp_tok_tag; _temp7090.f1= Cyc_Absyn_post_dec_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7091= yyvs; struct _xtunion_struct**
_temp7093= _temp7091.curr +( yyvsp_offset - 1); if( _temp7091.base == 0? 1:(
_temp7093 < _temp7091.base? 1: _temp7093 >= _temp7091.last_plus_one)){ _throw(
Null_Exception);}* _temp7093;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7094= yyls; struct Cyc_Yyltype* _temp7096= _temp7094.curr +(
yylsp_offset - 1); if( _temp7094.base == 0? 1:( _temp7096 < _temp7094.base? 1:
_temp7096 >= _temp7094.last_plus_one)){ _throw( Null_Exception);}* _temp7096;}).first_line,({
struct _tagged_ptr3 _temp7097= yyls; struct Cyc_Yyltype* _temp7099= _temp7097.curr
+ yylsp_offset; if( _temp7097.base == 0? 1:( _temp7099 < _temp7097.base? 1:
_temp7099 >= _temp7097.last_plus_one)){ _throw( Null_Exception);}* _temp7099;}).last_line));
_temp7090;}); _temp7089;}); break; case 371: _LL7088: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7101=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7101[ 0]=({ struct Cyc_Exp_tok_struct
_temp7102; _temp7102.tag= Cyc_Exp_tok_tag; _temp7102.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_CompoundLit_e_struct* _temp7103=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp7103[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp7104; _temp7104.tag= Cyc_Absyn_CompoundLit_e_tag;
_temp7104.f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp7105= yyvs;
struct _xtunion_struct** _temp7107= _temp7105.curr +( yyvsp_offset - 4); if(
_temp7105.base == 0? 1:( _temp7107 < _temp7105.base? 1: _temp7107 >= _temp7105.last_plus_one)){
_throw( Null_Exception);}* _temp7107;})); _temp7104.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp7108= yyvs; struct _xtunion_struct** _temp7110=
_temp7108.curr +( yyvsp_offset - 1); if( _temp7108.base == 0? 1:( _temp7110 <
_temp7108.base? 1: _temp7110 >= _temp7108.last_plus_one)){ _throw(
Null_Exception);}* _temp7110;}))); _temp7104;}); _temp7103;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7111= yyls; struct Cyc_Yyltype* _temp7113= _temp7111.curr
+( yylsp_offset - 5); if( _temp7111.base == 0? 1:( _temp7113 < _temp7111.base? 1:
_temp7113 >= _temp7111.last_plus_one)){ _throw( Null_Exception);}* _temp7113;}).first_line,({
struct _tagged_ptr3 _temp7114= yyls; struct Cyc_Yyltype* _temp7116= _temp7114.curr
+ yylsp_offset; if( _temp7114.base == 0? 1:( _temp7116 < _temp7114.base? 1:
_temp7116 >= _temp7114.last_plus_one)){ _throw( Null_Exception);}* _temp7116;}).last_line));
_temp7102;}); _temp7101;}); break; case 372: _LL7100: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7118=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7118[ 0]=({ struct Cyc_Exp_tok_struct
_temp7119; _temp7119.tag= Cyc_Exp_tok_tag; _temp7119.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_CompoundLit_e_struct* _temp7120=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp7120[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp7121; _temp7121.tag= Cyc_Absyn_CompoundLit_e_tag;
_temp7121.f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp7122= yyvs;
struct _xtunion_struct** _temp7124= _temp7122.curr +( yyvsp_offset - 5); if(
_temp7122.base == 0? 1:( _temp7124 < _temp7122.base? 1: _temp7124 >= _temp7122.last_plus_one)){
_throw( Null_Exception);}* _temp7124;})); _temp7121.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp7125= yyvs; struct _xtunion_struct** _temp7127=
_temp7125.curr +( yyvsp_offset - 2); if( _temp7125.base == 0? 1:( _temp7127 <
_temp7125.base? 1: _temp7127 >= _temp7125.last_plus_one)){ _throw(
Null_Exception);}* _temp7127;}))); _temp7121;}); _temp7120;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7128= yyls; struct Cyc_Yyltype* _temp7130= _temp7128.curr
+( yylsp_offset - 6); if( _temp7128.base == 0? 1:( _temp7130 < _temp7128.base? 1:
_temp7130 >= _temp7128.last_plus_one)){ _throw( Null_Exception);}* _temp7130;}).first_line,({
struct _tagged_ptr3 _temp7131= yyls; struct Cyc_Yyltype* _temp7133= _temp7131.curr
+ yylsp_offset; if( _temp7131.base == 0? 1:( _temp7133 < _temp7131.base? 1:
_temp7133 >= _temp7131.last_plus_one)){ _throw( Null_Exception);}* _temp7133;}).last_line));
_temp7119;}); _temp7118;}); break; case 373: _LL7117: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7135=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7135[ 0]=({ struct Cyc_Exp_tok_struct
_temp7136; _temp7136.tag= Cyc_Exp_tok_tag; _temp7136.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Fill_e_struct* _temp7137=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp7137[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp7138; _temp7138.tag= Cyc_Absyn_Fill_e_tag; _temp7138.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp7139= yyvs; struct _xtunion_struct** _temp7141=
_temp7139.curr +( yyvsp_offset - 1); if( _temp7139.base == 0? 1:( _temp7141 <
_temp7139.base? 1: _temp7141 >= _temp7139.last_plus_one)){ _throw(
Null_Exception);}* _temp7141;})); _temp7138;}); _temp7137;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7142= yyls; struct Cyc_Yyltype* _temp7144= _temp7142.curr
+( yylsp_offset - 3); if( _temp7142.base == 0? 1:( _temp7144 < _temp7142.base? 1:
_temp7144 >= _temp7142.last_plus_one)){ _throw( Null_Exception);}* _temp7144;}).first_line,({
struct _tagged_ptr3 _temp7145= yyls; struct Cyc_Yyltype* _temp7147= _temp7145.curr
+ yylsp_offset; if( _temp7145.base == 0? 1:( _temp7147 < _temp7145.base? 1:
_temp7147 >= _temp7145.last_plus_one)){ _throw( Null_Exception);}* _temp7147;}).last_line));
_temp7136;}); _temp7135;}); break; case 374: _LL7134: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7149=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7149[ 0]=({ struct Cyc_Exp_tok_struct
_temp7150; _temp7150.tag= Cyc_Exp_tok_tag; _temp7150.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Codegen_e_struct* _temp7151=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp7151[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp7152; _temp7152.tag= Cyc_Absyn_Codegen_e_tag;
_temp7152.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp7153= yyvs;
struct _xtunion_struct** _temp7155= _temp7153.curr +( yyvsp_offset - 1); if(
_temp7153.base == 0? 1:( _temp7155 < _temp7153.base? 1: _temp7155 >= _temp7153.last_plus_one)){
_throw( Null_Exception);}* _temp7155;})); _temp7152;}); _temp7151;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7156= yyls; struct Cyc_Yyltype* _temp7158= _temp7156.curr
+( yylsp_offset - 3); if( _temp7156.base == 0? 1:( _temp7158 < _temp7156.base? 1:
_temp7158 >= _temp7156.last_plus_one)){ _throw( Null_Exception);}* _temp7158;}).first_line,({
struct _tagged_ptr3 _temp7159= yyls; struct Cyc_Yyltype* _temp7161= _temp7159.curr
+ yylsp_offset; if( _temp7159.base == 0? 1:( _temp7161 < _temp7159.base? 1:
_temp7161 >= _temp7159.last_plus_one)){ _throw( Null_Exception);}* _temp7161;}).last_line));
_temp7150;}); _temp7149;}); break; case 375: _LL7148: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7163=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7163[ 0]=({ struct Cyc_Exp_tok_struct
_temp7164; _temp7164.tag= Cyc_Exp_tok_tag; _temp7164.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp7165=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp7165[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp7166; _temp7166.tag= Cyc_Absyn_UnknownId_e_tag;
_temp7166.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp7167= yyvs;
struct _xtunion_struct** _temp7169= _temp7167.curr + yyvsp_offset; if( _temp7167.base
== 0? 1:( _temp7169 < _temp7167.base? 1: _temp7169 >= _temp7167.last_plus_one)){
_throw( Null_Exception);}* _temp7169;})); _temp7166;}); _temp7165;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7170= yyls; struct Cyc_Yyltype* _temp7172= _temp7170.curr
+ yylsp_offset; if( _temp7170.base == 0? 1:( _temp7172 < _temp7170.base? 1:
_temp7172 >= _temp7170.last_plus_one)){ _throw( Null_Exception);}* _temp7172;}).first_line,({
struct _tagged_ptr3 _temp7173= yyls; struct Cyc_Yyltype* _temp7175= _temp7173.curr
+ yylsp_offset; if( _temp7173.base == 0? 1:( _temp7175 < _temp7173.base? 1:
_temp7175 >= _temp7173.last_plus_one)){ _throw( Null_Exception);}* _temp7175;}).last_line));
_temp7164;}); _temp7163;}); break; case 376: _LL7162: yyval=({ struct
_tagged_ptr2 _temp7177= yyvs; struct _xtunion_struct** _temp7179= _temp7177.curr
+ yyvsp_offset; if( _temp7177.base == 0? 1:( _temp7179 < _temp7177.base? 1:
_temp7179 >= _temp7177.last_plus_one)){ _throw( Null_Exception);}* _temp7179;});
break; case 377: _LL7176: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7181=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7181[ 0]=({ struct Cyc_Exp_tok_struct _temp7182; _temp7182.tag= Cyc_Exp_tok_tag;
_temp7182.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp7183= yyvs; struct _xtunion_struct** _temp7185= _temp7183.curr +
yyvsp_offset; if( _temp7183.base == 0? 1:( _temp7185 < _temp7183.base? 1:
_temp7185 >= _temp7183.last_plus_one)){ _throw( Null_Exception);}* _temp7185;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7186= yyls; struct Cyc_Yyltype*
_temp7188= _temp7186.curr + yylsp_offset; if( _temp7186.base == 0? 1:( _temp7188
< _temp7186.base? 1: _temp7188 >= _temp7186.last_plus_one)){ _throw(
Null_Exception);}* _temp7188;}).first_line,({ struct _tagged_ptr3 _temp7189=
yyls; struct Cyc_Yyltype* _temp7191= _temp7189.curr + yylsp_offset; if(
_temp7189.base == 0? 1:( _temp7191 < _temp7189.base? 1: _temp7191 >= _temp7189.last_plus_one)){
_throw( Null_Exception);}* _temp7191;}).last_line)); _temp7182;}); _temp7181;});
break; case 378: _LL7180: yyval=({ struct _tagged_ptr2 _temp7193= yyvs; struct
_xtunion_struct** _temp7195= _temp7193.curr +( yyvsp_offset - 1); if( _temp7193.base
== 0? 1:( _temp7195 < _temp7193.base? 1: _temp7195 >= _temp7193.last_plus_one)){
_throw( Null_Exception);}* _temp7195;}); break; case 379: _LL7192: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7197=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7197[ 0]=({ struct Cyc_Exp_tok_struct
_temp7198; _temp7198.tag= Cyc_Exp_tok_tag; _temp7198.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp7199=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7199[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7200; _temp7200.tag=
Cyc_Absyn_UnknownId_e_tag; _temp7200.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp7201= yyvs; struct _xtunion_struct** _temp7203= _temp7201.curr
+( yyvsp_offset - 1); if( _temp7201.base == 0? 1:( _temp7203 < _temp7201.base? 1:
_temp7203 >= _temp7201.last_plus_one)){ _throw( Null_Exception);}* _temp7203;}));
_temp7200;}); _temp7199;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7204= yyls; struct Cyc_Yyltype* _temp7206= _temp7204.curr +( yylsp_offset -
1); if( _temp7204.base == 0? 1:( _temp7206 < _temp7204.base? 1: _temp7206 >=
_temp7204.last_plus_one)){ _throw( Null_Exception);}* _temp7206;}).first_line,({
struct _tagged_ptr3 _temp7207= yyls; struct Cyc_Yyltype* _temp7209= _temp7207.curr
+( yylsp_offset - 1); if( _temp7207.base == 0? 1:( _temp7209 < _temp7207.base? 1:
_temp7209 >= _temp7207.last_plus_one)){ _throw( Null_Exception);}* _temp7209;}).last_line)),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7210= yyls; struct Cyc_Yyltype*
_temp7212= _temp7210.curr +( yylsp_offset - 1); if( _temp7210.base == 0? 1:(
_temp7212 < _temp7210.base? 1: _temp7212 >= _temp7210.last_plus_one)){ _throw(
Null_Exception);}* _temp7212;}).first_line,({ struct _tagged_ptr3 _temp7213=
yyls; struct Cyc_Yyltype* _temp7215= _temp7213.curr + yylsp_offset; if(
_temp7213.base == 0? 1:( _temp7215 < _temp7213.base? 1: _temp7215 >= _temp7213.last_plus_one)){
_throw( Null_Exception);}* _temp7215;}).last_line)); _temp7198;}); _temp7197;});
break; case 380: _LL7196: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7217=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7217[ 0]=({ struct Cyc_Exp_tok_struct _temp7218; _temp7218.tag= Cyc_Exp_tok_tag;
_temp7218.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp7219=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7219[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7220; _temp7220.tag=
Cyc_Absyn_UnknownId_e_tag; _temp7220.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp7221= yyvs; struct _xtunion_struct** _temp7223= _temp7221.curr
+( yyvsp_offset - 4); if( _temp7221.base == 0? 1:( _temp7223 < _temp7221.base? 1:
_temp7223 >= _temp7221.last_plus_one)){ _throw( Null_Exception);}* _temp7223;}));
_temp7220;}); _temp7219;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7224= yyls; struct Cyc_Yyltype* _temp7226= _temp7224.curr +( yylsp_offset -
4); if( _temp7224.base == 0? 1:( _temp7226 < _temp7224.base? 1: _temp7226 >=
_temp7224.last_plus_one)){ _throw( Null_Exception);}* _temp7226;}).first_line,({
struct _tagged_ptr3 _temp7227= yyls; struct Cyc_Yyltype* _temp7229= _temp7227.curr
+( yylsp_offset - 4); if( _temp7227.base == 0? 1:( _temp7229 < _temp7227.base? 1:
_temp7229 >= _temp7227.last_plus_one)){ _throw( Null_Exception);}* _temp7229;}).last_line)),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp7230= yyvs; struct _xtunion_struct** _temp7232=
_temp7230.curr +( yyvsp_offset - 1); if( _temp7230.base == 0? 1:( _temp7232 <
_temp7230.base? 1: _temp7232 >= _temp7230.last_plus_one)){ _throw(
Null_Exception);}* _temp7232;}))), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7233= yyls; struct Cyc_Yyltype* _temp7235= _temp7233.curr +(
yylsp_offset - 4); if( _temp7233.base == 0? 1:( _temp7235 < _temp7233.base? 1:
_temp7235 >= _temp7233.last_plus_one)){ _throw( Null_Exception);}* _temp7235;}).first_line,({
struct _tagged_ptr3 _temp7236= yyls; struct Cyc_Yyltype* _temp7238= _temp7236.curr
+ yylsp_offset; if( _temp7236.base == 0? 1:( _temp7238 < _temp7236.base? 1:
_temp7238 >= _temp7236.last_plus_one)){ _throw( Null_Exception);}* _temp7238;}).last_line));
_temp7218;}); _temp7217;}); break; case 381: _LL7216: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7240=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7240[ 0]=({ struct Cyc_Exp_tok_struct
_temp7241; _temp7241.tag= Cyc_Exp_tok_tag; _temp7241.f1= Cyc_Absyn_tuple_exp(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp7242= yyvs; struct
_xtunion_struct** _temp7244= _temp7242.curr +( yyvsp_offset - 1); if( _temp7242.base
== 0? 1:( _temp7244 < _temp7242.base? 1: _temp7244 >= _temp7242.last_plus_one)){
_throw( Null_Exception);}* _temp7244;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7245= yyls; struct Cyc_Yyltype* _temp7247= _temp7245.curr +(
yylsp_offset - 3); if( _temp7245.base == 0? 1:( _temp7247 < _temp7245.base? 1:
_temp7247 >= _temp7245.last_plus_one)){ _throw( Null_Exception);}* _temp7247;}).first_line,({
struct _tagged_ptr3 _temp7248= yyls; struct Cyc_Yyltype* _temp7250= _temp7248.curr
+ yylsp_offset; if( _temp7248.base == 0? 1:( _temp7250 < _temp7248.base? 1:
_temp7250 >= _temp7248.last_plus_one)){ _throw( Null_Exception);}* _temp7250;}).last_line));
_temp7241;}); _temp7240;}); break; case 382: _LL7239: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7252=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7252[ 0]=({ struct Cyc_Exp_tok_struct
_temp7253; _temp7253.tag= Cyc_Exp_tok_tag; _temp7253.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Struct_e_struct* _temp7254=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp7254[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp7255; _temp7255.tag= Cyc_Absyn_Struct_e_tag;
_temp7255.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp7256= yyvs;
struct _xtunion_struct** _temp7258= _temp7256.curr +( yyvsp_offset - 3); if(
_temp7256.base == 0? 1:( _temp7258 < _temp7256.base? 1: _temp7258 >= _temp7256.last_plus_one)){
_throw( Null_Exception);}* _temp7258;})); _temp7255.f2= 0; _temp7255.f3=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp7259= yyvs; struct _xtunion_struct** _temp7261=
_temp7259.curr +( yyvsp_offset - 1); if( _temp7259.base == 0? 1:( _temp7261 <
_temp7259.base? 1: _temp7261 >= _temp7259.last_plus_one)){ _throw(
Null_Exception);}* _temp7261;}))); _temp7255.f4= 0; _temp7255;}); _temp7254;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7262= yyls; struct Cyc_Yyltype*
_temp7264= _temp7262.curr +( yylsp_offset - 3); if( _temp7262.base == 0? 1:(
_temp7264 < _temp7262.base? 1: _temp7264 >= _temp7262.last_plus_one)){ _throw(
Null_Exception);}* _temp7264;}).first_line,({ struct _tagged_ptr3 _temp7265=
yyls; struct Cyc_Yyltype* _temp7267= _temp7265.curr + yylsp_offset; if(
_temp7265.base == 0? 1:( _temp7267 < _temp7265.base? 1: _temp7267 >= _temp7265.last_plus_one)){
_throw( Null_Exception);}* _temp7267;}).last_line)); _temp7253;}); _temp7252;});
break; case 383: _LL7251: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7269=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7269[ 0]=({ struct Cyc_Exp_tok_struct _temp7270; _temp7270.tag= Cyc_Exp_tok_tag;
_temp7270.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp7271= yyvs; struct _xtunion_struct** _temp7273= _temp7271.curr +(
yyvsp_offset - 2); if( _temp7271.base == 0? 1:( _temp7273 < _temp7271.base? 1:
_temp7273 >= _temp7271.last_plus_one)){ _throw( Null_Exception);}* _temp7273;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7274= yyls; struct Cyc_Yyltype*
_temp7276= _temp7274.curr +( yylsp_offset - 4); if( _temp7274.base == 0? 1:(
_temp7276 < _temp7274.base? 1: _temp7276 >= _temp7274.last_plus_one)){ _throw(
Null_Exception);}* _temp7276;}).first_line,({ struct _tagged_ptr3 _temp7277=
yyls; struct Cyc_Yyltype* _temp7279= _temp7277.curr + yylsp_offset; if(
_temp7277.base == 0? 1:( _temp7279 < _temp7277.base? 1: _temp7279 >= _temp7277.last_plus_one)){
_throw( Null_Exception);}* _temp7279;}).last_line)); _temp7270;}); _temp7269;});
break; case 384: _LL7268: yyval=( struct _xtunion_struct*)({ struct Cyc_ExpList_tok_struct*
_temp7281=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp7281[ 0]=({ struct Cyc_ExpList_tok_struct _temp7282; _temp7282.tag= Cyc_ExpList_tok_tag;
_temp7282.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp7283= yyvs; struct
_xtunion_struct** _temp7285= _temp7283.curr + yyvsp_offset; if( _temp7283.base
== 0? 1:( _temp7285 < _temp7283.base? 1: _temp7285 >= _temp7283.last_plus_one)){
_throw( Null_Exception);}* _temp7285;}))); _temp7282;}); _temp7281;}); break;
case 385: _LL7280: yyval=( struct _xtunion_struct*)({ struct Cyc_ExpList_tok_struct*
_temp7287=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp7287[ 0]=({ struct Cyc_ExpList_tok_struct _temp7288; _temp7288.tag= Cyc_ExpList_tok_tag;
_temp7288.f1=({ struct Cyc_List_List* _temp7289=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp7289->hd=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp7290= yyvs; struct _xtunion_struct** _temp7292=
_temp7290.curr + yyvsp_offset; if( _temp7290.base == 0? 1:( _temp7292 <
_temp7290.base? 1: _temp7292 >= _temp7290.last_plus_one)){ _throw(
Null_Exception);}* _temp7292;})); _temp7289->tl= 0; _temp7289;}); _temp7288;});
_temp7287;}); break; case 386: _LL7286: yyval=( struct _xtunion_struct*)({
struct Cyc_ExpList_tok_struct* _temp7294=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp7294[ 0]=({ struct Cyc_ExpList_tok_struct
_temp7295; _temp7295.tag= Cyc_ExpList_tok_tag; _temp7295.f1=({ struct Cyc_List_List*
_temp7296=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp7296->hd=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7300= yyvs;
struct _xtunion_struct** _temp7302= _temp7300.curr + yyvsp_offset; if( _temp7300.base
== 0? 1:( _temp7302 < _temp7300.base? 1: _temp7302 >= _temp7300.last_plus_one)){
_throw( Null_Exception);}* _temp7302;})); _temp7296->tl= Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp7297= yyvs; struct _xtunion_struct** _temp7299=
_temp7297.curr +( yyvsp_offset - 2); if( _temp7297.base == 0? 1:( _temp7299 <
_temp7297.base? 1: _temp7299 >= _temp7297.last_plus_one)){ _throw(
Null_Exception);}* _temp7299;})); _temp7296;}); _temp7295;}); _temp7294;});
break; case 387: _LL7293: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7304=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7304[ 0]=({ struct Cyc_Exp_tok_struct _temp7305; _temp7305.tag= Cyc_Exp_tok_tag;
_temp7305.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(({ struct _tagged_ptr2
_temp7306= yyvs; struct _xtunion_struct** _temp7308= _temp7306.curr +
yyvsp_offset; if( _temp7306.base == 0? 1:( _temp7308 < _temp7306.base? 1:
_temp7308 >= _temp7306.last_plus_one)){ _throw( Null_Exception);}* _temp7308;}))).f1,(*
Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp7309= yyvs; struct _xtunion_struct**
_temp7311= _temp7309.curr + yyvsp_offset; if( _temp7309.base == 0? 1:( _temp7311
< _temp7309.base? 1: _temp7311 >= _temp7309.last_plus_one)){ _throw(
Null_Exception);}* _temp7311;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7312= yyls; struct Cyc_Yyltype* _temp7314= _temp7312.curr +
yylsp_offset; if( _temp7312.base == 0? 1:( _temp7314 < _temp7312.base? 1:
_temp7314 >= _temp7312.last_plus_one)){ _throw( Null_Exception);}* _temp7314;}).first_line,({
struct _tagged_ptr3 _temp7315= yyls; struct Cyc_Yyltype* _temp7317= _temp7315.curr
+ yylsp_offset; if( _temp7315.base == 0? 1:( _temp7317 < _temp7315.base? 1:
_temp7317 >= _temp7315.last_plus_one)){ _throw( Null_Exception);}* _temp7317;}).last_line));
_temp7305;}); _temp7304;}); break; case 388: _LL7303: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7319=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7319[ 0]=({ struct Cyc_Exp_tok_struct
_temp7320; _temp7320.tag= Cyc_Exp_tok_tag; _temp7320.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok(({
struct _tagged_ptr2 _temp7321= yyvs; struct _xtunion_struct** _temp7323=
_temp7321.curr + yyvsp_offset; if( _temp7321.base == 0? 1:( _temp7323 <
_temp7321.base? 1: _temp7323 >= _temp7321.last_plus_one)){ _throw(
Null_Exception);}* _temp7323;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7324= yyls; struct Cyc_Yyltype* _temp7326= _temp7324.curr +
yylsp_offset; if( _temp7324.base == 0? 1:( _temp7326 < _temp7324.base? 1:
_temp7326 >= _temp7324.last_plus_one)){ _throw( Null_Exception);}* _temp7326;}).first_line,({
struct _tagged_ptr3 _temp7327= yyls; struct Cyc_Yyltype* _temp7329= _temp7327.curr
+ yylsp_offset; if( _temp7327.base == 0? 1:( _temp7329 < _temp7327.base? 1:
_temp7329 >= _temp7327.last_plus_one)){ _throw( Null_Exception);}* _temp7329;}).last_line));
_temp7320;}); _temp7319;}); break; case 389: _LL7318: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7331=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7331[ 0]=({ struct Cyc_Exp_tok_struct
_temp7332; _temp7332.tag= Cyc_Exp_tok_tag; _temp7332.f1= Cyc_Absyn_float_exp(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp7333= yyvs; struct
_xtunion_struct** _temp7335= _temp7333.curr + yyvsp_offset; if( _temp7333.base
== 0? 1:( _temp7335 < _temp7333.base? 1: _temp7335 >= _temp7333.last_plus_one)){
_throw( Null_Exception);}* _temp7335;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7336= yyls; struct Cyc_Yyltype* _temp7338= _temp7336.curr +
yylsp_offset; if( _temp7336.base == 0? 1:( _temp7338 < _temp7336.base? 1:
_temp7338 >= _temp7336.last_plus_one)){ _throw( Null_Exception);}* _temp7338;}).first_line,({
struct _tagged_ptr3 _temp7339= yyls; struct Cyc_Yyltype* _temp7341= _temp7339.curr
+ yylsp_offset; if( _temp7339.base == 0? 1:( _temp7341 < _temp7339.base? 1:
_temp7341 >= _temp7339.last_plus_one)){ _throw( Null_Exception);}* _temp7341;}).last_line));
_temp7332;}); _temp7331;}); break; case 390: _LL7330: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7343=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7343[ 0]=({ struct Cyc_Exp_tok_struct
_temp7344; _temp7344.tag= Cyc_Exp_tok_tag; _temp7344.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7345= yyls; struct Cyc_Yyltype* _temp7347= _temp7345.curr
+ yylsp_offset; if( _temp7345.base == 0? 1:( _temp7347 < _temp7345.base? 1:
_temp7347 >= _temp7345.last_plus_one)){ _throw( Null_Exception);}* _temp7347;}).first_line,({
struct _tagged_ptr3 _temp7348= yyls; struct Cyc_Yyltype* _temp7350= _temp7348.curr
+ yylsp_offset; if( _temp7348.base == 0? 1:( _temp7350 < _temp7348.base? 1:
_temp7350 >= _temp7348.last_plus_one)){ _throw( Null_Exception);}* _temp7350;}).last_line));
_temp7344;}); _temp7343;}); break; case 391: _LL7342: yyval=( struct
_xtunion_struct*)({ struct Cyc_QualId_tok_struct* _temp7352=( struct Cyc_QualId_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualId_tok_struct)); _temp7352[ 0]=({ struct Cyc_QualId_tok_struct
_temp7353; _temp7353.tag= Cyc_QualId_tok_tag; _temp7353.f1=({ struct _tuple1*
_temp7354=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp7354->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp7359=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp7359[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp7360; _temp7360.tag= Cyc_Absyn_Rel_n_tag; _temp7360.f1= 0; _temp7360;});
_temp7359;}); _temp7354->f2=({ struct _tagged_string* _temp7355=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp7355[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp7356= yyvs; struct
_xtunion_struct** _temp7358= _temp7356.curr + yyvsp_offset; if( _temp7356.base
== 0? 1:( _temp7358 < _temp7356.base? 1: _temp7358 >= _temp7356.last_plus_one)){
_throw( Null_Exception);}* _temp7358;})); _temp7355;}); _temp7354;}); _temp7353;});
_temp7352;}); break; case 392: _LL7351: yyval=({ struct _tagged_ptr2 _temp7362=
yyvs; struct _xtunion_struct** _temp7364= _temp7362.curr + yyvsp_offset; if(
_temp7362.base == 0? 1:( _temp7364 < _temp7362.base? 1: _temp7364 >= _temp7362.last_plus_one)){
_throw( Null_Exception);}* _temp7364;}); break; default: _LL7361:( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Yyimpossible_struct* _temp7366=( struct
Cyc_Yyimpossible_struct*) GC_malloc( sizeof( struct Cyc_Yyimpossible_struct));
_temp7366[ 0]=({ struct Cyc_Yyimpossible_struct _temp7367; _temp7367.tag= Cyc_Yyimpossible_tag;
_temp7367;}); _temp7366;}));} yyvsp_offset -= yylen; yyssp_offset -= yylen;
yylsp_offset -= yylen;({ struct _tagged_ptr2 _temp7368= yyvs; struct
_xtunion_struct** _temp7370= _temp7368.curr +( ++ yyvsp_offset); if( _temp7368.base
== 0? 1:( _temp7370 < _temp7368.base? 1: _temp7370 >= _temp7368.last_plus_one)){
_throw( Null_Exception);}* _temp7370= yyval;}); yylsp_offset ++; if( yylen == 0){({
struct _tagged_ptr3 _temp7371= yyls; struct Cyc_Yyltype* _temp7373= _temp7371.curr
+ yylsp_offset; if( _temp7371.base == 0? 1:( _temp7373 < _temp7371.base? 1:
_temp7373 >= _temp7371.last_plus_one)){ _throw( Null_Exception);}(* _temp7373).first_line=
Cyc_yylloc.first_line;});({ struct _tagged_ptr3 _temp7374= yyls; struct Cyc_Yyltype*
_temp7376= _temp7374.curr + yylsp_offset; if( _temp7374.base == 0? 1:( _temp7376
< _temp7374.base? 1: _temp7376 >= _temp7374.last_plus_one)){ _throw(
Null_Exception);}(* _temp7376).first_column= Cyc_yylloc.first_column;});({
struct _tagged_ptr3 _temp7377= yyls; struct Cyc_Yyltype* _temp7379= _temp7377.curr
+ yylsp_offset; if( _temp7377.base == 0? 1:( _temp7379 < _temp7377.base? 1:
_temp7379 >= _temp7377.last_plus_one)){ _throw( Null_Exception);}(* _temp7379).last_line=({
struct _tagged_ptr3 _temp7380= yyls; struct Cyc_Yyltype* _temp7382= _temp7380.curr
+( yylsp_offset - 1); if( _temp7380.base == 0? 1:( _temp7382 < _temp7380.base? 1:
_temp7382 >= _temp7380.last_plus_one)){ _throw( Null_Exception);}* _temp7382;}).last_line;});({
struct _tagged_ptr3 _temp7383= yyls; struct Cyc_Yyltype* _temp7385= _temp7383.curr
+ yylsp_offset; if( _temp7383.base == 0? 1:( _temp7385 < _temp7383.base? 1:
_temp7385 >= _temp7383.last_plus_one)){ _throw( Null_Exception);}(* _temp7385).last_column=({
struct _tagged_ptr3 _temp7386= yyls; struct Cyc_Yyltype* _temp7388= _temp7386.curr
+( yylsp_offset - 1); if( _temp7386.base == 0? 1:( _temp7388 < _temp7386.base? 1:
_temp7388 >= _temp7386.last_plus_one)){ _throw( Null_Exception);}* _temp7388;}).last_column;});({
struct _tagged_ptr3 _temp7389= yyls; struct Cyc_Yyltype* _temp7391= _temp7389.curr
+ yylsp_offset; if( _temp7389.base == 0? 1:( _temp7391 < _temp7389.base? 1:
_temp7391 >= _temp7389.last_plus_one)){ _throw( Null_Exception);}(* _temp7391).text=(
struct _tagged_string)({ char* _temp7392=( char*)""; struct _tagged_string
_temp7393; _temp7393.curr= _temp7392; _temp7393.base= _temp7392; _temp7393.last_plus_one=
_temp7392 + 1; _temp7393;});});} else{({ struct _tagged_ptr3 _temp7394= yyls;
struct Cyc_Yyltype* _temp7396= _temp7394.curr + yylsp_offset; if( _temp7394.base
== 0? 1:( _temp7396 < _temp7394.base? 1: _temp7396 >= _temp7394.last_plus_one)){
_throw( Null_Exception);}(* _temp7396).last_line=({ struct _tagged_ptr3
_temp7397= yyls; struct Cyc_Yyltype* _temp7399= _temp7397.curr +(( yylsp_offset
+ yylen) - 1); if( _temp7397.base == 0? 1:( _temp7399 < _temp7397.base? 1:
_temp7399 >= _temp7397.last_plus_one)){ _throw( Null_Exception);}* _temp7399;}).last_line;});({
struct _tagged_ptr3 _temp7400= yyls; struct Cyc_Yyltype* _temp7402= _temp7400.curr
+ yylsp_offset; if( _temp7400.base == 0? 1:( _temp7402 < _temp7400.base? 1:
_temp7402 >= _temp7400.last_plus_one)){ _throw( Null_Exception);}(* _temp7402).last_column=({
struct _tagged_ptr3 _temp7403= yyls; struct Cyc_Yyltype* _temp7405= _temp7403.curr
+(( yylsp_offset + yylen) - 1); if( _temp7403.base == 0? 1:( _temp7405 <
_temp7403.base? 1: _temp7405 >= _temp7403.last_plus_one)){ _throw(
Null_Exception);}* _temp7405;}).last_column;});} yyn=( int)({ short* _temp7406=(
short*) Cyc_yyr1; unsigned int _temp7407= yyn; if( _temp7407 >= 393u){ _throw(
Null_Exception);} _temp7406[ _temp7407];}); yystate=({ short* _temp7408=( short*)
Cyc_yypgoto; unsigned int _temp7409= yyn - 124; if( _temp7409 >= 106u){ _throw(
Null_Exception);} _temp7408[ _temp7409];}) +({ struct _tagged_ptr1 _temp7410=
yyss; short* _temp7412= _temp7410.curr + yyssp_offset; if( _temp7410.base == 0?
1:( _temp7412 < _temp7410.base? 1: _temp7412 >= _temp7410.last_plus_one)){
_throw( Null_Exception);}* _temp7412;}); if(( yystate >= 0? yystate <= 4214: 0)?({
short* _temp7413=( short*) Cyc_yycheck; unsigned int _temp7414= yystate; if(
_temp7414 >= 4215u){ _throw( Null_Exception);} _temp7413[ _temp7414];}) ==({
struct _tagged_ptr1 _temp7415= yyss; short* _temp7417= _temp7415.curr +
yyssp_offset; if( _temp7415.base == 0? 1:( _temp7417 < _temp7415.base? 1:
_temp7417 >= _temp7415.last_plus_one)){ _throw( Null_Exception);}* _temp7417;}):
0){ yystate=( int)({ short* _temp7418=( short*) Cyc_yytable; unsigned int
_temp7419= yystate; if( _temp7419 >= 4215u){ _throw( Null_Exception);} _temp7418[
_temp7419];});} else{ yystate=( int)({ short* _temp7420=( short*) Cyc_yydefgoto;
unsigned int _temp7421= yyn - 124; if( _temp7421 >= 106u){ _throw(
Null_Exception);} _temp7420[ _temp7421];});} goto yynewstate; yyerrlab: if(
yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int)({ short* _temp7422=( short*) Cyc_yypact;
unsigned int _temp7423= yystate; if( _temp7423 >= 755u){ _throw( Null_Exception);}
_temp7422[ _temp7423];}); if( yyn > - 32768? yyn < 4214: 0){ int sze= 0; struct
_tagged_string msg; int x; int count; count= 0; for( x= yyn < 0? - yyn: 0; x <
230u / sizeof( char*); x ++){ if(({ short* _temp7424=( short*) Cyc_yycheck;
unsigned int _temp7425= x + yyn; if( _temp7425 >= 4215u){ _throw( Null_Exception);}
_temp7424[ _temp7425];}) == x){ sze += Cyc_String_strlen(({ struct
_tagged_string* _temp7426=( struct _tagged_string*) Cyc_yytname; unsigned int
_temp7427= x; if( _temp7427 >= 230u){ _throw( Null_Exception);} _temp7426[
_temp7427];})) + 15, count ++;}} msg=({ unsigned int _temp7428=( unsigned int)(
sze + 15); char* _temp7429=( char*) GC_malloc_atomic( sizeof( char) * _temp7428);
unsigned int i; struct _tagged_string _temp7430={ _temp7429, _temp7429,
_temp7429 + _temp7428}; for( i= 0; i < _temp7428; i ++){ _temp7429[ i]='\000';}
_temp7430;}); Cyc_String_strcpy( msg,( struct _tagged_string)({ char* _temp7431=(
char*)"parse error"; struct _tagged_string _temp7432; _temp7432.curr= _temp7431;
_temp7432.base= _temp7431; _temp7432.last_plus_one= _temp7431 + 12; _temp7432;}));
if( count < 5){ count= 0; for( x= yyn < 0? - yyn: 0; x < 230u / sizeof( char*);
x ++){ if(({ short* _temp7433=( short*) Cyc_yycheck; unsigned int _temp7434= x +
yyn; if( _temp7434 >= 4215u){ _throw( Null_Exception);} _temp7433[ _temp7434];})
== x){ Cyc_String_strcat( msg, count == 0?( struct _tagged_string)({ char*
_temp7435=( char*)", expecting `"; struct _tagged_string _temp7436; _temp7436.curr=
_temp7435; _temp7436.base= _temp7435; _temp7436.last_plus_one= _temp7435 + 14;
_temp7436;}):( struct _tagged_string)({ char* _temp7437=( char*)" or `"; struct
_tagged_string _temp7438; _temp7438.curr= _temp7437; _temp7438.base= _temp7437;
_temp7438.last_plus_one= _temp7437 + 6; _temp7438;})); Cyc_String_strcat( msg,({
struct _tagged_string* _temp7439=( struct _tagged_string*) Cyc_yytname;
unsigned int _temp7440= x; if( _temp7440 >= 230u){ _throw( Null_Exception);}
_temp7439[ _temp7440];})); Cyc_String_strcat( msg,( struct _tagged_string)({
char* _temp7441=( char*)"'"; struct _tagged_string _temp7442; _temp7442.curr=
_temp7441; _temp7442.base= _temp7441; _temp7442.last_plus_one= _temp7441 + 2;
_temp7442;})); count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror(( struct
_tagged_string)({ char* _temp7443=( char*)"parse error"; struct _tagged_string
_temp7444; _temp7444.curr= _temp7443; _temp7444.base= _temp7443; _temp7444.last_plus_one=
_temp7443 + 12; _temp7444;}));}} goto yyerrlab1; yyerrlab1: if( yyerrstatus == 3){
if( Cyc_yychar == 0){ return 1;} Cyc_yychar= - 2;} yyerrstatus= 3; goto
yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset == 0){ return 1;}
yyvsp_offset --; yystate=( int)({ struct _tagged_ptr1 _temp7445= yyss; short*
_temp7447= _temp7445.curr +( -- yyssp_offset); if( _temp7445.base == 0? 1:(
_temp7447 < _temp7445.base? 1: _temp7447 >= _temp7445.last_plus_one)){ _throw(
Null_Exception);}* _temp7447;}); yylsp_offset --; yyerrhandle: yyn=( int)({
short* _temp7448=( short*) Cyc_yypact; unsigned int _temp7449= yystate; if(
_temp7449 >= 755u){ _throw( Null_Exception);} _temp7448[ _temp7449];}); if( yyn
== - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn < 0? 1: yyn > 4214)? 1:({
short* _temp7450=( short*) Cyc_yycheck; unsigned int _temp7451= yyn; if(
_temp7451 >= 4215u){ _throw( Null_Exception);} _temp7450[ _temp7451];}) != 1){
goto yyerrdefault;} yyn=( int)({ short* _temp7452=( short*) Cyc_yytable;
unsigned int _temp7453= yyn; if( _temp7453 >= 4215u){ _throw( Null_Exception);}
_temp7452[ _temp7453];}); if( yyn < 0){ if( yyn == - 32768){ goto yyerrpop;} yyn=
- yyn; goto yyreduce;} else{ if( yyn == 0){ goto yyerrpop;}} if( yyn == 754){
return 0;}({ struct _tagged_ptr2 _temp7454= yyvs; struct _xtunion_struct**
_temp7456= _temp7454.curr +( ++ yyvsp_offset); if( _temp7454.base == 0? 1:(
_temp7456 < _temp7454.base? 1: _temp7456 >= _temp7454.last_plus_one)){ _throw(
Null_Exception);}* _temp7456= Cyc_yylval;});({ struct _tagged_ptr3 _temp7457=
yyls; struct Cyc_Yyltype* _temp7459= _temp7457.curr +( ++ yylsp_offset); if(
_temp7457.base == 0? 1:( _temp7459 < _temp7457.base? 1: _temp7459 >= _temp7457.last_plus_one)){
_throw( Null_Exception);}* _temp7459= Cyc_yylloc;}); goto yynewstate;} void Cyc_yyprint(
int i, struct _xtunion_struct* v){ struct _xtunion_struct* _temp7460= v; struct
_tuple12* _temp7480; struct _tuple12 _temp7482; int _temp7483; void* _temp7485;
char _temp7487; short _temp7489; struct _tagged_string _temp7491; struct Cyc_Core_Opt*
_temp7493; struct Cyc_Core_Opt* _temp7495; struct Cyc_Core_Opt _temp7497; struct
_tagged_string* _temp7498; struct _tuple1* _temp7500; struct _tuple1 _temp7502;
struct _tagged_string* _temp7503; void* _temp7505; _LL7462: if( _temp7460->tag
== Cyc_Okay_tok_tag){ goto _LL7463;} else{ goto _LL7464;} _LL7464: if((*((
struct _xtunion_struct*) _temp7460)).tag == Cyc_Int_tok_tag){ _LL7481: _temp7480=((
struct Cyc_Int_tok_struct*) _temp7460)->f1; _temp7482=* _temp7480; _LL7486:
_temp7485= _temp7482.f1; goto _LL7484; _LL7484: _temp7483= _temp7482.f2; goto
_LL7465;} else{ goto _LL7466;} _LL7466: if((*(( struct _xtunion_struct*)
_temp7460)).tag == Cyc_Char_tok_tag){ _LL7488: _temp7487=(( struct Cyc_Char_tok_struct*)
_temp7460)->f1; goto _LL7467;} else{ goto _LL7468;} _LL7468: if((*(( struct
_xtunion_struct*) _temp7460)).tag == Cyc_Short_tok_tag){ _LL7490: _temp7489=((
struct Cyc_Short_tok_struct*) _temp7460)->f1; goto _LL7469;} else{ goto _LL7470;}
_LL7470: if((*(( struct _xtunion_struct*) _temp7460)).tag == Cyc_String_tok_tag){
_LL7492: _temp7491=(( struct Cyc_String_tok_struct*) _temp7460)->f1; goto
_LL7471;} else{ goto _LL7472;} _LL7472: if((*(( struct _xtunion_struct*)
_temp7460)).tag == Cyc_Stringopt_tok_tag){ _LL7494: _temp7493=(( struct Cyc_Stringopt_tok_struct*)
_temp7460)->f1; if( _temp7493 == 0){ goto _LL7473;} else{ goto _LL7474;}} else{
goto _LL7474;} _LL7474: if((*(( struct _xtunion_struct*) _temp7460)).tag == Cyc_Stringopt_tok_tag){
_LL7496: _temp7495=(( struct Cyc_Stringopt_tok_struct*) _temp7460)->f1; if(
_temp7495 == 0){ goto _LL7476;} else{ _temp7497=* _temp7495; _LL7499: _temp7498=(
struct _tagged_string*) _temp7497.v; goto _LL7475;}} else{ goto _LL7476;}
_LL7476: if((*(( struct _xtunion_struct*) _temp7460)).tag == Cyc_QualId_tok_tag){
_LL7501: _temp7500=(( struct Cyc_QualId_tok_struct*) _temp7460)->f1; _temp7502=*
_temp7500; _LL7506: _temp7505= _temp7502.f1; goto _LL7504; _LL7504: _temp7503=
_temp7502.f2; goto _LL7477;} else{ goto _LL7478;} _LL7478: goto _LL7479; _LL7463:
fprintf( Cyc_Stdio_stderr,"ok"); goto _LL7461; _LL7465: fprintf( Cyc_Stdio_stderr,"%d",
_temp7483); goto _LL7461; _LL7467: fprintf( Cyc_Stdio_stderr,"%c",( int)
_temp7487); goto _LL7461; _LL7469: fprintf( Cyc_Stdio_stderr,"%ds",( int)
_temp7489); goto _LL7461; _LL7471:({ struct _tagged_string _temp7507= _temp7491;
fprintf( Cyc_Stdio_stderr,"\"%.*s\"", _temp7507.last_plus_one - _temp7507.curr,
_temp7507.curr);}); goto _LL7461; _LL7473: fprintf( Cyc_Stdio_stderr,"null");
goto _LL7461; _LL7475:({ struct _tagged_string _temp7508=* _temp7498; fprintf(
Cyc_Stdio_stderr,"\"%.*s\"", _temp7508.last_plus_one - _temp7508.curr, _temp7508.curr);});
goto _LL7461; _LL7477: { struct Cyc_List_List* prefix= 0;{ void* _temp7509=
_temp7505; struct Cyc_List_List* _temp7517; struct Cyc_List_List* _temp7519;
_LL7511: if(( unsigned int) _temp7509 > 1u?(( struct _tunion_struct*) _temp7509)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL7518: _temp7517=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp7509)->f1; goto _LL7512;} else{ goto _LL7513;}
_LL7513: if(( unsigned int) _temp7509 > 1u?(( struct _tunion_struct*) _temp7509)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL7520: _temp7519=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp7509)->f1; goto _LL7514;} else{ goto _LL7515;}
_LL7515: if(( int) _temp7509 == Cyc_Absyn_Loc_n){ goto _LL7516;} else{ goto
_LL7510;} _LL7512: prefix= _temp7517; goto _LL7510; _LL7514: prefix= _temp7519;
goto _LL7510; _LL7516: goto _LL7510; _LL7510:;} for( 0; prefix != 0; prefix=({
struct Cyc_List_List* _temp7521= prefix; if( _temp7521 == 0){ _throw(
Null_Exception);} _temp7521->tl;})){({ struct _tagged_string _temp7523=*((
struct _tagged_string*)({ struct Cyc_List_List* _temp7522= prefix; if( _temp7522
== 0){ _throw( Null_Exception);} _temp7522->hd;})); fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp7523.last_plus_one - _temp7523.curr, _temp7523.curr);});}({ struct
_tagged_string _temp7524=* _temp7503; fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp7524.last_plus_one - _temp7524.curr, _temp7524.curr);}); goto _LL7461;}
_LL7479: fprintf( Cyc_Stdio_stderr,"?"); goto _LL7461; _LL7461:;} struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0;
Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp7525=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp7525->v=( void*) Cyc_Lexing_from_file(
f); _temp7525;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}