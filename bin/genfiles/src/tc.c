#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ void*
f1; void* f2; } ; struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ;
struct _tuple5{ struct Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple6{ void*
f1; int f2; } ; struct _tuple7{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2;
} ; struct _tuple8{ struct Cyc_Absyn_XTuniondecl* f1; } ; typedef unsigned int
Cyc_uint; typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string;
typedef struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[
11u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char*
tag; } ; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern int Cyc_List_mem( int(* compare)( void*, void*), struct
Cyc_List_List* l, void* x); extern struct Cyc_List_List* Cyc_List_filter_c( int(*
f)( void*, void*), void* env, struct Cyc_List_List* l); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; extern int Cyc_String_strcmp( struct _tagged_string s1, struct
_tagged_string s2); extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
struct _tagged_string*); struct Cyc_Lineno_Pos{ struct _tagged_string
logical_file; struct _tagged_string line; int line_no; int col; } ; typedef
struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[ 5u];
struct Cyc_Position_Exit_struct{ char* tag; } ; struct Cyc_Position_Segment;
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
char* tag; } ; extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*);
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct _tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string( void*);
extern struct _tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*); struct
Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set*
Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern struct Cyc_Set_Set*
Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member(
struct Cyc_Set_Set* s, void* elt); extern char Cyc_Set_Absent_tag[ 7u]; struct
Cyc_Set_Absent_struct{ char* tag; } ; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict*
Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[
8u]; struct Cyc_Dict_Present_struct{ char* tag; } ; extern char Cyc_Dict_Absent_tag[
7u]; extern int Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern
struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void*
data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern
struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key);
typedef void* Cyc_Tcenv_Resolved; static const int Cyc_Tcenv_VarRes_tag= 0;
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
extern struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(); extern struct Cyc_Tcenv_Fenv*
Cyc_Tcenv_new_fenv( struct Cyc_Position_Segment*, struct Cyc_Absyn_Fndecl*);
extern struct Cyc_List_List* Cyc_Tcenv_resolve_namespace( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tagged_string*, struct Cyc_List_List*);
extern struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*); extern char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{
char* tag; } ; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern
void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_string);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_string);
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_unify(
void*, void*); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*, void*);
extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); extern struct Cyc_Absyn_Exp*
Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*, void*, struct Cyc_Position_Segment*);
extern void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual* tq1, struct Cyc_Absyn_Tqual* tq2); extern int Cyc_Tcutil_same_atts(
struct Cyc_List_List*, struct Cyc_List_List*); extern int Cyc_Tcutil_bits_only(
void* t); extern int Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e); extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*,
void**, struct Cyc_Absyn_Exp*); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s, int new_block); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int
add_cyc_namespace, struct Cyc_List_List* ds); extern struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*); static void Cyc_Tc_redecl_err(
struct Cyc_Position_Segment* loc, struct _tagged_string t, struct _tagged_string*
v, struct _tagged_string new_sc, struct _tagged_string old_sc){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp0= t; struct _tagged_string _temp1=* v; struct
_tagged_string _temp2= new_sc; struct _tagged_string _temp3= old_sc; xprintf("redeclaration of %.*s %.*s cannot be %.*s when earlier definition is %.*s",
_temp0.last_plus_one - _temp0.curr, _temp0.curr, _temp1.last_plus_one - _temp1.curr,
_temp1.curr, _temp2.last_plus_one - _temp2.curr, _temp2.curr, _temp3.last_plus_one
- _temp3.curr, _temp3.curr);}));} static void* Cyc_Tc_scope_redecl_okay( void*
s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt* fields2, struct
Cyc_Position_Segment* loc, struct _tagged_string t, struct _tagged_string* v){{
struct _tuple3 _temp5=({ struct _tuple3 _temp4; _temp4.f1= s1; _temp4.f2= s2;
_temp4;}); void* _temp35; void* _temp37; void* _temp39; void* _temp41; void*
_temp43; void* _temp45; void* _temp47; void* _temp49; void* _temp51; void*
_temp53; void* _temp55; void* _temp57; void* _temp59; void* _temp61; void*
_temp63; void* _temp65; void* _temp68; void* _temp70; void* _temp72; void*
_temp74; void* _temp76; void* _temp78; void* _temp80; void* _temp82; void*
_temp85; void* _temp87; void* _temp89; void* _temp91; _LL7: _LL38: _temp37=
_temp5.f1; if(( int) _temp37 == Cyc_Absyn_ExternC){ goto _LL36;} else{ goto _LL9;}
_LL36: _temp35= _temp5.f2; if(( int) _temp35 == Cyc_Absyn_ExternC){ goto _LL8;}
else{ goto _LL9;} _LL9: _LL42: _temp41= _temp5.f1; goto _LL40; _LL40: _temp39=
_temp5.f2; if(( int) _temp39 == Cyc_Absyn_ExternC){ goto _LL10;} else{ goto
_LL11;} _LL11: _LL46: _temp45= _temp5.f1; if(( int) _temp45 == Cyc_Absyn_Static){
goto _LL44;} else{ goto _LL13;} _LL44: _temp43= _temp5.f2; if(( int) _temp43 ==
Cyc_Absyn_Static){ goto _LL12;} else{ goto _LL13;} _LL13: _LL50: _temp49= _temp5.f1;
goto _LL48; _LL48: _temp47= _temp5.f2; if(( int) _temp47 == Cyc_Absyn_Static){
goto _LL14;} else{ goto _LL15;} _LL15: _LL54: _temp53= _temp5.f1; if(( int)
_temp53 == Cyc_Absyn_Static){ goto _LL52;} else{ goto _LL17;} _LL52: _temp51=
_temp5.f2; if(( int) _temp51 == Cyc_Absyn_Public){ goto _LL16;} else{ goto _LL17;}
_LL17: _LL58: _temp57= _temp5.f1; if(( int) _temp57 == Cyc_Absyn_Abstract){ goto
_LL56;} else{ goto _LL19;} _LL56: _temp55= _temp5.f2; if(( int) _temp55 == Cyc_Absyn_Public){
goto _LL18;} else{ goto _LL19;} _LL19: _LL62: _temp61= _temp5.f1; goto _LL60;
_LL60: _temp59= _temp5.f2; if(( int) _temp59 == Cyc_Absyn_Public){ goto _LL20;}
else{ goto _LL21;} _LL21: _LL66: _temp65= _temp5.f1; if(( int) _temp65 == Cyc_Absyn_Static){
goto _LL64;} else{ goto _LL23;} _LL64: _temp63= _temp5.f2; if(( int) _temp63 ==
Cyc_Absyn_Extern){ goto _LL22;} else{ goto _LL23;} _LL23: _LL71: _temp70= _temp5.f1;
if(( int) _temp70 == Cyc_Absyn_Abstract){ goto _LL69;} else{ goto _LL25;} _LL69:
_temp68= _temp5.f2; if(( int) _temp68 == Cyc_Absyn_Extern){ goto _LL67;} else{
goto _LL25;} _LL67: if( fields2 != 0){ goto _LL24;} else{ goto _LL25;} _LL25:
_LL75: _temp74= _temp5.f1; goto _LL73; _LL73: _temp72= _temp5.f2; if(( int)
_temp72 == Cyc_Absyn_Extern){ goto _LL26;} else{ goto _LL27;} _LL27: _LL79:
_temp78= _temp5.f1; if(( int) _temp78 == Cyc_Absyn_Static){ goto _LL77;} else{
goto _LL29;} _LL77: _temp76= _temp5.f2; if(( int) _temp76 == Cyc_Absyn_Abstract){
goto _LL28;} else{ goto _LL29;} _LL29: _LL83: _temp82= _temp5.f1; if(( int)
_temp82 == Cyc_Absyn_Public){ goto _LL81;} else{ goto _LL31;} _LL81: _temp80=
_temp5.f2; if(( int) _temp80 == Cyc_Absyn_Abstract){ goto _LL30;} else{ goto
_LL31;} _LL31: _LL88: _temp87= _temp5.f1; if(( int) _temp87 == Cyc_Absyn_Extern){
goto _LL86;} else{ goto _LL33;} _LL86: _temp85= _temp5.f2; if(( int) _temp85 ==
Cyc_Absyn_Abstract){ goto _LL84;} else{ goto _LL33;} _LL84: if( fields1 != 0){
goto _LL32;} else{ goto _LL33;} _LL33: _LL92: _temp91= _temp5.f1; goto _LL90;
_LL90: _temp89= _temp5.f2; if(( int) _temp89 == Cyc_Absyn_Abstract){ goto _LL34;}
else{ goto _LL6;} _LL8: goto _LL6; _LL10: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp93=( char*)"non-extern \"C\""; struct
_tagged_string _temp94; _temp94.curr= _temp93; _temp94.base= _temp93; _temp94.last_plus_one=
_temp93 + 15; _temp94;}),( struct _tagged_string)({ char* _temp95=( char*)"extern \"C\"";
struct _tagged_string _temp96; _temp96.curr= _temp95; _temp96.base= _temp95;
_temp96.last_plus_one= _temp95 + 11; _temp96;})); goto _LL6; _LL12: goto _LL6;
_LL14: Cyc_Tc_redecl_err( loc, t, v,( struct _tagged_string)({ char* _temp97=(
char*)"non-static"; struct _tagged_string _temp98; _temp98.curr= _temp97;
_temp98.base= _temp97; _temp98.last_plus_one= _temp97 + 11; _temp98;}),( struct
_tagged_string)({ char* _temp99=( char*)"static"; struct _tagged_string _temp100;
_temp100.curr= _temp99; _temp100.base= _temp99; _temp100.last_plus_one= _temp99
+ 7; _temp100;})); goto _LL6; _LL16: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp101=( char*)"static"; struct _tagged_string
_temp102; _temp102.curr= _temp101; _temp102.base= _temp101; _temp102.last_plus_one=
_temp101 + 7; _temp102;}),( struct _tagged_string)({ char* _temp103=( char*)"public";
struct _tagged_string _temp104; _temp104.curr= _temp103; _temp104.base= _temp103;
_temp104.last_plus_one= _temp103 + 7; _temp104;})); goto _LL6; _LL18: Cyc_Tc_redecl_err(
loc, t, v,( struct _tagged_string)({ char* _temp105=( char*)"abstract"; struct
_tagged_string _temp106; _temp106.curr= _temp105; _temp106.base= _temp105;
_temp106.last_plus_one= _temp105 + 9; _temp106;}),( struct _tagged_string)({
char* _temp107=( char*)"public"; struct _tagged_string _temp108; _temp108.curr=
_temp107; _temp108.base= _temp107; _temp108.last_plus_one= _temp107 + 7;
_temp108;})); goto _LL6; _LL20: s1=( void*) Cyc_Absyn_Public; goto _LL6; _LL22:
Cyc_Tc_redecl_err( loc, t, v,( struct _tagged_string)({ char* _temp109=( char*)"static";
struct _tagged_string _temp110; _temp110.curr= _temp109; _temp110.base= _temp109;
_temp110.last_plus_one= _temp109 + 7; _temp110;}),( struct _tagged_string)({
char* _temp111=( char*)"extern"; struct _tagged_string _temp112; _temp112.curr=
_temp111; _temp112.base= _temp111; _temp112.last_plus_one= _temp111 + 7;
_temp112;})); goto _LL6; _LL24: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp113=( char*)"abstract"; struct _tagged_string
_temp114; _temp114.curr= _temp113; _temp114.base= _temp113; _temp114.last_plus_one=
_temp113 + 9; _temp114;}),( struct _tagged_string)({ char* _temp115=( char*)"transparent";
struct _tagged_string _temp116; _temp116.curr= _temp115; _temp116.base= _temp115;
_temp116.last_plus_one= _temp115 + 12; _temp116;})); goto _LL6; _LL26: goto _LL6;
_LL28: Cyc_Tc_redecl_err( loc, t, v,( struct _tagged_string)({ char* _temp117=(
char*)"static"; struct _tagged_string _temp118; _temp118.curr= _temp117;
_temp118.base= _temp117; _temp118.last_plus_one= _temp117 + 7; _temp118;}),(
struct _tagged_string)({ char* _temp119=( char*)"abstract"; struct
_tagged_string _temp120; _temp120.curr= _temp119; _temp120.base= _temp119;
_temp120.last_plus_one= _temp119 + 9; _temp120;})); goto _LL6; _LL30: Cyc_Tc_redecl_err(
loc, t, v,( struct _tagged_string)({ char* _temp121=( char*)"public"; struct
_tagged_string _temp122; _temp122.curr= _temp121; _temp122.base= _temp121;
_temp122.last_plus_one= _temp121 + 7; _temp122;}),( struct _tagged_string)({
char* _temp123=( char*)"abstract"; struct _tagged_string _temp124; _temp124.curr=
_temp123; _temp124.base= _temp123; _temp124.last_plus_one= _temp123 + 9;
_temp124;})); goto _LL6; _LL32: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp125=( char*)"[extern] transparent"; struct
_tagged_string _temp126; _temp126.curr= _temp125; _temp126.base= _temp125;
_temp126.last_plus_one= _temp125 + 21; _temp126;}),( struct _tagged_string)({
char* _temp127=( char*)"abstract"; struct _tagged_string _temp128; _temp128.curr=
_temp127; _temp128.base= _temp127; _temp128.last_plus_one= _temp127 + 9;
_temp128;})); goto _LL6; _LL34: s1=( void*) Cyc_Absyn_Abstract; goto _LL6; _LL6:;}
return s1;} static void Cyc_Tc_field_redecl_okay( struct Cyc_Absyn_Tunionfield*
f1, struct Cyc_Absyn_Tunionfield* f2, struct Cyc_List_List* inst, struct Cyc_Tcenv_Tenv*
te, struct _tagged_string t, struct _tagged_string* v){ struct Cyc_Position_Segment*
loc= f1->loc; if( Cyc_String_zstrptrcmp((* f1->name).f2,(* f2->name).f2) != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp129= t; struct _tagged_string
_temp130=* v; struct _tagged_string _temp131=*(* f1->name).f2; struct
_tagged_string _temp132=*(* f2->name).f2; xprintf("redeclaration of %.*s %.*s: field name mismatch %.*s != %.*s",
_temp129.last_plus_one - _temp129.curr, _temp129.curr, _temp130.last_plus_one -
_temp130.curr, _temp130.curr, _temp131.last_plus_one - _temp131.curr, _temp131.curr,
_temp132.last_plus_one - _temp132.curr, _temp132.curr);}));}{ struct Cyc_List_List*
tvs1= f1->tvs; struct Cyc_List_List* tvs2= f2->tvs; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs1) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp133= t; struct
_tagged_string _temp134=* v; struct _tagged_string _temp135=*(* f1->name).f2;
xprintf("redeclaration of %.*s %.*s, field %.*s: type parameter number mismatch",
_temp133.last_plus_one - _temp133.curr, _temp133.curr, _temp134.last_plus_one -
_temp134.curr, _temp134.curr, _temp135.last_plus_one - _temp135.curr, _temp135.curr);}));}
for( 0; tvs1 != 0; tvs1= tvs1->tl, tvs2= tvs2->tl){ inst=({ struct Cyc_List_List*
_temp136=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp136->hd=( void*)({ struct _tuple4* _temp137=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp137->f1=( struct Cyc_Absyn_Tvar*) tvs2->hd;
_temp137->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp138=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp138[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp139; _temp139.tag= Cyc_Absyn_VarType_tag; _temp139.f1=( struct Cyc_Absyn_Tvar*)
tvs1->hd; _temp139;}); _temp138;}); _temp137;}); _temp136->tl= inst; _temp136;});}{
struct Cyc_List_List* typs1= f1->typs; struct Cyc_List_List* typs2= f2->typs;
if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( typs1) !=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( typs2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp140= t; struct _tagged_string _temp141=* v; struct
_tagged_string _temp142=*(* f1->name).f2; xprintf("redeclaration of %.*s %.*s, field %.*s: parameter number mismatch",
_temp140.last_plus_one - _temp140.curr, _temp140.curr, _temp141.last_plus_one -
_temp141.curr, _temp141.curr, _temp142.last_plus_one - _temp142.curr, _temp142.curr);}));}
for( 0; typs1 != 0; typs1= typs1->tl, typs2= typs2->tl){ if( ! Cyc_Tcutil_equal_tqual((*((
struct _tuple5*) typs1->hd)).f1,(*(( struct _tuple5*) typs2->hd)).f1)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp143= t; struct _tagged_string _temp144=* v;
struct _tagged_string _temp145=*(* f1->name).f2; xprintf("redeclaration of %.*s %.*s, field %.*s: parameter qualifier",
_temp143.last_plus_one - _temp143.curr, _temp143.curr, _temp144.last_plus_one -
_temp144.curr, _temp144.curr, _temp145.last_plus_one - _temp145.curr, _temp145.curr);}));}{
void* subst_t2= Cyc_Tcutil_substitute( inst,(*(( struct _tuple5*) typs2->hd)).f2);
if( ! Cyc_Tcutil_unify((*(( struct _tuple5*) typs1->hd)).f2, subst_t2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp146= t; struct _tagged_string _temp147=* v;
struct _tagged_string _temp148=*(* f1->name).f2; struct _tagged_string _temp149=
Cyc_Absynpp_typ2string((*(( struct _tuple5*) typs1->hd)).f2); struct
_tagged_string _temp150= Cyc_Absynpp_typ2string( subst_t2); xprintf("redeclaration of %.*s %.*s, field %.*s: parameter type mismatch %.*s != %.*s",
_temp146.last_plus_one - _temp146.curr, _temp146.curr, _temp147.last_plus_one -
_temp147.curr, _temp147.curr, _temp148.last_plus_one - _temp148.curr, _temp148.curr,
_temp149.last_plus_one - _temp149.curr, _temp149.curr, _temp150.last_plus_one -
_temp150.curr, _temp150.curr);}));}}}}}} static int Cyc_Tc_var_redecl_okay(
struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_string* name, void* t, void* sc, struct Cyc_List_List* atts){ struct Cyc_Core_Opt*
ans=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup_opt)( ge->ordinaries, name); if( ans == 0){ return 0;}(*((
struct _tuple6*) ans->v)).f2= 1;{ void* _temp151=(*(( struct _tuple6*) ans->v)).f1;
void* _temp157; struct Cyc_Absyn_Vardecl* _temp159; _LL153: if((( struct
_tunion_struct*) _temp151)->tag == Cyc_Tcenv_VarRes_tag){ _LL158: _temp157=(
void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp151)->f1; if(( unsigned int)
_temp157 > 1u?(( struct _tunion_struct*) _temp157)->tag == Cyc_Absyn_Global_b_tag:
0){ _LL160: _temp159=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*)
_temp157)->f1; goto _LL154;} else{ goto _LL155;}} else{ goto _LL155;} _LL155:
goto _LL156; _LL154: if( sc ==( void*) Cyc_Absyn_Static?( void*) _temp159->sc !=(
void*) Cyc_Absyn_Static: 0){ Cyc_Tcutil_warn( loc,({ struct _tagged_string
_temp161=* name; xprintf("static declaration of %.*s follows non-static declaration",
_temp161.last_plus_one - _temp161.curr, _temp161.curr);}));} if(( void*)
_temp159->sc ==( void*) Cyc_Absyn_Static? sc !=( void*) Cyc_Absyn_Static: 0){
Cyc_Tcutil_warn( loc,({ struct _tagged_string _temp162=* name; xprintf("non-static declaration of %.*s follows static declaration",
_temp162.last_plus_one - _temp162.curr, _temp162.curr);}));} if((( void*)
_temp159->sc ==( void*) Cyc_Absyn_ExternC? sc !=( void*) Cyc_Absyn_ExternC: 0)?
1:(( void*) _temp159->sc !=( void*) Cyc_Absyn_ExternC? sc ==( void*) Cyc_Absyn_ExternC:
0)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp163=* name; xprintf("incompatible redeclaration of %.*s",
_temp163.last_plus_one - _temp163.curr, _temp163.curr);}));} else{ if((( sc !=(
void*) Cyc_Absyn_Extern?( void*) _temp159->sc !=( void*) Cyc_Absyn_Extern: 0)? !(
sc ==( void*) Cyc_Absyn_Static?( void*) _temp159->sc ==( void*) Cyc_Absyn_Static:
0): 0)? !( sc ==( void*) Cyc_Absyn_Public?( void*) _temp159->sc ==( void*) Cyc_Absyn_Public:
0): 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp164=* name; xprintf("incompatible redeclaration of %.*s",
_temp164.last_plus_one - _temp164.curr, _temp164.curr);}));}} if( ! Cyc_Tcutil_unify((
void*) _temp159->type, t)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp165=* name; struct _tagged_string _temp166= Cyc_Absynpp_typ2string( t);
struct _tagged_string _temp167= Cyc_Absynpp_typ2string(( void*) _temp159->type);
xprintf("redeclaration of %.*s at a different type: %.*s != %.*s", _temp165.last_plus_one
- _temp165.curr, _temp165.curr, _temp166.last_plus_one - _temp166.curr, _temp166.curr,
_temp167.last_plus_one - _temp167.curr, _temp167.curr);}));} if( ! Cyc_Tcutil_same_atts(
atts, _temp159->attributes)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp168=* name; xprintf("redeclaration of %.*s with different attributes",
_temp168.last_plus_one - _temp168.curr, _temp168.curr);}));} goto _LL152; _LL156:
goto _LL152; _LL152:;} return 1;} static int Cyc_Tc_is_function_type( void* t){
void* _temp169= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp175;
_LL171: if(( unsigned int) _temp169 > 4u?(( struct _tunion_struct*) _temp169)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL176: _temp175=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp169)->f1; goto _LL172;} else{ goto _LL173;}
_LL173: goto _LL174; _LL172: return 1; _LL174: return 0; _LL170:;} static struct
Cyc_List_List* Cyc_Tc_transfer_fn_type_atts( void* t, struct Cyc_List_List* atts){
void* _temp177= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp183;
struct Cyc_List_List* _temp185; struct Cyc_List_List** _temp187; int _temp188;
struct Cyc_List_List* _temp190; void* _temp192; struct Cyc_Core_Opt* _temp194;
struct Cyc_List_List* _temp196; _LL179: if(( unsigned int) _temp177 > 4u?((
struct _tunion_struct*) _temp177)->tag == Cyc_Absyn_FnType_tag: 0){ _LL184:
_temp183=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp177)->f1;
_LL197: _temp196=( struct Cyc_List_List*) _temp183.tvars; goto _LL195; _LL195:
_temp194=( struct Cyc_Core_Opt*) _temp183.effect; goto _LL193; _LL193: _temp192=(
void*) _temp183.ret_typ; goto _LL191; _LL191: _temp190=( struct Cyc_List_List*)
_temp183.args; goto _LL189; _LL189: _temp188=( int) _temp183.varargs; goto
_LL186; _LL186: _temp185=( struct Cyc_List_List*) _temp183.attributes; _temp187=&(((
struct Cyc_Absyn_FnType_struct*) _temp177)->f1).attributes; goto _LL180;} else{
goto _LL181;} _LL181: goto _LL182; _LL180: { struct Cyc_List_List* res_atts= 0;
for( 0; atts != 0; atts= atts->tl){ void* _temp198=( void*) atts->hd; int
_temp212; _LL200: if(( unsigned int) _temp198 > 15u?(( struct _tunion_struct*)
_temp198)->tag == Cyc_Absyn_Regparm_att_tag: 0){ _LL213: _temp212=( int)((
struct Cyc_Absyn_Regparm_att_struct*) _temp198)->f1; goto _LL201;} else{ goto
_LL202;} _LL202: if(( int) _temp198 == Cyc_Absyn_Stdcall_att){ goto _LL203;}
else{ goto _LL204;} _LL204: if(( int) _temp198 == Cyc_Absyn_Cdecl_att){ goto
_LL205;} else{ goto _LL206;} _LL206: if(( int) _temp198 == Cyc_Absyn_Noreturn_att){
goto _LL207;} else{ goto _LL208;} _LL208: if(( int) _temp198 == Cyc_Absyn_Const_att){
goto _LL209;} else{ goto _LL210;} _LL210: goto _LL211; _LL201: goto _LL203;
_LL203: goto _LL205; _LL205: goto _LL207; _LL207: goto _LL209; _LL209:* _temp187=({
struct Cyc_List_List* _temp214=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp214->hd=( void*)(( void*) atts->hd); _temp214->tl=*
_temp187; _temp214;}); continue; _LL211: res_atts=({ struct Cyc_List_List*
_temp215=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp215->hd=( void*)(( void*) atts->hd); _temp215->tl= res_atts; _temp215;});
continue; _LL199:;} return res_atts;} _LL182: return(( struct Cyc_List_List*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp216=( char*)"transfer_fn_type_atts"; struct _tagged_string _temp217;
_temp217.curr= _temp216; _temp217.base= _temp216; _temp217.last_plus_one=
_temp216 + 22; _temp217;})); _LL178:;} static void Cyc_Tc_tcVardecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Vardecl*
vd){ struct _tagged_string* v=(* vd->name).f2; void* t=( void*) vd->type; void*
sc=( void*) vd->sc; struct Cyc_List_List* atts= vd->attributes;{ void* _temp218=(*
vd->name).f1; struct Cyc_List_List* _temp226; struct Cyc_List_List* _temp228;
_LL220: if(( unsigned int) _temp218 > 1u?(( struct _tunion_struct*) _temp218)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL227: _temp226=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp218)->f1; if( _temp226 == 0){ goto _LL221;} else{
goto _LL222;}} else{ goto _LL222;} _LL222: if(( unsigned int) _temp218 > 1u?((
struct _tunion_struct*) _temp218)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL229:
_temp228=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp218)->f1;
if( _temp228 == 0){ goto _LL223;} else{ goto _LL224;}} else{ goto _LL224;}
_LL224: goto _LL225; _LL221: goto _LL219; _LL223: goto _LL219; _LL225: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp230=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp231; _temp231.curr= _temp230; _temp231.base= _temp230;
_temp231.last_plus_one= _temp230 + 43; _temp231;})); return; _LL219:;}(* vd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp232=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp232[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp233; _temp233.tag= Cyc_Absyn_Abs_n_tag; _temp233.f1= te->ns; _temp233;});
_temp232;});{ void* _temp234= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp*
_temp241; struct Cyc_Absyn_Tqual* _temp243; void* _temp245; _LL236: if((
unsigned int) _temp234 > 4u?(( struct _tunion_struct*) _temp234)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL246: _temp245=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp234)->f1;
goto _LL244; _LL244: _temp243=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp234)->f2; goto _LL242; _LL242: _temp241=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp234)->f3; if( _temp241 == 0){ goto _LL240;}
else{ goto _LL238;}} else{ goto _LL238;} _LL240: if( vd->initializer != 0){ goto
_LL237;} else{ goto _LL238;} _LL238: goto _LL239; _LL237:{ void* _temp247=( void*)(
vd->initializer)->r; void* _temp257; struct _tagged_string _temp259; struct Cyc_List_List*
_temp261; struct Cyc_Core_Opt* _temp263; struct Cyc_List_List* _temp265; _LL249:
if((( struct _tunion_struct*) _temp247)->tag == Cyc_Absyn_Const_e_tag){ _LL258:
_temp257=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp247)->f1; if((
unsigned int) _temp257 > 1u?(( struct _tunion_struct*) _temp257)->tag == Cyc_Absyn_String_c_tag:
0){ _LL260: _temp259=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp257)->f1; goto _LL250;} else{ goto _LL251;}} else{ goto _LL251;} _LL251:
if((( struct _tunion_struct*) _temp247)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL264: _temp263=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp247)->f1; goto _LL262; _LL262: _temp261=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp247)->f2; goto _LL252;} else{ goto
_LL253;} _LL253: if((( struct _tunion_struct*) _temp247)->tag == Cyc_Absyn_Array_e_tag){
_LL266: _temp265=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp247)->f1; goto _LL254;} else{ goto _LL255;} _LL255: goto _LL256; _LL250: t=(
void*)( vd->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp267=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp267[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp268; _temp268.tag= Cyc_Absyn_ArrayType_tag;
_temp268.f1=( void*) _temp245; _temp268.f2= _temp243; _temp268.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(({ struct _tagged_string _temp269= _temp259;( unsigned int)(
_temp269.last_plus_one - _temp269.curr);}), 0); _temp268;}); _temp267;}))); goto
_LL248; _LL252: _temp265= _temp261; goto _LL254; _LL254: t=( void*)( vd->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp270=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp270[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp271; _temp271.tag= Cyc_Absyn_ArrayType_tag;
_temp271.f1=( void*) _temp245; _temp271.f2= _temp243; _temp271.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp265), 0); _temp271;}); _temp270;}))); goto _LL248; _LL256: goto _LL248;
_LL248:;} goto _LL235; _LL239: goto _LL235; _LL235:;} Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t); if( Cyc_Tc_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts(
t, atts);}{ int is_redecl= Cyc_Tc_var_redecl_okay( ge, loc, v, t, sc, atts); if(
sc ==( void*) Cyc_Absyn_Extern? 1: sc ==( void*) Cyc_Absyn_ExternC){ if( vd->initializer
!= 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp272=( char*)"extern declaration should not have initializer";
struct _tagged_string _temp273; _temp273.curr= _temp272; _temp273.base= _temp272;
_temp273.last_plus_one= _temp272 + 47; _temp273;}));}} else{ if( ! Cyc_Tc_is_function_type(
t)){ for( 0; atts != 0; atts= atts->tl){ void* _temp274=( void*) atts->hd; int
_temp294; struct _tagged_string _temp296; _LL276: if(( unsigned int) _temp274 >
15u?(( struct _tunion_struct*) _temp274)->tag == Cyc_Absyn_Aligned_att_tag: 0){
_LL295: _temp294=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp274)->f1;
goto _LL277;} else{ goto _LL278;} _LL278: if(( unsigned int) _temp274 > 15u?((
struct _tunion_struct*) _temp274)->tag == Cyc_Absyn_Section_att_tag: 0){ _LL297:
_temp296=( struct _tagged_string)(( struct Cyc_Absyn_Section_att_struct*)
_temp274)->f1; goto _LL279;} else{ goto _LL280;} _LL280: if(( int) _temp274 ==
Cyc_Absyn_Nocommon_att){ goto _LL281;} else{ goto _LL282;} _LL282: if(( int)
_temp274 == Cyc_Absyn_Shared_att){ goto _LL283;} else{ goto _LL284;} _LL284: if((
int) _temp274 == Cyc_Absyn_Unused_att){ goto _LL285;} else{ goto _LL286;} _LL286:
if(( int) _temp274 == Cyc_Absyn_Weak_att){ goto _LL287;} else{ goto _LL288;}
_LL288: if(( int) _temp274 == Cyc_Absyn_Dllimport_att){ goto _LL289;} else{ goto
_LL290;} _LL290: if(( int) _temp274 == Cyc_Absyn_Dllexport_att){ goto _LL291;}
else{ goto _LL292;} _LL292: goto _LL293; _LL277: continue; _LL279: continue;
_LL281: continue; _LL283: continue; _LL285: continue; _LL287: continue; _LL289:
continue; _LL291: continue; _LL293: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp298= Cyc_Absyn_attribute2string(( void*) atts->hd); struct
_tagged_string _temp299= Cyc_Absynpp_qvar2string( vd->name); xprintf("bad attribute %.*s for variable %.*s",
_temp298.last_plus_one - _temp298.curr, _temp298.curr, _temp299.last_plus_one -
_temp299.curr, _temp299.curr);})); goto _LL275; _LL275:;}{ struct Cyc_Absyn_Exp*
e; if( vd->initializer == 0){ e= Cyc_Tcutil_default_initializer( te, t, loc); vd->initializer=(
struct Cyc_Absyn_Exp*) e;} else{ e=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp300= vd->initializer; if( _temp300 == 0){ _throw( Null_Exception);}
_temp300;});}{ void* t2= Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( !
Cyc_Tcutil_coerce_assign( te, e, t)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp301= Cyc_Absynpp_qvar2string( vd->name); struct
_tagged_string _temp302= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp303= Cyc_Absynpp_typ2string( t2); xprintf("%.*s is declared with type %.*s but initialized with type %.*s",
_temp301.last_plus_one - _temp301.curr, _temp301.curr, _temp302.last_plus_one -
_temp302.curr, _temp302.curr, _temp303.last_plus_one - _temp303.curr, _temp303.curr);}));}
if( ! Cyc_Tcutil_is_const_exp( te, e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp304=( char*)"initializer is not a constant expression";
struct _tagged_string _temp305; _temp305.curr= _temp304; _temp305.base= _temp304;
_temp305.last_plus_one= _temp304 + 41; _temp305;}));}}}} else{ for( 0; atts != 0;
atts= atts->tl){ void* _temp306=( void*) atts->hd; int _temp324; int _temp326;
_LL308: if(( unsigned int) _temp306 > 15u?(( struct _tunion_struct*) _temp306)->tag
== Cyc_Absyn_Regparm_att_tag: 0){ _LL325: _temp324=( int)(( struct Cyc_Absyn_Regparm_att_struct*)
_temp306)->f1; goto _LL309;} else{ goto _LL310;} _LL310: if(( int) _temp306 ==
Cyc_Absyn_Stdcall_att){ goto _LL311;} else{ goto _LL312;} _LL312: if(( int)
_temp306 == Cyc_Absyn_Cdecl_att){ goto _LL313;} else{ goto _LL314;} _LL314: if((
int) _temp306 == Cyc_Absyn_Noreturn_att){ goto _LL315;} else{ goto _LL316;}
_LL316: if(( int) _temp306 == Cyc_Absyn_Const_att){ goto _LL317;} else{ goto
_LL318;} _LL318: if(( unsigned int) _temp306 > 15u?(( struct _tunion_struct*)
_temp306)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL327: _temp326=( int)((
struct Cyc_Absyn_Aligned_att_struct*) _temp306)->f1; goto _LL319;} else{ goto
_LL320;} _LL320: if(( int) _temp306 == Cyc_Absyn_Packed_att){ goto _LL321;}
else{ goto _LL322;} _LL322: goto _LL323; _LL309: goto _LL311; _LL311: goto
_LL313; _LL313: goto _LL315; _LL315: goto _LL317; _LL317:(( void(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp328=(
char*)"tcVardecl: fn type attributes in function var decl"; struct
_tagged_string _temp329; _temp329.curr= _temp328; _temp329.base= _temp328;
_temp329.last_plus_one= _temp328 + 51; _temp329;})); goto _LL307; _LL319: goto
_LL321; _LL321: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp330= Cyc_Absyn_attribute2string((
void*) atts->hd); xprintf("bad attribute %.*s in function declaration", _temp330.last_plus_one
- _temp330.curr, _temp330.curr);})); goto _LL307; _LL323: continue; _LL307:;}}}
ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries, v,({
struct _tuple6* _temp331=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp331->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp332=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp332[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp333; _temp333.tag= Cyc_Tcenv_VarRes_tag; _temp333.f1=( void*)(( void*)({
struct Cyc_Absyn_Global_b_struct* _temp334=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp334[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp335; _temp335.tag= Cyc_Absyn_Global_b_tag;
_temp335.f1= vd; _temp335;}); _temp334;})); _temp333;}); _temp332;}); _temp331->f2=
is_redecl; _temp331;}));}} static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl*
fd){ struct _tagged_string* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void*
_temp336=(* fd->name).f1; struct Cyc_List_List* _temp344; struct Cyc_List_List*
_temp346; _LL338: if(( unsigned int) _temp336 > 1u?(( struct _tunion_struct*)
_temp336)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL345: _temp344=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp336)->f1; if( _temp344 == 0){ goto _LL339;}
else{ goto _LL340;}} else{ goto _LL340;} _LL340: if(( unsigned int) _temp336 > 1u?((
struct _tunion_struct*) _temp336)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL347:
_temp346=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp336)->f1;
goto _LL341;} else{ goto _LL342;} _LL342: goto _LL343; _LL339: goto _LL337;
_LL341:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp348=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp348[ 0]=({ struct Cyc_Core_Impossible_struct _temp349; _temp349.tag= Cyc_Core_Impossible_tag;
_temp349.f1=( struct _tagged_string)({ char* _temp350=( char*)"tc: Abs_n in tcFndecl";
struct _tagged_string _temp351; _temp351.curr= _temp350; _temp351.base= _temp350;
_temp351.last_plus_one= _temp350 + 22; _temp351;}); _temp349;}); _temp348;}));
_LL343: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp352=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp353; _temp353.curr= _temp352; _temp353.base= _temp352;
_temp353.last_plus_one= _temp352 + 43; _temp353;})); return; _LL337:;}(* fd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp354=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp354[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp355; _temp355.tag= Cyc_Absyn_Abs_n_tag; _temp355.f1= te->ns; _temp355;});
_temp354;}); Cyc_Tcutil_check_fndecl_valid_type( loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ(
fd); int is_redecl= Cyc_Tc_var_redecl_okay( ge, loc, v, t, sc, fd->attributes);{
struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts= atts->tl){
void* _temp356=( void*) atts->hd; int _temp364; _LL358: if(( int) _temp356 ==
Cyc_Absyn_Packed_att){ goto _LL359;} else{ goto _LL360;} _LL360: if((
unsigned int) _temp356 > 15u?(( struct _tunion_struct*) _temp356)->tag == Cyc_Absyn_Aligned_att_tag:
0){ _LL365: _temp364=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp356)->f1;
goto _LL361;} else{ goto _LL362;} _LL362: goto _LL363; _LL359: goto _LL361;
_LL361: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp366= Cyc_Absyn_attribute2string((
void*) atts->hd); xprintf("bad attribute %.*s for function", _temp366.last_plus_one
- _temp366.curr, _temp366.curr);})); goto _LL357; _LL363: goto _LL357; _LL357:;}}
ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries, v,({
struct _tuple6* _temp367=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp367->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp368=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp368[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp369; _temp369.tag= Cyc_Tcenv_VarRes_tag; _temp369.f1=( void*)(( void*)({
struct Cyc_Absyn_Funname_b_struct* _temp370=( struct Cyc_Absyn_Funname_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct)); _temp370[ 0]=({ struct
Cyc_Absyn_Funname_b_struct _temp371; _temp371.tag= Cyc_Absyn_Funname_b_tag;
_temp371.f1= fd; _temp371;}); _temp370;})); _temp369;}); _temp368;}); _temp367->f2=
is_redecl; _temp367;})); te->le=({ struct Cyc_Core_Opt* _temp372=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp372->v=( void*)(( void*)({ struct
Cyc_Tcenv_Outermost_struct* _temp373=( struct Cyc_Tcenv_Outermost_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct)); _temp373[ 0]=({ struct
Cyc_Tcenv_Outermost_struct _temp374; _temp374.tag= Cyc_Tcenv_Outermost_tag;
_temp374.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp374;}); _temp373;}));
_temp372;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp375=( char*)"function has goto statements to undefined labels";
struct _tagged_string _temp376; _temp376.curr= _temp375; _temp376.base= _temp375;
_temp376.last_plus_one= _temp375 + 49; _temp376;}));} te->le= 0;}} static void
Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_string* v=(* td->name).f2;{ void* _temp377=(* td->name).f1; struct Cyc_List_List*
_temp385; struct Cyc_List_List* _temp387; _LL379: if(( unsigned int) _temp377 >
1u?(( struct _tunion_struct*) _temp377)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL386:
_temp385=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp377)->f1;
if( _temp385 == 0){ goto _LL380;} else{ goto _LL381;}} else{ goto _LL381;}
_LL381: if(( unsigned int) _temp377 > 1u?(( struct _tunion_struct*) _temp377)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL388: _temp387=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp377)->f1; if( _temp387 == 0){ goto _LL382;} else{
goto _LL383;}} else{ goto _LL383;} _LL383: goto _LL384; _LL380: goto _LL378;
_LL382: goto _LL378; _LL384: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp389=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp390; _temp390.curr= _temp389; _temp390.base= _temp389;
_temp390.last_plus_one= _temp389 + 43; _temp390;})); return; _LL378:;} if((( int(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)( ge->typedefs,
v)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp391=* v; xprintf("redeclaration of typedef %.*s",
_temp391.last_plus_one - _temp391.curr, _temp391.curr);})); return;}(* td->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp392=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp392[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp393; _temp393.tag= Cyc_Absyn_Abs_n_tag; _temp393.f1= te->ns; _temp393;});
_temp392;}); Cyc_Tcutil_check_unique_tvars( loc, td->tvs); Cyc_Tcutil_check_type(
loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind,( void*) td->defn); ge->typedefs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs, v, td);}
static void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ if( sd->name
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp394=( char*)"anonymous structs are not allowed at top level";
struct _tagged_string _temp395; _temp395.curr= _temp394; _temp395.base= _temp394;
_temp395.last_plus_one= _temp394 + 47; _temp395;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)( sd->name)->v)).f2;{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts= atts->tl){ void* _temp396=( void*)
atts->hd; int _temp404; _LL398: if(( int) _temp396 == Cyc_Absyn_Packed_att){
goto _LL399;} else{ goto _LL400;} _LL400: if(( unsigned int) _temp396 > 15u?((
struct _tunion_struct*) _temp396)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL405:
_temp404=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp396)->f1; goto
_LL401;} else{ goto _LL402;} _LL402: goto _LL403; _LL399: continue; _LL401:
continue; _LL403: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp406= Cyc_Absyn_attribute2string((
void*) atts->hd); struct _tagged_string _temp407=* v; xprintf("bad attribute %.*s in struct %.*s definition",
_temp406.last_plus_one - _temp406.curr, _temp406.curr, _temp407.last_plus_one -
_temp407.curr, _temp407.curr);})); goto _LL397; _LL397:;}}{ struct Cyc_List_List*
tvs2= sd->tvs; for( 0; tvs2 != 0; tvs2= tvs2->tl){ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->kind); void* _temp408=( void*) c->v; void*
_temp416; _LL410: if(( int) _temp408 == Cyc_Absyn_No_constr){ goto _LL411;}
else{ goto _LL412;} _LL412: if(( unsigned int) _temp408 > 1u?(( struct
_tunion_struct*) _temp408)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL417: _temp416=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp408)->f1; if(( int) _temp416
== Cyc_Absyn_MemKind){ goto _LL413;} else{ goto _LL414;}} else{ goto _LL414;}
_LL414: goto _LL415; _LL411:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp418=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp418[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp419; _temp419.tag= Cyc_Absyn_Eq_constr_tag;
_temp419.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp419;}); _temp418;})));
continue; _LL413: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp420=* v;
struct _tagged_string _temp421=*(( struct Cyc_Absyn_Tvar*) tvs2->hd)->name;
xprintf("struct %.*s attempts to abstract type variable %.*s of kind M",
_temp420.last_plus_one - _temp420.curr, _temp420.curr, _temp421.last_plus_one -
_temp421.curr, _temp421.curr);})); continue; _LL415: continue; _LL409:;}}{
struct Cyc_List_List* tvs= sd->tvs;{ void* _temp422=(*(( struct _tuple0*)( sd->name)->v)).f1;
struct Cyc_List_List* _temp430; struct Cyc_List_List* _temp432; _LL424: if((
unsigned int) _temp422 > 1u?(( struct _tunion_struct*) _temp422)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL431: _temp430=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp422)->f1; if( _temp430 == 0){ goto _LL425;} else{ goto _LL426;}} else{ goto
_LL426;} _LL426: if(( unsigned int) _temp422 > 1u?(( struct _tunion_struct*)
_temp422)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL433: _temp432=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp422)->f1; if( _temp432 == 0){ goto _LL427;}
else{ goto _LL428;}} else{ goto _LL428;} _LL428: goto _LL429; _LL425: goto
_LL423; _LL427: goto _LL423; _LL429: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp434=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp435; _temp435.curr= _temp434; _temp435.base= _temp434;
_temp435.last_plus_one= _temp434 + 43; _temp435;})); return; _LL423:;}(*((
struct _tuple0*)( sd->name)->v)).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp436=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp436[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp437; _temp437.tag= Cyc_Absyn_Abs_n_tag;
_temp437.f1= te->ns; _temp437;}); _temp436;}); Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_Core_Opt* _temp438=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->structdecls, v);
struct Cyc_Core_Opt _temp444; struct Cyc_Absyn_Structdecl** _temp445; _LL440:
if( _temp438 == 0){ goto _LL441;} else{ goto _LL442;} _LL442: if( _temp438 == 0){
goto _LL439;} else{ _temp444=* _temp438; _LL446: _temp445=( struct Cyc_Absyn_Structdecl**)
_temp444.v; goto _LL443;} _LL441: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, v,({ struct Cyc_Absyn_Structdecl**
_temp447=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp447[ 0]= sd; _temp447;})); if( sd->fields == 0){ return;} goto _LL439;
_LL443: { struct Cyc_Absyn_Structdecl* sd2=* _temp445; struct Cyc_List_List*
tvs2= sd2->tvs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs)
!=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp448=* v; xprintf("redeclaration of struct %.*s has a different number of type parameters",
_temp448.last_plus_one - _temp448.curr, _temp448.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1= x1->tl, x2= x2->tl){
struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x1->hd)->kind); struct
Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x2->hd)->kind); if(( void*)
c1->v ==( void*) Cyc_Absyn_No_constr? c1 != c2: 0){( void*)( c1->v=( void*)((
void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp449=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp449[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp450; _temp450.tag= Cyc_Absyn_Forward_constr_tag;
_temp450.f1= c2; _temp450;}); _temp449;})));} if((( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp451=* v; struct
_tagged_string _temp452= Cyc_Absynpp_ckind2string( c1); struct _tagged_string
_temp453=*(( struct Cyc_Absyn_Tvar*) x1->hd)->name; struct _tagged_string
_temp454= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of struct %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp451.last_plus_one - _temp451.curr, _temp451.curr, _temp452.last_plus_one -
_temp452.curr, _temp452.curr, _temp453.last_plus_one - _temp453.curr, _temp453.curr,
_temp454.last_plus_one - _temp454.curr, _temp454.curr);}));}}( void*)( sd->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) sd->sc, sd->fields,( void*)
sd2->sc, sd2->fields, loc,( struct _tagged_string)({ char* _temp455=( char*)"struct";
struct _tagged_string _temp456; _temp456.curr= _temp455; _temp456.base= _temp455;
_temp456.last_plus_one= _temp455 + 7; _temp456;}), v));{ struct _tuple7 _temp458=({
struct _tuple7 _temp457; _temp457.f1= sd->fields; _temp457.f2= sd2->fields;
_temp457;}); struct Cyc_Core_Opt* _temp468; struct Cyc_Core_Opt* _temp470;
struct Cyc_Core_Opt* _temp472; struct Cyc_Core_Opt* _temp474; struct Cyc_Core_Opt*
_temp476; struct Cyc_Core_Opt* _temp478; struct Cyc_Core_Opt* _temp480; struct
Cyc_Core_Opt* _temp482; _LL460: _LL471: _temp470= _temp458.f1; if( _temp470 == 0){
goto _LL469;} else{ goto _LL462;} _LL469: _temp468= _temp458.f2; if( _temp468 ==
0){ goto _LL461;} else{ goto _LL462;} _LL462: _LL475: _temp474= _temp458.f1;
goto _LL473; _LL473: _temp472= _temp458.f2; if( _temp472 == 0){ goto _LL463;}
else{ goto _LL464;} _LL464: _LL479: _temp478= _temp458.f1; if( _temp478 == 0){
goto _LL477;} else{ goto _LL466;} _LL477: _temp476= _temp458.f2; goto _LL465;
_LL466: _LL483: _temp482= _temp458.f1; goto _LL481; _LL481: _temp480= _temp458.f2;
goto _LL467; _LL461: return; _LL463:* _temp445= sd; goto _LL459; _LL465: sd->fields=
sd2->fields; sd->tvs= tvs; return; _LL467: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0= tvs0->tl, tvs2= tvs2->tl){
inst=({ struct Cyc_List_List* _temp484=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp484->hd=( void*)({ struct _tuple4* _temp485=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp485->f1=( struct Cyc_Absyn_Tvar*)
tvs2->hd; _temp485->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp486=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp486[ 0]=({ struct Cyc_Absyn_VarType_struct _temp487; _temp487.tag= Cyc_Absyn_VarType_tag;
_temp487.f1=( struct Cyc_Absyn_Tvar*) tvs0->hd; _temp487;}); _temp486;});
_temp485;}); _temp484->tl= inst; _temp484;});}} if( ! Cyc_Tcutil_same_atts( sd->attributes,
sd2->attributes)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp488=* v;
xprintf("redeclaration of struct %.*s has different attributes", _temp488.last_plus_one
- _temp488.curr, _temp488.curr);}));}{ struct Cyc_List_List* f1s=( struct Cyc_List_List*)(
sd->fields)->v; struct Cyc_List_List* f2s=( struct Cyc_List_List*)( sd2->fields)->v;
for( 0; f1s != 0? f2s != 0: 0; f1s= f1s->tl, f2s= f2s->tl){ struct Cyc_Absyn_Structfield
_temp491; struct Cyc_List_List* _temp492; struct Cyc_Core_Opt* _temp494; void*
_temp496; struct Cyc_Absyn_Tqual* _temp498; struct _tagged_string* _temp500;
struct Cyc_Absyn_Structfield* _temp489=( struct Cyc_Absyn_Structfield*) f1s->hd;
_temp491=* _temp489; _LL501: _temp500=( struct _tagged_string*) _temp491.name;
goto _LL499; _LL499: _temp498=( struct Cyc_Absyn_Tqual*) _temp491.tq; goto
_LL497; _LL497: _temp496=( void*) _temp491.type; goto _LL495; _LL495: _temp494=(
struct Cyc_Core_Opt*) _temp491.width; goto _LL493; _LL493: _temp492=( struct Cyc_List_List*)
_temp491.attributes; goto _LL490; _LL490: { struct Cyc_Absyn_Structfield
_temp504; struct Cyc_List_List* _temp505; struct Cyc_Core_Opt* _temp507; void*
_temp509; struct Cyc_Absyn_Tqual* _temp511; struct _tagged_string* _temp513;
struct Cyc_Absyn_Structfield* _temp502=( struct Cyc_Absyn_Structfield*) f2s->hd;
_temp504=* _temp502; _LL514: _temp513=( struct _tagged_string*) _temp504.name;
goto _LL512; _LL512: _temp511=( struct Cyc_Absyn_Tqual*) _temp504.tq; goto
_LL510; _LL510: _temp509=( void*) _temp504.type; goto _LL508; _LL508: _temp507=(
struct Cyc_Core_Opt*) _temp504.width; goto _LL506; _LL506: _temp505=( struct Cyc_List_List*)
_temp504.attributes; goto _LL503; _LL503: if( Cyc_String_zstrptrcmp( _temp500,
_temp513) != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp515=* v;
struct _tagged_string _temp516=* _temp500; struct _tagged_string _temp517=*
_temp513; xprintf("redeclaration of struct %.*s: field name mismatch %.*s != %.*s",
_temp515.last_plus_one - _temp515.curr, _temp515.curr, _temp516.last_plus_one -
_temp516.curr, _temp516.curr, _temp517.last_plus_one - _temp517.curr, _temp517.curr);}));}
if( ! Cyc_Tcutil_same_atts( _temp492, _temp505)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp518=* v; struct _tagged_string _temp519=* _temp500; xprintf("redeclaration of struct %.*s: attribute mismatch on field %.*s",
_temp518.last_plus_one - _temp518.curr, _temp518.curr, _temp519.last_plus_one -
_temp519.curr, _temp519.curr);}));} if( ! Cyc_Tcutil_equal_tqual( _temp498,
_temp511)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp520=* v; struct
_tagged_string _temp521=* _temp500; xprintf("redeclaration of struct %.*s: qualifier mismatch on field %.*s",
_temp520.last_plus_one - _temp520.curr, _temp520.curr, _temp521.last_plus_one -
_temp521.curr, _temp521.curr);}));} if( _temp494 != 0? ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*) _temp494->v): 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp522=* v; struct _tagged_string _temp523=* _temp500; xprintf("redeclaration of struct %.*s: bad bitfield %.*s",
_temp522.last_plus_one - _temp522.curr, _temp522.curr, _temp523.last_plus_one -
_temp523.curr, _temp523.curr);}));} else{ if((( _temp494 != 0? _temp507 != 0: 0)?
Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _temp494->v) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _temp507->v): 0)? 1: _temp494 != _temp507){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp524=* v; struct _tagged_string _temp525=*
_temp500; xprintf("redeclaration of struct %.*s: bitfield mismatch on field %.*s",
_temp524.last_plus_one - _temp524.curr, _temp524.curr, _temp525.last_plus_one -
_temp525.curr, _temp525.curr);}));}} Cyc_Tcutil_check_type( loc, te, tvs,( void*)
Cyc_Absyn_MemKind, _temp496);{ void* subst_t2= Cyc_Tcutil_substitute( inst,
_temp509); if( ! Cyc_Tcutil_unify( _temp496, subst_t2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp526=* v; struct _tagged_string _temp527=* _temp500;
struct _tagged_string _temp528= Cyc_Absynpp_typ2string( _temp496); struct
_tagged_string _temp529= Cyc_Absynpp_typ2string( subst_t2); xprintf("redeclaration of struct %.*s: type mismatch on field %.*s: %.*s != %.*s",
_temp526.last_plus_one - _temp526.curr, _temp526.curr, _temp527.last_plus_one -
_temp527.curr, _temp527.curr, _temp528.last_plus_one - _temp528.curr, _temp528.curr,
_temp529.last_plus_one - _temp529.curr, _temp529.curr);}));}}}} if( f2s != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp530=* v; struct
_tagged_string _temp531=*(( struct Cyc_Absyn_Structfield*) f2s->hd)->name;
xprintf("redeclaration of struct %.*s is missing field %.*s", _temp530.last_plus_one
- _temp530.curr, _temp530.curr, _temp531.last_plus_one - _temp531.curr, _temp531.curr);}));}
if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp532=* v;
struct _tagged_string _temp533=*(( struct Cyc_Absyn_Structfield*) f1s->hd)->name;
xprintf("redeclaration of struct %.*s has extra field %.*s", _temp532.last_plus_one
- _temp532.curr, _temp532.curr, _temp533.last_plus_one - _temp533.curr, _temp533.curr);}));}
return;}} _LL459:;} goto _LL439;}} _LL439:;}{ struct Cyc_Absyn_Structdecl** sdp=((
struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->structdecls, v);* sdp=({ struct Cyc_Absyn_Structdecl*
_temp534=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp534->sc=( void*)(( void*) sd->sc); _temp534->name= sd->name; _temp534->tvs=
tvs; _temp534->fields= 0; _temp534->attributes= 0; _temp534;});{ struct Cyc_List_List*
prev_fields= 0;{ struct Cyc_List_List* fs=( struct Cyc_List_List*)( sd->fields)->v;
for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Structfield _temp537; struct Cyc_List_List*
_temp538; struct Cyc_Core_Opt* _temp540; void* _temp542; struct Cyc_Absyn_Tqual*
_temp544; struct _tagged_string* _temp546; struct Cyc_Absyn_Structfield*
_temp535=( struct Cyc_Absyn_Structfield*) fs->hd; _temp537=* _temp535; _LL547:
_temp546=( struct _tagged_string*) _temp537.name; goto _LL545; _LL545: _temp544=(
struct Cyc_Absyn_Tqual*) _temp537.tq; goto _LL543; _LL543: _temp542=( void*)
_temp537.type; goto _LL541; _LL541: _temp540=( struct Cyc_Core_Opt*) _temp537.width;
goto _LL539; _LL539: _temp538=( struct Cyc_List_List*) _temp537.attributes; goto
_LL536; _LL536: if((( int(*)( int(* compare)( struct _tagged_string*, struct
_tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp546)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp548=* _temp546; xprintf("duplicate field %.*s in struct",
_temp548.last_plus_one - _temp548.curr, _temp548.curr);}));} if( Cyc_String_strcmp(*
_temp546,( struct _tagged_string)({ char* _temp549=( char*)""; struct
_tagged_string _temp550; _temp550.curr= _temp549; _temp550.base= _temp549;
_temp550.last_plus_one= _temp549 + 1; _temp550;})) != 0){ prev_fields=({ struct
Cyc_List_List* _temp551=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp551->hd=( void*) _temp546; _temp551->tl= prev_fields; _temp551;});} Cyc_Tcutil_check_type(
loc, te, tvs,( void*) Cyc_Absyn_MemKind, _temp542); if( _temp540 != 0){
unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_temp540->v)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp552=* _temp546;
xprintf("bitfield %.*s does not have constant width", _temp552.last_plus_one -
_temp552.curr, _temp552.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _temp540->v);}{ void* _temp553= Cyc_Tcutil_compress(
_temp542); void* _temp559; void* _temp561; _LL555: if(( unsigned int) _temp553 >
4u?(( struct _tunion_struct*) _temp553)->tag == Cyc_Absyn_IntType_tag: 0){
_LL562: _temp561=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp553)->f1;
goto _LL560; _LL560: _temp559=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp553)->f2; goto _LL556;} else{ goto _LL557;} _LL557: goto _LL558; _LL556:{
void* _temp563= _temp559; _LL565: if(( int) _temp563 == Cyc_Absyn_B1){ goto
_LL566;} else{ goto _LL567;} _LL567: if(( int) _temp563 == Cyc_Absyn_B2){ goto
_LL568;} else{ goto _LL569;} _LL569: if(( int) _temp563 == Cyc_Absyn_B4){ goto
_LL570;} else{ goto _LL571;} _LL571: if(( int) _temp563 == Cyc_Absyn_B8){ goto
_LL572;} else{ goto _LL564;} _LL566: if( w >( unsigned int) 8){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp573=( char*)"bitfield larger than type";
struct _tagged_string _temp574; _temp574.curr= _temp573; _temp574.base= _temp573;
_temp574.last_plus_one= _temp573 + 26; _temp574;}));} goto _LL564; _LL568: if( w
>( unsigned int) 16){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp575=( char*)"bitfield larger than type"; struct _tagged_string _temp576;
_temp576.curr= _temp575; _temp576.base= _temp575; _temp576.last_plus_one=
_temp575 + 26; _temp576;}));} goto _LL564; _LL570: if( w >( unsigned int) 32){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp577=( char*)"bitfield larger than type";
struct _tagged_string _temp578; _temp578.curr= _temp577; _temp578.base= _temp577;
_temp578.last_plus_one= _temp577 + 26; _temp578;}));} goto _LL564; _LL572: if( w
>( unsigned int) 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp579=( char*)"bitfield larger than type"; struct _tagged_string _temp580;
_temp580.curr= _temp579; _temp580.base= _temp579; _temp580.last_plus_one=
_temp579 + 26; _temp580;}));} goto _LL564; _LL564:;} goto _LL554; _LL558: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp581=* _temp546; struct _tagged_string _temp582=
Cyc_Absynpp_typ2string( _temp542); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp581.last_plus_one - _temp581.curr, _temp581.curr, _temp582.last_plus_one -
_temp582.curr, _temp582.curr);})); goto _LL554; _LL554:;}} for( 0; _temp538 != 0;
_temp538= _temp538->tl){ void* _temp583=( void*) _temp538->hd; int _temp591;
_LL585: if(( int) _temp583 == Cyc_Absyn_Packed_att){ goto _LL586;} else{ goto
_LL587;} _LL587: if(( unsigned int) _temp583 > 15u?(( struct _tunion_struct*)
_temp583)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL592: _temp591=( int)((
struct Cyc_Absyn_Aligned_att_struct*) _temp583)->f1; goto _LL588;} else{ goto
_LL589;} _LL589: goto _LL590; _LL586: continue; _LL588: continue; _LL590: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp593= Cyc_Absyn_attribute2string(( void*)
_temp538->hd); struct _tagged_string _temp594=* v; struct _tagged_string
_temp595=* _temp546; xprintf("bad attribute %.*s in struct %.*s, member %.*s",
_temp593.last_plus_one - _temp593.curr, _temp593.curr, _temp594.last_plus_one -
_temp594.curr, _temp594.curr, _temp595.last_plus_one - _temp595.curr, _temp595.curr);}));
goto _LL584; _LL584:;}}} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple6* _temp596=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp596->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct* _temp597=(
struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp597[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp598; _temp598.tag= Cyc_Tcenv_StructRes_tag;
_temp598.f1= sd; _temp598;}); _temp597;}); _temp596->f2= 1; _temp596;}));* sdp=
sd;}}}}} static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ if( ud->name
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp599=( char*)"anonymous unions are not allowed at top level";
struct _tagged_string _temp600; _temp600.curr= _temp599; _temp600.base= _temp599;
_temp600.last_plus_one= _temp599 + 46; _temp600;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)( ud->name)->v)).f2;{ struct Cyc_List_List*
atts= ud->attributes; for( 0; atts != 0; atts= atts->tl){ void* _temp601=( void*)
atts->hd; int _temp609; _LL603: if(( int) _temp601 == Cyc_Absyn_Packed_att){
goto _LL604;} else{ goto _LL605;} _LL605: if(( unsigned int) _temp601 > 15u?((
struct _tunion_struct*) _temp601)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL610:
_temp609=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp601)->f1; goto
_LL606;} else{ goto _LL607;} _LL607: goto _LL608; _LL604: continue; _LL606:
continue; _LL608: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp611= Cyc_Absyn_attribute2string((
void*) atts->hd); struct _tagged_string _temp612=* v; xprintf("bad attribute %.*s in union %.*s definition",
_temp611.last_plus_one - _temp611.curr, _temp611.curr, _temp612.last_plus_one -
_temp612.curr, _temp612.curr);})); goto _LL602; _LL602:;}}{ struct Cyc_List_List*
tvs2= ud->tvs; for( 0; tvs2 != 0; tvs2= tvs2->tl){ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->kind); void* _temp613=( void*) c->v; void*
_temp621; _LL615: if(( int) _temp613 == Cyc_Absyn_No_constr){ goto _LL616;}
else{ goto _LL617;} _LL617: if(( unsigned int) _temp613 > 1u?(( struct
_tunion_struct*) _temp613)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL622: _temp621=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp613)->f1; if(( int) _temp621
== Cyc_Absyn_MemKind){ goto _LL618;} else{ goto _LL619;}} else{ goto _LL619;}
_LL619: goto _LL620; _LL616:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp623=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp623[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp624; _temp624.tag= Cyc_Absyn_Eq_constr_tag;
_temp624.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp624;}); _temp623;})));
continue; _LL618: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp625=* v;
struct _tagged_string _temp626=*(( struct Cyc_Absyn_Tvar*) tvs2->hd)->name;
xprintf("union %.*s attempts to abstract type variable %.*s of kind M", _temp625.last_plus_one
- _temp625.curr, _temp625.curr, _temp626.last_plus_one - _temp626.curr, _temp626.curr);}));
continue; _LL620: continue; _LL614:;}}{ struct Cyc_List_List* tvs= ud->tvs;{
void* _temp627=(*(( struct _tuple0*)( ud->name)->v)).f1; struct Cyc_List_List*
_temp635; struct Cyc_List_List* _temp637; _LL629: if(( unsigned int) _temp627 >
1u?(( struct _tunion_struct*) _temp627)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL636:
_temp635=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp627)->f1;
if( _temp635 == 0){ goto _LL630;} else{ goto _LL631;}} else{ goto _LL631;}
_LL631: if(( unsigned int) _temp627 > 1u?(( struct _tunion_struct*) _temp627)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL638: _temp637=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp627)->f1; if( _temp637 == 0){ goto _LL632;} else{
goto _LL633;}} else{ goto _LL633;} _LL633: goto _LL634; _LL630: goto _LL628;
_LL632: goto _LL628; _LL634: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp639=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp640; _temp640.curr= _temp639; _temp640.base= _temp639;
_temp640.last_plus_one= _temp639 + 43; _temp640;})); return; _LL628:;}(*((
struct _tuple0*)( ud->name)->v)).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp641=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp641[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp642; _temp642.tag= Cyc_Absyn_Abs_n_tag;
_temp642.f1= te->ns; _temp642;}); _temp641;}); Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_Core_Opt* _temp643=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->uniondecls, v); struct
Cyc_Core_Opt _temp649; struct Cyc_Absyn_Uniondecl** _temp650; _LL645: if(
_temp643 == 0){ goto _LL646;} else{ goto _LL647;} _LL647: if( _temp643 == 0){
goto _LL644;} else{ _temp649=* _temp643; _LL651: _temp650=( struct Cyc_Absyn_Uniondecl**)
_temp649.v; goto _LL648;} _LL646: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl**
data)) Cyc_Dict_insert)( ge->uniondecls, v,({ struct Cyc_Absyn_Uniondecl**
_temp652=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp652[ 0]= ud; _temp652;})); if( ud->fields == 0){ return;} goto _LL644;
_LL648: { struct Cyc_Absyn_Uniondecl* ud2=* _temp650; struct Cyc_List_List* tvs2=
ud2->tvs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs) !=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp653=* v; xprintf("redeclaration of union %.*s has a different number of type parameters",
_temp653.last_plus_one - _temp653.curr, _temp653.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1= x1->tl, x2= x2->tl){
struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x1->hd)->kind); struct
Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x2->hd)->kind); if(( void*)
c1->v ==( void*) Cyc_Absyn_No_constr? c1 != c2: 0){( void*)( c1->v=( void*)((
void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp654=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp654[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp655; _temp655.tag= Cyc_Absyn_Forward_constr_tag;
_temp655.f1= c2; _temp655;}); _temp654;})));} if((( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp656=* v; struct
_tagged_string _temp657= Cyc_Absynpp_ckind2string( c1); struct _tagged_string
_temp658=*(( struct Cyc_Absyn_Tvar*) x1->hd)->name; struct _tagged_string
_temp659= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of union %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp656.last_plus_one - _temp656.curr, _temp656.curr, _temp657.last_plus_one -
_temp657.curr, _temp657.curr, _temp658.last_plus_one - _temp658.curr, _temp658.curr,
_temp659.last_plus_one - _temp659.curr, _temp659.curr);}));}}( void*)( ud->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ud->sc, ud->fields,( void*)
ud2->sc, ud2->fields, loc,( struct _tagged_string)({ char* _temp660=( char*)"union";
struct _tagged_string _temp661; _temp661.curr= _temp660; _temp661.base= _temp660;
_temp661.last_plus_one= _temp660 + 6; _temp661;}), v));{ struct _tuple7 _temp663=({
struct _tuple7 _temp662; _temp662.f1= ud->fields; _temp662.f2= ud2->fields;
_temp662;}); struct Cyc_Core_Opt* _temp673; struct Cyc_Core_Opt* _temp675;
struct Cyc_Core_Opt* _temp677; struct Cyc_Core_Opt* _temp679; struct Cyc_Core_Opt*
_temp681; struct Cyc_Core_Opt* _temp683; struct Cyc_Core_Opt* _temp685; struct
Cyc_Core_Opt* _temp687; _LL665: _LL676: _temp675= _temp663.f1; if( _temp675 == 0){
goto _LL674;} else{ goto _LL667;} _LL674: _temp673= _temp663.f2; if( _temp673 ==
0){ goto _LL666;} else{ goto _LL667;} _LL667: _LL680: _temp679= _temp663.f1;
goto _LL678; _LL678: _temp677= _temp663.f2; if( _temp677 == 0){ goto _LL668;}
else{ goto _LL669;} _LL669: _LL684: _temp683= _temp663.f1; if( _temp683 == 0){
goto _LL682;} else{ goto _LL671;} _LL682: _temp681= _temp663.f2; goto _LL670;
_LL671: _LL688: _temp687= _temp663.f1; goto _LL686; _LL686: _temp685= _temp663.f2;
goto _LL672; _LL666: return; _LL668:* _temp650= ud; goto _LL664; _LL670: ud->fields=
ud2->fields; ud->tvs= tvs; return; _LL672: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0= tvs0->tl, tvs2= tvs2->tl){
inst=({ struct Cyc_List_List* _temp689=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp689->hd=( void*)({ struct _tuple4* _temp690=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp690->f1=( struct Cyc_Absyn_Tvar*)
tvs2->hd; _temp690->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp691=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp691[ 0]=({ struct Cyc_Absyn_VarType_struct _temp692; _temp692.tag= Cyc_Absyn_VarType_tag;
_temp692.f1=( struct Cyc_Absyn_Tvar*) tvs0->hd; _temp692;}); _temp691;});
_temp690;}); _temp689->tl= inst; _temp689;});}} if( ! Cyc_Tcutil_same_atts( ud->attributes,
ud2->attributes)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp693=* v;
xprintf("redeclaration of union %.*s has different attributes", _temp693.last_plus_one
- _temp693.curr, _temp693.curr);}));}{ struct Cyc_List_List* f1s=( struct Cyc_List_List*)(
ud->fields)->v; struct Cyc_List_List* f2s=( struct Cyc_List_List*)( ud2->fields)->v;
for( 0; f1s != 0? f2s != 0: 0; f1s= f1s->tl, f2s= f2s->tl){ struct Cyc_Absyn_Structfield
_temp696; struct Cyc_List_List* _temp697; struct Cyc_Core_Opt* _temp699; void*
_temp701; struct Cyc_Absyn_Tqual* _temp703; struct _tagged_string* _temp705;
struct Cyc_Absyn_Structfield* _temp694=( struct Cyc_Absyn_Structfield*) f1s->hd;
_temp696=* _temp694; _LL706: _temp705=( struct _tagged_string*) _temp696.name;
goto _LL704; _LL704: _temp703=( struct Cyc_Absyn_Tqual*) _temp696.tq; goto
_LL702; _LL702: _temp701=( void*) _temp696.type; goto _LL700; _LL700: _temp699=(
struct Cyc_Core_Opt*) _temp696.width; goto _LL698; _LL698: _temp697=( struct Cyc_List_List*)
_temp696.attributes; goto _LL695; _LL695: { struct Cyc_Absyn_Structfield
_temp709; struct Cyc_List_List* _temp710; struct Cyc_Core_Opt* _temp712; void*
_temp714; struct Cyc_Absyn_Tqual* _temp716; struct _tagged_string* _temp718;
struct Cyc_Absyn_Structfield* _temp707=( struct Cyc_Absyn_Structfield*) f2s->hd;
_temp709=* _temp707; _LL719: _temp718=( struct _tagged_string*) _temp709.name;
goto _LL717; _LL717: _temp716=( struct Cyc_Absyn_Tqual*) _temp709.tq; goto
_LL715; _LL715: _temp714=( void*) _temp709.type; goto _LL713; _LL713: _temp712=(
struct Cyc_Core_Opt*) _temp709.width; goto _LL711; _LL711: _temp710=( struct Cyc_List_List*)
_temp709.attributes; goto _LL708; _LL708: if( Cyc_String_zstrptrcmp( _temp705,
_temp718) != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp720=* v;
struct _tagged_string _temp721=* _temp705; struct _tagged_string _temp722=*
_temp718; xprintf("redeclaration of union %.*s: field name mismatch %.*s != %.*s",
_temp720.last_plus_one - _temp720.curr, _temp720.curr, _temp721.last_plus_one -
_temp721.curr, _temp721.curr, _temp722.last_plus_one - _temp722.curr, _temp722.curr);}));}
if( ! Cyc_Tcutil_same_atts( _temp697, _temp710)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp723=* v; struct _tagged_string _temp724=* _temp705; xprintf("redeclaration of union %.*s: attribute mismatch on field %.*s",
_temp723.last_plus_one - _temp723.curr, _temp723.curr, _temp724.last_plus_one -
_temp724.curr, _temp724.curr);}));} if( ! Cyc_Tcutil_equal_tqual( _temp703,
_temp716)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp725=* v; struct
_tagged_string _temp726=* _temp705; xprintf("redeclaration of union %.*s: qualifier mismatch on field %.*s",
_temp725.last_plus_one - _temp725.curr, _temp725.curr, _temp726.last_plus_one -
_temp726.curr, _temp726.curr);}));} if( _temp699 != 0? ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*) _temp699->v): 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp727=* v; struct _tagged_string _temp728=* _temp705; xprintf("redeclaration of struct %.*s: bad bitfield %.*s",
_temp727.last_plus_one - _temp727.curr, _temp727.curr, _temp728.last_plus_one -
_temp728.curr, _temp728.curr);}));} else{ if((( _temp699 != 0? _temp712 != 0: 0)?
Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*) _temp699->v) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _temp712->v): 0)? 1: _temp699 != _temp712){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp729=* v; struct _tagged_string _temp730=*
_temp705; xprintf("redeclaration of struct %.*s: bitfield mismatch on field %.*s",
_temp729.last_plus_one - _temp729.curr, _temp729.curr, _temp730.last_plus_one -
_temp730.curr, _temp730.curr);}));}} Cyc_Tcutil_check_type( loc, te, tvs,( void*)
Cyc_Absyn_MemKind, _temp701);{ void* subst_t2= Cyc_Tcutil_substitute( inst,
_temp714); if( ! Cyc_Tcutil_unify( _temp701, subst_t2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp731=* v; struct _tagged_string _temp732=* _temp705;
struct _tagged_string _temp733= Cyc_Absynpp_typ2string( _temp701); struct
_tagged_string _temp734= Cyc_Absynpp_typ2string( subst_t2); xprintf("redeclaration of union %.*s: type mismatch on field %.*s: %.*s != %.*s",
_temp731.last_plus_one - _temp731.curr, _temp731.curr, _temp732.last_plus_one -
_temp732.curr, _temp732.curr, _temp733.last_plus_one - _temp733.curr, _temp733.curr,
_temp734.last_plus_one - _temp734.curr, _temp734.curr);}));}}}} if( f2s != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp735=* v; struct
_tagged_string _temp736=*(( struct Cyc_Absyn_Structfield*) f2s->hd)->name;
xprintf("redeclaration of union %.*s is missing field %.*s", _temp735.last_plus_one
- _temp735.curr, _temp735.curr, _temp736.last_plus_one - _temp736.curr, _temp736.curr);}));}
if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp737=* v;
struct _tagged_string _temp738=*(( struct Cyc_Absyn_Structfield*) f1s->hd)->name;
xprintf("redeclaration of union %.*s has extra field %.*s", _temp737.last_plus_one
- _temp737.curr, _temp737.curr, _temp738.last_plus_one - _temp738.curr, _temp738.curr);}));}
return;}} _LL664:;} goto _LL644;}} _LL644:;}{ struct Cyc_Absyn_Uniondecl** udp=((
struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->uniondecls, v);* udp=({ struct Cyc_Absyn_Uniondecl*
_temp739=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp739->sc=( void*)(( void*) ud->sc); _temp739->name= ud->name; _temp739->tvs=
tvs; _temp739->fields= 0; _temp739->attributes= 0; _temp739;});{ struct Cyc_List_List*
prev_fields= 0; struct Cyc_List_List* prev_types= 0;{ struct Cyc_List_List* fs=(
struct Cyc_List_List*)( ud->fields)->v; for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Structfield
_temp742; struct Cyc_List_List* _temp743; struct Cyc_Core_Opt* _temp745; void*
_temp747; struct Cyc_Absyn_Tqual* _temp749; struct _tagged_string* _temp751;
struct Cyc_Absyn_Structfield* _temp740=( struct Cyc_Absyn_Structfield*) fs->hd;
_temp742=* _temp740; _LL752: _temp751=( struct _tagged_string*) _temp742.name;
goto _LL750; _LL750: _temp749=( struct Cyc_Absyn_Tqual*) _temp742.tq; goto
_LL748; _LL748: _temp747=( void*) _temp742.type; goto _LL746; _LL746: _temp745=(
struct Cyc_Core_Opt*) _temp742.width; goto _LL744; _LL744: _temp743=( struct Cyc_List_List*)
_temp742.attributes; goto _LL741; _LL741: if((( int(*)( int(* compare)( struct
_tagged_string*, struct _tagged_string*), struct Cyc_List_List* l, struct
_tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields, _temp751)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp753=* _temp751; xprintf("duplicate field %.*s in union",
_temp753.last_plus_one - _temp753.curr, _temp753.curr);}));} prev_fields=({
struct Cyc_List_List* _temp754=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp754->hd=( void*) _temp751; _temp754->tl=
prev_fields; _temp754;}); Cyc_Tcutil_check_type( loc, te, tvs,( void*) Cyc_Absyn_MemKind,
_temp747); if( _temp745 != 0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*) _temp745->v)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp755=* _temp751; xprintf("bitfield %.*s does not have constant width",
_temp755.last_plus_one - _temp755.curr, _temp755.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _temp745->v);}{ void* _temp756= Cyc_Tcutil_compress(
_temp747); void* _temp762; void* _temp764; _LL758: if(( unsigned int) _temp756 >
4u?(( struct _tunion_struct*) _temp756)->tag == Cyc_Absyn_IntType_tag: 0){
_LL765: _temp764=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp756)->f1;
goto _LL763; _LL763: _temp762=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp756)->f2; goto _LL759;} else{ goto _LL760;} _LL760: goto _LL761; _LL759:{
void* _temp766= _temp762; _LL768: if(( int) _temp766 == Cyc_Absyn_B1){ goto
_LL769;} else{ goto _LL770;} _LL770: if(( int) _temp766 == Cyc_Absyn_B2){ goto
_LL771;} else{ goto _LL772;} _LL772: if(( int) _temp766 == Cyc_Absyn_B4){ goto
_LL773;} else{ goto _LL774;} _LL774: if(( int) _temp766 == Cyc_Absyn_B8){ goto
_LL775;} else{ goto _LL767;} _LL769: if( w >( unsigned int) 8){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp776=( char*)"bitfield larger than type";
struct _tagged_string _temp777; _temp777.curr= _temp776; _temp777.base= _temp776;
_temp777.last_plus_one= _temp776 + 26; _temp777;}));} goto _LL767; _LL771: if( w
>( unsigned int) 16){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp778=( char*)"bitfield larger than type"; struct _tagged_string _temp779;
_temp779.curr= _temp778; _temp779.base= _temp778; _temp779.last_plus_one=
_temp778 + 26; _temp779;}));} goto _LL767; _LL773: if( w >( unsigned int) 32){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp780=( char*)"bitfield larger than type";
struct _tagged_string _temp781; _temp781.curr= _temp780; _temp781.base= _temp780;
_temp781.last_plus_one= _temp780 + 26; _temp781;}));} goto _LL767; _LL775: if( w
>( unsigned int) 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp782=( char*)"bitfield larger than type"; struct _tagged_string _temp783;
_temp783.curr= _temp782; _temp783.base= _temp782; _temp783.last_plus_one=
_temp782 + 26; _temp783;}));} goto _LL767; _LL767:;} goto _LL757; _LL761: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp784=* _temp751; struct _tagged_string _temp785=
Cyc_Absynpp_typ2string( _temp747); xprintf("bitfield %.*s must have integral type but has type %.*s",
_temp784.last_plus_one - _temp784.curr, _temp784.curr, _temp785.last_plus_one -
_temp785.curr, _temp785.curr);})); goto _LL757; _LL757:;}} if( ! Cyc_Tcutil_bits_only(
_temp747)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp786=* _temp751;
struct _tagged_string _temp787=* v; xprintf("%.*s has a type that is possibly incompatible with other members of union %.*s",
_temp786.last_plus_one - _temp786.curr, _temp786.curr, _temp787.last_plus_one -
_temp787.curr, _temp787.curr);}));} for( 0; _temp743 != 0; _temp743= _temp743->tl){
void* _temp788=( void*) _temp743->hd; int _temp796; _LL790: if(( int) _temp788
== Cyc_Absyn_Packed_att){ goto _LL791;} else{ goto _LL792;} _LL792: if((
unsigned int) _temp788 > 15u?(( struct _tunion_struct*) _temp788)->tag == Cyc_Absyn_Aligned_att_tag:
0){ _LL797: _temp796=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp788)->f1;
goto _LL793;} else{ goto _LL794;} _LL794: goto _LL795; _LL791: continue; _LL793:
continue; _LL795: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp798= Cyc_Absyn_attribute2string((
void*) _temp743->hd); struct _tagged_string _temp799=* v; struct _tagged_string
_temp800=* _temp751; xprintf("bad attribute %.*s in union %.*s, member %.*s",
_temp798.last_plus_one - _temp798.curr, _temp798.curr, _temp799.last_plus_one -
_temp799.curr, _temp799.curr, _temp800.last_plus_one - _temp800.curr, _temp800.curr);}));
goto _LL789; _LL789:;}}}* udp= ud;}}}}} static void Cyc_Tc_tcTuniondecl( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc,
struct Cyc_Absyn_Tuniondecl* ed){ if( ed->name == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp801=( char*)"anonymous tunions are not allowed";
struct _tagged_string _temp802; _temp802.curr= _temp801; _temp802.base= _temp801;
_temp802.last_plus_one= _temp801 + 34; _temp802;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)( ed->name)->v)).f2; struct Cyc_List_List*
tvs= ed->tvs;{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0; tvs2= tvs2->tl){
struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) tvs2->hd)->kind); void*
_temp803=( void*) c->v; void* _temp811; _LL805: if(( int) _temp803 == Cyc_Absyn_No_constr){
goto _LL806;} else{ goto _LL807;} _LL807: if(( unsigned int) _temp803 > 1u?((
struct _tunion_struct*) _temp803)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL812:
_temp811=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp803)->f1; if(( int)
_temp811 == Cyc_Absyn_MemKind){ goto _LL808;} else{ goto _LL809;}} else{ goto
_LL809;} _LL809: goto _LL810; _LL806:( void*)( c->v=( void*)(( void*)({ struct
Cyc_Absyn_Eq_constr_struct* _temp813=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp813[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp814; _temp814.tag= Cyc_Absyn_Eq_constr_tag;
_temp814.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp814;}); _temp813;})));
goto _LL804; _LL808: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp815=* v;
struct _tagged_string _temp816=*(( struct Cyc_Absyn_Tvar*) tvs2->hd)->name;
xprintf("tunion %.*s attempts to abstract type variable %.*s of kind M",
_temp815.last_plus_one - _temp815.curr, _temp815.curr, _temp816.last_plus_one -
_temp816.curr, _temp816.curr);})); goto _LL804; _LL810: goto _LL804; _LL804:;}}{
void* _temp817=(*(( struct _tuple0*)( ed->name)->v)).f1; struct Cyc_List_List*
_temp825; struct Cyc_List_List* _temp827; _LL819: if(( unsigned int) _temp817 >
1u?(( struct _tunion_struct*) _temp817)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL826:
_temp825=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp817)->f1;
if( _temp825 == 0){ goto _LL820;} else{ goto _LL821;}} else{ goto _LL821;}
_LL821: if(( unsigned int) _temp817 > 1u?(( struct _tunion_struct*) _temp817)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL828: _temp827=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp817)->f1; if( _temp827 == 0){ goto _LL822;} else{
goto _LL823;}} else{ goto _LL823;} _LL823: goto _LL824; _LL820: goto _LL818;
_LL822: goto _LL818; _LL824: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp829=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp830; _temp830.curr= _temp829; _temp830.base= _temp829;
_temp830.last_plus_one= _temp829 + 43; _temp830;})); return; _LL818:;}(*((
struct _tuple0*)( ed->name)->v)).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp831=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp831[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp832; _temp832.tag= Cyc_Absyn_Abs_n_tag;
_temp832.f1= te->ns; _temp832;}); _temp831;}); Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_Core_Opt* _temp833=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->tuniondecls, v);
struct Cyc_Core_Opt _temp839; struct Cyc_Absyn_Tuniondecl** _temp840; _LL835:
if( _temp833 == 0){ goto _LL836;} else{ goto _LL837;} _LL837: if( _temp833 == 0){
goto _LL834;} else{ _temp839=* _temp833; _LL841: _temp840=( struct Cyc_Absyn_Tuniondecl**)
_temp839.v; goto _LL838;} _LL836: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl**
data)) Cyc_Dict_insert)( ge->tuniondecls, v,({ struct Cyc_Absyn_Tuniondecl**
_temp842=( struct Cyc_Absyn_Tuniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp842[ 0]= ed; _temp842;})); if( ed->fields == 0){ return;} goto _LL834;
_LL838: { struct Cyc_Absyn_Tuniondecl* ed2=* _temp840; struct Cyc_List_List*
tvs2= ed2->tvs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs)
!=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp843=* v; xprintf("redeclaration of tunion %.*s has a different number of type parameters",
_temp843.last_plus_one - _temp843.curr, _temp843.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1= x1->tl, x2= x2->tl){
struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x1->hd)->kind); struct
Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x2->hd)->kind); if(( void*)
c1->v ==( void*) Cyc_Absyn_No_constr? c1 != c2: 0){( void*)( c1->v=( void*)((
void*)({ struct Cyc_Absyn_Forward_constr_struct* _temp844=( struct Cyc_Absyn_Forward_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Forward_constr_struct)); _temp844[ 0]=({
struct Cyc_Absyn_Forward_constr_struct _temp845; _temp845.tag= Cyc_Absyn_Forward_constr_tag;
_temp845.f1= c2; _temp845;}); _temp844;})));} if((( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp846=* v; struct
_tagged_string _temp847= Cyc_Absynpp_ckind2string( c1); struct _tagged_string
_temp848=*(( struct Cyc_Absyn_Tvar*) x1->hd)->name; struct _tagged_string
_temp849= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of tunion %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp846.last_plus_one - _temp846.curr, _temp846.curr, _temp847.last_plus_one -
_temp847.curr, _temp847.curr, _temp848.last_plus_one - _temp848.curr, _temp848.curr,
_temp849.last_plus_one - _temp849.curr, _temp849.curr);}));}}( void*)( ed->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ed->sc, ed->fields,( void*)
ed2->sc, ed2->fields, loc,( struct _tagged_string)({ char* _temp850=( char*)"tunion";
struct _tagged_string _temp851; _temp851.curr= _temp850; _temp851.base= _temp850;
_temp851.last_plus_one= _temp850 + 7; _temp851;}), v));{ struct _tuple7 _temp853=({
struct _tuple7 _temp852; _temp852.f1= ed->fields; _temp852.f2= ed2->fields;
_temp852;}); struct Cyc_Core_Opt* _temp863; struct Cyc_Core_Opt* _temp865;
struct Cyc_Core_Opt* _temp867; struct Cyc_Core_Opt* _temp869; struct Cyc_Core_Opt*
_temp871; struct Cyc_Core_Opt* _temp873; struct Cyc_Core_Opt* _temp875; struct
Cyc_Core_Opt* _temp877; _LL855: _LL866: _temp865= _temp853.f1; if( _temp865 == 0){
goto _LL864;} else{ goto _LL857;} _LL864: _temp863= _temp853.f2; if( _temp863 ==
0){ goto _LL856;} else{ goto _LL857;} _LL857: _LL870: _temp869= _temp853.f1;
goto _LL868; _LL868: _temp867= _temp853.f2; if( _temp867 == 0){ goto _LL858;}
else{ goto _LL859;} _LL859: _LL874: _temp873= _temp853.f1; if( _temp873 == 0){
goto _LL872;} else{ goto _LL861;} _LL872: _temp871= _temp853.f2; goto _LL860;
_LL861: _LL878: _temp877= _temp853.f1; goto _LL876; _LL876: _temp875= _temp853.f2;
goto _LL862; _LL856: return; _LL858:* _temp840= ed; goto _LL854; _LL860: ed->fields=
ed2->fields; ed->tvs= tvs2; return; _LL862: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0= tvs0->tl, tvs2= tvs2->tl){
inst=({ struct Cyc_List_List* _temp879=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp879->hd=( void*)({ struct _tuple4* _temp880=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp880->f1=( struct Cyc_Absyn_Tvar*)
tvs2->hd; _temp880->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp881=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp881[ 0]=({ struct Cyc_Absyn_VarType_struct _temp882; _temp882.tag= Cyc_Absyn_VarType_tag;
_temp882.f1=( struct Cyc_Absyn_Tvar*) tvs0->hd; _temp882;}); _temp881;});
_temp880;}); _temp879->tl= inst; _temp879;});}}{ struct Cyc_List_List* f1s=(
struct Cyc_List_List*)( ed->fields)->v; struct Cyc_List_List* f2s=( struct Cyc_List_List*)(
ed2->fields)->v; for( 0; f1s != 0? f2s != 0: 0; f1s= f1s->tl, f2s= f2s->tl){(*((
struct Cyc_Absyn_Tunionfield*) f1s->hd)->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp883=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp883[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp884; _temp884.tag= Cyc_Absyn_Abs_n_tag;
_temp884.f1= te->ns; _temp884;}); _temp883;});{ struct Cyc_List_List* alltvs=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tvs,(( struct Cyc_Absyn_Tunionfield*) f1s->hd)->tvs); Cyc_Tcutil_check_unique_tvars(((
struct Cyc_Absyn_Tunionfield*) f1s->hd)->loc, alltvs);{ struct Cyc_List_List*
typs=(( struct Cyc_Absyn_Tunionfield*) f1s->hd)->typs; for( 0; typs != 0; typs=
typs->tl){ void* t=(*(( struct _tuple5*) typs->hd)).f2; Cyc_Tcutil_check_type(((
struct Cyc_Absyn_Tunionfield*) f1s->hd)->loc, te, alltvs,( void*) Cyc_Absyn_MemKind,
t);}} Cyc_Tc_field_redecl_okay(( struct Cyc_Absyn_Tunionfield*) f1s->hd,( struct
Cyc_Absyn_Tunionfield*) f2s->hd, inst, te,( struct _tagged_string)({ char*
_temp885=( char*)"tunion"; struct _tagged_string _temp886; _temp886.curr=
_temp885; _temp886.base= _temp885; _temp886.last_plus_one= _temp885 + 7;
_temp886;}), v);}} if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp887=* v; struct _tagged_string _temp888=*(*(( struct Cyc_Absyn_Tunionfield*)
f1s->hd)->name).f2; xprintf("redeclaration of tunion %.*s has extra field %.*s",
_temp887.last_plus_one - _temp887.curr, _temp887.curr, _temp888.last_plus_one -
_temp888.curr, _temp888.curr);}));} if( f2s != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp889=* v; struct _tagged_string _temp890=*(*(( struct
Cyc_Absyn_Tunionfield*) f2s->hd)->name).f2; xprintf("redeclaration of tunion %.*s is missing field %.*s",
_temp889.last_plus_one - _temp889.curr, _temp889.curr, _temp890.last_plus_one -
_temp890.curr, _temp890.curr);}));} return;}} _LL854:;} goto _LL834;}} _LL834:;}{
struct Cyc_Absyn_Tuniondecl** edp=(( struct Cyc_Absyn_Tuniondecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->tuniondecls,
v);* edp=({ struct Cyc_Absyn_Tuniondecl* _temp891=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp891->sc=( void*)(( void*)
ed->sc); _temp891->name= ed->name; _temp891->tvs= tvs; _temp891->fields= 0;
_temp891;});{ struct Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs=(
struct Cyc_List_List*)( ed->fields)->v; for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*) fs->hd; if((( int(*)( int(* compare)( struct
_tagged_string*, struct _tagged_string*), struct Cyc_List_List* l, struct
_tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,(* f->name).f2)){
Cyc_Tcutil_terr( f->loc,({ struct _tagged_string _temp892=*(* f->name).f2;
xprintf("duplicate field name %.*s", _temp892.last_plus_one - _temp892.curr,
_temp892.curr);}));} prev_fields=({ struct Cyc_List_List* _temp893=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp893->hd=( void*)(* f->name).f2;
_temp893->tl= prev_fields; _temp893;});{ struct Cyc_List_List* tvs= f->tvs; for(
0; tvs != 0; tvs= tvs->tl){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)
tvs->hd; struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind); void* _temp894=( void*) c->v; void*
_temp902; _LL896: if(( int) _temp894 == Cyc_Absyn_No_constr){ goto _LL897;}
else{ goto _LL898;} _LL898: if(( unsigned int) _temp894 > 1u?(( struct
_tunion_struct*) _temp894)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL903: _temp902=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp894)->f1; if(( int) _temp902
== Cyc_Absyn_MemKind){ goto _LL899;} else{ goto _LL900;}} else{ goto _LL900;}
_LL900: goto _LL901; _LL897:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp904=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp904[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp905; _temp905.tag= Cyc_Absyn_Eq_constr_tag;
_temp905.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp905;}); _temp904;})));
goto _LL895; _LL899: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp906=*(*
f->name).f2; xprintf("field %.*s abstracts type variable of kind M", _temp906.last_plus_one
- _temp906.curr, _temp906.curr);})); goto _LL895; _LL901: goto _LL895; _LL895:;}}{
struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, f->tvs); Cyc_Tcutil_check_unique_tvars(
loc, alltvs);{ struct Cyc_List_List* typs= f->typs; for( 0; typs != 0; typs=
typs->tl){ void* t=(*(( struct _tuple5*) typs->hd)).f2; Cyc_Tcutil_check_type( f->loc,
te, alltvs,( void*) Cyc_Absyn_MemKind, t);}}(* f->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp907=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp907[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp908; _temp908.tag= Cyc_Absyn_Abs_n_tag;
_temp908.f1= te->ns; _temp908;}); _temp907;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)(
ge->ordinaries,(* f->name).f2,({ struct _tuple6* _temp909=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp909->f1=( void*)({ struct Cyc_Tcenv_TunionRes_struct*
_temp910=( struct Cyc_Tcenv_TunionRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct));
_temp910[ 0]=({ struct Cyc_Tcenv_TunionRes_struct _temp911; _temp911.tag= Cyc_Tcenv_TunionRes_tag;
_temp911.f1= ed; _temp911.f2= f; _temp911;}); _temp910;}); _temp909->f2= 1;
_temp909;}));}}} ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)(
ge->tuniondecls, v,({ struct Cyc_Absyn_Tuniondecl** _temp912=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp912[ 0]= ed; _temp912;}));*
edp= ed;}}}} static void Cyc_Tc_tcXTuniondecl( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_XTuniondecl*
xd){ struct _tagged_string* v=(* xd->name).f2; struct Cyc_Core_Opt* xd2o;{
struct _handler_cons _temp913; _push_handler(& _temp913);{ struct
_xtunion_struct* _temp914=( struct _xtunion_struct*) setjmp( _temp913.handler);
if( ! _temp914){ xd2o= Cyc_Tcenv_lookup_xtuniondecl( te, loc, xd->name);;
_pop_handler();} else{ struct _xtunion_struct* _temp916= _temp914; _LL918: if(
_temp916->tag == Cyc_Dict_Absent_tag){ goto _LL919;} else{ goto _LL920;} _LL920:
goto _LL921; _LL919: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp922= Cyc_Absynpp_qvar2string(
xd->name); xprintf("qualified xtunion declaration %.*s is not an existing xtunion",
_temp922.last_plus_one - _temp922.curr, _temp922.curr);})); return; _LL921:(
void) _throw( _temp916); _LL917:;}}}{ void* _temp923=(* xd->name).f1; struct Cyc_List_List*
_temp933; struct Cyc_List_List* _temp935; struct Cyc_List_List* _temp937; _LL925:
if(( int) _temp923 == Cyc_Absyn_Loc_n){ goto _LL926;} else{ goto _LL927;} _LL927:
if(( unsigned int) _temp923 > 1u?(( struct _tunion_struct*) _temp923)->tag ==
Cyc_Absyn_Rel_n_tag: 0){ _LL934: _temp933=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp923)->f1; if( _temp933 == 0){ goto _LL928;} else{ goto _LL929;}} else{ goto
_LL929;} _LL929: if(( unsigned int) _temp923 > 1u?(( struct _tunion_struct*)
_temp923)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL936: _temp935=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp923)->f1; goto _LL930;} else{ goto _LL931;}
_LL931: if(( unsigned int) _temp923 > 1u?(( struct _tunion_struct*) _temp923)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL938: _temp937=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp923)->f1; goto _LL932;} else{ goto _LL924;} _LL926:(
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp939=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp939[ 0]=({ struct Cyc_Core_Impossible_struct _temp940; _temp940.tag= Cyc_Core_Impossible_tag;
_temp940.f1=( struct _tagged_string)({ char* _temp941=( char*)"tcXTuniondecl: Loc_n";
struct _tagged_string _temp942; _temp942.curr= _temp941; _temp942.base= _temp941;
_temp942.last_plus_one= _temp941 + 21; _temp942;}); _temp940;}); _temp939;}));
_LL928:(* xd->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp943=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp943[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp944; _temp944.tag= Cyc_Absyn_Abs_n_tag;
_temp944.f1= te->ns; _temp944;}); _temp943;}); goto _LL924; _LL930:( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp945=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp945[ 0]=({ struct Cyc_Core_Impossible_struct _temp946; _temp946.tag= Cyc_Core_Impossible_tag;
_temp946.f1=( struct _tagged_string)({ char* _temp947=( char*)"tcXTuniondecl: Rel_n non-null";
struct _tagged_string _temp948; _temp948.curr= _temp947; _temp948.base= _temp947;
_temp948.last_plus_one= _temp947 + 30; _temp948;}); _temp946;}); _temp945;}));
_LL932:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp949=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp949[ 0]=({ struct Cyc_Core_Impossible_struct _temp950; _temp950.tag= Cyc_Core_Impossible_tag;
_temp950.f1=( struct _tagged_string)({ char* _temp951=( char*)"tcXTuniondecl: Abs_n";
struct _tagged_string _temp952; _temp952.curr= _temp951; _temp952.base= _temp951;
_temp952.last_plus_one= _temp951 + 21; _temp952;}); _temp950;}); _temp949;}));
_LL924:;} if( xd2o == 0){ struct Cyc_Absyn_XTuniondecl** x=( struct Cyc_Absyn_XTuniondecl**)({
struct _tuple8* _temp953=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp953->f1=({ struct Cyc_Absyn_XTuniondecl* _temp954=( struct Cyc_Absyn_XTuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_XTuniondecl)); _temp954->sc=( void*)(( void*)
xd->sc); _temp954->name= xd->name; _temp954->fields= 0; _temp954;}); _temp953;});
xd2o=({ struct Cyc_Core_Opt* _temp955=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp955->v=( void*) x; _temp955;}); ge->xtuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_XTuniondecl** data)) Cyc_Dict_insert)( ge->xtuniondecls, v, x);}
else{ struct Cyc_Absyn_XTuniondecl* x=*(( struct Cyc_Absyn_XTuniondecl**) xd2o->v);(
void*)( x->sc=( void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void*
s2, struct Cyc_Core_Opt* fields2, struct Cyc_Position_Segment* loc, struct
_tagged_string t, struct _tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*)
xd->sc,({ struct Cyc_Core_Opt* _temp956=( struct Cyc_Core_Opt*) GC_malloc_atomic(
sizeof( struct Cyc_Core_Opt)); _temp956->v=( void*) 0; _temp956;}),( void*) x->sc,({
struct Cyc_Core_Opt* _temp957=( struct Cyc_Core_Opt*) GC_malloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp957->v=( void*) 0; _temp957;}), loc,( struct
_tagged_string)({ char* _temp958=( char*)"xtunion"; struct _tagged_string
_temp959; _temp959.curr= _temp958; _temp959.base= _temp958; _temp959.last_plus_one=
_temp958 + 8; _temp959;}), v));}{ struct Cyc_Absyn_XTuniondecl* xd_result=*((
struct Cyc_Absyn_XTuniondecl**) xd2o->v); struct Cyc_List_List* fs= xd->fields;
for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)
fs->hd;{ void* _temp960=(* f->name).f1; struct Cyc_List_List* _temp970; struct
Cyc_List_List* _temp972; struct Cyc_List_List* _temp974; _LL962: if((
unsigned int) _temp960 > 1u?(( struct _tunion_struct*) _temp960)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL971: _temp970=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp960)->f1; if( _temp970 == 0){ goto _LL963;} else{ goto _LL964;}} else{ goto
_LL964;} _LL964: if(( unsigned int) _temp960 > 1u?(( struct _tunion_struct*)
_temp960)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL973: _temp972=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp960)->f1; goto _LL965;} else{ goto _LL966;}
_LL966: if(( unsigned int) _temp960 > 1u?(( struct _tunion_struct*) _temp960)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL975: _temp974=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp960)->f1; goto _LL967;} else{ goto _LL968;} _LL968:
if(( int) _temp960 == Cyc_Absyn_Loc_n){ goto _LL969;} else{ goto _LL961;} _LL963:(*
f->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp976=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp976[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp977; _temp977.tag= Cyc_Absyn_Abs_n_tag; _temp977.f1= te->ns; _temp977;});
_temp976;}); goto _LL961; _LL965: goto _LL967; _LL967: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp978=( char*)"qualified declarations are not allowed\n";
struct _tagged_string _temp979; _temp979.curr= _temp978; _temp979.base= _temp978;
_temp979.last_plus_one= _temp978 + 40; _temp979;})); goto _LL961; _LL969:( void)
_throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct* _temp980=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp980[ 0]=({ struct Cyc_Core_Impossible_struct _temp981; _temp981.tag= Cyc_Core_Impossible_tag;
_temp981.f1=( struct _tagged_string)({ char* _temp982=( char*)"tcXtuniondecl: Loc_n";
struct _tagged_string _temp983; _temp983.curr= _temp982; _temp983.base= _temp982;
_temp983.last_plus_one= _temp982 + 21; _temp983;}); _temp981;}); _temp980;}));
goto _LL961; _LL961:;}{ struct Cyc_List_List* tvs= f->tvs; for( 0; tvs != 0; tvs=
tvs->tl){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*) tvs->hd; struct
Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)( tv->kind); void* _temp984=( void*) c->v; void*
_temp992; _LL986: if(( int) _temp984 == Cyc_Absyn_No_constr){ goto _LL987;}
else{ goto _LL988;} _LL988: if(( unsigned int) _temp984 > 1u?(( struct
_tunion_struct*) _temp984)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL993: _temp992=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp984)->f1; if(( int) _temp992
== Cyc_Absyn_MemKind){ goto _LL989;} else{ goto _LL990;}} else{ goto _LL990;}
_LL990: goto _LL991; _LL987:( void*)( c->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp994=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp994[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp995; _temp995.tag= Cyc_Absyn_Eq_constr_tag;
_temp995.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp995;}); _temp994;})));
goto _LL985; _LL989: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp996=*(*
f->name).f2; xprintf("field %.*s abstracts type variable of kind M", _temp996.last_plus_one
- _temp996.curr, _temp996.curr);})); goto _LL985; _LL991: goto _LL985; _LL985:;}}{
struct Cyc_List_List* tvs= f->tvs; Cyc_Tcutil_check_unique_tvars( loc, tvs);{
struct Cyc_List_List* typs= f->typs; for( 0; typs != 0; typs= typs->tl){ void* t=(*((
struct _tuple5*) typs->hd)).f2; Cyc_Tcutil_check_type( f->loc, te, tvs,( void*)
Cyc_Absyn_MemKind, t);}}{ int dup= 0;{ struct Cyc_List_List* prev_fs= xd_result->fields;
for( 0; prev_fs != 0; prev_fs= prev_fs->tl){ struct Cyc_Absyn_Tunionfield* prev=(
struct Cyc_Absyn_Tunionfield*) prev_fs->hd; if( Cyc_Absyn_qvar_cmp( f->name,
prev->name) == 0){ Cyc_Tc_field_redecl_okay( prev, f, 0, te,( struct
_tagged_string)({ char* _temp997=( char*)"xtunion"; struct _tagged_string
_temp998; _temp998.curr= _temp997; _temp998.base= _temp997; _temp998.last_plus_one=
_temp997 + 8; _temp998;}), v); dup= 1; break;}}} if( dup){ continue;} xd_result->fields=({
struct Cyc_List_List* _temp999=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp999->hd=( void*) f; _temp999->tl= xd_result->fields;
_temp999;});{ struct Cyc_Tcenv_Genv* ge_f; struct Cyc_List_List* ns_f;{ void*
_temp1000=(* f->name).f1; struct Cyc_List_List* _temp1008; struct Cyc_List_List*
_temp1010; _LL1002: if(( unsigned int) _temp1000 > 1u?(( struct _tunion_struct*)
_temp1000)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL1009: _temp1008=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp1000)->f1; goto _LL1003;} else{ goto
_LL1004;} _LL1004: if(( int) _temp1000 == Cyc_Absyn_Loc_n){ goto _LL1005;} else{
goto _LL1006;} _LL1006: if(( unsigned int) _temp1000 > 1u?(( struct
_tunion_struct*) _temp1000)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL1011: _temp1010=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp1000)->f1; goto
_LL1007;} else{ goto _LL1001;} _LL1003: ns_f= _temp1008; goto _LL1001; _LL1005:
goto _LL1007; _LL1007:( void) _throw(( struct _xtunion_struct*)({ struct Cyc_Core_Impossible_struct*
_temp1012=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1012[ 0]=({ struct Cyc_Core_Impossible_struct _temp1013; _temp1013.tag= Cyc_Core_Impossible_tag;
_temp1013.f1=( struct _tagged_string)({ char* _temp1014=( char*)"tcXTuniondecl: Rel_n or Loc_n";
struct _tagged_string _temp1015; _temp1015.curr= _temp1014; _temp1015.base=
_temp1014; _temp1015.last_plus_one= _temp1014 + 30; _temp1015;}); _temp1013;});
_temp1012;})); goto _LL1001; _LL1001:;}{ struct _handler_cons _temp1016;
_push_handler(& _temp1016);{ struct _xtunion_struct* _temp1017=( struct
_xtunion_struct*) setjmp( _temp1016.handler); if( ! _temp1017){ ge_f=(( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)(
te->ae, ns_f);; _pop_handler();} else{ struct _xtunion_struct* _temp1019=
_temp1017; _LL1021: if( _temp1019->tag == Cyc_Dict_Absent_tag){ goto _LL1022;}
else{ goto _LL1023;} _LL1023: goto _LL1024; _LL1022: Cyc_Tcutil_terr( f->loc,({
struct _tagged_string _temp1025= Cyc_Absynpp_qvar2string( f->name); xprintf("bad namespace for xtunion field %.*s",
_temp1025.last_plus_one - _temp1025.curr, _temp1025.curr);})); return; _LL1024:(
void) _throw( _temp1019); _LL1020:;}}} ge_f->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)(
ge_f->ordinaries,(* f->name).f2,({ struct _tuple6* _temp1026=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp1026->f1=( void*)({ struct Cyc_Tcenv_XTunionRes_struct*
_temp1027=( struct Cyc_Tcenv_XTunionRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_XTunionRes_struct));
_temp1027[ 0]=({ struct Cyc_Tcenv_XTunionRes_struct _temp1028; _temp1028.tag=
Cyc_Tcenv_XTunionRes_tag; _temp1028.f1= xd_result; _temp1028.f2= f; _temp1028;});
_temp1027;}); _temp1026->f2= 1; _temp1026;}));}}}}}} static void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl* ed){ struct _tagged_string* v=(* ed->name).f2;{
void* _temp1029=(* ed->name).f1; struct Cyc_List_List* _temp1037; struct Cyc_List_List*
_temp1039; _LL1031: if(( unsigned int) _temp1029 > 1u?(( struct _tunion_struct*)
_temp1029)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL1038: _temp1037=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp1029)->f1; if( _temp1037 == 0){ goto
_LL1032;} else{ goto _LL1033;}} else{ goto _LL1033;} _LL1033: if(( unsigned int)
_temp1029 > 1u?(( struct _tunion_struct*) _temp1029)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL1040: _temp1039=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp1029)->f1; if( _temp1039 == 0){ goto _LL1034;} else{ goto _LL1035;}} else{
goto _LL1035;} _LL1035: goto _LL1036; _LL1032: goto _LL1030; _LL1034: goto
_LL1030; _LL1036: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp1041=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp1042; _temp1042.curr= _temp1041; _temp1042.base= _temp1041;
_temp1042.last_plus_one= _temp1041 + 43; _temp1042;})); return; _LL1030:;}(* ed->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1043=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1043[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1044; _temp1044.tag= Cyc_Absyn_Abs_n_tag; _temp1044.f1= te->ns; _temp1044;});
_temp1043;});{ struct Cyc_Core_Opt* _temp1045=(( struct Cyc_Core_Opt*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->enumdecls,
v); struct Cyc_Core_Opt _temp1051; struct Cyc_Absyn_Enumdecl** _temp1052;
_LL1047: if( _temp1045 == 0){ goto _LL1048;} else{ goto _LL1049;} _LL1049: if(
_temp1045 == 0){ goto _LL1046;} else{ _temp1051=* _temp1045; _LL1053: _temp1052=(
struct Cyc_Absyn_Enumdecl**) _temp1051.v; goto _LL1050;} _LL1048: ge->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)( ge->enumdecls, v,({ struct
Cyc_Absyn_Enumdecl** _temp1054=( struct Cyc_Absyn_Enumdecl**) GC_malloc( sizeof(
struct Cyc_Absyn_Enumdecl*)); _temp1054[ 0]= ed; _temp1054;})); goto _LL1046;
_LL1050: { struct Cyc_Absyn_Enumdecl* ed2=* _temp1052;( void*)( ed->sc=( void*)((
void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ed->sc, 0,( void*) ed2->sc,
0, loc,( struct _tagged_string)({ char* _temp1055=( char*)"enum"; struct
_tagged_string _temp1056; _temp1056.curr= _temp1055; _temp1056.base= _temp1055;
_temp1056.last_plus_one= _temp1055 + 5; _temp1056;}), v));{ struct Cyc_List_List*
f1s= ed->fields; struct Cyc_List_List* f2s= ed2->fields; unsigned int tag_count=
0; for( 0; f1s != 0? f2s != 0: 0; f1s= f1s->tl, f2s= f2s->tl){ struct Cyc_Absyn_Enumfield
_temp1059; struct Cyc_Position_Segment* _temp1060; struct Cyc_Absyn_Exp*
_temp1062; struct Cyc_Absyn_Exp** _temp1064; struct _tuple0* _temp1065; struct
Cyc_Absyn_Enumfield* _temp1057=( struct Cyc_Absyn_Enumfield*) f1s->hd; _temp1059=*
_temp1057; _LL1066: _temp1065=( struct _tuple0*) _temp1059.name; goto _LL1063;
_LL1063: _temp1062=( struct Cyc_Absyn_Exp*) _temp1059.tag; _temp1064=&(*
_temp1057).tag; goto _LL1061; _LL1061: _temp1060=( struct Cyc_Position_Segment*)
_temp1059.loc; goto _LL1058; _LL1058: { struct Cyc_Absyn_Enumfield _temp1069;
struct Cyc_Position_Segment* _temp1070; struct Cyc_Absyn_Exp* _temp1072; struct
_tuple0* _temp1074; struct Cyc_Absyn_Enumfield* _temp1067=( struct Cyc_Absyn_Enumfield*)
f2s->hd; _temp1069=* _temp1067; _LL1075: _temp1074=( struct _tuple0*) _temp1069.name;
goto _LL1073; _LL1073: _temp1072=( struct Cyc_Absyn_Exp*) _temp1069.tag; goto
_LL1071; _LL1071: _temp1070=( struct Cyc_Position_Segment*) _temp1069.loc; goto
_LL1068; _LL1068:(* _temp1065).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp1076=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp1076[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp1077; _temp1077.tag= Cyc_Absyn_Abs_n_tag;
_temp1077.f1= te->ns; _temp1077;}); _temp1076;}); if( Cyc_String_zstrptrcmp((*
_temp1065).f2,(* _temp1074).f2) != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp1078=* v; struct _tagged_string _temp1079=*(* _temp1065).f2;
struct _tagged_string _temp1080=*(* _temp1074).f2; xprintf("redeclaration of enum %.*s: field name mismatch %.*s != %.*s",
_temp1078.last_plus_one - _temp1078.curr, _temp1078.curr, _temp1079.last_plus_one
- _temp1079.curr, _temp1079.curr, _temp1080.last_plus_one - _temp1080.curr,
_temp1080.curr);}));} if(* _temp1064 == 0){* _temp1064=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( tag_count, _temp1060);} else{ if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1081=* _temp1064; if(
_temp1081 == 0){ _throw( Null_Exception);} _temp1081;}))){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1082=* v; struct _tagged_string _temp1083=*(*
_temp1065).f2; xprintf("redeclaration of enum %.*s, field %.*s: expression is not constant",
_temp1082.last_plus_one - _temp1082.curr, _temp1082.curr, _temp1083.last_plus_one
- _temp1083.curr, _temp1083.curr);}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1084=* _temp1064; if(
_temp1084 == 0){ _throw( Null_Exception);} _temp1084;})); tag_count= t1 +(
unsigned int) 1; if( t1 != Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1085= _temp1072; if( _temp1085 == 0){ _throw(
Null_Exception);} _temp1085;}))){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp1086=* v; struct _tagged_string _temp1087=*(* _temp1065).f2; xprintf("redeclaration of enum %.*s, field %.*s, value mismatch",
_temp1086.last_plus_one - _temp1086.curr, _temp1086.curr, _temp1087.last_plus_one
- _temp1087.curr, _temp1087.curr);}));}}}} return;}} _LL1046:;}{ struct Cyc_List_List*
prev_fields= 0; unsigned int tag_count= 0;{ struct Cyc_List_List* fs= ed->fields;
for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Enumfield* f=( struct Cyc_Absyn_Enumfield*)
fs->hd; if((( int(*)( int(* compare)( struct _tagged_string*, struct
_tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields,(* f->name).f2)){ Cyc_Tcutil_terr( f->loc,({
struct _tagged_string _temp1088=*(* f->name).f2; xprintf("duplicate field name %.*s",
_temp1088.last_plus_one - _temp1088.curr, _temp1088.curr);}));} prev_fields=({
struct Cyc_List_List* _temp1089=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1089->hd=( void*)(* f->name).f2; _temp1089->tl=
prev_fields; _temp1089;}); if( f->tag == 0){ f->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
tag_count, f->loc);} else{ if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1090= f->tag; if( _temp1090 == 0){ _throw(
Null_Exception);} _temp1090;}))){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp1091=* v; struct _tagged_string _temp1092=*(* f->name).f2; xprintf("enum %.*s, field %.*s: expression is not constant",
_temp1091.last_plus_one - _temp1091.curr, _temp1091.curr, _temp1092.last_plus_one
- _temp1092.curr, _temp1092.curr);}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1093= f->tag; if( _temp1093
== 0){ _throw( Null_Exception);} _temp1093;})); tag_count= t1 +( unsigned int) 1;(*
f->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1094=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1094[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1095; _temp1095.tag= Cyc_Absyn_Abs_n_tag; _temp1095.f1= te->ns; _temp1095;});
_temp1094;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,(*
f->name).f2,({ struct _tuple6* _temp1096=( struct _tuple6*) GC_malloc( sizeof(
struct _tuple6)); _temp1096->f1=( void*)({ struct Cyc_Tcenv_EnumRes_struct*
_temp1097=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_EnumRes_struct));
_temp1097[ 0]=({ struct Cyc_Tcenv_EnumRes_struct _temp1098; _temp1098.tag= Cyc_Tcenv_EnumRes_tag;
_temp1098.f1= ed; _temp1098.f2= f; _temp1098;}); _temp1097;}); _temp1096->f2= 1;
_temp1096;}));}}} ge->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)(
ge->enumdecls, v,({ struct Cyc_Absyn_Enumdecl** _temp1099=( struct Cyc_Absyn_Enumdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl*)); _temp1099[ 0]= ed; _temp1099;}));}}
static int Cyc_Tc_okay_externC( struct Cyc_Position_Segment* loc, void* sc){
void* _temp1100= sc; _LL1102: if(( int) _temp1100 == Cyc_Absyn_Static){ goto
_LL1103;} else{ goto _LL1104;} _LL1104: if(( int) _temp1100 == Cyc_Absyn_Abstract){
goto _LL1105;} else{ goto _LL1106;} _LL1106: if(( int) _temp1100 == Cyc_Absyn_Public){
goto _LL1107;} else{ goto _LL1108;} _LL1108: if(( int) _temp1100 == Cyc_Absyn_Extern){
goto _LL1109;} else{ goto _LL1110;} _LL1110: if(( int) _temp1100 == Cyc_Absyn_ExternC){
goto _LL1111;} else{ goto _LL1101;} _LL1103: Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ char* _temp1112=( char*)"static declaration nested within extern \"C\"";
struct _tagged_string _temp1113; _temp1113.curr= _temp1112; _temp1113.base=
_temp1112; _temp1113.last_plus_one= _temp1112 + 44; _temp1113;})); return 0;
_LL1105: Cyc_Tcutil_warn( loc,( struct _tagged_string)({ char* _temp1114=( char*)"abstract declaration nested within extern \"C\"";
struct _tagged_string _temp1115; _temp1115.curr= _temp1114; _temp1115.base=
_temp1114; _temp1115.last_plus_one= _temp1114 + 46; _temp1115;})); return 0;
_LL1107: return 1; _LL1109: return 1; _LL1111: Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ char* _temp1116=( char*)"nested extern \"C\" declaration";
struct _tagged_string _temp1117; _temp1117.curr= _temp1116; _temp1117.base=
_temp1116; _temp1117.last_plus_one= _temp1116 + 30; _temp1117;})); return 1;
_LL1101:;} static void Cyc_Tc_tc_decls( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
ds0, int in_externC){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
te->ns); struct Cyc_List_List* ds= ds0; for( 0; ds != 0; ds= ds->tl){ struct Cyc_Absyn_Decl*
d=( struct Cyc_Absyn_Decl*) ds->hd; struct Cyc_Position_Segment* loc= d->loc;
void* _temp1118=( void*) d->r; int _temp1144; struct Cyc_Absyn_Exp* _temp1146;
struct Cyc_Core_Opt* _temp1148; struct Cyc_Core_Opt* _temp1150; struct Cyc_Absyn_Pat*
_temp1152; struct Cyc_Absyn_Vardecl* _temp1154; struct Cyc_Absyn_Fndecl*
_temp1156; struct Cyc_Absyn_Typedefdecl* _temp1158; struct Cyc_Absyn_Structdecl*
_temp1160; struct Cyc_Absyn_Uniondecl* _temp1162; struct Cyc_Absyn_Tuniondecl*
_temp1164; struct Cyc_Absyn_XTuniondecl* _temp1166; struct Cyc_Absyn_Enumdecl*
_temp1168; struct Cyc_List_List* _temp1170; struct _tagged_string* _temp1172;
struct Cyc_List_List* _temp1174; struct _tuple0* _temp1176; struct _tuple0
_temp1178; struct _tagged_string* _temp1179; void* _temp1181; struct Cyc_List_List*
_temp1183; _LL1120: if((( struct _tunion_struct*) _temp1118)->tag == Cyc_Absyn_Let_d_tag){
_LL1153: _temp1152=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1118)->f1; goto _LL1151; _LL1151: _temp1150=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1118)->f2; goto _LL1149; _LL1149: _temp1148=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1118)->f3; goto
_LL1147; _LL1147: _temp1146=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1118)->f4; goto _LL1145; _LL1145: _temp1144=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1118)->f5; goto _LL1121;} else{ goto _LL1122;} _LL1122: if((( struct
_tunion_struct*) _temp1118)->tag == Cyc_Absyn_Var_d_tag){ _LL1155: _temp1154=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*) _temp1118)->f1;
goto _LL1123;} else{ goto _LL1124;} _LL1124: if((( struct _tunion_struct*)
_temp1118)->tag == Cyc_Absyn_Fn_d_tag){ _LL1157: _temp1156=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Fn_d_struct*) _temp1118)->f1; goto _LL1125;} else{ goto _LL1126;}
_LL1126: if((( struct _tunion_struct*) _temp1118)->tag == Cyc_Absyn_Typedef_d_tag){
_LL1159: _temp1158=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp1118)->f1; goto _LL1127;} else{ goto _LL1128;} _LL1128: if((( struct
_tunion_struct*) _temp1118)->tag == Cyc_Absyn_Struct_d_tag){ _LL1161: _temp1160=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp1118)->f1;
goto _LL1129;} else{ goto _LL1130;} _LL1130: if((( struct _tunion_struct*)
_temp1118)->tag == Cyc_Absyn_Union_d_tag){ _LL1163: _temp1162=( struct Cyc_Absyn_Uniondecl*)((
struct Cyc_Absyn_Union_d_struct*) _temp1118)->f1; goto _LL1131;} else{ goto
_LL1132;} _LL1132: if((( struct _tunion_struct*) _temp1118)->tag == Cyc_Absyn_Tunion_d_tag){
_LL1165: _temp1164=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*)
_temp1118)->f1; goto _LL1133;} else{ goto _LL1134;} _LL1134: if((( struct
_tunion_struct*) _temp1118)->tag == Cyc_Absyn_XTunion_d_tag){ _LL1167: _temp1166=(
struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_d_struct*) _temp1118)->f1;
goto _LL1135;} else{ goto _LL1136;} _LL1136: if((( struct _tunion_struct*)
_temp1118)->tag == Cyc_Absyn_Enum_d_tag){ _LL1169: _temp1168=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp1118)->f1; goto _LL1137;} else{ goto
_LL1138;} _LL1138: if((( struct _tunion_struct*) _temp1118)->tag == Cyc_Absyn_Namespace_d_tag){
_LL1173: _temp1172=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1118)->f1; goto _LL1171; _LL1171: _temp1170=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1118)->f2; goto _LL1139;} else{ goto
_LL1140;} _LL1140: if((( struct _tunion_struct*) _temp1118)->tag == Cyc_Absyn_Using_d_tag){
_LL1177: _temp1176=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1118)->f1; _temp1178=* _temp1176; _LL1182: _temp1181= _temp1178.f1; goto
_LL1180; _LL1180: _temp1179= _temp1178.f2; goto _LL1175; _LL1175: _temp1174=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*) _temp1118)->f2; goto
_LL1141;} else{ goto _LL1142;} _LL1142: if((( struct _tunion_struct*) _temp1118)->tag
== Cyc_Absyn_ExternC_d_tag){ _LL1184: _temp1183=( struct Cyc_List_List*)((
struct Cyc_Absyn_ExternC_d_struct*) _temp1118)->f1; goto _LL1143;} else{ goto
_LL1119;} _LL1121: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp1185=( char*)"top level let-declarations are not implemented"; struct
_tagged_string _temp1186; _temp1186.curr= _temp1185; _temp1186.base= _temp1185;
_temp1186.last_plus_one= _temp1185 + 47; _temp1186;})); goto _LL1119; _LL1123:
if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1154->sc): 0){( void*)(
_temp1154->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcVardecl( te, ge,
loc, _temp1154); goto _LL1119; _LL1125: if( in_externC? Cyc_Tc_okay_externC( d->loc,(
void*) _temp1156->sc): 0){( void*)( _temp1156->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp1156); goto _LL1119; _LL1127: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp1158); goto _LL1119; _LL1129: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp1160->sc): 0){( void*)( _temp1160->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp1160); goto _LL1119; _LL1131: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1162->sc): 0){( void*)(
_temp1162->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge,
loc, _temp1162); goto _LL1119; _LL1133: if( in_externC? Cyc_Tc_okay_externC( d->loc,(
void*) _temp1164->sc): 0){( void*)( _temp1164->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp1164); goto _LL1119; _LL1135: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1166->sc): 0){( void*)(
_temp1166->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcXTuniondecl( te,
ge, loc, _temp1166); goto _LL1119; _LL1137: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp1168->sc): 0){( void*)( _temp1168->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcEnumdecl( te, ge, loc, _temp1168); goto _LL1119; _LL1139: { struct Cyc_List_List*
ns= te->ns; struct Cyc_List_List* ns2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( ns,({ struct Cyc_List_List*
_temp1187=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1187->hd=( void*) _temp1172; _temp1187->tl= 0; _temp1187;})); if( !(( int(*)(
struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)( ge->namespaces,
_temp1172)){ ge->namespaces=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s,
struct _tagged_string* elt)) Cyc_Set_insert)( ge->namespaces, _temp1172); te->ae=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae, ns2, Cyc_Tcenv_empty_genv());}
te->ns= ns2; Cyc_Tc_tc_decls( te, _temp1170, in_externC); te->ns= ns; goto
_LL1119;} _LL1141: { struct _tagged_string* first; struct Cyc_List_List* rest;{
void* _temp1188= _temp1181; struct Cyc_List_List* _temp1200; struct Cyc_List_List*
_temp1202; struct Cyc_List_List* _temp1204; struct Cyc_List_List _temp1206;
struct Cyc_List_List* _temp1207; struct _tagged_string* _temp1209; struct Cyc_List_List*
_temp1211; struct Cyc_List_List _temp1213; struct Cyc_List_List* _temp1214;
struct _tagged_string* _temp1216; _LL1190: if(( int) _temp1188 == Cyc_Absyn_Loc_n){
goto _LL1191;} else{ goto _LL1192;} _LL1192: if(( unsigned int) _temp1188 > 1u?((
struct _tunion_struct*) _temp1188)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL1201:
_temp1200=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp1188)->f1;
if( _temp1200 == 0){ goto _LL1193;} else{ goto _LL1194;}} else{ goto _LL1194;}
_LL1194: if(( unsigned int) _temp1188 > 1u?(( struct _tunion_struct*) _temp1188)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL1203: _temp1202=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp1188)->f1; if( _temp1202 == 0){ goto _LL1195;}
else{ goto _LL1196;}} else{ goto _LL1196;} _LL1196: if(( unsigned int) _temp1188
> 1u?(( struct _tunion_struct*) _temp1188)->tag == Cyc_Absyn_Rel_n_tag: 0){
_LL1205: _temp1204=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1188)->f1; if( _temp1204 == 0){ goto _LL1198;} else{ _temp1206=* _temp1204;
_LL1210: _temp1209=( struct _tagged_string*) _temp1206.hd; goto _LL1208; _LL1208:
_temp1207=( struct Cyc_List_List*) _temp1206.tl; goto _LL1197;}} else{ goto
_LL1198;} _LL1198: if(( unsigned int) _temp1188 > 1u?(( struct _tunion_struct*)
_temp1188)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL1212: _temp1211=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp1188)->f1; if( _temp1211 == 0){ goto
_LL1189;} else{ _temp1213=* _temp1211; _LL1217: _temp1216=( struct
_tagged_string*) _temp1213.hd; goto _LL1215; _LL1215: _temp1214=( struct Cyc_List_List*)
_temp1213.tl; goto _LL1199;}} else{ goto _LL1189;} _LL1191: goto _LL1193;
_LL1193: goto _LL1195; _LL1195: first= _temp1179; rest= 0; goto _LL1189; _LL1197:
_temp1216= _temp1209; _temp1214= _temp1207; goto _LL1199; _LL1199: first=
_temp1216; rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp1214,({ struct Cyc_List_List* _temp1218=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1218->hd=( void*) _temp1179;
_temp1218->tl= 0; _temp1218;})); goto _LL1189; _LL1189:;}{ struct Cyc_List_List*
ns2= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); ge->availables=({
struct Cyc_List_List* _temp1219=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1219->hd=( void*) ns2; _temp1219->tl= ge->availables;
_temp1219;}); Cyc_Tc_tc_decls( te, _temp1174, in_externC); ge->availables=( ge->availables)->tl;
goto _LL1119;}} _LL1143: Cyc_Tc_tc_decls( te, _temp1183, 1); goto _LL1119;
_LL1119:;}} static char _temp1222[ 4u]="Cyc"; static struct _tagged_string Cyc_Tc_cyc_string=(
struct _tagged_string){ _temp1222, _temp1222, _temp1222 + 4u}; static struct
_tagged_string* Cyc_Tc_cyc_ns=& Cyc_Tc_cyc_string; void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv*
te, int add_cyc_namespace, struct Cyc_List_List* ds){ if( add_cyc_namespace){ ds=({
struct Cyc_List_List* _temp1223=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1223->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1224=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1224->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1225=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1225[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1226; _temp1226.tag=
Cyc_Absyn_Namespace_d_tag; _temp1226.f1= Cyc_Tc_cyc_ns; _temp1226.f2= ds;
_temp1226;}); _temp1225;})); _temp1224->loc= 0; _temp1224;}); _temp1223->tl= 0;
_temp1223;});} Cyc_Tc_tc_decls( te, ds, 0);} static int Cyc_Tc_vardecl_needed(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Decl* d){ void* _temp1227=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp1237; struct Cyc_List_List* _temp1239; struct Cyc_List_List**
_temp1241; struct _tuple0* _temp1242; struct Cyc_List_List* _temp1244; struct
Cyc_List_List** _temp1246; struct _tagged_string* _temp1247; _LL1229: if(((
struct _tunion_struct*) _temp1227)->tag == Cyc_Absyn_Var_d_tag){ _LL1238:
_temp1237=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp1227)->f1; goto _LL1230;} else{ goto _LL1231;} _LL1231: if((( struct
_tunion_struct*) _temp1227)->tag == Cyc_Absyn_Using_d_tag){ _LL1243: _temp1242=(
struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp1227)->f1; goto
_LL1240; _LL1240: _temp1239=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1227)->f2; _temp1241=&(( struct Cyc_Absyn_Using_d_struct*) _temp1227)->f2;
goto _LL1232;} else{ goto _LL1233;} _LL1233: if((( struct _tunion_struct*)
_temp1227)->tag == Cyc_Absyn_Namespace_d_tag){ _LL1248: _temp1247=( struct
_tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp1227)->f1; goto
_LL1245; _LL1245: _temp1244=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1227)->f2; _temp1246=&(( struct Cyc_Absyn_Namespace_d_struct*) _temp1227)->f2;
goto _LL1234;} else{ goto _LL1235;} _LL1235: goto _LL1236; _LL1230: if(( void*)
_temp1237->sc !=( void*) Cyc_Absyn_Extern){ return 1;}{ struct _tuple0 _temp1251;
struct _tagged_string* _temp1252; void* _temp1254; struct _tuple0* _temp1249=
_temp1237->name; _temp1251=* _temp1249; _LL1255: _temp1254= _temp1251.f1; goto
_LL1253; _LL1253: _temp1252= _temp1251.f2; goto _LL1250; _LL1250: { struct Cyc_List_List*
ns;{ void* _temp1256= _temp1254; struct Cyc_List_List* _temp1264; struct Cyc_List_List*
_temp1266; _LL1258: if(( int) _temp1256 == Cyc_Absyn_Loc_n){ goto _LL1259;}
else{ goto _LL1260;} _LL1260: if(( unsigned int) _temp1256 > 1u?(( struct
_tunion_struct*) _temp1256)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL1265: _temp1264=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp1256)->f1; goto
_LL1261;} else{ goto _LL1262;} _LL1262: if(( unsigned int) _temp1256 > 1u?((
struct _tunion_struct*) _temp1256)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL1267:
_temp1266=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp1256)->f1;
goto _LL1263;} else{ goto _LL1257;} _LL1259: ns= 0; goto _LL1257; _LL1261: ns=
_temp1264; goto _LL1257; _LL1263: ns= _temp1266; goto _LL1257; _LL1257:;}{
struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, ns); return(*(( struct
_tuple6*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)(
ge->ordinaries, _temp1252)).f2;}}} _LL1232: _temp1246= _temp1241; goto _LL1234;
_LL1234:* _temp1246= Cyc_Tc_treeshake( te,* _temp1246); return 1; _LL1236:
return 1; _LL1228:;} struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)( int(* f)(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Decl*), struct Cyc_Tcenv_Tenv* env,
struct Cyc_List_List* l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, te, ds);}