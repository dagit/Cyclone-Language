#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ struct
Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple4{ void* f1; void* f2; } ;
struct _tuple5{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; typedef
unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct _tagged_string
Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef struct
_tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit( int);
extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found_tag[ 10u]; struct
Cyc_Core_Not_found_struct{ char* tag; } ; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_List_List{ void*
hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[ 14u]; struct
Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern int Cyc_List_list_cmp( int(* cmp)( void*, void*), struct
Cyc_List_List* l1, struct Cyc_List_List* l2); struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
5u]; struct Cyc_Position_Exit_struct{ char* tag; } ; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; typedef void* Cyc_Position_Error_kind;
extern void* Cyc_Position_Lex; extern void* Cyc_Position_Parse; extern void* Cyc_Position_Elab;
typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag; } ; typedef struct _tagged_string* Cyc_Absyn_field_name; typedef
struct _tagged_string* Cyc_Absyn_var; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
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
typedef void* Cyc_Absyn_Attribute; struct Cyc_Absyn_Structfield; typedef void*
Cyc_Absyn_Mallocarg; typedef void* Cyc_Absyn_scope; typedef struct Cyc_Absyn_Tqual*
Cyc_Absyn_tqual; typedef void* Cyc_Absyn_size_of_t; typedef void* Cyc_Absyn_kind_t;
typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar; typedef void* Cyc_Absyn_sign_t;
typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref; typedef void* Cyc_Absyn_constraint_t;
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
Cyc_Absyn_stmt_annot_t; typedef void* Cyc_Absyn_attribute_t; typedef struct Cyc_List_List*
Cyc_Absyn_attributes_t; typedef struct Cyc_Absyn_Structfield* Cyc_Absyn_structfield_t;
typedef void* Cyc_Absyn_mallocarg_t; void* Cyc_Absyn_Loc_n=( void*) 0; const int
Cyc_Absyn_Rel_n_tag= 0; struct Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List*
f1; } ; const int Cyc_Absyn_Abs_n_tag= 1; struct Cyc_Absyn_Abs_n_struct{ int tag;
struct Cyc_List_List* f1; } ; void* Cyc_Absyn_Static=( void*) 0; void* Cyc_Absyn_Abstract=(
void*) 1; void* Cyc_Absyn_Public=( void*) 2; void* Cyc_Absyn_Extern=( void*) 3;
void* Cyc_Absyn_ExternC=( void*) 4; struct Cyc_Absyn_Tqual{ int q_const; int
q_volatile; int q_restrict; } ; void* Cyc_Absyn_B1=( void*) 0; void* Cyc_Absyn_B2=(
void*) 1; void* Cyc_Absyn_B4=( void*) 2; void* Cyc_Absyn_B8=( void*) 3; void*
Cyc_Absyn_AnyKind=( void*) 0; void* Cyc_Absyn_MemKind=( void*) 1; void* Cyc_Absyn_BoxKind=(
void*) 2; void* Cyc_Absyn_RgnKind=( void*) 3; void* Cyc_Absyn_EffKind=( void*) 4;
void* Cyc_Absyn_Signed=( void*) 0; void* Cyc_Absyn_Unsigned=( void*) 1; struct
Cyc_Absyn_Conref{ void* v; } ; const int Cyc_Absyn_Eq_constr_tag= 0; struct Cyc_Absyn_Eq_constr_struct{
int tag; void* f1; } ; const int Cyc_Absyn_Forward_constr_tag= 1; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1; } ; void* Cyc_Absyn_No_constr=( void*) 0;
struct Cyc_Absyn_Tvar{ struct _tagged_string* name; struct Cyc_Absyn_Conref*
kind; } ; void* Cyc_Absyn_Unknown_b=( void*) 0; const int Cyc_Absyn_Upper_b_tag=
0; struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual* tq; struct Cyc_Absyn_Conref* bounds; } ;
struct Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt*
effect; void* ret_typ; struct Cyc_List_List* args; int varargs; struct Cyc_List_List*
attributes; } ; void* Cyc_Absyn_VoidType=( void*) 0; const int Cyc_Absyn_Evar_tag=
0; struct Cyc_Absyn_Evar_struct{ int tag; void* f1; struct Cyc_Core_Opt* f2; int
f3; } ; const int Cyc_Absyn_VarType_tag= 1; struct Cyc_Absyn_VarType_struct{ int
tag; struct Cyc_Absyn_Tvar* f1; } ; const int Cyc_Absyn_EnumType_tag= 2; struct
Cyc_Absyn_EnumType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List* f2;
struct Cyc_Absyn_Enumdecl** f3; } ; const int Cyc_Absyn_XenumType_tag= 3; struct
Cyc_Absyn_XenumType_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Xenumdecl**
f2; } ; const int Cyc_Absyn_PointerType_tag= 4; struct Cyc_Absyn_PointerType_struct{
int tag; struct Cyc_Absyn_PtrInfo f1; } ; const int Cyc_Absyn_IntType_tag= 5;
struct Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2; } ; void* Cyc_Absyn_FloatType=(
void*) 1; void* Cyc_Absyn_DoubleType=( void*) 2; const int Cyc_Absyn_ArrayType_tag=
6; struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual*
f2; struct Cyc_Absyn_Exp* f3; } ; const int Cyc_Absyn_FnType_tag= 7; struct Cyc_Absyn_FnType_struct{
int tag; struct Cyc_Absyn_FnInfo f1; } ; const int Cyc_Absyn_TupleType_tag= 8;
struct Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; const
int Cyc_Absyn_StructType_tag= 9; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; void* Cyc_Absyn_UnionType=( void*) 3; const int Cyc_Absyn_RgnHandleType_tag=
10; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; const int Cyc_Absyn_TypedefType_tag=
11; struct Cyc_Absyn_TypedefType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Core_Opt* f3; } ; void* Cyc_Absyn_HeapRgn=( void*) 4; const int
Cyc_Absyn_AccessEff_tag= 12; struct Cyc_Absyn_AccessEff_struct{ int tag; void*
f1; } ; const int Cyc_Absyn_JoinEff_tag= 13; struct Cyc_Absyn_JoinEff_struct{
int tag; struct Cyc_List_List* f1; } ; const int Cyc_Absyn_NoTypes_tag= 0;
struct Cyc_Absyn_NoTypes_struct{ int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment*
f2; } ; const int Cyc_Absyn_WithTypes_tag= 1; struct Cyc_Absyn_WithTypes_struct{
int tag; struct Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3; } ; typedef
void* Cyc_Absyn_Pointer_Sort; const int Cyc_Absyn_NonNullable_ps_tag= 0; struct
Cyc_Absyn_NonNullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; const
int Cyc_Absyn_Nullable_ps_tag= 1; struct Cyc_Absyn_Nullable_ps_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; void* Cyc_Absyn_TaggedArray_ps=( void*) 0; const
int Cyc_Absyn_Regparm_att_tag= 0; struct Cyc_Absyn_Regparm_att_struct{ int tag;
int f1; } ; void* Cyc_Absyn_Stdcall_att=( void*) 0; void* Cyc_Absyn_Cdecl_att=(
void*) 1; void* Cyc_Absyn_Noreturn_att=( void*) 2; void* Cyc_Absyn_Const_att=(
void*) 3; const int Cyc_Absyn_Aligned_att_tag= 1; struct Cyc_Absyn_Aligned_att_struct{
int tag; int f1; } ; void* Cyc_Absyn_Packed_att=( void*) 4; const int Cyc_Absyn_Section_att_tag=
2; struct Cyc_Absyn_Section_att_struct{ int tag; struct _tagged_string f1; } ;
void* Cyc_Absyn_Nocommon_att=( void*) 5; void* Cyc_Absyn_Shared_att=( void*) 6;
void* Cyc_Absyn_Unused_att=( void*) 7; void* Cyc_Absyn_Weak_att=( void*) 8; void*
Cyc_Absyn_Dllimport_att=( void*) 9; void* Cyc_Absyn_Dllexport_att=( void*) 10;
void* Cyc_Absyn_No_instrument_function_att=( void*) 11; void* Cyc_Absyn_Constructor_att=(
void*) 12; void* Cyc_Absyn_Destructor_att=( void*) 13; void* Cyc_Absyn_No_check_memory_usage_att=(
void*) 14; void* Cyc_Absyn_Carray_mod=( void*) 0; const int Cyc_Absyn_ConstArray_mod_tag=
0; struct Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; }
; const int Cyc_Absyn_Pointer_mod_tag= 1; struct Cyc_Absyn_Pointer_mod_struct{
int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual* f3; } ; const int Cyc_Absyn_Function_mod_tag=
2; struct Cyc_Absyn_Function_mod_struct{ int tag; void* f1; } ; const int Cyc_Absyn_TypeParams_mod_tag=
3; struct Cyc_Absyn_TypeParams_mod_struct{ int tag; struct Cyc_List_List* f1;
struct Cyc_Position_Segment* f2; int f3; } ; const int Cyc_Absyn_Attributes_mod_tag=
4; struct Cyc_Absyn_Attributes_mod_struct{ int tag; struct Cyc_Position_Segment*
f1; struct Cyc_List_List* f2; } ; const int Cyc_Absyn_Char_c_tag= 0; struct Cyc_Absyn_Char_c_struct{
int tag; void* f1; char f2; } ; const int Cyc_Absyn_Short_c_tag= 1; struct Cyc_Absyn_Short_c_struct{
int tag; void* f1; short f2; } ; const int Cyc_Absyn_Int_c_tag= 2; struct Cyc_Absyn_Int_c_struct{
int tag; void* f1; int f2; } ; const int Cyc_Absyn_LongLong_c_tag= 3; struct Cyc_Absyn_LongLong_c_struct{
int tag; void* f1; long long f2; } ; const int Cyc_Absyn_Float_c_tag= 4; struct
Cyc_Absyn_Float_c_struct{ int tag; struct _tagged_string f1; } ; const int Cyc_Absyn_String_c_tag=
5; struct Cyc_Absyn_String_c_struct{ int tag; int f1; struct _tagged_string f2;
} ; void* Cyc_Absyn_Null_c=( void*) 0; void* Cyc_Absyn_Plus=( void*) 0; void*
Cyc_Absyn_Times=( void*) 1; void* Cyc_Absyn_Minus=( void*) 2; void* Cyc_Absyn_Div=(
void*) 3; void* Cyc_Absyn_Mod=( void*) 4; void* Cyc_Absyn_Eq=( void*) 5; void*
Cyc_Absyn_Neq=( void*) 6; void* Cyc_Absyn_Gt=( void*) 7; void* Cyc_Absyn_Lt=(
void*) 8; void* Cyc_Absyn_Gte=( void*) 9; void* Cyc_Absyn_Lte=( void*) 10; void*
Cyc_Absyn_Not=( void*) 11; void* Cyc_Absyn_Bitnot=( void*) 12; void* Cyc_Absyn_Bitand=(
void*) 13; void* Cyc_Absyn_Bitor=( void*) 14; void* Cyc_Absyn_Bitxor=( void*) 15;
void* Cyc_Absyn_Bitlshift=( void*) 16; void* Cyc_Absyn_Bitlrshift=( void*) 17;
void* Cyc_Absyn_Bitarshift=( void*) 18; void* Cyc_Absyn_Size=( void*) 19; void*
Cyc_Absyn_Printf=( void*) 20; void* Cyc_Absyn_Fprintf=( void*) 21; void* Cyc_Absyn_Xprintf=(
void*) 22; void* Cyc_Absyn_Scanf=( void*) 23; void* Cyc_Absyn_Fscanf=( void*) 24;
void* Cyc_Absyn_Sscanf=( void*) 25; void* Cyc_Absyn_PreInc=( void*) 0; void* Cyc_Absyn_PostInc=(
void*) 1; void* Cyc_Absyn_PreDec=( void*) 2; void* Cyc_Absyn_PostDec=( void*) 3;
const int Cyc_Absyn_Typ_m_tag= 0; struct Cyc_Absyn_Typ_m_struct{ int tag; void*
f1; } ; const int Cyc_Absyn_Unresolved_m_tag= 1; struct Cyc_Absyn_Unresolved_m_struct{
int tag; struct _tuple0* f1; } ; const int Cyc_Absyn_Enum_m_tag= 2; struct Cyc_Absyn_Enum_m_struct{
int tag; struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ;
const int Cyc_Absyn_Xenum_m_tag= 3; struct Cyc_Absyn_Xenum_m_struct{ int tag;
struct Cyc_Absyn_Xenumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; const int
Cyc_Absyn_Const_e_tag= 0; struct Cyc_Absyn_Const_e_struct{ int tag; void* f1; }
; const int Cyc_Absyn_Var_e_tag= 1; struct Cyc_Absyn_Var_e_struct{ int tag;
struct _tuple0* f1; void* f2; } ; const int Cyc_Absyn_UnknownId_e_tag= 2; struct
Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple0* f1; } ; const int Cyc_Absyn_Primop_e_tag=
3; struct Cyc_Absyn_Primop_e_struct{ int tag; void* f1; struct Cyc_List_List* f2;
} ; const int Cyc_Absyn_AssignOp_e_tag= 4; struct Cyc_Absyn_AssignOp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp*
f3; } ; const int Cyc_Absyn_Increment_e_tag= 5; struct Cyc_Absyn_Increment_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; const int Cyc_Absyn_Conditional_e_tag=
6; struct Cyc_Absyn_Conditional_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; const int Cyc_Absyn_SeqExp_e_tag=
7; struct Cyc_Absyn_SeqExp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Exp* f2; } ; const int Cyc_Absyn_UnknownCall_e_tag= 8; struct Cyc_Absyn_UnknownCall_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; const int Cyc_Absyn_FnCall_e_tag=
9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_List_List* f2; } ; const int Cyc_Absyn_Throw_e_tag= 10; struct Cyc_Absyn_Throw_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; const int Cyc_Absyn_NoInstantiate_e_tag=
11; struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
} ; const int Cyc_Absyn_Instantiate_e_tag= 12; struct Cyc_Absyn_Instantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; const int Cyc_Absyn_Cast_e_tag=
13; struct Cyc_Absyn_Cast_e_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2;
} ; const int Cyc_Absyn_Address_e_tag= 14; struct Cyc_Absyn_Address_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; const int Cyc_Absyn_Sizeof_e_tag= 15;
struct Cyc_Absyn_Sizeof_e_struct{ int tag; void* f1; } ; const int Cyc_Absyn_Deref_e_tag=
16; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
const int Cyc_Absyn_StructMember_e_tag= 17; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; const int Cyc_Absyn_StructArrow_e_tag=
18; struct Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_string* f2; } ; const int Cyc_Absyn_Subscript_e_tag= 19; struct
Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp*
f2; } ; const int Cyc_Absyn_Tuple_e_tag= 20; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1; } ; const int Cyc_Absyn_CompoundLit_e_tag= 21;
struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; } ; const int Cyc_Absyn_Array_e_tag= 22; struct Cyc_Absyn_Array_e_struct{
int tag; int f1; struct Cyc_List_List* f2; } ; const int Cyc_Absyn_Comprehension_e_tag=
23; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; const int Cyc_Absyn_Struct_e_tag=
24; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; const int Cyc_Absyn_Enum_e_tag=
25; struct Cyc_Absyn_Enum_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Enumdecl* f4; struct Cyc_Absyn_Enumfield*
f5; } ; const int Cyc_Absyn_Xenum_e_tag= 26; struct Cyc_Absyn_Xenum_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Xenumdecl*
f3; struct Cyc_Absyn_Enumfield* f4; } ; const int Cyc_Absyn_Malloc_e_tag= 27;
struct Cyc_Absyn_Malloc_e_struct{ int tag; void* f1; } ; const int Cyc_Absyn_UnresolvedMem_e_tag=
28; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; const int Cyc_Absyn_StmtExp_e_tag= 29; struct Cyc_Absyn_StmtExp_e_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; const int Cyc_Absyn_Codegen_e_tag= 30;
struct Cyc_Absyn_Codegen_e_struct{ int tag; struct Cyc_Absyn_Fndecl* f1; } ;
const int Cyc_Absyn_Fill_e_tag= 31; struct Cyc_Absyn_Fill_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt* topt;
void* r; struct Cyc_Position_Segment* loc; } ; void* Cyc_Absyn_Skip_s=( void*) 0;
const int Cyc_Absyn_Exp_s_tag= 0; struct Cyc_Absyn_Exp_s_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; const int Cyc_Absyn_Seq_s_tag= 1; struct Cyc_Absyn_Seq_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; const int Cyc_Absyn_Return_s_tag=
2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
const int Cyc_Absyn_IfThenElse_s_tag= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; const int Cyc_Absyn_While_s_tag= 4; struct Cyc_Absyn_While_s_struct{ int
tag; struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; const int Cyc_Absyn_Break_s_tag=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
const int Cyc_Absyn_Continue_s_tag= 6; struct Cyc_Absyn_Continue_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; } ; const int Cyc_Absyn_Goto_s_tag= 7; struct
Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
f2; } ; const int Cyc_Absyn_For_s_tag= 8; struct Cyc_Absyn_For_s_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt*
f4; } ; const int Cyc_Absyn_Switch_s_tag= 9; struct Cyc_Absyn_Switch_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; const int Cyc_Absyn_Fallthru_s_tag=
10; struct Cyc_Absyn_Fallthru_s_struct{ int tag; struct Cyc_List_List* f1;
struct Cyc_Absyn_Switch_clause** f2; } ; const int Cyc_Absyn_Decl_s_tag= 11;
struct Cyc_Absyn_Decl_s_struct{ int tag; struct Cyc_Absyn_Decl* f1; struct Cyc_Absyn_Stmt*
f2; } ; const int Cyc_Absyn_Cut_s_tag= 12; struct Cyc_Absyn_Cut_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; } ; const int Cyc_Absyn_Splice_s_tag= 13; struct
Cyc_Absyn_Splice_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; const int Cyc_Absyn_Label_s_tag=
14; struct Cyc_Absyn_Label_s_struct{ int tag; struct _tagged_string* f1; struct
Cyc_Absyn_Stmt* f2; } ; const int Cyc_Absyn_Do_s_tag= 15; struct Cyc_Absyn_Do_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct _tuple2 f2; } ; const int Cyc_Absyn_TryCatch_s_tag=
16; struct Cyc_Absyn_TryCatch_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1;
struct Cyc_List_List* f2; } ; struct Cyc_Absyn_Stmt{ void* r; struct Cyc_Position_Segment*
loc; struct Cyc_List_List* non_local_preds; int try_depth; struct _xenum_struct*
annot; } ; void* Cyc_Absyn_Wild_p=( void*) 0; const int Cyc_Absyn_Var_p_tag= 0;
struct Cyc_Absyn_Var_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; void*
Cyc_Absyn_Null_p=( void*) 1; const int Cyc_Absyn_Int_p_tag= 1; struct Cyc_Absyn_Int_p_struct{
int tag; void* f1; int f2; } ; const int Cyc_Absyn_Char_p_tag= 2; struct Cyc_Absyn_Char_p_struct{
int tag; char f1; } ; const int Cyc_Absyn_Float_p_tag= 3; struct Cyc_Absyn_Float_p_struct{
int tag; struct _tagged_string f1; } ; const int Cyc_Absyn_Tuple_p_tag= 4;
struct Cyc_Absyn_Tuple_p_struct{ int tag; struct Cyc_List_List* f1; } ; const
int Cyc_Absyn_Pointer_p_tag= 5; struct Cyc_Absyn_Pointer_p_struct{ int tag;
struct Cyc_Absyn_Pat* f1; } ; const int Cyc_Absyn_Reference_p_tag= 6; struct Cyc_Absyn_Reference_p_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; const int Cyc_Absyn_Struct_p_tag= 7;
struct Cyc_Absyn_Struct_p_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;
struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; } ;
const int Cyc_Absyn_Enum_p_tag= 8; struct Cyc_Absyn_Enum_p_struct{ int tag;
struct _tuple0* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3; struct
Cyc_List_List* f4; struct Cyc_Absyn_Enumdecl* f5; struct Cyc_Absyn_Enumfield* f6;
} ; const int Cyc_Absyn_Xenum_p_tag= 9; struct Cyc_Absyn_Xenum_p_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; struct
Cyc_Absyn_Xenumdecl* f4; struct Cyc_Absyn_Enumfield* f5; } ; const int Cyc_Absyn_UnknownId_p_tag=
10; struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple0* f1; } ; const
int Cyc_Absyn_UnknownCall_p_tag= 11; struct Cyc_Absyn_UnknownCall_p_struct{ int
tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ;
const int Cyc_Absyn_UnknownFields_p_tag= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; struct Cyc_Absyn_Pat{ void* r; struct Cyc_Core_Opt* topt; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Switch_clause{ struct Cyc_Absyn_Pat* pattern; struct
Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp* where_clause; struct Cyc_Absyn_Stmt*
body; struct Cyc_Position_Segment* loc; } ; void* Cyc_Absyn_Unresolved_b=( void*)
0; const int Cyc_Absyn_Global_b_tag= 0; struct Cyc_Absyn_Global_b_struct{ int
tag; struct Cyc_Absyn_Vardecl* f1; } ; const int Cyc_Absyn_Funname_b_tag= 1;
struct Cyc_Absyn_Funname_b_struct{ int tag; struct Cyc_Absyn_Fndecl* f1; } ;
const int Cyc_Absyn_Param_b_tag= 2; struct Cyc_Absyn_Param_b_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; const int Cyc_Absyn_Local_b_tag= 3; struct Cyc_Absyn_Local_b_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; const int Cyc_Absyn_Pat_b_tag= 4;
struct Cyc_Absyn_Pat_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; struct
Cyc_Absyn_Vardecl{ void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual* tq;
void* type; struct Cyc_Absyn_Exp* initializer; int shadow; struct Cyc_Core_Opt*
region; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc;
int is_inline; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt*
body; struct Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structfield{ struct
_tagged_string* name; struct Cyc_Absyn_Tqual* tq; void* type; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Enumfield{ struct _tuple0* name; struct Cyc_Absyn_Exp*
tag; struct Cyc_List_List* tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct Cyc_Core_Opt* name; struct
Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Xenumdecl{
void* sc; struct _tuple0* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple0* name; struct Cyc_List_List* tvs; void* defn; } ; const int Cyc_Absyn_Var_d_tag=
0; struct Cyc_Absyn_Var_d_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
const int Cyc_Absyn_Fn_d_tag= 1; struct Cyc_Absyn_Fn_d_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; const int Cyc_Absyn_Let_d_tag= 2; struct Cyc_Absyn_Let_d_struct{
int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt* f2; struct Cyc_Core_Opt*
f3; struct Cyc_Absyn_Exp* f4; int f5; } ; const int Cyc_Absyn_Struct_d_tag= 3;
struct Cyc_Absyn_Struct_d_struct{ int tag; struct Cyc_Absyn_Structdecl* f1; } ;
void* Cyc_Absyn_Union_d=( void*) 0; const int Cyc_Absyn_Enum_d_tag= 4; struct
Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; } ; const int
Cyc_Absyn_Xenum_d_tag= 5; struct Cyc_Absyn_Xenum_d_struct{ int tag; struct Cyc_Absyn_Xenumdecl*
f1; } ; const int Cyc_Absyn_Typedef_d_tag= 6; struct Cyc_Absyn_Typedef_d_struct{
int tag; struct Cyc_Absyn_Typedefdecl* f1; } ; const int Cyc_Absyn_Namespace_d_tag=
7; struct Cyc_Absyn_Namespace_d_struct{ int tag; struct _tagged_string* f1;
struct Cyc_List_List* f2; } ; const int Cyc_Absyn_Using_d_tag= 8; struct Cyc_Absyn_Using_d_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; } ; const int Cyc_Absyn_ExternC_d_tag=
9; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; const int
Cyc_Absyn_ArrayElement_tag= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; const int Cyc_Absyn_FieldName_tag= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_string* f1; } ; char Cyc_Absyn_EmptyAnnot_tag[ 11u]="EmptyAnnot";
struct Cyc_Absyn_EmptyAnnot_struct{ char* tag; } ; extern int Cyc_Absyn_qvar_cmp(
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
struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_enum_decl( void* s, struct Cyc_Core_Opt*
n, struct Cyc_List_List* ts, struct Cyc_Core_Opt* fs, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_xenum_decl( void* s, struct
_tuple0* n, struct Cyc_List_List* fs, struct Cyc_Position_Segment* loc); extern
int Cyc_Absyn_is_format_prim( void* p); extern void* Cyc_Absyn_function_typ(
struct Cyc_List_List* tvs, struct Cyc_Core_Opt* eff_typ, void* ret_typ, struct
Cyc_List_List* args, int varargs, struct Cyc_List_List*); extern void* Cyc_Absyn_pointer_expand(
void*); extern int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*); extern struct
Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl*,
struct _tagged_string*); extern struct _tuple3* Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*, int); extern struct _tagged_string Cyc_Absyn_attribute2string(
void*); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t; extern char
Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[ 15u];
struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); struct Cyc_Set_Set; typedef
struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern char Cyc_Set_Absent_tag[ 7u];
struct Cyc_Set_Absent_struct{ char* tag; } ; struct Cyc_Dict_Dict; typedef
struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;
} ; extern char Cyc_Dict_Absent_tag[ 7u]; typedef void* Cyc_Tcenv_Resolved;
extern const int Cyc_Tcenv_VarRes_tag; struct Cyc_Tcenv_VarRes_struct{ int tag;
void* f1; } ; extern const int Cyc_Tcenv_StructRes_tag; struct Cyc_Tcenv_StructRes_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; extern const int Cyc_Tcenv_EnumRes_tag;
struct Cyc_Tcenv_EnumRes_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; struct
Cyc_Absyn_Enumfield* f2; } ; extern const int Cyc_Tcenv_XenumRes_tag; struct Cyc_Tcenv_XenumRes_struct{
int tag; struct Cyc_Absyn_Xenumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ;
typedef void* Cyc_Tcenv_resolved_t; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set*
namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* xenumdecls; struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict*
ordinaries; struct Cyc_List_List* availables; } ; typedef struct Cyc_Tcenv_Genv*
Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv; typedef struct Cyc_Tcenv_Fenv* Cyc_Tcenv_fenv_t;
typedef void* Cyc_Tcenv_Jumpee; extern void* Cyc_Tcenv_NotLoop_j; extern void*
Cyc_Tcenv_CaseEnd_j; extern void* Cyc_Tcenv_FnEnd_j; extern const int Cyc_Tcenv_Stmt_j_tag;
struct Cyc_Tcenv_Stmt_j_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; typedef
void* Cyc_Tcenv_jumpee_t; typedef void* Cyc_Tcenv_Frames; extern const int Cyc_Tcenv_Outermost_tag;
struct Cyc_Tcenv_Outermost_struct{ int tag; void* f1; } ; extern const int Cyc_Tcenv_Frame_tag;
struct Cyc_Tcenv_Frame_struct{ int tag; void* f1; void* f2; } ; extern const int
Cyc_Tcenv_Hidden_tag; struct Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void*
f2; } ; typedef void* Cyc_Tcenv_frames; struct Cyc_Tcenv_Tenv{ struct Cyc_List_List*
ns; struct Cyc_Dict_Dict* ae; struct Cyc_Core_Opt* le; } ; typedef struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tenv; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern char Cyc_Tcutil_TypeErr_tag[
8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag; } ; extern void* Cyc_Tcutil_impos(
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); static int
Cyc_Absyn_zstrlist_cmp( struct Cyc_List_List* ss1, struct Cyc_List_List* ss2){
return(( int(*)( int(* cmp)( struct _tagged_string*, struct _tagged_string*),
struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_String_zstrptrcmp,
ss1, ss2);} int Cyc_Absyn_varlist_cmp( struct Cyc_List_List* vs1, struct Cyc_List_List*
vs2){ return Cyc_Absyn_zstrlist_cmp( vs1, vs2);} int Cyc_Absyn_qvar_cmp( struct
_tuple0* q1, struct _tuple0* q2){ void* n1=(* q1).f1; void* n2=(* q2).f1;{
struct _tuple4 _temp1=({ struct _tuple4 _temp0; _temp0.f1= n1; _temp0.f2= n2;
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
_temp78;});} static struct Cyc_Absyn_Eq_constr_struct _temp79={ 0u,( void*) 1};
struct Cyc_Absyn_Conref Cyc_Absyn_true_conref_v=( struct Cyc_Absyn_Conref){.v=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*)& _temp79)}; static struct Cyc_Absyn_Eq_constr_struct
_temp80={ 0u,( void*) 0}; struct Cyc_Absyn_Conref Cyc_Absyn_false_conref_v=(
struct Cyc_Absyn_Conref){.v=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)&
_temp80)}; struct Cyc_Absyn_Conref* Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v;
struct Cyc_Absyn_Conref* Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v;
struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x){
void* _temp81=( void*) x->v; void* _temp89; struct Cyc_Absyn_Conref* _temp91;
struct Cyc_Absyn_Conref** _temp93; _LL83: if( _temp81 == Cyc_Absyn_No_constr){
goto _LL84;} else{ goto _LL85;} _LL85: if(( unsigned int) _temp81 > 1u?(( struct
_enum_struct*) _temp81)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL90: _temp89=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp81)->f1; goto _LL86;} else{
goto _LL87;} _LL87: if(( unsigned int) _temp81 > 1u?(( struct _enum_struct*)
_temp81)->tag == Cyc_Absyn_Forward_constr_tag: 0){ _LL92: _temp91=( struct Cyc_Absyn_Conref*)((
struct Cyc_Absyn_Forward_constr_struct*) _temp81)->f1; _temp93=&(( struct Cyc_Absyn_Forward_constr_struct*)
_temp81)->f1; goto _LL88;} else{ goto _LL82;} _LL84: return x; _LL86: return x;
_LL88: { struct Cyc_Absyn_Conref* z=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)(* _temp93);* _temp93= z; return z;} _LL82:;} void*
Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x){ void* _temp94=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
x))->v; void* _temp100; _LL96: if(( unsigned int) _temp94 > 1u?(( struct
_enum_struct*) _temp94)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL101: _temp100=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp94)->f1; goto _LL97;} else{
goto _LL98;} _LL98: goto _LL99; _LL97: return _temp100; _LL99: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp102=( char*)"conref_val"; struct _tagged_string _temp103; _temp103.curr=
_temp102; _temp103.base= _temp102; _temp103.last_plus_one= _temp102 + 11;
_temp103;})); _LL95:;} static struct Cyc_Absyn_IntType_struct _temp104={ 5u,(
void*)(( void*) 1u),( void*)(( void*) 0u)}; void* Cyc_Absyn_uchar_t=( struct Cyc_Absyn_IntType_struct*)&
_temp104; static struct Cyc_Absyn_IntType_struct _temp105={ 5u,( void*)(( void*)
1u),( void*)(( void*) 1u)}; void* Cyc_Absyn_ushort_t=( struct Cyc_Absyn_IntType_struct*)&
_temp105; static struct Cyc_Absyn_IntType_struct _temp106={ 5u,( void*)(( void*)
1u),( void*)(( void*) 2u)}; void* Cyc_Absyn_uint_t=( struct Cyc_Absyn_IntType_struct*)&
_temp106; static struct Cyc_Absyn_IntType_struct _temp107={ 5u,( void*)(( void*)
1u),( void*)(( void*) 3u)}; void* Cyc_Absyn_ulong_t=( struct Cyc_Absyn_IntType_struct*)&
_temp107; static struct Cyc_Absyn_IntType_struct _temp108={ 5u,( void*)(( void*)
0u),( void*)(( void*) 0u)}; void* Cyc_Absyn_schar_t=( struct Cyc_Absyn_IntType_struct*)&
_temp108; static struct Cyc_Absyn_IntType_struct _temp109={ 5u,( void*)(( void*)
0u),( void*)(( void*) 1u)}; void* Cyc_Absyn_sshort_t=( struct Cyc_Absyn_IntType_struct*)&
_temp109; static struct Cyc_Absyn_IntType_struct _temp110={ 5u,( void*)(( void*)
0u),( void*)(( void*) 2u)}; void* Cyc_Absyn_sint_t=( struct Cyc_Absyn_IntType_struct*)&
_temp110; static struct Cyc_Absyn_IntType_struct _temp111={ 5u,( void*)(( void*)
0u),( void*)(( void*) 3u)}; void* Cyc_Absyn_slong_t=( struct Cyc_Absyn_IntType_struct*)&
_temp111; void* Cyc_Absyn_float_t=( void*) 1u; void* Cyc_Absyn_double_t=( void*)
2u; static char _temp114[ 4u]="exn"; static struct _tagged_string Cyc_Absyn_exn_string=(
struct _tagged_string){ _temp114, _temp114, _temp114 + 4u}; static struct Cyc_Absyn_Abs_n_struct
_temp115={ 1u, 0}; static struct _tuple0 Cyc_Absyn_exn_name_v=( struct _tuple0){.f1=(
struct Cyc_Absyn_Abs_n_struct*)& _temp115,.f2=& Cyc_Absyn_exn_string}; struct
_tuple0* Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v; static struct Cyc_Absyn_Xenumdecl
Cyc_Absyn_exn_xed_v=( struct Cyc_Absyn_Xenumdecl){.sc=( void*)(( void*) 3u),.name=&
Cyc_Absyn_exn_name_v,.fields= 0}; struct Cyc_Absyn_Xenumdecl* Cyc_Absyn_exn_xed=&
Cyc_Absyn_exn_xed_v; static struct Cyc_Absyn_XenumType_struct _temp116={ 3u,&
Cyc_Absyn_exn_name_v,( struct Cyc_Absyn_Xenumdecl**)& Cyc_Absyn_exn_xed}; void*
Cyc_Absyn_exn_typ=( struct Cyc_Absyn_XenumType_struct*)& _temp116; static struct
Cyc_Core_Opt* Cyc_Absyn_string_t_opt= 0; void* Cyc_Absyn_string_typ( void* rgn){
if( Cyc_Absyn_string_t_opt == 0){ void* t= Cyc_Absyn_starb_typ( Cyc_Absyn_uchar_t,
rgn, Cyc_Absyn_empty_tqual(), Cyc_Absyn_Unknown_b); Cyc_Absyn_string_t_opt=({
struct Cyc_Core_Opt* _temp117=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp117->v=( void*) t; _temp117;});} return( void*) Cyc_Absyn_string_t_opt->v;}
void* Cyc_Absyn_starb_typ( void* t, void* r, struct Cyc_Absyn_Tqual* tq, void* b){
return({ struct Cyc_Absyn_PointerType_struct* _temp118=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp118->tag= Cyc_Absyn_PointerType_tag;
_temp118->f1=({ struct Cyc_Absyn_PtrInfo _temp119; _temp119.elt_typ=( void*) t;
_temp119.rgn_typ=( void*) r; _temp119.nullable= Cyc_Absyn_true_conref; _temp119.tq=
tq; _temp119.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
b); _temp119;});( void*) _temp118;});} void* Cyc_Absyn_atb_typ( void* t, void* r,
struct Cyc_Absyn_Tqual* tq, void* b){ return({ struct Cyc_Absyn_PointerType_struct*
_temp120=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp120->tag= Cyc_Absyn_PointerType_tag; _temp120->f1=({ struct Cyc_Absyn_PtrInfo
_temp121; _temp121.elt_typ=( void*) t; _temp121.rgn_typ=( void*) r; _temp121.nullable=
Cyc_Absyn_false_conref; _temp121.tq= tq; _temp121.bounds=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)( b); _temp121;});( void*) _temp120;});} void*
Cyc_Absyn_star_typ( void* t, void* r, struct Cyc_Absyn_Tqual* tq){ return Cyc_Absyn_starb_typ(
t, r, tq,({ struct Cyc_Absyn_Upper_b_struct* _temp122=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp122->tag= Cyc_Absyn_Upper_b_tag;
_temp122->f1= Cyc_Absyn_signed_int_exp( 1, 0);( void*) _temp122;}));} void* Cyc_Absyn_cstar_typ(
void* t, struct Cyc_Absyn_Tqual* tq){ return Cyc_Absyn_starb_typ( t, Cyc_Absyn_HeapRgn,
tq,({ struct Cyc_Absyn_Upper_b_struct* _temp123=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp123->tag= Cyc_Absyn_Upper_b_tag;
_temp123->f1= Cyc_Absyn_signed_int_exp( 1, 0);( void*) _temp123;}));} void* Cyc_Absyn_at_typ(
void* t, void* r, struct Cyc_Absyn_Tqual* tq){ return Cyc_Absyn_atb_typ( t, r,
tq,({ struct Cyc_Absyn_Upper_b_struct* _temp124=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp124->tag= Cyc_Absyn_Upper_b_tag;
_temp124->f1= Cyc_Absyn_signed_int_exp( 1, 0);( void*) _temp124;}));} void* Cyc_Absyn_tagged_typ(
void* t, void* r, struct Cyc_Absyn_Tqual* tq){ return({ struct Cyc_Absyn_PointerType_struct*
_temp125=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp125->tag= Cyc_Absyn_PointerType_tag; _temp125->f1=({ struct Cyc_Absyn_PtrInfo
_temp126; _temp126.elt_typ=( void*) t; _temp126.rgn_typ=( void*) r; _temp126.nullable=
Cyc_Absyn_true_conref; _temp126.tq= tq; _temp126.bounds=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)( Cyc_Absyn_Unknown_b); _temp126;});( void*)
_temp125;});} static struct Cyc_Core_Opt* Cyc_Absyn_file_t_opt= 0; static char
_temp129[ 8u]="__sFILE"; static struct _tagged_string Cyc_Absyn_sf_str=( struct
_tagged_string){ _temp129, _temp129, _temp129 + 8u}; static struct
_tagged_string* Cyc_Absyn_sf=& Cyc_Absyn_sf_str; static char _temp132[ 4u]="Cyc";
static struct _tagged_string Cyc_Absyn_cyc_str=( struct _tagged_string){
_temp132, _temp132, _temp132 + 4u}; static struct _tagged_string* Cyc_Absyn_cyc=&
Cyc_Absyn_cyc_str; static char _temp135[ 6u]="Stdio"; static struct
_tagged_string Cyc_Absyn_st_str=( struct _tagged_string){ _temp135, _temp135,
_temp135 + 6u}; static struct _tagged_string* Cyc_Absyn_st=& Cyc_Absyn_st_str;
void* Cyc_Absyn_file_typ(){ if( Cyc_Absyn_file_t_opt == 0){ struct _tuple0*
file_t_name=({ struct _tuple0* _temp136=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0) * 1); _temp136[ 0]=({ struct _tuple0 _temp137; _temp137.f1=({
struct Cyc_Absyn_Abs_n_struct* _temp138=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp138->tag= Cyc_Absyn_Abs_n_tag;
_temp138->f1=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp139=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp139->hd=( void*)
Cyc_Absyn_cyc; _temp139->tl=({ struct Cyc_List_List* _temp140=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp140->hd=( void*) Cyc_Absyn_st;
_temp140->tl= 0; _temp140;}); _temp139;});( void*) _temp138;}); _temp137.f2= Cyc_Absyn_sf;
_temp137;}); _temp136;}); struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl*
_temp141=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp141->sc=( void*) Cyc_Absyn_Abstract; _temp141->name=({ struct Cyc_Core_Opt*
_temp142=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp142->v=( void*) file_t_name; _temp142;}); _temp141->tvs= 0; _temp141->fields=
0; _temp141->attributes= 0; _temp141;}); void* file_struct_typ=({ struct Cyc_Absyn_StructType_struct*
_temp143=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp143->tag= Cyc_Absyn_StructType_tag; _temp143->f1=( struct _tuple0*)
file_t_name; _temp143->f2= 0; _temp143->f3=( struct Cyc_Absyn_Structdecl**)({
struct Cyc_Absyn_Structdecl** _temp144=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*) * 1); _temp144[ 0]= sd;
_temp144;});( void*) _temp143;}); Cyc_Absyn_file_t_opt=({ struct Cyc_Core_Opt*
_temp145=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp145->v=( void*) Cyc_Absyn_at_typ( file_struct_typ, Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
_temp145;});} return( void*) Cyc_Absyn_file_t_opt->v;} static struct Cyc_Core_Opt*
Cyc_Absyn_void_star_t_opt= 0; void* Cyc_Absyn_void_star_typ(){ if( Cyc_Absyn_void_star_t_opt
== 0){ Cyc_Absyn_void_star_t_opt=({ struct Cyc_Core_Opt* _temp146=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp146->v=( void*) Cyc_Absyn_star_typ(
Cyc_Absyn_VoidType, Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); _temp146;});}
return( void*) Cyc_Absyn_void_star_t_opt->v;} void* Cyc_Absyn_strct( struct
_tagged_string* name){ return({ struct Cyc_Absyn_StructType_struct* _temp147=(
struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp147->tag= Cyc_Absyn_StructType_tag; _temp147->f1=( struct _tuple0*)({
struct _tuple0* _temp148=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0) *
1); _temp148[ 0]=({ struct _tuple0 _temp149; _temp149.f1=({ struct Cyc_Absyn_Abs_n_struct*
_temp150=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp150->tag= Cyc_Absyn_Abs_n_tag; _temp150->f1= 0;( void*) _temp150;});
_temp149.f2= name; _temp149;}); _temp148;}); _temp147->f2= 0; _temp147->f3= 0;(
void*) _temp147;});} void* Cyc_Absyn_strctq( struct _tuple0* name){ return({
struct Cyc_Absyn_StructType_struct* _temp151=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp151->tag= Cyc_Absyn_StructType_tag;
_temp151->f1=( struct _tuple0*) name; _temp151->f2= 0; _temp151->f3= 0;( void*)
_temp151;});} struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void* r, struct Cyc_Position_Segment*
loc){ return({ struct Cyc_Absyn_Exp* _temp152=( struct Cyc_Absyn_Exp*) GC_malloc(
sizeof( struct Cyc_Absyn_Exp)); _temp152->topt= 0; _temp152->r=( void*) r;
_temp152->loc= loc; _temp152;});} struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp* e){ return({ struct Cyc_Absyn_Exp* _temp153=( struct Cyc_Absyn_Exp*)
GC_malloc( sizeof( struct Cyc_Absyn_Exp)); _temp153->topt= e->topt; _temp153->r=(
void*)(( void*) e->r); _temp153->loc= e->loc; _temp153;});} struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp( void* c, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Const_e_struct* _temp154=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp154->tag= Cyc_Absyn_Const_e_tag;
_temp154->f1=( void*) c;( void*) _temp154;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Const_e_struct*
_temp155=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp155->tag= Cyc_Absyn_Const_e_tag; _temp155->f1=( void*) Cyc_Absyn_Null_c;(
void*) _temp155;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void* s, int
i, struct Cyc_Position_Segment* seg){ return Cyc_Absyn_const_exp(({ struct Cyc_Absyn_Int_c_struct*
_temp156=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp156->tag= Cyc_Absyn_Int_c_tag; _temp156->f1=( void*) s; _temp156->f2= i;(
void*) _temp156;}), seg);} struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int i,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_int_exp( Cyc_Absyn_Signed, i,
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int i, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_int_exp( Cyc_Absyn_Unsigned,( int) i, loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_bool_exp( int b, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_signed_int_exp(
b? 1: 0, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_bool_exp( 1, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_bool_exp( 0, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_char_exp( char c, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_const_exp(({ struct Cyc_Absyn_Char_c_struct* _temp157=( struct
Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp157->tag= Cyc_Absyn_Char_c_tag; _temp157->f1=( void*) Cyc_Absyn_Unsigned;
_temp157->f2= c;( void*) _temp157;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp(
struct _tagged_string f, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_const_exp(({
struct Cyc_Absyn_Float_c_struct* _temp158=( struct Cyc_Absyn_Float_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp158->tag= Cyc_Absyn_Float_c_tag;
_temp158->f1= f;( void*) _temp158;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
int heap_allocate, struct _tagged_string s, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_const_exp(({ struct Cyc_Absyn_String_c_struct* _temp159=(
struct Cyc_Absyn_String_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct));
_temp159->tag= Cyc_Absyn_String_c_tag; _temp159->f1= heap_allocate; _temp159->f2=
s;( void*) _temp159;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_var_exp( struct
_tuple0* q, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Var_e_struct* _temp160=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp160->tag= Cyc_Absyn_Var_e_tag;
_temp160->f1= q; _temp160->f2=( void*) Cyc_Absyn_Unresolved_b;( void*) _temp160;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple0* q, void* b,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Var_e_struct*
_temp161=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp161->tag= Cyc_Absyn_Var_e_tag; _temp161->f1= q; _temp161->f2=( void*) b;(
void*) _temp161;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_unknownid_exp( struct
_tuple0* q, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnknownId_e_struct* _temp162=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp162->tag= Cyc_Absyn_UnknownId_e_tag;
_temp162->f1= q;( void*) _temp162;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp(
void* p, struct Cyc_List_List* es, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Primop_e_struct* _temp163=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp163->tag= Cyc_Absyn_Primop_e_tag;
_temp163->f1=( void*) p; _temp163->f2= es;( void*) _temp163;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp( void* p, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_primop_exp( p,({ struct Cyc_List_List* _temp164=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp164->hd=( void*)
e; _temp164->tl= 0; _temp164;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp(
void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_primop_exp( p,({ struct Cyc_List_List* _temp165=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp165->hd=( void*)
e1; _temp165->tl=({ struct Cyc_List_List* _temp166=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp166->hd=( void*) e2; _temp166->tl=
0; _temp166;}); _temp165;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_add_exp(
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
struct Cyc_Absyn_AssignOp_e_struct* _temp167=( struct Cyc_Absyn_AssignOp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AssignOp_e_struct)); _temp167->tag= Cyc_Absyn_AssignOp_e_tag;
_temp167->f1= e1; _temp167->f2= popt; _temp167->f3= e2;( void*) _temp167;}), loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_assignop_exp( e1, 0, e2,
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp* e,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Increment_e_struct*
_temp168=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp168->tag= Cyc_Absyn_Increment_e_tag; _temp168->f1= e; _temp168->f2=( void*)
Cyc_Absyn_PostInc;( void*) _temp168;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Increment_e_struct* _temp169=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp169->tag= Cyc_Absyn_Increment_e_tag;
_temp169->f1= e; _temp169->f2=( void*) Cyc_Absyn_PreInc;( void*) _temp169;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_pre_dec_exp( struct Cyc_Absyn_Exp* e,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Increment_e_struct*
_temp170=( struct Cyc_Absyn_Increment_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct));
_temp170->tag= Cyc_Absyn_Increment_e_tag; _temp170->f1= e; _temp170->f2=( void*)
Cyc_Absyn_PreDec;( void*) _temp170;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Increment_e_struct* _temp171=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp171->tag= Cyc_Absyn_Increment_e_tag;
_temp171->f1= e; _temp171->f2=( void*) Cyc_Absyn_PostDec;( void*) _temp171;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Conditional_e_struct*
_temp172=( struct Cyc_Absyn_Conditional_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Conditional_e_struct));
_temp172->tag= Cyc_Absyn_Conditional_e_tag; _temp172->f1= e1; _temp172->f2= e2;
_temp172->f3= e3;( void*) _temp172;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_conditional_exp( e1, e2, Cyc_Absyn_false_exp( loc), loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_conditional_exp( e1, Cyc_Absyn_true_exp(
loc), e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_SeqExp_e_struct* _temp173=( struct Cyc_Absyn_SeqExp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_SeqExp_e_struct)); _temp173->tag= Cyc_Absyn_SeqExp_e_tag;
_temp173->f1= e1; _temp173->f2= e2;( void*) _temp173;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp( struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_UnknownCall_e_struct*
_temp174=( struct Cyc_Absyn_UnknownCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownCall_e_struct));
_temp174->tag= Cyc_Absyn_UnknownCall_e_tag; _temp174->f1= e; _temp174->f2= es;(
void*) _temp174;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct
Cyc_Absyn_Exp* e, struct Cyc_List_List* es, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_FnCall_e_struct* _temp175=( struct
Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp175->tag= Cyc_Absyn_FnCall_e_tag; _temp175->f1= e; _temp175->f2= es;( void*)
_temp175;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_noinstantiate_exp( struct
Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_NoInstantiate_e_struct* _temp176=( struct Cyc_Absyn_NoInstantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_NoInstantiate_e_struct)); _temp176->tag= Cyc_Absyn_NoInstantiate_e_tag;
_temp176->f1= e;( void*) _temp176;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_List_List* ts, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Instantiate_e_struct*
_temp177=( struct Cyc_Absyn_Instantiate_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct));
_temp177->tag= Cyc_Absyn_Instantiate_e_tag; _temp177->f1= e; _temp177->f2= ts;(
void*) _temp177;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_cast_exp( void* t,
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Cast_e_struct* _temp178=( struct Cyc_Absyn_Cast_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct)); _temp178->tag= Cyc_Absyn_Cast_e_tag;
_temp178->f1=( void*) t; _temp178->f2= e;( void*) _temp178;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Throw_e_struct* _temp179=( struct
Cyc_Absyn_Throw_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Throw_e_struct));
_temp179->tag= Cyc_Absyn_Throw_e_tag; _temp179->f1= e;( void*) _temp179;}), loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Address_e_struct* _temp180=(
struct Cyc_Absyn_Address_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Address_e_struct));
_temp180->tag= Cyc_Absyn_Address_e_tag; _temp180->f1= e;( void*) _temp180;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_sizeof_exp( void* t, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Sizeof_e_struct* _temp181=(
struct Cyc_Absyn_Sizeof_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeof_e_struct));
_temp181->tag= Cyc_Absyn_Sizeof_e_tag; _temp181->f1=( void*) t;( void*) _temp181;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp( struct Cyc_Absyn_Exp* e,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Deref_e_struct*
_temp182=( struct Cyc_Absyn_Deref_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Deref_e_struct));
_temp182->tag= Cyc_Absyn_Deref_e_tag; _temp182->f1= e;( void*) _temp182;}), loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp* e,
struct _tagged_string* n, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({
struct Cyc_Absyn_StructMember_e_struct* _temp183=( struct Cyc_Absyn_StructMember_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructMember_e_struct)); _temp183->tag= Cyc_Absyn_StructMember_e_tag;
_temp183->f1= e; _temp183->f2= n;( void*) _temp183;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_structarrow_exp( struct Cyc_Absyn_Exp* e, struct _tagged_string* n,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_StructArrow_e_struct*
_temp184=( struct Cyc_Absyn_StructArrow_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructArrow_e_struct));
_temp184->tag= Cyc_Absyn_StructArrow_e_tag; _temp184->f1= e; _temp184->f2= n;(
void*) _temp184;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_arrow_exp( struct Cyc_Absyn_Exp*
e, struct _tagged_string* n, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_deref_exp(
Cyc_Absyn_structmember_exp( e, n, loc), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Subscript_e_struct* _temp185=(
struct Cyc_Absyn_Subscript_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Subscript_e_struct));
_temp185->tag= Cyc_Absyn_Subscript_e_tag; _temp185->f1= e1; _temp185->f2= e2;(
void*) _temp185;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_tuple_exp( struct Cyc_List_List*
es, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Tuple_e_struct*
_temp186=( struct Cyc_Absyn_Tuple_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tuple_e_struct));
_temp186->tag= Cyc_Absyn_Tuple_e_tag; _temp186->f1= es;( void*) _temp186;}), loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp( struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_StmtExp_e_struct* _temp187=(
struct Cyc_Absyn_StmtExp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StmtExp_e_struct));
_temp187->tag= Cyc_Absyn_StmtExp_e_tag; _temp187->f1= s;( void*) _temp187;}),
loc);} static char _temp190[ 15u]="Null_Exception"; static struct _tagged_string
Cyc_Absyn_ne_str=( struct _tagged_string){ _temp190, _temp190, _temp190 + 15u};
static struct _tagged_string* Cyc_Absyn_ne=& Cyc_Absyn_ne_str; struct Cyc_Absyn_Exp*
Cyc_Absyn_null_pointer_exn_exp( struct Cyc_Position_Segment* loc){ return Cyc_Absyn_var_exp(({
struct _tuple0* _temp191=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp191->f1=({ struct Cyc_Absyn_Abs_n_struct* _temp192=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp192->tag= Cyc_Absyn_Abs_n_tag;
_temp192->f1= 0;( void*) _temp192;}); _temp191->f2= Cyc_Absyn_ne; _temp191;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_array_exp( int heap_allocate, struct Cyc_List_List*
es, struct Cyc_Position_Segment* loc){ struct Cyc_List_List* dles= 0; for( 0; es
!= 0; es= es->tl){ dles=({ struct Cyc_List_List* _temp193=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp193->hd=( void*)({ struct
_tuple5* _temp194=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp194->f1= 0; _temp194->f2=( struct Cyc_Absyn_Exp*) es->hd; _temp194;});
_temp193->tl= dles; _temp193;});} dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( dles); return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Array_e_struct*
_temp195=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp195->tag= Cyc_Absyn_Array_e_tag; _temp195->f1= heap_allocate; _temp195->f2=
dles;( void*) _temp195;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt* n, struct Cyc_List_List* dles, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp196=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp196->tag= Cyc_Absyn_UnresolvedMem_e_tag;
_temp196->f1= n; _temp196->f2= dles;( void*) _temp196;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt( void* s, struct Cyc_Position_Segment* loc){ return({ struct
Cyc_Absyn_Stmt* _temp197=( struct Cyc_Absyn_Stmt*) GC_malloc( sizeof( struct Cyc_Absyn_Stmt));
_temp197->r=( void*) s; _temp197->loc= loc; _temp197->non_local_preds= 0;
_temp197->try_depth= 0; _temp197->annot=({ struct Cyc_Absyn_EmptyAnnot_struct*
_temp198=( struct Cyc_Absyn_EmptyAnnot_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Absyn_EmptyAnnot_struct));* _temp198=( struct Cyc_Absyn_EmptyAnnot_struct){.tag=
Cyc_Absyn_EmptyAnnot_tag};( struct _xenum_struct*) _temp198;}); _temp197;});}
struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_stmt( Cyc_Absyn_Skip_s, loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({
struct Cyc_Absyn_Exp_s_struct* _temp199=( struct Cyc_Absyn_Exp_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Exp_s_struct)); _temp199->tag= Cyc_Absyn_Exp_s_tag;
_temp199->f1= e;( void*) _temp199;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts(
struct Cyc_List_List* ss, struct Cyc_Position_Segment* loc){ if( ss == 0){
return Cyc_Absyn_skip_stmt( loc);} else{ if( ss->tl == 0){ return( struct Cyc_Absyn_Stmt*)
ss->hd;} else{ return Cyc_Absyn_seq_stmt(( struct Cyc_Absyn_Stmt*) ss->hd, Cyc_Absyn_seq_stmts(
ss->tl, loc), loc);}}} struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Return_s_struct*
_temp200=( struct Cyc_Absyn_Return_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Return_s_struct));
_temp200->tag= Cyc_Absyn_Return_s_tag; _temp200->f1= e;( void*) _temp200;}), loc);}
struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt( struct Cyc_Absyn_Exp* e,
struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_IfThenElse_s_struct*
_temp201=( struct Cyc_Absyn_IfThenElse_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IfThenElse_s_struct));
_temp201->tag= Cyc_Absyn_IfThenElse_s_tag; _temp201->f1= e; _temp201->f2= s1;
_temp201->f3= s2;( void*) _temp201;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_while_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_While_s_struct* _temp202=(
struct Cyc_Absyn_While_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_While_s_struct));
_temp202->tag= Cyc_Absyn_While_s_tag; _temp202->f1=({ struct _tuple2 _temp203;
_temp203.f1= e; _temp203.f2= Cyc_Absyn_skip_stmt( e->loc); _temp203;}); _temp202->f2=
s;( void*) _temp202;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Break_s_struct*
_temp204=( struct Cyc_Absyn_Break_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Break_s_struct));
_temp204->tag= Cyc_Absyn_Break_s_tag; _temp204->f1= 0;( void*) _temp204;}), loc);}
struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt( struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Continue_s_struct* _temp205=(
struct Cyc_Absyn_Continue_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Continue_s_struct));
_temp205->tag= Cyc_Absyn_Continue_s_tag; _temp205->f1= 0;( void*) _temp205;}),
loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3, struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_For_s_struct*
_temp206=( struct Cyc_Absyn_For_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_For_s_struct));
_temp206->tag= Cyc_Absyn_For_s_tag; _temp206->f1= e1; _temp206->f2=({ struct
_tuple2 _temp207; _temp207.f1= e2; _temp207.f2= Cyc_Absyn_skip_stmt( e3->loc);
_temp207;}); _temp206->f3=({ struct _tuple2 _temp208; _temp208.f1= e3; _temp208.f2=
Cyc_Absyn_skip_stmt( e3->loc); _temp208;}); _temp206->f4= s;( void*) _temp206;}),
loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* scs, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({
struct Cyc_Absyn_Switch_s_struct* _temp209=( struct Cyc_Absyn_Switch_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_s_struct)); _temp209->tag= Cyc_Absyn_Switch_s_tag;
_temp209->f1= e; _temp209->f2= scs;( void*) _temp209;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct
Cyc_Position_Segment* loc){ void* _temp210=( void*) s1->r; _LL212: if( _temp210
== Cyc_Absyn_Skip_s){ goto _LL213;} else{ goto _LL214;} _LL214: goto _LL215;
_LL213: return s2; _LL215: return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Seq_s_struct*
_temp216=( struct Cyc_Absyn_Seq_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Seq_s_struct));
_temp216->tag= Cyc_Absyn_Seq_s_tag; _temp216->f1= s1; _temp216->f2= s2;( void*)
_temp216;}), loc); _LL211:;} struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List* el, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({
struct Cyc_Absyn_Fallthru_s_struct* _temp217=( struct Cyc_Absyn_Fallthru_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fallthru_s_struct)); _temp217->tag= Cyc_Absyn_Fallthru_s_tag;
_temp217->f1= el; _temp217->f2= 0;( void*) _temp217;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_decl_stmt( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Decl_s_struct*
_temp218=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp218->tag= Cyc_Absyn_Decl_s_tag; _temp218->f1= d; _temp218->f2= s;( void*)
_temp218;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt( struct
_tuple0* x, void* t, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Decl* d= Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Var_d_struct* _temp219=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp219->tag= Cyc_Absyn_Var_d_tag;
_temp219->f1= Cyc_Absyn_new_vardecl( x, t, init);( void*) _temp219;}), loc);
return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Decl_s_struct* _temp220=( struct
Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp220->tag= Cyc_Absyn_Decl_s_tag; _temp220->f1= d; _temp220->f2= s;( void*)
_temp220;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_cut_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Cut_s_struct*
_temp221=( struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp221->tag= Cyc_Absyn_Cut_s_tag; _temp221->f1= s;( void*) _temp221;}), loc);}
struct Cyc_Absyn_Stmt* Cyc_Absyn_splice_stmt( struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Splice_s_struct*
_temp222=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp222->tag= Cyc_Absyn_Splice_s_tag; _temp222->f1= s;( void*) _temp222;}), loc);}
struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt( struct _tagged_string* v, struct
Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({
struct Cyc_Absyn_Label_s_struct* _temp223=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp223->tag= Cyc_Absyn_Label_s_tag;
_temp223->f1= v; _temp223->f2= s;( void*) _temp223;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_do_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Do_s_struct* _temp224=(
struct Cyc_Absyn_Do_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Do_s_struct));
_temp224->tag= Cyc_Absyn_Do_s_tag; _temp224->f1= s; _temp224->f2=({ struct
_tuple2 _temp225; _temp225.f1= e; _temp225.f2= Cyc_Absyn_skip_stmt( e->loc);
_temp225;});( void*) _temp224;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_trycatch_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* scs, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_TryCatch_s_struct* _temp226=(
struct Cyc_Absyn_TryCatch_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TryCatch_s_struct));
_temp226->tag= Cyc_Absyn_TryCatch_s_tag; _temp226->f1= s; _temp226->f2= scs;(
void*) _temp226;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct
_tagged_string* lab, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(({
struct Cyc_Absyn_Goto_s_struct* _temp227=( struct Cyc_Absyn_Goto_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Goto_s_struct)); _temp227->tag= Cyc_Absyn_Goto_s_tag;
_temp227->f1= lab; _temp227->f2= 0;( void*) _temp227;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
e1, e2, loc), loc);} struct Cyc_Absyn_Pat* Cyc_Absyn_new_pat( void* p, struct
Cyc_Position_Segment* s){ return({ struct Cyc_Absyn_Pat* _temp228=( struct Cyc_Absyn_Pat*)
GC_malloc( sizeof( struct Cyc_Absyn_Pat)); _temp228->r=( void*) p; _temp228->topt=
0; _temp228->loc= s; _temp228;});} struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl(
void* r, struct Cyc_Position_Segment* loc){ return({ struct Cyc_Absyn_Decl*
_temp229=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp229->r=( void*) r; _temp229->loc= loc; _temp229;});} struct Cyc_Absyn_Decl*
Cyc_Absyn_let_decl( struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* t_opt, struct
Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Let_d_struct* _temp230=( struct Cyc_Absyn_Let_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Let_d_struct)); _temp230->tag= Cyc_Absyn_Let_d_tag;
_temp230->f1= p; _temp230->f2= 0; _temp230->f3= t_opt; _temp230->f4= e; _temp230->f5=
0;( void*) _temp230;}), loc);} struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl(
struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init){ return({ struct Cyc_Absyn_Vardecl*
_temp231=( struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp231->sc=( void*) Cyc_Absyn_Public; _temp231->name= x; _temp231->tq= Cyc_Absyn_empty_tqual();
_temp231->type=( void*) t; _temp231->initializer= init; _temp231->shadow= 0;
_temp231->region= 0; _temp231->attributes= 0; _temp231;});} struct Cyc_Absyn_Vardecl*
Cyc_Absyn_static_vardecl( struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init){
return({ struct Cyc_Absyn_Vardecl* _temp232=( struct Cyc_Absyn_Vardecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)); _temp232->sc=( void*) Cyc_Absyn_Static;
_temp232->name= x; _temp232->tq= Cyc_Absyn_empty_tqual(); _temp232->type=( void*)
t; _temp232->initializer= init; _temp232->shadow= 0; _temp232->region= 0;
_temp232->attributes= 0; _temp232;});} struct Cyc_Absyn_Decl* Cyc_Absyn_struct_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Struct_d_struct* _temp233=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp233->tag= Cyc_Absyn_Struct_d_tag;
_temp233->f1=({ struct Cyc_Absyn_Structdecl* _temp234=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp234->sc=( void*) s;
_temp234->name= n; _temp234->tvs= ts; _temp234->fields= fs; _temp234->attributes=
atts; _temp234;});( void*) _temp233;}), loc);} struct Cyc_Absyn_Decl* Cyc_Absyn_enum_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Enum_d_struct*
_temp235=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp235->tag= Cyc_Absyn_Enum_d_tag; _temp235->f1=({ struct Cyc_Absyn_Enumdecl*
_temp236=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp236->sc=( void*) s; _temp236->name= n; _temp236->tvs= ts; _temp236->fields=
fs; _temp236;});( void*) _temp235;}), loc);} struct Cyc_Absyn_Decl* Cyc_Absyn_xenum_decl(
void* s, struct _tuple0* n, struct Cyc_List_List* fs, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Xenum_d_struct* _temp237=(
struct Cyc_Absyn_Xenum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Xenum_d_struct));
_temp237->tag= Cyc_Absyn_Xenum_d_tag; _temp237->f1=({ struct Cyc_Absyn_Xenumdecl*
_temp238=( struct Cyc_Absyn_Xenumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Xenumdecl));
_temp238->sc=( void*) s; _temp238->name= n; _temp238->fields= fs; _temp238;});(
void*) _temp237;}), loc);} static struct _tuple1* Cyc_Absyn_expand_arg( struct
_tuple1* a){ return({ struct _tuple1* _temp239=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp239->f1=(* a).f1; _temp239->f2=(* a).f2; _temp239->f3=
Cyc_Absyn_pointer_expand((* a).f3); _temp239;});} void* Cyc_Absyn_function_typ(
struct Cyc_List_List* tvs, struct Cyc_Core_Opt* eff_typ, void* ret_typ, struct
Cyc_List_List* args, int varargs, struct Cyc_List_List* atts){ return({ struct
Cyc_Absyn_FnType_struct* _temp240=( struct Cyc_Absyn_FnType_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_FnType_struct)); _temp240->tag= Cyc_Absyn_FnType_tag;
_temp240->f1=({ struct Cyc_Absyn_FnInfo _temp241; _temp241.tvars= tvs; _temp241.ret_typ=(
void*) Cyc_Absyn_pointer_expand( ret_typ); _temp241.effect= eff_typ; _temp241.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absyn_expand_arg, args); _temp241.varargs= varargs;
_temp241.attributes= atts; _temp241;});( void*) _temp240;});} void* Cyc_Absyn_pointer_expand(
void* t){ void* _temp242= t; struct Cyc_Absyn_FnInfo _temp248; _LL244: if((
unsigned int) _temp242 > 5u?(( struct _enum_struct*) _temp242)->tag == Cyc_Absyn_FnType_tag:
0){ _LL249: _temp248=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp242)->f1; goto _LL245;} else{ goto _LL246;} _LL246: goto _LL247; _LL245:
return Cyc_Absyn_at_typ( t, Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); _LL247:
return t; _LL243:;} int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp* e){ void*
_temp250=( void*) e->r; void* _temp270; struct Cyc_Absyn_Fndecl* _temp272;
struct _tuple0* _temp274; void* _temp276; struct Cyc_Absyn_Vardecl* _temp278;
struct _tuple0* _temp280; void* _temp282; struct Cyc_Absyn_Vardecl* _temp284;
struct _tuple0* _temp286; void* _temp288; struct _tuple0* _temp290; struct Cyc_Absyn_Exp*
_temp292; struct Cyc_Absyn_Exp* _temp294; struct _tagged_string* _temp296;
struct Cyc_Absyn_Exp* _temp298; struct _tagged_string* _temp300; struct Cyc_Absyn_Exp*
_temp302; struct Cyc_Absyn_Exp* _temp304; _LL252: if((( struct _enum_struct*)
_temp250)->tag == Cyc_Absyn_Var_e_tag){ _LL275: _temp274=( struct _tuple0*)((
struct Cyc_Absyn_Var_e_struct*) _temp250)->f1; goto _LL271; _LL271: _temp270=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp250)->f2; if(( unsigned int)
_temp270 > 1u?(( struct _enum_struct*) _temp270)->tag == Cyc_Absyn_Funname_b_tag:
0){ _LL273: _temp272=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp270)->f1; goto _LL253;} else{ goto _LL254;}} else{ goto _LL254;} _LL254:
if((( struct _enum_struct*) _temp250)->tag == Cyc_Absyn_Var_e_tag){ _LL281:
_temp280=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp250)->f1;
goto _LL277; _LL277: _temp276=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp250)->f2; if(( unsigned int) _temp276 > 1u?(( struct _enum_struct*)
_temp276)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL279: _temp278=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp276)->f1; goto _LL255;} else{ goto
_LL256;}} else{ goto _LL256;} _LL256: if((( struct _enum_struct*) _temp250)->tag
== Cyc_Absyn_Var_e_tag){ _LL287: _temp286=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp250)->f1; goto _LL283; _LL283: _temp282=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp250)->f2; if(( unsigned int) _temp282 > 1u?(( struct _enum_struct*)
_temp282)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL285: _temp284=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp282)->f1; goto _LL257;} else{ goto _LL258;}}
else{ goto _LL258;} _LL258: if((( struct _enum_struct*) _temp250)->tag == Cyc_Absyn_Var_e_tag){
_LL291: _temp290=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp250)->f1;
goto _LL289; _LL289: _temp288=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp250)->f2; goto _LL259;} else{ goto _LL260;} _LL260: if((( struct
_enum_struct*) _temp250)->tag == Cyc_Absyn_Subscript_e_tag){ _LL295: _temp294=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp250)->f1;
goto _LL293; _LL293: _temp292=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp250)->f2; goto _LL261;} else{ goto _LL262;} _LL262: if((( struct
_enum_struct*) _temp250)->tag == Cyc_Absyn_StructMember_e_tag){ _LL299: _temp298=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp250)->f1;
goto _LL297; _LL297: _temp296=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp250)->f2; goto _LL263;} else{ goto _LL264;} _LL264: if((( struct
_enum_struct*) _temp250)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL303: _temp302=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp250)->f1;
goto _LL301; _LL301: _temp300=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp250)->f2; goto _LL265;} else{ goto _LL266;} _LL266: if((( struct
_enum_struct*) _temp250)->tag == Cyc_Absyn_Deref_e_tag){ _LL305: _temp304=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp250)->f1; goto
_LL267;} else{ goto _LL268;} _LL268: goto _LL269; _LL253: return 0; _LL255:
_temp284= _temp278; goto _LL257; _LL257: { void* _temp306= Cyc_Tcutil_compress((
void*) _temp284->type); struct Cyc_Absyn_Exp* _temp312; struct Cyc_Absyn_Tqual*
_temp314; void* _temp316; _LL308: if(( unsigned int) _temp306 > 5u?(( struct
_enum_struct*) _temp306)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL317: _temp316=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp306)->f1; goto _LL315; _LL315:
_temp314=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp306)->f2; goto _LL313; _LL313: _temp312=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp306)->f3; goto _LL309;} else{ goto _LL310;}
_LL310: goto _LL311; _LL309: return 0; _LL311: return 1; _LL307:;} _LL259:
return 1; _LL261: return 1; _LL263: return Cyc_Absyn_is_lvalue( _temp298);
_LL265: return 1; _LL267: return 1; _LL269: return 0; _LL251:;} struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl* sd, struct
_tagged_string* v){ if( sd->fields == 0){ return 0;}{ struct Cyc_List_List* fs=(
struct Cyc_List_List*)( sd->fields)->v; for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Structfield*
f=( struct Cyc_Absyn_Structfield*) fs->hd; if( Cyc_String_zstrptrcmp( f->name, v)
== 0){ return( struct Cyc_Absyn_Structfield*) f;}}} return 0;} struct _tuple3*
Cyc_Absyn_lookup_tuple_field( struct Cyc_List_List* ts, int i){ for( 0; i != 0;
-- i){ if( ts == 0){ return 0;} ts= ts->tl;} if( ts == 0){ return 0;} return(
struct _tuple3*)(( struct _tuple3*) ts->hd);} struct _tagged_string Cyc_Absyn_attribute2string(
void* a){ void* _temp318= a; int _temp356; int _temp358; struct _tagged_string
_temp360; _LL320: if(( unsigned int) _temp318 > 15u?(( struct _enum_struct*)
_temp318)->tag == Cyc_Absyn_Regparm_att_tag: 0){ _LL357: _temp356=( int)((
struct Cyc_Absyn_Regparm_att_struct*) _temp318)->f1; goto _LL321;} else{ goto
_LL322;} _LL322: if( _temp318 == Cyc_Absyn_Stdcall_att){ goto _LL323;} else{
goto _LL324;} _LL324: if( _temp318 == Cyc_Absyn_Cdecl_att){ goto _LL325;} else{
goto _LL326;} _LL326: if( _temp318 == Cyc_Absyn_Noreturn_att){ goto _LL327;}
else{ goto _LL328;} _LL328: if( _temp318 == Cyc_Absyn_Const_att){ goto _LL329;}
else{ goto _LL330;} _LL330: if(( unsigned int) _temp318 > 15u?(( struct
_enum_struct*) _temp318)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL359: _temp358=(
int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp318)->f1; goto _LL331;} else{
goto _LL332;} _LL332: if( _temp318 == Cyc_Absyn_Packed_att){ goto _LL333;} else{
goto _LL334;} _LL334: if(( unsigned int) _temp318 > 15u?(( struct _enum_struct*)
_temp318)->tag == Cyc_Absyn_Section_att_tag: 0){ _LL361: _temp360=( struct
_tagged_string)(( struct Cyc_Absyn_Section_att_struct*) _temp318)->f1; goto
_LL335;} else{ goto _LL336;} _LL336: if( _temp318 == Cyc_Absyn_Nocommon_att){
goto _LL337;} else{ goto _LL338;} _LL338: if( _temp318 == Cyc_Absyn_Shared_att){
goto _LL339;} else{ goto _LL340;} _LL340: if( _temp318 == Cyc_Absyn_Unused_att){
goto _LL341;} else{ goto _LL342;} _LL342: if( _temp318 == Cyc_Absyn_Weak_att){
goto _LL343;} else{ goto _LL344;} _LL344: if( _temp318 == Cyc_Absyn_Dllimport_att){
goto _LL345;} else{ goto _LL346;} _LL346: if( _temp318 == Cyc_Absyn_Dllexport_att){
goto _LL347;} else{ goto _LL348;} _LL348: if( _temp318 == Cyc_Absyn_No_instrument_function_att){
goto _LL349;} else{ goto _LL350;} _LL350: if( _temp318 == Cyc_Absyn_Constructor_att){
goto _LL351;} else{ goto _LL352;} _LL352: if( _temp318 == Cyc_Absyn_Destructor_att){
goto _LL353;} else{ goto _LL354;} _LL354: if( _temp318 == Cyc_Absyn_No_check_memory_usage_att){
goto _LL355;} else{ goto _LL319;} _LL321: return xprintf("regparm(%d)", _temp356);
_LL323: return( struct _tagged_string)({ char* _temp362=( char*)"stdcall";
struct _tagged_string _temp363; _temp363.curr= _temp362; _temp363.base= _temp362;
_temp363.last_plus_one= _temp362 + 8; _temp363;}); _LL325: return( struct
_tagged_string)({ char* _temp364=( char*)"cdecl"; struct _tagged_string _temp365;
_temp365.curr= _temp364; _temp365.base= _temp364; _temp365.last_plus_one=
_temp364 + 6; _temp365;}); _LL327: return( struct _tagged_string)({ char*
_temp366=( char*)"noreturn"; struct _tagged_string _temp367; _temp367.curr=
_temp366; _temp367.base= _temp366; _temp367.last_plus_one= _temp366 + 9;
_temp367;}); _LL329: return( struct _tagged_string)({ char* _temp368=( char*)"const";
struct _tagged_string _temp369; _temp369.curr= _temp368; _temp369.base= _temp368;
_temp369.last_plus_one= _temp368 + 6; _temp369;}); _LL331: if( _temp358 == - 1){
return( struct _tagged_string)({ char* _temp370=( char*)"aligned"; struct
_tagged_string _temp371; _temp371.curr= _temp370; _temp371.base= _temp370;
_temp371.last_plus_one= _temp370 + 8; _temp371;});} else{ return xprintf("aligned(%d)",
_temp358);} _LL333: return( struct _tagged_string)({ char* _temp372=( char*)"packed";
struct _tagged_string _temp373; _temp373.curr= _temp372; _temp373.base= _temp372;
_temp373.last_plus_one= _temp372 + 7; _temp373;}); _LL335: return({ struct
_tagged_string _temp374= _temp360; xprintf("section(\"%.*s\")", _temp374.last_plus_one
- _temp374.curr, _temp374.curr);}); _LL337: return( struct _tagged_string)({
char* _temp375=( char*)"nocommon"; struct _tagged_string _temp376; _temp376.curr=
_temp375; _temp376.base= _temp375; _temp376.last_plus_one= _temp375 + 9;
_temp376;}); _LL339: return( struct _tagged_string)({ char* _temp377=( char*)"shared";
struct _tagged_string _temp378; _temp378.curr= _temp377; _temp378.base= _temp377;
_temp378.last_plus_one= _temp377 + 7; _temp378;}); _LL341: return( struct
_tagged_string)({ char* _temp379=( char*)"unused"; struct _tagged_string
_temp380; _temp380.curr= _temp379; _temp380.base= _temp379; _temp380.last_plus_one=
_temp379 + 7; _temp380;}); _LL343: return( struct _tagged_string)({ char*
_temp381=( char*)"weak"; struct _tagged_string _temp382; _temp382.curr= _temp381;
_temp382.base= _temp381; _temp382.last_plus_one= _temp381 + 5; _temp382;});
_LL345: return( struct _tagged_string)({ char* _temp383=( char*)"dllimport";
struct _tagged_string _temp384; _temp384.curr= _temp383; _temp384.base= _temp383;
_temp384.last_plus_one= _temp383 + 10; _temp384;}); _LL347: return( struct
_tagged_string)({ char* _temp385=( char*)"dllexport"; struct _tagged_string
_temp386; _temp386.curr= _temp385; _temp386.base= _temp385; _temp386.last_plus_one=
_temp385 + 10; _temp386;}); _LL349: return( struct _tagged_string)({ char*
_temp387=( char*)"no_instrument_function"; struct _tagged_string _temp388;
_temp388.curr= _temp387; _temp388.base= _temp387; _temp388.last_plus_one=
_temp387 + 23; _temp388;}); _LL351: return( struct _tagged_string)({ char*
_temp389=( char*)"constructor"; struct _tagged_string _temp390; _temp390.curr=
_temp389; _temp390.base= _temp389; _temp390.last_plus_one= _temp389 + 12;
_temp390;}); _LL353: return( struct _tagged_string)({ char* _temp391=( char*)"destructor";
struct _tagged_string _temp392; _temp392.curr= _temp391; _temp392.base= _temp391;
_temp392.last_plus_one= _temp391 + 11; _temp392;}); _LL355: return( struct
_tagged_string)({ char* _temp393=( char*)"no_check_memory_usage"; struct
_tagged_string _temp394; _temp394.curr= _temp393; _temp394.base= _temp393;
_temp394.last_plus_one= _temp393 + 22; _temp394;}); _LL319:;}