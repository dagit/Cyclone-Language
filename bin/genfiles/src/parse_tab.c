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
!=( unsigned int) 1){ Cyc_Parse_err(({ struct _tagged_string _temp879= s;
xprintf("bad kind: %.*s", _temp879.last_plus_one - _temp879.curr, _temp879.curr);}),
loc); return( void*) Cyc_Absyn_BoxKind;} else{ switch(({ struct _tagged_string
_temp880= s; char* _temp882= _temp880.curr + 0; if( _temp880.base == 0? 1:(
_temp882 < _temp880.base? 1: _temp882 >= _temp880.last_plus_one)){ _throw(
Null_Exception);}* _temp882;})){ case 'A': _LL883: return( void*) Cyc_Absyn_AnyKind;
case 'M': _LL884: return( void*) Cyc_Absyn_MemKind; case 'B': _LL885: return(
void*) Cyc_Absyn_BoxKind; case 'R': _LL886: return( void*) Cyc_Absyn_RgnKind;
case 'E': _LL887: return( void*) Cyc_Absyn_EffKind; default: _LL888: Cyc_Parse_err(({
struct _tagged_string _temp890= s; xprintf("bad kind: %.*s", _temp890.last_plus_one
- _temp890.curr, _temp890.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;}}}
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
_temp1454, _temp1454 + 1u}}; static short Cyc_yytranslate[ 348u]={ (short)0,
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
(short)89, (short)90, (short)91, (short)92}; static char _temp1457[ 2u]="$";
static char _temp1460[ 6u]="error"; static char _temp1463[ 12u]="$undefined.";
static char _temp1466[ 5u]="AUTO"; static char _temp1469[ 9u]="REGISTER"; static
char _temp1472[ 7u]="STATIC"; static char _temp1475[ 7u]="EXTERN"; static char
_temp1478[ 8u]="TYPEDEF"; static char _temp1481[ 5u]="VOID"; static char
_temp1484[ 5u]="CHAR"; static char _temp1487[ 6u]="SHORT"; static char _temp1490[
4u]="INT"; static char _temp1493[ 5u]="LONG"; static char _temp1496[ 6u]="FLOAT";
static char _temp1499[ 7u]="DOUBLE"; static char _temp1502[ 7u]="SIGNED"; static
char _temp1505[ 9u]="UNSIGNED"; static char _temp1508[ 6u]="CONST"; static char
_temp1511[ 9u]="VOLATILE"; static char _temp1514[ 9u]="RESTRICT"; static char
_temp1517[ 7u]="STRUCT"; static char _temp1520[ 6u]="UNION"; static char
_temp1523[ 5u]="CASE"; static char _temp1526[ 8u]="DEFAULT"; static char
_temp1529[ 7u]="INLINE"; static char _temp1532[ 3u]="IF"; static char _temp1535[
5u]="ELSE"; static char _temp1538[ 7u]="SWITCH"; static char _temp1541[ 6u]="WHILE";
static char _temp1544[ 3u]="DO"; static char _temp1547[ 4u]="FOR"; static char
_temp1550[ 5u]="GOTO"; static char _temp1553[ 9u]="CONTINUE"; static char
_temp1556[ 6u]="BREAK"; static char _temp1559[ 7u]="RETURN"; static char
_temp1562[ 7u]="SIZEOF"; static char _temp1565[ 5u]="ENUM"; static char
_temp1568[ 8u]="NULL_kw"; static char _temp1571[ 4u]="LET"; static char
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
_temp1637[ 7u]="PTR_OP"; static char _temp1640[ 7u]="INC_OP"; static char
_temp1643[ 7u]="DEC_OP"; static char _temp1646[ 8u]="LEFT_OP"; static char
_temp1649[ 9u]="RIGHT_OP"; static char _temp1652[ 6u]="LE_OP"; static char
_temp1655[ 6u]="GE_OP"; static char _temp1658[ 6u]="EQ_OP"; static char
_temp1661[ 6u]="NE_OP"; static char _temp1664[ 7u]="AND_OP"; static char
_temp1667[ 6u]="OR_OP"; static char _temp1670[ 11u]="MUL_ASSIGN"; static char
_temp1673[ 11u]="DIV_ASSIGN"; static char _temp1676[ 11u]="MOD_ASSIGN"; static
char _temp1679[ 11u]="ADD_ASSIGN"; static char _temp1682[ 11u]="SUB_ASSIGN";
static char _temp1685[ 12u]="LEFT_ASSIGN"; static char _temp1688[ 13u]="RIGHT_ASSIGN";
static char _temp1691[ 11u]="AND_ASSIGN"; static char _temp1694[ 11u]="XOR_ASSIGN";
static char _temp1697[ 10u]="OR_ASSIGN"; static char _temp1700[ 9u]="ELLIPSIS";
static char _temp1703[ 11u]="LEFT_RIGHT"; static char _temp1706[ 12u]="COLON_COLON";
static char _temp1709[ 11u]="IDENTIFIER"; static char _temp1712[ 17u]="INTEGER_CONSTANT";
static char _temp1715[ 7u]="STRING"; static char _temp1718[ 19u]="CHARACTER_CONSTANT";
static char _temp1721[ 18u]="FLOATING_CONSTANT"; static char _temp1724[ 9u]="TYPE_VAR";
static char _temp1727[ 16u]="QUAL_IDENTIFIER"; static char _temp1730[ 18u]="QUAL_TYPEDEF_NAME";
static char _temp1733[ 10u]="ATTRIBUTE"; static char _temp1736[ 4u]="';'";
static char _temp1739[ 4u]="'{'"; static char _temp1742[ 4u]="'}'"; static char
_temp1745[ 4u]="'='"; static char _temp1748[ 4u]="'('"; static char _temp1751[ 4u]="')'";
static char _temp1754[ 4u]="','"; static char _temp1757[ 4u]="'_'"; static char
_temp1760[ 4u]="'$'"; static char _temp1763[ 4u]="'<'"; static char _temp1766[ 4u]="'>'";
static char _temp1769[ 4u]="':'"; static char _temp1772[ 4u]="'.'"; static char
_temp1775[ 4u]="'['"; static char _temp1778[ 4u]="']'"; static char _temp1781[ 4u]="'*'";
static char _temp1784[ 4u]="'@'"; static char _temp1787[ 4u]="'?'"; static char
_temp1790[ 4u]="'+'"; static char _temp1793[ 4u]="'-'"; static char _temp1796[ 4u]="'&'";
static char _temp1799[ 4u]="'|'"; static char _temp1802[ 4u]="'^'"; static char
_temp1805[ 4u]="'/'"; static char _temp1808[ 4u]="'%'"; static char _temp1811[ 4u]="'~'";
static char _temp1814[ 4u]="'!'"; static char _temp1817[ 5u]="prog"; static char
_temp1820[ 17u]="translation_unit"; static char _temp1823[ 21u]="translation_unit_opt";
static char _temp1826[ 21u]="external_declaration"; static char _temp1829[ 20u]="function_definition";
static char _temp1832[ 21u]="function_definition2"; static char _temp1835[ 13u]="using_action";
static char _temp1838[ 15u]="unusing_action"; static char _temp1841[ 17u]="namespace_action";
static char _temp1844[ 19u]="unnamespace_action"; static char _temp1847[ 12u]="declaration";
static char _temp1850[ 17u]="declaration_list"; static char _temp1853[ 23u]="declaration_specifiers";
static char _temp1856[ 24u]="storage_class_specifier"; static char _temp1859[ 15u]="attributes_opt";
static char _temp1862[ 11u]="attributes"; static char _temp1865[ 15u]="attribute_list";
static char _temp1868[ 10u]="attribute"; static char _temp1871[ 15u]="type_specifier";
static char _temp1874[ 5u]="kind"; static char _temp1877[ 15u]="type_qualifier";
static char _temp1880[ 15u]="enum_specifier"; static char _temp1883[ 11u]="enum_field";
static char _temp1886[ 22u]="enum_declaration_list"; static char _temp1889[ 26u]="struct_or_union_specifier";
static char _temp1892[ 16u]="type_params_opt"; static char _temp1895[ 16u]="struct_or_union";
static char _temp1898[ 24u]="struct_declaration_list"; static char _temp1901[ 25u]="struct_declaration_list0";
static char _temp1904[ 21u]="init_declarator_list"; static char _temp1907[ 22u]="init_declarator_list0";
static char _temp1910[ 16u]="init_declarator"; static char _temp1913[ 19u]="struct_declaration";
static char _temp1916[ 25u]="specifier_qualifier_list"; static char _temp1919[
23u]="struct_declarator_list"; static char _temp1922[ 24u]="struct_declarator_list0";
static char _temp1925[ 18u]="struct_declarator"; static char _temp1928[ 17u]="tunion_specifier";
static char _temp1931[ 17u]="tunionfield_list"; static char _temp1934[ 12u]="tunionfield";
static char _temp1937[ 11u]="declarator"; static char _temp1940[ 18u]="direct_declarator";
static char _temp1943[ 8u]="pointer"; static char _temp1946[ 13u]="pointer_char";
static char _temp1949[ 8u]="rgn_opt"; static char _temp1952[ 4u]="rgn"; static
char _temp1955[ 20u]="type_qualifier_list"; static char _temp1958[ 20u]="parameter_type_list";
static char _temp1961[ 11u]="effect_set"; static char _temp1964[ 14u]="atomic_effect";
static char _temp1967[ 11u]="region_set"; static char _temp1970[ 15u]="parameter_list";
static char _temp1973[ 22u]="parameter_declaration"; static char _temp1976[ 16u]="identifier_list";
static char _temp1979[ 17u]="identifier_list0"; static char _temp1982[ 12u]="initializer";
static char _temp1985[ 18u]="array_initializer"; static char _temp1988[ 17u]="initializer_list";
static char _temp1991[ 12u]="designation"; static char _temp1994[ 16u]="designator_list";
static char _temp1997[ 11u]="designator"; static char _temp2000[ 10u]="type_name";
static char _temp2003[ 14u]="any_type_name"; static char _temp2006[ 15u]="type_name_list";
static char _temp2009[ 20u]="abstract_declarator"; static char _temp2012[ 27u]="direct_abstract_declarator";
static char _temp2015[ 10u]="statement"; static char _temp2018[ 18u]="labeled_statement";
static char _temp2021[ 21u]="expression_statement"; static char _temp2024[ 19u]="compound_statement";
static char _temp2027[ 16u]="block_item_list"; static char _temp2030[ 11u]="block_item";
static char _temp2033[ 20u]="selection_statement"; static char _temp2036[ 15u]="switch_clauses";
static char _temp2039[ 20u]="iteration_statement"; static char _temp2042[ 15u]="jump_statement";
static char _temp2045[ 8u]="pattern"; static char _temp2048[ 19u]="tuple_pattern_list";
static char _temp2051[ 20u]="tuple_pattern_list0"; static char _temp2054[ 14u]="field_pattern";
static char _temp2057[ 19u]="field_pattern_list"; static char _temp2060[ 20u]="field_pattern_list0";
static char _temp2063[ 11u]="expression"; static char _temp2066[ 22u]="assignment_expression";
static char _temp2069[ 20u]="assignment_operator"; static char _temp2072[ 23u]="conditional_expression";
static char _temp2075[ 20u]="constant_expression"; static char _temp2078[ 22u]="logical_or_expression";
static char _temp2081[ 23u]="logical_and_expression"; static char _temp2084[ 24u]="inclusive_or_expression";
static char _temp2087[ 24u]="exclusive_or_expression"; static char _temp2090[ 15u]="and_expression";
static char _temp2093[ 20u]="equality_expression"; static char _temp2096[ 22u]="relational_expression";
static char _temp2099[ 17u]="shift_expression"; static char _temp2102[ 20u]="additive_expression";
static char _temp2105[ 26u]="multiplicative_expression"; static char _temp2108[
16u]="cast_expression"; static char _temp2111[ 17u]="unary_expression"; static
char _temp2114[ 14u]="format_primop"; static char _temp2117[ 15u]="unary_operator";
static char _temp2120[ 19u]="postfix_expression"; static char _temp2123[ 19u]="primary_expression";
static char _temp2126[ 25u]="argument_expression_list"; static char _temp2129[
26u]="argument_expression_list0"; static char _temp2132[ 9u]="constant"; static
char _temp2135[ 20u]="qual_opt_identifier"; static struct _tagged_string Cyc_yytname[
227u]={( struct _tagged_string){ _temp1457, _temp1457, _temp1457 + 2u},( struct
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
struct _tagged_string){ _temp1637, _temp1637, _temp1637 + 7u},( struct
_tagged_string){ _temp1640, _temp1640, _temp1640 + 7u},( struct _tagged_string){
_temp1643, _temp1643, _temp1643 + 7u},( struct _tagged_string){ _temp1646,
_temp1646, _temp1646 + 8u},( struct _tagged_string){ _temp1649, _temp1649,
_temp1649 + 9u},( struct _tagged_string){ _temp1652, _temp1652, _temp1652 + 6u},(
struct _tagged_string){ _temp1655, _temp1655, _temp1655 + 6u},( struct
_tagged_string){ _temp1658, _temp1658, _temp1658 + 6u},( struct _tagged_string){
_temp1661, _temp1661, _temp1661 + 6u},( struct _tagged_string){ _temp1664,
_temp1664, _temp1664 + 7u},( struct _tagged_string){ _temp1667, _temp1667,
_temp1667 + 6u},( struct _tagged_string){ _temp1670, _temp1670, _temp1670 + 11u},(
struct _tagged_string){ _temp1673, _temp1673, _temp1673 + 11u},( struct
_tagged_string){ _temp1676, _temp1676, _temp1676 + 11u},( struct _tagged_string){
_temp1679, _temp1679, _temp1679 + 11u},( struct _tagged_string){ _temp1682,
_temp1682, _temp1682 + 11u},( struct _tagged_string){ _temp1685, _temp1685,
_temp1685 + 12u},( struct _tagged_string){ _temp1688, _temp1688, _temp1688 + 13u},(
struct _tagged_string){ _temp1691, _temp1691, _temp1691 + 11u},( struct
_tagged_string){ _temp1694, _temp1694, _temp1694 + 11u},( struct _tagged_string){
_temp1697, _temp1697, _temp1697 + 10u},( struct _tagged_string){ _temp1700,
_temp1700, _temp1700 + 9u},( struct _tagged_string){ _temp1703, _temp1703,
_temp1703 + 11u},( struct _tagged_string){ _temp1706, _temp1706, _temp1706 + 12u},(
struct _tagged_string){ _temp1709, _temp1709, _temp1709 + 11u},( struct
_tagged_string){ _temp1712, _temp1712, _temp1712 + 17u},( struct _tagged_string){
_temp1715, _temp1715, _temp1715 + 7u},( struct _tagged_string){ _temp1718,
_temp1718, _temp1718 + 19u},( struct _tagged_string){ _temp1721, _temp1721,
_temp1721 + 18u},( struct _tagged_string){ _temp1724, _temp1724, _temp1724 + 9u},(
struct _tagged_string){ _temp1727, _temp1727, _temp1727 + 16u},( struct
_tagged_string){ _temp1730, _temp1730, _temp1730 + 18u},( struct _tagged_string){
_temp1733, _temp1733, _temp1733 + 10u},( struct _tagged_string){ _temp1736,
_temp1736, _temp1736 + 4u},( struct _tagged_string){ _temp1739, _temp1739,
_temp1739 + 4u},( struct _tagged_string){ _temp1742, _temp1742, _temp1742 + 4u},(
struct _tagged_string){ _temp1745, _temp1745, _temp1745 + 4u},( struct
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
struct _tagged_string){ _temp1817, _temp1817, _temp1817 + 5u},( struct
_tagged_string){ _temp1820, _temp1820, _temp1820 + 17u},( struct _tagged_string){
_temp1823, _temp1823, _temp1823 + 21u},( struct _tagged_string){ _temp1826,
_temp1826, _temp1826 + 21u},( struct _tagged_string){ _temp1829, _temp1829,
_temp1829 + 20u},( struct _tagged_string){ _temp1832, _temp1832, _temp1832 + 21u},(
struct _tagged_string){ _temp1835, _temp1835, _temp1835 + 13u},( struct
_tagged_string){ _temp1838, _temp1838, _temp1838 + 15u},( struct _tagged_string){
_temp1841, _temp1841, _temp1841 + 17u},( struct _tagged_string){ _temp1844,
_temp1844, _temp1844 + 19u},( struct _tagged_string){ _temp1847, _temp1847,
_temp1847 + 12u},( struct _tagged_string){ _temp1850, _temp1850, _temp1850 + 17u},(
struct _tagged_string){ _temp1853, _temp1853, _temp1853 + 23u},( struct
_tagged_string){ _temp1856, _temp1856, _temp1856 + 24u},( struct _tagged_string){
_temp1859, _temp1859, _temp1859 + 15u},( struct _tagged_string){ _temp1862,
_temp1862, _temp1862 + 11u},( struct _tagged_string){ _temp1865, _temp1865,
_temp1865 + 15u},( struct _tagged_string){ _temp1868, _temp1868, _temp1868 + 10u},(
struct _tagged_string){ _temp1871, _temp1871, _temp1871 + 15u},( struct
_tagged_string){ _temp1874, _temp1874, _temp1874 + 5u},( struct _tagged_string){
_temp1877, _temp1877, _temp1877 + 15u},( struct _tagged_string){ _temp1880,
_temp1880, _temp1880 + 15u},( struct _tagged_string){ _temp1883, _temp1883,
_temp1883 + 11u},( struct _tagged_string){ _temp1886, _temp1886, _temp1886 + 22u},(
struct _tagged_string){ _temp1889, _temp1889, _temp1889 + 26u},( struct
_tagged_string){ _temp1892, _temp1892, _temp1892 + 16u},( struct _tagged_string){
_temp1895, _temp1895, _temp1895 + 16u},( struct _tagged_string){ _temp1898,
_temp1898, _temp1898 + 24u},( struct _tagged_string){ _temp1901, _temp1901,
_temp1901 + 25u},( struct _tagged_string){ _temp1904, _temp1904, _temp1904 + 21u},(
struct _tagged_string){ _temp1907, _temp1907, _temp1907 + 22u},( struct
_tagged_string){ _temp1910, _temp1910, _temp1910 + 16u},( struct _tagged_string){
_temp1913, _temp1913, _temp1913 + 19u},( struct _tagged_string){ _temp1916,
_temp1916, _temp1916 + 25u},( struct _tagged_string){ _temp1919, _temp1919,
_temp1919 + 23u},( struct _tagged_string){ _temp1922, _temp1922, _temp1922 + 24u},(
struct _tagged_string){ _temp1925, _temp1925, _temp1925 + 18u},( struct
_tagged_string){ _temp1928, _temp1928, _temp1928 + 17u},( struct _tagged_string){
_temp1931, _temp1931, _temp1931 + 17u},( struct _tagged_string){ _temp1934,
_temp1934, _temp1934 + 12u},( struct _tagged_string){ _temp1937, _temp1937,
_temp1937 + 11u},( struct _tagged_string){ _temp1940, _temp1940, _temp1940 + 18u},(
struct _tagged_string){ _temp1943, _temp1943, _temp1943 + 8u},( struct
_tagged_string){ _temp1946, _temp1946, _temp1946 + 13u},( struct _tagged_string){
_temp1949, _temp1949, _temp1949 + 8u},( struct _tagged_string){ _temp1952,
_temp1952, _temp1952 + 4u},( struct _tagged_string){ _temp1955, _temp1955,
_temp1955 + 20u},( struct _tagged_string){ _temp1958, _temp1958, _temp1958 + 20u},(
struct _tagged_string){ _temp1961, _temp1961, _temp1961 + 11u},( struct
_tagged_string){ _temp1964, _temp1964, _temp1964 + 14u},( struct _tagged_string){
_temp1967, _temp1967, _temp1967 + 11u},( struct _tagged_string){ _temp1970,
_temp1970, _temp1970 + 15u},( struct _tagged_string){ _temp1973, _temp1973,
_temp1973 + 22u},( struct _tagged_string){ _temp1976, _temp1976, _temp1976 + 16u},(
struct _tagged_string){ _temp1979, _temp1979, _temp1979 + 17u},( struct
_tagged_string){ _temp1982, _temp1982, _temp1982 + 12u},( struct _tagged_string){
_temp1985, _temp1985, _temp1985 + 18u},( struct _tagged_string){ _temp1988,
_temp1988, _temp1988 + 17u},( struct _tagged_string){ _temp1991, _temp1991,
_temp1991 + 12u},( struct _tagged_string){ _temp1994, _temp1994, _temp1994 + 16u},(
struct _tagged_string){ _temp1997, _temp1997, _temp1997 + 11u},( struct
_tagged_string){ _temp2000, _temp2000, _temp2000 + 10u},( struct _tagged_string){
_temp2003, _temp2003, _temp2003 + 14u},( struct _tagged_string){ _temp2006,
_temp2006, _temp2006 + 15u},( struct _tagged_string){ _temp2009, _temp2009,
_temp2009 + 20u},( struct _tagged_string){ _temp2012, _temp2012, _temp2012 + 27u},(
struct _tagged_string){ _temp2015, _temp2015, _temp2015 + 10u},( struct
_tagged_string){ _temp2018, _temp2018, _temp2018 + 18u},( struct _tagged_string){
_temp2021, _temp2021, _temp2021 + 21u},( struct _tagged_string){ _temp2024,
_temp2024, _temp2024 + 19u},( struct _tagged_string){ _temp2027, _temp2027,
_temp2027 + 16u},( struct _tagged_string){ _temp2030, _temp2030, _temp2030 + 11u},(
struct _tagged_string){ _temp2033, _temp2033, _temp2033 + 20u},( struct
_tagged_string){ _temp2036, _temp2036, _temp2036 + 15u},( struct _tagged_string){
_temp2039, _temp2039, _temp2039 + 20u},( struct _tagged_string){ _temp2042,
_temp2042, _temp2042 + 15u},( struct _tagged_string){ _temp2045, _temp2045,
_temp2045 + 8u},( struct _tagged_string){ _temp2048, _temp2048, _temp2048 + 19u},(
struct _tagged_string){ _temp2051, _temp2051, _temp2051 + 20u},( struct
_tagged_string){ _temp2054, _temp2054, _temp2054 + 14u},( struct _tagged_string){
_temp2057, _temp2057, _temp2057 + 19u},( struct _tagged_string){ _temp2060,
_temp2060, _temp2060 + 20u},( struct _tagged_string){ _temp2063, _temp2063,
_temp2063 + 11u},( struct _tagged_string){ _temp2066, _temp2066, _temp2066 + 22u},(
struct _tagged_string){ _temp2069, _temp2069, _temp2069 + 20u},( struct
_tagged_string){ _temp2072, _temp2072, _temp2072 + 23u},( struct _tagged_string){
_temp2075, _temp2075, _temp2075 + 20u},( struct _tagged_string){ _temp2078,
_temp2078, _temp2078 + 22u},( struct _tagged_string){ _temp2081, _temp2081,
_temp2081 + 23u},( struct _tagged_string){ _temp2084, _temp2084, _temp2084 + 24u},(
struct _tagged_string){ _temp2087, _temp2087, _temp2087 + 24u},( struct
_tagged_string){ _temp2090, _temp2090, _temp2090 + 15u},( struct _tagged_string){
_temp2093, _temp2093, _temp2093 + 20u},( struct _tagged_string){ _temp2096,
_temp2096, _temp2096 + 22u},( struct _tagged_string){ _temp2099, _temp2099,
_temp2099 + 17u},( struct _tagged_string){ _temp2102, _temp2102, _temp2102 + 20u},(
struct _tagged_string){ _temp2105, _temp2105, _temp2105 + 26u},( struct
_tagged_string){ _temp2108, _temp2108, _temp2108 + 16u},( struct _tagged_string){
_temp2111, _temp2111, _temp2111 + 17u},( struct _tagged_string){ _temp2114,
_temp2114, _temp2114 + 14u},( struct _tagged_string){ _temp2117, _temp2117,
_temp2117 + 15u},( struct _tagged_string){ _temp2120, _temp2120, _temp2120 + 19u},(
struct _tagged_string){ _temp2123, _temp2123, _temp2123 + 19u},( struct
_tagged_string){ _temp2126, _temp2126, _temp2126 + 25u},( struct _tagged_string){
_temp2129, _temp2129, _temp2129 + 26u},( struct _tagged_string){ _temp2132,
_temp2132, _temp2132 + 9u},( struct _tagged_string){ _temp2135, _temp2135,
_temp2135 + 20u}}; static short Cyc_yyr1[ 389u]={ (short)0, (short)120, (short)121,
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
short* _temp2138=( short*)({ unsigned int _temp2136= 200u; short* _temp2137=(
short*) GC_malloc_atomic( sizeof( short) * _temp2136); unsigned int i; for( i= 0;
i < _temp2136; i ++){ _temp2137[ i]= (short)0;} _temp2137;}); struct
_tagged_ptr1 _temp2139; _temp2139.curr= _temp2138; _temp2139.base= _temp2138;
_temp2139.last_plus_one= _temp2138 + 200; _temp2139;}); int yyvsp_offset; struct
_tagged_ptr2 yyvs=( struct _tagged_ptr2)({ struct _xtunion_struct** _temp2142=(
struct _xtunion_struct**)({ unsigned int _temp2140= 200u; struct _xtunion_struct**
_temp2141=( struct _xtunion_struct**) GC_malloc( sizeof( struct _xtunion_struct*)
* _temp2140); unsigned int i; for( i= 0; i < _temp2140; i ++){ _temp2141[ i]=
Cyc_yylval;} _temp2141;}); struct _tagged_ptr2 _temp2143; _temp2143.curr=
_temp2142; _temp2143.base= _temp2142; _temp2143.last_plus_one= _temp2142 + 200;
_temp2143;}); int yylsp_offset; struct _tagged_ptr3 yyls=( struct _tagged_ptr3)({
struct Cyc_Yyltype* _temp2146=( struct Cyc_Yyltype*)({ unsigned int _temp2144=
200u; struct Cyc_Yyltype* _temp2145=( struct Cyc_Yyltype*) GC_malloc( sizeof(
struct Cyc_Yyltype) * _temp2144); unsigned int i; for( i= 0; i < _temp2144; i ++){
_temp2145[ i]= Cyc_yynewloc();} _temp2145;}); struct _tagged_ptr3 _temp2147;
_temp2147.curr= _temp2146; _temp2147.base= _temp2146; _temp2147.last_plus_one=
_temp2146 + 200; _temp2147;}); int yystacksize= 200; struct _xtunion_struct*
yyval= Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar=
- 2; yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate:({ struct
_tagged_ptr1 _temp2148= yyss; short* _temp2150= _temp2148.curr +( ++
yyssp_offset); if( _temp2148.base == 0? 1:( _temp2150 < _temp2148.base? 1:
_temp2150 >= _temp2148.last_plus_one)){ _throw( Null_Exception);}* _temp2150=(
short) yystate;}); if( yyssp_offset >= yystacksize - 1){ if( yystacksize >=
10000){ Cyc_yyerror(( struct _tagged_string)({ char* _temp2151=( char*)"parser stack overflow";
struct _tagged_string _temp2152; _temp2152.curr= _temp2151; _temp2152.base=
_temp2151; _temp2152.last_plus_one= _temp2151 + 22; _temp2152;}));( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Yystack_overflow_struct* _temp2153=(
struct Cyc_Yystack_overflow_struct*) GC_malloc( sizeof( struct Cyc_Yystack_overflow_struct));
_temp2153[ 0]=({ struct Cyc_Yystack_overflow_struct _temp2154; _temp2154.tag=
Cyc_Yystack_overflow_tag; _temp2154;}); _temp2153;}));} yystacksize *= 2; if(
yystacksize > 10000){ yystacksize= 10000;}{ struct _tagged_ptr1 yyss1=({
unsigned int _temp2155=( unsigned int) yystacksize; short* _temp2156=( short*)
GC_malloc_atomic( sizeof( short) * _temp2155); unsigned int i; struct
_tagged_ptr1 _temp2157={ _temp2156, _temp2156, _temp2156 + _temp2155}; for( i= 0;
i < _temp2155; i ++){ _temp2156[ i]= (short)0;} _temp2157;}); struct
_tagged_ptr2 yyvs1=({ unsigned int _temp2158=( unsigned int) yystacksize; struct
_xtunion_struct** _temp2159=( struct _xtunion_struct**) GC_malloc( sizeof(
struct _xtunion_struct*) * _temp2158); unsigned int i; struct _tagged_ptr2
_temp2160={ _temp2159, _temp2159, _temp2159 + _temp2158}; for( i= 0; i <
_temp2158; i ++){ _temp2159[ i]= Cyc_yylval;} _temp2160;}); struct _tagged_ptr3
yyls1=({ unsigned int _temp2161=( unsigned int) yystacksize; struct Cyc_Yyltype*
_temp2162=( struct Cyc_Yyltype*) GC_malloc( sizeof( struct Cyc_Yyltype) *
_temp2161); unsigned int i; struct _tagged_ptr3 _temp2163={ _temp2162, _temp2162,
_temp2162 + _temp2161}; for( i= 0; i < _temp2161; i ++){ _temp2162[ i]= Cyc_yynewloc();}
_temp2163;});{ int i= 0; for( 0; i <= yyssp_offset; i ++){({ struct _tagged_ptr1
_temp2164= yyss1; short* _temp2166= _temp2164.curr + i; if( _temp2164.base == 0?
1:( _temp2166 < _temp2164.base? 1: _temp2166 >= _temp2164.last_plus_one)){
_throw( Null_Exception);}* _temp2166=({ struct _tagged_ptr1 _temp2167= yyss;
short* _temp2169= _temp2167.curr + i; if( _temp2167.base == 0? 1:( _temp2169 <
_temp2167.base? 1: _temp2169 >= _temp2167.last_plus_one)){ _throw(
Null_Exception);}* _temp2169;});});({ struct _tagged_ptr2 _temp2170= yyvs1;
struct _xtunion_struct** _temp2172= _temp2170.curr + i; if( _temp2170.base == 0?
1:( _temp2172 < _temp2170.base? 1: _temp2172 >= _temp2170.last_plus_one)){
_throw( Null_Exception);}* _temp2172=({ struct _tagged_ptr2 _temp2173= yyvs;
struct _xtunion_struct** _temp2175= _temp2173.curr + i; if( _temp2173.base == 0?
1:( _temp2175 < _temp2173.base? 1: _temp2175 >= _temp2173.last_plus_one)){
_throw( Null_Exception);}* _temp2175;});});({ struct _tagged_ptr3 _temp2176=
yyls1; struct Cyc_Yyltype* _temp2178= _temp2176.curr + i; if( _temp2176.base ==
0? 1:( _temp2178 < _temp2176.base? 1: _temp2178 >= _temp2176.last_plus_one)){
_throw( Null_Exception);}* _temp2178=({ struct _tagged_ptr3 _temp2179= yyls;
struct Cyc_Yyltype* _temp2181= _temp2179.curr + i; if( _temp2179.base == 0? 1:(
_temp2181 < _temp2179.base? 1: _temp2181 >= _temp2179.last_plus_one)){ _throw(
Null_Exception);}* _temp2181;});});}} yyss= yyss1; yyvs= yyvs1; yyls= yyls1;}}
goto yybackup; yybackup: yyn=( int)({ short* _temp2182=( short*) Cyc_yypact;
unsigned int _temp2183= yystate; if( _temp2183 >= 729u){ _throw( Null_Exception);}
_temp2182[ _temp2183];}); if( yyn == - 32768){ goto yydefault;} if( Cyc_yychar
== - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 0){ yychar1= 0; Cyc_yychar=
0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <= 347: 0)?( int)({ short*
_temp2184=( short*) Cyc_yytranslate; unsigned int _temp2185= Cyc_yychar; if(
_temp2185 >= 348u){ _throw( Null_Exception);} _temp2184[ _temp2185];}): 227;}
yyn += yychar1; if(( yyn < 0? 1: yyn > 4017)? 1:( int)({ short* _temp2186=(
short*) Cyc_yycheck; unsigned int _temp2187= yyn; if( _temp2187 >= 4018u){
_throw( Null_Exception);} _temp2186[ _temp2187];}) != yychar1){ goto yydefault;}
yyn=( int)({ short* _temp2188=( short*) Cyc_yytable; unsigned int _temp2189= yyn;
if( _temp2189 >= 4018u){ _throw( Null_Exception);} _temp2188[ _temp2189];}); if(
yyn < 0){ if( yyn == - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{
if( yyn == 0){ goto yyerrlab;}} if( yyn == 728){ return 0;} if( Cyc_yychar != 0){
Cyc_yychar= - 2;}({ struct _tagged_ptr2 _temp2190= yyvs; struct _xtunion_struct**
_temp2192= _temp2190.curr +( ++ yyvsp_offset); if( _temp2190.base == 0? 1:(
_temp2192 < _temp2190.base? 1: _temp2192 >= _temp2190.last_plus_one)){ _throw(
Null_Exception);}* _temp2192= Cyc_yylval;});({ struct _tagged_ptr3 _temp2193=
yyls; struct Cyc_Yyltype* _temp2195= _temp2193.curr +( ++ yylsp_offset); if(
_temp2193.base == 0? 1:( _temp2195 < _temp2193.base? 1: _temp2195 >= _temp2193.last_plus_one)){
_throw( Null_Exception);}* _temp2195= Cyc_yylloc;}); if( yyerrstatus != 0){
yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int)({ short*
_temp2196=( short*) Cyc_yydefact; unsigned int _temp2197= yystate; if( _temp2197
>= 729u){ _throw( Null_Exception);} _temp2196[ _temp2197];}); if( yyn == 0){
goto yyerrlab;} yyreduce: yylen=( int)({ short* _temp2198=( short*) Cyc_yyr2;
unsigned int _temp2199= yyn; if( _temp2199 >= 389u){ _throw( Null_Exception);}
_temp2198[ _temp2199];}); if( yylen > 0){ yyval=({ struct _tagged_ptr2 _temp2200=
yyvs; struct _xtunion_struct** _temp2202= _temp2200.curr +(( yyvsp_offset + 1) -
yylen); if( _temp2200.base == 0? 1:( _temp2202 < _temp2200.base? 1: _temp2202 >=
_temp2200.last_plus_one)){ _throw( Null_Exception);}* _temp2202;});} switch( yyn){
case 1: _LL2203: yyval=({ struct _tagged_ptr2 _temp2205= yyvs; struct
_xtunion_struct** _temp2207= _temp2205.curr + yyvsp_offset; if( _temp2205.base
== 0? 1:( _temp2207 < _temp2205.base? 1: _temp2207 >= _temp2205.last_plus_one)){
_throw( Null_Exception);}* _temp2207;}); Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2208= yyvs; struct _xtunion_struct** _temp2210=
_temp2208.curr + yyvsp_offset; if( _temp2208.base == 0? 1:( _temp2210 <
_temp2208.base? 1: _temp2210 >= _temp2208.last_plus_one)){ _throw(
Null_Exception);}* _temp2210;})); break; case 2: _LL2204: yyval=({ struct
_tagged_ptr2 _temp2212= yyvs; struct _xtunion_struct** _temp2214= _temp2212.curr
+ yyvsp_offset; if( _temp2212.base == 0? 1:( _temp2214 < _temp2212.base? 1:
_temp2214 >= _temp2212.last_plus_one)){ _throw( Null_Exception);}* _temp2214;});
break; case 3: _LL2211: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2216=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2216[ 0]=({ struct Cyc_DeclList_tok_struct _temp2217; _temp2217.tag= Cyc_DeclList_tok_tag;
_temp2217.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2218= yyvs; struct _xtunion_struct** _temp2220= _temp2218.curr +(
yyvsp_offset - 1); if( _temp2218.base == 0? 1:( _temp2220 < _temp2218.base? 1:
_temp2220 >= _temp2218.last_plus_one)){ _throw( Null_Exception);}* _temp2220;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2221= yyvs; struct
_xtunion_struct** _temp2223= _temp2221.curr + yyvsp_offset; if( _temp2221.base
== 0? 1:( _temp2223 < _temp2221.base? 1: _temp2223 >= _temp2221.last_plus_one)){
_throw( Null_Exception);}* _temp2223;}))); _temp2217;}); _temp2216;}); break;
case 4: _LL2215: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2225=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2225[ 0]=({ struct Cyc_DeclList_tok_struct _temp2226; _temp2226.tag= Cyc_DeclList_tok_tag;
_temp2226.f1=({ struct Cyc_List_List* _temp2227=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2227->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2228=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2228->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2235=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2235[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2236; _temp2236.tag= Cyc_Absyn_Using_d_tag;
_temp2236.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2237= yyvs;
struct _xtunion_struct** _temp2239= _temp2237.curr +( yyvsp_offset - 2); if(
_temp2237.base == 0? 1:( _temp2239 < _temp2237.base? 1: _temp2239 >= _temp2237.last_plus_one)){
_throw( Null_Exception);}* _temp2239;})); _temp2236.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2240= yyvs; struct _xtunion_struct** _temp2242=
_temp2240.curr + yyvsp_offset; if( _temp2240.base == 0? 1:( _temp2242 <
_temp2240.base? 1: _temp2242 >= _temp2240.last_plus_one)){ _throw(
Null_Exception);}* _temp2242;})); _temp2236;}); _temp2235;})); _temp2228->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2229= yyls; struct Cyc_Yyltype*
_temp2231= _temp2229.curr +( yylsp_offset - 2); if( _temp2229.base == 0? 1:(
_temp2231 < _temp2229.base? 1: _temp2231 >= _temp2229.last_plus_one)){ _throw(
Null_Exception);}* _temp2231;}).first_line,({ struct _tagged_ptr3 _temp2232=
yyls; struct Cyc_Yyltype* _temp2234= _temp2232.curr + yylsp_offset; if(
_temp2232.base == 0? 1:( _temp2234 < _temp2232.base? 1: _temp2234 >= _temp2232.last_plus_one)){
_throw( Null_Exception);}* _temp2234;}).last_line); _temp2228;}); _temp2227->tl=
0; _temp2227;}); _temp2226;}); _temp2225;}); Cyc_Lex_leave_using(); break; case
5: _LL2224: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2244=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2244[ 0]=({ struct Cyc_DeclList_tok_struct _temp2245; _temp2245.tag= Cyc_DeclList_tok_tag;
_temp2245.f1=({ struct Cyc_List_List* _temp2246=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2246->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2250=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2250->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2257=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2257[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2258; _temp2258.tag= Cyc_Absyn_Using_d_tag;
_temp2258.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2259= yyvs;
struct _xtunion_struct** _temp2261= _temp2259.curr +( yyvsp_offset - 4); if(
_temp2259.base == 0? 1:( _temp2261 < _temp2259.base? 1: _temp2261 >= _temp2259.last_plus_one)){
_throw( Null_Exception);}* _temp2261;})); _temp2258.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2262= yyvs; struct _xtunion_struct** _temp2264=
_temp2262.curr +( yyvsp_offset - 2); if( _temp2262.base == 0? 1:( _temp2264 <
_temp2262.base? 1: _temp2264 >= _temp2262.last_plus_one)){ _throw(
Null_Exception);}* _temp2264;})); _temp2258;}); _temp2257;})); _temp2250->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2251= yyls; struct Cyc_Yyltype*
_temp2253= _temp2251.curr +( yylsp_offset - 4); if( _temp2251.base == 0? 1:(
_temp2253 < _temp2251.base? 1: _temp2253 >= _temp2251.last_plus_one)){ _throw(
Null_Exception);}* _temp2253;}).first_line,({ struct _tagged_ptr3 _temp2254=
yyls; struct Cyc_Yyltype* _temp2256= _temp2254.curr +( yylsp_offset - 1); if(
_temp2254.base == 0? 1:( _temp2256 < _temp2254.base? 1: _temp2256 >= _temp2254.last_plus_one)){
_throw( Null_Exception);}* _temp2256;}).last_line); _temp2250;}); _temp2246->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2247= yyvs; struct
_xtunion_struct** _temp2249= _temp2247.curr + yyvsp_offset; if( _temp2247.base
== 0? 1:( _temp2249 < _temp2247.base? 1: _temp2249 >= _temp2247.last_plus_one)){
_throw( Null_Exception);}* _temp2249;})); _temp2246;}); _temp2245;}); _temp2244;});
break; case 6: _LL2243: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2266=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2266[ 0]=({ struct Cyc_DeclList_tok_struct _temp2267; _temp2267.tag= Cyc_DeclList_tok_tag;
_temp2267.f1=({ struct Cyc_List_List* _temp2268=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2268->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2269=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2269->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2276=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2276[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2277; _temp2277.tag=
Cyc_Absyn_Namespace_d_tag; _temp2277.f1=({ struct _tagged_string* _temp2278=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp2278[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2279= yyvs;
struct _xtunion_struct** _temp2281= _temp2279.curr +( yyvsp_offset - 2); if(
_temp2279.base == 0? 1:( _temp2281 < _temp2279.base? 1: _temp2281 >= _temp2279.last_plus_one)){
_throw( Null_Exception);}* _temp2281;})); _temp2278;}); _temp2277.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2282= yyvs; struct _xtunion_struct** _temp2284=
_temp2282.curr + yyvsp_offset; if( _temp2282.base == 0? 1:( _temp2284 <
_temp2282.base? 1: _temp2284 >= _temp2282.last_plus_one)){ _throw(
Null_Exception);}* _temp2284;})); _temp2277;}); _temp2276;})); _temp2269->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2270= yyls; struct Cyc_Yyltype*
_temp2272= _temp2270.curr +( yylsp_offset - 2); if( _temp2270.base == 0? 1:(
_temp2272 < _temp2270.base? 1: _temp2272 >= _temp2270.last_plus_one)){ _throw(
Null_Exception);}* _temp2272;}).first_line,({ struct _tagged_ptr3 _temp2273=
yyls; struct Cyc_Yyltype* _temp2275= _temp2273.curr + yylsp_offset; if(
_temp2273.base == 0? 1:( _temp2275 < _temp2273.base? 1: _temp2275 >= _temp2273.last_plus_one)){
_throw( Null_Exception);}* _temp2275;}).last_line); _temp2269;}); _temp2268->tl=
0; _temp2268;}); _temp2267;}); _temp2266;}); Cyc_Lex_leave_namespace(); break;
case 7: _LL2265: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2286=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2286[ 0]=({ struct Cyc_DeclList_tok_struct _temp2287; _temp2287.tag= Cyc_DeclList_tok_tag;
_temp2287.f1=({ struct Cyc_List_List* _temp2288=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2288->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2292=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2292->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2299=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2299[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2300; _temp2300.tag=
Cyc_Absyn_Namespace_d_tag; _temp2300.f1=({ struct _tagged_string* _temp2301=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp2301[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2302= yyvs;
struct _xtunion_struct** _temp2304= _temp2302.curr +( yyvsp_offset - 4); if(
_temp2302.base == 0? 1:( _temp2304 < _temp2302.base? 1: _temp2304 >= _temp2302.last_plus_one)){
_throw( Null_Exception);}* _temp2304;})); _temp2301;}); _temp2300.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2305= yyvs; struct _xtunion_struct** _temp2307=
_temp2305.curr +( yyvsp_offset - 2); if( _temp2305.base == 0? 1:( _temp2307 <
_temp2305.base? 1: _temp2307 >= _temp2305.last_plus_one)){ _throw(
Null_Exception);}* _temp2307;})); _temp2300;}); _temp2299;})); _temp2292->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2293= yyls; struct Cyc_Yyltype*
_temp2295= _temp2293.curr +( yylsp_offset - 4); if( _temp2293.base == 0? 1:(
_temp2295 < _temp2293.base? 1: _temp2295 >= _temp2293.last_plus_one)){ _throw(
Null_Exception);}* _temp2295;}).first_line,({ struct _tagged_ptr3 _temp2296=
yyls; struct Cyc_Yyltype* _temp2298= _temp2296.curr +( yylsp_offset - 1); if(
_temp2296.base == 0? 1:( _temp2298 < _temp2296.base? 1: _temp2298 >= _temp2296.last_plus_one)){
_throw( Null_Exception);}* _temp2298;}).last_line); _temp2292;}); _temp2288->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2289= yyvs; struct
_xtunion_struct** _temp2291= _temp2289.curr + yyvsp_offset; if( _temp2289.base
== 0? 1:( _temp2291 < _temp2289.base? 1: _temp2291 >= _temp2289.last_plus_one)){
_throw( Null_Exception);}* _temp2291;})); _temp2288;}); _temp2287;}); _temp2286;});
break; case 8: _LL2285: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2309= yyvs; struct _xtunion_struct** _temp2311= _temp2309.curr
+( yyvsp_offset - 4); if( _temp2309.base == 0? 1:( _temp2311 < _temp2309.base? 1:
_temp2311 >= _temp2309.last_plus_one)){ _throw( Null_Exception);}* _temp2311;})),(
struct _tagged_string)({ char* _temp2312=( char*)"C"; struct _tagged_string
_temp2313; _temp2313.curr= _temp2312; _temp2313.base= _temp2312; _temp2313.last_plus_one=
_temp2312 + 2; _temp2313;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp2314=( char*)"only extern \"C\" { ... } is allowed"; struct
_tagged_string _temp2315; _temp2315.curr= _temp2314; _temp2315.base= _temp2314;
_temp2315.last_plus_one= _temp2314 + 35; _temp2315;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2316= yyls; struct Cyc_Yyltype* _temp2318= _temp2316.curr
+( yylsp_offset - 5); if( _temp2316.base == 0? 1:( _temp2318 < _temp2316.base? 1:
_temp2318 >= _temp2316.last_plus_one)){ _throw( Null_Exception);}* _temp2318;}).first_line,({
struct _tagged_ptr3 _temp2319= yyls; struct Cyc_Yyltype* _temp2321= _temp2319.curr
+( yylsp_offset - 4); if( _temp2319.base == 0? 1:( _temp2321 < _temp2319.base? 1:
_temp2321 >= _temp2319.last_plus_one)){ _throw( Null_Exception);}* _temp2321;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2322=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2322[ 0]=({ struct Cyc_DeclList_tok_struct _temp2323; _temp2323.tag= Cyc_DeclList_tok_tag;
_temp2323.f1=({ struct Cyc_List_List* _temp2324=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2324->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2328=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2328->r=( void*)(( void*)({ struct Cyc_Absyn_ExternC_d_struct* _temp2335=(
struct Cyc_Absyn_ExternC_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct));
_temp2335[ 0]=({ struct Cyc_Absyn_ExternC_d_struct _temp2336; _temp2336.tag= Cyc_Absyn_ExternC_d_tag;
_temp2336.f1= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2337= yyvs;
struct _xtunion_struct** _temp2339= _temp2337.curr +( yyvsp_offset - 2); if(
_temp2337.base == 0? 1:( _temp2339 < _temp2337.base? 1: _temp2339 >= _temp2337.last_plus_one)){
_throw( Null_Exception);}* _temp2339;})); _temp2336;}); _temp2335;})); _temp2328->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2329= yyls; struct Cyc_Yyltype*
_temp2331= _temp2329.curr +( yylsp_offset - 5); if( _temp2329.base == 0? 1:(
_temp2331 < _temp2329.base? 1: _temp2331 >= _temp2329.last_plus_one)){ _throw(
Null_Exception);}* _temp2331;}).first_line,({ struct _tagged_ptr3 _temp2332=
yyls; struct Cyc_Yyltype* _temp2334= _temp2332.curr +( yylsp_offset - 1); if(
_temp2332.base == 0? 1:( _temp2334 < _temp2332.base? 1: _temp2334 >= _temp2332.last_plus_one)){
_throw( Null_Exception);}* _temp2334;}).last_line); _temp2328;}); _temp2324->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2325= yyvs; struct
_xtunion_struct** _temp2327= _temp2325.curr + yyvsp_offset; if( _temp2325.base
== 0? 1:( _temp2327 < _temp2325.base? 1: _temp2327 >= _temp2325.last_plus_one)){
_throw( Null_Exception);}* _temp2327;})); _temp2324;}); _temp2323;}); _temp2322;});
break; case 9: _LL2308: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2341=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2341[ 0]=({ struct Cyc_DeclList_tok_struct _temp2342; _temp2342.tag= Cyc_DeclList_tok_tag;
_temp2342.f1= 0; _temp2342;}); _temp2341;}); break; case 10: _LL2340: yyval=({
struct _tagged_ptr2 _temp2344= yyvs; struct _xtunion_struct** _temp2346=
_temp2344.curr + yyvsp_offset; if( _temp2344.base == 0? 1:( _temp2346 <
_temp2344.base? 1: _temp2346 >= _temp2344.last_plus_one)){ _throw(
Null_Exception);}* _temp2346;}); break; case 11: _LL2343: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2348=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2348[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2349; _temp2349.tag= Cyc_DeclList_tok_tag; _temp2349.f1=({ struct Cyc_List_List*
_temp2350=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2350->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp2351=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2351[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2352; _temp2352.tag= Cyc_Absyn_Fn_d_tag;
_temp2352.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp2353= yyvs;
struct _xtunion_struct** _temp2355= _temp2353.curr + yyvsp_offset; if( _temp2353.base
== 0? 1:( _temp2355 < _temp2353.base? 1: _temp2355 >= _temp2353.last_plus_one)){
_throw( Null_Exception);}* _temp2355;})); _temp2352;}); _temp2351;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2356= yyls; struct Cyc_Yyltype* _temp2358= _temp2356.curr
+ yylsp_offset; if( _temp2356.base == 0? 1:( _temp2358 < _temp2356.base? 1:
_temp2358 >= _temp2356.last_plus_one)){ _throw( Null_Exception);}* _temp2358;}).first_line,({
struct _tagged_ptr3 _temp2359= yyls; struct Cyc_Yyltype* _temp2361= _temp2359.curr
+ yylsp_offset; if( _temp2359.base == 0? 1:( _temp2361 < _temp2359.base? 1:
_temp2361 >= _temp2359.last_plus_one)){ _throw( Null_Exception);}* _temp2361;}).last_line));
_temp2350->tl= 0; _temp2350;}); _temp2349;}); _temp2348;}); break; case 12:
_LL2347: yyval=({ struct _tagged_ptr2 _temp2363= yyvs; struct _xtunion_struct**
_temp2365= _temp2363.curr + yyvsp_offset; if( _temp2363.base == 0? 1:( _temp2365
< _temp2363.base? 1: _temp2365 >= _temp2363.last_plus_one)){ _throw(
Null_Exception);}* _temp2365;}); break; case 13: _LL2362: yyval=( struct
_xtunion_struct*)({ struct Cyc_FnDecl_tok_struct* _temp2367=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2367[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2368; _temp2368.tag= Cyc_FnDecl_tok_tag; _temp2368.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp2369= yyvs; struct
_xtunion_struct** _temp2371= _temp2369.curr +( yyvsp_offset - 1); if( _temp2369.base
== 0? 1:( _temp2371 < _temp2369.base? 1: _temp2371 >= _temp2369.last_plus_one)){
_throw( Null_Exception);}* _temp2371;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2372= yyvs; struct _xtunion_struct** _temp2374= _temp2372.curr
+ yyvsp_offset; if( _temp2372.base == 0? 1:( _temp2374 < _temp2372.base? 1:
_temp2374 >= _temp2372.last_plus_one)){ _throw( Null_Exception);}* _temp2374;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2375= yyls; struct Cyc_Yyltype*
_temp2377= _temp2375.curr +( yylsp_offset - 1); if( _temp2375.base == 0? 1:(
_temp2377 < _temp2375.base? 1: _temp2377 >= _temp2375.last_plus_one)){ _throw(
Null_Exception);}* _temp2377;}).first_line,({ struct _tagged_ptr3 _temp2378=
yyls; struct Cyc_Yyltype* _temp2380= _temp2378.curr + yylsp_offset; if(
_temp2378.base == 0? 1:( _temp2380 < _temp2378.base? 1: _temp2380 >= _temp2378.last_plus_one)){
_throw( Null_Exception);}* _temp2380;}).last_line)); _temp2368;}); _temp2367;});
break; case 14: _LL2366: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2382=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2382[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2383; _temp2383.tag= Cyc_FnDecl_tok_tag;
_temp2383.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2384=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2384->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2385= yyvs; struct
_xtunion_struct** _temp2387= _temp2385.curr +( yyvsp_offset - 2); if( _temp2385.base
== 0? 1:( _temp2387 < _temp2385.base? 1: _temp2387 >= _temp2385.last_plus_one)){
_throw( Null_Exception);}* _temp2387;})); _temp2384;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2388= yyvs; struct _xtunion_struct** _temp2390=
_temp2388.curr +( yyvsp_offset - 1); if( _temp2388.base == 0? 1:( _temp2390 <
_temp2388.base? 1: _temp2390 >= _temp2388.last_plus_one)){ _throw(
Null_Exception);}* _temp2390;})), 0, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2391= yyvs; struct _xtunion_struct** _temp2393= _temp2391.curr +
yyvsp_offset; if( _temp2391.base == 0? 1:( _temp2393 < _temp2391.base? 1:
_temp2393 >= _temp2391.last_plus_one)){ _throw( Null_Exception);}* _temp2393;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2394= yyls; struct Cyc_Yyltype*
_temp2396= _temp2394.curr +( yylsp_offset - 2); if( _temp2394.base == 0? 1:(
_temp2396 < _temp2394.base? 1: _temp2396 >= _temp2394.last_plus_one)){ _throw(
Null_Exception);}* _temp2396;}).first_line,({ struct _tagged_ptr3 _temp2397=
yyls; struct Cyc_Yyltype* _temp2399= _temp2397.curr + yylsp_offset; if(
_temp2397.base == 0? 1:( _temp2399 < _temp2397.base? 1: _temp2399 >= _temp2397.last_plus_one)){
_throw( Null_Exception);}* _temp2399;}).last_line)); _temp2383;}); _temp2382;});
break; case 15: _LL2381: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2401=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2401[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2402; _temp2402.tag= Cyc_FnDecl_tok_tag;
_temp2402.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp2403= yyvs; struct _xtunion_struct** _temp2405= _temp2403.curr
+( yyvsp_offset - 2); if( _temp2403.base == 0? 1:( _temp2405 < _temp2403.base? 1:
_temp2405 >= _temp2403.last_plus_one)){ _throw( Null_Exception);}* _temp2405;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2406= yyvs; struct
_xtunion_struct** _temp2408= _temp2406.curr +( yyvsp_offset - 1); if( _temp2406.base
== 0? 1:( _temp2408 < _temp2406.base? 1: _temp2408 >= _temp2406.last_plus_one)){
_throw( Null_Exception);}* _temp2408;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2409= yyvs; struct _xtunion_struct** _temp2411= _temp2409.curr
+ yyvsp_offset; if( _temp2409.base == 0? 1:( _temp2411 < _temp2409.base? 1:
_temp2411 >= _temp2409.last_plus_one)){ _throw( Null_Exception);}* _temp2411;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2412= yyls; struct Cyc_Yyltype*
_temp2414= _temp2412.curr +( yylsp_offset - 2); if( _temp2412.base == 0? 1:(
_temp2414 < _temp2412.base? 1: _temp2414 >= _temp2412.last_plus_one)){ _throw(
Null_Exception);}* _temp2414;}).first_line,({ struct _tagged_ptr3 _temp2415=
yyls; struct Cyc_Yyltype* _temp2417= _temp2415.curr + yylsp_offset; if(
_temp2415.base == 0? 1:( _temp2417 < _temp2415.base? 1: _temp2417 >= _temp2415.last_plus_one)){
_throw( Null_Exception);}* _temp2417;}).last_line)); _temp2402;}); _temp2401;});
break; case 16: _LL2400: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2419=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2419[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2420; _temp2420.tag= Cyc_FnDecl_tok_tag;
_temp2420.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2421=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2421->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2422= yyvs; struct
_xtunion_struct** _temp2424= _temp2422.curr +( yyvsp_offset - 3); if( _temp2422.base
== 0? 1:( _temp2424 < _temp2422.base? 1: _temp2424 >= _temp2422.last_plus_one)){
_throw( Null_Exception);}* _temp2424;})); _temp2421;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2425= yyvs; struct _xtunion_struct** _temp2427=
_temp2425.curr +( yyvsp_offset - 2); if( _temp2425.base == 0? 1:( _temp2427 <
_temp2425.base? 1: _temp2427 >= _temp2425.last_plus_one)){ _throw(
Null_Exception);}* _temp2427;})), Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2428= yyvs; struct _xtunion_struct** _temp2430= _temp2428.curr +(
yyvsp_offset - 1); if( _temp2428.base == 0? 1:( _temp2430 < _temp2428.base? 1:
_temp2430 >= _temp2428.last_plus_one)){ _throw( Null_Exception);}* _temp2430;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2431= yyvs; struct
_xtunion_struct** _temp2433= _temp2431.curr + yyvsp_offset; if( _temp2431.base
== 0? 1:( _temp2433 < _temp2431.base? 1: _temp2433 >= _temp2431.last_plus_one)){
_throw( Null_Exception);}* _temp2433;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2434= yyls; struct Cyc_Yyltype* _temp2436= _temp2434.curr +(
yylsp_offset - 3); if( _temp2434.base == 0? 1:( _temp2436 < _temp2434.base? 1:
_temp2436 >= _temp2434.last_plus_one)){ _throw( Null_Exception);}* _temp2436;}).first_line,({
struct _tagged_ptr3 _temp2437= yyls; struct Cyc_Yyltype* _temp2439= _temp2437.curr
+ yylsp_offset; if( _temp2437.base == 0? 1:( _temp2439 < _temp2437.base? 1:
_temp2439 >= _temp2437.last_plus_one)){ _throw( Null_Exception);}* _temp2439;}).last_line));
_temp2420;}); _temp2419;}); break; case 17: _LL2418: yyval=( struct
_xtunion_struct*)({ struct Cyc_FnDecl_tok_struct* _temp2441=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2441[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2442; _temp2442.tag= Cyc_FnDecl_tok_tag; _temp2442.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2443=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2443->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2444= yyvs; struct _xtunion_struct** _temp2446= _temp2444.curr
+( yyvsp_offset - 2); if( _temp2444.base == 0? 1:( _temp2446 < _temp2444.base? 1:
_temp2446 >= _temp2444.last_plus_one)){ _throw( Null_Exception);}* _temp2446;}));
_temp2443;}), Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp2447= yyvs;
struct _xtunion_struct** _temp2449= _temp2447.curr +( yyvsp_offset - 1); if(
_temp2447.base == 0? 1:( _temp2449 < _temp2447.base? 1: _temp2449 >= _temp2447.last_plus_one)){
_throw( Null_Exception);}* _temp2449;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2450= yyvs; struct _xtunion_struct** _temp2452= _temp2450.curr
+ yyvsp_offset; if( _temp2450.base == 0? 1:( _temp2452 < _temp2450.base? 1:
_temp2452 >= _temp2450.last_plus_one)){ _throw( Null_Exception);}* _temp2452;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2453= yyls; struct Cyc_Yyltype*
_temp2455= _temp2453.curr +( yylsp_offset - 2); if( _temp2453.base == 0? 1:(
_temp2455 < _temp2453.base? 1: _temp2455 >= _temp2453.last_plus_one)){ _throw(
Null_Exception);}* _temp2455;}).first_line,({ struct _tagged_ptr3 _temp2456=
yyls; struct Cyc_Yyltype* _temp2458= _temp2456.curr + yylsp_offset; if(
_temp2456.base == 0? 1:( _temp2458 < _temp2456.base? 1: _temp2458 >= _temp2456.last_plus_one)){
_throw( Null_Exception);}* _temp2458;}).last_line)); _temp2442;}); _temp2441;});
break; case 18: _LL2440: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2460=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2460[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2461; _temp2461.tag= Cyc_FnDecl_tok_tag;
_temp2461.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2462=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2462->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2463= yyvs; struct
_xtunion_struct** _temp2465= _temp2463.curr +( yyvsp_offset - 3); if( _temp2463.base
== 0? 1:( _temp2465 < _temp2463.base? 1: _temp2465 >= _temp2463.last_plus_one)){
_throw( Null_Exception);}* _temp2465;})); _temp2462;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2466= yyvs; struct _xtunion_struct** _temp2468=
_temp2466.curr +( yyvsp_offset - 2); if( _temp2466.base == 0? 1:( _temp2468 <
_temp2466.base? 1: _temp2468 >= _temp2466.last_plus_one)){ _throw(
Null_Exception);}* _temp2468;})), Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2469= yyvs; struct _xtunion_struct** _temp2471= _temp2469.curr +(
yyvsp_offset - 1); if( _temp2469.base == 0? 1:( _temp2471 < _temp2469.base? 1:
_temp2471 >= _temp2469.last_plus_one)){ _throw( Null_Exception);}* _temp2471;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2472= yyvs; struct
_xtunion_struct** _temp2474= _temp2472.curr + yyvsp_offset; if( _temp2472.base
== 0? 1:( _temp2474 < _temp2472.base? 1: _temp2474 >= _temp2472.last_plus_one)){
_throw( Null_Exception);}* _temp2474;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2475= yyls; struct Cyc_Yyltype* _temp2477= _temp2475.curr +(
yylsp_offset - 3); if( _temp2475.base == 0? 1:( _temp2477 < _temp2475.base? 1:
_temp2477 >= _temp2475.last_plus_one)){ _throw( Null_Exception);}* _temp2477;}).first_line,({
struct _tagged_ptr3 _temp2478= yyls; struct Cyc_Yyltype* _temp2480= _temp2478.curr
+ yylsp_offset; if( _temp2478.base == 0? 1:( _temp2480 < _temp2478.base? 1:
_temp2480 >= _temp2478.last_plus_one)){ _throw( Null_Exception);}* _temp2480;}).last_line));
_temp2461;}); _temp2460;}); break; case 19: _LL2459: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2482= yyvs; struct _xtunion_struct** _temp2484=
_temp2482.curr + yyvsp_offset; if( _temp2482.base == 0? 1:( _temp2484 <
_temp2482.base? 1: _temp2484 >= _temp2482.last_plus_one)){ _throw(
Null_Exception);}* _temp2484;}))); yyval=({ struct _tagged_ptr2 _temp2485= yyvs;
struct _xtunion_struct** _temp2487= _temp2485.curr + yyvsp_offset; if( _temp2485.base
== 0? 1:( _temp2487 < _temp2485.base? 1: _temp2487 >= _temp2485.last_plus_one)){
_throw( Null_Exception);}* _temp2487;}); break; case 20: _LL2481: Cyc_Lex_leave_using();
break; case 21: _LL2488: Cyc_Lex_enter_namespace(({ struct _tagged_string*
_temp2490=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2490[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2491= yyvs;
struct _xtunion_struct** _temp2493= _temp2491.curr + yyvsp_offset; if( _temp2491.base
== 0? 1:( _temp2493 < _temp2491.base? 1: _temp2493 >= _temp2491.last_plus_one)){
_throw( Null_Exception);}* _temp2493;})); _temp2490;})); yyval=({ struct
_tagged_ptr2 _temp2494= yyvs; struct _xtunion_struct** _temp2496= _temp2494.curr
+ yyvsp_offset; if( _temp2494.base == 0? 1:( _temp2496 < _temp2494.base? 1:
_temp2496 >= _temp2494.last_plus_one)){ _throw( Null_Exception);}* _temp2496;});
break; case 22: _LL2489: Cyc_Lex_leave_namespace(); break; case 23: _LL2497:
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2499=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2499[ 0]=({ struct Cyc_DeclList_tok_struct _temp2500; _temp2500.tag= Cyc_DeclList_tok_tag;
_temp2500.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2501= yyvs; struct _xtunion_struct** _temp2503= _temp2501.curr
+( yyvsp_offset - 1); if( _temp2501.base == 0? 1:( _temp2503 < _temp2501.base? 1:
_temp2503 >= _temp2501.last_plus_one)){ _throw( Null_Exception);}* _temp2503;})),
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2504= yyls; struct
Cyc_Yyltype* _temp2506= _temp2504.curr +( yylsp_offset - 1); if( _temp2504.base
== 0? 1:( _temp2506 < _temp2504.base? 1: _temp2506 >= _temp2504.last_plus_one)){
_throw( Null_Exception);}* _temp2506;}).first_line,({ struct _tagged_ptr3
_temp2507= yyls; struct Cyc_Yyltype* _temp2509= _temp2507.curr +( yylsp_offset -
1); if( _temp2507.base == 0? 1:( _temp2509 < _temp2507.base? 1: _temp2509 >=
_temp2507.last_plus_one)){ _throw( Null_Exception);}* _temp2509;}).last_line));
_temp2500;}); _temp2499;}); break; case 24: _LL2498: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2511=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2511[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2512; _temp2512.tag= Cyc_DeclList_tok_tag; _temp2512.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2513= yyvs; struct
_xtunion_struct** _temp2515= _temp2513.curr +( yyvsp_offset - 2); if( _temp2513.base
== 0? 1:( _temp2515 < _temp2513.base? 1: _temp2515 >= _temp2513.last_plus_one)){
_throw( Null_Exception);}* _temp2515;})), Cyc_yyget_InitDeclList_tok(({ struct
_tagged_ptr2 _temp2516= yyvs; struct _xtunion_struct** _temp2518= _temp2516.curr
+( yyvsp_offset - 1); if( _temp2516.base == 0? 1:( _temp2518 < _temp2516.base? 1:
_temp2518 >= _temp2516.last_plus_one)){ _throw( Null_Exception);}* _temp2518;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2519= yyls; struct Cyc_Yyltype*
_temp2521= _temp2519.curr +( yylsp_offset - 2); if( _temp2519.base == 0? 1:(
_temp2521 < _temp2519.base? 1: _temp2521 >= _temp2519.last_plus_one)){ _throw(
Null_Exception);}* _temp2521;}).first_line,({ struct _tagged_ptr3 _temp2522=
yyls; struct Cyc_Yyltype* _temp2524= _temp2522.curr + yylsp_offset; if(
_temp2522.base == 0? 1:( _temp2524 < _temp2522.base? 1: _temp2524 >= _temp2522.last_plus_one)){
_throw( Null_Exception);}* _temp2524;}).last_line)); _temp2512;}); _temp2511;});
break; case 25: _LL2510: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2526=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2526[ 0]=({ struct Cyc_DeclList_tok_struct _temp2527; _temp2527.tag= Cyc_DeclList_tok_tag;
_temp2527.f1=({ struct Cyc_List_List* _temp2528=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2528->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp2529= yyvs; struct
_xtunion_struct** _temp2531= _temp2529.curr +( yyvsp_offset - 3); if( _temp2529.base
== 0? 1:( _temp2531 < _temp2529.base? 1: _temp2531 >= _temp2529.last_plus_one)){
_throw( Null_Exception);}* _temp2531;})), 0, Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp2532= yyvs; struct _xtunion_struct** _temp2534= _temp2532.curr
+( yyvsp_offset - 1); if( _temp2532.base == 0? 1:( _temp2534 < _temp2532.base? 1:
_temp2534 >= _temp2532.last_plus_one)){ _throw( Null_Exception);}* _temp2534;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2535= yyls; struct Cyc_Yyltype*
_temp2537= _temp2535.curr +( yylsp_offset - 4); if( _temp2535.base == 0? 1:(
_temp2537 < _temp2535.base? 1: _temp2537 >= _temp2535.last_plus_one)){ _throw(
Null_Exception);}* _temp2537;}).first_line,({ struct _tagged_ptr3 _temp2538=
yyls; struct Cyc_Yyltype* _temp2540= _temp2538.curr + yylsp_offset; if(
_temp2538.base == 0? 1:( _temp2540 < _temp2538.base? 1: _temp2540 >= _temp2538.last_plus_one)){
_throw( Null_Exception);}* _temp2540;}).last_line)); _temp2528->tl= 0; _temp2528;});
_temp2527;}); _temp2526;}); break; case 26: _LL2525: yyval=({ struct
_tagged_ptr2 _temp2542= yyvs; struct _xtunion_struct** _temp2544= _temp2542.curr
+ yyvsp_offset; if( _temp2542.base == 0? 1:( _temp2544 < _temp2542.base? 1:
_temp2544 >= _temp2542.last_plus_one)){ _throw( Null_Exception);}* _temp2544;});
break; case 27: _LL2541: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2546=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2546[ 0]=({ struct Cyc_DeclList_tok_struct _temp2547; _temp2547.tag= Cyc_DeclList_tok_tag;
_temp2547.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2548= yyvs; struct _xtunion_struct** _temp2550= _temp2548.curr +(
yyvsp_offset - 1); if( _temp2548.base == 0? 1:( _temp2550 < _temp2548.base? 1:
_temp2550 >= _temp2548.last_plus_one)){ _throw( Null_Exception);}* _temp2550;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2551= yyvs; struct
_xtunion_struct** _temp2553= _temp2551.curr + yyvsp_offset; if( _temp2551.base
== 0? 1:( _temp2553 < _temp2551.base? 1: _temp2553 >= _temp2551.last_plus_one)){
_throw( Null_Exception);}* _temp2553;}))); _temp2547;}); _temp2546;}); break;
case 28: _LL2545: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct*
_temp2555=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2555[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2556; _temp2556.tag= Cyc_DeclSpec_tok_tag;
_temp2556.f1=({ struct Cyc_Parse_Declaration_spec* _temp2557=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2557->sc=({ struct
Cyc_Core_Opt* _temp2561=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2561->v=( void*) Cyc_yyget_StorageClass_tok(({ struct _tagged_ptr2
_temp2562= yyvs; struct _xtunion_struct** _temp2564= _temp2562.curr +(
yyvsp_offset - 1); if( _temp2562.base == 0? 1:( _temp2564 < _temp2562.base? 1:
_temp2564 >= _temp2562.last_plus_one)){ _throw( Null_Exception);}* _temp2564;}));
_temp2561;}); _temp2557->tq= Cyc_Absyn_empty_tqual(); _temp2557->type_specs= 0;
_temp2557->is_inline= 0; _temp2557->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2558= yyvs; struct _xtunion_struct** _temp2560=
_temp2558.curr + yyvsp_offset; if( _temp2558.base == 0? 1:( _temp2560 <
_temp2558.base? 1: _temp2560 >= _temp2558.last_plus_one)){ _throw(
Null_Exception);}* _temp2560;})); _temp2557;}); _temp2556;}); _temp2555;});
break; case 29: _LL2554: if(( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2566= yyvs; struct _xtunion_struct** _temp2568= _temp2566.curr +
yyvsp_offset; if( _temp2566.base == 0? 1:( _temp2568 < _temp2566.base? 1:
_temp2568 >= _temp2566.last_plus_one)){ _throw( Null_Exception);}* _temp2568;})))->sc
!= 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp2569=( char*)"Only one storage class is allowed in a declaration";
struct _tagged_string _temp2570; _temp2570.curr= _temp2569; _temp2570.base=
_temp2569; _temp2570.last_plus_one= _temp2569 + 51; _temp2570;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2571= yyls; struct Cyc_Yyltype* _temp2573= _temp2571.curr
+( yylsp_offset - 2); if( _temp2571.base == 0? 1:( _temp2573 < _temp2571.base? 1:
_temp2573 >= _temp2571.last_plus_one)){ _throw( Null_Exception);}* _temp2573;}).first_line,({
struct _tagged_ptr3 _temp2574= yyls; struct Cyc_Yyltype* _temp2576= _temp2574.curr
+( yylsp_offset - 1); if( _temp2574.base == 0? 1:( _temp2576 < _temp2574.base? 1:
_temp2576 >= _temp2574.last_plus_one)){ _throw( Null_Exception);}* _temp2576;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2577=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2577[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2578; _temp2578.tag= Cyc_DeclSpec_tok_tag;
_temp2578.f1=({ struct Cyc_Parse_Declaration_spec* _temp2579=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2579->sc=({ struct
Cyc_Core_Opt* _temp2595=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2595->v=( void*) Cyc_yyget_StorageClass_tok(({ struct _tagged_ptr2
_temp2596= yyvs; struct _xtunion_struct** _temp2598= _temp2596.curr +(
yyvsp_offset - 2); if( _temp2596.base == 0? 1:( _temp2598 < _temp2596.base? 1:
_temp2598 >= _temp2596.last_plus_one)){ _throw( Null_Exception);}* _temp2598;}));
_temp2595;}); _temp2579->tq=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2592= yyvs; struct _xtunion_struct** _temp2594= _temp2592.curr +
yyvsp_offset; if( _temp2592.base == 0? 1:( _temp2594 < _temp2592.base? 1:
_temp2594 >= _temp2592.last_plus_one)){ _throw( Null_Exception);}* _temp2594;})))->tq;
_temp2579->type_specs=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2589=
yyvs; struct _xtunion_struct** _temp2591= _temp2589.curr + yyvsp_offset; if(
_temp2589.base == 0? 1:( _temp2591 < _temp2589.base? 1: _temp2591 >= _temp2589.last_plus_one)){
_throw( Null_Exception);}* _temp2591;})))->type_specs; _temp2579->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2586= yyvs; struct
_xtunion_struct** _temp2588= _temp2586.curr + yyvsp_offset; if( _temp2586.base
== 0? 1:( _temp2588 < _temp2586.base? 1: _temp2588 >= _temp2586.last_plus_one)){
_throw( Null_Exception);}* _temp2588;})))->is_inline; _temp2579->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2580= yyvs; struct
_xtunion_struct** _temp2582= _temp2580.curr +( yyvsp_offset - 1); if( _temp2580.base
== 0? 1:( _temp2582 < _temp2580.base? 1: _temp2582 >= _temp2580.last_plus_one)){
_throw( Null_Exception);}* _temp2582;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2583= yyvs; struct _xtunion_struct** _temp2585= _temp2583.curr
+ yyvsp_offset; if( _temp2583.base == 0? 1:( _temp2585 < _temp2583.base? 1:
_temp2585 >= _temp2583.last_plus_one)){ _throw( Null_Exception);}* _temp2585;})))->attributes);
_temp2579;}); _temp2578;}); _temp2577;}); break; case 30: _LL2565: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2600=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2600[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2601; _temp2601.tag= Cyc_DeclSpec_tok_tag;
_temp2601.f1=({ struct Cyc_Parse_Declaration_spec* _temp2602=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2602->sc= 0;
_temp2602->tq= Cyc_Absyn_empty_tqual(); _temp2602->type_specs=({ struct Cyc_List_List*
_temp2606=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2606->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2607= yyvs; struct _xtunion_struct** _temp2609= _temp2607.curr +(
yyvsp_offset - 1); if( _temp2607.base == 0? 1:( _temp2609 < _temp2607.base? 1:
_temp2609 >= _temp2607.last_plus_one)){ _throw( Null_Exception);}* _temp2609;}));
_temp2606->tl= 0; _temp2606;}); _temp2602->is_inline= 0; _temp2602->attributes=
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2603= yyvs; struct
_xtunion_struct** _temp2605= _temp2603.curr + yyvsp_offset; if( _temp2603.base
== 0? 1:( _temp2605 < _temp2603.base? 1: _temp2605 >= _temp2603.last_plus_one)){
_throw( Null_Exception);}* _temp2605;})); _temp2602;}); _temp2601;}); _temp2600;});
break; case 31: _LL2599: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct*
_temp2611=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2611[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2612; _temp2612.tag= Cyc_DeclSpec_tok_tag;
_temp2612.f1=({ struct Cyc_Parse_Declaration_spec* _temp2613=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2613->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2633= yyvs; struct _xtunion_struct** _temp2635=
_temp2633.curr + yyvsp_offset; if( _temp2633.base == 0? 1:( _temp2635 <
_temp2633.base? 1: _temp2635 >= _temp2633.last_plus_one)){ _throw(
Null_Exception);}* _temp2635;})))->sc; _temp2613->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2630= yyvs; struct _xtunion_struct** _temp2632=
_temp2630.curr + yyvsp_offset; if( _temp2630.base == 0? 1:( _temp2632 <
_temp2630.base? 1: _temp2632 >= _temp2630.last_plus_one)){ _throw(
Null_Exception);}* _temp2632;})))->tq; _temp2613->type_specs=({ struct Cyc_List_List*
_temp2623=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2623->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2627= yyvs; struct _xtunion_struct** _temp2629= _temp2627.curr +(
yyvsp_offset - 2); if( _temp2627.base == 0? 1:( _temp2629 < _temp2627.base? 1:
_temp2629 >= _temp2627.last_plus_one)){ _throw( Null_Exception);}* _temp2629;}));
_temp2623->tl=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2624= yyvs;
struct _xtunion_struct** _temp2626= _temp2624.curr + yyvsp_offset; if( _temp2624.base
== 0? 1:( _temp2626 < _temp2624.base? 1: _temp2626 >= _temp2624.last_plus_one)){
_throw( Null_Exception);}* _temp2626;})))->type_specs; _temp2623;}); _temp2613->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2620= yyvs; struct
_xtunion_struct** _temp2622= _temp2620.curr + yyvsp_offset; if( _temp2620.base
== 0? 1:( _temp2622 < _temp2620.base? 1: _temp2622 >= _temp2620.last_plus_one)){
_throw( Null_Exception);}* _temp2622;})))->is_inline; _temp2613->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2614= yyvs; struct
_xtunion_struct** _temp2616= _temp2614.curr +( yyvsp_offset - 1); if( _temp2614.base
== 0? 1:( _temp2616 < _temp2614.base? 1: _temp2616 >= _temp2614.last_plus_one)){
_throw( Null_Exception);}* _temp2616;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2617= yyvs; struct _xtunion_struct** _temp2619= _temp2617.curr
+ yyvsp_offset; if( _temp2617.base == 0? 1:( _temp2619 < _temp2617.base? 1:
_temp2619 >= _temp2617.last_plus_one)){ _throw( Null_Exception);}* _temp2619;})))->attributes);
_temp2613;}); _temp2612;}); _temp2611;}); break; case 32: _LL2610: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2637=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2637[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2638; _temp2638.tag= Cyc_DeclSpec_tok_tag;
_temp2638.f1=({ struct Cyc_Parse_Declaration_spec* _temp2639=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2639->sc= 0;
_temp2639->tq= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp2643= yyvs;
struct _xtunion_struct** _temp2645= _temp2643.curr +( yyvsp_offset - 1); if(
_temp2643.base == 0? 1:( _temp2645 < _temp2643.base? 1: _temp2645 >= _temp2643.last_plus_one)){
_throw( Null_Exception);}* _temp2645;})); _temp2639->type_specs= 0; _temp2639->is_inline=
0; _temp2639->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2640= yyvs; struct _xtunion_struct** _temp2642= _temp2640.curr +
yyvsp_offset; if( _temp2640.base == 0? 1:( _temp2642 < _temp2640.base? 1:
_temp2642 >= _temp2640.last_plus_one)){ _throw( Null_Exception);}* _temp2642;}));
_temp2639;}); _temp2638;}); _temp2637;}); break; case 33: _LL2636: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2647=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2647[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2648; _temp2648.tag= Cyc_DeclSpec_tok_tag;
_temp2648.f1=({ struct Cyc_Parse_Declaration_spec* _temp2649=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2649->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2668= yyvs; struct _xtunion_struct** _temp2670=
_temp2668.curr + yyvsp_offset; if( _temp2668.base == 0? 1:( _temp2670 <
_temp2668.base? 1: _temp2670 >= _temp2668.last_plus_one)){ _throw(
Null_Exception);}* _temp2670;})))->sc; _temp2649->tq= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp2662= yyvs; struct
_xtunion_struct** _temp2664= _temp2662.curr +( yyvsp_offset - 2); if( _temp2662.base
== 0? 1:( _temp2664 < _temp2662.base? 1: _temp2664 >= _temp2662.last_plus_one)){
_throw( Null_Exception);}* _temp2664;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2665= yyvs; struct _xtunion_struct** _temp2667= _temp2665.curr
+ yyvsp_offset; if( _temp2665.base == 0? 1:( _temp2667 < _temp2665.base? 1:
_temp2667 >= _temp2665.last_plus_one)){ _throw( Null_Exception);}* _temp2667;})))->tq);
_temp2649->type_specs=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2659=
yyvs; struct _xtunion_struct** _temp2661= _temp2659.curr + yyvsp_offset; if(
_temp2659.base == 0? 1:( _temp2661 < _temp2659.base? 1: _temp2661 >= _temp2659.last_plus_one)){
_throw( Null_Exception);}* _temp2661;})))->type_specs; _temp2649->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2656= yyvs; struct
_xtunion_struct** _temp2658= _temp2656.curr + yyvsp_offset; if( _temp2656.base
== 0? 1:( _temp2658 < _temp2656.base? 1: _temp2658 >= _temp2656.last_plus_one)){
_throw( Null_Exception);}* _temp2658;})))->is_inline; _temp2649->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2650= yyvs; struct
_xtunion_struct** _temp2652= _temp2650.curr +( yyvsp_offset - 1); if( _temp2650.base
== 0? 1:( _temp2652 < _temp2650.base? 1: _temp2652 >= _temp2650.last_plus_one)){
_throw( Null_Exception);}* _temp2652;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2653= yyvs; struct _xtunion_struct** _temp2655= _temp2653.curr
+ yyvsp_offset; if( _temp2653.base == 0? 1:( _temp2655 < _temp2653.base? 1:
_temp2655 >= _temp2653.last_plus_one)){ _throw( Null_Exception);}* _temp2655;})))->attributes);
_temp2649;}); _temp2648;}); _temp2647;}); break; case 34: _LL2646: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2672=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2672[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2673; _temp2673.tag= Cyc_DeclSpec_tok_tag;
_temp2673.f1=({ struct Cyc_Parse_Declaration_spec* _temp2674=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2674->sc= 0;
_temp2674->tq= Cyc_Absyn_empty_tqual(); _temp2674->type_specs= 0; _temp2674->is_inline=
1; _temp2674->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2675= yyvs; struct _xtunion_struct** _temp2677= _temp2675.curr +
yyvsp_offset; if( _temp2675.base == 0? 1:( _temp2677 < _temp2675.base? 1:
_temp2677 >= _temp2675.last_plus_one)){ _throw( Null_Exception);}* _temp2677;}));
_temp2674;}); _temp2673;}); _temp2672;}); break; case 35: _LL2671: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2679=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2679[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2680; _temp2680.tag= Cyc_DeclSpec_tok_tag;
_temp2680.f1=({ struct Cyc_Parse_Declaration_spec* _temp2681=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2681->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2694= yyvs; struct _xtunion_struct** _temp2696=
_temp2694.curr + yyvsp_offset; if( _temp2694.base == 0? 1:( _temp2696 <
_temp2694.base? 1: _temp2696 >= _temp2694.last_plus_one)){ _throw(
Null_Exception);}* _temp2696;})))->sc; _temp2681->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2691= yyvs; struct _xtunion_struct** _temp2693=
_temp2691.curr + yyvsp_offset; if( _temp2691.base == 0? 1:( _temp2693 <
_temp2691.base? 1: _temp2693 >= _temp2691.last_plus_one)){ _throw(
Null_Exception);}* _temp2693;})))->tq; _temp2681->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2688= yyvs; struct _xtunion_struct** _temp2690=
_temp2688.curr + yyvsp_offset; if( _temp2688.base == 0? 1:( _temp2690 <
_temp2688.base? 1: _temp2690 >= _temp2688.last_plus_one)){ _throw(
Null_Exception);}* _temp2690;})))->type_specs; _temp2681->is_inline= 1;
_temp2681->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2682= yyvs; struct _xtunion_struct** _temp2684=
_temp2682.curr +( yyvsp_offset - 1); if( _temp2682.base == 0? 1:( _temp2684 <
_temp2682.base? 1: _temp2684 >= _temp2682.last_plus_one)){ _throw(
Null_Exception);}* _temp2684;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2685= yyvs; struct _xtunion_struct** _temp2687= _temp2685.curr +
yyvsp_offset; if( _temp2685.base == 0? 1:( _temp2687 < _temp2685.base? 1:
_temp2687 >= _temp2685.last_plus_one)){ _throw( Null_Exception);}* _temp2687;})))->attributes);
_temp2681;}); _temp2680;}); _temp2679;}); break; case 36: _LL2678: yyval=(
struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2698=(
struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2698[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2699; _temp2699.tag=
Cyc_StorageClass_tok_tag; _temp2699.f1=( void*)(( void*) Cyc_Parse_Auto_sc);
_temp2699;}); _temp2698;}); break; case 37: _LL2697: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2701=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2701[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2702; _temp2702.tag= Cyc_StorageClass_tok_tag;
_temp2702.f1=( void*)(( void*) Cyc_Parse_Register_sc); _temp2702;}); _temp2701;});
break; case 38: _LL2700: yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct*
_temp2704=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2704[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2705; _temp2705.tag=
Cyc_StorageClass_tok_tag; _temp2705.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp2705;}); _temp2704;}); break; case 39: _LL2703: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2707=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2707[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2708; _temp2708.tag= Cyc_StorageClass_tok_tag;
_temp2708.f1=( void*)(( void*) Cyc_Parse_Extern_sc); _temp2708;}); _temp2707;});
break; case 40: _LL2706: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2710= yyvs; struct _xtunion_struct** _temp2712= _temp2710.curr
+ yyvsp_offset; if( _temp2710.base == 0? 1:( _temp2712 < _temp2710.base? 1:
_temp2712 >= _temp2710.last_plus_one)){ _throw( Null_Exception);}* _temp2712;})),(
struct _tagged_string)({ char* _temp2713=( char*)"C"; struct _tagged_string
_temp2714; _temp2714.curr= _temp2713; _temp2714.base= _temp2713; _temp2714.last_plus_one=
_temp2713 + 2; _temp2714;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp2715=( char*)"only extern or extern \"C\" is allowed"; struct
_tagged_string _temp2716; _temp2716.curr= _temp2715; _temp2716.base= _temp2715;
_temp2716.last_plus_one= _temp2715 + 37; _temp2716;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2717= yyls; struct Cyc_Yyltype* _temp2719= _temp2717.curr
+( yylsp_offset - 1); if( _temp2717.base == 0? 1:( _temp2719 < _temp2717.base? 1:
_temp2719 >= _temp2717.last_plus_one)){ _throw( Null_Exception);}* _temp2719;}).first_line,({
struct _tagged_ptr3 _temp2720= yyls; struct Cyc_Yyltype* _temp2722= _temp2720.curr
+ yylsp_offset; if( _temp2720.base == 0? 1:( _temp2722 < _temp2720.base? 1:
_temp2722 >= _temp2720.last_plus_one)){ _throw( Null_Exception);}* _temp2722;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2723=(
struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2723[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2724; _temp2724.tag=
Cyc_StorageClass_tok_tag; _temp2724.f1=( void*)(( void*) Cyc_Parse_ExternC_sc);
_temp2724;}); _temp2723;}); break; case 41: _LL2709: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2726=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2726[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2727; _temp2727.tag= Cyc_StorageClass_tok_tag;
_temp2727.f1=( void*)(( void*) Cyc_Parse_Typedef_sc); _temp2727;}); _temp2726;});
break; case 42: _LL2725: yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct*
_temp2729=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2729[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2730; _temp2730.tag=
Cyc_StorageClass_tok_tag; _temp2730.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp2730;}); _temp2729;}); break; case 43: _LL2728: yyval=( struct
_xtunion_struct*)({ struct Cyc_AttributeList_tok_struct* _temp2732=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2732[ 0]=({
struct Cyc_AttributeList_tok_struct _temp2733; _temp2733.tag= Cyc_AttributeList_tok_tag;
_temp2733.f1= 0; _temp2733;}); _temp2732;}); break; case 44: _LL2731: yyval=({
struct _tagged_ptr2 _temp2735= yyvs; struct _xtunion_struct** _temp2737=
_temp2735.curr + yyvsp_offset; if( _temp2735.base == 0? 1:( _temp2737 <
_temp2735.base? 1: _temp2737 >= _temp2735.last_plus_one)){ _throw(
Null_Exception);}* _temp2737;}); break; case 45: _LL2734: yyval=( struct
_xtunion_struct*)({ struct Cyc_AttributeList_tok_struct* _temp2739=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2739[ 0]=({
struct Cyc_AttributeList_tok_struct _temp2740; _temp2740.tag= Cyc_AttributeList_tok_tag;
_temp2740.f1= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2741= yyvs;
struct _xtunion_struct** _temp2743= _temp2741.curr +( yyvsp_offset - 2); if(
_temp2741.base == 0? 1:( _temp2743 < _temp2741.base? 1: _temp2743 >= _temp2741.last_plus_one)){
_throw( Null_Exception);}* _temp2743;})); _temp2740;}); _temp2739;}); break;
case 46: _LL2738: yyval=( struct _xtunion_struct*)({ struct Cyc_AttributeList_tok_struct*
_temp2745=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2745[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2746; _temp2746.tag=
Cyc_AttributeList_tok_tag; _temp2746.f1=({ struct Cyc_List_List* _temp2747=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2747->hd=(
void*) Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2748= yyvs; struct
_xtunion_struct** _temp2750= _temp2748.curr + yyvsp_offset; if( _temp2748.base
== 0? 1:( _temp2750 < _temp2748.base? 1: _temp2750 >= _temp2748.last_plus_one)){
_throw( Null_Exception);}* _temp2750;})); _temp2747->tl= 0; _temp2747;});
_temp2746;}); _temp2745;}); break; case 47: _LL2744: yyval=( struct
_xtunion_struct*)({ struct Cyc_AttributeList_tok_struct* _temp2752=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2752[ 0]=({
struct Cyc_AttributeList_tok_struct _temp2753; _temp2753.tag= Cyc_AttributeList_tok_tag;
_temp2753.f1=({ struct Cyc_List_List* _temp2754=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2754->hd=( void*) Cyc_yyget_Attribute_tok(({
struct _tagged_ptr2 _temp2758= yyvs; struct _xtunion_struct** _temp2760=
_temp2758.curr +( yyvsp_offset - 2); if( _temp2758.base == 0? 1:( _temp2760 <
_temp2758.base? 1: _temp2760 >= _temp2758.last_plus_one)){ _throw(
Null_Exception);}* _temp2760;})); _temp2754->tl= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2755= yyvs; struct _xtunion_struct** _temp2757=
_temp2755.curr + yyvsp_offset; if( _temp2755.base == 0? 1:( _temp2757 <
_temp2755.base? 1: _temp2757 >= _temp2755.last_plus_one)){ _throw(
Null_Exception);}* _temp2757;})); _temp2754;}); _temp2753;}); _temp2752;});
break; case 48: _LL2751: { struct _tagged_string s= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2762= yyvs; struct _xtunion_struct** _temp2764=
_temp2762.curr + yyvsp_offset; if( _temp2762.base == 0? 1:( _temp2764 <
_temp2762.base? 1: _temp2764 >= _temp2762.last_plus_one)){ _throw(
Null_Exception);}* _temp2764;})); void* a; if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2765=( char*)"stdcall"; struct _tagged_string
_temp2766; _temp2766.curr= _temp2765; _temp2766.base= _temp2765; _temp2766.last_plus_one=
_temp2765 + 8; _temp2766;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2767=( char*)"__stdcall__"; struct _tagged_string
_temp2768; _temp2768.curr= _temp2767; _temp2768.base= _temp2767; _temp2768.last_plus_one=
_temp2767 + 12; _temp2768;})) == 0){ a=( void*) Cyc_Absyn_Stdcall_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2769=( char*)"cdecl";
struct _tagged_string _temp2770; _temp2770.curr= _temp2769; _temp2770.base=
_temp2769; _temp2770.last_plus_one= _temp2769 + 6; _temp2770;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2771=( char*)"__cdecl__"; struct
_tagged_string _temp2772; _temp2772.curr= _temp2771; _temp2772.base= _temp2771;
_temp2772.last_plus_one= _temp2771 + 10; _temp2772;})) == 0){ a=( void*) Cyc_Absyn_Cdecl_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2773=(
char*)"noreturn"; struct _tagged_string _temp2774; _temp2774.curr= _temp2773;
_temp2774.base= _temp2773; _temp2774.last_plus_one= _temp2773 + 9; _temp2774;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2775=( char*)"__noreturn__";
struct _tagged_string _temp2776; _temp2776.curr= _temp2775; _temp2776.base=
_temp2775; _temp2776.last_plus_one= _temp2775 + 13; _temp2776;})) == 0){ a=(
void*) Cyc_Absyn_Noreturn_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2777=( char*)"noreturn"; struct _tagged_string
_temp2778; _temp2778.curr= _temp2777; _temp2778.base= _temp2777; _temp2778.last_plus_one=
_temp2777 + 9; _temp2778;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2779=( char*)"__noreturn__"; struct _tagged_string
_temp2780; _temp2780.curr= _temp2779; _temp2780.base= _temp2779; _temp2780.last_plus_one=
_temp2779 + 13; _temp2780;})) == 0){ a=( void*) Cyc_Absyn_Noreturn_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2781=( char*)"__const__";
struct _tagged_string _temp2782; _temp2782.curr= _temp2781; _temp2782.base=
_temp2781; _temp2782.last_plus_one= _temp2781 + 10; _temp2782;})) == 0){ a=(
void*) Cyc_Absyn_Const_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2783=( char*)"aligned"; struct _tagged_string
_temp2784; _temp2784.curr= _temp2783; _temp2784.base= _temp2783; _temp2784.last_plus_one=
_temp2783 + 8; _temp2784;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2785=( char*)"__aligned__"; struct _tagged_string
_temp2786; _temp2786.curr= _temp2785; _temp2786.base= _temp2785; _temp2786.last_plus_one=
_temp2785 + 12; _temp2786;})) == 0){ a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp2787=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct));
_temp2787[ 0]=({ struct Cyc_Absyn_Aligned_att_struct _temp2788; _temp2788.tag=
Cyc_Absyn_Aligned_att_tag; _temp2788.f1= - 1; _temp2788;}); _temp2787;});} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2789=( char*)"packed";
struct _tagged_string _temp2790; _temp2790.curr= _temp2789; _temp2790.base=
_temp2789; _temp2790.last_plus_one= _temp2789 + 7; _temp2790;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2791=( char*)"__packed__"; struct
_tagged_string _temp2792; _temp2792.curr= _temp2791; _temp2792.base= _temp2791;
_temp2792.last_plus_one= _temp2791 + 11; _temp2792;})) == 0){ a=( void*) Cyc_Absyn_Packed_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2793=(
char*)"shared"; struct _tagged_string _temp2794; _temp2794.curr= _temp2793;
_temp2794.base= _temp2793; _temp2794.last_plus_one= _temp2793 + 7; _temp2794;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2795=( char*)"__shared__";
struct _tagged_string _temp2796; _temp2796.curr= _temp2795; _temp2796.base=
_temp2795; _temp2796.last_plus_one= _temp2795 + 11; _temp2796;})) == 0){ a=(
void*) Cyc_Absyn_Shared_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2797=( char*)"unused"; struct _tagged_string
_temp2798; _temp2798.curr= _temp2797; _temp2798.base= _temp2797; _temp2798.last_plus_one=
_temp2797 + 7; _temp2798;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2799=( char*)"__unused__"; struct _tagged_string
_temp2800; _temp2800.curr= _temp2799; _temp2800.base= _temp2799; _temp2800.last_plus_one=
_temp2799 + 11; _temp2800;})) == 0){ a=( void*) Cyc_Absyn_Unused_att;} else{ if(
Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2801=( char*)"weak";
struct _tagged_string _temp2802; _temp2802.curr= _temp2801; _temp2802.base=
_temp2801; _temp2802.last_plus_one= _temp2801 + 5; _temp2802;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2803=( char*)"__weak__"; struct
_tagged_string _temp2804; _temp2804.curr= _temp2803; _temp2804.base= _temp2803;
_temp2804.last_plus_one= _temp2803 + 9; _temp2804;})) == 0){ a=( void*) Cyc_Absyn_Weak_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2805=(
char*)"dllimport"; struct _tagged_string _temp2806; _temp2806.curr= _temp2805;
_temp2806.base= _temp2805; _temp2806.last_plus_one= _temp2805 + 10; _temp2806;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2807=( char*)"__dllimport__";
struct _tagged_string _temp2808; _temp2808.curr= _temp2807; _temp2808.base=
_temp2807; _temp2808.last_plus_one= _temp2807 + 14; _temp2808;})) == 0){ a=(
void*) Cyc_Absyn_Dllimport_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2809=( char*)"dllexport"; struct _tagged_string
_temp2810; _temp2810.curr= _temp2809; _temp2810.base= _temp2809; _temp2810.last_plus_one=
_temp2809 + 10; _temp2810;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2811=( char*)"__dllexport__"; struct _tagged_string
_temp2812; _temp2812.curr= _temp2811; _temp2812.base= _temp2811; _temp2812.last_plus_one=
_temp2811 + 14; _temp2812;})) == 0){ a=( void*) Cyc_Absyn_Dllexport_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2813=( char*)"no_instrument_function";
struct _tagged_string _temp2814; _temp2814.curr= _temp2813; _temp2814.base=
_temp2813; _temp2814.last_plus_one= _temp2813 + 23; _temp2814;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2815=( char*)"__no_instrument_function__";
struct _tagged_string _temp2816; _temp2816.curr= _temp2815; _temp2816.base=
_temp2815; _temp2816.last_plus_one= _temp2815 + 27; _temp2816;})) == 0){ a=(
void*) Cyc_Absyn_No_instrument_function_att;} else{ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2817=( char*)"constructor"; struct
_tagged_string _temp2818; _temp2818.curr= _temp2817; _temp2818.base= _temp2817;
_temp2818.last_plus_one= _temp2817 + 12; _temp2818;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2819=( char*)"__constructor__"; struct
_tagged_string _temp2820; _temp2820.curr= _temp2819; _temp2820.base= _temp2819;
_temp2820.last_plus_one= _temp2819 + 16; _temp2820;})) == 0){ a=( void*) Cyc_Absyn_Constructor_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2821=(
char*)"destructor"; struct _tagged_string _temp2822; _temp2822.curr= _temp2821;
_temp2822.base= _temp2821; _temp2822.last_plus_one= _temp2821 + 11; _temp2822;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2823=( char*)"__destructor__";
struct _tagged_string _temp2824; _temp2824.curr= _temp2823; _temp2824.base=
_temp2823; _temp2824.last_plus_one= _temp2823 + 15; _temp2824;})) == 0){ a=(
void*) Cyc_Absyn_Destructor_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2825=( char*)"no_check_memory_usage"; struct
_tagged_string _temp2826; _temp2826.curr= _temp2825; _temp2826.base= _temp2825;
_temp2826.last_plus_one= _temp2825 + 22; _temp2826;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2827=( char*)"__no_check_memory_usage__";
struct _tagged_string _temp2828; _temp2828.curr= _temp2827; _temp2828.base=
_temp2827; _temp2828.last_plus_one= _temp2827 + 26; _temp2828;})) == 0){ a=(
void*) Cyc_Absyn_No_check_memory_usage_att;} else{ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2829=( char*)"unrecognized attribute"; struct
_tagged_string _temp2830; _temp2830.curr= _temp2829; _temp2830.base= _temp2829;
_temp2830.last_plus_one= _temp2829 + 23; _temp2830;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2831= yyls; struct Cyc_Yyltype* _temp2833= _temp2831.curr
+ yylsp_offset; if( _temp2831.base == 0? 1:( _temp2833 < _temp2831.base? 1:
_temp2833 >= _temp2831.last_plus_one)){ _throw( Null_Exception);}* _temp2833;}).first_line,({
struct _tagged_ptr3 _temp2834= yyls; struct Cyc_Yyltype* _temp2836= _temp2834.curr
+ yylsp_offset; if( _temp2834.base == 0? 1:( _temp2836 < _temp2834.base? 1:
_temp2836 >= _temp2834.last_plus_one)){ _throw( Null_Exception);}* _temp2836;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}}}}}}}}}}}}}}}} yyval=( struct _xtunion_struct*)({
struct Cyc_Attribute_tok_struct* _temp2837=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2837[ 0]=({ struct
Cyc_Attribute_tok_struct _temp2838; _temp2838.tag= Cyc_Attribute_tok_tag;
_temp2838.f1=( void*) a; _temp2838;}); _temp2837;}); break;} case 49: _LL2761:
yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct* _temp2840=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2840[ 0]=({ struct Cyc_Attribute_tok_struct _temp2841; _temp2841.tag= Cyc_Attribute_tok_tag;
_temp2841.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp2841;}); _temp2840;});
break; case 50: _LL2839: { struct _tagged_string s= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2843= yyvs; struct _xtunion_struct** _temp2845=
_temp2843.curr +( yyvsp_offset - 3); if( _temp2843.base == 0? 1:( _temp2845 <
_temp2843.base? 1: _temp2845 >= _temp2843.last_plus_one)){ _throw(
Null_Exception);}* _temp2845;})); struct _tuple12 _temp2851; int _temp2852; void*
_temp2854; struct _tuple12* _temp2849= Cyc_yyget_Int_tok(({ struct _tagged_ptr2
_temp2846= yyvs; struct _xtunion_struct** _temp2848= _temp2846.curr +(
yyvsp_offset - 1); if( _temp2846.base == 0? 1:( _temp2848 < _temp2846.base? 1:
_temp2848 >= _temp2846.last_plus_one)){ _throw( Null_Exception);}* _temp2848;}));
_temp2851=* _temp2849; _LL2855: _temp2854= _temp2851.f1; goto _LL2853; _LL2853:
_temp2852= _temp2851.f2; goto _LL2850; _LL2850: { void* a; if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2856=( char*)"regparm"; struct
_tagged_string _temp2857; _temp2857.curr= _temp2856; _temp2857.base= _temp2856;
_temp2857.last_plus_one= _temp2856 + 8; _temp2857;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2858=( char*)"__regparm__"; struct
_tagged_string _temp2859; _temp2859.curr= _temp2858; _temp2859.base= _temp2858;
_temp2859.last_plus_one= _temp2858 + 12; _temp2859;})) == 0){ if( _temp2852 <= 0?
1: _temp2852 > 3){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2860=(
char*)"regparm requires value between 1 and 3"; struct _tagged_string _temp2861;
_temp2861.curr= _temp2860; _temp2861.base= _temp2860; _temp2861.last_plus_one=
_temp2860 + 39; _temp2861;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2862= yyls; struct Cyc_Yyltype* _temp2864= _temp2862.curr +( yylsp_offset -
1); if( _temp2862.base == 0? 1:( _temp2864 < _temp2862.base? 1: _temp2864 >=
_temp2862.last_plus_one)){ _throw( Null_Exception);}* _temp2864;}).first_line,({
struct _tagged_ptr3 _temp2865= yyls; struct Cyc_Yyltype* _temp2867= _temp2865.curr
+( yylsp_offset - 1); if( _temp2865.base == 0? 1:( _temp2867 < _temp2865.base? 1:
_temp2867 >= _temp2865.last_plus_one)){ _throw( Null_Exception);}* _temp2867;}).last_line));}
a=( void*)({ struct Cyc_Absyn_Regparm_att_struct* _temp2868=( struct Cyc_Absyn_Regparm_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Regparm_att_struct)); _temp2868[ 0]=({
struct Cyc_Absyn_Regparm_att_struct _temp2869; _temp2869.tag= Cyc_Absyn_Regparm_att_tag;
_temp2869.f1= _temp2852; _temp2869;}); _temp2868;});} else{ if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2870=( char*)"aligned"; struct
_tagged_string _temp2871; _temp2871.curr= _temp2870; _temp2871.base= _temp2870;
_temp2871.last_plus_one= _temp2870 + 8; _temp2871;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2872=( char*)"__aligned__"; struct
_tagged_string _temp2873; _temp2873.curr= _temp2872; _temp2873.base= _temp2872;
_temp2873.last_plus_one= _temp2872 + 12; _temp2873;})) == 0){ if( _temp2852 < 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2874=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2875; _temp2875.curr= _temp2874; _temp2875.base=
_temp2874; _temp2875.last_plus_one= _temp2874 + 39; _temp2875;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2876= yyls; struct Cyc_Yyltype* _temp2878= _temp2876.curr
+( yylsp_offset - 1); if( _temp2876.base == 0? 1:( _temp2878 < _temp2876.base? 1:
_temp2878 >= _temp2876.last_plus_one)){ _throw( Null_Exception);}* _temp2878;}).first_line,({
struct _tagged_ptr3 _temp2879= yyls; struct Cyc_Yyltype* _temp2881= _temp2879.curr
+( yylsp_offset - 1); if( _temp2879.base == 0? 1:( _temp2881 < _temp2879.base? 1:
_temp2881 >= _temp2879.last_plus_one)){ _throw( Null_Exception);}* _temp2881;}).last_line));}{
unsigned int j=( unsigned int) _temp2852; for( 0;( j &( unsigned int) 1) ==(
unsigned int) 0; j= j >>( unsigned int) 1){;} j= j >>( unsigned int) 1; if( j !=(
unsigned int) 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2882=(
char*)"aligned requires positive power of two"; struct _tagged_string _temp2883;
_temp2883.curr= _temp2882; _temp2883.base= _temp2882; _temp2883.last_plus_one=
_temp2882 + 39; _temp2883;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2884= yyls; struct Cyc_Yyltype* _temp2886= _temp2884.curr +( yylsp_offset -
1); if( _temp2884.base == 0? 1:( _temp2886 < _temp2884.base? 1: _temp2886 >=
_temp2884.last_plus_one)){ _throw( Null_Exception);}* _temp2886;}).first_line,({
struct _tagged_ptr3 _temp2887= yyls; struct Cyc_Yyltype* _temp2889= _temp2887.curr
+( yylsp_offset - 1); if( _temp2887.base == 0? 1:( _temp2889 < _temp2887.base? 1:
_temp2889 >= _temp2887.last_plus_one)){ _throw( Null_Exception);}* _temp2889;}).last_line));}
a=( void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp2890=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp2890[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp2891; _temp2891.tag= Cyc_Absyn_Aligned_att_tag;
_temp2891.f1= _temp2852; _temp2891;}); _temp2890;});}} else{ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2892=( char*)"unrecognized attribute";
struct _tagged_string _temp2893; _temp2893.curr= _temp2892; _temp2893.base=
_temp2892; _temp2893.last_plus_one= _temp2892 + 23; _temp2893;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2894= yyls; struct Cyc_Yyltype* _temp2896= _temp2894.curr
+( yylsp_offset - 3); if( _temp2894.base == 0? 1:( _temp2896 < _temp2894.base? 1:
_temp2896 >= _temp2894.last_plus_one)){ _throw( Null_Exception);}* _temp2896;}).first_line,({
struct _tagged_ptr3 _temp2897= yyls; struct Cyc_Yyltype* _temp2899= _temp2897.curr
+( yylsp_offset - 3); if( _temp2897.base == 0? 1:( _temp2899 < _temp2897.base? 1:
_temp2899 >= _temp2897.last_plus_one)){ _throw( Null_Exception);}* _temp2899;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}} yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct*
_temp2900=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2900[ 0]=({ struct Cyc_Attribute_tok_struct _temp2901; _temp2901.tag= Cyc_Attribute_tok_tag;
_temp2901.f1=( void*) a; _temp2901;}); _temp2900;}); break;}} case 51: _LL2842: {
struct _tagged_string s= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2903=
yyvs; struct _xtunion_struct** _temp2905= _temp2903.curr +( yyvsp_offset - 3);
if( _temp2903.base == 0? 1:( _temp2905 < _temp2903.base? 1: _temp2905 >=
_temp2903.last_plus_one)){ _throw( Null_Exception);}* _temp2905;})); struct
_tagged_string str= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2906= yyvs;
struct _xtunion_struct** _temp2908= _temp2906.curr +( yyvsp_offset - 1); if(
_temp2906.base == 0? 1:( _temp2908 < _temp2906.base? 1: _temp2908 >= _temp2906.last_plus_one)){
_throw( Null_Exception);}* _temp2908;})); void* a; if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2909=( char*)"section"; struct
_tagged_string _temp2910; _temp2910.curr= _temp2909; _temp2910.base= _temp2909;
_temp2910.last_plus_one= _temp2909 + 8; _temp2910;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2911=( char*)"__section__"; struct
_tagged_string _temp2912; _temp2912.curr= _temp2911; _temp2912.base= _temp2911;
_temp2912.last_plus_one= _temp2911 + 12; _temp2912;}))){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp2913=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp2913[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp2914; _temp2914.tag=
Cyc_Absyn_Section_att_tag; _temp2914.f1= str; _temp2914;}); _temp2913;});} else{
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2915=( char*)"unrecognized attribute";
struct _tagged_string _temp2916; _temp2916.curr= _temp2915; _temp2916.base=
_temp2915; _temp2916.last_plus_one= _temp2915 + 23; _temp2916;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2917= yyls; struct Cyc_Yyltype* _temp2919= _temp2917.curr
+( yylsp_offset - 3); if( _temp2917.base == 0? 1:( _temp2919 < _temp2917.base? 1:
_temp2919 >= _temp2917.last_plus_one)){ _throw( Null_Exception);}* _temp2919;}).first_line,({
struct _tagged_ptr3 _temp2920= yyls; struct Cyc_Yyltype* _temp2922= _temp2920.curr
+( yylsp_offset - 3); if( _temp2920.base == 0? 1:( _temp2922 < _temp2920.base? 1:
_temp2922 >= _temp2920.last_plus_one)){ _throw( Null_Exception);}* _temp2922;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct*
_temp2923=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2923[ 0]=({ struct Cyc_Attribute_tok_struct _temp2924; _temp2924.tag= Cyc_Attribute_tok_tag;
_temp2924.f1=( void*) a; _temp2924;}); _temp2923;}); break;} case 52: _LL2902:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2926=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2926[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2927; _temp2927.tag=
Cyc_TypeSpecifier_tok_tag; _temp2927.f1=( void*) Cyc_Parse_type_spec(( void*)
Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2928=
yyls; struct Cyc_Yyltype* _temp2930= _temp2928.curr + yylsp_offset; if(
_temp2928.base == 0? 1:( _temp2930 < _temp2928.base? 1: _temp2930 >= _temp2928.last_plus_one)){
_throw( Null_Exception);}* _temp2930;}).first_line,({ struct _tagged_ptr3
_temp2931= yyls; struct Cyc_Yyltype* _temp2933= _temp2931.curr + yylsp_offset;
if( _temp2931.base == 0? 1:( _temp2933 < _temp2931.base? 1: _temp2933 >=
_temp2931.last_plus_one)){ _throw( Null_Exception);}* _temp2933;}).last_line));
_temp2927;}); _temp2926;}); break; case 53: _LL2925: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2935=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2935[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2936; _temp2936.tag= Cyc_TypeSpecifier_tok_tag;
_temp2936.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2937= yyls; struct Cyc_Yyltype*
_temp2939= _temp2937.curr + yylsp_offset; if( _temp2937.base == 0? 1:( _temp2939
< _temp2937.base? 1: _temp2939 >= _temp2937.last_plus_one)){ _throw(
Null_Exception);}* _temp2939;}).first_line,({ struct _tagged_ptr3 _temp2940=
yyls; struct Cyc_Yyltype* _temp2942= _temp2940.curr + yylsp_offset; if(
_temp2940.base == 0? 1:( _temp2942 < _temp2940.base? 1: _temp2942 >= _temp2940.last_plus_one)){
_throw( Null_Exception);}* _temp2942;}).last_line)); _temp2936;}); _temp2935;});
break; case 54: _LL2934: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2944=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2944[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2945; _temp2945.tag=
Cyc_TypeSpecifier_tok_tag; _temp2945.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2946= yyls; struct Cyc_Yyltype*
_temp2948= _temp2946.curr + yylsp_offset; if( _temp2946.base == 0? 1:( _temp2948
< _temp2946.base? 1: _temp2948 >= _temp2946.last_plus_one)){ _throw(
Null_Exception);}* _temp2948;}).first_line,({ struct _tagged_ptr3 _temp2949=
yyls; struct Cyc_Yyltype* _temp2951= _temp2949.curr + yylsp_offset; if(
_temp2949.base == 0? 1:( _temp2951 < _temp2949.base? 1: _temp2951 >= _temp2949.last_plus_one)){
_throw( Null_Exception);}* _temp2951;}).last_line)); _temp2945;}); _temp2944;});
break; case 55: _LL2943: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2953=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2953[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2954; _temp2954.tag=
Cyc_TypeSpecifier_tok_tag; _temp2954.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp2955=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp2955[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp2956; _temp2956.tag=
Cyc_Parse_Short_spec_tag; _temp2956.f1= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2957= yyls; struct Cyc_Yyltype* _temp2959= _temp2957.curr +
yylsp_offset; if( _temp2957.base == 0? 1:( _temp2959 < _temp2957.base? 1:
_temp2959 >= _temp2957.last_plus_one)){ _throw( Null_Exception);}* _temp2959;}).first_line,({
struct _tagged_ptr3 _temp2960= yyls; struct Cyc_Yyltype* _temp2962= _temp2960.curr
+ yylsp_offset; if( _temp2960.base == 0? 1:( _temp2962 < _temp2960.base? 1:
_temp2962 >= _temp2960.last_plus_one)){ _throw( Null_Exception);}* _temp2962;}).last_line);
_temp2956;}); _temp2955;})); _temp2954;}); _temp2953;}); break; case 56: _LL2952:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2964=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2964[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2965; _temp2965.tag=
Cyc_TypeSpecifier_tok_tag; _temp2965.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2966= yyls; struct Cyc_Yyltype*
_temp2968= _temp2966.curr + yylsp_offset; if( _temp2966.base == 0? 1:( _temp2968
< _temp2966.base? 1: _temp2968 >= _temp2966.last_plus_one)){ _throw(
Null_Exception);}* _temp2968;}).first_line,({ struct _tagged_ptr3 _temp2969=
yyls; struct Cyc_Yyltype* _temp2971= _temp2969.curr + yylsp_offset; if(
_temp2969.base == 0? 1:( _temp2971 < _temp2969.base? 1: _temp2971 >= _temp2969.last_plus_one)){
_throw( Null_Exception);}* _temp2971;}).last_line)); _temp2965;}); _temp2964;});
break; case 57: _LL2963: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2973=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2973[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2974; _temp2974.tag=
Cyc_TypeSpecifier_tok_tag; _temp2974.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp2975=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp2975[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp2976; _temp2976.tag= Cyc_Parse_Long_spec_tag;
_temp2976.f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2977= yyls;
struct Cyc_Yyltype* _temp2979= _temp2977.curr + yylsp_offset; if( _temp2977.base
== 0? 1:( _temp2979 < _temp2977.base? 1: _temp2979 >= _temp2977.last_plus_one)){
_throw( Null_Exception);}* _temp2979;}).first_line,({ struct _tagged_ptr3
_temp2980= yyls; struct Cyc_Yyltype* _temp2982= _temp2980.curr + yylsp_offset;
if( _temp2980.base == 0? 1:( _temp2982 < _temp2980.base? 1: _temp2982 >=
_temp2980.last_plus_one)){ _throw( Null_Exception);}* _temp2982;}).last_line);
_temp2976;}); _temp2975;})); _temp2974;}); _temp2973;}); break; case 58: _LL2972:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2984=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2984[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2985; _temp2985.tag=
Cyc_TypeSpecifier_tok_tag; _temp2985.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2986= yyls; struct Cyc_Yyltype*
_temp2988= _temp2986.curr + yylsp_offset; if( _temp2986.base == 0? 1:( _temp2988
< _temp2986.base? 1: _temp2988 >= _temp2986.last_plus_one)){ _throw(
Null_Exception);}* _temp2988;}).first_line,({ struct _tagged_ptr3 _temp2989=
yyls; struct Cyc_Yyltype* _temp2991= _temp2989.curr + yylsp_offset; if(
_temp2989.base == 0? 1:( _temp2991 < _temp2989.base? 1: _temp2991 >= _temp2989.last_plus_one)){
_throw( Null_Exception);}* _temp2991;}).last_line)); _temp2985;}); _temp2984;});
break; case 59: _LL2983: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2993=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2993[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2994; _temp2994.tag=
Cyc_TypeSpecifier_tok_tag; _temp2994.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2995= yyls; struct Cyc_Yyltype*
_temp2997= _temp2995.curr + yylsp_offset; if( _temp2995.base == 0? 1:( _temp2997
< _temp2995.base? 1: _temp2997 >= _temp2995.last_plus_one)){ _throw(
Null_Exception);}* _temp2997;}).first_line,({ struct _tagged_ptr3 _temp2998=
yyls; struct Cyc_Yyltype* _temp3000= _temp2998.curr + yylsp_offset; if(
_temp2998.base == 0? 1:( _temp3000 < _temp2998.base? 1: _temp3000 >= _temp2998.last_plus_one)){
_throw( Null_Exception);}* _temp3000;}).last_line)); _temp2994;}); _temp2993;});
break; case 60: _LL2992: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3002=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3002[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3003; _temp3003.tag=
Cyc_TypeSpecifier_tok_tag; _temp3003.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp3004=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp3004[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp3005; _temp3005.tag=
Cyc_Parse_Signed_spec_tag; _temp3005.f1= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3006= yyls; struct Cyc_Yyltype* _temp3008= _temp3006.curr +
yylsp_offset; if( _temp3006.base == 0? 1:( _temp3008 < _temp3006.base? 1:
_temp3008 >= _temp3006.last_plus_one)){ _throw( Null_Exception);}* _temp3008;}).first_line,({
struct _tagged_ptr3 _temp3009= yyls; struct Cyc_Yyltype* _temp3011= _temp3009.curr
+ yylsp_offset; if( _temp3009.base == 0? 1:( _temp3011 < _temp3009.base? 1:
_temp3011 >= _temp3009.last_plus_one)){ _throw( Null_Exception);}* _temp3011;}).last_line);
_temp3005;}); _temp3004;})); _temp3003;}); _temp3002;}); break; case 61: _LL3001:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3013=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3013[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3014; _temp3014.tag=
Cyc_TypeSpecifier_tok_tag; _temp3014.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp3015=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp3015[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp3016; _temp3016.tag= Cyc_Parse_Unsigned_spec_tag; _temp3016.f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3017= yyls; struct Cyc_Yyltype* _temp3019= _temp3017.curr
+ yylsp_offset; if( _temp3017.base == 0? 1:( _temp3019 < _temp3017.base? 1:
_temp3019 >= _temp3017.last_plus_one)){ _throw( Null_Exception);}* _temp3019;}).first_line,({
struct _tagged_ptr3 _temp3020= yyls; struct Cyc_Yyltype* _temp3022= _temp3020.curr
+ yylsp_offset; if( _temp3020.base == 0? 1:( _temp3022 < _temp3020.base? 1:
_temp3022 >= _temp3020.last_plus_one)){ _throw( Null_Exception);}* _temp3022;}).last_line);
_temp3016;}); _temp3015;})); _temp3014;}); _temp3013;}); break; case 62: _LL3012:
yyval=({ struct _tagged_ptr2 _temp3024= yyvs; struct _xtunion_struct** _temp3026=
_temp3024.curr + yyvsp_offset; if( _temp3024.base == 0? 1:( _temp3026 <
_temp3024.base? 1: _temp3026 >= _temp3024.last_plus_one)){ _throw(
Null_Exception);}* _temp3026;}); break; case 63: _LL3023: yyval=({ struct
_tagged_ptr2 _temp3028= yyvs; struct _xtunion_struct** _temp3030= _temp3028.curr
+ yyvsp_offset; if( _temp3028.base == 0? 1:( _temp3030 < _temp3028.base? 1:
_temp3030 >= _temp3028.last_plus_one)){ _throw( Null_Exception);}* _temp3030;});
break; case 64: _LL3027: yyval=({ struct _tagged_ptr2 _temp3032= yyvs; struct
_xtunion_struct** _temp3034= _temp3032.curr + yyvsp_offset; if( _temp3032.base
== 0? 1:( _temp3034 < _temp3032.base? 1: _temp3034 >= _temp3032.last_plus_one)){
_throw( Null_Exception);}* _temp3034;}); break; case 65: _LL3031: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3036=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3036[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3037; _temp3037.tag= Cyc_TypeSpecifier_tok_tag;
_temp3037.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp3038= yyvs; struct _xtunion_struct** _temp3040=
_temp3038.curr + yyvsp_offset; if( _temp3038.base == 0? 1:( _temp3040 <
_temp3038.base? 1: _temp3040 >= _temp3038.last_plus_one)){ _throw(
Null_Exception);}* _temp3040;})),(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3041= yyls; struct Cyc_Yyltype*
_temp3043= _temp3041.curr + yylsp_offset; if( _temp3041.base == 0? 1:( _temp3043
< _temp3041.base? 1: _temp3043 >= _temp3041.last_plus_one)){ _throw(
Null_Exception);}* _temp3043;}).first_line,({ struct _tagged_ptr3 _temp3044=
yyls; struct Cyc_Yyltype* _temp3046= _temp3044.curr + yylsp_offset; if(
_temp3044.base == 0? 1:( _temp3046 < _temp3044.base? 1: _temp3046 >= _temp3044.last_plus_one)){
_throw( Null_Exception);}* _temp3046;}).last_line)); _temp3037;}); _temp3036;});
break; case 66: _LL3035: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3048=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3048[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3049; _temp3049.tag=
Cyc_TypeSpecifier_tok_tag; _temp3049.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3050= yyvs; struct
_xtunion_struct** _temp3052= _temp3050.curr +( yyvsp_offset - 2); if( _temp3050.base
== 0? 1:( _temp3052 < _temp3050.base? 1: _temp3052 >= _temp3050.last_plus_one)){
_throw( Null_Exception);}* _temp3052;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp3053= yyvs;
struct _xtunion_struct** _temp3055= _temp3053.curr + yyvsp_offset; if( _temp3053.base
== 0? 1:( _temp3055 < _temp3053.base? 1: _temp3055 >= _temp3053.last_plus_one)){
_throw( Null_Exception);}* _temp3055;})))), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3056= yyls; struct Cyc_Yyltype* _temp3058= _temp3056.curr
+( yylsp_offset - 2); if( _temp3056.base == 0? 1:( _temp3058 < _temp3056.base? 1:
_temp3058 >= _temp3056.last_plus_one)){ _throw( Null_Exception);}* _temp3058;}).first_line,({
struct _tagged_ptr3 _temp3059= yyls; struct Cyc_Yyltype* _temp3061= _temp3059.curr
+ yylsp_offset; if( _temp3059.base == 0? 1:( _temp3061 < _temp3059.base? 1:
_temp3061 >= _temp3059.last_plus_one)){ _throw( Null_Exception);}* _temp3061;}).last_line));
_temp3049;}); _temp3048;}); break; case 67: _LL3047: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3063=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3063[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3064; _temp3064.tag= Cyc_TypeSpecifier_tok_tag;
_temp3064.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp3065=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp3065[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp3066; _temp3066.tag=
Cyc_Absyn_TypedefType_tag; _temp3066.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3067= yyvs; struct _xtunion_struct** _temp3069= _temp3067.curr
+( yyvsp_offset - 1); if( _temp3067.base == 0? 1:( _temp3069 < _temp3067.base? 1:
_temp3069 >= _temp3067.last_plus_one)){ _throw( Null_Exception);}* _temp3069;}));
_temp3066.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3070= yyvs;
struct _xtunion_struct** _temp3072= _temp3070.curr + yyvsp_offset; if( _temp3070.base
== 0? 1:( _temp3072 < _temp3070.base? 1: _temp3072 >= _temp3070.last_plus_one)){
_throw( Null_Exception);}* _temp3072;})); _temp3066.f3= 0; _temp3066;});
_temp3065;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3073= yyls;
struct Cyc_Yyltype* _temp3075= _temp3073.curr +( yylsp_offset - 1); if(
_temp3073.base == 0? 1:( _temp3075 < _temp3073.base? 1: _temp3075 >= _temp3073.last_plus_one)){
_throw( Null_Exception);}* _temp3075;}).first_line,({ struct _tagged_ptr3
_temp3076= yyls; struct Cyc_Yyltype* _temp3078= _temp3076.curr + yylsp_offset;
if( _temp3076.base == 0? 1:( _temp3078 < _temp3076.base? 1: _temp3078 >=
_temp3076.last_plus_one)){ _throw( Null_Exception);}* _temp3078;}).last_line));
_temp3064;}); _temp3063;}); break; case 68: _LL3062: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3080=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3080[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3081; _temp3081.tag= Cyc_TypeSpecifier_tok_tag;
_temp3081.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp3082=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp3082[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp3083; _temp3083.tag= Cyc_Absyn_TupleType_tag;
_temp3083.f1=(( struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3084= yyls; struct Cyc_Yyltype* _temp3086= _temp3084.curr +(
yylsp_offset - 1); if( _temp3084.base == 0? 1:( _temp3086 < _temp3084.base? 1:
_temp3086 >= _temp3084.last_plus_one)){ _throw( Null_Exception);}* _temp3086;}).first_line,({
struct _tagged_ptr3 _temp3087= yyls; struct Cyc_Yyltype* _temp3089= _temp3087.curr
+( yylsp_offset - 1); if( _temp3087.base == 0? 1:( _temp3089 < _temp3087.base? 1:
_temp3089 >= _temp3087.last_plus_one)){ _throw( Null_Exception);}* _temp3089;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3090= yyvs; struct _xtunion_struct** _temp3092=
_temp3090.curr +( yyvsp_offset - 1); if( _temp3090.base == 0? 1:( _temp3092 <
_temp3090.base? 1: _temp3092 >= _temp3090.last_plus_one)){ _throw(
Null_Exception);}* _temp3092;})))); _temp3083;}); _temp3082;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3093= yyls; struct Cyc_Yyltype* _temp3095= _temp3093.curr
+( yylsp_offset - 3); if( _temp3093.base == 0? 1:( _temp3095 < _temp3093.base? 1:
_temp3095 >= _temp3093.last_plus_one)){ _throw( Null_Exception);}* _temp3095;}).first_line,({
struct _tagged_ptr3 _temp3096= yyls; struct Cyc_Yyltype* _temp3098= _temp3096.curr
+ yylsp_offset; if( _temp3096.base == 0? 1:( _temp3098 < _temp3096.base? 1:
_temp3098 >= _temp3096.last_plus_one)){ _throw( Null_Exception);}* _temp3098;}).last_line));
_temp3081;}); _temp3080;}); break; case 69: _LL3079: yyval=( struct
_xtunion_struct*)({ struct Cyc_Kind_tok_struct* _temp3100=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp3100[ 0]=({ struct Cyc_Kind_tok_struct
_temp3101; _temp3101.tag= Cyc_Kind_tok_tag; _temp3101.f1=( void*) Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3102= yyvs; struct
_xtunion_struct** _temp3104= _temp3102.curr + yyvsp_offset; if( _temp3102.base
== 0? 1:( _temp3104 < _temp3102.base? 1: _temp3104 >= _temp3102.last_plus_one)){
_throw( Null_Exception);}* _temp3104;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3105= yyls; struct Cyc_Yyltype* _temp3107= _temp3105.curr +
yylsp_offset; if( _temp3105.base == 0? 1:( _temp3107 < _temp3105.base? 1:
_temp3107 >= _temp3105.last_plus_one)){ _throw( Null_Exception);}* _temp3107;}).first_line,({
struct _tagged_ptr3 _temp3108= yyls; struct Cyc_Yyltype* _temp3110= _temp3108.curr
+ yylsp_offset; if( _temp3108.base == 0? 1:( _temp3110 < _temp3108.base? 1:
_temp3110 >= _temp3108.last_plus_one)){ _throw( Null_Exception);}* _temp3110;}).last_line));
_temp3101;}); _temp3100;}); break; case 70: _LL3099: { struct _tagged_string*
_temp3117; void* _temp3119; struct _tuple1 _temp3115=* Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3112= yyvs; struct _xtunion_struct** _temp3114=
_temp3112.curr + yyvsp_offset; if( _temp3112.base == 0? 1:( _temp3114 <
_temp3112.base? 1: _temp3114 >= _temp3112.last_plus_one)){ _throw(
Null_Exception);}* _temp3114;})); _LL3120: _temp3119= _temp3115.f1; goto _LL3118;
_LL3118: _temp3117= _temp3115.f2; goto _LL3116; _LL3116: if( _temp3119 !=( void*)
Cyc_Absyn_Loc_n){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp3121=(
char*)"bad kind in type specifier"; struct _tagged_string _temp3122; _temp3122.curr=
_temp3121; _temp3122.base= _temp3121; _temp3122.last_plus_one= _temp3121 + 27;
_temp3122;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3123= yyls;
struct Cyc_Yyltype* _temp3125= _temp3123.curr + yylsp_offset; if( _temp3123.base
== 0? 1:( _temp3125 < _temp3123.base? 1: _temp3125 >= _temp3123.last_plus_one)){
_throw( Null_Exception);}* _temp3125;}).first_line,({ struct _tagged_ptr3
_temp3126= yyls; struct Cyc_Yyltype* _temp3128= _temp3126.curr + yylsp_offset;
if( _temp3126.base == 0? 1:( _temp3128 < _temp3126.base? 1: _temp3128 >=
_temp3126.last_plus_one)){ _throw( Null_Exception);}* _temp3128;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_Kind_tok_struct* _temp3129=(
struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp3129[ 0]=({ struct Cyc_Kind_tok_struct _temp3130; _temp3130.tag= Cyc_Kind_tok_tag;
_temp3130.f1=( void*) Cyc_Parse_id_to_kind(* _temp3117, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3131= yyls; struct Cyc_Yyltype* _temp3133= _temp3131.curr
+ yylsp_offset; if( _temp3131.base == 0? 1:( _temp3133 < _temp3131.base? 1:
_temp3133 >= _temp3131.last_plus_one)){ _throw( Null_Exception);}* _temp3133;}).first_line,({
struct _tagged_ptr3 _temp3134= yyls; struct Cyc_Yyltype* _temp3136= _temp3134.curr
+ yylsp_offset; if( _temp3134.base == 0? 1:( _temp3136 < _temp3134.base? 1:
_temp3136 >= _temp3134.last_plus_one)){ _throw( Null_Exception);}* _temp3136;}).last_line));
_temp3130;}); _temp3129;}); break;} case 71: _LL3111: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeQual_tok_struct* _temp3138=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp3138[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3139; _temp3139.tag= Cyc_TypeQual_tok_tag; _temp3139.f1=({ struct Cyc_Absyn_Tqual*
_temp3140=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp3140->q_const= 1; _temp3140->q_volatile= 0; _temp3140->q_restrict= 0;
_temp3140;}); _temp3139;}); _temp3138;}); break; case 72: _LL3137: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeQual_tok_struct* _temp3142=( struct
Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp3142[ 0]=({ struct Cyc_TypeQual_tok_struct _temp3143; _temp3143.tag= Cyc_TypeQual_tok_tag;
_temp3143.f1=({ struct Cyc_Absyn_Tqual* _temp3144=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp3144->q_const= 0;
_temp3144->q_volatile= 1; _temp3144->q_restrict= 0; _temp3144;}); _temp3143;});
_temp3142;}); break; case 73: _LL3141: yyval=( struct _xtunion_struct*)({ struct
Cyc_TypeQual_tok_struct* _temp3146=( struct Cyc_TypeQual_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeQual_tok_struct)); _temp3146[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3147; _temp3147.tag= Cyc_TypeQual_tok_tag; _temp3147.f1=({ struct Cyc_Absyn_Tqual*
_temp3148=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp3148->q_const= 0; _temp3148->q_volatile= 0; _temp3148->q_restrict= 1;
_temp3148;}); _temp3147;}); _temp3146;}); break; case 74: _LL3145: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3150=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3150[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3151; _temp3151.tag=
Cyc_TypeSpecifier_tok_tag; _temp3151.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3152=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3152[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3153; _temp3153.tag= Cyc_Parse_Decl_spec_tag;
_temp3153.f1=({ struct Cyc_Absyn_Decl* _temp3154=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp3154->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp3161=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp3161[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp3162; _temp3162.tag= Cyc_Absyn_Enum_d_tag; _temp3162.f1=({ struct Cyc_Absyn_Enumdecl*
_temp3163=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp3163->sc=( void*)(( void*) Cyc_Absyn_Public); _temp3163->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3167= yyvs; struct _xtunion_struct** _temp3169=
_temp3167.curr +( yyvsp_offset - 3); if( _temp3167.base == 0? 1:( _temp3169 <
_temp3167.base? 1: _temp3169 >= _temp3167.last_plus_one)){ _throw(
Null_Exception);}* _temp3169;})); _temp3163->fields= Cyc_yyget_EnumfieldList_tok(({
struct _tagged_ptr2 _temp3164= yyvs; struct _xtunion_struct** _temp3166=
_temp3164.curr +( yyvsp_offset - 1); if( _temp3164.base == 0? 1:( _temp3166 <
_temp3164.base? 1: _temp3166 >= _temp3164.last_plus_one)){ _throw(
Null_Exception);}* _temp3166;})); _temp3163;}); _temp3162;}); _temp3161;}));
_temp3154->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3155=
yyls; struct Cyc_Yyltype* _temp3157= _temp3155.curr +( yylsp_offset - 4); if(
_temp3155.base == 0? 1:( _temp3157 < _temp3155.base? 1: _temp3157 >= _temp3155.last_plus_one)){
_throw( Null_Exception);}* _temp3157;}).first_line,({ struct _tagged_ptr3
_temp3158= yyls; struct Cyc_Yyltype* _temp3160= _temp3158.curr + yylsp_offset;
if( _temp3158.base == 0? 1:( _temp3160 < _temp3158.base? 1: _temp3160 >=
_temp3158.last_plus_one)){ _throw( Null_Exception);}* _temp3160;}).last_line);
_temp3154;}); _temp3153;}); _temp3152;})); _temp3151;}); _temp3150;}); break;
case 75: _LL3149: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3171=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3171[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3172; _temp3172.tag=
Cyc_TypeSpecifier_tok_tag; _temp3172.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp3173=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp3173[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp3174; _temp3174.tag= Cyc_Absyn_EnumType_tag;
_temp3174.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3175= yyvs;
struct _xtunion_struct** _temp3177= _temp3175.curr + yyvsp_offset; if( _temp3175.base
== 0? 1:( _temp3177 < _temp3175.base? 1: _temp3177 >= _temp3175.last_plus_one)){
_throw( Null_Exception);}* _temp3177;})); _temp3174.f2= 0; _temp3174;});
_temp3173;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3178= yyls;
struct Cyc_Yyltype* _temp3180= _temp3178.curr +( yylsp_offset - 1); if(
_temp3178.base == 0? 1:( _temp3180 < _temp3178.base? 1: _temp3180 >= _temp3178.last_plus_one)){
_throw( Null_Exception);}* _temp3180;}).first_line,({ struct _tagged_ptr3
_temp3181= yyls; struct Cyc_Yyltype* _temp3183= _temp3181.curr + yylsp_offset;
if( _temp3181.base == 0? 1:( _temp3183 < _temp3181.base? 1: _temp3183 >=
_temp3181.last_plus_one)){ _throw( Null_Exception);}* _temp3183;}).last_line));
_temp3172;}); _temp3171;}); break; case 76: _LL3170: yyval=( struct
_xtunion_struct*)({ struct Cyc_Enumfield_tok_struct* _temp3185=( struct Cyc_Enumfield_tok_struct*)
GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct)); _temp3185[ 0]=({ struct
Cyc_Enumfield_tok_struct _temp3186; _temp3186.tag= Cyc_Enumfield_tok_tag;
_temp3186.f1=({ struct Cyc_Absyn_Enumfield* _temp3187=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3187->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3194= yyvs; struct _xtunion_struct** _temp3196=
_temp3194.curr + yyvsp_offset; if( _temp3194.base == 0? 1:( _temp3196 <
_temp3194.base? 1: _temp3196 >= _temp3194.last_plus_one)){ _throw(
Null_Exception);}* _temp3196;})); _temp3187->tag= 0; _temp3187->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3188= yyls; struct Cyc_Yyltype* _temp3190= _temp3188.curr
+ yylsp_offset; if( _temp3188.base == 0? 1:( _temp3190 < _temp3188.base? 1:
_temp3190 >= _temp3188.last_plus_one)){ _throw( Null_Exception);}* _temp3190;}).first_line,({
struct _tagged_ptr3 _temp3191= yyls; struct Cyc_Yyltype* _temp3193= _temp3191.curr
+ yylsp_offset; if( _temp3191.base == 0? 1:( _temp3193 < _temp3191.base? 1:
_temp3193 >= _temp3191.last_plus_one)){ _throw( Null_Exception);}* _temp3193;}).last_line);
_temp3187;}); _temp3186;}); _temp3185;}); break; case 77: _LL3184: yyval=(
struct _xtunion_struct*)({ struct Cyc_Enumfield_tok_struct* _temp3198=( struct
Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp3198[ 0]=({ struct Cyc_Enumfield_tok_struct _temp3199; _temp3199.tag= Cyc_Enumfield_tok_tag;
_temp3199.f1=({ struct Cyc_Absyn_Enumfield* _temp3200=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3200->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3210= yyvs; struct _xtunion_struct** _temp3212=
_temp3210.curr +( yyvsp_offset - 2); if( _temp3210.base == 0? 1:( _temp3212 <
_temp3210.base? 1: _temp3212 >= _temp3210.last_plus_one)){ _throw(
Null_Exception);}* _temp3212;})); _temp3200->tag=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3207= yyvs; struct _xtunion_struct** _temp3209=
_temp3207.curr + yyvsp_offset; if( _temp3207.base == 0? 1:( _temp3209 <
_temp3207.base? 1: _temp3209 >= _temp3207.last_plus_one)){ _throw(
Null_Exception);}* _temp3209;})); _temp3200->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3201= yyls; struct Cyc_Yyltype* _temp3203= _temp3201.curr
+( yylsp_offset - 2); if( _temp3201.base == 0? 1:( _temp3203 < _temp3201.base? 1:
_temp3203 >= _temp3201.last_plus_one)){ _throw( Null_Exception);}* _temp3203;}).first_line,({
struct _tagged_ptr3 _temp3204= yyls; struct Cyc_Yyltype* _temp3206= _temp3204.curr
+ yylsp_offset; if( _temp3204.base == 0? 1:( _temp3206 < _temp3204.base? 1:
_temp3206 >= _temp3204.last_plus_one)){ _throw( Null_Exception);}* _temp3206;}).last_line);
_temp3200;}); _temp3199;}); _temp3198;}); break; case 78: _LL3197: yyval=(
struct _xtunion_struct*)({ struct Cyc_EnumfieldList_tok_struct* _temp3214=(
struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp3214[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp3215; _temp3215.tag=
Cyc_EnumfieldList_tok_tag; _temp3215.f1=({ struct Cyc_List_List* _temp3216=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3216->hd=(
void*) Cyc_yyget_Enumfield_tok(({ struct _tagged_ptr2 _temp3217= yyvs; struct
_xtunion_struct** _temp3219= _temp3217.curr + yyvsp_offset; if( _temp3217.base
== 0? 1:( _temp3219 < _temp3217.base? 1: _temp3219 >= _temp3217.last_plus_one)){
_throw( Null_Exception);}* _temp3219;})); _temp3216->tl= 0; _temp3216;});
_temp3215;}); _temp3214;}); break; case 79: _LL3213: yyval=( struct
_xtunion_struct*)({ struct Cyc_EnumfieldList_tok_struct* _temp3221=( struct Cyc_EnumfieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct)); _temp3221[ 0]=({
struct Cyc_EnumfieldList_tok_struct _temp3222; _temp3222.tag= Cyc_EnumfieldList_tok_tag;
_temp3222.f1=({ struct Cyc_List_List* _temp3223=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3223->hd=( void*) Cyc_yyget_Enumfield_tok(({
struct _tagged_ptr2 _temp3227= yyvs; struct _xtunion_struct** _temp3229=
_temp3227.curr +( yyvsp_offset - 2); if( _temp3227.base == 0? 1:( _temp3229 <
_temp3227.base? 1: _temp3229 >= _temp3227.last_plus_one)){ _throw(
Null_Exception);}* _temp3229;})); _temp3223->tl= Cyc_yyget_EnumfieldList_tok(({
struct _tagged_ptr2 _temp3224= yyvs; struct _xtunion_struct** _temp3226=
_temp3224.curr + yyvsp_offset; if( _temp3224.base == 0? 1:( _temp3226 <
_temp3224.base? 1: _temp3226 >= _temp3224.last_plus_one)){ _throw(
Null_Exception);}* _temp3226;})); _temp3223;}); _temp3222;}); _temp3221;});
break; case 80: _LL3220: { struct Cyc_Absyn_Decl* d;{ void* _temp3234= Cyc_yyget_StructOrUnion_tok(({
struct _tagged_ptr2 _temp3231= yyvs; struct _xtunion_struct** _temp3233=
_temp3231.curr +( yyvsp_offset - 3); if( _temp3231.base == 0? 1:( _temp3233 <
_temp3231.base? 1: _temp3233 >= _temp3231.last_plus_one)){ _throw(
Null_Exception);}* _temp3233;})); _LL3236: if(( int) _temp3234 == Cyc_Parse_Struct_su){
goto _LL3237;} else{ goto _LL3238;} _LL3238: if(( int) _temp3234 == Cyc_Parse_Union_su){
goto _LL3239;} else{ goto _LL3235;} _LL3237: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public, 0, 0,({ struct Cyc_Core_Opt* _temp3240=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3240->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3241= yyvs; struct _xtunion_struct** _temp3243=
_temp3241.curr +( yyvsp_offset - 1); if( _temp3241.base == 0? 1:( _temp3243 <
_temp3241.base? 1: _temp3243 >= _temp3241.last_plus_one)){ _throw(
Null_Exception);}* _temp3243;})); _temp3240;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3244= yyls; struct Cyc_Yyltype* _temp3246= _temp3244.curr
+( yylsp_offset - 3); if( _temp3244.base == 0? 1:( _temp3246 < _temp3244.base? 1:
_temp3246 >= _temp3244.last_plus_one)){ _throw( Null_Exception);}* _temp3246;}).first_line,({
struct _tagged_ptr3 _temp3247= yyls; struct Cyc_Yyltype* _temp3249= _temp3247.curr
+ yylsp_offset; if( _temp3247.base == 0? 1:( _temp3249 < _temp3247.base? 1:
_temp3249 >= _temp3247.last_plus_one)){ _throw( Null_Exception);}* _temp3249;}).last_line));
goto _LL3235; _LL3239: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public, 0, 0,({
struct Cyc_Core_Opt* _temp3250=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3250->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3251= yyvs; struct _xtunion_struct** _temp3253=
_temp3251.curr +( yyvsp_offset - 1); if( _temp3251.base == 0? 1:( _temp3253 <
_temp3251.base? 1: _temp3253 >= _temp3251.last_plus_one)){ _throw(
Null_Exception);}* _temp3253;})); _temp3250;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3254= yyls; struct Cyc_Yyltype* _temp3256= _temp3254.curr
+( yylsp_offset - 3); if( _temp3254.base == 0? 1:( _temp3256 < _temp3254.base? 1:
_temp3256 >= _temp3254.last_plus_one)){ _throw( Null_Exception);}* _temp3256;}).first_line,({
struct _tagged_ptr3 _temp3257= yyls; struct Cyc_Yyltype* _temp3259= _temp3257.curr
+ yylsp_offset; if( _temp3257.base == 0? 1:( _temp3259 < _temp3257.base? 1:
_temp3259 >= _temp3257.last_plus_one)){ _throw( Null_Exception);}* _temp3259;}).last_line));
goto _LL3235; _LL3235:;} yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3260=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3260[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3261; _temp3261.tag=
Cyc_TypeSpecifier_tok_tag; _temp3261.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3262=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3262[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3263; _temp3263.tag= Cyc_Parse_Decl_spec_tag;
_temp3263.f1= d; _temp3263;}); _temp3262;})); _temp3261;}); _temp3260;}); Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp3264=( char*)"anonymous structs/unions";
struct _tagged_string _temp3265; _temp3265.curr= _temp3264; _temp3265.base=
_temp3264; _temp3265.last_plus_one= _temp3264 + 25; _temp3265;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3266= yyls; struct Cyc_Yyltype* _temp3268= _temp3266.curr
+( yylsp_offset - 3); if( _temp3266.base == 0? 1:( _temp3268 < _temp3266.base? 1:
_temp3268 >= _temp3266.last_plus_one)){ _throw( Null_Exception);}* _temp3268;}).first_line,({
struct _tagged_ptr3 _temp3269= yyls; struct Cyc_Yyltype* _temp3271= _temp3269.curr
+ yylsp_offset; if( _temp3269.base == 0? 1:( _temp3271 < _temp3269.base? 1:
_temp3271 >= _temp3269.last_plus_one)){ _throw( Null_Exception);}* _temp3271;}).last_line));
break;} case 81: _LL3230: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3273= yyls; struct Cyc_Yyltype* _temp3275= _temp3273.curr
+( yylsp_offset - 3); if( _temp3273.base == 0? 1:( _temp3275 < _temp3273.base? 1:
_temp3275 >= _temp3273.last_plus_one)){ _throw( Null_Exception);}* _temp3275;}).first_line,({
struct _tagged_ptr3 _temp3276= yyls; struct Cyc_Yyltype* _temp3278= _temp3276.curr
+( yylsp_offset - 3); if( _temp3276.base == 0? 1:( _temp3278 < _temp3276.base? 1:
_temp3278 >= _temp3276.last_plus_one)){ _throw( Null_Exception);}* _temp3278;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3279= yyvs; struct
_xtunion_struct** _temp3281= _temp3279.curr +( yyvsp_offset - 3); if( _temp3279.base
== 0? 1:( _temp3281 < _temp3279.base? 1: _temp3281 >= _temp3279.last_plus_one)){
_throw( Null_Exception);}* _temp3281;}))); struct Cyc_Absyn_Decl* d;{ void*
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
case 82: _LL3272: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3324= yyls; struct Cyc_Yyltype* _temp3326= _temp3324.curr
+( yylsp_offset - 3); if( _temp3324.base == 0? 1:( _temp3326 < _temp3324.base? 1:
_temp3326 >= _temp3324.last_plus_one)){ _throw( Null_Exception);}* _temp3326;}).first_line,({
struct _tagged_ptr3 _temp3327= yyls; struct Cyc_Yyltype* _temp3329= _temp3327.curr
+( yylsp_offset - 3); if( _temp3327.base == 0? 1:( _temp3329 < _temp3327.base? 1:
_temp3329 >= _temp3327.last_plus_one)){ _throw( Null_Exception);}* _temp3329;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3330= yyvs; struct
_xtunion_struct** _temp3332= _temp3330.curr +( yyvsp_offset - 3); if( _temp3330.base
== 0? 1:( _temp3332 < _temp3330.base? 1: _temp3332 >= _temp3330.last_plus_one)){
_throw( Null_Exception);}* _temp3332;}))); struct Cyc_Absyn_Decl* d;{ void*
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
case 83: _LL3323:{ void* _temp3378= Cyc_yyget_StructOrUnion_tok(({ struct
_tagged_ptr2 _temp3375= yyvs; struct _xtunion_struct** _temp3377= _temp3375.curr
+( yyvsp_offset - 2); if( _temp3375.base == 0? 1:( _temp3377 < _temp3375.base? 1:
_temp3377 >= _temp3375.last_plus_one)){ _throw( Null_Exception);}* _temp3377;}));
_LL3380: if(( int) _temp3378 == Cyc_Parse_Struct_su){ goto _LL3381;} else{ goto
_LL3382;} _LL3382: if(( int) _temp3378 == Cyc_Parse_Union_su){ goto _LL3383;}
else{ goto _LL3379;} _LL3381: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3384=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3384[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3385; _temp3385.tag=
Cyc_TypeSpecifier_tok_tag; _temp3385.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp3386=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3386[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp3387; _temp3387.tag= Cyc_Absyn_StructType_tag;
_temp3387.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3388= yyvs; struct _xtunion_struct** _temp3390= _temp3388.curr +(
yyvsp_offset - 1); if( _temp3388.base == 0? 1:( _temp3390 < _temp3388.base? 1:
_temp3390 >= _temp3388.last_plus_one)){ _throw( Null_Exception);}* _temp3390;}));
_temp3387.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3391= yyvs;
struct _xtunion_struct** _temp3393= _temp3391.curr + yyvsp_offset; if( _temp3391.base
== 0? 1:( _temp3393 < _temp3391.base? 1: _temp3393 >= _temp3391.last_plus_one)){
_throw( Null_Exception);}* _temp3393;})); _temp3387.f3= 0; _temp3387;});
_temp3386;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3394= yyls;
struct Cyc_Yyltype* _temp3396= _temp3394.curr +( yylsp_offset - 2); if(
_temp3394.base == 0? 1:( _temp3396 < _temp3394.base? 1: _temp3396 >= _temp3394.last_plus_one)){
_throw( Null_Exception);}* _temp3396;}).first_line,({ struct _tagged_ptr3
_temp3397= yyls; struct Cyc_Yyltype* _temp3399= _temp3397.curr + yylsp_offset;
if( _temp3397.base == 0? 1:( _temp3399 < _temp3397.base? 1: _temp3399 >=
_temp3397.last_plus_one)){ _throw( Null_Exception);}* _temp3399;}).last_line));
_temp3385;}); _temp3384;}); goto _LL3379; _LL3383: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3400=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3400[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3401; _temp3401.tag= Cyc_TypeSpecifier_tok_tag;
_temp3401.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp3402=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp3402[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp3403; _temp3403.tag= Cyc_Absyn_UnionType_tag;
_temp3403.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3404= yyvs; struct _xtunion_struct** _temp3406= _temp3404.curr +(
yyvsp_offset - 1); if( _temp3404.base == 0? 1:( _temp3406 < _temp3404.base? 1:
_temp3406 >= _temp3404.last_plus_one)){ _throw( Null_Exception);}* _temp3406;}));
_temp3403.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3407= yyvs;
struct _xtunion_struct** _temp3409= _temp3407.curr + yyvsp_offset; if( _temp3407.base
== 0? 1:( _temp3409 < _temp3407.base? 1: _temp3409 >= _temp3407.last_plus_one)){
_throw( Null_Exception);}* _temp3409;})); _temp3403.f3= 0; _temp3403;});
_temp3402;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3410= yyls;
struct Cyc_Yyltype* _temp3412= _temp3410.curr +( yylsp_offset - 2); if(
_temp3410.base == 0? 1:( _temp3412 < _temp3410.base? 1: _temp3412 >= _temp3410.last_plus_one)){
_throw( Null_Exception);}* _temp3412;}).first_line,({ struct _tagged_ptr3
_temp3413= yyls; struct Cyc_Yyltype* _temp3415= _temp3413.curr + yylsp_offset;
if( _temp3413.base == 0? 1:( _temp3415 < _temp3413.base? 1: _temp3415 >=
_temp3413.last_plus_one)){ _throw( Null_Exception);}* _temp3415;}).last_line));
_temp3401;}); _temp3400;}); goto _LL3379; _LL3379:;} break; case 84: _LL3374:{
void* _temp3420= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3417=
yyvs; struct _xtunion_struct** _temp3419= _temp3417.curr +( yyvsp_offset - 2);
if( _temp3417.base == 0? 1:( _temp3419 < _temp3417.base? 1: _temp3419 >=
_temp3417.last_plus_one)){ _throw( Null_Exception);}* _temp3419;})); _LL3422:
if(( int) _temp3420 == Cyc_Parse_Struct_su){ goto _LL3423;} else{ goto _LL3424;}
_LL3424: if(( int) _temp3420 == Cyc_Parse_Union_su){ goto _LL3425;} else{ goto
_LL3421;} _LL3423: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3426=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3426[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3427; _temp3427.tag=
Cyc_TypeSpecifier_tok_tag; _temp3427.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp3428=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3428[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp3429; _temp3429.tag= Cyc_Absyn_StructType_tag;
_temp3429.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3430= yyvs; struct _xtunion_struct** _temp3432= _temp3430.curr +(
yyvsp_offset - 1); if( _temp3430.base == 0? 1:( _temp3432 < _temp3430.base? 1:
_temp3432 >= _temp3430.last_plus_one)){ _throw( Null_Exception);}* _temp3432;}));
_temp3429.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3433= yyvs;
struct _xtunion_struct** _temp3435= _temp3433.curr + yyvsp_offset; if( _temp3433.base
== 0? 1:( _temp3435 < _temp3433.base? 1: _temp3435 >= _temp3433.last_plus_one)){
_throw( Null_Exception);}* _temp3435;})); _temp3429.f3= 0; _temp3429;});
_temp3428;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3436= yyls;
struct Cyc_Yyltype* _temp3438= _temp3436.curr +( yylsp_offset - 2); if(
_temp3436.base == 0? 1:( _temp3438 < _temp3436.base? 1: _temp3438 >= _temp3436.last_plus_one)){
_throw( Null_Exception);}* _temp3438;}).first_line,({ struct _tagged_ptr3
_temp3439= yyls; struct Cyc_Yyltype* _temp3441= _temp3439.curr + yylsp_offset;
if( _temp3439.base == 0? 1:( _temp3441 < _temp3439.base? 1: _temp3441 >=
_temp3439.last_plus_one)){ _throw( Null_Exception);}* _temp3441;}).last_line));
_temp3427;}); _temp3426;}); goto _LL3421; _LL3425: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3442=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3442[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3443; _temp3443.tag= Cyc_TypeSpecifier_tok_tag;
_temp3443.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp3444=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp3444[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp3445; _temp3445.tag= Cyc_Absyn_UnionType_tag;
_temp3445.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3446= yyvs; struct _xtunion_struct** _temp3448= _temp3446.curr +(
yyvsp_offset - 1); if( _temp3446.base == 0? 1:( _temp3448 < _temp3446.base? 1:
_temp3448 >= _temp3446.last_plus_one)){ _throw( Null_Exception);}* _temp3448;}));
_temp3445.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3449= yyvs;
struct _xtunion_struct** _temp3451= _temp3449.curr + yyvsp_offset; if( _temp3449.base
== 0? 1:( _temp3451 < _temp3449.base? 1: _temp3451 >= _temp3449.last_plus_one)){
_throw( Null_Exception);}* _temp3451;})); _temp3445.f3= 0; _temp3445;});
_temp3444;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3452= yyls;
struct Cyc_Yyltype* _temp3454= _temp3452.curr +( yylsp_offset - 2); if(
_temp3452.base == 0? 1:( _temp3454 < _temp3452.base? 1: _temp3454 >= _temp3452.last_plus_one)){
_throw( Null_Exception);}* _temp3454;}).first_line,({ struct _tagged_ptr3
_temp3455= yyls; struct Cyc_Yyltype* _temp3457= _temp3455.curr + yylsp_offset;
if( _temp3455.base == 0? 1:( _temp3457 < _temp3455.base? 1: _temp3457 >=
_temp3455.last_plus_one)){ _throw( Null_Exception);}* _temp3457;}).last_line));
_temp3443;}); _temp3442;}); goto _LL3421; _LL3421:;} break; case 85: _LL3416:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp3459=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3459[ 0]=({ struct Cyc_TypeList_tok_struct _temp3460; _temp3460.tag= Cyc_TypeList_tok_tag;
_temp3460.f1= 0; _temp3460;}); _temp3459;}); break; case 86: _LL3458: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp3462=( struct
Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3462[ 0]=({ struct Cyc_TypeList_tok_struct _temp3463; _temp3463.tag= Cyc_TypeList_tok_tag;
_temp3463.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3464= yyvs; struct
_xtunion_struct** _temp3466= _temp3464.curr +( yyvsp_offset - 1); if( _temp3464.base
== 0? 1:( _temp3466 < _temp3464.base? 1: _temp3466 >= _temp3464.last_plus_one)){
_throw( Null_Exception);}* _temp3466;}))); _temp3463;}); _temp3462;}); break;
case 87: _LL3461:(( struct Cyc_Lexing_lexbuf*)({ struct Cyc_Core_Opt* _temp3468=
Cyc_Parse_lbuf; if( _temp3468 == 0){ _throw( Null_Exception);} _temp3468->v;}))->lex_curr_pos
-= 1; yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct*
_temp3469=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3469[ 0]=({ struct Cyc_TypeList_tok_struct _temp3470; _temp3470.tag= Cyc_TypeList_tok_tag;
_temp3470.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3471= yyvs; struct
_xtunion_struct** _temp3473= _temp3471.curr +( yyvsp_offset - 1); if( _temp3471.base
== 0? 1:( _temp3473 < _temp3471.base? 1: _temp3473 >= _temp3471.last_plus_one)){
_throw( Null_Exception);}* _temp3473;}))); _temp3470;}); _temp3469;}); break;
case 88: _LL3467: yyval=( struct _xtunion_struct*)({ struct Cyc_StructOrUnion_tok_struct*
_temp3475=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp3475[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp3476; _temp3476.tag=
Cyc_StructOrUnion_tok_tag; _temp3476.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp3476;}); _temp3475;}); break; case 89: _LL3474: yyval=( struct
_xtunion_struct*)({ struct Cyc_StructOrUnion_tok_struct* _temp3478=( struct Cyc_StructOrUnion_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct)); _temp3478[ 0]=({
struct Cyc_StructOrUnion_tok_struct _temp3479; _temp3479.tag= Cyc_StructOrUnion_tok_tag;
_temp3479.f1=( void*)(( void*) Cyc_Parse_Union_su); _temp3479;}); _temp3478;});
break; case 90: _LL3477: yyval=( struct _xtunion_struct*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3481=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp3481[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp3482; _temp3482.tag= Cyc_StructFieldDeclList_tok_tag; _temp3482.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3483= yyvs; struct _xtunion_struct** _temp3485=
_temp3483.curr + yyvsp_offset; if( _temp3483.base == 0? 1:( _temp3485 <
_temp3483.base? 1: _temp3485 >= _temp3483.last_plus_one)){ _throw(
Null_Exception);}* _temp3485;})))); _temp3482;}); _temp3481;}); break; case 91:
_LL3480: yyval=( struct _xtunion_struct*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3487=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp3487[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp3488; _temp3488.tag= Cyc_StructFieldDeclListList_tok_tag; _temp3488.f1=({
struct Cyc_List_List* _temp3489=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3489->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3490= yyvs; struct _xtunion_struct** _temp3492=
_temp3490.curr + yyvsp_offset; if( _temp3490.base == 0? 1:( _temp3492 <
_temp3490.base? 1: _temp3492 >= _temp3490.last_plus_one)){ _throw(
Null_Exception);}* _temp3492;})); _temp3489->tl= 0; _temp3489;}); _temp3488;});
_temp3487;}); break; case 92: _LL3486: yyval=( struct _xtunion_struct*)({ struct
Cyc_StructFieldDeclListList_tok_struct* _temp3494=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp3494[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp3495; _temp3495.tag= Cyc_StructFieldDeclListList_tok_tag;
_temp3495.f1=({ struct Cyc_List_List* _temp3496=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3496->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3500= yyvs; struct _xtunion_struct** _temp3502=
_temp3500.curr + yyvsp_offset; if( _temp3500.base == 0? 1:( _temp3502 <
_temp3500.base? 1: _temp3502 >= _temp3500.last_plus_one)){ _throw(
Null_Exception);}* _temp3502;})); _temp3496->tl= Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3497= yyvs; struct _xtunion_struct** _temp3499=
_temp3497.curr +( yyvsp_offset - 1); if( _temp3497.base == 0? 1:( _temp3499 <
_temp3497.base? 1: _temp3499 >= _temp3497.last_plus_one)){ _throw(
Null_Exception);}* _temp3499;})); _temp3496;}); _temp3495;}); _temp3494;});
break; case 93: _LL3493: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDeclList_tok_struct*
_temp3504=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3504[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp3505; _temp3505.tag=
Cyc_InitDeclList_tok_tag; _temp3505.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(({ struct _tagged_ptr2
_temp3506= yyvs; struct _xtunion_struct** _temp3508= _temp3506.curr +
yyvsp_offset; if( _temp3506.base == 0? 1:( _temp3508 < _temp3506.base? 1:
_temp3508 >= _temp3506.last_plus_one)){ _throw( Null_Exception);}* _temp3508;})));
_temp3505;}); _temp3504;}); break; case 94: _LL3503: yyval=( struct
_xtunion_struct*)({ struct Cyc_InitDeclList_tok_struct* _temp3510=( struct Cyc_InitDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp3510[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp3511; _temp3511.tag= Cyc_InitDeclList_tok_tag;
_temp3511.f1=({ struct Cyc_List_List* _temp3512=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3512->hd=( void*) Cyc_yyget_InitDecl_tok(({
struct _tagged_ptr2 _temp3513= yyvs; struct _xtunion_struct** _temp3515=
_temp3513.curr + yyvsp_offset; if( _temp3513.base == 0? 1:( _temp3515 <
_temp3513.base? 1: _temp3515 >= _temp3513.last_plus_one)){ _throw(
Null_Exception);}* _temp3515;})); _temp3512->tl= 0; _temp3512;}); _temp3511;});
_temp3510;}); break; case 95: _LL3509: yyval=( struct _xtunion_struct*)({ struct
Cyc_InitDeclList_tok_struct* _temp3517=( struct Cyc_InitDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp3517[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp3518; _temp3518.tag= Cyc_InitDeclList_tok_tag;
_temp3518.f1=({ struct Cyc_List_List* _temp3519=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3519->hd=( void*) Cyc_yyget_InitDecl_tok(({
struct _tagged_ptr2 _temp3523= yyvs; struct _xtunion_struct** _temp3525=
_temp3523.curr + yyvsp_offset; if( _temp3523.base == 0? 1:( _temp3525 <
_temp3523.base? 1: _temp3525 >= _temp3523.last_plus_one)){ _throw(
Null_Exception);}* _temp3525;})); _temp3519->tl= Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp3520= yyvs; struct _xtunion_struct** _temp3522=
_temp3520.curr +( yyvsp_offset - 2); if( _temp3520.base == 0? 1:( _temp3522 <
_temp3520.base? 1: _temp3522 >= _temp3520.last_plus_one)){ _throw(
Null_Exception);}* _temp3522;})); _temp3519;}); _temp3518;}); _temp3517;});
break; case 96: _LL3516: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDecl_tok_struct*
_temp3527=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp3527[ 0]=({ struct Cyc_InitDecl_tok_struct _temp3528; _temp3528.tag= Cyc_InitDecl_tok_tag;
_temp3528.f1=({ struct _tuple13* _temp3529=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp3529->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3530= yyvs; struct _xtunion_struct** _temp3532= _temp3530.curr
+ yyvsp_offset; if( _temp3530.base == 0? 1:( _temp3532 < _temp3530.base? 1:
_temp3532 >= _temp3530.last_plus_one)){ _throw( Null_Exception);}* _temp3532;}));
_temp3529->f2= 0; _temp3529;}); _temp3528;}); _temp3527;}); break; case 97:
_LL3526: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDecl_tok_struct*
_temp3534=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp3534[ 0]=({ struct Cyc_InitDecl_tok_struct _temp3535; _temp3535.tag= Cyc_InitDecl_tok_tag;
_temp3535.f1=({ struct _tuple13* _temp3536=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp3536->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3540= yyvs; struct _xtunion_struct** _temp3542= _temp3540.curr
+( yyvsp_offset - 2); if( _temp3540.base == 0? 1:( _temp3542 < _temp3540.base? 1:
_temp3542 >= _temp3540.last_plus_one)){ _throw( Null_Exception);}* _temp3542;}));
_temp3536->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp3537= yyvs; struct _xtunion_struct** _temp3539= _temp3537.curr +
yyvsp_offset; if( _temp3537.base == 0? 1:( _temp3539 < _temp3537.base? 1:
_temp3539 >= _temp3537.last_plus_one)){ _throw( Null_Exception);}* _temp3539;}));
_temp3536;}); _temp3535;}); _temp3534;}); break; case 98: _LL3533: { struct Cyc_Absyn_Tqual*
tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3544= yyvs; struct
_xtunion_struct** _temp3546= _temp3544.curr +( yyvsp_offset - 2); if( _temp3544.base
== 0? 1:( _temp3546 < _temp3544.base? 1: _temp3546 >= _temp3544.last_plus_one)){
_throw( Null_Exception);}* _temp3546;}))).f1; struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3547= yyvs; struct _xtunion_struct** _temp3549=
_temp3547.curr +( yyvsp_offset - 2); if( _temp3547.base == 0? 1:( _temp3549 <
_temp3547.base? 1: _temp3549 >= _temp3547.last_plus_one)){ _throw(
Null_Exception);}* _temp3549;}))).f3; void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3550= yyvs; struct _xtunion_struct** _temp3552=
_temp3550.curr +( yyvsp_offset - 2); if( _temp3550.base == 0? 1:( _temp3552 <
_temp3550.base? 1: _temp3552 >= _temp3550.last_plus_one)){ _throw(
Null_Exception);}* _temp3552;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3553= yyls; struct Cyc_Yyltype* _temp3555= _temp3553.curr +(
yylsp_offset - 2); if( _temp3553.base == 0? 1:( _temp3555 < _temp3553.base? 1:
_temp3555 >= _temp3553.last_plus_one)){ _throw( Null_Exception);}* _temp3555;}).first_line,({
struct _tagged_ptr3 _temp3556= yyls; struct Cyc_Yyltype* _temp3558= _temp3556.curr
+( yylsp_offset - 2); if( _temp3556.base == 0? 1:( _temp3558 < _temp3556.base? 1:
_temp3558 >= _temp3556.last_plus_one)){ _throw( Null_Exception);}* _temp3558;}).last_line));
struct Cyc_List_List* _temp3564; struct Cyc_List_List* _temp3566; struct _tuple0
_temp3562=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3559= yyvs; struct _xtunion_struct** _temp3561=
_temp3559.curr +( yyvsp_offset - 1); if( _temp3559.base == 0? 1:( _temp3561 <
_temp3559.base? 1: _temp3561 >= _temp3559.last_plus_one)){ _throw(
Null_Exception);}* _temp3561;}))); _LL3567: _temp3566= _temp3562.f1; goto
_LL3565; _LL3565: _temp3564= _temp3562.f2; goto _LL3563; _LL3563: { struct Cyc_List_List*
info=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( Cyc_Parse_apply_tmss( tq, t, _temp3566, atts), _temp3564);
yyval=( struct _xtunion_struct*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3568=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp3568[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp3569; _temp3569.tag= Cyc_StructFieldDeclList_tok_tag; _temp3569.f1=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple8*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3570= yyls; struct Cyc_Yyltype* _temp3572= _temp3570.curr
+( yylsp_offset - 2); if( _temp3570.base == 0? 1:( _temp3572 < _temp3570.base? 1:
_temp3572 >= _temp3570.last_plus_one)){ _throw( Null_Exception);}* _temp3572;}).first_line,({
struct _tagged_ptr3 _temp3573= yyls; struct Cyc_Yyltype* _temp3575= _temp3573.curr
+( yylsp_offset - 1); if( _temp3573.base == 0? 1:( _temp3575 < _temp3573.base? 1:
_temp3575 >= _temp3573.last_plus_one)){ _throw( Null_Exception);}* _temp3575;}).last_line),
info); _temp3569;}); _temp3568;}); break;}} case 99: _LL3543: yyval=( struct
_xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct* _temp3577=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp3577[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp3578; _temp3578.tag= Cyc_QualSpecList_tok_tag;
_temp3578.f1=({ struct _tuple14* _temp3579=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3579->f1= Cyc_Absyn_empty_tqual(); _temp3579->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp3583=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3583->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3584= yyvs; struct _xtunion_struct** _temp3586=
_temp3584.curr +( yyvsp_offset - 1); if( _temp3584.base == 0? 1:( _temp3586 <
_temp3584.base? 1: _temp3586 >= _temp3584.last_plus_one)){ _throw(
Null_Exception);}* _temp3586;})); _temp3583->tl= 0; _temp3583;}); _temp3579->f3=
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp3580= yyvs; struct
_xtunion_struct** _temp3582= _temp3580.curr + yyvsp_offset; if( _temp3580.base
== 0? 1:( _temp3582 < _temp3580.base? 1: _temp3582 >= _temp3580.last_plus_one)){
_throw( Null_Exception);}* _temp3582;})); _temp3579;}); _temp3578;}); _temp3577;});
break; case 100: _LL3576: yyval=( struct _xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct*
_temp3588=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3588[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3589; _temp3589.tag=
Cyc_QualSpecList_tok_tag; _temp3589.f1=({ struct _tuple14* _temp3590=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3590->f1=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3604= yyvs; struct _xtunion_struct** _temp3606=
_temp3604.curr + yyvsp_offset; if( _temp3604.base == 0? 1:( _temp3606 <
_temp3604.base? 1: _temp3606 >= _temp3604.last_plus_one)){ _throw(
Null_Exception);}* _temp3606;}))).f1; _temp3590->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp3597=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3597->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3601= yyvs; struct _xtunion_struct** _temp3603=
_temp3601.curr +( yyvsp_offset - 2); if( _temp3601.base == 0? 1:( _temp3603 <
_temp3601.base? 1: _temp3603 >= _temp3601.last_plus_one)){ _throw(
Null_Exception);}* _temp3603;})); _temp3597->tl=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3598= yyvs; struct _xtunion_struct** _temp3600=
_temp3598.curr + yyvsp_offset; if( _temp3598.base == 0? 1:( _temp3600 <
_temp3598.base? 1: _temp3600 >= _temp3598.last_plus_one)){ _throw(
Null_Exception);}* _temp3600;}))).f2; _temp3597;}); _temp3590->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3591= yyvs; struct _xtunion_struct** _temp3593=
_temp3591.curr +( yyvsp_offset - 1); if( _temp3591.base == 0? 1:( _temp3593 <
_temp3591.base? 1: _temp3593 >= _temp3591.last_plus_one)){ _throw(
Null_Exception);}* _temp3593;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3594= yyvs; struct _xtunion_struct** _temp3596= _temp3594.curr
+ yyvsp_offset; if( _temp3594.base == 0? 1:( _temp3596 < _temp3594.base? 1:
_temp3596 >= _temp3594.last_plus_one)){ _throw( Null_Exception);}* _temp3596;}))).f3);
_temp3590;}); _temp3589;}); _temp3588;}); break; case 101: _LL3587: yyval=(
struct _xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct* _temp3608=(
struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3608[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3609; _temp3609.tag=
Cyc_QualSpecList_tok_tag; _temp3609.f1=({ struct _tuple14* _temp3610=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3610->f1= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3614= yyvs; struct _xtunion_struct** _temp3616=
_temp3614.curr +( yyvsp_offset - 1); if( _temp3614.base == 0? 1:( _temp3616 <
_temp3614.base? 1: _temp3616 >= _temp3614.last_plus_one)){ _throw(
Null_Exception);}* _temp3616;})); _temp3610->f2= 0; _temp3610->f3= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3611= yyvs; struct _xtunion_struct** _temp3613=
_temp3611.curr + yyvsp_offset; if( _temp3611.base == 0? 1:( _temp3613 <
_temp3611.base? 1: _temp3613 >= _temp3611.last_plus_one)){ _throw(
Null_Exception);}* _temp3613;})); _temp3610;}); _temp3609;}); _temp3608;});
break; case 102: _LL3607: yyval=( struct _xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct*
_temp3618=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3618[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3619; _temp3619.tag=
Cyc_QualSpecList_tok_tag; _temp3619.f1=({ struct _tuple14* _temp3620=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3620->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp3630= yyvs; struct
_xtunion_struct** _temp3632= _temp3630.curr +( yyvsp_offset - 2); if( _temp3630.base
== 0? 1:( _temp3632 < _temp3630.base? 1: _temp3632 >= _temp3630.last_plus_one)){
_throw( Null_Exception);}* _temp3632;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3633= yyvs; struct _xtunion_struct** _temp3635= _temp3633.curr
+ yyvsp_offset; if( _temp3633.base == 0? 1:( _temp3635 < _temp3633.base? 1:
_temp3635 >= _temp3633.last_plus_one)){ _throw( Null_Exception);}* _temp3635;}))).f1);
_temp3620->f2=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3627=
yyvs; struct _xtunion_struct** _temp3629= _temp3627.curr + yyvsp_offset; if(
_temp3627.base == 0? 1:( _temp3629 < _temp3627.base? 1: _temp3629 >= _temp3627.last_plus_one)){
_throw( Null_Exception);}* _temp3629;}))).f2; _temp3620->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3621= yyvs; struct _xtunion_struct** _temp3623=
_temp3621.curr +( yyvsp_offset - 1); if( _temp3621.base == 0? 1:( _temp3623 <
_temp3621.base? 1: _temp3623 >= _temp3621.last_plus_one)){ _throw(
Null_Exception);}* _temp3623;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3624= yyvs; struct _xtunion_struct** _temp3626= _temp3624.curr
+ yyvsp_offset; if( _temp3624.base == 0? 1:( _temp3626 < _temp3624.base? 1:
_temp3626 >= _temp3624.last_plus_one)){ _throw( Null_Exception);}* _temp3626;}))).f3);
_temp3620;}); _temp3619;}); _temp3618;}); break; case 103: _LL3617: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp3637=(
struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct));
_temp3637[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct _temp3638; _temp3638.tag=
Cyc_DeclaratorExpoptList_tok_tag; _temp3638.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3639= yyvs; struct _xtunion_struct** _temp3641=
_temp3639.curr + yyvsp_offset; if( _temp3639.base == 0? 1:( _temp3641 <
_temp3639.base? 1: _temp3641 >= _temp3639.last_plus_one)){ _throw(
Null_Exception);}* _temp3641;}))); _temp3638;}); _temp3637;}); break; case 104:
_LL3636: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclaratorExpoptList_tok_struct*
_temp3643=( struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof(
struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3643[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct
_temp3644; _temp3644.tag= Cyc_DeclaratorExpoptList_tok_tag; _temp3644.f1=({
struct Cyc_List_List* _temp3645=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3645->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(({
struct _tagged_ptr2 _temp3646= yyvs; struct _xtunion_struct** _temp3648=
_temp3646.curr + yyvsp_offset; if( _temp3646.base == 0? 1:( _temp3648 <
_temp3646.base? 1: _temp3648 >= _temp3646.last_plus_one)){ _throw(
Null_Exception);}* _temp3648;})); _temp3645->tl= 0; _temp3645;}); _temp3644;});
_temp3643;}); break; case 105: _LL3642: yyval=( struct _xtunion_struct*)({
struct Cyc_DeclaratorExpoptList_tok_struct* _temp3650=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3650[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp3651; _temp3651.tag= Cyc_DeclaratorExpoptList_tok_tag;
_temp3651.f1=({ struct Cyc_List_List* _temp3652=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3652->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(({
struct _tagged_ptr2 _temp3656= yyvs; struct _xtunion_struct** _temp3658=
_temp3656.curr + yyvsp_offset; if( _temp3656.base == 0? 1:( _temp3658 <
_temp3656.base? 1: _temp3658 >= _temp3656.last_plus_one)){ _throw(
Null_Exception);}* _temp3658;})); _temp3652->tl= Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3653= yyvs; struct _xtunion_struct** _temp3655=
_temp3653.curr +( yyvsp_offset - 2); if( _temp3653.base == 0? 1:( _temp3655 <
_temp3653.base? 1: _temp3655 >= _temp3653.last_plus_one)){ _throw(
Null_Exception);}* _temp3655;})); _temp3652;}); _temp3651;}); _temp3650;});
break; case 106: _LL3649: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclaratorExpopt_tok_struct*
_temp3660=( struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct
Cyc_DeclaratorExpopt_tok_struct)); _temp3660[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct
_temp3661; _temp3661.tag= Cyc_DeclaratorExpopt_tok_tag; _temp3661.f1=({ struct
_tuple15* _temp3662=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15));
_temp3662->f1= Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3663= yyvs;
struct _xtunion_struct** _temp3665= _temp3663.curr + yyvsp_offset; if( _temp3663.base
== 0? 1:( _temp3665 < _temp3663.base? 1: _temp3665 >= _temp3663.last_plus_one)){
_throw( Null_Exception);}* _temp3665;})); _temp3662->f2= 0; _temp3662;});
_temp3661;}); _temp3660;}); break; case 107: _LL3659: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp3667=( struct
Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp3667[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp3668; _temp3668.tag=
Cyc_DeclaratorExpopt_tok_tag; _temp3668.f1=({ struct _tuple15* _temp3669=(
struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp3669->f1=({ struct
Cyc_Parse_Declarator* _temp3674=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3674->id=({ struct _tuple1*
_temp3675=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3675->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp3679=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp3679[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp3680; _temp3680.tag= Cyc_Absyn_Rel_n_tag; _temp3680.f1= 0; _temp3680;});
_temp3679;}); _temp3675->f2=({ struct _tagged_string* _temp3676=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3676[ 0]=(
struct _tagged_string)({ char* _temp3677=( char*)""; struct _tagged_string
_temp3678; _temp3678.curr= _temp3677; _temp3678.base= _temp3677; _temp3678.last_plus_one=
_temp3677 + 1; _temp3678;}); _temp3676;}); _temp3675;}); _temp3674->tms= 0;
_temp3674;}); _temp3669->f2=({ struct Cyc_Core_Opt* _temp3670=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3670->v=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3671= yyvs; struct _xtunion_struct** _temp3673=
_temp3671.curr + yyvsp_offset; if( _temp3671.base == 0? 1:( _temp3673 <
_temp3671.base? 1: _temp3673 >= _temp3671.last_plus_one)){ _throw(
Null_Exception);}* _temp3673;})); _temp3670;}); _temp3669;}); _temp3668;});
_temp3667;}); break; case 108: _LL3666: yyval=( struct _xtunion_struct*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp3682=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp3682[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp3683; _temp3683.tag= Cyc_DeclaratorExpopt_tok_tag;
_temp3683.f1=({ struct _tuple15* _temp3684=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp3684->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3689= yyvs; struct _xtunion_struct** _temp3691= _temp3689.curr
+( yyvsp_offset - 2); if( _temp3689.base == 0? 1:( _temp3691 < _temp3689.base? 1:
_temp3691 >= _temp3689.last_plus_one)){ _throw( Null_Exception);}* _temp3691;}));
_temp3684->f2=({ struct Cyc_Core_Opt* _temp3685=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3685->v=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3686= yyvs; struct _xtunion_struct** _temp3688=
_temp3686.curr + yyvsp_offset; if( _temp3686.base == 0? 1:( _temp3688 <
_temp3686.base? 1: _temp3688 >= _temp3686.last_plus_one)){ _throw(
Null_Exception);}* _temp3688;})); _temp3685;}); _temp3684;}); _temp3683;});
_temp3682;}); break; case 109: _LL3681: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3693= yyls; struct Cyc_Yyltype* _temp3695= _temp3693.curr
+( yylsp_offset - 3); if( _temp3693.base == 0? 1:( _temp3695 < _temp3693.base? 1:
_temp3695 >= _temp3693.last_plus_one)){ _throw( Null_Exception);}* _temp3695;}).first_line,({
struct _tagged_ptr3 _temp3696= yyls; struct Cyc_Yyltype* _temp3698= _temp3696.curr
+( yylsp_offset - 3); if( _temp3696.base == 0? 1:( _temp3698 < _temp3696.base? 1:
_temp3698 >= _temp3696.last_plus_one)){ _throw( Null_Exception);}* _temp3698;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3699= yyvs; struct
_xtunion_struct** _temp3701= _temp3699.curr +( yyvsp_offset - 3); if( _temp3699.base
== 0? 1:( _temp3701 < _temp3699.base? 1: _temp3701 >= _temp3699.last_plus_one)){
_throw( Null_Exception);}* _temp3701;}))); yyval=( struct _xtunion_struct*)({
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
_temp3705;}); _temp3704;})); _temp3703;}); _temp3702;}); break;} case 110:
_LL3692: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3721=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3721[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3722; _temp3722.tag=
Cyc_TypeSpecifier_tok_tag; _temp3722.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp3723=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp3723[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp3724; _temp3724.tag= Cyc_Absyn_TunionType_tag;
_temp3724.f1=({ struct Cyc_Absyn_TunionInfo _temp3725; _temp3725.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3729= yyvs; struct _xtunion_struct** _temp3731=
_temp3729.curr +( yyvsp_offset - 1); if( _temp3729.base == 0? 1:( _temp3731 <
_temp3729.base? 1: _temp3731 >= _temp3729.last_plus_one)){ _throw(
Null_Exception);}* _temp3731;})); _temp3725.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3726= yyvs; struct _xtunion_struct** _temp3728=
_temp3726.curr + yyvsp_offset; if( _temp3726.base == 0? 1:( _temp3728 <
_temp3726.base? 1: _temp3728 >= _temp3726.last_plus_one)){ _throw(
Null_Exception);}* _temp3728;})); _temp3725.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp3725.tud= 0; _temp3725;}); _temp3724;}); _temp3723;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3732= yyls; struct Cyc_Yyltype* _temp3734= _temp3732.curr
+( yylsp_offset - 2); if( _temp3732.base == 0? 1:( _temp3734 < _temp3732.base? 1:
_temp3734 >= _temp3732.last_plus_one)){ _throw( Null_Exception);}* _temp3734;}).first_line,({
struct _tagged_ptr3 _temp3735= yyls; struct Cyc_Yyltype* _temp3737= _temp3735.curr
+ yylsp_offset; if( _temp3735.base == 0? 1:( _temp3737 < _temp3735.base? 1:
_temp3737 >= _temp3735.last_plus_one)){ _throw( Null_Exception);}* _temp3737;}).last_line));
_temp3722;}); _temp3721;}); break; case 111: _LL3720: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3739=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3739[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3740; _temp3740.tag= Cyc_TypeSpecifier_tok_tag;
_temp3740.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp3741=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp3741[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp3742; _temp3742.tag=
Cyc_Absyn_TunionType_tag; _temp3742.f1=({ struct Cyc_Absyn_TunionInfo _temp3743;
_temp3743.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3750= yyvs;
struct _xtunion_struct** _temp3752= _temp3750.curr +( yyvsp_offset - 1); if(
_temp3750.base == 0? 1:( _temp3752 < _temp3750.base? 1: _temp3752 >= _temp3750.last_plus_one)){
_throw( Null_Exception);}* _temp3752;})); _temp3743.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3747= yyvs; struct _xtunion_struct** _temp3749=
_temp3747.curr + yyvsp_offset; if( _temp3747.base == 0? 1:( _temp3749 <
_temp3747.base? 1: _temp3749 >= _temp3747.last_plus_one)){ _throw(
Null_Exception);}* _temp3749;})); _temp3743.rgn=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3744= yyvs; struct _xtunion_struct** _temp3746=
_temp3744.curr +( yyvsp_offset - 2); if( _temp3744.base == 0? 1:( _temp3746 <
_temp3744.base? 1: _temp3746 >= _temp3744.last_plus_one)){ _throw(
Null_Exception);}* _temp3746;})); _temp3743.tud= 0; _temp3743;}); _temp3742;});
_temp3741;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3753= yyls;
struct Cyc_Yyltype* _temp3755= _temp3753.curr +( yylsp_offset - 3); if(
_temp3753.base == 0? 1:( _temp3755 < _temp3753.base? 1: _temp3755 >= _temp3753.last_plus_one)){
_throw( Null_Exception);}* _temp3755;}).first_line,({ struct _tagged_ptr3
_temp3756= yyls; struct Cyc_Yyltype* _temp3758= _temp3756.curr + yylsp_offset;
if( _temp3756.base == 0? 1:( _temp3758 < _temp3756.base? 1: _temp3758 >=
_temp3756.last_plus_one)){ _throw( Null_Exception);}* _temp3758;}).last_line));
_temp3740;}); _temp3739;}); break; case 112: _LL3738: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3760=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3760[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3761; _temp3761.tag= Cyc_TypeSpecifier_tok_tag;
_temp3761.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp3762=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp3762[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp3763; _temp3763.tag= Cyc_Absyn_TunionFieldType_tag; _temp3763.f1=({ struct
Cyc_Absyn_TunionFieldInfo _temp3764; _temp3764.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3771= yyvs; struct _xtunion_struct** _temp3773=
_temp3771.curr +( yyvsp_offset - 3); if( _temp3771.base == 0? 1:( _temp3773 <
_temp3771.base? 1: _temp3773 >= _temp3771.last_plus_one)){ _throw(
Null_Exception);}* _temp3773;})); _temp3764.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3768= yyvs; struct _xtunion_struct** _temp3770=
_temp3768.curr + yyvsp_offset; if( _temp3768.base == 0? 1:( _temp3770 <
_temp3768.base? 1: _temp3770 >= _temp3768.last_plus_one)){ _throw(
Null_Exception);}* _temp3770;})); _temp3764.fname= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3765= yyvs; struct _xtunion_struct** _temp3767=
_temp3765.curr +( yyvsp_offset - 1); if( _temp3765.base == 0? 1:( _temp3767 <
_temp3765.base? 1: _temp3767 >= _temp3765.last_plus_one)){ _throw(
Null_Exception);}* _temp3767;})); _temp3764.tud= 0; _temp3764.tufd= 0; _temp3764;});
_temp3763;}); _temp3762;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3774= yyls; struct Cyc_Yyltype* _temp3776= _temp3774.curr +( yylsp_offset -
4); if( _temp3774.base == 0? 1:( _temp3776 < _temp3774.base? 1: _temp3776 >=
_temp3774.last_plus_one)){ _throw( Null_Exception);}* _temp3776;}).first_line,({
struct _tagged_ptr3 _temp3777= yyls; struct Cyc_Yyltype* _temp3779= _temp3777.curr
+ yylsp_offset; if( _temp3777.base == 0? 1:( _temp3779 < _temp3777.base? 1:
_temp3779 >= _temp3777.last_plus_one)){ _throw( Null_Exception);}* _temp3779;}).last_line));
_temp3761;}); _temp3760;}); break; case 113: _LL3759: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3781=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3781[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3782; _temp3782.tag= Cyc_TypeSpecifier_tok_tag;
_temp3782.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp3783=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3783[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3784; _temp3784.tag= Cyc_Parse_Decl_spec_tag;
_temp3784.f1= Cyc_Absyn_xtunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3785= yyvs; struct _xtunion_struct** _temp3787=
_temp3785.curr +( yyvsp_offset - 3); if( _temp3785.base == 0? 1:( _temp3787 <
_temp3785.base? 1: _temp3787 >= _temp3785.last_plus_one)){ _throw(
Null_Exception);}* _temp3787;})), Cyc_yyget_TunionFieldList_tok(({ struct
_tagged_ptr2 _temp3788= yyvs; struct _xtunion_struct** _temp3790= _temp3788.curr
+( yyvsp_offset - 1); if( _temp3788.base == 0? 1:( _temp3790 < _temp3788.base? 1:
_temp3790 >= _temp3788.last_plus_one)){ _throw( Null_Exception);}* _temp3790;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3791= yyls; struct Cyc_Yyltype*
_temp3793= _temp3791.curr +( yylsp_offset - 4); if( _temp3791.base == 0? 1:(
_temp3793 < _temp3791.base? 1: _temp3793 >= _temp3791.last_plus_one)){ _throw(
Null_Exception);}* _temp3793;}).first_line,({ struct _tagged_ptr3 _temp3794=
yyls; struct Cyc_Yyltype* _temp3796= _temp3794.curr + yylsp_offset; if(
_temp3794.base == 0? 1:( _temp3796 < _temp3794.base? 1: _temp3796 >= _temp3794.last_plus_one)){
_throw( Null_Exception);}* _temp3796;}).last_line)); _temp3784;}); _temp3783;}));
_temp3782;}); _temp3781;}); break; case 114: _LL3780: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3798=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3798[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3799; _temp3799.tag= Cyc_TypeSpecifier_tok_tag;
_temp3799.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_XTunionType_struct*
_temp3800=( struct Cyc_Absyn_XTunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct));
_temp3800[ 0]=({ struct Cyc_Absyn_XTunionType_struct _temp3801; _temp3801.tag=
Cyc_Absyn_XTunionType_tag; _temp3801.f1=({ struct Cyc_Absyn_XTunionInfo
_temp3802; _temp3802.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3803=
yyvs; struct _xtunion_struct** _temp3805= _temp3803.curr + yyvsp_offset; if(
_temp3803.base == 0? 1:( _temp3805 < _temp3803.base? 1: _temp3805 >= _temp3803.last_plus_one)){
_throw( Null_Exception);}* _temp3805;})); _temp3802.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp3802.xtud= 0; _temp3802;}); _temp3801;}); _temp3800;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3806= yyls; struct Cyc_Yyltype* _temp3808= _temp3806.curr
+( yylsp_offset - 1); if( _temp3806.base == 0? 1:( _temp3808 < _temp3806.base? 1:
_temp3808 >= _temp3806.last_plus_one)){ _throw( Null_Exception);}* _temp3808;}).first_line,({
struct _tagged_ptr3 _temp3809= yyls; struct Cyc_Yyltype* _temp3811= _temp3809.curr
+ yylsp_offset; if( _temp3809.base == 0? 1:( _temp3811 < _temp3809.base? 1:
_temp3811 >= _temp3809.last_plus_one)){ _throw( Null_Exception);}* _temp3811;}).last_line));
_temp3799;}); _temp3798;}); break; case 115: _LL3797: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3813=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3813[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3814; _temp3814.tag= Cyc_TypeSpecifier_tok_tag;
_temp3814.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_XTunionType_struct*
_temp3815=( struct Cyc_Absyn_XTunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct));
_temp3815[ 0]=({ struct Cyc_Absyn_XTunionType_struct _temp3816; _temp3816.tag=
Cyc_Absyn_XTunionType_tag; _temp3816.f1=({ struct Cyc_Absyn_XTunionInfo
_temp3817; _temp3817.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3821=
yyvs; struct _xtunion_struct** _temp3823= _temp3821.curr + yyvsp_offset; if(
_temp3821.base == 0? 1:( _temp3823 < _temp3821.base? 1: _temp3823 >= _temp3821.last_plus_one)){
_throw( Null_Exception);}* _temp3823;})); _temp3817.rgn=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3818= yyvs; struct _xtunion_struct** _temp3820=
_temp3818.curr +( yyvsp_offset - 1); if( _temp3818.base == 0? 1:( _temp3820 <
_temp3818.base? 1: _temp3820 >= _temp3818.last_plus_one)){ _throw(
Null_Exception);}* _temp3820;})); _temp3817.xtud= 0; _temp3817;}); _temp3816;});
_temp3815;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3824= yyls;
struct Cyc_Yyltype* _temp3826= _temp3824.curr +( yylsp_offset - 2); if(
_temp3824.base == 0? 1:( _temp3826 < _temp3824.base? 1: _temp3826 >= _temp3824.last_plus_one)){
_throw( Null_Exception);}* _temp3826;}).first_line,({ struct _tagged_ptr3
_temp3827= yyls; struct Cyc_Yyltype* _temp3829= _temp3827.curr + yylsp_offset;
if( _temp3827.base == 0? 1:( _temp3829 < _temp3827.base? 1: _temp3829 >=
_temp3827.last_plus_one)){ _throw( Null_Exception);}* _temp3829;}).last_line));
_temp3814;}); _temp3813;}); break; case 116: _LL3812: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3831=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3831[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3832; _temp3832.tag= Cyc_TypeSpecifier_tok_tag;
_temp3832.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_XTunionFieldType_struct*
_temp3833=( struct Cyc_Absyn_XTunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_XTunionFieldType_struct)); _temp3833[ 0]=({ struct Cyc_Absyn_XTunionFieldType_struct
_temp3834; _temp3834.tag= Cyc_Absyn_XTunionFieldType_tag; _temp3834.f1=({ struct
Cyc_Absyn_XTunionFieldInfo _temp3835; _temp3835.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3839= yyvs; struct _xtunion_struct** _temp3841=
_temp3839.curr +( yyvsp_offset - 2); if( _temp3839.base == 0? 1:( _temp3841 <
_temp3839.base? 1: _temp3841 >= _temp3839.last_plus_one)){ _throw(
Null_Exception);}* _temp3841;})); _temp3835.fname= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3836= yyvs; struct _xtunion_struct** _temp3838=
_temp3836.curr + yyvsp_offset; if( _temp3836.base == 0? 1:( _temp3838 <
_temp3836.base? 1: _temp3838 >= _temp3836.last_plus_one)){ _throw(
Null_Exception);}* _temp3838;})); _temp3835.xtud= 0; _temp3835.xtufd= 0;
_temp3835;}); _temp3834;}); _temp3833;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3842= yyls; struct Cyc_Yyltype* _temp3844= _temp3842.curr +(
yylsp_offset - 3); if( _temp3842.base == 0? 1:( _temp3844 < _temp3842.base? 1:
_temp3844 >= _temp3842.last_plus_one)){ _throw( Null_Exception);}* _temp3844;}).first_line,({
struct _tagged_ptr3 _temp3845= yyls; struct Cyc_Yyltype* _temp3847= _temp3845.curr
+ yylsp_offset; if( _temp3845.base == 0? 1:( _temp3847 < _temp3845.base? 1:
_temp3847 >= _temp3845.last_plus_one)){ _throw( Null_Exception);}* _temp3847;}).last_line));
_temp3832;}); _temp3831;}); break; case 117: _LL3830: yyval=( struct
_xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct* _temp3849=( struct
Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3849[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3850; _temp3850.tag=
Cyc_TunionFieldList_tok_tag; _temp3850.f1=({ struct Cyc_List_List* _temp3851=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3851->hd=(
void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2 _temp3852= yyvs; struct
_xtunion_struct** _temp3854= _temp3852.curr + yyvsp_offset; if( _temp3852.base
== 0? 1:( _temp3854 < _temp3852.base? 1: _temp3854 >= _temp3852.last_plus_one)){
_throw( Null_Exception);}* _temp3854;})); _temp3851->tl= 0; _temp3851;});
_temp3850;}); _temp3849;}); break; case 118: _LL3848: yyval=( struct
_xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct* _temp3856=( struct
Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3856[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3857; _temp3857.tag=
Cyc_TunionFieldList_tok_tag; _temp3857.f1=({ struct Cyc_List_List* _temp3858=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3858->hd=(
void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2 _temp3859= yyvs; struct
_xtunion_struct** _temp3861= _temp3859.curr +( yyvsp_offset - 1); if( _temp3859.base
== 0? 1:( _temp3861 < _temp3859.base? 1: _temp3861 >= _temp3859.last_plus_one)){
_throw( Null_Exception);}* _temp3861;})); _temp3858->tl= 0; _temp3858;});
_temp3857;}); _temp3856;}); break; case 119: _LL3855: yyval=( struct
_xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct* _temp3863=( struct
Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3863[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3864; _temp3864.tag=
Cyc_TunionFieldList_tok_tag; _temp3864.f1=({ struct Cyc_List_List* _temp3865=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3865->hd=(
void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2 _temp3869= yyvs; struct
_xtunion_struct** _temp3871= _temp3869.curr +( yyvsp_offset - 2); if( _temp3869.base
== 0? 1:( _temp3871 < _temp3869.base? 1: _temp3871 >= _temp3869.last_plus_one)){
_throw( Null_Exception);}* _temp3871;})); _temp3865->tl= Cyc_yyget_TunionFieldList_tok(({
struct _tagged_ptr2 _temp3866= yyvs; struct _xtunion_struct** _temp3868=
_temp3866.curr + yyvsp_offset; if( _temp3866.base == 0? 1:( _temp3868 <
_temp3866.base? 1: _temp3868 >= _temp3866.last_plus_one)){ _throw(
Null_Exception);}* _temp3868;})); _temp3865;}); _temp3864;}); _temp3863;});
break; case 120: _LL3862: yyval=( struct _xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct*
_temp3873=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp3873[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp3874; _temp3874.tag= Cyc_TunionFieldList_tok_tag; _temp3874.f1=({ struct
Cyc_List_List* _temp3875=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3875->hd=( void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2
_temp3879= yyvs; struct _xtunion_struct** _temp3881= _temp3879.curr +(
yyvsp_offset - 2); if( _temp3879.base == 0? 1:( _temp3881 < _temp3879.base? 1:
_temp3881 >= _temp3879.last_plus_one)){ _throw( Null_Exception);}* _temp3881;}));
_temp3875->tl= Cyc_yyget_TunionFieldList_tok(({ struct _tagged_ptr2 _temp3876=
yyvs; struct _xtunion_struct** _temp3878= _temp3876.curr + yyvsp_offset; if(
_temp3876.base == 0? 1:( _temp3878 < _temp3876.base? 1: _temp3878 >= _temp3876.last_plus_one)){
_throw( Null_Exception);}* _temp3878;})); _temp3875;}); _temp3874;}); _temp3873;});
break; case 121: _LL3872: yyval=( struct _xtunion_struct*)({ struct Cyc_TunionField_tok_struct*
_temp3883=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp3883[ 0]=({ struct Cyc_TunionField_tok_struct _temp3884; _temp3884.tag= Cyc_TunionField_tok_tag;
_temp3884.f1=({ struct Cyc_Absyn_Tunionfield* _temp3885=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3885->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3892= yyvs; struct _xtunion_struct** _temp3894=
_temp3892.curr + yyvsp_offset; if( _temp3892.base == 0? 1:( _temp3894 <
_temp3892.base? 1: _temp3894 >= _temp3892.last_plus_one)){ _throw(
Null_Exception);}* _temp3894;})); _temp3885->tvs= 0; _temp3885->typs= 0;
_temp3885->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3886=
yyls; struct Cyc_Yyltype* _temp3888= _temp3886.curr + yylsp_offset; if(
_temp3886.base == 0? 1:( _temp3888 < _temp3886.base? 1: _temp3888 >= _temp3886.last_plus_one)){
_throw( Null_Exception);}* _temp3888;}).first_line,({ struct _tagged_ptr3
_temp3889= yyls; struct Cyc_Yyltype* _temp3891= _temp3889.curr + yylsp_offset;
if( _temp3889.base == 0? 1:( _temp3891 < _temp3889.base? 1: _temp3891 >=
_temp3889.last_plus_one)){ _throw( Null_Exception);}* _temp3891;}).last_line);
_temp3885;}); _temp3884;}); _temp3883;}); break; case 122: _LL3882: { struct Cyc_List_List*
typs=(( struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3896= yyls; struct Cyc_Yyltype* _temp3898= _temp3896.curr +(
yylsp_offset - 1); if( _temp3896.base == 0? 1:( _temp3898 < _temp3896.base? 1:
_temp3898 >= _temp3896.last_plus_one)){ _throw( Null_Exception);}* _temp3898;}).first_line,({
struct _tagged_ptr3 _temp3899= yyls; struct Cyc_Yyltype* _temp3901= _temp3899.curr
+( yylsp_offset - 1); if( _temp3899.base == 0? 1:( _temp3901 < _temp3899.base? 1:
_temp3901 >= _temp3899.last_plus_one)){ _throw( Null_Exception);}* _temp3901;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3902= yyvs; struct _xtunion_struct** _temp3904=
_temp3902.curr +( yyvsp_offset - 1); if( _temp3902.base == 0? 1:( _temp3904 <
_temp3902.base? 1: _temp3904 >= _temp3902.last_plus_one)){ _throw(
Null_Exception);}* _temp3904;})))); struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3905= yyls; struct Cyc_Yyltype* _temp3907= _temp3905.curr
+( yylsp_offset - 3); if( _temp3905.base == 0? 1:( _temp3907 < _temp3905.base? 1:
_temp3907 >= _temp3905.last_plus_one)){ _throw( Null_Exception);}* _temp3907;}).first_line,({
struct _tagged_ptr3 _temp3908= yyls; struct Cyc_Yyltype* _temp3910= _temp3908.curr
+( yylsp_offset - 3); if( _temp3908.base == 0? 1:( _temp3910 < _temp3908.base? 1:
_temp3910 >= _temp3908.last_plus_one)){ _throw( Null_Exception);}* _temp3910;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3911= yyvs; struct
_xtunion_struct** _temp3913= _temp3911.curr +( yyvsp_offset - 3); if( _temp3911.base
== 0? 1:( _temp3913 < _temp3911.base? 1: _temp3913 >= _temp3911.last_plus_one)){
_throw( Null_Exception);}* _temp3913;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_TunionField_tok_struct* _temp3914=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp3914[ 0]=({ struct
Cyc_TunionField_tok_struct _temp3915; _temp3915.tag= Cyc_TunionField_tok_tag;
_temp3915.f1=({ struct Cyc_Absyn_Tunionfield* _temp3916=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3916->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3923= yyvs; struct _xtunion_struct** _temp3925=
_temp3923.curr +( yyvsp_offset - 4); if( _temp3923.base == 0? 1:( _temp3925 <
_temp3923.base? 1: _temp3925 >= _temp3923.last_plus_one)){ _throw(
Null_Exception);}* _temp3925;})); _temp3916->tvs= tvs; _temp3916->typs= typs;
_temp3916->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3917=
yyls; struct Cyc_Yyltype* _temp3919= _temp3917.curr +( yylsp_offset - 4); if(
_temp3917.base == 0? 1:( _temp3919 < _temp3917.base? 1: _temp3919 >= _temp3917.last_plus_one)){
_throw( Null_Exception);}* _temp3919;}).first_line,({ struct _tagged_ptr3
_temp3920= yyls; struct Cyc_Yyltype* _temp3922= _temp3920.curr + yylsp_offset;
if( _temp3920.base == 0? 1:( _temp3922 < _temp3920.base? 1: _temp3922 >=
_temp3920.last_plus_one)){ _throw( Null_Exception);}* _temp3922;}).last_line);
_temp3916;}); _temp3915;}); _temp3914;}); break;} case 123: _LL3895: yyval=({
struct _tagged_ptr2 _temp3927= yyvs; struct _xtunion_struct** _temp3929=
_temp3927.curr + yyvsp_offset; if( _temp3927.base == 0? 1:( _temp3929 <
_temp3927.base? 1: _temp3929 >= _temp3927.last_plus_one)){ _throw(
Null_Exception);}* _temp3929;}); break; case 124: _LL3926: yyval=( struct
_xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp3931=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3931[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3932; _temp3932.tag= Cyc_Declarator_tok_tag;
_temp3932.f1=({ struct Cyc_Parse_Declarator* _temp3933=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3933->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3940= yyvs; struct _xtunion_struct** _temp3942=
_temp3940.curr + yyvsp_offset; if( _temp3940.base == 0? 1:( _temp3942 <
_temp3940.base? 1: _temp3942 >= _temp3940.last_plus_one)){ _throw(
Null_Exception);}* _temp3942;})))->id; _temp3933->tms=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp3934= yyvs; struct _xtunion_struct** _temp3936=
_temp3934.curr +( yyvsp_offset - 1); if( _temp3934.base == 0? 1:( _temp3936 <
_temp3934.base? 1: _temp3936 >= _temp3934.last_plus_one)){ _throw(
Null_Exception);}* _temp3936;})),( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3937= yyvs; struct _xtunion_struct** _temp3939= _temp3937.curr
+ yyvsp_offset; if( _temp3937.base == 0? 1:( _temp3939 < _temp3937.base? 1:
_temp3939 >= _temp3937.last_plus_one)){ _throw( Null_Exception);}* _temp3939;})))->tms);
_temp3933;}); _temp3932;}); _temp3931;}); break; case 125: _LL3930: yyval=(
struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp3944=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3944[ 0]=({ struct Cyc_Declarator_tok_struct _temp3945; _temp3945.tag= Cyc_Declarator_tok_tag;
_temp3945.f1=({ struct Cyc_Parse_Declarator* _temp3946=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3946->id= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3947= yyvs; struct _xtunion_struct** _temp3949=
_temp3947.curr + yyvsp_offset; if( _temp3947.base == 0? 1:( _temp3949 <
_temp3947.base? 1: _temp3949 >= _temp3947.last_plus_one)){ _throw(
Null_Exception);}* _temp3949;})); _temp3946->tms= 0; _temp3946;}); _temp3945;});
_temp3944;}); break; case 126: _LL3943: yyval=({ struct _tagged_ptr2 _temp3951=
yyvs; struct _xtunion_struct** _temp3953= _temp3951.curr +( yyvsp_offset - 1);
if( _temp3951.base == 0? 1:( _temp3953 < _temp3951.base? 1: _temp3953 >=
_temp3951.last_plus_one)){ _throw( Null_Exception);}* _temp3953;}); break; case
127: _LL3950: yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct*
_temp3955=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3955[ 0]=({ struct Cyc_Declarator_tok_struct _temp3956; _temp3956.tag= Cyc_Declarator_tok_tag;
_temp3956.f1=({ struct Cyc_Parse_Declarator* _temp3957=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3957->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3962= yyvs; struct _xtunion_struct** _temp3964=
_temp3962.curr +( yyvsp_offset - 2); if( _temp3962.base == 0? 1:( _temp3964 <
_temp3962.base? 1: _temp3964 >= _temp3962.last_plus_one)){ _throw(
Null_Exception);}* _temp3964;})))->id; _temp3957->tms=({ struct Cyc_List_List*
_temp3958=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3958->hd=( void*)(( void*) Cyc_Absyn_Carray_mod); _temp3958->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3959= yyvs; struct _xtunion_struct** _temp3961=
_temp3959.curr +( yyvsp_offset - 2); if( _temp3959.base == 0? 1:( _temp3961 <
_temp3959.base? 1: _temp3961 >= _temp3959.last_plus_one)){ _throw(
Null_Exception);}* _temp3961;})))->tms; _temp3958;}); _temp3957;}); _temp3956;});
_temp3955;}); break; case 128: _LL3954: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp3966=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3966[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3967; _temp3967.tag= Cyc_Declarator_tok_tag;
_temp3967.f1=({ struct Cyc_Parse_Declarator* _temp3968=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3968->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3978= yyvs; struct _xtunion_struct** _temp3980=
_temp3978.curr +( yyvsp_offset - 3); if( _temp3978.base == 0? 1:( _temp3980 <
_temp3978.base? 1: _temp3980 >= _temp3978.last_plus_one)){ _throw(
Null_Exception);}* _temp3980;})))->id; _temp3968->tms=({ struct Cyc_List_List*
_temp3969=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3969->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp3973=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp3973[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp3974; _temp3974.tag= Cyc_Absyn_ConstArray_mod_tag; _temp3974.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3975= yyvs; struct _xtunion_struct** _temp3977=
_temp3975.curr +( yyvsp_offset - 1); if( _temp3975.base == 0? 1:( _temp3977 <
_temp3975.base? 1: _temp3977 >= _temp3975.last_plus_one)){ _throw(
Null_Exception);}* _temp3977;})); _temp3974;}); _temp3973;})); _temp3969->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3970= yyvs; struct
_xtunion_struct** _temp3972= _temp3970.curr +( yyvsp_offset - 3); if( _temp3970.base
== 0? 1:( _temp3972 < _temp3970.base? 1: _temp3972 >= _temp3970.last_plus_one)){
_throw( Null_Exception);}* _temp3972;})))->tms; _temp3969;}); _temp3968;});
_temp3967;}); _temp3966;}); break; case 129: _LL3965: { struct _tuple16
_temp3987; struct Cyc_Core_Opt* _temp3988; int _temp3990; struct Cyc_List_List*
_temp3992; struct _tuple16* _temp3985= Cyc_yyget_ParamDeclListBool_tok(({ struct
_tagged_ptr2 _temp3982= yyvs; struct _xtunion_struct** _temp3984= _temp3982.curr
+( yyvsp_offset - 1); if( _temp3982.base == 0? 1:( _temp3984 < _temp3982.base? 1:
_temp3984 >= _temp3982.last_plus_one)){ _throw( Null_Exception);}* _temp3984;}));
_temp3987=* _temp3985; _LL3993: _temp3992= _temp3987.f1; goto _LL3991; _LL3991:
_temp3990= _temp3987.f2; goto _LL3989; _LL3989: _temp3988= _temp3987.f3; goto
_LL3986; _LL3986: yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct*
_temp3994=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3994[ 0]=({ struct Cyc_Declarator_tok_struct _temp3995; _temp3995.tag= Cyc_Declarator_tok_tag;
_temp3995.f1=({ struct Cyc_Parse_Declarator* _temp3996=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3996->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4005= yyvs; struct _xtunion_struct** _temp4007=
_temp4005.curr +( yyvsp_offset - 3); if( _temp4005.base == 0? 1:( _temp4007 <
_temp4005.base? 1: _temp4007 >= _temp4005.last_plus_one)){ _throw(
Null_Exception);}* _temp4007;})))->id; _temp3996->tms=({ struct Cyc_List_List*
_temp3997=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3997->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4001=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4001[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4002; _temp4002.tag=
Cyc_Absyn_Function_mod_tag; _temp4002.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4003=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4003[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4004; _temp4004.tag= Cyc_Absyn_WithTypes_tag;
_temp4004.f1= _temp3992; _temp4004.f2= _temp3990; _temp4004.f3= _temp3988;
_temp4004;}); _temp4003;})); _temp4002;}); _temp4001;})); _temp3997->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3998= yyvs; struct _xtunion_struct** _temp4000=
_temp3998.curr +( yyvsp_offset - 3); if( _temp3998.base == 0? 1:( _temp4000 <
_temp3998.base? 1: _temp4000 >= _temp3998.last_plus_one)){ _throw(
Null_Exception);}* _temp4000;})))->tms; _temp3997;}); _temp3996;}); _temp3995;});
_temp3994;}); break;} case 130: _LL3981: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp4009=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4009[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4010; _temp4010.tag= Cyc_Declarator_tok_tag;
_temp4010.f1=({ struct Cyc_Parse_Declarator* _temp4011=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4011->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4020= yyvs; struct _xtunion_struct** _temp4022=
_temp4020.curr +( yyvsp_offset - 2); if( _temp4020.base == 0? 1:( _temp4022 <
_temp4020.base? 1: _temp4022 >= _temp4020.last_plus_one)){ _throw(
Null_Exception);}* _temp4022;})))->id; _temp4011->tms=({ struct Cyc_List_List*
_temp4012=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4012->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4016=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4016[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4017; _temp4017.tag=
Cyc_Absyn_Function_mod_tag; _temp4017.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4018=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4018[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4019; _temp4019.tag= Cyc_Absyn_WithTypes_tag;
_temp4019.f1= 0; _temp4019.f2= 0; _temp4019.f3= 0; _temp4019;}); _temp4018;}));
_temp4017;}); _temp4016;})); _temp4012->tl=( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp4013= yyvs; struct _xtunion_struct** _temp4015= _temp4013.curr
+( yyvsp_offset - 2); if( _temp4013.base == 0? 1:( _temp4015 < _temp4013.base? 1:
_temp4015 >= _temp4013.last_plus_one)){ _throw( Null_Exception);}* _temp4015;})))->tms;
_temp4012;}); _temp4011;}); _temp4010;}); _temp4009;}); break; case 131: _LL4008:
yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp4024=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp4024[ 0]=({ struct Cyc_Declarator_tok_struct _temp4025; _temp4025.tag= Cyc_Declarator_tok_tag;
_temp4025.f1=({ struct Cyc_Parse_Declarator* _temp4026=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4026->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4041= yyvs; struct _xtunion_struct** _temp4043=
_temp4041.curr +( yyvsp_offset - 4); if( _temp4041.base == 0? 1:( _temp4043 <
_temp4041.base? 1: _temp4043 >= _temp4041.last_plus_one)){ _throw(
Null_Exception);}* _temp4043;})))->id; _temp4026->tms=({ struct Cyc_List_List*
_temp4027=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4027->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4031=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4031[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4032; _temp4032.tag=
Cyc_Absyn_Function_mod_tag; _temp4032.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4033=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4033[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4034; _temp4034.tag= Cyc_Absyn_WithTypes_tag;
_temp4034.f1= 0; _temp4034.f2= 0; _temp4034.f3=({ struct Cyc_Core_Opt* _temp4035=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4035->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4036=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4036[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4037; _temp4037.tag= Cyc_Absyn_JoinEff_tag;
_temp4037.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4038= yyvs;
struct _xtunion_struct** _temp4040= _temp4038.curr +( yyvsp_offset - 1); if(
_temp4038.base == 0? 1:( _temp4040 < _temp4038.base? 1: _temp4040 >= _temp4038.last_plus_one)){
_throw( Null_Exception);}* _temp4040;})); _temp4037;}); _temp4036;})); _temp4035;});
_temp4034;}); _temp4033;})); _temp4032;}); _temp4031;})); _temp4027->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4028= yyvs; struct _xtunion_struct** _temp4030=
_temp4028.curr +( yyvsp_offset - 4); if( _temp4028.base == 0? 1:( _temp4030 <
_temp4028.base? 1: _temp4030 >= _temp4028.last_plus_one)){ _throw(
Null_Exception);}* _temp4030;})))->tms; _temp4027;}); _temp4026;}); _temp4025;});
_temp4024;}); break; case 132: _LL4023: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp4045=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4045[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4046; _temp4046.tag= Cyc_Declarator_tok_tag;
_temp4046.f1=({ struct Cyc_Parse_Declarator* _temp4047=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4047->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4065= yyvs; struct _xtunion_struct** _temp4067=
_temp4065.curr +( yyvsp_offset - 3); if( _temp4065.base == 0? 1:( _temp4067 <
_temp4065.base? 1: _temp4067 >= _temp4065.last_plus_one)){ _throw(
Null_Exception);}* _temp4067;})))->id; _temp4047->tms=({ struct Cyc_List_List*
_temp4048=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4048->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4052=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4052[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4053; _temp4053.tag=
Cyc_Absyn_Function_mod_tag; _temp4053.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct*
_temp4054=( struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp4054[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp4055; _temp4055.tag= Cyc_Absyn_NoTypes_tag;
_temp4055.f1= Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp4056= yyvs;
struct _xtunion_struct** _temp4058= _temp4056.curr +( yyvsp_offset - 1); if(
_temp4056.base == 0? 1:( _temp4058 < _temp4056.base? 1: _temp4058 >= _temp4056.last_plus_one)){
_throw( Null_Exception);}* _temp4058;})); _temp4055.f2= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4059= yyls; struct Cyc_Yyltype* _temp4061= _temp4059.curr
+( yylsp_offset - 3); if( _temp4059.base == 0? 1:( _temp4061 < _temp4059.base? 1:
_temp4061 >= _temp4059.last_plus_one)){ _throw( Null_Exception);}* _temp4061;}).first_line,({
struct _tagged_ptr3 _temp4062= yyls; struct Cyc_Yyltype* _temp4064= _temp4062.curr
+ yylsp_offset; if( _temp4062.base == 0? 1:( _temp4064 < _temp4062.base? 1:
_temp4064 >= _temp4062.last_plus_one)){ _throw( Null_Exception);}* _temp4064;}).last_line);
_temp4055;}); _temp4054;})); _temp4053;}); _temp4052;})); _temp4048->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4049= yyvs; struct _xtunion_struct** _temp4051=
_temp4049.curr +( yyvsp_offset - 3); if( _temp4049.base == 0? 1:( _temp4051 <
_temp4049.base? 1: _temp4051 >= _temp4049.last_plus_one)){ _throw(
Null_Exception);}* _temp4051;})))->tms; _temp4048;}); _temp4047;}); _temp4046;});
_temp4045;}); break; case 133: _LL4044: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4069= yyls; struct Cyc_Yyltype* _temp4071= _temp4069.curr
+( yylsp_offset - 2); if( _temp4069.base == 0? 1:( _temp4071 < _temp4069.base? 1:
_temp4071 >= _temp4069.last_plus_one)){ _throw( Null_Exception);}* _temp4071;}).first_line,({
struct _tagged_ptr3 _temp4072= yyls; struct Cyc_Yyltype* _temp4074= _temp4072.curr
+ yylsp_offset; if( _temp4072.base == 0? 1:( _temp4074 < _temp4072.base? 1:
_temp4074 >= _temp4072.last_plus_one)){ _throw( Null_Exception);}* _temp4074;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4075= yyvs; struct _xtunion_struct** _temp4077=
_temp4075.curr +( yyvsp_offset - 1); if( _temp4075.base == 0? 1:( _temp4077 <
_temp4075.base? 1: _temp4077 >= _temp4075.last_plus_one)){ _throw(
Null_Exception);}* _temp4077;})))); yyval=( struct _xtunion_struct*)({ struct
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
_temp4078;}); break;} case 134: _LL4068:(( struct Cyc_Lexing_lexbuf*)({ struct
Cyc_Core_Opt* _temp4097= Cyc_Parse_lbuf; if( _temp4097 == 0){ _throw(
Null_Exception);} _temp4097->v;}))->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4098=
yyls; struct Cyc_Yyltype* _temp4100= _temp4098.curr +( yylsp_offset - 2); if(
_temp4098.base == 0? 1:( _temp4100 < _temp4098.base? 1: _temp4100 >= _temp4098.last_plus_one)){
_throw( Null_Exception);}* _temp4100;}).first_line,({ struct _tagged_ptr3
_temp4101= yyls; struct Cyc_Yyltype* _temp4103= _temp4101.curr + yylsp_offset;
if( _temp4101.base == 0? 1:( _temp4103 < _temp4101.base? 1: _temp4103 >=
_temp4101.last_plus_one)){ _throw( Null_Exception);}* _temp4103;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4104= yyvs; struct _xtunion_struct** _temp4106=
_temp4104.curr +( yyvsp_offset - 1); if( _temp4104.base == 0? 1:( _temp4106 <
_temp4104.base? 1: _temp4106 >= _temp4104.last_plus_one)){ _throw(
Null_Exception);}* _temp4106;})))); yyval=( struct _xtunion_struct*)({ struct
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
_temp4107;}); break;} case 135: _LL4096: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp4126=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4126[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4127; _temp4127.tag= Cyc_Declarator_tok_tag;
_temp4127.f1=({ struct Cyc_Parse_Declarator* _temp4128=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4128->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4144= yyvs; struct _xtunion_struct** _temp4146=
_temp4144.curr +( yyvsp_offset - 1); if( _temp4144.base == 0? 1:( _temp4146 <
_temp4144.base? 1: _temp4146 >= _temp4144.last_plus_one)){ _throw(
Null_Exception);}* _temp4146;})))->id; _temp4128->tms=({ struct Cyc_List_List*
_temp4129=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4129->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp4133=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp4133[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp4134; _temp4134.tag= Cyc_Absyn_Attributes_mod_tag; _temp4134.f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4135= yyls; struct Cyc_Yyltype* _temp4137= _temp4135.curr
+ yylsp_offset; if( _temp4135.base == 0? 1:( _temp4137 < _temp4135.base? 1:
_temp4137 >= _temp4135.last_plus_one)){ _throw( Null_Exception);}* _temp4137;}).first_line,({
struct _tagged_ptr3 _temp4138= yyls; struct Cyc_Yyltype* _temp4140= _temp4138.curr
+ yylsp_offset; if( _temp4138.base == 0? 1:( _temp4140 < _temp4138.base? 1:
_temp4140 >= _temp4138.last_plus_one)){ _throw( Null_Exception);}* _temp4140;}).last_line);
_temp4134.f2= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4141= yyvs;
struct _xtunion_struct** _temp4143= _temp4141.curr + yyvsp_offset; if( _temp4141.base
== 0? 1:( _temp4143 < _temp4141.base? 1: _temp4143 >= _temp4141.last_plus_one)){
_throw( Null_Exception);}* _temp4143;})); _temp4134;}); _temp4133;})); _temp4129->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp4130= yyvs; struct
_xtunion_struct** _temp4132= _temp4130.curr +( yyvsp_offset - 1); if( _temp4130.base
== 0? 1:( _temp4132 < _temp4130.base? 1: _temp4132 >= _temp4130.last_plus_one)){
_throw( Null_Exception);}* _temp4132;})))->tms; _temp4129;}); _temp4128;});
_temp4127;}); _temp4126;}); break; case 136: _LL4125: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeModifierList_tok_struct* _temp4148=( struct
Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp4148[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp4149; _temp4149.tag=
Cyc_TypeModifierList_tok_tag; _temp4149.f1=({ struct Cyc_List_List* _temp4150=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4150->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4160=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4160[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4161; _temp4161.tag= Cyc_Absyn_Pointer_mod_tag;
_temp4161.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4162= yyvs; struct _xtunion_struct** _temp4164= _temp4162.curr +(
yyvsp_offset - 2); if( _temp4162.base == 0? 1:( _temp4164 < _temp4162.base? 1:
_temp4164 >= _temp4162.last_plus_one)){ _throw( Null_Exception);}* _temp4164;}));
_temp4161.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4165= yyvs;
struct _xtunion_struct** _temp4167= _temp4165.curr +( yyvsp_offset - 1); if(
_temp4165.base == 0? 1:( _temp4167 < _temp4165.base? 1: _temp4167 >= _temp4165.last_plus_one)){
_throw( Null_Exception);}* _temp4167;})); _temp4161.f3= Cyc_Absyn_empty_tqual();
_temp4161;}); _temp4160;})); _temp4150->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4151= yyls; struct Cyc_Yyltype* _temp4153= _temp4151.curr
+ yylsp_offset; if( _temp4151.base == 0? 1:( _temp4153 < _temp4151.base? 1:
_temp4153 >= _temp4151.last_plus_one)){ _throw( Null_Exception);}* _temp4153;}).first_line,({
struct _tagged_ptr3 _temp4154= yyls; struct Cyc_Yyltype* _temp4156= _temp4154.curr
+ yylsp_offset; if( _temp4154.base == 0? 1:( _temp4156 < _temp4154.base? 1:
_temp4156 >= _temp4154.last_plus_one)){ _throw( Null_Exception);}* _temp4156;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4157= yyvs; struct
_xtunion_struct** _temp4159= _temp4157.curr + yyvsp_offset; if( _temp4157.base
== 0? 1:( _temp4159 < _temp4157.base? 1: _temp4159 >= _temp4157.last_plus_one)){
_throw( Null_Exception);}* _temp4159;})), 0); _temp4150;}); _temp4149;});
_temp4148;}); break; case 137: _LL4147: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeModifierList_tok_struct* _temp4169=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp4169[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp4170; _temp4170.tag= Cyc_TypeModifierList_tok_tag;
_temp4170.f1=({ struct Cyc_List_List* _temp4171=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4171->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp4181=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4181[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4182; _temp4182.tag= Cyc_Absyn_Pointer_mod_tag;
_temp4182.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4183= yyvs; struct _xtunion_struct** _temp4185= _temp4183.curr +(
yyvsp_offset - 3); if( _temp4183.base == 0? 1:( _temp4185 < _temp4183.base? 1:
_temp4185 >= _temp4183.last_plus_one)){ _throw( Null_Exception);}* _temp4185;}));
_temp4182.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4186= yyvs;
struct _xtunion_struct** _temp4188= _temp4186.curr +( yyvsp_offset - 2); if(
_temp4186.base == 0? 1:( _temp4188 < _temp4186.base? 1: _temp4188 >= _temp4186.last_plus_one)){
_throw( Null_Exception);}* _temp4188;})); _temp4182.f3= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp4189= yyvs; struct _xtunion_struct** _temp4191=
_temp4189.curr + yyvsp_offset; if( _temp4189.base == 0? 1:( _temp4191 <
_temp4189.base? 1: _temp4191 >= _temp4189.last_plus_one)){ _throw(
Null_Exception);}* _temp4191;})); _temp4182;}); _temp4181;})); _temp4171->tl=
Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4172= yyls; struct Cyc_Yyltype* _temp4174= _temp4172.curr +( yylsp_offset -
1); if( _temp4172.base == 0? 1:( _temp4174 < _temp4172.base? 1: _temp4174 >=
_temp4172.last_plus_one)){ _throw( Null_Exception);}* _temp4174;}).first_line,({
struct _tagged_ptr3 _temp4175= yyls; struct Cyc_Yyltype* _temp4177= _temp4175.curr
+( yylsp_offset - 1); if( _temp4175.base == 0? 1:( _temp4177 < _temp4175.base? 1:
_temp4177 >= _temp4175.last_plus_one)){ _throw( Null_Exception);}* _temp4177;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4178= yyvs; struct
_xtunion_struct** _temp4180= _temp4178.curr +( yyvsp_offset - 1); if( _temp4178.base
== 0? 1:( _temp4180 < _temp4178.base? 1: _temp4180 >= _temp4178.last_plus_one)){
_throw( Null_Exception);}* _temp4180;})), 0); _temp4171;}); _temp4170;});
_temp4169;}); break; case 138: _LL4168: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeModifierList_tok_struct* _temp4193=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp4193[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp4194; _temp4194.tag= Cyc_TypeModifierList_tok_tag;
_temp4194.f1=({ struct Cyc_List_List* _temp4195=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4195->hd=( void*)(( void*)({
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
_throw( Null_Exception);}* _temp4215;})); _temp4209.f3= Cyc_Absyn_empty_tqual();
_temp4209;}); _temp4208;})); _temp4195->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4196= yyls; struct Cyc_Yyltype* _temp4198= _temp4196.curr
+( yylsp_offset - 1); if( _temp4196.base == 0? 1:( _temp4198 < _temp4196.base? 1:
_temp4198 >= _temp4196.last_plus_one)){ _throw( Null_Exception);}* _temp4198;}).first_line,({
struct _tagged_ptr3 _temp4199= yyls; struct Cyc_Yyltype* _temp4201= _temp4199.curr
+( yylsp_offset - 1); if( _temp4199.base == 0? 1:( _temp4201 < _temp4199.base? 1:
_temp4201 >= _temp4199.last_plus_one)){ _throw( Null_Exception);}* _temp4201;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4202= yyvs; struct
_xtunion_struct** _temp4204= _temp4202.curr +( yyvsp_offset - 1); if( _temp4202.base
== 0? 1:( _temp4204 < _temp4202.base? 1: _temp4204 >= _temp4202.last_plus_one)){
_throw( Null_Exception);}* _temp4204;})), Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp4205= yyvs; struct _xtunion_struct** _temp4207=
_temp4205.curr + yyvsp_offset; if( _temp4205.base == 0? 1:( _temp4207 <
_temp4205.base? 1: _temp4207 >= _temp4205.last_plus_one)){ _throw(
Null_Exception);}* _temp4207;}))); _temp4195;}); _temp4194;}); _temp4193;});
break; case 139: _LL4192: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeModifierList_tok_struct*
_temp4217=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp4217[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp4218; _temp4218.tag= Cyc_TypeModifierList_tok_tag; _temp4218.f1=({ struct
Cyc_List_List* _temp4219=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4219->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4232=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp4232[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp4233; _temp4233.tag=
Cyc_Absyn_Pointer_mod_tag; _temp4233.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp4234= yyvs; struct _xtunion_struct** _temp4236=
_temp4234.curr +( yyvsp_offset - 4); if( _temp4234.base == 0? 1:( _temp4236 <
_temp4234.base? 1: _temp4236 >= _temp4234.last_plus_one)){ _throw(
Null_Exception);}* _temp4236;})); _temp4233.f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp4237= yyvs; struct _xtunion_struct** _temp4239=
_temp4237.curr +( yyvsp_offset - 3); if( _temp4237.base == 0? 1:( _temp4239 <
_temp4237.base? 1: _temp4239 >= _temp4237.last_plus_one)){ _throw(
Null_Exception);}* _temp4239;})); _temp4233.f3= Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp4240= yyvs; struct _xtunion_struct** _temp4242= _temp4240.curr
+( yyvsp_offset - 1); if( _temp4240.base == 0? 1:( _temp4242 < _temp4240.base? 1:
_temp4242 >= _temp4240.last_plus_one)){ _throw( Null_Exception);}* _temp4242;}));
_temp4233;}); _temp4232;})); _temp4219->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4220= yyls; struct Cyc_Yyltype* _temp4222= _temp4220.curr
+( yylsp_offset - 2); if( _temp4220.base == 0? 1:( _temp4222 < _temp4220.base? 1:
_temp4222 >= _temp4220.last_plus_one)){ _throw( Null_Exception);}* _temp4222;}).first_line,({
struct _tagged_ptr3 _temp4223= yyls; struct Cyc_Yyltype* _temp4225= _temp4223.curr
+( yylsp_offset - 2); if( _temp4223.base == 0? 1:( _temp4225 < _temp4223.base? 1:
_temp4225 >= _temp4223.last_plus_one)){ _throw( Null_Exception);}* _temp4225;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4226= yyvs; struct
_xtunion_struct** _temp4228= _temp4226.curr +( yyvsp_offset - 2); if( _temp4226.base
== 0? 1:( _temp4228 < _temp4226.base? 1: _temp4228 >= _temp4226.last_plus_one)){
_throw( Null_Exception);}* _temp4228;})), Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp4229= yyvs; struct _xtunion_struct** _temp4231=
_temp4229.curr + yyvsp_offset; if( _temp4229.base == 0? 1:( _temp4231 <
_temp4229.base? 1: _temp4231 >= _temp4229.last_plus_one)){ _throw(
Null_Exception);}* _temp4231;}))); _temp4219;}); _temp4218;}); _temp4217;});
break; case 140: _LL4216: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4244=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4244[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4245; _temp4245.tag=
Cyc_Pointer_Sort_tok_tag; _temp4245.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp4246=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp4246[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp4247; _temp4247.tag=
Cyc_Absyn_Nullable_ps_tag; _temp4247.f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4248= yyls; struct Cyc_Yyltype* _temp4250= _temp4248.curr
+ yylsp_offset; if( _temp4248.base == 0? 1:( _temp4250 < _temp4248.base? 1:
_temp4250 >= _temp4248.last_plus_one)){ _throw( Null_Exception);}* _temp4250;}).first_line,({
struct _tagged_ptr3 _temp4251= yyls; struct Cyc_Yyltype* _temp4253= _temp4251.curr
+ yylsp_offset; if( _temp4251.base == 0? 1:( _temp4253 < _temp4251.base? 1:
_temp4253 >= _temp4251.last_plus_one)){ _throw( Null_Exception);}* _temp4253;}).last_line));
_temp4247;}); _temp4246;})); _temp4245;}); _temp4244;}); break; case 141:
_LL4243: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4255=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4255[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4256; _temp4256.tag=
Cyc_Pointer_Sort_tok_tag; _temp4256.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp4257=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp4257[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp4258; _temp4258.tag= Cyc_Absyn_NonNullable_ps_tag; _temp4258.f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4259= yyls; struct
Cyc_Yyltype* _temp4261= _temp4259.curr + yylsp_offset; if( _temp4259.base == 0?
1:( _temp4261 < _temp4259.base? 1: _temp4261 >= _temp4259.last_plus_one)){
_throw( Null_Exception);}* _temp4261;}).first_line,({ struct _tagged_ptr3
_temp4262= yyls; struct Cyc_Yyltype* _temp4264= _temp4262.curr + yylsp_offset;
if( _temp4262.base == 0? 1:( _temp4264 < _temp4262.base? 1: _temp4264 >=
_temp4262.last_plus_one)){ _throw( Null_Exception);}* _temp4264;}).last_line));
_temp4258;}); _temp4257;})); _temp4256;}); _temp4255;}); break; case 142:
_LL4254: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4266=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4266[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4267; _temp4267.tag=
Cyc_Pointer_Sort_tok_tag; _temp4267.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp4268=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp4268[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp4269; _temp4269.tag=
Cyc_Absyn_Nullable_ps_tag; _temp4269.f1= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4270= yyvs; struct _xtunion_struct** _temp4272= _temp4270.curr
+( yyvsp_offset - 1); if( _temp4270.base == 0? 1:( _temp4272 < _temp4270.base? 1:
_temp4272 >= _temp4270.last_plus_one)){ _throw( Null_Exception);}* _temp4272;}));
_temp4269;}); _temp4268;})); _temp4267;}); _temp4266;}); break; case 143:
_LL4265: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4274=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4274[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4275; _temp4275.tag=
Cyc_Pointer_Sort_tok_tag; _temp4275.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp4276=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp4276[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp4277; _temp4277.tag= Cyc_Absyn_NonNullable_ps_tag; _temp4277.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4278= yyvs; struct _xtunion_struct** _temp4280=
_temp4278.curr +( yyvsp_offset - 1); if( _temp4278.base == 0? 1:( _temp4280 <
_temp4278.base? 1: _temp4280 >= _temp4278.last_plus_one)){ _throw(
Null_Exception);}* _temp4280;})); _temp4277;}); _temp4276;})); _temp4275;});
_temp4274;}); break; case 144: _LL4273: yyval=( struct _xtunion_struct*)({
struct Cyc_Pointer_Sort_tok_struct* _temp4282=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp4282[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp4283; _temp4283.tag= Cyc_Pointer_Sort_tok_tag;
_temp4283.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp4283;});
_temp4282;}); break; case 145: _LL4281: yyval=( struct _xtunion_struct*)({
struct Cyc_Rgn_tok_struct* _temp4285=( struct Cyc_Rgn_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgn_tok_struct)); _temp4285[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4286; _temp4286.tag= Cyc_Rgn_tok_tag; _temp4286.f1=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp4286;}); _temp4285;}); break; case 146: _LL4284: yyval=({ struct
_tagged_ptr2 _temp4288= yyvs; struct _xtunion_struct** _temp4290= _temp4288.curr
+ yyvsp_offset; if( _temp4288.base == 0? 1:( _temp4290 < _temp4288.base? 1:
_temp4290 >= _temp4288.last_plus_one)){ _throw( Null_Exception);}* _temp4290;});
break; case 147: _LL4287: yyval=( struct _xtunion_struct*)({ struct Cyc_Rgn_tok_struct*
_temp4292=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp4292[ 0]=({ struct Cyc_Rgn_tok_struct _temp4293; _temp4293.tag= Cyc_Rgn_tok_tag;
_temp4293.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4294= yyvs; struct _xtunion_struct** _temp4296= _temp4294.curr
+ yyvsp_offset; if( _temp4294.base == 0? 1:( _temp4296 < _temp4294.base? 1:
_temp4296 >= _temp4294.last_plus_one)){ _throw( Null_Exception);}* _temp4296;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4293;}); _temp4292;}); break; case 148: _LL4291: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp4298= yyvs; struct _xtunion_struct** _temp4300=
_temp4298.curr + yyvsp_offset; if( _temp4298.base == 0? 1:( _temp4300 <
_temp4298.base? 1: _temp4300 >= _temp4298.last_plus_one)){ _throw(
Null_Exception);}* _temp4300;})) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4301=( char*)"expecting region kind\n";
struct _tagged_string _temp4302; _temp4302.curr= _temp4301; _temp4302.base=
_temp4301; _temp4302.last_plus_one= _temp4301 + 23; _temp4302;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4303= yyls; struct Cyc_Yyltype* _temp4305= _temp4303.curr
+ yylsp_offset; if( _temp4303.base == 0? 1:( _temp4305 < _temp4303.base? 1:
_temp4305 >= _temp4303.last_plus_one)){ _throw( Null_Exception);}* _temp4305;}).first_line,({
struct _tagged_ptr3 _temp4306= yyls; struct Cyc_Yyltype* _temp4308= _temp4306.curr
+ yylsp_offset; if( _temp4306.base == 0? 1:( _temp4308 < _temp4306.base? 1:
_temp4308 >= _temp4306.last_plus_one)){ _throw( Null_Exception);}* _temp4308;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_Rgn_tok_struct* _temp4309=( struct
Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4309[
0]=({ struct Cyc_Rgn_tok_struct _temp4310; _temp4310.tag= Cyc_Rgn_tok_tag;
_temp4310.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4311= yyvs; struct _xtunion_struct** _temp4313= _temp4311.curr
+( yyvsp_offset - 2); if( _temp4311.base == 0? 1:( _temp4313 < _temp4311.base? 1:
_temp4313 >= _temp4311.last_plus_one)){ _throw( Null_Exception);}* _temp4313;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4310;}); _temp4309;}); break; case 149: _LL4297: yyval=( struct
_xtunion_struct*)({ struct Cyc_Rgn_tok_struct* _temp4315=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4315[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4316; _temp4316.tag= Cyc_Rgn_tok_tag; _temp4316.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp4316;}); _temp4315;}); break; case 150: _LL4314:
yyval=({ struct _tagged_ptr2 _temp4318= yyvs; struct _xtunion_struct** _temp4320=
_temp4318.curr + yyvsp_offset; if( _temp4318.base == 0? 1:( _temp4320 <
_temp4318.base? 1: _temp4320 >= _temp4318.last_plus_one)){ _throw(
Null_Exception);}* _temp4320;}); break; case 151: _LL4317: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeQual_tok_struct* _temp4322=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp4322[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp4323; _temp4323.tag= Cyc_TypeQual_tok_tag; _temp4323.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp4324= yyvs; struct
_xtunion_struct** _temp4326= _temp4324.curr +( yyvsp_offset - 1); if( _temp4324.base
== 0? 1:( _temp4326 < _temp4324.base? 1: _temp4326 >= _temp4324.last_plus_one)){
_throw( Null_Exception);}* _temp4326;})), Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp4327= yyvs; struct _xtunion_struct** _temp4329= _temp4327.curr
+ yyvsp_offset; if( _temp4327.base == 0? 1:( _temp4329 < _temp4327.base? 1:
_temp4329 >= _temp4327.last_plus_one)){ _throw( Null_Exception);}* _temp4329;})));
_temp4323;}); _temp4322;}); break; case 152: _LL4321: yyval=( struct
_xtunion_struct*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp4331=( struct
Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp4331[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp4332; _temp4332.tag=
Cyc_ParamDeclListBool_tok_tag; _temp4332.f1=({ struct _tuple16* _temp4333=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp4333->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4334= yyvs; struct _xtunion_struct** _temp4336=
_temp4334.curr + yyvsp_offset; if( _temp4334.base == 0? 1:( _temp4336 <
_temp4334.base? 1: _temp4336 >= _temp4334.last_plus_one)){ _throw(
Null_Exception);}* _temp4336;}))); _temp4333->f2= 0; _temp4333->f3= 0; _temp4333;});
_temp4332;}); _temp4331;}); break; case 153: _LL4330: yyval=( struct
_xtunion_struct*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp4338=( struct
Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp4338[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp4339; _temp4339.tag=
Cyc_ParamDeclListBool_tok_tag; _temp4339.f1=({ struct _tuple16* _temp4340=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp4340->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4341= yyvs; struct _xtunion_struct** _temp4343=
_temp4341.curr +( yyvsp_offset - 2); if( _temp4341.base == 0? 1:( _temp4343 <
_temp4341.base? 1: _temp4343 >= _temp4341.last_plus_one)){ _throw(
Null_Exception);}* _temp4343;}))); _temp4340->f2= 1; _temp4340->f3= 0; _temp4340;});
_temp4339;}); _temp4338;}); break; case 154: _LL4337: yyval=( struct
_xtunion_struct*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp4345=( struct
Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp4345[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp4346; _temp4346.tag=
Cyc_ParamDeclListBool_tok_tag; _temp4346.f1=({ struct _tuple16* _temp4347=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp4347->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4354= yyvs; struct _xtunion_struct** _temp4356=
_temp4354.curr +( yyvsp_offset - 2); if( _temp4354.base == 0? 1:( _temp4356 <
_temp4354.base? 1: _temp4356 >= _temp4354.last_plus_one)){ _throw(
Null_Exception);}* _temp4356;}))); _temp4347->f2= 0; _temp4347->f3=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp4348=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp4348->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp4349=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4349[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4350; _temp4350.tag= Cyc_Absyn_JoinEff_tag;
_temp4350.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4351= yyvs;
struct _xtunion_struct** _temp4353= _temp4351.curr + yyvsp_offset; if( _temp4351.base
== 0? 1:( _temp4353 < _temp4351.base? 1: _temp4353 >= _temp4351.last_plus_one)){
_throw( Null_Exception);}* _temp4353;})); _temp4350;}); _temp4349;})); _temp4348;});
_temp4347;}); _temp4346;}); _temp4345;}); break; case 155: _LL4344: yyval=({
struct _tagged_ptr2 _temp4358= yyvs; struct _xtunion_struct** _temp4360=
_temp4358.curr + yyvsp_offset; if( _temp4358.base == 0? 1:( _temp4360 <
_temp4358.base? 1: _temp4360 >= _temp4358.last_plus_one)){ _throw(
Null_Exception);}* _temp4360;}); break; case 156: _LL4357: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4362=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4362[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4363; _temp4363.tag= Cyc_TypeList_tok_tag; _temp4363.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4364= yyvs; struct _xtunion_struct** _temp4366=
_temp4364.curr +( yyvsp_offset - 2); if( _temp4364.base == 0? 1:( _temp4366 <
_temp4364.base? 1: _temp4366 >= _temp4364.last_plus_one)){ _throw(
Null_Exception);}* _temp4366;})), Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2
_temp4367= yyvs; struct _xtunion_struct** _temp4369= _temp4367.curr +
yyvsp_offset; if( _temp4367.base == 0? 1:( _temp4369 < _temp4367.base? 1:
_temp4369 >= _temp4367.last_plus_one)){ _throw( Null_Exception);}* _temp4369;})));
_temp4363;}); _temp4362;}); break; case 157: _LL4361: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4371=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4371[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4372; _temp4372.tag= Cyc_TypeList_tok_tag; _temp4372.f1= 0; _temp4372;});
_temp4371;}); break; case 158: _LL4370: yyval=({ struct _tagged_ptr2 _temp4374=
yyvs; struct _xtunion_struct** _temp4376= _temp4374.curr +( yyvsp_offset - 1);
if( _temp4374.base == 0? 1:( _temp4376 < _temp4374.base? 1: _temp4376 >=
_temp4374.last_plus_one)){ _throw( Null_Exception);}* _temp4376;}); break; case
159: _LL4373: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct*
_temp4378=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4378[ 0]=({ struct Cyc_TypeList_tok_struct _temp4379; _temp4379.tag= Cyc_TypeList_tok_tag;
_temp4379.f1=({ struct Cyc_List_List* _temp4380=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4380->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4381= yyvs; struct
_xtunion_struct** _temp4383= _temp4381.curr + yyvsp_offset; if( _temp4381.base
== 0? 1:( _temp4383 < _temp4381.base? 1: _temp4383 >= _temp4381.last_plus_one)){
_throw( Null_Exception);}* _temp4383;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp4380->tl= 0; _temp4380;});
_temp4379;}); _temp4378;}); break; case 160: _LL4377: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp4385= yyvs; struct _xtunion_struct** _temp4387=
_temp4385.curr + yyvsp_offset; if( _temp4385.base == 0? 1:( _temp4387 <
_temp4385.base? 1: _temp4387 >= _temp4385.last_plus_one)){ _throw(
Null_Exception);}* _temp4387;})) !=( void*) Cyc_Absyn_EffKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4388=( char*)"expecing effect kind (E)";
struct _tagged_string _temp4389; _temp4389.curr= _temp4388; _temp4389.base=
_temp4388; _temp4389.last_plus_one= _temp4388 + 25; _temp4389;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4390= yyls; struct Cyc_Yyltype* _temp4392= _temp4390.curr
+ yylsp_offset; if( _temp4390.base == 0? 1:( _temp4392 < _temp4390.base? 1:
_temp4392 >= _temp4390.last_plus_one)){ _throw( Null_Exception);}* _temp4392;}).first_line,({
struct _tagged_ptr3 _temp4393= yyls; struct Cyc_Yyltype* _temp4395= _temp4393.curr
+ yylsp_offset; if( _temp4393.base == 0? 1:( _temp4395 < _temp4393.base? 1:
_temp4395 >= _temp4393.last_plus_one)){ _throw( Null_Exception);}* _temp4395;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4396=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4396[ 0]=({ struct Cyc_TypeList_tok_struct _temp4397; _temp4397.tag= Cyc_TypeList_tok_tag;
_temp4397.f1=({ struct Cyc_List_List* _temp4398=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4398->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4399= yyvs; struct
_xtunion_struct** _temp4401= _temp4399.curr +( yyvsp_offset - 2); if( _temp4399.base
== 0? 1:( _temp4401 < _temp4399.base? 1: _temp4401 >= _temp4399.last_plus_one)){
_throw( Null_Exception);}* _temp4401;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp4398->tl= 0; _temp4398;});
_temp4397;}); _temp4396;}); break; case 161: _LL4384: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4403=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4403[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4404; _temp4404.tag= Cyc_TypeList_tok_tag; _temp4404.f1=({ struct Cyc_List_List*
_temp4405=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4405->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4406=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4406[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4407; _temp4407.tag= Cyc_Absyn_AccessEff_tag;
_temp4407.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4408= yyvs; struct _xtunion_struct** _temp4410= _temp4408.curr
+ yyvsp_offset; if( _temp4408.base == 0? 1:( _temp4410 < _temp4408.base? 1:
_temp4410 >= _temp4408.last_plus_one)){ _throw( Null_Exception);}* _temp4410;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4407;}); _temp4406;})); _temp4405->tl= 0; _temp4405;}); _temp4404;});
_temp4403;}); break; case 162: _LL4402: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeList_tok_struct* _temp4412=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4412[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4413; _temp4413.tag= Cyc_TypeList_tok_tag; _temp4413.f1=({ struct Cyc_List_List*
_temp4414=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4414->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4418=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4418[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4419; _temp4419.tag= Cyc_Absyn_AccessEff_tag;
_temp4419.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4420= yyvs; struct _xtunion_struct** _temp4422= _temp4420.curr
+( yyvsp_offset - 2); if( _temp4420.base == 0? 1:( _temp4422 < _temp4420.base? 1:
_temp4422 >= _temp4420.last_plus_one)){ _throw( Null_Exception);}* _temp4422;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4419;}); _temp4418;})); _temp4414->tl= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4415= yyvs; struct _xtunion_struct** _temp4417= _temp4415.curr
+ yyvsp_offset; if( _temp4415.base == 0? 1:( _temp4417 < _temp4415.base? 1:
_temp4417 >= _temp4415.last_plus_one)){ _throw( Null_Exception);}* _temp4417;}));
_temp4414;}); _temp4413;}); _temp4412;}); break; case 163: _LL4411: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp4424= yyvs; struct _xtunion_struct** _temp4426=
_temp4424.curr + yyvsp_offset; if( _temp4424.base == 0? 1:( _temp4426 <
_temp4424.base? 1: _temp4426 >= _temp4424.last_plus_one)){ _throw(
Null_Exception);}* _temp4426;})) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4427=( char*)"expecting region kind (R)";
struct _tagged_string _temp4428; _temp4428.curr= _temp4427; _temp4428.base=
_temp4427; _temp4428.last_plus_one= _temp4427 + 26; _temp4428;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4429= yyls; struct Cyc_Yyltype* _temp4431= _temp4429.curr
+ yylsp_offset; if( _temp4429.base == 0? 1:( _temp4431 < _temp4429.base? 1:
_temp4431 >= _temp4429.last_plus_one)){ _throw( Null_Exception);}* _temp4431;}).first_line,({
struct _tagged_ptr3 _temp4432= yyls; struct Cyc_Yyltype* _temp4434= _temp4432.curr
+ yylsp_offset; if( _temp4432.base == 0? 1:( _temp4434 < _temp4432.base? 1:
_temp4434 >= _temp4432.last_plus_one)){ _throw( Null_Exception);}* _temp4434;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4435=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4435[ 0]=({ struct Cyc_TypeList_tok_struct _temp4436; _temp4436.tag= Cyc_TypeList_tok_tag;
_temp4436.f1=({ struct Cyc_List_List* _temp4437=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4437->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp4438=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp4438[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp4439; _temp4439.tag= Cyc_Absyn_AccessEff_tag;
_temp4439.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4440= yyvs; struct _xtunion_struct** _temp4442= _temp4440.curr
+( yyvsp_offset - 2); if( _temp4440.base == 0? 1:( _temp4442 < _temp4440.base? 1:
_temp4442 >= _temp4440.last_plus_one)){ _throw( Null_Exception);}* _temp4442;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4439;}); _temp4438;})); _temp4437->tl= 0; _temp4437;}); _temp4436;});
_temp4435;}); break; case 164: _LL4423: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp4444= yyvs; struct _xtunion_struct** _temp4446= _temp4444.curr
+( yyvsp_offset - 2); if( _temp4444.base == 0? 1:( _temp4446 < _temp4444.base? 1:
_temp4446 >= _temp4444.last_plus_one)){ _throw( Null_Exception);}* _temp4446;}))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp4447=( char*)"expecting region kind (R)"; struct _tagged_string _temp4448;
_temp4448.curr= _temp4447; _temp4448.base= _temp4447; _temp4448.last_plus_one=
_temp4447 + 26; _temp4448;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4449= yyls; struct Cyc_Yyltype* _temp4451= _temp4449.curr +( yylsp_offset -
2); if( _temp4449.base == 0? 1:( _temp4451 < _temp4449.base? 1: _temp4451 >=
_temp4449.last_plus_one)){ _throw( Null_Exception);}* _temp4451;}).first_line,({
struct _tagged_ptr3 _temp4452= yyls; struct Cyc_Yyltype* _temp4454= _temp4452.curr
+( yylsp_offset - 2); if( _temp4452.base == 0? 1:( _temp4454 < _temp4452.base? 1:
_temp4454 >= _temp4452.last_plus_one)){ _throw( Null_Exception);}* _temp4454;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4455=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4455[ 0]=({ struct Cyc_TypeList_tok_struct _temp4456; _temp4456.tag= Cyc_TypeList_tok_tag;
_temp4456.f1=({ struct Cyc_List_List* _temp4457=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4457->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp4461=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp4461[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp4462; _temp4462.tag= Cyc_Absyn_AccessEff_tag;
_temp4462.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4463= yyvs; struct _xtunion_struct** _temp4465= _temp4463.curr
+( yyvsp_offset - 4); if( _temp4463.base == 0? 1:( _temp4465 < _temp4463.base? 1:
_temp4465 >= _temp4463.last_plus_one)){ _throw( Null_Exception);}* _temp4465;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4462;}); _temp4461;})); _temp4457->tl= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4458= yyvs; struct _xtunion_struct** _temp4460= _temp4458.curr
+ yyvsp_offset; if( _temp4458.base == 0? 1:( _temp4460 < _temp4458.base? 1:
_temp4460 >= _temp4458.last_plus_one)){ _throw( Null_Exception);}* _temp4460;}));
_temp4457;}); _temp4456;}); _temp4455;}); break; case 165: _LL4443: yyval=(
struct _xtunion_struct*)({ struct Cyc_ParamDeclList_tok_struct* _temp4467=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp4467[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp4468; _temp4468.tag=
Cyc_ParamDeclList_tok_tag; _temp4468.f1=({ struct Cyc_List_List* _temp4469=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4469->hd=(
void*) Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4470= yyvs; struct
_xtunion_struct** _temp4472= _temp4470.curr + yyvsp_offset; if( _temp4470.base
== 0? 1:( _temp4472 < _temp4470.base? 1: _temp4472 >= _temp4470.last_plus_one)){
_throw( Null_Exception);}* _temp4472;})); _temp4469->tl= 0; _temp4469;});
_temp4468;}); _temp4467;}); break; case 166: _LL4466: yyval=( struct
_xtunion_struct*)({ struct Cyc_ParamDeclList_tok_struct* _temp4474=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp4474[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp4475; _temp4475.tag= Cyc_ParamDeclList_tok_tag;
_temp4475.f1=({ struct Cyc_List_List* _temp4476=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4476->hd=( void*) Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp4480= yyvs; struct _xtunion_struct** _temp4482=
_temp4480.curr + yyvsp_offset; if( _temp4480.base == 0? 1:( _temp4482 <
_temp4480.base? 1: _temp4482 >= _temp4480.last_plus_one)){ _throw(
Null_Exception);}* _temp4482;})); _temp4476->tl= Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4477= yyvs; struct _xtunion_struct** _temp4479=
_temp4477.curr +( yyvsp_offset - 2); if( _temp4477.base == 0? 1:( _temp4479 <
_temp4477.base? 1: _temp4479 >= _temp4477.last_plus_one)){ _throw(
Null_Exception);}* _temp4479;})); _temp4476;}); _temp4475;}); _temp4474;});
break; case 167: _LL4473: { void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4484= yyvs; struct _xtunion_struct** _temp4486=
_temp4484.curr +( yyvsp_offset - 1); if( _temp4484.base == 0? 1:( _temp4486 <
_temp4484.base? 1: _temp4486 >= _temp4484.last_plus_one)){ _throw(
Null_Exception);}* _temp4486;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4487= yyls; struct Cyc_Yyltype* _temp4489= _temp4487.curr +(
yylsp_offset - 1); if( _temp4487.base == 0? 1:( _temp4489 < _temp4487.base? 1:
_temp4489 >= _temp4487.last_plus_one)){ _throw( Null_Exception);}* _temp4489;}).first_line,({
struct _tagged_ptr3 _temp4490= yyls; struct Cyc_Yyltype* _temp4492= _temp4490.curr
+( yylsp_offset - 1); if( _temp4490.base == 0? 1:( _temp4492 < _temp4490.base? 1:
_temp4492 >= _temp4490.last_plus_one)){ _throw( Null_Exception);}* _temp4492;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4493= yyvs; struct _xtunion_struct** _temp4495= _temp4493.curr +(
yyvsp_offset - 1); if( _temp4493.base == 0? 1:( _temp4495 < _temp4493.base? 1:
_temp4495 >= _temp4493.last_plus_one)){ _throw( Null_Exception);}* _temp4495;}))).f3;
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4496= yyvs; struct _xtunion_struct** _temp4498= _temp4496.curr +(
yyvsp_offset - 1); if( _temp4496.base == 0? 1:( _temp4498 < _temp4496.base? 1:
_temp4498 >= _temp4496.last_plus_one)){ _throw( Null_Exception);}* _temp4498;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp4499= yyvs; struct _xtunion_struct** _temp4501= _temp4499.curr +
yyvsp_offset; if( _temp4499.base == 0? 1:( _temp4501 < _temp4499.base? 1:
_temp4501 >= _temp4499.last_plus_one)){ _throw( Null_Exception);}* _temp4501;})))->tms;
struct _tuple6 t_info= Cyc_Parse_apply_tms( tq, t, atts, tms); if( t_info.f3 !=
0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp4502=( char*)"parameter with bad type params";
struct _tagged_string _temp4503; _temp4503.curr= _temp4502; _temp4503.base=
_temp4502; _temp4503.last_plus_one= _temp4502 + 31; _temp4503;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4504= yyls; struct Cyc_Yyltype* _temp4506= _temp4504.curr
+ yylsp_offset; if( _temp4504.base == 0? 1:( _temp4506 < _temp4504.base? 1:
_temp4506 >= _temp4504.last_plus_one)){ _throw( Null_Exception);}* _temp4506;}).first_line,({
struct _tagged_ptr3 _temp4507= yyls; struct Cyc_Yyltype* _temp4509= _temp4507.curr
+ yylsp_offset; if( _temp4507.base == 0? 1:( _temp4509 < _temp4507.base? 1:
_temp4509 >= _temp4507.last_plus_one)){ _throw( Null_Exception);}* _temp4509;}).last_line));}{
struct _tuple1* q=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp4510=
yyvs; struct _xtunion_struct** _temp4512= _temp4510.curr + yyvsp_offset; if(
_temp4510.base == 0? 1:( _temp4512 < _temp4510.base? 1: _temp4512 >= _temp4510.last_plus_one)){
_throw( Null_Exception);}* _temp4512;})))->id;{ void* _temp4513=(* q).f1; struct
Cyc_List_List* _temp4523; struct Cyc_List_List* _temp4525; _LL4515: if(( int)
_temp4513 == Cyc_Absyn_Loc_n){ goto _LL4516;} else{ goto _LL4517;} _LL4517: if((
unsigned int) _temp4513 > 1u?(( struct _tunion_struct*) _temp4513)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL4524: _temp4523=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp4513)->f1; if( _temp4523 == 0){ goto _LL4518;} else{ goto _LL4519;}} else{
goto _LL4519;} _LL4519: if(( unsigned int) _temp4513 > 1u?(( struct
_tunion_struct*) _temp4513)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL4526: _temp4525=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp4513)->f1; if(
_temp4525 == 0){ goto _LL4520;} else{ goto _LL4521;}} else{ goto _LL4521;}
_LL4521: goto _LL4522; _LL4516: goto _LL4514; _LL4518: goto _LL4514; _LL4520:
goto _LL4514; _LL4522: Cyc_Parse_err(( struct _tagged_string)({ char* _temp4527=(
char*)"parameter cannot be qualified with a module name"; struct _tagged_string
_temp4528; _temp4528.curr= _temp4527; _temp4528.base= _temp4527; _temp4528.last_plus_one=
_temp4527 + 49; _temp4528;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4529= yyls; struct Cyc_Yyltype* _temp4531= _temp4529.curr +( yylsp_offset -
1); if( _temp4529.base == 0? 1:( _temp4531 < _temp4529.base? 1: _temp4531 >=
_temp4529.last_plus_one)){ _throw( Null_Exception);}* _temp4531;}).first_line,({
struct _tagged_ptr3 _temp4532= yyls; struct Cyc_Yyltype* _temp4534= _temp4532.curr
+( yylsp_offset - 1); if( _temp4532.base == 0? 1:( _temp4534 < _temp4532.base? 1:
_temp4534 >= _temp4532.last_plus_one)){ _throw( Null_Exception);}* _temp4534;}).last_line));
goto _LL4514; _LL4514:;}{ struct Cyc_Core_Opt* idopt=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp4535=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp4535->v=( void*)(* q).f2; _temp4535;}); if( t_info.f4 != 0){
Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4536=( char*)"extra attributes on parameter, ignoring";
struct _tagged_string _temp4537; _temp4537.curr= _temp4536; _temp4537.base=
_temp4536; _temp4537.last_plus_one= _temp4536 + 40; _temp4537;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4538= yyls; struct Cyc_Yyltype* _temp4540= _temp4538.curr
+( yylsp_offset - 1); if( _temp4538.base == 0? 1:( _temp4540 < _temp4538.base? 1:
_temp4540 >= _temp4538.last_plus_one)){ _throw( Null_Exception);}* _temp4540;}).first_line,({
struct _tagged_ptr3 _temp4541= yyls; struct Cyc_Yyltype* _temp4543= _temp4541.curr
+ yylsp_offset; if( _temp4541.base == 0? 1:( _temp4543 < _temp4541.base? 1:
_temp4543 >= _temp4541.last_plus_one)){ _throw( Null_Exception);}* _temp4543;}).last_line));}
if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4544=(
char*)"extra type variables on parameter, ignoring"; struct _tagged_string
_temp4545; _temp4545.curr= _temp4544; _temp4545.base= _temp4544; _temp4545.last_plus_one=
_temp4544 + 44; _temp4545;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4546= yyls; struct Cyc_Yyltype* _temp4548= _temp4546.curr +( yylsp_offset -
1); if( _temp4546.base == 0? 1:( _temp4548 < _temp4546.base? 1: _temp4548 >=
_temp4546.last_plus_one)){ _throw( Null_Exception);}* _temp4548;}).first_line,({
struct _tagged_ptr3 _temp4549= yyls; struct Cyc_Yyltype* _temp4551= _temp4549.curr
+ yylsp_offset; if( _temp4549.base == 0? 1:( _temp4551 < _temp4549.base? 1:
_temp4551 >= _temp4549.last_plus_one)){ _throw( Null_Exception);}* _temp4551;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4552=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4552[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4553; _temp4553.tag= Cyc_ParamDecl_tok_tag;
_temp4553.f1=({ struct _tuple2* _temp4554=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4554->f1= idopt; _temp4554->f2= t_info.f1; _temp4554->f3=
t_info.f2; _temp4554;}); _temp4553;}); _temp4552;}); break;}}} case 168: _LL4483: {
void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp4556= yyvs; struct _xtunion_struct** _temp4558= _temp4556.curr
+ yyvsp_offset; if( _temp4556.base == 0? 1:( _temp4558 < _temp4556.base? 1:
_temp4558 >= _temp4556.last_plus_one)){ _throw( Null_Exception);}* _temp4558;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4559= yyls; struct Cyc_Yyltype*
_temp4561= _temp4559.curr + yylsp_offset; if( _temp4559.base == 0? 1:( _temp4561
< _temp4559.base? 1: _temp4561 >= _temp4559.last_plus_one)){ _throw(
Null_Exception);}* _temp4561;}).first_line,({ struct _tagged_ptr3 _temp4562=
yyls; struct Cyc_Yyltype* _temp4564= _temp4562.curr + yylsp_offset; if(
_temp4562.base == 0? 1:( _temp4564 < _temp4562.base? 1: _temp4564 >= _temp4562.last_plus_one)){
_throw( Null_Exception);}* _temp4564;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4565= yyvs; struct
_xtunion_struct** _temp4567= _temp4565.curr + yyvsp_offset; if( _temp4565.base
== 0? 1:( _temp4567 < _temp4565.base? 1: _temp4567 >= _temp4565.last_plus_one)){
_throw( Null_Exception);}* _temp4567;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4568= yyvs; struct _xtunion_struct** _temp4570=
_temp4568.curr + yyvsp_offset; if( _temp4568.base == 0? 1:( _temp4570 <
_temp4568.base? 1: _temp4570 >= _temp4568.last_plus_one)){ _throw(
Null_Exception);}* _temp4570;}))).f1; if( atts != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp4571=( char*)"bad attributes on parameter, ignoring";
struct _tagged_string _temp4572; _temp4572.curr= _temp4571; _temp4572.base=
_temp4571; _temp4572.last_plus_one= _temp4571 + 38; _temp4572;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4573= yyls; struct Cyc_Yyltype* _temp4575= _temp4573.curr
+ yylsp_offset; if( _temp4573.base == 0? 1:( _temp4575 < _temp4573.base? 1:
_temp4575 >= _temp4573.last_plus_one)){ _throw( Null_Exception);}* _temp4575;}).first_line,({
struct _tagged_ptr3 _temp4576= yyls; struct Cyc_Yyltype* _temp4578= _temp4576.curr
+ yylsp_offset; if( _temp4576.base == 0? 1:( _temp4578 < _temp4576.base? 1:
_temp4578 >= _temp4576.last_plus_one)){ _throw( Null_Exception);}* _temp4578;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4579=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4579[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4580; _temp4580.tag= Cyc_ParamDecl_tok_tag;
_temp4580.f1=({ struct _tuple2* _temp4581=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4581->f1= 0; _temp4581->f2= tq; _temp4581->f3= t;
_temp4581;}); _temp4580;}); _temp4579;}); break;} case 169: _LL4555: { void* t=
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4583= yyvs; struct _xtunion_struct** _temp4585= _temp4583.curr +(
yyvsp_offset - 1); if( _temp4583.base == 0? 1:( _temp4585 < _temp4583.base? 1:
_temp4585 >= _temp4583.last_plus_one)){ _throw( Null_Exception);}* _temp4585;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4586= yyls; struct Cyc_Yyltype*
_temp4588= _temp4586.curr +( yylsp_offset - 1); if( _temp4586.base == 0? 1:(
_temp4588 < _temp4586.base? 1: _temp4588 >= _temp4586.last_plus_one)){ _throw(
Null_Exception);}* _temp4588;}).first_line,({ struct _tagged_ptr3 _temp4589=
yyls; struct Cyc_Yyltype* _temp4591= _temp4589.curr +( yylsp_offset - 1); if(
_temp4589.base == 0? 1:( _temp4591 < _temp4589.base? 1: _temp4591 >= _temp4589.last_plus_one)){
_throw( Null_Exception);}* _temp4591;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4592= yyvs; struct
_xtunion_struct** _temp4594= _temp4592.curr +( yyvsp_offset - 1); if( _temp4592.base
== 0? 1:( _temp4594 < _temp4592.base? 1: _temp4594 >= _temp4592.last_plus_one)){
_throw( Null_Exception);}* _temp4594;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4595= yyvs; struct _xtunion_struct** _temp4597=
_temp4595.curr +( yyvsp_offset - 1); if( _temp4595.base == 0? 1:( _temp4597 <
_temp4595.base? 1: _temp4597 >= _temp4595.last_plus_one)){ _throw(
Null_Exception);}* _temp4597;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4598= yyvs; struct _xtunion_struct** _temp4600=
_temp4598.curr + yyvsp_offset; if( _temp4598.base == 0? 1:( _temp4600 <
_temp4598.base? 1: _temp4600 >= _temp4598.last_plus_one)){ _throw(
Null_Exception);}* _temp4600;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp4601=( char*)"bad type parameters on formal argument, ignoring";
struct _tagged_string _temp4602; _temp4602.curr= _temp4601; _temp4602.base=
_temp4601; _temp4602.last_plus_one= _temp4601 + 49; _temp4602;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4603= yyls; struct Cyc_Yyltype* _temp4605= _temp4603.curr
+( yylsp_offset - 1); if( _temp4603.base == 0? 1:( _temp4605 < _temp4603.base? 1:
_temp4605 >= _temp4603.last_plus_one)){ _throw( Null_Exception);}* _temp4605;}).first_line,({
struct _tagged_ptr3 _temp4606= yyls; struct Cyc_Yyltype* _temp4608= _temp4606.curr
+ yylsp_offset; if( _temp4606.base == 0? 1:( _temp4608 < _temp4606.base? 1:
_temp4608 >= _temp4606.last_plus_one)){ _throw( Null_Exception);}* _temp4608;}).last_line));}
if( t_info.f4 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4609=(
char*)"bad attributes on parameter, ignoring"; struct _tagged_string _temp4610;
_temp4610.curr= _temp4609; _temp4610.base= _temp4609; _temp4610.last_plus_one=
_temp4609 + 38; _temp4610;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4611= yyls; struct Cyc_Yyltype* _temp4613= _temp4611.curr +( yylsp_offset -
1); if( _temp4611.base == 0? 1:( _temp4613 < _temp4611.base? 1: _temp4613 >=
_temp4611.last_plus_one)){ _throw( Null_Exception);}* _temp4613;}).first_line,({
struct _tagged_ptr3 _temp4614= yyls; struct Cyc_Yyltype* _temp4616= _temp4614.curr
+ yylsp_offset; if( _temp4614.base == 0? 1:( _temp4616 < _temp4614.base? 1:
_temp4616 >= _temp4614.last_plus_one)){ _throw( Null_Exception);}* _temp4616;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4617=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4617[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4618; _temp4618.tag= Cyc_ParamDecl_tok_tag;
_temp4618.f1=({ struct _tuple2* _temp4619=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4619->f1= 0; _temp4619->f2= t_info.f1; _temp4619->f3=
t_info.f2; _temp4619;}); _temp4618;}); _temp4617;}); break;} case 170: _LL4582:
yyval=( struct _xtunion_struct*)({ struct Cyc_IdList_tok_struct* _temp4621=(
struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4621[ 0]=({ struct Cyc_IdList_tok_struct _temp4622; _temp4622.tag= Cyc_IdList_tok_tag;
_temp4622.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp4623= yyvs; struct
_xtunion_struct** _temp4625= _temp4623.curr + yyvsp_offset; if( _temp4623.base
== 0? 1:( _temp4625 < _temp4623.base? 1: _temp4625 >= _temp4623.last_plus_one)){
_throw( Null_Exception);}* _temp4625;}))); _temp4622;}); _temp4621;}); break;
case 171: _LL4620: yyval=( struct _xtunion_struct*)({ struct Cyc_IdList_tok_struct*
_temp4627=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4627[ 0]=({ struct Cyc_IdList_tok_struct _temp4628; _temp4628.tag= Cyc_IdList_tok_tag;
_temp4628.f1=({ struct Cyc_List_List* _temp4629=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4629->hd=( void*)({ struct
_tagged_string* _temp4630=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4630[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp4631= yyvs; struct _xtunion_struct** _temp4633= _temp4631.curr +
yyvsp_offset; if( _temp4631.base == 0? 1:( _temp4633 < _temp4631.base? 1:
_temp4633 >= _temp4631.last_plus_one)){ _throw( Null_Exception);}* _temp4633;}));
_temp4630;}); _temp4629->tl= 0; _temp4629;}); _temp4628;}); _temp4627;}); break;
case 172: _LL4626: yyval=( struct _xtunion_struct*)({ struct Cyc_IdList_tok_struct*
_temp4635=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4635[ 0]=({ struct Cyc_IdList_tok_struct _temp4636; _temp4636.tag= Cyc_IdList_tok_tag;
_temp4636.f1=({ struct Cyc_List_List* _temp4637=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4637->hd=( void*)({ struct
_tagged_string* _temp4641=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4641[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp4642= yyvs; struct _xtunion_struct** _temp4644= _temp4642.curr +
yyvsp_offset; if( _temp4642.base == 0? 1:( _temp4644 < _temp4642.base? 1:
_temp4644 >= _temp4642.last_plus_one)){ _throw( Null_Exception);}* _temp4644;}));
_temp4641;}); _temp4637->tl= Cyc_yyget_IdList_tok(({ struct _tagged_ptr2
_temp4638= yyvs; struct _xtunion_struct** _temp4640= _temp4638.curr +(
yyvsp_offset - 2); if( _temp4638.base == 0? 1:( _temp4640 < _temp4638.base? 1:
_temp4640 >= _temp4638.last_plus_one)){ _throw( Null_Exception);}* _temp4640;}));
_temp4637;}); _temp4636;}); _temp4635;}); break; case 173: _LL4634: yyval=({
struct _tagged_ptr2 _temp4646= yyvs; struct _xtunion_struct** _temp4648=
_temp4646.curr + yyvsp_offset; if( _temp4646.base == 0? 1:( _temp4648 <
_temp4646.base? 1: _temp4648 >= _temp4646.last_plus_one)){ _throw(
Null_Exception);}* _temp4648;}); break; case 174: _LL4645: yyval=({ struct
_tagged_ptr2 _temp4650= yyvs; struct _xtunion_struct** _temp4652= _temp4650.curr
+ yyvsp_offset; if( _temp4650.base == 0? 1:( _temp4652 < _temp4650.base? 1:
_temp4652 >= _temp4650.last_plus_one)){ _throw( Null_Exception);}* _temp4652;});
break; case 175: _LL4649: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp4654=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4654[ 0]=({ struct Cyc_Exp_tok_struct _temp4655; _temp4655.tag= Cyc_Exp_tok_tag;
_temp4655.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp4656=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp4656[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp4657; _temp4657.tag= Cyc_Absyn_UnresolvedMem_e_tag; _temp4657.f1= 0;
_temp4657.f2= 0; _temp4657;}); _temp4656;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4658= yyls; struct Cyc_Yyltype* _temp4660= _temp4658.curr
+( yylsp_offset - 1); if( _temp4658.base == 0? 1:( _temp4660 < _temp4658.base? 1:
_temp4660 >= _temp4658.last_plus_one)){ _throw( Null_Exception);}* _temp4660;}).first_line,({
struct _tagged_ptr3 _temp4661= yyls; struct Cyc_Yyltype* _temp4663= _temp4661.curr
+ yylsp_offset; if( _temp4661.base == 0? 1:( _temp4663 < _temp4661.base? 1:
_temp4663 >= _temp4661.last_plus_one)){ _throw( Null_Exception);}* _temp4663;}).last_line));
_temp4655;}); _temp4654;}); break; case 176: _LL4653: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp4665=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4665[ 0]=({ struct Cyc_Exp_tok_struct
_temp4666; _temp4666.tag= Cyc_Exp_tok_tag; _temp4666.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4667=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4667[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4668; _temp4668.tag= Cyc_Absyn_UnresolvedMem_e_tag;
_temp4668.f1= 0; _temp4668.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp4669= yyvs; struct _xtunion_struct** _temp4671= _temp4669.curr +(
yyvsp_offset - 1); if( _temp4669.base == 0? 1:( _temp4671 < _temp4669.base? 1:
_temp4671 >= _temp4669.last_plus_one)){ _throw( Null_Exception);}* _temp4671;})));
_temp4668;}); _temp4667;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4672= yyls; struct Cyc_Yyltype* _temp4674= _temp4672.curr +( yylsp_offset -
2); if( _temp4672.base == 0? 1:( _temp4674 < _temp4672.base? 1: _temp4674 >=
_temp4672.last_plus_one)){ _throw( Null_Exception);}* _temp4674;}).first_line,({
struct _tagged_ptr3 _temp4675= yyls; struct Cyc_Yyltype* _temp4677= _temp4675.curr
+ yylsp_offset; if( _temp4675.base == 0? 1:( _temp4677 < _temp4675.base? 1:
_temp4677 >= _temp4675.last_plus_one)){ _throw( Null_Exception);}* _temp4677;}).last_line));
_temp4666;}); _temp4665;}); break; case 177: _LL4664: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp4679=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4679[ 0]=({ struct Cyc_Exp_tok_struct
_temp4680; _temp4680.tag= Cyc_Exp_tok_tag; _temp4680.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4681=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4681[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4682; _temp4682.tag= Cyc_Absyn_UnresolvedMem_e_tag;
_temp4682.f1= 0; _temp4682.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp4683= yyvs; struct _xtunion_struct** _temp4685= _temp4683.curr +(
yyvsp_offset - 2); if( _temp4683.base == 0? 1:( _temp4685 < _temp4683.base? 1:
_temp4685 >= _temp4683.last_plus_one)){ _throw( Null_Exception);}* _temp4685;})));
_temp4682;}); _temp4681;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4686= yyls; struct Cyc_Yyltype* _temp4688= _temp4686.curr +( yylsp_offset -
3); if( _temp4686.base == 0? 1:( _temp4688 < _temp4686.base? 1: _temp4688 >=
_temp4686.last_plus_one)){ _throw( Null_Exception);}* _temp4688;}).first_line,({
struct _tagged_ptr3 _temp4689= yyls; struct Cyc_Yyltype* _temp4691= _temp4689.curr
+ yylsp_offset; if( _temp4689.base == 0? 1:( _temp4691 < _temp4689.base? 1:
_temp4691 >= _temp4689.last_plus_one)){ _throw( Null_Exception);}* _temp4691;}).last_line));
_temp4680;}); _temp4679;}); break; case 178: _LL4678: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp4693=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4693[ 0]=({ struct Cyc_Exp_tok_struct
_temp4694; _temp4694.tag= Cyc_Exp_tok_tag; _temp4694.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Comprehension_e_struct* _temp4695=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp4695[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp4696; _temp4696.tag= Cyc_Absyn_Comprehension_e_tag;
_temp4696.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp4697=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp4697->f1=( void*) Cyc_Absyn_Loc_n;
_temp4697->f2=({ struct _tagged_string* _temp4698=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4698[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4699= yyvs; struct _xtunion_struct** _temp4701=
_temp4699.curr +( yyvsp_offset - 5); if( _temp4699.base == 0? 1:( _temp4701 <
_temp4699.base? 1: _temp4701 >= _temp4699.last_plus_one)){ _throw(
Null_Exception);}* _temp4701;})); _temp4698;}); _temp4697;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4702= yyls; struct Cyc_Yyltype* _temp4704= _temp4702.curr
+( yylsp_offset - 5); if( _temp4702.base == 0? 1:( _temp4704 < _temp4702.base? 1:
_temp4704 >= _temp4702.last_plus_one)){ _throw( Null_Exception);}* _temp4704;}).first_line,({
struct _tagged_ptr3 _temp4705= yyls; struct Cyc_Yyltype* _temp4707= _temp4705.curr
+( yylsp_offset - 5); if( _temp4705.base == 0? 1:( _temp4707 < _temp4705.base? 1:
_temp4707 >= _temp4705.last_plus_one)){ _throw( Null_Exception);}* _temp4707;}).last_line)));
_temp4696.f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4708= yyvs; struct
_xtunion_struct** _temp4710= _temp4708.curr +( yyvsp_offset - 3); if( _temp4708.base
== 0? 1:( _temp4710 < _temp4708.base? 1: _temp4710 >= _temp4708.last_plus_one)){
_throw( Null_Exception);}* _temp4710;})); _temp4696.f3= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4711= yyvs; struct _xtunion_struct** _temp4713=
_temp4711.curr +( yyvsp_offset - 1); if( _temp4711.base == 0? 1:( _temp4713 <
_temp4711.base? 1: _temp4713 >= _temp4711.last_plus_one)){ _throw(
Null_Exception);}* _temp4713;})); _temp4696;}); _temp4695;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4714= yyls; struct Cyc_Yyltype* _temp4716= _temp4714.curr
+( yylsp_offset - 7); if( _temp4714.base == 0? 1:( _temp4716 < _temp4714.base? 1:
_temp4716 >= _temp4714.last_plus_one)){ _throw( Null_Exception);}* _temp4716;}).first_line,({
struct _tagged_ptr3 _temp4717= yyls; struct Cyc_Yyltype* _temp4719= _temp4717.curr
+ yylsp_offset; if( _temp4717.base == 0? 1:( _temp4719 < _temp4717.base? 1:
_temp4719 >= _temp4717.last_plus_one)){ _throw( Null_Exception);}* _temp4719;}).last_line));
_temp4694;}); _temp4693;}); break; case 179: _LL4692: yyval=( struct
_xtunion_struct*)({ struct Cyc_InitializerList_tok_struct* _temp4721=( struct
Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp4721[ 0]=({ struct Cyc_InitializerList_tok_struct _temp4722; _temp4722.tag=
Cyc_InitializerList_tok_tag; _temp4722.f1=({ struct Cyc_List_List* _temp4723=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4723->hd=(
void*)({ struct _tuple18* _temp4724=( struct _tuple18*) GC_malloc( sizeof(
struct _tuple18)); _temp4724->f1= 0; _temp4724->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4725= yyvs; struct _xtunion_struct** _temp4727= _temp4725.curr
+ yyvsp_offset; if( _temp4725.base == 0? 1:( _temp4727 < _temp4725.base? 1:
_temp4727 >= _temp4725.last_plus_one)){ _throw( Null_Exception);}* _temp4727;}));
_temp4724;}); _temp4723->tl= 0; _temp4723;}); _temp4722;}); _temp4721;}); break;
case 180: _LL4720: yyval=( struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct*
_temp4729=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4729[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4730; _temp4730.tag= Cyc_InitializerList_tok_tag; _temp4730.f1=({ struct
Cyc_List_List* _temp4731=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4731->hd=( void*)({ struct _tuple18* _temp4732=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4732->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4736= yyvs; struct _xtunion_struct** _temp4738=
_temp4736.curr +( yyvsp_offset - 1); if( _temp4736.base == 0? 1:( _temp4738 <
_temp4736.base? 1: _temp4738 >= _temp4736.last_plus_one)){ _throw(
Null_Exception);}* _temp4738;})); _temp4732->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4733= yyvs; struct _xtunion_struct** _temp4735= _temp4733.curr
+ yyvsp_offset; if( _temp4733.base == 0? 1:( _temp4735 < _temp4733.base? 1:
_temp4735 >= _temp4733.last_plus_one)){ _throw( Null_Exception);}* _temp4735;}));
_temp4732;}); _temp4731->tl= 0; _temp4731;}); _temp4730;}); _temp4729;}); break;
case 181: _LL4728: yyval=( struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct*
_temp4740=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4740[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4741; _temp4741.tag= Cyc_InitializerList_tok_tag; _temp4741.f1=({ struct
Cyc_List_List* _temp4742=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4742->hd=( void*)({ struct _tuple18* _temp4746=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4746->f1= 0; _temp4746->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4747= yyvs; struct _xtunion_struct** _temp4749=
_temp4747.curr + yyvsp_offset; if( _temp4747.base == 0? 1:( _temp4749 <
_temp4747.base? 1: _temp4749 >= _temp4747.last_plus_one)){ _throw(
Null_Exception);}* _temp4749;})); _temp4746;}); _temp4742->tl= Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4743= yyvs; struct _xtunion_struct** _temp4745=
_temp4743.curr +( yyvsp_offset - 2); if( _temp4743.base == 0? 1:( _temp4745 <
_temp4743.base? 1: _temp4745 >= _temp4743.last_plus_one)){ _throw(
Null_Exception);}* _temp4745;})); _temp4742;}); _temp4741;}); _temp4740;});
break; case 182: _LL4739: yyval=( struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct*
_temp4751=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4751[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4752; _temp4752.tag= Cyc_InitializerList_tok_tag; _temp4752.f1=({ struct
Cyc_List_List* _temp4753=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4753->hd=( void*)({ struct _tuple18* _temp4757=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4757->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4761= yyvs; struct _xtunion_struct** _temp4763=
_temp4761.curr +( yyvsp_offset - 1); if( _temp4761.base == 0? 1:( _temp4763 <
_temp4761.base? 1: _temp4763 >= _temp4761.last_plus_one)){ _throw(
Null_Exception);}* _temp4763;})); _temp4757->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4758= yyvs; struct _xtunion_struct** _temp4760= _temp4758.curr
+ yyvsp_offset; if( _temp4758.base == 0? 1:( _temp4760 < _temp4758.base? 1:
_temp4760 >= _temp4758.last_plus_one)){ _throw( Null_Exception);}* _temp4760;}));
_temp4757;}); _temp4753->tl= Cyc_yyget_InitializerList_tok(({ struct
_tagged_ptr2 _temp4754= yyvs; struct _xtunion_struct** _temp4756= _temp4754.curr
+( yyvsp_offset - 3); if( _temp4754.base == 0? 1:( _temp4756 < _temp4754.base? 1:
_temp4756 >= _temp4754.last_plus_one)){ _throw( Null_Exception);}* _temp4756;}));
_temp4753;}); _temp4752;}); _temp4751;}); break; case 183: _LL4750: yyval=(
struct _xtunion_struct*)({ struct Cyc_DesignatorList_tok_struct* _temp4765=(
struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4765[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4766; _temp4766.tag=
Cyc_DesignatorList_tok_tag; _temp4766.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2
_temp4767= yyvs; struct _xtunion_struct** _temp4769= _temp4767.curr +(
yyvsp_offset - 1); if( _temp4767.base == 0? 1:( _temp4769 < _temp4767.base? 1:
_temp4769 >= _temp4767.last_plus_one)){ _throw( Null_Exception);}* _temp4769;})));
_temp4766;}); _temp4765;}); break; case 184: _LL4764: yyval=( struct
_xtunion_struct*)({ struct Cyc_DesignatorList_tok_struct* _temp4771=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp4771[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp4772; _temp4772.tag= Cyc_DesignatorList_tok_tag;
_temp4772.f1=({ struct Cyc_List_List* _temp4773=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4773->hd=( void*) Cyc_yyget_Designator_tok(({
struct _tagged_ptr2 _temp4774= yyvs; struct _xtunion_struct** _temp4776=
_temp4774.curr + yyvsp_offset; if( _temp4774.base == 0? 1:( _temp4776 <
_temp4774.base? 1: _temp4776 >= _temp4774.last_plus_one)){ _throw(
Null_Exception);}* _temp4776;})); _temp4773->tl= 0; _temp4773;}); _temp4772;});
_temp4771;}); break; case 185: _LL4770: yyval=( struct _xtunion_struct*)({
struct Cyc_DesignatorList_tok_struct* _temp4778=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp4778[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp4779; _temp4779.tag= Cyc_DesignatorList_tok_tag;
_temp4779.f1=({ struct Cyc_List_List* _temp4780=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4780->hd=( void*) Cyc_yyget_Designator_tok(({
struct _tagged_ptr2 _temp4784= yyvs; struct _xtunion_struct** _temp4786=
_temp4784.curr + yyvsp_offset; if( _temp4784.base == 0? 1:( _temp4786 <
_temp4784.base? 1: _temp4786 >= _temp4784.last_plus_one)){ _throw(
Null_Exception);}* _temp4786;})); _temp4780->tl= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4781= yyvs; struct _xtunion_struct** _temp4783=
_temp4781.curr +( yyvsp_offset - 1); if( _temp4781.base == 0? 1:( _temp4783 <
_temp4781.base? 1: _temp4783 >= _temp4781.last_plus_one)){ _throw(
Null_Exception);}* _temp4783;})); _temp4780;}); _temp4779;}); _temp4778;});
break; case 186: _LL4777: yyval=( struct _xtunion_struct*)({ struct Cyc_Designator_tok_struct*
_temp4788=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4788[ 0]=({ struct Cyc_Designator_tok_struct _temp4789; _temp4789.tag= Cyc_Designator_tok_tag;
_temp4789.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp4790=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp4790[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp4791; _temp4791.tag=
Cyc_Absyn_ArrayElement_tag; _temp4791.f1= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4792= yyvs; struct _xtunion_struct** _temp4794= _temp4792.curr
+( yyvsp_offset - 1); if( _temp4792.base == 0? 1:( _temp4794 < _temp4792.base? 1:
_temp4794 >= _temp4792.last_plus_one)){ _throw( Null_Exception);}* _temp4794;}));
_temp4791;}); _temp4790;})); _temp4789;}); _temp4788;}); break; case 187:
_LL4787: yyval=( struct _xtunion_struct*)({ struct Cyc_Designator_tok_struct*
_temp4796=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4796[ 0]=({ struct Cyc_Designator_tok_struct _temp4797; _temp4797.tag= Cyc_Designator_tok_tag;
_temp4797.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp4798=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp4798[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp4799; _temp4799.tag= Cyc_Absyn_FieldName_tag;
_temp4799.f1=({ struct _tagged_string* _temp4800=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4800[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4801= yyvs; struct _xtunion_struct** _temp4803=
_temp4801.curr + yyvsp_offset; if( _temp4801.base == 0? 1:( _temp4803 <
_temp4801.base? 1: _temp4803 >= _temp4801.last_plus_one)){ _throw(
Null_Exception);}* _temp4803;})); _temp4800;}); _temp4799;}); _temp4798;}));
_temp4797;}); _temp4796;}); break; case 188: _LL4795: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4805= yyvs; struct
_xtunion_struct** _temp4807= _temp4805.curr + yyvsp_offset; if( _temp4805.base
== 0? 1:( _temp4807 < _temp4805.base? 1: _temp4807 >= _temp4805.last_plus_one)){
_throw( Null_Exception);}* _temp4807;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4808= yyls; struct Cyc_Yyltype* _temp4810= _temp4808.curr
+ yylsp_offset; if( _temp4808.base == 0? 1:( _temp4810 < _temp4808.base? 1:
_temp4810 >= _temp4808.last_plus_one)){ _throw( Null_Exception);}* _temp4810;}).first_line,({
struct _tagged_ptr3 _temp4811= yyls; struct Cyc_Yyltype* _temp4813= _temp4811.curr
+ yylsp_offset; if( _temp4811.base == 0? 1:( _temp4813 < _temp4811.base? 1:
_temp4813 >= _temp4811.last_plus_one)){ _throw( Null_Exception);}* _temp4813;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4814= yyvs; struct _xtunion_struct** _temp4816= _temp4814.curr +
yyvsp_offset; if( _temp4814.base == 0? 1:( _temp4816 < _temp4814.base? 1:
_temp4816 >= _temp4814.last_plus_one)){ _throw( Null_Exception);}* _temp4816;}))).f3;
if( atts != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4817=( char*)"ignoring attributes in type";
struct _tagged_string _temp4818; _temp4818.curr= _temp4817; _temp4818.base=
_temp4817; _temp4818.last_plus_one= _temp4817 + 28; _temp4818;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4819= yyls; struct Cyc_Yyltype* _temp4821= _temp4819.curr
+ yylsp_offset; if( _temp4819.base == 0? 1:( _temp4821 < _temp4819.base? 1:
_temp4821 >= _temp4819.last_plus_one)){ _throw( Null_Exception);}* _temp4821;}).first_line,({
struct _tagged_ptr3 _temp4822= yyls; struct Cyc_Yyltype* _temp4824= _temp4822.curr
+ yylsp_offset; if( _temp4822.base == 0? 1:( _temp4824 < _temp4822.base? 1:
_temp4824 >= _temp4822.last_plus_one)){ _throw( Null_Exception);}* _temp4824;}).last_line));}{
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4825= yyvs; struct _xtunion_struct** _temp4827= _temp4825.curr +
yyvsp_offset; if( _temp4825.base == 0? 1:( _temp4827 < _temp4825.base? 1:
_temp4827 >= _temp4825.last_plus_one)){ _throw( Null_Exception);}* _temp4827;}))).f1;
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4828=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4828[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4829; _temp4829.tag= Cyc_ParamDecl_tok_tag;
_temp4829.f1=({ struct _tuple2* _temp4830=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2) * 1); _temp4830[ 0]=({ struct _tuple2 _temp4831; _temp4831.f1= 0;
_temp4831.f2= tq; _temp4831.f3= t; _temp4831;}); _temp4830;}); _temp4829;});
_temp4828;}); break;}} case 189: _LL4804: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4833= yyvs; struct
_xtunion_struct** _temp4835= _temp4833.curr +( yyvsp_offset - 1); if( _temp4833.base
== 0? 1:( _temp4835 < _temp4833.base? 1: _temp4835 >= _temp4833.last_plus_one)){
_throw( Null_Exception);}* _temp4835;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4836= yyls; struct Cyc_Yyltype* _temp4838= _temp4836.curr
+( yylsp_offset - 1); if( _temp4836.base == 0? 1:( _temp4838 < _temp4836.base? 1:
_temp4838 >= _temp4836.last_plus_one)){ _throw( Null_Exception);}* _temp4838;}).first_line,({
struct _tagged_ptr3 _temp4839= yyls; struct Cyc_Yyltype* _temp4841= _temp4839.curr
+( yylsp_offset - 1); if( _temp4839.base == 0? 1:( _temp4841 < _temp4839.base? 1:
_temp4841 >= _temp4839.last_plus_one)){ _throw( Null_Exception);}* _temp4841;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4842= yyvs; struct _xtunion_struct** _temp4844= _temp4842.curr +(
yyvsp_offset - 1); if( _temp4842.base == 0? 1:( _temp4844 < _temp4842.base? 1:
_temp4844 >= _temp4842.last_plus_one)){ _throw( Null_Exception);}* _temp4844;}))).f3;
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4845= yyvs; struct _xtunion_struct** _temp4847= _temp4845.curr +(
yyvsp_offset - 1); if( _temp4845.base == 0? 1:( _temp4847 < _temp4845.base? 1:
_temp4847 >= _temp4845.last_plus_one)){ _throw( Null_Exception);}* _temp4847;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4848= yyvs; struct _xtunion_struct** _temp4850= _temp4848.curr
+ yyvsp_offset; if( _temp4848.base == 0? 1:( _temp4850 < _temp4848.base? 1:
_temp4850 >= _temp4848.last_plus_one)){ _throw( Null_Exception);}* _temp4850;})))->tms;
struct _tuple6 t_info= Cyc_Parse_apply_tms( tq, t, atts, tms); if( t_info.f3 !=
0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4851=( char*)"bad type params, ignoring";
struct _tagged_string _temp4852; _temp4852.curr= _temp4851; _temp4852.base=
_temp4851; _temp4852.last_plus_one= _temp4851 + 26; _temp4852;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4853= yyls; struct Cyc_Yyltype* _temp4855= _temp4853.curr
+ yylsp_offset; if( _temp4853.base == 0? 1:( _temp4855 < _temp4853.base? 1:
_temp4855 >= _temp4853.last_plus_one)){ _throw( Null_Exception);}* _temp4855;}).first_line,({
struct _tagged_ptr3 _temp4856= yyls; struct Cyc_Yyltype* _temp4858= _temp4856.curr
+ yylsp_offset; if( _temp4856.base == 0? 1:( _temp4858 < _temp4856.base? 1:
_temp4858 >= _temp4856.last_plus_one)){ _throw( Null_Exception);}* _temp4858;}).last_line));}
if( t_info.f4 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4859=(
char*)"bad specifiers, ignoring"; struct _tagged_string _temp4860; _temp4860.curr=
_temp4859; _temp4860.base= _temp4859; _temp4860.last_plus_one= _temp4859 + 25;
_temp4860;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4861= yyls;
struct Cyc_Yyltype* _temp4863= _temp4861.curr + yylsp_offset; if( _temp4861.base
== 0? 1:( _temp4863 < _temp4861.base? 1: _temp4863 >= _temp4861.last_plus_one)){
_throw( Null_Exception);}* _temp4863;}).first_line,({ struct _tagged_ptr3
_temp4864= yyls; struct Cyc_Yyltype* _temp4866= _temp4864.curr + yylsp_offset;
if( _temp4864.base == 0? 1:( _temp4866 < _temp4864.base? 1: _temp4866 >=
_temp4864.last_plus_one)){ _throw( Null_Exception);}* _temp4866;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4867=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4867[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4868; _temp4868.tag= Cyc_ParamDecl_tok_tag;
_temp4868.f1=({ struct _tuple2* _temp4869=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4869->f1= 0; _temp4869->f2= t_info.f1; _temp4869->f3=
t_info.f2; _temp4869;}); _temp4868;}); _temp4867;}); break;} case 190: _LL4832:
yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct* _temp4871=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4871[ 0]=({ struct Cyc_Type_tok_struct _temp4872; _temp4872.tag= Cyc_Type_tok_tag;
_temp4872.f1=( void*)(* Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4873=
yyvs; struct _xtunion_struct** _temp4875= _temp4873.curr + yyvsp_offset; if(
_temp4873.base == 0? 1:( _temp4875 < _temp4873.base? 1: _temp4875 >= _temp4873.last_plus_one)){
_throw( Null_Exception);}* _temp4875;}))).f3; _temp4872;}); _temp4871;}); break;
case 191: _LL4870: yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct*
_temp4877=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4877[ 0]=({ struct Cyc_Type_tok_struct _temp4878; _temp4878.tag= Cyc_Type_tok_tag;
_temp4878.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4879=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4879[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4880; _temp4880.tag= Cyc_Absyn_JoinEff_tag;
_temp4880.f1= 0; _temp4880;}); _temp4879;})); _temp4878;}); _temp4877;}); break;
case 192: _LL4876: yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct*
_temp4882=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4882[ 0]=({ struct Cyc_Type_tok_struct _temp4883; _temp4883.tag= Cyc_Type_tok_tag;
_temp4883.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4884=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4884[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4885; _temp4885.tag= Cyc_Absyn_JoinEff_tag;
_temp4885.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4886= yyvs;
struct _xtunion_struct** _temp4888= _temp4886.curr +( yyvsp_offset - 1); if(
_temp4886.base == 0? 1:( _temp4888 < _temp4886.base? 1: _temp4888 >= _temp4886.last_plus_one)){
_throw( Null_Exception);}* _temp4888;})); _temp4885;}); _temp4884;})); _temp4883;});
_temp4882;}); break; case 193: _LL4881: yyval=( struct _xtunion_struct*)({
struct Cyc_Type_tok_struct* _temp4890=( struct Cyc_Type_tok_struct*) GC_malloc(
sizeof( struct Cyc_Type_tok_struct)); _temp4890[ 0]=({ struct Cyc_Type_tok_struct
_temp4891; _temp4891.tag= Cyc_Type_tok_tag; _temp4891.f1=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp4892=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4892[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4893; _temp4893.tag= Cyc_Absyn_JoinEff_tag;
_temp4893.f1=({ struct Cyc_List_List* _temp4894=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4894->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4898= yyvs; struct _xtunion_struct** _temp4900=
_temp4898.curr +( yyvsp_offset - 2); if( _temp4898.base == 0? 1:( _temp4900 <
_temp4898.base? 1: _temp4900 >= _temp4898.last_plus_one)){ _throw(
Null_Exception);}* _temp4900;})); _temp4894->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4895= yyvs; struct _xtunion_struct** _temp4897=
_temp4895.curr + yyvsp_offset; if( _temp4895.base == 0? 1:( _temp4897 <
_temp4895.base? 1: _temp4897 >= _temp4895.last_plus_one)){ _throw(
Null_Exception);}* _temp4897;})); _temp4894;}); _temp4893;}); _temp4892;}));
_temp4891;}); _temp4890;}); break; case 194: _LL4889: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4902=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4902[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4903; _temp4903.tag= Cyc_TypeList_tok_tag; _temp4903.f1=({ struct Cyc_List_List*
_temp4904=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4904->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4905= yyvs;
struct _xtunion_struct** _temp4907= _temp4905.curr + yyvsp_offset; if( _temp4905.base
== 0? 1:( _temp4907 < _temp4905.base? 1: _temp4907 >= _temp4905.last_plus_one)){
_throw( Null_Exception);}* _temp4907;})); _temp4904->tl= 0; _temp4904;});
_temp4903;}); _temp4902;}); break; case 195: _LL4901: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4909=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4909[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4910; _temp4910.tag= Cyc_TypeList_tok_tag; _temp4910.f1=({ struct Cyc_List_List*
_temp4911=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4911->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4915= yyvs;
struct _xtunion_struct** _temp4917= _temp4915.curr + yyvsp_offset; if( _temp4915.base
== 0? 1:( _temp4917 < _temp4915.base? 1: _temp4917 >= _temp4915.last_plus_one)){
_throw( Null_Exception);}* _temp4917;})); _temp4911->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4912= yyvs; struct _xtunion_struct** _temp4914=
_temp4912.curr +( yyvsp_offset - 2); if( _temp4912.base == 0? 1:( _temp4914 <
_temp4912.base? 1: _temp4914 >= _temp4912.last_plus_one)){ _throw(
Null_Exception);}* _temp4914;})); _temp4911;}); _temp4910;}); _temp4909;});
break; case 196: _LL4908: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4919=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4919[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4920; _temp4920.tag= Cyc_AbstractDeclarator_tok_tag; _temp4920.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4921=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4921->tms= Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp4922= yyvs; struct _xtunion_struct** _temp4924=
_temp4922.curr + yyvsp_offset; if( _temp4922.base == 0? 1:( _temp4924 <
_temp4922.base? 1: _temp4924 >= _temp4922.last_plus_one)){ _throw(
Null_Exception);}* _temp4924;})); _temp4921;}); _temp4920;}); _temp4919;});
break; case 197: _LL4918: yyval=({ struct _tagged_ptr2 _temp4926= yyvs; struct
_xtunion_struct** _temp4928= _temp4926.curr + yyvsp_offset; if( _temp4926.base
== 0? 1:( _temp4928 < _temp4926.base? 1: _temp4928 >= _temp4926.last_plus_one)){
_throw( Null_Exception);}* _temp4928;}); break; case 198: _LL4925: yyval=(
struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4930=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4930[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4931; _temp4931.tag=
Cyc_AbstractDeclarator_tok_tag; _temp4931.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4932=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4932->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2
_temp4933= yyvs; struct _xtunion_struct** _temp4935= _temp4933.curr +(
yyvsp_offset - 1); if( _temp4933.base == 0? 1:( _temp4935 < _temp4933.base? 1:
_temp4935 >= _temp4933.last_plus_one)){ _throw( Null_Exception);}* _temp4935;})),(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4936= yyvs; struct
_xtunion_struct** _temp4938= _temp4936.curr + yyvsp_offset; if( _temp4936.base
== 0? 1:( _temp4938 < _temp4936.base? 1: _temp4938 >= _temp4936.last_plus_one)){
_throw( Null_Exception);}* _temp4938;})))->tms); _temp4932;}); _temp4931;});
_temp4930;}); break; case 199: _LL4929: yyval=({ struct _tagged_ptr2 _temp4940=
yyvs; struct _xtunion_struct** _temp4942= _temp4940.curr +( yyvsp_offset - 1);
if( _temp4940.base == 0? 1:( _temp4942 < _temp4940.base? 1: _temp4942 >=
_temp4940.last_plus_one)){ _throw( Null_Exception);}* _temp4942;}); break; case
200: _LL4939: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4944=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4944[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4945; _temp4945.tag= Cyc_AbstractDeclarator_tok_tag; _temp4945.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4946=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4946->tms=({
struct Cyc_List_List* _temp4947=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4947->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4947->tl= 0; _temp4947;}); _temp4946;}); _temp4945;}); _temp4944;}); break;
case 201: _LL4943: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4949=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4949[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4950; _temp4950.tag= Cyc_AbstractDeclarator_tok_tag; _temp4950.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4951=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4951->tms=({
struct Cyc_List_List* _temp4952=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4952->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4952->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4953= yyvs; struct _xtunion_struct** _temp4955= _temp4953.curr +(
yyvsp_offset - 2); if( _temp4953.base == 0? 1:( _temp4955 < _temp4953.base? 1:
_temp4955 >= _temp4953.last_plus_one)){ _throw( Null_Exception);}* _temp4955;})))->tms;
_temp4952;}); _temp4951;}); _temp4950;}); _temp4949;}); break; case 202: _LL4948:
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4957=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4957[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4958; _temp4958.tag= Cyc_AbstractDeclarator_tok_tag; _temp4958.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4959=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4959->tms=({
struct Cyc_List_List* _temp4960=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4960->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4961=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4961[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp4962; _temp4962.tag= Cyc_Absyn_ConstArray_mod_tag; _temp4962.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4963= yyvs; struct _xtunion_struct** _temp4965=
_temp4963.curr +( yyvsp_offset - 1); if( _temp4963.base == 0? 1:( _temp4965 <
_temp4963.base? 1: _temp4965 >= _temp4963.last_plus_one)){ _throw(
Null_Exception);}* _temp4965;})); _temp4962;}); _temp4961;})); _temp4960->tl= 0;
_temp4960;}); _temp4959;}); _temp4958;}); _temp4957;}); break; case 203: _LL4956:
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4967=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4967[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4968; _temp4968.tag= Cyc_AbstractDeclarator_tok_tag; _temp4968.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4969=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4969->tms=({
struct Cyc_List_List* _temp4970=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4970->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4974=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4974[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp4975; _temp4975.tag= Cyc_Absyn_ConstArray_mod_tag; _temp4975.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4976= yyvs; struct _xtunion_struct** _temp4978=
_temp4976.curr +( yyvsp_offset - 1); if( _temp4976.base == 0? 1:( _temp4978 <
_temp4976.base? 1: _temp4978 >= _temp4976.last_plus_one)){ _throw(
Null_Exception);}* _temp4978;})); _temp4975;}); _temp4974;})); _temp4970->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4971= yyvs; struct
_xtunion_struct** _temp4973= _temp4971.curr +( yyvsp_offset - 3); if( _temp4971.base
== 0? 1:( _temp4973 < _temp4971.base? 1: _temp4973 >= _temp4971.last_plus_one)){
_throw( Null_Exception);}* _temp4973;})))->tms; _temp4970;}); _temp4969;});
_temp4968;}); _temp4967;}); break; case 204: _LL4966: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4980=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4980[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4981; _temp4981.tag=
Cyc_AbstractDeclarator_tok_tag; _temp4981.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4982=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4982->tms=({ struct Cyc_List_List* _temp4983=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4983->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4984=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4984[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4985; _temp4985.tag= Cyc_Absyn_Function_mod_tag;
_temp4985.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp4986=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4986[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4987; _temp4987.tag= Cyc_Absyn_WithTypes_tag;
_temp4987.f1= 0; _temp4987.f2= 0; _temp4987.f3= 0; _temp4987;}); _temp4986;}));
_temp4985;}); _temp4984;})); _temp4983->tl= 0; _temp4983;}); _temp4982;});
_temp4981;}); _temp4980;}); break; case 205: _LL4979: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4989=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4989[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4990; _temp4990.tag=
Cyc_AbstractDeclarator_tok_tag; _temp4990.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4991=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4991->tms=({ struct Cyc_List_List* _temp4992=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4992->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4993=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4993[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4994; _temp4994.tag= Cyc_Absyn_Function_mod_tag;
_temp4994.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp4995=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4995[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4996; _temp4996.tag= Cyc_Absyn_WithTypes_tag;
_temp4996.f1= 0; _temp4996.f2= 0; _temp4996.f3=({ struct Cyc_Core_Opt* _temp4997=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4997->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4998=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4998[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4999; _temp4999.tag= Cyc_Absyn_JoinEff_tag;
_temp4999.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5000= yyvs;
struct _xtunion_struct** _temp5002= _temp5000.curr +( yyvsp_offset - 1); if(
_temp5000.base == 0? 1:( _temp5002 < _temp5000.base? 1: _temp5002 >= _temp5000.last_plus_one)){
_throw( Null_Exception);}* _temp5002;})); _temp4999;}); _temp4998;})); _temp4997;});
_temp4996;}); _temp4995;})); _temp4994;}); _temp4993;})); _temp4992->tl= 0;
_temp4992;}); _temp4991;}); _temp4990;}); _temp4989;}); break; case 206: _LL4988: {
struct _tuple16 _temp5009; struct Cyc_Core_Opt* _temp5010; int _temp5012; struct
Cyc_List_List* _temp5014; struct _tuple16* _temp5007= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp5004= yyvs; struct _xtunion_struct** _temp5006=
_temp5004.curr +( yyvsp_offset - 1); if( _temp5004.base == 0? 1:( _temp5006 <
_temp5004.base? 1: _temp5006 >= _temp5004.last_plus_one)){ _throw(
Null_Exception);}* _temp5006;})); _temp5009=* _temp5007; _LL5015: _temp5014=
_temp5009.f1; goto _LL5013; _LL5013: _temp5012= _temp5009.f2; goto _LL5011;
_LL5011: _temp5010= _temp5009.f3; goto _LL5008; _LL5008: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5016=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5016[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5017; _temp5017.tag=
Cyc_AbstractDeclarator_tok_tag; _temp5017.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5018=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5018->tms=({ struct Cyc_List_List* _temp5019=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5019->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5020=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5020[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5021; _temp5021.tag= Cyc_Absyn_Function_mod_tag;
_temp5021.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5022=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5022[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5023; _temp5023.tag= Cyc_Absyn_WithTypes_tag;
_temp5023.f1= _temp5014; _temp5023.f2= _temp5012; _temp5023.f3= _temp5010;
_temp5023;}); _temp5022;})); _temp5021;}); _temp5020;})); _temp5019->tl= 0;
_temp5019;}); _temp5018;}); _temp5017;}); _temp5016;}); break;} case 207:
_LL5003: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5025=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5025[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5026; _temp5026.tag= Cyc_AbstractDeclarator_tok_tag; _temp5026.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp5027=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5027->tms=({
struct Cyc_List_List* _temp5028=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5028->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp5032=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp5032[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp5033; _temp5033.tag=
Cyc_Absyn_Function_mod_tag; _temp5033.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp5034=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5034[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5035; _temp5035.tag= Cyc_Absyn_WithTypes_tag;
_temp5035.f1= 0; _temp5035.f2= 0; _temp5035.f3= 0; _temp5035;}); _temp5034;}));
_temp5033;}); _temp5032;})); _temp5028->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5029= yyvs; struct _xtunion_struct** _temp5031=
_temp5029.curr +( yyvsp_offset - 2); if( _temp5029.base == 0? 1:( _temp5031 <
_temp5029.base? 1: _temp5031 >= _temp5029.last_plus_one)){ _throw(
Null_Exception);}* _temp5031;})))->tms; _temp5028;}); _temp5027;}); _temp5026;});
_temp5025;}); break; case 208: _LL5024: { struct Cyc_Core_Opt* eff=({ struct Cyc_Core_Opt*
_temp5037=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5037->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp5038=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp5038[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp5039; _temp5039.tag= Cyc_Absyn_JoinEff_tag;
_temp5039.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5040= yyvs;
struct _xtunion_struct** _temp5042= _temp5040.curr +( yyvsp_offset - 1); if(
_temp5040.base == 0? 1:( _temp5042 < _temp5040.base? 1: _temp5042 >= _temp5040.last_plus_one)){
_throw( Null_Exception);}* _temp5042;})); _temp5039;}); _temp5038;})); _temp5037;});
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5043=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5043[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5044; _temp5044.tag= Cyc_AbstractDeclarator_tok_tag; _temp5044.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp5045=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5045->tms=({
struct Cyc_List_List* _temp5046=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5046->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp5050=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp5050[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp5051; _temp5051.tag=
Cyc_Absyn_Function_mod_tag; _temp5051.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp5052=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5052[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5053; _temp5053.tag= Cyc_Absyn_WithTypes_tag;
_temp5053.f1= 0; _temp5053.f2= 0; _temp5053.f3= eff; _temp5053;}); _temp5052;}));
_temp5051;}); _temp5050;})); _temp5046->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5047= yyvs; struct _xtunion_struct** _temp5049=
_temp5047.curr +( yyvsp_offset - 4); if( _temp5047.base == 0? 1:( _temp5049 <
_temp5047.base? 1: _temp5049 >= _temp5047.last_plus_one)){ _throw(
Null_Exception);}* _temp5049;})))->tms; _temp5046;}); _temp5045;}); _temp5044;});
_temp5043;}); break;} case 209: _LL5036: { struct _tuple16 _temp5060; struct Cyc_Core_Opt*
_temp5061; int _temp5063; struct Cyc_List_List* _temp5065; struct _tuple16*
_temp5058= Cyc_yyget_ParamDeclListBool_tok(({ struct _tagged_ptr2 _temp5055=
yyvs; struct _xtunion_struct** _temp5057= _temp5055.curr +( yyvsp_offset - 1);
if( _temp5055.base == 0? 1:( _temp5057 < _temp5055.base? 1: _temp5057 >=
_temp5055.last_plus_one)){ _throw( Null_Exception);}* _temp5057;})); _temp5060=*
_temp5058; _LL5066: _temp5065= _temp5060.f1; goto _LL5064; _LL5064: _temp5063=
_temp5060.f2; goto _LL5062; _LL5062: _temp5061= _temp5060.f3; goto _LL5059;
_LL5059: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5067=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5067[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5068; _temp5068.tag= Cyc_AbstractDeclarator_tok_tag; _temp5068.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp5069=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5069->tms=({
struct Cyc_List_List* _temp5070=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5070->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp5074=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp5074[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp5075; _temp5075.tag=
Cyc_Absyn_Function_mod_tag; _temp5075.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp5076=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5076[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5077; _temp5077.tag= Cyc_Absyn_WithTypes_tag;
_temp5077.f1= _temp5065; _temp5077.f2= _temp5063; _temp5077.f3= _temp5061;
_temp5077;}); _temp5076;})); _temp5075;}); _temp5074;})); _temp5070->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5071= yyvs; struct _xtunion_struct** _temp5073=
_temp5071.curr +( yyvsp_offset - 3); if( _temp5071.base == 0? 1:( _temp5073 <
_temp5071.base? 1: _temp5073 >= _temp5071.last_plus_one)){ _throw(
Null_Exception);}* _temp5073;})))->tms; _temp5070;}); _temp5069;}); _temp5068;});
_temp5067;}); break;} case 210: _LL5054: { struct Cyc_List_List* ts=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5079=
yyls; struct Cyc_Yyltype* _temp5081= _temp5079.curr +( yylsp_offset - 2); if(
_temp5079.base == 0? 1:( _temp5081 < _temp5079.base? 1: _temp5081 >= _temp5079.last_plus_one)){
_throw( Null_Exception);}* _temp5081;}).first_line,({ struct _tagged_ptr3
_temp5082= yyls; struct Cyc_Yyltype* _temp5084= _temp5082.curr + yylsp_offset;
if( _temp5082.base == 0? 1:( _temp5084 < _temp5082.base? 1: _temp5084 >=
_temp5082.last_plus_one)){ _throw( Null_Exception);}* _temp5084;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5085= yyvs; struct _xtunion_struct** _temp5087=
_temp5085.curr +( yyvsp_offset - 1); if( _temp5085.base == 0? 1:( _temp5087 <
_temp5085.base? 1: _temp5087 >= _temp5085.last_plus_one)){ _throw(
Null_Exception);}* _temp5087;})))); yyval=( struct _xtunion_struct*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp5088=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5088[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5089; _temp5089.tag= Cyc_AbstractDeclarator_tok_tag;
_temp5089.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5090=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5090->tms=({
struct Cyc_List_List* _temp5091=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5091->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp5095=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp5095[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp5096; _temp5096.tag= Cyc_Absyn_TypeParams_mod_tag; _temp5096.f1= ts;
_temp5096.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5097= yyls;
struct Cyc_Yyltype* _temp5099= _temp5097.curr +( yylsp_offset - 2); if(
_temp5097.base == 0? 1:( _temp5099 < _temp5097.base? 1: _temp5099 >= _temp5097.last_plus_one)){
_throw( Null_Exception);}* _temp5099;}).first_line,({ struct _tagged_ptr3
_temp5100= yyls; struct Cyc_Yyltype* _temp5102= _temp5100.curr + yylsp_offset;
if( _temp5100.base == 0? 1:( _temp5102 < _temp5100.base? 1: _temp5102 >=
_temp5100.last_plus_one)){ _throw( Null_Exception);}* _temp5102;}).last_line);
_temp5096.f3= 0; _temp5096;}); _temp5095;})); _temp5091->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5092= yyvs; struct _xtunion_struct** _temp5094=
_temp5092.curr +( yyvsp_offset - 3); if( _temp5092.base == 0? 1:( _temp5094 <
_temp5092.base? 1: _temp5094 >= _temp5092.last_plus_one)){ _throw(
Null_Exception);}* _temp5094;})))->tms; _temp5091;}); _temp5090;}); _temp5089;});
_temp5088;}); break;} case 211: _LL5078:(( struct Cyc_Lexing_lexbuf*)({ struct
Cyc_Core_Opt* _temp5104= Cyc_Parse_lbuf; if( _temp5104 == 0){ _throw(
Null_Exception);} _temp5104->v;}))->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5105=
yyls; struct Cyc_Yyltype* _temp5107= _temp5105.curr +( yylsp_offset - 2); if(
_temp5105.base == 0? 1:( _temp5107 < _temp5105.base? 1: _temp5107 >= _temp5105.last_plus_one)){
_throw( Null_Exception);}* _temp5107;}).first_line,({ struct _tagged_ptr3
_temp5108= yyls; struct Cyc_Yyltype* _temp5110= _temp5108.curr + yylsp_offset;
if( _temp5108.base == 0? 1:( _temp5110 < _temp5108.base? 1: _temp5110 >=
_temp5108.last_plus_one)){ _throw( Null_Exception);}* _temp5110;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5111= yyvs; struct _xtunion_struct** _temp5113=
_temp5111.curr +( yyvsp_offset - 1); if( _temp5111.base == 0? 1:( _temp5113 <
_temp5111.base? 1: _temp5113 >= _temp5111.last_plus_one)){ _throw(
Null_Exception);}* _temp5113;})))); yyval=( struct _xtunion_struct*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp5114=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5114[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5115; _temp5115.tag= Cyc_AbstractDeclarator_tok_tag;
_temp5115.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5116=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5116->tms=({
struct Cyc_List_List* _temp5117=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5117->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp5121=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp5121[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp5122; _temp5122.tag= Cyc_Absyn_TypeParams_mod_tag; _temp5122.f1= ts;
_temp5122.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5123= yyls;
struct Cyc_Yyltype* _temp5125= _temp5123.curr +( yylsp_offset - 2); if(
_temp5123.base == 0? 1:( _temp5125 < _temp5123.base? 1: _temp5125 >= _temp5123.last_plus_one)){
_throw( Null_Exception);}* _temp5125;}).first_line,({ struct _tagged_ptr3
_temp5126= yyls; struct Cyc_Yyltype* _temp5128= _temp5126.curr + yylsp_offset;
if( _temp5126.base == 0? 1:( _temp5128 < _temp5126.base? 1: _temp5128 >=
_temp5126.last_plus_one)){ _throw( Null_Exception);}* _temp5128;}).last_line);
_temp5122.f3= 0; _temp5122;}); _temp5121;})); _temp5117->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5118= yyvs; struct _xtunion_struct** _temp5120=
_temp5118.curr +( yyvsp_offset - 3); if( _temp5118.base == 0? 1:( _temp5120 <
_temp5118.base? 1: _temp5120 >= _temp5118.last_plus_one)){ _throw(
Null_Exception);}* _temp5120;})))->tms; _temp5117;}); _temp5116;}); _temp5115;});
_temp5114;}); break;} case 212: _LL5103: yyval=( struct _xtunion_struct*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp5130=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5130[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5131; _temp5131.tag= Cyc_AbstractDeclarator_tok_tag;
_temp5131.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5132=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5132->tms=({
struct Cyc_List_List* _temp5133=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5133->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp5137=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp5137[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp5138; _temp5138.tag= Cyc_Absyn_Attributes_mod_tag; _temp5138.f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5139= yyls; struct Cyc_Yyltype* _temp5141= _temp5139.curr
+ yylsp_offset; if( _temp5139.base == 0? 1:( _temp5141 < _temp5139.base? 1:
_temp5141 >= _temp5139.last_plus_one)){ _throw( Null_Exception);}* _temp5141;}).first_line,({
struct _tagged_ptr3 _temp5142= yyls; struct Cyc_Yyltype* _temp5144= _temp5142.curr
+ yylsp_offset; if( _temp5142.base == 0? 1:( _temp5144 < _temp5142.base? 1:
_temp5144 >= _temp5142.last_plus_one)){ _throw( Null_Exception);}* _temp5144;}).last_line);
_temp5138.f2= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp5145= yyvs;
struct _xtunion_struct** _temp5147= _temp5145.curr + yyvsp_offset; if( _temp5145.base
== 0? 1:( _temp5147 < _temp5145.base? 1: _temp5147 >= _temp5145.last_plus_one)){
_throw( Null_Exception);}* _temp5147;})); _temp5138;}); _temp5137;})); _temp5133->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp5134= yyvs; struct
_xtunion_struct** _temp5136= _temp5134.curr +( yyvsp_offset - 1); if( _temp5134.base
== 0? 1:( _temp5136 < _temp5134.base? 1: _temp5136 >= _temp5134.last_plus_one)){
_throw( Null_Exception);}* _temp5136;})))->tms; _temp5133;}); _temp5132;});
_temp5131;}); _temp5130;}); break; case 213: _LL5129: yyval=({ struct
_tagged_ptr2 _temp5149= yyvs; struct _xtunion_struct** _temp5151= _temp5149.curr
+ yyvsp_offset; if( _temp5149.base == 0? 1:( _temp5151 < _temp5149.base? 1:
_temp5151 >= _temp5149.last_plus_one)){ _throw( Null_Exception);}* _temp5151;});
break; case 214: _LL5148: yyval=({ struct _tagged_ptr2 _temp5153= yyvs; struct
_xtunion_struct** _temp5155= _temp5153.curr + yyvsp_offset; if( _temp5153.base
== 0? 1:( _temp5155 < _temp5153.base? 1: _temp5155 >= _temp5153.last_plus_one)){
_throw( Null_Exception);}* _temp5155;}); break; case 215: _LL5152: yyval=({
struct _tagged_ptr2 _temp5157= yyvs; struct _xtunion_struct** _temp5159=
_temp5157.curr + yyvsp_offset; if( _temp5157.base == 0? 1:( _temp5159 <
_temp5157.base? 1: _temp5159 >= _temp5157.last_plus_one)){ _throw(
Null_Exception);}* _temp5159;}); break; case 216: _LL5156: yyval=({ struct
_tagged_ptr2 _temp5161= yyvs; struct _xtunion_struct** _temp5163= _temp5161.curr
+ yyvsp_offset; if( _temp5161.base == 0? 1:( _temp5163 < _temp5161.base? 1:
_temp5163 >= _temp5161.last_plus_one)){ _throw( Null_Exception);}* _temp5163;});
break; case 217: _LL5160: yyval=({ struct _tagged_ptr2 _temp5165= yyvs; struct
_xtunion_struct** _temp5167= _temp5165.curr + yyvsp_offset; if( _temp5165.base
== 0? 1:( _temp5167 < _temp5165.base? 1: _temp5167 >= _temp5165.last_plus_one)){
_throw( Null_Exception);}* _temp5167;}); break; case 218: _LL5164: yyval=({
struct _tagged_ptr2 _temp5169= yyvs; struct _xtunion_struct** _temp5171=
_temp5169.curr + yyvsp_offset; if( _temp5169.base == 0? 1:( _temp5171 <
_temp5169.base? 1: _temp5171 >= _temp5169.last_plus_one)){ _throw(
Null_Exception);}* _temp5171;}); break; case 219: _LL5168: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5173=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5173[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5174; _temp5174.tag= Cyc_Stmt_tok_tag; _temp5174.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Cut_s_struct* _temp5175=( struct Cyc_Absyn_Cut_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct)); _temp5175[ 0]=({ struct Cyc_Absyn_Cut_s_struct
_temp5176; _temp5176.tag= Cyc_Absyn_Cut_s_tag; _temp5176.f1= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5177= yyvs; struct _xtunion_struct** _temp5179=
_temp5177.curr + yyvsp_offset; if( _temp5177.base == 0? 1:( _temp5179 <
_temp5177.base? 1: _temp5179 >= _temp5177.last_plus_one)){ _throw(
Null_Exception);}* _temp5179;})); _temp5176;}); _temp5175;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5180= yyls; struct Cyc_Yyltype* _temp5182= _temp5180.curr
+( yylsp_offset - 1); if( _temp5180.base == 0? 1:( _temp5182 < _temp5180.base? 1:
_temp5182 >= _temp5180.last_plus_one)){ _throw( Null_Exception);}* _temp5182;}).first_line,({
struct _tagged_ptr3 _temp5183= yyls; struct Cyc_Yyltype* _temp5185= _temp5183.curr
+ yylsp_offset; if( _temp5183.base == 0? 1:( _temp5185 < _temp5183.base? 1:
_temp5185 >= _temp5183.last_plus_one)){ _throw( Null_Exception);}* _temp5185;}).last_line));
_temp5174;}); _temp5173;}); break; case 220: _LL5172: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5187=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5187[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5188; _temp5188.tag= Cyc_Stmt_tok_tag; _temp5188.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Splice_s_struct* _temp5189=( struct Cyc_Absyn_Splice_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp5189[ 0]=({ struct
Cyc_Absyn_Splice_s_struct _temp5190; _temp5190.tag= Cyc_Absyn_Splice_s_tag;
_temp5190.f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5191= yyvs; struct
_xtunion_struct** _temp5193= _temp5191.curr + yyvsp_offset; if( _temp5191.base
== 0? 1:( _temp5193 < _temp5191.base? 1: _temp5193 >= _temp5191.last_plus_one)){
_throw( Null_Exception);}* _temp5193;})); _temp5190;}); _temp5189;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5194= yyls; struct Cyc_Yyltype* _temp5196= _temp5194.curr
+( yylsp_offset - 1); if( _temp5194.base == 0? 1:( _temp5196 < _temp5194.base? 1:
_temp5196 >= _temp5194.last_plus_one)){ _throw( Null_Exception);}* _temp5196;}).first_line,({
struct _tagged_ptr3 _temp5197= yyls; struct Cyc_Yyltype* _temp5199= _temp5197.curr
+ yylsp_offset; if( _temp5197.base == 0? 1:( _temp5199 < _temp5197.base? 1:
_temp5199 >= _temp5197.last_plus_one)){ _throw( Null_Exception);}* _temp5199;}).last_line));
_temp5188;}); _temp5187;}); break; case 221: _LL5186: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5201=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5201[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5202; _temp5202.tag= Cyc_Stmt_tok_tag; _temp5202.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Label_s_struct* _temp5203=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp5203[ 0]=({ struct
Cyc_Absyn_Label_s_struct _temp5204; _temp5204.tag= Cyc_Absyn_Label_s_tag;
_temp5204.f1=({ struct _tagged_string* _temp5205=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp5205[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp5206= yyvs; struct _xtunion_struct** _temp5208=
_temp5206.curr +( yyvsp_offset - 2); if( _temp5206.base == 0? 1:( _temp5208 <
_temp5206.base? 1: _temp5208 >= _temp5206.last_plus_one)){ _throw(
Null_Exception);}* _temp5208;})); _temp5205;}); _temp5204.f2= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5209= yyvs; struct _xtunion_struct** _temp5211=
_temp5209.curr + yyvsp_offset; if( _temp5209.base == 0? 1:( _temp5211 <
_temp5209.base? 1: _temp5211 >= _temp5209.last_plus_one)){ _throw(
Null_Exception);}* _temp5211;})); _temp5204;}); _temp5203;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5212= yyls; struct Cyc_Yyltype* _temp5214= _temp5212.curr
+( yylsp_offset - 2); if( _temp5212.base == 0? 1:( _temp5214 < _temp5212.base? 1:
_temp5214 >= _temp5212.last_plus_one)){ _throw( Null_Exception);}* _temp5214;}).first_line,({
struct _tagged_ptr3 _temp5215= yyls; struct Cyc_Yyltype* _temp5217= _temp5215.curr
+ yylsp_offset; if( _temp5215.base == 0? 1:( _temp5217 < _temp5215.base? 1:
_temp5217 >= _temp5215.last_plus_one)){ _throw( Null_Exception);}* _temp5217;}).last_line));
_temp5202;}); _temp5201;}); break; case 222: _LL5200: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5219=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5219[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5220; _temp5220.tag= Cyc_Stmt_tok_tag; _temp5220.f1= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5221= yyls; struct Cyc_Yyltype*
_temp5223= _temp5221.curr + yylsp_offset; if( _temp5221.base == 0? 1:( _temp5223
< _temp5221.base? 1: _temp5223 >= _temp5221.last_plus_one)){ _throw(
Null_Exception);}* _temp5223;}).first_line,({ struct _tagged_ptr3 _temp5224=
yyls; struct Cyc_Yyltype* _temp5226= _temp5224.curr + yylsp_offset; if(
_temp5224.base == 0? 1:( _temp5226 < _temp5224.base? 1: _temp5226 >= _temp5224.last_plus_one)){
_throw( Null_Exception);}* _temp5226;}).last_line)); _temp5220;}); _temp5219;});
break; case 223: _LL5218: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5228=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5228[ 0]=({ struct Cyc_Stmt_tok_struct _temp5229; _temp5229.tag= Cyc_Stmt_tok_tag;
_temp5229.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5230= yyvs; struct _xtunion_struct** _temp5232= _temp5230.curr +(
yyvsp_offset - 1); if( _temp5230.base == 0? 1:( _temp5232 < _temp5230.base? 1:
_temp5232 >= _temp5230.last_plus_one)){ _throw( Null_Exception);}* _temp5232;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5233= yyls; struct Cyc_Yyltype*
_temp5235= _temp5233.curr +( yylsp_offset - 1); if( _temp5233.base == 0? 1:(
_temp5235 < _temp5233.base? 1: _temp5235 >= _temp5233.last_plus_one)){ _throw(
Null_Exception);}* _temp5235;}).first_line,({ struct _tagged_ptr3 _temp5236=
yyls; struct Cyc_Yyltype* _temp5238= _temp5236.curr + yylsp_offset; if(
_temp5236.base == 0? 1:( _temp5238 < _temp5236.base? 1: _temp5238 >= _temp5236.last_plus_one)){
_throw( Null_Exception);}* _temp5238;}).last_line)); _temp5229;}); _temp5228;});
break; case 224: _LL5227: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5240=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5240[ 0]=({ struct Cyc_Stmt_tok_struct _temp5241; _temp5241.tag= Cyc_Stmt_tok_tag;
_temp5241.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5242= yyls; struct Cyc_Yyltype* _temp5244= _temp5242.curr +(
yylsp_offset - 1); if( _temp5242.base == 0? 1:( _temp5244 < _temp5242.base? 1:
_temp5244 >= _temp5242.last_plus_one)){ _throw( Null_Exception);}* _temp5244;}).first_line,({
struct _tagged_ptr3 _temp5245= yyls; struct Cyc_Yyltype* _temp5247= _temp5245.curr
+ yylsp_offset; if( _temp5245.base == 0? 1:( _temp5247 < _temp5245.base? 1:
_temp5247 >= _temp5245.last_plus_one)){ _throw( Null_Exception);}* _temp5247;}).last_line));
_temp5241;}); _temp5240;}); break; case 225: _LL5239: yyval=({ struct
_tagged_ptr2 _temp5249= yyvs; struct _xtunion_struct** _temp5251= _temp5249.curr
+( yyvsp_offset - 1); if( _temp5249.base == 0? 1:( _temp5251 < _temp5249.base? 1:
_temp5251 >= _temp5249.last_plus_one)){ _throw( Null_Exception);}* _temp5251;});
break; case 226: _LL5248:{ void* _temp5256= Cyc_yyget_BlockItem_tok(({ struct
_tagged_ptr2 _temp5253= yyvs; struct _xtunion_struct** _temp5255= _temp5253.curr
+ yyvsp_offset; if( _temp5253.base == 0? 1:( _temp5255 < _temp5253.base? 1:
_temp5255 >= _temp5253.last_plus_one)){ _throw( Null_Exception);}* _temp5255;}));
struct Cyc_List_List* _temp5264; struct Cyc_Absyn_Fndecl* _temp5266; struct Cyc_Absyn_Stmt*
_temp5268; _LL5258: if((( struct _tunion_struct*) _temp5256)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL5265: _temp5264=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp5256)->f1; goto _LL5259;} else{ goto _LL5260;} _LL5260: if((( struct
_tunion_struct*) _temp5256)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL5267: _temp5266=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp5256)->f1;
goto _LL5261;} else{ goto _LL5262;} _LL5262: if((( struct _tunion_struct*)
_temp5256)->tag == Cyc_Parse_Stmt_bl_tag){ _LL5269: _temp5268=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp5256)->f1; goto _LL5263;} else{ goto
_LL5257;} _LL5259: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5270=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5270[ 0]=({ struct Cyc_Stmt_tok_struct _temp5271; _temp5271.tag= Cyc_Stmt_tok_tag;
_temp5271.f1= Cyc_Parse_flatten_declarations( _temp5264, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5272= yyls; struct Cyc_Yyltype*
_temp5274= _temp5272.curr + yylsp_offset; if( _temp5272.base == 0? 1:( _temp5274
< _temp5272.base? 1: _temp5274 >= _temp5272.last_plus_one)){ _throw(
Null_Exception);}* _temp5274;}).first_line,({ struct _tagged_ptr3 _temp5275=
yyls; struct Cyc_Yyltype* _temp5277= _temp5275.curr + yylsp_offset; if(
_temp5275.base == 0? 1:( _temp5277 < _temp5275.base? 1: _temp5277 >= _temp5275.last_plus_one)){
_throw( Null_Exception);}* _temp5277;}).last_line))); _temp5271;}); _temp5270;});
goto _LL5257; _LL5261: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5278=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5278[ 0]=({ struct Cyc_Stmt_tok_struct _temp5279; _temp5279.tag= Cyc_Stmt_tok_tag;
_temp5279.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp5280=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5280[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5281; _temp5281.tag= Cyc_Absyn_Fn_d_tag;
_temp5281.f1= _temp5266; _temp5281;}); _temp5280;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5282= yyls; struct Cyc_Yyltype* _temp5284= _temp5282.curr
+ yylsp_offset; if( _temp5282.base == 0? 1:( _temp5284 < _temp5282.base? 1:
_temp5284 >= _temp5282.last_plus_one)){ _throw( Null_Exception);}* _temp5284;}).first_line,({
struct _tagged_ptr3 _temp5285= yyls; struct Cyc_Yyltype* _temp5287= _temp5285.curr
+ yylsp_offset; if( _temp5285.base == 0? 1:( _temp5287 < _temp5285.base? 1:
_temp5287 >= _temp5285.last_plus_one)){ _throw( Null_Exception);}* _temp5287;}).last_line)),
Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5288= yyls; struct Cyc_Yyltype* _temp5290= _temp5288.curr + yylsp_offset;
if( _temp5288.base == 0? 1:( _temp5290 < _temp5288.base? 1: _temp5290 >=
_temp5288.last_plus_one)){ _throw( Null_Exception);}* _temp5290;}).first_line,({
struct _tagged_ptr3 _temp5291= yyls; struct Cyc_Yyltype* _temp5293= _temp5291.curr
+ yylsp_offset; if( _temp5291.base == 0? 1:( _temp5293 < _temp5291.base? 1:
_temp5293 >= _temp5291.last_plus_one)){ _throw( Null_Exception);}* _temp5293;}).last_line)));
_temp5279;}); _temp5278;}); goto _LL5257; _LL5263: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5294=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5294[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5295; _temp5295.tag= Cyc_Stmt_tok_tag; _temp5295.f1= _temp5268; _temp5295;});
_temp5294;}); goto _LL5257; _LL5257:;} break; case 227: _LL5252:{ void*
_temp5300= Cyc_yyget_BlockItem_tok(({ struct _tagged_ptr2 _temp5297= yyvs;
struct _xtunion_struct** _temp5299= _temp5297.curr +( yyvsp_offset - 1); if(
_temp5297.base == 0? 1:( _temp5299 < _temp5297.base? 1: _temp5299 >= _temp5297.last_plus_one)){
_throw( Null_Exception);}* _temp5299;})); struct Cyc_List_List* _temp5308;
struct Cyc_Absyn_Fndecl* _temp5310; struct Cyc_Absyn_Stmt* _temp5312; _LL5302:
if((( struct _tunion_struct*) _temp5300)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL5309: _temp5308=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp5300)->f1; goto _LL5303;} else{ goto _LL5304;} _LL5304: if((( struct
_tunion_struct*) _temp5300)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL5311: _temp5310=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp5300)->f1;
goto _LL5305;} else{ goto _LL5306;} _LL5306: if((( struct _tunion_struct*)
_temp5300)->tag == Cyc_Parse_Stmt_bl_tag){ _LL5313: _temp5312=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp5300)->f1; goto _LL5307;} else{ goto
_LL5301;} _LL5303: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5314=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5314[ 0]=({ struct Cyc_Stmt_tok_struct _temp5315; _temp5315.tag= Cyc_Stmt_tok_tag;
_temp5315.f1= Cyc_Parse_flatten_declarations( _temp5308, Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5316= yyvs; struct _xtunion_struct** _temp5318=
_temp5316.curr + yyvsp_offset; if( _temp5316.base == 0? 1:( _temp5318 <
_temp5316.base? 1: _temp5318 >= _temp5316.last_plus_one)){ _throw(
Null_Exception);}* _temp5318;}))); _temp5315;}); _temp5314;}); goto _LL5301;
_LL5305: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5319=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5319[ 0]=({ struct Cyc_Stmt_tok_struct _temp5320; _temp5320.tag= Cyc_Stmt_tok_tag;
_temp5320.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp5321=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5321[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5322; _temp5322.tag= Cyc_Absyn_Fn_d_tag;
_temp5322.f1= _temp5310; _temp5322;}); _temp5321;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5323= yyls; struct Cyc_Yyltype* _temp5325= _temp5323.curr
+( yylsp_offset - 1); if( _temp5323.base == 0? 1:( _temp5325 < _temp5323.base? 1:
_temp5325 >= _temp5323.last_plus_one)){ _throw( Null_Exception);}* _temp5325;}).first_line,({
struct _tagged_ptr3 _temp5326= yyls; struct Cyc_Yyltype* _temp5328= _temp5326.curr
+( yylsp_offset - 1); if( _temp5326.base == 0? 1:( _temp5328 < _temp5326.base? 1:
_temp5328 >= _temp5326.last_plus_one)){ _throw( Null_Exception);}* _temp5328;}).last_line)),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5329= yyvs; struct
_xtunion_struct** _temp5331= _temp5329.curr + yyvsp_offset; if( _temp5329.base
== 0? 1:( _temp5331 < _temp5329.base? 1: _temp5331 >= _temp5329.last_plus_one)){
_throw( Null_Exception);}* _temp5331;}))); _temp5320;}); _temp5319;}); goto
_LL5301; _LL5307: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5332=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5332[ 0]=({ struct Cyc_Stmt_tok_struct _temp5333; _temp5333.tag= Cyc_Stmt_tok_tag;
_temp5333.f1= Cyc_Absyn_seq_stmt( _temp5312, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5334= yyvs; struct _xtunion_struct** _temp5336= _temp5334.curr
+ yyvsp_offset; if( _temp5334.base == 0? 1:( _temp5336 < _temp5334.base? 1:
_temp5336 >= _temp5334.last_plus_one)){ _throw( Null_Exception);}* _temp5336;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5337= yyls; struct Cyc_Yyltype*
_temp5339= _temp5337.curr +( yylsp_offset - 1); if( _temp5337.base == 0? 1:(
_temp5339 < _temp5337.base? 1: _temp5339 >= _temp5337.last_plus_one)){ _throw(
Null_Exception);}* _temp5339;}).first_line,({ struct _tagged_ptr3 _temp5340=
yyls; struct Cyc_Yyltype* _temp5342= _temp5340.curr + yylsp_offset; if(
_temp5340.base == 0? 1:( _temp5342 < _temp5340.base? 1: _temp5342 >= _temp5340.last_plus_one)){
_throw( Null_Exception);}* _temp5342;}).last_line)); _temp5333;}); _temp5332;});
goto _LL5301; _LL5301:;} break; case 228: _LL5296: yyval=( struct
_xtunion_struct*)({ struct Cyc_BlockItem_tok_struct* _temp5344=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp5344[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp5345; _temp5345.tag= Cyc_BlockItem_tok_tag;
_temp5345.f1=( void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp5346=(
struct Cyc_Parse_TopDecls_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct));
_temp5346[ 0]=({ struct Cyc_Parse_TopDecls_bl_struct _temp5347; _temp5347.tag=
Cyc_Parse_TopDecls_bl_tag; _temp5347.f1= Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp5348= yyvs; struct _xtunion_struct** _temp5350= _temp5348.curr
+ yyvsp_offset; if( _temp5348.base == 0? 1:( _temp5350 < _temp5348.base? 1:
_temp5350 >= _temp5348.last_plus_one)){ _throw( Null_Exception);}* _temp5350;}));
_temp5347;}); _temp5346;})); _temp5345;}); _temp5344;}); break; case 229:
_LL5343: yyval=( struct _xtunion_struct*)({ struct Cyc_BlockItem_tok_struct*
_temp5352=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp5352[ 0]=({ struct Cyc_BlockItem_tok_struct _temp5353; _temp5353.tag= Cyc_BlockItem_tok_tag;
_temp5353.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp5354=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp5354[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp5355; _temp5355.tag= Cyc_Parse_Stmt_bl_tag;
_temp5355.f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5356= yyvs; struct
_xtunion_struct** _temp5358= _temp5356.curr + yyvsp_offset; if( _temp5356.base
== 0? 1:( _temp5358 < _temp5356.base? 1: _temp5358 >= _temp5356.last_plus_one)){
_throw( Null_Exception);}* _temp5358;})); _temp5355;}); _temp5354;})); _temp5353;});
_temp5352;}); break; case 230: _LL5351: yyval=( struct _xtunion_struct*)({
struct Cyc_BlockItem_tok_struct* _temp5360=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp5360[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp5361; _temp5361.tag= Cyc_BlockItem_tok_tag;
_temp5361.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp5362=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp5362[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp5363; _temp5363.tag= Cyc_Parse_FnDecl_bl_tag;
_temp5363.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp5364= yyvs;
struct _xtunion_struct** _temp5366= _temp5364.curr + yyvsp_offset; if( _temp5364.base
== 0? 1:( _temp5366 < _temp5364.base? 1: _temp5366 >= _temp5364.last_plus_one)){
_throw( Null_Exception);}* _temp5366;})); _temp5363;}); _temp5362;})); _temp5361;});
_temp5360;}); break; case 231: _LL5359: yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5368=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5368[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5369; _temp5369.tag= Cyc_Stmt_tok_tag; _temp5369.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5370= yyvs; struct _xtunion_struct**
_temp5372= _temp5370.curr +( yyvsp_offset - 2); if( _temp5370.base == 0? 1:(
_temp5372 < _temp5370.base? 1: _temp5372 >= _temp5370.last_plus_one)){ _throw(
Null_Exception);}* _temp5372;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5373= yyvs; struct _xtunion_struct** _temp5375= _temp5373.curr +
yyvsp_offset; if( _temp5373.base == 0? 1:( _temp5375 < _temp5373.base? 1:
_temp5375 >= _temp5373.last_plus_one)){ _throw( Null_Exception);}* _temp5375;})),
Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5376= yyls; struct Cyc_Yyltype* _temp5378= _temp5376.curr +( yylsp_offset -
4); if( _temp5376.base == 0? 1:( _temp5378 < _temp5376.base? 1: _temp5378 >=
_temp5376.last_plus_one)){ _throw( Null_Exception);}* _temp5378;}).first_line,({
struct _tagged_ptr3 _temp5379= yyls; struct Cyc_Yyltype* _temp5381= _temp5379.curr
+ yylsp_offset; if( _temp5379.base == 0? 1:( _temp5381 < _temp5379.base? 1:
_temp5381 >= _temp5379.last_plus_one)){ _throw( Null_Exception);}* _temp5381;}).last_line));
_temp5369;}); _temp5368;}); break; case 232: _LL5367: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5383=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5383[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5384; _temp5384.tag= Cyc_Stmt_tok_tag; _temp5384.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5385= yyvs; struct _xtunion_struct**
_temp5387= _temp5385.curr +( yyvsp_offset - 4); if( _temp5385.base == 0? 1:(
_temp5387 < _temp5385.base? 1: _temp5387 >= _temp5385.last_plus_one)){ _throw(
Null_Exception);}* _temp5387;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5388= yyvs; struct _xtunion_struct** _temp5390= _temp5388.curr +(
yyvsp_offset - 2); if( _temp5388.base == 0? 1:( _temp5390 < _temp5388.base? 1:
_temp5390 >= _temp5388.last_plus_one)){ _throw( Null_Exception);}* _temp5390;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5391= yyvs; struct
_xtunion_struct** _temp5393= _temp5391.curr + yyvsp_offset; if( _temp5391.base
== 0? 1:( _temp5393 < _temp5391.base? 1: _temp5393 >= _temp5391.last_plus_one)){
_throw( Null_Exception);}* _temp5393;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5394= yyls; struct Cyc_Yyltype* _temp5396= _temp5394.curr +(
yylsp_offset - 6); if( _temp5394.base == 0? 1:( _temp5396 < _temp5394.base? 1:
_temp5396 >= _temp5394.last_plus_one)){ _throw( Null_Exception);}* _temp5396;}).first_line,({
struct _tagged_ptr3 _temp5397= yyls; struct Cyc_Yyltype* _temp5399= _temp5397.curr
+ yylsp_offset; if( _temp5397.base == 0? 1:( _temp5399 < _temp5397.base? 1:
_temp5399 >= _temp5397.last_plus_one)){ _throw( Null_Exception);}* _temp5399;}).last_line));
_temp5384;}); _temp5383;}); break; case 233: _LL5382: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5401=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5401[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5402; _temp5402.tag= Cyc_Stmt_tok_tag; _temp5402.f1= Cyc_Absyn_switch_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5403= yyvs; struct _xtunion_struct**
_temp5405= _temp5403.curr +( yyvsp_offset - 4); if( _temp5403.base == 0? 1:(
_temp5405 < _temp5403.base? 1: _temp5405 >= _temp5403.last_plus_one)){ _throw(
Null_Exception);}* _temp5405;})), Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5406= yyvs; struct _xtunion_struct** _temp5408= _temp5406.curr
+( yyvsp_offset - 1); if( _temp5406.base == 0? 1:( _temp5408 < _temp5406.base? 1:
_temp5408 >= _temp5406.last_plus_one)){ _throw( Null_Exception);}* _temp5408;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5409= yyls; struct Cyc_Yyltype*
_temp5411= _temp5409.curr +( yylsp_offset - 6); if( _temp5409.base == 0? 1:(
_temp5411 < _temp5409.base? 1: _temp5411 >= _temp5409.last_plus_one)){ _throw(
Null_Exception);}* _temp5411;}).first_line,({ struct _tagged_ptr3 _temp5412=
yyls; struct Cyc_Yyltype* _temp5414= _temp5412.curr + yylsp_offset; if(
_temp5412.base == 0? 1:( _temp5414 < _temp5412.base? 1: _temp5414 >= _temp5412.last_plus_one)){
_throw( Null_Exception);}* _temp5414;}).last_line)); _temp5402;}); _temp5401;});
break; case 234: _LL5400: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5416=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5416[ 0]=({ struct Cyc_Stmt_tok_struct _temp5417; _temp5417.tag= Cyc_Stmt_tok_tag;
_temp5417.f1= Cyc_Absyn_trycatch_stmt( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5418= yyvs; struct _xtunion_struct** _temp5420= _temp5418.curr +(
yyvsp_offset - 4); if( _temp5418.base == 0? 1:( _temp5420 < _temp5418.base? 1:
_temp5420 >= _temp5418.last_plus_one)){ _throw( Null_Exception);}* _temp5420;})),
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp5421= yyvs; struct
_xtunion_struct** _temp5423= _temp5421.curr +( yyvsp_offset - 1); if( _temp5421.base
== 0? 1:( _temp5423 < _temp5421.base? 1: _temp5423 >= _temp5421.last_plus_one)){
_throw( Null_Exception);}* _temp5423;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5424= yyls; struct Cyc_Yyltype* _temp5426= _temp5424.curr +(
yylsp_offset - 5); if( _temp5424.base == 0? 1:( _temp5426 < _temp5424.base? 1:
_temp5426 >= _temp5424.last_plus_one)){ _throw( Null_Exception);}* _temp5426;}).first_line,({
struct _tagged_ptr3 _temp5427= yyls; struct Cyc_Yyltype* _temp5429= _temp5427.curr
+ yylsp_offset; if( _temp5427.base == 0? 1:( _temp5429 < _temp5427.base? 1:
_temp5429 >= _temp5427.last_plus_one)){ _throw( Null_Exception);}* _temp5429;}).last_line));
_temp5417;}); _temp5416;}); break; case 235: _LL5415: yyval=( struct
_xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5431=( struct
Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5431[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5432; _temp5432.tag=
Cyc_SwitchClauseList_tok_tag; _temp5432.f1= 0; _temp5432;}); _temp5431;});
break; case 236: _LL5430: yyval=( struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp5434=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp5434[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp5435; _temp5435.tag= Cyc_SwitchClauseList_tok_tag; _temp5435.f1=({ struct
Cyc_List_List* _temp5436=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5436->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp5437=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5437->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5447= yyls; struct Cyc_Yyltype* _temp5449= _temp5447.curr +( yylsp_offset -
2); if( _temp5447.base == 0? 1:( _temp5449 < _temp5447.base? 1: _temp5449 >=
_temp5447.last_plus_one)){ _throw( Null_Exception);}* _temp5449;}).first_line,({
struct _tagged_ptr3 _temp5450= yyls; struct Cyc_Yyltype* _temp5452= _temp5450.curr
+( yylsp_offset - 2); if( _temp5450.base == 0? 1:( _temp5452 < _temp5450.base? 1:
_temp5452 >= _temp5450.last_plus_one)){ _throw( Null_Exception);}* _temp5452;}).last_line));
_temp5437->pat_vars= 0; _temp5437->where_clause= 0; _temp5437->body= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5444= yyvs; struct _xtunion_struct** _temp5446=
_temp5444.curr + yyvsp_offset; if( _temp5444.base == 0? 1:( _temp5446 <
_temp5444.base? 1: _temp5446 >= _temp5444.last_plus_one)){ _throw(
Null_Exception);}* _temp5446;})); _temp5437->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5438= yyls; struct Cyc_Yyltype* _temp5440= _temp5438.curr
+( yylsp_offset - 2); if( _temp5438.base == 0? 1:( _temp5440 < _temp5438.base? 1:
_temp5440 >= _temp5438.last_plus_one)){ _throw( Null_Exception);}* _temp5440;}).first_line,({
struct _tagged_ptr3 _temp5441= yyls; struct Cyc_Yyltype* _temp5443= _temp5441.curr
+ yylsp_offset; if( _temp5441.base == 0? 1:( _temp5443 < _temp5441.base? 1:
_temp5443 >= _temp5441.last_plus_one)){ _throw( Null_Exception);}* _temp5443;}).last_line);
_temp5437;}); _temp5436->tl= 0; _temp5436;}); _temp5435;}); _temp5434;}); break;
case 237: _LL5433: yyval=( struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp5454=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp5454[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp5455; _temp5455.tag= Cyc_SwitchClauseList_tok_tag; _temp5455.f1=({ struct
Cyc_List_List* _temp5456=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5456->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp5460=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5460->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5473= yyvs; struct _xtunion_struct** _temp5475=
_temp5473.curr +( yyvsp_offset - 2); if( _temp5473.base == 0? 1:( _temp5475 <
_temp5473.base? 1: _temp5475 >= _temp5473.last_plus_one)){ _throw(
Null_Exception);}* _temp5475;})); _temp5460->pat_vars= 0; _temp5460->where_clause=
0; _temp5460->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5467= yyls; struct Cyc_Yyltype* _temp5469= _temp5467.curr +(
yylsp_offset - 1); if( _temp5467.base == 0? 1:( _temp5469 < _temp5467.base? 1:
_temp5469 >= _temp5467.last_plus_one)){ _throw( Null_Exception);}* _temp5469;}).first_line,({
struct _tagged_ptr3 _temp5470= yyls; struct Cyc_Yyltype* _temp5472= _temp5470.curr
+( yylsp_offset - 1); if( _temp5470.base == 0? 1:( _temp5472 < _temp5470.base? 1:
_temp5472 >= _temp5470.last_plus_one)){ _throw( Null_Exception);}* _temp5472;}).last_line));
_temp5460->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5461=
yyls; struct Cyc_Yyltype* _temp5463= _temp5461.curr +( yylsp_offset - 3); if(
_temp5461.base == 0? 1:( _temp5463 < _temp5461.base? 1: _temp5463 >= _temp5461.last_plus_one)){
_throw( Null_Exception);}* _temp5463;}).first_line,({ struct _tagged_ptr3
_temp5464= yyls; struct Cyc_Yyltype* _temp5466= _temp5464.curr + yylsp_offset;
if( _temp5464.base == 0? 1:( _temp5466 < _temp5464.base? 1: _temp5466 >=
_temp5464.last_plus_one)){ _throw( Null_Exception);}* _temp5466;}).last_line);
_temp5460;}); _temp5456->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5457= yyvs; struct _xtunion_struct** _temp5459= _temp5457.curr
+ yyvsp_offset; if( _temp5457.base == 0? 1:( _temp5459 < _temp5457.base? 1:
_temp5459 >= _temp5457.last_plus_one)){ _throw( Null_Exception);}* _temp5459;}));
_temp5456;}); _temp5455;}); _temp5454;}); break; case 238: _LL5453: yyval=(
struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5477=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5477[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5478; _temp5478.tag=
Cyc_SwitchClauseList_tok_tag; _temp5478.f1=({ struct Cyc_List_List* _temp5479=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5479->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5483=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5483->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5493= yyvs; struct _xtunion_struct** _temp5495=
_temp5493.curr +( yyvsp_offset - 3); if( _temp5493.base == 0? 1:( _temp5495 <
_temp5493.base? 1: _temp5495 >= _temp5493.last_plus_one)){ _throw(
Null_Exception);}* _temp5495;})); _temp5483->pat_vars= 0; _temp5483->where_clause=
0; _temp5483->body= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5490= yyvs;
struct _xtunion_struct** _temp5492= _temp5490.curr +( yyvsp_offset - 1); if(
_temp5490.base == 0? 1:( _temp5492 < _temp5490.base? 1: _temp5492 >= _temp5490.last_plus_one)){
_throw( Null_Exception);}* _temp5492;})); _temp5483->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5484= yyls; struct Cyc_Yyltype* _temp5486= _temp5484.curr
+( yylsp_offset - 4); if( _temp5484.base == 0? 1:( _temp5486 < _temp5484.base? 1:
_temp5486 >= _temp5484.last_plus_one)){ _throw( Null_Exception);}* _temp5486;}).first_line,({
struct _tagged_ptr3 _temp5487= yyls; struct Cyc_Yyltype* _temp5489= _temp5487.curr
+( yylsp_offset - 1); if( _temp5487.base == 0? 1:( _temp5489 < _temp5487.base? 1:
_temp5489 >= _temp5487.last_plus_one)){ _throw( Null_Exception);}* _temp5489;}).last_line);
_temp5483;}); _temp5479->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5480= yyvs; struct _xtunion_struct** _temp5482= _temp5480.curr
+ yyvsp_offset; if( _temp5480.base == 0? 1:( _temp5482 < _temp5480.base? 1:
_temp5482 >= _temp5480.last_plus_one)){ _throw( Null_Exception);}* _temp5482;}));
_temp5479;}); _temp5478;}); _temp5477;}); break; case 239: _LL5476: yyval=(
struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5497=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5497[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5498; _temp5498.tag=
Cyc_SwitchClauseList_tok_tag; _temp5498.f1=({ struct Cyc_List_List* _temp5499=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5499->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5503=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5503->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5519= yyvs; struct _xtunion_struct** _temp5521=
_temp5519.curr +( yyvsp_offset - 4); if( _temp5519.base == 0? 1:( _temp5521 <
_temp5519.base? 1: _temp5521 >= _temp5519.last_plus_one)){ _throw(
Null_Exception);}* _temp5521;})); _temp5503->pat_vars= 0; _temp5503->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5516= yyvs;
struct _xtunion_struct** _temp5518= _temp5516.curr +( yyvsp_offset - 2); if(
_temp5516.base == 0? 1:( _temp5518 < _temp5516.base? 1: _temp5518 >= _temp5516.last_plus_one)){
_throw( Null_Exception);}* _temp5518;})); _temp5503->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5510= yyls; struct Cyc_Yyltype*
_temp5512= _temp5510.curr +( yylsp_offset - 1); if( _temp5510.base == 0? 1:(
_temp5512 < _temp5510.base? 1: _temp5512 >= _temp5510.last_plus_one)){ _throw(
Null_Exception);}* _temp5512;}).first_line,({ struct _tagged_ptr3 _temp5513=
yyls; struct Cyc_Yyltype* _temp5515= _temp5513.curr +( yylsp_offset - 1); if(
_temp5513.base == 0? 1:( _temp5515 < _temp5513.base? 1: _temp5515 >= _temp5513.last_plus_one)){
_throw( Null_Exception);}* _temp5515;}).last_line)); _temp5503->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5504= yyls; struct Cyc_Yyltype* _temp5506= _temp5504.curr
+( yylsp_offset - 5); if( _temp5504.base == 0? 1:( _temp5506 < _temp5504.base? 1:
_temp5506 >= _temp5504.last_plus_one)){ _throw( Null_Exception);}* _temp5506;}).first_line,({
struct _tagged_ptr3 _temp5507= yyls; struct Cyc_Yyltype* _temp5509= _temp5507.curr
+ yylsp_offset; if( _temp5507.base == 0? 1:( _temp5509 < _temp5507.base? 1:
_temp5509 >= _temp5507.last_plus_one)){ _throw( Null_Exception);}* _temp5509;}).last_line);
_temp5503;}); _temp5499->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5500= yyvs; struct _xtunion_struct** _temp5502= _temp5500.curr
+ yyvsp_offset; if( _temp5500.base == 0? 1:( _temp5502 < _temp5500.base? 1:
_temp5502 >= _temp5500.last_plus_one)){ _throw( Null_Exception);}* _temp5502;}));
_temp5499;}); _temp5498;}); _temp5497;}); break; case 240: _LL5496: yyval=(
struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5523=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5523[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5524; _temp5524.tag=
Cyc_SwitchClauseList_tok_tag; _temp5524.f1=({ struct Cyc_List_List* _temp5525=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5525->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5529=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5529->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5542= yyvs; struct _xtunion_struct** _temp5544=
_temp5542.curr +( yyvsp_offset - 5); if( _temp5542.base == 0? 1:( _temp5544 <
_temp5542.base? 1: _temp5544 >= _temp5542.last_plus_one)){ _throw(
Null_Exception);}* _temp5544;})); _temp5529->pat_vars= 0; _temp5529->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5539= yyvs;
struct _xtunion_struct** _temp5541= _temp5539.curr +( yyvsp_offset - 3); if(
_temp5539.base == 0? 1:( _temp5541 < _temp5539.base? 1: _temp5541 >= _temp5539.last_plus_one)){
_throw( Null_Exception);}* _temp5541;})); _temp5529->body= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5536= yyvs; struct _xtunion_struct** _temp5538=
_temp5536.curr +( yyvsp_offset - 1); if( _temp5536.base == 0? 1:( _temp5538 <
_temp5536.base? 1: _temp5538 >= _temp5536.last_plus_one)){ _throw(
Null_Exception);}* _temp5538;})); _temp5529->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5530= yyls; struct Cyc_Yyltype* _temp5532= _temp5530.curr
+( yylsp_offset - 6); if( _temp5530.base == 0? 1:( _temp5532 < _temp5530.base? 1:
_temp5532 >= _temp5530.last_plus_one)){ _throw( Null_Exception);}* _temp5532;}).first_line,({
struct _tagged_ptr3 _temp5533= yyls; struct Cyc_Yyltype* _temp5535= _temp5533.curr
+ yylsp_offset; if( _temp5533.base == 0? 1:( _temp5535 < _temp5533.base? 1:
_temp5535 >= _temp5533.last_plus_one)){ _throw( Null_Exception);}* _temp5535;}).last_line);
_temp5529;}); _temp5525->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5526= yyvs; struct _xtunion_struct** _temp5528= _temp5526.curr
+ yyvsp_offset; if( _temp5526.base == 0? 1:( _temp5528 < _temp5526.base? 1:
_temp5528 >= _temp5526.last_plus_one)){ _throw( Null_Exception);}* _temp5528;}));
_temp5525;}); _temp5524;}); _temp5523;}); break; case 241: _LL5522: yyval=(
struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5546=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5546[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5547; _temp5547.tag= Cyc_Stmt_tok_tag; _temp5547.f1= Cyc_Absyn_while_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5548= yyvs; struct _xtunion_struct**
_temp5550= _temp5548.curr +( yyvsp_offset - 2); if( _temp5548.base == 0? 1:(
_temp5550 < _temp5548.base? 1: _temp5550 >= _temp5548.last_plus_one)){ _throw(
Null_Exception);}* _temp5550;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5551= yyvs; struct _xtunion_struct** _temp5553= _temp5551.curr +
yyvsp_offset; if( _temp5551.base == 0? 1:( _temp5553 < _temp5551.base? 1:
_temp5553 >= _temp5551.last_plus_one)){ _throw( Null_Exception);}* _temp5553;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5554= yyls; struct Cyc_Yyltype*
_temp5556= _temp5554.curr +( yylsp_offset - 4); if( _temp5554.base == 0? 1:(
_temp5556 < _temp5554.base? 1: _temp5556 >= _temp5554.last_plus_one)){ _throw(
Null_Exception);}* _temp5556;}).first_line,({ struct _tagged_ptr3 _temp5557=
yyls; struct Cyc_Yyltype* _temp5559= _temp5557.curr + yylsp_offset; if(
_temp5557.base == 0? 1:( _temp5559 < _temp5557.base? 1: _temp5559 >= _temp5557.last_plus_one)){
_throw( Null_Exception);}* _temp5559;}).last_line)); _temp5547;}); _temp5546;});
break; case 242: _LL5545: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5561=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5561[ 0]=({ struct Cyc_Stmt_tok_struct _temp5562; _temp5562.tag= Cyc_Stmt_tok_tag;
_temp5562.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5563= yyvs; struct _xtunion_struct** _temp5565= _temp5563.curr +(
yyvsp_offset - 5); if( _temp5563.base == 0? 1:( _temp5565 < _temp5563.base? 1:
_temp5565 >= _temp5563.last_plus_one)){ _throw( Null_Exception);}* _temp5565;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5566= yyvs; struct _xtunion_struct**
_temp5568= _temp5566.curr +( yyvsp_offset - 2); if( _temp5566.base == 0? 1:(
_temp5568 < _temp5566.base? 1: _temp5568 >= _temp5566.last_plus_one)){ _throw(
Null_Exception);}* _temp5568;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5569= yyls; struct Cyc_Yyltype* _temp5571= _temp5569.curr +(
yylsp_offset - 6); if( _temp5569.base == 0? 1:( _temp5571 < _temp5569.base? 1:
_temp5571 >= _temp5569.last_plus_one)){ _throw( Null_Exception);}* _temp5571;}).first_line,({
struct _tagged_ptr3 _temp5572= yyls; struct Cyc_Yyltype* _temp5574= _temp5572.curr
+ yylsp_offset; if( _temp5572.base == 0? 1:( _temp5574 < _temp5572.base? 1:
_temp5574 >= _temp5572.last_plus_one)){ _throw( Null_Exception);}* _temp5574;}).last_line));
_temp5562;}); _temp5561;}); break; case 243: _LL5560: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5576=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5576[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5577; _temp5577.tag= Cyc_Stmt_tok_tag; _temp5577.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5578= yyvs; struct _xtunion_struct** _temp5580=
_temp5578.curr + yyvsp_offset; if( _temp5578.base == 0? 1:( _temp5580 <
_temp5578.base? 1: _temp5580 >= _temp5578.last_plus_one)){ _throw(
Null_Exception);}* _temp5580;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5581= yyls; struct Cyc_Yyltype* _temp5583= _temp5581.curr +(
yylsp_offset - 5); if( _temp5581.base == 0? 1:( _temp5583 < _temp5581.base? 1:
_temp5583 >= _temp5581.last_plus_one)){ _throw( Null_Exception);}* _temp5583;}).first_line,({
struct _tagged_ptr3 _temp5584= yyls; struct Cyc_Yyltype* _temp5586= _temp5584.curr
+ yylsp_offset; if( _temp5584.base == 0? 1:( _temp5586 < _temp5584.base? 1:
_temp5586 >= _temp5584.last_plus_one)){ _throw( Null_Exception);}* _temp5586;}).last_line));
_temp5577;}); _temp5576;}); break; case 244: _LL5575: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5588=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5588[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5589; _temp5589.tag= Cyc_Stmt_tok_tag; _temp5589.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5590= yyvs; struct _xtunion_struct** _temp5592= _temp5590.curr
+( yyvsp_offset - 2); if( _temp5590.base == 0? 1:( _temp5592 < _temp5590.base? 1:
_temp5592 >= _temp5590.last_plus_one)){ _throw( Null_Exception);}* _temp5592;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5593= yyvs; struct
_xtunion_struct** _temp5595= _temp5593.curr + yyvsp_offset; if( _temp5593.base
== 0? 1:( _temp5595 < _temp5593.base? 1: _temp5595 >= _temp5593.last_plus_one)){
_throw( Null_Exception);}* _temp5595;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5596= yyls; struct Cyc_Yyltype* _temp5598= _temp5596.curr +(
yylsp_offset - 6); if( _temp5596.base == 0? 1:( _temp5598 < _temp5596.base? 1:
_temp5598 >= _temp5596.last_plus_one)){ _throw( Null_Exception);}* _temp5598;}).first_line,({
struct _tagged_ptr3 _temp5599= yyls; struct Cyc_Yyltype* _temp5601= _temp5599.curr
+ yylsp_offset; if( _temp5599.base == 0? 1:( _temp5601 < _temp5599.base? 1:
_temp5601 >= _temp5599.last_plus_one)){ _throw( Null_Exception);}* _temp5601;}).last_line));
_temp5589;}); _temp5588;}); break; case 245: _LL5587: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5603=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5603[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5604; _temp5604.tag= Cyc_Stmt_tok_tag; _temp5604.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5605=
yyvs; struct _xtunion_struct** _temp5607= _temp5605.curr +( yyvsp_offset - 3);
if( _temp5605.base == 0? 1:( _temp5607 < _temp5605.base? 1: _temp5607 >=
_temp5605.last_plus_one)){ _throw( Null_Exception);}* _temp5607;})), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5608= yyvs; struct
_xtunion_struct** _temp5610= _temp5608.curr + yyvsp_offset; if( _temp5608.base
== 0? 1:( _temp5610 < _temp5608.base? 1: _temp5610 >= _temp5608.last_plus_one)){
_throw( Null_Exception);}* _temp5610;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5611= yyls; struct Cyc_Yyltype* _temp5613= _temp5611.curr +(
yylsp_offset - 6); if( _temp5611.base == 0? 1:( _temp5613 < _temp5611.base? 1:
_temp5613 >= _temp5611.last_plus_one)){ _throw( Null_Exception);}* _temp5613;}).first_line,({
struct _tagged_ptr3 _temp5614= yyls; struct Cyc_Yyltype* _temp5616= _temp5614.curr
+ yylsp_offset; if( _temp5614.base == 0? 1:( _temp5616 < _temp5614.base? 1:
_temp5616 >= _temp5614.last_plus_one)){ _throw( Null_Exception);}* _temp5616;}).last_line));
_temp5604;}); _temp5603;}); break; case 246: _LL5602: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5618=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5618[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5619; _temp5619.tag= Cyc_Stmt_tok_tag; _temp5619.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5620=
yyvs; struct _xtunion_struct** _temp5622= _temp5620.curr +( yyvsp_offset - 4);
if( _temp5620.base == 0? 1:( _temp5622 < _temp5620.base? 1: _temp5622 >=
_temp5620.last_plus_one)){ _throw( Null_Exception);}* _temp5622;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5623= yyvs; struct _xtunion_struct** _temp5625=
_temp5623.curr +( yyvsp_offset - 2); if( _temp5623.base == 0? 1:( _temp5625 <
_temp5623.base? 1: _temp5625 >= _temp5623.last_plus_one)){ _throw(
Null_Exception);}* _temp5625;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5626= yyvs; struct _xtunion_struct** _temp5628= _temp5626.curr +
yyvsp_offset; if( _temp5626.base == 0? 1:( _temp5628 < _temp5626.base? 1:
_temp5628 >= _temp5626.last_plus_one)){ _throw( Null_Exception);}* _temp5628;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5629= yyls; struct Cyc_Yyltype*
_temp5631= _temp5629.curr +( yylsp_offset - 7); if( _temp5629.base == 0? 1:(
_temp5631 < _temp5629.base? 1: _temp5631 >= _temp5629.last_plus_one)){ _throw(
Null_Exception);}* _temp5631;}).first_line,({ struct _tagged_ptr3 _temp5632=
yyls; struct Cyc_Yyltype* _temp5634= _temp5632.curr +( yylsp_offset - 1); if(
_temp5632.base == 0? 1:( _temp5634 < _temp5632.base? 1: _temp5634 >= _temp5632.last_plus_one)){
_throw( Null_Exception);}* _temp5634;}).last_line)); _temp5619;}); _temp5618;});
break; case 247: _LL5617: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5636=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5636[ 0]=({ struct Cyc_Stmt_tok_struct _temp5637; _temp5637.tag= Cyc_Stmt_tok_tag;
_temp5637.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5638= yyvs; struct _xtunion_struct** _temp5640= _temp5638.curr +(
yyvsp_offset - 4); if( _temp5638.base == 0? 1:( _temp5640 < _temp5638.base? 1:
_temp5640 >= _temp5638.last_plus_one)){ _throw( Null_Exception);}* _temp5640;})),
Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5641= yyvs; struct _xtunion_struct** _temp5643= _temp5641.curr
+ yyvsp_offset; if( _temp5641.base == 0? 1:( _temp5643 < _temp5641.base? 1:
_temp5643 >= _temp5641.last_plus_one)){ _throw( Null_Exception);}* _temp5643;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5644= yyls; struct Cyc_Yyltype*
_temp5646= _temp5644.curr +( yylsp_offset - 6); if( _temp5644.base == 0? 1:(
_temp5646 < _temp5644.base? 1: _temp5646 >= _temp5644.last_plus_one)){ _throw(
Null_Exception);}* _temp5646;}).first_line,({ struct _tagged_ptr3 _temp5647=
yyls; struct Cyc_Yyltype* _temp5649= _temp5647.curr + yylsp_offset; if(
_temp5647.base == 0? 1:( _temp5649 < _temp5647.base? 1: _temp5649 >= _temp5647.last_plus_one)){
_throw( Null_Exception);}* _temp5649;}).last_line)); _temp5637;}); _temp5636;});
break; case 248: _LL5635: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5651=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5651[ 0]=({ struct Cyc_Stmt_tok_struct _temp5652; _temp5652.tag= Cyc_Stmt_tok_tag;
_temp5652.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5653= yyvs; struct _xtunion_struct** _temp5655= _temp5653.curr +(
yyvsp_offset - 5); if( _temp5653.base == 0? 1:( _temp5655 < _temp5653.base? 1:
_temp5655 >= _temp5653.last_plus_one)){ _throw( Null_Exception);}* _temp5655;})),
Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5656= yyvs;
struct _xtunion_struct** _temp5658= _temp5656.curr +( yyvsp_offset - 2); if(
_temp5656.base == 0? 1:( _temp5658 < _temp5656.base? 1: _temp5658 >= _temp5656.last_plus_one)){
_throw( Null_Exception);}* _temp5658;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5659= yyvs; struct _xtunion_struct** _temp5661= _temp5659.curr
+ yyvsp_offset; if( _temp5659.base == 0? 1:( _temp5661 < _temp5659.base? 1:
_temp5661 >= _temp5659.last_plus_one)){ _throw( Null_Exception);}* _temp5661;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5662= yyls; struct Cyc_Yyltype*
_temp5664= _temp5662.curr +( yylsp_offset - 7); if( _temp5662.base == 0? 1:(
_temp5664 < _temp5662.base? 1: _temp5664 >= _temp5662.last_plus_one)){ _throw(
Null_Exception);}* _temp5664;}).first_line,({ struct _tagged_ptr3 _temp5665=
yyls; struct Cyc_Yyltype* _temp5667= _temp5665.curr + yylsp_offset; if(
_temp5665.base == 0? 1:( _temp5667 < _temp5665.base? 1: _temp5667 >= _temp5665.last_plus_one)){
_throw( Null_Exception);}* _temp5667;}).last_line)); _temp5652;}); _temp5651;});
break; case 249: _LL5650: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5669=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5669[ 0]=({ struct Cyc_Stmt_tok_struct _temp5670; _temp5670.tag= Cyc_Stmt_tok_tag;
_temp5670.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5671= yyvs; struct _xtunion_struct** _temp5673= _temp5671.curr +(
yyvsp_offset - 5); if( _temp5671.base == 0? 1:( _temp5673 < _temp5671.base? 1:
_temp5673 >= _temp5671.last_plus_one)){ _throw( Null_Exception);}* _temp5673;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5674= yyvs; struct _xtunion_struct**
_temp5676= _temp5674.curr +( yyvsp_offset - 3); if( _temp5674.base == 0? 1:(
_temp5676 < _temp5674.base? 1: _temp5676 >= _temp5674.last_plus_one)){ _throw(
Null_Exception);}* _temp5676;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5677= yyvs; struct _xtunion_struct** _temp5679=
_temp5677.curr + yyvsp_offset; if( _temp5677.base == 0? 1:( _temp5679 <
_temp5677.base? 1: _temp5679 >= _temp5677.last_plus_one)){ _throw(
Null_Exception);}* _temp5679;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5680= yyls; struct Cyc_Yyltype* _temp5682= _temp5680.curr +(
yylsp_offset - 7); if( _temp5680.base == 0? 1:( _temp5682 < _temp5680.base? 1:
_temp5682 >= _temp5680.last_plus_one)){ _throw( Null_Exception);}* _temp5682;}).first_line,({
struct _tagged_ptr3 _temp5683= yyls; struct Cyc_Yyltype* _temp5685= _temp5683.curr
+ yylsp_offset; if( _temp5683.base == 0? 1:( _temp5685 < _temp5683.base? 1:
_temp5685 >= _temp5683.last_plus_one)){ _throw( Null_Exception);}* _temp5685;}).last_line));
_temp5670;}); _temp5669;}); break; case 250: _LL5668: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5687=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5687[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5688; _temp5688.tag= Cyc_Stmt_tok_tag; _temp5688.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5689= yyvs; struct _xtunion_struct**
_temp5691= _temp5689.curr +( yyvsp_offset - 6); if( _temp5689.base == 0? 1:(
_temp5691 < _temp5689.base? 1: _temp5691 >= _temp5689.last_plus_one)){ _throw(
Null_Exception);}* _temp5691;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5692= yyvs; struct _xtunion_struct** _temp5694= _temp5692.curr +(
yyvsp_offset - 4); if( _temp5692.base == 0? 1:( _temp5694 < _temp5692.base? 1:
_temp5694 >= _temp5692.last_plus_one)){ _throw( Null_Exception);}* _temp5694;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5695= yyvs; struct _xtunion_struct**
_temp5697= _temp5695.curr +( yyvsp_offset - 2); if( _temp5695.base == 0? 1:(
_temp5697 < _temp5695.base? 1: _temp5697 >= _temp5695.last_plus_one)){ _throw(
Null_Exception);}* _temp5697;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5698= yyvs; struct _xtunion_struct** _temp5700= _temp5698.curr +
yyvsp_offset; if( _temp5698.base == 0? 1:( _temp5700 < _temp5698.base? 1:
_temp5700 >= _temp5698.last_plus_one)){ _throw( Null_Exception);}* _temp5700;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5701= yyls; struct Cyc_Yyltype*
_temp5703= _temp5701.curr +( yylsp_offset - 8); if( _temp5701.base == 0? 1:(
_temp5703 < _temp5701.base? 1: _temp5703 >= _temp5701.last_plus_one)){ _throw(
Null_Exception);}* _temp5703;}).first_line,({ struct _tagged_ptr3 _temp5704=
yyls; struct Cyc_Yyltype* _temp5706= _temp5704.curr + yylsp_offset; if(
_temp5704.base == 0? 1:( _temp5706 < _temp5704.base? 1: _temp5706 >= _temp5704.last_plus_one)){
_throw( Null_Exception);}* _temp5706;}).last_line)); _temp5688;}); _temp5687;});
break; case 251: _LL5686: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5708= yyvs; struct _xtunion_struct** _temp5710=
_temp5708.curr +( yyvsp_offset - 3); if( _temp5708.base == 0? 1:( _temp5710 <
_temp5708.base? 1: _temp5710 >= _temp5708.last_plus_one)){ _throw(
Null_Exception);}* _temp5710;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5711= yyvs; struct _xtunion_struct** _temp5713=
_temp5711.curr + yyvsp_offset; if( _temp5711.base == 0? 1:( _temp5713 <
_temp5711.base? 1: _temp5713 >= _temp5711.last_plus_one)){ _throw(
Null_Exception);}* _temp5713;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5714= yyls; struct Cyc_Yyltype* _temp5716= _temp5714.curr +(
yylsp_offset - 5); if( _temp5714.base == 0? 1:( _temp5716 < _temp5714.base? 1:
_temp5716 >= _temp5714.last_plus_one)){ _throw( Null_Exception);}* _temp5716;}).first_line,({
struct _tagged_ptr3 _temp5717= yyls; struct Cyc_Yyltype* _temp5719= _temp5717.curr
+ yylsp_offset; if( _temp5717.base == 0? 1:( _temp5719 < _temp5717.base? 1:
_temp5719 >= _temp5717.last_plus_one)){ _throw( Null_Exception);}* _temp5719;}).last_line));
yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5720=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5720[ 0]=({ struct Cyc_Stmt_tok_struct _temp5721; _temp5721.tag= Cyc_Stmt_tok_tag;
_temp5721.f1= Cyc_Parse_flatten_declarations( decls, s); _temp5721;}); _temp5720;});
break;} case 252: _LL5707: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5723= yyvs; struct _xtunion_struct** _temp5725=
_temp5723.curr +( yyvsp_offset - 4); if( _temp5723.base == 0? 1:( _temp5725 <
_temp5723.base? 1: _temp5725 >= _temp5723.last_plus_one)){ _throw(
Null_Exception);}* _temp5725;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5726=
yyvs; struct _xtunion_struct** _temp5728= _temp5726.curr +( yyvsp_offset - 3);
if( _temp5726.base == 0? 1:( _temp5728 < _temp5726.base? 1: _temp5728 >=
_temp5726.last_plus_one)){ _throw( Null_Exception);}* _temp5728;})), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5729= yyvs; struct
_xtunion_struct** _temp5731= _temp5729.curr + yyvsp_offset; if( _temp5729.base
== 0? 1:( _temp5731 < _temp5729.base? 1: _temp5731 >= _temp5729.last_plus_one)){
_throw( Null_Exception);}* _temp5731;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5732= yyls; struct Cyc_Yyltype* _temp5734= _temp5732.curr +(
yylsp_offset - 6); if( _temp5732.base == 0? 1:( _temp5734 < _temp5732.base? 1:
_temp5734 >= _temp5732.last_plus_one)){ _throw( Null_Exception);}* _temp5734;}).first_line,({
struct _tagged_ptr3 _temp5735= yyls; struct Cyc_Yyltype* _temp5737= _temp5735.curr
+ yylsp_offset; if( _temp5735.base == 0? 1:( _temp5737 < _temp5735.base? 1:
_temp5737 >= _temp5735.last_plus_one)){ _throw( Null_Exception);}* _temp5737;}).last_line));
yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5738=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5738[ 0]=({ struct Cyc_Stmt_tok_struct _temp5739; _temp5739.tag= Cyc_Stmt_tok_tag;
_temp5739.f1= Cyc_Parse_flatten_declarations( decls, s); _temp5739;}); _temp5738;});
break;} case 253: _LL5722: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5741= yyvs; struct _xtunion_struct** _temp5743=
_temp5741.curr +( yyvsp_offset - 4); if( _temp5741.base == 0? 1:( _temp5743 <
_temp5741.base? 1: _temp5743 >= _temp5741.last_plus_one)){ _throw(
Null_Exception);}* _temp5743;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5744= yyvs; struct _xtunion_struct** _temp5746= _temp5744.curr
+( yyvsp_offset - 2); if( _temp5744.base == 0? 1:( _temp5746 < _temp5744.base? 1:
_temp5746 >= _temp5744.last_plus_one)){ _throw( Null_Exception);}* _temp5746;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5747= yyvs; struct
_xtunion_struct** _temp5749= _temp5747.curr + yyvsp_offset; if( _temp5747.base
== 0? 1:( _temp5749 < _temp5747.base? 1: _temp5749 >= _temp5747.last_plus_one)){
_throw( Null_Exception);}* _temp5749;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5750= yyls; struct Cyc_Yyltype* _temp5752= _temp5750.curr +(
yylsp_offset - 6); if( _temp5750.base == 0? 1:( _temp5752 < _temp5750.base? 1:
_temp5752 >= _temp5750.last_plus_one)){ _throw( Null_Exception);}* _temp5752;}).first_line,({
struct _tagged_ptr3 _temp5753= yyls; struct Cyc_Yyltype* _temp5755= _temp5753.curr
+ yylsp_offset; if( _temp5753.base == 0? 1:( _temp5755 < _temp5753.base? 1:
_temp5755 >= _temp5753.last_plus_one)){ _throw( Null_Exception);}* _temp5755;}).last_line));
yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5756=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5756[ 0]=({ struct Cyc_Stmt_tok_struct _temp5757; _temp5757.tag= Cyc_Stmt_tok_tag;
_temp5757.f1= Cyc_Parse_flatten_declarations( decls, s); _temp5757;}); _temp5756;});
break;} case 254: _LL5740: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5759= yyvs; struct _xtunion_struct** _temp5761=
_temp5759.curr +( yyvsp_offset - 5); if( _temp5759.base == 0? 1:( _temp5761 <
_temp5759.base? 1: _temp5761 >= _temp5759.last_plus_one)){ _throw(
Null_Exception);}* _temp5761;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5762=
yyvs; struct _xtunion_struct** _temp5764= _temp5762.curr +( yyvsp_offset - 4);
if( _temp5762.base == 0? 1:( _temp5764 < _temp5762.base? 1: _temp5764 >=
_temp5762.last_plus_one)){ _throw( Null_Exception);}* _temp5764;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5765= yyvs; struct _xtunion_struct** _temp5767=
_temp5765.curr +( yyvsp_offset - 2); if( _temp5765.base == 0? 1:( _temp5767 <
_temp5765.base? 1: _temp5767 >= _temp5765.last_plus_one)){ _throw(
Null_Exception);}* _temp5767;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5768= yyvs; struct _xtunion_struct** _temp5770= _temp5768.curr +
yyvsp_offset; if( _temp5768.base == 0? 1:( _temp5770 < _temp5768.base? 1:
_temp5770 >= _temp5768.last_plus_one)){ _throw( Null_Exception);}* _temp5770;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5771= yyls; struct Cyc_Yyltype*
_temp5773= _temp5771.curr +( yylsp_offset - 7); if( _temp5771.base == 0? 1:(
_temp5773 < _temp5771.base? 1: _temp5773 >= _temp5771.last_plus_one)){ _throw(
Null_Exception);}* _temp5773;}).first_line,({ struct _tagged_ptr3 _temp5774=
yyls; struct Cyc_Yyltype* _temp5776= _temp5774.curr + yylsp_offset; if(
_temp5774.base == 0? 1:( _temp5776 < _temp5774.base? 1: _temp5776 >= _temp5774.last_plus_one)){
_throw( Null_Exception);}* _temp5776;}).last_line)); yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5777=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5777[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5778; _temp5778.tag= Cyc_Stmt_tok_tag; _temp5778.f1= Cyc_Parse_flatten_declarations(
decls, s); _temp5778;}); _temp5777;}); break;} case 255: _LL5758: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5780=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5780[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5781; _temp5781.tag= Cyc_Stmt_tok_tag; _temp5781.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_string* _temp5782=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp5782[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5783= yyvs; struct _xtunion_struct** _temp5785= _temp5783.curr
+( yyvsp_offset - 1); if( _temp5783.base == 0? 1:( _temp5785 < _temp5783.base? 1:
_temp5785 >= _temp5783.last_plus_one)){ _throw( Null_Exception);}* _temp5785;}));
_temp5782;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5786= yyls;
struct Cyc_Yyltype* _temp5788= _temp5786.curr +( yylsp_offset - 2); if(
_temp5786.base == 0? 1:( _temp5788 < _temp5786.base? 1: _temp5788 >= _temp5786.last_plus_one)){
_throw( Null_Exception);}* _temp5788;}).first_line,({ struct _tagged_ptr3
_temp5789= yyls; struct Cyc_Yyltype* _temp5791= _temp5789.curr +( yylsp_offset -
1); if( _temp5789.base == 0? 1:( _temp5791 < _temp5789.base? 1: _temp5791 >=
_temp5789.last_plus_one)){ _throw( Null_Exception);}* _temp5791;}).last_line));
_temp5781;}); _temp5780;}); break; case 256: _LL5779: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5793=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5793[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5794; _temp5794.tag= Cyc_Stmt_tok_tag; _temp5794.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5795= yyls; struct Cyc_Yyltype*
_temp5797= _temp5795.curr +( yylsp_offset - 1); if( _temp5795.base == 0? 1:(
_temp5797 < _temp5795.base? 1: _temp5797 >= _temp5795.last_plus_one)){ _throw(
Null_Exception);}* _temp5797;}).first_line,({ struct _tagged_ptr3 _temp5798=
yyls; struct Cyc_Yyltype* _temp5800= _temp5798.curr +( yylsp_offset - 1); if(
_temp5798.base == 0? 1:( _temp5800 < _temp5798.base? 1: _temp5800 >= _temp5798.last_plus_one)){
_throw( Null_Exception);}* _temp5800;}).last_line)); _temp5794;}); _temp5793;});
break; case 257: _LL5792: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5802=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5802[ 0]=({ struct Cyc_Stmt_tok_struct _temp5803; _temp5803.tag= Cyc_Stmt_tok_tag;
_temp5803.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5804= yyls; struct Cyc_Yyltype* _temp5806= _temp5804.curr +(
yylsp_offset - 1); if( _temp5804.base == 0? 1:( _temp5806 < _temp5804.base? 1:
_temp5806 >= _temp5804.last_plus_one)){ _throw( Null_Exception);}* _temp5806;}).first_line,({
struct _tagged_ptr3 _temp5807= yyls; struct Cyc_Yyltype* _temp5809= _temp5807.curr
+( yylsp_offset - 1); if( _temp5807.base == 0? 1:( _temp5809 < _temp5807.base? 1:
_temp5809 >= _temp5807.last_plus_one)){ _throw( Null_Exception);}* _temp5809;}).last_line));
_temp5803;}); _temp5802;}); break; case 258: _LL5801: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5811=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5811[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5812; _temp5812.tag= Cyc_Stmt_tok_tag; _temp5812.f1= Cyc_Absyn_return_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5813= yyls; struct
Cyc_Yyltype* _temp5815= _temp5813.curr +( yylsp_offset - 1); if( _temp5813.base
== 0? 1:( _temp5815 < _temp5813.base? 1: _temp5815 >= _temp5813.last_plus_one)){
_throw( Null_Exception);}* _temp5815;}).first_line,({ struct _tagged_ptr3
_temp5816= yyls; struct Cyc_Yyltype* _temp5818= _temp5816.curr +( yylsp_offset -
1); if( _temp5816.base == 0? 1:( _temp5818 < _temp5816.base? 1: _temp5818 >=
_temp5816.last_plus_one)){ _throw( Null_Exception);}* _temp5818;}).last_line));
_temp5812;}); _temp5811;}); break; case 259: _LL5810: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5820=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5820[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5821; _temp5821.tag= Cyc_Stmt_tok_tag; _temp5821.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5822= yyvs;
struct _xtunion_struct** _temp5824= _temp5822.curr +( yyvsp_offset - 1); if(
_temp5822.base == 0? 1:( _temp5824 < _temp5822.base? 1: _temp5824 >= _temp5822.last_plus_one)){
_throw( Null_Exception);}* _temp5824;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5825= yyls; struct Cyc_Yyltype* _temp5827= _temp5825.curr +(
yylsp_offset - 2); if( _temp5825.base == 0? 1:( _temp5827 < _temp5825.base? 1:
_temp5827 >= _temp5825.last_plus_one)){ _throw( Null_Exception);}* _temp5827;}).first_line,({
struct _tagged_ptr3 _temp5828= yyls; struct Cyc_Yyltype* _temp5830= _temp5828.curr
+( yylsp_offset - 1); if( _temp5828.base == 0? 1:( _temp5830 < _temp5828.base? 1:
_temp5830 >= _temp5828.last_plus_one)){ _throw( Null_Exception);}* _temp5830;}).last_line));
_temp5821;}); _temp5820;}); break; case 260: _LL5819: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5832=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5832[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5833; _temp5833.tag= Cyc_Stmt_tok_tag; _temp5833.f1= Cyc_Absyn_fallthru_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5834= yyls; struct
Cyc_Yyltype* _temp5836= _temp5834.curr +( yylsp_offset - 1); if( _temp5834.base
== 0? 1:( _temp5836 < _temp5834.base? 1: _temp5836 >= _temp5834.last_plus_one)){
_throw( Null_Exception);}* _temp5836;}).first_line,({ struct _tagged_ptr3
_temp5837= yyls; struct Cyc_Yyltype* _temp5839= _temp5837.curr +( yylsp_offset -
1); if( _temp5837.base == 0? 1:( _temp5839 < _temp5837.base? 1: _temp5839 >=
_temp5837.last_plus_one)){ _throw( Null_Exception);}* _temp5839;}).last_line));
_temp5833;}); _temp5832;}); break; case 261: _LL5831: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5841=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5841[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5842; _temp5842.tag= Cyc_Stmt_tok_tag; _temp5842.f1= Cyc_Absyn_fallthru_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5843= yyls; struct
Cyc_Yyltype* _temp5845= _temp5843.curr +( yylsp_offset - 3); if( _temp5843.base
== 0? 1:( _temp5845 < _temp5843.base? 1: _temp5845 >= _temp5843.last_plus_one)){
_throw( Null_Exception);}* _temp5845;}).first_line,({ struct _tagged_ptr3
_temp5846= yyls; struct Cyc_Yyltype* _temp5848= _temp5846.curr +( yylsp_offset -
3); if( _temp5846.base == 0? 1:( _temp5848 < _temp5846.base? 1: _temp5848 >=
_temp5846.last_plus_one)){ _throw( Null_Exception);}* _temp5848;}).last_line));
_temp5842;}); _temp5841;}); break; case 262: _LL5840: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5850=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5850[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5851; _temp5851.tag= Cyc_Stmt_tok_tag; _temp5851.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp5852= yyvs; struct
_xtunion_struct** _temp5854= _temp5852.curr +( yyvsp_offset - 2); if( _temp5852.base
== 0? 1:( _temp5854 < _temp5852.base? 1: _temp5854 >= _temp5852.last_plus_one)){
_throw( Null_Exception);}* _temp5854;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5855= yyls; struct Cyc_Yyltype* _temp5857= _temp5855.curr +(
yylsp_offset - 4); if( _temp5855.base == 0? 1:( _temp5857 < _temp5855.base? 1:
_temp5857 >= _temp5855.last_plus_one)){ _throw( Null_Exception);}* _temp5857;}).first_line,({
struct _tagged_ptr3 _temp5858= yyls; struct Cyc_Yyltype* _temp5860= _temp5858.curr
+( yylsp_offset - 4); if( _temp5858.base == 0? 1:( _temp5860 < _temp5858.base? 1:
_temp5860 >= _temp5858.last_plus_one)){ _throw( Null_Exception);}* _temp5860;}).last_line));
_temp5851;}); _temp5850;}); break; case 263: _LL5849: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5862=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5862[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5863; _temp5863.tag= Cyc_Pattern_tok_tag; _temp5863.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5864= yyls; struct Cyc_Yyltype* _temp5866= _temp5864.curr + yylsp_offset;
if( _temp5864.base == 0? 1:( _temp5866 < _temp5864.base? 1: _temp5866 >=
_temp5864.last_plus_one)){ _throw( Null_Exception);}* _temp5866;}).first_line,({
struct _tagged_ptr3 _temp5867= yyls; struct Cyc_Yyltype* _temp5869= _temp5867.curr
+ yylsp_offset; if( _temp5867.base == 0? 1:( _temp5869 < _temp5867.base? 1:
_temp5869 >= _temp5867.last_plus_one)){ _throw( Null_Exception);}* _temp5869;}).last_line));
_temp5863;}); _temp5862;}); break; case 264: _LL5861: yyval=({ struct
_tagged_ptr2 _temp5871= yyvs; struct _xtunion_struct** _temp5873= _temp5871.curr
+( yyvsp_offset - 1); if( _temp5871.base == 0? 1:( _temp5873 < _temp5871.base? 1:
_temp5873 >= _temp5871.last_plus_one)){ _throw( Null_Exception);}* _temp5873;});
break; case 265: _LL5870: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5875=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5875[ 0]=({ struct Cyc_Pattern_tok_struct _temp5876; _temp5876.tag= Cyc_Pattern_tok_tag;
_temp5876.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp5877=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp5877[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp5878; _temp5878.tag= Cyc_Absyn_Int_p_tag;
_temp5878.f1=( void*)(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp5879= yyvs;
struct _xtunion_struct** _temp5881= _temp5879.curr + yyvsp_offset; if( _temp5879.base
== 0? 1:( _temp5881 < _temp5879.base? 1: _temp5881 >= _temp5879.last_plus_one)){
_throw( Null_Exception);}* _temp5881;}))).f1; _temp5878.f2=(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5882= yyvs; struct _xtunion_struct** _temp5884=
_temp5882.curr + yyvsp_offset; if( _temp5882.base == 0? 1:( _temp5884 <
_temp5882.base? 1: _temp5884 >= _temp5882.last_plus_one)){ _throw(
Null_Exception);}* _temp5884;}))).f2; _temp5878;}); _temp5877;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5885= yyls; struct Cyc_Yyltype* _temp5887= _temp5885.curr
+ yylsp_offset; if( _temp5885.base == 0? 1:( _temp5887 < _temp5885.base? 1:
_temp5887 >= _temp5885.last_plus_one)){ _throw( Null_Exception);}* _temp5887;}).first_line,({
struct _tagged_ptr3 _temp5888= yyls; struct Cyc_Yyltype* _temp5890= _temp5888.curr
+ yylsp_offset; if( _temp5888.base == 0? 1:( _temp5890 < _temp5888.base? 1:
_temp5890 >= _temp5888.last_plus_one)){ _throw( Null_Exception);}* _temp5890;}).last_line));
_temp5876;}); _temp5875;}); break; case 266: _LL5874: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5892=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5892[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5893; _temp5893.tag= Cyc_Pattern_tok_tag; _temp5893.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp5894=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp5894[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp5895; _temp5895.tag= Cyc_Absyn_Int_p_tag; _temp5895.f1=( void*)(( void*)
Cyc_Absyn_Signed); _temp5895.f2= -(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2
_temp5896= yyvs; struct _xtunion_struct** _temp5898= _temp5896.curr +
yyvsp_offset; if( _temp5896.base == 0? 1:( _temp5898 < _temp5896.base? 1:
_temp5898 >= _temp5896.last_plus_one)){ _throw( Null_Exception);}* _temp5898;}))).f2;
_temp5895;}); _temp5894;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5899= yyls; struct Cyc_Yyltype* _temp5901= _temp5899.curr +( yylsp_offset -
1); if( _temp5899.base == 0? 1:( _temp5901 < _temp5899.base? 1: _temp5901 >=
_temp5899.last_plus_one)){ _throw( Null_Exception);}* _temp5901;}).first_line,({
struct _tagged_ptr3 _temp5902= yyls; struct Cyc_Yyltype* _temp5904= _temp5902.curr
+ yylsp_offset; if( _temp5902.base == 0? 1:( _temp5904 < _temp5902.base? 1:
_temp5904 >= _temp5902.last_plus_one)){ _throw( Null_Exception);}* _temp5904;}).last_line));
_temp5893;}); _temp5892;}); break; case 267: _LL5891: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5906=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5906[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5907; _temp5907.tag= Cyc_Pattern_tok_tag; _temp5907.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp5908=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp5908[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp5909; _temp5909.tag= Cyc_Absyn_Float_p_tag;
_temp5909.f1= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5910= yyvs;
struct _xtunion_struct** _temp5912= _temp5910.curr + yyvsp_offset; if( _temp5910.base
== 0? 1:( _temp5912 < _temp5910.base? 1: _temp5912 >= _temp5910.last_plus_one)){
_throw( Null_Exception);}* _temp5912;})); _temp5909;}); _temp5908;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5913= yyls; struct Cyc_Yyltype* _temp5915= _temp5913.curr
+ yylsp_offset; if( _temp5913.base == 0? 1:( _temp5915 < _temp5913.base? 1:
_temp5915 >= _temp5913.last_plus_one)){ _throw( Null_Exception);}* _temp5915;}).first_line,({
struct _tagged_ptr3 _temp5916= yyls; struct Cyc_Yyltype* _temp5918= _temp5916.curr
+ yylsp_offset; if( _temp5916.base == 0? 1:( _temp5918 < _temp5916.base? 1:
_temp5918 >= _temp5916.last_plus_one)){ _throw( Null_Exception);}* _temp5918;}).last_line));
_temp5907;}); _temp5906;}); break; case 268: _LL5905: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5920=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5920[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5921; _temp5921.tag= Cyc_Pattern_tok_tag; _temp5921.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Char_p_struct* _temp5922=( struct Cyc_Absyn_Char_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_p_struct)); _temp5922[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp5923; _temp5923.tag= Cyc_Absyn_Char_p_tag; _temp5923.f1= Cyc_yyget_Char_tok(({
struct _tagged_ptr2 _temp5924= yyvs; struct _xtunion_struct** _temp5926=
_temp5924.curr + yyvsp_offset; if( _temp5924.base == 0? 1:( _temp5926 <
_temp5924.base? 1: _temp5926 >= _temp5924.last_plus_one)){ _throw(
Null_Exception);}* _temp5926;})); _temp5923;}); _temp5922;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5927= yyls; struct Cyc_Yyltype* _temp5929= _temp5927.curr
+ yylsp_offset; if( _temp5927.base == 0? 1:( _temp5929 < _temp5927.base? 1:
_temp5929 >= _temp5927.last_plus_one)){ _throw( Null_Exception);}* _temp5929;}).first_line,({
struct _tagged_ptr3 _temp5930= yyls; struct Cyc_Yyltype* _temp5932= _temp5930.curr
+ yylsp_offset; if( _temp5930.base == 0? 1:( _temp5932 < _temp5930.base? 1:
_temp5932 >= _temp5930.last_plus_one)){ _throw( Null_Exception);}* _temp5932;}).last_line));
_temp5921;}); _temp5920;}); break; case 269: _LL5919: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5934=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5934[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5935; _temp5935.tag= Cyc_Pattern_tok_tag; _temp5935.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5936= yyls; struct Cyc_Yyltype* _temp5938= _temp5936.curr + yylsp_offset;
if( _temp5936.base == 0? 1:( _temp5938 < _temp5936.base? 1: _temp5938 >=
_temp5936.last_plus_one)){ _throw( Null_Exception);}* _temp5938;}).first_line,({
struct _tagged_ptr3 _temp5939= yyls; struct Cyc_Yyltype* _temp5941= _temp5939.curr
+ yylsp_offset; if( _temp5939.base == 0? 1:( _temp5941 < _temp5939.base? 1:
_temp5941 >= _temp5939.last_plus_one)){ _throw( Null_Exception);}* _temp5941;}).last_line));
_temp5935;}); _temp5934;}); break; case 270: _LL5933: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5943=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5943[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5944; _temp5944.tag= Cyc_Pattern_tok_tag; _temp5944.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownId_p_struct* _temp5945=( struct Cyc_Absyn_UnknownId_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct)); _temp5945[ 0]=({
struct Cyc_Absyn_UnknownId_p_struct _temp5946; _temp5946.tag= Cyc_Absyn_UnknownId_p_tag;
_temp5946.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp5947= yyvs;
struct _xtunion_struct** _temp5949= _temp5947.curr + yyvsp_offset; if( _temp5947.base
== 0? 1:( _temp5949 < _temp5947.base? 1: _temp5949 >= _temp5947.last_plus_one)){
_throw( Null_Exception);}* _temp5949;})); _temp5946;}); _temp5945;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5950= yyls; struct Cyc_Yyltype* _temp5952= _temp5950.curr
+ yylsp_offset; if( _temp5950.base == 0? 1:( _temp5952 < _temp5950.base? 1:
_temp5952 >= _temp5950.last_plus_one)){ _throw( Null_Exception);}* _temp5952;}).first_line,({
struct _tagged_ptr3 _temp5953= yyls; struct Cyc_Yyltype* _temp5955= _temp5953.curr
+ yylsp_offset; if( _temp5953.base == 0? 1:( _temp5955 < _temp5953.base? 1:
_temp5955 >= _temp5953.last_plus_one)){ _throw( Null_Exception);}* _temp5955;}).last_line));
_temp5944;}); _temp5943;}); break; case 271: _LL5942: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5957=
yyls; struct Cyc_Yyltype* _temp5959= _temp5957.curr +( yylsp_offset - 3); if(
_temp5957.base == 0? 1:( _temp5959 < _temp5957.base? 1: _temp5959 >= _temp5957.last_plus_one)){
_throw( Null_Exception);}* _temp5959;}).first_line,({ struct _tagged_ptr3
_temp5960= yyls; struct Cyc_Yyltype* _temp5962= _temp5960.curr +( yylsp_offset -
3); if( _temp5960.base == 0? 1:( _temp5962 < _temp5960.base? 1: _temp5962 >=
_temp5960.last_plus_one)){ _throw( Null_Exception);}* _temp5962;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5963= yyvs; struct
_xtunion_struct** _temp5965= _temp5963.curr +( yyvsp_offset - 3); if( _temp5963.base
== 0? 1:( _temp5965 < _temp5963.base? 1: _temp5965 >= _temp5963.last_plus_one)){
_throw( Null_Exception);}* _temp5965;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_Pattern_tok_struct* _temp5966=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5966[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5967; _temp5967.tag= Cyc_Pattern_tok_tag; _temp5967.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownCall_p_struct* _temp5968=( struct Cyc_Absyn_UnknownCall_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownCall_p_struct)); _temp5968[ 0]=({
struct Cyc_Absyn_UnknownCall_p_struct _temp5969; _temp5969.tag= Cyc_Absyn_UnknownCall_p_tag;
_temp5969.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp5970= yyvs;
struct _xtunion_struct** _temp5972= _temp5970.curr +( yyvsp_offset - 4); if(
_temp5970.base == 0? 1:( _temp5972 < _temp5970.base? 1: _temp5972 >= _temp5970.last_plus_one)){
_throw( Null_Exception);}* _temp5972;})); _temp5969.f2= tvs; _temp5969.f3= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp5973= yyvs; struct _xtunion_struct** _temp5975=
_temp5973.curr +( yyvsp_offset - 1); if( _temp5973.base == 0? 1:( _temp5975 <
_temp5973.base? 1: _temp5975 >= _temp5973.last_plus_one)){ _throw(
Null_Exception);}* _temp5975;})); _temp5969;}); _temp5968;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5976= yyls; struct Cyc_Yyltype* _temp5978= _temp5976.curr
+( yylsp_offset - 4); if( _temp5976.base == 0? 1:( _temp5978 < _temp5976.base? 1:
_temp5978 >= _temp5976.last_plus_one)){ _throw( Null_Exception);}* _temp5978;}).first_line,({
struct _tagged_ptr3 _temp5979= yyls; struct Cyc_Yyltype* _temp5981= _temp5979.curr
+ yylsp_offset; if( _temp5979.base == 0? 1:( _temp5981 < _temp5979.base? 1:
_temp5981 >= _temp5979.last_plus_one)){ _throw( Null_Exception);}* _temp5981;}).last_line));
_temp5967;}); _temp5966;}); break;} case 272: _LL5956: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5983=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5983[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5984; _temp5984.tag= Cyc_Pattern_tok_tag; _temp5984.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Tuple_p_struct* _temp5985=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp5985[ 0]=({ struct
Cyc_Absyn_Tuple_p_struct _temp5986; _temp5986.tag= Cyc_Absyn_Tuple_p_tag;
_temp5986.f1= Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp5987= yyvs;
struct _xtunion_struct** _temp5989= _temp5987.curr +( yyvsp_offset - 1); if(
_temp5987.base == 0? 1:( _temp5989 < _temp5987.base? 1: _temp5989 >= _temp5987.last_plus_one)){
_throw( Null_Exception);}* _temp5989;})); _temp5986;}); _temp5985;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5990= yyls; struct Cyc_Yyltype* _temp5992= _temp5990.curr
+( yylsp_offset - 3); if( _temp5990.base == 0? 1:( _temp5992 < _temp5990.base? 1:
_temp5992 >= _temp5990.last_plus_one)){ _throw( Null_Exception);}* _temp5992;}).first_line,({
struct _tagged_ptr3 _temp5993= yyls; struct Cyc_Yyltype* _temp5995= _temp5993.curr
+ yylsp_offset; if( _temp5993.base == 0? 1:( _temp5995 < _temp5993.base? 1:
_temp5995 >= _temp5993.last_plus_one)){ _throw( Null_Exception);}* _temp5995;}).last_line));
_temp5984;}); _temp5983;}); break; case 273: _LL5982: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5997=
yyls; struct Cyc_Yyltype* _temp5999= _temp5997.curr +( yylsp_offset - 2); if(
_temp5997.base == 0? 1:( _temp5999 < _temp5997.base? 1: _temp5999 >= _temp5997.last_plus_one)){
_throw( Null_Exception);}* _temp5999;}).first_line,({ struct _tagged_ptr3
_temp6000= yyls; struct Cyc_Yyltype* _temp6002= _temp6000.curr +( yylsp_offset -
2); if( _temp6000.base == 0? 1:( _temp6002 < _temp6000.base? 1: _temp6002 >=
_temp6000.last_plus_one)){ _throw( Null_Exception);}* _temp6002;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6003= yyvs; struct
_xtunion_struct** _temp6005= _temp6003.curr +( yyvsp_offset - 2); if( _temp6003.base
== 0? 1:( _temp6005 < _temp6003.base? 1: _temp6005 >= _temp6003.last_plus_one)){
_throw( Null_Exception);}* _temp6005;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_Pattern_tok_struct* _temp6006=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6006[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6007; _temp6007.tag= Cyc_Pattern_tok_tag; _temp6007.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp6008=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp6008[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp6009; _temp6009.tag= Cyc_Absyn_UnknownFields_p_tag;
_temp6009.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6010= yyvs;
struct _xtunion_struct** _temp6012= _temp6010.curr +( yyvsp_offset - 3); if(
_temp6010.base == 0? 1:( _temp6012 < _temp6010.base? 1: _temp6012 >= _temp6010.last_plus_one)){
_throw( Null_Exception);}* _temp6012;})); _temp6009.f2= tvs; _temp6009.f3= 0;
_temp6009;}); _temp6008;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6013= yyls; struct Cyc_Yyltype* _temp6015= _temp6013.curr +( yylsp_offset -
3); if( _temp6013.base == 0? 1:( _temp6015 < _temp6013.base? 1: _temp6015 >=
_temp6013.last_plus_one)){ _throw( Null_Exception);}* _temp6015;}).first_line,({
struct _tagged_ptr3 _temp6016= yyls; struct Cyc_Yyltype* _temp6018= _temp6016.curr
+ yylsp_offset; if( _temp6016.base == 0? 1:( _temp6018 < _temp6016.base? 1:
_temp6018 >= _temp6016.last_plus_one)){ _throw( Null_Exception);}* _temp6018;}).last_line));
_temp6007;}); _temp6006;}); break;} case 274: _LL5996: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6020=
yyls; struct Cyc_Yyltype* _temp6022= _temp6020.curr +( yylsp_offset - 3); if(
_temp6020.base == 0? 1:( _temp6022 < _temp6020.base? 1: _temp6022 >= _temp6020.last_plus_one)){
_throw( Null_Exception);}* _temp6022;}).first_line,({ struct _tagged_ptr3
_temp6023= yyls; struct Cyc_Yyltype* _temp6025= _temp6023.curr +( yylsp_offset -
3); if( _temp6023.base == 0? 1:( _temp6025 < _temp6023.base? 1: _temp6025 >=
_temp6023.last_plus_one)){ _throw( Null_Exception);}* _temp6025;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6026= yyvs; struct
_xtunion_struct** _temp6028= _temp6026.curr +( yyvsp_offset - 3); if( _temp6026.base
== 0? 1:( _temp6028 < _temp6026.base? 1: _temp6028 >= _temp6026.last_plus_one)){
_throw( Null_Exception);}* _temp6028;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_Pattern_tok_struct* _temp6029=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6029[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6030; _temp6030.tag= Cyc_Pattern_tok_tag; _temp6030.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp6031=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp6031[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp6032; _temp6032.tag= Cyc_Absyn_UnknownFields_p_tag;
_temp6032.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6033= yyvs;
struct _xtunion_struct** _temp6035= _temp6033.curr +( yyvsp_offset - 4); if(
_temp6033.base == 0? 1:( _temp6035 < _temp6033.base? 1: _temp6035 >= _temp6033.last_plus_one)){
_throw( Null_Exception);}* _temp6035;})); _temp6032.f2= tvs; _temp6032.f3= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp6036= yyvs; struct _xtunion_struct** _temp6038=
_temp6036.curr +( yyvsp_offset - 1); if( _temp6036.base == 0? 1:( _temp6038 <
_temp6036.base? 1: _temp6038 >= _temp6036.last_plus_one)){ _throw(
Null_Exception);}* _temp6038;})); _temp6032;}); _temp6031;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6039= yyls; struct Cyc_Yyltype* _temp6041= _temp6039.curr
+( yylsp_offset - 4); if( _temp6039.base == 0? 1:( _temp6041 < _temp6039.base? 1:
_temp6041 >= _temp6039.last_plus_one)){ _throw( Null_Exception);}* _temp6041;}).first_line,({
struct _tagged_ptr3 _temp6042= yyls; struct Cyc_Yyltype* _temp6044= _temp6042.curr
+ yylsp_offset; if( _temp6042.base == 0? 1:( _temp6044 < _temp6042.base? 1:
_temp6044 >= _temp6042.last_plus_one)){ _throw( Null_Exception);}* _temp6044;}).last_line));
_temp6030;}); _temp6029;}); break;} case 275: _LL6019: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6046=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6046[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6047; _temp6047.tag= Cyc_Pattern_tok_tag; _temp6047.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Pointer_p_struct* _temp6048=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp6048[ 0]=({ struct
Cyc_Absyn_Pointer_p_struct _temp6049; _temp6049.tag= Cyc_Absyn_Pointer_p_tag;
_temp6049.f1= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6050= yyvs;
struct _xtunion_struct** _temp6052= _temp6050.curr + yyvsp_offset; if( _temp6050.base
== 0? 1:( _temp6052 < _temp6050.base? 1: _temp6052 >= _temp6050.last_plus_one)){
_throw( Null_Exception);}* _temp6052;})); _temp6049;}); _temp6048;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6053= yyls; struct Cyc_Yyltype* _temp6055= _temp6053.curr
+( yylsp_offset - 1); if( _temp6053.base == 0? 1:( _temp6055 < _temp6053.base? 1:
_temp6055 >= _temp6053.last_plus_one)){ _throw( Null_Exception);}* _temp6055;}).first_line,({
struct _tagged_ptr3 _temp6056= yyls; struct Cyc_Yyltype* _temp6058= _temp6056.curr
+ yylsp_offset; if( _temp6056.base == 0? 1:( _temp6058 < _temp6056.base? 1:
_temp6058 >= _temp6056.last_plus_one)){ _throw( Null_Exception);}* _temp6058;}).last_line));
_temp6047;}); _temp6046;}); break; case 276: _LL6045: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6060=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6060[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6061; _temp6061.tag= Cyc_Pattern_tok_tag; _temp6061.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Reference_p_struct* _temp6062=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp6062[ 0]=({
struct Cyc_Absyn_Reference_p_struct _temp6063; _temp6063.tag= Cyc_Absyn_Reference_p_tag;
_temp6063.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp6064=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp6064->f1=( void*) Cyc_Absyn_Loc_n;
_temp6064->f2=({ struct _tagged_string* _temp6065=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp6065[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp6066= yyvs; struct _xtunion_struct** _temp6068=
_temp6066.curr + yyvsp_offset; if( _temp6066.base == 0? 1:( _temp6068 <
_temp6066.base? 1: _temp6068 >= _temp6066.last_plus_one)){ _throw(
Null_Exception);}* _temp6068;})); _temp6065;}); _temp6064;}),( void*) Cyc_Absyn_VoidType,
0); _temp6063;}); _temp6062;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6069= yyls; struct Cyc_Yyltype* _temp6071= _temp6069.curr +(
yylsp_offset - 1); if( _temp6069.base == 0? 1:( _temp6071 < _temp6069.base? 1:
_temp6071 >= _temp6069.last_plus_one)){ _throw( Null_Exception);}* _temp6071;}).first_line,({
struct _tagged_ptr3 _temp6072= yyls; struct Cyc_Yyltype* _temp6074= _temp6072.curr
+ yylsp_offset; if( _temp6072.base == 0? 1:( _temp6074 < _temp6072.base? 1:
_temp6074 >= _temp6072.last_plus_one)){ _throw( Null_Exception);}* _temp6074;}).last_line));
_temp6061;}); _temp6060;}); break; case 277: _LL6059: yyval=( struct
_xtunion_struct*)({ struct Cyc_PatternList_tok_struct* _temp6076=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp6076[ 0]=({ struct
Cyc_PatternList_tok_struct _temp6077; _temp6077.tag= Cyc_PatternList_tok_tag;
_temp6077.f1= 0; _temp6077;}); _temp6076;}); break; case 278: _LL6075: yyval=(
struct _xtunion_struct*)({ struct Cyc_PatternList_tok_struct* _temp6079=( struct
Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6079[ 0]=({ struct Cyc_PatternList_tok_struct _temp6080; _temp6080.tag= Cyc_PatternList_tok_tag;
_temp6080.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp6081= yyvs; struct
_xtunion_struct** _temp6083= _temp6081.curr + yyvsp_offset; if( _temp6081.base
== 0? 1:( _temp6083 < _temp6081.base? 1: _temp6083 >= _temp6081.last_plus_one)){
_throw( Null_Exception);}* _temp6083;}))); _temp6080;}); _temp6079;}); break;
case 279: _LL6078: yyval=( struct _xtunion_struct*)({ struct Cyc_PatternList_tok_struct*
_temp6085=( struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6085[ 0]=({ struct Cyc_PatternList_tok_struct _temp6086; _temp6086.tag= Cyc_PatternList_tok_tag;
_temp6086.f1=({ struct Cyc_List_List* _temp6087=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6087->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp6088= yyvs; struct _xtunion_struct** _temp6090=
_temp6088.curr + yyvsp_offset; if( _temp6088.base == 0? 1:( _temp6090 <
_temp6088.base? 1: _temp6090 >= _temp6088.last_plus_one)){ _throw(
Null_Exception);}* _temp6090;})); _temp6087->tl= 0; _temp6087;}); _temp6086;});
_temp6085;}); break; case 280: _LL6084: yyval=( struct _xtunion_struct*)({
struct Cyc_PatternList_tok_struct* _temp6092=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp6092[ 0]=({ struct
Cyc_PatternList_tok_struct _temp6093; _temp6093.tag= Cyc_PatternList_tok_tag;
_temp6093.f1=({ struct Cyc_List_List* _temp6094=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6094->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp6098= yyvs; struct _xtunion_struct** _temp6100=
_temp6098.curr + yyvsp_offset; if( _temp6098.base == 0? 1:( _temp6100 <
_temp6098.base? 1: _temp6100 >= _temp6098.last_plus_one)){ _throw(
Null_Exception);}* _temp6100;})); _temp6094->tl= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp6095= yyvs; struct _xtunion_struct** _temp6097=
_temp6095.curr +( yyvsp_offset - 2); if( _temp6095.base == 0? 1:( _temp6097 <
_temp6095.base? 1: _temp6097 >= _temp6095.last_plus_one)){ _throw(
Null_Exception);}* _temp6097;})); _temp6094;}); _temp6093;}); _temp6092;});
break; case 281: _LL6091: yyval=( struct _xtunion_struct*)({ struct Cyc_FieldPattern_tok_struct*
_temp6102=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6102[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6103; _temp6103.tag=
Cyc_FieldPattern_tok_tag; _temp6103.f1=({ struct _tuple17* _temp6104=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp6104->f1= 0; _temp6104->f2=
Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6105= yyvs; struct
_xtunion_struct** _temp6107= _temp6105.curr + yyvsp_offset; if( _temp6105.base
== 0? 1:( _temp6107 < _temp6105.base? 1: _temp6107 >= _temp6105.last_plus_one)){
_throw( Null_Exception);}* _temp6107;})); _temp6104;}); _temp6103;}); _temp6102;});
break; case 282: _LL6101: yyval=( struct _xtunion_struct*)({ struct Cyc_FieldPattern_tok_struct*
_temp6109=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6109[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6110; _temp6110.tag=
Cyc_FieldPattern_tok_tag; _temp6110.f1=({ struct _tuple17* _temp6111=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp6111->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp6115= yyvs; struct _xtunion_struct** _temp6117=
_temp6115.curr +( yyvsp_offset - 1); if( _temp6115.base == 0? 1:( _temp6117 <
_temp6115.base? 1: _temp6117 >= _temp6115.last_plus_one)){ _throw(
Null_Exception);}* _temp6117;})); _temp6111->f2= Cyc_yyget_Pattern_tok(({ struct
_tagged_ptr2 _temp6112= yyvs; struct _xtunion_struct** _temp6114= _temp6112.curr
+ yyvsp_offset; if( _temp6112.base == 0? 1:( _temp6114 < _temp6112.base? 1:
_temp6114 >= _temp6112.last_plus_one)){ _throw( Null_Exception);}* _temp6114;}));
_temp6111;}); _temp6110;}); _temp6109;}); break; case 283: _LL6108: yyval=(
struct _xtunion_struct*)({ struct Cyc_FieldPatternList_tok_struct* _temp6119=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp6119[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp6120; _temp6120.tag=
Cyc_FieldPatternList_tok_tag; _temp6120.f1=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok(({ struct
_tagged_ptr2 _temp6121= yyvs; struct _xtunion_struct** _temp6123= _temp6121.curr
+ yyvsp_offset; if( _temp6121.base == 0? 1:( _temp6123 < _temp6121.base? 1:
_temp6123 >= _temp6121.last_plus_one)){ _throw( Null_Exception);}* _temp6123;})));
_temp6120;}); _temp6119;}); break; case 284: _LL6118: yyval=( struct
_xtunion_struct*)({ struct Cyc_FieldPatternList_tok_struct* _temp6125=( struct
Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp6125[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp6126; _temp6126.tag=
Cyc_FieldPatternList_tok_tag; _temp6126.f1=({ struct Cyc_List_List* _temp6127=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp6127->hd=(
void*) Cyc_yyget_FieldPattern_tok(({ struct _tagged_ptr2 _temp6128= yyvs; struct
_xtunion_struct** _temp6130= _temp6128.curr + yyvsp_offset; if( _temp6128.base
== 0? 1:( _temp6130 < _temp6128.base? 1: _temp6130 >= _temp6128.last_plus_one)){
_throw( Null_Exception);}* _temp6130;})); _temp6127->tl= 0; _temp6127;});
_temp6126;}); _temp6125;}); break; case 285: _LL6124: yyval=( struct
_xtunion_struct*)({ struct Cyc_FieldPatternList_tok_struct* _temp6132=( struct
Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp6132[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp6133; _temp6133.tag=
Cyc_FieldPatternList_tok_tag; _temp6133.f1=({ struct Cyc_List_List* _temp6134=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp6134->hd=(
void*) Cyc_yyget_FieldPattern_tok(({ struct _tagged_ptr2 _temp6138= yyvs; struct
_xtunion_struct** _temp6140= _temp6138.curr + yyvsp_offset; if( _temp6138.base
== 0? 1:( _temp6140 < _temp6138.base? 1: _temp6140 >= _temp6138.last_plus_one)){
_throw( Null_Exception);}* _temp6140;})); _temp6134->tl= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp6135= yyvs; struct _xtunion_struct** _temp6137=
_temp6135.curr +( yyvsp_offset - 2); if( _temp6135.base == 0? 1:( _temp6137 <
_temp6135.base? 1: _temp6137 >= _temp6135.last_plus_one)){ _throw(
Null_Exception);}* _temp6137;})); _temp6134;}); _temp6133;}); _temp6132;});
break; case 286: _LL6131: yyval=({ struct _tagged_ptr2 _temp6142= yyvs; struct
_xtunion_struct** _temp6144= _temp6142.curr + yyvsp_offset; if( _temp6142.base
== 0? 1:( _temp6144 < _temp6142.base? 1: _temp6144 >= _temp6142.last_plus_one)){
_throw( Null_Exception);}* _temp6144;}); break; case 287: _LL6141: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6146=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6146[ 0]=({ struct Cyc_Exp_tok_struct
_temp6147; _temp6147.tag= Cyc_Exp_tok_tag; _temp6147.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6148= yyvs; struct _xtunion_struct** _temp6150=
_temp6148.curr +( yyvsp_offset - 2); if( _temp6148.base == 0? 1:( _temp6150 <
_temp6148.base? 1: _temp6150 >= _temp6148.last_plus_one)){ _throw(
Null_Exception);}* _temp6150;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6151= yyvs; struct _xtunion_struct** _temp6153= _temp6151.curr +
yyvsp_offset; if( _temp6151.base == 0? 1:( _temp6153 < _temp6151.base? 1:
_temp6153 >= _temp6151.last_plus_one)){ _throw( Null_Exception);}* _temp6153;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6154= yyls; struct Cyc_Yyltype*
_temp6156= _temp6154.curr +( yylsp_offset - 2); if( _temp6154.base == 0? 1:(
_temp6156 < _temp6154.base? 1: _temp6156 >= _temp6154.last_plus_one)){ _throw(
Null_Exception);}* _temp6156;}).first_line,({ struct _tagged_ptr3 _temp6157=
yyls; struct Cyc_Yyltype* _temp6159= _temp6157.curr + yylsp_offset; if(
_temp6157.base == 0? 1:( _temp6159 < _temp6157.base? 1: _temp6159 >= _temp6157.last_plus_one)){
_throw( Null_Exception);}* _temp6159;}).last_line)); _temp6147;}); _temp6146;});
break; case 288: _LL6145: yyval=({ struct _tagged_ptr2 _temp6161= yyvs; struct
_xtunion_struct** _temp6163= _temp6161.curr + yyvsp_offset; if( _temp6161.base
== 0? 1:( _temp6163 < _temp6161.base? 1: _temp6163 >= _temp6161.last_plus_one)){
_throw( Null_Exception);}* _temp6163;}); break; case 289: _LL6160: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6165=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6165[ 0]=({ struct Cyc_Exp_tok_struct
_temp6166; _temp6166.tag= Cyc_Exp_tok_tag; _temp6166.f1= Cyc_Absyn_assignop_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6167= yyvs; struct _xtunion_struct**
_temp6169= _temp6167.curr +( yyvsp_offset - 2); if( _temp6167.base == 0? 1:(
_temp6169 < _temp6167.base? 1: _temp6169 >= _temp6167.last_plus_one)){ _throw(
Null_Exception);}* _temp6169;})), Cyc_yyget_Primopopt_tok(({ struct _tagged_ptr2
_temp6170= yyvs; struct _xtunion_struct** _temp6172= _temp6170.curr +(
yyvsp_offset - 1); if( _temp6170.base == 0? 1:( _temp6172 < _temp6170.base? 1:
_temp6172 >= _temp6170.last_plus_one)){ _throw( Null_Exception);}* _temp6172;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6173= yyvs; struct _xtunion_struct**
_temp6175= _temp6173.curr + yyvsp_offset; if( _temp6173.base == 0? 1:( _temp6175
< _temp6173.base? 1: _temp6175 >= _temp6173.last_plus_one)){ _throw(
Null_Exception);}* _temp6175;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6176= yyls; struct Cyc_Yyltype* _temp6178= _temp6176.curr +(
yylsp_offset - 2); if( _temp6176.base == 0? 1:( _temp6178 < _temp6176.base? 1:
_temp6178 >= _temp6176.last_plus_one)){ _throw( Null_Exception);}* _temp6178;}).first_line,({
struct _tagged_ptr3 _temp6179= yyls; struct Cyc_Yyltype* _temp6181= _temp6179.curr
+ yylsp_offset; if( _temp6179.base == 0? 1:( _temp6181 < _temp6179.base? 1:
_temp6181 >= _temp6179.last_plus_one)){ _throw( Null_Exception);}* _temp6181;}).last_line));
_temp6166;}); _temp6165;}); break; case 290: _LL6164: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6183=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6183[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6184; _temp6184.tag= Cyc_Primopopt_tok_tag;
_temp6184.f1= 0; _temp6184;}); _temp6183;}); break; case 291: _LL6182: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6186=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6186[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6187; _temp6187.tag= Cyc_Primopopt_tok_tag;
_temp6187.f1=({ struct Cyc_Core_Opt* _temp6188=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6188->v=( void*)(( void*) Cyc_Absyn_Times);
_temp6188;}); _temp6187;}); _temp6186;}); break; case 292: _LL6185: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6190=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6190[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6191; _temp6191.tag= Cyc_Primopopt_tok_tag;
_temp6191.f1=({ struct Cyc_Core_Opt* _temp6192=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6192->v=( void*)(( void*) Cyc_Absyn_Div);
_temp6192;}); _temp6191;}); _temp6190;}); break; case 293: _LL6189: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6194=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6194[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6195; _temp6195.tag= Cyc_Primopopt_tok_tag;
_temp6195.f1=({ struct Cyc_Core_Opt* _temp6196=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6196->v=( void*)(( void*) Cyc_Absyn_Mod);
_temp6196;}); _temp6195;}); _temp6194;}); break; case 294: _LL6193: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6198=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6198[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6199; _temp6199.tag= Cyc_Primopopt_tok_tag;
_temp6199.f1=({ struct Cyc_Core_Opt* _temp6200=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6200->v=( void*)(( void*) Cyc_Absyn_Plus);
_temp6200;}); _temp6199;}); _temp6198;}); break; case 295: _LL6197: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6202=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6202[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6203; _temp6203.tag= Cyc_Primopopt_tok_tag;
_temp6203.f1=({ struct Cyc_Core_Opt* _temp6204=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6204->v=( void*)(( void*) Cyc_Absyn_Minus);
_temp6204;}); _temp6203;}); _temp6202;}); break; case 296: _LL6201: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6206=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6206[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6207; _temp6207.tag= Cyc_Primopopt_tok_tag;
_temp6207.f1=({ struct Cyc_Core_Opt* _temp6208=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6208->v=( void*)(( void*) Cyc_Absyn_Bitlshift);
_temp6208;}); _temp6207;}); _temp6206;}); break; case 297: _LL6205: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6210=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6210[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6211; _temp6211.tag= Cyc_Primopopt_tok_tag;
_temp6211.f1=({ struct Cyc_Core_Opt* _temp6212=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6212->v=( void*)(( void*) Cyc_Absyn_Bitlrshift);
_temp6212;}); _temp6211;}); _temp6210;}); break; case 298: _LL6209: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6214=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6214[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6215; _temp6215.tag= Cyc_Primopopt_tok_tag;
_temp6215.f1=({ struct Cyc_Core_Opt* _temp6216=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6216->v=( void*)(( void*) Cyc_Absyn_Bitand);
_temp6216;}); _temp6215;}); _temp6214;}); break; case 299: _LL6213: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6218=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6218[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6219; _temp6219.tag= Cyc_Primopopt_tok_tag;
_temp6219.f1=({ struct Cyc_Core_Opt* _temp6220=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6220->v=( void*)(( void*) Cyc_Absyn_Bitxor);
_temp6220;}); _temp6219;}); _temp6218;}); break; case 300: _LL6217: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6222=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6222[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6223; _temp6223.tag= Cyc_Primopopt_tok_tag;
_temp6223.f1=({ struct Cyc_Core_Opt* _temp6224=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6224->v=( void*)(( void*) Cyc_Absyn_Bitor);
_temp6224;}); _temp6223;}); _temp6222;}); break; case 301: _LL6221: yyval=({
struct _tagged_ptr2 _temp6226= yyvs; struct _xtunion_struct** _temp6228=
_temp6226.curr + yyvsp_offset; if( _temp6226.base == 0? 1:( _temp6228 <
_temp6226.base? 1: _temp6228 >= _temp6226.last_plus_one)){ _throw(
Null_Exception);}* _temp6228;}); break; case 302: _LL6225: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6230=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6230[ 0]=({ struct Cyc_Exp_tok_struct
_temp6231; _temp6231.tag= Cyc_Exp_tok_tag; _temp6231.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6232= yyvs; struct _xtunion_struct**
_temp6234= _temp6232.curr +( yyvsp_offset - 4); if( _temp6232.base == 0? 1:(
_temp6234 < _temp6232.base? 1: _temp6234 >= _temp6232.last_plus_one)){ _throw(
Null_Exception);}* _temp6234;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6235= yyvs; struct _xtunion_struct** _temp6237= _temp6235.curr +(
yyvsp_offset - 2); if( _temp6235.base == 0? 1:( _temp6237 < _temp6235.base? 1:
_temp6237 >= _temp6235.last_plus_one)){ _throw( Null_Exception);}* _temp6237;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6238= yyvs; struct _xtunion_struct**
_temp6240= _temp6238.curr + yyvsp_offset; if( _temp6238.base == 0? 1:( _temp6240
< _temp6238.base? 1: _temp6240 >= _temp6238.last_plus_one)){ _throw(
Null_Exception);}* _temp6240;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6241= yyls; struct Cyc_Yyltype* _temp6243= _temp6241.curr +(
yylsp_offset - 4); if( _temp6241.base == 0? 1:( _temp6243 < _temp6241.base? 1:
_temp6243 >= _temp6241.last_plus_one)){ _throw( Null_Exception);}* _temp6243;}).first_line,({
struct _tagged_ptr3 _temp6244= yyls; struct Cyc_Yyltype* _temp6246= _temp6244.curr
+ yylsp_offset; if( _temp6244.base == 0? 1:( _temp6246 < _temp6244.base? 1:
_temp6246 >= _temp6244.last_plus_one)){ _throw( Null_Exception);}* _temp6246;}).last_line));
_temp6231;}); _temp6230;}); break; case 303: _LL6229: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6248=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6248[ 0]=({ struct Cyc_Exp_tok_struct
_temp6249; _temp6249.tag= Cyc_Exp_tok_tag; _temp6249.f1= Cyc_Absyn_throw_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6250= yyvs; struct _xtunion_struct**
_temp6252= _temp6250.curr + yyvsp_offset; if( _temp6250.base == 0? 1:( _temp6252
< _temp6250.base? 1: _temp6252 >= _temp6250.last_plus_one)){ _throw(
Null_Exception);}* _temp6252;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6253= yyls; struct Cyc_Yyltype* _temp6255= _temp6253.curr +(
yylsp_offset - 1); if( _temp6253.base == 0? 1:( _temp6255 < _temp6253.base? 1:
_temp6255 >= _temp6253.last_plus_one)){ _throw( Null_Exception);}* _temp6255;}).first_line,({
struct _tagged_ptr3 _temp6256= yyls; struct Cyc_Yyltype* _temp6258= _temp6256.curr
+ yylsp_offset; if( _temp6256.base == 0? 1:( _temp6258 < _temp6256.base? 1:
_temp6258 >= _temp6256.last_plus_one)){ _throw( Null_Exception);}* _temp6258;}).last_line));
_temp6249;}); _temp6248;}); break; case 304: _LL6247: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6260=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6260[ 0]=({ struct Cyc_Exp_tok_struct
_temp6261; _temp6261.tag= Cyc_Exp_tok_tag; _temp6261.f1= Cyc_Absyn_New_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6262= yyvs; struct _xtunion_struct** _temp6264=
_temp6262.curr + yyvsp_offset; if( _temp6262.base == 0? 1:( _temp6264 <
_temp6262.base? 1: _temp6264 >= _temp6262.last_plus_one)){ _throw(
Null_Exception);}* _temp6264;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6265= yyls; struct Cyc_Yyltype* _temp6267= _temp6265.curr +(
yylsp_offset - 1); if( _temp6265.base == 0? 1:( _temp6267 < _temp6265.base? 1:
_temp6267 >= _temp6265.last_plus_one)){ _throw( Null_Exception);}* _temp6267;}).first_line,({
struct _tagged_ptr3 _temp6268= yyls; struct Cyc_Yyltype* _temp6270= _temp6268.curr
+( yylsp_offset + 1); if( _temp6268.base == 0? 1:( _temp6270 < _temp6268.base? 1:
_temp6270 >= _temp6268.last_plus_one)){ _throw( Null_Exception);}* _temp6270;}).last_line));
_temp6261;}); _temp6260;}); break; case 305: _LL6259: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6272=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6272[ 0]=({ struct Cyc_Exp_tok_struct
_temp6273; _temp6273.tag= Cyc_Exp_tok_tag; _temp6273.f1= Cyc_Absyn_New_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6274= yyvs; struct _xtunion_struct** _temp6276=
_temp6274.curr + yyvsp_offset; if( _temp6274.base == 0? 1:( _temp6276 <
_temp6274.base? 1: _temp6276 >= _temp6274.last_plus_one)){ _throw(
Null_Exception);}* _temp6276;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6277= yyls; struct Cyc_Yyltype* _temp6279= _temp6277.curr +(
yylsp_offset - 1); if( _temp6277.base == 0? 1:( _temp6279 < _temp6277.base? 1:
_temp6279 >= _temp6277.last_plus_one)){ _throw( Null_Exception);}* _temp6279;}).first_line,({
struct _tagged_ptr3 _temp6280= yyls; struct Cyc_Yyltype* _temp6282= _temp6280.curr
+( yylsp_offset + 1); if( _temp6280.base == 0? 1:( _temp6282 < _temp6280.base? 1:
_temp6282 >= _temp6280.last_plus_one)){ _throw( Null_Exception);}* _temp6282;}).last_line));
_temp6273;}); _temp6272;}); break; case 306: _LL6271: yyval=({ struct
_tagged_ptr2 _temp6284= yyvs; struct _xtunion_struct** _temp6286= _temp6284.curr
+ yyvsp_offset; if( _temp6284.base == 0? 1:( _temp6286 < _temp6284.base? 1:
_temp6286 >= _temp6284.last_plus_one)){ _throw( Null_Exception);}* _temp6286;});
break; case 307: _LL6283: yyval=({ struct _tagged_ptr2 _temp6288= yyvs; struct
_xtunion_struct** _temp6290= _temp6288.curr + yyvsp_offset; if( _temp6288.base
== 0? 1:( _temp6290 < _temp6288.base? 1: _temp6290 >= _temp6288.last_plus_one)){
_throw( Null_Exception);}* _temp6290;}); break; case 308: _LL6287: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6292=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6292[ 0]=({ struct Cyc_Exp_tok_struct
_temp6293; _temp6293.tag= Cyc_Exp_tok_tag; _temp6293.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6294= yyvs; struct _xtunion_struct** _temp6296=
_temp6294.curr +( yyvsp_offset - 2); if( _temp6294.base == 0? 1:( _temp6296 <
_temp6294.base? 1: _temp6296 >= _temp6294.last_plus_one)){ _throw(
Null_Exception);}* _temp6296;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6297= yyvs; struct _xtunion_struct** _temp6299= _temp6297.curr +
yyvsp_offset; if( _temp6297.base == 0? 1:( _temp6299 < _temp6297.base? 1:
_temp6299 >= _temp6297.last_plus_one)){ _throw( Null_Exception);}* _temp6299;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6300= yyls; struct Cyc_Yyltype*
_temp6302= _temp6300.curr +( yylsp_offset - 2); if( _temp6300.base == 0? 1:(
_temp6302 < _temp6300.base? 1: _temp6302 >= _temp6300.last_plus_one)){ _throw(
Null_Exception);}* _temp6302;}).first_line,({ struct _tagged_ptr3 _temp6303=
yyls; struct Cyc_Yyltype* _temp6305= _temp6303.curr + yylsp_offset; if(
_temp6303.base == 0? 1:( _temp6305 < _temp6303.base? 1: _temp6305 >= _temp6303.last_plus_one)){
_throw( Null_Exception);}* _temp6305;}).last_line)); _temp6293;}); _temp6292;});
break; case 309: _LL6291: yyval=({ struct _tagged_ptr2 _temp6307= yyvs; struct
_xtunion_struct** _temp6309= _temp6307.curr + yyvsp_offset; if( _temp6307.base
== 0? 1:( _temp6309 < _temp6307.base? 1: _temp6309 >= _temp6307.last_plus_one)){
_throw( Null_Exception);}* _temp6309;}); break; case 310: _LL6306: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6311=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6311[ 0]=({ struct Cyc_Exp_tok_struct
_temp6312; _temp6312.tag= Cyc_Exp_tok_tag; _temp6312.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6313= yyvs; struct _xtunion_struct** _temp6315=
_temp6313.curr +( yyvsp_offset - 2); if( _temp6313.base == 0? 1:( _temp6315 <
_temp6313.base? 1: _temp6315 >= _temp6313.last_plus_one)){ _throw(
Null_Exception);}* _temp6315;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6316= yyvs; struct _xtunion_struct** _temp6318= _temp6316.curr +
yyvsp_offset; if( _temp6316.base == 0? 1:( _temp6318 < _temp6316.base? 1:
_temp6318 >= _temp6316.last_plus_one)){ _throw( Null_Exception);}* _temp6318;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6319= yyls; struct Cyc_Yyltype*
_temp6321= _temp6319.curr +( yylsp_offset - 2); if( _temp6319.base == 0? 1:(
_temp6321 < _temp6319.base? 1: _temp6321 >= _temp6319.last_plus_one)){ _throw(
Null_Exception);}* _temp6321;}).first_line,({ struct _tagged_ptr3 _temp6322=
yyls; struct Cyc_Yyltype* _temp6324= _temp6322.curr + yylsp_offset; if(
_temp6322.base == 0? 1:( _temp6324 < _temp6322.base? 1: _temp6324 >= _temp6322.last_plus_one)){
_throw( Null_Exception);}* _temp6324;}).last_line)); _temp6312;}); _temp6311;});
break; case 311: _LL6310: yyval=({ struct _tagged_ptr2 _temp6326= yyvs; struct
_xtunion_struct** _temp6328= _temp6326.curr + yyvsp_offset; if( _temp6326.base
== 0? 1:( _temp6328 < _temp6326.base? 1: _temp6328 >= _temp6326.last_plus_one)){
_throw( Null_Exception);}* _temp6328;}); break; case 312: _LL6325: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6330=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6330[ 0]=({ struct Cyc_Exp_tok_struct
_temp6331; _temp6331.tag= Cyc_Exp_tok_tag; _temp6331.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6332= yyvs;
struct _xtunion_struct** _temp6334= _temp6332.curr +( yyvsp_offset - 2); if(
_temp6332.base == 0? 1:( _temp6334 < _temp6332.base? 1: _temp6334 >= _temp6332.last_plus_one)){
_throw( Null_Exception);}* _temp6334;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6335= yyvs; struct _xtunion_struct** _temp6337= _temp6335.curr
+ yyvsp_offset; if( _temp6335.base == 0? 1:( _temp6337 < _temp6335.base? 1:
_temp6337 >= _temp6335.last_plus_one)){ _throw( Null_Exception);}* _temp6337;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6338= yyls; struct Cyc_Yyltype*
_temp6340= _temp6338.curr +( yylsp_offset - 2); if( _temp6338.base == 0? 1:(
_temp6340 < _temp6338.base? 1: _temp6340 >= _temp6338.last_plus_one)){ _throw(
Null_Exception);}* _temp6340;}).first_line,({ struct _tagged_ptr3 _temp6341=
yyls; struct Cyc_Yyltype* _temp6343= _temp6341.curr + yylsp_offset; if(
_temp6341.base == 0? 1:( _temp6343 < _temp6341.base? 1: _temp6343 >= _temp6341.last_plus_one)){
_throw( Null_Exception);}* _temp6343;}).last_line)); _temp6331;}); _temp6330;});
break; case 313: _LL6329: yyval=({ struct _tagged_ptr2 _temp6345= yyvs; struct
_xtunion_struct** _temp6347= _temp6345.curr + yyvsp_offset; if( _temp6345.base
== 0? 1:( _temp6347 < _temp6345.base? 1: _temp6347 >= _temp6345.last_plus_one)){
_throw( Null_Exception);}* _temp6347;}); break; case 314: _LL6344: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6349=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6349[ 0]=({ struct Cyc_Exp_tok_struct
_temp6350; _temp6350.tag= Cyc_Exp_tok_tag; _temp6350.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6351=
yyvs; struct _xtunion_struct** _temp6353= _temp6351.curr +( yyvsp_offset - 2);
if( _temp6351.base == 0? 1:( _temp6353 < _temp6351.base? 1: _temp6353 >=
_temp6351.last_plus_one)){ _throw( Null_Exception);}* _temp6353;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6354= yyvs; struct _xtunion_struct** _temp6356=
_temp6354.curr + yyvsp_offset; if( _temp6354.base == 0? 1:( _temp6356 <
_temp6354.base? 1: _temp6356 >= _temp6354.last_plus_one)){ _throw(
Null_Exception);}* _temp6356;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6357= yyls; struct Cyc_Yyltype* _temp6359= _temp6357.curr +(
yylsp_offset - 2); if( _temp6357.base == 0? 1:( _temp6359 < _temp6357.base? 1:
_temp6359 >= _temp6357.last_plus_one)){ _throw( Null_Exception);}* _temp6359;}).first_line,({
struct _tagged_ptr3 _temp6360= yyls; struct Cyc_Yyltype* _temp6362= _temp6360.curr
+ yylsp_offset; if( _temp6360.base == 0? 1:( _temp6362 < _temp6360.base? 1:
_temp6362 >= _temp6360.last_plus_one)){ _throw( Null_Exception);}* _temp6362;}).last_line));
_temp6350;}); _temp6349;}); break; case 315: _LL6348: yyval=({ struct
_tagged_ptr2 _temp6364= yyvs; struct _xtunion_struct** _temp6366= _temp6364.curr
+ yyvsp_offset; if( _temp6364.base == 0? 1:( _temp6366 < _temp6364.base? 1:
_temp6366 >= _temp6364.last_plus_one)){ _throw( Null_Exception);}* _temp6366;});
break; case 316: _LL6363: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6368=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6368[ 0]=({ struct Cyc_Exp_tok_struct _temp6369; _temp6369.tag= Cyc_Exp_tok_tag;
_temp6369.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6370= yyvs; struct _xtunion_struct** _temp6372=
_temp6370.curr +( yyvsp_offset - 2); if( _temp6370.base == 0? 1:( _temp6372 <
_temp6370.base? 1: _temp6372 >= _temp6370.last_plus_one)){ _throw(
Null_Exception);}* _temp6372;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6373= yyvs; struct _xtunion_struct** _temp6375= _temp6373.curr +
yyvsp_offset; if( _temp6373.base == 0? 1:( _temp6375 < _temp6373.base? 1:
_temp6375 >= _temp6373.last_plus_one)){ _throw( Null_Exception);}* _temp6375;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6376= yyls; struct Cyc_Yyltype*
_temp6378= _temp6376.curr +( yylsp_offset - 2); if( _temp6376.base == 0? 1:(
_temp6378 < _temp6376.base? 1: _temp6378 >= _temp6376.last_plus_one)){ _throw(
Null_Exception);}* _temp6378;}).first_line,({ struct _tagged_ptr3 _temp6379=
yyls; struct Cyc_Yyltype* _temp6381= _temp6379.curr + yylsp_offset; if(
_temp6379.base == 0? 1:( _temp6381 < _temp6379.base? 1: _temp6381 >= _temp6379.last_plus_one)){
_throw( Null_Exception);}* _temp6381;}).last_line)); _temp6369;}); _temp6368;});
break; case 317: _LL6367: yyval=({ struct _tagged_ptr2 _temp6383= yyvs; struct
_xtunion_struct** _temp6385= _temp6383.curr + yyvsp_offset; if( _temp6383.base
== 0? 1:( _temp6385 < _temp6383.base? 1: _temp6385 >= _temp6383.last_plus_one)){
_throw( Null_Exception);}* _temp6385;}); break; case 318: _LL6382: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6387=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6387[ 0]=({ struct Cyc_Exp_tok_struct
_temp6388; _temp6388.tag= Cyc_Exp_tok_tag; _temp6388.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6389= yyvs; struct _xtunion_struct** _temp6391=
_temp6389.curr +( yyvsp_offset - 2); if( _temp6389.base == 0? 1:( _temp6391 <
_temp6389.base? 1: _temp6391 >= _temp6389.last_plus_one)){ _throw(
Null_Exception);}* _temp6391;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6392= yyvs; struct _xtunion_struct** _temp6394= _temp6392.curr +
yyvsp_offset; if( _temp6392.base == 0? 1:( _temp6394 < _temp6392.base? 1:
_temp6394 >= _temp6392.last_plus_one)){ _throw( Null_Exception);}* _temp6394;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6395= yyls; struct Cyc_Yyltype*
_temp6397= _temp6395.curr +( yylsp_offset - 2); if( _temp6395.base == 0? 1:(
_temp6397 < _temp6395.base? 1: _temp6397 >= _temp6395.last_plus_one)){ _throw(
Null_Exception);}* _temp6397;}).first_line,({ struct _tagged_ptr3 _temp6398=
yyls; struct Cyc_Yyltype* _temp6400= _temp6398.curr + yylsp_offset; if(
_temp6398.base == 0? 1:( _temp6400 < _temp6398.base? 1: _temp6400 >= _temp6398.last_plus_one)){
_throw( Null_Exception);}* _temp6400;}).last_line)); _temp6388;}); _temp6387;});
break; case 319: _LL6386: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6402=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6402[ 0]=({ struct Cyc_Exp_tok_struct _temp6403; _temp6403.tag= Cyc_Exp_tok_tag;
_temp6403.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6404= yyvs; struct _xtunion_struct** _temp6406= _temp6404.curr +(
yyvsp_offset - 2); if( _temp6404.base == 0? 1:( _temp6406 < _temp6404.base? 1:
_temp6406 >= _temp6404.last_plus_one)){ _throw( Null_Exception);}* _temp6406;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6407= yyvs; struct _xtunion_struct**
_temp6409= _temp6407.curr + yyvsp_offset; if( _temp6407.base == 0? 1:( _temp6409
< _temp6407.base? 1: _temp6409 >= _temp6407.last_plus_one)){ _throw(
Null_Exception);}* _temp6409;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6410= yyls; struct Cyc_Yyltype* _temp6412= _temp6410.curr +(
yylsp_offset - 2); if( _temp6410.base == 0? 1:( _temp6412 < _temp6410.base? 1:
_temp6412 >= _temp6410.last_plus_one)){ _throw( Null_Exception);}* _temp6412;}).first_line,({
struct _tagged_ptr3 _temp6413= yyls; struct Cyc_Yyltype* _temp6415= _temp6413.curr
+ yylsp_offset; if( _temp6413.base == 0? 1:( _temp6415 < _temp6413.base? 1:
_temp6415 >= _temp6413.last_plus_one)){ _throw( Null_Exception);}* _temp6415;}).last_line));
_temp6403;}); _temp6402;}); break; case 320: _LL6401: yyval=({ struct
_tagged_ptr2 _temp6417= yyvs; struct _xtunion_struct** _temp6419= _temp6417.curr
+ yyvsp_offset; if( _temp6417.base == 0? 1:( _temp6419 < _temp6417.base? 1:
_temp6419 >= _temp6417.last_plus_one)){ _throw( Null_Exception);}* _temp6419;});
break; case 321: _LL6416: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6421=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6421[ 0]=({ struct Cyc_Exp_tok_struct _temp6422; _temp6422.tag= Cyc_Exp_tok_tag;
_temp6422.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6423= yyvs; struct _xtunion_struct** _temp6425= _temp6423.curr +(
yyvsp_offset - 2); if( _temp6423.base == 0? 1:( _temp6425 < _temp6423.base? 1:
_temp6425 >= _temp6423.last_plus_one)){ _throw( Null_Exception);}* _temp6425;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6426= yyvs; struct _xtunion_struct**
_temp6428= _temp6426.curr + yyvsp_offset; if( _temp6426.base == 0? 1:( _temp6428
< _temp6426.base? 1: _temp6428 >= _temp6426.last_plus_one)){ _throw(
Null_Exception);}* _temp6428;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6429= yyls; struct Cyc_Yyltype* _temp6431= _temp6429.curr +(
yylsp_offset - 2); if( _temp6429.base == 0? 1:( _temp6431 < _temp6429.base? 1:
_temp6431 >= _temp6429.last_plus_one)){ _throw( Null_Exception);}* _temp6431;}).first_line,({
struct _tagged_ptr3 _temp6432= yyls; struct Cyc_Yyltype* _temp6434= _temp6432.curr
+ yylsp_offset; if( _temp6432.base == 0? 1:( _temp6434 < _temp6432.base? 1:
_temp6434 >= _temp6432.last_plus_one)){ _throw( Null_Exception);}* _temp6434;}).last_line));
_temp6422;}); _temp6421;}); break; case 322: _LL6420: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6436=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6436[ 0]=({ struct Cyc_Exp_tok_struct
_temp6437; _temp6437.tag= Cyc_Exp_tok_tag; _temp6437.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6438= yyvs; struct _xtunion_struct** _temp6440=
_temp6438.curr +( yyvsp_offset - 2); if( _temp6438.base == 0? 1:( _temp6440 <
_temp6438.base? 1: _temp6440 >= _temp6438.last_plus_one)){ _throw(
Null_Exception);}* _temp6440;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6441= yyvs; struct _xtunion_struct** _temp6443= _temp6441.curr +
yyvsp_offset; if( _temp6441.base == 0? 1:( _temp6443 < _temp6441.base? 1:
_temp6443 >= _temp6441.last_plus_one)){ _throw( Null_Exception);}* _temp6443;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6444= yyls; struct Cyc_Yyltype*
_temp6446= _temp6444.curr +( yylsp_offset - 2); if( _temp6444.base == 0? 1:(
_temp6446 < _temp6444.base? 1: _temp6446 >= _temp6444.last_plus_one)){ _throw(
Null_Exception);}* _temp6446;}).first_line,({ struct _tagged_ptr3 _temp6447=
yyls; struct Cyc_Yyltype* _temp6449= _temp6447.curr + yylsp_offset; if(
_temp6447.base == 0? 1:( _temp6449 < _temp6447.base? 1: _temp6449 >= _temp6447.last_plus_one)){
_throw( Null_Exception);}* _temp6449;}).last_line)); _temp6437;}); _temp6436;});
break; case 323: _LL6435: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6451=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6451[ 0]=({ struct Cyc_Exp_tok_struct _temp6452; _temp6452.tag= Cyc_Exp_tok_tag;
_temp6452.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6453= yyvs; struct _xtunion_struct** _temp6455= _temp6453.curr +(
yyvsp_offset - 2); if( _temp6453.base == 0? 1:( _temp6455 < _temp6453.base? 1:
_temp6455 >= _temp6453.last_plus_one)){ _throw( Null_Exception);}* _temp6455;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6456= yyvs; struct _xtunion_struct**
_temp6458= _temp6456.curr + yyvsp_offset; if( _temp6456.base == 0? 1:( _temp6458
< _temp6456.base? 1: _temp6458 >= _temp6456.last_plus_one)){ _throw(
Null_Exception);}* _temp6458;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6459= yyls; struct Cyc_Yyltype* _temp6461= _temp6459.curr +(
yylsp_offset - 2); if( _temp6459.base == 0? 1:( _temp6461 < _temp6459.base? 1:
_temp6461 >= _temp6459.last_plus_one)){ _throw( Null_Exception);}* _temp6461;}).first_line,({
struct _tagged_ptr3 _temp6462= yyls; struct Cyc_Yyltype* _temp6464= _temp6462.curr
+ yylsp_offset; if( _temp6462.base == 0? 1:( _temp6464 < _temp6462.base? 1:
_temp6464 >= _temp6462.last_plus_one)){ _throw( Null_Exception);}* _temp6464;}).last_line));
_temp6452;}); _temp6451;}); break; case 324: _LL6450: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6466=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6466[ 0]=({ struct Cyc_Exp_tok_struct
_temp6467; _temp6467.tag= Cyc_Exp_tok_tag; _temp6467.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6468= yyvs; struct _xtunion_struct** _temp6470=
_temp6468.curr +( yyvsp_offset - 2); if( _temp6468.base == 0? 1:( _temp6470 <
_temp6468.base? 1: _temp6470 >= _temp6468.last_plus_one)){ _throw(
Null_Exception);}* _temp6470;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6471= yyvs; struct _xtunion_struct** _temp6473= _temp6471.curr +
yyvsp_offset; if( _temp6471.base == 0? 1:( _temp6473 < _temp6471.base? 1:
_temp6473 >= _temp6471.last_plus_one)){ _throw( Null_Exception);}* _temp6473;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6474= yyls; struct Cyc_Yyltype*
_temp6476= _temp6474.curr +( yylsp_offset - 2); if( _temp6474.base == 0? 1:(
_temp6476 < _temp6474.base? 1: _temp6476 >= _temp6474.last_plus_one)){ _throw(
Null_Exception);}* _temp6476;}).first_line,({ struct _tagged_ptr3 _temp6477=
yyls; struct Cyc_Yyltype* _temp6479= _temp6477.curr + yylsp_offset; if(
_temp6477.base == 0? 1:( _temp6479 < _temp6477.base? 1: _temp6479 >= _temp6477.last_plus_one)){
_throw( Null_Exception);}* _temp6479;}).last_line)); _temp6467;}); _temp6466;});
break; case 325: _LL6465: yyval=({ struct _tagged_ptr2 _temp6481= yyvs; struct
_xtunion_struct** _temp6483= _temp6481.curr + yyvsp_offset; if( _temp6481.base
== 0? 1:( _temp6483 < _temp6481.base? 1: _temp6483 >= _temp6481.last_plus_one)){
_throw( Null_Exception);}* _temp6483;}); break; case 326: _LL6480: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6485=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6485[ 0]=({ struct Cyc_Exp_tok_struct
_temp6486; _temp6486.tag= Cyc_Exp_tok_tag; _temp6486.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6487=
yyvs; struct _xtunion_struct** _temp6489= _temp6487.curr +( yyvsp_offset - 2);
if( _temp6487.base == 0? 1:( _temp6489 < _temp6487.base? 1: _temp6489 >=
_temp6487.last_plus_one)){ _throw( Null_Exception);}* _temp6489;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6490= yyvs; struct _xtunion_struct** _temp6492=
_temp6490.curr + yyvsp_offset; if( _temp6490.base == 0? 1:( _temp6492 <
_temp6490.base? 1: _temp6492 >= _temp6490.last_plus_one)){ _throw(
Null_Exception);}* _temp6492;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6493= yyls; struct Cyc_Yyltype* _temp6495= _temp6493.curr +(
yylsp_offset - 2); if( _temp6493.base == 0? 1:( _temp6495 < _temp6493.base? 1:
_temp6495 >= _temp6493.last_plus_one)){ _throw( Null_Exception);}* _temp6495;}).first_line,({
struct _tagged_ptr3 _temp6496= yyls; struct Cyc_Yyltype* _temp6498= _temp6496.curr
+ yylsp_offset; if( _temp6496.base == 0? 1:( _temp6498 < _temp6496.base? 1:
_temp6498 >= _temp6496.last_plus_one)){ _throw( Null_Exception);}* _temp6498;}).last_line));
_temp6486;}); _temp6485;}); break; case 327: _LL6484: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6500=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6500[ 0]=({ struct Cyc_Exp_tok_struct
_temp6501; _temp6501.tag= Cyc_Exp_tok_tag; _temp6501.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6502=
yyvs; struct _xtunion_struct** _temp6504= _temp6502.curr +( yyvsp_offset - 2);
if( _temp6502.base == 0? 1:( _temp6504 < _temp6502.base? 1: _temp6504 >=
_temp6502.last_plus_one)){ _throw( Null_Exception);}* _temp6504;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6505= yyvs; struct _xtunion_struct** _temp6507=
_temp6505.curr + yyvsp_offset; if( _temp6505.base == 0? 1:( _temp6507 <
_temp6505.base? 1: _temp6507 >= _temp6505.last_plus_one)){ _throw(
Null_Exception);}* _temp6507;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6508= yyls; struct Cyc_Yyltype* _temp6510= _temp6508.curr +(
yylsp_offset - 2); if( _temp6508.base == 0? 1:( _temp6510 < _temp6508.base? 1:
_temp6510 >= _temp6508.last_plus_one)){ _throw( Null_Exception);}* _temp6510;}).first_line,({
struct _tagged_ptr3 _temp6511= yyls; struct Cyc_Yyltype* _temp6513= _temp6511.curr
+ yylsp_offset; if( _temp6511.base == 0? 1:( _temp6513 < _temp6511.base? 1:
_temp6513 >= _temp6511.last_plus_one)){ _throw( Null_Exception);}* _temp6513;}).last_line));
_temp6501;}); _temp6500;}); break; case 328: _LL6499: yyval=({ struct
_tagged_ptr2 _temp6515= yyvs; struct _xtunion_struct** _temp6517= _temp6515.curr
+ yyvsp_offset; if( _temp6515.base == 0? 1:( _temp6517 < _temp6515.base? 1:
_temp6517 >= _temp6515.last_plus_one)){ _throw( Null_Exception);}* _temp6517;});
break; case 329: _LL6514: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6519=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6519[ 0]=({ struct Cyc_Exp_tok_struct _temp6520; _temp6520.tag= Cyc_Exp_tok_tag;
_temp6520.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6521= yyvs; struct _xtunion_struct** _temp6523=
_temp6521.curr +( yyvsp_offset - 2); if( _temp6521.base == 0? 1:( _temp6523 <
_temp6521.base? 1: _temp6523 >= _temp6521.last_plus_one)){ _throw(
Null_Exception);}* _temp6523;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6524= yyvs; struct _xtunion_struct** _temp6526= _temp6524.curr +
yyvsp_offset; if( _temp6524.base == 0? 1:( _temp6526 < _temp6524.base? 1:
_temp6526 >= _temp6524.last_plus_one)){ _throw( Null_Exception);}* _temp6526;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6527= yyls; struct Cyc_Yyltype*
_temp6529= _temp6527.curr +( yylsp_offset - 2); if( _temp6527.base == 0? 1:(
_temp6529 < _temp6527.base? 1: _temp6529 >= _temp6527.last_plus_one)){ _throw(
Null_Exception);}* _temp6529;}).first_line,({ struct _tagged_ptr3 _temp6530=
yyls; struct Cyc_Yyltype* _temp6532= _temp6530.curr + yylsp_offset; if(
_temp6530.base == 0? 1:( _temp6532 < _temp6530.base? 1: _temp6532 >= _temp6530.last_plus_one)){
_throw( Null_Exception);}* _temp6532;}).last_line)); _temp6520;}); _temp6519;});
break; case 330: _LL6518: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6534=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6534[ 0]=({ struct Cyc_Exp_tok_struct _temp6535; _temp6535.tag= Cyc_Exp_tok_tag;
_temp6535.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6536= yyvs; struct _xtunion_struct** _temp6538=
_temp6536.curr +( yyvsp_offset - 2); if( _temp6536.base == 0? 1:( _temp6538 <
_temp6536.base? 1: _temp6538 >= _temp6536.last_plus_one)){ _throw(
Null_Exception);}* _temp6538;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6539= yyvs; struct _xtunion_struct** _temp6541= _temp6539.curr +
yyvsp_offset; if( _temp6539.base == 0? 1:( _temp6541 < _temp6539.base? 1:
_temp6541 >= _temp6539.last_plus_one)){ _throw( Null_Exception);}* _temp6541;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6542= yyls; struct Cyc_Yyltype*
_temp6544= _temp6542.curr +( yylsp_offset - 2); if( _temp6542.base == 0? 1:(
_temp6544 < _temp6542.base? 1: _temp6544 >= _temp6542.last_plus_one)){ _throw(
Null_Exception);}* _temp6544;}).first_line,({ struct _tagged_ptr3 _temp6545=
yyls; struct Cyc_Yyltype* _temp6547= _temp6545.curr + yylsp_offset; if(
_temp6545.base == 0? 1:( _temp6547 < _temp6545.base? 1: _temp6547 >= _temp6545.last_plus_one)){
_throw( Null_Exception);}* _temp6547;}).last_line)); _temp6535;}); _temp6534;});
break; case 331: _LL6533: yyval=({ struct _tagged_ptr2 _temp6549= yyvs; struct
_xtunion_struct** _temp6551= _temp6549.curr + yyvsp_offset; if( _temp6549.base
== 0? 1:( _temp6551 < _temp6549.base? 1: _temp6551 >= _temp6549.last_plus_one)){
_throw( Null_Exception);}* _temp6551;}); break; case 332: _LL6548: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6553=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6553[ 0]=({ struct Cyc_Exp_tok_struct
_temp6554; _temp6554.tag= Cyc_Exp_tok_tag; _temp6554.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Times, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6555= yyvs;
struct _xtunion_struct** _temp6557= _temp6555.curr +( yyvsp_offset - 2); if(
_temp6555.base == 0? 1:( _temp6557 < _temp6555.base? 1: _temp6557 >= _temp6555.last_plus_one)){
_throw( Null_Exception);}* _temp6557;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6558= yyvs; struct _xtunion_struct** _temp6560= _temp6558.curr
+ yyvsp_offset; if( _temp6558.base == 0? 1:( _temp6560 < _temp6558.base? 1:
_temp6560 >= _temp6558.last_plus_one)){ _throw( Null_Exception);}* _temp6560;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6561= yyls; struct Cyc_Yyltype*
_temp6563= _temp6561.curr +( yylsp_offset - 2); if( _temp6561.base == 0? 1:(
_temp6563 < _temp6561.base? 1: _temp6563 >= _temp6561.last_plus_one)){ _throw(
Null_Exception);}* _temp6563;}).first_line,({ struct _tagged_ptr3 _temp6564=
yyls; struct Cyc_Yyltype* _temp6566= _temp6564.curr + yylsp_offset; if(
_temp6564.base == 0? 1:( _temp6566 < _temp6564.base? 1: _temp6566 >= _temp6564.last_plus_one)){
_throw( Null_Exception);}* _temp6566;}).last_line)); _temp6554;}); _temp6553;});
break; case 333: _LL6552: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6568=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6568[ 0]=({ struct Cyc_Exp_tok_struct _temp6569; _temp6569.tag= Cyc_Exp_tok_tag;
_temp6569.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6570= yyvs; struct _xtunion_struct** _temp6572=
_temp6570.curr +( yyvsp_offset - 2); if( _temp6570.base == 0? 1:( _temp6572 <
_temp6570.base? 1: _temp6572 >= _temp6570.last_plus_one)){ _throw(
Null_Exception);}* _temp6572;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6573= yyvs; struct _xtunion_struct** _temp6575= _temp6573.curr +
yyvsp_offset; if( _temp6573.base == 0? 1:( _temp6575 < _temp6573.base? 1:
_temp6575 >= _temp6573.last_plus_one)){ _throw( Null_Exception);}* _temp6575;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6576= yyls; struct Cyc_Yyltype*
_temp6578= _temp6576.curr +( yylsp_offset - 2); if( _temp6576.base == 0? 1:(
_temp6578 < _temp6576.base? 1: _temp6578 >= _temp6576.last_plus_one)){ _throw(
Null_Exception);}* _temp6578;}).first_line,({ struct _tagged_ptr3 _temp6579=
yyls; struct Cyc_Yyltype* _temp6581= _temp6579.curr + yylsp_offset; if(
_temp6579.base == 0? 1:( _temp6581 < _temp6579.base? 1: _temp6581 >= _temp6579.last_plus_one)){
_throw( Null_Exception);}* _temp6581;}).last_line)); _temp6569;}); _temp6568;});
break; case 334: _LL6567: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6583=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6583[ 0]=({ struct Cyc_Exp_tok_struct _temp6584; _temp6584.tag= Cyc_Exp_tok_tag;
_temp6584.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6585= yyvs; struct _xtunion_struct** _temp6587=
_temp6585.curr +( yyvsp_offset - 2); if( _temp6585.base == 0? 1:( _temp6587 <
_temp6585.base? 1: _temp6587 >= _temp6585.last_plus_one)){ _throw(
Null_Exception);}* _temp6587;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6588= yyvs; struct _xtunion_struct** _temp6590= _temp6588.curr +
yyvsp_offset; if( _temp6588.base == 0? 1:( _temp6590 < _temp6588.base? 1:
_temp6590 >= _temp6588.last_plus_one)){ _throw( Null_Exception);}* _temp6590;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6591= yyls; struct Cyc_Yyltype*
_temp6593= _temp6591.curr +( yylsp_offset - 2); if( _temp6591.base == 0? 1:(
_temp6593 < _temp6591.base? 1: _temp6593 >= _temp6591.last_plus_one)){ _throw(
Null_Exception);}* _temp6593;}).first_line,({ struct _tagged_ptr3 _temp6594=
yyls; struct Cyc_Yyltype* _temp6596= _temp6594.curr + yylsp_offset; if(
_temp6594.base == 0? 1:( _temp6596 < _temp6594.base? 1: _temp6596 >= _temp6594.last_plus_one)){
_throw( Null_Exception);}* _temp6596;}).last_line)); _temp6584;}); _temp6583;});
break; case 335: _LL6582: yyval=({ struct _tagged_ptr2 _temp6598= yyvs; struct
_xtunion_struct** _temp6600= _temp6598.curr + yyvsp_offset; if( _temp6598.base
== 0? 1:( _temp6600 < _temp6598.base? 1: _temp6600 >= _temp6598.last_plus_one)){
_throw( Null_Exception);}* _temp6600;}); break; case 336: _LL6597: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6602=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6602[ 0]=({ struct Cyc_Exp_tok_struct
_temp6603; _temp6603.tag= Cyc_Exp_tok_tag; _temp6603.f1= Cyc_Absyn_cast_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6604= yyvs; struct
_xtunion_struct** _temp6606= _temp6604.curr +( yyvsp_offset - 2); if( _temp6604.base
== 0? 1:( _temp6606 < _temp6604.base? 1: _temp6606 >= _temp6604.last_plus_one)){
_throw( Null_Exception);}* _temp6606;}))).f3, Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6607= yyvs; struct _xtunion_struct** _temp6609= _temp6607.curr
+ yyvsp_offset; if( _temp6607.base == 0? 1:( _temp6609 < _temp6607.base? 1:
_temp6609 >= _temp6607.last_plus_one)){ _throw( Null_Exception);}* _temp6609;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6610= yyls; struct Cyc_Yyltype*
_temp6612= _temp6610.curr +( yylsp_offset - 3); if( _temp6610.base == 0? 1:(
_temp6612 < _temp6610.base? 1: _temp6612 >= _temp6610.last_plus_one)){ _throw(
Null_Exception);}* _temp6612;}).first_line,({ struct _tagged_ptr3 _temp6613=
yyls; struct Cyc_Yyltype* _temp6615= _temp6613.curr + yylsp_offset; if(
_temp6613.base == 0? 1:( _temp6615 < _temp6613.base? 1: _temp6615 >= _temp6613.last_plus_one)){
_throw( Null_Exception);}* _temp6615;}).last_line)); _temp6603;}); _temp6602;});
break; case 337: _LL6601: yyval=({ struct _tagged_ptr2 _temp6617= yyvs; struct
_xtunion_struct** _temp6619= _temp6617.curr + yyvsp_offset; if( _temp6617.base
== 0? 1:( _temp6619 < _temp6617.base? 1: _temp6619 >= _temp6617.last_plus_one)){
_throw( Null_Exception);}* _temp6619;}); break; case 338: _LL6616: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6621=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6621[ 0]=({ struct Cyc_Exp_tok_struct
_temp6622; _temp6622.tag= Cyc_Exp_tok_tag; _temp6622.f1= Cyc_Absyn_pre_inc_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6623= yyvs; struct _xtunion_struct**
_temp6625= _temp6623.curr + yyvsp_offset; if( _temp6623.base == 0? 1:( _temp6625
< _temp6623.base? 1: _temp6625 >= _temp6623.last_plus_one)){ _throw(
Null_Exception);}* _temp6625;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6626= yyls; struct Cyc_Yyltype* _temp6628= _temp6626.curr +(
yylsp_offset - 1); if( _temp6626.base == 0? 1:( _temp6628 < _temp6626.base? 1:
_temp6628 >= _temp6626.last_plus_one)){ _throw( Null_Exception);}* _temp6628;}).first_line,({
struct _tagged_ptr3 _temp6629= yyls; struct Cyc_Yyltype* _temp6631= _temp6629.curr
+ yylsp_offset; if( _temp6629.base == 0? 1:( _temp6631 < _temp6629.base? 1:
_temp6631 >= _temp6629.last_plus_one)){ _throw( Null_Exception);}* _temp6631;}).last_line));
_temp6622;}); _temp6621;}); break; case 339: _LL6620: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6633=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6633[ 0]=({ struct Cyc_Exp_tok_struct
_temp6634; _temp6634.tag= Cyc_Exp_tok_tag; _temp6634.f1= Cyc_Absyn_pre_dec_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6635= yyvs; struct _xtunion_struct**
_temp6637= _temp6635.curr + yyvsp_offset; if( _temp6635.base == 0? 1:( _temp6637
< _temp6635.base? 1: _temp6637 >= _temp6635.last_plus_one)){ _throw(
Null_Exception);}* _temp6637;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6638= yyls; struct Cyc_Yyltype* _temp6640= _temp6638.curr +(
yylsp_offset - 1); if( _temp6638.base == 0? 1:( _temp6640 < _temp6638.base? 1:
_temp6640 >= _temp6638.last_plus_one)){ _throw( Null_Exception);}* _temp6640;}).first_line,({
struct _tagged_ptr3 _temp6641= yyls; struct Cyc_Yyltype* _temp6643= _temp6641.curr
+ yylsp_offset; if( _temp6641.base == 0? 1:( _temp6643 < _temp6641.base? 1:
_temp6643 >= _temp6641.last_plus_one)){ _throw( Null_Exception);}* _temp6643;}).last_line));
_temp6634;}); _temp6633;}); break; case 340: _LL6632: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6645=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6645[ 0]=({ struct Cyc_Exp_tok_struct
_temp6646; _temp6646.tag= Cyc_Exp_tok_tag; _temp6646.f1= Cyc_Absyn_address_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6647= yyvs; struct _xtunion_struct**
_temp6649= _temp6647.curr + yyvsp_offset; if( _temp6647.base == 0? 1:( _temp6649
< _temp6647.base? 1: _temp6649 >= _temp6647.last_plus_one)){ _throw(
Null_Exception);}* _temp6649;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6650= yyls; struct Cyc_Yyltype* _temp6652= _temp6650.curr +(
yylsp_offset - 1); if( _temp6650.base == 0? 1:( _temp6652 < _temp6650.base? 1:
_temp6652 >= _temp6650.last_plus_one)){ _throw( Null_Exception);}* _temp6652;}).first_line,({
struct _tagged_ptr3 _temp6653= yyls; struct Cyc_Yyltype* _temp6655= _temp6653.curr
+ yylsp_offset; if( _temp6653.base == 0? 1:( _temp6655 < _temp6653.base? 1:
_temp6655 >= _temp6653.last_plus_one)){ _throw( Null_Exception);}* _temp6655;}).last_line));
_temp6646;}); _temp6645;}); break; case 341: _LL6644: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6657=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6657[ 0]=({ struct Cyc_Exp_tok_struct
_temp6658; _temp6658.tag= Cyc_Exp_tok_tag; _temp6658.f1= Cyc_Absyn_deref_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6659= yyvs; struct _xtunion_struct**
_temp6661= _temp6659.curr + yyvsp_offset; if( _temp6659.base == 0? 1:( _temp6661
< _temp6659.base? 1: _temp6661 >= _temp6659.last_plus_one)){ _throw(
Null_Exception);}* _temp6661;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6662= yyls; struct Cyc_Yyltype* _temp6664= _temp6662.curr +(
yylsp_offset - 1); if( _temp6662.base == 0? 1:( _temp6664 < _temp6662.base? 1:
_temp6664 >= _temp6662.last_plus_one)){ _throw( Null_Exception);}* _temp6664;}).first_line,({
struct _tagged_ptr3 _temp6665= yyls; struct Cyc_Yyltype* _temp6667= _temp6665.curr
+ yylsp_offset; if( _temp6665.base == 0? 1:( _temp6667 < _temp6665.base? 1:
_temp6667 >= _temp6665.last_plus_one)){ _throw( Null_Exception);}* _temp6667;}).last_line));
_temp6658;}); _temp6657;}); break; case 342: _LL6656: yyval=({ struct
_tagged_ptr2 _temp6669= yyvs; struct _xtunion_struct** _temp6671= _temp6669.curr
+ yyvsp_offset; if( _temp6669.base == 0? 1:( _temp6671 < _temp6669.base? 1:
_temp6671 >= _temp6669.last_plus_one)){ _throw( Null_Exception);}* _temp6671;});
break; case 343: _LL6668: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6673=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6673[ 0]=({ struct Cyc_Exp_tok_struct _temp6674; _temp6674.tag= Cyc_Exp_tok_tag;
_temp6674.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(({ struct _tagged_ptr2
_temp6675= yyvs; struct _xtunion_struct** _temp6677= _temp6675.curr +(
yyvsp_offset - 1); if( _temp6675.base == 0? 1:( _temp6677 < _temp6675.base? 1:
_temp6677 >= _temp6675.last_plus_one)){ _throw( Null_Exception);}* _temp6677;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6678= yyvs; struct _xtunion_struct**
_temp6680= _temp6678.curr + yyvsp_offset; if( _temp6678.base == 0? 1:( _temp6680
< _temp6678.base? 1: _temp6680 >= _temp6678.last_plus_one)){ _throw(
Null_Exception);}* _temp6680;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6681= yyls; struct Cyc_Yyltype* _temp6683= _temp6681.curr +(
yylsp_offset - 1); if( _temp6681.base == 0? 1:( _temp6683 < _temp6681.base? 1:
_temp6683 >= _temp6681.last_plus_one)){ _throw( Null_Exception);}* _temp6683;}).first_line,({
struct _tagged_ptr3 _temp6684= yyls; struct Cyc_Yyltype* _temp6686= _temp6684.curr
+ yylsp_offset; if( _temp6684.base == 0? 1:( _temp6686 < _temp6684.base? 1:
_temp6686 >= _temp6684.last_plus_one)){ _throw( Null_Exception);}* _temp6686;}).last_line));
_temp6674;}); _temp6673;}); break; case 344: _LL6672: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6688=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6688[ 0]=({ struct Cyc_Exp_tok_struct
_temp6689; _temp6689.tag= Cyc_Exp_tok_tag; _temp6689.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6690= yyvs; struct
_xtunion_struct** _temp6692= _temp6690.curr +( yyvsp_offset - 1); if( _temp6690.base
== 0? 1:( _temp6692 < _temp6690.base? 1: _temp6692 >= _temp6690.last_plus_one)){
_throw( Null_Exception);}* _temp6692;}))).f3, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6693= yyls; struct Cyc_Yyltype* _temp6695= _temp6693.curr
+( yylsp_offset - 3); if( _temp6693.base == 0? 1:( _temp6695 < _temp6693.base? 1:
_temp6695 >= _temp6693.last_plus_one)){ _throw( Null_Exception);}* _temp6695;}).first_line,({
struct _tagged_ptr3 _temp6696= yyls; struct Cyc_Yyltype* _temp6698= _temp6696.curr
+ yylsp_offset; if( _temp6696.base == 0? 1:( _temp6698 < _temp6696.base? 1:
_temp6698 >= _temp6696.last_plus_one)){ _throw( Null_Exception);}* _temp6698;}).last_line));
_temp6689;}); _temp6688;}); break; case 345: _LL6687: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6700=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6700[ 0]=({ struct Cyc_Exp_tok_struct
_temp6701; _temp6701.tag= Cyc_Exp_tok_tag; _temp6701.f1= Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6702= yyvs; struct _xtunion_struct**
_temp6704= _temp6702.curr + yyvsp_offset; if( _temp6702.base == 0? 1:( _temp6704
< _temp6702.base? 1: _temp6704 >= _temp6702.last_plus_one)){ _throw(
Null_Exception);}* _temp6704;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6705= yyls; struct Cyc_Yyltype* _temp6707= _temp6705.curr +(
yylsp_offset - 1); if( _temp6705.base == 0? 1:( _temp6707 < _temp6705.base? 1:
_temp6707 >= _temp6705.last_plus_one)){ _throw( Null_Exception);}* _temp6707;}).first_line,({
struct _tagged_ptr3 _temp6708= yyls; struct Cyc_Yyltype* _temp6710= _temp6708.curr
+ yylsp_offset; if( _temp6708.base == 0? 1:( _temp6710 < _temp6708.base? 1:
_temp6710 >= _temp6708.last_plus_one)){ _throw( Null_Exception);}* _temp6710;}).last_line));
_temp6701;}); _temp6700;}); break; case 346: _LL6699: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6712=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6712[ 0]=({ struct Cyc_Exp_tok_struct
_temp6713; _temp6713.tag= Cyc_Exp_tok_tag; _temp6713.f1= Cyc_Absyn_primop_exp(
Cyc_yyget_Primop_tok(({ struct _tagged_ptr2 _temp6714= yyvs; struct
_xtunion_struct** _temp6716= _temp6714.curr +( yyvsp_offset - 3); if( _temp6714.base
== 0? 1:( _temp6716 < _temp6714.base? 1: _temp6716 >= _temp6714.last_plus_one)){
_throw( Null_Exception);}* _temp6716;})), Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp6717= yyvs; struct _xtunion_struct** _temp6719= _temp6717.curr
+( yyvsp_offset - 1); if( _temp6717.base == 0? 1:( _temp6719 < _temp6717.base? 1:
_temp6719 >= _temp6717.last_plus_one)){ _throw( Null_Exception);}* _temp6719;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6720= yyls; struct Cyc_Yyltype*
_temp6722= _temp6720.curr +( yylsp_offset - 3); if( _temp6720.base == 0? 1:(
_temp6722 < _temp6720.base? 1: _temp6722 >= _temp6720.last_plus_one)){ _throw(
Null_Exception);}* _temp6722;}).first_line,({ struct _tagged_ptr3 _temp6723=
yyls; struct Cyc_Yyltype* _temp6725= _temp6723.curr + yylsp_offset; if(
_temp6723.base == 0? 1:( _temp6725 < _temp6723.base? 1: _temp6725 >= _temp6723.last_plus_one)){
_throw( Null_Exception);}* _temp6725;}).last_line)); _temp6713;}); _temp6712;});
break; case 347: _LL6711: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6727=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6727[ 0]=({ struct Cyc_Exp_tok_struct _temp6728; _temp6728.tag= Cyc_Exp_tok_tag;
_temp6728.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp6729=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp6729[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp6730; _temp6730.tag= Cyc_Absyn_Malloc_e_tag;
_temp6730.f1=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp6731= yyvs; struct _xtunion_struct** _temp6733=
_temp6731.curr +( yyvsp_offset - 2); if( _temp6731.base == 0? 1:( _temp6733 <
_temp6731.base? 1: _temp6733 >= _temp6731.last_plus_one)){ _throw(
Null_Exception);}* _temp6733;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6734= yyls; struct Cyc_Yyltype* _temp6736= _temp6734.curr +(
yylsp_offset - 2); if( _temp6734.base == 0? 1:( _temp6736 < _temp6734.base? 1:
_temp6736 >= _temp6734.last_plus_one)){ _throw( Null_Exception);}* _temp6736;}).first_line,({
struct _tagged_ptr3 _temp6737= yyls; struct Cyc_Yyltype* _temp6739= _temp6737.curr
+( yylsp_offset - 2); if( _temp6737.base == 0? 1:( _temp6739 < _temp6737.base? 1:
_temp6739 >= _temp6737.last_plus_one)){ _throw( Null_Exception);}* _temp6739;}).last_line));
_temp6730;}); _temp6729;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6740= yyls; struct Cyc_Yyltype* _temp6742= _temp6740.curr +( yylsp_offset -
6); if( _temp6740.base == 0? 1:( _temp6742 < _temp6740.base? 1: _temp6742 >=
_temp6740.last_plus_one)){ _throw( Null_Exception);}* _temp6742;}).first_line,({
struct _tagged_ptr3 _temp6743= yyls; struct Cyc_Yyltype* _temp6745= _temp6743.curr
+ yylsp_offset; if( _temp6743.base == 0? 1:( _temp6745 < _temp6743.base? 1:
_temp6745 >= _temp6743.last_plus_one)){ _throw( Null_Exception);}* _temp6745;}).last_line));
_temp6728;}); _temp6727;}); break; case 348: _LL6726: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6747=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6747[ 0]=({ struct Cyc_Primop_tok_struct
_temp6748; _temp6748.tag= Cyc_Primop_tok_tag; _temp6748.f1=( void*)(( void*) Cyc_Absyn_Printf);
_temp6748;}); _temp6747;}); break; case 349: _LL6746: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6750=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6750[ 0]=({ struct Cyc_Primop_tok_struct
_temp6751; _temp6751.tag= Cyc_Primop_tok_tag; _temp6751.f1=( void*)(( void*) Cyc_Absyn_Fprintf);
_temp6751;}); _temp6750;}); break; case 350: _LL6749: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6753=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6753[ 0]=({ struct Cyc_Primop_tok_struct
_temp6754; _temp6754.tag= Cyc_Primop_tok_tag; _temp6754.f1=( void*)(( void*) Cyc_Absyn_Xprintf);
_temp6754;}); _temp6753;}); break; case 351: _LL6752: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6756=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6756[ 0]=({ struct Cyc_Primop_tok_struct
_temp6757; _temp6757.tag= Cyc_Primop_tok_tag; _temp6757.f1=( void*)(( void*) Cyc_Absyn_Scanf);
_temp6757;}); _temp6756;}); break; case 352: _LL6755: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6759=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6759[ 0]=({ struct Cyc_Primop_tok_struct
_temp6760; _temp6760.tag= Cyc_Primop_tok_tag; _temp6760.f1=( void*)(( void*) Cyc_Absyn_Fscanf);
_temp6760;}); _temp6759;}); break; case 353: _LL6758: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6762=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6762[ 0]=({ struct Cyc_Primop_tok_struct
_temp6763; _temp6763.tag= Cyc_Primop_tok_tag; _temp6763.f1=( void*)(( void*) Cyc_Absyn_Sscanf);
_temp6763;}); _temp6762;}); break; case 354: _LL6761: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6765=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6765[ 0]=({ struct Cyc_Primop_tok_struct
_temp6766; _temp6766.tag= Cyc_Primop_tok_tag; _temp6766.f1=( void*)(( void*) Cyc_Absyn_Bitnot);
_temp6766;}); _temp6765;}); break; case 355: _LL6764: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6768=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6768[ 0]=({ struct Cyc_Primop_tok_struct
_temp6769; _temp6769.tag= Cyc_Primop_tok_tag; _temp6769.f1=( void*)(( void*) Cyc_Absyn_Not);
_temp6769;}); _temp6768;}); break; case 356: _LL6767: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6771=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6771[ 0]=({ struct Cyc_Primop_tok_struct
_temp6772; _temp6772.tag= Cyc_Primop_tok_tag; _temp6772.f1=( void*)(( void*) Cyc_Absyn_Minus);
_temp6772;}); _temp6771;}); break; case 357: _LL6770: yyval=({ struct
_tagged_ptr2 _temp6774= yyvs; struct _xtunion_struct** _temp6776= _temp6774.curr
+ yyvsp_offset; if( _temp6774.base == 0? 1:( _temp6776 < _temp6774.base? 1:
_temp6776 >= _temp6774.last_plus_one)){ _throw( Null_Exception);}* _temp6776;});
break; case 358: _LL6773: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6778=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6778[ 0]=({ struct Cyc_Exp_tok_struct _temp6779; _temp6779.tag= Cyc_Exp_tok_tag;
_temp6779.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6780= yyvs; struct _xtunion_struct** _temp6782= _temp6780.curr +(
yyvsp_offset - 3); if( _temp6780.base == 0? 1:( _temp6782 < _temp6780.base? 1:
_temp6782 >= _temp6780.last_plus_one)){ _throw( Null_Exception);}* _temp6782;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6783= yyvs; struct _xtunion_struct**
_temp6785= _temp6783.curr +( yyvsp_offset - 1); if( _temp6783.base == 0? 1:(
_temp6785 < _temp6783.base? 1: _temp6785 >= _temp6783.last_plus_one)){ _throw(
Null_Exception);}* _temp6785;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6786= yyls; struct Cyc_Yyltype* _temp6788= _temp6786.curr +(
yylsp_offset - 3); if( _temp6786.base == 0? 1:( _temp6788 < _temp6786.base? 1:
_temp6788 >= _temp6786.last_plus_one)){ _throw( Null_Exception);}* _temp6788;}).first_line,({
struct _tagged_ptr3 _temp6789= yyls; struct Cyc_Yyltype* _temp6791= _temp6789.curr
+ yylsp_offset; if( _temp6789.base == 0? 1:( _temp6791 < _temp6789.base? 1:
_temp6791 >= _temp6789.last_plus_one)){ _throw( Null_Exception);}* _temp6791;}).last_line));
_temp6779;}); _temp6778;}); break; case 359: _LL6777: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6793=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6793[ 0]=({ struct Cyc_Exp_tok_struct
_temp6794; _temp6794.tag= Cyc_Exp_tok_tag; _temp6794.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6795= yyvs; struct _xtunion_struct**
_temp6797= _temp6795.curr +( yyvsp_offset - 2); if( _temp6795.base == 0? 1:(
_temp6797 < _temp6795.base? 1: _temp6797 >= _temp6795.last_plus_one)){ _throw(
Null_Exception);}* _temp6797;})), 0, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6798= yyls; struct Cyc_Yyltype* _temp6800= _temp6798.curr +(
yylsp_offset - 2); if( _temp6798.base == 0? 1:( _temp6800 < _temp6798.base? 1:
_temp6800 >= _temp6798.last_plus_one)){ _throw( Null_Exception);}* _temp6800;}).first_line,({
struct _tagged_ptr3 _temp6801= yyls; struct Cyc_Yyltype* _temp6803= _temp6801.curr
+ yylsp_offset; if( _temp6801.base == 0? 1:( _temp6803 < _temp6801.base? 1:
_temp6803 >= _temp6801.last_plus_one)){ _throw( Null_Exception);}* _temp6803;}).last_line));
_temp6794;}); _temp6793;}); break; case 360: _LL6792: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6805=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6805[ 0]=({ struct Cyc_Exp_tok_struct
_temp6806; _temp6806.tag= Cyc_Exp_tok_tag; _temp6806.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6807= yyvs; struct _xtunion_struct**
_temp6809= _temp6807.curr +( yyvsp_offset - 3); if( _temp6807.base == 0? 1:(
_temp6809 < _temp6807.base? 1: _temp6809 >= _temp6807.last_plus_one)){ _throw(
Null_Exception);}* _temp6809;})), Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2
_temp6810= yyvs; struct _xtunion_struct** _temp6812= _temp6810.curr +(
yyvsp_offset - 1); if( _temp6810.base == 0? 1:( _temp6812 < _temp6810.base? 1:
_temp6812 >= _temp6810.last_plus_one)){ _throw( Null_Exception);}* _temp6812;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6813= yyls; struct Cyc_Yyltype*
_temp6815= _temp6813.curr +( yylsp_offset - 3); if( _temp6813.base == 0? 1:(
_temp6815 < _temp6813.base? 1: _temp6815 >= _temp6813.last_plus_one)){ _throw(
Null_Exception);}* _temp6815;}).first_line,({ struct _tagged_ptr3 _temp6816=
yyls; struct Cyc_Yyltype* _temp6818= _temp6816.curr + yylsp_offset; if(
_temp6816.base == 0? 1:( _temp6818 < _temp6816.base? 1: _temp6818 >= _temp6816.last_plus_one)){
_throw( Null_Exception);}* _temp6818;}).last_line)); _temp6806;}); _temp6805;});
break; case 361: _LL6804: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6820=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6820[ 0]=({ struct Cyc_Exp_tok_struct _temp6821; _temp6821.tag= Cyc_Exp_tok_tag;
_temp6821.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6822= yyvs; struct _xtunion_struct** _temp6824= _temp6822.curr
+( yyvsp_offset - 2); if( _temp6822.base == 0? 1:( _temp6824 < _temp6822.base? 1:
_temp6824 >= _temp6822.last_plus_one)){ _throw( Null_Exception);}* _temp6824;})),({
struct _tagged_string* _temp6825=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp6825[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp6826= yyvs; struct _xtunion_struct** _temp6828= _temp6826.curr
+ yyvsp_offset; if( _temp6826.base == 0? 1:( _temp6828 < _temp6826.base? 1:
_temp6828 >= _temp6826.last_plus_one)){ _throw( Null_Exception);}* _temp6828;}));
_temp6825;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6829= yyls;
struct Cyc_Yyltype* _temp6831= _temp6829.curr +( yylsp_offset - 2); if(
_temp6829.base == 0? 1:( _temp6831 < _temp6829.base? 1: _temp6831 >= _temp6829.last_plus_one)){
_throw( Null_Exception);}* _temp6831;}).first_line,({ struct _tagged_ptr3
_temp6832= yyls; struct Cyc_Yyltype* _temp6834= _temp6832.curr + yylsp_offset;
if( _temp6832.base == 0? 1:( _temp6834 < _temp6832.base? 1: _temp6834 >=
_temp6832.last_plus_one)){ _throw( Null_Exception);}* _temp6834;}).last_line));
_temp6821;}); _temp6820;}); break; case 362: _LL6819: { struct _tuple1* q= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp6836= yyvs; struct _xtunion_struct** _temp6838=
_temp6836.curr + yyvsp_offset; if( _temp6836.base == 0? 1:( _temp6838 <
_temp6836.base? 1: _temp6838 >= _temp6836.last_plus_one)){ _throw(
Null_Exception);}* _temp6838;}));{ void* _temp6839=(* q).f1; struct Cyc_List_List*
_temp6849; struct Cyc_List_List* _temp6851; _LL6841: if(( int) _temp6839 == Cyc_Absyn_Loc_n){
goto _LL6842;} else{ goto _LL6843;} _LL6843: if(( unsigned int) _temp6839 > 1u?((
struct _tunion_struct*) _temp6839)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL6850:
_temp6849=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp6839)->f1;
if( _temp6849 == 0){ goto _LL6844;} else{ goto _LL6845;}} else{ goto _LL6845;}
_LL6845: if(( unsigned int) _temp6839 > 1u?(( struct _tunion_struct*) _temp6839)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6852: _temp6851=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6839)->f1; if( _temp6851 == 0){ goto _LL6846;}
else{ goto _LL6847;}} else{ goto _LL6847;} _LL6847: goto _LL6848; _LL6842: goto
_LL6840; _LL6844: goto _LL6840; _LL6846: goto _LL6840; _LL6848: Cyc_Parse_err((
struct _tagged_string)({ char* _temp6853=( char*)"struct field name is qualified";
struct _tagged_string _temp6854; _temp6854.curr= _temp6853; _temp6854.base=
_temp6853; _temp6854.last_plus_one= _temp6853 + 31; _temp6854;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6855= yyls; struct Cyc_Yyltype* _temp6857= _temp6855.curr
+ yylsp_offset; if( _temp6855.base == 0? 1:( _temp6857 < _temp6855.base? 1:
_temp6857 >= _temp6855.last_plus_one)){ _throw( Null_Exception);}* _temp6857;}).first_line,({
struct _tagged_ptr3 _temp6858= yyls; struct Cyc_Yyltype* _temp6860= _temp6858.curr
+ yylsp_offset; if( _temp6858.base == 0? 1:( _temp6860 < _temp6858.base? 1:
_temp6860 >= _temp6858.last_plus_one)){ _throw( Null_Exception);}* _temp6860;}).last_line));
goto _LL6840; _LL6840:;} yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6861=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6861[ 0]=({ struct Cyc_Exp_tok_struct _temp6862; _temp6862.tag= Cyc_Exp_tok_tag;
_temp6862.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6863= yyvs; struct _xtunion_struct** _temp6865= _temp6863.curr
+( yyvsp_offset - 2); if( _temp6863.base == 0? 1:( _temp6865 < _temp6863.base? 1:
_temp6865 >= _temp6863.last_plus_one)){ _throw( Null_Exception);}* _temp6865;})),(*
q).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6866= yyls;
struct Cyc_Yyltype* _temp6868= _temp6866.curr +( yylsp_offset - 2); if(
_temp6866.base == 0? 1:( _temp6868 < _temp6866.base? 1: _temp6868 >= _temp6866.last_plus_one)){
_throw( Null_Exception);}* _temp6868;}).first_line,({ struct _tagged_ptr3
_temp6869= yyls; struct Cyc_Yyltype* _temp6871= _temp6869.curr + yylsp_offset;
if( _temp6869.base == 0? 1:( _temp6871 < _temp6869.base? 1: _temp6871 >=
_temp6869.last_plus_one)){ _throw( Null_Exception);}* _temp6871;}).last_line));
_temp6862;}); _temp6861;}); break;} case 363: _LL6835: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6873=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6873[ 0]=({ struct Cyc_Exp_tok_struct
_temp6874; _temp6874.tag= Cyc_Exp_tok_tag; _temp6874.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6875= yyvs; struct _xtunion_struct**
_temp6877= _temp6875.curr +( yyvsp_offset - 2); if( _temp6875.base == 0? 1:(
_temp6877 < _temp6875.base? 1: _temp6877 >= _temp6875.last_plus_one)){ _throw(
Null_Exception);}* _temp6877;})),({ struct _tagged_string* _temp6878=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp6878[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6879= yyvs; struct
_xtunion_struct** _temp6881= _temp6879.curr + yyvsp_offset; if( _temp6879.base
== 0? 1:( _temp6881 < _temp6879.base? 1: _temp6881 >= _temp6879.last_plus_one)){
_throw( Null_Exception);}* _temp6881;})); _temp6878;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6882= yyls; struct Cyc_Yyltype* _temp6884= _temp6882.curr
+( yylsp_offset - 2); if( _temp6882.base == 0? 1:( _temp6884 < _temp6882.base? 1:
_temp6884 >= _temp6882.last_plus_one)){ _throw( Null_Exception);}* _temp6884;}).first_line,({
struct _tagged_ptr3 _temp6885= yyls; struct Cyc_Yyltype* _temp6887= _temp6885.curr
+ yylsp_offset; if( _temp6885.base == 0? 1:( _temp6887 < _temp6885.base? 1:
_temp6887 >= _temp6885.last_plus_one)){ _throw( Null_Exception);}* _temp6887;}).last_line));
_temp6874;}); _temp6873;}); break; case 364: _LL6872: { struct _tuple1* q= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp6889= yyvs; struct _xtunion_struct** _temp6891=
_temp6889.curr + yyvsp_offset; if( _temp6889.base == 0? 1:( _temp6891 <
_temp6889.base? 1: _temp6891 >= _temp6889.last_plus_one)){ _throw(
Null_Exception);}* _temp6891;}));{ void* _temp6892=(* q).f1; struct Cyc_List_List*
_temp6902; struct Cyc_List_List* _temp6904; _LL6894: if(( int) _temp6892 == Cyc_Absyn_Loc_n){
goto _LL6895;} else{ goto _LL6896;} _LL6896: if(( unsigned int) _temp6892 > 1u?((
struct _tunion_struct*) _temp6892)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL6903:
_temp6902=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp6892)->f1;
if( _temp6902 == 0){ goto _LL6897;} else{ goto _LL6898;}} else{ goto _LL6898;}
_LL6898: if(( unsigned int) _temp6892 > 1u?(( struct _tunion_struct*) _temp6892)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6905: _temp6904=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6892)->f1; if( _temp6904 == 0){ goto _LL6899;}
else{ goto _LL6900;}} else{ goto _LL6900;} _LL6900: goto _LL6901; _LL6895: goto
_LL6893; _LL6897: goto _LL6893; _LL6899: goto _LL6893; _LL6901: Cyc_Parse_err((
struct _tagged_string)({ char* _temp6906=( char*)"struct field is qualified with module name";
struct _tagged_string _temp6907; _temp6907.curr= _temp6906; _temp6907.base=
_temp6906; _temp6907.last_plus_one= _temp6906 + 43; _temp6907;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6908= yyls; struct Cyc_Yyltype* _temp6910= _temp6908.curr
+ yylsp_offset; if( _temp6908.base == 0? 1:( _temp6910 < _temp6908.base? 1:
_temp6910 >= _temp6908.last_plus_one)){ _throw( Null_Exception);}* _temp6910;}).first_line,({
struct _tagged_ptr3 _temp6911= yyls; struct Cyc_Yyltype* _temp6913= _temp6911.curr
+ yylsp_offset; if( _temp6911.base == 0? 1:( _temp6913 < _temp6911.base? 1:
_temp6913 >= _temp6911.last_plus_one)){ _throw( Null_Exception);}* _temp6913;}).last_line));
goto _LL6893; _LL6893:;} yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6914=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6914[ 0]=({ struct Cyc_Exp_tok_struct _temp6915; _temp6915.tag= Cyc_Exp_tok_tag;
_temp6915.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6916= yyvs; struct _xtunion_struct** _temp6918= _temp6916.curr
+( yyvsp_offset - 2); if( _temp6916.base == 0? 1:( _temp6918 < _temp6916.base? 1:
_temp6918 >= _temp6916.last_plus_one)){ _throw( Null_Exception);}* _temp6918;})),(*
q).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6919= yyls;
struct Cyc_Yyltype* _temp6921= _temp6919.curr +( yylsp_offset - 2); if(
_temp6919.base == 0? 1:( _temp6921 < _temp6919.base? 1: _temp6921 >= _temp6919.last_plus_one)){
_throw( Null_Exception);}* _temp6921;}).first_line,({ struct _tagged_ptr3
_temp6922= yyls; struct Cyc_Yyltype* _temp6924= _temp6922.curr + yylsp_offset;
if( _temp6922.base == 0? 1:( _temp6924 < _temp6922.base? 1: _temp6924 >=
_temp6922.last_plus_one)){ _throw( Null_Exception);}* _temp6924;}).last_line));
_temp6915;}); _temp6914;}); break;} case 365: _LL6888: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6926=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6926[ 0]=({ struct Cyc_Exp_tok_struct
_temp6927; _temp6927.tag= Cyc_Exp_tok_tag; _temp6927.f1= Cyc_Absyn_post_inc_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6928= yyvs; struct _xtunion_struct**
_temp6930= _temp6928.curr +( yyvsp_offset - 1); if( _temp6928.base == 0? 1:(
_temp6930 < _temp6928.base? 1: _temp6930 >= _temp6928.last_plus_one)){ _throw(
Null_Exception);}* _temp6930;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6931= yyls; struct Cyc_Yyltype* _temp6933= _temp6931.curr +(
yylsp_offset - 1); if( _temp6931.base == 0? 1:( _temp6933 < _temp6931.base? 1:
_temp6933 >= _temp6931.last_plus_one)){ _throw( Null_Exception);}* _temp6933;}).first_line,({
struct _tagged_ptr3 _temp6934= yyls; struct Cyc_Yyltype* _temp6936= _temp6934.curr
+ yylsp_offset; if( _temp6934.base == 0? 1:( _temp6936 < _temp6934.base? 1:
_temp6936 >= _temp6934.last_plus_one)){ _throw( Null_Exception);}* _temp6936;}).last_line));
_temp6927;}); _temp6926;}); break; case 366: _LL6925: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6938=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6938[ 0]=({ struct Cyc_Exp_tok_struct
_temp6939; _temp6939.tag= Cyc_Exp_tok_tag; _temp6939.f1= Cyc_Absyn_post_dec_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6940= yyvs; struct _xtunion_struct**
_temp6942= _temp6940.curr +( yyvsp_offset - 1); if( _temp6940.base == 0? 1:(
_temp6942 < _temp6940.base? 1: _temp6942 >= _temp6940.last_plus_one)){ _throw(
Null_Exception);}* _temp6942;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6943= yyls; struct Cyc_Yyltype* _temp6945= _temp6943.curr +(
yylsp_offset - 1); if( _temp6943.base == 0? 1:( _temp6945 < _temp6943.base? 1:
_temp6945 >= _temp6943.last_plus_one)){ _throw( Null_Exception);}* _temp6945;}).first_line,({
struct _tagged_ptr3 _temp6946= yyls; struct Cyc_Yyltype* _temp6948= _temp6946.curr
+ yylsp_offset; if( _temp6946.base == 0? 1:( _temp6948 < _temp6946.base? 1:
_temp6948 >= _temp6946.last_plus_one)){ _throw( Null_Exception);}* _temp6948;}).last_line));
_temp6939;}); _temp6938;}); break; case 367: _LL6937: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6950=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6950[ 0]=({ struct Cyc_Exp_tok_struct
_temp6951; _temp6951.tag= Cyc_Exp_tok_tag; _temp6951.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_CompoundLit_e_struct* _temp6952=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp6952[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp6953; _temp6953.tag= Cyc_Absyn_CompoundLit_e_tag;
_temp6953.f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6954= yyvs;
struct _xtunion_struct** _temp6956= _temp6954.curr +( yyvsp_offset - 4); if(
_temp6954.base == 0? 1:( _temp6956 < _temp6954.base? 1: _temp6956 >= _temp6954.last_plus_one)){
_throw( Null_Exception);}* _temp6956;})); _temp6953.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6957= yyvs; struct _xtunion_struct** _temp6959=
_temp6957.curr +( yyvsp_offset - 1); if( _temp6957.base == 0? 1:( _temp6959 <
_temp6957.base? 1: _temp6959 >= _temp6957.last_plus_one)){ _throw(
Null_Exception);}* _temp6959;}))); _temp6953;}); _temp6952;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6960= yyls; struct Cyc_Yyltype* _temp6962= _temp6960.curr
+( yylsp_offset - 5); if( _temp6960.base == 0? 1:( _temp6962 < _temp6960.base? 1:
_temp6962 >= _temp6960.last_plus_one)){ _throw( Null_Exception);}* _temp6962;}).first_line,({
struct _tagged_ptr3 _temp6963= yyls; struct Cyc_Yyltype* _temp6965= _temp6963.curr
+ yylsp_offset; if( _temp6963.base == 0? 1:( _temp6965 < _temp6963.base? 1:
_temp6965 >= _temp6963.last_plus_one)){ _throw( Null_Exception);}* _temp6965;}).last_line));
_temp6951;}); _temp6950;}); break; case 368: _LL6949: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6967=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6967[ 0]=({ struct Cyc_Exp_tok_struct
_temp6968; _temp6968.tag= Cyc_Exp_tok_tag; _temp6968.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_CompoundLit_e_struct* _temp6969=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp6969[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp6970; _temp6970.tag= Cyc_Absyn_CompoundLit_e_tag;
_temp6970.f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6971= yyvs;
struct _xtunion_struct** _temp6973= _temp6971.curr +( yyvsp_offset - 5); if(
_temp6971.base == 0? 1:( _temp6973 < _temp6971.base? 1: _temp6973 >= _temp6971.last_plus_one)){
_throw( Null_Exception);}* _temp6973;})); _temp6970.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6974= yyvs; struct _xtunion_struct** _temp6976=
_temp6974.curr +( yyvsp_offset - 2); if( _temp6974.base == 0? 1:( _temp6976 <
_temp6974.base? 1: _temp6976 >= _temp6974.last_plus_one)){ _throw(
Null_Exception);}* _temp6976;}))); _temp6970;}); _temp6969;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6977= yyls; struct Cyc_Yyltype* _temp6979= _temp6977.curr
+( yylsp_offset - 6); if( _temp6977.base == 0? 1:( _temp6979 < _temp6977.base? 1:
_temp6979 >= _temp6977.last_plus_one)){ _throw( Null_Exception);}* _temp6979;}).first_line,({
struct _tagged_ptr3 _temp6980= yyls; struct Cyc_Yyltype* _temp6982= _temp6980.curr
+ yylsp_offset; if( _temp6980.base == 0? 1:( _temp6982 < _temp6980.base? 1:
_temp6982 >= _temp6980.last_plus_one)){ _throw( Null_Exception);}* _temp6982;}).last_line));
_temp6968;}); _temp6967;}); break; case 369: _LL6966: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6984=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6984[ 0]=({ struct Cyc_Exp_tok_struct
_temp6985; _temp6985.tag= Cyc_Exp_tok_tag; _temp6985.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Fill_e_struct* _temp6986=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp6986[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp6987; _temp6987.tag= Cyc_Absyn_Fill_e_tag; _temp6987.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6988= yyvs; struct _xtunion_struct** _temp6990=
_temp6988.curr +( yyvsp_offset - 1); if( _temp6988.base == 0? 1:( _temp6990 <
_temp6988.base? 1: _temp6990 >= _temp6988.last_plus_one)){ _throw(
Null_Exception);}* _temp6990;})); _temp6987;}); _temp6986;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6991= yyls; struct Cyc_Yyltype* _temp6993= _temp6991.curr
+( yylsp_offset - 3); if( _temp6991.base == 0? 1:( _temp6993 < _temp6991.base? 1:
_temp6993 >= _temp6991.last_plus_one)){ _throw( Null_Exception);}* _temp6993;}).first_line,({
struct _tagged_ptr3 _temp6994= yyls; struct Cyc_Yyltype* _temp6996= _temp6994.curr
+ yylsp_offset; if( _temp6994.base == 0? 1:( _temp6996 < _temp6994.base? 1:
_temp6996 >= _temp6994.last_plus_one)){ _throw( Null_Exception);}* _temp6996;}).last_line));
_temp6985;}); _temp6984;}); break; case 370: _LL6983: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6998=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6998[ 0]=({ struct Cyc_Exp_tok_struct
_temp6999; _temp6999.tag= Cyc_Exp_tok_tag; _temp6999.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Codegen_e_struct* _temp7000=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp7000[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp7001; _temp7001.tag= Cyc_Absyn_Codegen_e_tag;
_temp7001.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp7002= yyvs;
struct _xtunion_struct** _temp7004= _temp7002.curr +( yyvsp_offset - 1); if(
_temp7002.base == 0? 1:( _temp7004 < _temp7002.base? 1: _temp7004 >= _temp7002.last_plus_one)){
_throw( Null_Exception);}* _temp7004;})); _temp7001;}); _temp7000;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7005= yyls; struct Cyc_Yyltype* _temp7007= _temp7005.curr
+( yylsp_offset - 3); if( _temp7005.base == 0? 1:( _temp7007 < _temp7005.base? 1:
_temp7007 >= _temp7005.last_plus_one)){ _throw( Null_Exception);}* _temp7007;}).first_line,({
struct _tagged_ptr3 _temp7008= yyls; struct Cyc_Yyltype* _temp7010= _temp7008.curr
+ yylsp_offset; if( _temp7008.base == 0? 1:( _temp7010 < _temp7008.base? 1:
_temp7010 >= _temp7008.last_plus_one)){ _throw( Null_Exception);}* _temp7010;}).last_line));
_temp6999;}); _temp6998;}); break; case 371: _LL6997: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7012=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7012[ 0]=({ struct Cyc_Exp_tok_struct
_temp7013; _temp7013.tag= Cyc_Exp_tok_tag; _temp7013.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp7014=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp7014[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp7015; _temp7015.tag= Cyc_Absyn_UnknownId_e_tag;
_temp7015.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp7016= yyvs;
struct _xtunion_struct** _temp7018= _temp7016.curr + yyvsp_offset; if( _temp7016.base
== 0? 1:( _temp7018 < _temp7016.base? 1: _temp7018 >= _temp7016.last_plus_one)){
_throw( Null_Exception);}* _temp7018;})); _temp7015;}); _temp7014;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7019= yyls; struct Cyc_Yyltype* _temp7021= _temp7019.curr
+ yylsp_offset; if( _temp7019.base == 0? 1:( _temp7021 < _temp7019.base? 1:
_temp7021 >= _temp7019.last_plus_one)){ _throw( Null_Exception);}* _temp7021;}).first_line,({
struct _tagged_ptr3 _temp7022= yyls; struct Cyc_Yyltype* _temp7024= _temp7022.curr
+ yylsp_offset; if( _temp7022.base == 0? 1:( _temp7024 < _temp7022.base? 1:
_temp7024 >= _temp7022.last_plus_one)){ _throw( Null_Exception);}* _temp7024;}).last_line));
_temp7013;}); _temp7012;}); break; case 372: _LL7011: yyval=({ struct
_tagged_ptr2 _temp7026= yyvs; struct _xtunion_struct** _temp7028= _temp7026.curr
+ yyvsp_offset; if( _temp7026.base == 0? 1:( _temp7028 < _temp7026.base? 1:
_temp7028 >= _temp7026.last_plus_one)){ _throw( Null_Exception);}* _temp7028;});
break; case 373: _LL7025: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7030=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7030[ 0]=({ struct Cyc_Exp_tok_struct _temp7031; _temp7031.tag= Cyc_Exp_tok_tag;
_temp7031.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp7032= yyvs; struct _xtunion_struct** _temp7034= _temp7032.curr +
yyvsp_offset; if( _temp7032.base == 0? 1:( _temp7034 < _temp7032.base? 1:
_temp7034 >= _temp7032.last_plus_one)){ _throw( Null_Exception);}* _temp7034;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7035= yyls; struct Cyc_Yyltype*
_temp7037= _temp7035.curr + yylsp_offset; if( _temp7035.base == 0? 1:( _temp7037
< _temp7035.base? 1: _temp7037 >= _temp7035.last_plus_one)){ _throw(
Null_Exception);}* _temp7037;}).first_line,({ struct _tagged_ptr3 _temp7038=
yyls; struct Cyc_Yyltype* _temp7040= _temp7038.curr + yylsp_offset; if(
_temp7038.base == 0? 1:( _temp7040 < _temp7038.base? 1: _temp7040 >= _temp7038.last_plus_one)){
_throw( Null_Exception);}* _temp7040;}).last_line)); _temp7031;}); _temp7030;});
break; case 374: _LL7029: yyval=({ struct _tagged_ptr2 _temp7042= yyvs; struct
_xtunion_struct** _temp7044= _temp7042.curr +( yyvsp_offset - 1); if( _temp7042.base
== 0? 1:( _temp7044 < _temp7042.base? 1: _temp7044 >= _temp7042.last_plus_one)){
_throw( Null_Exception);}* _temp7044;}); break; case 375: _LL7041: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7046=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7046[ 0]=({ struct Cyc_Exp_tok_struct
_temp7047; _temp7047.tag= Cyc_Exp_tok_tag; _temp7047.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp7048=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7048[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7049; _temp7049.tag=
Cyc_Absyn_UnknownId_e_tag; _temp7049.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp7050= yyvs; struct _xtunion_struct** _temp7052= _temp7050.curr
+( yyvsp_offset - 1); if( _temp7050.base == 0? 1:( _temp7052 < _temp7050.base? 1:
_temp7052 >= _temp7050.last_plus_one)){ _throw( Null_Exception);}* _temp7052;}));
_temp7049;}); _temp7048;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7053= yyls; struct Cyc_Yyltype* _temp7055= _temp7053.curr +( yylsp_offset -
1); if( _temp7053.base == 0? 1:( _temp7055 < _temp7053.base? 1: _temp7055 >=
_temp7053.last_plus_one)){ _throw( Null_Exception);}* _temp7055;}).first_line,({
struct _tagged_ptr3 _temp7056= yyls; struct Cyc_Yyltype* _temp7058= _temp7056.curr
+( yylsp_offset - 1); if( _temp7056.base == 0? 1:( _temp7058 < _temp7056.base? 1:
_temp7058 >= _temp7056.last_plus_one)){ _throw( Null_Exception);}* _temp7058;}).last_line)),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7059= yyls; struct Cyc_Yyltype*
_temp7061= _temp7059.curr +( yylsp_offset - 1); if( _temp7059.base == 0? 1:(
_temp7061 < _temp7059.base? 1: _temp7061 >= _temp7059.last_plus_one)){ _throw(
Null_Exception);}* _temp7061;}).first_line,({ struct _tagged_ptr3 _temp7062=
yyls; struct Cyc_Yyltype* _temp7064= _temp7062.curr + yylsp_offset; if(
_temp7062.base == 0? 1:( _temp7064 < _temp7062.base? 1: _temp7064 >= _temp7062.last_plus_one)){
_throw( Null_Exception);}* _temp7064;}).last_line)); _temp7047;}); _temp7046;});
break; case 376: _LL7045: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7066=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7066[ 0]=({ struct Cyc_Exp_tok_struct _temp7067; _temp7067.tag= Cyc_Exp_tok_tag;
_temp7067.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp7068=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7068[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7069; _temp7069.tag=
Cyc_Absyn_UnknownId_e_tag; _temp7069.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp7070= yyvs; struct _xtunion_struct** _temp7072= _temp7070.curr
+( yyvsp_offset - 4); if( _temp7070.base == 0? 1:( _temp7072 < _temp7070.base? 1:
_temp7072 >= _temp7070.last_plus_one)){ _throw( Null_Exception);}* _temp7072;}));
_temp7069;}); _temp7068;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7073= yyls; struct Cyc_Yyltype* _temp7075= _temp7073.curr +( yylsp_offset -
4); if( _temp7073.base == 0? 1:( _temp7075 < _temp7073.base? 1: _temp7075 >=
_temp7073.last_plus_one)){ _throw( Null_Exception);}* _temp7075;}).first_line,({
struct _tagged_ptr3 _temp7076= yyls; struct Cyc_Yyltype* _temp7078= _temp7076.curr
+( yylsp_offset - 4); if( _temp7076.base == 0? 1:( _temp7078 < _temp7076.base? 1:
_temp7078 >= _temp7076.last_plus_one)){ _throw( Null_Exception);}* _temp7078;}).last_line)),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp7079= yyvs; struct _xtunion_struct** _temp7081=
_temp7079.curr +( yyvsp_offset - 1); if( _temp7079.base == 0? 1:( _temp7081 <
_temp7079.base? 1: _temp7081 >= _temp7079.last_plus_one)){ _throw(
Null_Exception);}* _temp7081;}))), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7082= yyls; struct Cyc_Yyltype* _temp7084= _temp7082.curr +(
yylsp_offset - 4); if( _temp7082.base == 0? 1:( _temp7084 < _temp7082.base? 1:
_temp7084 >= _temp7082.last_plus_one)){ _throw( Null_Exception);}* _temp7084;}).first_line,({
struct _tagged_ptr3 _temp7085= yyls; struct Cyc_Yyltype* _temp7087= _temp7085.curr
+ yylsp_offset; if( _temp7085.base == 0? 1:( _temp7087 < _temp7085.base? 1:
_temp7087 >= _temp7085.last_plus_one)){ _throw( Null_Exception);}* _temp7087;}).last_line));
_temp7067;}); _temp7066;}); break; case 377: _LL7065: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7089=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7089[ 0]=({ struct Cyc_Exp_tok_struct
_temp7090; _temp7090.tag= Cyc_Exp_tok_tag; _temp7090.f1= Cyc_Absyn_tuple_exp(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp7091= yyvs; struct
_xtunion_struct** _temp7093= _temp7091.curr +( yyvsp_offset - 1); if( _temp7091.base
== 0? 1:( _temp7093 < _temp7091.base? 1: _temp7093 >= _temp7091.last_plus_one)){
_throw( Null_Exception);}* _temp7093;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7094= yyls; struct Cyc_Yyltype* _temp7096= _temp7094.curr +(
yylsp_offset - 3); if( _temp7094.base == 0? 1:( _temp7096 < _temp7094.base? 1:
_temp7096 >= _temp7094.last_plus_one)){ _throw( Null_Exception);}* _temp7096;}).first_line,({
struct _tagged_ptr3 _temp7097= yyls; struct Cyc_Yyltype* _temp7099= _temp7097.curr
+ yylsp_offset; if( _temp7097.base == 0? 1:( _temp7099 < _temp7097.base? 1:
_temp7099 >= _temp7097.last_plus_one)){ _throw( Null_Exception);}* _temp7099;}).last_line));
_temp7090;}); _temp7089;}); break; case 378: _LL7088: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7101=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7101[ 0]=({ struct Cyc_Exp_tok_struct
_temp7102; _temp7102.tag= Cyc_Exp_tok_tag; _temp7102.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Struct_e_struct* _temp7103=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp7103[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp7104; _temp7104.tag= Cyc_Absyn_Struct_e_tag;
_temp7104.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp7105= yyvs;
struct _xtunion_struct** _temp7107= _temp7105.curr +( yyvsp_offset - 3); if(
_temp7105.base == 0? 1:( _temp7107 < _temp7105.base? 1: _temp7107 >= _temp7105.last_plus_one)){
_throw( Null_Exception);}* _temp7107;})); _temp7104.f2= 0; _temp7104.f3=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp7108= yyvs; struct _xtunion_struct** _temp7110=
_temp7108.curr +( yyvsp_offset - 1); if( _temp7108.base == 0? 1:( _temp7110 <
_temp7108.base? 1: _temp7110 >= _temp7108.last_plus_one)){ _throw(
Null_Exception);}* _temp7110;}))); _temp7104.f4= 0; _temp7104;}); _temp7103;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7111= yyls; struct Cyc_Yyltype*
_temp7113= _temp7111.curr +( yylsp_offset - 3); if( _temp7111.base == 0? 1:(
_temp7113 < _temp7111.base? 1: _temp7113 >= _temp7111.last_plus_one)){ _throw(
Null_Exception);}* _temp7113;}).first_line,({ struct _tagged_ptr3 _temp7114=
yyls; struct Cyc_Yyltype* _temp7116= _temp7114.curr + yylsp_offset; if(
_temp7114.base == 0? 1:( _temp7116 < _temp7114.base? 1: _temp7116 >= _temp7114.last_plus_one)){
_throw( Null_Exception);}* _temp7116;}).last_line)); _temp7102;}); _temp7101;});
break; case 379: _LL7100: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7118=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7118[ 0]=({ struct Cyc_Exp_tok_struct _temp7119; _temp7119.tag= Cyc_Exp_tok_tag;
_temp7119.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp7120= yyvs; struct _xtunion_struct** _temp7122= _temp7120.curr +(
yyvsp_offset - 2); if( _temp7120.base == 0? 1:( _temp7122 < _temp7120.base? 1:
_temp7122 >= _temp7120.last_plus_one)){ _throw( Null_Exception);}* _temp7122;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7123= yyls; struct Cyc_Yyltype*
_temp7125= _temp7123.curr +( yylsp_offset - 4); if( _temp7123.base == 0? 1:(
_temp7125 < _temp7123.base? 1: _temp7125 >= _temp7123.last_plus_one)){ _throw(
Null_Exception);}* _temp7125;}).first_line,({ struct _tagged_ptr3 _temp7126=
yyls; struct Cyc_Yyltype* _temp7128= _temp7126.curr + yylsp_offset; if(
_temp7126.base == 0? 1:( _temp7128 < _temp7126.base? 1: _temp7128 >= _temp7126.last_plus_one)){
_throw( Null_Exception);}* _temp7128;}).last_line)); _temp7119;}); _temp7118;});
break; case 380: _LL7117: yyval=( struct _xtunion_struct*)({ struct Cyc_ExpList_tok_struct*
_temp7130=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp7130[ 0]=({ struct Cyc_ExpList_tok_struct _temp7131; _temp7131.tag= Cyc_ExpList_tok_tag;
_temp7131.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp7132= yyvs; struct
_xtunion_struct** _temp7134= _temp7132.curr + yyvsp_offset; if( _temp7132.base
== 0? 1:( _temp7134 < _temp7132.base? 1: _temp7134 >= _temp7132.last_plus_one)){
_throw( Null_Exception);}* _temp7134;}))); _temp7131;}); _temp7130;}); break;
case 381: _LL7129: yyval=( struct _xtunion_struct*)({ struct Cyc_ExpList_tok_struct*
_temp7136=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp7136[ 0]=({ struct Cyc_ExpList_tok_struct _temp7137; _temp7137.tag= Cyc_ExpList_tok_tag;
_temp7137.f1=({ struct Cyc_List_List* _temp7138=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp7138->hd=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp7139= yyvs; struct _xtunion_struct** _temp7141=
_temp7139.curr + yyvsp_offset; if( _temp7139.base == 0? 1:( _temp7141 <
_temp7139.base? 1: _temp7141 >= _temp7139.last_plus_one)){ _throw(
Null_Exception);}* _temp7141;})); _temp7138->tl= 0; _temp7138;}); _temp7137;});
_temp7136;}); break; case 382: _LL7135: yyval=( struct _xtunion_struct*)({
struct Cyc_ExpList_tok_struct* _temp7143=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp7143[ 0]=({ struct Cyc_ExpList_tok_struct
_temp7144; _temp7144.tag= Cyc_ExpList_tok_tag; _temp7144.f1=({ struct Cyc_List_List*
_temp7145=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp7145->hd=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7149= yyvs;
struct _xtunion_struct** _temp7151= _temp7149.curr + yyvsp_offset; if( _temp7149.base
== 0? 1:( _temp7151 < _temp7149.base? 1: _temp7151 >= _temp7149.last_plus_one)){
_throw( Null_Exception);}* _temp7151;})); _temp7145->tl= Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp7146= yyvs; struct _xtunion_struct** _temp7148=
_temp7146.curr +( yyvsp_offset - 2); if( _temp7146.base == 0? 1:( _temp7148 <
_temp7146.base? 1: _temp7148 >= _temp7146.last_plus_one)){ _throw(
Null_Exception);}* _temp7148;})); _temp7145;}); _temp7144;}); _temp7143;});
break; case 383: _LL7142: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7153=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7153[ 0]=({ struct Cyc_Exp_tok_struct _temp7154; _temp7154.tag= Cyc_Exp_tok_tag;
_temp7154.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(({ struct _tagged_ptr2
_temp7155= yyvs; struct _xtunion_struct** _temp7157= _temp7155.curr +
yyvsp_offset; if( _temp7155.base == 0? 1:( _temp7157 < _temp7155.base? 1:
_temp7157 >= _temp7155.last_plus_one)){ _throw( Null_Exception);}* _temp7157;}))).f1,(*
Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp7158= yyvs; struct _xtunion_struct**
_temp7160= _temp7158.curr + yyvsp_offset; if( _temp7158.base == 0? 1:( _temp7160
< _temp7158.base? 1: _temp7160 >= _temp7158.last_plus_one)){ _throw(
Null_Exception);}* _temp7160;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7161= yyls; struct Cyc_Yyltype* _temp7163= _temp7161.curr +
yylsp_offset; if( _temp7161.base == 0? 1:( _temp7163 < _temp7161.base? 1:
_temp7163 >= _temp7161.last_plus_one)){ _throw( Null_Exception);}* _temp7163;}).first_line,({
struct _tagged_ptr3 _temp7164= yyls; struct Cyc_Yyltype* _temp7166= _temp7164.curr
+ yylsp_offset; if( _temp7164.base == 0? 1:( _temp7166 < _temp7164.base? 1:
_temp7166 >= _temp7164.last_plus_one)){ _throw( Null_Exception);}* _temp7166;}).last_line));
_temp7154;}); _temp7153;}); break; case 384: _LL7152: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7168=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7168[ 0]=({ struct Cyc_Exp_tok_struct
_temp7169; _temp7169.tag= Cyc_Exp_tok_tag; _temp7169.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok(({
struct _tagged_ptr2 _temp7170= yyvs; struct _xtunion_struct** _temp7172=
_temp7170.curr + yyvsp_offset; if( _temp7170.base == 0? 1:( _temp7172 <
_temp7170.base? 1: _temp7172 >= _temp7170.last_plus_one)){ _throw(
Null_Exception);}* _temp7172;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7173= yyls; struct Cyc_Yyltype* _temp7175= _temp7173.curr +
yylsp_offset; if( _temp7173.base == 0? 1:( _temp7175 < _temp7173.base? 1:
_temp7175 >= _temp7173.last_plus_one)){ _throw( Null_Exception);}* _temp7175;}).first_line,({
struct _tagged_ptr3 _temp7176= yyls; struct Cyc_Yyltype* _temp7178= _temp7176.curr
+ yylsp_offset; if( _temp7176.base == 0? 1:( _temp7178 < _temp7176.base? 1:
_temp7178 >= _temp7176.last_plus_one)){ _throw( Null_Exception);}* _temp7178;}).last_line));
_temp7169;}); _temp7168;}); break; case 385: _LL7167: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7180=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7180[ 0]=({ struct Cyc_Exp_tok_struct
_temp7181; _temp7181.tag= Cyc_Exp_tok_tag; _temp7181.f1= Cyc_Absyn_float_exp(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp7182= yyvs; struct
_xtunion_struct** _temp7184= _temp7182.curr + yyvsp_offset; if( _temp7182.base
== 0? 1:( _temp7184 < _temp7182.base? 1: _temp7184 >= _temp7182.last_plus_one)){
_throw( Null_Exception);}* _temp7184;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7185= yyls; struct Cyc_Yyltype* _temp7187= _temp7185.curr +
yylsp_offset; if( _temp7185.base == 0? 1:( _temp7187 < _temp7185.base? 1:
_temp7187 >= _temp7185.last_plus_one)){ _throw( Null_Exception);}* _temp7187;}).first_line,({
struct _tagged_ptr3 _temp7188= yyls; struct Cyc_Yyltype* _temp7190= _temp7188.curr
+ yylsp_offset; if( _temp7188.base == 0? 1:( _temp7190 < _temp7188.base? 1:
_temp7190 >= _temp7188.last_plus_one)){ _throw( Null_Exception);}* _temp7190;}).last_line));
_temp7181;}); _temp7180;}); break; case 386: _LL7179: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7192=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7192[ 0]=({ struct Cyc_Exp_tok_struct
_temp7193; _temp7193.tag= Cyc_Exp_tok_tag; _temp7193.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7194= yyls; struct Cyc_Yyltype* _temp7196= _temp7194.curr
+ yylsp_offset; if( _temp7194.base == 0? 1:( _temp7196 < _temp7194.base? 1:
_temp7196 >= _temp7194.last_plus_one)){ _throw( Null_Exception);}* _temp7196;}).first_line,({
struct _tagged_ptr3 _temp7197= yyls; struct Cyc_Yyltype* _temp7199= _temp7197.curr
+ yylsp_offset; if( _temp7197.base == 0? 1:( _temp7199 < _temp7197.base? 1:
_temp7199 >= _temp7197.last_plus_one)){ _throw( Null_Exception);}* _temp7199;}).last_line));
_temp7193;}); _temp7192;}); break; case 387: _LL7191: yyval=( struct
_xtunion_struct*)({ struct Cyc_QualId_tok_struct* _temp7201=( struct Cyc_QualId_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualId_tok_struct)); _temp7201[ 0]=({ struct Cyc_QualId_tok_struct
_temp7202; _temp7202.tag= Cyc_QualId_tok_tag; _temp7202.f1=({ struct _tuple1*
_temp7203=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp7203->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp7208=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp7208[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp7209; _temp7209.tag= Cyc_Absyn_Rel_n_tag; _temp7209.f1= 0; _temp7209;});
_temp7208;}); _temp7203->f2=({ struct _tagged_string* _temp7204=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp7204[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp7205= yyvs; struct
_xtunion_struct** _temp7207= _temp7205.curr + yyvsp_offset; if( _temp7205.base
== 0? 1:( _temp7207 < _temp7205.base? 1: _temp7207 >= _temp7205.last_plus_one)){
_throw( Null_Exception);}* _temp7207;})); _temp7204;}); _temp7203;}); _temp7202;});
_temp7201;}); break; case 388: _LL7200: yyval=({ struct _tagged_ptr2 _temp7211=
yyvs; struct _xtunion_struct** _temp7213= _temp7211.curr + yyvsp_offset; if(
_temp7211.base == 0? 1:( _temp7213 < _temp7211.base? 1: _temp7213 >= _temp7211.last_plus_one)){
_throw( Null_Exception);}* _temp7213;}); break; default: _LL7210:( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Yyimpossible_struct* _temp7215=( struct
Cyc_Yyimpossible_struct*) GC_malloc( sizeof( struct Cyc_Yyimpossible_struct));
_temp7215[ 0]=({ struct Cyc_Yyimpossible_struct _temp7216; _temp7216.tag= Cyc_Yyimpossible_tag;
_temp7216;}); _temp7215;}));} yyvsp_offset -= yylen; yyssp_offset -= yylen;
yylsp_offset -= yylen;({ struct _tagged_ptr2 _temp7217= yyvs; struct
_xtunion_struct** _temp7219= _temp7217.curr +( ++ yyvsp_offset); if( _temp7217.base
== 0? 1:( _temp7219 < _temp7217.base? 1: _temp7219 >= _temp7217.last_plus_one)){
_throw( Null_Exception);}* _temp7219= yyval;}); yylsp_offset ++; if( yylen == 0){({
struct _tagged_ptr3 _temp7220= yyls; struct Cyc_Yyltype* _temp7222= _temp7220.curr
+ yylsp_offset; if( _temp7220.base == 0? 1:( _temp7222 < _temp7220.base? 1:
_temp7222 >= _temp7220.last_plus_one)){ _throw( Null_Exception);}(* _temp7222).first_line=
Cyc_yylloc.first_line;});({ struct _tagged_ptr3 _temp7223= yyls; struct Cyc_Yyltype*
_temp7225= _temp7223.curr + yylsp_offset; if( _temp7223.base == 0? 1:( _temp7225
< _temp7223.base? 1: _temp7225 >= _temp7223.last_plus_one)){ _throw(
Null_Exception);}(* _temp7225).first_column= Cyc_yylloc.first_column;});({
struct _tagged_ptr3 _temp7226= yyls; struct Cyc_Yyltype* _temp7228= _temp7226.curr
+ yylsp_offset; if( _temp7226.base == 0? 1:( _temp7228 < _temp7226.base? 1:
_temp7228 >= _temp7226.last_plus_one)){ _throw( Null_Exception);}(* _temp7228).last_line=({
struct _tagged_ptr3 _temp7229= yyls; struct Cyc_Yyltype* _temp7231= _temp7229.curr
+( yylsp_offset - 1); if( _temp7229.base == 0? 1:( _temp7231 < _temp7229.base? 1:
_temp7231 >= _temp7229.last_plus_one)){ _throw( Null_Exception);}* _temp7231;}).last_line;});({
struct _tagged_ptr3 _temp7232= yyls; struct Cyc_Yyltype* _temp7234= _temp7232.curr
+ yylsp_offset; if( _temp7232.base == 0? 1:( _temp7234 < _temp7232.base? 1:
_temp7234 >= _temp7232.last_plus_one)){ _throw( Null_Exception);}(* _temp7234).last_column=({
struct _tagged_ptr3 _temp7235= yyls; struct Cyc_Yyltype* _temp7237= _temp7235.curr
+( yylsp_offset - 1); if( _temp7235.base == 0? 1:( _temp7237 < _temp7235.base? 1:
_temp7237 >= _temp7235.last_plus_one)){ _throw( Null_Exception);}* _temp7237;}).last_column;});({
struct _tagged_ptr3 _temp7238= yyls; struct Cyc_Yyltype* _temp7240= _temp7238.curr
+ yylsp_offset; if( _temp7238.base == 0? 1:( _temp7240 < _temp7238.base? 1:
_temp7240 >= _temp7238.last_plus_one)){ _throw( Null_Exception);}(* _temp7240).text=(
struct _tagged_string)({ char* _temp7241=( char*)""; struct _tagged_string
_temp7242; _temp7242.curr= _temp7241; _temp7242.base= _temp7241; _temp7242.last_plus_one=
_temp7241 + 1; _temp7242;});});} else{({ struct _tagged_ptr3 _temp7243= yyls;
struct Cyc_Yyltype* _temp7245= _temp7243.curr + yylsp_offset; if( _temp7243.base
== 0? 1:( _temp7245 < _temp7243.base? 1: _temp7245 >= _temp7243.last_plus_one)){
_throw( Null_Exception);}(* _temp7245).last_line=({ struct _tagged_ptr3
_temp7246= yyls; struct Cyc_Yyltype* _temp7248= _temp7246.curr +(( yylsp_offset
+ yylen) - 1); if( _temp7246.base == 0? 1:( _temp7248 < _temp7246.base? 1:
_temp7248 >= _temp7246.last_plus_one)){ _throw( Null_Exception);}* _temp7248;}).last_line;});({
struct _tagged_ptr3 _temp7249= yyls; struct Cyc_Yyltype* _temp7251= _temp7249.curr
+ yylsp_offset; if( _temp7249.base == 0? 1:( _temp7251 < _temp7249.base? 1:
_temp7251 >= _temp7249.last_plus_one)){ _throw( Null_Exception);}(* _temp7251).last_column=({
struct _tagged_ptr3 _temp7252= yyls; struct Cyc_Yyltype* _temp7254= _temp7252.curr
+(( yylsp_offset + yylen) - 1); if( _temp7252.base == 0? 1:( _temp7254 <
_temp7252.base? 1: _temp7254 >= _temp7252.last_plus_one)){ _throw(
Null_Exception);}* _temp7254;}).last_column;});} yyn=( int)({ short* _temp7255=(
short*) Cyc_yyr1; unsigned int _temp7256= yyn; if( _temp7256 >= 389u){ _throw(
Null_Exception);} _temp7255[ _temp7256];}); yystate=( int)({ short* _temp7257=(
short*) Cyc_yypgoto; unsigned int _temp7258= yyn - 120; if( _temp7258 >= 107u){
_throw( Null_Exception);} _temp7257[ _temp7258];}) +( int)({ struct _tagged_ptr1
_temp7259= yyss; short* _temp7261= _temp7259.curr + yyssp_offset; if( _temp7259.base
== 0? 1:( _temp7261 < _temp7259.base? 1: _temp7261 >= _temp7259.last_plus_one)){
_throw( Null_Exception);}* _temp7261;}); if(( yystate >= 0? yystate <= 4017: 0)?(
int)({ short* _temp7262=( short*) Cyc_yycheck; unsigned int _temp7263= yystate;
if( _temp7263 >= 4018u){ _throw( Null_Exception);} _temp7262[ _temp7263];}) ==(
int)({ struct _tagged_ptr1 _temp7264= yyss; short* _temp7266= _temp7264.curr +
yyssp_offset; if( _temp7264.base == 0? 1:( _temp7266 < _temp7264.base? 1:
_temp7266 >= _temp7264.last_plus_one)){ _throw( Null_Exception);}* _temp7266;}):
0){ yystate=( int)({ short* _temp7267=( short*) Cyc_yytable; unsigned int
_temp7268= yystate; if( _temp7268 >= 4018u){ _throw( Null_Exception);} _temp7267[
_temp7268];});} else{ yystate=( int)({ short* _temp7269=( short*) Cyc_yydefgoto;
unsigned int _temp7270= yyn - 120; if( _temp7270 >= 107u){ _throw(
Null_Exception);} _temp7269[ _temp7270];});} goto yynewstate; yyerrlab: if(
yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int)({ short* _temp7271=( short*) Cyc_yypact;
unsigned int _temp7272= yystate; if( _temp7272 >= 729u){ _throw( Null_Exception);}
_temp7271[ _temp7272];}); if( yyn > - 32768? yyn < 4017: 0){ int sze= 0; struct
_tagged_string msg; int x; int count; count= 0; for( x= yyn < 0? - yyn: 0;(
unsigned int) x < 227u / sizeof( char*); x ++){ if(( int)({ short* _temp7273=(
short*) Cyc_yycheck; unsigned int _temp7274= x + yyn; if( _temp7274 >= 4018u){
_throw( Null_Exception);} _temp7273[ _temp7274];}) == x){( unsigned int)( sze +=
Cyc_String_strlen(({ struct _tagged_string* _temp7275=( struct _tagged_string*)
Cyc_yytname; unsigned int _temp7276= x; if( _temp7276 >= 227u){ _throw(
Null_Exception);} _temp7275[ _temp7276];})) +( unsigned int) 15), count ++;}}
msg=({ unsigned int _temp7277=( unsigned int)( sze + 15); char* _temp7278=( char*)
GC_malloc_atomic( sizeof( char) * _temp7277); unsigned int i; struct
_tagged_string _temp7279={ _temp7278, _temp7278, _temp7278 + _temp7277}; for( i=
0; i < _temp7277; i ++){ _temp7278[ i]='\000';} _temp7279;}); Cyc_String_strcpy(
msg,( struct _tagged_string)({ char* _temp7280=( char*)"parse error"; struct
_tagged_string _temp7281; _temp7281.curr= _temp7280; _temp7281.base= _temp7280;
_temp7281.last_plus_one= _temp7280 + 12; _temp7281;})); if( count < 5){ count= 0;
for( x= yyn < 0? - yyn: 0;( unsigned int) x < 227u / sizeof( char*); x ++){ if((
int)({ short* _temp7282=( short*) Cyc_yycheck; unsigned int _temp7283= x + yyn;
if( _temp7283 >= 4018u){ _throw( Null_Exception);} _temp7282[ _temp7283];}) == x){
Cyc_String_strcat( msg, count == 0?( struct _tagged_string)({ char* _temp7284=(
char*)", expecting `"; struct _tagged_string _temp7285; _temp7285.curr=
_temp7284; _temp7285.base= _temp7284; _temp7285.last_plus_one= _temp7284 + 14;
_temp7285;}):( struct _tagged_string)({ char* _temp7286=( char*)" or `"; struct
_tagged_string _temp7287; _temp7287.curr= _temp7286; _temp7287.base= _temp7286;
_temp7287.last_plus_one= _temp7286 + 6; _temp7287;})); Cyc_String_strcat( msg,({
struct _tagged_string* _temp7288=( struct _tagged_string*) Cyc_yytname;
unsigned int _temp7289= x; if( _temp7289 >= 227u){ _throw( Null_Exception);}
_temp7288[ _temp7289];})); Cyc_String_strcat( msg,( struct _tagged_string)({
char* _temp7290=( char*)"'"; struct _tagged_string _temp7291; _temp7291.curr=
_temp7290; _temp7291.base= _temp7290; _temp7291.last_plus_one= _temp7290 + 2;
_temp7291;})); count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror(( struct
_tagged_string)({ char* _temp7292=( char*)"parse error"; struct _tagged_string
_temp7293; _temp7293.curr= _temp7292; _temp7293.base= _temp7292; _temp7293.last_plus_one=
_temp7292 + 12; _temp7293;}));}} goto yyerrlab1; yyerrlab1: if( yyerrstatus == 3){
if( Cyc_yychar == 0){ return 1;} Cyc_yychar= - 2;} yyerrstatus= 3; goto
yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset == 0){ return 1;}
yyvsp_offset --; yystate=( int)({ struct _tagged_ptr1 _temp7294= yyss; short*
_temp7296= _temp7294.curr +( -- yyssp_offset); if( _temp7294.base == 0? 1:(
_temp7296 < _temp7294.base? 1: _temp7296 >= _temp7294.last_plus_one)){ _throw(
Null_Exception);}* _temp7296;}); yylsp_offset --; yyerrhandle: yyn=( int)({
short* _temp7297=( short*) Cyc_yypact; unsigned int _temp7298= yystate; if(
_temp7298 >= 729u){ _throw( Null_Exception);} _temp7297[ _temp7298];}); if( yyn
== - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn < 0? 1: yyn > 4017)? 1:( int)({
short* _temp7299=( short*) Cyc_yycheck; unsigned int _temp7300= yyn; if(
_temp7300 >= 4018u){ _throw( Null_Exception);} _temp7299[ _temp7300];}) != 1){
goto yyerrdefault;} yyn=( int)({ short* _temp7301=( short*) Cyc_yytable;
unsigned int _temp7302= yyn; if( _temp7302 >= 4018u){ _throw( Null_Exception);}
_temp7301[ _temp7302];}); if( yyn < 0){ if( yyn == - 32768){ goto yyerrpop;} yyn=
- yyn; goto yyreduce;} else{ if( yyn == 0){ goto yyerrpop;}} if( yyn == 728){
return 0;}({ struct _tagged_ptr2 _temp7303= yyvs; struct _xtunion_struct**
_temp7305= _temp7303.curr +( ++ yyvsp_offset); if( _temp7303.base == 0? 1:(
_temp7305 < _temp7303.base? 1: _temp7305 >= _temp7303.last_plus_one)){ _throw(
Null_Exception);}* _temp7305= Cyc_yylval;});({ struct _tagged_ptr3 _temp7306=
yyls; struct Cyc_Yyltype* _temp7308= _temp7306.curr +( ++ yylsp_offset); if(
_temp7306.base == 0? 1:( _temp7308 < _temp7306.base? 1: _temp7308 >= _temp7306.last_plus_one)){
_throw( Null_Exception);}* _temp7308= Cyc_yylloc;}); goto yynewstate;} void Cyc_yyprint(
int i, struct _xtunion_struct* v){ struct _xtunion_struct* _temp7309= v; struct
_tuple12* _temp7329; struct _tuple12 _temp7331; int _temp7332; void* _temp7334;
char _temp7336; short _temp7338; struct _tagged_string _temp7340; struct Cyc_Core_Opt*
_temp7342; struct Cyc_Core_Opt* _temp7344; struct Cyc_Core_Opt _temp7346; struct
_tagged_string* _temp7347; struct _tuple1* _temp7349; struct _tuple1 _temp7351;
struct _tagged_string* _temp7352; void* _temp7354; _LL7311: if( _temp7309->tag
== Cyc_Okay_tok_tag){ goto _LL7312;} else{ goto _LL7313;} _LL7313: if((*((
struct _xtunion_struct*) _temp7309)).tag == Cyc_Int_tok_tag){ _LL7330: _temp7329=((
struct Cyc_Int_tok_struct*) _temp7309)->f1; _temp7331=* _temp7329; _LL7335:
_temp7334= _temp7331.f1; goto _LL7333; _LL7333: _temp7332= _temp7331.f2; goto
_LL7314;} else{ goto _LL7315;} _LL7315: if((*(( struct _xtunion_struct*)
_temp7309)).tag == Cyc_Char_tok_tag){ _LL7337: _temp7336=(( struct Cyc_Char_tok_struct*)
_temp7309)->f1; goto _LL7316;} else{ goto _LL7317;} _LL7317: if((*(( struct
_xtunion_struct*) _temp7309)).tag == Cyc_Short_tok_tag){ _LL7339: _temp7338=((
struct Cyc_Short_tok_struct*) _temp7309)->f1; goto _LL7318;} else{ goto _LL7319;}
_LL7319: if((*(( struct _xtunion_struct*) _temp7309)).tag == Cyc_String_tok_tag){
_LL7341: _temp7340=(( struct Cyc_String_tok_struct*) _temp7309)->f1; goto
_LL7320;} else{ goto _LL7321;} _LL7321: if((*(( struct _xtunion_struct*)
_temp7309)).tag == Cyc_Stringopt_tok_tag){ _LL7343: _temp7342=(( struct Cyc_Stringopt_tok_struct*)
_temp7309)->f1; if( _temp7342 == 0){ goto _LL7322;} else{ goto _LL7323;}} else{
goto _LL7323;} _LL7323: if((*(( struct _xtunion_struct*) _temp7309)).tag == Cyc_Stringopt_tok_tag){
_LL7345: _temp7344=(( struct Cyc_Stringopt_tok_struct*) _temp7309)->f1; if(
_temp7344 == 0){ goto _LL7325;} else{ _temp7346=* _temp7344; _LL7348: _temp7347=(
struct _tagged_string*) _temp7346.v; goto _LL7324;}} else{ goto _LL7325;}
_LL7325: if((*(( struct _xtunion_struct*) _temp7309)).tag == Cyc_QualId_tok_tag){
_LL7350: _temp7349=(( struct Cyc_QualId_tok_struct*) _temp7309)->f1; _temp7351=*
_temp7349; _LL7355: _temp7354= _temp7351.f1; goto _LL7353; _LL7353: _temp7352=
_temp7351.f2; goto _LL7326;} else{ goto _LL7327;} _LL7327: goto _LL7328; _LL7312:
fprintf( Cyc_Stdio_stderr,"ok"); goto _LL7310; _LL7314: fprintf( Cyc_Stdio_stderr,"%d",
_temp7332); goto _LL7310; _LL7316: fprintf( Cyc_Stdio_stderr,"%c",( int)
_temp7336); goto _LL7310; _LL7318: fprintf( Cyc_Stdio_stderr,"%ds",( int)
_temp7338); goto _LL7310; _LL7320:({ struct _tagged_string _temp7356= _temp7340;
fprintf( Cyc_Stdio_stderr,"\"%.*s\"", _temp7356.last_plus_one - _temp7356.curr,
_temp7356.curr);}); goto _LL7310; _LL7322: fprintf( Cyc_Stdio_stderr,"null");
goto _LL7310; _LL7324:({ struct _tagged_string _temp7357=* _temp7347; fprintf(
Cyc_Stdio_stderr,"\"%.*s\"", _temp7357.last_plus_one - _temp7357.curr, _temp7357.curr);});
goto _LL7310; _LL7326: { struct Cyc_List_List* prefix= 0;{ void* _temp7358=
_temp7354; struct Cyc_List_List* _temp7366; struct Cyc_List_List* _temp7368;
_LL7360: if(( unsigned int) _temp7358 > 1u?(( struct _tunion_struct*) _temp7358)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL7367: _temp7366=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp7358)->f1; goto _LL7361;} else{ goto _LL7362;}
_LL7362: if(( unsigned int) _temp7358 > 1u?(( struct _tunion_struct*) _temp7358)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL7369: _temp7368=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp7358)->f1; goto _LL7363;} else{ goto _LL7364;}
_LL7364: if(( int) _temp7358 == Cyc_Absyn_Loc_n){ goto _LL7365;} else{ goto
_LL7359;} _LL7361: prefix= _temp7366; goto _LL7359; _LL7363: prefix= _temp7368;
goto _LL7359; _LL7365: goto _LL7359; _LL7359:;} for( 0; prefix != 0; prefix=({
struct Cyc_List_List* _temp7370= prefix; if( _temp7370 == 0){ _throw(
Null_Exception);} _temp7370->tl;})){({ struct _tagged_string _temp7372=*((
struct _tagged_string*)({ struct Cyc_List_List* _temp7371= prefix; if( _temp7371
== 0){ _throw( Null_Exception);} _temp7371->hd;})); fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp7372.last_plus_one - _temp7372.curr, _temp7372.curr);});}({ struct
_tagged_string _temp7373=* _temp7352; fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp7373.last_plus_one - _temp7373.curr, _temp7373.curr);}); goto _LL7310;}
_LL7328: fprintf( Cyc_Stdio_stderr,"?"); goto _LL7310; _LL7310:;} struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0;
Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp7374=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp7374->v=( void*) Cyc_Lexing_from_file(
f); _temp7374;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}