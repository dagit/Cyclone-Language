#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple4{
struct Cyc_Absyn_Tqual* f1; void* f2; struct Cyc_List_List* f3; } ; struct
_tuple5{ struct Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple6{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; } ; struct _tuple7{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; struct _tuple8{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Pat* f2; } ; typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring;
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
char* tag; struct _tagged_string f1; } ; extern struct _tagged_string Cyc_Core_string_of_int(
int); extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stdout; typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t;
extern int Cyc_Stdio_fputc( int, struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileOpenError_tag[
14u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; extern int Cyc_Stdio_file_string_write( struct Cyc_Stdio___sFILE*
fd, struct _tagged_string src, int src_offset, int max_count); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern int Cyc_List_length( struct Cyc_List_List* x); extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
5u]; struct Cyc_Position_Exit_struct{ char* tag; } ; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; typedef void* Cyc_Position_Error_kind;
static const unsigned int Cyc_Position_Lex= 0; static const unsigned int Cyc_Position_Parse=
1; static const unsigned int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag; } ; typedef struct _tagged_string* Cyc_Absyn_field_name_t; typedef
struct _tagged_string* Cyc_Absyn_var_t; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
typedef void* Cyc_Absyn_Nmspace; typedef void* Cyc_Absyn_nmspace_t; typedef
struct _tuple0* Cyc_Absyn_qvar_t; typedef struct _tuple0* Cyc_Absyn_qvar_opt_t;
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
char* tag; } ; extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct
Cyc_Absyn_Conref* x); extern int Cyc_Absyn_is_format_prim( void* p); extern
struct _tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern int Cyc_Absynpp_qvar_to_Cids; extern int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*); extern struct _tagged_string Cyc_Absynpp_char_escape(
char); extern struct _tagged_string Cyc_Absynpp_string_escape( struct
_tagged_string); extern struct _tagged_string Cyc_Absynpp_prim2str( void* p);
extern int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s); extern struct
_tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); extern struct _tuple4 Cyc_Absynpp_to_tms(
struct Cyc_Absyn_Tqual* tq, void* t); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t;
typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{ char* tag; }
; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef
struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[ 8u];
struct Cyc_Dict_Present_struct{ char* tag; } ; extern char Cyc_Dict_Absent_tag[
7u]; typedef void* Cyc_Tcenv_Resolved; static const int Cyc_Tcenv_VarRes_tag= 0;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes_tag=
1; struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;
} ; static const int Cyc_Tcenv_TunionRes_tag= 2; struct Cyc_Tcenv_TunionRes_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
static const int Cyc_Tcenv_XTunionRes_tag= 3; struct Cyc_Tcenv_XTunionRes_struct{
int tag; struct Cyc_Absyn_XTuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
static const int Cyc_Tcenv_EnumRes_tag= 4; struct Cyc_Tcenv_EnumRes_struct{ int
tag; struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; typedef
void* Cyc_Tcenv_resolved_t; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set*
namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict* uniondecls;
struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* xtuniondecls; struct
Cyc_Dict_Dict* enumdecls; struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict*
ordinaries; struct Cyc_List_List* availables; } ; typedef struct Cyc_Tcenv_Genv*
Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv; typedef struct Cyc_Tcenv_Fenv* Cyc_Tcenv_fenv_t;
typedef void* Cyc_Tcenv_Jumpee; static const unsigned int Cyc_Tcenv_NotLoop_j= 0;
static const unsigned int Cyc_Tcenv_CaseEnd_j= 1; static const unsigned int Cyc_Tcenv_FnEnd_j=
2; static const int Cyc_Tcenv_Stmt_j_tag= 0; struct Cyc_Tcenv_Stmt_j_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; } ; typedef void* Cyc_Tcenv_jumpee_t; typedef
void* Cyc_Tcenv_Frames; static const int Cyc_Tcenv_Outermost_tag= 0; struct Cyc_Tcenv_Outermost_struct{
int tag; void* f1; } ; static const int Cyc_Tcenv_Frame_tag= 1; struct Cyc_Tcenv_Frame_struct{
int tag; void* f1; void* f2; } ; static const int Cyc_Tcenv_Hidden_tag= 2;
struct Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void* f2; } ; typedef void*
Cyc_Tcenv_frames_t; struct Cyc_Tcenv_Tenv{ struct Cyc_List_List* ns; struct Cyc_Dict_Dict*
ae; struct Cyc_Core_Opt* le; } ; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t;
extern char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{ char*
tag; } ; extern void* Cyc_Tcutil_compress( void* t); extern void Cyc_Absyndump_dumptyp(
void*); extern void Cyc_Absyndump_dumpntyp( void* t); extern void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp*); extern void Cyc_Absyndump_dumpexp_prec( int, struct Cyc_Absyn_Exp*);
extern void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat*); extern void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt*); extern void Cyc_Absyndump_dumpdecl( struct Cyc_Absyn_Decl*);
extern void Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual*, void*, void(* f)(
void*), void*); typedef struct _tagged_string Cyc_Absyndump_dump_string_t;
struct Cyc_Stdio___sFILE** Cyc_Absyndump_dump_file=& Cyc_Stdio_stdout; void Cyc_Absyndump_ignore(
void* x){ return;} static unsigned int Cyc_Absyndump_pos= 0; void Cyc_Absyndump_dump(
struct _tagged_string s){ int sz=( int)({ struct _tagged_string _temp0= s;(
unsigned int)( _temp0.last_plus_one - _temp0.curr);}); if( !(( int)({ struct
_tagged_string _temp1= s; char* _temp3= _temp1.curr +( sz - 1); if( _temp3 <
_temp1.base? 1: _temp3 >= _temp1.last_plus_one){ _throw( Null_Exception);}*
_temp3;}))){ -- sz;} Cyc_Absyndump_pos +=( unsigned int)( sz + 1); if( Cyc_Absyndump_pos
>( unsigned int) 80){ Cyc_Absyndump_pos=( unsigned int) sz; Cyc_Stdio_fputc((
int)'\n',( struct Cyc_Stdio___sFILE*)* Cyc_Absyndump_dump_file);} else{ Cyc_Stdio_fputc((
int)' ',( struct Cyc_Stdio___sFILE*)* Cyc_Absyndump_dump_file);} Cyc_Stdio_file_string_write((
struct Cyc_Stdio___sFILE*)* Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_nospace(
struct _tagged_string s){ int sz=( int)({ struct _tagged_string _temp4= s;(
unsigned int)( _temp4.last_plus_one - _temp4.curr);}); if( !(( int)({ struct
_tagged_string _temp5= s; char* _temp7= _temp5.curr +( sz - 1); if( _temp7 <
_temp5.base? 1: _temp7 >= _temp5.last_plus_one){ _throw( Null_Exception);}*
_temp7;}))){ -- sz;} Cyc_Absyndump_pos +=( unsigned int) sz; Cyc_Stdio_file_string_write((
struct Cyc_Stdio___sFILE*)* Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_char(
int c){ ++ Cyc_Absyndump_pos; Cyc_Stdio_fputc( c,( struct Cyc_Stdio___sFILE*)*
Cyc_Absyndump_dump_file);} void Cyc_Absyndump_dump_str( struct _tagged_string* s){
Cyc_Absyndump_dump(* s);} void Cyc_Absyndump_dump_semi(){ Cyc_Absyndump_dump_char((
int)';');} void Cyc_Absyndump_dump_sep( void(* f)( void*), struct Cyc_List_List*
l, struct _tagged_string sep){ if( l == 0){ return;} for( 0; l->tl != 0; l= l->tl){
f(( void*) l->hd); Cyc_Absyndump_dump_nospace( sep);} f(( void*) l->hd);} void
Cyc_Absyndump_dump_sep_c( void(* f)( void*, void*), void* env, struct Cyc_List_List*
l, struct _tagged_string sep){ if( l == 0){ return;} for( 0; l->tl != 0; l= l->tl){
f( env,( void*) l->hd); Cyc_Absyndump_dump_nospace( sep);} f( env,( void*) l->hd);}
void Cyc_Absyndump_group( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep){ Cyc_Absyndump_dump_nospace(
start);(( void(*)( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string sep)) Cyc_Absyndump_dump_sep)( f, l, sep); Cyc_Absyndump_dump_nospace(
end);} void Cyc_Absyndump_group_c( void(* f)( void*, void*), void* env, struct
Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end, struct
_tagged_string sep){ Cyc_Absyndump_dump_nospace( start);(( void(*)( void(* f)(
void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_string sep))
Cyc_Absyndump_dump_sep_c)( f, env, l, sep); Cyc_Absyndump_dump_nospace( end);}
void Cyc_Absyndump_egroup( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep){ if(
l != 0){(( void(*)( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
f, l, start, end, sep);}} void Cyc_Absyndump_dumpqvar( struct _tuple0* v){
struct Cyc_List_List* nsl= 0;{ void* _temp8=(* v).f1; struct Cyc_List_List*
_temp20; struct Cyc_List_List* _temp22; struct Cyc_List_List* _temp24; struct
Cyc_List_List* _temp26; _LL10: if(( int) _temp8 == Cyc_Absyn_Loc_n){ goto _LL11;}
else{ goto _LL12;} _LL12: if(( unsigned int) _temp8 > 1u?(( struct
_tunion_struct*) _temp8)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL21: _temp20=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp8)->f1; if(
_temp20 == 0){ goto _LL13;} else{ goto _LL14;}} else{ goto _LL14;} _LL14: if((
unsigned int) _temp8 > 1u?(( struct _tunion_struct*) _temp8)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL23: _temp22=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp8)->f1; if( _temp22 == 0){ goto _LL15;} else{ goto _LL16;}} else{ goto
_LL16;} _LL16: if(( unsigned int) _temp8 > 1u?(( struct _tunion_struct*) _temp8)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL25: _temp24=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp8)->f1; goto _LL17;} else{ goto _LL18;} _LL18: if(( unsigned int) _temp8 >
1u?(( struct _tunion_struct*) _temp8)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL27:
_temp26=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp8)->f1;
goto _LL19;} else{ goto _LL9;} _LL11: goto _LL13; _LL13: goto _LL15; _LL15: Cyc_Absyndump_dump_str((*
v).f2); return; _LL17: _temp26= _temp24; goto _LL19; _LL19: nsl= _temp26; goto
_LL9; _LL9:;} Cyc_Absyndump_dump_str(( struct _tagged_string*) nsl->hd); for(
nsl= nsl->tl; nsl != 0; nsl= nsl->tl){ if( Cyc_Absynpp_qvar_to_Cids){ Cyc_Absyndump_dump_char((
int)'_');} else{ Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp28=( char*)"::"; struct _tagged_string _temp29; _temp29.curr= _temp28;
_temp29.base= _temp28; _temp29.last_plus_one= _temp28 + 3; _temp29;}));} Cyc_Absyndump_dump_nospace(*((
struct _tagged_string*) nsl->hd));} if( Cyc_Absynpp_qvar_to_Cids){ Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp30=( char*)"_"; struct _tagged_string
_temp31; _temp31.curr= _temp30; _temp31.base= _temp30; _temp31.last_plus_one=
_temp30 + 2; _temp31;}));} else{ Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp32=( char*)"::"; struct _tagged_string _temp33;
_temp33.curr= _temp32; _temp33.base= _temp32; _temp33.last_plus_one= _temp32 + 3;
_temp33;}));} Cyc_Absyndump_dump_nospace(*(* v).f2);} void Cyc_Absyndump_dumptq(
struct Cyc_Absyn_Tqual* tq){ if( tq->q_restrict){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp34=( char*)"restrict"; struct _tagged_string
_temp35; _temp35.curr= _temp34; _temp35.base= _temp34; _temp35.last_plus_one=
_temp34 + 9; _temp35;}));} if( tq->q_volatile){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp36=( char*)"volatile"; struct _tagged_string
_temp37; _temp37.curr= _temp36; _temp37.base= _temp36; _temp37.last_plus_one=
_temp36 + 9; _temp37;}));} if( tq->q_const){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp38=( char*)"const"; struct _tagged_string _temp39;
_temp39.curr= _temp38; _temp39.base= _temp38; _temp39.last_plus_one= _temp38 + 6;
_temp39;}));}} void Cyc_Absyndump_dumpscope( void* sc){ void* _temp40= sc; _LL42:
if(( int) _temp40 == Cyc_Absyn_Static){ goto _LL43;} else{ goto _LL44;} _LL44:
if(( int) _temp40 == Cyc_Absyn_Public){ goto _LL45;} else{ goto _LL46;} _LL46:
if(( int) _temp40 == Cyc_Absyn_Extern){ goto _LL47;} else{ goto _LL48;} _LL48:
if(( int) _temp40 == Cyc_Absyn_ExternC){ goto _LL49;} else{ goto _LL50;} _LL50:
if(( int) _temp40 == Cyc_Absyn_Abstract){ goto _LL51;} else{ goto _LL41;} _LL43:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp52=( char*)"static";
struct _tagged_string _temp53; _temp53.curr= _temp52; _temp53.base= _temp52;
_temp53.last_plus_one= _temp52 + 7; _temp53;})); return; _LL45: return; _LL47:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp54=( char*)"extern";
struct _tagged_string _temp55; _temp55.curr= _temp54; _temp55.base= _temp54;
_temp55.last_plus_one= _temp54 + 7; _temp55;})); return; _LL49: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp56=( char*)"extern \"C\""; struct
_tagged_string _temp57; _temp57.curr= _temp56; _temp57.base= _temp56; _temp57.last_plus_one=
_temp56 + 11; _temp57;})); return; _LL51: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp58=( char*)"abstract"; struct _tagged_string
_temp59; _temp59.curr= _temp58; _temp59.base= _temp58; _temp59.last_plus_one=
_temp58 + 9; _temp59;})); return; _LL41:;} void Cyc_Absyndump_dumpkind( void* k){
void* _temp60= k; _LL62: if(( int) _temp60 == Cyc_Absyn_AnyKind){ goto _LL63;}
else{ goto _LL64;} _LL64: if(( int) _temp60 == Cyc_Absyn_MemKind){ goto _LL65;}
else{ goto _LL66;} _LL66: if(( int) _temp60 == Cyc_Absyn_BoxKind){ goto _LL67;}
else{ goto _LL68;} _LL68: if(( int) _temp60 == Cyc_Absyn_RgnKind){ goto _LL69;}
else{ goto _LL70;} _LL70: if(( int) _temp60 == Cyc_Absyn_EffKind){ goto _LL71;}
else{ goto _LL61;} _LL63: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp72=( char*)"A"; struct _tagged_string _temp73; _temp73.curr= _temp72;
_temp73.base= _temp72; _temp73.last_plus_one= _temp72 + 2; _temp73;})); return;
_LL65: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp74=( char*)"M";
struct _tagged_string _temp75; _temp75.curr= _temp74; _temp75.base= _temp74;
_temp75.last_plus_one= _temp74 + 2; _temp75;})); return; _LL67: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp76=( char*)"B"; struct _tagged_string
_temp77; _temp77.curr= _temp76; _temp77.base= _temp76; _temp77.last_plus_one=
_temp76 + 2; _temp77;})); return; _LL69: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp78=( char*)"R"; struct _tagged_string _temp79;
_temp79.curr= _temp78; _temp79.base= _temp78; _temp79.last_plus_one= _temp78 + 2;
_temp79;})); return; _LL71: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp80=( char*)"E"; struct _tagged_string _temp81; _temp81.curr= _temp80;
_temp81.base= _temp80; _temp81.last_plus_one= _temp80 + 2; _temp81;})); return;
_LL61:;} void Cyc_Absyndump_dumptps( struct Cyc_List_List* ts){(( void(*)( void(*
f)( void*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptyp,
ts,( struct _tagged_string)({ char* _temp82=( char*)"<"; struct _tagged_string
_temp83; _temp83.curr= _temp82; _temp83.base= _temp82; _temp83.last_plus_one=
_temp82 + 2; _temp83;}),( struct _tagged_string)({ char* _temp84=( char*)">";
struct _tagged_string _temp85; _temp85.curr= _temp84; _temp85.base= _temp84;
_temp85.last_plus_one= _temp84 + 2; _temp85;}),( struct _tagged_string)({ char*
_temp86=( char*)","; struct _tagged_string _temp87; _temp87.curr= _temp86;
_temp87.base= _temp86; _temp87.last_plus_one= _temp86 + 2; _temp87;}));} void
Cyc_Absyndump_dumptvar( struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str( tv->name);}
void Cyc_Absyndump_dumpkindedtvar( struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str(
tv->name);{ void* _temp88=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind))->v; void* _temp96; void* _temp98;
_LL90: if(( unsigned int) _temp88 > 1u?(( struct _tunion_struct*) _temp88)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL97: _temp96=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp88)->f1; if(( int) _temp96 == Cyc_Absyn_BoxKind){ goto _LL91;} else{ goto
_LL92;}} else{ goto _LL92;} _LL92: if(( unsigned int) _temp88 > 1u?(( struct
_tunion_struct*) _temp88)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL99: _temp98=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp88)->f1; goto _LL93;} else{
goto _LL94;} _LL94: goto _LL95; _LL91: goto _LL89; _LL93: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp100=( char*)"::"; struct _tagged_string
_temp101; _temp101.curr= _temp100; _temp101.base= _temp100; _temp101.last_plus_one=
_temp100 + 3; _temp101;})); Cyc_Absyndump_dumpkind( _temp98); goto _LL89; _LL95:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp102=( char*)"::?";
struct _tagged_string _temp103; _temp103.curr= _temp102; _temp103.base= _temp102;
_temp103.last_plus_one= _temp102 + 4; _temp103;})); goto _LL89; _LL89:;}} void
Cyc_Absyndump_dumptvars( struct Cyc_List_List* tvs){(( void(*)( void(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar, tvs,( struct _tagged_string)({ char* _temp104=( char*)"<";
struct _tagged_string _temp105; _temp105.curr= _temp104; _temp105.base= _temp104;
_temp105.last_plus_one= _temp104 + 2; _temp105;}),( struct _tagged_string)({
char* _temp106=( char*)">"; struct _tagged_string _temp107; _temp107.curr=
_temp106; _temp107.base= _temp106; _temp107.last_plus_one= _temp106 + 2;
_temp107;}),( struct _tagged_string)({ char* _temp108=( char*)","; struct
_tagged_string _temp109; _temp109.curr= _temp108; _temp109.base= _temp108;
_temp109.last_plus_one= _temp108 + 2; _temp109;}));} void Cyc_Absyndump_dumpkindedtvars(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumpkindedtvar,
tvs,( struct _tagged_string)({ char* _temp110=( char*)"<"; struct _tagged_string
_temp111; _temp111.curr= _temp110; _temp111.base= _temp110; _temp111.last_plus_one=
_temp110 + 2; _temp111;}),( struct _tagged_string)({ char* _temp112=( char*)">";
struct _tagged_string _temp113; _temp113.curr= _temp112; _temp113.base= _temp112;
_temp113.last_plus_one= _temp112 + 2; _temp113;}),( struct _tagged_string)({
char* _temp114=( char*)","; struct _tagged_string _temp115; _temp115.curr=
_temp114; _temp115.base= _temp114; _temp115.last_plus_one= _temp114 + 2;
_temp115;}));} void Cyc_Absyndump_dumparg( struct _tuple5* pr){(( void(*)(
struct Cyc_Absyn_Tqual*, void*, void(* f)( int), int)) Cyc_Absyndump_dumptqtd)((
struct Cyc_Absyn_Tqual*)(* pr).f1,(* pr).f2,( void(*)( int x)) Cyc_Absyndump_ignore,
0);} void Cyc_Absyndump_dumpargs( struct Cyc_List_List* ts){(( void(*)( void(* f)(
struct _tuple5*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumparg,
ts,( struct _tagged_string)({ char* _temp116=( char*)"("; struct _tagged_string
_temp117; _temp117.curr= _temp116; _temp117.base= _temp116; _temp117.last_plus_one=
_temp116 + 2; _temp117;}),( struct _tagged_string)({ char* _temp118=( char*)")";
struct _tagged_string _temp119; _temp119.curr= _temp118; _temp119.base= _temp118;
_temp119.last_plus_one= _temp118 + 2; _temp119;}),( struct _tagged_string)({
char* _temp120=( char*)","; struct _tagged_string _temp121; _temp121.curr=
_temp120; _temp121.base= _temp120; _temp121.last_plus_one= _temp120 + 2;
_temp121;}));} void Cyc_Absyndump_dumpatts( struct Cyc_List_List* atts){ if(
atts == 0){ return;} Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp122=( char*)" __attribute__(("; struct _tagged_string _temp123; _temp123.curr=
_temp122; _temp123.base= _temp122; _temp123.last_plus_one= _temp122 + 17;
_temp123;})); for( 0; atts != 0; atts= atts->tl){ Cyc_Absyndump_dump( Cyc_Absyn_attribute2string((
void*) atts->hd)); if( atts->tl != 0){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp124=( char*)","; struct _tagged_string _temp125;
_temp125.curr= _temp124; _temp125.base= _temp124; _temp125.last_plus_one=
_temp124 + 2; _temp125;}));}} Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp126=( char*)")) "; struct _tagged_string _temp127; _temp127.curr= _temp126;
_temp127.base= _temp126; _temp127.last_plus_one= _temp126 + 4; _temp127;}));}
int Cyc_Absyndump_next_is_pointer( struct Cyc_List_List* tms){ if( tms == 0){
return 0;}{ void* _temp128=( void*) tms->hd; struct Cyc_Absyn_Tqual* _temp134;
void* _temp136; void* _temp138; _LL130: if(( unsigned int) _temp128 > 1u?((
struct _tunion_struct*) _temp128)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL139:
_temp138=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp128)->f1; goto
_LL137; _LL137: _temp136=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp128)->f2; goto _LL135; _LL135: _temp134=( struct Cyc_Absyn_Tqual*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp128)->f3; goto _LL131;} else{ goto _LL132;}
_LL132: goto _LL133; _LL131: return 1; _LL133: return 0; _LL129:;}} static void
Cyc_Absyndump_dumprgn( void* t){ void* _temp140= Cyc_Tcutil_compress( t); _LL142:
if(( int) _temp140 == Cyc_Absyn_HeapRgn){ goto _LL143;} else{ goto _LL144;}
_LL144: goto _LL145; _LL143: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp146=( char*)"`H"; struct _tagged_string _temp147; _temp147.curr= _temp146;
_temp147.base= _temp146; _temp147.last_plus_one= _temp146 + 3; _temp147;}));
goto _LL141; _LL145: Cyc_Absyndump_dumpntyp( t); goto _LL141; _LL141:;} static
struct _tuple6 Cyc_Absyndump_effects_split( void* t){ struct Cyc_List_List*
regions= 0; struct Cyc_List_List* effects= 0;{ void* _temp148= Cyc_Tcutil_compress(
t); void* _temp156; struct Cyc_List_List* _temp158; _LL150: if(( unsigned int)
_temp148 > 4u?(( struct _tunion_struct*) _temp148)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL157: _temp156=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp148)->f1;
goto _LL151;} else{ goto _LL152;} _LL152: if(( unsigned int) _temp148 > 4u?((
struct _tunion_struct*) _temp148)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL159:
_temp158=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp148)->f1;
goto _LL153;} else{ goto _LL154;} _LL154: goto _LL155; _LL151: regions=({ struct
Cyc_List_List* _temp160=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp160->hd=( void*) _temp156; _temp160->tl= regions; _temp160;}); goto _LL149;
_LL153: for( 0; _temp158 != 0; _temp158= _temp158->tl){ struct Cyc_List_List*
_temp163; struct Cyc_List_List* _temp165; struct _tuple6 _temp161= Cyc_Absyndump_effects_split((
void*) _temp158->hd); _LL166: _temp165= _temp161.f1; goto _LL164; _LL164:
_temp163= _temp161.f2; goto _LL162; _LL162: regions=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp165, regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp163, effects);} goto _LL149;
_LL155: effects=({ struct Cyc_List_List* _temp167=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp167->hd=( void*) t; _temp167->tl=
effects; _temp167;}); goto _LL149; _LL149:;} return({ struct _tuple6 _temp168;
_temp168.f1= regions; _temp168.f2= effects; _temp168;});} static void Cyc_Absyndump_dumpeff(
void* t){ struct Cyc_List_List* _temp171; struct Cyc_List_List* _temp173; struct
_tuple6 _temp169= Cyc_Absyndump_effects_split( t); _LL174: _temp173= _temp169.f1;
goto _LL172; _LL172: _temp171= _temp169.f2; goto _LL170; _LL170: _temp173=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp173);
_temp171=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp171); for( 0; _temp171 != 0; _temp171= _temp171->tl){ Cyc_Absyndump_dumpntyp((
void*) _temp171->hd); Cyc_Absyndump_dump_char(( int)'+');} Cyc_Absyndump_dump_char((
int)'{'); for( 0; _temp173 != 0; _temp173= _temp173->tl){ Cyc_Absyndump_dumprgn((
void*) _temp173->hd); if( _temp173->tl != 0){ Cyc_Absyndump_dump_char(( int)',');}}
Cyc_Absyndump_dump_char(( int)'}');} void Cyc_Absyndump_dumpntyp( void* t){ void*
_temp175= t; struct Cyc_Absyn_Exp* _temp241; struct Cyc_Absyn_Tqual* _temp243;
void* _temp245; struct Cyc_Absyn_FnInfo _temp247; struct Cyc_Absyn_PtrInfo
_temp249; struct Cyc_Absyn_Tvar* _temp251; int _temp253; struct Cyc_Core_Opt*
_temp255; void* _temp257; int _temp259; struct Cyc_Core_Opt* _temp261; struct
Cyc_Core_Opt _temp263; void* _temp264; void* _temp266; struct Cyc_Absyn_TunionInfo
_temp268; struct Cyc_Absyn_Tuniondecl* _temp270; void* _temp272; struct Cyc_List_List*
_temp274; struct _tuple0* _temp276; struct Cyc_Absyn_XTunionInfo _temp278;
struct Cyc_Absyn_XTuniondecl* _temp280; void* _temp282; struct _tuple0* _temp284;
struct Cyc_Absyn_TunionFieldInfo _temp286; struct Cyc_Absyn_Tunionfield*
_temp288; struct Cyc_Absyn_Tuniondecl* _temp290; struct _tuple0* _temp292;
struct Cyc_List_List* _temp294; struct _tuple0* _temp296; struct Cyc_Absyn_XTunionFieldInfo
_temp298; struct Cyc_Absyn_Tunionfield* _temp300; struct Cyc_Absyn_XTuniondecl*
_temp302; struct _tuple0* _temp304; struct _tuple0* _temp306; struct Cyc_Absyn_Enumdecl*
_temp308; struct _tuple0* _temp310; void* _temp312; void* _temp314; void*
_temp316; void* _temp318; void* _temp320; void* _temp322; void* _temp324; void*
_temp326; void* _temp328; void* _temp330; void* _temp332; void* _temp334; void*
_temp336; void* _temp338; void* _temp340; void* _temp342; struct Cyc_List_List*
_temp344; struct Cyc_Absyn_Structdecl** _temp346; struct Cyc_List_List* _temp348;
struct _tuple0* _temp350; struct Cyc_Absyn_Structdecl** _temp352; struct Cyc_List_List*
_temp354; struct _tuple0* _temp356; struct Cyc_Absyn_Uniondecl** _temp358;
struct Cyc_List_List* _temp360; struct _tuple0* _temp362; struct Cyc_Absyn_Uniondecl**
_temp364; struct Cyc_List_List* _temp366; struct _tuple0* _temp368; struct Cyc_Core_Opt*
_temp370; struct Cyc_List_List* _temp372; struct _tuple0* _temp374; void*
_temp376; void* _temp378; struct Cyc_List_List* _temp380; _LL177: if((
unsigned int) _temp175 > 4u?(( struct _tunion_struct*) _temp175)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL246: _temp245=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp175)->f1;
goto _LL244; _LL244: _temp243=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp175)->f2; goto _LL242; _LL242: _temp241=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp175)->f3; goto _LL178;} else{ goto _LL179;}
_LL179: if(( unsigned int) _temp175 > 4u?(( struct _tunion_struct*) _temp175)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL248: _temp247=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp175)->f1; goto _LL180;} else{ goto _LL181;}
_LL181: if(( unsigned int) _temp175 > 4u?(( struct _tunion_struct*) _temp175)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL250: _temp249=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp175)->f1; goto _LL182;} else{ goto
_LL183;} _LL183: if(( int) _temp175 == Cyc_Absyn_VoidType){ goto _LL184;} else{
goto _LL185;} _LL185: if(( unsigned int) _temp175 > 4u?(( struct _tunion_struct*)
_temp175)->tag == Cyc_Absyn_VarType_tag: 0){ _LL252: _temp251=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp175)->f1; goto _LL186;} else{ goto _LL187;}
_LL187: if(( unsigned int) _temp175 > 4u?(( struct _tunion_struct*) _temp175)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL258: _temp257=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp175)->f1; goto _LL256; _LL256: _temp255=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp175)->f2; if( _temp255 == 0){ goto _LL254;} else{
goto _LL189;} _LL254: _temp253=( int)(( struct Cyc_Absyn_Evar_struct*) _temp175)->f3;
goto _LL188;} else{ goto _LL189;} _LL189: if(( unsigned int) _temp175 > 4u?((
struct _tunion_struct*) _temp175)->tag == Cyc_Absyn_Evar_tag: 0){ _LL267:
_temp266=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp175)->f1; goto _LL262;
_LL262: _temp261=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp175)->f2; if( _temp261 == 0){ goto _LL191;} else{ _temp263=* _temp261;
_LL265: _temp264=( void*) _temp263.v; goto _LL260;} _LL260: _temp259=( int)((
struct Cyc_Absyn_Evar_struct*) _temp175)->f3; goto _LL190;} else{ goto _LL191;}
_LL191: if(( unsigned int) _temp175 > 4u?(( struct _tunion_struct*) _temp175)->tag
== Cyc_Absyn_TunionType_tag: 0){ _LL269: _temp268=( struct Cyc_Absyn_TunionInfo)((
struct Cyc_Absyn_TunionType_struct*) _temp175)->f1; _LL277: _temp276=( struct
_tuple0*) _temp268.name; goto _LL275; _LL275: _temp274=( struct Cyc_List_List*)
_temp268.targs; goto _LL273; _LL273: _temp272=( void*) _temp268.rgn; goto _LL271;
_LL271: _temp270=( struct Cyc_Absyn_Tuniondecl*) _temp268.tud; goto _LL192;}
else{ goto _LL193;} _LL193: if(( unsigned int) _temp175 > 4u?(( struct
_tunion_struct*) _temp175)->tag == Cyc_Absyn_XTunionType_tag: 0){ _LL279:
_temp278=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp175)->f1; _LL285: _temp284=( struct _tuple0*) _temp278.name; goto _LL283;
_LL283: _temp282=( void*) _temp278.rgn; goto _LL281; _LL281: _temp280=( struct
Cyc_Absyn_XTuniondecl*) _temp278.xtud; goto _LL194;} else{ goto _LL195;} _LL195:
if(( unsigned int) _temp175 > 4u?(( struct _tunion_struct*) _temp175)->tag ==
Cyc_Absyn_TunionFieldType_tag: 0){ _LL287: _temp286=( struct Cyc_Absyn_TunionFieldInfo)((
struct Cyc_Absyn_TunionFieldType_struct*) _temp175)->f1; _LL297: _temp296=(
struct _tuple0*) _temp286.name; goto _LL295; _LL295: _temp294=( struct Cyc_List_List*)
_temp286.targs; goto _LL293; _LL293: _temp292=( struct _tuple0*) _temp286.fname;
goto _LL291; _LL291: _temp290=( struct Cyc_Absyn_Tuniondecl*) _temp286.tud; goto
_LL289; _LL289: _temp288=( struct Cyc_Absyn_Tunionfield*) _temp286.tufd; goto
_LL196;} else{ goto _LL197;} _LL197: if(( unsigned int) _temp175 > 4u?(( struct
_tunion_struct*) _temp175)->tag == Cyc_Absyn_XTunionFieldType_tag: 0){ _LL299:
_temp298=( struct Cyc_Absyn_XTunionFieldInfo)(( struct Cyc_Absyn_XTunionFieldType_struct*)
_temp175)->f1; _LL307: _temp306=( struct _tuple0*) _temp298.name; goto _LL305;
_LL305: _temp304=( struct _tuple0*) _temp298.fname; goto _LL303; _LL303:
_temp302=( struct Cyc_Absyn_XTuniondecl*) _temp298.xtud; goto _LL301; _LL301:
_temp300=( struct Cyc_Absyn_Tunionfield*) _temp298.xtufd; goto _LL198;} else{
goto _LL199;} _LL199: if(( unsigned int) _temp175 > 4u?(( struct _tunion_struct*)
_temp175)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL311: _temp310=( struct _tuple0*)((
struct Cyc_Absyn_EnumType_struct*) _temp175)->f1; goto _LL309; _LL309: _temp308=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp175)->f2;
goto _LL200;} else{ goto _LL201;} _LL201: if(( unsigned int) _temp175 > 4u?((
struct _tunion_struct*) _temp175)->tag == Cyc_Absyn_IntType_tag: 0){ _LL315:
_temp314=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f1; if(( int)
_temp314 == Cyc_Absyn_Signed){ goto _LL313;} else{ goto _LL203;} _LL313:
_temp312=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f2; if(( int)
_temp312 == Cyc_Absyn_B1){ goto _LL202;} else{ goto _LL203;}} else{ goto _LL203;}
_LL203: if(( unsigned int) _temp175 > 4u?(( struct _tunion_struct*) _temp175)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL319: _temp318=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f1; if(( int) _temp318 == Cyc_Absyn_Unsigned){ goto _LL317;} else{
goto _LL205;} _LL317: _temp316=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f2; if(( int) _temp316 == Cyc_Absyn_B1){ goto _LL204;} else{ goto
_LL205;}} else{ goto _LL205;} _LL205: if(( unsigned int) _temp175 > 4u?(( struct
_tunion_struct*) _temp175)->tag == Cyc_Absyn_IntType_tag: 0){ _LL323: _temp322=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f1; if(( int) _temp322 ==
Cyc_Absyn_Signed){ goto _LL321;} else{ goto _LL207;} _LL321: _temp320=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp175)->f2; if(( int) _temp320 == Cyc_Absyn_B2){
goto _LL206;} else{ goto _LL207;}} else{ goto _LL207;} _LL207: if(( unsigned int)
_temp175 > 4u?(( struct _tunion_struct*) _temp175)->tag == Cyc_Absyn_IntType_tag:
0){ _LL327: _temp326=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f1;
if(( int) _temp326 == Cyc_Absyn_Unsigned){ goto _LL325;} else{ goto _LL209;}
_LL325: _temp324=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f2; if((
int) _temp324 == Cyc_Absyn_B2){ goto _LL208;} else{ goto _LL209;}} else{ goto
_LL209;} _LL209: if(( unsigned int) _temp175 > 4u?(( struct _tunion_struct*)
_temp175)->tag == Cyc_Absyn_IntType_tag: 0){ _LL331: _temp330=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp175)->f1; if(( int) _temp330 == Cyc_Absyn_Signed){
goto _LL329;} else{ goto _LL211;} _LL329: _temp328=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f2; if(( int) _temp328 == Cyc_Absyn_B4){ goto _LL210;} else{ goto
_LL211;}} else{ goto _LL211;} _LL211: if(( unsigned int) _temp175 > 4u?(( struct
_tunion_struct*) _temp175)->tag == Cyc_Absyn_IntType_tag: 0){ _LL335: _temp334=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f1; if(( int) _temp334 ==
Cyc_Absyn_Unsigned){ goto _LL333;} else{ goto _LL213;} _LL333: _temp332=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp175)->f2; if(( int) _temp332 == Cyc_Absyn_B4){
goto _LL212;} else{ goto _LL213;}} else{ goto _LL213;} _LL213: if(( unsigned int)
_temp175 > 4u?(( struct _tunion_struct*) _temp175)->tag == Cyc_Absyn_IntType_tag:
0){ _LL339: _temp338=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f1;
if(( int) _temp338 == Cyc_Absyn_Signed){ goto _LL337;} else{ goto _LL215;}
_LL337: _temp336=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp175)->f2; if((
int) _temp336 == Cyc_Absyn_B8){ goto _LL214;} else{ goto _LL215;}} else{ goto
_LL215;} _LL215: if(( unsigned int) _temp175 > 4u?(( struct _tunion_struct*)
_temp175)->tag == Cyc_Absyn_IntType_tag: 0){ _LL343: _temp342=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp175)->f1; if(( int) _temp342 == Cyc_Absyn_Unsigned){
goto _LL341;} else{ goto _LL217;} _LL341: _temp340=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp175)->f2; if(( int) _temp340 == Cyc_Absyn_B8){ goto _LL216;} else{ goto
_LL217;}} else{ goto _LL217;} _LL217: if(( int) _temp175 == Cyc_Absyn_FloatType){
goto _LL218;} else{ goto _LL219;} _LL219: if(( int) _temp175 == Cyc_Absyn_DoubleType){
goto _LL220;} else{ goto _LL221;} _LL221: if(( unsigned int) _temp175 > 4u?((
struct _tunion_struct*) _temp175)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL345:
_temp344=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp175)->f1;
goto _LL222;} else{ goto _LL223;} _LL223: if(( unsigned int) _temp175 > 4u?((
struct _tunion_struct*) _temp175)->tag == Cyc_Absyn_StructType_tag: 0){ _LL351:
_temp350=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp175)->f1;
if( _temp350 == 0){ goto _LL349;} else{ goto _LL225;} _LL349: _temp348=( struct
Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*) _temp175)->f2; goto
_LL347; _LL347: _temp346=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp175)->f3; goto _LL224;} else{ goto _LL225;} _LL225: if(( unsigned int)
_temp175 > 4u?(( struct _tunion_struct*) _temp175)->tag == Cyc_Absyn_StructType_tag:
0){ _LL357: _temp356=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp175)->f1; goto _LL355; _LL355: _temp354=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp175)->f2; goto _LL353; _LL353: _temp352=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp175)->f3;
goto _LL226;} else{ goto _LL227;} _LL227: if(( unsigned int) _temp175 > 4u?((
struct _tunion_struct*) _temp175)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL363:
_temp362=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*) _temp175)->f1;
if( _temp362 == 0){ goto _LL361;} else{ goto _LL229;} _LL361: _temp360=( struct
Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp175)->f2; goto _LL359;
_LL359: _temp358=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp175)->f3; goto _LL228;} else{ goto _LL229;} _LL229: if(( unsigned int)
_temp175 > 4u?(( struct _tunion_struct*) _temp175)->tag == Cyc_Absyn_UnionType_tag:
0){ _LL369: _temp368=( struct _tuple0*)(( struct Cyc_Absyn_UnionType_struct*)
_temp175)->f1; goto _LL367; _LL367: _temp366=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp175)->f2; goto _LL365; _LL365: _temp364=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp175)->f3;
goto _LL230;} else{ goto _LL231;} _LL231: if(( unsigned int) _temp175 > 4u?((
struct _tunion_struct*) _temp175)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL375:
_temp374=( struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp175)->f1;
goto _LL373; _LL373: _temp372=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp175)->f2; goto _LL371; _LL371: _temp370=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp175)->f3; goto _LL232;} else{ goto _LL233;}
_LL233: if(( unsigned int) _temp175 > 4u?(( struct _tunion_struct*) _temp175)->tag
== Cyc_Absyn_RgnHandleType_tag: 0){ _LL377: _temp376=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp175)->f1; goto _LL234;} else{ goto _LL235;} _LL235: if(( int) _temp175 ==
Cyc_Absyn_HeapRgn){ goto _LL236;} else{ goto _LL237;} _LL237: if(( unsigned int)
_temp175 > 4u?(( struct _tunion_struct*) _temp175)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL379: _temp378=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp175)->f1;
goto _LL238;} else{ goto _LL239;} _LL239: if(( unsigned int) _temp175 > 4u?((
struct _tunion_struct*) _temp175)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL381:
_temp380=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp175)->f1;
goto _LL240;} else{ goto _LL176;} _LL178: return; _LL180: return; _LL182:
return; _LL184: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp382=(
char*)"void"; struct _tagged_string _temp383; _temp383.curr= _temp382; _temp383.base=
_temp382; _temp383.last_plus_one= _temp382 + 5; _temp383;})); return; _LL186:
Cyc_Absyndump_dump_str( _temp251->name); return; _LL188: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp384=( char*)"%"; struct _tagged_string
_temp385; _temp385.curr= _temp384; _temp385.base= _temp384; _temp385.last_plus_one=
_temp384 + 2; _temp385;})); Cyc_Absyndump_dumpkind( _temp257); Cyc_Absyndump_dump(
xprintf("(%d)", _temp253)); return; _LL190: Cyc_Absyndump_dumpntyp( _temp264);
return; _LL192: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp386=(
char*)"tunion "; struct _tagged_string _temp387; _temp387.curr= _temp386;
_temp387.base= _temp386; _temp387.last_plus_one= _temp386 + 8; _temp387;}));{
void* _temp388= Cyc_Tcutil_compress( _temp272); _LL390: if(( int) _temp388 ==
Cyc_Absyn_HeapRgn){ goto _LL391;} else{ goto _LL392;} _LL392: goto _LL393;
_LL391: goto _LL389; _LL393: Cyc_Absyndump_dumptyp( _temp272); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp394=( char*)" "; struct _tagged_string
_temp395; _temp395.curr= _temp394; _temp395.base= _temp394; _temp395.last_plus_one=
_temp394 + 2; _temp395;})); goto _LL389; _LL389:;} Cyc_Absyndump_dumpqvar(
_temp276); Cyc_Absyndump_dumptps( _temp274); goto _LL176; _LL194: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp396=( char*)"xtunion "; struct
_tagged_string _temp397; _temp397.curr= _temp396; _temp397.base= _temp396;
_temp397.last_plus_one= _temp396 + 9; _temp397;}));{ void* _temp398= Cyc_Tcutil_compress(
_temp282); _LL400: if(( int) _temp398 == Cyc_Absyn_HeapRgn){ goto _LL401;} else{
goto _LL402;} _LL402: goto _LL403; _LL401: goto _LL399; _LL403: Cyc_Absyndump_dumptyp(
_temp282); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp404=( char*)" ";
struct _tagged_string _temp405; _temp405.curr= _temp404; _temp405.base= _temp404;
_temp405.last_plus_one= _temp404 + 2; _temp405;})); goto _LL399; _LL399:;} Cyc_Absyndump_dumpqvar(
_temp284); goto _LL176; _LL196: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp406=( char*)"tunion "; struct _tagged_string _temp407; _temp407.curr=
_temp406; _temp407.base= _temp406; _temp407.last_plus_one= _temp406 + 8;
_temp407;})); Cyc_Absyndump_dumpqvar( _temp296); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp408=( char*)"."; struct _tagged_string _temp409;
_temp409.curr= _temp408; _temp409.base= _temp408; _temp409.last_plus_one=
_temp408 + 2; _temp409;})); Cyc_Absyndump_dumpqvar( _temp292); Cyc_Absyndump_dumptps(
_temp294); goto _LL176; _LL198: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp410=( char*)"xtunion "; struct _tagged_string _temp411; _temp411.curr=
_temp410; _temp411.base= _temp410; _temp411.last_plus_one= _temp410 + 9;
_temp411;})); Cyc_Absyndump_dumpqvar( _temp306); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp412=( char*)"."; struct _tagged_string _temp413;
_temp413.curr= _temp412; _temp413.base= _temp412; _temp413.last_plus_one=
_temp412 + 2; _temp413;})); Cyc_Absyndump_dumpqvar( _temp304); goto _LL176;
_LL200: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp414=( char*)"enum ";
struct _tagged_string _temp415; _temp415.curr= _temp414; _temp415.base= _temp414;
_temp415.last_plus_one= _temp414 + 6; _temp415;})); Cyc_Absyndump_dumpqvar(
_temp310); return; _LL202: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp416=( char*)"signed char"; struct _tagged_string _temp417; _temp417.curr=
_temp416; _temp417.base= _temp416; _temp417.last_plus_one= _temp416 + 12;
_temp417;})); return; _LL204: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp418=( char*)"char"; struct _tagged_string _temp419; _temp419.curr= _temp418;
_temp419.base= _temp418; _temp419.last_plus_one= _temp418 + 5; _temp419;}));
return; _LL206: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp420=(
char*)"short"; struct _tagged_string _temp421; _temp421.curr= _temp420; _temp421.base=
_temp420; _temp421.last_plus_one= _temp420 + 6; _temp421;})); return; _LL208:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp422=( char*)"unsigned short";
struct _tagged_string _temp423; _temp423.curr= _temp422; _temp423.base= _temp422;
_temp423.last_plus_one= _temp422 + 15; _temp423;})); return; _LL210: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp424=( char*)"int"; struct _tagged_string
_temp425; _temp425.curr= _temp424; _temp425.base= _temp424; _temp425.last_plus_one=
_temp424 + 4; _temp425;})); return; _LL212: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp426=( char*)"unsigned int"; struct _tagged_string
_temp427; _temp427.curr= _temp426; _temp427.base= _temp426; _temp427.last_plus_one=
_temp426 + 13; _temp427;})); return; _LL214: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp428=( char*)"long long"; struct _tagged_string
_temp429; _temp429.curr= _temp428; _temp429.base= _temp428; _temp429.last_plus_one=
_temp428 + 10; _temp429;})); return; _LL216: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp430=( char*)"unsigned long long"; struct
_tagged_string _temp431; _temp431.curr= _temp430; _temp431.base= _temp430;
_temp431.last_plus_one= _temp430 + 19; _temp431;})); return; _LL218: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp432=( char*)"float"; struct _tagged_string
_temp433; _temp433.curr= _temp432; _temp433.base= _temp432; _temp433.last_plus_one=
_temp432 + 6; _temp433;})); return; _LL220: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp434=( char*)"double"; struct _tagged_string
_temp435; _temp435.curr= _temp434; _temp435.base= _temp434; _temp435.last_plus_one=
_temp434 + 7; _temp435;})); return; _LL222: Cyc_Absyndump_dump_char(( int)'$');
Cyc_Absyndump_dumpargs( _temp344); return; _LL224: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp436=( char*)"struct"; struct _tagged_string
_temp437; _temp437.curr= _temp436; _temp437.base= _temp436; _temp437.last_plus_one=
_temp436 + 7; _temp437;})); Cyc_Absyndump_dumptps( _temp348); return; _LL226:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp438=( char*)"struct";
struct _tagged_string _temp439; _temp439.curr= _temp438; _temp439.base= _temp438;
_temp439.last_plus_one= _temp438 + 7; _temp439;})); Cyc_Absyndump_dumpqvar((
struct _tuple0*)({ struct _tuple0* _temp440= _temp356; if( _temp440 == 0){
_throw( Null_Exception);} _temp440;})); Cyc_Absyndump_dumptps( _temp354);
return; _LL228: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp441=(
char*)"union"; struct _tagged_string _temp442; _temp442.curr= _temp441; _temp442.base=
_temp441; _temp442.last_plus_one= _temp441 + 6; _temp442;})); Cyc_Absyndump_dumptps(
_temp360); return; _LL230: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp443=( char*)"union"; struct _tagged_string _temp444; _temp444.curr=
_temp443; _temp444.base= _temp443; _temp444.last_plus_one= _temp443 + 6;
_temp444;})); Cyc_Absyndump_dumpqvar(( struct _tuple0*)({ struct _tuple0*
_temp445= _temp368; if( _temp445 == 0){ _throw( Null_Exception);} _temp445;}));
Cyc_Absyndump_dumptps( _temp366); return; _LL232: Cyc_Absyndump_dumpqvar(
_temp374), Cyc_Absyndump_dumptps( _temp372); return; _LL234: Cyc_Absyndump_dumprgn(
_temp376); return; _LL236: return; _LL238: return; _LL240: return; _LL176:;}
void Cyc_Absyndump_dumpvaropt( struct Cyc_Core_Opt* vo){ if( vo != 0){ Cyc_Absyndump_dump_str((
struct _tagged_string*) vo->v);}} void Cyc_Absyndump_dumpfunarg( struct _tuple1*
t){(( void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( struct Cyc_Core_Opt*),
struct Cyc_Core_Opt*)) Cyc_Absyndump_dumptqtd)(( struct Cyc_Absyn_Tqual*)(* t).f2,(*
t).f3, Cyc_Absyndump_dumpvaropt,(* t).f1);} void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt* effopt){ Cyc_Absyndump_dump_char((
int)'('); for( 0; args != 0; args= args->tl){ Cyc_Absyndump_dumpfunarg(( struct
_tuple1*) args->hd); if( args->tl != 0? 1: varargs){ Cyc_Absyndump_dump_char((
int)',');}} if( varargs){ Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp446=( char*)"..."; struct _tagged_string _temp447; _temp447.curr= _temp446;
_temp447.base= _temp446; _temp447.last_plus_one= _temp446 + 4; _temp447;}));}
if( effopt != 0){ Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpeff(( void*)
effopt->v);} Cyc_Absyndump_dump_char(( int)')');} void Cyc_Absyndump_dumptyp(
void* t){(( void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( int), int)) Cyc_Absyndump_dumptqtd)(
0, t,( void(*)( int x)) Cyc_Absyndump_ignore, 0);} void Cyc_Absyndump_dumpdesignator(
void* d){ void* _temp448= d; struct Cyc_Absyn_Exp* _temp454; struct
_tagged_string* _temp456; _LL450: if((( struct _tunion_struct*) _temp448)->tag
== Cyc_Absyn_ArrayElement_tag){ _LL455: _temp454=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp448)->f1; goto _LL451;} else{ goto
_LL452;} _LL452: if((( struct _tunion_struct*) _temp448)->tag == Cyc_Absyn_FieldName_tag){
_LL457: _temp456=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp448)->f1; goto _LL453;} else{ goto _LL449;} _LL451: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp458=( char*)".["; struct _tagged_string
_temp459; _temp459.curr= _temp458; _temp459.base= _temp458; _temp459.last_plus_one=
_temp458 + 3; _temp459;})); Cyc_Absyndump_dumpexp( _temp454); Cyc_Absyndump_dump_char((
int)']'); goto _LL449; _LL453: Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp456); goto _LL449; _LL449:;} void Cyc_Absyndump_dumpde( struct _tuple7* de){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* de).f1,( struct _tagged_string)({ char* _temp460=(
char*)""; struct _tagged_string _temp461; _temp461.curr= _temp460; _temp461.base=
_temp460; _temp461.last_plus_one= _temp460 + 1; _temp461;}),( struct
_tagged_string)({ char* _temp462=( char*)"="; struct _tagged_string _temp463;
_temp463.curr= _temp462; _temp463.base= _temp462; _temp463.last_plus_one=
_temp462 + 2; _temp463;}),( struct _tagged_string)({ char* _temp464=( char*)"=";
struct _tagged_string _temp465; _temp465.curr= _temp464; _temp465.base= _temp464;
_temp465.last_plus_one= _temp464 + 2; _temp465;})); Cyc_Absyndump_dumpexp((* de).f2);}
void Cyc_Absyndump_dumpexps_prec( int inprec, struct Cyc_List_List* es){(( void(*)(
void(* f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec, inprec, es,( struct _tagged_string)({ char* _temp466=(
char*)""; struct _tagged_string _temp467; _temp467.curr= _temp466; _temp467.base=
_temp466; _temp467.last_plus_one= _temp466 + 1; _temp467;}),( struct
_tagged_string)({ char* _temp468=( char*)""; struct _tagged_string _temp469;
_temp469.curr= _temp468; _temp469.base= _temp468; _temp469.last_plus_one=
_temp468 + 1; _temp469;}),( struct _tagged_string)({ char* _temp470=( char*)",";
struct _tagged_string _temp471; _temp471.curr= _temp470; _temp471.base= _temp470;
_temp471.last_plus_one= _temp470 + 2; _temp471;}));} void Cyc_Absyndump_dumpexp_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e); if(
inprec >= myprec){ Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp472=( char*)"("; struct _tagged_string _temp473; _temp473.curr= _temp472;
_temp473.base= _temp472; _temp473.last_plus_one= _temp472 + 2; _temp473;}));}{
void* _temp474=( void*) e->r; void* _temp566; char _temp568; void* _temp570;
void* _temp572; short _temp574; void* _temp576; void* _temp578; int _temp580;
void* _temp582; void* _temp584; int _temp586; void* _temp588; void* _temp590;
long long _temp592; void* _temp594; void* _temp596; struct _tagged_string
_temp598; void* _temp600; void* _temp602; struct _tagged_string _temp604; struct
_tuple0* _temp606; void* _temp608; struct _tuple0* _temp610; struct Cyc_List_List*
_temp612; void* _temp614; struct Cyc_Absyn_Exp* _temp616; struct Cyc_Core_Opt*
_temp618; struct Cyc_Absyn_Exp* _temp620; void* _temp622; struct Cyc_Absyn_Exp*
_temp624; void* _temp626; struct Cyc_Absyn_Exp* _temp628; void* _temp630; struct
Cyc_Absyn_Exp* _temp632; void* _temp634; struct Cyc_Absyn_Exp* _temp636; struct
Cyc_Absyn_Exp* _temp638; struct Cyc_Absyn_Exp* _temp640; struct Cyc_Absyn_Exp*
_temp642; struct Cyc_Absyn_Exp* _temp644; struct Cyc_Absyn_Exp* _temp646; struct
Cyc_List_List* _temp648; struct Cyc_Absyn_Exp* _temp650; struct Cyc_List_List*
_temp652; struct Cyc_Absyn_Exp* _temp654; struct Cyc_Absyn_Exp* _temp656; struct
Cyc_Absyn_Exp* _temp658; struct Cyc_List_List* _temp660; struct Cyc_Absyn_Exp*
_temp662; struct Cyc_Absyn_Exp* _temp664; void* _temp666; struct Cyc_Absyn_Exp*
_temp668; struct Cyc_Absyn_Exp* _temp670; void* _temp672; struct Cyc_Absyn_Exp*
_temp674; struct Cyc_Absyn_Exp* _temp676; struct _tagged_string* _temp678;
struct Cyc_Absyn_Exp* _temp680; struct _tagged_string* _temp682; struct Cyc_Absyn_Exp*
_temp684; struct Cyc_Absyn_Exp* _temp686; struct Cyc_Absyn_Exp* _temp688; struct
Cyc_List_List* _temp690; struct Cyc_List_List* _temp692; struct _tuple1*
_temp694; struct Cyc_List_List* _temp696; struct Cyc_Absyn_Exp* _temp698; struct
Cyc_Absyn_Exp* _temp700; struct Cyc_Absyn_Vardecl* _temp702; struct Cyc_Absyn_Structdecl*
_temp704; struct Cyc_List_List* _temp706; struct Cyc_Core_Opt* _temp708; struct
_tuple0* _temp710; struct Cyc_Absyn_Tunionfield* _temp712; struct Cyc_Absyn_Tuniondecl*
_temp714; struct Cyc_List_List* _temp716; struct Cyc_Core_Opt* _temp718; struct
Cyc_Core_Opt* _temp720; struct Cyc_Absyn_Tunionfield* _temp722; struct Cyc_Absyn_XTuniondecl*
_temp724; struct Cyc_List_List* _temp726; struct Cyc_Core_Opt* _temp728; struct
Cyc_Absyn_Enumfield* _temp730; struct Cyc_Absyn_Enumdecl* _temp732; struct
_tuple0* _temp734; void* _temp736; struct Cyc_List_List* _temp738; struct Cyc_Core_Opt*
_temp740; struct Cyc_Absyn_Stmt* _temp742; struct Cyc_Absyn_Fndecl* _temp744;
struct Cyc_Absyn_Exp* _temp746; _LL476: if((( struct _tunion_struct*) _temp474)->tag
== Cyc_Absyn_Const_e_tag){ _LL567: _temp566=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp474)->f1; if(( unsigned int) _temp566 > 1u?(( struct _tunion_struct*)
_temp566)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL571: _temp570=( void*)(( struct
Cyc_Absyn_Char_c_struct*) _temp566)->f1; goto _LL569; _LL569: _temp568=( char)((
struct Cyc_Absyn_Char_c_struct*) _temp566)->f2; goto _LL477;} else{ goto _LL478;}}
else{ goto _LL478;} _LL478: if((( struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_Const_e_tag){
_LL573: _temp572=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp474)->f1; if((
unsigned int) _temp572 > 1u?(( struct _tunion_struct*) _temp572)->tag == Cyc_Absyn_Short_c_tag:
0){ _LL577: _temp576=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp572)->f1;
goto _LL575; _LL575: _temp574=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp572)->f2; goto _LL479;} else{ goto _LL480;}} else{ goto _LL480;} _LL480:
if((( struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_Const_e_tag){ _LL579:
_temp578=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp474)->f1; if((
unsigned int) _temp578 > 1u?(( struct _tunion_struct*) _temp578)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL583: _temp582=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp578)->f1;
if(( int) _temp582 == Cyc_Absyn_Signed){ goto _LL581;} else{ goto _LL482;}
_LL581: _temp580=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp578)->f2; goto
_LL481;} else{ goto _LL482;}} else{ goto _LL482;} _LL482: if((( struct
_tunion_struct*) _temp474)->tag == Cyc_Absyn_Const_e_tag){ _LL585: _temp584=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp474)->f1; if(( unsigned int)
_temp584 > 1u?(( struct _tunion_struct*) _temp584)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL589: _temp588=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp584)->f1;
if(( int) _temp588 == Cyc_Absyn_Unsigned){ goto _LL587;} else{ goto _LL484;}
_LL587: _temp586=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp584)->f2; goto
_LL483;} else{ goto _LL484;}} else{ goto _LL484;} _LL484: if((( struct
_tunion_struct*) _temp474)->tag == Cyc_Absyn_Const_e_tag){ _LL591: _temp590=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp474)->f1; if(( unsigned int)
_temp590 > 1u?(( struct _tunion_struct*) _temp590)->tag == Cyc_Absyn_LongLong_c_tag:
0){ _LL595: _temp594=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp590)->f1;
goto _LL593; _LL593: _temp592=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp590)->f2; goto _LL485;} else{ goto _LL486;}} else{ goto _LL486;} _LL486:
if((( struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_Const_e_tag){ _LL597:
_temp596=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp474)->f1; if((
unsigned int) _temp596 > 1u?(( struct _tunion_struct*) _temp596)->tag == Cyc_Absyn_Float_c_tag:
0){ _LL599: _temp598=( struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*)
_temp596)->f1; goto _LL487;} else{ goto _LL488;}} else{ goto _LL488;} _LL488:
if((( struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_Const_e_tag){ _LL601:
_temp600=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp474)->f1; if(( int)
_temp600 == Cyc_Absyn_Null_c){ goto _LL489;} else{ goto _LL490;}} else{ goto
_LL490;} _LL490: if((( struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_Const_e_tag){
_LL603: _temp602=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp474)->f1; if((
unsigned int) _temp602 > 1u?(( struct _tunion_struct*) _temp602)->tag == Cyc_Absyn_String_c_tag:
0){ _LL605: _temp604=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp602)->f1; goto _LL491;} else{ goto _LL492;}} else{ goto _LL492;} _LL492:
if((( struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_UnknownId_e_tag){
_LL607: _temp606=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp474)->f1; goto _LL493;} else{ goto _LL494;} _LL494: if((( struct
_tunion_struct*) _temp474)->tag == Cyc_Absyn_Var_e_tag){ _LL611: _temp610=(
struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp474)->f1; goto _LL609;
_LL609: _temp608=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp474)->f2; goto
_LL495;} else{ goto _LL496;} _LL496: if((( struct _tunion_struct*) _temp474)->tag
== Cyc_Absyn_Primop_e_tag){ _LL615: _temp614=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp474)->f1; goto _LL613; _LL613: _temp612=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp474)->f2; goto _LL497;} else{ goto _LL498;}
_LL498: if((( struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL621: _temp620=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp474)->f1; goto _LL619; _LL619: _temp618=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp474)->f2; goto _LL617; _LL617: _temp616=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp474)->f3;
goto _LL499;} else{ goto _LL500;} _LL500: if((( struct _tunion_struct*) _temp474)->tag
== Cyc_Absyn_Increment_e_tag){ _LL625: _temp624=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp474)->f1; goto _LL623; _LL623:
_temp622=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp474)->f2; if((
int) _temp622 == Cyc_Absyn_PreInc){ goto _LL501;} else{ goto _LL502;}} else{
goto _LL502;} _LL502: if((( struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_Increment_e_tag){
_LL629: _temp628=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp474)->f1; goto _LL627; _LL627: _temp626=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp474)->f2; if(( int) _temp626 == Cyc_Absyn_PreDec){ goto _LL503;} else{ goto
_LL504;}} else{ goto _LL504;} _LL504: if((( struct _tunion_struct*) _temp474)->tag
== Cyc_Absyn_Increment_e_tag){ _LL633: _temp632=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp474)->f1; goto _LL631; _LL631:
_temp630=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp474)->f2; if((
int) _temp630 == Cyc_Absyn_PostInc){ goto _LL505;} else{ goto _LL506;}} else{
goto _LL506;} _LL506: if((( struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_Increment_e_tag){
_LL637: _temp636=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp474)->f1; goto _LL635; _LL635: _temp634=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp474)->f2; if(( int) _temp634 == Cyc_Absyn_PostDec){ goto _LL507;} else{
goto _LL508;}} else{ goto _LL508;} _LL508: if((( struct _tunion_struct*)
_temp474)->tag == Cyc_Absyn_Conditional_e_tag){ _LL643: _temp642=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp474)->f1; goto _LL641; _LL641:
_temp640=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp474)->f2; goto _LL639; _LL639: _temp638=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp474)->f3; goto _LL509;} else{ goto _LL510;}
_LL510: if((( struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_SeqExp_e_tag){
_LL647: _temp646=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp474)->f1; goto _LL645; _LL645: _temp644=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp474)->f2; goto _LL511;} else{ goto _LL512;}
_LL512: if((( struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL651: _temp650=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp474)->f1; goto _LL649; _LL649: _temp648=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_e_struct*) _temp474)->f2; goto _LL513;} else{ goto _LL514;}
_LL514: if((( struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_FnCall_e_tag){
_LL655: _temp654=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp474)->f1; goto _LL653; _LL653: _temp652=( struct Cyc_List_List*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp474)->f2; goto _LL515;} else{ goto _LL516;}
_LL516: if((( struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_Throw_e_tag){
_LL657: _temp656=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp474)->f1; goto _LL517;} else{ goto _LL518;} _LL518: if((( struct
_tunion_struct*) _temp474)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL659:
_temp658=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp474)->f1; goto _LL519;} else{ goto _LL520;} _LL520: if((( struct
_tunion_struct*) _temp474)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL663:
_temp662=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp474)->f1; goto _LL661; _LL661: _temp660=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp474)->f2; goto _LL521;} else{ goto _LL522;}
_LL522: if((( struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_Cast_e_tag){
_LL667: _temp666=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp474)->f1; goto
_LL665; _LL665: _temp664=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp474)->f2; goto _LL523;} else{ goto _LL524;} _LL524: if((( struct
_tunion_struct*) _temp474)->tag == Cyc_Absyn_Address_e_tag){ _LL669: _temp668=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp474)->f1; goto
_LL525;} else{ goto _LL526;} _LL526: if((( struct _tunion_struct*) _temp474)->tag
== Cyc_Absyn_New_e_tag){ _LL671: _temp670=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp474)->f1; goto _LL527;} else{ goto _LL528;} _LL528: if((( struct
_tunion_struct*) _temp474)->tag == Cyc_Absyn_Sizeoftyp_e_tag){ _LL673: _temp672=(
void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp474)->f1; goto _LL529;}
else{ goto _LL530;} _LL530: if((( struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_Sizeofexp_e_tag){
_LL675: _temp674=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp474)->f1; goto _LL531;} else{ goto _LL532;} _LL532: if((( struct
_tunion_struct*) _temp474)->tag == Cyc_Absyn_Deref_e_tag){ _LL677: _temp676=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp474)->f1; goto
_LL533;} else{ goto _LL534;} _LL534: if((( struct _tunion_struct*) _temp474)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL681: _temp680=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp474)->f1; goto _LL679; _LL679:
_temp678=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp474)->f2; goto _LL535;} else{ goto _LL536;} _LL536: if((( struct
_tunion_struct*) _temp474)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL685:
_temp684=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp474)->f1; goto _LL683; _LL683: _temp682=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp474)->f2; goto _LL537;} else{ goto _LL538;}
_LL538: if((( struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_Subscript_e_tag){
_LL689: _temp688=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp474)->f1; goto _LL687; _LL687: _temp686=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp474)->f2; goto _LL539;} else{ goto _LL540;}
_LL540: if((( struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_Tuple_e_tag){
_LL691: _temp690=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp474)->f1; goto _LL541;} else{ goto _LL542;} _LL542: if((( struct
_tunion_struct*) _temp474)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL695:
_temp694=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp474)->f1;
goto _LL693; _LL693: _temp692=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp474)->f2; goto _LL543;} else{ goto _LL544;} _LL544: if((( struct
_tunion_struct*) _temp474)->tag == Cyc_Absyn_Array_e_tag){ _LL697: _temp696=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp474)->f1; goto
_LL545;} else{ goto _LL546;} _LL546: if((( struct _tunion_struct*) _temp474)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL703: _temp702=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp474)->f1; goto _LL701; _LL701:
_temp700=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp474)->f2; goto _LL699; _LL699: _temp698=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp474)->f3; goto _LL547;} else{ goto
_LL548;} _LL548: if((( struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_Struct_e_tag){
_LL711: _temp710=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp474)->f1; goto _LL709; _LL709: _temp708=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp474)->f2; goto _LL707; _LL707: _temp706=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp474)->f3; goto
_LL705; _LL705: _temp704=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp474)->f4; goto _LL549;} else{ goto _LL550;} _LL550: if((( struct
_tunion_struct*) _temp474)->tag == Cyc_Absyn_Tunion_e_tag){ _LL721: _temp720=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp474)->f1; goto
_LL719; _LL719: _temp718=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp474)->f2; goto _LL717; _LL717: _temp716=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tunion_e_struct*) _temp474)->f3; goto _LL715; _LL715: _temp714=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp474)->f4;
goto _LL713; _LL713: _temp712=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp474)->f5; goto _LL551;} else{ goto _LL552;} _LL552: if((( struct
_tunion_struct*) _temp474)->tag == Cyc_Absyn_XTunion_e_tag){ _LL729: _temp728=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp474)->f1; goto
_LL727; _LL727: _temp726=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp474)->f2; goto _LL725; _LL725: _temp724=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp474)->f3; goto _LL723; _LL723: _temp722=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp474)->f4;
goto _LL553;} else{ goto _LL554;} _LL554: if((( struct _tunion_struct*) _temp474)->tag
== Cyc_Absyn_Enum_e_tag){ _LL735: _temp734=( struct _tuple0*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp474)->f1; goto _LL733; _LL733: _temp732=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp474)->f2; goto _LL731; _LL731: _temp730=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*) _temp474)->f3;
goto _LL555;} else{ goto _LL556;} _LL556: if((( struct _tunion_struct*) _temp474)->tag
== Cyc_Absyn_Malloc_e_tag){ _LL737: _temp736=( void*)(( struct Cyc_Absyn_Malloc_e_struct*)
_temp474)->f1; goto _LL557;} else{ goto _LL558;} _LL558: if((( struct
_tunion_struct*) _temp474)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL741:
_temp740=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp474)->f1; goto _LL739; _LL739: _temp738=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp474)->f2; goto _LL559;} else{ goto
_LL560;} _LL560: if((( struct _tunion_struct*) _temp474)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL743: _temp742=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp474)->f1; goto _LL561;} else{ goto _LL562;} _LL562: if((( struct
_tunion_struct*) _temp474)->tag == Cyc_Absyn_Codegen_e_tag){ _LL745: _temp744=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp474)->f1;
goto _LL563;} else{ goto _LL564;} _LL564: if((( struct _tunion_struct*) _temp474)->tag
== Cyc_Absyn_Fill_e_tag){ _LL747: _temp746=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp474)->f1; goto _LL565;} else{ goto _LL475;} _LL477: Cyc_Absyndump_dump_char((
int)'\''); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp568)); Cyc_Absyndump_dump_char((
int)'\''); goto _LL475; _LL479: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp748=( char*)"(short)"; struct _tagged_string _temp749; _temp749.curr=
_temp748; _temp749.base= _temp748; _temp749.last_plus_one= _temp748 + 8;
_temp749;})); Cyc_Absyndump_dump_nospace( Cyc_Core_string_of_int(( int) _temp574));
goto _LL475; _LL481: Cyc_Absyndump_dump( Cyc_Core_string_of_int( _temp580));
goto _LL475; _LL483: Cyc_Absyndump_dump( Cyc_Core_string_of_int( _temp586)); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp750=( char*)"u"; struct _tagged_string
_temp751; _temp751.curr= _temp750; _temp751.base= _temp750; _temp751.last_plus_one=
_temp750 + 2; _temp751;})); goto _LL475; _LL485: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp752=( char*)"<<FIX LONG LONG CONSTANT>>"; struct
_tagged_string _temp753; _temp753.curr= _temp752; _temp753.base= _temp752;
_temp753.last_plus_one= _temp752 + 27; _temp753;})); goto _LL475; _LL487: Cyc_Absyndump_dump(
_temp598); goto _LL475; _LL489: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp754=( char*)"null"; struct _tagged_string _temp755; _temp755.curr=
_temp754; _temp755.base= _temp754; _temp755.last_plus_one= _temp754 + 5;
_temp755;})); goto _LL475; _LL491: Cyc_Absyndump_dump_char(( int)'"'); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_string_escape( _temp604)); Cyc_Absyndump_dump_char(( int)'"'); goto
_LL475; _LL493: _temp610= _temp606; goto _LL495; _LL495: Cyc_Absyndump_dumpqvar(
_temp610); goto _LL475; _LL497: { struct _tagged_string pstr= Cyc_Absynpp_prim2str(
_temp614); if( ! Cyc_Absyn_is_format_prim( _temp614)){ switch((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp612)){ case 1: _LL756: if( _temp614 ==(
void*) Cyc_Absyn_Size){ Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)
_temp612->hd); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp758=(
char*)".size"; struct _tagged_string _temp759; _temp759.curr= _temp758; _temp759.base=
_temp758; _temp759.last_plus_one= _temp758 + 6; _temp759;}));} else{ Cyc_Absyndump_dump(
pstr); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*) _temp612->hd);}
break; case 2: _LL757: Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)
_temp612->hd); Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dumpexp_prec( myprec,(
struct Cyc_Absyn_Exp*)( _temp612->tl)->hd); break; default: _LL760:( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Failure_struct* _temp762=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp762[ 0]=({ struct Cyc_Core_Failure_struct _temp763; _temp763.tag= Cyc_Core_Failure_tag;
_temp763.f1=( struct _tagged_string)({ char* _temp764=( char*)"Absyndump -- Bad number of arguments to primop";
struct _tagged_string _temp765; _temp765.curr= _temp764; _temp765.base= _temp764;
_temp765.last_plus_one= _temp764 + 47; _temp765;}); _temp763;}); _temp762;}));}}
else{ Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp766=( char*)"("; struct _tagged_string _temp767;
_temp767.curr= _temp766; _temp767.base= _temp766; _temp767.last_plus_one=
_temp766 + 2; _temp767;})); Cyc_Absyndump_dumpexps_prec( 20, _temp612); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp768=( char*)")"; struct _tagged_string
_temp769; _temp769.curr= _temp768; _temp769.base= _temp768; _temp769.last_plus_one=
_temp768 + 2; _temp769;}));} goto _LL475;} _LL499: Cyc_Absyndump_dumpexp_prec(
myprec, _temp620); if( _temp618 != 0){ Cyc_Absyndump_dump( Cyc_Absynpp_prim2str((
void*) _temp618->v));} Cyc_Absyndump_dump_nospace(( struct _tagged_string)({
char* _temp770=( char*)"="; struct _tagged_string _temp771; _temp771.curr=
_temp770; _temp771.base= _temp770; _temp771.last_plus_one= _temp770 + 2;
_temp771;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp616); goto _LL475; _LL501:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp772=( char*)"++";
struct _tagged_string _temp773; _temp773.curr= _temp772; _temp773.base= _temp772;
_temp773.last_plus_one= _temp772 + 3; _temp773;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp624); goto _LL475; _LL503: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp774=( char*)"--"; struct _tagged_string _temp775;
_temp775.curr= _temp774; _temp775.base= _temp774; _temp775.last_plus_one=
_temp774 + 3; _temp775;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp628); goto
_LL475; _LL505: Cyc_Absyndump_dumpexp_prec( myprec, _temp632); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp776=( char*)"++"; struct _tagged_string
_temp777; _temp777.curr= _temp776; _temp777.base= _temp776; _temp777.last_plus_one=
_temp776 + 3; _temp777;})); goto _LL475; _LL507: Cyc_Absyndump_dumpexp_prec(
myprec, _temp636); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp778=(
char*)"--"; struct _tagged_string _temp779; _temp779.curr= _temp778; _temp779.base=
_temp778; _temp779.last_plus_one= _temp778 + 3; _temp779;})); goto _LL475;
_LL509: Cyc_Absyndump_dumpexp_prec( myprec, _temp642); Cyc_Absyndump_dump_char((
int)'?'); Cyc_Absyndump_dumpexp_prec( 0, _temp640); Cyc_Absyndump_dump_char((
int)':'); Cyc_Absyndump_dumpexp_prec( myprec, _temp638); goto _LL475; _LL511:
Cyc_Absyndump_dumpexp_prec( myprec, _temp646); Cyc_Absyndump_dump_char(( int)',');
Cyc_Absyndump_dumpexp_prec( myprec, _temp644); goto _LL475; _LL513: _temp654=
_temp650; _temp652= _temp648; goto _LL515; _LL515: Cyc_Absyndump_dumpexp_prec(
myprec, _temp654); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp780=( char*)"("; struct _tagged_string _temp781; _temp781.curr= _temp780;
_temp781.base= _temp780; _temp781.last_plus_one= _temp780 + 2; _temp781;})); Cyc_Absyndump_dumpexps_prec(
20, _temp652); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp782=( char*)")"; struct _tagged_string _temp783; _temp783.curr= _temp782;
_temp783.base= _temp782; _temp783.last_plus_one= _temp782 + 2; _temp783;}));
goto _LL475; _LL517: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp784=( char*)"throw"; struct _tagged_string _temp785; _temp785.curr=
_temp784; _temp785.base= _temp784; _temp785.last_plus_one= _temp784 + 6;
_temp785;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp656); goto _LL475; _LL519:
_temp662= _temp658; goto _LL521; _LL521: Cyc_Absyndump_dumpexp_prec( inprec,
_temp662); goto _LL475; _LL523: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp(
_temp666); Cyc_Absyndump_dump_char(( int)')'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp664); goto _LL475; _LL525: Cyc_Absyndump_dump_char(( int)'&'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp668); goto _LL475; _LL527: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp786=( char*)"new "; struct _tagged_string _temp787;
_temp787.curr= _temp786; _temp787.base= _temp786; _temp787.last_plus_one=
_temp786 + 5; _temp787;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp670); goto
_LL475; _LL529: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp788=(
char*)"sizeof("; struct _tagged_string _temp789; _temp789.curr= _temp788;
_temp789.base= _temp788; _temp789.last_plus_one= _temp788 + 8; _temp789;})); Cyc_Absyndump_dumptyp(
_temp672); Cyc_Absyndump_dump_char(( int)')'); goto _LL475; _LL531: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp790=( char*)"sizeof("; struct _tagged_string
_temp791; _temp791.curr= _temp790; _temp791.base= _temp790; _temp791.last_plus_one=
_temp790 + 8; _temp791;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp674); Cyc_Absyndump_dump_char((
int)')'); goto _LL475; _LL533: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp676); goto _LL475; _LL535: Cyc_Absyndump_dumpexp_prec( myprec,
_temp680); Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp678); goto _LL475; _LL537: Cyc_Absyndump_dumpexp_prec( myprec, _temp684);
Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp792=( char*)"->";
struct _tagged_string _temp793; _temp793.curr= _temp792; _temp793.base= _temp792;
_temp793.last_plus_one= _temp792 + 3; _temp793;})); Cyc_Absyndump_dump_nospace(*
_temp682); goto _LL475; _LL539: Cyc_Absyndump_dumpexp_prec( myprec, _temp688);
Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp( _temp686); Cyc_Absyndump_dump_char((
int)']'); goto _LL475; _LL541: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp794=( char*)"$("; struct _tagged_string _temp795; _temp795.curr=
_temp794; _temp795.base= _temp794; _temp795.last_plus_one= _temp794 + 3;
_temp795;})); Cyc_Absyndump_dumpexps_prec( 20, _temp690); Cyc_Absyndump_dump_char((
int)')'); goto _LL475; _LL543: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp((*
_temp694).f3); Cyc_Absyndump_dump_char(( int)')');(( void(*)( void(* f)( struct
_tuple7*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp692,( struct _tagged_string)({ char* _temp796=( char*)"{"; struct
_tagged_string _temp797; _temp797.curr= _temp796; _temp797.base= _temp796;
_temp797.last_plus_one= _temp796 + 2; _temp797;}),( struct _tagged_string)({
char* _temp798=( char*)"}"; struct _tagged_string _temp799; _temp799.curr=
_temp798; _temp799.base= _temp798; _temp799.last_plus_one= _temp798 + 2;
_temp799;}),( struct _tagged_string)({ char* _temp800=( char*)","; struct
_tagged_string _temp801; _temp801.curr= _temp800; _temp801.base= _temp800;
_temp801.last_plus_one= _temp800 + 2; _temp801;})); goto _LL475; _LL545:(( void(*)(
void(* f)( struct _tuple7*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp696,( struct _tagged_string)({ char* _temp802=( char*)"{";
struct _tagged_string _temp803; _temp803.curr= _temp802; _temp803.base= _temp802;
_temp803.last_plus_one= _temp802 + 2; _temp803;}),( struct _tagged_string)({
char* _temp804=( char*)"}"; struct _tagged_string _temp805; _temp805.curr=
_temp804; _temp805.base= _temp804; _temp805.last_plus_one= _temp804 + 2;
_temp805;}),( struct _tagged_string)({ char* _temp806=( char*)","; struct
_tagged_string _temp807; _temp807.curr= _temp806; _temp807.base= _temp806;
_temp807.last_plus_one= _temp806 + 2; _temp807;})); goto _LL475; _LL547: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp808=( char*)"new {for"; struct
_tagged_string _temp809; _temp809.curr= _temp808; _temp809.base= _temp808;
_temp809.last_plus_one= _temp808 + 9; _temp809;})); Cyc_Absyndump_dump_str((*
_temp702->name).f2); Cyc_Absyndump_dump_char(( int)'<'); Cyc_Absyndump_dumpexp(
_temp700); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp( _temp698);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL475; _LL549: Cyc_Absyndump_dumpqvar(
_temp710);(( void(*)( void(* f)( struct _tuple7*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde, _temp706,( struct
_tagged_string)({ char* _temp810=( char*)"{"; struct _tagged_string _temp811;
_temp811.curr= _temp810; _temp811.base= _temp810; _temp811.last_plus_one=
_temp810 + 2; _temp811;}),( struct _tagged_string)({ char* _temp812=( char*)"}";
struct _tagged_string _temp813; _temp813.curr= _temp812; _temp813.base= _temp812;
_temp813.last_plus_one= _temp812 + 2; _temp813;}),( struct _tagged_string)({
char* _temp814=( char*)","; struct _tagged_string _temp815; _temp815.curr=
_temp814; _temp815.base= _temp814; _temp815.last_plus_one= _temp814 + 2;
_temp815;})); goto _LL475; _LL551: _temp726= _temp716; _temp722= _temp712; goto
_LL553; _LL553: Cyc_Absyndump_dumpqvar( _temp722->name); if( _temp726 != 0){((
void(*)( void(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp, _temp726,( struct _tagged_string)({ char* _temp816=( char*)"(";
struct _tagged_string _temp817; _temp817.curr= _temp816; _temp817.base= _temp816;
_temp817.last_plus_one= _temp816 + 2; _temp817;}),( struct _tagged_string)({
char* _temp818=( char*)")"; struct _tagged_string _temp819; _temp819.curr=
_temp818; _temp819.base= _temp818; _temp819.last_plus_one= _temp818 + 2;
_temp819;}),( struct _tagged_string)({ char* _temp820=( char*)","; struct
_tagged_string _temp821; _temp821.curr= _temp820; _temp821.base= _temp820;
_temp821.last_plus_one= _temp820 + 2; _temp821;}));} goto _LL475; _LL555: Cyc_Absyndump_dumpqvar(
_temp734); goto _LL475; _LL557: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp822=( char*)"malloc(sizeof("; struct _tagged_string _temp823;
_temp823.curr= _temp822; _temp823.base= _temp822; _temp823.last_plus_one=
_temp822 + 15; _temp823;})); Cyc_Absyndump_dumptyp( _temp736); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp824=( char*)"))"; struct _tagged_string
_temp825; _temp825.curr= _temp824; _temp825.base= _temp824; _temp825.last_plus_one=
_temp824 + 3; _temp825;})); goto _LL475; _LL559:(( void(*)( void(* f)( struct
_tuple7*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp738,( struct _tagged_string)({ char* _temp826=( char*)"{"; struct
_tagged_string _temp827; _temp827.curr= _temp826; _temp827.base= _temp826;
_temp827.last_plus_one= _temp826 + 2; _temp827;}),( struct _tagged_string)({
char* _temp828=( char*)"}"; struct _tagged_string _temp829; _temp829.curr=
_temp828; _temp829.base= _temp828; _temp829.last_plus_one= _temp828 + 2;
_temp829;}),( struct _tagged_string)({ char* _temp830=( char*)","; struct
_tagged_string _temp831; _temp831.curr= _temp830; _temp831.base= _temp830;
_temp831.last_plus_one= _temp830 + 2; _temp831;})); goto _LL475; _LL561: Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp832=( char*)"({"; struct _tagged_string
_temp833; _temp833.curr= _temp832; _temp833.base= _temp832; _temp833.last_plus_one=
_temp832 + 3; _temp833;})); Cyc_Absyndump_dumpstmt( _temp742); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp834=( char*)"})"; struct _tagged_string
_temp835; _temp835.curr= _temp834; _temp835.base= _temp834; _temp835.last_plus_one=
_temp834 + 3; _temp835;})); goto _LL475; _LL563: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp836=( char*)"codegen("; struct _tagged_string
_temp837; _temp837.curr= _temp836; _temp837.base= _temp836; _temp837.last_plus_one=
_temp836 + 9; _temp837;})); Cyc_Absyndump_dumpdecl(({ struct Cyc_Absyn_Decl*
_temp838=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp838->r=( void*)(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp839=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp839[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp840; _temp840.tag= Cyc_Absyn_Fn_d_tag;
_temp840.f1= _temp744; _temp840;}); _temp839;})); _temp838->loc= e->loc;
_temp838;})); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp841=(
char*)")"; struct _tagged_string _temp842; _temp842.curr= _temp841; _temp842.base=
_temp841; _temp842.last_plus_one= _temp841 + 2; _temp842;})); goto _LL475;
_LL565: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp843=( char*)"fill(";
struct _tagged_string _temp844; _temp844.curr= _temp843; _temp844.base= _temp843;
_temp844.last_plus_one= _temp843 + 6; _temp844;})); Cyc_Absyndump_dumpexp(
_temp746); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp845=( char*)")";
struct _tagged_string _temp846; _temp846.curr= _temp845; _temp846.base= _temp845;
_temp846.last_plus_one= _temp845 + 2; _temp846;})); goto _LL475; _LL475:;} if(
inprec >= myprec){ Cyc_Absyndump_dump_char(( int)')');}} void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp* e){ Cyc_Absyndump_dumpexp_prec( 0, e);} void Cyc_Absyndump_dumpswitchclauses(
struct Cyc_List_List* scs){ for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause*
c=( struct Cyc_Absyn_Switch_clause*) scs->hd; if( c->where_clause == 0?( void*)(
c->pattern)->r ==( void*) Cyc_Absyn_Wild_p: 0){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp847=( char*)"default:"; struct _tagged_string
_temp848; _temp848.curr= _temp847; _temp848.base= _temp847; _temp848.last_plus_one=
_temp847 + 9; _temp848;}));} else{ Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp849=( char*)"case"; struct _tagged_string _temp850; _temp850.curr=
_temp849; _temp850.base= _temp849; _temp850.last_plus_one= _temp849 + 5;
_temp850;})); Cyc_Absyndump_dumppat( c->pattern); if( c->where_clause != 0){ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp851=( char*)"&&"; struct _tagged_string
_temp852; _temp852.curr= _temp851; _temp852.base= _temp851; _temp852.last_plus_one=
_temp851 + 3; _temp852;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp853= c->where_clause; if( _temp853 == 0){ _throw(
Null_Exception);} _temp853;}));} Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp854=( char*)":"; struct _tagged_string _temp855;
_temp855.curr= _temp854; _temp855.base= _temp854; _temp855.last_plus_one=
_temp854 + 2; _temp855;}));} Cyc_Absyndump_dumpstmt( c->body);}} void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt* s){ void* _temp856=( void*) s->r; struct Cyc_Absyn_Exp*
_temp898; struct Cyc_Absyn_Stmt* _temp900; struct Cyc_Absyn_Stmt* _temp902;
struct Cyc_Absyn_Exp* _temp904; struct Cyc_Absyn_Exp* _temp906; struct Cyc_Absyn_Stmt*
_temp908; struct Cyc_Absyn_Stmt* _temp910; struct Cyc_Absyn_Exp* _temp912;
struct Cyc_Absyn_Stmt* _temp914; struct _tuple2 _temp916; struct Cyc_Absyn_Stmt*
_temp918; struct Cyc_Absyn_Exp* _temp920; struct Cyc_Absyn_Stmt* _temp922;
struct Cyc_Absyn_Stmt* _temp924; struct Cyc_Absyn_Stmt* _temp926; struct
_tagged_string* _temp928; struct Cyc_Absyn_Switch_clause** _temp930; struct Cyc_List_List*
_temp932; struct Cyc_Absyn_Switch_clause** _temp934; struct Cyc_List_List*
_temp936; struct Cyc_Absyn_Stmt* _temp938; struct _tuple2 _temp940; struct Cyc_Absyn_Stmt*
_temp942; struct Cyc_Absyn_Exp* _temp944; struct _tuple2 _temp946; struct Cyc_Absyn_Stmt*
_temp948; struct Cyc_Absyn_Exp* _temp950; struct Cyc_Absyn_Exp* _temp952; struct
Cyc_List_List* _temp954; struct Cyc_Absyn_Exp* _temp956; struct Cyc_Absyn_Stmt*
_temp958; struct Cyc_Absyn_Decl* _temp960; struct Cyc_Absyn_Stmt* _temp962;
struct Cyc_Absyn_Stmt* _temp964; struct Cyc_Absyn_Stmt* _temp966; struct
_tagged_string* _temp968; struct _tuple2 _temp970; struct Cyc_Absyn_Stmt*
_temp972; struct Cyc_Absyn_Exp* _temp974; struct Cyc_Absyn_Stmt* _temp976;
struct Cyc_List_List* _temp978; struct Cyc_Absyn_Stmt* _temp980; _LL858: if((
int) _temp856 == Cyc_Absyn_Skip_s){ goto _LL859;} else{ goto _LL860;} _LL860:
if(( unsigned int) _temp856 > 1u?(( struct _tunion_struct*) _temp856)->tag ==
Cyc_Absyn_Exp_s_tag: 0){ _LL899: _temp898=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp856)->f1; goto _LL861;} else{ goto _LL862;} _LL862: if(( unsigned int)
_temp856 > 1u?(( struct _tunion_struct*) _temp856)->tag == Cyc_Absyn_Seq_s_tag:
0){ _LL903: _temp902=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp856)->f1; goto _LL901; _LL901: _temp900=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Seq_s_struct*) _temp856)->f2; goto _LL863;} else{ goto _LL864;} _LL864:
if(( unsigned int) _temp856 > 1u?(( struct _tunion_struct*) _temp856)->tag ==
Cyc_Absyn_Return_s_tag: 0){ _LL905: _temp904=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Return_s_struct*) _temp856)->f1; if( _temp904 == 0){ goto _LL865;}
else{ goto _LL866;}} else{ goto _LL866;} _LL866: if(( unsigned int) _temp856 > 1u?((
struct _tunion_struct*) _temp856)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL907:
_temp906=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*) _temp856)->f1;
goto _LL867;} else{ goto _LL868;} _LL868: if(( unsigned int) _temp856 > 1u?((
struct _tunion_struct*) _temp856)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL913:
_temp912=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp856)->f1; goto _LL911; _LL911: _temp910=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp856)->f2; goto _LL909; _LL909: _temp908=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp856)->f3;
goto _LL869;} else{ goto _LL870;} _LL870: if(( unsigned int) _temp856 > 1u?((
struct _tunion_struct*) _temp856)->tag == Cyc_Absyn_While_s_tag: 0){ _LL917:
_temp916=( struct _tuple2)(( struct Cyc_Absyn_While_s_struct*) _temp856)->f1;
_LL921: _temp920= _temp916.f1; goto _LL919; _LL919: _temp918= _temp916.f2; goto
_LL915; _LL915: _temp914=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp856)->f2; goto _LL871;} else{ goto _LL872;} _LL872: if(( unsigned int)
_temp856 > 1u?(( struct _tunion_struct*) _temp856)->tag == Cyc_Absyn_Break_s_tag:
0){ _LL923: _temp922=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp856)->f1; goto _LL873;} else{ goto _LL874;} _LL874: if(( unsigned int)
_temp856 > 1u?(( struct _tunion_struct*) _temp856)->tag == Cyc_Absyn_Continue_s_tag:
0){ _LL925: _temp924=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp856)->f1; goto _LL875;} else{ goto _LL876;} _LL876: if(( unsigned int)
_temp856 > 1u?(( struct _tunion_struct*) _temp856)->tag == Cyc_Absyn_Goto_s_tag:
0){ _LL929: _temp928=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp856)->f1; goto _LL927; _LL927: _temp926=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Goto_s_struct*) _temp856)->f2; goto _LL877;} else{ goto _LL878;}
_LL878: if(( unsigned int) _temp856 > 1u?(( struct _tunion_struct*) _temp856)->tag
== Cyc_Absyn_Fallthru_s_tag: 0){ _LL933: _temp932=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp856)->f1; if( _temp932 == 0){ goto
_LL931;} else{ goto _LL880;} _LL931: _temp930=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp856)->f2; goto _LL879;} else{ goto
_LL880;} _LL880: if(( unsigned int) _temp856 > 1u?(( struct _tunion_struct*)
_temp856)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL937: _temp936=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp856)->f1; goto _LL935; _LL935:
_temp934=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp856)->f2; goto _LL881;} else{ goto _LL882;} _LL882: if(( unsigned int)
_temp856 > 1u?(( struct _tunion_struct*) _temp856)->tag == Cyc_Absyn_For_s_tag:
0){ _LL953: _temp952=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp856)->f1; goto _LL947; _LL947: _temp946=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp856)->f2; _LL951: _temp950= _temp946.f1; goto _LL949; _LL949: _temp948=
_temp946.f2; goto _LL941; _LL941: _temp940=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp856)->f3; _LL945: _temp944= _temp940.f1; goto _LL943; _LL943: _temp942=
_temp940.f2; goto _LL939; _LL939: _temp938=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_For_s_struct*) _temp856)->f4; goto _LL883;} else{ goto _LL884;} _LL884:
if(( unsigned int) _temp856 > 1u?(( struct _tunion_struct*) _temp856)->tag ==
Cyc_Absyn_Switch_s_tag: 0){ _LL957: _temp956=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Switch_s_struct*) _temp856)->f1; goto _LL955; _LL955: _temp954=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*) _temp856)->f2; goto
_LL885;} else{ goto _LL886;} _LL886: if(( unsigned int) _temp856 > 1u?(( struct
_tunion_struct*) _temp856)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL961: _temp960=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp856)->f1; goto
_LL959; _LL959: _temp958=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp856)->f2; goto _LL887;} else{ goto _LL888;} _LL888: if(( unsigned int)
_temp856 > 1u?(( struct _tunion_struct*) _temp856)->tag == Cyc_Absyn_Cut_s_tag:
0){ _LL963: _temp962=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*)
_temp856)->f1; goto _LL889;} else{ goto _LL890;} _LL890: if(( unsigned int)
_temp856 > 1u?(( struct _tunion_struct*) _temp856)->tag == Cyc_Absyn_Splice_s_tag:
0){ _LL965: _temp964=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*)
_temp856)->f1; goto _LL891;} else{ goto _LL892;} _LL892: if(( unsigned int)
_temp856 > 1u?(( struct _tunion_struct*) _temp856)->tag == Cyc_Absyn_Label_s_tag:
0){ _LL969: _temp968=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp856)->f1; goto _LL967; _LL967: _temp966=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Label_s_struct*) _temp856)->f2; goto _LL893;} else{ goto _LL894;}
_LL894: if(( unsigned int) _temp856 > 1u?(( struct _tunion_struct*) _temp856)->tag
== Cyc_Absyn_Do_s_tag: 0){ _LL977: _temp976=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Do_s_struct*) _temp856)->f1; goto _LL971; _LL971: _temp970=( struct
_tuple2)(( struct Cyc_Absyn_Do_s_struct*) _temp856)->f2; _LL975: _temp974=
_temp970.f1; goto _LL973; _LL973: _temp972= _temp970.f2; goto _LL895;} else{
goto _LL896;} _LL896: if(( unsigned int) _temp856 > 1u?(( struct _tunion_struct*)
_temp856)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL981: _temp980=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp856)->f1; goto _LL979; _LL979:
_temp978=( struct Cyc_List_List*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp856)->f2; goto _LL897;} else{ goto _LL857;} _LL859: Cyc_Absyndump_dump_semi();
goto _LL857; _LL861: Cyc_Absyndump_dumpexp( _temp898); Cyc_Absyndump_dump_semi();
goto _LL857; _LL863: if( Cyc_Absynpp_is_declaration( _temp902)){ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstmt( _temp902); Cyc_Absyndump_dump_char(( int)'}');}
else{ Cyc_Absyndump_dumpstmt( _temp902);} if( Cyc_Absynpp_is_declaration(
_temp900)){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt( _temp900);
Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dumpstmt( _temp900);}
goto _LL857; _LL865: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp982=( char*)"return;"; struct _tagged_string _temp983; _temp983.curr=
_temp982; _temp983.base= _temp982; _temp983.last_plus_one= _temp982 + 8;
_temp983;})); goto _LL857; _LL867: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp984=( char*)"return"; struct _tagged_string _temp985; _temp985.curr=
_temp984; _temp985.base= _temp984; _temp985.last_plus_one= _temp984 + 7;
_temp985;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp986= _temp906; if( _temp986 == 0){ _throw( Null_Exception);} _temp986;}));
Cyc_Absyndump_dump_semi(); goto _LL857; _LL869: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp987=( char*)"if("; struct _tagged_string _temp988;
_temp988.curr= _temp987; _temp988.base= _temp987; _temp988.last_plus_one=
_temp987 + 4; _temp988;})); Cyc_Absyndump_dumpexp( _temp912); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp989=( char*)"){"; struct _tagged_string
_temp990; _temp990.curr= _temp989; _temp990.base= _temp989; _temp990.last_plus_one=
_temp989 + 3; _temp990;})); Cyc_Absyndump_dumpstmt( _temp910); Cyc_Absyndump_dump_char((
int)'}');{ void* _temp991=( void*) _temp908->r; _LL993: if(( int) _temp991 ==
Cyc_Absyn_Skip_s){ goto _LL994;} else{ goto _LL995;} _LL995: goto _LL996; _LL994:
goto _LL992; _LL996: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp997=( char*)"else{"; struct _tagged_string _temp998; _temp998.curr=
_temp997; _temp998.base= _temp997; _temp998.last_plus_one= _temp997 + 6;
_temp998;})); Cyc_Absyndump_dumpstmt( _temp908); Cyc_Absyndump_dump_char(( int)'}');
goto _LL992; _LL992:;} goto _LL857; _LL871: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp999=( char*)"while("; struct _tagged_string
_temp1000; _temp1000.curr= _temp999; _temp1000.base= _temp999; _temp1000.last_plus_one=
_temp999 + 7; _temp1000;})); Cyc_Absyndump_dumpexp( _temp920); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1001=( char*)") {"; struct _tagged_string
_temp1002; _temp1002.curr= _temp1001; _temp1002.base= _temp1001; _temp1002.last_plus_one=
_temp1001 + 4; _temp1002;})); Cyc_Absyndump_dumpstmt( _temp914); Cyc_Absyndump_dump_char((
int)'}'); goto _LL857; _LL873: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1003=( char*)"break;"; struct _tagged_string _temp1004; _temp1004.curr=
_temp1003; _temp1004.base= _temp1003; _temp1004.last_plus_one= _temp1003 + 7;
_temp1004;})); goto _LL857; _LL875: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1005=( char*)"continue;"; struct _tagged_string _temp1006; _temp1006.curr=
_temp1005; _temp1006.base= _temp1005; _temp1006.last_plus_one= _temp1005 + 10;
_temp1006;})); goto _LL857; _LL877: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1007=( char*)"goto"; struct _tagged_string _temp1008; _temp1008.curr=
_temp1007; _temp1008.base= _temp1007; _temp1008.last_plus_one= _temp1007 + 5;
_temp1008;})); Cyc_Absyndump_dump_str( _temp928); Cyc_Absyndump_dump_semi();
goto _LL857; _LL879: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1009=( char*)"fallthru;"; struct _tagged_string _temp1010; _temp1010.curr=
_temp1009; _temp1010.base= _temp1009; _temp1010.last_plus_one= _temp1009 + 10;
_temp1010;})); goto _LL857; _LL881: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1011=( char*)"fallthru("; struct _tagged_string _temp1012; _temp1012.curr=
_temp1011; _temp1012.base= _temp1011; _temp1012.last_plus_one= _temp1011 + 10;
_temp1012;})); Cyc_Absyndump_dumpexps_prec( 20, _temp936); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1013=( char*)");"; struct _tagged_string
_temp1014; _temp1014.curr= _temp1013; _temp1014.base= _temp1013; _temp1014.last_plus_one=
_temp1013 + 3; _temp1014;})); goto _LL857; _LL883: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1015=( char*)"for("; struct _tagged_string
_temp1016; _temp1016.curr= _temp1015; _temp1016.base= _temp1015; _temp1016.last_plus_one=
_temp1015 + 5; _temp1016;})); Cyc_Absyndump_dumpexp( _temp952); Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp( _temp950); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp(
_temp944); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp1017=(
char*)"){"; struct _tagged_string _temp1018; _temp1018.curr= _temp1017;
_temp1018.base= _temp1017; _temp1018.last_plus_one= _temp1017 + 3; _temp1018;}));
Cyc_Absyndump_dumpstmt( _temp938); Cyc_Absyndump_dump_char(( int)'}'); goto
_LL857; _LL885: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1019=(
char*)"switch("; struct _tagged_string _temp1020; _temp1020.curr= _temp1019;
_temp1020.base= _temp1019; _temp1020.last_plus_one= _temp1019 + 8; _temp1020;}));
Cyc_Absyndump_dumpexp( _temp956); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp1021=( char*)"){"; struct _tagged_string _temp1022;
_temp1022.curr= _temp1021; _temp1022.base= _temp1021; _temp1022.last_plus_one=
_temp1021 + 3; _temp1022;})); Cyc_Absyndump_dumpswitchclauses( _temp954); Cyc_Absyndump_dump_char((
int)'}'); goto _LL857; _LL887: Cyc_Absyndump_dumpdecl( _temp960); Cyc_Absyndump_dumpstmt(
_temp958); goto _LL857; _LL889: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1023=( char*)"cut"; struct _tagged_string _temp1024; _temp1024.curr=
_temp1023; _temp1024.base= _temp1023; _temp1024.last_plus_one= _temp1023 + 4;
_temp1024;})); Cyc_Absyndump_dumpstmt( _temp962); goto _LL857; _LL891: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1025=( char*)"splice"; struct _tagged_string
_temp1026; _temp1026.curr= _temp1025; _temp1026.base= _temp1025; _temp1026.last_plus_one=
_temp1025 + 7; _temp1026;})); Cyc_Absyndump_dumpstmt( _temp964); goto _LL857;
_LL893: if( Cyc_Absynpp_is_declaration( _temp966)){ Cyc_Absyndump_dump_str(
_temp968); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp1027=(
char*)": {"; struct _tagged_string _temp1028; _temp1028.curr= _temp1027;
_temp1028.base= _temp1027; _temp1028.last_plus_one= _temp1027 + 4; _temp1028;}));
Cyc_Absyndump_dumpstmt( _temp966); Cyc_Absyndump_dump_char(( int)'}');} else{
Cyc_Absyndump_dump_str( _temp968); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpstmt(
_temp966);} goto _LL857; _LL895: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1029=( char*)"do {"; struct _tagged_string _temp1030; _temp1030.curr=
_temp1029; _temp1030.base= _temp1029; _temp1030.last_plus_one= _temp1029 + 5;
_temp1030;})); Cyc_Absyndump_dumpstmt( _temp976); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1031=( char*)"} while ("; struct
_tagged_string _temp1032; _temp1032.curr= _temp1031; _temp1032.base= _temp1031;
_temp1032.last_plus_one= _temp1031 + 10; _temp1032;})); Cyc_Absyndump_dumpexp(
_temp974); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp1033=(
char*)");"; struct _tagged_string _temp1034; _temp1034.curr= _temp1033;
_temp1034.base= _temp1033; _temp1034.last_plus_one= _temp1033 + 3; _temp1034;}));
goto _LL857; _LL897: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1035=( char*)"try"; struct _tagged_string _temp1036; _temp1036.curr=
_temp1035; _temp1036.base= _temp1035; _temp1036.last_plus_one= _temp1035 + 4;
_temp1036;})); Cyc_Absyndump_dumpstmt( _temp980); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1037=( char*)"catch {"; struct _tagged_string
_temp1038; _temp1038.curr= _temp1037; _temp1038.base= _temp1037; _temp1038.last_plus_one=
_temp1037 + 8; _temp1038;})); Cyc_Absyndump_dumpswitchclauses( _temp978); Cyc_Absyndump_dump_char((
int)'}'); goto _LL857; _LL857:;} void Cyc_Absyndump_dumpdp( struct _tuple8* dp){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* dp).f1,( struct _tagged_string)({ char*
_temp1039=( char*)""; struct _tagged_string _temp1040; _temp1040.curr= _temp1039;
_temp1040.base= _temp1039; _temp1040.last_plus_one= _temp1039 + 1; _temp1040;}),(
struct _tagged_string)({ char* _temp1041=( char*)"="; struct _tagged_string
_temp1042; _temp1042.curr= _temp1041; _temp1042.base= _temp1041; _temp1042.last_plus_one=
_temp1041 + 2; _temp1042;}),( struct _tagged_string)({ char* _temp1043=( char*)"=";
struct _tagged_string _temp1044; _temp1044.curr= _temp1043; _temp1044.base=
_temp1043; _temp1044.last_plus_one= _temp1043 + 2; _temp1044;})); Cyc_Absyndump_dumppat((*
dp).f2);} void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat* p){ void* _temp1045=(
void*) p->r; int _temp1081; void* _temp1083; int _temp1085; void* _temp1087;
char _temp1089; struct _tagged_string _temp1091; struct Cyc_Absyn_Vardecl*
_temp1093; struct Cyc_List_List* _temp1095; struct Cyc_Absyn_Pat* _temp1097;
struct Cyc_Absyn_Vardecl* _temp1099; struct _tuple0* _temp1101; struct Cyc_List_List*
_temp1103; struct Cyc_List_List* _temp1105; struct _tuple0* _temp1107; struct
Cyc_List_List* _temp1109; struct Cyc_List_List* _temp1111; struct _tuple0*
_temp1113; struct Cyc_List_List* _temp1115; struct Cyc_List_List* _temp1117;
struct Cyc_Core_Opt* _temp1119; struct Cyc_Absyn_Structdecl* _temp1121; struct
Cyc_Absyn_Tunionfield* _temp1123; struct Cyc_Absyn_Tuniondecl* _temp1125; struct
Cyc_List_List* _temp1127; struct Cyc_List_List* _temp1129; struct Cyc_Core_Opt*
_temp1131; struct _tuple0* _temp1133; struct Cyc_Absyn_Tunionfield* _temp1135;
struct Cyc_Absyn_XTuniondecl* _temp1137; struct Cyc_List_List* _temp1139; struct
Cyc_List_List* _temp1141; struct _tuple0* _temp1143; struct Cyc_Absyn_Enumfield*
_temp1145; struct Cyc_Absyn_Enumdecl* _temp1147; struct _tuple0* _temp1149;
_LL1047: if(( int) _temp1045 == Cyc_Absyn_Wild_p){ goto _LL1048;} else{ goto
_LL1049;} _LL1049: if(( int) _temp1045 == Cyc_Absyn_Null_p){ goto _LL1050;}
else{ goto _LL1051;} _LL1051: if(( unsigned int) _temp1045 > 2u?(( struct
_tunion_struct*) _temp1045)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL1084: _temp1083=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp1045)->f1; if(( int) _temp1083 ==
Cyc_Absyn_Signed){ goto _LL1082;} else{ goto _LL1053;} _LL1082: _temp1081=( int)((
struct Cyc_Absyn_Int_p_struct*) _temp1045)->f2; goto _LL1052;} else{ goto
_LL1053;} _LL1053: if(( unsigned int) _temp1045 > 2u?(( struct _tunion_struct*)
_temp1045)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL1088: _temp1087=( void*)(( struct
Cyc_Absyn_Int_p_struct*) _temp1045)->f1; if(( int) _temp1087 == Cyc_Absyn_Unsigned){
goto _LL1086;} else{ goto _LL1055;} _LL1086: _temp1085=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1045)->f2; goto _LL1054;} else{ goto _LL1055;} _LL1055: if(( unsigned int)
_temp1045 > 2u?(( struct _tunion_struct*) _temp1045)->tag == Cyc_Absyn_Char_p_tag:
0){ _LL1090: _temp1089=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1045)->f1;
goto _LL1056;} else{ goto _LL1057;} _LL1057: if(( unsigned int) _temp1045 > 2u?((
struct _tunion_struct*) _temp1045)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1092:
_temp1091=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1045)->f1;
goto _LL1058;} else{ goto _LL1059;} _LL1059: if(( unsigned int) _temp1045 > 2u?((
struct _tunion_struct*) _temp1045)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1094:
_temp1093=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1045)->f1; goto _LL1060;} else{ goto _LL1061;} _LL1061: if(( unsigned int)
_temp1045 > 2u?(( struct _tunion_struct*) _temp1045)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL1096: _temp1095=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1045)->f1; goto _LL1062;} else{ goto _LL1063;} _LL1063: if(( unsigned int)
_temp1045 > 2u?(( struct _tunion_struct*) _temp1045)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL1098: _temp1097=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1045)->f1; goto _LL1064;} else{ goto _LL1065;} _LL1065: if(( unsigned int)
_temp1045 > 2u?(( struct _tunion_struct*) _temp1045)->tag == Cyc_Absyn_Reference_p_tag:
0){ _LL1100: _temp1099=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1045)->f1; goto _LL1066;} else{ goto _LL1067;} _LL1067: if(( unsigned int)
_temp1045 > 2u?(( struct _tunion_struct*) _temp1045)->tag == Cyc_Absyn_UnknownId_p_tag:
0){ _LL1102: _temp1101=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1045)->f1; goto _LL1068;} else{ goto _LL1069;} _LL1069: if(( unsigned int)
_temp1045 > 2u?(( struct _tunion_struct*) _temp1045)->tag == Cyc_Absyn_UnknownCall_p_tag:
0){ _LL1108: _temp1107=( struct _tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1045)->f1; goto _LL1106; _LL1106: _temp1105=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1045)->f2; goto _LL1104; _LL1104:
_temp1103=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1045)->f3; goto _LL1070;} else{ goto _LL1071;} _LL1071: if(( unsigned int)
_temp1045 > 2u?(( struct _tunion_struct*) _temp1045)->tag == Cyc_Absyn_UnknownFields_p_tag:
0){ _LL1114: _temp1113=( struct _tuple0*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1045)->f1; goto _LL1112; _LL1112: _temp1111=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1045)->f2; goto _LL1110; _LL1110:
_temp1109=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1045)->f3; goto _LL1072;} else{ goto _LL1073;} _LL1073: if(( unsigned int)
_temp1045 > 2u?(( struct _tunion_struct*) _temp1045)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL1122: _temp1121=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1045)->f1; goto _LL1120; _LL1120: _temp1119=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1045)->f2; goto _LL1118; _LL1118:
_temp1117=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1045)->f3; goto _LL1116; _LL1116: _temp1115=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1045)->f4; goto _LL1074;} else{ goto
_LL1075;} _LL1075: if(( unsigned int) _temp1045 > 2u?(( struct _tunion_struct*)
_temp1045)->tag == Cyc_Absyn_Tunion_p_tag: 0){ _LL1134: _temp1133=( struct
_tuple0*)(( struct Cyc_Absyn_Tunion_p_struct*) _temp1045)->f1; goto _LL1132;
_LL1132: _temp1131=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1045)->f2; goto _LL1130; _LL1130: _temp1129=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1045)->f3; goto _LL1128; _LL1128:
_temp1127=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1045)->f4; goto _LL1126; _LL1126: _temp1125=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_p_struct*) _temp1045)->f5; goto _LL1124; _LL1124:
_temp1123=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_p_struct*)
_temp1045)->f6; goto _LL1076;} else{ goto _LL1077;} _LL1077: if(( unsigned int)
_temp1045 > 2u?(( struct _tunion_struct*) _temp1045)->tag == Cyc_Absyn_XTunion_p_tag:
0){ _LL1144: _temp1143=( struct _tuple0*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1045)->f1; goto _LL1142; _LL1142: _temp1141=( struct Cyc_List_List*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1045)->f2; goto _LL1140; _LL1140:
_temp1139=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1045)->f3; goto _LL1138; _LL1138: _temp1137=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_p_struct*) _temp1045)->f4; goto _LL1136; _LL1136:
_temp1135=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_p_struct*)
_temp1045)->f5; goto _LL1078;} else{ goto _LL1079;} _LL1079: if(( unsigned int)
_temp1045 > 2u?(( struct _tunion_struct*) _temp1045)->tag == Cyc_Absyn_Enum_p_tag:
0){ _LL1150: _temp1149=( struct _tuple0*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1045)->f1; goto _LL1148; _LL1148: _temp1147=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1045)->f2; goto _LL1146; _LL1146:
_temp1145=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1045)->f3; goto _LL1080;} else{ goto _LL1046;} _LL1048: Cyc_Absyndump_dump_char((
int)'_'); goto _LL1046; _LL1050: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1151=( char*)"null"; struct _tagged_string _temp1152; _temp1152.curr=
_temp1151; _temp1152.base= _temp1151; _temp1152.last_plus_one= _temp1151 + 5;
_temp1152;})); goto _LL1046; _LL1052: Cyc_Absyndump_dump( xprintf("%d",
_temp1081)); goto _LL1046; _LL1054: Cyc_Absyndump_dump( xprintf("%u",(
unsigned int) _temp1085)); goto _LL1046; _LL1056: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1153=( char*)"'"; struct _tagged_string _temp1154;
_temp1154.curr= _temp1153; _temp1154.base= _temp1153; _temp1154.last_plus_one=
_temp1153 + 2; _temp1154;})); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape(
_temp1089)); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp1155=( char*)"'"; struct _tagged_string _temp1156; _temp1156.curr=
_temp1155; _temp1156.base= _temp1155; _temp1156.last_plus_one= _temp1155 + 2;
_temp1156;})); goto _LL1046; _LL1058: Cyc_Absyndump_dump( _temp1091); goto
_LL1046; _LL1060: Cyc_Absyndump_dumpqvar( _temp1093->name); goto _LL1046;
_LL1062:(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1095,( struct
_tagged_string)({ char* _temp1157=( char*)"$("; struct _tagged_string _temp1158;
_temp1158.curr= _temp1157; _temp1158.base= _temp1157; _temp1158.last_plus_one=
_temp1157 + 3; _temp1158;}),( struct _tagged_string)({ char* _temp1159=( char*)")";
struct _tagged_string _temp1160; _temp1160.curr= _temp1159; _temp1160.base=
_temp1159; _temp1160.last_plus_one= _temp1159 + 2; _temp1160;}),( struct
_tagged_string)({ char* _temp1161=( char*)","; struct _tagged_string _temp1162;
_temp1162.curr= _temp1161; _temp1162.base= _temp1161; _temp1162.last_plus_one=
_temp1161 + 2; _temp1162;})); goto _LL1046; _LL1064: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1163=( char*)"&"; struct _tagged_string _temp1164;
_temp1164.curr= _temp1163; _temp1164.base= _temp1163; _temp1164.last_plus_one=
_temp1163 + 2; _temp1164;})); Cyc_Absyndump_dumppat( _temp1097); goto _LL1046;
_LL1066: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1165=( char*)"*";
struct _tagged_string _temp1166; _temp1166.curr= _temp1165; _temp1166.base=
_temp1165; _temp1166.last_plus_one= _temp1165 + 2; _temp1166;})); Cyc_Absyndump_dumpqvar(
_temp1099->name); goto _LL1046; _LL1068: Cyc_Absyndump_dumpqvar( _temp1101);
goto _LL1046; _LL1070: Cyc_Absyndump_dumpqvar( _temp1107); Cyc_Absyndump_dumptvars(
_temp1105);(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1103,( struct
_tagged_string)({ char* _temp1167=( char*)"("; struct _tagged_string _temp1168;
_temp1168.curr= _temp1167; _temp1168.base= _temp1167; _temp1168.last_plus_one=
_temp1167 + 2; _temp1168;}),( struct _tagged_string)({ char* _temp1169=( char*)")";
struct _tagged_string _temp1170; _temp1170.curr= _temp1169; _temp1170.base=
_temp1169; _temp1170.last_plus_one= _temp1169 + 2; _temp1170;}),( struct
_tagged_string)({ char* _temp1171=( char*)","; struct _tagged_string _temp1172;
_temp1172.curr= _temp1171; _temp1172.base= _temp1171; _temp1172.last_plus_one=
_temp1171 + 2; _temp1172;})); goto _LL1046; _LL1072: Cyc_Absyndump_dumpqvar(
_temp1113); Cyc_Absyndump_dumptvars( _temp1111);(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp1109,( struct _tagged_string)({ char* _temp1173=( char*)"{"; struct
_tagged_string _temp1174; _temp1174.curr= _temp1173; _temp1174.base= _temp1173;
_temp1174.last_plus_one= _temp1173 + 2; _temp1174;}),( struct _tagged_string)({
char* _temp1175=( char*)"}"; struct _tagged_string _temp1176; _temp1176.curr=
_temp1175; _temp1176.base= _temp1175; _temp1176.last_plus_one= _temp1175 + 2;
_temp1176;}),( struct _tagged_string)({ char* _temp1177=( char*)","; struct
_tagged_string _temp1178; _temp1178.curr= _temp1177; _temp1178.base= _temp1177;
_temp1178.last_plus_one= _temp1177 + 2; _temp1178;})); goto _LL1046; _LL1074:
if( _temp1121->name != 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)( _temp1121->name)->v);}
Cyc_Absyndump_dumptvars( _temp1117);(( void(*)( void(* f)( struct _tuple8*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp1115,( struct _tagged_string)({ char* _temp1179=( char*)"{"; struct
_tagged_string _temp1180; _temp1180.curr= _temp1179; _temp1180.base= _temp1179;
_temp1180.last_plus_one= _temp1179 + 2; _temp1180;}),( struct _tagged_string)({
char* _temp1181=( char*)"}"; struct _tagged_string _temp1182; _temp1182.curr=
_temp1181; _temp1182.base= _temp1181; _temp1182.last_plus_one= _temp1181 + 2;
_temp1182;}),( struct _tagged_string)({ char* _temp1183=( char*)","; struct
_tagged_string _temp1184; _temp1184.curr= _temp1183; _temp1184.base= _temp1183;
_temp1184.last_plus_one= _temp1183 + 2; _temp1184;})); goto _LL1046; _LL1076:
_temp1143= _temp1133; _temp1141= _temp1129; _temp1139= _temp1127; goto _LL1078;
_LL1078: Cyc_Absyndump_dumpqvar( _temp1143); Cyc_Absyndump_dumptvars( _temp1141);
if( _temp1139 != 0){(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1139,( struct
_tagged_string)({ char* _temp1185=( char*)"("; struct _tagged_string _temp1186;
_temp1186.curr= _temp1185; _temp1186.base= _temp1185; _temp1186.last_plus_one=
_temp1185 + 2; _temp1186;}),( struct _tagged_string)({ char* _temp1187=( char*)")";
struct _tagged_string _temp1188; _temp1188.curr= _temp1187; _temp1188.base=
_temp1187; _temp1188.last_plus_one= _temp1187 + 2; _temp1188;}),( struct
_tagged_string)({ char* _temp1189=( char*)","; struct _tagged_string _temp1190;
_temp1190.curr= _temp1189; _temp1190.base= _temp1189; _temp1190.last_plus_one=
_temp1189 + 2; _temp1190;}));} goto _LL1046; _LL1080: Cyc_Absyndump_dumpqvar(
_temp1149); goto _LL1046; _LL1046:;} void Cyc_Absyndump_dumptunionfield( struct
Cyc_Absyn_Tunionfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->typs != 0){
Cyc_Absyndump_dumpargs( ef->typs);}} void Cyc_Absyndump_dumptunionfields( struct
Cyc_List_List* fields){(( void(*)( void(* f)( struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List* l, struct _tagged_string sep)) Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dumptunionfield, fields,( struct _tagged_string)({ char* _temp1191=(
char*)","; struct _tagged_string _temp1192; _temp1192.curr= _temp1191; _temp1192.base=
_temp1191; _temp1192.last_plus_one= _temp1191 + 2; _temp1192;}));} void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->tag
!= 0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1193=( char*)" = ";
struct _tagged_string _temp1194; _temp1194.curr= _temp1193; _temp1194.base=
_temp1193; _temp1194.last_plus_one= _temp1193 + 4; _temp1194;})); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1195= ef->tag; if( _temp1195
== 0){ _throw( Null_Exception);} _temp1195;}));}} void Cyc_Absyndump_dumpenumfields(
struct Cyc_List_List* fields){(( void(*)( void(* f)( struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List* l, struct _tagged_string sep)) Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dumpenumfield, fields,( struct _tagged_string)({ char* _temp1196=(
char*)","; struct _tagged_string _temp1197; _temp1197.curr= _temp1196; _temp1197.base=
_temp1196; _temp1197.last_plus_one= _temp1196 + 2; _temp1197;}));} void Cyc_Absyndump_dumpstructfields(
struct Cyc_List_List* fields){ for( 0; fields != 0; fields= fields->tl){ struct
Cyc_Absyn_Structfield _temp1200; struct Cyc_List_List* _temp1201; struct Cyc_Core_Opt*
_temp1203; void* _temp1205; struct Cyc_Absyn_Tqual* _temp1207; struct
_tagged_string* _temp1209; struct Cyc_Absyn_Structfield* _temp1198=( struct Cyc_Absyn_Structfield*)
fields->hd; _temp1200=* _temp1198; _LL1210: _temp1209=( struct _tagged_string*)
_temp1200.name; goto _LL1208; _LL1208: _temp1207=( struct Cyc_Absyn_Tqual*)
_temp1200.tq; goto _LL1206; _LL1206: _temp1205=( void*) _temp1200.type; goto
_LL1204; _LL1204: _temp1203=( struct Cyc_Core_Opt*) _temp1200.width; goto
_LL1202; _LL1202: _temp1201=( struct Cyc_List_List*) _temp1200.attributes; goto
_LL1199; _LL1199:(( void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( struct
_tagged_string*), struct _tagged_string*)) Cyc_Absyndump_dumptqtd)(( struct Cyc_Absyn_Tqual*)
_temp1207, _temp1205, Cyc_Absyndump_dump_str, _temp1209); Cyc_Absyndump_dumpatts(
_temp1201); if( _temp1203 != 0){ Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*) _temp1203->v);} Cyc_Absyndump_dump_semi();}} void Cyc_Absyndump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl* td){ Cyc_Absyndump_dumpqvar( td->name); Cyc_Absyndump_dumptvars(
td->tvs);} static void Cyc_Absyndump_dump_atts_qvar( struct Cyc_Absyn_Fndecl* fd){
Cyc_Absyndump_dumpatts( fd->attributes); Cyc_Absyndump_dumpqvar( fd->name);}
void Cyc_Absyndump_dumpdecl( struct Cyc_Absyn_Decl* d){ void* _temp1211=( void*)
d->r; struct Cyc_Absyn_Fndecl* _temp1237; struct Cyc_Absyn_Structdecl* _temp1239;
struct Cyc_Absyn_Uniondecl* _temp1241; struct Cyc_Absyn_Vardecl* _temp1243;
struct Cyc_Absyn_Vardecl _temp1245; struct Cyc_List_List* _temp1246; struct Cyc_Core_Opt*
_temp1248; int _temp1250; struct Cyc_Absyn_Exp* _temp1252; void* _temp1254;
struct Cyc_Absyn_Tqual* _temp1256; struct _tuple0* _temp1258; void* _temp1260;
struct Cyc_Absyn_Tuniondecl* _temp1262; struct Cyc_Absyn_XTuniondecl* _temp1264;
struct Cyc_Absyn_Enumdecl* _temp1266; struct Cyc_Absyn_Enumdecl _temp1268;
struct Cyc_List_List* _temp1269; struct _tuple0* _temp1271; void* _temp1273; int
_temp1275; struct Cyc_Absyn_Exp* _temp1277; struct Cyc_Core_Opt* _temp1279;
struct Cyc_Core_Opt* _temp1281; struct Cyc_Absyn_Pat* _temp1283; struct Cyc_Absyn_Typedefdecl*
_temp1285; struct Cyc_List_List* _temp1287; struct _tagged_string* _temp1289;
struct Cyc_List_List* _temp1291; struct _tuple0* _temp1293; struct Cyc_List_List*
_temp1295; _LL1213: if((( struct _tunion_struct*) _temp1211)->tag == Cyc_Absyn_Fn_d_tag){
_LL1238: _temp1237=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp1211)->f1; goto _LL1214;} else{ goto _LL1215;} _LL1215: if((( struct
_tunion_struct*) _temp1211)->tag == Cyc_Absyn_Struct_d_tag){ _LL1240: _temp1239=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp1211)->f1;
goto _LL1216;} else{ goto _LL1217;} _LL1217: if((( struct _tunion_struct*)
_temp1211)->tag == Cyc_Absyn_Union_d_tag){ _LL1242: _temp1241=( struct Cyc_Absyn_Uniondecl*)((
struct Cyc_Absyn_Union_d_struct*) _temp1211)->f1; goto _LL1218;} else{ goto
_LL1219;} _LL1219: if((( struct _tunion_struct*) _temp1211)->tag == Cyc_Absyn_Var_d_tag){
_LL1244: _temp1243=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp1211)->f1; _temp1245=* _temp1243; _LL1261: _temp1260=( void*) _temp1245.sc;
goto _LL1259; _LL1259: _temp1258=( struct _tuple0*) _temp1245.name; goto _LL1257;
_LL1257: _temp1256=( struct Cyc_Absyn_Tqual*) _temp1245.tq; goto _LL1255;
_LL1255: _temp1254=( void*) _temp1245.type; goto _LL1253; _LL1253: _temp1252=(
struct Cyc_Absyn_Exp*) _temp1245.initializer; goto _LL1251; _LL1251: _temp1250=(
int) _temp1245.shadow; goto _LL1249; _LL1249: _temp1248=( struct Cyc_Core_Opt*)
_temp1245.region; goto _LL1247; _LL1247: _temp1246=( struct Cyc_List_List*)
_temp1245.attributes; goto _LL1220;} else{ goto _LL1221;} _LL1221: if((( struct
_tunion_struct*) _temp1211)->tag == Cyc_Absyn_Tunion_d_tag){ _LL1263: _temp1262=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*) _temp1211)->f1;
goto _LL1222;} else{ goto _LL1223;} _LL1223: if((( struct _tunion_struct*)
_temp1211)->tag == Cyc_Absyn_XTunion_d_tag){ _LL1265: _temp1264=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_d_struct*) _temp1211)->f1; goto _LL1224;} else{ goto
_LL1225;} _LL1225: if((( struct _tunion_struct*) _temp1211)->tag == Cyc_Absyn_Enum_d_tag){
_LL1267: _temp1266=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp1211)->f1; _temp1268=* _temp1266; _LL1274: _temp1273=( void*) _temp1268.sc;
goto _LL1272; _LL1272: _temp1271=( struct _tuple0*) _temp1268.name; goto _LL1270;
_LL1270: _temp1269=( struct Cyc_List_List*) _temp1268.fields; goto _LL1226;}
else{ goto _LL1227;} _LL1227: if((( struct _tunion_struct*) _temp1211)->tag ==
Cyc_Absyn_Let_d_tag){ _LL1284: _temp1283=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1211)->f1; goto _LL1282; _LL1282: _temp1281=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1211)->f2; goto _LL1280; _LL1280: _temp1279=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1211)->f3; goto
_LL1278; _LL1278: _temp1277=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1211)->f4; goto _LL1276; _LL1276: _temp1275=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1211)->f5; goto _LL1228;} else{ goto _LL1229;} _LL1229: if((( struct
_tunion_struct*) _temp1211)->tag == Cyc_Absyn_Typedef_d_tag){ _LL1286: _temp1285=(
struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp1211)->f1;
goto _LL1230;} else{ goto _LL1231;} _LL1231: if((( struct _tunion_struct*)
_temp1211)->tag == Cyc_Absyn_Namespace_d_tag){ _LL1290: _temp1289=( struct
_tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp1211)->f1; goto
_LL1288; _LL1288: _temp1287=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1211)->f2; goto _LL1232;} else{ goto _LL1233;} _LL1233: if((( struct
_tunion_struct*) _temp1211)->tag == Cyc_Absyn_Using_d_tag){ _LL1294: _temp1293=(
struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp1211)->f1; goto
_LL1292; _LL1292: _temp1291=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1211)->f2; goto _LL1234;} else{ goto _LL1235;} _LL1235: if((( struct
_tunion_struct*) _temp1211)->tag == Cyc_Absyn_ExternC_d_tag){ _LL1296: _temp1295=(
struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*) _temp1211)->f1;
goto _LL1236;} else{ goto _LL1212;} _LL1214: if( _temp1237->is_inline){ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1297=( char*)"inline"; struct _tagged_string
_temp1298; _temp1298.curr= _temp1297; _temp1298.base= _temp1297; _temp1298.last_plus_one=
_temp1297 + 7; _temp1298;}));} Cyc_Absyndump_dumpscope(( void*) _temp1237->sc);{
void* t=( void*)({ struct Cyc_Absyn_FnType_struct* _temp1299=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1299[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1300; _temp1300.tag= Cyc_Absyn_FnType_tag; _temp1300.f1=({ struct Cyc_Absyn_FnInfo
_temp1301; _temp1301.tvars= _temp1237->tvs; _temp1301.effect= _temp1237->effect;
_temp1301.ret_typ=( void*)(( void*) _temp1237->ret_type); _temp1301.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1237->args); _temp1301.varargs=
_temp1237->varargs; _temp1301.attributes= 0; _temp1301;}); _temp1300;});
_temp1299;});(( void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( struct Cyc_Absyn_Fndecl*),
struct Cyc_Absyn_Fndecl*)) Cyc_Absyndump_dumptqtd)( 0, t, Cyc_Absyndump_dump_atts_qvar,
_temp1237); Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt(
_temp1237->body); Cyc_Absyndump_dump_char(( int)'}'); goto _LL1212;} _LL1216:
Cyc_Absyndump_dumpscope(( void*) _temp1239->sc); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1302=( char*)"struct"; struct _tagged_string
_temp1303; _temp1303.curr= _temp1302; _temp1303.base= _temp1302; _temp1303.last_plus_one=
_temp1302 + 7; _temp1303;})); if( _temp1239->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)( _temp1239->name)->v);} Cyc_Absyndump_dumptvars( _temp1239->tvs);
if( _temp1239->fields == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)( _temp1239->fields)->v);
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1304=( char*)"}";
struct _tagged_string _temp1305; _temp1305.curr= _temp1304; _temp1305.base=
_temp1304; _temp1305.last_plus_one= _temp1304 + 2; _temp1305;})); Cyc_Absyndump_dumpatts(
_temp1239->attributes); Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1306=( char*)";"; struct _tagged_string _temp1307; _temp1307.curr=
_temp1306; _temp1307.base= _temp1306; _temp1307.last_plus_one= _temp1306 + 2;
_temp1307;}));} goto _LL1212; _LL1218: Cyc_Absyndump_dumpscope(( void*)
_temp1241->sc); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1308=(
char*)"union"; struct _tagged_string _temp1309; _temp1309.curr= _temp1308;
_temp1309.base= _temp1308; _temp1309.last_plus_one= _temp1308 + 6; _temp1309;}));
if( _temp1241->name != 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)( _temp1241->name)->v);}
Cyc_Absyndump_dumptvars( _temp1241->tvs); if( _temp1241->fields == 0){ Cyc_Absyndump_dump_semi();}
else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstructfields((
struct Cyc_List_List*)( _temp1241->fields)->v); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1310=( char*)"}"; struct _tagged_string _temp1311;
_temp1311.curr= _temp1310; _temp1311.base= _temp1310; _temp1311.last_plus_one=
_temp1310 + 2; _temp1311;})); Cyc_Absyndump_dumpatts( _temp1241->attributes);
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1312=( char*)";";
struct _tagged_string _temp1313; _temp1313.curr= _temp1312; _temp1313.base=
_temp1312; _temp1313.last_plus_one= _temp1312 + 2; _temp1313;}));} goto _LL1212;
_LL1220: Cyc_Absyndump_dumpscope( _temp1260);(( void(*)( struct Cyc_Absyn_Tqual*,
void*, void(* f)( struct _tuple0*), struct _tuple0*)) Cyc_Absyndump_dumptqtd)((
struct Cyc_Absyn_Tqual*) _temp1256, _temp1254, Cyc_Absyndump_dumpqvar, _temp1258);
Cyc_Absyndump_dumpatts( _temp1246); if( _temp1252 != 0){ Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1314= _temp1252; if( _temp1314 == 0){ _throw( Null_Exception);} _temp1314;}));}
Cyc_Absyndump_dump_semi(); goto _LL1212; _LL1222: Cyc_Absyndump_dumpscope(( void*)
_temp1262->sc); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1315=(
char*)"tunion "; struct _tagged_string _temp1316; _temp1316.curr= _temp1315;
_temp1316.base= _temp1315; _temp1316.last_plus_one= _temp1315 + 8; _temp1316;}));
if( _temp1262->name != 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)( _temp1262->name)->v);}
Cyc_Absyndump_dumptvars( _temp1262->tvs); if( _temp1262->fields == 0){ Cyc_Absyndump_dump_semi();}
else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumptunionfields((
struct Cyc_List_List*)( _temp1262->fields)->v); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1317=( char*)"};"; struct _tagged_string
_temp1318; _temp1318.curr= _temp1317; _temp1318.base= _temp1317; _temp1318.last_plus_one=
_temp1317 + 3; _temp1318;}));} goto _LL1212; _LL1224: Cyc_Absyndump_dumpscope((
void*) _temp1264->sc); Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1319=( char*)"xtunion "; struct _tagged_string _temp1320; _temp1320.curr=
_temp1319; _temp1320.base= _temp1319; _temp1320.last_plus_one= _temp1319 + 9;
_temp1320;})); Cyc_Absyndump_dumpqvar( _temp1264->name); if( _temp1264->fields
== 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumptunionfields(
_temp1264->fields); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp1321=( char*)"};"; struct _tagged_string _temp1322; _temp1322.curr=
_temp1321; _temp1322.base= _temp1321; _temp1322.last_plus_one= _temp1321 + 3;
_temp1322;}));} goto _LL1212; _LL1226: Cyc_Absyndump_dumpscope( _temp1273); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1323=( char*)"enum "; struct _tagged_string
_temp1324; _temp1324.curr= _temp1323; _temp1324.base= _temp1323; _temp1324.last_plus_one=
_temp1323 + 6; _temp1324;})); Cyc_Absyndump_dumpqvar( _temp1271); Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpenumfields( _temp1269); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1325=( char*)"};"; struct _tagged_string
_temp1326; _temp1326.curr= _temp1325; _temp1326.base= _temp1325; _temp1326.last_plus_one=
_temp1325 + 3; _temp1326;})); return; _LL1228: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1327=( char*)"let"; struct _tagged_string _temp1328;
_temp1328.curr= _temp1327; _temp1328.base= _temp1327; _temp1328.last_plus_one=
_temp1327 + 4; _temp1328;})); Cyc_Absyndump_dumppat( _temp1283); Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp( _temp1277); Cyc_Absyndump_dump_semi(); goto
_LL1212; _LL1230: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1329=(
char*)"typedef"; struct _tagged_string _temp1330; _temp1330.curr= _temp1329;
_temp1330.base= _temp1329; _temp1330.last_plus_one= _temp1329 + 8; _temp1330;}));((
void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*)) Cyc_Absyndump_dumptqtd)( 0,( void*) _temp1285->defn,
Cyc_Absyndump_dumptypedefname, _temp1285); Cyc_Absyndump_dump_semi(); goto
_LL1212; _LL1232: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1331=(
char*)"namespace"; struct _tagged_string _temp1332; _temp1332.curr= _temp1331;
_temp1332.base= _temp1331; _temp1332.last_plus_one= _temp1331 + 10; _temp1332;}));
Cyc_Absyndump_dump_str( _temp1289); Cyc_Absyndump_dump_char(( int)'{'); for( 0;
_temp1287 != 0; _temp1287= _temp1287->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)
_temp1287->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL1212; _LL1234: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1333=( char*)"using"; struct _tagged_string
_temp1334; _temp1334.curr= _temp1333; _temp1334.base= _temp1333; _temp1334.last_plus_one=
_temp1333 + 6; _temp1334;})); Cyc_Absyndump_dumpqvar( _temp1293); Cyc_Absyndump_dump_char((
int)'{'); for( 0; _temp1291 != 0; _temp1291= _temp1291->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*) _temp1291->hd);} Cyc_Absyndump_dump_char(( int)'}');
goto _LL1212; _LL1236: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1335=( char*)"extern \"C\" {"; struct _tagged_string _temp1336; _temp1336.curr=
_temp1335; _temp1336.base= _temp1335; _temp1336.last_plus_one= _temp1335 + 13;
_temp1336;})); for( 0; _temp1295 != 0; _temp1295= _temp1295->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*) _temp1295->hd);} Cyc_Absyndump_dump_char(( int)'}');
goto _LL1212; _LL1212:;} static void Cyc_Absyndump_dump_upperbound( struct Cyc_Absyn_Exp*
e){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if( i !=( unsigned int) 1){
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpexp( e); Cyc_Absyndump_dump_char((
int)'}');}} void Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)(
void*), void* a){ if( tms == 0){ f( a); return;}{ void* _temp1337=( void*) tms->hd;
struct Cyc_Absyn_Tqual* _temp1355; void* _temp1357; void* _temp1359; struct Cyc_Absyn_Exp*
_temp1361; struct Cyc_Absyn_Tqual* _temp1363; void* _temp1365; void* _temp1367;
struct Cyc_Absyn_Exp* _temp1369; struct Cyc_Absyn_Tqual* _temp1371; void*
_temp1373; void* _temp1375; struct Cyc_Absyn_Tqual* _temp1377; void* _temp1379;
struct Cyc_Absyn_Tvar* _temp1381; void* _temp1383; struct Cyc_Absyn_Exp*
_temp1385; struct Cyc_Absyn_Tqual* _temp1387; void* _temp1389; struct Cyc_Absyn_Tvar*
_temp1391; void* _temp1393; struct Cyc_Absyn_Exp* _temp1395; struct Cyc_Absyn_Tqual*
_temp1397; void* _temp1399; struct Cyc_Absyn_Tvar* _temp1401; void* _temp1403;
struct Cyc_Absyn_Tqual* _temp1405; void* _temp1407; void* _temp1409; _LL1339:
if(( unsigned int) _temp1337 > 1u?(( struct _tunion_struct*) _temp1337)->tag ==
Cyc_Absyn_Pointer_mod_tag: 0){ _LL1360: _temp1359=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1337)->f1; if(( unsigned int) _temp1359 > 1u?(( struct _tunion_struct*)
_temp1359)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL1362: _temp1361=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1359)->f1; goto
_LL1358;} else{ goto _LL1341;} _LL1358: _temp1357=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1337)->f2; if(( int) _temp1357 == Cyc_Absyn_HeapRgn){ goto _LL1356;} else{
goto _LL1341;} _LL1356: _temp1355=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1337)->f3; goto _LL1340;} else{ goto _LL1341;} _LL1341: if(( unsigned int)
_temp1337 > 1u?(( struct _tunion_struct*) _temp1337)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1368: _temp1367=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1337)->f1; if(( unsigned int) _temp1367 > 1u?(( struct _tunion_struct*)
_temp1367)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL1370: _temp1369=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1367)->f1; goto
_LL1366;} else{ goto _LL1343;} _LL1366: _temp1365=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1337)->f2; if(( int) _temp1365 == Cyc_Absyn_HeapRgn){ goto _LL1364;} else{
goto _LL1343;} _LL1364: _temp1363=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1337)->f3; goto _LL1342;} else{ goto _LL1343;} _LL1343: if(( unsigned int)
_temp1337 > 1u?(( struct _tunion_struct*) _temp1337)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1376: _temp1375=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1337)->f1; if(( int) _temp1375 == Cyc_Absyn_TaggedArray_ps){ goto _LL1374;}
else{ goto _LL1345;} _LL1374: _temp1373=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1337)->f2; if(( int) _temp1373 == Cyc_Absyn_HeapRgn){ goto _LL1372;} else{
goto _LL1345;} _LL1372: _temp1371=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1337)->f3; goto _LL1344;} else{ goto _LL1345;} _LL1345: if(( unsigned int)
_temp1337 > 1u?(( struct _tunion_struct*) _temp1337)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1384: _temp1383=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1337)->f1; if(( unsigned int) _temp1383 > 1u?(( struct _tunion_struct*)
_temp1383)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL1386: _temp1385=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1383)->f1; goto
_LL1380;} else{ goto _LL1347;} _LL1380: _temp1379=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1337)->f2; if(( unsigned int) _temp1379 > 4u?(( struct _tunion_struct*)
_temp1379)->tag == Cyc_Absyn_VarType_tag: 0){ _LL1382: _temp1381=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp1379)->f1; goto _LL1378;} else{ goto
_LL1347;} _LL1378: _temp1377=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1337)->f3; goto _LL1346;} else{ goto _LL1347;} _LL1347: if(( unsigned int)
_temp1337 > 1u?(( struct _tunion_struct*) _temp1337)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1394: _temp1393=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1337)->f1; if(( unsigned int) _temp1393 > 1u?(( struct _tunion_struct*)
_temp1393)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL1396: _temp1395=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1393)->f1; goto
_LL1390;} else{ goto _LL1349;} _LL1390: _temp1389=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1337)->f2; if(( unsigned int) _temp1389 > 4u?(( struct _tunion_struct*)
_temp1389)->tag == Cyc_Absyn_VarType_tag: 0){ _LL1392: _temp1391=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp1389)->f1; goto _LL1388;} else{ goto
_LL1349;} _LL1388: _temp1387=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1337)->f3; goto _LL1348;} else{ goto _LL1349;} _LL1349: if(( unsigned int)
_temp1337 > 1u?(( struct _tunion_struct*) _temp1337)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1404: _temp1403=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1337)->f1; if(( int) _temp1403 == Cyc_Absyn_TaggedArray_ps){ goto _LL1400;}
else{ goto _LL1351;} _LL1400: _temp1399=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1337)->f2; if(( unsigned int) _temp1399 > 4u?(( struct _tunion_struct*)
_temp1399)->tag == Cyc_Absyn_VarType_tag: 0){ _LL1402: _temp1401=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp1399)->f1; goto _LL1398;} else{ goto
_LL1351;} _LL1398: _temp1397=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1337)->f3; goto _LL1350;} else{ goto _LL1351;} _LL1351: if(( unsigned int)
_temp1337 > 1u?(( struct _tunion_struct*) _temp1337)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1410: _temp1409=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1337)->f1; goto _LL1408; _LL1408: _temp1407=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1337)->f2; goto _LL1406; _LL1406: _temp1405=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1337)->f3; goto _LL1352;} else{ goto
_LL1353;} _LL1353: goto _LL1354; _LL1340: Cyc_Absyndump_dump_char(( int)'*');
Cyc_Absyndump_dump_upperbound( _temp1361);(( void(*)( struct Cyc_List_List* tms,
void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)( tms->tl, f, a); return;
_LL1342: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1369);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)( tms->tl, f, a); return; _LL1344: Cyc_Absyndump_dump_char((
int)'?');(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(
tms->tl, f, a); return; _LL1346: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dump_upperbound(
_temp1385); Cyc_Absyndump_dump_str( _temp1381->name);(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)( tms->tl, f, a);
return; _LL1348: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1395); Cyc_Absyndump_dump_str( _temp1391->name);(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)( tms->tl, f, a);
return; _LL1350: Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dump_str(
_temp1401->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)( tms->tl, f, a); return; _LL1352:( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp1411=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1411[ 0]=({ struct Cyc_Core_Impossible_struct _temp1412; _temp1412.tag= Cyc_Core_Impossible_tag;
_temp1412.f1=( struct _tagged_string)({ char* _temp1413=( char*)"dumptms: bad Pointer_mod";
struct _tagged_string _temp1414; _temp1414.curr= _temp1413; _temp1414.base=
_temp1413; _temp1414.last_plus_one= _temp1413 + 25; _temp1414;}); _temp1412;});
_temp1411;})); _LL1354: { int next_is_pointer= 0; if( tms->tl != 0){ void*
_temp1415=( void*)( tms->tl)->hd; struct Cyc_Absyn_Tqual* _temp1421; void*
_temp1423; void* _temp1425; _LL1417: if(( unsigned int) _temp1415 > 1u?(( struct
_tunion_struct*) _temp1415)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL1426:
_temp1425=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1415)->f1; goto
_LL1424; _LL1424: _temp1423=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1415)->f2; goto _LL1422; _LL1422: _temp1421=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1415)->f3; goto _LL1418;} else{ goto
_LL1419;} _LL1419: goto _LL1420; _LL1418: next_is_pointer= 1; goto _LL1416;
_LL1420: goto _LL1416; _LL1416:;} if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)'(');}(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)( tms->tl, f, a); if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)')');}{ void* _temp1427=( void*) tms->hd; struct Cyc_Absyn_Exp* _temp1443;
void* _temp1445; struct Cyc_Core_Opt* _temp1447; int _temp1449; struct Cyc_List_List*
_temp1451; void* _temp1453; struct Cyc_Position_Segment* _temp1455; struct Cyc_List_List*
_temp1457; int _temp1459; struct Cyc_Position_Segment* _temp1461; struct Cyc_List_List*
_temp1463; struct Cyc_List_List* _temp1465; struct Cyc_Position_Segment*
_temp1467; struct Cyc_Absyn_Tqual* _temp1469; void* _temp1471; void* _temp1473;
_LL1429: if(( int) _temp1427 == Cyc_Absyn_Carray_mod){ goto _LL1430;} else{ goto
_LL1431;} _LL1431: if(( unsigned int) _temp1427 > 1u?(( struct _tunion_struct*)
_temp1427)->tag == Cyc_Absyn_ConstArray_mod_tag: 0){ _LL1444: _temp1443=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp1427)->f1; goto
_LL1432;} else{ goto _LL1433;} _LL1433: if(( unsigned int) _temp1427 > 1u?((
struct _tunion_struct*) _temp1427)->tag == Cyc_Absyn_Function_mod_tag: 0){
_LL1446: _temp1445=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1427)->f1;
if((( struct _tunion_struct*) _temp1445)->tag == Cyc_Absyn_WithTypes_tag){
_LL1452: _temp1451=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp1445)->f1; goto _LL1450; _LL1450: _temp1449=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp1445)->f2; goto _LL1448; _LL1448: _temp1447=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_WithTypes_struct*) _temp1445)->f3; goto _LL1434;} else{ goto
_LL1435;}} else{ goto _LL1435;} _LL1435: if(( unsigned int) _temp1427 > 1u?((
struct _tunion_struct*) _temp1427)->tag == Cyc_Absyn_Function_mod_tag: 0){
_LL1454: _temp1453=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1427)->f1;
if((( struct _tunion_struct*) _temp1453)->tag == Cyc_Absyn_NoTypes_tag){ _LL1458:
_temp1457=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*) _temp1453)->f1;
goto _LL1456; _LL1456: _temp1455=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp1453)->f2; goto _LL1436;} else{ goto _LL1437;}} else{ goto _LL1437;}
_LL1437: if(( unsigned int) _temp1427 > 1u?(( struct _tunion_struct*) _temp1427)->tag
== Cyc_Absyn_TypeParams_mod_tag: 0){ _LL1464: _temp1463=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp1427)->f1; goto _LL1462; _LL1462:
_temp1461=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1427)->f2; goto _LL1460; _LL1460: _temp1459=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1427)->f3; goto _LL1438;} else{ goto _LL1439;} _LL1439: if(( unsigned int)
_temp1427 > 1u?(( struct _tunion_struct*) _temp1427)->tag == Cyc_Absyn_Attributes_mod_tag:
0){ _LL1468: _temp1467=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp1427)->f1; goto _LL1466; _LL1466: _temp1465=( struct Cyc_List_List*)((
struct Cyc_Absyn_Attributes_mod_struct*) _temp1427)->f2; goto _LL1440;} else{
goto _LL1441;} _LL1441: if(( unsigned int) _temp1427 > 1u?(( struct
_tunion_struct*) _temp1427)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL1474:
_temp1473=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1427)->f1; goto
_LL1472; _LL1472: _temp1471=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1427)->f2; goto _LL1470; _LL1470: _temp1469=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1427)->f3; goto _LL1442;} else{ goto
_LL1428;} _LL1430: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1475=(
char*)"[]"; struct _tagged_string _temp1476; _temp1476.curr= _temp1475;
_temp1476.base= _temp1475; _temp1476.last_plus_one= _temp1475 + 3; _temp1476;}));
goto _LL1428; _LL1432: Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp(
_temp1443); Cyc_Absyndump_dump_char(( int)']'); goto _LL1428; _LL1434: Cyc_Absyndump_dumpfunargs(
_temp1451, _temp1449, _temp1447); goto _LL1428; _LL1436:(( void(*)( void(* f)(
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dump_str,
_temp1457,( struct _tagged_string)({ char* _temp1477=( char*)"("; struct
_tagged_string _temp1478; _temp1478.curr= _temp1477; _temp1478.base= _temp1477;
_temp1478.last_plus_one= _temp1477 + 2; _temp1478;}),( struct _tagged_string)({
char* _temp1479=( char*)")"; struct _tagged_string _temp1480; _temp1480.curr=
_temp1479; _temp1480.base= _temp1479; _temp1480.last_plus_one= _temp1479 + 2;
_temp1480;}),( struct _tagged_string)({ char* _temp1481=( char*)","; struct
_tagged_string _temp1482; _temp1482.curr= _temp1481; _temp1482.base= _temp1481;
_temp1482.last_plus_one= _temp1481 + 2; _temp1482;})); goto _LL1428; _LL1438:
if( _temp1459){ Cyc_Absyndump_dumpkindedtvars( _temp1463);} else{ Cyc_Absyndump_dumptvars(
_temp1463);} goto _LL1428; _LL1440: Cyc_Absyndump_dumpatts( _temp1465); goto
_LL1428; _LL1442:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp1483=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1483[ 0]=({ struct Cyc_Core_Impossible_struct _temp1484; _temp1484.tag= Cyc_Core_Impossible_tag;
_temp1484.f1=( struct _tagged_string)({ char* _temp1485=( char*)"dumptms";
struct _tagged_string _temp1486; _temp1486.curr= _temp1485; _temp1486.base=
_temp1485; _temp1486.last_plus_one= _temp1485 + 8; _temp1486;}); _temp1484;});
_temp1483;})); _LL1428:;} return;} _LL1338:;}} static struct Cyc_Absyn_Tqual Cyc_Absyndump_empty_tq_v=(
struct Cyc_Absyn_Tqual){.q_const= 0,.q_volatile= 0,.q_restrict= 0}; static
struct Cyc_Absyn_Tqual* Cyc_Absyndump_empty_tq=& Cyc_Absyndump_empty_tq_v; void
Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual* tq, void* t, void(* f)( void*),
void* a){ if( tq == 0){ tq=( struct Cyc_Absyn_Tqual*) Cyc_Absyndump_empty_tq;}{
struct Cyc_List_List* _temp1490; void* _temp1492; struct Cyc_Absyn_Tqual*
_temp1494; struct _tuple4 _temp1488= Cyc_Absynpp_to_tms(( struct Cyc_Absyn_Tqual*)({
struct Cyc_Absyn_Tqual* _temp1487= tq; if( _temp1487 == 0){ _throw(
Null_Exception);} _temp1487;}), t); _LL1495: _temp1494= _temp1488.f1; goto
_LL1493; _LL1493: _temp1492= _temp1488.f2; goto _LL1491; _LL1491: _temp1490=
_temp1488.f3; goto _LL1489; _LL1489: Cyc_Absyndump_dumptq( _temp1494); Cyc_Absyndump_dumpntyp(
_temp1492);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp1490), f, a);}} void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f){ Cyc_Absyndump_pos= 0;*
Cyc_Absyndump_dump_file= f; for( 0; tdl != 0; tdl= tdl->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*) tdl->hd);}}