#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2;}; struct _tuple1{ void* f1; struct
_tagged_string* f2;}; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual*
f2; void* f3;}; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt*
f2;}; struct _tuple4{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual* f2;
void* f3;}; struct _tuple5{ struct Cyc_Absyn_Tvar* f1; void* f2;}; struct
_tuple6{ int f1; void* f2;}; struct _tuple7{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Exp* f2;}; struct _tuple8{ struct Cyc_Absyn_Tqual* f1; void* f2;};
struct _tuple9{ struct _tuple4* f1; struct Cyc_Absyn_Exp* f2;}; struct _tuple10{
struct Cyc_Absyn_Enumdecl* f1;}; struct _tuple11{ struct Cyc_Absyn_Xenumdecl* f1;};
typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v;}; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern void* Cyc_Core_snd( struct _tuple0*); extern char Cyc_Core_InvalidArg_tag[
11u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1;};
extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1;}; extern char Cyc_Core_Impossible_tag[ 11u]; struct
Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1;}; extern char
Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char* tag;};
extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1;}; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;}; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag;}; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[ 14u]; struct
Cyc_List_List_mismatch_struct{ char* tag;}; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag;}; extern void* Cyc_List_nth( struct Cyc_List_List*
x, int i); extern struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x,
struct Cyc_List_List* y); struct Cyc_Lineno_Pos{ struct _tagged_string
logical_file; struct _tagged_string line; int line_no; int col;}; typedef struct
Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[ 5u]; struct
Cyc_Position_Exit_struct{ char* tag;}; struct Cyc_Position_Segment; typedef
struct Cyc_Position_Segment* Cyc_Position_seg_t; typedef void* Cyc_Position_Error_kind;
extern void* Cyc_Position_Lex; extern void* Cyc_Position_Parse; extern void* Cyc_Position_Elab;
typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc;}; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag;}; typedef struct _tagged_string* Cyc_Absyn_field_name; typedef struct
_tagged_string* Cyc_Absyn_var; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
typedef void* Cyc_Absyn_Nmspace; typedef struct _tuple1* Cyc_Absyn_qvar; typedef
struct _tuple1* Cyc_Absyn_qvar_opt_t; typedef struct _tuple1* Cyc_Absyn_typedef_name_t;
typedef struct _tuple1* Cyc_Absyn_typedef_name_opt_t; struct Cyc_Absyn_Tvar;
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
Cyc_Absyn_stmt_annot_t; extern void* Cyc_Absyn_Loc_n; extern const int Cyc_Absyn_Rel_n_tag;
struct Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1;}; extern const
int Cyc_Absyn_Abs_n_tag; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1;}; extern void* Cyc_Absyn_Static; extern void* Cyc_Absyn_Abstract; extern
void* Cyc_Absyn_Public; extern void* Cyc_Absyn_Extern; extern void* Cyc_Absyn_ExternC;
struct Cyc_Absyn_Tqual{ int q_const; int q_volatile; int q_restrict;}; extern
void* Cyc_Absyn_B1; extern void* Cyc_Absyn_B2; extern void* Cyc_Absyn_B4; extern
void* Cyc_Absyn_B8; extern void* Cyc_Absyn_AnyKind; extern void* Cyc_Absyn_MemKind;
extern void* Cyc_Absyn_BoxKind; extern void* Cyc_Absyn_RgnKind; extern void* Cyc_Absyn_EffKind;
extern void* Cyc_Absyn_Signed; extern void* Cyc_Absyn_Unsigned; struct Cyc_Absyn_Conref{
void* v;}; extern const int Cyc_Absyn_Eq_constr_tag; struct Cyc_Absyn_Eq_constr_struct{
int tag; void* f1;}; extern const int Cyc_Absyn_Forward_constr_tag; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1;}; extern void* Cyc_Absyn_No_constr; struct
Cyc_Absyn_Tvar{ struct _tagged_string* name; struct Cyc_Absyn_Conref* kind;};
extern void* Cyc_Absyn_Unknown_b; extern const int Cyc_Absyn_Upper_b_tag; struct
Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1;}; struct Cyc_Absyn_PtrInfo{
void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual*
tq; struct Cyc_Absyn_Conref* bounds;}; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int varargs;}; extern void* Cyc_Absyn_VoidType; extern const int Cyc_Absyn_Evar_tag;
struct Cyc_Absyn_Evar_struct{ int tag; void* f1; struct Cyc_Core_Opt* f2; int f3;};
extern const int Cyc_Absyn_VarType_tag; struct Cyc_Absyn_VarType_struct{ int tag;
struct Cyc_Absyn_Tvar* f1;}; extern const int Cyc_Absyn_EnumType_tag; struct Cyc_Absyn_EnumType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Enumdecl**
f3;}; extern const int Cyc_Absyn_XenumType_tag; struct Cyc_Absyn_XenumType_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Xenumdecl** f2;}; extern const int
Cyc_Absyn_PointerType_tag; struct Cyc_Absyn_PointerType_struct{ int tag; struct
Cyc_Absyn_PtrInfo f1;}; extern const int Cyc_Absyn_IntType_tag; struct Cyc_Absyn_IntType_struct{
int tag; void* f1; void* f2;}; extern void* Cyc_Absyn_FloatType; extern void*
Cyc_Absyn_DoubleType; extern const int Cyc_Absyn_ArrayType_tag; struct Cyc_Absyn_ArrayType_struct{
int tag; void* f1; struct Cyc_Absyn_Tqual* f2; struct Cyc_Absyn_Exp* f3;};
extern const int Cyc_Absyn_FnType_tag; struct Cyc_Absyn_FnType_struct{ int tag;
struct Cyc_Absyn_FnInfo f1;}; extern const int Cyc_Absyn_TupleType_tag; struct
Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1;}; extern const
int Cyc_Absyn_StructType_tag; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;};
extern void* Cyc_Absyn_UnionType; extern const int Cyc_Absyn_RgnHandleType_tag;
struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1;}; extern const int Cyc_Absyn_TypedefType_tag;
struct Cyc_Absyn_TypedefType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Core_Opt* f3;}; extern void* Cyc_Absyn_HeapRgn; extern const int
Cyc_Absyn_AccessEff_tag; struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1;};
extern const int Cyc_Absyn_JoinEff_tag; struct Cyc_Absyn_JoinEff_struct{ int tag;
struct Cyc_List_List* f1;}; extern const int Cyc_Absyn_NoTypes_tag; struct Cyc_Absyn_NoTypes_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2;}; extern
const int Cyc_Absyn_WithTypes_tag; struct Cyc_Absyn_WithTypes_struct{ int tag;
struct Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3;}; typedef void* Cyc_Absyn_Pointer_Sort;
extern const int Cyc_Absyn_NonNullable_ps_tag; struct Cyc_Absyn_NonNullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1;}; extern const int Cyc_Absyn_Nullable_ps_tag;
struct Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1;}; extern
void* Cyc_Absyn_TaggedArray_ps; extern void* Cyc_Absyn_Carray_mod; extern const
int Cyc_Absyn_ConstArray_mod_tag; struct Cyc_Absyn_ConstArray_mod_struct{ int
tag; struct Cyc_Absyn_Exp* f1;}; extern const int Cyc_Absyn_Pointer_mod_tag;
struct Cyc_Absyn_Pointer_mod_struct{ int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual*
f3;}; extern const int Cyc_Absyn_Function_mod_tag; struct Cyc_Absyn_Function_mod_struct{
int tag; void* f1;}; extern const int Cyc_Absyn_TypeParams_mod_tag; struct Cyc_Absyn_TypeParams_mod_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3;};
extern const int Cyc_Absyn_Char_c_tag; struct Cyc_Absyn_Char_c_struct{ int tag;
void* f1; char f2;}; extern const int Cyc_Absyn_Short_c_tag; struct Cyc_Absyn_Short_c_struct{
int tag; void* f1; short f2;}; extern const int Cyc_Absyn_Int_c_tag; struct Cyc_Absyn_Int_c_struct{
int tag; void* f1; int f2;}; extern const int Cyc_Absyn_LongLong_c_tag; struct
Cyc_Absyn_LongLong_c_struct{ int tag; void* f1; long long f2;}; extern const int
Cyc_Absyn_Float_c_tag; struct Cyc_Absyn_Float_c_struct{ int tag; struct
_tagged_string f1;}; extern const int Cyc_Absyn_String_c_tag; struct Cyc_Absyn_String_c_struct{
int tag; int f1; struct _tagged_string f2;}; extern void* Cyc_Absyn_Null_c;
extern void* Cyc_Absyn_Plus; extern void* Cyc_Absyn_Times; extern void* Cyc_Absyn_Minus;
extern void* Cyc_Absyn_Div; extern void* Cyc_Absyn_Mod; extern void* Cyc_Absyn_Eq;
extern void* Cyc_Absyn_Neq; extern void* Cyc_Absyn_Gt; extern void* Cyc_Absyn_Lt;
extern void* Cyc_Absyn_Gte; extern void* Cyc_Absyn_Lte; extern void* Cyc_Absyn_Not;
extern void* Cyc_Absyn_Bitnot; extern void* Cyc_Absyn_Bitand; extern void* Cyc_Absyn_Bitor;
extern void* Cyc_Absyn_Bitxor; extern void* Cyc_Absyn_Bitlshift; extern void*
Cyc_Absyn_Bitlrshift; extern void* Cyc_Absyn_Bitarshift; extern void* Cyc_Absyn_Size;
extern void* Cyc_Absyn_Printf; extern void* Cyc_Absyn_Fprintf; extern void* Cyc_Absyn_Xprintf;
extern void* Cyc_Absyn_Scanf; extern void* Cyc_Absyn_Fscanf; extern void* Cyc_Absyn_Sscanf;
extern void* Cyc_Absyn_PreInc; extern void* Cyc_Absyn_PostInc; extern void* Cyc_Absyn_PreDec;
extern void* Cyc_Absyn_PostDec; extern const int Cyc_Absyn_Const_e_tag; struct
Cyc_Absyn_Const_e_struct{ int tag; void* f1;}; extern const int Cyc_Absyn_Var_e_tag;
struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple1* f1; void* f2;}; extern
const int Cyc_Absyn_UnknownId_e_tag; struct Cyc_Absyn_UnknownId_e_struct{ int
tag; struct _tuple1* f1;}; extern const int Cyc_Absyn_Primop_e_tag; struct Cyc_Absyn_Primop_e_struct{
int tag; void* f1; struct Cyc_List_List* f2;}; extern const int Cyc_Absyn_AssignOp_e_tag;
struct Cyc_Absyn_AssignOp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp* f3;}; extern const int Cyc_Absyn_Increment_e_tag;
struct Cyc_Absyn_Increment_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; void* f2;};
extern const int Cyc_Absyn_Conditional_e_tag; struct Cyc_Absyn_Conditional_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3;}; extern const int Cyc_Absyn_SeqExp_e_tag; struct Cyc_Absyn_SeqExp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2;}; extern const int
Cyc_Absyn_UnknownCall_e_tag; struct Cyc_Absyn_UnknownCall_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2;}; extern const int Cyc_Absyn_FnCall_e_tag;
struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List*
f2;}; extern const int Cyc_Absyn_Throw_e_tag; struct Cyc_Absyn_Throw_e_struct{
int tag; struct Cyc_Absyn_Exp* f1;}; extern const int Cyc_Absyn_NoInstantiate_e_tag;
struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;};
extern const int Cyc_Absyn_Instantiate_e_tag; struct Cyc_Absyn_Instantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2;}; extern const int
Cyc_Absyn_Cast_e_tag; struct Cyc_Absyn_Cast_e_struct{ int tag; void* f1; struct
Cyc_Absyn_Exp* f2;}; extern const int Cyc_Absyn_Address_e_tag; struct Cyc_Absyn_Address_e_struct{
int tag; struct Cyc_Absyn_Exp* f1;}; extern const int Cyc_Absyn_Sizeof_e_tag;
struct Cyc_Absyn_Sizeof_e_struct{ int tag; void* f1;}; extern const int Cyc_Absyn_Deref_e_tag;
struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;}; extern
const int Cyc_Absyn_StructMember_e_tag; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2;}; extern const int
Cyc_Absyn_StructArrow_e_tag; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2;}; extern const int Cyc_Absyn_Subscript_e_tag;
struct Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Exp* f2;}; extern const int Cyc_Absyn_Tuple_e_tag; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1;}; extern const int Cyc_Absyn_CompoundLit_e_tag;
struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct Cyc_List_List*
f2;}; extern const int Cyc_Absyn_Array_e_tag; struct Cyc_Absyn_Array_e_struct{
int tag; int f1; struct Cyc_List_List* f2;}; extern const int Cyc_Absyn_Comprehension_e_tag;
struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3;}; extern const int Cyc_Absyn_Struct_e_tag;
struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4;}; extern const
int Cyc_Absyn_Enum_e_tag; struct Cyc_Absyn_Enum_e_struct{ int tag; struct Cyc_Core_Opt*
f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Enumdecl*
f4; struct Cyc_Absyn_Enumfield* f5;}; extern const int Cyc_Absyn_Xenum_e_tag;
struct Cyc_Absyn_Xenum_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Xenumdecl* f3; struct Cyc_Absyn_Enumfield* f4;}; extern
const int Cyc_Absyn_UnresolvedMem_e_tag; struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2;}; extern const int
Cyc_Absyn_StmtExp_e_tag; struct Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt*
f1;}; extern const int Cyc_Absyn_Codegen_e_tag; struct Cyc_Absyn_Codegen_e_struct{
int tag; struct Cyc_Absyn_Fndecl* f1;}; extern const int Cyc_Absyn_Fill_e_tag;
struct Cyc_Absyn_Fill_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;}; struct Cyc_Absyn_Exp{
struct Cyc_Core_Opt* topt; void* r; struct Cyc_Position_Segment* loc;}; extern
void* Cyc_Absyn_Skip_s; extern const int Cyc_Absyn_Exp_s_tag; struct Cyc_Absyn_Exp_s_struct{
int tag; struct Cyc_Absyn_Exp* f1;}; extern const int Cyc_Absyn_Seq_s_tag;
struct Cyc_Absyn_Seq_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt*
f2;}; extern const int Cyc_Absyn_Return_s_tag; struct Cyc_Absyn_Return_s_struct{
int tag; struct Cyc_Absyn_Exp* f1;}; extern const int Cyc_Absyn_IfThenElse_s_tag;
struct Cyc_Absyn_IfThenElse_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt* f3;}; extern const int Cyc_Absyn_While_s_tag;
struct Cyc_Absyn_While_s_struct{ int tag; struct _tuple3 f1; struct Cyc_Absyn_Stmt*
f2;}; extern const int Cyc_Absyn_Break_s_tag; struct Cyc_Absyn_Break_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1;}; extern const int Cyc_Absyn_Continue_s_tag;
struct Cyc_Absyn_Continue_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1;}; extern
const int Cyc_Absyn_Goto_s_tag; struct Cyc_Absyn_Goto_s_struct{ int tag; struct
_tagged_string* f1; struct Cyc_Absyn_Stmt* f2;}; extern const int Cyc_Absyn_For_s_tag;
struct Cyc_Absyn_For_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct _tuple3
f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt* f4;}; extern const int Cyc_Absyn_Switch_s_tag;
struct Cyc_Absyn_Switch_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List*
f2;}; extern const int Cyc_Absyn_Fallthru_s_tag; struct Cyc_Absyn_Fallthru_s_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Absyn_Stmt* f2;}; extern const int
Cyc_Absyn_Decl_s_tag; struct Cyc_Absyn_Decl_s_struct{ int tag; struct Cyc_Absyn_Decl*
f1; struct Cyc_Absyn_Stmt* f2;}; extern const int Cyc_Absyn_Cut_s_tag; struct
Cyc_Absyn_Cut_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1;}; extern const int
Cyc_Absyn_Splice_s_tag; struct Cyc_Absyn_Splice_s_struct{ int tag; struct Cyc_Absyn_Stmt*
f1;}; extern const int Cyc_Absyn_Label_s_tag; struct Cyc_Absyn_Label_s_struct{
int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2;}; extern const
int Cyc_Absyn_Do_s_tag; struct Cyc_Absyn_Do_s_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; struct _tuple3 f2;}; extern const int Cyc_Absyn_TryCatch_s_tag; struct Cyc_Absyn_TryCatch_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_List_List* f2;}; struct Cyc_Absyn_Stmt{
void* r; struct Cyc_Position_Segment* loc; struct Cyc_List_List* non_local_preds;
struct _xenum_struct* annot;}; extern void* Cyc_Absyn_Wild_p; extern const int
Cyc_Absyn_Var_p_tag; struct Cyc_Absyn_Var_p_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1;}; extern void* Cyc_Absyn_Null_p; extern const int Cyc_Absyn_Int_p_tag;
struct Cyc_Absyn_Int_p_struct{ int tag; void* f1; int f2;}; extern const int Cyc_Absyn_Char_p_tag;
struct Cyc_Absyn_Char_p_struct{ int tag; char f1;}; extern const int Cyc_Absyn_Float_p_tag;
struct Cyc_Absyn_Float_p_struct{ int tag; struct _tagged_string f1;}; extern
const int Cyc_Absyn_Tuple_p_tag; struct Cyc_Absyn_Tuple_p_struct{ int tag;
struct Cyc_List_List* f1;}; extern const int Cyc_Absyn_Pointer_p_tag; struct Cyc_Absyn_Pointer_p_struct{
int tag; struct Cyc_Absyn_Pat* f1;}; extern const int Cyc_Absyn_Reference_p_tag;
struct Cyc_Absyn_Reference_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;};
extern const int Cyc_Absyn_Struct_p_tag; struct Cyc_Absyn_Struct_p_struct{ int
tag; struct Cyc_Absyn_Structdecl* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_List_List* f4;}; extern const int Cyc_Absyn_Enum_p_tag; struct
Cyc_Absyn_Enum_p_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; struct Cyc_Absyn_Enumdecl*
f5; struct Cyc_Absyn_Enumfield* f6;}; extern const int Cyc_Absyn_Xenum_p_tag;
struct Cyc_Absyn_Xenum_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Xenumdecl* f4; struct Cyc_Absyn_Enumfield*
f5;}; extern const int Cyc_Absyn_UnknownId_p_tag; struct Cyc_Absyn_UnknownId_p_struct{
int tag; struct _tuple1* f1;}; extern const int Cyc_Absyn_UnknownCall_p_tag;
struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3;}; extern const int Cyc_Absyn_UnknownFields_p_tag;
struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3;}; struct Cyc_Absyn_Pat{ void* r; struct Cyc_Core_Opt*
topt; struct Cyc_Position_Segment* loc;}; struct Cyc_Absyn_Switch_clause{ struct
Cyc_Absyn_Pat* pattern; struct Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp*
where_clause; struct Cyc_Absyn_Stmt* body; struct Cyc_Position_Segment* loc;};
extern void* Cyc_Absyn_Unresolved_b; extern const int Cyc_Absyn_Global_b_tag;
struct Cyc_Absyn_Global_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;};
extern const int Cyc_Absyn_Funname_b_tag; struct Cyc_Absyn_Funname_b_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1;}; extern const int Cyc_Absyn_Param_b_tag;
struct Cyc_Absyn_Param_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;}; extern
const int Cyc_Absyn_Local_b_tag; struct Cyc_Absyn_Local_b_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1;}; extern const int Cyc_Absyn_Pat_b_tag; struct Cyc_Absyn_Pat_b_struct{
int tag; struct Cyc_Absyn_Vardecl* f1;}; struct Cyc_Absyn_Vardecl{ void* sc;
struct _tuple1* name; struct Cyc_Absyn_Tqual* tq; void* type; struct Cyc_Absyn_Exp*
initializer; int shadow; struct Cyc_Core_Opt* region;}; struct Cyc_Absyn_Fndecl{
void* sc; int is_inline; struct _tuple1* name; struct Cyc_List_List* tvs; struct
Cyc_Core_Opt* effect; void* ret_type; struct Cyc_List_List* args; int varargs;
struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ;}; struct Cyc_Absyn_Structdecl{
void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields;}; struct Cyc_Absyn_Enumfield{ struct _tuple1* name; struct Cyc_Absyn_Exp*
tag; struct Cyc_List_List* tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment*
loc;}; struct Cyc_Absyn_Enumdecl{ void* sc; struct Cyc_Core_Opt* name; struct
Cyc_List_List* tvs; struct Cyc_Core_Opt* fields;}; struct Cyc_Absyn_Xenumdecl{
void* sc; struct _tuple1* name; struct Cyc_List_List* fields;}; struct Cyc_Absyn_Typedefdecl{
struct _tuple1* name; struct Cyc_List_List* tvs; void* defn;}; extern const int
Cyc_Absyn_Var_d_tag; struct Cyc_Absyn_Var_d_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1;}; extern const int Cyc_Absyn_Fn_d_tag; struct Cyc_Absyn_Fn_d_struct{ int tag;
struct Cyc_Absyn_Fndecl* f1;}; extern const int Cyc_Absyn_Let_d_tag; struct Cyc_Absyn_Let_d_struct{
int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt* f2; struct Cyc_Core_Opt*
f3; struct Cyc_Absyn_Exp* f4; int f5;}; extern const int Cyc_Absyn_Struct_d_tag;
struct Cyc_Absyn_Struct_d_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;};
extern void* Cyc_Absyn_Union_d; extern const int Cyc_Absyn_Enum_d_tag; struct
Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1;}; extern const
int Cyc_Absyn_Xenum_d_tag; struct Cyc_Absyn_Xenum_d_struct{ int tag; struct Cyc_Absyn_Xenumdecl*
f1;}; extern const int Cyc_Absyn_Typedef_d_tag; struct Cyc_Absyn_Typedef_d_struct{
int tag; struct Cyc_Absyn_Typedefdecl* f1;}; extern const int Cyc_Absyn_Namespace_d_tag;
struct Cyc_Absyn_Namespace_d_struct{ int tag; struct _tagged_string* f1; struct
Cyc_List_List* f2;}; extern const int Cyc_Absyn_Using_d_tag; struct Cyc_Absyn_Using_d_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2;}; extern const int Cyc_Absyn_ExternC_d_tag;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1;}; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc;}; extern const int
Cyc_Absyn_ArrayElement_tag; struct Cyc_Absyn_ArrayElement_struct{ int tag;
struct Cyc_Absyn_Exp* f1;}; extern const int Cyc_Absyn_FieldName_tag; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_string* f1;}; extern char Cyc_Absyn_EmptyAnnot_tag[ 11u];
struct Cyc_Absyn_EmptyAnnot_struct{ char* tag;}; extern struct Cyc_Absyn_Tqual*
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern
struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar(
void*); extern void* Cyc_Absyn_wildtyp(); extern void* Cyc_Absyn_uchar_t; extern
void* Cyc_Absyn_ushort_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t;
extern void* Cyc_Absyn_schar_t; extern void* Cyc_Absyn_sshort_t; extern void*
Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t; extern void* Cyc_Absyn_float_t;
extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_string_typ( void* rgn);
extern void* Cyc_Absyn_file_typ(); extern void* Cyc_Absyn_starb_typ( void* t,
void* rgn, struct Cyc_Absyn_Tqual* tq, void* b); extern void* Cyc_Absyn_atb_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq, void* b); extern void* Cyc_Absyn_star_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq); extern void* Cyc_Absyn_at_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern void* Cyc_Absyn_pointer_expand(
void*); extern int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*); extern struct
_tuple4* Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl*, struct
_tagged_string*); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t; extern char
Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1;}; extern char Cyc_Stdio_FileCloseError_tag[ 15u];
struct Cyc_Stdio_FileCloseError_struct{ char* tag;}; extern int Cyc_String_zstrcmp(
struct _tagged_string, struct _tagged_string); struct Cyc_PP_Ppstate; typedef
struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out*
Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern
struct _tagged_string Cyc_Absynpp_typ2string( void*); extern struct
_tagged_string Cyc_Absynpp_qvar2string( struct _tuple1*); struct Cyc_Set_Set;
typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
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
Cyc_Tcenv_tenv; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern void*
Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*); extern struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Absyn_Stmt*
Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*); extern
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_block( struct Cyc_Tcenv_Tenv*); extern void
Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
void* rgn); extern void Cyc_Tcenv_check_effect_accessible( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void* eff); extern char Cyc_Tcutil_TypeErr_tag[
8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag;}; extern void* Cyc_Tcutil_impos(
struct _tagged_string); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern void
Cyc_Tcutil_unchecked_cast( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*);
extern int Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_to_bool( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_list( struct Cyc_Tcenv_Tenv*, void*, struct Cyc_List_List*);
extern int Cyc_Tcutil_coerce_uint_typ( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_sint_typ( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerceable( void*); extern int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void*, void*); extern int
Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void*,
void*); extern int Cyc_Tcutil_integral_promote( struct Cyc_Tcenv_Tenv*, struct
Cyc_Absyn_Exp*); extern int Cyc_Tcutil_arithmetic_promote( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_comparison_promote( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*); extern void* Cyc_Tcutil_max_arithmetic_type( void*, void*);
extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*);
extern struct _tuple5* Cyc_Tcutil_make_inst_var( struct Cyc_Absyn_Tvar*); extern
void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp*); extern void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*
bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_nonzero_bound( struct
Cyc_Position_Segment*, struct Cyc_Absyn_Conref*); extern void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*, unsigned int i, struct Cyc_Absyn_Conref*); extern
struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd); extern int Cyc_Tcutil_is_tagged_pointer_typ(
void*); extern struct _tuple6 Cyc_Tcutil_addressof_props( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_szof( void* t, struct
Cyc_Position_Segment* loc); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s, int new_block); extern void Cyc_ControlFlow_set_encloser(
struct Cyc_Absyn_Stmt* enclosee, struct Cyc_Absyn_Stmt* encloser); extern struct
Cyc_List_List* Cyc_Formatstr_get_format_typs( struct _tagged_string, struct Cyc_Position_Segment*);
extern struct Cyc_List_List* Cyc_Formatstr_get_scanf_typs( struct _tagged_string,
struct Cyc_Position_Segment*); extern void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*,
void**, struct Cyc_Absyn_Exp*); extern void* Cyc_Tcexp_tcExpInitializer( struct
Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct _tagged_string
msg_part); extern int Cyc_Tcexp_is_const_exp( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*);
static void* Cyc_Tcexp_expr_err( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tagged_string msg){ Cyc_Tcutil_terr( loc, msg); return Cyc_Absyn_wildtyp();}
static void Cyc_Tcexp_resolve_unknown_id( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e, struct _tuple1* q){ struct _handler_cons _temp0; _push_handler(& _temp0);{
struct _xenum_struct* _temp1=( struct _xenum_struct*) setjmp( _temp0.handler);
if( ! _temp1){{ void* _temp2= Cyc_Tcenv_lookup_ordinary( te, e->loc, q); void*
_temp12; struct Cyc_Absyn_Enumfield* _temp14; struct Cyc_Absyn_Enumdecl* _temp16;
struct Cyc_Absyn_Enumfield* _temp18; struct Cyc_Absyn_Xenumdecl* _temp20; struct
Cyc_Absyn_Structdecl* _temp22; _LL4: if((( struct _enum_struct*) _temp2)->tag ==
Cyc_Tcenv_VarRes_tag){ _LL13: _temp12=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp2)->f1; goto _LL5;} else{ goto _LL6;} _LL6: if((( struct _enum_struct*)
_temp2)->tag == Cyc_Tcenv_EnumRes_tag){ _LL17: _temp16=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Tcenv_EnumRes_struct*) _temp2)->f1; goto _LL15; _LL15: _temp14=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Tcenv_EnumRes_struct*) _temp2)->f2;
goto _LL7;} else{ goto _LL8;} _LL8: if((( struct _enum_struct*) _temp2)->tag ==
Cyc_Tcenv_XenumRes_tag){ _LL21: _temp20=( struct Cyc_Absyn_Xenumdecl*)(( struct
Cyc_Tcenv_XenumRes_struct*) _temp2)->f1; goto _LL19; _LL19: _temp18=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Tcenv_XenumRes_struct*) _temp2)->f2; goto _LL9;} else{ goto _LL10;}
_LL10: if((( struct _enum_struct*) _temp2)->tag == Cyc_Tcenv_StructRes_tag){
_LL23: _temp22=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*)
_temp2)->f1; goto _LL11;} else{ goto _LL3;} _LL5:( void*)( e->r=( void*)({
struct Cyc_Absyn_Var_e_struct* _temp24=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp24->tag= Cyc_Absyn_Var_e_tag;
_temp24->f1= q; _temp24->f2=( void*) _temp12;( void*) _temp24;})); goto _LL3;
_LL7:( void*)( e->r=( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp25=( struct
Cyc_Absyn_Enum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp25->tag= Cyc_Absyn_Enum_e_tag; _temp25->f1= 0; _temp25->f2= 0; _temp25->f3=
0; _temp25->f4= _temp16; _temp25->f5= _temp14;( void*) _temp25;})); goto _LL3;
_LL9:( void*)( e->r=( void*)({ struct Cyc_Absyn_Xenum_e_struct* _temp26=( struct
Cyc_Absyn_Xenum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Xenum_e_struct));
_temp26->tag= Cyc_Absyn_Xenum_e_tag; _temp26->f1= 0; _temp26->f2= 0; _temp26->f3=
_temp20; _temp26->f4= _temp18;( void*) _temp26;})); goto _LL3; _LL11: Cyc_Tcutil_terr(
e->loc,({ struct _tagged_string _temp27= Cyc_Absynpp_qvar2string( q); xprintf("bad occurrence of struct name %.*s",
_temp27.last_plus_one - _temp27.curr, _temp27.curr);}));( void*)( e->r=( void*)({
struct Cyc_Absyn_Var_e_struct* _temp28=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp28->tag= Cyc_Absyn_Var_e_tag;
_temp28->f1= q; _temp28->f2=( void*) Cyc_Absyn_Unresolved_b;( void*) _temp28;}));
goto _LL3; _LL3:;} _pop_handler();} else{ struct _xenum_struct* _temp30= _temp1;
_LL32: if( _temp30->tag == Cyc_Dict_Absent_tag){ goto _LL33;} else{ goto _LL34;}
_LL34: goto _LL35; _LL33:( void*)( e->r=( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp36=( struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp36->tag= Cyc_Absyn_Var_e_tag; _temp36->f1= q; _temp36->f2=( void*) Cyc_Absyn_Unresolved_b;(
void*) _temp36;})); goto _LL31; _LL35:( void) _throw( _temp30); _LL31:;}}}
static struct _tuple7* Cyc_Tcexp_make_struct_arg( struct Cyc_Absyn_Exp* e){
return({ struct _tuple7* _temp37=( struct _tuple7*) GC_malloc( sizeof( struct
_tuple7)); _temp37->f1= 0; _temp37->f2= e; _temp37;});} static void Cyc_Tcexp_resolve_unknown_fn(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1,
struct Cyc_List_List* es){ void* _temp38=( void*) e1->r; struct _tuple1* _temp44;
_LL40: if((( struct _enum_struct*) _temp38)->tag == Cyc_Absyn_UnknownId_e_tag){
_LL45: _temp44=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp38)->f1; goto _LL41;} else{ goto _LL42;} _LL42: goto _LL43; _LL41: { struct
_handler_cons _temp46; _push_handler(& _temp46);{ struct _xenum_struct* _temp47=(
struct _xenum_struct*) setjmp( _temp46.handler); if( ! _temp47){{ void* _temp48=
Cyc_Tcenv_lookup_ordinary( te, e1->loc, _temp44); void* _temp58; struct Cyc_Absyn_Enumfield*
_temp60; struct Cyc_Absyn_Enumdecl* _temp62; struct Cyc_Absyn_Enumfield* _temp64;
struct Cyc_Absyn_Xenumdecl* _temp66; struct Cyc_Absyn_Structdecl* _temp68; _LL50:
if((( struct _enum_struct*) _temp48)->tag == Cyc_Tcenv_VarRes_tag){ _LL59:
_temp58=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp48)->f1; goto _LL51;}
else{ goto _LL52;} _LL52: if((( struct _enum_struct*) _temp48)->tag == Cyc_Tcenv_EnumRes_tag){
_LL63: _temp62=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Tcenv_EnumRes_struct*)
_temp48)->f1; goto _LL61; _LL61: _temp60=( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_Tcenv_EnumRes_struct*) _temp48)->f2; goto _LL53;} else{ goto _LL54;} _LL54:
if((( struct _enum_struct*) _temp48)->tag == Cyc_Tcenv_XenumRes_tag){ _LL67:
_temp66=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Tcenv_XenumRes_struct*)
_temp48)->f1; goto _LL65; _LL65: _temp64=( struct Cyc_Absyn_Enumfield*)(( struct
Cyc_Tcenv_XenumRes_struct*) _temp48)->f2; goto _LL55;} else{ goto _LL56;} _LL56:
if((( struct _enum_struct*) _temp48)->tag == Cyc_Tcenv_StructRes_tag){ _LL69:
_temp68=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Tcenv_StructRes_struct*)
_temp48)->f1; goto _LL57;} else{ goto _LL49;} _LL51:( void*)( e->r=( void*)({
struct Cyc_Absyn_FnCall_e_struct* _temp70=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp70->tag= Cyc_Absyn_FnCall_e_tag;
_temp70->f1= e1; _temp70->f2= es;( void*) _temp70;})); _npop_handler( 0u);
return; _LL53: if( _temp60->typs == 0){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp71= Cyc_Absynpp_qvar2string( _temp60->name); xprintf("%.*s is a constant, not a function",
_temp71.last_plus_one - _temp71.curr, _temp71.curr);}));}( void*)( e->r=( void*)({
struct Cyc_Absyn_Enum_e_struct* _temp72=( struct Cyc_Absyn_Enum_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_e_struct)); _temp72->tag= Cyc_Absyn_Enum_e_tag;
_temp72->f1= 0; _temp72->f2= 0; _temp72->f3= es; _temp72->f4= _temp62; _temp72->f5=
_temp60;( void*) _temp72;})); _npop_handler( 0u); return; _LL55: if( _temp64->typs
== 0){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp73= Cyc_Absynpp_qvar2string(
_temp64->name); xprintf("%.*s is a constant, not a function", _temp73.last_plus_one
- _temp73.curr, _temp73.curr);}));}( void*)( e->r=( void*)({ struct Cyc_Absyn_Xenum_e_struct*
_temp74=( struct Cyc_Absyn_Xenum_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Xenum_e_struct));
_temp74->tag= Cyc_Absyn_Xenum_e_tag; _temp74->f1= 0; _temp74->f2= es; _temp74->f3=
_temp66; _temp74->f4= _temp64;( void*) _temp74;})); _npop_handler( 0u); return;
_LL57: { struct Cyc_List_List* des=(( struct Cyc_List_List*(*)( struct _tuple7*(*
f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcexp_make_struct_arg,
es); if( _temp68->name != 0){( void*)( e->r=( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp75=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp75->tag= Cyc_Absyn_Struct_e_tag; _temp75->f1=( struct _tuple1*)( _temp68->name)->v;
_temp75->f2= 0; _temp75->f3= des; _temp75->f4=( struct Cyc_Absyn_Structdecl*)
_temp68;( void*) _temp75;}));} else{ Cyc_Tcutil_terr( e->loc,( struct
_tagged_string)({ char* _temp76=( char*)"missing struct name"; struct
_tagged_string _temp77; _temp77.curr= _temp76; _temp77.base= _temp76; _temp77.last_plus_one=
_temp76 + 20; _temp77;}));( void*)( e->r=( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp78=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp78->tag= Cyc_Absyn_Struct_e_tag; _temp78->f1= _temp44; _temp78->f2= 0;
_temp78->f3= des; _temp78->f4=( struct Cyc_Absyn_Structdecl*) _temp68;( void*)
_temp78;}));} _npop_handler( 0u); return;} _LL49:;} _pop_handler();} else{
struct _xenum_struct* _temp80= _temp47; _LL82: if( _temp80->tag == Cyc_Dict_Absent_tag){
goto _LL83;} else{ goto _LL84;} _LL84: goto _LL85; _LL83: Cyc_Tcutil_terr( e1->loc,({
struct _tagged_string _temp86= Cyc_Absynpp_qvar2string( _temp44); xprintf("unbound identifier %.*s",
_temp86.last_plus_one - _temp86.curr, _temp86.curr);}));( void*)( e->r=( void*)({
struct Cyc_Absyn_FnCall_e_struct* _temp87=( struct Cyc_Absyn_FnCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp87->tag= Cyc_Absyn_FnCall_e_tag;
_temp87->f1= e1; _temp87->f2= es;( void*) _temp87;})); return; _LL85:( void)
_throw( _temp80); _LL81:;}}} _LL43:( void*)( e->r=( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp88=( struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp88->tag= Cyc_Absyn_FnCall_e_tag; _temp88->f1= e1; _temp88->f2= es;( void*)
_temp88;})); return; _LL39:;} static void Cyc_Tcexp_resolve_unresolved_mem(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e, struct Cyc_List_List* des){ if( topt == 0){( void*)( e->r=(
void*)({ struct Cyc_Absyn_Array_e_struct* _temp89=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp89->tag= Cyc_Absyn_Array_e_tag;
_temp89->f1= 0; _temp89->f2= des;( void*) _temp89;})); return;}{ void* t=* topt;
void* _temp90= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp* _temp98; struct
Cyc_Absyn_Tqual* _temp100; void* _temp102; struct Cyc_Absyn_Structdecl**
_temp104; struct Cyc_List_List* _temp106; struct _tuple1* _temp108; _LL92: if((
unsigned int) _temp90 > 5u?(( struct _enum_struct*) _temp90)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL103: _temp102=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp90)->f1;
goto _LL101; _LL101: _temp100=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp90)->f2; goto _LL99; _LL99: _temp98=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp90)->f3; goto _LL93;} else{ goto _LL94;} _LL94: if(( unsigned int) _temp90
> 5u?(( struct _enum_struct*) _temp90)->tag == Cyc_Absyn_StructType_tag: 0){
_LL109: _temp108=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp90)->f1; goto _LL107; _LL107: _temp106=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp90)->f2; goto _LL105; _LL105: _temp104=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp90)->f3;
goto _LL95;} else{ goto _LL96;} _LL96: goto _LL97; _LL93:( void*)( e->r=( void*)({
struct Cyc_Absyn_Array_e_struct* _temp110=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp110->tag= Cyc_Absyn_Array_e_tag;
_temp110->f1= 0; _temp110->f2= des;( void*) _temp110;})); goto _LL91; _LL95: if(
_temp104 == 0? 1: _temp108 == 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp111=( char*)"Compiler bug: struct type not properly set"; struct
_tagged_string _temp112; _temp112.curr= _temp111; _temp112.base= _temp111;
_temp112.last_plus_one= _temp111 + 43; _temp112;}));}( void*)( e->r=( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp113=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp113->tag= Cyc_Absyn_Struct_e_tag;
_temp113->f1=( struct _tuple1*)({ struct _tuple1* _temp114= _temp108; if(
_temp114 == 0){ _throw( Null_Exception);} _temp114;}); _temp113->f2= 0; _temp113->f3=
des; _temp113->f4=( struct Cyc_Absyn_Structdecl*)* _temp104;( void*) _temp113;}));
goto _LL91; _LL97:( void*)( e->r=( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp115=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp115->tag= Cyc_Absyn_Array_e_tag; _temp115->f1= 0; _temp115->f2= des;( void*)
_temp115;})); goto _LL91; _LL91:;}} static void Cyc_Tcexp_tcExpNoInst( struct
Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e); static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e); static void
Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* es){ for(
0; es != 0; es= es->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*) es->hd);}}
void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e,
struct _tagged_string msg_part){ Cyc_Tcutil_check_contains_assign( e); Cyc_Tcexp_tcExp(
te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool( te, e)){
Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp116= msg_part; struct
_tagged_string _temp117= Cyc_Absynpp_typ2string(( void*)( e->topt)->v); xprintf("test of %.*s has type %.*s instead of integral or * type",
_temp116.last_plus_one - _temp116.curr, _temp116.curr, _temp117.last_plus_one -
_temp117.curr, _temp117.curr);}));}} static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* c, struct Cyc_Absyn_Exp*
e){ void* t;{ void* _temp118= c; char _temp134; void* _temp136; short _temp138;
void* _temp140; long long _temp142; void* _temp144; struct _tagged_string
_temp146; int _temp148; void* _temp150; struct _tagged_string _temp152; int
_temp154; _LL120: if(( unsigned int) _temp118 > 1u?(( struct _enum_struct*)
_temp118)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL137: _temp136=( void*)(( struct
Cyc_Absyn_Char_c_struct*) _temp118)->f1; goto _LL135; _LL135: _temp134=( char)((
struct Cyc_Absyn_Char_c_struct*) _temp118)->f2; goto _LL121;} else{ goto _LL122;}
_LL122: if(( unsigned int) _temp118 > 1u?(( struct _enum_struct*) _temp118)->tag
== Cyc_Absyn_Short_c_tag: 0){ _LL141: _temp140=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp118)->f1; goto _LL139; _LL139: _temp138=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp118)->f2; goto _LL123;} else{ goto _LL124;} _LL124: if(( unsigned int)
_temp118 > 1u?(( struct _enum_struct*) _temp118)->tag == Cyc_Absyn_LongLong_c_tag:
0){ _LL145: _temp144=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp118)->f1;
goto _LL143; _LL143: _temp142=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp118)->f2; goto _LL125;} else{ goto _LL126;} _LL126: if(( unsigned int)
_temp118 > 1u?(( struct _enum_struct*) _temp118)->tag == Cyc_Absyn_Float_c_tag:
0){ _LL147: _temp146=( struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*)
_temp118)->f1; goto _LL127;} else{ goto _LL128;} _LL128: if(( unsigned int)
_temp118 > 1u?(( struct _enum_struct*) _temp118)->tag == Cyc_Absyn_Int_c_tag: 0){
_LL151: _temp150=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp118)->f1; goto
_LL149; _LL149: _temp148=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp118)->f2;
goto _LL129;} else{ goto _LL130;} _LL130: if(( unsigned int) _temp118 > 1u?((
struct _enum_struct*) _temp118)->tag == Cyc_Absyn_String_c_tag: 0){ _LL155:
_temp154=( int)(( struct Cyc_Absyn_String_c_struct*) _temp118)->f1; goto _LL153;
_LL153: _temp152=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp118)->f2; goto _LL131;} else{ goto _LL132;} _LL132: if( _temp118 == Cyc_Absyn_Null_c){
goto _LL133;} else{ goto _LL119;} _LL121: t= _temp136 == Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t:
Cyc_Absyn_schar_t; goto _LL119; _LL123: t= _temp140 == Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t:
Cyc_Absyn_sshort_t; goto _LL119; _LL125: t= _temp144 == Cyc_Absyn_Unsigned? Cyc_Absyn_ulong_t:
Cyc_Absyn_slong_t; goto _LL119; _LL127: t= Cyc_Absyn_float_t; goto _LL119;
_LL129: if( topt == 0){ t= _temp150 == Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;}
else{ void* _temp156=* topt; void* _temp166; void* _temp168; void* _temp170;
void* _temp172; void* _temp174; void* _temp176; _LL158: if(( unsigned int)
_temp156 > 5u?(( struct _enum_struct*) _temp156)->tag == Cyc_Absyn_IntType_tag:
0){ _LL169: _temp168=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp156)->f1;
goto _LL167; _LL167: _temp166=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp156)->f2; if( _temp166 == Cyc_Absyn_B1){ goto _LL159;} else{ goto _LL160;}}
else{ goto _LL160;} _LL160: if(( unsigned int) _temp156 > 5u?(( struct
_enum_struct*) _temp156)->tag == Cyc_Absyn_IntType_tag: 0){ _LL173: _temp172=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp156)->f1; goto _LL171; _LL171:
_temp170=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp156)->f2; if(
_temp170 == Cyc_Absyn_B2){ goto _LL161;} else{ goto _LL162;}} else{ goto _LL162;}
_LL162: if(( unsigned int) _temp156 > 5u?(( struct _enum_struct*) _temp156)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL177: _temp176=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp156)->f1; goto _LL175; _LL175: _temp174=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp156)->f2; goto _LL163;} else{ goto _LL164;} _LL164: goto _LL165; _LL159: t=
_temp168 == Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;( void*)( e->r=(
void*)({ struct Cyc_Absyn_Const_e_struct* _temp178=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp178->tag= Cyc_Absyn_Const_e_tag;
_temp178->f1=( void*)({ struct Cyc_Absyn_Char_c_struct* _temp179=( struct Cyc_Absyn_Char_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp179->tag= Cyc_Absyn_Char_c_tag;
_temp179->f1=( void*) _temp168; _temp179->f2=( char) _temp148;( void*) _temp179;});(
void*) _temp178;})); goto _LL157; _LL161: t= _temp172 == Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t:
Cyc_Absyn_sshort_t;( void*)( e->r=( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp180=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp180->tag= Cyc_Absyn_Const_e_tag; _temp180->f1=( void*)({ struct Cyc_Absyn_Short_c_struct*
_temp181=( struct Cyc_Absyn_Short_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct));
_temp181->tag= Cyc_Absyn_Short_c_tag; _temp181->f1=( void*) _temp172; _temp181->f2=(
short) _temp148;( void*) _temp181;});( void*) _temp180;})); goto _LL157; _LL163:
t= _temp176 == Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto
_LL157; _LL165: t= _temp150 == Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;
goto _LL157; _LL157:;} goto _LL119; _LL131: { int len=( int)({ struct
_tagged_string _temp182= _temp152;( unsigned int)( _temp182.last_plus_one -
_temp182.curr);}); void* r=({ struct Cyc_Absyn_Const_e_struct* _temp183=( struct
Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp183->tag= Cyc_Absyn_Const_e_tag; _temp183->f1=( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp184=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp184->tag= Cyc_Absyn_Int_c_tag; _temp184->f1=( void*) Cyc_Absyn_Unsigned;
_temp184->f2= len;( void*) _temp184;});( void*) _temp183;}); struct Cyc_Absyn_Exp*
elen= Cyc_Absyn_new_exp( r, loc); elen->topt=({ struct Cyc_Core_Opt* _temp185=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp185->v=(
void*) Cyc_Absyn_uint_t; _temp185;}); t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual(),({ struct Cyc_Absyn_Upper_b_struct*
_temp186=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp186->tag= Cyc_Absyn_Upper_b_tag; _temp186->f1= elen;( void*) _temp186;}));
if( topt != 0){ void* _temp187= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_Exp*
_temp195; struct Cyc_Absyn_Tqual* _temp197; void* _temp199; struct Cyc_Absyn_PtrInfo
_temp201; struct Cyc_Absyn_Conref* _temp203; struct Cyc_Absyn_Tqual* _temp205;
struct Cyc_Absyn_Conref* _temp207; void* _temp209; void* _temp211; _LL189: if((
unsigned int) _temp187 > 5u?(( struct _enum_struct*) _temp187)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL200: _temp199=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp187)->f1;
goto _LL198; _LL198: _temp197=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp187)->f2; goto _LL196; _LL196: _temp195=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp187)->f3; goto _LL190;} else{ goto _LL191;}
_LL191: if(( unsigned int) _temp187 > 5u?(( struct _enum_struct*) _temp187)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL202: _temp201=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp187)->f1; _LL212: _temp211=( void*)
_temp201.elt_typ; goto _LL210; _LL210: _temp209=( void*) _temp201.rgn_typ; goto
_LL208; _LL208: _temp207=( struct Cyc_Absyn_Conref*) _temp201.nullable; goto
_LL206; _LL206: _temp205=( struct Cyc_Absyn_Tqual*) _temp201.tq; goto _LL204;
_LL204: _temp203=( struct Cyc_Absyn_Conref*) _temp201.bounds; goto _LL192;}
else{ goto _LL193;} _LL193: goto _LL194; _LL190: if( _temp154){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp213=( char*)"string is dynamically allocated -- returns char pointer";
struct _tagged_string _temp214; _temp214.curr= _temp213; _temp214.base= _temp213;
_temp214.last_plus_one= _temp213 + 56; _temp214;}));}{ unsigned int len2= Cyc_Evexp_eval_const_uint_exp(
_temp195); if( ! Cyc_Tcutil_unify( _temp199, Cyc_Absyn_uchar_t)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp215= Cyc_Absynpp_typ2string( _temp199);
xprintf("string constant cannot be used as %.*s array", _temp215.last_plus_one -
_temp215.curr, _temp215.curr);}));} if(( unsigned int) len != len2){ Cyc_Tcutil_terr(
loc, xprintf("string has length %d instead of %d", len,( int) len2));} return*
topt;} _LL192: if( ! Cyc_Tcutil_unify( _temp211, Cyc_Absyn_uchar_t)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp216= Cyc_Absynpp_typ2string( _temp211);
xprintf("string cannot be used as %.*s pointer", _temp216.last_plus_one -
_temp216.curr, _temp216.curr);}));} if( ! Cyc_Tcutil_unify( _temp209, Cyc_Absyn_HeapRgn)){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp217=( char*)"this string literal is heap allocated";
struct _tagged_string _temp218; _temp218.curr= _temp217; _temp218.base= _temp217;
_temp218.last_plus_one= _temp217 + 38; _temp218;}));} _temp203=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( _temp203);{ void*
_temp219=( void*) _temp203->v; void* _temp227; struct Cyc_Absyn_Exp* _temp229;
void* _temp231; _LL221: if(( unsigned int) _temp219 > 1u?(( struct _enum_struct*)
_temp219)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL228: _temp227=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp219)->f1; if(( unsigned int) _temp227 >
1u?(( struct _enum_struct*) _temp227)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL230:
_temp229=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp227)->f1;
goto _LL222;} else{ goto _LL223;}} else{ goto _LL223;} _LL223: if(( unsigned int)
_temp219 > 1u?(( struct _enum_struct*) _temp219)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL232: _temp231=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp219)->f1;
if( _temp231 == Cyc_Absyn_Unknown_b){ goto _LL224;} else{ goto _LL225;}} else{
goto _LL225;} _LL225: goto _LL226; _LL222: { unsigned int len2= Cyc_Evexp_eval_const_uint_exp(
_temp229); if(( unsigned int) len != len2){ Cyc_Tcutil_terr( loc, xprintf("string has length %d instead of %d",
len,( int) len2));} goto _LL220;} _LL224: e->topt=({ struct Cyc_Core_Opt*
_temp233=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp233->v=( void*) t; _temp233;}); Cyc_Tcutil_unchecked_cast( te, e,* topt);
goto _LL220; _LL226:( void*)( _temp203->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp234=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp234->tag= Cyc_Absyn_Eq_constr_tag; _temp234->f1=( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp235=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp235->tag= Cyc_Absyn_Upper_b_tag; _temp235->f1= elen;( void*) _temp235;});(
void*) _temp234;})); goto _LL220; _LL220:;} return* topt; _LL194: goto _LL188;
_LL188:;} goto _LL119;} _LL133: t=({ struct Cyc_Absyn_PointerType_struct*
_temp236=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp236->tag= Cyc_Absyn_PointerType_tag; _temp236->f1=({ struct Cyc_Absyn_PtrInfo
_temp237; _temp237.elt_typ=( void*) Cyc_Absyn_new_evar( Cyc_Absyn_MemKind);
_temp237.rgn_typ=( void*) Cyc_Absyn_new_evar( Cyc_Absyn_RgnKind); _temp237.nullable=((
struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 1); _temp237.tq= Cyc_Absyn_empty_tqual();
_temp237.bounds=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp237;});( void*) _temp236;}); goto _LL119; _LL119:;} return t;} static void*
Cyc_Tcexp_tcVar( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
struct _tuple1* q, void* b){ void* _temp238= b; struct Cyc_Absyn_Vardecl*
_temp252; struct Cyc_Absyn_Fndecl* _temp254; struct Cyc_Absyn_Vardecl* _temp256;
struct Cyc_Absyn_Vardecl* _temp258; struct Cyc_Absyn_Vardecl* _temp260; _LL240:
if( _temp238 == Cyc_Absyn_Unresolved_b){ goto _LL241;} else{ goto _LL242;}
_LL242: if(( unsigned int) _temp238 > 1u?(( struct _enum_struct*) _temp238)->tag
== Cyc_Absyn_Global_b_tag: 0){ _LL253: _temp252=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp238)->f1; goto _LL243;} else{ goto
_LL244;} _LL244: if(( unsigned int) _temp238 > 1u?(( struct _enum_struct*)
_temp238)->tag == Cyc_Absyn_Funname_b_tag: 0){ _LL255: _temp254=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp238)->f1; goto _LL245;} else{ goto
_LL246;} _LL246: if(( unsigned int) _temp238 > 1u?(( struct _enum_struct*)
_temp238)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL257: _temp256=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp238)->f1; goto _LL247;} else{ goto _LL248;}
_LL248: if(( unsigned int) _temp238 > 1u?(( struct _enum_struct*) _temp238)->tag
== Cyc_Absyn_Local_b_tag: 0){ _LL259: _temp258=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp238)->f1; goto _LL249;} else{ goto _LL250;}
_LL250: if(( unsigned int) _temp238 > 1u?(( struct _enum_struct*) _temp238)->tag
== Cyc_Absyn_Param_b_tag: 0){ _LL261: _temp260=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Param_b_struct*) _temp238)->f1; goto _LL251;} else{ goto _LL239;}
_LL241: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp262=
Cyc_Absynpp_qvar2string( q); xprintf("unbound identifier %.*s", _temp262.last_plus_one
- _temp262.curr, _temp262.curr);})); _LL243:({ struct _tuple1* _temp263= q;
unsigned int _temp264= 0; if( _temp264 >= 1u){ _throw( Null_Exception);}
_temp263[ _temp264]=({ struct _tuple1* _temp265= _temp252->name; unsigned int
_temp266= 0; if( _temp266 >= 1u){ _throw( Null_Exception);} _temp265[ _temp266];});});
return( void*) _temp252->type; _LL245:({ struct _tuple1* _temp267= q;
unsigned int _temp268= 0; if( _temp268 >= 1u){ _throw( Null_Exception);}
_temp267[ _temp268]=({ struct _tuple1* _temp269= _temp254->name; unsigned int
_temp270= 0; if( _temp270 >= 1u){ _throw( Null_Exception);} _temp269[ _temp270];});});
return Cyc_Tcutil_fndecl2typ( _temp254); _LL247: _temp258= _temp256; goto _LL249;
_LL249: _temp260= _temp258; goto _LL251; _LL251:(* q).f1= Cyc_Absyn_Loc_n;
return( void*) _temp260->type; _LL239:;} static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* fmt, struct Cyc_List_List* args,
struct Cyc_List_List*(* type_getter)( struct _tagged_string, struct Cyc_Position_Segment*)){
struct Cyc_List_List* desc_types;{ void* _temp271=( void*) fmt->r; void*
_temp277; struct _tagged_string _temp279; int _temp281; _LL273: if((( struct
_enum_struct*) _temp271)->tag == Cyc_Absyn_Const_e_tag){ _LL278: _temp277=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp271)->f1; if(( unsigned int) _temp277 > 1u?((
struct _enum_struct*) _temp277)->tag == Cyc_Absyn_String_c_tag: 0){ _LL282:
_temp281=( int)(( struct Cyc_Absyn_String_c_struct*) _temp277)->f1; if( _temp281
== 0){ goto _LL280;} else{ goto _LL275;} _LL280: _temp279=( struct
_tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp277)->f2; goto _LL274;}
else{ goto _LL275;}} else{ goto _LL275;} _LL275: goto _LL276; _LL274: desc_types=
type_getter( _temp279, fmt->loc); goto _LL272; _LL276: Cyc_Tcutil_terr( fmt->loc,(
struct _tagged_string)({ char* _temp283=( char*)"expecting a literal format string";
struct _tagged_string _temp284; _temp284.curr= _temp283; _temp284.base= _temp283;
_temp284.last_plus_one= _temp283 + 34; _temp284;})); return; _LL272:;} for( 0;
desc_types != 0? args != 0: 0; desc_types= desc_types->tl, args= args->tl){ void*
t=( void*) desc_types->hd; struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*) args->hd;
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp285= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp286= Cyc_Absynpp_typ2string(( void*)( e->topt)->v); xprintf("descriptor has type %.*s but argument has type %.*s",
_temp285.last_plus_one - _temp285.curr, _temp285.curr, _temp286.last_plus_one -
_temp286.curr, _temp286.curr);}));}} if( desc_types != 0){ Cyc_Tcutil_terr( fmt->loc,(
struct _tagged_string)({ char* _temp287=( char*)"too few arguments"; struct
_tagged_string _temp288; _temp288.curr= _temp287; _temp288.base= _temp287;
_temp288.last_plus_one= _temp287 + 18; _temp288;}));} if( args != 0){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*) args->hd)->loc,( struct _tagged_string)({ char* _temp289=(
char*)"too many arguments"; struct _tagged_string _temp290; _temp290.curr=
_temp289; _temp290.base= _temp289; _temp290.last_plus_one= _temp289 + 19;
_temp290;}));}} static void* Cyc_Tcexp_tcUnPrimop( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp* e){
void* t= Cyc_Tcutil_compress(( void*)( e->topt)->v); void* _temp291= p; _LL293:
if( _temp291 == Cyc_Absyn_Plus){ goto _LL294;} else{ goto _LL295;} _LL295: if(
_temp291 == Cyc_Absyn_Minus){ goto _LL296;} else{ goto _LL297;} _LL297: if(
_temp291 == Cyc_Absyn_Not){ goto _LL298;} else{ goto _LL299;} _LL299: if(
_temp291 == Cyc_Absyn_Bitnot){ goto _LL300;} else{ goto _LL301;} _LL301: if(
_temp291 == Cyc_Absyn_Size){ goto _LL302;} else{ goto _LL303;} _LL303: goto
_LL304; _LL294: goto _LL296; _LL296: if( ! Cyc_Tcutil_arithmetic_promote( te, e)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp305= Cyc_Absynpp_typ2string(
t); xprintf("expecting arithmetic type but found %.*s", _temp305.last_plus_one -
_temp305.curr, _temp305.curr);}));} return( void*)( e->topt)->v; _LL298: Cyc_Tcutil_check_contains_assign(
e); if( ! Cyc_Tcutil_coerce_to_bool( te, e)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp306= Cyc_Absynpp_typ2string( t); xprintf("expecting integral or * type but found %.*s",
_temp306.last_plus_one - _temp306.curr, _temp306.curr);}));} return Cyc_Absyn_sint_t;
_LL300: if( ! Cyc_Tcutil_integral_promote( te, e)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp307= Cyc_Absynpp_typ2string( t); xprintf("expecting integral type but found %.*s",
_temp307.last_plus_one - _temp307.curr, _temp307.curr);}));} return( void*)( e->topt)->v;
_LL302:{ void* _temp308= t; struct Cyc_Absyn_Exp* _temp316; struct Cyc_Absyn_Tqual*
_temp318; void* _temp320; struct Cyc_Absyn_PtrInfo _temp322; struct Cyc_Absyn_Conref*
_temp324; struct Cyc_Absyn_Tqual* _temp326; struct Cyc_Absyn_Conref* _temp328;
void* _temp330; void* _temp332; _LL310: if(( unsigned int) _temp308 > 5u?((
struct _enum_struct*) _temp308)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL321:
_temp320=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp308)->f1; goto
_LL319; _LL319: _temp318=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp308)->f2; goto _LL317; _LL317: _temp316=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp308)->f3; goto _LL311;} else{ goto _LL312;}
_LL312: if(( unsigned int) _temp308 > 5u?(( struct _enum_struct*) _temp308)->tag
== Cyc_Absyn_PointerType_tag: 0){ _LL323: _temp322=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp308)->f1; _LL333: _temp332=( void*)
_temp322.elt_typ; goto _LL331; _LL331: _temp330=( void*) _temp322.rgn_typ; goto
_LL329; _LL329: _temp328=( struct Cyc_Absyn_Conref*) _temp322.nullable; goto
_LL327; _LL327: _temp326=( struct Cyc_Absyn_Tqual*) _temp322.tq; goto _LL325;
_LL325: _temp324=( struct Cyc_Absyn_Conref*) _temp322.bounds; goto _LL313;}
else{ goto _LL314;} _LL314: goto _LL315; _LL311: goto _LL309; _LL313: goto
_LL309; _LL315: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp334= Cyc_Absynpp_typ2string(
t); xprintf("size requires pointer or array type, not %.*s", _temp334.last_plus_one
- _temp334.curr, _temp334.curr);})); goto _LL309; _LL309:;} return Cyc_Absyn_uint_t;
_LL304: return(( void*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp335=( char*)"Non-unary primop"; struct
_tagged_string _temp336; _temp336.curr= _temp335; _temp336.base= _temp335;
_temp336.last_plus_one= _temp335 + 17; _temp336;})); _LL292:;} static void* Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
int(* promoter)( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*)){ if( ! promoter(
te, e1)){ Cyc_Tcutil_terr( e1->loc,({ struct _tagged_string _temp337= Cyc_Absynpp_typ2string((
void*)( e1->topt)->v); xprintf("type %.*s cannot be used here", _temp337.last_plus_one
- _temp337.curr, _temp337.curr);})); return Cyc_Absyn_wildtyp();} if( ! promoter(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp338= Cyc_Absynpp_typ2string((
void*)( e2->topt)->v); xprintf("type %.*s cannot be used here", _temp338.last_plus_one
- _temp338.curr, _temp338.curr);})); return Cyc_Absyn_wildtyp();}{ void* t1= Cyc_Tcutil_compress((
void*)( e1->topt)->v); void* t2= Cyc_Tcutil_compress(( void*)( e2->topt)->v);
void* res= Cyc_Tcutil_max_arithmetic_type( t1, t2); Cyc_Tcutil_unchecked_cast(
te, e1, res); Cyc_Tcutil_unchecked_cast( te, e2, res); return res;}} static void*
Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct
Cyc_Absyn_Exp* e2){ void* t1=( void*)( e1->topt)->v; if( Cyc_Tcutil_is_tagged_pointer_typ(
t1)){ if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ Cyc_Tcutil_terr( e2->loc,({
struct _tagged_string _temp339= Cyc_Absynpp_typ2string(( void*)( e2->topt)->v);
xprintf("expecting int but found %.*s", _temp339.last_plus_one - _temp339.curr,
_temp339.curr);}));} return t1;} else{ return Cyc_Tcexp_tcArithBinop( te, e1, e2,
Cyc_Tcutil_arithmetic_promote);}} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)( e1->topt)->v;
void* t2=( void*)( e2->topt)->v; if( Cyc_Tcutil_is_tagged_pointer_typ( t1)){ if(
Cyc_Tcutil_is_tagged_pointer_typ( t2)){ if( ! Cyc_Tcutil_unify( t1, t2)){ Cyc_Tcutil_terr(
e1->loc,({ struct _tagged_string _temp340= Cyc_Absynpp_typ2string(( void*)( e1->topt)->v);
struct _tagged_string _temp341= Cyc_Absynpp_typ2string(( void*)( e2->topt)->v);
xprintf("pointer arithmetic on values of different types (%.*s != %.*s)",
_temp340.last_plus_one - _temp340.curr, _temp340.curr, _temp341.last_plus_one -
_temp341.curr, _temp341.curr);}));} return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){ Cyc_Tcutil_terr( e2->loc,({ struct _tagged_string _temp342= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp343= Cyc_Absynpp_typ2string( t2); xprintf("expecting either %.*s or int but found %.*s",
_temp342.last_plus_one - _temp342.curr, _temp342.curr, _temp343.last_plus_one -
_temp343.curr, _temp343.curr);}));} return t1;}} else{ return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_arithmetic_promote);}} static void* Cyc_Tcexp_tcAnyBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ int e1_is_unboxed_arith= Cyc_Tcutil_comparison_promote(
te, e1); int e2_is_unboxed_arith= Cyc_Tcutil_comparison_promote( te, e2); void*
t1= Cyc_Tcutil_compress(( void*)( e1->topt)->v); void* t2= Cyc_Tcutil_compress((
void*)( e2->topt)->v); if( e1_is_unboxed_arith? e2_is_unboxed_arith: 0){ void*
res= Cyc_Tcutil_max_arithmetic_type( t1, t2); Cyc_Tcutil_unchecked_cast( te, e1,
res); Cyc_Tcutil_unchecked_cast( te, e2, res); return Cyc_Absyn_sint_t;} else{
if( Cyc_Tcutil_unify( t1, t2)? Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar( Cyc_Absyn_BoxKind)):
0){ return Cyc_Absyn_sint_t;} else{ void* t1=( void*)( e1->topt)->v; void* t2=(
void*)( e2->topt)->v;{ struct _tuple0 _temp345=({ struct _tuple0 _temp344;
_temp344.f1= Cyc_Tcutil_compress( t1); _temp344.f2= Cyc_Tcutil_compress( t2);
_temp344;}); void* _temp351; struct Cyc_Absyn_PtrInfo _temp353; struct Cyc_Absyn_Conref*
_temp355; struct Cyc_Absyn_Tqual* _temp357; struct Cyc_Absyn_Conref* _temp359;
void* _temp361; void* _temp363; void* _temp365; struct Cyc_Absyn_PtrInfo
_temp367; struct Cyc_Absyn_Conref* _temp369; struct Cyc_Absyn_Tqual* _temp371;
struct Cyc_Absyn_Conref* _temp373; void* _temp375; void* _temp377; _LL347:
_LL366: _temp365= _temp345.f1; if(( unsigned int) _temp365 > 5u?(( struct
_enum_struct*) _temp365)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL368: _temp367=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp365)->f1;
_LL378: _temp377=( void*) _temp367.elt_typ; goto _LL376; _LL376: _temp375=( void*)
_temp367.rgn_typ; goto _LL374; _LL374: _temp373=( struct Cyc_Absyn_Conref*)
_temp367.nullable; goto _LL372; _LL372: _temp371=( struct Cyc_Absyn_Tqual*)
_temp367.tq; goto _LL370; _LL370: _temp369=( struct Cyc_Absyn_Conref*) _temp367.bounds;
goto _LL352;} else{ goto _LL349;} _LL352: _temp351= _temp345.f2; if((
unsigned int) _temp351 > 5u?(( struct _enum_struct*) _temp351)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL354: _temp353=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp351)->f1; _LL364: _temp363=( void*) _temp353.elt_typ; goto _LL362; _LL362:
_temp361=( void*) _temp353.rgn_typ; goto _LL360; _LL360: _temp359=( struct Cyc_Absyn_Conref*)
_temp353.nullable; goto _LL358; _LL358: _temp357=( struct Cyc_Absyn_Tqual*)
_temp353.tq; goto _LL356; _LL356: _temp355=( struct Cyc_Absyn_Conref*) _temp353.bounds;
goto _LL348;} else{ goto _LL349;} _LL349: goto _LL350; _LL348: if( Cyc_Tcutil_unify(
_temp377, _temp363)){ return Cyc_Absyn_sint_t;} goto _LL346; _LL350: goto _LL346;
_LL346:;} Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp379= Cyc_Absynpp_typ2string(
t1); struct _tagged_string _temp380= Cyc_Absynpp_typ2string( t2); xprintf("comparison not allowed between %.*s and %.*s",
_temp379.last_plus_one - _temp379.curr, _temp379.curr, _temp380.last_plus_one -
_temp380.curr, _temp380.curr);})); return Cyc_Absyn_wildtyp();}}} static void*
Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* _temp381= p; _LL383: if( _temp381 == Cyc_Absyn_Plus){ goto _LL384;} else{
goto _LL385;} _LL385: if( _temp381 == Cyc_Absyn_Minus){ goto _LL386;} else{ goto
_LL387;} _LL387: if( _temp381 == Cyc_Absyn_Times){ goto _LL388;} else{ goto
_LL389;} _LL389: if( _temp381 == Cyc_Absyn_Div){ goto _LL390;} else{ goto _LL391;}
_LL391: if( _temp381 == Cyc_Absyn_Mod){ goto _LL392;} else{ goto _LL393;} _LL393:
if( _temp381 == Cyc_Absyn_Bitand){ goto _LL394;} else{ goto _LL395;} _LL395: if(
_temp381 == Cyc_Absyn_Bitor){ goto _LL396;} else{ goto _LL397;} _LL397: if(
_temp381 == Cyc_Absyn_Bitxor){ goto _LL398;} else{ goto _LL399;} _LL399: if(
_temp381 == Cyc_Absyn_Bitlshift){ goto _LL400;} else{ goto _LL401;} _LL401: if(
_temp381 == Cyc_Absyn_Bitlrshift){ goto _LL402;} else{ goto _LL403;} _LL403: if(
_temp381 == Cyc_Absyn_Bitarshift){ goto _LL404;} else{ goto _LL405;} _LL405: if(
_temp381 == Cyc_Absyn_Eq){ goto _LL406;} else{ goto _LL407;} _LL407: if(
_temp381 == Cyc_Absyn_Neq){ goto _LL408;} else{ goto _LL409;} _LL409: if(
_temp381 == Cyc_Absyn_Gt){ goto _LL410;} else{ goto _LL411;} _LL411: if(
_temp381 == Cyc_Absyn_Lt){ goto _LL412;} else{ goto _LL413;} _LL413: if(
_temp381 == Cyc_Absyn_Gte){ goto _LL414;} else{ goto _LL415;} _LL415: if(
_temp381 == Cyc_Absyn_Lte){ goto _LL416;} else{ goto _LL417;} _LL417: goto
_LL418; _LL384: return Cyc_Tcexp_tcPlus( te, e1, e2); _LL386: return Cyc_Tcexp_tcMinus(
te, e1, e2); _LL388: goto _LL390; _LL390: return Cyc_Tcexp_tcArithBinop( te, e1,
e2, Cyc_Tcutil_arithmetic_promote); _LL392: goto _LL394; _LL394: goto _LL396;
_LL396: goto _LL398; _LL398: goto _LL400; _LL400: goto _LL402; _LL402: goto
_LL404; _LL404: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_integral_promote);
_LL406: goto _LL408; _LL408: goto _LL410; _LL410: goto _LL412; _LL412: goto
_LL414; _LL414: goto _LL416; _LL416: return Cyc_Tcexp_tcAnyBinop( te, loc, e1,
e2); _LL418:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp419=( struct
Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp419=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp420=( char*)"bad binary primop"; struct
_tagged_string _temp421; _temp421.curr= _temp420; _temp421.base= _temp420;
_temp421.last_plus_one= _temp420 + 18; _temp421;})};( struct _xenum_struct*)
_temp419;})); _LL382:;} static void* Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_List_List*
es){ if( p == Cyc_Absyn_Minus?(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
es) == 1: 0){ return Cyc_Tcexp_tcExp( te, topt,( struct Cyc_Absyn_Exp*) es->hd);}
Cyc_Tcexp_tcExpList( te, es);{ void* t;{ void* _temp422= p; _LL424: if( _temp422
== Cyc_Absyn_Printf){ goto _LL425;} else{ goto _LL426;} _LL426: if( _temp422 ==
Cyc_Absyn_Fprintf){ goto _LL427;} else{ goto _LL428;} _LL428: if( _temp422 ==
Cyc_Absyn_Xprintf){ goto _LL429;} else{ goto _LL430;} _LL430: if( _temp422 ==
Cyc_Absyn_Scanf){ goto _LL431;} else{ goto _LL432;} _LL432: if( _temp422 == Cyc_Absyn_Fscanf){
goto _LL433;} else{ goto _LL434;} _LL434: if( _temp422 == Cyc_Absyn_Sscanf){
goto _LL435;} else{ goto _LL436;} _LL436: goto _LL437; _LL425: if( es == 0){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp438=(
char*)"missing format string in printf"; struct _tagged_string _temp439;
_temp439.curr= _temp438; _temp439.base= _temp438; _temp439.last_plus_one=
_temp438 + 32; _temp439;}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)
es->hd, es->tl, Cyc_Formatstr_get_format_typs); t= Cyc_Absyn_VoidType; goto
_LL423; _LL427: if( es == 0? 1: es->tl == 0){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ char* _temp440=( char*)"too few arguments to fprintf";
struct _tagged_string _temp441; _temp441.curr= _temp440; _temp441.base= _temp440;
_temp441.last_plus_one= _temp440 + 29; _temp441;}));} if( ! Cyc_Tcutil_unify((
void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v, Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*) es->hd)->loc,({ struct _tagged_string _temp442= Cyc_Absynpp_typ2string((
void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v); xprintf("expecting FILE @ but found %.*s",
_temp442.last_plus_one - _temp442.curr, _temp442.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)( es->tl)->hd,( es->tl)->tl, Cyc_Formatstr_get_format_typs);
t= Cyc_Absyn_VoidType; goto _LL423; _LL429: if( es == 0){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp443=( char*)"missing format string in xprintf";
struct _tagged_string _temp444; _temp444.curr= _temp443; _temp444.base= _temp443;
_temp444.last_plus_one= _temp443 + 33; _temp444;}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*) es->hd, es->tl, Cyc_Formatstr_get_format_typs); t=
Cyc_Absyn_string_typ( Cyc_Absyn_new_evar( Cyc_Absyn_RgnKind)); goto _LL423;
_LL431: if( es == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp445=( char*)"missing format string in scanf"; struct _tagged_string
_temp446; _temp446.curr= _temp445; _temp446.base= _temp445; _temp446.last_plus_one=
_temp445 + 31; _temp446;}));} Cyc_Tcexp_check_format_args( te,( struct Cyc_Absyn_Exp*)
es->hd, es->tl, Cyc_Formatstr_get_scanf_typs); t= Cyc_Absyn_sint_t; goto _LL423;
_LL433: if( es == 0? 1: es->tl == 0){ return Cyc_Tcexp_expr_err( te, loc,(
struct _tagged_string)({ char* _temp447=( char*)"too few arguments to fscanf";
struct _tagged_string _temp448; _temp448.curr= _temp447; _temp448.base= _temp447;
_temp448.last_plus_one= _temp447 + 28; _temp448;}));} if( ! Cyc_Tcutil_unify((
void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v, Cyc_Absyn_file_typ())){ Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*) es->hd)->loc,({ struct _tagged_string _temp449= Cyc_Absynpp_typ2string((
void*)((( struct Cyc_Absyn_Exp*) es->hd)->topt)->v); xprintf("expecting FILE @ but found %.*s",
_temp449.last_plus_one - _temp449.curr, _temp449.curr);}));} Cyc_Tcexp_check_format_args(
te,( struct Cyc_Absyn_Exp*)( es->tl)->hd,( es->tl)->tl, Cyc_Formatstr_get_scanf_typs);
t= Cyc_Absyn_sint_t; goto _LL423; _LL435: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp450=( char*)"sscanf is not supported"; struct
_tagged_string _temp451; _temp451.curr= _temp450; _temp451.base= _temp450;
_temp451.last_plus_one= _temp450 + 24; _temp451;})); t= Cyc_Absyn_sint_t; goto
_LL423; _LL437: switch((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es)){
case 0: _LL452: return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({
char* _temp454=( char*)"primitive operator has 0 arguments"; struct
_tagged_string _temp455; _temp455.curr= _temp454; _temp455.base= _temp454;
_temp455.last_plus_one= _temp454 + 35; _temp455;})); case 1: _LL453: t= Cyc_Tcexp_tcUnPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*) es->hd); break; case 2: _LL456: t= Cyc_Tcexp_tcBinPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*) es->hd,( struct Cyc_Absyn_Exp*)( es->tl)->hd);
break; default: _LL457: return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp459=( char*)"primitive operator has > 2 arguments";
struct _tagged_string _temp460; _temp460.curr= _temp459; _temp460.base= _temp459;
_temp460.last_plus_one= _temp459 + 37; _temp460;}));} goto _LL423; _LL423:;}
return t;}} static void* Cyc_Tcexp_tcIncrement( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, void* i){
Cyc_Tcexp_tcExpNoPromote( te, 0, e); if( ! Cyc_Absyn_is_lvalue( e)){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp461=( char*)"increment/decrement of non-lvalue";
struct _tagged_string _temp462; _temp462.curr= _temp461; _temp462.base= _temp461;
_temp462.last_plus_one= _temp461 + 34; _temp462;}));}{ void* t=( void*)( e->topt)->v;
if( ! Cyc_Tcutil_arithmetic_promote( te, e)? ! Cyc_Tcutil_is_tagged_pointer_typ(
t): 0){ Cyc_Tcutil_terr( e->loc,({ struct _tagged_string _temp463= Cyc_Absynpp_typ2string(
t); xprintf("expecting arithmetic or pointer type but found %.*s", _temp463.last_plus_one
- _temp463.curr, _temp463.curr);}));} return t;}} static void* Cyc_Tcexp_tcConditional(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest(
te, e1,( struct _tagged_string)({ char* _temp464=( char*)"conditional expression";
struct _tagged_string _temp465; _temp465.curr= _temp464; _temp465.base= _temp464;
_temp465.last_plus_one= _temp464 + 23; _temp465;})); Cyc_Tcexp_tcExp( te, topt,
e2); Cyc_Tcexp_tcExp( te, topt, e3);{ void* t= Cyc_Absyn_new_evar( Cyc_Absyn_MemKind);
struct Cyc_List_List* l1=({ struct Cyc_List_List* _temp466=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp466->hd=( void*) e3; _temp466->tl=
0; _temp466;}); struct Cyc_List_List* l2=({ struct Cyc_List_List* _temp467=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp467->hd=(
void*) e2; _temp467->tl=( struct Cyc_List_List*) l1; _temp467;}); if( ! Cyc_Tcutil_coerce_list(
te, t,( struct Cyc_List_List*) l2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp468= Cyc_Absynpp_typ2string(( void*)( e2->topt)->v); struct
_tagged_string _temp469= Cyc_Absynpp_typ2string(( void*)( e3->topt)->v); xprintf("conditional clause types do not match: %.*s != %.*s",
_temp468.last_plus_one - _temp468.curr, _temp468.curr, _temp469.last_plus_one -
_temp469.curr, _temp469.curr);}));} return t;}} static void* Cyc_Tcexp_tcAssignOp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt* po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e1); Cyc_Tcexp_tcExp( te,( void**)(( void*)&( e1->topt)->v), e2);{ void*
t1=( void*)( e1->topt)->v; void* t2=( void*)( e2->topt)->v;{ void* _temp470= Cyc_Tcutil_compress(
t1); struct Cyc_Absyn_Exp* _temp476; struct Cyc_Absyn_Tqual* _temp478; void*
_temp480; _LL472: if(( unsigned int) _temp470 > 5u?(( struct _enum_struct*)
_temp470)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL481: _temp480=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp470)->f1; goto _LL479; _LL479: _temp478=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp470)->f2;
goto _LL477; _LL477: _temp476=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp470)->f3; goto _LL473;} else{ goto _LL474;} _LL474: goto _LL475; _LL473:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp482=( char*)"cannot assign to an array";
struct _tagged_string _temp483; _temp483.curr= _temp482; _temp483.base= _temp482;
_temp483.last_plus_one= _temp482 + 26; _temp483;})); goto _LL471; _LL475: goto
_LL471; _LL471:;} if( ! Cyc_Absyn_is_lvalue( e1)){ return Cyc_Tcexp_expr_err( te,
loc,( struct _tagged_string)({ char* _temp484=( char*)"assignment to non-lvalue";
struct _tagged_string _temp485; _temp485.curr= _temp484; _temp485.base= _temp484;
_temp485.last_plus_one= _temp484 + 25; _temp485;}));} if( po == 0){ if( ! Cyc_Tcutil_coerce_assign(
te, e2, t1)){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp486= Cyc_Absynpp_typ2string( t1); struct _tagged_string _temp487= Cyc_Absynpp_typ2string(
t2); xprintf("type mismatch: %.*s != %.*s", _temp486.last_plus_one - _temp486.curr,
_temp486.curr, _temp487.last_plus_one - _temp487.curr, _temp487.curr);}));}}
else{ void* p=( void*) po->v; void* t_result= Cyc_Tcexp_tcBinPrimop( te, loc, 0,
p, e1, e2); if( !( Cyc_Tcutil_unify( t_result, t1)? 1: Cyc_Tcutil_coerceable(
t_result))){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char*
_temp488=( char*)"Cannot use this operator in an assignment"; struct
_tagged_string _temp489; _temp489.curr= _temp488; _temp489.base= _temp488;
_temp489.last_plus_one= _temp488 + 42; _temp489;}));}} return t2;}} static void*
Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp(
te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)( e2->topt)->v;}
static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)( e->topt)->v); void* _temp490=
t; struct Cyc_Absyn_PtrInfo _temp496; struct Cyc_Absyn_Conref* _temp498; struct
Cyc_Absyn_Tqual* _temp500; struct Cyc_Absyn_Conref* _temp502; void* _temp504;
void* _temp506; _LL492: if(( unsigned int) _temp490 > 5u?(( struct _enum_struct*)
_temp490)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL497: _temp496=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp490)->f1; _LL507: _temp506=( void*)
_temp496.elt_typ; goto _LL505; _LL505: _temp504=( void*) _temp496.rgn_typ; goto
_LL503; _LL503: _temp502=( struct Cyc_Absyn_Conref*) _temp496.nullable; goto
_LL501; _LL501: _temp500=( struct Cyc_Absyn_Tqual*) _temp496.tq; goto _LL499;
_LL499: _temp498=( struct Cyc_Absyn_Conref*) _temp496.bounds; goto _LL493;}
else{ goto _LL494;} _LL494: goto _LL495; _LL493: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp504); Cyc_Tcutil_check_nonzero_bound( loc, _temp498);{ void*
_temp508= Cyc_Tcutil_compress( _temp506); struct Cyc_Absyn_FnInfo _temp514; int
_temp516; struct Cyc_List_List* _temp518; void* _temp520; struct Cyc_Core_Opt*
_temp522; struct Cyc_List_List* _temp524; _LL510: if(( unsigned int) _temp508 >
5u?(( struct _enum_struct*) _temp508)->tag == Cyc_Absyn_FnType_tag: 0){ _LL515:
_temp514=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp508)->f1;
_LL525: _temp524=( struct Cyc_List_List*) _temp514.tvars; goto _LL523; _LL523:
_temp522=( struct Cyc_Core_Opt*) _temp514.effect; goto _LL521; _LL521: _temp520=(
void*) _temp514.ret_typ; goto _LL519; _LL519: _temp518=( struct Cyc_List_List*)
_temp514.args; goto _LL517; _LL517: _temp516=( int) _temp514.varargs; goto
_LL511;} else{ goto _LL512;} _LL512: goto _LL513; _LL511: if( _temp516){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp526=( char*)"vararg functions (...) not supported yet";
struct _tagged_string _temp527; _temp527.curr= _temp526; _temp527.base= _temp526;
_temp527.last_plus_one= _temp526 + 41; _temp527;}));} if( topt != 0){ Cyc_Tcutil_unify(
_temp520,* topt);} while( es != 0? _temp518 != 0: 0) { struct Cyc_Absyn_Exp* e1=(
struct Cyc_Absyn_Exp*) es->hd; void* t2=(*(( struct _tuple2*) _temp518->hd)).f3;
Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( ! Cyc_Tcutil_coerce_arg( te, e1, t2)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp528= Cyc_Absynpp_typ2string((
void*)( e1->topt)->v); struct _tagged_string _temp529= Cyc_Absynpp_typ2string(
t2); xprintf("actual argument has type %.*s but formal has type %.*s", _temp528.last_plus_one
- _temp528.curr, _temp528.curr, _temp529.last_plus_one - _temp529.curr, _temp529.curr);}));}
es= es->tl; _temp518= _temp518->tl;} if( es != 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp530=( char*)"too many arguments for function";
struct _tagged_string _temp531; _temp531.curr= _temp530; _temp531.base= _temp530;
_temp531.last_plus_one= _temp530 + 32; _temp531;}));} if( _temp518 != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp532=( char*)"too few arguments for function";
struct _tagged_string _temp533; _temp533.curr= _temp532; _temp533.base= _temp532;
_temp533.last_plus_one= _temp532 + 31; _temp533;}));} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*) _temp522->v); return _temp520; _LL513: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp534=( char*)"expected pointer to function";
struct _tagged_string _temp535; _temp535.curr= _temp534; _temp535.base= _temp534;
_temp535.last_plus_one= _temp534 + 29; _temp535;})); _LL509:;} _LL495: return
Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp536=( char*)"expected pointer to function";
struct _tagged_string _temp537; _temp537.curr= _temp536; _temp537.base= _temp536;
_temp537.last_plus_one= _temp536 + 29; _temp537;})); _LL491:;}} static void* Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ, e);{ void*
t=( void*)( e->topt)->v; if( ! Cyc_Tcutil_unify( t, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp538= Cyc_Absynpp_typ2string( t); xprintf("expected xenum exn but found %.*s",
_temp538.last_plus_one - _temp538.curr, _temp538.curr);}));} return Cyc_Absyn_wildtyp();}}
static void* Cyc_Tcexp_tcInstantiate( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst(
te, 0, e);( void*)(( e->topt)->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)( e->topt)->v)));{ void* t1= Cyc_Tcutil_compress(( void*)( e->topt)->v);{
void* _temp539= t1; struct Cyc_Absyn_PtrInfo _temp545; struct Cyc_Absyn_Conref*
_temp547; struct Cyc_Absyn_Tqual* _temp549; struct Cyc_Absyn_Conref* _temp551;
void* _temp553; void* _temp555; _LL541: if(( unsigned int) _temp539 > 5u?((
struct _enum_struct*) _temp539)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL546:
_temp545=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp539)->f1; _LL556: _temp555=( void*) _temp545.elt_typ; goto _LL554; _LL554:
_temp553=( void*) _temp545.rgn_typ; goto _LL552; _LL552: _temp551=( struct Cyc_Absyn_Conref*)
_temp545.nullable; goto _LL550; _LL550: _temp549=( struct Cyc_Absyn_Tqual*)
_temp545.tq; goto _LL548; _LL548: _temp547=( struct Cyc_Absyn_Conref*) _temp545.bounds;
goto _LL542;} else{ goto _LL543;} _LL543: goto _LL544; _LL542:{ void* _temp557=
Cyc_Tcutil_compress( _temp555); struct Cyc_Absyn_FnInfo _temp563; int _temp565;
struct Cyc_List_List* _temp567; void* _temp569; struct Cyc_Core_Opt* _temp571;
struct Cyc_List_List* _temp573; _LL559: if(( unsigned int) _temp557 > 5u?((
struct _enum_struct*) _temp557)->tag == Cyc_Absyn_FnType_tag: 0){ _LL564:
_temp563=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp557)->f1;
_LL574: _temp573=( struct Cyc_List_List*) _temp563.tvars; goto _LL572; _LL572:
_temp571=( struct Cyc_Core_Opt*) _temp563.effect; goto _LL570; _LL570: _temp569=(
void*) _temp563.ret_typ; goto _LL568; _LL568: _temp567=( struct Cyc_List_List*)
_temp563.args; goto _LL566; _LL566: _temp565=( int) _temp563.varargs; goto
_LL560;} else{ goto _LL561;} _LL561: goto _LL562; _LL560: { struct Cyc_List_List*
instantiation= 0; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts)
!=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp573)){ return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp575=( char*)"type instantiation mismatch";
struct _tagged_string _temp576; _temp576.curr= _temp575; _temp576.base= _temp575;
_temp576.last_plus_one= _temp575 + 28; _temp576;}));} for( 0; ts != 0; ts= ts->tl,
_temp573= _temp573->tl){ void* k=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(((
struct Cyc_Absyn_Tvar*) _temp573->hd)->kind); Cyc_Tcutil_check_type( loc, te,
Cyc_Tcenv_lookup_type_vars( te), k,( void*) ts->hd); instantiation=({ struct Cyc_List_List*
_temp577=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp577->hd=( void*)({ struct _tuple5* _temp578=( struct _tuple5*) GC_malloc(
sizeof( struct _tuple5)); _temp578->f1=( struct Cyc_Absyn_Tvar*) _temp573->hd;
_temp578->f2=( void*) ts->hd; _temp578;}); _temp577->tl= instantiation; _temp577;});}{
void* new_fn_typ= Cyc_Tcutil_substitute( instantiation,({ struct Cyc_Absyn_FnType_struct*
_temp579=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp579->tag= Cyc_Absyn_FnType_tag; _temp579->f1=({ struct Cyc_Absyn_FnInfo
_temp580; _temp580.tvars= 0; _temp580.effect= _temp571; _temp580.ret_typ=( void*)
_temp569; _temp580.args= _temp567; _temp580.varargs= _temp565; _temp580;});(
void*) _temp579;})); void* new_typ=({ struct Cyc_Absyn_PointerType_struct*
_temp581=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp581->tag= Cyc_Absyn_PointerType_tag; _temp581->f1=({ struct Cyc_Absyn_PtrInfo
_temp582; _temp582.elt_typ=( void*) new_fn_typ; _temp582.rgn_typ=( void*)
_temp553; _temp582.nullable= _temp551; _temp582.tq= _temp549; _temp582.bounds=
_temp547; _temp582;});( void*) _temp581;}); return new_typ;}} _LL562: goto
_LL558; _LL558:;} goto _LL540; _LL544: goto _LL540; _LL540:;} return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp583= Cyc_Absynpp_typ2string( t1); xprintf("expecting polymorphic type but found %.*s",
_temp583.last_plus_one - _temp583.curr, _temp583.curr);}));}} static void* Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te), Cyc_Absyn_MemKind, t); Cyc_Tcexp_tcExp( te, 0, e);{ void* t2=( void*)( e->topt)->v;
if( ! Cyc_Tcutil_silent_castable( te, loc, t2, t)? ! Cyc_Tcutil_castable( te,
loc, t2, t): 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp584= Cyc_Absynpp_typ2string( t2); struct _tagged_string _temp585= Cyc_Absynpp_typ2string(
t); xprintf("cannot cast %.*s to %.*s", _temp584.last_plus_one - _temp584.curr,
_temp584.curr, _temp585.last_plus_one - _temp585.curr, _temp585.curr);}));}
return t;}} static void* Cyc_Tcexp_tcAddress( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){ void** topt2=
0; struct Cyc_Absyn_Tqual* tq2= Cyc_Absyn_empty_tqual(); if( topt != 0){ void*
_temp586= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp592; struct
Cyc_Absyn_Conref* _temp594; struct Cyc_Absyn_Tqual* _temp596; struct Cyc_Absyn_Conref*
_temp598; void* _temp600; void* _temp602; _LL588: if(( unsigned int) _temp586 >
5u?(( struct _enum_struct*) _temp586)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL593: _temp592=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp586)->f1; _LL603: _temp602=( void*) _temp592.elt_typ; goto _LL601; _LL601:
_temp600=( void*) _temp592.rgn_typ; goto _LL599; _LL599: _temp598=( struct Cyc_Absyn_Conref*)
_temp592.nullable; goto _LL597; _LL597: _temp596=( struct Cyc_Absyn_Tqual*)
_temp592.tq; goto _LL595; _LL595: _temp594=( struct Cyc_Absyn_Conref*) _temp592.bounds;
goto _LL589;} else{ goto _LL590;} _LL590: goto _LL591; _LL589: topt2=( void**)({
void** _temp604=( void**) GC_malloc( sizeof( void*) * 1); _temp604[ 0]= _temp602;
_temp604;}); tq2= _temp596; goto _LL587; _LL591: goto _LL587; _LL587:;} Cyc_Tcexp_tcExpNoInst(
te, topt2, e);{ void* _temp605=( void*) e->r; struct Cyc_Absyn_Structdecl*
_temp613; struct Cyc_List_List* _temp615; struct Cyc_Core_Opt* _temp617; struct
_tuple1* _temp619; struct Cyc_List_List* _temp621; _LL607: if((( struct
_enum_struct*) _temp605)->tag == Cyc_Absyn_Struct_e_tag){ _LL620: _temp619=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp605)->f1; goto _LL618;
_LL618: _temp617=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp605)->f2; goto _LL616; _LL616: _temp615=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp605)->f3; goto _LL614; _LL614: _temp613=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp605)->f4;
goto _LL608;} else{ goto _LL609;} _LL609: if((( struct _enum_struct*) _temp605)->tag
== Cyc_Absyn_Tuple_e_tag){ _LL622: _temp621=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tuple_e_struct*) _temp605)->f1; goto _LL610;} else{ goto _LL611;}
_LL611: goto _LL612; _LL608: goto _LL610; _LL610: { void*(* ptr_maker)( void* t,
void* rgn, struct Cyc_Absyn_Tqual* tq)= Cyc_Absyn_at_typ; if( topt != 0){ void*
_temp623= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp629; struct
Cyc_Absyn_Conref* _temp631; struct Cyc_Absyn_Tqual* _temp633; struct Cyc_Absyn_Conref*
_temp635; void* _temp637; void* _temp639; _LL625: if(( unsigned int) _temp623 >
5u?(( struct _enum_struct*) _temp623)->tag == Cyc_Absyn_PointerType_tag: 0){
_LL630: _temp629=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp623)->f1; _LL640: _temp639=( void*) _temp629.elt_typ; goto _LL638; _LL638:
_temp637=( void*) _temp629.rgn_typ; goto _LL636; _LL636: _temp635=( struct Cyc_Absyn_Conref*)
_temp629.nullable; goto _LL634; _LL634: _temp633=( struct Cyc_Absyn_Tqual*)
_temp629.tq; goto _LL632; _LL632: _temp631=( struct Cyc_Absyn_Conref*) _temp629.bounds;
goto _LL626;} else{ goto _LL627;} _LL627: goto _LL628; _LL626: if((( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp635)){ ptr_maker= Cyc_Absyn_star_typ;}
goto _LL624; _LL628: goto _LL624; _LL624:;} return ptr_maker(( void*)( e->topt)->v,
Cyc_Absyn_HeapRgn, tq2);} _LL612: goto _LL606; _LL606:;}{ void* _temp643; int
_temp645; struct _tuple6 _temp641= Cyc_Tcutil_addressof_props( te, e); _LL646:
_temp645= _temp641.f1; goto _LL644; _LL644: _temp643= _temp641.f2; goto _LL642;
_LL642: { struct Cyc_Absyn_Tqual* tq= Cyc_Absyn_empty_tqual(); if( _temp645){ tq->q_const=
1;}{ void* t= Cyc_Absyn_at_typ(( void*)( e->topt)->v, _temp643, tq); return t;}}}}
static void* Cyc_Tcexp_tcSizeof( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te), Cyc_Absyn_MemKind, t); Cyc_Evexp_szof( t, loc); return Cyc_Absyn_uint_t;}
static void* Cyc_Tcexp_tcDeref( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t=
Cyc_Tcutil_compress(( void*)( e->topt)->v); void* _temp647= t; struct Cyc_Absyn_PtrInfo
_temp653; struct Cyc_Absyn_Conref* _temp655; struct Cyc_Absyn_Tqual* _temp657;
struct Cyc_Absyn_Conref* _temp659; void* _temp661; void* _temp663; _LL649: if((
unsigned int) _temp647 > 5u?(( struct _enum_struct*) _temp647)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL654: _temp653=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp647)->f1; _LL664: _temp663=( void*) _temp653.elt_typ; goto _LL662; _LL662:
_temp661=( void*) _temp653.rgn_typ; goto _LL660; _LL660: _temp659=( struct Cyc_Absyn_Conref*)
_temp653.nullable; goto _LL658; _LL658: _temp657=( struct Cyc_Absyn_Tqual*)
_temp653.tq; goto _LL656; _LL656: _temp655=( struct Cyc_Absyn_Conref*) _temp653.bounds;
goto _LL650;} else{ goto _LL651;} _LL651: goto _LL652; _LL650: Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp661); Cyc_Tcutil_check_nonzero_bound( loc, _temp655); return
_temp663; _LL652: return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string
_temp665= Cyc_Absynpp_typ2string( t); xprintf("expecting * or @ type but found %.*s",
_temp665.last_plus_one - _temp665.curr, _temp665.curr);})); _LL648:;}} static
void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* e, struct
_tagged_string* f){ Cyc_Tcexp_tcExpNoPromote( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)( e->topt)->v); void* _temp666= t; struct Cyc_Absyn_Structdecl** _temp676;
struct Cyc_Absyn_Structdecl* _temp678; struct Cyc_List_List* _temp679; struct
_tuple1* _temp681; struct Cyc_Absyn_Exp* _temp684; struct Cyc_Absyn_Tqual*
_temp686; void* _temp688; struct Cyc_Absyn_PtrInfo _temp693; _LL668: if((
unsigned int) _temp666 > 5u?(( struct _enum_struct*) _temp666)->tag == Cyc_Absyn_StructType_tag:
0){ _LL682: _temp681=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp666)->f1; goto _LL680; _LL680: _temp679=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp666)->f2; goto _LL677; _LL677: _temp676=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp666)->f3;
if( _temp676 == 0){ goto _LL670;} else{ _temp678=* _temp676; goto _LL669;}}
else{ goto _LL670;} _LL670: if(( unsigned int) _temp666 > 5u?(( struct
_enum_struct*) _temp666)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL689: _temp688=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp666)->f1; goto _LL687; _LL687:
_temp686=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp666)->f2; goto _LL685; _LL685: _temp684=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp666)->f3; goto _LL683;} else{ goto _LL672;}
_LL683: if( Cyc_String_zstrcmp(* f,( struct _tagged_string)({ char* _temp690=(
char*)"size"; struct _tagged_string _temp691; _temp691.curr= _temp690; _temp691.base=
_temp690; _temp691.last_plus_one= _temp690 + 5; _temp691;})) == 0){ goto _LL671;}
else{ goto _LL672;} _LL672: if(( unsigned int) _temp666 > 5u?(( struct
_enum_struct*) _temp666)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL694: _temp693=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp666)->f1;
goto _LL692;} else{ goto _LL674;} _LL692: if( Cyc_String_zstrcmp(* f,( struct
_tagged_string)({ char* _temp695=( char*)"size"; struct _tagged_string _temp696;
_temp696.curr= _temp695; _temp696.base= _temp695; _temp696.last_plus_one=
_temp695 + 5; _temp696;})) == 0){ goto _LL673;} else{ goto _LL674;} _LL674: goto
_LL675; _LL669: if( _temp681 == 0){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp697=( char*)"unresolved struct type (COMPILER ERROR)";
struct _tagged_string _temp698; _temp698.curr= _temp697; _temp698.base= _temp697;
_temp698.last_plus_one= _temp697 + 40; _temp698;}));}{ struct _tuple4* finfo=
Cyc_Absyn_lookup_struct_field( _temp678, f); if( finfo == 0){ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp700= Cyc_Absynpp_qvar2string(( struct
_tuple1*)({ struct _tuple1* _temp699= _temp681; if( _temp699 == 0){ _throw(
Null_Exception);} _temp699;})); struct _tagged_string _temp701=* f; xprintf("struct %.*s has no %.*s field",
_temp700.last_plus_one - _temp700.curr, _temp700.curr, _temp701.last_plus_one -
_temp701.curr, _temp701.curr);}));}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp678->tvs,
_temp679); void* t2= Cyc_Tcutil_substitute( inst,(* finfo).f3); return t2;}}
_LL671:( void*)( outer_e->r=( void*)({ struct Cyc_Absyn_Primop_e_struct*
_temp702=( struct Cyc_Absyn_Primop_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct));
_temp702->tag= Cyc_Absyn_Primop_e_tag; _temp702->f1=( void*) Cyc_Absyn_Size;
_temp702->f2=({ struct Cyc_List_List* _temp703=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp703->hd=( void*) e; _temp703->tl=
0; _temp703;});( void*) _temp702;})); return Cyc_Absyn_uint_t; _LL673:( void*)(
outer_e->r=( void*)({ struct Cyc_Absyn_Primop_e_struct* _temp704=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp704->tag= Cyc_Absyn_Primop_e_tag;
_temp704->f1=( void*) Cyc_Absyn_Size; _temp704->f2=({ struct Cyc_List_List*
_temp705=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp705->hd=( void*) e; _temp705->tl= 0; _temp705;});( void*) _temp704;}));
return Cyc_Absyn_uint_t; _LL675: if( Cyc_String_zstrcmp(* f,( struct
_tagged_string)({ char* _temp706=( char*)"size"; struct _tagged_string _temp707;
_temp707.curr= _temp706; _temp707.base= _temp706; _temp707.last_plus_one=
_temp706 + 5; _temp707;})) == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp708= Cyc_Absynpp_typ2string( t); xprintf("expecting struct or array, found %.*s",
_temp708.last_plus_one - _temp708.curr, _temp708.curr);}));} else{ return Cyc_Tcexp_expr_err(
te, loc,({ struct _tagged_string _temp709= Cyc_Absynpp_typ2string( t); xprintf("expecting struct, found %.*s",
_temp709.last_plus_one - _temp709.curr, _temp709.curr);}));} _LL667:;}} static
void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, struct _tagged_string* f){ Cyc_Tcexp_tcExp(
te, 0, e);{ void* t= Cyc_Tcutil_compress(( void*)( e->topt)->v); void* _temp710=
t; struct Cyc_Absyn_PtrInfo _temp716; struct Cyc_Absyn_Conref* _temp718; struct
Cyc_Absyn_Tqual* _temp720; struct Cyc_Absyn_Conref* _temp722; void* _temp724;
void* _temp726; _LL712: if(( unsigned int) _temp710 > 5u?(( struct _enum_struct*)
_temp710)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL717: _temp716=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp710)->f1; _LL727: _temp726=( void*)
_temp716.elt_typ; goto _LL725; _LL725: _temp724=( void*) _temp716.rgn_typ; goto
_LL723; _LL723: _temp722=( struct Cyc_Absyn_Conref*) _temp716.nullable; goto
_LL721; _LL721: _temp720=( struct Cyc_Absyn_Tqual*) _temp716.tq; goto _LL719;
_LL719: _temp718=( struct Cyc_Absyn_Conref*) _temp716.bounds; goto _LL713;}
else{ goto _LL714;} _LL714: goto _LL715; _LL713: Cyc_Tcutil_check_nonzero_bound(
loc, _temp718);{ void* _temp728= Cyc_Tcutil_compress( _temp726); struct Cyc_Absyn_Structdecl**
_temp734; struct Cyc_Absyn_Structdecl* _temp736; struct Cyc_List_List* _temp737;
struct _tuple1* _temp739; _LL730: if(( unsigned int) _temp728 > 5u?(( struct
_enum_struct*) _temp728)->tag == Cyc_Absyn_StructType_tag: 0){ _LL740: _temp739=(
struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp728)->f1; goto
_LL738; _LL738: _temp737=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp728)->f2; goto _LL735; _LL735: _temp734=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp728)->f3; if( _temp734 == 0){ goto
_LL732;} else{ _temp736=* _temp734; goto _LL731;}} else{ goto _LL732;} _LL732:
goto _LL733; _LL731: { struct _tuple4* finfo= Cyc_Absyn_lookup_struct_field(
_temp736, f); if( finfo == 0){ return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp742= Cyc_Absynpp_qvar2string(( struct _tuple1*)({ struct
_tuple1* _temp741= _temp739; if( _temp741 == 0){ _throw( Null_Exception);}
_temp741;})); struct _tagged_string _temp743=* f; xprintf("struct %.*s has no %.*s field",
_temp742.last_plus_one - _temp742.curr, _temp742.curr, _temp743.last_plus_one -
_temp743.curr, _temp743.curr);}));}{ struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp736->tvs,
_temp737); void* t3= Cyc_Tcutil_substitute( inst,(* finfo).f3); Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp724); return t3;}} _LL733: return Cyc_Tcexp_expr_err( te, loc,({
struct _tagged_string _temp744= Cyc_Absynpp_typ2string( t); xprintf("expecting struct pointer, found %.*s",
_temp744.last_plus_one - _temp744.curr, _temp744.curr);})); _LL729:;} _LL715:
return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp745= Cyc_Absynpp_typ2string(
t); xprintf("expecting struct pointer, found %.*s", _temp745.last_plus_one -
_temp745.curr, _temp745.curr);})); _LL711:;}} static void* Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
ts, struct Cyc_Absyn_Exp* index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp(
index); struct _handler_cons _temp746; _push_handler(& _temp746);{ struct
_xenum_struct* _temp747=( struct _xenum_struct*) setjmp( _temp746.handler); if(
! _temp747){{ void* _temp748=(*(( struct _tuple8*(*)( struct Cyc_List_List* x,
int i)) Cyc_List_nth)( ts,( int) i)).f2; _npop_handler( 0u); return _temp748;}
_pop_handler();} else{ struct _xenum_struct* _temp750= _temp747; _LL752: if(
_temp750->tag == Cyc_List_Nth_tag){ goto _LL753;} else{ goto _LL754;} _LL754:
goto _LL755; _LL753: return Cyc_Tcexp_expr_err( te, loc, xprintf("index is %d but tuple has only %d fields",(
int) i,(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts))); _LL755:(
void) _throw( _temp750); _LL751:;}}} static void* Cyc_Tcexp_tcSubscript( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te,
0, e2);{ void* t1= Cyc_Tcutil_compress(( void*)( e1->topt)->v); void* t2= Cyc_Tcutil_compress((
void*)( e2->topt)->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return Cyc_Tcexp_expr_err(
te, e2->loc,({ struct _tagged_string _temp756= Cyc_Absynpp_typ2string( t2);
xprintf("expecting int subscript, found %.*s", _temp756.last_plus_one - _temp756.curr,
_temp756.curr);}));}{ void* _temp757= t1; struct Cyc_Absyn_PtrInfo _temp765;
struct Cyc_Absyn_Conref* _temp767; struct Cyc_Absyn_Tqual* _temp769; struct Cyc_Absyn_Conref*
_temp771; void* _temp773; void* _temp775; struct Cyc_List_List* _temp777; _LL759:
if(( unsigned int) _temp757 > 5u?(( struct _enum_struct*) _temp757)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL766: _temp765=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp757)->f1; _LL776: _temp775=( void*) _temp765.elt_typ; goto _LL774; _LL774:
_temp773=( void*) _temp765.rgn_typ; goto _LL772; _LL772: _temp771=( struct Cyc_Absyn_Conref*)
_temp765.nullable; goto _LL770; _LL770: _temp769=( struct Cyc_Absyn_Tqual*)
_temp765.tq; goto _LL768; _LL768: _temp767=( struct Cyc_Absyn_Conref*) _temp765.bounds;
goto _LL760;} else{ goto _LL761;} _LL761: if(( unsigned int) _temp757 > 5u?((
struct _enum_struct*) _temp757)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL778:
_temp777=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp757)->f1;
goto _LL762;} else{ goto _LL763;} _LL763: goto _LL764; _LL760: if( Cyc_Tcexp_is_const_exp(
te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp( e2),
_temp767);} else{ Cyc_Tcutil_check_nonzero_bound( loc, _temp767);} Cyc_Tcenv_check_rgn_accessible(
te, loc, _temp773); return _temp775; _LL762: return Cyc_Tcexp_ithTupleType( te,
loc, _temp777, e2); _LL764: return Cyc_Tcexp_expr_err( te, loc,({ struct
_tagged_string _temp779= Cyc_Absynpp_typ2string( t1); xprintf("subscript applied to %.*s",
_temp779.last_plus_one - _temp779.curr, _temp779.curr);})); _LL758:;}}} static
void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_List_List* es){ int done= 0; struct Cyc_List_List*
fields= 0; if( topt != 0){ void* _temp780= Cyc_Tcutil_compress(* topt); struct
Cyc_List_List* _temp786; _LL782: if(( unsigned int) _temp780 > 5u?(( struct
_enum_struct*) _temp780)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL787: _temp786=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp780)->f1; goto
_LL783;} else{ goto _LL784;} _LL784: goto _LL785; _LL783: if((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp786) !=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp788=( char*)"tuple expression has the wrong number of fields"; struct
_tagged_string _temp789; _temp789.curr= _temp788; _temp789.base= _temp788;
_temp789.last_plus_one= _temp788 + 48; _temp789;})); goto _LL781;}{ void* topt2;
for( 0; es != 0; es= es->tl, _temp786= _temp786->tl){ topt2=(*(( struct _tuple8*)
_temp786->hd)).f2; Cyc_Tcexp_tcExpInitializer( te,( void**)& topt2,( struct Cyc_Absyn_Exp*)
es->hd); fields=({ struct Cyc_List_List* _temp790=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp790->hd=( void*)({ struct
_tuple8* _temp791=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp791->f1=(*(( struct _tuple8*) _temp786->hd)).f1; _temp791->f2=( void*)(((
struct Cyc_Absyn_Exp*) es->hd)->topt)->v; _temp791;}); _temp790->tl= fields;
_temp790;});} done= 1; goto _LL781;} _LL785: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp792= Cyc_Absynpp_typ2string(* topt); xprintf("initializer for %.*s is a tuple",
_temp792.last_plus_one - _temp792.curr, _temp792.curr);})); goto _LL781; _LL781:;}
if( ! done){ for( 0; es != 0; es= es->tl){ Cyc_Tcexp_tcExpInitializer( te, 0,(
struct Cyc_Absyn_Exp*) es->hd); fields=({ struct Cyc_List_List* _temp793=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp793->hd=(
void*)({ struct _tuple8* _temp794=( struct _tuple8*) GC_malloc( sizeof( struct
_tuple8)); _temp794->f1= Cyc_Absyn_empty_tqual(); _temp794->f2=( void*)(((
struct Cyc_Absyn_Exp*) es->hd)->topt)->v; _temp794;}); _temp793->tl= fields;
_temp793;});}} return({ struct Cyc_Absyn_TupleType_struct* _temp795=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp795->tag= Cyc_Absyn_TupleType_tag;
_temp795->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields);( void*) _temp795;});} static void* Cyc_Tcexp_tcCompoundLit( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct _tuple2* t, struct Cyc_List_List*
des){ return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char*
_temp796=( char*)"tcCompoundLit"; struct _tagged_string _temp797; _temp797.curr=
_temp796; _temp797.base= _temp796; _temp797.last_plus_one= _temp796 + 14;
_temp797;}));} static void* Cyc_Tcexp_tcArray( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, int
heap_allocated, struct Cyc_List_List* des){ struct Cyc_List_List* es=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple7*), struct Cyc_List_List*
x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct _tuple7*)) Cyc_Core_snd,
des); void* res= Cyc_Absyn_new_evar( Cyc_Absyn_MemKind); void* sz_rexp=({ struct
Cyc_Absyn_Const_e_struct* _temp798=( struct Cyc_Absyn_Const_e_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Const_e_struct)); _temp798->tag= Cyc_Absyn_Const_e_tag;
_temp798->f1=( void*)({ struct Cyc_Absyn_Int_c_struct* _temp799=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp799->tag= Cyc_Absyn_Int_c_tag;
_temp799->f1=( void*) Cyc_Absyn_Unsigned; _temp799->f2=(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es);( void*) _temp799;});( void*) _temp798;}); struct Cyc_Absyn_Exp*
sz_exp= Cyc_Absyn_new_exp( sz_rexp, loc); sz_exp->topt=({ struct Cyc_Core_Opt*
_temp800=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp800->v=( void*) Cyc_Absyn_uint_t; _temp800;});{ void* res_t2; if(
heap_allocated){ res_t2= Cyc_Absyn_atb_typ( res, Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual(),({
struct Cyc_Absyn_Upper_b_struct* _temp801=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp801->tag= Cyc_Absyn_Upper_b_tag;
_temp801->f1= sz_exp;( void*) _temp801;}));} else{ res_t2=({ struct Cyc_Absyn_ArrayType_struct*
_temp802=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp802->tag= Cyc_Absyn_ArrayType_tag; _temp802->f1=( void*) res; _temp802->f2=
Cyc_Absyn_empty_tqual(); _temp802->f3= sz_exp;( void*) _temp802;});} if( topt ==
0){ struct Cyc_List_List* es2= es; for( 0; es2 != 0; es2= es2->tl){ Cyc_Tcexp_tcExpInitializer(
te, 0,( struct Cyc_Absyn_Exp*) es2->hd);}} else{ void* _temp803= Cyc_Tcutil_compress(*
topt); struct Cyc_Absyn_PtrInfo _temp811; struct Cyc_Absyn_Conref* _temp813;
struct Cyc_Absyn_Tqual* _temp815; struct Cyc_Absyn_Conref* _temp817; void*
_temp819; void* _temp821; struct Cyc_Absyn_Exp* _temp823; struct Cyc_Absyn_Tqual*
_temp825; void* _temp827; _LL805: if(( unsigned int) _temp803 > 5u?(( struct
_enum_struct*) _temp803)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL812: _temp811=(
struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp803)->f1;
_LL822: _temp821=( void*) _temp811.elt_typ; goto _LL820; _LL820: _temp819=( void*)
_temp811.rgn_typ; goto _LL818; _LL818: _temp817=( struct Cyc_Absyn_Conref*)
_temp811.nullable; goto _LL816; _LL816: _temp815=( struct Cyc_Absyn_Tqual*)
_temp811.tq; goto _LL814; _LL814: _temp813=( struct Cyc_Absyn_Conref*) _temp811.bounds;
goto _LL806;} else{ goto _LL807;} _LL807: if(( unsigned int) _temp803 > 5u?((
struct _enum_struct*) _temp803)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL828:
_temp827=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp803)->f1; goto
_LL826; _LL826: _temp825=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp803)->f2; goto _LL824; _LL824: _temp823=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp803)->f3; goto _LL808;} else{ goto _LL809;}
_LL809: goto _LL810; _LL806: _temp827= _temp821; goto _LL808; _LL808: { void*
topt1= _temp827;{ struct Cyc_List_List* es1= es; for( 0; es1 != 0; es1= es1->tl){
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*) es1->hd; Cyc_Tcexp_tcExpInitializer(
te,( void**)& topt1, e); Cyc_Tcutil_coerce_arg( te, e, _temp827);}} goto _LL804;}
_LL810: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp829= Cyc_Absynpp_typ2string(*
topt); xprintf("initializer for %.*s is an array", _temp829.last_plus_one -
_temp829.curr, _temp829.curr);})); Cyc_Tcexp_tcExpList( te, es); goto _LL804;
_LL804:;} if( ! Cyc_Tcutil_coerce_list( te, res, es)){ Cyc_Tcutil_terr((( struct
Cyc_Absyn_Exp*) es->hd)->loc,({ struct _tagged_string _temp830= Cyc_Absynpp_typ2string(
res); xprintf("elements of array do not all have the same type (%.*s)", _temp830.last_plus_one
- _temp830.curr, _temp830.curr);}));}{ int offset= 0; for( 0; des != 0; offset
++, des= des->tl){ struct Cyc_List_List* ds=(*(( struct _tuple7*) des->hd)).f1;
if( ds != 0){ void* _temp831=( void*) ds->hd; struct _tagged_string* _temp837;
struct Cyc_Absyn_Exp* _temp839; _LL833: if((( struct _enum_struct*) _temp831)->tag
== Cyc_Absyn_FieldName_tag){ _LL838: _temp837=( struct _tagged_string*)(( struct
Cyc_Absyn_FieldName_struct*) _temp831)->f1; goto _LL834;} else{ goto _LL835;}
_LL835: if((( struct _enum_struct*) _temp831)->tag == Cyc_Absyn_ArrayElement_tag){
_LL840: _temp839=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp831)->f1; goto _LL836;} else{ goto _LL832;} _LL834: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp841=( char*)"only array index designators are supported";
struct _tagged_string _temp842; _temp842.curr= _temp841; _temp842.base= _temp841;
_temp842.last_plus_one= _temp841 + 43; _temp842;})); goto _LL832; _LL836: Cyc_Tcexp_tcExpInitializer(
te, 0, _temp839);{ unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp839); if(
i !=( unsigned int) offset){ Cyc_Tcutil_terr( _temp839->loc, xprintf("expecting index designator %d but found %d",
offset,( int) i));} goto _LL832;} _LL832:;}}} if( topt != 0){ if( ! Cyc_Tcutil_unify(*
topt, res_t2)? Cyc_Tcutil_silent_castable( te, loc, res_t2,* topt): 0){ e->topt=({
struct Cyc_Core_Opt* _temp843=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp843->v=( void*) res_t2; _temp843;}); Cyc_Tcutil_unchecked_cast(
te, e,* topt); res_t2=* topt;}} return res_t2;}} static void* Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound, struct Cyc_Absyn_Exp* body){
Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ( te, bound)){
Cyc_Tcutil_terr( bound->loc,({ struct _tagged_string _temp844= Cyc_Absynpp_typ2string((
void*)( bound->topt)->v); xprintf("expecting unsigned int, found %.*s", _temp844.last_plus_one
- _temp844.curr, _temp844.curr);}));} te= Cyc_Tcenv_new_block( te); te= Cyc_Tcenv_add_local_var(
loc, te, vd);{ struct Cyc_Absyn_PtrInfo pinfo; void** topt2= 0; struct Cyc_Absyn_Conref**
nopt= 0; struct Cyc_Absyn_Tqual** tqopt= 0; struct Cyc_Absyn_Conref** bopt= 0;
if( topt != 0){ void* _temp845= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo
_temp851; _LL847: if(( unsigned int) _temp845 > 5u?(( struct _enum_struct*)
_temp845)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL852: _temp851=( struct Cyc_Absyn_PtrInfo)((
struct Cyc_Absyn_PointerType_struct*) _temp845)->f1; goto _LL848;} else{ goto
_LL849;} _LL849: goto _LL850; _LL848: pinfo= _temp851; topt2=( void**)(( void*)&
pinfo.elt_typ); nopt=( struct Cyc_Absyn_Conref**)& pinfo.nullable; tqopt=(
struct Cyc_Absyn_Tqual**)& pinfo.tq; bopt=( struct Cyc_Absyn_Conref**)& pinfo.bounds;
goto _LL846; _LL850: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp853= Cyc_Absynpp_typ2string(*
topt); xprintf("initializer for %.*s is an array", _temp853.last_plus_one -
_temp853.curr, _temp853.curr);})); goto _LL846; _LL846:;} Cyc_Tcexp_tcExp( te,
topt2, body);{ void* b_result; int bound_is_const= Cyc_Tcexp_is_const_exp( te,
bound); if( bopt != 0){ void* _temp854=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(* bopt))->v; void*
_temp862; void* _temp864; struct Cyc_Absyn_Exp* _temp866; _LL856: if((
unsigned int) _temp854 > 1u?(( struct _enum_struct*) _temp854)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL863: _temp862=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp854)->f1;
if( _temp862 == Cyc_Absyn_Unknown_b){ goto _LL857;} else{ goto _LL858;}} else{
goto _LL858;} _LL858: if(( unsigned int) _temp854 > 1u?(( struct _enum_struct*)
_temp854)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL865: _temp864=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp854)->f1; if(( unsigned int) _temp864 >
1u?(( struct _enum_struct*) _temp864)->tag == Cyc_Absyn_Upper_b_tag: 0){ _LL867:
_temp866=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*) _temp864)->f1;
goto _LL859;} else{ goto _LL860;}} else{ goto _LL860;} _LL860: goto _LL861;
_LL857: b_result= Cyc_Absyn_Unknown_b; goto _LL855; _LL859: if( bound_is_const?
Cyc_Evexp_eval_const_uint_exp( _temp866) <= Cyc_Evexp_eval_const_uint_exp( bound):
0){ b_result=({ struct Cyc_Absyn_Upper_b_struct* _temp868=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp868->tag= Cyc_Absyn_Upper_b_tag;
_temp868->f1= _temp866;( void*) _temp868;}); goto _LL855;} goto _LL861; _LL861:
b_result= bound_is_const?({ struct Cyc_Absyn_Upper_b_struct* _temp869=( struct
Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp869->tag= Cyc_Absyn_Upper_b_tag; _temp869->f1= bound;( void*) _temp869;}):
Cyc_Absyn_Unknown_b; goto _LL855; _LL855:;} else{ b_result= bound_is_const?({
struct Cyc_Absyn_Upper_b_struct* _temp870=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp870->tag= Cyc_Absyn_Upper_b_tag;
_temp870->f1= bound;( void*) _temp870;}): Cyc_Absyn_Unknown_b;}{ struct Cyc_Absyn_Tqual*
tq_result= tqopt != 0?* tqopt: Cyc_Absyn_empty_tqual(); void*(* ptr_maker)( void*
t, void* rgn, struct Cyc_Absyn_Tqual* tq, void* b)= b_result == Cyc_Absyn_Unknown_b?
Cyc_Absyn_starb_typ: Cyc_Absyn_atb_typ; return ptr_maker(( void*)( body->topt)->v,
Cyc_Absyn_HeapRgn, tq_result, b_result);}}}} static void* Cyc_Tcexp_tcStruct(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple1** tn, struct Cyc_Core_Opt** otyps, struct Cyc_List_List* args, struct
Cyc_Absyn_Structdecl** sd_opt){ struct Cyc_Absyn_Structdecl* sd; if(* sd_opt !=
0){ sd=( struct Cyc_Absyn_Structdecl*)({ struct Cyc_Absyn_Structdecl* _temp871=*
sd_opt; if( _temp871 == 0){ _throw( Null_Exception);} _temp871;});} else{{
struct _handler_cons _temp872; _push_handler(& _temp872);{ struct _xenum_struct*
_temp873=( struct _xenum_struct*) setjmp( _temp872.handler); if( ! _temp873){ sd=*
Cyc_Tcenv_lookup_structdecl( te, loc,* tn); _pop_handler();} else{ struct
_xenum_struct* _temp875= _temp873; _LL877: if( _temp875->tag == Cyc_Dict_Absent_tag){
goto _LL878;} else{ goto _LL879;} _LL879: goto _LL880; _LL878: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp881= Cyc_Absynpp_qvar2string(* tn); xprintf("unbound struct name %.*s",
_temp881.last_plus_one - _temp881.curr, _temp881.curr);})); return topt != 0?*
topt: Cyc_Absyn_VoidType; _LL880:( void) _throw( _temp875); _LL876:;}}}* sd_opt=(
struct Cyc_Absyn_Structdecl*) sd; if( sd->name != 0){* tn=( struct _tuple1*)( sd->name)->v;}}{
struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)( struct _tuple5*(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var,
sd->tvs); struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple5*)) Cyc_Core_snd, inst); void* res_typ=({ struct Cyc_Absyn_StructType_struct*
_temp882=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp882->tag= Cyc_Absyn_StructType_tag; _temp882->f1=( struct _tuple1*)* tn;
_temp882->f2= typs; _temp882->f3=( struct Cyc_Absyn_Structdecl**)({ struct Cyc_Absyn_Structdecl**
_temp883=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*)
* 1); _temp883[ 0]= sd; _temp883;});( void*) _temp882;}); if( topt != 0){ Cyc_Tcutil_unify(
res_typ,* topt);}* otyps=({ struct Cyc_Core_Opt* _temp884=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp884->v=( void*) typs; _temp884;});{
struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_Absyn_Structdecl* sd)) Cyc_Tcutil_resolve_struct_designators)(
loc, args, sd); for( 0; fields != 0; fields= fields->tl){ struct _tuple9
_temp887; struct Cyc_Absyn_Exp* _temp888; struct _tuple4* _temp890; struct
_tuple4 _temp892; void* _temp893; struct Cyc_Absyn_Tqual* _temp895; struct
_tagged_string* _temp897; struct _tuple9* _temp885=( struct _tuple9*) fields->hd;
_temp887=* _temp885; _LL891: _temp890= _temp887.f1; _temp892=* _temp890; _LL898:
_temp897= _temp892.f1; goto _LL896; _LL896: _temp895= _temp892.f2; goto _LL894;
_LL894: _temp893= _temp892.f3; goto _LL889; _LL889: _temp888= _temp887.f2; goto
_LL886; _LL886: { void* inst_fieldtyp= Cyc_Tcutil_substitute( inst, _temp893);
Cyc_Tcexp_tcExpInitializer( te,( void**)& inst_fieldtyp, _temp888); if( ! Cyc_Tcutil_coerce_arg(
te, _temp888, inst_fieldtyp)){ Cyc_Tcutil_terr( _temp888->loc,({ struct
_tagged_string _temp899=* _temp897; struct _tagged_string _temp900= Cyc_Absynpp_qvar2string(*
tn); struct _tagged_string _temp901= Cyc_Absynpp_typ2string( inst_fieldtyp);
struct _tagged_string _temp902= Cyc_Absynpp_typ2string(( void*)( _temp888->topt)->v);
xprintf("field %.*s of struct %.*s expects type %.*s != %.*s", _temp899.last_plus_one
- _temp899.curr, _temp899.curr, _temp900.last_plus_one - _temp900.curr, _temp900.curr,
_temp901.last_plus_one - _temp901.curr, _temp901.curr, _temp902.last_plus_one -
_temp902.curr, _temp902.curr);}));}}} return res_typ;}}} static void* Cyc_Tcexp_tcEnum(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref, struct Cyc_List_List*
es, struct Cyc_Absyn_Enumdecl* ed, struct Cyc_Absyn_Enumfield* ef){ struct Cyc_List_List*
all_inst=(( struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, ed->tvs);
struct Cyc_List_List* exist_inst=(( struct Cyc_List_List*(*)( struct _tuple5*(*
f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var,
ef->tvs); struct Cyc_List_List* all_typs=(( struct Cyc_List_List*(*)( void*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct
_tuple5*)) Cyc_Core_snd, all_inst); struct Cyc_List_List* exist_typs=(( struct
Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)((
void*(*)( struct _tuple5*)) Cyc_Core_snd, exist_inst); struct Cyc_List_List*
inst=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( all_inst, exist_inst);* all_ref=({ struct Cyc_Core_Opt*
_temp903=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp903->v=( void*) all_typs; _temp903;});* exist_ref=({ struct Cyc_Core_Opt*
_temp904=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp904->v=( void*) exist_typs; _temp904;});{ void* res=({ struct Cyc_Absyn_EnumType_struct*
_temp905=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp905->tag= Cyc_Absyn_EnumType_tag; _temp905->f1=( struct _tuple1*)(( struct
_tuple1*)( ed->name)->v); _temp905->f2= all_typs; _temp905->f3=( struct Cyc_Absyn_Enumdecl**)({
struct _tuple10* _temp906=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp906->f1= ed; _temp906;});( void*) _temp905;}); if( topt != 0){ Cyc_Tcutil_unify(*
topt, res);}{ struct Cyc_List_List* ts= ef->typs; while( es != 0? ts != 0: 0) {
struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*) es->hd; void* t= Cyc_Tcutil_substitute(
inst,(*(( struct _tuple8*) ts->hd)).f2); Cyc_Tcexp_tcExpInitializer( te,( void**)&
t, e); es= es->tl; ts= ts->tl; if( ! Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr(
e->loc,({ struct _tagged_string _temp909= Cyc_Absynpp_qvar2string( ef->name);
struct _tagged_string _temp910= Cyc_Absynpp_typ2string( t); struct
_tagged_string _temp911= e->topt == 0?( struct _tagged_string)({ char* _temp907=(
char*)"?"; struct _tagged_string _temp908; _temp908.curr= _temp907; _temp908.base=
_temp907; _temp908.last_plus_one= _temp907 + 2; _temp908;}): Cyc_Absynpp_typ2string((
void*)( e->topt)->v); xprintf("enum constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp909.last_plus_one - _temp909.curr, _temp909.curr, _temp910.last_plus_one -
_temp910.curr, _temp910.curr, _temp911.last_plus_one - _temp911.curr, _temp911.curr);}));}}
if( es != 0){ return Cyc_Tcexp_expr_err( te,(( struct Cyc_Absyn_Exp*) es->hd)->loc,({
struct _tagged_string _temp912= Cyc_Absynpp_qvar2string( ef->name); xprintf("too many arguments for enum constructor %.*s",
_temp912.last_plus_one - _temp912.curr, _temp912.curr);}));} if( ts != 0){
return Cyc_Tcexp_expr_err( te, loc,({ struct _tagged_string _temp913= Cyc_Absynpp_qvar2string(
ef->name); xprintf("too few arguments for enum constructor %.*s", _temp913.last_plus_one
- _temp913.curr, _temp913.curr);}));} return res;}}} static void* Cyc_Tcexp_tcXenum(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Core_Opt** exist_ref, struct Cyc_List_List* es, struct Cyc_Absyn_Xenumdecl*
xd, struct Cyc_Absyn_Enumfield* ef){ struct Cyc_List_List* exist_inst=(( struct
Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, ef->tvs); struct Cyc_List_List*
exist_typs=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List*
x)) Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd, exist_inst); void*
res=({ struct Cyc_Absyn_XenumType_struct* _temp914=( struct Cyc_Absyn_XenumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XenumType_struct)); _temp914->tag= Cyc_Absyn_XenumType_tag;
_temp914->f1= xd->name; _temp914->f2=( struct Cyc_Absyn_Xenumdecl**)({ struct
_tuple11* _temp915=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp915->f1= xd; _temp915;});( void*) _temp914;}); struct Cyc_List_List* ts= ef->typs;*
exist_ref=({ struct Cyc_Core_Opt* _temp916=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp916->v=( void*) exist_typs; _temp916;});
while( es != 0? ts != 0: 0) { struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)
es->hd; void* t= Cyc_Tcutil_substitute( exist_inst,(*(( struct _tuple8*) ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); es= es->tl; ts= ts->tl; if( !
Cyc_Tcutil_coerce_arg( te, e, t)){ Cyc_Tcutil_terr( e->loc,({ struct
_tagged_string _temp917= Cyc_Absynpp_qvar2string( ef->name); struct
_tagged_string _temp918= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp919= Cyc_Absynpp_typ2string(( void*)( e->topt)->v); xprintf("xenum constructor %.*s expects argument of type %.*s but this argument has type %.*s",
_temp917.last_plus_one - _temp917.curr, _temp917.curr, _temp918.last_plus_one -
_temp918.curr, _temp918.curr, _temp919.last_plus_one - _temp919.curr, _temp919.curr);}));}}
if( es != 0){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) es->hd)->loc,({ struct
_tagged_string _temp920= Cyc_Absynpp_qvar2string( ef->name); xprintf("too many arguments for xenum constructor %.*s",
_temp920.last_plus_one - _temp920.curr, _temp920.curr);}));} else{ if( ts != 0){
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*) es->hd)->loc,({ struct _tagged_string
_temp921= Cyc_Absynpp_qvar2string( ef->name); xprintf("too few arguments for xenum constructor %.*s",
_temp921.last_plus_one - _temp921.curr, _temp921.curr);}));}} return res;}
static void* Cyc_Tcexp_tcStmtExp( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
te, s), s, 1); Cyc_ControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));
while( 1) { void* _temp922=( void*) s->r; struct Cyc_Absyn_Exp* _temp932; struct
Cyc_Absyn_Stmt* _temp934; struct Cyc_Absyn_Stmt* _temp936; struct Cyc_Absyn_Stmt*
_temp938; struct Cyc_Absyn_Decl* _temp940; _LL924: if(( unsigned int) _temp922 >
1u?(( struct _enum_struct*) _temp922)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL933:
_temp932=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp922)->f1;
goto _LL925;} else{ goto _LL926;} _LL926: if(( unsigned int) _temp922 > 1u?((
struct _enum_struct*) _temp922)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL937:
_temp936=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp922)->f1;
goto _LL935; _LL935: _temp934=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp922)->f2; goto _LL927;} else{ goto _LL928;} _LL928: if(( unsigned int)
_temp922 > 1u?(( struct _enum_struct*) _temp922)->tag == Cyc_Absyn_Decl_s_tag: 0){
_LL941: _temp940=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp922)->f1; goto _LL939; _LL939: _temp938=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp922)->f2; goto _LL929;} else{ goto _LL930;}
_LL930: goto _LL931; _LL925: return( void*)( _temp932->topt)->v; _LL927: s=
_temp934; continue; _LL929: s= _temp938; continue; _LL931: return Cyc_Tcexp_expr_err(
te, loc,( struct _tagged_string)({ char* _temp942=( char*)"statement expression must end with expression";
struct _tagged_string _temp943; _temp943.curr= _temp942; _temp943.base= _temp942;
_temp943.last_plus_one= _temp942 + 46; _temp943;})); _LL923:;}} static void* Cyc_Tcexp_tcCodegen(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Fndecl* fd){ return Cyc_Tcexp_expr_err( te, loc,( struct
_tagged_string)({ char* _temp944=( char*)"tcCodegen"; struct _tagged_string
_temp945; _temp945.curr= _temp944; _temp945.base= _temp944; _temp945.last_plus_one=
_temp944 + 10; _temp945;}));} static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e){
return Cyc_Tcexp_expr_err( te, loc,( struct _tagged_string)({ char* _temp946=(
char*)"tcFill"; struct _tagged_string _temp947; _temp947.curr= _temp946;
_temp947.base= _temp946; _temp947.last_plus_one= _temp946 + 7; _temp947;}));}
static int Cyc_Tcexp_cnst_exp( struct Cyc_Tcenv_Tenv* te, int var_okay, struct
Cyc_Absyn_Exp* e){ void* _temp948=( void*) e->r; void* _temp980; void* _temp982;
void* _temp984; struct _tuple1* _temp986; struct Cyc_Absyn_Exp* _temp988; struct
Cyc_Absyn_Exp* _temp990; struct Cyc_Absyn_Exp* _temp992; struct Cyc_Absyn_Exp*
_temp994; struct Cyc_Absyn_Exp* _temp996; struct Cyc_List_List* _temp998; struct
Cyc_Absyn_Exp* _temp1000; struct Cyc_Absyn_Exp* _temp1002; void* _temp1004;
struct Cyc_Absyn_Exp* _temp1006; struct Cyc_List_List* _temp1008; int _temp1010;
struct Cyc_Absyn_Structdecl* _temp1012; struct Cyc_List_List* _temp1014; struct
Cyc_Core_Opt* _temp1016; struct _tuple1* _temp1018; struct Cyc_List_List*
_temp1020; void* _temp1022; struct Cyc_List_List* _temp1024; struct Cyc_Absyn_Enumfield*
_temp1026; struct Cyc_Absyn_Enumdecl* _temp1028; struct Cyc_List_List* _temp1030;
struct Cyc_Core_Opt* _temp1032; struct Cyc_Core_Opt* _temp1034; struct Cyc_Absyn_Enumfield*
_temp1036; struct Cyc_Absyn_Xenumdecl* _temp1038; struct Cyc_List_List*
_temp1040; struct Cyc_Core_Opt* _temp1042; _LL950: if((( struct _enum_struct*)
_temp948)->tag == Cyc_Absyn_Const_e_tag){ _LL981: _temp980=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp948)->f1; goto _LL951;} else{ goto _LL952;} _LL952: if((( struct
_enum_struct*) _temp948)->tag == Cyc_Absyn_Sizeof_e_tag){ _LL983: _temp982=(
void*)(( struct Cyc_Absyn_Sizeof_e_struct*) _temp948)->f1; goto _LL953;} else{
goto _LL954;} _LL954: if((( struct _enum_struct*) _temp948)->tag == Cyc_Absyn_Var_e_tag){
_LL987: _temp986=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp948)->f1;
goto _LL985; _LL985: _temp984=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp948)->f2; goto _LL955;} else{ goto _LL956;} _LL956: if((( struct
_enum_struct*) _temp948)->tag == Cyc_Absyn_Conditional_e_tag){ _LL993: _temp992=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp948)->f1;
goto _LL991; _LL991: _temp990=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp948)->f2; goto _LL989; _LL989: _temp988=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp948)->f3; goto _LL957;} else{ goto _LL958;}
_LL958: if((( struct _enum_struct*) _temp948)->tag == Cyc_Absyn_SeqExp_e_tag){
_LL997: _temp996=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp948)->f1; goto _LL995; _LL995: _temp994=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp948)->f2; goto _LL959;} else{ goto _LL960;}
_LL960: if((( struct _enum_struct*) _temp948)->tag == Cyc_Absyn_Instantiate_e_tag){
_LL1001: _temp1000=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp948)->f1; goto _LL999; _LL999: _temp998=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp948)->f2; goto _LL961;} else{ goto _LL962;}
_LL962: if((( struct _enum_struct*) _temp948)->tag == Cyc_Absyn_Cast_e_tag){
_LL1005: _temp1004=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp948)->f1;
goto _LL1003; _LL1003: _temp1002=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp948)->f2; goto _LL963;} else{ goto _LL964;} _LL964: if((( struct
_enum_struct*) _temp948)->tag == Cyc_Absyn_Address_e_tag){ _LL1007: _temp1006=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp948)->f1; goto
_LL965;} else{ goto _LL966;} _LL966: if((( struct _enum_struct*) _temp948)->tag
== Cyc_Absyn_Array_e_tag){ _LL1011: _temp1010=( int)(( struct Cyc_Absyn_Array_e_struct*)
_temp948)->f1; goto _LL1009; _LL1009: _temp1008=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp948)->f2; goto _LL967;} else{ goto _LL968;}
_LL968: if((( struct _enum_struct*) _temp948)->tag == Cyc_Absyn_Struct_e_tag){
_LL1019: _temp1018=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp948)->f1; goto _LL1017; _LL1017: _temp1016=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp948)->f2; goto _LL1015; _LL1015: _temp1014=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp948)->f3; goto
_LL1013; _LL1013: _temp1012=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp948)->f4; goto _LL969;} else{ goto _LL970;} _LL970: if((( struct
_enum_struct*) _temp948)->tag == Cyc_Absyn_Primop_e_tag){ _LL1023: _temp1022=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp948)->f1; goto _LL1021; _LL1021:
_temp1020=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*) _temp948)->f2;
goto _LL971;} else{ goto _LL972;} _LL972: if((( struct _enum_struct*) _temp948)->tag
== Cyc_Absyn_Tuple_e_tag){ _LL1025: _temp1024=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tuple_e_struct*) _temp948)->f1; goto _LL973;} else{ goto _LL974;}
_LL974: if((( struct _enum_struct*) _temp948)->tag == Cyc_Absyn_Enum_e_tag){
_LL1035: _temp1034=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp948)->f1; goto _LL1033; _LL1033: _temp1032=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp948)->f2; goto _LL1031; _LL1031: _temp1030=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_e_struct*) _temp948)->f3; goto
_LL1029; _LL1029: _temp1028=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp948)->f4; goto _LL1027; _LL1027: _temp1026=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp948)->f5; goto _LL975;} else{ goto _LL976;}
_LL976: if((( struct _enum_struct*) _temp948)->tag == Cyc_Absyn_Xenum_e_tag){
_LL1043: _temp1042=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp948)->f1; goto _LL1041; _LL1041: _temp1040=( struct Cyc_List_List*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp948)->f2; goto _LL1039; _LL1039:
_temp1038=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp948)->f3; goto _LL1037; _LL1037: _temp1036=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp948)->f4; goto _LL977;} else{ goto _LL978;}
_LL978: goto _LL979; _LL951: return 1; _LL953: return 1; _LL955: { struct
_handler_cons _temp1044; _push_handler(& _temp1044);{ struct _xenum_struct*
_temp1045=( struct _xenum_struct*) setjmp( _temp1044.handler); if( ! _temp1045){{
void* _temp1046= Cyc_Tcenv_lookup_ordinary( te, e->loc, _temp986); void*
_temp1054; struct Cyc_Absyn_Fndecl* _temp1056; void* _temp1058; struct Cyc_Absyn_Vardecl*
_temp1060; _LL1048: if((( struct _enum_struct*) _temp1046)->tag == Cyc_Tcenv_VarRes_tag){
_LL1055: _temp1054=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp1046)->f1;
if(( unsigned int) _temp1054 > 1u?(( struct _enum_struct*) _temp1054)->tag ==
Cyc_Absyn_Funname_b_tag: 0){ _LL1057: _temp1056=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Funname_b_struct*) _temp1054)->f1; goto _LL1049;} else{ goto
_LL1050;}} else{ goto _LL1050;} _LL1050: if((( struct _enum_struct*) _temp1046)->tag
== Cyc_Tcenv_VarRes_tag){ _LL1059: _temp1058=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp1046)->f1; if(( unsigned int) _temp1058 > 1u?(( struct _enum_struct*)
_temp1058)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL1061: _temp1060=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp1058)->f1; goto _LL1051;} else{ goto
_LL1052;}} else{ goto _LL1052;} _LL1052: goto _LL1053; _LL1049: { int _temp1062=
1; _npop_handler( 0u); return _temp1062;} _LL1051: { void* _temp1063= Cyc_Tcutil_compress((
void*) _temp1060->type); struct Cyc_Absyn_Exp* _temp1069; struct Cyc_Absyn_Tqual*
_temp1071; void* _temp1073; _LL1065: if(( unsigned int) _temp1063 > 5u?(( struct
_enum_struct*) _temp1063)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1074:
_temp1073=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1063)->f1; goto
_LL1072; _LL1072: _temp1071=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1063)->f2; goto _LL1070; _LL1070: _temp1069=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1063)->f3; goto _LL1066;} else{ goto
_LL1067;} _LL1067: goto _LL1068; _LL1066: { int _temp1075= 1; _npop_handler( 0u);
return _temp1075;} _LL1068: { int _temp1076= var_okay; _npop_handler( 0u);
return _temp1076;} _LL1064:;} _LL1053: { int _temp1077= var_okay; _npop_handler(
0u); return _temp1077;} _LL1047:;} _pop_handler();} else{ struct _xenum_struct*
_temp1079= _temp1045; _LL1081: if( _temp1079->tag == Cyc_Dict_Absent_tag){ goto
_LL1082;} else{ goto _LL1083;} _LL1083: goto _LL1084; _LL1082: return 0; _LL1084:(
void) _throw( _temp1079); _LL1080:;}}} _LL957: return( Cyc_Tcexp_cnst_exp( te, 0,
_temp992)? Cyc_Tcexp_cnst_exp( te, 0, _temp990): 0)? Cyc_Tcexp_cnst_exp( te, 0,
_temp988): 0; _LL959: return Cyc_Tcexp_cnst_exp( te, 0, _temp996)? Cyc_Tcexp_cnst_exp(
te, 0, _temp994): 0; _LL961: return Cyc_Tcexp_cnst_exp( te, var_okay, _temp1000);
_LL963: return Cyc_Tcexp_cnst_exp( te, var_okay, _temp1002); _LL965: return Cyc_Tcexp_cnst_exp(
te, 1, _temp1006); _LL967: _temp1014= _temp1008; goto _LL969; _LL969: for( 0;
_temp1014 != 0; _temp1014= _temp1014->tl){ if( ! Cyc_Tcexp_cnst_exp( te, 0,(*((
struct _tuple7*) _temp1014->hd)).f2)){ return 0;}} return 1; _LL971: _temp1024=
_temp1020; goto _LL973; _LL973: _temp1030= _temp1024; goto _LL975; _LL975:
_temp1040= _temp1030; goto _LL977; _LL977: for( 0; _temp1040 != 0; _temp1040=
_temp1040->tl){ if( ! Cyc_Tcexp_cnst_exp( te, 0,( struct Cyc_Absyn_Exp*)
_temp1040->hd)){ return 0;}} return 1; _LL979: return 0; _LL949:;} int Cyc_Tcexp_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){ return Cyc_Tcexp_cnst_exp(
te, 0, e);} void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv* te, void** topt,
struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te, topt, e); t=
Cyc_Tcutil_compress( t);{ void* _temp1085= t; struct Cyc_Absyn_Exp* _temp1091;
struct Cyc_Absyn_Tqual* _temp1093; void* _temp1095; _LL1087: if(( unsigned int)
_temp1085 > 5u?(( struct _enum_struct*) _temp1085)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL1096: _temp1095=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1085)->f1;
goto _LL1094; _LL1094: _temp1093=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1085)->f2; goto _LL1092; _LL1092: _temp1091=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1085)->f3; goto _LL1088;} else{ goto
_LL1089;} _LL1089: goto _LL1090; _LL1088: { void* _temp1099; int _temp1101;
struct _tuple6 _temp1097= Cyc_Tcutil_addressof_props( te, e); _LL1102: _temp1101=
_temp1097.f1; goto _LL1100; _LL1100: _temp1099= _temp1097.f2; goto _LL1098;
_LL1098: t= Cyc_Absyn_atb_typ( _temp1095, _temp1099, _temp1093,({ struct Cyc_Absyn_Upper_b_struct*
_temp1103=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1103->tag= Cyc_Absyn_Upper_b_tag; _temp1103->f1= _temp1091;( void*)
_temp1103;})); Cyc_Tcutil_unchecked_cast( te, e, t); return t;} _LL1090: return
t; _LL1086:;}} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te, topt, e);
void* _temp1104=( void*) e->r; struct Cyc_List_List* _temp1112; int _temp1114;
void* _temp1116; struct _tagged_string _temp1118; int _temp1120; _LL1106: if(((
struct _enum_struct*) _temp1104)->tag == Cyc_Absyn_Array_e_tag){ _LL1115:
_temp1114=( int)(( struct Cyc_Absyn_Array_e_struct*) _temp1104)->f1; goto
_LL1113; _LL1113: _temp1112=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp1104)->f2; goto _LL1107;} else{ goto _LL1108;} _LL1108: if((( struct
_enum_struct*) _temp1104)->tag == Cyc_Absyn_Const_e_tag){ _LL1117: _temp1116=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1104)->f1; if(( unsigned int)
_temp1116 > 1u?(( struct _enum_struct*) _temp1116)->tag == Cyc_Absyn_String_c_tag:
0){ _LL1121: _temp1120=( int)(( struct Cyc_Absyn_String_c_struct*) _temp1116)->f1;
goto _LL1119; _LL1119: _temp1118=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1116)->f2; goto _LL1109;} else{ goto _LL1110;}} else{ goto _LL1110;}
_LL1110: goto _LL1111; _LL1107: return t; _LL1109: return t; _LL1111: t= Cyc_Tcutil_compress(
t);{ void* _temp1122= t; struct Cyc_Absyn_Exp* _temp1128; struct Cyc_Absyn_Tqual*
_temp1130; void* _temp1132; _LL1124: if(( unsigned int) _temp1122 > 5u?(( struct
_enum_struct*) _temp1122)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL1133:
_temp1132=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1122)->f1; goto
_LL1131; _LL1131: _temp1130=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1122)->f2; goto _LL1129; _LL1129: _temp1128=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1122)->f3; goto _LL1125;} else{ goto
_LL1126;} _LL1126: goto _LL1127; _LL1125: { void* _temp1136; int _temp1138;
struct _tuple6 _temp1134= Cyc_Tcutil_addressof_props( te, e); _LL1139: _temp1138=
_temp1134.f1; goto _LL1137; _LL1137: _temp1136= _temp1134.f2; goto _LL1135;
_LL1135: t= Cyc_Absyn_atb_typ( _temp1132, _temp1136, _temp1130,({ struct Cyc_Absyn_Upper_b_struct*
_temp1140=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1140->tag= Cyc_Absyn_Upper_b_tag; _temp1140->f1= _temp1128;( void*)
_temp1140;})); Cyc_Tcutil_unchecked_cast( te, e, t); return t;} _LL1127: return
t; _LL1123:;} _LL1105:;} static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){{ void* _temp1141=( void*) e->r;
struct Cyc_Absyn_Exp* _temp1147; _LL1143: if((( struct _enum_struct*) _temp1141)->tag
== Cyc_Absyn_NoInstantiate_e_tag){ _LL1148: _temp1147=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1141)->f1; goto _LL1144;} else{
goto _LL1145;} _LL1145: goto _LL1146; _LL1144: Cyc_Tcexp_tcExpNoInst( te, topt,
_temp1147); e->topt= _temp1147->topt; goto _LL1142; _LL1146: Cyc_Tcexp_tcExpNoInst(
te, topt, e);( void*)(( e->topt)->v=( void*) Cyc_Absyn_pointer_expand( Cyc_Tcutil_compress((
void*)( e->topt)->v)));{ void* _temp1149= Cyc_Tcutil_compress(( void*)( e->topt)->v);
struct Cyc_Absyn_PtrInfo _temp1155; struct Cyc_Absyn_Conref* _temp1157; struct
Cyc_Absyn_Tqual* _temp1159; struct Cyc_Absyn_Conref* _temp1161; void* _temp1163;
void* _temp1165; _LL1151: if(( unsigned int) _temp1149 > 5u?(( struct
_enum_struct*) _temp1149)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL1156:
_temp1155=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp1149)->f1; _LL1166: _temp1165=( void*) _temp1155.elt_typ; goto _LL1164;
_LL1164: _temp1163=( void*) _temp1155.rgn_typ; goto _LL1162; _LL1162: _temp1161=(
struct Cyc_Absyn_Conref*) _temp1155.nullable; goto _LL1160; _LL1160: _temp1159=(
struct Cyc_Absyn_Tqual*) _temp1155.tq; goto _LL1158; _LL1158: _temp1157=( struct
Cyc_Absyn_Conref*) _temp1155.bounds; goto _LL1152;} else{ goto _LL1153;} _LL1153:
goto _LL1154; _LL1152:{ void* _temp1167= Cyc_Tcutil_compress( _temp1165); struct
Cyc_Absyn_FnInfo _temp1173; int _temp1175; struct Cyc_List_List* _temp1177; void*
_temp1179; struct Cyc_Core_Opt* _temp1181; struct Cyc_List_List* _temp1183;
_LL1169: if(( unsigned int) _temp1167 > 5u?(( struct _enum_struct*) _temp1167)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL1174: _temp1173=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp1167)->f1; _LL1184: _temp1183=( struct Cyc_List_List*)
_temp1173.tvars; goto _LL1182; _LL1182: _temp1181=( struct Cyc_Core_Opt*)
_temp1173.effect; goto _LL1180; _LL1180: _temp1179=( void*) _temp1173.ret_typ;
goto _LL1178; _LL1178: _temp1177=( struct Cyc_List_List*) _temp1173.args; goto
_LL1176; _LL1176: _temp1175=( int) _temp1173.varargs; goto _LL1170;} else{ goto
_LL1171;} _LL1171: goto _LL1172; _LL1170: if( _temp1183 != 0){ struct Cyc_List_List*
inst=(( struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp1183);
struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*),
struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd,
inst); void* ftyp= Cyc_Tcutil_substitute( inst,({ struct Cyc_Absyn_FnType_struct*
_temp1185=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1185->tag= Cyc_Absyn_FnType_tag; _temp1185->f1=({ struct Cyc_Absyn_FnInfo
_temp1186; _temp1186.tvars= 0; _temp1186.effect= _temp1181; _temp1186.ret_typ=(
void*) _temp1179; _temp1186.args= _temp1177; _temp1186.varargs= _temp1175;
_temp1186;});( void*) _temp1185;})); void* new_typ=({ struct Cyc_Absyn_PointerType_struct*
_temp1187=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1187->tag= Cyc_Absyn_PointerType_tag; _temp1187->f1=({ struct Cyc_Absyn_PtrInfo
_temp1188; _temp1188.elt_typ=( void*) ftyp; _temp1188.rgn_typ=( void*) _temp1163;
_temp1188.nullable= _temp1161; _temp1188.tq= _temp1159; _temp1188.bounds=
_temp1157; _temp1188;});( void*) _temp1187;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp(
e);( void*)( e->r=( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1189=(
struct Cyc_Absyn_Instantiate_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct));
_temp1189->tag= Cyc_Absyn_Instantiate_e_tag; _temp1189->f1= inner; _temp1189->f2=
ts;( void*) _temp1189;})); e->topt=({ struct Cyc_Core_Opt* _temp1190=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1190->v=( void*)
new_typ; _temp1190;});} goto _LL1168; _LL1172: goto _LL1168; _LL1168:;} goto
_LL1150; _LL1154: goto _LL1150; _LL1150:;} goto _LL1142; _LL1142:;} return( void*)(
e->topt)->v;} static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc; void*
t;{ void* _temp1191=( void*) e->r; struct Cyc_Absyn_Exp* _temp1255; struct
_tuple1* _temp1257; struct Cyc_List_List* _temp1259; struct Cyc_Absyn_Exp*
_temp1261; struct Cyc_List_List* _temp1263; struct Cyc_Core_Opt* _temp1265; void*
_temp1267; void* _temp1269; struct _tuple1* _temp1271; struct Cyc_List_List*
_temp1273; void* _temp1275; void* _temp1277; struct Cyc_Absyn_Exp* _temp1279;
struct Cyc_Absyn_Exp* _temp1281; struct Cyc_Core_Opt* _temp1283; struct Cyc_Absyn_Exp*
_temp1285; struct Cyc_Absyn_Exp* _temp1287; struct Cyc_Absyn_Exp* _temp1289;
struct Cyc_Absyn_Exp* _temp1291; struct Cyc_Absyn_Exp* _temp1293; struct Cyc_Absyn_Exp*
_temp1295; struct Cyc_List_List* _temp1297; struct Cyc_Absyn_Exp* _temp1299;
struct Cyc_Absyn_Exp* _temp1301; struct Cyc_List_List* _temp1303; struct Cyc_Absyn_Exp*
_temp1305; struct Cyc_Absyn_Exp* _temp1307; void* _temp1309; struct Cyc_Absyn_Exp*
_temp1311; void* _temp1313; struct Cyc_Absyn_Exp* _temp1315; struct
_tagged_string* _temp1317; struct Cyc_Absyn_Exp* _temp1319; struct
_tagged_string* _temp1321; struct Cyc_Absyn_Exp* _temp1323; struct Cyc_Absyn_Exp*
_temp1325; struct Cyc_Absyn_Exp* _temp1327; struct Cyc_List_List* _temp1329;
struct Cyc_List_List* _temp1331; struct _tuple2* _temp1333; struct Cyc_List_List*
_temp1335; int _temp1337; struct Cyc_Absyn_Stmt* _temp1339; struct Cyc_Absyn_Fndecl*
_temp1341; struct Cyc_Absyn_Exp* _temp1343; struct Cyc_Absyn_Exp* _temp1345;
struct Cyc_Absyn_Exp* _temp1347; struct Cyc_Absyn_Vardecl* _temp1349; struct Cyc_Absyn_Structdecl*
_temp1351; struct Cyc_Absyn_Structdecl** _temp1353; struct Cyc_List_List*
_temp1354; struct Cyc_Core_Opt* _temp1356; struct Cyc_Core_Opt** _temp1358;
struct _tuple1* _temp1359; struct _tuple1** _temp1361; struct Cyc_Absyn_Enumfield*
_temp1362; struct Cyc_Absyn_Enumdecl* _temp1364; struct Cyc_List_List* _temp1366;
struct Cyc_Core_Opt* _temp1368; struct Cyc_Core_Opt** _temp1370; struct Cyc_Core_Opt*
_temp1371; struct Cyc_Core_Opt** _temp1373; struct Cyc_Absyn_Enumfield*
_temp1374; struct Cyc_Absyn_Xenumdecl* _temp1376; struct Cyc_List_List*
_temp1378; struct Cyc_Core_Opt* _temp1380; struct Cyc_Core_Opt** _temp1382;
_LL1193: if((( struct _enum_struct*) _temp1191)->tag == Cyc_Absyn_NoInstantiate_e_tag){
_LL1256: _temp1255=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1191)->f1; goto _LL1194;} else{ goto _LL1195;} _LL1195: if((( struct
_enum_struct*) _temp1191)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL1258: _temp1257=(
struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1191)->f1; goto
_LL1196;} else{ goto _LL1197;} _LL1197: if((( struct _enum_struct*) _temp1191)->tag
== Cyc_Absyn_UnknownCall_e_tag){ _LL1262: _temp1261=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp1191)->f1; goto _LL1260; _LL1260:
_temp1259=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1191)->f2; goto _LL1198;} else{ goto _LL1199;} _LL1199: if((( struct
_enum_struct*) _temp1191)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL1266:
_temp1265=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1191)->f1; goto _LL1264; _LL1264: _temp1263=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp1191)->f2; goto _LL1200;} else{
goto _LL1201;} _LL1201: if((( struct _enum_struct*) _temp1191)->tag == Cyc_Absyn_Const_e_tag){
_LL1268: _temp1267=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1191)->f1;
goto _LL1202;} else{ goto _LL1203;} _LL1203: if((( struct _enum_struct*)
_temp1191)->tag == Cyc_Absyn_Var_e_tag){ _LL1272: _temp1271=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp1191)->f1; goto _LL1270; _LL1270: _temp1269=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1191)->f2; goto _LL1204;} else{
goto _LL1205;} _LL1205: if((( struct _enum_struct*) _temp1191)->tag == Cyc_Absyn_Primop_e_tag){
_LL1276: _temp1275=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1191)->f1;
goto _LL1274; _LL1274: _temp1273=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1191)->f2; goto _LL1206;} else{ goto _LL1207;} _LL1207: if((( struct
_enum_struct*) _temp1191)->tag == Cyc_Absyn_Increment_e_tag){ _LL1280: _temp1279=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*) _temp1191)->f1;
goto _LL1278; _LL1278: _temp1277=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1191)->f2; goto _LL1208;} else{ goto _LL1209;} _LL1209: if((( struct
_enum_struct*) _temp1191)->tag == Cyc_Absyn_AssignOp_e_tag){ _LL1286: _temp1285=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp1191)->f1;
goto _LL1284; _LL1284: _temp1283=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1191)->f2; goto _LL1282; _LL1282: _temp1281=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_AssignOp_e_struct*) _temp1191)->f3; goto _LL1210;} else{ goto
_LL1211;} _LL1211: if((( struct _enum_struct*) _temp1191)->tag == Cyc_Absyn_Conditional_e_tag){
_LL1292: _temp1291=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1191)->f1; goto _LL1290; _LL1290: _temp1289=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp1191)->f2; goto _LL1288; _LL1288:
_temp1287=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1191)->f3; goto _LL1212;} else{ goto _LL1213;} _LL1213: if((( struct
_enum_struct*) _temp1191)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL1296: _temp1295=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp1191)->f1; goto
_LL1294; _LL1294: _temp1293=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1191)->f2; goto _LL1214;} else{ goto _LL1215;} _LL1215: if((( struct
_enum_struct*) _temp1191)->tag == Cyc_Absyn_FnCall_e_tag){ _LL1300: _temp1299=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp1191)->f1; goto
_LL1298; _LL1298: _temp1297=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1191)->f2; goto _LL1216;} else{ goto _LL1217;} _LL1217: if((( struct
_enum_struct*) _temp1191)->tag == Cyc_Absyn_Throw_e_tag){ _LL1302: _temp1301=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp1191)->f1; goto
_LL1218;} else{ goto _LL1219;} _LL1219: if((( struct _enum_struct*) _temp1191)->tag
== Cyc_Absyn_Instantiate_e_tag){ _LL1306: _temp1305=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp1191)->f1; goto _LL1304; _LL1304:
_temp1303=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1191)->f2; goto _LL1220;} else{ goto _LL1221;} _LL1221: if((( struct
_enum_struct*) _temp1191)->tag == Cyc_Absyn_Cast_e_tag){ _LL1310: _temp1309=(
void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1191)->f1; goto _LL1308; _LL1308:
_temp1307=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp1191)->f2;
goto _LL1222;} else{ goto _LL1223;} _LL1223: if((( struct _enum_struct*)
_temp1191)->tag == Cyc_Absyn_Address_e_tag){ _LL1312: _temp1311=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp1191)->f1; goto _LL1224;} else{ goto
_LL1225;} _LL1225: if((( struct _enum_struct*) _temp1191)->tag == Cyc_Absyn_Sizeof_e_tag){
_LL1314: _temp1313=( void*)(( struct Cyc_Absyn_Sizeof_e_struct*) _temp1191)->f1;
goto _LL1226;} else{ goto _LL1227;} _LL1227: if((( struct _enum_struct*)
_temp1191)->tag == Cyc_Absyn_Deref_e_tag){ _LL1316: _temp1315=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp1191)->f1; goto _LL1228;} else{ goto
_LL1229;} _LL1229: if((( struct _enum_struct*) _temp1191)->tag == Cyc_Absyn_StructMember_e_tag){
_LL1320: _temp1319=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1191)->f1; goto _LL1318; _LL1318: _temp1317=( struct _tagged_string*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp1191)->f2; goto _LL1230;} else{
goto _LL1231;} _LL1231: if((( struct _enum_struct*) _temp1191)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL1324: _temp1323=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1191)->f1; goto _LL1322; _LL1322: _temp1321=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp1191)->f2; goto _LL1232;} else{
goto _LL1233;} _LL1233: if((( struct _enum_struct*) _temp1191)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1328: _temp1327=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1191)->f1; goto _LL1326; _LL1326: _temp1325=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp1191)->f2; goto _LL1234;} else{ goto
_LL1235;} _LL1235: if((( struct _enum_struct*) _temp1191)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1330: _temp1329=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1191)->f1; goto _LL1236;} else{ goto _LL1237;} _LL1237: if((( struct
_enum_struct*) _temp1191)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1334:
_temp1333=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp1191)->f1;
goto _LL1332; _LL1332: _temp1331=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1191)->f2; goto _LL1238;} else{ goto _LL1239;} _LL1239: if((( struct
_enum_struct*) _temp1191)->tag == Cyc_Absyn_Array_e_tag){ _LL1338: _temp1337=(
int)(( struct Cyc_Absyn_Array_e_struct*) _temp1191)->f1; goto _LL1336; _LL1336:
_temp1335=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp1191)->f2;
goto _LL1240;} else{ goto _LL1241;} _LL1241: if((( struct _enum_struct*)
_temp1191)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL1340: _temp1339=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_StmtExp_e_struct*) _temp1191)->f1; goto _LL1242;} else{ goto
_LL1243;} _LL1243: if((( struct _enum_struct*) _temp1191)->tag == Cyc_Absyn_Codegen_e_tag){
_LL1342: _temp1341=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1191)->f1; goto _LL1244;} else{ goto _LL1245;} _LL1245: if((( struct
_enum_struct*) _temp1191)->tag == Cyc_Absyn_Fill_e_tag){ _LL1344: _temp1343=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp1191)->f1; goto
_LL1246;} else{ goto _LL1247;} _LL1247: if((( struct _enum_struct*) _temp1191)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL1350: _temp1349=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1191)->f1; goto _LL1348; _LL1348:
_temp1347=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1191)->f2; goto _LL1346; _LL1346: _temp1345=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp1191)->f3; goto _LL1248;} else{
goto _LL1249;} _LL1249: if((( struct _enum_struct*) _temp1191)->tag == Cyc_Absyn_Struct_e_tag){
_LL1360: _temp1359=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1191)->f1; _temp1361=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1191)->f1;
goto _LL1357; _LL1357: _temp1356=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1191)->f2; _temp1358=&(( struct Cyc_Absyn_Struct_e_struct*) _temp1191)->f2;
goto _LL1355; _LL1355: _temp1354=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp1191)->f3; goto _LL1352; _LL1352: _temp1351=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_e_struct*) _temp1191)->f4; _temp1353=&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1191)->f4; goto _LL1250;} else{ goto _LL1251;} _LL1251: if((( struct
_enum_struct*) _temp1191)->tag == Cyc_Absyn_Enum_e_tag){ _LL1372: _temp1371=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp1191)->f1;
_temp1373=&(( struct Cyc_Absyn_Enum_e_struct*) _temp1191)->f1; goto _LL1369;
_LL1369: _temp1368=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1191)->f2; _temp1370=&(( struct Cyc_Absyn_Enum_e_struct*) _temp1191)->f2;
goto _LL1367; _LL1367: _temp1366=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1191)->f3; goto _LL1365; _LL1365: _temp1364=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_e_struct*) _temp1191)->f4; goto _LL1363; _LL1363:
_temp1362=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp1191)->f5; goto _LL1252;} else{ goto _LL1253;} _LL1253: if((( struct
_enum_struct*) _temp1191)->tag == Cyc_Absyn_Xenum_e_tag){ _LL1381: _temp1380=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp1191)->f1;
_temp1382=&(( struct Cyc_Absyn_Xenum_e_struct*) _temp1191)->f1; goto _LL1379;
_LL1379: _temp1378=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp1191)->f2; goto _LL1377; _LL1377: _temp1376=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp1191)->f3; goto _LL1375; _LL1375:
_temp1374=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp1191)->f4; goto _LL1254;} else{ goto _LL1192;} _LL1194: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1255); return; _LL1196: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1257); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1198: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1261, _temp1259); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1200: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1263); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1202: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1267, e);
goto _LL1192; _LL1204: t= Cyc_Tcexp_tcVar( te, loc, _temp1271, _temp1269); goto
_LL1192; _LL1206: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1275, _temp1273);
goto _LL1192; _LL1208: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1279,
_temp1277); goto _LL1192; _LL1210: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1285, _temp1283, _temp1281); goto _LL1192; _LL1212: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1291, _temp1289, _temp1287); goto _LL1192; _LL1214: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1295, _temp1293); goto _LL1192; _LL1216: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1299, _temp1297); goto _LL1192; _LL1218: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1301); goto _LL1192; _LL1220: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1305, _temp1303); goto _LL1192; _LL1222: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1309, _temp1307); goto _LL1192; _LL1224: t= Cyc_Tcexp_tcAddress(
te, loc, topt, _temp1311); goto _LL1192; _LL1226: t= Cyc_Tcexp_tcSizeof( te, loc,
topt, _temp1313); goto _LL1192; _LL1228: t= Cyc_Tcexp_tcDeref( te, loc, topt,
_temp1315); goto _LL1192; _LL1230: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e,
_temp1319, _temp1317); goto _LL1192; _LL1232: t= Cyc_Tcexp_tcStructArrow( te,
loc, topt, _temp1323, _temp1321); goto _LL1192; _LL1234: t= Cyc_Tcexp_tcSubscript(
te, loc, topt, _temp1327, _temp1325); goto _LL1192; _LL1236: t= Cyc_Tcexp_tcTuple(
te, loc, topt, _temp1329); goto _LL1192; _LL1238: t= Cyc_Tcexp_tcCompoundLit( te,
loc, topt, _temp1333, _temp1331); goto _LL1192; _LL1240: t= Cyc_Tcexp_tcArray(
te, loc, topt, e, _temp1337, _temp1335); goto _LL1192; _LL1242: t= Cyc_Tcexp_tcStmtExp(
te, loc, topt, _temp1339); goto _LL1192; _LL1244: t= Cyc_Tcexp_tcCodegen( te,
loc, topt, _temp1341); goto _LL1192; _LL1246: t= Cyc_Tcexp_tcFill( te, loc, topt,
_temp1343); goto _LL1192; _LL1248: t= Cyc_Tcexp_tcComprehension( te, loc, topt,
_temp1349, _temp1347, _temp1345); goto _LL1192; _LL1250: t= Cyc_Tcexp_tcStruct(
te, loc, topt, _temp1361, _temp1358, _temp1354, _temp1353); goto _LL1192;
_LL1252: t= Cyc_Tcexp_tcEnum( te, loc, topt, _temp1373, _temp1370, _temp1366,
_temp1364, _temp1362); goto _LL1192; _LL1254: t= Cyc_Tcexp_tcXenum( te, loc,
topt, _temp1382, _temp1378, _temp1376, _temp1374); goto _LL1192; _LL1192:;} e->topt=({
struct Cyc_Core_Opt* _temp1383=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp1383->v=( void*) t; _temp1383;});}