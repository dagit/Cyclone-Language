#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2;}; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3;}; struct _tuple2{ struct
Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2;}; struct _tuple3{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual* f2; void* f3;}; struct _tuple4{
struct Cyc_Absyn_Tqual* f1; void* f2;}; struct _tuple5{ void* f1; void* f2;};
struct _tuple6{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2;}; typedef
unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct _tagged_string
Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef struct
_tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit( int);
extern void* abort(); struct Cyc_Core_Opt{ void* v;}; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1;}; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1;}; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1;}; extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{
char* tag;}; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1;}; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;}; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[ 11u];
struct Cyc_List_List_empty_struct{ char* tag;}; extern struct Cyc_List_List* Cyc_List_map(
void*(* f)( void*), struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag;}; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag;}; extern int Cyc_List_list_cmp( int(* cmp)(
void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2); struct Cyc_Lineno_Pos{
struct _tagged_string logical_file; struct _tagged_string line; int line_no; int
col;}; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
5u]; struct Cyc_Position_Exit_struct{ char* tag;}; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; typedef void* Cyc_Position_Error_kind;
extern void* Cyc_Position_Lex; extern void* Cyc_Position_Parse; extern void* Cyc_Position_Elab;
typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc;}; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag;}; typedef struct _tagged_string* Cyc_Absyn_field_name; typedef struct
_tagged_string* Cyc_Absyn_var; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
typedef void* Cyc_Absyn_Nmspace; typedef struct _tuple0* Cyc_Absyn_qvar; typedef
struct _tuple0* Cyc_Absyn_qvar_opt_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t; struct Cyc_Absyn_Tvar;
typedef void* Cyc_Absyn_Scope; struct Cyc_Absyn_Tqual; typedef void* Cyc_Absyn_Size_of;
typedef void* Cyc_Absyn_Kind; typedef void* Cyc_Absyn_Array_kind; typedef void*
Cyc_Absyn_Sign; struct Cyc_Absyn_Conref; typedef void* Cyc_Absyn_Constraint;
typedef void* Cyc_Absyn_Bounds; struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_FnInfo;
typedef void* Cyc_Absyn_Typ; typedef void* Cyc_Absyn_Funcparams; typedef void*
Cyc_Absyn_Type_modifier; typedef void* Cyc_Absyn_Cnst; typedef void* Cyc_Absyn_Primop;
typedef void* Cyc_Absyn_Incrementor; typedef void* Cyc_Absyn_Raw_exp; struct Cyc_Absyn_Exp;
typedef void* Cyc_Absyn_Raw_stmt; struct Cyc_Absyn_Stmt; typedef void* Cyc_Absyn_Raw_pat;
struct Cyc_Absyn_Pat; typedef void* Cyc_Absyn_Binding; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Xenumdecl; struct Cyc_Absyn_Typedefdecl;
struct Cyc_Absyn_Vardecl; typedef void* Cyc_Absyn_Raw_decl; struct Cyc_Absyn_Decl;
typedef void* Cyc_Absyn_Designator; typedef struct _xenum_struct* Cyc_Absyn_StmtAnnot;
typedef void* Cyc_Absyn_scope; typedef struct Cyc_Absyn_Tqual* Cyc_Absyn_tqual;
typedef void* Cyc_Absyn_size_of_t; typedef void* Cyc_Absyn_kind_t; typedef
struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar; typedef void* Cyc_Absyn_sign_t; typedef
struct Cyc_Absyn_Conref* Cyc_Absyn_conref; typedef void* Cyc_Absyn_constraint_t;
typedef void* Cyc_Absyn_bounds_t; typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t;
typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t; typedef void* Cyc_Absyn_typ;
typedef void* Cyc_Absyn_funcparams_t; typedef void* Cyc_Absyn_type_modifier;
typedef void* Cyc_Absyn_cnst_t; typedef void* Cyc_Absyn_primop; typedef void*
Cyc_Absyn_incrementor_t; typedef void* Cyc_Absyn_raw_exp_t; typedef struct Cyc_Absyn_Exp*
Cyc_Absyn_exp; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt; typedef void*
Cyc_Absyn_raw_stmt_t; typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt; typedef
struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt_opt; typedef void* Cyc_Absyn_raw_pat_t;
typedef struct Cyc_Absyn_Pat* Cyc_Absyn_pat; typedef void* Cyc_Absyn_binding_t;
typedef struct Cyc_Absyn_Switch_clause* Cyc_Absyn_switch_clause; typedef struct
Cyc_Absyn_Fndecl* Cyc_Absyn_fndecl; typedef struct Cyc_Absyn_Structdecl* Cyc_Absyn_structdecl;
typedef struct Cyc_Absyn_Enumfield* Cyc_Absyn_enumfield; typedef struct Cyc_Absyn_Enumdecl*
Cyc_Absyn_enumdecl; typedef struct Cyc_Absyn_Xenumdecl* Cyc_Absyn_xenumdecl;
typedef struct Cyc_Absyn_Typedefdecl* Cyc_Absyn_typedefdecl; typedef struct Cyc_Absyn_Vardecl*
Cyc_Absyn_vardecl; typedef void* Cyc_Absyn_raw_decl; typedef struct Cyc_Absyn_Decl*
Cyc_Absyn_decl; typedef void* Cyc_Absyn_designator; typedef struct _xenum_struct*
Cyc_Absyn_stmt_annot_t; void* Cyc_Absyn_Loc_n=( void*) 0; const int Cyc_Absyn_Rel_n_tag=
0; struct Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1;}; const int
Cyc_Absyn_Abs_n_tag= 1; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1;}; void* Cyc_Absyn_Static=( void*) 0; void* Cyc_Absyn_Abstract=( void*) 1;
void* Cyc_Absyn_Public=( void*) 2; void* Cyc_Absyn_Extern=( void*) 3; void* Cyc_Absyn_ExternC=(
void*) 4; struct Cyc_Absyn_Tqual{ int q_const; int q_volatile; int q_restrict;};
void* Cyc_Absyn_B1=( void*) 0; void* Cyc_Absyn_B2=( void*) 1; void* Cyc_Absyn_B4=(
void*) 2; void* Cyc_Absyn_B8=( void*) 3; void* Cyc_Absyn_AnyKind=( void*) 0;
void* Cyc_Absyn_MemKind=( void*) 1; void* Cyc_Absyn_BoxKind=( void*) 2; void*
Cyc_Absyn_RgnKind=( void*) 3; void* Cyc_Absyn_EffKind=( void*) 4; void* Cyc_Absyn_Signed=(
void*) 0; void* Cyc_Absyn_Unsigned=( void*) 1; struct Cyc_Absyn_Conref{ void* v;};
const int Cyc_Absyn_Eq_constr_tag= 0; struct Cyc_Absyn_Eq_constr_struct{ int tag;
void* f1;}; const int Cyc_Absyn_Forward_constr_tag= 1; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1;}; void* Cyc_Absyn_No_constr=( void*) 0;
struct Cyc_Absyn_Tvar{ struct _tagged_string* name; struct Cyc_Absyn_Conref*
kind;}; void* Cyc_Absyn_Unknown_b=( void*) 0; const int Cyc_Absyn_Upper_b_tag= 0;
struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1;}; struct Cyc_Absyn_PtrInfo{
void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual*
tq; struct Cyc_Absyn_Conref* bounds;}; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int varargs;}; void* Cyc_Absyn_VoidType=( void*) 0; const int Cyc_Absyn_Evar_tag=
0; struct Cyc_Absyn_Evar_struct{ int tag; void* f1; struct Cyc_Core_Opt* f2; int
f3;}; const int Cyc_Absyn_VarType_tag= 1; struct Cyc_Absyn_VarType_struct{ int
tag; struct Cyc_Absyn_Tvar* f1;}; const int Cyc_Absyn_EnumType_tag= 2; struct
Cyc_Absyn_EnumType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List* f2;
struct Cyc_Absyn_Enumdecl** f3;}; const int Cyc_Absyn_XenumType_tag= 3; struct
Cyc_Absyn_XenumType_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Xenumdecl**
f2;}; const int Cyc_Absyn_PointerType_tag= 4; struct Cyc_Absyn_PointerType_struct{
int tag; struct Cyc_Absyn_PtrInfo f1;}; const int Cyc_Absyn_IntType_tag= 5;
struct Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2;}; void* Cyc_Absyn_FloatType=(
void*) 1; void* Cyc_Absyn_DoubleType=( void*) 2; const int Cyc_Absyn_ArrayType_tag=
6; struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual*
f2; struct Cyc_Absyn_Exp* f3;}; const int Cyc_Absyn_FnType_tag= 7; struct Cyc_Absyn_FnType_struct{
int tag; struct Cyc_Absyn_FnInfo f1;}; const int Cyc_Absyn_TupleType_tag= 8;
struct Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1;}; const
int Cyc_Absyn_StructType_tag= 9; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;};
void* Cyc_Absyn_UnionType=( void*) 3; const int Cyc_Absyn_RgnHandleType_tag= 10;
struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1;}; const int Cyc_Absyn_TypedefType_tag=
11; struct Cyc_Absyn_TypedefType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Core_Opt* f3;}; void* Cyc_Absyn_HeapRgn=( void*) 4; const int Cyc_Absyn_AccessEff_tag=
12; struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1;}; const int Cyc_Absyn_JoinEff_tag=
13; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1;}; const
int Cyc_Absyn_NoTypes_tag= 0; struct Cyc_Absyn_NoTypes_struct{ int tag; struct
Cyc_List_List* f1; struct Cyc_Position_Segment* f2;}; const int Cyc_Absyn_WithTypes_tag=
1; struct Cyc_Absyn_WithTypes_struct{ int tag; struct Cyc_List_List* f1; int f2;
struct Cyc_Core_Opt* f3;}; typedef void* Cyc_Absyn_Pointer_Sort; const int Cyc_Absyn_NonNullable_ps_tag=
0; struct Cyc_Absyn_NonNullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1;};
const int Cyc_Absyn_Nullable_ps_tag= 1; struct Cyc_Absyn_Nullable_ps_struct{ int
tag; struct Cyc_Absyn_Exp* f1;}; void* Cyc_Absyn_TaggedArray_ps=( void*) 0; void*
Cyc_Absyn_Carray_mod=( void*) 0; const int Cyc_Absyn_ConstArray_mod_tag= 0;
struct Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1;};
const int Cyc_Absyn_Pointer_mod_tag= 1; struct Cyc_Absyn_Pointer_mod_struct{ int
tag; void* f1; void* f2; struct Cyc_Absyn_Tqual* f3;}; const int Cyc_Absyn_Function_mod_tag=
2; struct Cyc_Absyn_Function_mod_struct{ int tag; void* f1;}; const int Cyc_Absyn_TypeParams_mod_tag=
3; struct Cyc_Absyn_TypeParams_mod_struct{ int tag; struct Cyc_List_List* f1;
struct Cyc_Position_Segment* f2; int f3;}; const int Cyc_Absyn_Char_c_tag= 0;
struct Cyc_Absyn_Char_c_struct{ int tag; void* f1; char f2;}; const int Cyc_Absyn_Short_c_tag=
1; struct Cyc_Absyn_Short_c_struct{ int tag; void* f1; short f2;}; const int Cyc_Absyn_Int_c_tag=
2; struct Cyc_Absyn_Int_c_struct{ int tag; void* f1; int f2;}; const int Cyc_Absyn_LongLong_c_tag=
3; struct Cyc_Absyn_LongLong_c_struct{ int tag; void* f1; long long f2;}; const
int Cyc_Absyn_Float_c_tag= 4; struct Cyc_Absyn_Float_c_struct{ int tag; struct
_tagged_string f1;}; const int Cyc_Absyn_String_c_tag= 5; struct Cyc_Absyn_String_c_struct{
int tag; int f1; struct _tagged_string f2;}; void* Cyc_Absyn_Null_c=( void*) 0;
void* Cyc_Absyn_Plus=( void*) 0; void* Cyc_Absyn_Times=( void*) 1; void* Cyc_Absyn_Minus=(
void*) 2; void* Cyc_Absyn_Div=( void*) 3; void* Cyc_Absyn_Mod=( void*) 4; void*
Cyc_Absyn_Eq=( void*) 5; void* Cyc_Absyn_Neq=( void*) 6; void* Cyc_Absyn_Gt=(
void*) 7; void* Cyc_Absyn_Lt=( void*) 8; void* Cyc_Absyn_Gte=( void*) 9; void*
Cyc_Absyn_Lte=( void*) 10; void* Cyc_Absyn_Not=( void*) 11; void* Cyc_Absyn_Bitnot=(
void*) 12; void* Cyc_Absyn_Bitand=( void*) 13; void* Cyc_Absyn_Bitor=( void*) 14;
void* Cyc_Absyn_Bitxor=( void*) 15; void* Cyc_Absyn_Bitlshift=( void*) 16; void*
Cyc_Absyn_Bitlrshift=( void*) 17; void* Cyc_Absyn_Bitarshift=( void*) 18; void*
Cyc_Absyn_Size=( void*) 19; void* Cyc_Absyn_Printf=( void*) 20; void* Cyc_Absyn_Fprintf=(
void*) 21; void* Cyc_Absyn_Xprintf=( void*) 22; void* Cyc_Absyn_Scanf=( void*)
23; void* Cyc_Absyn_Fscanf=( void*) 24; void* Cyc_Absyn_Sscanf=( void*) 25; void*
Cyc_Absyn_PreInc=( void*) 0; void* Cyc_Absyn_PostInc=( void*) 1; void* Cyc_Absyn_PreDec=(
void*) 2; void* Cyc_Absyn_PostDec=( void*) 3; const int Cyc_Absyn_Const_e_tag= 0;
struct Cyc_Absyn_Const_e_struct{ int tag; void* f1;}; const int Cyc_Absyn_Var_e_tag=
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple0* f1; void* f2;}; const
int Cyc_Absyn_UnknownId_e_tag= 2; struct Cyc_Absyn_UnknownId_e_struct{ int tag;
struct _tuple0* f1;}; const int Cyc_Absyn_Primop_e_tag= 3; struct Cyc_Absyn_Primop_e_struct{
int tag; void* f1; struct Cyc_List_List* f2;}; const int Cyc_Absyn_AssignOp_e_tag=
4; struct Cyc_Absyn_AssignOp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp* f3;}; const int Cyc_Absyn_Increment_e_tag=
5; struct Cyc_Absyn_Increment_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; void*
f2;}; const int Cyc_Absyn_Conditional_e_tag= 6; struct Cyc_Absyn_Conditional_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3;}; const int Cyc_Absyn_SeqExp_e_tag= 7; struct Cyc_Absyn_SeqExp_e_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2;}; const int Cyc_Absyn_UnknownCall_e_tag=
8; struct Cyc_Absyn_UnknownCall_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_List_List* f2;}; const int Cyc_Absyn_FnCall_e_tag= 9; struct Cyc_Absyn_FnCall_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2;}; const int Cyc_Absyn_Throw_e_tag=
10; struct Cyc_Absyn_Throw_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;}; const
int Cyc_Absyn_NoInstantiate_e_tag= 11; struct Cyc_Absyn_NoInstantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1;}; const int Cyc_Absyn_Instantiate_e_tag= 12;
struct Cyc_Absyn_Instantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_List_List* f2;}; const int Cyc_Absyn_Cast_e_tag= 13; struct Cyc_Absyn_Cast_e_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2;}; const int Cyc_Absyn_Address_e_tag=
14; struct Cyc_Absyn_Address_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;};
const int Cyc_Absyn_Sizeof_e_tag= 15; struct Cyc_Absyn_Sizeof_e_struct{ int tag;
void* f1;}; const int Cyc_Absyn_Deref_e_tag= 16; struct Cyc_Absyn_Deref_e_struct{
int tag; struct Cyc_Absyn_Exp* f1;}; const int Cyc_Absyn_StructMember_e_tag= 17;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_string* f2;}; const int Cyc_Absyn_StructArrow_e_tag= 18; struct
Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_string* f2;}; const int Cyc_Absyn_Subscript_e_tag= 19; struct Cyc_Absyn_Subscript_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2;}; const int Cyc_Absyn_Tuple_e_tag=
20; struct Cyc_Absyn_Tuple_e_struct{ int tag; struct Cyc_List_List* f1;}; const
int Cyc_Absyn_CompoundLit_e_tag= 21; struct Cyc_Absyn_CompoundLit_e_struct{ int
tag; struct _tuple1* f1; struct Cyc_List_List* f2;}; const int Cyc_Absyn_Array_e_tag=
22; struct Cyc_Absyn_Array_e_struct{ int tag; int f1; struct Cyc_List_List* f2;};
const int Cyc_Absyn_Comprehension_e_tag= 23; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3;}; const int Cyc_Absyn_Struct_e_tag= 24; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4;}; const int Cyc_Absyn_Enum_e_tag= 25; struct
Cyc_Absyn_Enum_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Enumdecl* f4; struct Cyc_Absyn_Enumfield*
f5;}; const int Cyc_Absyn_Xenum_e_tag= 26; struct Cyc_Absyn_Xenum_e_struct{ int
tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Xenumdecl*
f3; struct Cyc_Absyn_Enumfield* f4;}; const int Cyc_Absyn_UnresolvedMem_e_tag=
27; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2;}; const int Cyc_Absyn_StmtExp_e_tag= 28; struct Cyc_Absyn_StmtExp_e_struct{
int tag; struct Cyc_Absyn_Stmt* f1;}; const int Cyc_Absyn_Codegen_e_tag= 29;
struct Cyc_Absyn_Codegen_e_struct{ int tag; struct Cyc_Absyn_Fndecl* f1;}; const
int Cyc_Absyn_Fill_e_tag= 30; struct Cyc_Absyn_Fill_e_struct{ int tag; struct
Cyc_Absyn_Exp* f1;}; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt* topt; void* r;
struct Cyc_Position_Segment* loc;}; void* Cyc_Absyn_Skip_s=( void*) 0; const int
Cyc_Absyn_Exp_s_tag= 0; struct Cyc_Absyn_Exp_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1;}; const int Cyc_Absyn_Seq_s_tag= 1; struct Cyc_Absyn_Seq_s_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2;}; const int Cyc_Absyn_Return_s_tag=
2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp* f1;}; const
int Cyc_Absyn_IfThenElse_s_tag= 3; struct Cyc_Absyn_IfThenElse_s_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt* f3;};
const int Cyc_Absyn_While_s_tag= 4; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2;}; const int Cyc_Absyn_Break_s_tag=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1;}; const
int Cyc_Absyn_Continue_s_tag= 6; struct Cyc_Absyn_Continue_s_struct{ int tag;
struct Cyc_Absyn_Stmt* f1;}; const int Cyc_Absyn_Goto_s_tag= 7; struct Cyc_Absyn_Goto_s_struct{
int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2;}; const int Cyc_Absyn_For_s_tag=
8; struct Cyc_Absyn_For_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt* f4;}; const int Cyc_Absyn_Switch_s_tag=
9; struct Cyc_Absyn_Switch_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_List_List* f2;}; const int Cyc_Absyn_Fallthru_s_tag= 10; struct Cyc_Absyn_Fallthru_s_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Absyn_Switch_clause** f2;}; const
int Cyc_Absyn_Decl_s_tag= 11; struct Cyc_Absyn_Decl_s_struct{ int tag; struct
Cyc_Absyn_Decl* f1; struct Cyc_Absyn_Stmt* f2;}; const int Cyc_Absyn_Cut_s_tag=
12; struct Cyc_Absyn_Cut_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1;}; const
int Cyc_Absyn_Splice_s_tag= 13; struct Cyc_Absyn_Splice_s_struct{ int tag;
struct Cyc_Absyn_Stmt* f1;}; const int Cyc_Absyn_Label_s_tag= 14; struct Cyc_Absyn_Label_s_struct{
int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2;}; const int Cyc_Absyn_Do_s_tag=
15; struct Cyc_Absyn_Do_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct
_tuple2 f2;}; const int Cyc_Absyn_TryCatch_s_tag= 16; struct Cyc_Absyn_TryCatch_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_List_List* f2;}; struct Cyc_Absyn_Stmt{
void* r; struct Cyc_Position_Segment* loc; struct Cyc_List_List* non_local_preds;
int try_depth; struct _xenum_struct* annot;}; void* Cyc_Absyn_Wild_p=( void*) 0;
const int Cyc_Absyn_Var_p_tag= 0; struct Cyc_Absyn_Var_p_struct{ int tag; struct
Cyc_Absyn_Vardecl* f1;}; void* Cyc_Absyn_Null_p=( void*) 1; const int Cyc_Absyn_Int_p_tag=
1; struct Cyc_Absyn_Int_p_struct{ int tag; void* f1; int f2;}; const int Cyc_Absyn_Char_p_tag=
2; struct Cyc_Absyn_Char_p_struct{ int tag; char f1;}; const int Cyc_Absyn_Float_p_tag=
3; struct Cyc_Absyn_Float_p_struct{ int tag; struct _tagged_string f1;}; const
int Cyc_Absyn_Tuple_p_tag= 4; struct Cyc_Absyn_Tuple_p_struct{ int tag; struct
Cyc_List_List* f1;}; const int Cyc_Absyn_Pointer_p_tag= 5; struct Cyc_Absyn_Pointer_p_struct{
int tag; struct Cyc_Absyn_Pat* f1;}; const int Cyc_Absyn_Reference_p_tag= 6;
struct Cyc_Absyn_Reference_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;};
const int Cyc_Absyn_Struct_p_tag= 7; struct Cyc_Absyn_Struct_p_struct{ int tag;
struct Cyc_Absyn_Structdecl* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_List_List* f4;}; const int Cyc_Absyn_Enum_p_tag= 8; struct Cyc_Absyn_Enum_p_struct{
int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_List_List* f4; struct Cyc_Absyn_Enumdecl* f5; struct Cyc_Absyn_Enumfield*
f6;}; const int Cyc_Absyn_Xenum_p_tag= 9; struct Cyc_Absyn_Xenum_p_struct{ int
tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Xenumdecl* f4; struct Cyc_Absyn_Enumfield* f5;}; const int Cyc_Absyn_UnknownId_p_tag=
10; struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple0* f1;}; const
int Cyc_Absyn_UnknownCall_p_tag= 11; struct Cyc_Absyn_UnknownCall_p_struct{ int
tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;};
const int Cyc_Absyn_UnknownFields_p_tag= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;};
struct Cyc_Absyn_Pat{ void* r; struct Cyc_Core_Opt* topt; struct Cyc_Position_Segment*
loc;}; struct Cyc_Absyn_Switch_clause{ struct Cyc_Absyn_Pat* pattern; struct Cyc_Core_Opt*
pat_vars; struct Cyc_Absyn_Exp* where_clause; struct Cyc_Absyn_Stmt* body;
struct Cyc_Position_Segment* loc;}; void* Cyc_Absyn_Unresolved_b=( void*) 0;
const int Cyc_Absyn_Global_b_tag= 0; struct Cyc_Absyn_Global_b_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1;}; const int Cyc_Absyn_Funname_b_tag= 1; struct Cyc_Absyn_Funname_b_struct{
int tag; struct Cyc_Absyn_Fndecl* f1;}; const int Cyc_Absyn_Param_b_tag= 2;
struct Cyc_Absyn_Param_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;}; const
int Cyc_Absyn_Local_b_tag= 3; struct Cyc_Absyn_Local_b_struct{ int tag; struct
Cyc_Absyn_Vardecl* f1;}; const int Cyc_Absyn_Pat_b_tag= 4; struct Cyc_Absyn_Pat_b_struct{
int tag; struct Cyc_Absyn_Vardecl* f1;}; struct Cyc_Absyn_Vardecl{ void* sc;
struct _tuple0* name; struct Cyc_Absyn_Tqual* tq; void* type; struct Cyc_Absyn_Exp*
initializer; int shadow; struct Cyc_Core_Opt* region;}; struct Cyc_Absyn_Fndecl{
void* sc; int is_inline; struct _tuple0* name; struct Cyc_List_List* tvs; struct
Cyc_Core_Opt* effect; void* ret_type; struct Cyc_List_List* args; int varargs;
struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt*
param_vardecls;}; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt*
name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields;}; struct Cyc_Absyn_Enumfield{
struct _tuple0* name; struct Cyc_Absyn_Exp* tag; struct Cyc_List_List* tvs;
struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc;}; struct Cyc_Absyn_Enumdecl{
void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields;}; struct Cyc_Absyn_Xenumdecl{ void* sc; struct _tuple0* name; struct Cyc_List_List*
fields;}; struct Cyc_Absyn_Typedefdecl{ struct _tuple0* name; struct Cyc_List_List*
tvs; void* defn;}; const int Cyc_Absyn_Var_d_tag= 0; struct Cyc_Absyn_Var_d_struct{
int tag; struct Cyc_Absyn_Vardecl* f1;}; const int Cyc_Absyn_Fn_d_tag= 1; struct
Cyc_Absyn_Fn_d_struct{ int tag; struct Cyc_Absyn_Fndecl* f1;}; const int Cyc_Absyn_Let_d_tag=
2; struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt*
f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4; int f5;}; const int Cyc_Absyn_Struct_d_tag=
3; struct Cyc_Absyn_Struct_d_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;};
void* Cyc_Absyn_Union_d=( void*) 0; const int Cyc_Absyn_Enum_d_tag= 4; struct
Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1;}; const int Cyc_Absyn_Xenum_d_tag=
5; struct Cyc_Absyn_Xenum_d_struct{ int tag; struct Cyc_Absyn_Xenumdecl* f1;};
const int Cyc_Absyn_Typedef_d_tag= 6; struct Cyc_Absyn_Typedef_d_struct{ int tag;
struct Cyc_Absyn_Typedefdecl* f1;}; const int Cyc_Absyn_Namespace_d_tag= 7;
struct Cyc_Absyn_Namespace_d_struct{ int tag; struct _tagged_string* f1; struct
Cyc_List_List* f2;}; const int Cyc_Absyn_Using_d_tag= 8; struct Cyc_Absyn_Using_d_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2;}; const int Cyc_Absyn_ExternC_d_tag=
9; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1;};
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc;}; const int
Cyc_Absyn_ArrayElement_tag= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag;
struct Cyc_Absyn_Exp* f1;}; const int Cyc_Absyn_FieldName_tag= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_string* f1;}; char Cyc_Absyn_EmptyAnnot_tag[ 11u]="EmptyAnnot";
struct Cyc_Absyn_EmptyAnnot_struct{ char* tag;}; extern int Cyc_Absyn_qvar_cmp(
struct _tuple0*, struct _tuple0*); extern int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*,
struct Cyc_List_List*); extern int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*); extern struct Cyc_Absyn_Tqual* Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual* x, struct Cyc_Absyn_Tqual* y); extern struct Cyc_Absyn_Tqual*
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern
struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar(
void*); extern void* Cyc_Absyn_wildtyp(); extern void* Cyc_Absyn_uchar_t; extern
void* Cyc_Absyn_ushort_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t;
extern void* Cyc_Absyn_schar_t; extern void* Cyc_Absyn_sshort_t; extern void*
Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t; extern void* Cyc_Absyn_float_t;
extern void* Cyc_Absyn_double_t; extern struct _tuple0* Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Xenumdecl* Cyc_Absyn_exn_xed; extern void* Cyc_Absyn_exn_typ;
extern void* Cyc_Absyn_string_typ( void* rgn); extern void* Cyc_Absyn_file_typ();
extern void* Cyc_Absyn_starb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual* tq,
void* b); extern void* Cyc_Absyn_atb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual*
tq, void* b); extern void* Cyc_Absyn_star_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual*
tq); extern void* Cyc_Absyn_at_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual*
tq); extern void* Cyc_Absyn_cstar_typ( void* t, struct Cyc_Absyn_Tqual* tq);
extern void* Cyc_Absyn_tagged_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual*
tq); extern void* Cyc_Absyn_void_star_typ(); extern void* Cyc_Absyn_strct(
struct _tagged_string* name); extern void* Cyc_Absyn_strctq( struct _tuple0*
name); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_const_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_bool_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_string f,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
int heap_allocate, struct _tagged_string s, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_var_exp( struct _tuple0*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple0*, void*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknownid_exp(
struct _tuple0*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp(
void*, struct Cyc_List_List* es, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_subtract_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_divide_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_post_dec_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_pre_inc_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_pre_dec_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_conditional_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknowncall_exp( struct Cyc_Absyn_Exp*,
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_instantiate_exp( struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_cast_exp( void*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeof_exp( void* t, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp( struct Cyc_Absyn_Exp*,
struct _tagged_string*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_tuple_exp( struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_null_pointer_exn_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_array_exp( int heap_allocate, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp( struct Cyc_Core_Opt*,
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt( void* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment* loc); extern
struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt*
s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts( struct Cyc_List_List*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_while_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt( struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt( struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3, struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt( struct Cyc_List_List* el,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt( struct _tuple0*,
void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_cut_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_splice_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt( struct _tagged_string* v, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_trycatch_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* scs, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct _tagged_string* lab, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Pat* Cyc_Absyn_new_pat( void* p, struct Cyc_Position_Segment* s);
extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_let_decl( struct Cyc_Absyn_Pat* p,
struct Cyc_Core_Opt* t_opt, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x,
void* t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl(
struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Decl*
Cyc_Absyn_struct_decl( void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts,
struct Cyc_Core_Opt* fs, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl*
Cyc_Absyn_enum_decl( void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts,
struct Cyc_Core_Opt* fs, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl*
Cyc_Absyn_xenum_decl( void* s, struct _tuple0* n, struct Cyc_List_List* fs,
struct Cyc_Position_Segment* loc); extern int Cyc_Absyn_is_format_prim( void* p);
extern void* Cyc_Absyn_function_typ( struct Cyc_List_List* tvs, struct Cyc_Core_Opt*
eff_typ, void* ret_typ, struct Cyc_List_List* args, int varargs); extern void*
Cyc_Absyn_pointer_expand( void*); extern int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*);
extern struct _tuple3* Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl*,
struct _tagged_string*); extern struct _tuple4* Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*, int); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE
Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1;}; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag;}; extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); struct Cyc_Set_Set; typedef
struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern char Cyc_Set_Absent_tag[ 7u];
struct Cyc_Set_Absent_struct{ char* tag;}; struct Cyc_Dict_Dict; typedef struct
Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;};
extern char Cyc_Dict_Absent_tag[ 7u]; typedef void* Cyc_Tcenv_Resolved; extern
const int Cyc_Tcenv_VarRes_tag; struct Cyc_Tcenv_VarRes_struct{ int tag; void*
f1;}; extern const int Cyc_Tcenv_StructRes_tag; struct Cyc_Tcenv_StructRes_struct{
int tag; struct Cyc_Absyn_Structdecl* f1;}; extern const int Cyc_Tcenv_EnumRes_tag;
struct Cyc_Tcenv_EnumRes_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; struct
Cyc_Absyn_Enumfield* f2;}; extern const int Cyc_Tcenv_XenumRes_tag; struct Cyc_Tcenv_XenumRes_struct{
int tag; struct Cyc_Absyn_Xenumdecl* f1; struct Cyc_Absyn_Enumfield* f2;};
typedef void* Cyc_Tcenv_resolved_t; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set*
namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* xenumdecls; struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict*
ordinaries; struct Cyc_List_List* availables;}; typedef struct Cyc_Tcenv_Genv*
Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv; typedef struct Cyc_Tcenv_Fenv* Cyc_Tcenv_fenv_t;
typedef void* Cyc_Tcenv_Jumpee; extern void* Cyc_Tcenv_NotLoop_j; extern void*
Cyc_Tcenv_CaseEnd_j; extern void* Cyc_Tcenv_FnEnd_j; extern const int Cyc_Tcenv_Stmt_j_tag;
struct Cyc_Tcenv_Stmt_j_struct{ int tag; struct Cyc_Absyn_Stmt* f1;}; typedef
void* Cyc_Tcenv_jumpee_t; typedef void* Cyc_Tcenv_Frames; extern const int Cyc_Tcenv_Outermost_tag;
struct Cyc_Tcenv_Outermost_struct{ int tag; void* f1;}; extern const int Cyc_Tcenv_Frame_tag;
struct Cyc_Tcenv_Frame_struct{ int tag; void* f1; void* f2;}; extern const int
Cyc_Tcenv_Hidden_tag; struct Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void*
f2;}; typedef void* Cyc_Tcenv_frames; struct Cyc_Tcenv_Tenv{ struct Cyc_List_List*
ns; struct Cyc_Dict_Dict* ae; struct Cyc_Core_Opt* le;}; typedef struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tenv; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern char Cyc_Tcutil_TypeErr_tag[
8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag;}; extern void* Cyc_Tcutil_impos(
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); static int
Cyc_Absyn_zstrlist_cmp( struct Cyc_List_List* ss1, struct Cyc_List_List* ss2){
return(( int(*)( int(* cmp)( struct _tagged_string*, struct _tagged_string*),
struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_String_zstrptrcmp,
ss1, ss2);} int Cyc_Absyn_varlist_cmp( struct Cyc_List_List* vs1, struct Cyc_List_List*
vs2){ return Cyc_Absyn_zstrlist_cmp( vs1, vs2);} int Cyc_Absyn_qvar_cmp( struct
_tuple0* q1, struct _tuple0* q2){ void* n1=(* q1).f1; void* n2=(* q2).f1;{
struct _tuple5 _temp1=({ struct _tuple5 _temp0; _temp0.f1= n1; _temp0.f2= n2;
_temp0;}); void* _temp17; void* _temp19; void* _temp21; struct Cyc_List_List*
_temp23; void* _temp25; struct Cyc_List_List* _temp27; void* _temp29; struct Cyc_List_List*
_temp31; void* _temp33; struct Cyc_List_List* _temp35; void* _temp37; void*
_temp39; void* _temp41; void* _temp43; void* _temp45; void* _temp47; struct Cyc_List_List*
_temp49; void* _temp51; struct Cyc_List_List* _temp53; void* _temp55; _LL3:
_LL20: _temp19= _temp1.f1; if( _temp19 == Cyc_Absyn_Loc_n){ goto _LL18;} else{
goto _LL5;} _LL18: _temp17= _temp1.f2; if( _temp17 == Cyc_Absyn_Loc_n){ goto
_LL4;} else{ goto _LL5;} _LL5: _LL26: _temp25= _temp1.f1; if(( unsigned int)
_temp25 > 1u?(( struct _enum_struct*) _temp25)->tag == Cyc_Absyn_Rel_n_tag: 0){
_LL28: _temp27=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp25)->f1; goto _LL22;} else{ goto _LL7;} _LL22: _temp21= _temp1.f2; if((
unsigned int) _temp21 > 1u?(( struct _enum_struct*) _temp21)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL24: _temp23=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp21)->f1; goto _LL6;} else{ goto _LL7;} _LL7: _LL34: _temp33= _temp1.f1; if((
unsigned int) _temp33 > 1u?(( struct _enum_struct*) _temp33)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL36: _temp35=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp33)->f1; goto _LL30;} else{ goto _LL9;} _LL30: _temp29= _temp1.f2; if((
unsigned int) _temp29 > 1u?(( struct _enum_struct*) _temp29)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL32: _temp31=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp29)->f1; goto _LL8;} else{ goto _LL9;} _LL9: _LL40: _temp39= _temp1.f1; if(
_temp39 == Cyc_Absyn_Loc_n){ goto _LL38;} else{ goto _LL11;} _LL38: _temp37=
_temp1.f2; goto _LL10; _LL11: _LL44: _temp43= _temp1.f1; goto _LL42; _LL42:
_temp41= _temp1.f2; if( _temp41 == Cyc_Absyn_Loc_n){ goto _LL12;} else{ goto
_LL13;} _LL13: _LL48: _temp47= _temp1.f1; if(( unsigned int) _temp47 > 1u?((
struct _enum_struct*) _temp47)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL50: _temp49=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp47)->f1; goto
_LL46;} else{ goto _LL15;} _LL46: _temp45= _temp1.f2; goto _LL14; _LL15: _LL56:
_temp55= _temp1.f1; goto _LL52; _LL52: _temp51= _temp1.f2; if(( unsigned int)
_temp51 > 1u?(( struct _enum_struct*) _temp51)->tag == Cyc_Absyn_Rel_n_tag: 0){
_LL54: _temp53=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp51)->f1; goto _LL16;} else{ goto _LL2;} _LL4: goto _LL2; _LL6: _temp35=
_temp27; _temp31= _temp23; goto _LL8; _LL8: { int i= Cyc_Absyn_zstrlist_cmp(
_temp35, _temp31); if( i != 0){ return i;} goto _LL2;} _LL10: return - 1; _LL12:
return 1; _LL14: return - 1; _LL16: return 1; _LL2:;} return Cyc_String_zstrptrcmp((*
q1).f2,(* q2).f2);} int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar* tv1, struct
Cyc_Absyn_Tvar* tv2){ return Cyc_String_zstrptrcmp( tv1->name, tv2->name);}
static int Cyc_Absyn_new_type_counter= 0; void* Cyc_Absyn_new_evar( void* k){
return({ struct Cyc_Absyn_Evar_struct* _temp57=( struct Cyc_Absyn_Evar_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Evar_struct)); _temp57->tag= Cyc_Absyn_Evar_tag;
_temp57->f1=( void*) k; _temp57->f2= 0; _temp57->f3=( Cyc_Absyn_new_type_counter
++);( void*) _temp57;});} void* Cyc_Absyn_wildtyp(){ return Cyc_Absyn_new_evar(
Cyc_Absyn_MemKind);} struct Cyc_Absyn_Tqual* Cyc_Absyn_combine_tqual( struct Cyc_Absyn_Tqual*
x, struct Cyc_Absyn_Tqual* y){ return({ struct Cyc_Absyn_Tqual* _temp58=( struct
Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp58->q_const=
x->q_const? 1: y->q_const; _temp58->q_volatile= x->q_volatile? 1: y->q_volatile;
_temp58->q_restrict= x->q_restrict? 1: y->q_restrict; _temp58;});} struct Cyc_Absyn_Tqual*
Cyc_Absyn_empty_tqual(){ return({ struct Cyc_Absyn_Tqual* _temp59=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp59->q_const= 0; _temp59->q_volatile=
0; _temp59->q_restrict= 0; _temp59;});} int Cyc_Absyn_is_format_prim( void* p){
void* _temp60= p; _LL62: if( _temp60 == Cyc_Absyn_Printf){ goto _LL63;} else{
goto _LL64;} _LL64: if( _temp60 == Cyc_Absyn_Fprintf){ goto _LL65;} else{ goto
_LL66;} _LL66: if( _temp60 == Cyc_Absyn_Xprintf){ goto _LL67;} else{ goto _LL68;}
_LL68: if( _temp60 == Cyc_Absyn_Scanf){ goto _LL69;} else{ goto _LL70;} _LL70:
if( _temp60 == Cyc_Absyn_Fscanf){ goto _LL71;} else{ goto _LL72;} _LL72: if(
_temp60 == Cyc_Absyn_Sscanf){ goto _LL73;} else{ goto _LL74;} _LL74: goto _LL75;
_LL63: return 1; _LL65: return 1; _LL67: return 1; _LL69: return 1; _LL71:
return 1; _LL73: return 1; _LL75: return 0; _LL61:;} struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x){ return({ struct Cyc_Absyn_Conref* _temp76=(
struct Cyc_Absyn_Conref*) GC_malloc( sizeof( struct Cyc_Absyn_Conref)); _temp76->v=(
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp77=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp77->tag= Cyc_Absyn_Eq_constr_tag;
_temp77->f1=( void*) x;( void*) _temp77;}); _temp76;});} struct Cyc_Absyn_Conref*
Cyc_Absyn_empty_conref(){ return({ struct Cyc_Absyn_Conref* _temp78=( struct Cyc_Absyn_Conref*)
GC_malloc( sizeof( struct Cyc_Absyn_Conref)); _temp78->v=( void*) Cyc_Absyn_No_constr;
_temp78;});} struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x){ void* _temp79=( void*) x->v; void* _temp87; struct Cyc_Absyn_Conref* _temp89;
struct Cyc_Absyn_Conref** _temp91; _LL81: if( _temp79 == Cyc_Absyn_No_constr){
goto _LL82;} else{ goto _LL83;} _LL83: if(( unsigned int) _temp79 > 1u?(( struct
_enum_struct*) _temp79)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL88: _temp87=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp79)->f1; goto _LL84;} else{
goto _LL85;} _LL85: if(( unsigned int) _temp79 > 1u?(( struct _enum_struct*)
_temp79)->tag == Cyc_Absyn_Forward_constr_tag: 0){ _LL90: _temp89=( struct Cyc_Absyn_Conref*)((
struct Cyc_Absyn_Forward_constr_struct*) _temp79)->f1; _temp91=&(( struct Cyc_Absyn_Forward_constr_struct*)
_temp79)->f1; goto _LL86;} else{ goto _LL80;} _LL82: return x; _LL84: return x;
_LL86: { struct Cyc_Absyn_Conref* z=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)(* _temp91);* _temp91= z; return z;} _LL80:;} void*
Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x){ void* _temp92=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
x))->v; void* _temp98; _LL94: if(( unsigned int) _temp92 > 1u?(( struct
_enum_struct*) _temp92)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL99: _temp98=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp92)->f1; goto _LL95;} else{
goto _LL96;} _LL96: goto _LL97; _LL95: return _temp98; _LL97: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp100=( char*)"conref_val"; struct _tagged_string _temp101; _temp101.curr=
_temp100; _temp101.base= _temp100; _temp101.last_plus_one= _temp100 + 11;
_temp101;})); _LL93:;} static struct Cyc_Absyn_IntType_struct _temp102={ 5u,(
void*) 1u,( void*) 0u}; void* Cyc_Absyn_uchar_t=( struct Cyc_Absyn_IntType_struct*)&
_temp102; static struct Cyc_Absyn_IntType_struct _temp103={ 5u,( void*) 1u,(
void*) 1u}; void* Cyc_Absyn_ushort_t=( struct Cyc_Absyn_IntType_struct*)&
_temp103; static struct Cyc_Absyn_IntType_struct _temp104={ 5u,( void*) 1u,(
void*) 2u}; void* Cyc_Absyn_uint_t=( struct Cyc_Absyn_IntType_struct*)& _temp104;
static struct Cyc_Absyn_IntType_struct _temp105={ 5u,( void*) 1u,( void*) 3u};
void* Cyc_Absyn_ulong_t=( struct Cyc_Absyn_IntType_struct*)& _temp105; static
struct Cyc_Absyn_IntType_struct _temp106={ 5u,( void*) 0u,( void*) 0u}; void*
Cyc_Absyn_schar_t=( struct Cyc_Absyn_IntType_struct*)& _temp106; static struct
Cyc_Absyn_IntType_struct _temp107={ 5u,( void*) 0u,( void*) 1u}; void* Cyc_Absyn_sshort_t=(
struct Cyc_Absyn_IntType_struct*)& _temp107; static struct Cyc_Absyn_IntType_struct
_temp108={ 5u,( void*) 0u,( void*) 2u}; void* Cyc_Absyn_sint_t=( struct Cyc_Absyn_IntType_struct*)&
_temp108; static struct Cyc_Absyn_IntType_struct _temp109={ 5u,( void*) 0u,(
void*) 3u}; void* Cyc_Absyn_slong_t=( struct Cyc_Absyn_IntType_struct*)&
_temp109; void* Cyc_Absyn_float_t=( void*) 1u; void* Cyc_Absyn_double_t=( void*)
2u; static char _temp112[ 4u]="exn"; static struct _tagged_string Cyc_Absyn_exn_string=(
struct _tagged_string){ _temp112, _temp112, _temp112 + 4u}; static struct Cyc_Absyn_Abs_n_struct
_temp113={ 1u, 0}; static struct _tuple0 Cyc_Absyn_exn_name_v=( struct _tuple0){.f1=(
struct Cyc_Absyn_Abs_n_struct*)& _temp113,.f2=& Cyc_Absyn_exn_string}; struct
_tuple0* Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v; static struct Cyc_Absyn_Xenumdecl
Cyc_Absyn_exn_xed_v=( struct Cyc_Absyn_Xenumdecl){.sc=( void*)(( void*) 3u),.name=&
Cyc_Absyn_exn_name_v,.fields= 0}; struct Cyc_Absyn_Xenumdecl* Cyc_Absyn_exn_xed=&
Cyc_Absyn_exn_xed_v; static struct Cyc_Absyn_XenumType_struct _temp114={ 3u,&
Cyc_Absyn_exn_name_v,( struct Cyc_Absyn_Xenumdecl**)& Cyc_Absyn_exn_xed}; void*
Cyc_Absyn_exn_typ=( struct Cyc_Absyn_XenumType_struct*)& _temp114; static struct
Cyc_Core_Opt* Cyc_Absyn_string_t_opt= 0; void* Cyc_Absyn_string_typ( void* rgn){
if( Cyc_Absyn_string_t_opt == 0){ void* t= Cyc_Absyn_starb_typ( Cyc_Absyn_uchar_t,
rgn, Cyc_Absyn_empty_tqual(), Cyc_Absyn_Unknown_b); Cyc_Absyn_string_t_opt=({
struct Cyc_Core_Opt* _temp115=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp115->v=( void*) t; _temp115;});} return( void*) Cyc_Absyn_string_t_opt->v;}
void* Cyc_Absyn_starb_typ( void* t, void* r, struct Cyc_Absyn_Tqual* tq, void* b){
return({ struct Cyc_Absyn_PointerType_struct* _temp116=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp116->tag= Cyc_Absyn_PointerType_tag;
_temp116->f1=({ struct Cyc_Absyn_PtrInfo _temp117; _temp117.elt_typ=( void*) t;
_temp117.rgn_typ=( void*) r; _temp117.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 1); _temp117.tq= tq; _temp117.bounds=(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( b); _temp117;});( void*)
_temp116;});} void* Cyc_Absyn_atb_typ( void* t, void* r, struct Cyc_Absyn_Tqual*
tq, void* b){ return({ struct Cyc_Absyn_PointerType_struct* _temp118=( struct
Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp118->tag= Cyc_Absyn_PointerType_tag; _temp118->f1=({ struct Cyc_Absyn_PtrInfo
_temp119; _temp119.elt_typ=( void*) t; _temp119.rgn_typ=( void*) r; _temp119.nullable=((
struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 0); _temp119.tq= tq;
_temp119.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
b); _temp119;});( void*) _temp118;});} void* Cyc_Absyn_star_typ( void* t, void*
r, struct Cyc_Absyn_Tqual* tq){ return Cyc_Absyn_starb_typ( t, r, tq,({ struct
Cyc_Absyn_Upper_b_struct* _temp120=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp120->tag= Cyc_Absyn_Upper_b_tag;
_temp120->f1= Cyc_Absyn_signed_int_exp( 1, 0);( void*) _temp120;}));} void* Cyc_Absyn_cstar_typ(
void* t, struct Cyc_Absyn_Tqual* tq){ return Cyc_Absyn_starb_typ( t, Cyc_Absyn_HeapRgn,
tq,({ struct Cyc_Absyn_Upper_b_struct* _temp121=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp121->tag= Cyc_Absyn_Upper_b_tag;
_temp121->f1= Cyc_Absyn_signed_int_exp( 1, 0);( void*) _temp121;}));} void* Cyc_Absyn_at_typ(
void* t, void* r, struct Cyc_Absyn_Tqual* tq){ return Cyc_Absyn_atb_typ( t, r,
tq,({ struct Cyc_Absyn_Upper_b_struct* _temp122=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp122->tag= Cyc_Absyn_Upper_b_tag;
_temp122->f1= Cyc_Absyn_signed_int_exp( 1, 0);( void*) _temp122;}));} void* Cyc_Absyn_tagged_typ(
void* t, void* r, struct Cyc_Absyn_Tqual* tq){ return({ struct Cyc_Absyn_PointerType_struct*
_temp123=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp123->tag= Cyc_Absyn_PointerType_tag; _temp123->f1=({ struct Cyc_Absyn_PtrInfo
_temp124; _temp124.elt_typ=( void*) t; _temp124.rgn_typ=( void*) r; _temp124.nullable=((
struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 1); _temp124.tq= tq;
_temp124.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_Unknown_b); _temp124;});( void*) _temp123;});} static struct Cyc_Core_Opt*
Cyc_Absyn_file_t_opt= 0; static char _temp127[ 8u]="__sFILE"; static struct
_tagged_string Cyc_Absyn_sf_str=( struct _tagged_string){ _temp127, _temp127,
_temp127 + 8u}; static struct _tagged_string* Cyc_Absyn_sf=& Cyc_Absyn_sf_str;
static char _temp130[ 4u]="Cyc"; static struct _tagged_string Cyc_Absyn_cyc_str=(
struct _tagged_string){ _temp130, _temp130, _temp130 + 4u}; static struct
_tagged_string* Cyc_Absyn_cyc=& Cyc_Absyn_cyc_str; static char _temp133[ 6u]="Stdio";
static struct _tagged_string Cyc_Absyn_st_str=( struct _tagged_string){ _temp133,
_temp133, _temp133 + 6u}; static struct _tagged_string* Cyc_Absyn_st=& Cyc_Absyn_st_str;
void* Cyc_Absyn_file_typ(){ if( Cyc_Absyn_file_t_opt == 0){ struct _tuple0*
file_t_name=({ struct _tuple0* _temp134=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0) * 1); _temp134[ 0]=({ struct _tuple0 _temp135; _temp135.f1=({
struct Cyc_Absyn_Abs_n_struct* _temp136=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp136->tag= Cyc_Absyn_Abs_n_tag;
_temp136->f1=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp137=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp137->hd=( void*)
Cyc_Absyn_cyc; _temp137->tl=({ struct Cyc_List_List* _temp138=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp138->hd=( void*) Cyc_Absyn_st;
_temp138->tl= 0; _temp138;}); _temp137;});( void*) _temp136;}); _temp135.f2= Cyc_Absyn_sf;
_temp135;}); _temp134;}); struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl*
_temp139=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp139->sc=( void*) Cyc_Absyn_Abstract; _temp139->name=({ struct Cyc_Core_Opt*
_temp140=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp140->v=( void*) file_t_name; _temp140;}); _temp139->tvs= 0; _temp139->fields=
0; _temp139;}); void* file_struct_typ=({ struct Cyc_Absyn_StructType_struct*
_temp141=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp141->tag= Cyc_Absyn_StructType_tag; _temp141->f1=( struct _tuple0*)
file_t_name; _temp141->f2= 0; _temp141->f3=( struct Cyc_Absyn_Structdecl**)({
struct Cyc_Absyn_Structdecl** _temp142=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*) * 1); _temp142[ 0]= sd;
_temp142;});( void*) _temp141;}); Cyc_Absyn_file_t_opt=({ struct Cyc_Core_Opt*
_temp143=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp143->v=( void*) Cyc_Absyn_at_typ( file_struct_typ, Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
_temp143;});} return( void*) Cyc_Absyn_file_t_opt->v;} static struct Cyc_Core_Opt*
Cyc_Absyn_void_star_t_opt= 0; void* Cyc_Absyn_void_star_typ(){ if( Cyc_Absyn_void_star_t_opt
== 0){ Cyc_Absyn_void_star_t_opt=({ struct Cyc_Core_Opt* _temp144=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp144->v=( void*) Cyc_Absyn_star_typ(
Cyc_Absyn_VoidType, Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); _temp144;});}
return( void*) Cyc_Absyn_void_star_t_opt->v;} void* Cyc_Absyn_strct( struct
_tagged_string* name){ return({ struct Cyc_Absyn_StructType_struct* _temp145=(
struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp145->tag= Cyc_Absyn_StructType_tag; _temp145->f1=( struct _tuple0*)({
struct _tuple0* _temp146=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0) *
1); _temp146[ 0]=({ struct _tuple0 _temp147; _temp147.f1=({ struct Cyc_Absyn_Abs_n_struct*
_temp148=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp148->tag= Cyc_Absyn_Abs_n_tag; _temp148->f1= 0;( void*) _temp148;});
_temp147.f2= name; _temp147;}); _temp146;}); _temp145->f2= 0; _temp145->f3= 0;(
void*) _temp145;});} void* Cyc_Absyn_strctq( struct _tuple0* name){ return({
struct Cyc_Absyn_StructType_struct* _temp149=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp149->tag= Cyc_Absyn_StructType_tag;
_temp149->f1=( struct _tuple0*) name; _temp149->f2= 0; _temp149->f3= 0;( void*)
_temp149;});} struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void* r, struct Cyc_Position_Segment*
loc){ return({ struct Cyc_Absyn_Exp* _temp150=( struct Cyc_Absyn_Exp*) GC_malloc(
sizeof( struct Cyc_Absyn_Exp)); _temp150->topt= 0; _temp150->r=( void*) r;
_temp150->loc= loc; _temp150;});} struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp* e){ return({ struct Cyc_Absyn_Exp* _temp151=( struct Cyc_Absyn_Exp*)
GC_malloc( sizeof( struct Cyc_Absyn_Exp)); _temp151->topt= e->topt; _temp151->r=(
void*)(( void*) e->r); _temp151->loc= e->loc; _temp151;});} struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp( void* c, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Const_e_struct* _temp152=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp152->tag= Cyc_Absyn_Const_e_tag;
_temp152->f1=( void*) c;( void*) _temp152;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Const_e_struct*
_temp153=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp153->tag= Cyc_Absyn_Const_e_tag; _temp153->f1=( void*) Cyc_Absyn_Null_c;(
void*) _temp153;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void* s, int
i, struct Cyc_Position_Segment* seg){ return Cyc_Absyn_const_exp(({ struct Cyc_Absyn_Int_c_struct*
_temp154=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp154->tag= Cyc_Absyn_Int_c_tag; _temp154->f1=( void*) s; _temp154->f2= i;(
void*) _temp154;}), seg);} struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int i,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_int_exp( Cyc_Absyn_Signed, i,
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int i, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_int_exp( Cyc_Absyn_Unsigned,( int) i, loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_bool_exp( int b, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_signed_int_exp(
b? 1: 0, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_bool_exp( 1, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_bool_exp( 0, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_char_exp( char c, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_const_exp(({ struct Cyc_Absyn_Char_c_struct* _temp155=( struct
Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp155->tag= Cyc_Absyn_Char_c_tag; _temp155->f1=( void*) Cyc_Absyn_Unsigned;
_temp155->f2= c;( void*) _temp155;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp(
struct _tagged_string f, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_const_exp(({
struct Cyc_Absyn_Float_c_struct* _temp156=( struct Cyc_Absyn_Float_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp156->tag= Cyc_Absyn_Float_c_tag;
_temp156->f1= f;( void*) _temp156;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
int heap_allocate, struct _tagged_string s, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_const_exp(({ struct Cyc_Absyn_String_c_struct* _temp157=(
struct Cyc_Absyn_String_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct));
_temp157->tag= Cyc_Absyn_String_c_tag; _temp157->f1= heap_allocate; _temp157->f2=
s;( void*) _temp157;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_var_exp( struct
_tuple0* q, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Var_e_struct* _temp158=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp158->tag= Cyc_Absyn_Var_e_tag;
_temp158->f1= q; _temp158->f2=( void*) Cyc_Absyn_Unresolved_b;( void*) _temp158;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple0* q, void* b,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Var_e_struct*
_temp159=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp159->tag= Cyc_Absyn_Var_e_tag; _temp159->f1= q; _temp159->f2=( void*) b;(
void*) _temp159;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_unknownid_exp( struct
_tuple0* q, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnknownId_e_struct* _temp160=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp160->tag= Cyc_Absyn_UnknownId_e_tag;
_temp160->f1= q;( void*) _temp160;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp(
void* p, struct Cyc_List_List* es, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Primop_e_struct* _temp161=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp161->tag= Cyc_Absyn_Primop_e_tag;
_temp161->f1=( void*) p; _temp161->f2= es;( void*) _temp161;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp( void* p, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_primop_exp( p,({ struct Cyc_List_List* _temp162=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp162->hd=( void*)
e; _temp162->tl= 0; _temp162;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp(
void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_primop_exp( p,({ struct Cyc_List_List* _temp163=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp163->hd=( void*)
e1; _temp163->tl=({ struct Cyc_List_List* _temp164=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp164->hd=( void*) e2; _temp164->tl=
0; _temp164;}); _temp163;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp( Cyc_Absyn_Plus, e1, e2, loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_subtract_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp( Cyc_Absyn_Minus,
e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(
Cyc_Absyn_Times, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp( Cyc_Absyn_Div, e1, e2, loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp( Cyc_Absyn_Eq, e1, e2, loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_neq_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp( Cyc_Absyn_Neq, e1, e2,
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp( struct Cyc_Absyn_Exp* e1, struct
Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(
Cyc_Absyn_Gt, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(
Cyc_Absyn_Lt, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(
Cyc_Absyn_Gte, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp( struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_prim2_exp( Cyc_Absyn_Lte, e1, e2, loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_assignop_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt* popt,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_AssignOp_e_struct* _temp165=( struct Cyc_Absyn_AssignOp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AssignOp_e_struct)); _temp165->tag= Cyc_Absyn_AssignOp_e_tag;
_temp165->f1= e1; _temp165->f2= popt; _temp165->f3= e2;( void*) _temp165;}), loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_assignop_exp( e1, 0, e2,
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp* e,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Increment_e_struct*
_temp166=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp166->tag= Cyc_Absyn_Increment_e_tag; _temp166->f1= e; _temp166->f2=( void*)
Cyc_Absyn_PostInc;( void*) _temp166;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Increment_e_struct* _temp167=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp167->tag= Cyc_Absyn_Increment_e_tag;
_temp167->f1= e; _temp167->f2=( void*) Cyc_Absyn_PreInc;( void*) _temp167;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_pre_dec_exp( struct Cyc_Absyn_Exp* e,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Increment_e_struct*
_temp168=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp168->tag= Cyc_Absyn_Increment_e_tag; _temp168->f1= e; _temp168->f2=( void*)
Cyc_Absyn_PreDec;( void*) _temp168;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Increment_e_struct* _temp169=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp169->tag= Cyc_Absyn_Increment_e_tag;
_temp169->f1= e; _temp169->f2=( void*) Cyc_Absyn_PostDec;( void*) _temp169;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Conditional_e_struct*
_temp170=( struct Cyc_Absyn_Conditional_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Conditional_e_struct));
_temp170->tag= Cyc_Absyn_Conditional_e_tag; _temp170->f1= e1; _temp170->f2= e2;
_temp170->f3= e3;( void*) _temp170;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_conditional_exp( e1, e2, Cyc_Absyn_false_exp( loc), loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_conditional_exp( e1, Cyc_Absyn_true_exp(
loc), e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_SeqExp_e_struct* _temp171=( struct Cyc_Absyn_SeqExp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_SeqExp_e_struct)); _temp171->tag= Cyc_Absyn_SeqExp_e_tag;
_temp171->f1= e1; _temp171->f2= e2;( void*) _temp171;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp( struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_UnknownCall_e_struct*
_temp172=( struct Cyc_Absyn_UnknownCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownCall_e_struct));
_temp172->tag= Cyc_Absyn_UnknownCall_e_tag; _temp172->f1= e; _temp172->f2= es;(
void*) _temp172;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct
Cyc_Absyn_Exp* e, struct Cyc_List_List* es, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_FnCall_e_struct* _temp173=( struct
Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp173->tag= Cyc_Absyn_FnCall_e_tag; _temp173->f1= e; _temp173->f2= es;( void*)
_temp173;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_noinstantiate_exp( struct
Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_NoInstantiate_e_struct* _temp174=( struct Cyc_Absyn_NoInstantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_NoInstantiate_e_struct)); _temp174->tag= Cyc_Absyn_NoInstantiate_e_tag;
_temp174->f1= e;( void*) _temp174;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_List_List* ts, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Instantiate_e_struct*
_temp175=( struct Cyc_Absyn_Instantiate_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct));
_temp175->tag= Cyc_Absyn_Instantiate_e_tag; _temp175->f1= e; _temp175->f2= ts;(
void*) _temp175;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_cast_exp( void* t,
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Cast_e_struct* _temp176=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp176->tag= Cyc_Absyn_Cast_e_tag;
_temp176->f1=( void*) t; _temp176->f2= e;( void*) _temp176;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Throw_e_struct* _temp177=( struct
Cyc_Absyn_Throw_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Throw_e_struct));
_temp177->tag= Cyc_Absyn_Throw_e_tag; _temp177->f1= e;( void*) _temp177;}), loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Address_e_struct* _temp178=(
struct Cyc_Absyn_Address_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Address_e_struct));
_temp178->tag= Cyc_Absyn_Address_e_tag; _temp178->f1= e;( void*) _temp178;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_sizeof_exp( void* t, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Sizeof_e_struct* _temp179=(
struct Cyc_Absyn_Sizeof_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeof_e_struct));
_temp179->tag= Cyc_Absyn_Sizeof_e_tag; _temp179->f1=( void*) t;( void*) _temp179;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp( struct Cyc_Absyn_Exp* e,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Deref_e_struct*
_temp180=( struct Cyc_Absyn_Deref_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Deref_e_struct));
_temp180->tag= Cyc_Absyn_Deref_e_tag; _temp180->f1= e;( void*) _temp180;}), loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp* e,
struct _tagged_string* n, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_StructMember_e_struct* _temp181=( struct Cyc_Absyn_StructMember_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructMember_e_struct)); _temp181->tag= Cyc_Absyn_StructMember_e_tag;
_temp181->f1= e; _temp181->f2= n;( void*) _temp181;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_structarrow_exp( struct Cyc_Absyn_Exp* e, struct _tagged_string* n,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_StructArrow_e_struct*
_temp182=( struct Cyc_Absyn_StructArrow_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructArrow_e_struct));
_temp182->tag= Cyc_Absyn_StructArrow_e_tag; _temp182->f1= e; _temp182->f2= n;(
void*) _temp182;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_arrow_exp( struct Cyc_Absyn_Exp*
e, struct _tagged_string* n, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_deref_exp(
Cyc_Absyn_structmember_exp( e, n, loc), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Subscript_e_struct* _temp183=(
struct Cyc_Absyn_Subscript_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Subscript_e_struct));
_temp183->tag= Cyc_Absyn_Subscript_e_tag; _temp183->f1= e1; _temp183->f2= e2;(
void*) _temp183;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_tuple_exp( struct Cyc_List_List*
es, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Tuple_e_struct*
_temp184=( struct Cyc_Absyn_Tuple_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tuple_e_struct));
_temp184->tag= Cyc_Absyn_Tuple_e_tag; _temp184->f1= es;( void*) _temp184;}), loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp( struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_StmtExp_e_struct* _temp185=(
struct Cyc_Absyn_StmtExp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StmtExp_e_struct));
_temp185->tag= Cyc_Absyn_StmtExp_e_tag; _temp185->f1= s;( void*) _temp185;}),
loc);} static char _temp188[ 15u]="Null_Exception"; static struct _tagged_string
Cyc_Absyn_ne_str=( struct _tagged_string){ _temp188, _temp188, _temp188 + 15u};
static struct _tagged_string* Cyc_Absyn_ne=& Cyc_Absyn_ne_str; struct Cyc_Absyn_Exp*
Cyc_Absyn_null_pointer_exn_exp( struct Cyc_Position_Segment* loc){ return Cyc_Absyn_var_exp(({
struct _tuple0* _temp189=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp189->f1=({ struct Cyc_Absyn_Abs_n_struct* _temp190=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp190->tag= Cyc_Absyn_Abs_n_tag;
_temp190->f1= 0;( void*) _temp190;}); _temp189->f2= Cyc_Absyn_ne; _temp189;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_array_exp( int heap_allocate, struct Cyc_List_List*
es, struct Cyc_Position_Segment* loc){ struct Cyc_List_List* dles= 0; for( 0; es
!= 0; es= es->tl){ dles=({ struct Cyc_List_List* _temp191=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp191->hd=( void*)({ struct
_tuple6* _temp192=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp192->f1= 0; _temp192->f2=( struct Cyc_Absyn_Exp*) es->hd; _temp192;});
_temp191->tl= dles; _temp191;});} dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( dles); return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Array_e_struct*
_temp193=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp193->tag= Cyc_Absyn_Array_e_tag; _temp193->f1= heap_allocate; _temp193->f2=
dles;( void*) _temp193;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt* n, struct Cyc_List_List* dles, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp194=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp194->tag= Cyc_Absyn_UnresolvedMem_e_tag;
_temp194->f1= n; _temp194->f2= dles;( void*) _temp194;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt( void* s, struct Cyc_Position_Segment* loc){ return({ struct
Cyc_Absyn_Stmt* _temp195=( struct Cyc_Absyn_Stmt*) GC_malloc( sizeof( struct Cyc_Absyn_Stmt));
_temp195->r=( void*) s; _temp195->loc= loc; _temp195->non_local_preds= 0;
_temp195->try_depth= 0; _temp195->annot=({ struct Cyc_Absyn_EmptyAnnot_struct*
_temp196=( struct Cyc_Absyn_EmptyAnnot_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Absyn_EmptyAnnot_struct));* _temp196=( struct Cyc_Absyn_EmptyAnnot_struct){.tag=
Cyc_Absyn_EmptyAnnot_tag};( struct _xenum_struct*) _temp196;}); _temp195;});}
struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_stmt( Cyc_Absyn_Skip_s, loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({
struct Cyc_Absyn_Exp_s_struct* _temp197=( struct Cyc_Absyn_Exp_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Exp_s_struct)); _temp197->tag= Cyc_Absyn_Exp_s_tag;
_temp197->f1= e;( void*) _temp197;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts(
struct Cyc_List_List* ss, struct Cyc_Position_Segment* loc){ if( ss == 0){
return Cyc_Absyn_skip_stmt( loc);} else{ if( ss->tl == 0){ return( struct Cyc_Absyn_Stmt*)
ss->hd;} else{ return Cyc_Absyn_seq_stmt(( struct Cyc_Absyn_Stmt*) ss->hd, Cyc_Absyn_seq_stmts(
ss->tl, loc), loc);}}} struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Return_s_struct*
_temp198=( struct Cyc_Absyn_Return_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Return_s_struct));
_temp198->tag= Cyc_Absyn_Return_s_tag; _temp198->f1= e;( void*) _temp198;}), loc);}
struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt( struct Cyc_Absyn_Exp* e,
struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_IfThenElse_s_struct*
_temp199=( struct Cyc_Absyn_IfThenElse_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IfThenElse_s_struct));
_temp199->tag= Cyc_Absyn_IfThenElse_s_tag; _temp199->f1= e; _temp199->f2= s1;
_temp199->f3= s2;( void*) _temp199;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_while_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_While_s_struct* _temp200=(
struct Cyc_Absyn_While_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_While_s_struct));
_temp200->tag= Cyc_Absyn_While_s_tag; _temp200->f1=({ struct _tuple2 _temp201;
_temp201.f1= e; _temp201.f2= Cyc_Absyn_skip_stmt( e->loc); _temp201;}); _temp200->f2=
s;( void*) _temp200;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Break_s_struct*
_temp202=( struct Cyc_Absyn_Break_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Break_s_struct));
_temp202->tag= Cyc_Absyn_Break_s_tag; _temp202->f1= 0;( void*) _temp202;}), loc);}
struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt( struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Continue_s_struct* _temp203=(
struct Cyc_Absyn_Continue_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Continue_s_struct));
_temp203->tag= Cyc_Absyn_Continue_s_tag; _temp203->f1= 0;( void*) _temp203;}),
loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3, struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_For_s_struct*
_temp204=( struct Cyc_Absyn_For_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_For_s_struct));
_temp204->tag= Cyc_Absyn_For_s_tag; _temp204->f1= e1; _temp204->f2=({ struct
_tuple2 _temp205; _temp205.f1= e2; _temp205.f2= Cyc_Absyn_skip_stmt( e3->loc);
_temp205;}); _temp204->f3=({ struct _tuple2 _temp206; _temp206.f1= e3; _temp206.f2=
Cyc_Absyn_skip_stmt( e3->loc); _temp206;}); _temp204->f4= s;( void*) _temp204;}),
loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* scs, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({
struct Cyc_Absyn_Switch_s_struct* _temp207=( struct Cyc_Absyn_Switch_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_s_struct)); _temp207->tag= Cyc_Absyn_Switch_s_tag;
_temp207->f1= e; _temp207->f2= scs;( void*) _temp207;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct
Cyc_Position_Segment* loc){ void* _temp208=( void*) s1->r; _LL210: if( _temp208
== Cyc_Absyn_Skip_s){ goto _LL211;} else{ goto _LL212;} _LL212: goto _LL213;
_LL211: return s2; _LL213: return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Seq_s_struct*
_temp214=( struct Cyc_Absyn_Seq_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Seq_s_struct));
_temp214->tag= Cyc_Absyn_Seq_s_tag; _temp214->f1= s1; _temp214->f2= s2;( void*)
_temp214;}), loc); _LL209:;} struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List* el, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({
struct Cyc_Absyn_Fallthru_s_struct* _temp215=( struct Cyc_Absyn_Fallthru_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fallthru_s_struct)); _temp215->tag= Cyc_Absyn_Fallthru_s_tag;
_temp215->f1= el; _temp215->f2= 0;( void*) _temp215;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_decl_stmt( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Decl_s_struct*
_temp216=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp216->tag= Cyc_Absyn_Decl_s_tag; _temp216->f1= d; _temp216->f2= s;( void*)
_temp216;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt( struct
_tuple0* x, void* t, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Decl* d= Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Var_d_struct* _temp217=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp217->tag= Cyc_Absyn_Var_d_tag;
_temp217->f1= Cyc_Absyn_new_vardecl( x, t, init);( void*) _temp217;}), loc);
return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Decl_s_struct* _temp218=( struct
Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp218->tag= Cyc_Absyn_Decl_s_tag; _temp218->f1= d; _temp218->f2= s;( void*)
_temp218;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_cut_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Cut_s_struct*
_temp219=( struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp219->tag= Cyc_Absyn_Cut_s_tag; _temp219->f1= s;( void*) _temp219;}), loc);}
struct Cyc_Absyn_Stmt* Cyc_Absyn_splice_stmt( struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Splice_s_struct*
_temp220=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp220->tag= Cyc_Absyn_Splice_s_tag; _temp220->f1= s;( void*) _temp220;}), loc);}
struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt( struct _tagged_string* v, struct
Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({
struct Cyc_Absyn_Label_s_struct* _temp221=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp221->tag= Cyc_Absyn_Label_s_tag;
_temp221->f1= v; _temp221->f2= s;( void*) _temp221;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_do_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Do_s_struct* _temp222=(
struct Cyc_Absyn_Do_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Do_s_struct));
_temp222->tag= Cyc_Absyn_Do_s_tag; _temp222->f1= s; _temp222->f2=({ struct
_tuple2 _temp223; _temp223.f1= e; _temp223.f2= Cyc_Absyn_skip_stmt( e->loc);
_temp223;});( void*) _temp222;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_trycatch_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* scs, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_TryCatch_s_struct* _temp224=(
struct Cyc_Absyn_TryCatch_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TryCatch_s_struct));
_temp224->tag= Cyc_Absyn_TryCatch_s_tag; _temp224->f1= s; _temp224->f2= scs;(
void*) _temp224;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct
_tagged_string* lab, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({
struct Cyc_Absyn_Goto_s_struct* _temp225=( struct Cyc_Absyn_Goto_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Goto_s_struct)); _temp225->tag= Cyc_Absyn_Goto_s_tag;
_temp225->f1= lab; _temp225->f2= 0;( void*) _temp225;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
e1, e2, loc), loc);} struct Cyc_Absyn_Pat* Cyc_Absyn_new_pat( void* p, struct
Cyc_Position_Segment* s){ return({ struct Cyc_Absyn_Pat* _temp226=( struct Cyc_Absyn_Pat*)
GC_malloc( sizeof( struct Cyc_Absyn_Pat)); _temp226->r=( void*) p; _temp226->topt=
0; _temp226->loc= s; _temp226;});} struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl(
void* r, struct Cyc_Position_Segment* loc){ return({ struct Cyc_Absyn_Decl*
_temp227=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp227->r=( void*) r; _temp227->loc= loc; _temp227;});} struct Cyc_Absyn_Decl*
Cyc_Absyn_let_decl( struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* t_opt, struct
Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Let_d_struct* _temp228=( struct Cyc_Absyn_Let_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Let_d_struct)); _temp228->tag= Cyc_Absyn_Let_d_tag;
_temp228->f1= p; _temp228->f2= 0; _temp228->f3= t_opt; _temp228->f4= e; _temp228->f5=
0;( void*) _temp228;}), loc);} struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl(
struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init){ return({ struct Cyc_Absyn_Vardecl*
_temp229=( struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp229->sc=( void*) Cyc_Absyn_Public; _temp229->name= x; _temp229->tq= Cyc_Absyn_empty_tqual();
_temp229->type=( void*) t; _temp229->initializer= init; _temp229->shadow= 0;
_temp229->region= 0; _temp229;});} struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl(
struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init){ return({ struct Cyc_Absyn_Vardecl*
_temp230=( struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp230->sc=( void*) Cyc_Absyn_Static; _temp230->name= x; _temp230->tq= Cyc_Absyn_empty_tqual();
_temp230->type=( void*) t; _temp230->initializer= init; _temp230->shadow= 0;
_temp230->region= 0; _temp230;});} struct Cyc_Absyn_Decl* Cyc_Absyn_struct_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Struct_d_struct*
_temp231=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp231->tag= Cyc_Absyn_Struct_d_tag; _temp231->f1=({ struct Cyc_Absyn_Structdecl*
_temp232=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp232->sc=( void*) s; _temp232->name= n; _temp232->tvs= ts; _temp232->fields=
fs; _temp232;});( void*) _temp231;}), loc);} struct Cyc_Absyn_Decl* Cyc_Absyn_enum_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Enum_d_struct*
_temp233=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp233->tag= Cyc_Absyn_Enum_d_tag; _temp233->f1=({ struct Cyc_Absyn_Enumdecl*
_temp234=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp234->sc=( void*) s; _temp234->name= n; _temp234->tvs= ts; _temp234->fields=
fs; _temp234;});( void*) _temp233;}), loc);} struct Cyc_Absyn_Decl* Cyc_Absyn_xenum_decl(
void* s, struct _tuple0* n, struct Cyc_List_List* fs, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Xenum_d_struct* _temp235=(
struct Cyc_Absyn_Xenum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Xenum_d_struct));
_temp235->tag= Cyc_Absyn_Xenum_d_tag; _temp235->f1=({ struct Cyc_Absyn_Xenumdecl*
_temp236=( struct Cyc_Absyn_Xenumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Xenumdecl));
_temp236->sc=( void*) s; _temp236->name= n; _temp236->fields= fs; _temp236;});(
void*) _temp235;}), loc);} static struct _tuple1* Cyc_Absyn_expand_arg( struct
_tuple1* a){ return({ struct _tuple1* _temp237=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp237->f1=(* a).f1; _temp237->f2=(* a).f2; _temp237->f3=
Cyc_Absyn_pointer_expand((* a).f3); _temp237;});} void* Cyc_Absyn_function_typ(
struct Cyc_List_List* tvs, struct Cyc_Core_Opt* eff_typ, void* ret_typ, struct
Cyc_List_List* args, int varargs){ return({ struct Cyc_Absyn_FnType_struct*
_temp238=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp238->tag= Cyc_Absyn_FnType_tag; _temp238->f1=({ struct Cyc_Absyn_FnInfo
_temp239; _temp239.tvars= tvs; _temp239.ret_typ=( void*) Cyc_Absyn_pointer_expand(
ret_typ); _temp239.effect= eff_typ; _temp239.args=(( struct Cyc_List_List*(*)(
struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absyn_expand_arg, args); _temp239.varargs= varargs; _temp239;});( void*)
_temp238;});} void* Cyc_Absyn_pointer_expand( void* t){ void* _temp240= t;
struct Cyc_Absyn_FnInfo _temp246; _LL242: if(( unsigned int) _temp240 > 5u?((
struct _enum_struct*) _temp240)->tag == Cyc_Absyn_FnType_tag: 0){ _LL247:
_temp246=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp240)->f1;
goto _LL243;} else{ goto _LL244;} _LL244: goto _LL245; _LL243: return Cyc_Absyn_at_typ(
t, Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); _LL245: return t; _LL241:;} int
Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp* e){ void* _temp248=( void*) e->r;
void* _temp268; struct Cyc_Absyn_Fndecl* _temp270; struct _tuple0* _temp272;
void* _temp274; struct Cyc_Absyn_Vardecl* _temp276; struct _tuple0* _temp278;
void* _temp280; struct Cyc_Absyn_Vardecl* _temp282; struct _tuple0* _temp284;
void* _temp286; struct _tuple0* _temp288; struct Cyc_Absyn_Exp* _temp290; struct
Cyc_Absyn_Exp* _temp292; struct _tagged_string* _temp294; struct Cyc_Absyn_Exp*
_temp296; struct _tagged_string* _temp298; struct Cyc_Absyn_Exp* _temp300;
struct Cyc_Absyn_Exp* _temp302; _LL250: if((( struct _enum_struct*) _temp248)->tag
== Cyc_Absyn_Var_e_tag){ _LL273: _temp272=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp248)->f1; goto _LL269; _LL269: _temp268=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp248)->f2; if(( unsigned int) _temp268 > 1u?(( struct _enum_struct*)
_temp268)->tag == Cyc_Absyn_Funname_b_tag: 0){ _LL271: _temp270=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp268)->f1; goto _LL251;} else{ goto
_LL252;}} else{ goto _LL252;} _LL252: if((( struct _enum_struct*) _temp248)->tag
== Cyc_Absyn_Var_e_tag){ _LL279: _temp278=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp248)->f1; goto _LL275; _LL275: _temp274=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp248)->f2; if(( unsigned int) _temp274 > 1u?(( struct _enum_struct*)
_temp274)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL277: _temp276=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp274)->f1; goto _LL253;} else{ goto
_LL254;}} else{ goto _LL254;} _LL254: if((( struct _enum_struct*) _temp248)->tag
== Cyc_Absyn_Var_e_tag){ _LL285: _temp284=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp248)->f1; goto _LL281; _LL281: _temp280=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp248)->f2; if(( unsigned int) _temp280 > 1u?(( struct _enum_struct*)
_temp280)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL283: _temp282=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp280)->f1; goto _LL255;} else{ goto _LL256;}}
else{ goto _LL256;} _LL256: if((( struct _enum_struct*) _temp248)->tag == Cyc_Absyn_Var_e_tag){
_LL289: _temp288=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp248)->f1;
goto _LL287; _LL287: _temp286=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp248)->f2; goto _LL257;} else{ goto _LL258;} _LL258: if((( struct
_enum_struct*) _temp248)->tag == Cyc_Absyn_Subscript_e_tag){ _LL293: _temp292=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp248)->f1;
goto _LL291; _LL291: _temp290=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp248)->f2; goto _LL259;} else{ goto _LL260;} _LL260: if((( struct
_enum_struct*) _temp248)->tag == Cyc_Absyn_StructMember_e_tag){ _LL297: _temp296=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp248)->f1;
goto _LL295; _LL295: _temp294=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp248)->f2; goto _LL261;} else{ goto _LL262;} _LL262: if((( struct
_enum_struct*) _temp248)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL301: _temp300=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp248)->f1;
goto _LL299; _LL299: _temp298=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp248)->f2; goto _LL263;} else{ goto _LL264;} _LL264: if((( struct
_enum_struct*) _temp248)->tag == Cyc_Absyn_Deref_e_tag){ _LL303: _temp302=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp248)->f1; goto
_LL265;} else{ goto _LL266;} _LL266: goto _LL267; _LL251: return 0; _LL253:
_temp282= _temp276; goto _LL255; _LL255: { void* _temp304= Cyc_Tcutil_compress((
void*) _temp282->type); struct Cyc_Absyn_Exp* _temp310; struct Cyc_Absyn_Tqual*
_temp312; void* _temp314; _LL306: if(( unsigned int) _temp304 > 5u?(( struct
_enum_struct*) _temp304)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL315: _temp314=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp304)->f1; goto _LL313; _LL313:
_temp312=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp304)->f2; goto _LL311; _LL311: _temp310=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp304)->f3; goto _LL307;} else{ goto _LL308;}
_LL308: goto _LL309; _LL307: return 0; _LL309: return 1; _LL305:;} _LL257:
return 1; _LL259: return 1; _LL261: return Cyc_Absyn_is_lvalue( _temp296);
_LL263: return 1; _LL265: return 1; _LL267: return 0; _LL249:;} struct _tuple3*
Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl* sd, struct
_tagged_string* v){ if( sd->fields == 0){ return 0;}{ struct Cyc_List_List* fs=(
struct Cyc_List_List*)( sd->fields)->v; for( 0; fs != 0; fs= fs->tl){ struct
_tuple3* f=( struct _tuple3*) fs->hd; if( Cyc_String_zstrptrcmp((* f).f1, v) ==
0){ return( struct _tuple3*) f;}}} return 0;} struct _tuple4* Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List* ts, int i){ for( 0; i != 0; -- i){ if( ts == 0){ return 0;}
ts= ts->tl;} if( ts == 0){ return 0;} return( struct _tuple4*)(( struct _tuple4*)
ts->hd);}