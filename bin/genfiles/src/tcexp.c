#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2; } ; struct _tuple1{ void* f1; struct
_tagged_string* f2; } ; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual*
f2; void* f3; } ; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt*
f2; } ; struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; struct _tuple5{
int f1; void* f2; } ; struct _tuple6{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp*
f2; } ; struct _tuple7{ struct Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple8{
struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2; } ; typedef
unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct _tagged_string
Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef struct
_tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit( int);
extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern void* Cyc_Core_snd( struct _tuple0*); extern char Cyc_Core_InvalidArg_tag[
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
char* tag; } ; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[ 14u]; struct
Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag; } ; extern void* Cyc_List_nth( struct Cyc_List_List*
x, int i); extern struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x,
struct Cyc_List_List* y); struct Cyc_Lineno_Pos{ struct _tagged_string
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
char* tag; } ; extern struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual(); extern
struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_empty_conref(); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_new_evar( void*); extern void* Cyc_Absyn_wildtyp();
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_ushort_t; extern void*
Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t; extern void* Cyc_Absyn_schar_t;
extern void* Cyc_Absyn_sshort_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t;
extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_string_typ(
void* rgn); extern void* Cyc_Absyn_file_typ(); extern void* Cyc_Absyn_atb_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq, void* b); extern void* Cyc_Absyn_star_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq); extern void* Cyc_Absyn_at_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern void* Cyc_Absyn_pointer_expand(
void*); extern int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*); extern struct
Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl*,
struct _tagged_string*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_union_field(
struct Cyc_Absyn_Uniondecl*, struct _tagged_string*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; extern int Cyc_String_zstrcmp( struct _tagged_string, struct
_tagged_string); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*);
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{ char* tag; } ;
struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef
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
extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*); extern struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Absyn_Stmt*
Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*); extern
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_block( struct Cyc_Position_Segment*, struct
Cyc_Tcenv_Tenv*); extern void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, void* rgn); extern void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff); extern
char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag; }
; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_string); extern void* Cyc_Tcutil_compress(
void* t); extern void Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*, struct
Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_List_List*); extern int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerceable( void*); extern
int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
void*, void*); extern int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*, struct
Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_integral_promote(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_arithmetic_promote(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_comparison_promote(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern void* Cyc_Tcutil_max_arithmetic_type(
void*, void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*);
extern struct _tuple4* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar*); extern
void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp*); extern void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*
bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_nonzero_bound( struct
Cyc_Position_Segment*, struct Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*, unsigned int i, struct Cyc_Absyn_Conref*); extern
struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern struct _tuple5 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e); extern int Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_szof( void* t, struct
Cyc_Position_Segment* loc); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s, int new_block); extern struct Cyc_List_List* Cyc_Formatstr_get_format_typs(
struct _tagged_string, struct Cyc_Position_Segment*); extern struct Cyc_List_List*
Cyc_Formatstr_get_scanf_typs( struct _tagged_string, struct Cyc_Position_Segment*);
extern void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*);
extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*, void**, struct
Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e, struct _tagged_string msg_part); typedef void* Cyc_CfFlowInfo_LocalRoot;
typedef void* Cyc_CfFlowInfo_Field; struct Cyc_CfFlowInfo_Place; typedef void*
Cyc_CfFlowInfo_Escaped; typedef void* Cyc_CfFlowInfo_InitLevel; typedef void*
Cyc_CfFlowInfo_InitState; typedef void* Cyc_CfFlowInfo_PathInfo; typedef void*
Cyc_CfFlowInfo_FlowInfo; typedef void* Cyc_CfFlowInfo_local_root_t; typedef void*
Cyc_CfFlowInfo_field_t; typedef struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t;
typedef void* Cyc_CfFlowInfo_escaped_t; typedef void* Cyc_CfFlowInfo_init_level_t;
typedef void* Cyc_CfFlowInfo_init_state_t; typedef void* Cyc_CfFlowInfo_path_info_t;
typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_pinfo_dict_t; typedef void* Cyc_CfFlowInfo_flow_info_t;
static const int Cyc_CfFlowInfo_VarRoot_tag= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt_tag=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_CfFlowInfo_StructF_tag= 0; struct Cyc_CfFlowInfo_StructF_struct{
int tag; struct _tagged_string* f1; } ; static const int Cyc_CfFlowInfo_TupleF_tag=
1; struct Cyc_CfFlowInfo_TupleF_struct{ int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{
void* root; struct Cyc_List_List* fields; } ; static const unsigned int Cyc_CfFlowInfo_Esc=
0; static const unsigned int Cyc_CfFlowInfo_Unesc= 1; static const unsigned int
Cyc_CfFlowInfo_NoneIL= 0; static const unsigned int Cyc_CfFlowInfo_ThisIL= 1;
static const unsigned int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_UnknownIS_tag=
0; struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag; void* f1; void* f2; } ;
static const int Cyc_CfFlowInfo_MustPointTo_tag= 1; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_LeafPI_tag=
0; struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; void* f1; } ; static const int
Cyc_CfFlowInfo_TuplePI_tag= 1; struct Cyc_CfFlowInfo_TuplePI_struct{ int tag;
struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI_tag= 2;
struct Cyc_CfFlowInfo_StructPI_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
static const unsigned int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_InitsFL_tag=
0; struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1; } ;
extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt* enclosee,
struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{ struct Cyc_Dict_Dict*
roots; int in_try; void* tryflow; } ; typedef struct Cyc_NewControlFlow_AnalEnv*
Cyc_NewControlFlow_analenv_t; void Cyc_NewControlFlow_cf_check( struct Cyc_List_List*
ds); static void* Cyc_Tcexp_expr_err( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_string msg){ Cyc_Tcutil_terr( loc, msg); return Cyc_Absyn_wildtyp();}
static void Cyc_Tcexp_resolve_unknown_id( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, struct _tuple1* q){ struct _handler_cons _temp0; _push_handler(& _temp0);{
struct _xtunion_struct* _temp1=( struct _xtunion_struct*) setjmp( _temp0.handler);
if( ! _temp1){{ void* _temp2= Cyc_Tcenv_lookup_ordinary( te, e->loc, q); void*
_temp14; struct Cyc_Absyn_Enumfield* _temp16; struct Cyc_Absyn_Enumdecl* _temp18;
struct Cyc_Absyn_Tunionfield* _temp20; struct Cyc_Absyn_Tuniondecl* _temp22;
struct Cyc_Absyn_Tunionfield* _temp24; struct Cyc_Absyn_XTuniondecl* _temp26;
struct Cyc_Absyn_Structdecl* _temp28; _LL4: if((( struct _tunion_struct*) _temp2)->tag
== Cyc_Tcenv_VarRes_tag){ _LL15: _temp14=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp2)->f1; goto _LL5;} else{ goto _LL6;} _LL6: if((( struct _tunion_struct*)
_temp2)->tag == Cyc_Tcenv_EnumRes_tag){ _LL19: _temp18=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp2)->f1; goto _LL17; _LL17: _temp16=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Tcenv_EnumRes_struct*) _temp2)->f2;
goto _LL7;} else{ goto _LL8;} _LL8: if((( struct _tunion_struct*) _temp2)->tag
== Cyc_Tcenv_TunionRes_tag){ _LL23: _temp22=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp2)->f1; goto _LL21; _LL21: _temp20=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*) _temp2)->f2;
goto _LL9;} else{ goto _LL10;} _LL10: if((( struct _tunion_struct*) _temp2)->tag
== Cyc_Tcenv_XTunionRes_tag){ _LL27: _temp26=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp2)->f1; goto _LL25; _LL25: _temp24=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_XTunionRes_struct*) _temp2)->f2;
goto _LL11;} else{ goto _LL12;} _LL12: if((( struct _tunion_struct*) _temp2)->tag
== Cyc_Tcenv_StructRes_tag){ _LL29: _temp28=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp2)->f1; goto _LL13;} else{ goto _LL3;}
_LL5:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct* _temp30=(
struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp30[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp31; _temp31.tag= Cyc_Absyn_Var_e_tag;
_temp31.f1= q; _temp31.f2=( void*) _temp14; _temp31;}); _temp30;}))); goto _LL3;
_LL7:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp32=(
struct Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp32[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp33; _temp33.tag= Cyc_Absyn_Enum_e_tag;
_temp33.f1= q; _temp33.f2=( struct Cyc_Absyn_Enumdecl*) _temp18; _temp33.f3=(
struct Cyc_Absyn_Enumfield*) _temp16; _temp33;}); _temp32;}))); goto _LL3; _LL9:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp34=(
struct Cyc_Absyn_Tunion_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct));
_temp34[ 0]=({ struct Cyc_Absyn_Tunion_e_struct _temp35; _temp35.tag= Cyc_Absyn_Tunion_e_tag;
_temp35.f1= 0; _temp35.f2= 0; _temp35.f3= 0; _temp35.f4= _temp22; _temp35.f5=
_temp20; _temp35;}); _temp34;}))); goto _LL3; _LL11:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_XTunion_e_struct* _temp36=( struct Cyc_Absyn_XTunion_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunion_e_struct)); _temp36[ 0]=({ struct
Cyc_Absyn_XTunion_e_struct _temp37; _temp37.tag= Cyc_Absyn_XTunion_e_tag;
_temp37.f1= 0; _temp37.f2= 0; _temp37.f3= _temp26; _temp37.f4= _temp24; _temp37;});
_temp36;}))); goto _LL3; _LL13: Cyc_Tcutil_terr( e->loc,({ struct _tagged_string
_temp38= Cyc_Absynpp_qvar2string( q); xprintf("bad occurrence of struct name %.*s",
_temp38.last_plus_one - _temp38.curr, _temp38.curr);}));( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Var_e_struct* _temp39=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp39[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp40; _temp40.tag= Cyc_Absyn_Var_e_tag; _temp40.f1= q; _temp40.f2=( void*)((
void*) Cyc_Absyn_Unresolved_b); _temp40;}); _temp39;}))); goto _LL3; _LL3:;};
_pop_handler();} else{ struct _xtunion_struct* _temp42= _temp1; _LL44: if(
_temp42->tag == Cyc_Dict_Absent_tag){ goto _LL45;} else{ goto _LL46;} _LL46:
goto _LL47; _LL45:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp48=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp48[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp49; _temp49.tag= Cyc_Absyn_Var_e_tag;
_temp49.f1= q; _temp49.f2=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp49;});
_temp48;}))); goto _LL43; _LL47:( void) _throw( _temp42); _LL43:;}}} static
struct _tuple6* Cyc_Tcexp_make_struct_arg( struct Cyc_Absyn_Exp* e){ return({
struct _tuple6* _temp50=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp50->f1= 0; _temp50->f2= e; _temp50;});} static void Cyc_Tcexp_resolve_unknown_fn(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* es){ void* _temp51=( void*) e1->r; struct _tuple1* _temp57;
_LL53: if((( struct _tunion_struct*) _temp51)->tag == Cyc_Absyn_UnknownId_e_tag){
_LL58: _temp57=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp51)->f1; goto _LL54;} else{ goto _LL55;} _LL55: goto _LL56; _LL54: { struct
_handler_cons _temp59; _push_handler(& _temp59);{ struct _xtunion_struct*
_temp60=( struct _xtunion_struct*) setjmp( _temp59.handler); if( ! _temp60){{
void* _temp61= Cyc_Tcenv_lookup_ordinary( te, e1->loc, _temp57); void* _temp73;
struct Cyc_Absyn_Tunionfield* _temp75; struct Cyc_Absyn_Tuniondecl* _temp77;
struct Cyc_Absyn_Tunionfield* _temp79; struct Cyc_Absyn_XTuniondecl* _temp81;
struct Cyc_Absyn_Structdecl* _temp83; struct Cyc_Absyn_Enumfield* _temp85;
struct Cyc_Absyn_Enumdecl* _temp87; _LL63: if((( struct _tunion_struct*) _temp61)->tag
== Cyc_Tcenv_VarRes_tag){ _LL74: _temp73=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp61)->f1; goto _LL64;} else{ goto _LL65;} _LL65: if((( struct _tunion_struct*)
_temp61)->tag == Cyc_Tcenv_TunionRes_tag){ _LL78: _temp77=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Tcenv_TunionRes_struct*) _temp61)->f1; goto _LL76; _LL76: _temp75=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_TunionRes_struct*) _temp61)->f2;
goto _LL66;} else{ goto _LL67;} _LL67: if((( struct _tunion_struct*) _temp61)->tag
== Cyc_Tcenv_XTunionRes_tag){ _LL82: _temp81=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Tcenv_XTunionRes_struct*) _temp61)->f1; goto _LL80; _LL80: _temp79=(
struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Tcenv_XTunionRes_struct*) _temp61)->f2;
goto _LL68;} else{ goto _LL69;} _LL69: if((( struct _tunion_struct*) _temp61)->tag
== Cyc_Tcenv_StructRes_tag){ _LL84: _temp83=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Tcenv_StructRes_struct*) _temp61)->f1; goto _LL70;} else{ goto _LL71;}
_LL71: if((( struct _tunion_struct*) _temp61)->tag == Cyc_Tcenv_EnumRes_tag){
_LL88: _temp87=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp61)->f1; goto _LL86; _LL86: _temp85=( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_Tcenv_EnumRes_struct*) _temp61)->f2; goto _LL72;} else{ goto _LL62;} _LL64:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp89=(
struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp89[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp90; _temp90.tag= Cyc_Absyn_FnCall_e_tag;
_temp90.f1= e1; _temp90.f2= es; _temp90;}); _temp89;}))); _npop_handler( 0u);
return; _LL66: if( _temp75->typs == 0){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp91= Cyc_Absynpp_qvar2string( _temp75->name); xprintf("%.*s is a constant, not a function",
_temp91.last_plus_one - _temp91.curr, _temp91.curr);}));}( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp92=( struct Cyc_Absyn_Tunion_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_e_struct)); _temp92[ 0]=({ struct Cyc_Absyn_Tunion_e_struct
_temp93; _temp93.tag= Cyc_Absyn_Tunion_e_tag; _temp93.f1= 0; _temp93.f2= 0;
_temp93.f3= es; _temp93.f4= _temp77; _temp93.f5= _temp75; _temp93;}); _temp92;})));
_npop_handler( 0u); return; _LL68: if( _temp79->typs == 0){ Cyc_Tcutil_terr( e->loc,({
struct _tagged_string _temp94= Cyc_Absynpp_qvar2string( _temp79->name); xprintf("%.*s is a constant, not a function",
_temp94.last_plus_one - _temp94.curr, _temp94.curr);}));}( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_XTunion_e_struct* _temp95=( struct Cyc_Absyn_XTunion_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunion_e_struct)); _temp95[ 0]=({ struct
Cyc_Absyn_XTunion_e_struct _temp96; _temp96.tag= Cyc_Absyn_XTunion_e_tag;
_temp96.f1= 0; _temp96.f2= es; _temp96.f3= _temp81; _temp96.f4= _temp79; _temp96;});
_temp95;}))); _npop_handler( 0u); return; _LL70: { struct Cyc_List_List* des=((
struct Cyc_List_List*(*)( struct _tuple6*(* f)( struct Cyc_Absyn_Exp*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcexp_make_struct_arg, es); if( _temp83->name
!= 0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp97=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp97[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp98; _temp98.tag= Cyc_Absyn_Struct_e_tag;
_temp98.f1=( struct _tuple1*)( _temp83->name)->v; _temp98.f2= 0; _temp98.f3= des;
_temp98.f4=( struct Cyc_Absyn_Structdecl*) _temp83; _temp98;}); _temp97;})));}
else{ Cyc_Tcutil_terr( e->loc,( struct _tagged_string)({ char* _temp99=( char*)"missing struct name";
struct _tagged_string _temp100; _temp100.curr= _temp99; _temp100.base= _temp99;
_temp100.last_plus_one= _temp99 + 20; _temp100;}));( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Struct_e_struct* _temp101=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp101[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp102; _temp102.tag= Cyc_Absyn_Struct_e_tag;
_temp102.f1= _temp57; _temp102.f2= 0; _temp102.f3= des; _temp102.f4=( struct Cyc_Absyn_Structdecl*)
_temp83; _temp102;}); _temp101;})));} _npop_handler( 0u); return;} _LL72: Cyc_Tcutil_terr(
e->loc,({ struct _tagged_string _temp103= Cyc_Absynpp_qvar2string( _temp57);
xprintf("%.*s is an enum constructor, not a function", _temp103.last_plus_one -
_temp103.curr, _temp103.curr);})); _npop_handler( 0u); return; _LL62:;};
_pop_handler();} else{ struct _xtunion_struct* _temp105= _temp60; _LL107: if(
_temp105->tag == Cyc_Dict_Absent_tag){ goto _LL108;} else{ goto _LL109;} _LL109:
goto _LL110; _LL108: Cyc_Tcutil_terr( e1->loc,({ struct _tagged_string _temp111=
Cyc_Absynpp_qvar2string( _temp57); xprintf("unbound identifier %.*s", _temp111.last_plus_one
- _temp111.curr, _temp111.curr);}));( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_FnCall_e_struct* _temp112=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp112[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp113; _temp113.tag= Cyc_Absyn_FnCall_e_tag;
_temp113.f1= e1; _temp113.f2= es; _temp113;}); _temp112;}))); return; _LL110:(
void) _throw( _temp105); _LL106:;}}} _LL56:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_FnCall_e_struct* _temp114=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp114[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp115; _temp115.tag= Cyc_Absyn_FnCall_e_tag;
_temp115.f1= e1; _temp115.f2= es; _temp115;}); _temp114;}))); return; _LL52:;}
static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
des){ if( topt == 0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp116=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp116[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp117; _temp117.tag= Cyc_Absyn_Array_e_tag;
_temp117.f1= des; _temp117;}); _temp116;}))); return;}{ void* t=* topt; void*
_temp118= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp126; struct Cyc_Absyn_Tqual*
_temp128; void* _temp130; struct Cyc_Absyn_Structdecl** _temp132; struct Cyc_List_List*
_temp134; struct _tuple1* _temp136; _LL120: if(( unsigned int) _temp118 > 4u?((
struct _tunion_struct*) _temp118)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL131:
_temp130=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp118)->f1; goto
_LL129; _LL129: _temp128=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp118)->f2; goto _LL127; _LL127: _temp126=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp118)->f3; goto _LL121;} else{ goto _LL122;}
_LL122: if(( unsigned int) _temp118 > 4u?(( struct _tunion_struct*) _temp118)->tag
== Cyc_Absyn_StructType_tag: 0){ _LL137: _temp136=( struct _tuple1*)(( struct
Cyc_Absyn_StructType_struct*) _temp118)->f1; goto _LL135; _LL135: _temp134=(
struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*) _temp118)->f2;
goto _LL133; _LL133: _temp132=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp118)->f3; goto _LL123;} else{ goto _LL124;} _LL124: goto _LL125; _LL121:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct* _temp138=(
struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp138[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp139; _temp139.tag= Cyc_Absyn_Array_e_tag;
_temp139.f1= des; _temp139;}); _temp138;}))); goto _LL119; _LL123: if( _temp132
== 0? 1: _temp136 == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp140=( char*)"Compiler bug: struct type not properly set"; struct
_tagged_string _temp141; _temp141.curr= _temp140; _temp141.base= _temp140;
_temp141.last_plus_one= _temp140 + 43; _temp141;}));}( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Struct_e_struct* _temp142=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp142[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp143; _temp143.tag= Cyc_Absyn_Struct_e_tag;
_temp143.f1=( struct _tuple1*)({ struct _tuple1* _temp144= _temp136; if(
_temp144 == 0){ _throw( Null_Exception);} _temp144;}); _temp143.f2= 0; _temp143.f3=
des; _temp143.f4=( struct Cyc_Absyn_Structdecl*)* _temp132; _temp143;});
_temp142;}))); goto _LL119; _LL125:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp145=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp145[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp146; _temp146.tag= Cyc_Absyn_Array_e_tag;
_temp146.f1= des; _temp146;}); _temp145;}))); goto _LL119; _LL119:;}} static
void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* es){ for( 0; es != 0; es= es->tl){ Cyc_Tcexp_tcExp( te,
0,( struct Cyc_Absyn_Exp*) es->hd);}} void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, struct _tagged_string msg_part){ Cyc_Tcutil_check_contains_assign(
e); Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool(
te, e)){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp147= msg_part;
struct _tagged_string _temp148= Cyc_Absynpp_typ2string(( void*)( e->topt)->v);
xprintf("test of %.*s has type %.*s instead of integral or * type", _temp147.last_plus_one
- _temp147.curr, _temp147.curr, _temp148.last_plus_one - _temp148.curr, _temp148.curr);}));}}
static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* c, struct Cyc_Absyn_Exp* e){ void* t;{ void* _temp149= c;
char _temp165; void* _temp167; short _temp169; void* _temp171; long long
_temp173; void* _temp175; struct _tagged_string _temp177; int _temp179; void*
_temp181; struct _tagged_string _temp183; _LL151: if(( unsigned int) _temp149 >
1u?(( struct _tunion_struct*) _temp149)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL168:
_temp167=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp149)->f1; goto _LL166;
_LL166: _temp165=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp149)->f2; goto
_LL152;} else{ goto _LL153;} _LL153: if(( unsigned int) _temp149 > 1u?(( struct
_tunion_struct*) _temp149)->tag == Cyc_Absyn_Short_c_tag: 0){ _LL172: _temp171=(
void*)(( struct Cyc_Absyn_Short_c_struct*) _temp149)->f1; goto _LL170; _LL170:
_temp169=( short)(( struct Cyc_Absyn_Short_c_struct*) _temp149)->f2; goto _LL154;}
else{ goto _LL155;} _LL155: if(( unsigned int) _temp149 > 1u?(( struct
_tunion_struct*) _temp149)->tag == Cyc_Absyn_LongLong_c_tag: 0){ _LL176:
_temp175=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp149)->f1; goto
_LL174; _LL174: _temp173=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp149)->f2; goto _LL156;} else{ goto _LL157;} _LL157: if(( unsigned int)
_temp149 > 1u?(( struct _tunion_struct*) _temp149)->tag == Cyc_Absyn_Float_c_tag:
0){ _LL178: _temp177=( struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*)
_temp149)->f1; goto _LL158;} else{ goto _LL159;} _LL159: if(( unsigned int)
_temp149 > 1u?(( struct _tunion_struct*) _temp149)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL182: _temp181=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp149)->f1;
goto _LL180; _LL180: _temp179=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp149)->f2;
goto _LL160;} else{ goto _LL161;} _LL161: if(( unsigned int) _temp149 > 1u?((
struct _tunion_struct*) _temp149)->tag == Cyc_Absyn_String_c_tag: 0){ _LL184:
_temp183=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp149)->f1;
goto _LL162;} else{ goto _LL163;} _LL163: if(( int) _temp149 == Cyc_Absyn_Null_c){
goto _LL164;} else{ goto _LL150;} _LL152: t= _temp167 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t; goto _LL150; _LL154: t= _temp171 ==( void*)
Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t; goto _LL150; _LL156:
t= _temp175 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulong_t: Cyc_Absyn_slong_t;
goto _LL150; _LL158: t= Cyc_Absyn_float_t; goto _LL150; _LL160: if( topt == 0){
t= _temp181 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;}
else{ void* _temp185=* topt; void* _temp195; void* _temp197; void* _temp199;
void* _temp201; void* _temp203; void* _temp205; _LL187: if(( unsigned int)
_temp185 > 4u?(( struct _tunion_struct*) _temp185)->tag == Cyc_Absyn_IntType_tag:
0){ _LL198: _temp197=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp185)->f1;
goto _LL196; _LL196: _temp195=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp185)->f2; if(( int) _temp195 == Cyc_Absyn_B1){ goto _LL188;} else{ goto
_LL189;}} else{ goto _LL189;} _LL189: if(( unsigned int) _temp185 > 4u?(( struct
_tunion_struct*) _temp185)->tag == Cyc_Absyn_IntType_tag: 0){ _LL202: _temp201=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp185)->f1; goto _LL200; _LL200:
_temp199=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp185)->f2; if(( int)
_temp199 == Cyc_Absyn_B2){ goto _LL190;} else{ goto _LL191;}} else{ goto _LL191;}
_LL191: if(( unsigned int) _temp185 > 4u?(( struct _tunion_struct*) _temp185)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL206: _temp205=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp185)->f1; goto _LL204; _LL204: _temp203=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp185)->f2; goto _LL192;} else{ goto _LL193;} _LL193: goto _LL194; _LL188: t=
_temp197 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp207=(
struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp207[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp208; _temp208.tag= Cyc_Absyn_Const_e_tag;
_temp208.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp209=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp209[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp210; _temp210.tag= Cyc_Absyn_Char_c_tag;
_temp210.f1=( void*) _temp197; _temp210.f2=( char) _temp179; _temp210;});
_temp209;})); _temp208;}); _temp207;}))); goto _LL186; _LL190: t= _temp201 ==(
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp211=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp211[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp212; _temp212.tag= Cyc_Absyn_Const_e_tag; _temp212.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp213=( struct Cyc_Absyn_Short_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp213[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp214; _temp214.tag= Cyc_Absyn_Short_c_tag; _temp214.f1=( void*) _temp201;
_temp214.f2=( short) _temp179; _temp214;}); _temp213;})); _temp212;}); _temp211;})));
goto _LL186; _LL192: t= _temp205 ==( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL186; _LL194: t= _temp181 ==( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL186; _LL186:;} goto _LL150; _LL162: {
int len=( int)({ struct _tagged_string _temp215= _temp183;( unsigned int)(
_temp215.last_plus_one - _temp215.curr);}); struct Cyc_Absyn_Const_e_struct* r=({
struct Cyc_Absyn_Const_e_struct* _temp216=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp216[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp217; _temp217.tag= Cyc_Absyn_Const_e_tag; _temp217.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp218=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp218[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp219; _temp219.tag= Cyc_Absyn_Int_c_tag; _temp219.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp219.f2= len; _temp219;}); _temp218;})); _temp217;}); _temp216;}); struct
Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp(( void*) r, loc); elen->topt=({ struct
Cyc_Core_Opt* _temp220=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp220->v=( void*) Cyc_Absyn_uint_t; _temp220;}); t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp221=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp221[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp222; _temp222.tag= Cyc_Absyn_Upper_b_tag;
_temp222.f1= elen; _temp222;}); _temp221;})); if( topt != 0){ void* _temp223=
Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_Exp* _temp231; struct Cyc_Absyn_Tqual*
_temp233; void* _temp235; struct Cyc_Absyn_PtrInfo _temp237; struct Cyc_Absyn_Conref*
_temp239; struct Cyc_Absyn_Tqual* _temp241; struct Cyc_Absyn_Conref* _temp243;
void* _temp245; void* _temp247; _LL225: if(( unsigned int) _temp223 > 4u?((
struct _tunion_struct*) _temp223)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL236:
_temp235=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp223)->f1; goto
_LL234; _LL234: _temp233=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp223)->f2; goto _LL232; _LL232: _temp231=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp223)->f3; goto _LL226;} else{ goto _LL227;}
_LL227: if(( unsigned int) _temp223 > 4u?(( struct _tunion_struct*) _temp223)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL238: _temp237=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp223)->f1; _LL248: _temp247=( void*)
_temp237.elt_typ; goto _LL246; _LL246: _temp245=( void*) _temp237.rgn_typ; goto
_LL244; _LL244: _temp243=( struct Cyc_Absyn_Conref*) _temp237.nullable; goto
_LL242; _LL242: _temp241=( struct Cyc_Absyn_Tqual*) _temp237.tq; goto _LL240;
_LL240: _temp239=( struct Cyc_Absyn_Conref*) _temp237.bounds; goto _LL228;}
else{ goto _LL229;} _LL229: goto _LL230; _LL226: return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp249=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp249[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp250; _temp250.tag= Cyc_Absyn_ArrayType_tag;
_temp250.f1=( void*) Cyc_Absyn_uchar_t; _temp250.f2= _temp233; _temp250.f3=(
struct Cyc_Absyn_Exp*) elen; _temp250;}); _temp249;}); _LL228: if( ! Cyc_Tcutil_unify(*
topt, t)? Cyc_Tcutil_silent_castable( te, loc, t,* topt): 0){ e->topt=({ struct
Cyc_Core_Opt* _temp251=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp251->v=( void*) t; _temp251;}); Cyc_Tcutil_unchecked_cast( te, e,* topt); t=*
topt;} goto _LL224; _LL230: goto _LL224; _LL224:;} return t;} _LL164: t=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp252=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp252[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp253; _temp253.tag= Cyc_Absyn_PointerType_tag;
_temp253.f1=({ struct Cyc_Absyn_PtrInfo _temp254; _temp254.elt_typ=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind); _temp254.rgn_typ=( void*) Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_RgnKind); _temp254.nullable=(( struct Cyc_Absyn_Conref*(*)( int x))
Cyc_Absyn_new_conref)( 1); _temp254.tq= Cyc_Absyn_empty_tqual(); _temp254.bounds=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp254;}); _temp253;});
_temp252;}); goto _LL150; _LL150:;} return t;} static void* Cyc_Tcexp_tcVar(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple1* q,
void* b){ void* _temp255= b; struct Cyc_Absyn_Vardecl* _temp269; struct Cyc_Absyn_Fndecl*
_temp271; struct Cyc_Absyn_Vardecl* _temp273; struct Cyc_Absyn_Vardecl* _temp275;
struct Cyc_Absyn_Vardecl* _temp277; _LL257: if(( int) _temp255 == Cyc_Absyn_Unresolved_b){
goto _LL258;} else{ goto _LL259;} _LL259: if(( unsigned int) _temp255 > 1u?((
struct _tunion_struct*) _temp255)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL270:
_temp269=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*)
_temp255)->f1; goto _LL260;} else{ goto _LL261;} _LL261: if(( unsigned int)
_temp255 > 1u?(( struct _tunion_struct*) _temp255)->tag == Cyc_Absyn_Funname_b_tag:
0){ _LL272: _temp271=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp255)->f1; goto _LL262;} else{ goto _LL263;} _LL263: if(( unsigned int)
_temp255 > 1u?(( struct _tunion_struct*) _temp255)->tag == Cyc_Absyn_Pat_b_tag:
0){ _LL274: _temp273=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Pat_b_struct*)
_temp255)->f1; goto _LL264;} else{ goto _LL265;} _LL265: if(( unsigned int)
_temp255 > 1u?(( struct _tunion_struct*) _temp255)->tag == Cyc_Absyn_Local_b_tag:
0){ _LL276: _temp275=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*)
_temp255)->f1; goto _LL266;} else{ goto _LL267;} _LL267: if(( unsigned int)
_temp255 > 1u?(( struct _tunion_struct*) _temp255)->tag == Cyc_Absyn_Param_b_tag:
0){ _LL278: _temp277=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp255)->f1; goto _LL268;} else{ goto _LL256;} _LL258: return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp279= Cyc_Absynpp_qvar2string( q); xprintf("unbound identifier %.*s",
_temp279.last_plus_one - _temp279.curr, _temp279.curr);})); _LL260:({ struct
_tuple1* _temp280= q; unsigned int _temp281= 0; if( _temp281 >= 1u){ _throw(
Null_Exception);} _temp280[ _temp281]=({ struct _tuple1* _temp282= _temp269->name;
unsigned int _temp283= 0; if( _temp283 >= 1u){ _throw( Null_Exception);}
_temp282[ _temp283];});}); return( void*) _temp269->type; _LL262:({ struct
_tuple1* _temp284= q; unsigned int _temp285= 0; if( _temp285 >= 1u){ _throw(
Null_Exception);} _temp284[ _temp285]=({ struct _tuple1* _temp286= _temp271->name;
unsigned int _temp287= 0; if( _temp287 >= 1u){ _throw( Null_Exception);}
_temp286[ _temp287];});}); return Cyc_Tcutil_fndecl2typ( _temp271); _LL264:
_temp275= _temp273; goto _LL266; _LL266: _temp277= _temp275; goto _LL268; _LL268:(*
q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp277->type; _LL256:;} static
void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_List_List* args, struct Cyc_List_List*(* type_getter)( struct
_tagged_string, struct Cyc_Position_Segment*)){ struct Cyc_List_List* desc_types;{
void* _temp288=( void*) fmt->r; void* _temp294; struct _tagged_string _temp296;
_LL290: if((( struct _tunion_struct*) _temp288)->tag == Cyc_Absyn_Const_e_tag){
_LL295: _temp294=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp288)->f1; if((
unsigned int) _temp294 > 1u?(( struct _tunion_struct*) _temp294)->tag == Cyc_Absyn_String_c_tag:
0){ _LL297: _temp296=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp294)->f1; goto _LL291;} else{ goto _LL292;}} else{ goto _LL292;} _LL292:
goto _LL293; _LL291: desc_types= type_getter( _temp296, fmt->loc); goto _LL289;
_LL293: Cyc_Tcutil_terr( fmt->loc,( struct _tagged_string)({ char* _temp298=(
char*)"expecting a literal format string"; struct _tagged_string _temp299;
_temp299.curr= _temp298; _temp299.base= _temp298; _temp299.last_plus_one=
_temp298 + 34; _temp299;})); return; _LL289:;} for( 0; desc_types != 0? args !=
0: 0; desc_types= desc_types->tl, args= args->tl){ void* t=( void*) desc_types->hd;
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*) args->hd; if( ! Cyc_Tcutil_coerce_arg(
te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp300= Cyc_Absynpp_typ2string(
t); struct _tagged_string _temp301= Cyc_Absynpp_typ2string(( void*)( e->topt)->v);
xprintf("descriptor has type %.*s but argument has type %.*s", _temp300.last_plus_one
- _temp300.curr, _temp300.curr, _temp301.last_plus_one - _temp301.curr, _temp301.curr);}));}}
if( desc_types != 0){ Cyc_Tcutil_terr( fmt->loc,( struct _tagged_string)({ char*
_temp302=( char*)"too few arguments"; struct _tagged_string _temp303; _temp303.curr=
_temp302; _temp303.base= _temp302; _temp303.last_plus_one= _temp302 + 18;
_temp303;}));} if( args != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) args->hd)->loc,(
struct _tagged_string)({ char* _temp304=( char*)"too many arguments"; struct
_tagged_string _temp305; _temp305.curr= _temp304; _temp305.base= _temp304;
_temp305.last_plus_one= _temp304 + 19; _temp305;}));}} static void* Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
p, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress(( void*)( e->topt)->v);
void* _temp306= p; _LL308: if(( int) _temp306 == Cyc_Absyn_Plus){ goto _LL309;}
else{ goto _LL310;} _LL310: if(( int) _temp306 == Cyc_Absyn_Minus){ goto _LL311;}
else{ goto _LL312;} _LL312: if(( int) _temp306 == Cyc_Absyn_Not){ goto _LL313;}
else{ goto _LL314;} _LL314: if(( int) _temp306 == Cyc_Absyn_Bitnot){ goto _LL315;}
else{ goto _LL316;} _LL316: if(( int) _temp306 == Cyc_Absyn_Size){ goto _LL317;}
else{ goto _LL318;} _LL318: goto _LL319; _LL309: goto _LL311; _LL311: if( ! Cyc_Tcutil_arithmetic_promote(
te, e)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp320= Cyc_Absynpp_typ2string(
t); xprintf("expecting arithmetic type but found %.*s", _temp320.last_plus_one -
_temp320.curr, _temp320.curr);}));} return( void*)( e->topt)->v; _LL313: Cyc_Tcutil_check_contains_assign(
e); if( ! Cyc_Tcutil_coerce_to_bool( te, e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp321= Cyc_Absynpp_typ2string( t); xprintf("expecting integral or * type but found %.*s",
_temp321.last_plus_one - _temp321.curr, _temp321.curr);}));} return Cyc_Absyn_sint_t;
_LL315: if( ! Cyc_Tcutil_integral_promote( te, e)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp322= Cyc_Absynpp_typ2string( t); xprintf("expecting integral type but found %.*s",
_temp322.last_plus_one - _temp322.curr, _temp322.curr);}));} return( void*)( e->topt)->v;
_LL317:{ void* _temp323= t; struct Cyc_Absyn_Exp* _temp331; struct Cyc_Absyn_Tqual*
_temp333; void* _temp335; struct Cyc_Absyn_PtrInfo _temp337; struct Cyc_Absyn_Conref*
_temp339; struct Cyc_Absyn_Tqual* _temp341; struct Cyc_Absyn_Conref* _temp343;
void* _temp345; void* _temp347; _LL325: if(( unsigned int) _temp323 > 4u?((
struct _tunion_struct*) _temp323)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL336:
_temp335=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp323)->f1; goto
_LL334; _LL334: _temp333=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp323)->f2; goto _LL332; _LL332: _temp331=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp323)->f3; goto _LL326;} else{ goto _LL327;}
_LL327: if(( unsigned int) _temp323 > 4u?(( struct _tunion_struct*) _temp323)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL338: _temp337=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp323)->f1; _LL348: _temp347=( void*)
_temp337.elt_typ; goto _LL346; _LL346: _temp345=( void*) _temp337.rgn_typ; goto
_LL344; _LL344: _temp343=( struct Cyc_Absyn_Conref*) _temp337.nullable; goto
_LL342; _LL342: _temp341=( struct Cyc_Absyn_Tqual*) _temp337.tq; goto _LL340;
_LL340: _temp339=( struct Cyc_Absyn_Conref*) _temp337.bounds; goto _LL328;}
else{ goto _LL329;} _LL329: goto _LL330; _LL326: goto _LL324; _LL328: goto
_LL324; _LL330: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp349= Cyc_Absynpp_typ2string(
t); xprintf("size requires pointer or array type, not %.*s", _temp349.last_plus_one
- _temp349.curr, _temp349.curr);})); goto _LL324; _LL324:;} return Cyc_Absyn_uint_t;
_LL319: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp350=( char*)"Non-unary primop"; struct
_tagged_string _temp351; _temp351.curr= _temp350; _temp351.base= _temp350;
_temp351.last_plus_one= _temp350 + 17; _temp351;})); _LL307:;} static void* Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
int(* promoter)( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*)){ if( ! promoter(
te, e1)){ Cyc_Tcutil_terr( e1->loc,({ struct _tagged_string _temp352= Cyc_Absynpp_typ2string((
void*)( e1->topt)->v); xprintf("type %.*s cannot be used here", _temp352.last_plus_one
- _temp352.curr, _temp352.curr);})); return Cyc_Absyn_wildtyp();} if( ! promoter(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp353= Cyc_Absynpp_typ2string((
void*)( e2->topt)->v); xprintf("type %.*s cannot be used here", _temp353.last_plus_one
- _temp353.curr, _temp353.curr);})); return Cyc_Absyn_wildtyp();}{ void* t1= Cyc_Tcutil_compress((
void*)( e1->topt)->v); void* t2= Cyc_Tcutil_compress(( void*)( e2->topt)->v);
void* res= Cyc_Tcutil_max_arithmetic_type( t1, t2); Cyc_Tcutil_unchecked_cast(
te, e1, res); Cyc_Tcutil_unchecked_cast( te, e2, res); return res;}} static void*
Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct
Cyc_Absyn_Exp* e2){ void* t1=( void*)( e1->topt)->v; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ Cyc_Tcutil_terr( e2->loc,({
struct _tagged_string _temp354= Cyc_Absynpp_typ2string(( void*)( e2->topt)->v);
xprintf("expecting int but found %.*s", _temp354.last_plus_one - _temp354.curr,
_temp354.curr);}));} return t1;} else{ return Cyc_Tcexp_tcArithBinop( te, e1, e2,
Cyc_Tcutil_arithmetic_promote);}} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)( e1->topt)->v;
void* t2=( void*)( e2->topt)->v; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if(
Cyc_Tcutil_is_tagged_pointer_typ( t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){ Cyc_Tcutil_terr(
e1->loc,({ struct _tagged_string _temp355= Cyc_Absynpp_typ2string(( void*)( e1->topt)->v);
struct _tagged_string _temp356= Cyc_Absynpp_typ2string(( void*)( e2->topt)->v);
xprintf("pointer arithmetic on values of different types (%.*s != %.*s)",
_temp355.last_plus_one - _temp355.curr, _temp355.curr, _temp356.last_plus_one -
_temp356.curr, _temp356.curr);}));} return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp357= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp358= Cyc_Absynpp_typ2string( t2); xprintf("expecting either %.*s or int but found %.*s",
_temp357.last_plus_one - _temp357.curr, _temp357.curr, _temp358.last_plus_one -
_temp358.curr, _temp358.curr);}));} return t1;}} else{ return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_arithmetic_promote);}} static void* Cyc_Tcexp_tcAnyBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ int e1_is_unboxed_arith= Cyc_Tcutil_comparison_promote(
te, e1); int e2_is_unboxed_arith= Cyc_Tcutil_comparison_promote( te, e2); void*
t1= Cyc_Tcutil_compress(( void*)( e1->topt)->v); void* t2= Cyc_Tcutil_compress((
void*)( e2->topt)->v); if( e1_is_unboxed_arith? e2_is_unboxed_arith: 0){ void*
res= Cyc_Tcutil_max_arithmetic_type( t1, t2); Cyc_Tcutil_unchecked_cast( te, e1,
res); Cyc_Tcutil_unchecked_cast( te, e2, res); return Cyc_Absyn_sint_t;} else{
if( Cyc_Tcutil_unify( t1, t2)? Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar(( void*)
Cyc_Absyn_BoxKind)): 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable(
te, loc, t2, t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;}
else{ if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast(
te, e1, t2); return Cyc_Absyn_sint_t;} else{ void* t1=( void*)( e1->topt)->v;
void* t2=( void*)( e2->topt)->v;{ struct _tuple0 _temp360=({ struct _tuple0
_temp359; _temp359.f1= Cyc_Tcutil_compress( t1); _temp359.f2= Cyc_Tcutil_compress(
t2); _temp359;}); void* _temp366; struct Cyc_Absyn_PtrInfo _temp368; struct Cyc_Absyn_Conref*
_temp370; struct Cyc_Absyn_Tqual* _temp372; struct Cyc_Absyn_Conref* _temp374;
void* _temp376; void* _temp378; void* _temp380; struct Cyc_Absyn_PtrInfo
_temp382; struct Cyc_Absyn_Conref* _temp384; struct Cyc_Absyn_Tqual* _temp386;
struct Cyc_Absyn_Conref* _temp388; void* _temp390; void* _temp392; _LL362:
_LL381: _temp380= _temp360.f1; if(( unsigned int) _temp380 > 4u?(( struct
_tunion_struct*) _temp380)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL383:
_temp382=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp380)->f1; _LL393: _temp392=( void*) _temp382.elt_typ; goto _LL391; _LL391:
_temp390=( void*) _temp382.rgn_typ; goto _LL389; _LL389: _temp388=( struct Cyc_Absyn_Conref*)
_temp382.nullable; goto _LL387; _LL387: _temp386=( struct Cyc_Absyn_Tqual*)
_temp382.tq; goto _LL385; _LL385: _temp384=( struct Cyc_Absyn_Conref*) _temp382.bounds;
goto _LL367;} else{ goto _LL364;} _LL367: _temp366= _temp360.f2; if((
unsigned int) _temp366 > 4u?(( struct _tunion_struct*) _temp366)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL369: _temp368=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp366)->f1; _LL379: _temp378=( void*) _temp368.elt_typ; goto _LL377; _LL377:
_temp376=( void*) _temp368.rgn_typ; goto _LL375; _LL375: _temp374=( struct Cyc_Absyn_Conref*)
_temp368.nullable; goto _LL373; _LL373: _temp372=( struct Cyc_Absyn_Tqual*)
_temp368.tq; goto _LL371; _LL371: _temp370=( struct Cyc_Absyn_Conref*) _temp368.bounds;
goto _LL363;} else{ goto _LL364;} _LL364: goto _LL365; _LL363: if( Cyc_Tcutil_unify(
_temp392, _temp378)){ return Cyc_Absyn_sint_t;} goto _LL361; _LL365: goto _LL361;
_LL361:;} Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp394= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp395= Cyc_Absynpp_typ2string( t2); xprintf("comparison not allowed between %.*s and %.*s",
_temp394.last_plus_one - _temp394.curr, _temp394.curr, _temp395.last_plus_one -
_temp395.curr, _temp395.curr);})); return Cyc_Absyn_wildtyp();}}}}} static void*
Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* _temp396= p; _LL398: if(( int) _temp396 == Cyc_Absyn_Plus){ goto _LL399;}
else{ goto _LL400;} _LL400: if(( int) _temp396 == Cyc_Absyn_Minus){ goto _LL401;}
else{ goto _LL402;} _LL402: if(( int) _temp396 == Cyc_Absyn_Times){ goto _LL403;}
else{ goto _LL404;} _LL404: if(( int) _temp396 == Cyc_Absyn_Div){ goto _LL405;}
else{ goto _LL406;} _LL406: if(( int) _temp396 == Cyc_Absyn_Mod){ goto _LL407;}
else{ goto _LL408;} _LL408: if(( int) _temp396 == Cyc_Absyn_Bitand){ goto _LL409;}
else{ goto _LL410;} _LL410: if(( int) _temp396 == Cyc_Absyn_Bitor){ goto _LL411;}
else{ goto _LL412;} _LL412: if(( int) _temp396 == Cyc_Absyn_Bitxor){ goto _LL413;}
else{ goto _LL414;} _LL414: if(( int) _temp396 == Cyc_Absyn_Bitlshift){ goto
_LL415;} else{ goto _LL416;} _LL416: if(( int) _temp396 == Cyc_Absyn_Bitlrshift){
goto _LL417;} else{ goto _LL418;} _LL418: if(( int) _temp396 == Cyc_Absyn_Bitarshift){
goto _LL419;} else{ goto _LL420;} _LL420: if(( int) _temp396 == Cyc_Absyn_Eq){
goto _LL421;} else{ goto _LL422;} _LL422: if(( int) _temp396 == Cyc_Absyn_Neq){
goto _LL423;} else{ goto _LL424;} _LL424: if(( int) _temp396 == Cyc_Absyn_Gt){
goto _LL425;} else{ goto _LL426;} _LL426: if(( int) _temp396 == Cyc_Absyn_Lt){
goto _LL427;} else{ goto _LL428;} _LL428: if(( int) _temp396 == Cyc_Absyn_Gte){
goto _LL429;} else{ goto _LL430;} _LL430: if(( int) _temp396 == Cyc_Absyn_Lte){
goto _LL431;} else{ goto _LL432;} _LL432: goto _LL433; _LL399: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL401: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL403: goto _LL405;
_LL405: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_arithmetic_promote);
_LL407: goto _LL409; _LL409: goto _LL411; _LL411: goto _LL413; _LL413: goto
_LL415; _LL415: goto _LL417; _LL417: goto _LL419; _LL419: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_integral_promote); _LL421: goto _LL423; _LL423: goto
_LL425; _LL425: goto _LL427; _LL427: goto _LL429; _LL429: goto _LL431; _LL431:
return Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL433:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Core_Failure_struct* _temp434=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp434[ 0]=({ struct Cyc_Core_Failure_struct
_temp435; _temp435.tag= Cyc_Core_Failure_tag; _temp435.f1=( struct
_tagged_string)({ char* _temp436=( char*)"bad binary primop"; struct
_tagged_string _temp437; _temp437.curr= _temp436; _temp437.base= _temp436;
_temp437.last_plus_one= _temp436 + 18; _temp437;}); _temp435;}); _temp434;}));
_LL397:;} static void* Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_List_List* es){ if( p ==( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) == 1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*) es->hd);} Cyc_Tcexp_tcExpList( te, es);{ void*
t;{ void* _temp438= p; _LL440: if(( int) _temp438 == Cyc_Absyn_Printf){ goto
_LL441;} else{ goto _LL442;} _LL442: if(( int) _temp438 == Cyc_Absyn_Fprintf){
goto _LL443;} else{ goto _LL444;} _LL444: if(( int) _temp438 == Cyc_Absyn_Xprintf){
goto _LL445;} else{ goto _LL446;} _LL446: if(( int) _temp438 == Cyc_Absyn_Scanf){
goto _LL447;} else{ goto _LL448;} _LL448: if(( int) _temp438 == Cyc_Absyn_Fscanf){
goto _LL449;} else{ goto _LL450;} _LL450: if(( int) _temp438 == Cyc_Absyn_Sscanf){
goto _LL451;} else{ goto _LL452;} _LL452: goto _LL453; _LL441: if( es == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp454=(
char*)"missing format string in printf"; struct _tagged_string _temp455;
_temp455.curr= _temp454; _temp455.base= _temp454; _temp455.last_plus_one=
_temp454 + 32; _temp455;}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)
es->hd, es->tl, Cyc_Formatstr_get_format_typs); t=( void*) Cyc_Absyn_VoidType;
goto _LL439; _LL443: if( es == 0? 1: es->tl == 0){ return Cyc_Tcexp_expr_err( te,
loc,( struct _tagged_string)({ char* _temp456=( char*)"too few arguments to fprintf";
struct _tagged_string _temp457; _temp457.curr= _temp456; _temp457.base= _temp456;
_temp457.last_plus_one= _temp456 + 29; _temp457;}));} if( ! Cyc_Tcutil_unify((
void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v, Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*) es->hd)->loc,({ struct _tagged_string _temp458= Cyc_Absynpp_typ2string((
void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v); xprintf("expecting FILE @ but found %.*s",
_temp458.last_plus_one - _temp458.curr, _temp458.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)( es->tl)->hd,( es->tl)->tl, Cyc_Formatstr_get_format_typs);
t=( void*) Cyc_Absyn_VoidType; goto _LL439; _LL445: if( es == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp459=( char*)"missing format string in xprintf";
struct _tagged_string _temp460; _temp460.curr= _temp459; _temp460.base= _temp459;
_temp460.last_plus_one= _temp459 + 33; _temp460;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*) es->hd, es->tl, Cyc_Formatstr_get_format_typs); t=
Cyc_Absyn_string_typ( Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind)); goto
_LL439; _LL447: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp461=( char*)"missing format string in scanf";
struct _tagged_string _temp462; _temp462.curr= _temp461; _temp462.base= _temp461;
_temp462.last_plus_one= _temp461 + 31; _temp462;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*) es->hd, es->tl, Cyc_Formatstr_get_scanf_typs); t=
Cyc_Absyn_sint_t; goto _LL439; _LL449: if( es == 0? 1: es->tl == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp463=( char*)"too few arguments to fscanf";
struct _tagged_string _temp464; _temp464.curr= _temp463; _temp464.base= _temp463;
_temp464.last_plus_one= _temp463 + 28; _temp464;}));} if( ! Cyc_Tcutil_unify((
void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v, Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*) es->hd)->loc,({ struct _tagged_string _temp465= Cyc_Absynpp_typ2string((
void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v); xprintf("expecting FILE @ but found %.*s",
_temp465.last_plus_one - _temp465.curr, _temp465.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)( es->tl)->hd,( es->tl)->tl, Cyc_Formatstr_get_scanf_typs);
t= Cyc_Absyn_sint_t; goto _LL439; _LL451: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp466=( char*)"sscanf is not supported"; struct
_tagged_string _temp467; _temp467.curr= _temp466; _temp467.base= _temp466;
_temp467.last_plus_one= _temp466 + 24; _temp467;})); t= Cyc_Absyn_sint_t; goto
_LL439; _LL453: switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es)){
case 0: _LL468: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp470=( char*)"primitive operator has 0 arguments"; struct
_tagged_string _temp471; _temp471.curr= _temp470; _temp471.base= _temp470;
_temp471.last_plus_one= _temp470 + 35; _temp471;})); case 1: _LL469: t= Cyc_Tcexp_tcUnPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*) es->hd); break; case 2: _LL472: t= Cyc_Tcexp_tcBinPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*) es->hd,( struct Cyc_Absyn_Exp*)( es->tl)->hd);
break; default: _LL473: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp475=( char*)"primitive operator has > 2 arguments";
struct _tagged_string _temp476; _temp476.curr= _temp475; _temp476.base= _temp475;
_temp476.last_plus_one= _temp475 + 37; _temp476;}));} goto _LL439; _LL439:;}
return t;}} static void* Cyc_Tcexp_tcIncrement( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, void* i){
Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue( e)){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp477=( char*)"increment/decrement of non-lvalue";
struct _tagged_string _temp478; _temp478.curr= _temp477; _temp478.base= _temp477;
_temp478.last_plus_one= _temp477 + 34; _temp478;}));}{ void* t=( void*)( e->topt)->v;
if( ! Cyc_Tcutil_arithmetic_promote( te, e)? ! Cyc_Tcutil_is_tagged_pointer_typ(
t): 0){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp479= Cyc_Absynpp_typ2string(
t); xprintf("expecting arithmetic or pointer type but found %.*s", _temp479.last_plus_one
- _temp479.curr, _temp479.curr);}));} return t;}} static void* Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest(
te, e1,( struct _tagged_string)({ char* _temp480=( char*)"conditional expression";
struct _tagged_string _temp481; _temp481.curr= _temp480; _temp481.base= _temp480;
_temp481.last_plus_one= _temp480 + 23; _temp481;})); Cyc_Tcexp_tcExp( te, topt,
e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);
struct Cyc_List_List* l1=({ struct Cyc_List_List* _temp482=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp482->hd=( void*) e3; _temp482->tl=
0; _temp482;}); struct Cyc_List_List* l2=({ struct Cyc_List_List* _temp483=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp483->hd=(
void*) e2; _temp483->tl= l1; _temp483;}); if( ! Cyc_Tcutil_coerce_list( te, t,
l2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp484= Cyc_Absynpp_typ2string((
void*)( e2->topt)->v); struct _tagged_string _temp485= Cyc_Absynpp_typ2string((
void*)( e3->topt)->v); xprintf("conditional clause types do not match: %.*s != %.*s",
_temp484.last_plus_one - _temp484.curr, _temp484.curr, _temp485.last_plus_one -
_temp485.curr, _temp485.curr);}));} return t;}} static void* Cyc_Tcexp_tcAssignOp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt* po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e1); Cyc_Tcexp_tcExp( te,( void**)(( void*)&( e1->topt)->v), e2);{ void*
t1=( void*)( e1->topt)->v; void* t2=( void*)( e2->topt)->v;{ void* _temp486= Cyc_Tcutil_compress(
t1); struct Cyc_Absyn_Exp* _temp492; struct Cyc_Absyn_Tqual* _temp494; void*
_temp496; _LL488: if(( unsigned int) _temp486 > 4u?(( struct _tunion_struct*)
_temp486)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL497: _temp496=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp486)->f1; goto _LL495; _LL495: _temp494=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp486)->f2;
goto _LL493; _LL493: _temp492=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp486)->f3; goto _LL489;} else{ goto _LL490;} _LL490: goto _LL491; _LL489:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp498=( char*)"cannot assign to an array";
struct _tagged_string _temp499; _temp499.curr= _temp498; _temp499.base= _temp498;
_temp499.last_plus_one= _temp498 + 26; _temp499;})); goto _LL487; _LL491: goto
_LL487; _LL487:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return Cyc_Tcexp_expr_err( te,
loc,( struct _tagged_string)({ char* _temp500=( char*)"assignment to non-lvalue";
struct _tagged_string _temp501; _temp501.curr= _temp500; _temp501.base= _temp500;
_temp501.last_plus_one= _temp500 + 25; _temp501;}));} if( po == 0){ if( ! Cyc_Tcutil_coerce_assign(
te, e2, t1)){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp502= Cyc_Absynpp_typ2string( t1); struct _tagged_string _temp503= Cyc_Absynpp_typ2string(
t2); xprintf("type mismatch: %.*s != %.*s", _temp502.last_plus_one - _temp502.curr,
_temp502.curr, _temp503.last_plus_one - _temp503.curr, _temp503.curr);}));}}
else{ void* p=( void*) po->v; void* t_result= Cyc_Tcexp_tcBinPrimop( te, loc, 0,
p, e1, e2); if( !( Cyc_Tcutil_unify( t_result, t1)? 1: Cyc_Tcutil_coerceable(
t_result))){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char*
_temp504=( char*)"Cannot use this operator in an assignment"; struct
_tagged_string _temp505; _temp505.curr= _temp504; _temp505.base= _temp504;
_temp505.last_plus_one= _temp504 + 42; _temp505;}));}} return t2;}} static void*
Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp(
te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)( e2->topt)->v;}
static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)( e->topt)->v); void* _temp506=
t; struct Cyc_Absyn_PtrInfo _temp512; struct Cyc_Absyn_Conref* _temp514; struct
Cyc_Absyn_Tqual* _temp516; struct Cyc_Absyn_Conref* _temp518; void* _temp520;
void* _temp522; _LL508: if(( unsigned int) _temp506 > 4u?(( struct
_tunion_struct*) _temp506)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL513:
_temp512=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp506)->f1; _LL523: _temp522=( void*) _temp512.elt_typ; goto _LL521; _LL521:
_temp520=( void*) _temp512.rgn_typ; goto _LL519; _LL519: _temp518=( struct Cyc_Absyn_Conref*)
_temp512.nullable; goto _LL517; _LL517: _temp516=( struct Cyc_Absyn_Tqual*)
_temp512.tq; goto _LL515; _LL515: _temp514=( struct Cyc_Absyn_Conref*) _temp512.bounds;
goto _LL509;} else{ goto _LL510;} _LL510: goto _LL511; _LL509: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp520); Cyc_Tcutil_check_nonzero_bound( loc, _temp514);{ void*
_temp524= Cyc_Tcutil_compress( _temp522); struct Cyc_Absyn_FnInfo _temp530;
struct Cyc_List_List* _temp532; int _temp534; struct Cyc_List_List* _temp536;
void* _temp538; struct Cyc_Core_Opt* _temp540; struct Cyc_List_List* _temp542;
_LL526: if(( unsigned int) _temp524 > 4u?(( struct _tunion_struct*) _temp524)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL531: _temp530=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp524)->f1; _LL543: _temp542=( struct Cyc_List_List*)
_temp530.tvars; goto _LL541; _LL541: _temp540=( struct Cyc_Core_Opt*) _temp530.effect;
goto _LL539; _LL539: _temp538=( void*) _temp530.ret_typ; goto _LL537; _LL537:
_temp536=( struct Cyc_List_List*) _temp530.args; goto _LL535; _LL535: _temp534=(
int) _temp530.varargs; goto _LL533; _LL533: _temp532=( struct Cyc_List_List*)
_temp530.attributes; goto _LL527;} else{ goto _LL528;} _LL528: goto _LL529;
_LL527: if( _temp534){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp544=( char*)"vararg functions (...) not supported yet"; struct
_tagged_string _temp545; _temp545.curr= _temp544; _temp545.base= _temp544;
_temp545.last_plus_one= _temp544 + 41; _temp545;}));} if( topt != 0){ Cyc_Tcutil_unify(
_temp538,* topt);} while( es != 0? _temp536 != 0: 0) { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*) es->hd; void* t2=(*(( struct _tuple2*) _temp536->hd)).f3;
Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, t2)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp546= Cyc_Absynpp_typ2string((
void*)( e1->topt)->v); struct _tagged_string _temp547= Cyc_Absynpp_typ2string(
t2); xprintf("actual argument has type %.*s but formal has type %.*s", _temp546.last_plus_one
- _temp546.curr, _temp546.curr, _temp547.last_plus_one - _temp547.curr, _temp547.curr);}));}
es= es->tl; _temp536= _temp536->tl;} if( es != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp548=( char*)"too many arguments for function";
struct _tagged_string _temp549; _temp549.curr= _temp548; _temp549.base= _temp548;
_temp549.last_plus_one= _temp548 + 32; _temp549;}));} if( _temp536 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp550=( char*)"too few arguments for function";
struct _tagged_string _temp551; _temp551.curr= _temp550; _temp551.base= _temp550;
_temp551.last_plus_one= _temp550 + 31; _temp551;}));} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*) _temp540->v); return _temp538; _LL529: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp552=( char*)"expected pointer to function";
struct _tagged_string _temp553; _temp553.curr= _temp552; _temp553.base= _temp552;
_temp553.last_plus_one= _temp552 + 29; _temp553;})); _LL525:;} _LL511: return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp554=( char*)"expected pointer to function";
struct _tagged_string _temp555; _temp555.curr= _temp554; _temp555.base= _temp554;
_temp555.last_plus_one= _temp554 + 29; _temp555;})); _LL507:;}} static void* Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e); if( !
Cyc_Tcutil_coerce_use( te, e, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp556= Cyc_Absynpp_typ2string(( void*)( e->topt)->v);
xprintf("expected xtunion exn but found %.*s", _temp556.last_plus_one - _temp556.curr,
_temp556.curr);}));} return Cyc_Absyn_wildtyp();} static void* Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst( te, 0, e);(
void*)(( e->topt)->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)( e->topt)->v)));{ void* t1= Cyc_Tcutil_compress(( void*)( e->topt)->v);{
void* _temp557= t1; struct Cyc_Absyn_PtrInfo _temp563; struct Cyc_Absyn_Conref*
_temp565; struct Cyc_Absyn_Tqual* _temp567; struct Cyc_Absyn_Conref* _temp569;
void* _temp571; void* _temp573; _LL559: if(( unsigned int) _temp557 > 4u?((
struct _tunion_struct*) _temp557)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL564:
_temp563=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp557)->f1; _LL574: _temp573=( void*) _temp563.elt_typ; goto _LL572; _LL572:
_temp571=( void*) _temp563.rgn_typ; goto _LL570; _LL570: _temp569=( struct Cyc_Absyn_Conref*)
_temp563.nullable; goto _LL568; _LL568: _temp567=( struct Cyc_Absyn_Tqual*)
_temp563.tq; goto _LL566; _LL566: _temp565=( struct Cyc_Absyn_Conref*) _temp563.bounds;
goto _LL560;} else{ goto _LL561;} _LL561: goto _LL562; _LL560:{ void* _temp575=
Cyc_Tcutil_compress( _temp573); struct Cyc_Absyn_FnInfo _temp581; struct Cyc_List_List*
_temp583; int _temp585; struct Cyc_List_List* _temp587; void* _temp589; struct
Cyc_Core_Opt* _temp591; struct Cyc_List_List* _temp593; _LL577: if((
unsigned int) _temp575 > 4u?(( struct _tunion_struct*) _temp575)->tag == Cyc_Absyn_FnType_tag:
0){ _LL582: _temp581=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp575)->f1; _LL594: _temp593=( struct Cyc_List_List*) _temp581.tvars; goto
_LL592; _LL592: _temp591=( struct Cyc_Core_Opt*) _temp581.effect; goto _LL590;
_LL590: _temp589=( void*) _temp581.ret_typ; goto _LL588; _LL588: _temp587=(
struct Cyc_List_List*) _temp581.args; goto _LL586; _LL586: _temp585=( int)
_temp581.varargs; goto _LL584; _LL584: _temp583=( struct Cyc_List_List*)
_temp581.attributes; goto _LL578;} else{ goto _LL579;} _LL579: goto _LL580;
_LL578: { struct Cyc_List_List* instantiation= 0; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( ts) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp593)){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char*
_temp595=( char*)"type instantiation mismatch"; struct _tagged_string _temp596;
_temp596.curr= _temp595; _temp596.base= _temp595; _temp596.last_plus_one=
_temp595 + 28; _temp596;}));} for( 0; ts != 0; ts= ts->tl, _temp593= _temp593->tl){
void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*) _temp593->hd)->kind); Cyc_Tcutil_check_type( loc, te,
Cyc_Tcenv_lookup_type_vars( te), k,( void*) ts->hd); instantiation=({ struct Cyc_List_List*
_temp597=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp597->hd=( void*)({ struct _tuple4* _temp598=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp598->f1=( struct Cyc_Absyn_Tvar*) _temp593->hd;
_temp598->f2=( void*) ts->hd; _temp598;}); _temp597->tl= instantiation; _temp597;});}{
void* new_fn_typ= Cyc_Tcutil_substitute( instantiation,( void*)({ struct Cyc_Absyn_FnType_struct*
_temp599=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp599[ 0]=({ struct Cyc_Absyn_FnType_struct _temp600; _temp600.tag= Cyc_Absyn_FnType_tag;
_temp600.f1=({ struct Cyc_Absyn_FnInfo _temp601; _temp601.tvars= 0; _temp601.effect=
_temp591; _temp601.ret_typ=( void*) _temp589; _temp601.args= _temp587; _temp601.varargs=
_temp585; _temp601.attributes= _temp583; _temp601;}); _temp600;}); _temp599;}));
void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp602=( struct
Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp602[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp603; _temp603.tag= Cyc_Absyn_PointerType_tag;
_temp603.f1=({ struct Cyc_Absyn_PtrInfo _temp604; _temp604.elt_typ=( void*)
new_fn_typ; _temp604.rgn_typ=( void*) _temp571; _temp604.nullable= _temp569;
_temp604.tq= _temp567; _temp604.bounds= _temp565; _temp604;}); _temp603;});
_temp602;}); return new_typ;}} _LL580: goto _LL576; _LL576:;} goto _LL558;
_LL562: goto _LL558; _LL558:;} return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp605= Cyc_Absynpp_typ2string( t1); xprintf("expecting polymorphic type but found %.*s",
_temp605.last_plus_one - _temp605.curr, _temp605.curr);}));}} static void* Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)(
e->topt)->v; if( ! Cyc_Tcutil_silent_castable( te, loc, t2, t)? ! Cyc_Tcutil_castable(
te, loc, t2, t): 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp606= Cyc_Absynpp_typ2string( t2); struct _tagged_string _temp607= Cyc_Absynpp_typ2string(
t); xprintf("cannot cast %.*s to %.*s", _temp606.last_plus_one - _temp606.curr,
_temp606.curr, _temp607.last_plus_one - _temp607.curr, _temp607.curr);}));}
return t;}} static void* Cyc_Tcexp_tcAddress( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ void** topt2=
0; struct Cyc_Absyn_Tqual* tq2= Cyc_Absyn_empty_tqual(); if( topt != 0){ void*
_temp608= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp614; struct
Cyc_Absyn_Conref* _temp616; struct Cyc_Absyn_Tqual* _temp618; struct Cyc_Absyn_Conref*
_temp620; void* _temp622; void* _temp624; _LL610: if(( unsigned int) _temp608 >
4u?(( struct _tunion_struct*) _temp608)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL615: _temp614=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp608)->f1; _LL625: _temp624=( void*) _temp614.elt_typ; goto _LL623; _LL623:
_temp622=( void*) _temp614.rgn_typ; goto _LL621; _LL621: _temp620=( struct Cyc_Absyn_Conref*)
_temp614.nullable; goto _LL619; _LL619: _temp618=( struct Cyc_Absyn_Tqual*)
_temp614.tq; goto _LL617; _LL617: _temp616=( struct Cyc_Absyn_Conref*) _temp614.bounds;
goto _LL611;} else{ goto _LL612;} _LL612: goto _LL613; _LL611: topt2=({ void**
_temp626=( void**) GC_malloc( sizeof( void*)); _temp626[ 0]= _temp624; _temp626;});
tq2= _temp618; goto _LL609; _LL613: goto _LL609; _LL609:;} Cyc_Tcexp_tcExpNoInst(
te, topt2, e);{ void* _temp627=( void*) e->r; struct Cyc_Absyn_Structdecl*
_temp635; struct Cyc_List_List* _temp637; struct Cyc_Core_Opt* _temp639; struct
_tuple1* _temp641; struct Cyc_List_List* _temp643; _LL629: if((( struct
_tunion_struct*) _temp627)->tag == Cyc_Absyn_Struct_e_tag){ _LL642: _temp641=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp627)->f1; goto _LL640;
_LL640: _temp639=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp627)->f2; goto _LL638; _LL638: _temp637=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp627)->f3; goto _LL636; _LL636: _temp635=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp627)->f4;
goto _LL630;} else{ goto _LL631;} _LL631: if((( struct _tunion_struct*) _temp627)->tag
== Cyc_Absyn_Tuple_e_tag){ _LL644: _temp643=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tuple_e_struct*) _temp627)->f1; goto _LL632;} else{ goto _LL633;}
_LL633: goto _LL634; _LL630: goto _LL632; _LL632: Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ char* _temp645=( char*)"& used to allocate"; struct
_tagged_string _temp646; _temp646.curr= _temp645; _temp646.base= _temp645;
_temp646.last_plus_one= _temp645 + 19; _temp646;}));{ void*(* ptr_maker)( void*
t, void* rgn, struct Cyc_Absyn_Tqual* tq)= Cyc_Absyn_at_typ; if( topt != 0){
void* _temp647= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp653;
struct Cyc_Absyn_Conref* _temp655; struct Cyc_Absyn_Tqual* _temp657; struct Cyc_Absyn_Conref*
_temp659; void* _temp661; void* _temp663; _LL649: if(( unsigned int) _temp647 >
4u?(( struct _tunion_struct*) _temp647)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL654: _temp653=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp647)->f1; _LL664: _temp663=( void*) _temp653.elt_typ; goto _LL662; _LL662:
_temp661=( void*) _temp653.rgn_typ; goto _LL660; _LL660: _temp659=( struct Cyc_Absyn_Conref*)
_temp653.nullable; goto _LL658; _LL658: _temp657=( struct Cyc_Absyn_Tqual*)
_temp653.tq; goto _LL656; _LL656: _temp655=( struct Cyc_Absyn_Conref*) _temp653.bounds;
goto _LL650;} else{ goto _LL651;} _LL651: goto _LL652; _LL650: if((( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp659)){ ptr_maker= Cyc_Absyn_star_typ;}
goto _LL648; _LL652: goto _LL648; _LL648:;} return ptr_maker(( void*)( e->topt)->v,(
void*) Cyc_Absyn_HeapRgn, tq2);} _LL634: goto _LL628; _LL628:;}{ void* _temp667;
int _temp669; struct _tuple5 _temp665= Cyc_Tcutil_addressof_props( te, e);
_LL670: _temp669= _temp665.f1; goto _LL668; _LL668: _temp667= _temp665.f2; goto
_LL666; _LL666: { struct Cyc_Absyn_Tqual* tq= Cyc_Absyn_empty_tqual(); if(
_temp669){ tq->q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)( e->topt)->v,
_temp667, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* t){ Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, t); Cyc_Evexp_szof(
t, loc); return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)( e->topt)->v); void* _temp671=
t; struct Cyc_Absyn_PtrInfo _temp677; struct Cyc_Absyn_Conref* _temp679; struct
Cyc_Absyn_Tqual* _temp681; struct Cyc_Absyn_Conref* _temp683; void* _temp685;
void* _temp687; _LL673: if(( unsigned int) _temp671 > 4u?(( struct
_tunion_struct*) _temp671)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL678:
_temp677=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp671)->f1; _LL688: _temp687=( void*) _temp677.elt_typ; goto _LL686; _LL686:
_temp685=( void*) _temp677.rgn_typ; goto _LL684; _LL684: _temp683=( struct Cyc_Absyn_Conref*)
_temp677.nullable; goto _LL682; _LL682: _temp681=( struct Cyc_Absyn_Tqual*)
_temp677.tq; goto _LL680; _LL680: _temp679=( struct Cyc_Absyn_Conref*) _temp677.bounds;
goto _LL674;} else{ goto _LL675;} _LL675: goto _LL676; _LL674: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp685); Cyc_Tcutil_check_nonzero_bound( loc, _temp679); return
_temp687; _LL676: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp689= Cyc_Absynpp_typ2string( t); xprintf("expecting * or @ type but found %.*s",
_temp689.last_plus_one - _temp689.curr, _temp689.curr);})); _LL672:;}} static
void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_string* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)( e->topt)->v); void* _temp690= t; struct Cyc_Absyn_Structdecl** _temp702;
struct Cyc_Absyn_Structdecl* _temp704; struct Cyc_List_List* _temp705; struct
_tuple1* _temp707; struct Cyc_Absyn_Uniondecl** _temp709; struct Cyc_Absyn_Uniondecl*
_temp711; struct Cyc_List_List* _temp712; struct _tuple1* _temp714; struct Cyc_Absyn_Exp*
_temp717; struct Cyc_Absyn_Tqual* _temp719; void* _temp721; struct Cyc_Absyn_PtrInfo
_temp726; _LL692: if(( unsigned int) _temp690 > 4u?(( struct _tunion_struct*)
_temp690)->tag == Cyc_Absyn_StructType_tag: 0){ _LL708: _temp707=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp690)->f1; goto _LL706;
_LL706: _temp705=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp690)->f2; goto _LL703; _LL703: _temp702=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp690)->f3; if( _temp702 == 0){ goto
_LL694;} else{ _temp704=* _temp702; goto _LL693;}} else{ goto _LL694;} _LL694:
if(( unsigned int) _temp690 > 4u?(( struct _tunion_struct*) _temp690)->tag ==
Cyc_Absyn_UnionType_tag: 0){ _LL715: _temp714=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp690)->f1; goto _LL713; _LL713: _temp712=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp690)->f2; goto _LL710; _LL710: _temp709=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp690)->f3;
if( _temp709 == 0){ goto _LL696;} else{ _temp711=* _temp709; goto _LL695;}}
else{ goto _LL696;} _LL696: if(( unsigned int) _temp690 > 4u?(( struct
_tunion_struct*) _temp690)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL722: _temp721=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp690)->f1; goto _LL720; _LL720:
_temp719=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp690)->f2; goto _LL718; _LL718: _temp717=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp690)->f3; goto _LL716;} else{ goto _LL698;}
_LL716: if( Cyc_String_zstrcmp(* f,( struct _tagged_string)({ char* _temp723=(
char*)"size"; struct _tagged_string _temp724; _temp724.curr= _temp723; _temp724.base=
_temp723; _temp724.last_plus_one= _temp723 + 5; _temp724;})) == 0){ goto _LL697;}
else{ goto _LL698;} _LL698: if(( unsigned int) _temp690 > 4u?(( struct
_tunion_struct*) _temp690)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL727:
_temp726=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp690)->f1; goto _LL725;} else{ goto _LL700;} _LL725: if( Cyc_String_zstrcmp(*
f,( struct _tagged_string)({ char* _temp728=( char*)"size"; struct
_tagged_string _temp729; _temp729.curr= _temp728; _temp729.base= _temp728;
_temp729.last_plus_one= _temp728 + 5; _temp729;})) == 0){ goto _LL699;} else{
goto _LL700;} _LL700: goto _LL701; _LL693: if( _temp707 == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp730=( char*)"unresolved struct type (COMPILER ERROR)";
struct _tagged_string _temp731; _temp731.curr= _temp730; _temp731.base= _temp730;
_temp731.last_plus_one= _temp730 + 40; _temp731;}));}{ struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_struct_field( _temp704, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp733= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp732= _temp707; if( _temp732 == 0){ _throw(
Null_Exception);} _temp732;})); struct _tagged_string _temp734=* f; xprintf("struct %.*s has no %.*s field",
_temp733.last_plus_one - _temp733.curr, _temp733.curr, _temp734.last_plus_one -
_temp734.curr, _temp734.curr);}));}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp704->tvs,
_temp705); void* t2= Cyc_Tcutil_substitute( inst,( void*) finfo->type); return
t2;}} _LL695: if( _temp714 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp735=( char*)"unresolved union type (COMPILER ERROR)";
struct _tagged_string _temp736; _temp736.curr= _temp735; _temp736.base= _temp735;
_temp736.last_plus_one= _temp735 + 39; _temp736;}));}{ struct Cyc_Absyn_Structfield*
finfo= Cyc_Absyn_lookup_union_field( _temp711, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp738= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp737= _temp714; if( _temp737 == 0){ _throw(
Null_Exception);} _temp737;})); struct _tagged_string _temp739=* f; xprintf("union %.*s has no %.*s field",
_temp738.last_plus_one - _temp738.curr, _temp738.curr, _temp739.last_plus_one -
_temp739.curr, _temp739.curr);}));}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp711->tvs,
_temp712); void* t2= Cyc_Tcutil_substitute( inst,( void*) finfo->type); return
t2;}} _LL697:( void*)( outer_e->r=( void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct*
_temp740=( struct Cyc_Absyn_Primop_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct));
_temp740[ 0]=({ struct Cyc_Absyn_Primop_e_struct _temp741; _temp741.tag= Cyc_Absyn_Primop_e_tag;
_temp741.f1=( void*)(( void*) Cyc_Absyn_Size); _temp741.f2=({ struct Cyc_List_List*
_temp742=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp742->hd=( void*) e; _temp742->tl= 0; _temp742;}); _temp741;}); _temp740;})));
return Cyc_Absyn_uint_t; _LL699:( void*)( outer_e->r=( void*)(( void*)({ struct
Cyc_Absyn_Primop_e_struct* _temp743=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp743[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp744; _temp744.tag= Cyc_Absyn_Primop_e_tag;
_temp744.f1=( void*)(( void*) Cyc_Absyn_Size); _temp744.f2=({ struct Cyc_List_List*
_temp745=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp745->hd=( void*) e; _temp745->tl= 0; _temp745;}); _temp744;}); _temp743;})));
return Cyc_Absyn_uint_t; _LL701: if( Cyc_String_zstrcmp(* f,( struct
_tagged_string)({ char* _temp746=( char*)"size"; struct _tagged_string _temp747;
_temp747.curr= _temp746; _temp747.base= _temp746; _temp747.last_plus_one=
_temp746 + 5; _temp747;})) == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp748= Cyc_Absynpp_typ2string( t); xprintf("expecting struct, union or array, found %.*s",
_temp748.last_plus_one - _temp748.curr, _temp748.curr);}));} else{ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp749= Cyc_Absynpp_typ2string( t); xprintf("expecting struct or union, found %.*s",
_temp749.last_plus_one - _temp749.curr, _temp749.curr);}));} _LL691:;}} static
void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct _tagged_string* f){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)( e->topt)->v); void* _temp750=
t; struct Cyc_Absyn_PtrInfo _temp756; struct Cyc_Absyn_Conref* _temp758; struct
Cyc_Absyn_Tqual* _temp760; struct Cyc_Absyn_Conref* _temp762; void* _temp764;
void* _temp766; _LL752: if(( unsigned int) _temp750 > 4u?(( struct
_tunion_struct*) _temp750)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL757:
_temp756=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp750)->f1; _LL767: _temp766=( void*) _temp756.elt_typ; goto _LL765; _LL765:
_temp764=( void*) _temp756.rgn_typ; goto _LL763; _LL763: _temp762=( struct Cyc_Absyn_Conref*)
_temp756.nullable; goto _LL761; _LL761: _temp760=( struct Cyc_Absyn_Tqual*)
_temp756.tq; goto _LL759; _LL759: _temp758=( struct Cyc_Absyn_Conref*) _temp756.bounds;
goto _LL753;} else{ goto _LL754;} _LL754: goto _LL755; _LL753: Cyc_Tcutil_check_nonzero_bound(
loc, _temp758);{ void* _temp768= Cyc_Tcutil_compress( _temp766); struct Cyc_Absyn_Structdecl**
_temp776; struct Cyc_Absyn_Structdecl* _temp778; struct Cyc_List_List* _temp779;
struct _tuple1* _temp781; struct Cyc_Absyn_Uniondecl** _temp783; struct Cyc_Absyn_Uniondecl*
_temp785; struct Cyc_List_List* _temp786; struct _tuple1* _temp788; _LL770: if((
unsigned int) _temp768 > 4u?(( struct _tunion_struct*) _temp768)->tag == Cyc_Absyn_StructType_tag:
0){ _LL782: _temp781=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp768)->f1; goto _LL780; _LL780: _temp779=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp768)->f2; goto _LL777; _LL777: _temp776=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp768)->f3;
if( _temp776 == 0){ goto _LL772;} else{ _temp778=* _temp776; goto _LL771;}}
else{ goto _LL772;} _LL772: if(( unsigned int) _temp768 > 4u?(( struct
_tunion_struct*) _temp768)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL789: _temp788=(
struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp768)->f1; goto
_LL787; _LL787: _temp786=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp768)->f2; goto _LL784; _LL784: _temp783=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp768)->f3; if( _temp783 == 0){ goto
_LL774;} else{ _temp785=* _temp783; goto _LL773;}} else{ goto _LL774;} _LL774:
goto _LL775; _LL771: { struct Cyc_Absyn_Structfield* finfo= Cyc_Absyn_lookup_struct_field(
_temp778, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp791= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct
_tuple1* _temp790= _temp781; if( _temp790 == 0){ _throw( Null_Exception);}
_temp790;})); struct _tagged_string _temp792=* f; xprintf("struct %.*s has no %.*s field",
_temp791.last_plus_one - _temp791.curr, _temp791.curr, _temp792.last_plus_one -
_temp792.curr, _temp792.curr);}));}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp778->tvs,
_temp779); void* t3= Cyc_Tcutil_substitute( inst,( void*) finfo->type); Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp764); return t3;}} _LL773: { struct Cyc_Absyn_Structfield* finfo=
Cyc_Absyn_lookup_union_field( _temp785, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp794= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp793= _temp788; if( _temp793 == 0){ _throw(
Null_Exception);} _temp793;})); struct _tagged_string _temp795=* f; xprintf("union %.*s has no %.*s field",
_temp794.last_plus_one - _temp794.curr, _temp794.curr, _temp795.last_plus_one -
_temp795.curr, _temp795.curr);}));}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp785->tvs,
_temp786); void* t3= Cyc_Tcutil_substitute( inst,( void*) finfo->type); Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp764); return t3;}} _LL775: return Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp796= Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp796.last_plus_one - _temp796.curr, _temp796.curr);})); _LL769:;} _LL755:
return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp797= Cyc_Absynpp_typ2string(
t); xprintf("expecting struct pointer, found %.*s", _temp797.last_plus_one -
_temp797.curr, _temp797.curr);})); _LL751:;}} static void* Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
ts, struct Cyc_Absyn_Exp* index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
index); struct _handler_cons _temp798; _push_handler(& _temp798);{ struct
_xtunion_struct* _temp799=( struct _xtunion_struct*) setjmp( _temp798.handler);
if( ! _temp799){{ void* _temp800=(*(( struct _tuple7*(*)( struct Cyc_List_List*
x, int i)) Cyc_List_nth)( ts,( int) i)).f2; _npop_handler( 0u); return _temp800;};
_pop_handler();} else{ struct _xtunion_struct* _temp802= _temp799; _LL804: if(
_temp802->tag == Cyc_List_Nth_tag){ goto _LL805;} else{ goto _LL806;} _LL806:
goto _LL807; _LL805: return Cyc_Tcexp_expr_err( te, loc, xprintf("index is %d but tuple has only %d fields",(
int) i,(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts))); _LL807:(
void) _throw( _temp802); _LL803:;}}} static void* Cyc_Tcexp_tcSubscript( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te,
0, e2);{ void* t1= Cyc_Tcutil_compress(( void*)( e1->topt)->v); void* t2= Cyc_Tcutil_compress((
void*)( e2->topt)->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return Cyc_Tcexp_expr_err(
te, e2->loc,({ struct _tagged_string _temp808= Cyc_Absynpp_typ2string( t2);
xprintf("expecting int subscript, found %.*s", _temp808.last_plus_one - _temp808.curr,
_temp808.curr);}));}{ void* _temp809= t1; struct Cyc_Absyn_PtrInfo _temp817;
struct Cyc_Absyn_Conref* _temp819; struct Cyc_Absyn_Tqual* _temp821; struct Cyc_Absyn_Conref*
_temp823; void* _temp825; void* _temp827; struct Cyc_List_List* _temp829; _LL811:
if(( unsigned int) _temp809 > 4u?(( struct _tunion_struct*) _temp809)->tag ==
Cyc_Absyn_PointerType_tag: 0){ _LL818: _temp817=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp809)->f1; _LL828: _temp827=( void*)
_temp817.elt_typ; goto _LL826; _LL826: _temp825=( void*) _temp817.rgn_typ; goto
_LL824; _LL824: _temp823=( struct Cyc_Absyn_Conref*) _temp817.nullable; goto
_LL822; _LL822: _temp821=( struct Cyc_Absyn_Tqual*) _temp817.tq; goto _LL820;
_LL820: _temp819=( struct Cyc_Absyn_Conref*) _temp817.bounds; goto _LL812;}
else{ goto _LL813;} _LL813: if(( unsigned int) _temp809 > 4u?(( struct
_tunion_struct*) _temp809)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL830: _temp829=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp809)->f1; goto
_LL814;} else{ goto _LL815;} _LL815: goto _LL816; _LL812: if( Cyc_Tcutil_is_const_exp(
te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp( e2),
_temp819);} else{ Cyc_Tcutil_check_nonzero_bound( loc, _temp819);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp825); return _temp827; _LL814: return Cyc_Tcexp_ithTupleType( te,
loc, _temp829, e2); _LL816: return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp831= Cyc_Absynpp_typ2string( t1); xprintf("subscript applied to %.*s",
_temp831.last_plus_one - _temp831.curr, _temp831.curr);})); _LL810:;}}} static
void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_List_List* es){ int done= 0; struct Cyc_List_List*
fields= 0; if( topt != 0){ void* _temp832= Cyc_Tcutil_compress(* topt); struct
Cyc_List_List* _temp838; _LL834: if(( unsigned int) _temp832 > 4u?(( struct
_tunion_struct*) _temp832)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL839: _temp838=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp832)->f1; goto
_LL835;} else{ goto _LL836;} _LL836: goto _LL837; _LL835: if((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp838) !=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp840=( char*)"tuple expression has the wrong number of fields"; struct
_tagged_string _temp841; _temp841.curr= _temp840; _temp841.base= _temp840;
_temp841.last_plus_one= _temp840 + 48; _temp841;})); goto _LL833;} for( 0; es !=
0; es= es->tl, _temp838= _temp838->tl){ void* topt2=(*(( struct _tuple7*)
_temp838->hd)).f2; Cyc_Tcexp_tcExpInitializer( te,( void**)& topt2,( struct Cyc_Absyn_Exp*)
es->hd); fields=({ struct Cyc_List_List* _temp842=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp842->hd=( void*)({ struct
_tuple7* _temp843=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp843->f1=(*(( struct _tuple7*) _temp838->hd)).f1; _temp843->f2=( void*)(((
struct Cyc_Absyn_Exp*) es->hd)->topt)->v; _temp843;}); _temp842->tl= fields;
_temp842;});} done= 1; goto _LL833; _LL837: goto _LL833; _LL833:;} if( ! done){
for( 0; es != 0; es= es->tl){ Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*)
es->hd); fields=({ struct Cyc_List_List* _temp844=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp844->hd=( void*)({ struct
_tuple7* _temp845=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp845->f1= Cyc_Absyn_empty_tqual(); _temp845->f2=( void*)((( struct Cyc_Absyn_Exp*)
es->hd)->topt)->v; _temp845;}); _temp844->tl= fields; _temp844;});}} return(
void*)({ struct Cyc_Absyn_TupleType_struct* _temp846=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp846[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp847; _temp847.tag= Cyc_Absyn_TupleType_tag;
_temp847.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp847;}); _temp846;});} static void* Cyc_Tcexp_tcCompoundLit( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ char* _temp848=( char*)"tcCompoundLit"; struct
_tagged_string _temp849; _temp849.curr= _temp848; _temp849.base= _temp848;
_temp849.last_plus_one= _temp848 + 14; _temp849;}));} static void* Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** elt_topt,
struct Cyc_List_List* des){ struct Cyc_List_List* es=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple6*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind); struct Cyc_Absyn_Const_e_struct* sz_rexp=({ struct
Cyc_Absyn_Const_e_struct* _temp850=( struct Cyc_Absyn_Const_e_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Const_e_struct)); _temp850[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp851; _temp851.tag= Cyc_Absyn_Const_e_tag; _temp851.f1=( void*)(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp852=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp852[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp853; _temp853.tag= Cyc_Absyn_Int_c_tag; _temp853.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp853.f2=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es); _temp853;});
_temp852;})); _temp851;}); _temp850;}); struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_new_exp((
void*) sz_rexp, loc); sz_exp->topt=({ struct Cyc_Core_Opt* _temp854=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp854->v=( void*) Cyc_Absyn_uint_t;
_temp854;});{ void* res_t2=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp855=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp855[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp856; _temp856.tag= Cyc_Absyn_ArrayType_tag;
_temp856.f1=( void*) res; _temp856.f2= Cyc_Absyn_empty_tqual(); _temp856.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp856;}); _temp855;});{ struct Cyc_List_List*
es2= es; for( 0; es2 != 0; es2= es2->tl){ Cyc_Tcexp_tcExpInitializer( te,
elt_topt,( struct Cyc_Absyn_Exp*) es2->hd);}} if( ! Cyc_Tcutil_coerce_list( te,
res, es)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) es->hd)->loc,({ struct
_tagged_string _temp857= Cyc_Absynpp_typ2string( res); xprintf("elements of array do not all have the same type (%.*s)",
_temp857.last_plus_one - _temp857.curr, _temp857.curr);}));}{ int offset= 0;
for( 0; des != 0; offset ++, des= des->tl){ struct Cyc_List_List* ds=(*(( struct
_tuple6*) des->hd)).f1; if( ds != 0){ void* _temp858=( void*) ds->hd; struct
_tagged_string* _temp864; struct Cyc_Absyn_Exp* _temp866; _LL860: if((( struct
_tunion_struct*) _temp858)->tag == Cyc_Absyn_FieldName_tag){ _LL865: _temp864=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp858)->f1;
goto _LL861;} else{ goto _LL862;} _LL862: if((( struct _tunion_struct*) _temp858)->tag
== Cyc_Absyn_ArrayElement_tag){ _LL867: _temp866=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayElement_struct*) _temp858)->f1; goto _LL863;} else{ goto
_LL859;} _LL861: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp868=(
char*)"only array index designators are supported"; struct _tagged_string
_temp869; _temp869.curr= _temp868; _temp869.base= _temp868; _temp869.last_plus_one=
_temp868 + 43; _temp869;})); goto _LL859; _LL863: Cyc_Tcexp_tcExpInitializer( te,
0, _temp866);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp866); if( i
!=( unsigned int) offset){ Cyc_Tcutil_terr( _temp866->loc, xprintf("expecting index designator %d but found %d",
offset,( int) i));} goto _LL859;} _LL859:;}}} return res_t2;}} static void* Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound, struct Cyc_Absyn_Exp* body){
Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ( te, bound)){
Cyc_Tcutil_terr( bound->loc,({ struct _tagged_string _temp870= Cyc_Absynpp_typ2string((
void*)( bound->topt)->v); xprintf("expecting unsigned int, found %.*s", _temp870.last_plus_one
- _temp870.curr, _temp870.curr);}));} if( te->le != 0){ te= Cyc_Tcenv_new_block(
loc, te); te= Cyc_Tcenv_add_local_var( loc, te, vd);}{ struct Cyc_Absyn_PtrInfo
pinfo; void** topt2= 0; struct Cyc_Absyn_Tqual** tqopt= 0; if( topt != 0){ void*
_temp871= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp879; struct
Cyc_Absyn_Exp* _temp881; struct Cyc_Absyn_Tqual* _temp883; struct Cyc_Absyn_Tqual**
_temp885; void* _temp886; void** _temp888; _LL873: if(( unsigned int) _temp871 >
4u?(( struct _tunion_struct*) _temp871)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL880: _temp879=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp871)->f1; goto _LL874;} else{ goto _LL875;} _LL875: if(( unsigned int)
_temp871 > 4u?(( struct _tunion_struct*) _temp871)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL887: _temp886=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp871)->f1;
_temp888=&(( struct Cyc_Absyn_ArrayType_struct*) _temp871)->f1; goto _LL884;
_LL884: _temp883=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp871)->f2; _temp885=&(( struct Cyc_Absyn_ArrayType_struct*) _temp871)->f2;
goto _LL882; _LL882: _temp881=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp871)->f3; goto _LL876;} else{ goto _LL877;} _LL877: goto _LL878; _LL874:
pinfo= _temp879; topt2=( void**)(( void*)& pinfo.elt_typ); tqopt=( struct Cyc_Absyn_Tqual**)&
pinfo.tq; goto _LL872; _LL876: topt2=( void**) _temp888; tqopt=( struct Cyc_Absyn_Tqual**)
_temp885; goto _LL872; _LL878: goto _LL872; _LL872:;}{ void* t= Cyc_Tcexp_tcExp(
te, topt2, body); return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp889=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp889[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp890; _temp890.tag= Cyc_Absyn_ArrayType_tag;
_temp890.f1=( void*) t; _temp890.f2= tqopt == 0? Cyc_Absyn_empty_tqual():* tqopt;
_temp890.f3=( struct Cyc_Absyn_Exp*) bound; _temp890;}); _temp889;});}}} static
void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct _tuple1** tn, struct Cyc_Core_Opt** otyps, struct Cyc_List_List*
args, struct Cyc_Absyn_Structdecl** sd_opt){ struct Cyc_Absyn_Structdecl* sd;
if(* sd_opt != 0){ sd=( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl*
_temp891=* sd_opt; if( _temp891 == 0){ _throw( Null_Exception);} _temp891;});}
else{{ struct _handler_cons _temp892; _push_handler(& _temp892);{ struct
_xtunion_struct* _temp893=( struct _xtunion_struct*) setjmp( _temp892.handler);
if( ! _temp893){ sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();}
else{ struct _xtunion_struct* _temp895= _temp893; _LL897: if( _temp895->tag ==
Cyc_Dict_Absent_tag){ goto _LL898;} else{ goto _LL899;} _LL899: goto _LL900;
_LL898: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp901= Cyc_Absynpp_qvar2string(*
tn); xprintf("unbound struct name %.*s", _temp901.last_plus_one - _temp901.curr,
_temp901.curr);})); return topt != 0?* topt:( void*) Cyc_Absyn_VoidType; _LL900:(
void) _throw( _temp895); _LL896:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd;
if( sd->name != 0){* tn=( struct _tuple1*)( sd->name)->v;}}{ struct Cyc_List_List*
inst=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, sd->tvs);
struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*))
Cyc_Core_snd, inst); struct Cyc_Absyn_StructType_struct* res_typ=({ struct Cyc_Absyn_StructType_struct*
_temp902=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp902[ 0]=({ struct Cyc_Absyn_StructType_struct _temp903; _temp903.tag= Cyc_Absyn_StructType_tag;
_temp903.f1=( struct _tuple1*)* tn; _temp903.f2= typs; _temp903.f3=({ struct Cyc_Absyn_Structdecl**
_temp904=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp904[ 0]= sd; _temp904;}); _temp903;}); _temp902;}); if( topt != 0){ Cyc_Tcutil_unify((
void*) res_typ,* topt);}* otyps=({ struct Cyc_Core_Opt* _temp905=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp905->v=( void*) typs; _temp905;});{
struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd)) Cyc_Tcutil_resolve_struct_designators)(
loc, args, sd); for( 0; fields != 0; fields= fields->tl){ struct _tuple8
_temp908; struct Cyc_Absyn_Exp* _temp909; struct Cyc_Absyn_Structfield* _temp911;
struct _tuple8* _temp906=( struct _tuple8*) fields->hd; _temp908=* _temp906;
_LL912: _temp911= _temp908.f1; goto _LL910; _LL910: _temp909= _temp908.f2; goto
_LL907; _LL907: { void* inst_fieldtyp= Cyc_Tcutil_substitute( inst,( void*)
_temp911->type); Cyc_Tcexp_tcExpInitializer( te,( void**)& inst_fieldtyp,
_temp909); if( ! Cyc_Tcutil_coerce_arg( te, _temp909, inst_fieldtyp)){ Cyc_Tcutil_terr(
_temp909->loc,({ struct _tagged_string _temp913=* _temp911->name; struct
_tagged_string _temp914= Cyc_Absynpp_qvar2string(* tn); struct _tagged_string
_temp915= Cyc_Absynpp_typ2string( inst_fieldtyp); struct _tagged_string _temp916=
Cyc_Absynpp_typ2string(( void*)( _temp909->topt)->v); xprintf("field %.*s of struct %.*s expects type %.*s != %.*s",
_temp913.last_plus_one - _temp913.curr, _temp913.curr, _temp914.last_plus_one -
_temp914.curr, _temp914.curr, _temp915.last_plus_one - _temp915.curr, _temp915.curr,
_temp916.last_plus_one - _temp916.curr, _temp916.curr);}));}}} return( void*)
res_typ;}}} static void* Cyc_Tcexp_tcTunion( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_Core_Opt**
all_ref, struct Cyc_Core_Opt** exist_ref, struct Cyc_List_List* es, struct Cyc_Absyn_Tuniondecl*
ed, struct Cyc_Absyn_Tunionfield* ef){ struct Cyc_List_List* all_inst=(( struct
Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, ed->tvs); struct Cyc_List_List*
exist_inst=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, ef->tvs);
struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*))
Cyc_Core_snd, all_inst); struct Cyc_List_List* exist_typs=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple4*)) Cyc_Core_snd, exist_inst); struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
all_inst, exist_inst);* all_ref=({ struct Cyc_Core_Opt* _temp917=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp917->v=( void*) all_typs;
_temp917;});* exist_ref=({ struct Cyc_Core_Opt* _temp918=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp918->v=( void*) exist_typs;
_temp918;});{ void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp919=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp919[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp920; _temp920.tag= Cyc_Absyn_TunionFieldType_tag; _temp920.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp921; _temp921.name=( struct _tuple1*)( ed->name)->v; _temp921.targs=
all_typs; _temp921.fname= ef->name; _temp921.tud=( struct Cyc_Absyn_Tuniondecl*)
ed; _temp921.tufd=( struct Cyc_Absyn_Tunionfield*) ef; _temp921;}); _temp920;});
_temp919;}); if( topt != 0){ void* _temp922= Cyc_Tcutil_compress(* topt); struct
Cyc_Absyn_TunionFieldInfo _temp930; struct Cyc_Absyn_TunionInfo _temp932; struct
Cyc_Absyn_Tuniondecl* _temp934; void* _temp936; struct Cyc_List_List* _temp938;
struct _tuple1* _temp940; _LL924: if(( unsigned int) _temp922 > 4u?(( struct
_tunion_struct*) _temp922)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL931:
_temp930=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp922)->f1; goto _LL925;} else{ goto _LL926;} _LL926: if(( unsigned int)
_temp922 > 4u?(( struct _tunion_struct*) _temp922)->tag == Cyc_Absyn_TunionType_tag:
0){ _LL933: _temp932=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp922)->f1; _LL941: _temp940=( struct _tuple1*) _temp932.name; goto _LL939;
_LL939: _temp938=( struct Cyc_List_List*) _temp932.targs; goto _LL937; _LL937:
_temp936=( void*) _temp932.rgn; goto _LL935; _LL935: _temp934=( struct Cyc_Absyn_Tuniondecl*)
_temp932.tud; goto _LL927;} else{ goto _LL928;} _LL928: goto _LL929; _LL925: Cyc_Tcutil_unify(*
topt, res); goto _LL923; _LL927:{ struct Cyc_List_List* a= all_typs; for( 0; a
!= 0? _temp938 != 0: 0; a= a->tl, _temp938= _temp938->tl){ Cyc_Tcutil_unify((
void*) a->hd,( void*) _temp938->hd);}} if( ef->typs == 0? es == 0: 0){ e->topt=({
struct Cyc_Core_Opt* _temp942=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp942->v=( void*) res; _temp942;}); res=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp943=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp943[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp944; _temp944.tag= Cyc_Absyn_TunionType_tag;
_temp944.f1=({ struct Cyc_Absyn_TunionInfo _temp945; _temp945.name=( struct
_tuple1*)( ed->name)->v; _temp945.targs= all_typs; _temp945.rgn=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp945.tud=( struct Cyc_Absyn_Tuniondecl*) ed;
_temp945;}); _temp944;}); _temp943;}); Cyc_Tcutil_unchecked_cast( te, e, res);
return res;} goto _LL923; _LL929: goto _LL923; _LL923:;}{ struct Cyc_List_List*
ts= ef->typs; for( 0; es != 0? ts != 0: 0; es= es->tl, ts= ts->tl){ struct Cyc_Absyn_Exp*
e=( struct Cyc_Absyn_Exp*) es->hd; void* t= Cyc_Tcutil_substitute( inst,(*((
struct _tuple7*) ts->hd)).f2); Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e);
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp948= Cyc_Absynpp_qvar2string( ef->name); struct
_tagged_string _temp949= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp950= e->topt == 0?( struct _tagged_string)({ char* _temp946=( char*)"?";
struct _tagged_string _temp947; _temp947.curr= _temp946; _temp947.base= _temp946;
_temp947.last_plus_one= _temp946 + 2; _temp947;}): Cyc_Absynpp_typ2string(( void*)(
e->topt)->v); xprintf("tunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp948.last_plus_one - _temp948.curr, _temp948.curr, _temp949.last_plus_one -
_temp949.curr, _temp949.curr, _temp950.last_plus_one - _temp950.curr, _temp950.curr);}));}}
if( es != 0){ return Cyc_Tcexp_expr_err( te,(( struct Cyc_Absyn_Exp*) es->hd)->loc,({
struct _tagged_string _temp951= Cyc_Absynpp_qvar2string( ef->name); xprintf("too many arguments for tunion constructor %.*s",
_temp951.last_plus_one - _temp951.curr, _temp951.curr);}));} if( ts != 0){
return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp952= Cyc_Absynpp_qvar2string(
ef->name); xprintf("too few arguments for tunion constructor %.*s", _temp952.last_plus_one
- _temp952.curr, _temp952.curr);}));} return res;}}} static void* Cyc_Tcexp_tcXTunion(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Core_Opt** exist_ref, struct Cyc_List_List* es, struct Cyc_Absyn_XTuniondecl*
xd, struct Cyc_Absyn_Tunionfield* ef){ struct Cyc_List_List* exist_inst=((
struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, ef->tvs); struct Cyc_List_List*
exist_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List*
x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd, exist_inst);
struct Cyc_Absyn_XTunionFieldType_struct* res=({ struct Cyc_Absyn_XTunionFieldType_struct*
_temp953=( struct Cyc_Absyn_XTunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_XTunionFieldType_struct)); _temp953[ 0]=({ struct Cyc_Absyn_XTunionFieldType_struct
_temp954; _temp954.tag= Cyc_Absyn_XTunionFieldType_tag; _temp954.f1=({ struct
Cyc_Absyn_XTunionFieldInfo _temp955; _temp955.name= xd->name; _temp955.fname= ef->name;
_temp955.xtud=( struct Cyc_Absyn_XTuniondecl*) xd; _temp955.xtufd=( struct Cyc_Absyn_Tunionfield*)
ef; _temp955;}); _temp954;}); _temp953;}); struct Cyc_List_List* ts= ef->typs;*
exist_ref=({ struct Cyc_Core_Opt* _temp956=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp956->v=( void*) exist_typs; _temp956;});
while( es != 0? ts != 0: 0) { struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)
es->hd; void* t= Cyc_Tcutil_substitute( exist_inst,(*(( struct _tuple7*) ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); es= es->tl; ts= ts->tl; if( !
Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp957= Cyc_Absynpp_qvar2string( ef->name); struct
_tagged_string _temp958= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp959= Cyc_Absynpp_typ2string(( void*)( e->topt)->v); xprintf("xtunion constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp957.last_plus_one - _temp957.curr, _temp957.curr, _temp958.last_plus_one -
_temp958.curr, _temp958.curr, _temp959.last_plus_one - _temp959.curr, _temp959.curr);}));}}
if( es != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) es->hd)->loc,({ struct
_tagged_string _temp960= Cyc_Absynpp_qvar2string( ef->name); xprintf("too many arguments for xtunion constructor %.*s",
_temp960.last_plus_one - _temp960.curr, _temp960.curr);}));} else{ if( ts != 0){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) es->hd)->loc,({ struct _tagged_string
_temp961= Cyc_Absynpp_qvar2string( ef->name); xprintf("too few arguments for xtunion constructor %.*s",
_temp961.last_plus_one - _temp961.curr, _temp961.curr);}));}} return( void*) res;}
static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, t);{ void* _temp962= t; struct Cyc_Absyn_TunionFieldInfo
_temp970; struct Cyc_Absyn_Tunionfield* _temp972; struct Cyc_Absyn_Tuniondecl*
_temp974; struct _tuple1* _temp976; struct Cyc_List_List* _temp978; struct
_tuple1* _temp980; struct Cyc_Absyn_XTunionFieldInfo _temp982; struct Cyc_Absyn_Tunionfield*
_temp984; struct Cyc_Absyn_XTuniondecl* _temp986; struct _tuple1* _temp988;
struct _tuple1* _temp990; _LL964: if(( unsigned int) _temp962 > 4u?(( struct
_tunion_struct*) _temp962)->tag == Cyc_Absyn_TunionFieldType_tag: 0){ _LL971:
_temp970=( struct Cyc_Absyn_TunionFieldInfo)(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp962)->f1; _LL981: _temp980=( struct _tuple1*) _temp970.name; goto _LL979;
_LL979: _temp978=( struct Cyc_List_List*) _temp970.targs; goto _LL977; _LL977:
_temp976=( struct _tuple1*) _temp970.fname; goto _LL975; _LL975: _temp974=(
struct Cyc_Absyn_Tuniondecl*) _temp970.tud; goto _LL973; _LL973: _temp972=(
struct Cyc_Absyn_Tunionfield*) _temp970.tufd; goto _LL965;} else{ goto _LL966;}
_LL966: if(( unsigned int) _temp962 > 4u?(( struct _tunion_struct*) _temp962)->tag
== Cyc_Absyn_XTunionFieldType_tag: 0){ _LL983: _temp982=( struct Cyc_Absyn_XTunionFieldInfo)((
struct Cyc_Absyn_XTunionFieldType_struct*) _temp962)->f1; _LL991: _temp990=(
struct _tuple1*) _temp982.name; goto _LL989; _LL989: _temp988=( struct _tuple1*)
_temp982.fname; goto _LL987; _LL987: _temp986=( struct Cyc_Absyn_XTuniondecl*)
_temp982.xtud; goto _LL985; _LL985: _temp984=( struct Cyc_Absyn_Tunionfield*)
_temp982.xtufd; goto _LL967;} else{ goto _LL968;} _LL968: goto _LL969; _LL965:
_temp984= _temp972; goto _LL967; _LL967: if( _temp984->tvs != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp992=( char*)"malloc with existential types not yet implemented";
struct _tagged_string _temp993; _temp993.curr= _temp992; _temp993.base= _temp992;
_temp993.last_plus_one= _temp992 + 50; _temp993;}));} goto _LL963; _LL969: goto
_LL963; _LL963:;}{ void*(* ptr_maker)( void* t, void* rgn, struct Cyc_Absyn_Tqual*
tq)= Cyc_Absyn_at_typ; if( topt != 0){ void* _temp994= Cyc_Tcutil_compress(*
topt); struct Cyc_Absyn_PtrInfo _temp1000; struct Cyc_Absyn_Conref* _temp1002;
struct Cyc_Absyn_Tqual* _temp1004; struct Cyc_Absyn_Conref* _temp1006; void*
_temp1008; void* _temp1010; _LL996: if(( unsigned int) _temp994 > 4u?(( struct
_tunion_struct*) _temp994)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1001:
_temp1000=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp994)->f1; _LL1011: _temp1010=( void*) _temp1000.elt_typ; goto _LL1009;
_LL1009: _temp1008=( void*) _temp1000.rgn_typ; goto _LL1007; _LL1007: _temp1006=(
struct Cyc_Absyn_Conref*) _temp1000.nullable; goto _LL1005; _LL1005: _temp1004=(
struct Cyc_Absyn_Tqual*) _temp1000.tq; goto _LL1003; _LL1003: _temp1002=( struct
Cyc_Absyn_Conref*) _temp1000.bounds; goto _LL997;} else{ goto _LL998;} _LL998:
goto _LL999; _LL997: if((( int(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp1006)){ ptr_maker= Cyc_Absyn_star_typ;} goto _LL995; _LL999: goto _LL995;
_LL995:;} return ptr_maker( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());}}
static void* Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp1012=( void*) s->r; struct Cyc_Absyn_Exp* _temp1022;
struct Cyc_Absyn_Stmt* _temp1024; struct Cyc_Absyn_Stmt* _temp1026; struct Cyc_Absyn_Stmt*
_temp1028; struct Cyc_Absyn_Decl* _temp1030; _LL1014: if(( unsigned int)
_temp1012 > 1u?(( struct _tunion_struct*) _temp1012)->tag == Cyc_Absyn_Exp_s_tag:
0){ _LL1023: _temp1022=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp1012)->f1; goto _LL1015;} else{ goto _LL1016;} _LL1016: if(( unsigned int)
_temp1012 > 1u?(( struct _tunion_struct*) _temp1012)->tag == Cyc_Absyn_Seq_s_tag:
0){ _LL1027: _temp1026=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1012)->f1; goto _LL1025; _LL1025: _temp1024=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1012)->f2; goto _LL1017;} else{ goto
_LL1018;} _LL1018: if(( unsigned int) _temp1012 > 1u?(( struct _tunion_struct*)
_temp1012)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1031: _temp1030=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1012)->f1; goto _LL1029; _LL1029:
_temp1028=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1012)->f2;
goto _LL1019;} else{ goto _LL1020;} _LL1020: goto _LL1021; _LL1015: return( void*)(
_temp1022->topt)->v; _LL1017: s= _temp1024; continue; _LL1019: s= _temp1028;
continue; _LL1021: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp1032=( char*)"statement expression must end with expression"; struct
_tagged_string _temp1033; _temp1033.curr= _temp1032; _temp1033.base= _temp1032;
_temp1033.last_plus_one= _temp1032 + 46; _temp1033;})); _LL1013:;}} static void*
Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ char* _temp1034=( char*)"tcCodegen"; struct
_tagged_string _temp1035; _temp1035.curr= _temp1034; _temp1035.base= _temp1034;
_temp1035.last_plus_one= _temp1034 + 10; _temp1035;}));} static void* Cyc_Tcexp_tcFill(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp1036=( char*)"tcFill"; struct _tagged_string _temp1037; _temp1037.curr=
_temp1036; _temp1037.base= _temp1036; _temp1037.last_plus_one= _temp1036 + 7;
_temp1037;}));} static void* Cyc_Tcexp_tcNew( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp*
e1){ void* _temp1038=( void*) e1->r; struct Cyc_Absyn_Exp* _temp1050; struct Cyc_Absyn_Exp*
_temp1052; struct Cyc_Absyn_Vardecl* _temp1054; struct Cyc_List_List* _temp1056;
struct Cyc_Core_Opt* _temp1058; struct Cyc_List_List* _temp1060; void* _temp1062;
struct _tagged_string _temp1064; _LL1040: if((( struct _tunion_struct*)
_temp1038)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL1055: _temp1054=( struct
Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1038)->f1;
goto _LL1053; _LL1053: _temp1052=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1038)->f2; goto _LL1051; _LL1051: _temp1050=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1038)->f3; goto _LL1041;} else{
goto _LL1042;} _LL1042: if((( struct _tunion_struct*) _temp1038)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL1059: _temp1058=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1038)->f1; goto _LL1057; _LL1057: _temp1056=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1038)->f2; goto _LL1043;} else{
goto _LL1044;} _LL1044: if((( struct _tunion_struct*) _temp1038)->tag == Cyc_Absyn_Array_e_tag){
_LL1061: _temp1060=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1038)->f1; goto _LL1045;} else{ goto _LL1046;} _LL1046: if((( struct
_tunion_struct*) _temp1038)->tag == Cyc_Absyn_Const_e_tag){ _LL1063: _temp1062=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1038)->f1; if(( unsigned int)
_temp1062 > 1u?(( struct _tunion_struct*) _temp1062)->tag == Cyc_Absyn_String_c_tag:
0){ _LL1065: _temp1064=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1062)->f1; goto _LL1047;} else{ goto _LL1048;}} else{ goto _LL1048;}
_LL1048: goto _LL1049; _LL1041: { void* t_arr= Cyc_Tcexp_tcExpNoPromote( te,
topt, e1); void* _temp1066= Cyc_Tcutil_compress( t_arr); struct Cyc_Absyn_Exp*
_temp1072; struct Cyc_Absyn_Tqual* _temp1074; void* _temp1076; _LL1068: if((
unsigned int) _temp1066 > 4u?(( struct _tunion_struct*) _temp1066)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1077: _temp1076=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1066)->f1;
goto _LL1075; _LL1075: _temp1074=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1066)->f2; goto _LL1073; _LL1073: _temp1072=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1066)->f3; goto _LL1069;} else{ goto
_LL1070;} _LL1070: goto _LL1071; _LL1069: { void* b= Cyc_Tcutil_is_const_exp( te,
_temp1052)?( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1078=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1078[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1079; _temp1079.tag= Cyc_Absyn_Upper_b_tag;
_temp1079.f1= _temp1052; _temp1079;}); _temp1078;}):( void*) Cyc_Absyn_Unknown_b;
void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1080=( struct
Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1080[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1081; _temp1081.tag=
Cyc_Absyn_PointerType_tag; _temp1081.f1=({ struct Cyc_Absyn_PtrInfo _temp1082;
_temp1082.elt_typ=( void*) _temp1076; _temp1082.rgn_typ=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp1082.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp1082.tq= _temp1074; _temp1082.bounds=(( struct Cyc_Absyn_Conref*(*)( void*
x)) Cyc_Absyn_new_conref)( b); _temp1082;}); _temp1081;}); _temp1080;}); if(
topt != 0){ if( ! Cyc_Tcutil_unify(* topt, res_typ)? Cyc_Tcutil_silent_castable(
te, loc, res_typ,* topt): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1083=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1083->v=( void*)
res_typ; _temp1083;}); Cyc_Tcutil_unchecked_cast( te, e,* topt); res_typ=* topt;}}
return res_typ;} _LL1071: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp1084=( char*)"tcNew: comprehension returned non-array type";
struct _tagged_string _temp1085; _temp1085.curr= _temp1084; _temp1085.base=
_temp1084; _temp1085.last_plus_one= _temp1084 + 45; _temp1085;})); _LL1067:;}
_LL1043:( void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp1086=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp1086[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp1087; _temp1087.tag= Cyc_Absyn_Array_e_tag;
_temp1087.f1= _temp1056; _temp1087;}); _temp1086;}))); _temp1060= _temp1056;
goto _LL1045; _LL1045: { void** elt_typ_opt= 0; if( topt != 0){ void* _temp1088=
Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp1094; struct Cyc_Absyn_Conref*
_temp1096; struct Cyc_Absyn_Tqual* _temp1098; struct Cyc_Absyn_Conref* _temp1100;
void* _temp1102; void* _temp1104; void** _temp1106; _LL1090: if(( unsigned int)
_temp1088 > 4u?(( struct _tunion_struct*) _temp1088)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL1095: _temp1094=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1088)->f1; _LL1105: _temp1104=( void*) _temp1094.elt_typ; _temp1106=&(((
struct Cyc_Absyn_PointerType_struct*) _temp1088)->f1).elt_typ; goto _LL1103;
_LL1103: _temp1102=( void*) _temp1094.rgn_typ; goto _LL1101; _LL1101: _temp1100=(
struct Cyc_Absyn_Conref*) _temp1094.nullable; goto _LL1099; _LL1099: _temp1098=(
struct Cyc_Absyn_Tqual*) _temp1094.tq; goto _LL1097; _LL1097: _temp1096=( struct
Cyc_Absyn_Conref*) _temp1094.bounds; goto _LL1091;} else{ goto _LL1092;} _LL1092:
goto _LL1093; _LL1091: elt_typ_opt=( void**) _temp1106; goto _LL1089; _LL1093:
goto _LL1089; _LL1089:;}{ void* t_array= Cyc_Tcexp_tcExpNoPromote( te,
elt_typ_opt, e1); void* res_typ;{ void* _temp1107= Cyc_Tcutil_compress( t_array);
struct Cyc_Absyn_Exp* _temp1113; struct Cyc_Absyn_Tqual* _temp1115; void*
_temp1117; _LL1109: if(( unsigned int) _temp1107 > 4u?(( struct _tunion_struct*)
_temp1107)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1118: _temp1117=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1107)->f1; goto _LL1116; _LL1116:
_temp1115=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1107)->f2; goto _LL1114; _LL1114: _temp1113=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1107)->f3; goto _LL1110;} else{ goto
_LL1111;} _LL1111: goto _LL1112; _LL1110: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1119=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1119[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1120; _temp1120.tag=
Cyc_Absyn_PointerType_tag; _temp1120.f1=({ struct Cyc_Absyn_PtrInfo _temp1121;
_temp1121.elt_typ=( void*) _temp1117; _temp1121.rgn_typ=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp1121.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp1121.tq= _temp1115; _temp1121.bounds=(( struct Cyc_Absyn_Conref*(*)( void*
x)) Cyc_Absyn_new_conref)(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1122=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1122[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1123; _temp1123.tag= Cyc_Absyn_Upper_b_tag;
_temp1123.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1124=
_temp1113; if( _temp1124 == 0){ _throw( Null_Exception);} _temp1124;});
_temp1123;}); _temp1122;})); _temp1121;}); _temp1120;}); _temp1119;}); if( topt
!= 0){ if( ! Cyc_Tcutil_unify(* topt, res_typ)? Cyc_Tcutil_silent_castable( te,
loc, res_typ,* topt): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1125=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1125->v=( void*) res_typ;
_temp1125;}); Cyc_Tcutil_unchecked_cast( te, e,* topt); res_typ=* topt;}} goto
_LL1108; _LL1112: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp1126=( char*)"tcExpNoPromote on Array_e returned non-array type";
struct _tagged_string _temp1127; _temp1127.curr= _temp1126; _temp1127.base=
_temp1126; _temp1127.last_plus_one= _temp1126 + 50; _temp1127;})); _LL1108:;}
return res_typ;}} _LL1047: { void* topt2= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual(),( void*) Cyc_Absyn_Unknown_b);
void* t= Cyc_Tcexp_tcExp( te,( void**)& topt2, e1); return Cyc_Absyn_atb_typ( t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1128=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1128[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1129; _temp1129.tag= Cyc_Absyn_Upper_b_tag;
_temp1129.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1129;}); _temp1128;}));} _LL1049: {
void** topt2= 0; if( topt != 0){ void* _temp1130= Cyc_Tcutil_compress(* topt);
struct Cyc_Absyn_PtrInfo _temp1138; struct Cyc_Absyn_Conref* _temp1140; struct
Cyc_Absyn_Tqual* _temp1142; struct Cyc_Absyn_Conref* _temp1144; void* _temp1146;
void* _temp1148; void** _temp1150; struct Cyc_Absyn_TunionInfo _temp1151;
_LL1132: if(( unsigned int) _temp1130 > 4u?(( struct _tunion_struct*) _temp1130)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL1139: _temp1138=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp1130)->f1; _LL1149: _temp1148=( void*)
_temp1138.elt_typ; _temp1150=&((( struct Cyc_Absyn_PointerType_struct*)
_temp1130)->f1).elt_typ; goto _LL1147; _LL1147: _temp1146=( void*) _temp1138.rgn_typ;
goto _LL1145; _LL1145: _temp1144=( struct Cyc_Absyn_Conref*) _temp1138.nullable;
goto _LL1143; _LL1143: _temp1142=( struct Cyc_Absyn_Tqual*) _temp1138.tq; goto
_LL1141; _LL1141: _temp1140=( struct Cyc_Absyn_Conref*) _temp1138.bounds; goto
_LL1133;} else{ goto _LL1134;} _LL1134: if(( unsigned int) _temp1130 > 4u?((
struct _tunion_struct*) _temp1130)->tag == Cyc_Absyn_TunionType_tag: 0){ _LL1152:
_temp1151=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp1130)->f1; goto _LL1135;} else{ goto _LL1136;} _LL1136: goto _LL1137;
_LL1133: topt2=( void**) _temp1150; goto _LL1131; _LL1135: topt2=({ void**
_temp1153=( void**) GC_malloc( sizeof( void*)); _temp1153[ 0]=* topt; _temp1153;});
goto _LL1131; _LL1137: goto _LL1131; _LL1131:;}{ void* telt= Cyc_Tcexp_tcExp( te,
topt2, e1); void* res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1154=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1154[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1155; _temp1155.tag=
Cyc_Absyn_PointerType_tag; _temp1155.f1=({ struct Cyc_Absyn_PtrInfo _temp1156;
_temp1156.elt_typ=( void*) telt; _temp1156.rgn_typ=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp1156.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp1156.tq= Cyc_Absyn_empty_tqual(); _temp1156.bounds=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1157=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1157[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1158; _temp1158.tag= Cyc_Absyn_Upper_b_tag;
_temp1158.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1158;}); _temp1157;})); _temp1156;});
_temp1155;}); _temp1154;}); if( topt != 0){ if( ! Cyc_Tcutil_unify(* topt,
res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,* topt): 0){ e->topt=({
struct Cyc_Core_Opt* _temp1159=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1159->v=( void*) res_typ; _temp1159;}); Cyc_Tcutil_unchecked_cast(
te, e,* topt); res_typ=* topt;}} return res_typ;}} _LL1039:;} void* Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote(
te, topt, e); t= Cyc_Tcutil_compress( t);{ void* _temp1160= t; struct Cyc_Absyn_Exp*
_temp1166; struct Cyc_Absyn_Tqual* _temp1168; void* _temp1170; _LL1162: if((
unsigned int) _temp1160 > 4u?(( struct _tunion_struct*) _temp1160)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1171: _temp1170=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1160)->f1;
goto _LL1169; _LL1169: _temp1168=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1160)->f2; goto _LL1167; _LL1167: _temp1166=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1160)->f3; goto _LL1163;} else{ goto
_LL1164;} _LL1164: goto _LL1165; _LL1163: { void* _temp1174; int _temp1176;
struct _tuple5 _temp1172= Cyc_Tcutil_addressof_props( te, e); _LL1177: _temp1176=
_temp1172.f1; goto _LL1175; _LL1175: _temp1174= _temp1172.f2; goto _LL1173;
_LL1173: { void* b= _temp1166 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp1178=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1178[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1179; _temp1179.tag= Cyc_Absyn_Upper_b_tag;
_temp1179.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1180=
_temp1166; if( _temp1180 == 0){ _throw( Null_Exception);} _temp1180;});
_temp1179;}); _temp1178;}); t= Cyc_Absyn_atb_typ( _temp1170, _temp1174,
_temp1168, b); Cyc_Tcutil_unchecked_cast( te, e, t); return t;}} _LL1165: return
t; _LL1161:;}} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te, topt, e);
void* _temp1181=( void*) e->r; struct Cyc_List_List* _temp1191; struct Cyc_Absyn_Exp*
_temp1193; struct Cyc_Absyn_Exp* _temp1195; struct Cyc_Absyn_Vardecl* _temp1197;
void* _temp1199; struct _tagged_string _temp1201; _LL1183: if((( struct
_tunion_struct*) _temp1181)->tag == Cyc_Absyn_Array_e_tag){ _LL1192: _temp1191=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1181)->f1; goto
_LL1184;} else{ goto _LL1185;} _LL1185: if((( struct _tunion_struct*) _temp1181)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1198: _temp1197=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1181)->f1; goto _LL1196; _LL1196:
_temp1195=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1181)->f2; goto _LL1194; _LL1194: _temp1193=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1181)->f3; goto _LL1186;} else{
goto _LL1187;} _LL1187: if((( struct _tunion_struct*) _temp1181)->tag == Cyc_Absyn_Const_e_tag){
_LL1200: _temp1199=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1181)->f1;
if(( unsigned int) _temp1199 > 1u?(( struct _tunion_struct*) _temp1199)->tag ==
Cyc_Absyn_String_c_tag: 0){ _LL1202: _temp1201=( struct _tagged_string)(( struct
Cyc_Absyn_String_c_struct*) _temp1199)->f1; goto _LL1188;} else{ goto _LL1189;}}
else{ goto _LL1189;} _LL1189: goto _LL1190; _LL1184: return t; _LL1186: return t;
_LL1188: return t; _LL1190: t= Cyc_Tcutil_compress( t);{ void* _temp1203= t;
struct Cyc_Absyn_Exp* _temp1209; struct Cyc_Absyn_Tqual* _temp1211; void*
_temp1213; _LL1205: if(( unsigned int) _temp1203 > 4u?(( struct _tunion_struct*)
_temp1203)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1214: _temp1213=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1203)->f1; goto _LL1212; _LL1212:
_temp1211=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1203)->f2; goto _LL1210; _LL1210: _temp1209=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1203)->f3; goto _LL1206;} else{ goto
_LL1207;} _LL1207: goto _LL1208; _LL1206: { void* _temp1217; int _temp1219;
struct _tuple5 _temp1215= Cyc_Tcutil_addressof_props( te, e); _LL1220: _temp1219=
_temp1215.f1; goto _LL1218; _LL1218: _temp1217= _temp1215.f2; goto _LL1216;
_LL1216: { void* b= _temp1209 == 0?( void*) Cyc_Absyn_Unknown_b:( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp1221=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1221[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1222; _temp1222.tag= Cyc_Absyn_Upper_b_tag;
_temp1222.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1223=
_temp1209; if( _temp1223 == 0){ _throw( Null_Exception);} _temp1223;});
_temp1222;}); _temp1221;}); t= Cyc_Absyn_atb_typ( _temp1213, _temp1217,
_temp1211, b); Cyc_Tcutil_unchecked_cast( te, e, t); return t;}} _LL1208: return
t; _LL1204:;} _LL1182:;} static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){{ void* _temp1224=( void*) e->r;
struct Cyc_Absyn_Exp* _temp1230; _LL1226: if((( struct _tunion_struct*)
_temp1224)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1231: _temp1230=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1224)->f1; goto
_LL1227;} else{ goto _LL1228;} _LL1228: goto _LL1229; _LL1227: Cyc_Tcexp_tcExpNoInst(
te, topt, _temp1230); e->topt= _temp1230->topt; goto _LL1225; _LL1229: Cyc_Tcexp_tcExpNoInst(
te, topt, e);( void*)(( e->topt)->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)( e->topt)->v)));{ void* _temp1232= Cyc_Tcutil_compress(( void*)( e->topt)->v);
struct Cyc_Absyn_PtrInfo _temp1238; struct Cyc_Absyn_Conref* _temp1240; struct
Cyc_Absyn_Tqual* _temp1242; struct Cyc_Absyn_Conref* _temp1244; void* _temp1246;
void* _temp1248; _LL1234: if(( unsigned int) _temp1232 > 4u?(( struct
_tunion_struct*) _temp1232)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1239:
_temp1238=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1232)->f1; _LL1249: _temp1248=( void*) _temp1238.elt_typ; goto _LL1247;
_LL1247: _temp1246=( void*) _temp1238.rgn_typ; goto _LL1245; _LL1245: _temp1244=(
struct Cyc_Absyn_Conref*) _temp1238.nullable; goto _LL1243; _LL1243: _temp1242=(
struct Cyc_Absyn_Tqual*) _temp1238.tq; goto _LL1241; _LL1241: _temp1240=( struct
Cyc_Absyn_Conref*) _temp1238.bounds; goto _LL1235;} else{ goto _LL1236;} _LL1236:
goto _LL1237; _LL1235:{ void* _temp1250= Cyc_Tcutil_compress( _temp1248); struct
Cyc_Absyn_FnInfo _temp1256; struct Cyc_List_List* _temp1258; int _temp1260;
struct Cyc_List_List* _temp1262; void* _temp1264; struct Cyc_Core_Opt* _temp1266;
struct Cyc_List_List* _temp1268; _LL1252: if(( unsigned int) _temp1250 > 4u?((
struct _tunion_struct*) _temp1250)->tag == Cyc_Absyn_FnType_tag: 0){ _LL1257:
_temp1256=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp1250)->f1; _LL1269: _temp1268=( struct Cyc_List_List*) _temp1256.tvars;
goto _LL1267; _LL1267: _temp1266=( struct Cyc_Core_Opt*) _temp1256.effect; goto
_LL1265; _LL1265: _temp1264=( void*) _temp1256.ret_typ; goto _LL1263; _LL1263:
_temp1262=( struct Cyc_List_List*) _temp1256.args; goto _LL1261; _LL1261:
_temp1260=( int) _temp1256.varargs; goto _LL1259; _LL1259: _temp1258=( struct
Cyc_List_List*) _temp1256.attributes; goto _LL1253;} else{ goto _LL1254;}
_LL1254: goto _LL1255; _LL1253: if( _temp1268 != 0){ struct Cyc_List_List* inst=((
struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp1268); struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple4*)) Cyc_Core_snd,
inst); void* ftyp= Cyc_Tcutil_substitute( inst,( void*)({ struct Cyc_Absyn_FnType_struct*
_temp1270=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1270[ 0]=({ struct Cyc_Absyn_FnType_struct _temp1271; _temp1271.tag= Cyc_Absyn_FnType_tag;
_temp1271.f1=({ struct Cyc_Absyn_FnInfo _temp1272; _temp1272.tvars= 0; _temp1272.effect=
_temp1266; _temp1272.ret_typ=( void*) _temp1264; _temp1272.args= _temp1262;
_temp1272.varargs= _temp1260; _temp1272.attributes= _temp1258; _temp1272;});
_temp1271;}); _temp1270;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp1273=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1273[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1274; _temp1274.tag=
Cyc_Absyn_PointerType_tag; _temp1274.f1=({ struct Cyc_Absyn_PtrInfo _temp1275;
_temp1275.elt_typ=( void*) ftyp; _temp1275.rgn_typ=( void*) _temp1246; _temp1275.nullable=
_temp1244; _temp1275.tq= _temp1242; _temp1275.bounds= _temp1240; _temp1275;});
_temp1274;}); _temp1273;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct*
_temp1276=( struct Cyc_Absyn_Instantiate_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Instantiate_e_struct)); _temp1276[ 0]=({ struct Cyc_Absyn_Instantiate_e_struct
_temp1277; _temp1277.tag= Cyc_Absyn_Instantiate_e_tag; _temp1277.f1= inner;
_temp1277.f2= ts; _temp1277;}); _temp1276;}))); e->topt=({ struct Cyc_Core_Opt*
_temp1278=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1278->v=( void*) new_typ; _temp1278;});} goto _LL1251; _LL1255: goto
_LL1251; _LL1251:;} goto _LL1233; _LL1237: goto _LL1233; _LL1233:;} goto _LL1225;
_LL1225:;} return( void*)( e->topt)->v;} static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment*
loc= e->loc; void* t;{ void* _temp1279=( void*) e->r; struct Cyc_Absyn_Exp*
_temp1351; struct _tuple1* _temp1353; struct Cyc_List_List* _temp1355; struct
Cyc_Absyn_Exp* _temp1357; struct Cyc_List_List* _temp1359; struct Cyc_Core_Opt*
_temp1361; void* _temp1363; void* _temp1365; struct _tuple1* _temp1367; struct
Cyc_List_List* _temp1369; void* _temp1371; void* _temp1373; struct Cyc_Absyn_Exp*
_temp1375; struct Cyc_Absyn_Exp* _temp1377; struct Cyc_Core_Opt* _temp1379;
struct Cyc_Absyn_Exp* _temp1381; struct Cyc_Absyn_Exp* _temp1383; struct Cyc_Absyn_Exp*
_temp1385; struct Cyc_Absyn_Exp* _temp1387; struct Cyc_Absyn_Exp* _temp1389;
struct Cyc_Absyn_Exp* _temp1391; struct Cyc_List_List* _temp1393; struct Cyc_Absyn_Exp*
_temp1395; struct Cyc_Absyn_Exp* _temp1397; struct Cyc_List_List* _temp1399;
struct Cyc_Absyn_Exp* _temp1401; struct Cyc_Absyn_Exp* _temp1403; void*
_temp1405; struct Cyc_Absyn_Exp* _temp1407; struct Cyc_Absyn_Exp* _temp1409;
struct Cyc_Absyn_Exp* _temp1411; void* _temp1413; struct Cyc_Absyn_Exp*
_temp1415; struct _tagged_string* _temp1417; struct Cyc_Absyn_Exp* _temp1419;
struct _tagged_string* _temp1421; struct Cyc_Absyn_Exp* _temp1423; struct Cyc_Absyn_Exp*
_temp1425; struct Cyc_Absyn_Exp* _temp1427; struct Cyc_List_List* _temp1429;
struct Cyc_List_List* _temp1431; struct _tuple2* _temp1433; struct Cyc_List_List*
_temp1435; struct Cyc_Absyn_Stmt* _temp1437; struct Cyc_Absyn_Fndecl* _temp1439;
struct Cyc_Absyn_Exp* _temp1441; struct Cyc_Absyn_Exp* _temp1443; struct Cyc_Absyn_Exp*
_temp1445; struct Cyc_Absyn_Vardecl* _temp1447; struct Cyc_Absyn_Structdecl*
_temp1449; struct Cyc_Absyn_Structdecl** _temp1451; struct Cyc_List_List*
_temp1452; struct Cyc_Core_Opt* _temp1454; struct Cyc_Core_Opt** _temp1456;
struct _tuple1* _temp1457; struct _tuple1** _temp1459; struct Cyc_Absyn_Tunionfield*
_temp1460; struct Cyc_Absyn_Tuniondecl* _temp1462; struct Cyc_List_List*
_temp1464; struct Cyc_Core_Opt* _temp1466; struct Cyc_Core_Opt** _temp1468;
struct Cyc_Core_Opt* _temp1469; struct Cyc_Core_Opt** _temp1471; struct Cyc_Absyn_Tunionfield*
_temp1472; struct Cyc_Absyn_XTuniondecl* _temp1474; struct Cyc_List_List*
_temp1476; struct Cyc_Core_Opt* _temp1478; struct Cyc_Core_Opt** _temp1480;
struct Cyc_Absyn_Enumfield* _temp1481; struct Cyc_Absyn_Enumdecl* _temp1483;
struct _tuple1* _temp1485; void* _temp1487; _LL1281: if((( struct _tunion_struct*)
_temp1279)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1352: _temp1351=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1279)->f1; goto
_LL1282;} else{ goto _LL1283;} _LL1283: if((( struct _tunion_struct*) _temp1279)->tag
== Cyc_Absyn_UnknownId_e_tag){ _LL1354: _temp1353=( struct _tuple1*)(( struct
Cyc_Absyn_UnknownId_e_struct*) _temp1279)->f1; goto _LL1284;} else{ goto _LL1285;}
_LL1285: if((( struct _tunion_struct*) _temp1279)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL1358: _temp1357=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1279)->f1; goto _LL1356; _LL1356: _temp1355=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1279)->f2; goto _LL1286;} else{
goto _LL1287;} _LL1287: if((( struct _tunion_struct*) _temp1279)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL1362: _temp1361=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1279)->f1; goto _LL1360; _LL1360: _temp1359=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1279)->f2; goto _LL1288;} else{
goto _LL1289;} _LL1289: if((( struct _tunion_struct*) _temp1279)->tag == Cyc_Absyn_Const_e_tag){
_LL1364: _temp1363=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1279)->f1;
goto _LL1290;} else{ goto _LL1291;} _LL1291: if((( struct _tunion_struct*)
_temp1279)->tag == Cyc_Absyn_Var_e_tag){ _LL1368: _temp1367=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp1279)->f1; goto _LL1366; _LL1366: _temp1365=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1279)->f2; goto _LL1292;} else{
goto _LL1293;} _LL1293: if((( struct _tunion_struct*) _temp1279)->tag == Cyc_Absyn_Primop_e_tag){
_LL1372: _temp1371=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1279)->f1;
goto _LL1370; _LL1370: _temp1369=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1279)->f2; goto _LL1294;} else{ goto _LL1295;} _LL1295: if((( struct
_tunion_struct*) _temp1279)->tag == Cyc_Absyn_Increment_e_tag){ _LL1376:
_temp1375=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1279)->f1; goto _LL1374; _LL1374: _temp1373=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1279)->f2; goto _LL1296;} else{ goto _LL1297;} _LL1297: if((( struct
_tunion_struct*) _temp1279)->tag == Cyc_Absyn_AssignOp_e_tag){ _LL1382:
_temp1381=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1279)->f1; goto _LL1380; _LL1380: _temp1379=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1279)->f2; goto _LL1378; _LL1378:
_temp1377=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1279)->f3; goto _LL1298;} else{ goto _LL1299;} _LL1299: if((( struct
_tunion_struct*) _temp1279)->tag == Cyc_Absyn_Conditional_e_tag){ _LL1388:
_temp1387=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1279)->f1; goto _LL1386; _LL1386: _temp1385=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1279)->f2; goto _LL1384; _LL1384:
_temp1383=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1279)->f3; goto _LL1300;} else{ goto _LL1301;} _LL1301: if((( struct
_tunion_struct*) _temp1279)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL1392: _temp1391=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp1279)->f1; goto
_LL1390; _LL1390: _temp1389=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1279)->f2; goto _LL1302;} else{ goto _LL1303;} _LL1303: if((( struct
_tunion_struct*) _temp1279)->tag == Cyc_Absyn_FnCall_e_tag){ _LL1396: _temp1395=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp1279)->f1; goto
_LL1394; _LL1394: _temp1393=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1279)->f2; goto _LL1304;} else{ goto _LL1305;} _LL1305: if((( struct
_tunion_struct*) _temp1279)->tag == Cyc_Absyn_Throw_e_tag){ _LL1398: _temp1397=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp1279)->f1; goto
_LL1306;} else{ goto _LL1307;} _LL1307: if((( struct _tunion_struct*) _temp1279)->tag
== Cyc_Absyn_Instantiate_e_tag){ _LL1402: _temp1401=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1279)->f1; goto _LL1400; _LL1400:
_temp1399=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1279)->f2; goto _LL1308;} else{ goto _LL1309;} _LL1309: if((( struct
_tunion_struct*) _temp1279)->tag == Cyc_Absyn_Cast_e_tag){ _LL1406: _temp1405=(
void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1279)->f1; goto _LL1404; _LL1404:
_temp1403=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1279)->f2;
goto _LL1310;} else{ goto _LL1311;} _LL1311: if((( struct _tunion_struct*)
_temp1279)->tag == Cyc_Absyn_Address_e_tag){ _LL1408: _temp1407=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp1279)->f1; goto _LL1312;} else{ goto
_LL1313;} _LL1313: if((( struct _tunion_struct*) _temp1279)->tag == Cyc_Absyn_New_e_tag){
_LL1410: _temp1409=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_New_e_struct*)
_temp1279)->f1; goto _LL1314;} else{ goto _LL1315;} _LL1315: if((( struct
_tunion_struct*) _temp1279)->tag == Cyc_Absyn_Sizeofexp_e_tag){ _LL1412:
_temp1411=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1279)->f1; goto _LL1316;} else{ goto _LL1317;} _LL1317: if((( struct
_tunion_struct*) _temp1279)->tag == Cyc_Absyn_Sizeoftyp_e_tag){ _LL1414:
_temp1413=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1279)->f1; goto
_LL1318;} else{ goto _LL1319;} _LL1319: if((( struct _tunion_struct*) _temp1279)->tag
== Cyc_Absyn_Deref_e_tag){ _LL1416: _temp1415=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp1279)->f1; goto _LL1320;} else{ goto _LL1321;}
_LL1321: if((( struct _tunion_struct*) _temp1279)->tag == Cyc_Absyn_StructMember_e_tag){
_LL1420: _temp1419=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1279)->f1; goto _LL1418; _LL1418: _temp1417=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1279)->f2; goto _LL1322;} else{
goto _LL1323;} _LL1323: if((( struct _tunion_struct*) _temp1279)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL1424: _temp1423=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1279)->f1; goto _LL1422; _LL1422: _temp1421=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1279)->f2; goto _LL1324;} else{
goto _LL1325;} _LL1325: if((( struct _tunion_struct*) _temp1279)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1428: _temp1427=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1279)->f1; goto _LL1426; _LL1426: _temp1425=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1279)->f2; goto _LL1326;} else{ goto
_LL1327;} _LL1327: if((( struct _tunion_struct*) _temp1279)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1430: _temp1429=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1279)->f1; goto _LL1328;} else{ goto _LL1329;} _LL1329: if((( struct
_tunion_struct*) _temp1279)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1434:
_temp1433=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1279)->f1;
goto _LL1432; _LL1432: _temp1431=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1279)->f2; goto _LL1330;} else{ goto _LL1331;} _LL1331: if((( struct
_tunion_struct*) _temp1279)->tag == Cyc_Absyn_Array_e_tag){ _LL1436: _temp1435=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1279)->f1; goto
_LL1332;} else{ goto _LL1333;} _LL1333: if((( struct _tunion_struct*) _temp1279)->tag
== Cyc_Absyn_StmtExp_e_tag){ _LL1438: _temp1437=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp1279)->f1; goto _LL1334;} else{ goto
_LL1335;} _LL1335: if((( struct _tunion_struct*) _temp1279)->tag == Cyc_Absyn_Codegen_e_tag){
_LL1440: _temp1439=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1279)->f1; goto _LL1336;} else{ goto _LL1337;} _LL1337: if((( struct
_tunion_struct*) _temp1279)->tag == Cyc_Absyn_Fill_e_tag){ _LL1442: _temp1441=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp1279)->f1; goto
_LL1338;} else{ goto _LL1339;} _LL1339: if((( struct _tunion_struct*) _temp1279)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1448: _temp1447=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1279)->f1; goto _LL1446; _LL1446:
_temp1445=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1279)->f2; goto _LL1444; _LL1444: _temp1443=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1279)->f3; goto _LL1340;} else{
goto _LL1341;} _LL1341: if((( struct _tunion_struct*) _temp1279)->tag == Cyc_Absyn_Struct_e_tag){
_LL1458: _temp1457=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1279)->f1; _temp1459=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1279)->f1;
goto _LL1455; _LL1455: _temp1454=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1279)->f2; _temp1456=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1279)->f2;
goto _LL1453; _LL1453: _temp1452=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1279)->f3; goto _LL1450; _LL1450: _temp1449=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1279)->f4; _temp1451=&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1279)->f4; goto _LL1342;} else{ goto _LL1343;} _LL1343: if((( struct
_tunion_struct*) _temp1279)->tag == Cyc_Absyn_Tunion_e_tag){ _LL1470: _temp1469=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*) _temp1279)->f1;
_temp1471=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1279)->f1; goto _LL1467;
_LL1467: _temp1466=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1279)->f2; _temp1468=&(( struct Cyc_Absyn_Tunion_e_struct*) _temp1279)->f2;
goto _LL1465; _LL1465: _temp1464=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1279)->f3; goto _LL1463; _LL1463: _temp1462=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_e_struct*) _temp1279)->f4; goto _LL1461; _LL1461:
_temp1460=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1279)->f5; goto _LL1344;} else{ goto _LL1345;} _LL1345: if((( struct
_tunion_struct*) _temp1279)->tag == Cyc_Absyn_XTunion_e_tag){ _LL1479: _temp1478=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_XTunion_e_struct*) _temp1279)->f1;
_temp1480=&(( struct Cyc_Absyn_XTunion_e_struct*) _temp1279)->f1; goto _LL1477;
_LL1477: _temp1476=( struct Cyc_List_List*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1279)->f2; goto _LL1475; _LL1475: _temp1474=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_e_struct*) _temp1279)->f3; goto _LL1473; _LL1473:
_temp1472=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_Absyn_XTunion_e_struct*)
_temp1279)->f4; goto _LL1346;} else{ goto _LL1347;} _LL1347: if((( struct
_tunion_struct*) _temp1279)->tag == Cyc_Absyn_Enum_e_tag){ _LL1486: _temp1485=(
struct _tuple1*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1279)->f1; goto _LL1484;
_LL1484: _temp1483=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1279)->f2; goto _LL1482; _LL1482: _temp1481=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1279)->f3; goto _LL1348;} else{ goto
_LL1349;} _LL1349: if((( struct _tunion_struct*) _temp1279)->tag == Cyc_Absyn_Malloc_e_tag){
_LL1488: _temp1487=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp1279)->f1;
goto _LL1350;} else{ goto _LL1280;} _LL1282: Cyc_Tcexp_tcExpNoInst( te, 0,
_temp1351); return; _LL1284: Cyc_Tcexp_resolve_unknown_id( te, e, _temp1353);
Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1286: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1357, _temp1355); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1288: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1359); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1290: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1363, e);
goto _LL1280; _LL1292: t= Cyc_Tcexp_tcVar( te, loc, _temp1367, _temp1365); goto
_LL1280; _LL1294: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1371, _temp1369);
goto _LL1280; _LL1296: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1375,
_temp1373); goto _LL1280; _LL1298: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1381, _temp1379, _temp1377); goto _LL1280; _LL1300: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1387, _temp1385, _temp1383); goto _LL1280; _LL1302: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1391, _temp1389); goto _LL1280; _LL1304: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1395, _temp1393); goto _LL1280; _LL1306: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1397); goto _LL1280; _LL1308: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1401, _temp1399); goto _LL1280; _LL1310: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1405, _temp1403); goto _LL1280; _LL1312: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1407); goto _LL1280; _LL1314: t= Cyc_Tcexp_tcNew( te, loc,
topt, e, _temp1409); goto _LL1280; _LL1316: { void* t1= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1411); t= Cyc_Tcexp_tcSizeof( te, loc, topt, t1); goto _LL1280;}
_LL1318: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1413); goto _LL1280; _LL1320:
t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1415); goto _LL1280; _LL1322: t= Cyc_Tcexp_tcStructMember(
te, loc, topt, e, _temp1419, _temp1417); goto _LL1280; _LL1324: t= Cyc_Tcexp_tcStructArrow(
te, loc, topt, _temp1423, _temp1421); goto _LL1280; _LL1326: t= Cyc_Tcexp_tcSubscript(
te, loc, topt, _temp1427, _temp1425); goto _LL1280; _LL1328: t= Cyc_Tcexp_tcTuple(
te, loc, topt, _temp1429); goto _LL1280; _LL1330: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1433, _temp1431); goto _LL1280; _LL1332: { void** elt_topt= 0;
if( topt != 0){ void* _temp1489= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_Exp*
_temp1495; struct Cyc_Absyn_Tqual* _temp1497; void* _temp1499; void** _temp1501;
_LL1491: if(( unsigned int) _temp1489 > 4u?(( struct _tunion_struct*) _temp1489)->tag
== Cyc_Absyn_ArrayType_tag: 0){ _LL1500: _temp1499=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1489)->f1; _temp1501=&(( struct Cyc_Absyn_ArrayType_struct*) _temp1489)->f1;
goto _LL1498; _LL1498: _temp1497=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1489)->f2; goto _LL1496; _LL1496: _temp1495=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1489)->f3; goto _LL1492;} else{ goto
_LL1493;} _LL1493: goto _LL1494; _LL1492: elt_topt=( void**) _temp1501; goto
_LL1490; _LL1494: goto _LL1490; _LL1490:;} t= Cyc_Tcexp_tcArray( te, loc,
elt_topt, _temp1435); goto _LL1280;} _LL1334: t= Cyc_Tcexp_tcStmtExp( te, loc,
topt, _temp1437); goto _LL1280; _LL1336: t= Cyc_Tcexp_tcCodegen( te, loc, topt,
_temp1439); goto _LL1280; _LL1338: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp1441);
goto _LL1280; _LL1340: t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp1447,
_temp1445, _temp1443); goto _LL1280; _LL1342: t= Cyc_Tcexp_tcStruct( te, loc,
topt, _temp1459, _temp1456, _temp1452, _temp1451); goto _LL1280; _LL1344: t= Cyc_Tcexp_tcTunion(
te, loc, topt, e, _temp1471, _temp1468, _temp1464, _temp1462, _temp1460); goto
_LL1280; _LL1346: t= Cyc_Tcexp_tcXTunion( te, loc, topt, _temp1480, _temp1476,
_temp1474, _temp1472); goto _LL1280; _LL1348: t=( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp1502=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp1502[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp1503; _temp1503.tag= Cyc_Absyn_EnumType_tag;
_temp1503.f1= _temp1483->name; _temp1503.f2= _temp1483; _temp1503;}); _temp1502;});
goto _LL1280; _LL1350: t= Cyc_Tcexp_tcMalloc( te, loc, topt, _temp1487); goto
_LL1280; _LL1280:;} e->topt=({ struct Cyc_Core_Opt* _temp1504=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1504->v=( void*) t; _temp1504;});}