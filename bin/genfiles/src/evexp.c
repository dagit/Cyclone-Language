#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2;}; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3;}; struct _tuple2{ struct
Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2;}; struct _tuple3{ struct Cyc_Absyn_Tqual*
f1; void* f2;}; struct _tuple4{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual*
f2; void* f3;}; struct _tuple5{ void* f1; void* f2;}; struct _tuple6{ void* f1;
int f2;}; typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef
struct _tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t;
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void*
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v;}; typedef struct
Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u]; struct
Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1;}; extern char
Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char* tag; struct
_tagged_string f1;}; extern char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{
char* tag; struct _tagged_string f1;}; extern char Cyc_Core_Not_found_tag[ 10u];
struct Cyc_Core_Not_found_struct{ char* tag;}; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1;};
extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_List_List{ void*
hd; struct Cyc_List_List* tl;}; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag;}; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag;}; extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{ char*
tag;}; struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct
_tagged_string line; int line_no; int col;}; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t;
extern char Cyc_Position_Exit_tag[ 5u]; struct Cyc_Position_Exit_struct{ char*
tag;}; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
typedef void* Cyc_Position_Error_kind; extern void* Cyc_Position_Lex; extern
void* Cyc_Position_Parse; extern void* Cyc_Position_Elab; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc;}; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*, struct _tagged_string); extern char Cyc_Position_Nocontext_tag[
10u]; struct Cyc_Position_Nocontext_struct{ char* tag;}; extern void Cyc_Position_post_error(
struct Cyc_Position_Error*); typedef struct _tagged_string* Cyc_Absyn_field_name;
typedef struct _tagged_string* Cyc_Absyn_var; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
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
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Enumdecl**
f3;}; extern const int Cyc_Absyn_XenumType_tag; struct Cyc_Absyn_XenumType_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Xenumdecl** f2;}; extern const int
Cyc_Absyn_PointerType_tag; struct Cyc_Absyn_PointerType_struct{ int tag; struct
Cyc_Absyn_PtrInfo f1;}; extern const int Cyc_Absyn_IntType_tag; struct Cyc_Absyn_IntType_struct{
int tag; void* f1; void* f2;}; extern void* Cyc_Absyn_FloatType; extern void*
Cyc_Absyn_DoubleType; extern const int Cyc_Absyn_ArrayType_tag; struct Cyc_Absyn_ArrayType_struct{
int tag; void* f1; struct Cyc_Absyn_Tqual* f2; struct Cyc_Absyn_Exp* f3;};
extern const int Cyc_Absyn_FnType_tag; struct Cyc_Absyn_FnType_struct{ int tag;
struct Cyc_Absyn_FnInfo f1;}; extern const int Cyc_Absyn_TupleType_tag; struct
Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1;}; extern const
int Cyc_Absyn_StructType_tag; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;};
extern void* Cyc_Absyn_UnionType; extern const int Cyc_Absyn_RgnHandleType_tag;
struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1;}; extern const int Cyc_Absyn_TypedefType_tag;
struct Cyc_Absyn_TypedefType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
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
struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple0* f1; void* f2;}; extern
const int Cyc_Absyn_UnknownId_e_tag; struct Cyc_Absyn_UnknownId_e_struct{ int
tag; struct _tuple0* f1;}; extern const int Cyc_Absyn_Primop_e_tag; struct Cyc_Absyn_Primop_e_struct{
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
struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2;}; extern const int Cyc_Absyn_Array_e_tag; struct Cyc_Absyn_Array_e_struct{
int tag; int f1; struct Cyc_List_List* f2;}; extern const int Cyc_Absyn_Comprehension_e_tag;
struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3;}; extern const int Cyc_Absyn_Struct_e_tag;
struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt*
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
struct Cyc_Absyn_While_s_struct{ int tag; struct _tuple2 f1; struct Cyc_Absyn_Stmt*
f2;}; extern const int Cyc_Absyn_Break_s_tag; struct Cyc_Absyn_Break_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1;}; extern const int Cyc_Absyn_Continue_s_tag;
struct Cyc_Absyn_Continue_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1;}; extern
const int Cyc_Absyn_Goto_s_tag; struct Cyc_Absyn_Goto_s_struct{ int tag; struct
_tagged_string* f1; struct Cyc_Absyn_Stmt* f2;}; extern const int Cyc_Absyn_For_s_tag;
struct Cyc_Absyn_For_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct _tuple2
f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt* f4;}; extern const int Cyc_Absyn_Switch_s_tag;
struct Cyc_Absyn_Switch_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List*
f2;}; extern const int Cyc_Absyn_Fallthru_s_tag; struct Cyc_Absyn_Fallthru_s_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Absyn_Switch_clause** f2;}; extern
const int Cyc_Absyn_Decl_s_tag; struct Cyc_Absyn_Decl_s_struct{ int tag; struct
Cyc_Absyn_Decl* f1; struct Cyc_Absyn_Stmt* f2;}; extern const int Cyc_Absyn_Cut_s_tag;
struct Cyc_Absyn_Cut_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1;}; extern
const int Cyc_Absyn_Splice_s_tag; struct Cyc_Absyn_Splice_s_struct{ int tag;
struct Cyc_Absyn_Stmt* f1;}; extern const int Cyc_Absyn_Label_s_tag; struct Cyc_Absyn_Label_s_struct{
int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2;}; extern const
int Cyc_Absyn_Do_s_tag; struct Cyc_Absyn_Do_s_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; struct _tuple2 f2;}; extern const int Cyc_Absyn_TryCatch_s_tag; struct Cyc_Absyn_TryCatch_s_struct{
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
Cyc_Absyn_Enum_p_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; struct Cyc_Absyn_Enumdecl*
f5; struct Cyc_Absyn_Enumfield* f6;}; extern const int Cyc_Absyn_Xenum_p_tag;
struct Cyc_Absyn_Xenum_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Xenumdecl* f4; struct Cyc_Absyn_Enumfield*
f5;}; extern const int Cyc_Absyn_UnknownId_p_tag; struct Cyc_Absyn_UnknownId_p_struct{
int tag; struct _tuple0* f1;}; extern const int Cyc_Absyn_UnknownCall_p_tag;
struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3;}; extern const int Cyc_Absyn_UnknownFields_p_tag;
struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
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
tvs; void* defn;}; extern const int Cyc_Absyn_Var_d_tag; struct Cyc_Absyn_Var_d_struct{
int tag; struct Cyc_Absyn_Vardecl* f1;}; extern const int Cyc_Absyn_Fn_d_tag;
struct Cyc_Absyn_Fn_d_struct{ int tag; struct Cyc_Absyn_Fndecl* f1;}; extern
const int Cyc_Absyn_Let_d_tag; struct Cyc_Absyn_Let_d_struct{ int tag; struct
Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt* f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp*
f4; int f5;}; extern const int Cyc_Absyn_Struct_d_tag; struct Cyc_Absyn_Struct_d_struct{
int tag; struct Cyc_Absyn_Structdecl* f1;}; extern void* Cyc_Absyn_Union_d;
extern const int Cyc_Absyn_Enum_d_tag; struct Cyc_Absyn_Enum_d_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1;}; extern const int Cyc_Absyn_Xenum_d_tag; struct
Cyc_Absyn_Xenum_d_struct{ int tag; struct Cyc_Absyn_Xenumdecl* f1;}; extern
const int Cyc_Absyn_Typedef_d_tag; struct Cyc_Absyn_Typedef_d_struct{ int tag;
struct Cyc_Absyn_Typedefdecl* f1;}; extern const int Cyc_Absyn_Namespace_d_tag;
struct Cyc_Absyn_Namespace_d_struct{ int tag; struct _tagged_string* f1; struct
Cyc_List_List* f2;}; extern const int Cyc_Absyn_Using_d_tag; struct Cyc_Absyn_Using_d_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2;}; extern const int Cyc_Absyn_ExternC_d_tag;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1;}; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc;}; extern const int
Cyc_Absyn_ArrayElement_tag; struct Cyc_Absyn_ArrayElement_struct{ int tag;
struct Cyc_Absyn_Exp* f1;}; extern const int Cyc_Absyn_FieldName_tag; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_string* f1;}; extern char Cyc_Absyn_EmptyAnnot_tag[ 11u];
struct Cyc_Absyn_EmptyAnnot_struct{ char* tag;}; extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref* x); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE
Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1;}; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag;}; struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string( void*);
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{ char* tag;}; struct
Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct
Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[ 8u]; struct
Cyc_Dict_Present_struct{ char* tag;}; extern char Cyc_Dict_Absent_tag[ 7u];
typedef void* Cyc_Tcenv_Resolved; extern const int Cyc_Tcenv_VarRes_tag; struct
Cyc_Tcenv_VarRes_struct{ int tag; void* f1;}; extern const int Cyc_Tcenv_StructRes_tag;
struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;};
extern const int Cyc_Tcenv_EnumRes_tag; struct Cyc_Tcenv_EnumRes_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2;}; extern const
int Cyc_Tcenv_XenumRes_tag; struct Cyc_Tcenv_XenumRes_struct{ int tag; struct
Cyc_Absyn_Xenumdecl* f1; struct Cyc_Absyn_Enumfield* f2;}; typedef void* Cyc_Tcenv_resolved_t;
struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict*
structdecls; struct Cyc_Dict_Dict* enumdecls; struct Cyc_Dict_Dict* xenumdecls;
struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List*
availables;}; typedef struct Cyc_Tcenv_Genv* Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv;
typedef struct Cyc_Tcenv_Fenv* Cyc_Tcenv_fenv_t; typedef void* Cyc_Tcenv_Jumpee;
extern void* Cyc_Tcenv_NotLoop_j; extern void* Cyc_Tcenv_CaseEnd_j; extern void*
Cyc_Tcenv_FnEnd_j; extern const int Cyc_Tcenv_Stmt_j_tag; struct Cyc_Tcenv_Stmt_j_struct{
int tag; struct Cyc_Absyn_Stmt* f1;}; typedef void* Cyc_Tcenv_jumpee_t; typedef
void* Cyc_Tcenv_Frames; extern const int Cyc_Tcenv_Outermost_tag; struct Cyc_Tcenv_Outermost_struct{
int tag; void* f1;}; extern const int Cyc_Tcenv_Frame_tag; struct Cyc_Tcenv_Frame_struct{
int tag; void* f1; void* f2;}; extern const int Cyc_Tcenv_Hidden_tag; struct Cyc_Tcenv_Hidden_struct{
int tag; void* f1; void* f2;}; typedef void* Cyc_Tcenv_frames; struct Cyc_Tcenv_Tenv{
struct Cyc_List_List* ns; struct Cyc_Dict_Dict* ae; struct Cyc_Core_Opt* le;};
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv; typedef struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tenv_t; extern char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{
char* tag;}; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern void
Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_string); extern
void* Cyc_Tcutil_compress( void* t); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_szof( void* t, struct
Cyc_Position_Segment* loc); void Cyc_Evexp_exp_err( struct Cyc_Position_Segment*
loc, struct _tagged_string msg){ Cyc_Position_post_error( Cyc_Position_mk_err_elab(
loc,({ struct _tagged_string _temp0= msg; xprintf("Exp_err: %.*s", _temp0.last_plus_one
- _temp0.curr, _temp0.curr);})));} extern void* Cyc_Evexp_promote_const( void*
cn); extern void* Cyc_Evexp_eval_const_exp( struct Cyc_Absyn_Exp* e); extern
void* Cyc_Evexp_eval_const_unprimop( void* p, struct Cyc_Absyn_Exp* e); extern
void* Cyc_Evexp_eval_const_binprimop( void* p, struct Cyc_Absyn_Exp* e1, struct
Cyc_Absyn_Exp* e2); extern void* Cyc_Evexp_eval_sizeof( void* t, struct Cyc_Position_Segment*
loc); unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e){ void*
_temp1= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp( e)); int _temp7; void*
_temp9; _LL3: if(( unsigned int) _temp1 > 1u?(( struct _enum_struct*) _temp1)->tag
== Cyc_Absyn_Int_c_tag: 0){ _LL10: _temp9=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp1)->f1; goto _LL8; _LL8: _temp7=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp1)->f2; goto _LL4;} else{ goto _LL5;} _LL5: goto _LL6; _LL4: return(
unsigned int) _temp7; _LL6: Cyc_Evexp_exp_err( e->loc,( struct _tagged_string)({
char* _temp11=( char*)"expecting unsigned int"; struct _tagged_string _temp12;
_temp12.curr= _temp11; _temp12.base= _temp11; _temp12.last_plus_one= _temp11 +
23; _temp12;})); return 0u; _LL2:;} void* Cyc_Evexp_promote_const( void* cn){
void* _temp13= cn; char _temp21; void* _temp23; short _temp25; void* _temp27;
_LL15: if(( unsigned int) _temp13 > 1u?(( struct _enum_struct*) _temp13)->tag ==
Cyc_Absyn_Char_c_tag: 0){ _LL24: _temp23=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp13)->f1; goto _LL22; _LL22: _temp21=( char)(( struct Cyc_Absyn_Char_c_struct*)
_temp13)->f2; goto _LL16;} else{ goto _LL17;} _LL17: if(( unsigned int) _temp13
> 1u?(( struct _enum_struct*) _temp13)->tag == Cyc_Absyn_Short_c_tag: 0){ _LL28:
_temp27=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp13)->f1; goto _LL26;
_LL26: _temp25=( short)(( struct Cyc_Absyn_Short_c_struct*) _temp13)->f2; goto
_LL18;} else{ goto _LL19;} _LL19: goto _LL20; _LL16: return({ struct Cyc_Absyn_Int_c_struct*
_temp29=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp29->tag= Cyc_Absyn_Int_c_tag; _temp29->f1=( void*) _temp23; _temp29->f2=(
int) _temp21;( void*) _temp29;}); _LL18: return({ struct Cyc_Absyn_Int_c_struct*
_temp30=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp30->tag= Cyc_Absyn_Int_c_tag; _temp30->f1=( void*) _temp27; _temp30->f2=(
int) _temp25;( void*) _temp30;}); _LL20: return cn; _LL14:;} int Cyc_Evexp_eval_const_bool_exp(
struct Cyc_Absyn_Exp* e){ void* _temp31= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); int _temp41; void* _temp43; long long _temp45; void* _temp47; _LL33: if((
unsigned int) _temp31 > 1u?(( struct _enum_struct*) _temp31)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL44: _temp43=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp31)->f1; goto
_LL42; _LL42: _temp41=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp31)->f2;
goto _LL34;} else{ goto _LL35;} _LL35: if(( unsigned int) _temp31 > 1u?(( struct
_enum_struct*) _temp31)->tag == Cyc_Absyn_LongLong_c_tag: 0){ _LL48: _temp47=(
void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp31)->f1; goto _LL46; _LL46:
_temp45=( long long)(( struct Cyc_Absyn_LongLong_c_struct*) _temp31)->f2; goto
_LL36;} else{ goto _LL37;} _LL37: if( _temp31 == Cyc_Absyn_Null_c){ goto _LL38;}
else{ goto _LL39;} _LL39: goto _LL40; _LL34: return _temp41 != 0; _LL36: return
_temp45 !=( long long) 0; _LL38: return 0; _LL40: Cyc_Evexp_exp_err( e->loc,(
struct _tagged_string)({ char* _temp49=( char*)"expecting bool"; struct
_tagged_string _temp50; _temp50.curr= _temp49; _temp50.base= _temp49; _temp50.last_plus_one=
_temp49 + 15; _temp50;})); return 0; _LL32:;} void* Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp* e){ void* cn;{ void* _temp51=( void*) e->r; void* _temp65;
struct Cyc_Absyn_Exp* _temp67; struct Cyc_Absyn_Exp* _temp69; struct Cyc_Absyn_Exp*
_temp71; struct Cyc_List_List* _temp73; void* _temp75; void* _temp77; struct Cyc_Absyn_Exp*
_temp79; void* _temp81; _LL53: if((( struct _enum_struct*) _temp51)->tag == Cyc_Absyn_Const_e_tag){
_LL66: _temp65=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp51)->f1; goto
_LL54;} else{ goto _LL55;} _LL55: if((( struct _enum_struct*) _temp51)->tag ==
Cyc_Absyn_Conditional_e_tag){ _LL72: _temp71=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp51)->f1; goto _LL70; _LL70: _temp69=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp51)->f2;
goto _LL68; _LL68: _temp67=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp51)->f3; goto _LL56;} else{ goto _LL57;} _LL57: if((( struct _enum_struct*)
_temp51)->tag == Cyc_Absyn_Primop_e_tag){ _LL76: _temp75=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp51)->f1; goto _LL74; _LL74: _temp73=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp51)->f2; goto _LL58;} else{ goto _LL59;} _LL59: if((( struct _enum_struct*)
_temp51)->tag == Cyc_Absyn_Sizeof_e_tag){ _LL78: _temp77=( void*)(( struct Cyc_Absyn_Sizeof_e_struct*)
_temp51)->f1; goto _LL60;} else{ goto _LL61;} _LL61: if((( struct _enum_struct*)
_temp51)->tag == Cyc_Absyn_Cast_e_tag){ _LL82: _temp81=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp51)->f1; goto _LL80; _LL80: _temp79=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp51)->f2; goto _LL62;} else{ goto _LL63;} _LL63: goto _LL64; _LL54: cn=
_temp65; goto _LL52; _LL56: cn= Cyc_Evexp_eval_const_bool_exp( _temp71)? Cyc_Evexp_eval_const_exp(
_temp69): Cyc_Evexp_eval_const_exp( _temp67); goto _LL52; _LL58: if( _temp73 ==
0){ Cyc_Evexp_exp_err( e->loc,( struct _tagged_string)({ char* _temp83=( char*)"bad static expression (no args to primop)";
struct _tagged_string _temp84; _temp84.curr= _temp83; _temp84.base= _temp83;
_temp84.last_plus_one= _temp83 + 42; _temp84;})); return({ struct Cyc_Absyn_Int_c_struct*
_temp85=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp85->tag= Cyc_Absyn_Int_c_tag; _temp85->f1=( void*) Cyc_Absyn_Signed;
_temp85->f2= 0;( void*) _temp85;});} if( _temp73->tl == 0){ cn= Cyc_Evexp_eval_const_unprimop(
_temp75,( struct Cyc_Absyn_Exp*) _temp73->hd);} else{ if(( _temp73->tl)->tl != 0){
Cyc_Evexp_exp_err( e->loc,( struct _tagged_string)({ char* _temp86=( char*)"bad static expression (too many args to primop)";
struct _tagged_string _temp87; _temp87.curr= _temp86; _temp87.base= _temp86;
_temp87.last_plus_one= _temp86 + 48; _temp87;})); return({ struct Cyc_Absyn_Int_c_struct*
_temp88=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp88->tag= Cyc_Absyn_Int_c_tag; _temp88->f1=( void*) Cyc_Absyn_Signed;
_temp88->f2= 0;( void*) _temp88;});} else{ cn= Cyc_Evexp_eval_const_binprimop(
_temp75,( struct Cyc_Absyn_Exp*) _temp73->hd,( struct Cyc_Absyn_Exp*)( _temp73->tl)->hd);}}
goto _LL52; _LL60: cn= Cyc_Evexp_eval_sizeof( _temp77, e->loc); goto _LL52;
_LL62: cn= Cyc_Evexp_eval_const_exp( _temp79);{ void* _temp89= _temp81; void*
_temp95; void* _temp97; _LL91: if(( unsigned int) _temp89 > 5u?(( struct
_enum_struct*) _temp89)->tag == Cyc_Absyn_IntType_tag: 0){ _LL98: _temp97=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp89)->f1; goto _LL96; _LL96: _temp95=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp89)->f2; goto _LL92;} else{ goto
_LL93;} _LL93: goto _LL94; _LL92:{ void* _temp99= cn; int _temp105; void*
_temp107; _LL101: if(( unsigned int) _temp99 > 1u?(( struct _enum_struct*)
_temp99)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL108: _temp107=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp99)->f1; goto _LL106; _LL106: _temp105=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp99)->f2; goto _LL102;} else{ goto _LL103;} _LL103: goto _LL104; _LL102: cn=({
struct Cyc_Absyn_Int_c_struct* _temp109=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp109->tag= Cyc_Absyn_Int_c_tag;
_temp109->f1=( void*) _temp97; _temp109->f2= _temp105;( void*) _temp109;}); goto
_LL100; _LL104: Cyc_Evexp_exp_err( e->loc,({ struct _tagged_string _temp110= Cyc_Absynpp_typ2string(
_temp81); xprintf("eval_const: cannot cast to %.*s", _temp110.last_plus_one -
_temp110.curr, _temp110.curr);})); return({ struct Cyc_Absyn_Int_c_struct*
_temp111=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp111->tag= Cyc_Absyn_Int_c_tag; _temp111->f1=( void*) Cyc_Absyn_Signed;
_temp111->f2= 0;( void*) _temp111;}); _LL100:;} goto _LL90; _LL94: Cyc_Evexp_exp_err(
e->loc,({ struct _tagged_string _temp112= Cyc_Absynpp_typ2string( _temp81);
xprintf("eval_const: cannot cast to %.*s", _temp112.last_plus_one - _temp112.curr,
_temp112.curr);})); return({ struct Cyc_Absyn_Int_c_struct* _temp113=( struct
Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp113->tag= Cyc_Absyn_Int_c_tag; _temp113->f1=( void*) Cyc_Absyn_Signed;
_temp113->f2= 0;( void*) _temp113;}); _LL90:;} goto _LL52; _LL64: Cyc_Evexp_exp_err(
e->loc,( struct _tagged_string)({ char* _temp114=( char*)"bad static expression";
struct _tagged_string _temp115; _temp115.curr= _temp114; _temp115.base= _temp114;
_temp115.last_plus_one= _temp114 + 22; _temp115;})); return({ struct Cyc_Absyn_Int_c_struct*
_temp116=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp116->tag= Cyc_Absyn_Int_c_tag; _temp116->f1=( void*) Cyc_Absyn_Signed;
_temp116->f2= 0;( void*) _temp116;}); _LL52:;}( void*)( e->r=( void*)({ struct
Cyc_Absyn_Const_e_struct* _temp117=( struct Cyc_Absyn_Const_e_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Const_e_struct)); _temp117->tag= Cyc_Absyn_Const_e_tag;
_temp117->f1=( void*) cn;( void*) _temp117;})); return cn;} unsigned int Cyc_Evexp_szof(
void* t, struct Cyc_Position_Segment* loc){ void* _temp118= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tvar* _temp166; int _temp168; struct Cyc_Core_Opt* _temp170;
void* _temp172; int _temp174; struct Cyc_Core_Opt* _temp176; void* _temp178;
struct Cyc_Absyn_Enumdecl** _temp180; struct Cyc_List_List* _temp182; struct
_tuple0* _temp184; struct Cyc_Absyn_Xenumdecl** _temp186; struct _tuple0*
_temp188; struct Cyc_Absyn_PtrInfo _temp190; struct Cyc_Absyn_Conref* _temp192;
struct Cyc_Absyn_Tqual* _temp194; struct Cyc_Absyn_Conref* _temp196; void*
_temp198; void* _temp200; void* _temp202; void* _temp204; void* _temp206; void*
_temp208; void* _temp210; void* _temp212; void* _temp214; void* _temp216; struct
Cyc_Absyn_Exp* _temp218; struct Cyc_Absyn_Tqual* _temp220; void* _temp222;
struct Cyc_Absyn_FnInfo _temp224; struct Cyc_List_List* _temp226; void* _temp228;
struct Cyc_Absyn_Structdecl** _temp230; struct Cyc_List_List* _temp232; struct
_tuple0* _temp234; struct Cyc_Core_Opt* _temp236; struct Cyc_List_List* _temp238;
struct _tuple0* _temp240; struct Cyc_List_List* _temp242; void* _temp244; _LL120:
if( _temp118 == Cyc_Absyn_VoidType){ goto _LL121;} else{ goto _LL122;} _LL122:
if(( unsigned int) _temp118 > 5u?(( struct _enum_struct*) _temp118)->tag == Cyc_Absyn_VarType_tag:
0){ _LL167: _temp166=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*)
_temp118)->f1; goto _LL123;} else{ goto _LL124;} _LL124: if(( unsigned int)
_temp118 > 5u?(( struct _enum_struct*) _temp118)->tag == Cyc_Absyn_Evar_tag: 0){
_LL173: _temp172=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp118)->f1; if(
_temp172 == Cyc_Absyn_BoxKind){ goto _LL171;} else{ goto _LL126;} _LL171:
_temp170=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp118)->f2;
goto _LL169; _LL169: _temp168=( int)(( struct Cyc_Absyn_Evar_struct*) _temp118)->f3;
goto _LL125;} else{ goto _LL126;} _LL126: if(( unsigned int) _temp118 > 5u?((
struct _enum_struct*) _temp118)->tag == Cyc_Absyn_Evar_tag: 0){ _LL179: _temp178=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp118)->f1; goto _LL177; _LL177:
_temp176=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp118)->f2;
goto _LL175; _LL175: _temp174=( int)(( struct Cyc_Absyn_Evar_struct*) _temp118)->f3;
goto _LL127;} else{ goto _LL128;} _LL128: if(( unsigned int) _temp118 > 5u?((
struct _enum_struct*) _temp118)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL185:
_temp184=( struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*) _temp118)->f1;
goto _LL183; _LL183: _temp182=( struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*)
_temp118)->f2; goto _LL181; _LL181: _temp180=( struct Cyc_Absyn_Enumdecl**)((
struct Cyc_Absyn_EnumType_struct*) _temp118)->f3; goto _LL129;} else{ goto
_LL130;} _LL130: if(( unsigned int) _temp118 > 5u?(( struct _enum_struct*)
_temp118)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL189: _temp188=( struct _tuple0*)((
struct Cyc_Absyn_XenumType_struct*) _temp118)->f1; goto _LL187; _LL187: _temp186=(
struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*) _temp118)->f2;
goto _LL131;} else{ goto _LL132;} _LL132: if(( unsigned int) _temp118 > 5u?((
struct _enum_struct*) _temp118)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL191:
_temp190=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp118)->f1; _LL201: _temp200=( void*) _temp190.elt_typ; goto _LL199; _LL199:
_temp198=( void*) _temp190.rgn_typ; goto _LL197; _LL197: _temp196=( struct Cyc_Absyn_Conref*)
_temp190.nullable; goto _LL195; _LL195: _temp194=( struct Cyc_Absyn_Tqual*)
_temp190.tq; goto _LL193; _LL193: _temp192=( struct Cyc_Absyn_Conref*) _temp190.bounds;
goto _LL133;} else{ goto _LL134;} _LL134: if(( unsigned int) _temp118 > 5u?((
struct _enum_struct*) _temp118)->tag == Cyc_Absyn_IntType_tag: 0){ _LL205:
_temp204=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp118)->f1; goto _LL203;
_LL203: _temp202=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp118)->f2; if(
_temp202 == Cyc_Absyn_B1){ goto _LL135;} else{ goto _LL136;}} else{ goto _LL136;}
_LL136: if(( unsigned int) _temp118 > 5u?(( struct _enum_struct*) _temp118)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL209: _temp208=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp118)->f1; goto _LL207; _LL207: _temp206=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp118)->f2; if( _temp206 == Cyc_Absyn_B2){ goto _LL137;} else{ goto _LL138;}}
else{ goto _LL138;} _LL138: if(( unsigned int) _temp118 > 5u?(( struct
_enum_struct*) _temp118)->tag == Cyc_Absyn_IntType_tag: 0){ _LL213: _temp212=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp118)->f1; goto _LL211; _LL211:
_temp210=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp118)->f2; if(
_temp210 == Cyc_Absyn_B4){ goto _LL139;} else{ goto _LL140;}} else{ goto _LL140;}
_LL140: if(( unsigned int) _temp118 > 5u?(( struct _enum_struct*) _temp118)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL217: _temp216=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp118)->f1; goto _LL215; _LL215: _temp214=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp118)->f2; if( _temp214 == Cyc_Absyn_B8){ goto _LL141;} else{ goto _LL142;}}
else{ goto _LL142;} _LL142: if( _temp118 == Cyc_Absyn_FloatType){ goto _LL143;}
else{ goto _LL144;} _LL144: if( _temp118 == Cyc_Absyn_DoubleType){ goto _LL145;}
else{ goto _LL146;} _LL146: if(( unsigned int) _temp118 > 5u?(( struct
_enum_struct*) _temp118)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL223: _temp222=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp118)->f1; goto _LL221; _LL221:
_temp220=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp118)->f2; goto _LL219; _LL219: _temp218=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp118)->f3; goto _LL147;} else{ goto _LL148;}
_LL148: if(( unsigned int) _temp118 > 5u?(( struct _enum_struct*) _temp118)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL225: _temp224=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp118)->f1; goto _LL149;} else{ goto _LL150;}
_LL150: if(( unsigned int) _temp118 > 5u?(( struct _enum_struct*) _temp118)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL227: _temp226=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp118)->f1; goto _LL151;} else{ goto
_LL152;} _LL152: if(( unsigned int) _temp118 > 5u?(( struct _enum_struct*)
_temp118)->tag == Cyc_Absyn_RgnHandleType_tag: 0){ _LL229: _temp228=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp118)->f1; goto _LL153;} else{ goto
_LL154;} _LL154: if(( unsigned int) _temp118 > 5u?(( struct _enum_struct*)
_temp118)->tag == Cyc_Absyn_StructType_tag: 0){ _LL235: _temp234=( struct
_tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp118)->f1; goto _LL233;
_LL233: _temp232=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp118)->f2; goto _LL231; _LL231: _temp230=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp118)->f3; goto _LL155;} else{ goto
_LL156;} _LL156: if( _temp118 == Cyc_Absyn_HeapRgn){ goto _LL157;} else{ goto
_LL158;} _LL158: if(( unsigned int) _temp118 > 5u?(( struct _enum_struct*)
_temp118)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL241: _temp240=( struct
_tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp118)->f1; goto _LL239;
_LL239: _temp238=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp118)->f2; goto _LL237; _LL237: _temp236=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp118)->f3; goto _LL159;} else{ goto _LL160;}
_LL160: if( _temp118 == Cyc_Absyn_UnionType){ goto _LL161;} else{ goto _LL162;}
_LL162: if(( unsigned int) _temp118 > 5u?(( struct _enum_struct*) _temp118)->tag
== Cyc_Absyn_JoinEff_tag: 0){ _LL243: _temp242=( struct Cyc_List_List*)(( struct
Cyc_Absyn_JoinEff_struct*) _temp118)->f1; goto _LL163;} else{ goto _LL164;}
_LL164: if(( unsigned int) _temp118 > 5u?(( struct _enum_struct*) _temp118)->tag
== Cyc_Absyn_AccessEff_tag: 0){ _LL245: _temp244=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp118)->f1; goto _LL165;} else{ goto _LL119;} _LL121: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp246=( char*)"cannot apply sizeof to void type";
struct _tagged_string _temp247; _temp247.curr= _temp246; _temp247.base= _temp246;
_temp247.last_plus_one= _temp246 + 33; _temp247;})); return 0u; _LL123: { void*
_temp248=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp166->kind); _LL250: if( _temp248 == Cyc_Absyn_BoxKind){ goto _LL251;} else{
goto _LL252;} _LL252: if( _temp248 == Cyc_Absyn_AnyKind){ goto _LL253;} else{
goto _LL254;} _LL254: if( _temp248 == Cyc_Absyn_MemKind){ goto _LL255;} else{
goto _LL256;} _LL256: if( _temp248 == Cyc_Absyn_RgnKind){ goto _LL257;} else{
goto _LL258;} _LL258: if( _temp248 == Cyc_Absyn_EffKind){ goto _LL259;} else{
goto _LL249;} _LL251: return 4u; _LL253: goto _LL255; _LL255: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp260=( char*)"cannot apply sizeof to a non-boxed type variable";
struct _tagged_string _temp261; _temp261.curr= _temp260; _temp261.base= _temp260;
_temp261.last_plus_one= _temp260 + 49; _temp261;})); return 0u; _LL257: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp262=( char*)"cannot apply sizeof to a region";
struct _tagged_string _temp263; _temp263.curr= _temp262; _temp263.base= _temp262;
_temp263.last_plus_one= _temp262 + 32; _temp263;})); return 0u; _LL259: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp264=( char*)"cannot apply sizeof to an effect";
struct _tagged_string _temp265; _temp265.curr= _temp264; _temp265.base= _temp264;
_temp265.last_plus_one= _temp264 + 33; _temp265;})); return 0u; _LL249:;} _LL125:
return 4u; _LL127: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp266=( char*)"cannot apply sizeof to unknown type"; struct _tagged_string
_temp267; _temp267.curr= _temp266; _temp267.base= _temp266; _temp267.last_plus_one=
_temp266 + 36; _temp267;})); return 0u; _LL129: return 4u; _LL131: return 4u;
_LL133: { void* _temp268=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp192))->v; void* _temp276; void* _temp278;
_LL270: if(( unsigned int) _temp268 > 1u?(( struct _enum_struct*) _temp268)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL277: _temp276=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp268)->f1; if( _temp276 == Cyc_Absyn_Unknown_b){ goto _LL271;} else{ goto
_LL272;}} else{ goto _LL272;} _LL272: if(( unsigned int) _temp268 > 1u?(( struct
_enum_struct*) _temp268)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL279: _temp278=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp268)->f1; goto _LL273;} else{
goto _LL274;} _LL274: goto _LL275; _LL271: return 12u; _LL273: return 4u; _LL275:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp280=( char*)"bounds information not known for pointer type";
struct _tagged_string _temp281; _temp281.curr= _temp280; _temp281.base= _temp280;
_temp281.last_plus_one= _temp280 + 46; _temp281;})); return 0u; _LL269:;} _LL135:
return 1u; _LL137: return 1u; _LL139: return 1u; _LL141: return 1u; _LL143:
return 4u; _LL145: return 8u; _LL147: return Cyc_Evexp_szof( _temp222, loc) *
Cyc_Evexp_eval_const_uint_exp( _temp218); _LL149: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp282=( char*)"cannot apply sizeof to function type";
struct _tagged_string _temp283; _temp283.curr= _temp282; _temp283.base= _temp282;
_temp283.last_plus_one= _temp282 + 37; _temp283;})); return 0u; _LL151: {
unsigned int s= 0; for( 0; _temp226 != 0; _temp226= _temp226->tl){ s += Cyc_Evexp_szof((*((
struct _tuple3*) _temp226->hd)).f2, loc);} return s;} _LL153: return 4u; _LL155:
if( _temp230 == 0){ return(( unsigned int(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp284=( char*)"szof on unchecked StructType";
struct _tagged_string _temp285; _temp285.curr= _temp284; _temp285.base= _temp284;
_temp285.last_plus_one= _temp284 + 29; _temp285;}));}{ struct Cyc_Absyn_Structdecl*
sd=* _temp230; if( sd->fields == 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp286=( char*)"cannot apply sizeof to abstract struct type";
struct _tagged_string _temp287; _temp287.curr= _temp286; _temp287.base= _temp286;
_temp287.last_plus_one= _temp286 + 44; _temp287;}));}{ unsigned int s= 0;{
struct Cyc_List_List* fs=( struct Cyc_List_List*)( sd->fields)->v; for( 0; fs !=
0; fs= fs->tl){ s += Cyc_Evexp_szof((*(( struct _tuple4*) fs->hd)).f3, loc);}}
return s;}} _LL157: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp288=( char*)"cannot apply sizoef to a region"; struct _tagged_string
_temp289; _temp289.curr= _temp288; _temp289.base= _temp288; _temp289.last_plus_one=
_temp288 + 32; _temp289;})); return 0u; _LL159: return(( unsigned int(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp290=(
char*)"szof typedeftype"; struct _tagged_string _temp291; _temp291.curr=
_temp290; _temp291.base= _temp290; _temp291.last_plus_one= _temp290 + 17;
_temp291;})); _LL161: return(( unsigned int(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp292=( char*)"szof uniontype"; struct
_tagged_string _temp293; _temp293.curr= _temp292; _temp293.base= _temp292;
_temp293.last_plus_one= _temp292 + 15; _temp293;})); _LL163: goto _LL165; _LL165:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp294=( char*)"cannot apply sizoef to an effect";
struct _tagged_string _temp295; _temp295.curr= _temp294; _temp295.base= _temp294;
_temp295.last_plus_one= _temp294 + 33; _temp295;})); return 0u; _LL119:;} void*
Cyc_Evexp_eval_sizeof( void* t, struct Cyc_Position_Segment* loc){ return({
struct Cyc_Absyn_Int_c_struct* _temp296=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp296->tag= Cyc_Absyn_Int_c_tag;
_temp296->f1=( void*) Cyc_Absyn_Unsigned; _temp296->f2=( int) Cyc_Evexp_szof( t,
loc);( void*) _temp296;});} int Cyc_Evexp_is_arith_const( void* cn){ void*
_temp297= cn; char _temp307; void* _temp309; int _temp311; void* _temp313;
struct _tagged_string _temp315; _LL299: if(( unsigned int) _temp297 > 1u?((
struct _enum_struct*) _temp297)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL310:
_temp309=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp297)->f1; goto _LL308;
_LL308: _temp307=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp297)->f2; goto
_LL300;} else{ goto _LL301;} _LL301: if(( unsigned int) _temp297 > 1u?(( struct
_enum_struct*) _temp297)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL314: _temp313=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp297)->f1; goto _LL312; _LL312:
_temp311=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp297)->f2; goto _LL302;}
else{ goto _LL303;} _LL303: if(( unsigned int) _temp297 > 1u?(( struct
_enum_struct*) _temp297)->tag == Cyc_Absyn_Float_c_tag: 0){ _LL316: _temp315=(
struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*) _temp297)->f1; goto
_LL304;} else{ goto _LL305;} _LL305: goto _LL306; _LL300: return 1; _LL302:
return 1; _LL304: return 1; _LL306: return 0; _LL298:;} void* Cyc_Evexp_eval_const_unprimop(
void* p, struct Cyc_Absyn_Exp* e){ void* cn= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); struct _tuple5 _temp318=({ struct _tuple5 _temp317; _temp317.f1= p;
_temp317.f2= cn; _temp317;}); void* _temp336; void* _temp338; void* _temp340;
int _temp342; void* _temp344; void* _temp346; void* _temp348; short _temp350;
void* _temp352; void* _temp354; void* _temp356; char _temp358; void* _temp360;
void* _temp362; void* _temp364; int _temp366; void* _temp368; void* _temp370;
void* _temp372; void* _temp374; void* _temp376; int _temp378; void* _temp380;
void* _temp382; _LL320: _LL339: _temp338= _temp318.f1; if( _temp338 == Cyc_Absyn_Plus){
goto _LL337;} else{ goto _LL322;} _LL337: _temp336= _temp318.f2; goto _LL321;
_LL322: _LL347: _temp346= _temp318.f1; if( _temp346 == Cyc_Absyn_Minus){ goto
_LL341;} else{ goto _LL324;} _LL341: _temp340= _temp318.f2; if(( unsigned int)
_temp340 > 1u?(( struct _enum_struct*) _temp340)->tag == Cyc_Absyn_Int_c_tag: 0){
_LL345: _temp344=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp340)->f1; goto
_LL343; _LL343: _temp342=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp340)->f2;
goto _LL323;} else{ goto _LL324;} _LL324: _LL355: _temp354= _temp318.f1; if(
_temp354 == Cyc_Absyn_Minus){ goto _LL349;} else{ goto _LL326;} _LL349: _temp348=
_temp318.f2; if(( unsigned int) _temp348 > 1u?(( struct _enum_struct*) _temp348)->tag
== Cyc_Absyn_Short_c_tag: 0){ _LL353: _temp352=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp348)->f1; goto _LL351; _LL351: _temp350=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp348)->f2; goto _LL325;} else{ goto _LL326;} _LL326: _LL363: _temp362=
_temp318.f1; if( _temp362 == Cyc_Absyn_Minus){ goto _LL357;} else{ goto _LL328;}
_LL357: _temp356= _temp318.f2; if(( unsigned int) _temp356 > 1u?(( struct
_enum_struct*) _temp356)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL361: _temp360=(
void*)(( struct Cyc_Absyn_Char_c_struct*) _temp356)->f1; goto _LL359; _LL359:
_temp358=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp356)->f2; goto _LL327;}
else{ goto _LL328;} _LL328: _LL371: _temp370= _temp318.f1; if( _temp370 == Cyc_Absyn_Not){
goto _LL365;} else{ goto _LL330;} _LL365: _temp364= _temp318.f2; if((
unsigned int) _temp364 > 1u?(( struct _enum_struct*) _temp364)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL369: _temp368=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp364)->f1;
goto _LL367; _LL367: _temp366=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp364)->f2;
goto _LL329;} else{ goto _LL330;} _LL330: _LL375: _temp374= _temp318.f1; if(
_temp374 == Cyc_Absyn_Not){ goto _LL373;} else{ goto _LL332;} _LL373: _temp372=
_temp318.f2; if( _temp372 == Cyc_Absyn_Null_c){ goto _LL331;} else{ goto _LL332;}
_LL332: _LL383: _temp382= _temp318.f1; if( _temp382 == Cyc_Absyn_Bitnot){ goto
_LL377;} else{ goto _LL334;} _LL377: _temp376= _temp318.f2; if(( unsigned int)
_temp376 > 1u?(( struct _enum_struct*) _temp376)->tag == Cyc_Absyn_Int_c_tag: 0){
_LL381: _temp380=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp376)->f1; goto
_LL379; _LL379: _temp378=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp376)->f2;
goto _LL333;} else{ goto _LL334;} _LL334: goto _LL335; _LL321: if( ! Cyc_Evexp_is_arith_const(
cn)){ Cyc_Evexp_exp_err( e->loc,( struct _tagged_string)({ char* _temp384=( char*)"expecting arithmetic constant";
struct _tagged_string _temp385; _temp385.curr= _temp384; _temp385.base= _temp384;
_temp385.last_plus_one= _temp384 + 30; _temp385;}));} return cn; _LL323: return({
struct Cyc_Absyn_Int_c_struct* _temp386=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp386->tag= Cyc_Absyn_Int_c_tag;
_temp386->f1=( void*) Cyc_Absyn_Signed; _temp386->f2= - _temp342;( void*)
_temp386;}); _LL325: return({ struct Cyc_Absyn_Short_c_struct* _temp387=( struct
Cyc_Absyn_Short_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct));
_temp387->tag= Cyc_Absyn_Short_c_tag; _temp387->f1=( void*) Cyc_Absyn_Signed;
_temp387->f2= - _temp350;( void*) _temp387;}); _LL327: return({ struct Cyc_Absyn_Char_c_struct*
_temp388=( struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp388->tag= Cyc_Absyn_Char_c_tag; _temp388->f1=( void*) Cyc_Absyn_Signed;
_temp388->f2= - _temp358;( void*) _temp388;}); _LL329: return({ struct Cyc_Absyn_Int_c_struct*
_temp389=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp389->tag= Cyc_Absyn_Int_c_tag; _temp389->f1=( void*) Cyc_Absyn_Signed;
_temp389->f2= _temp366 == 0? 1: 0;( void*) _temp389;}); _LL331: return({ struct
Cyc_Absyn_Int_c_struct* _temp390=( struct Cyc_Absyn_Int_c_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Int_c_struct)); _temp390->tag= Cyc_Absyn_Int_c_tag;
_temp390->f1=( void*) Cyc_Absyn_Signed; _temp390->f2= 1;( void*) _temp390;});
_LL333: return({ struct Cyc_Absyn_Int_c_struct* _temp391=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp391->tag= Cyc_Absyn_Int_c_tag;
_temp391->f1=( void*) Cyc_Absyn_Unsigned; _temp391->f2= ~ _temp378;( void*)
_temp391;}); _LL335: Cyc_Evexp_exp_err( e->loc,( struct _tagged_string)({ char*
_temp392=( char*)"bad constant expression"; struct _tagged_string _temp393;
_temp393.curr= _temp392; _temp393.base= _temp392; _temp393.last_plus_one=
_temp392 + 24; _temp393;})); return cn; _LL319:;} void* Cyc_Evexp_eval_const_binprimop(
void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ void* cn1= Cyc_Evexp_promote_const(
Cyc_Evexp_eval_const_exp( e1)); void* cn2= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e2)); void* s1; void* s2; int i1; int i2;{ void* _temp394= cn1; int _temp400;
void* _temp402; _LL396: if(( unsigned int) _temp394 > 1u?(( struct _enum_struct*)
_temp394)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL403: _temp402=( void*)(( struct
Cyc_Absyn_Int_c_struct*) _temp394)->f1; goto _LL401; _LL401: _temp400=( int)((
struct Cyc_Absyn_Int_c_struct*) _temp394)->f2; goto _LL397;} else{ goto _LL398;}
_LL398: goto _LL399; _LL397: s1= _temp402; i1= _temp400; goto _LL395; _LL399:
Cyc_Evexp_exp_err( e1->loc,( struct _tagged_string)({ char* _temp404=( char*)"bad constant expression";
struct _tagged_string _temp405; _temp405.curr= _temp404; _temp405.base= _temp404;
_temp405.last_plus_one= _temp404 + 24; _temp405;})); return cn1; _LL395:;}{ void*
_temp406= cn2; int _temp412; void* _temp414; _LL408: if(( unsigned int) _temp406
> 1u?(( struct _enum_struct*) _temp406)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL415:
_temp414=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp406)->f1; goto _LL413;
_LL413: _temp412=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp406)->f2; goto
_LL409;} else{ goto _LL410;} _LL410: goto _LL411; _LL409: s2= _temp414; i2=
_temp412; goto _LL407; _LL411: Cyc_Evexp_exp_err( e2->loc,( struct
_tagged_string)({ char* _temp416=( char*)"bad constant expression"; struct
_tagged_string _temp417; _temp417.curr= _temp416; _temp417.base= _temp416;
_temp417.last_plus_one= _temp416 + 24; _temp417;})); return cn1; _LL407:;}{ void*
_temp418= p; _LL420: if( _temp418 == Cyc_Absyn_Div){ goto _LL421;} else{ goto
_LL422;} _LL422: if( _temp418 == Cyc_Absyn_Mod){ goto _LL423;} else{ goto _LL424;}
_LL424: goto _LL425; _LL421: if( i2 == 0){ Cyc_Evexp_exp_err( e2->loc,( struct
_tagged_string)({ char* _temp426=( char*)"division by zero in constant expression";
struct _tagged_string _temp427; _temp427.curr= _temp426; _temp427.base= _temp426;
_temp427.last_plus_one= _temp426 + 40; _temp427;})); return cn1;} goto _LL419;
_LL423: if( i2 == 0){ Cyc_Evexp_exp_err( e2->loc,( struct _tagged_string)({ char*
_temp428=( char*)"division by zero in constant expression"; struct
_tagged_string _temp429; _temp429.curr= _temp428; _temp429.base= _temp428;
_temp429.last_plus_one= _temp428 + 40; _temp429;})); return cn1;} goto _LL419;
_LL425: goto _LL419; _LL419:;}{ int has_u_arg= s1 == Cyc_Absyn_Unsigned? 1: s2
== Cyc_Absyn_Unsigned; unsigned int u1=( unsigned int) i1; unsigned int u2=(
unsigned int) i2; int i3= 0; unsigned int u3= 0; int b3= 1; int use_i3= 0; int
use_u3= 0; int use_b3= 0;{ struct _tuple6 _temp431=({ struct _tuple6 _temp430;
_temp430.f1= p; _temp430.f2= has_u_arg; _temp430;}); int _temp487; void*
_temp489; int _temp491; void* _temp493; int _temp495; void* _temp497; int
_temp499; void* _temp501; int _temp503; void* _temp505; int _temp507; void*
_temp509; int _temp511; void* _temp513; int _temp515; void* _temp517; int
_temp519; void* _temp521; int _temp523; void* _temp525; int _temp527; void*
_temp529; int _temp531; void* _temp533; int _temp535; void* _temp537; int
_temp539; void* _temp541; int _temp543; void* _temp545; int _temp547; void*
_temp549; int _temp551; void* _temp553; int _temp555; void* _temp557; int
_temp559; void* _temp561; int _temp563; void* _temp565; int _temp567; void*
_temp569; int _temp571; void* _temp573; int _temp575; void* _temp577; int
_temp579; void* _temp581; int _temp583; void* _temp585; int _temp587; void*
_temp589; _LL433: _LL490: _temp489= _temp431.f1; if( _temp489 == Cyc_Absyn_Plus){
goto _LL488;} else{ goto _LL435;} _LL488: _temp487= _temp431.f2; if( _temp487 ==
0){ goto _LL434;} else{ goto _LL435;} _LL435: _LL494: _temp493= _temp431.f1; if(
_temp493 == Cyc_Absyn_Times){ goto _LL492;} else{ goto _LL437;} _LL492: _temp491=
_temp431.f2; if( _temp491 == 0){ goto _LL436;} else{ goto _LL437;} _LL437:
_LL498: _temp497= _temp431.f1; if( _temp497 == Cyc_Absyn_Minus){ goto _LL496;}
else{ goto _LL439;} _LL496: _temp495= _temp431.f2; if( _temp495 == 0){ goto
_LL438;} else{ goto _LL439;} _LL439: _LL502: _temp501= _temp431.f1; if( _temp501
== Cyc_Absyn_Div){ goto _LL500;} else{ goto _LL441;} _LL500: _temp499= _temp431.f2;
if( _temp499 == 0){ goto _LL440;} else{ goto _LL441;} _LL441: _LL506: _temp505=
_temp431.f1; if( _temp505 == Cyc_Absyn_Mod){ goto _LL504;} else{ goto _LL443;}
_LL504: _temp503= _temp431.f2; if( _temp503 == 0){ goto _LL442;} else{ goto
_LL443;} _LL443: _LL510: _temp509= _temp431.f1; if( _temp509 == Cyc_Absyn_Plus){
goto _LL508;} else{ goto _LL445;} _LL508: _temp507= _temp431.f2; if( _temp507 ==
1){ goto _LL444;} else{ goto _LL445;} _LL445: _LL514: _temp513= _temp431.f1; if(
_temp513 == Cyc_Absyn_Times){ goto _LL512;} else{ goto _LL447;} _LL512: _temp511=
_temp431.f2; if( _temp511 == 1){ goto _LL446;} else{ goto _LL447;} _LL447:
_LL518: _temp517= _temp431.f1; if( _temp517 == Cyc_Absyn_Minus){ goto _LL516;}
else{ goto _LL449;} _LL516: _temp515= _temp431.f2; if( _temp515 == 1){ goto
_LL448;} else{ goto _LL449;} _LL449: _LL522: _temp521= _temp431.f1; if( _temp521
== Cyc_Absyn_Div){ goto _LL520;} else{ goto _LL451;} _LL520: _temp519= _temp431.f2;
if( _temp519 == 1){ goto _LL450;} else{ goto _LL451;} _LL451: _LL526: _temp525=
_temp431.f1; if( _temp525 == Cyc_Absyn_Mod){ goto _LL524;} else{ goto _LL453;}
_LL524: _temp523= _temp431.f2; if( _temp523 == 1){ goto _LL452;} else{ goto
_LL453;} _LL453: _LL530: _temp529= _temp431.f1; if( _temp529 == Cyc_Absyn_Eq){
goto _LL528;} else{ goto _LL455;} _LL528: _temp527= _temp431.f2; goto _LL454;
_LL455: _LL534: _temp533= _temp431.f1; if( _temp533 == Cyc_Absyn_Neq){ goto
_LL532;} else{ goto _LL457;} _LL532: _temp531= _temp431.f2; goto _LL456; _LL457:
_LL538: _temp537= _temp431.f1; if( _temp537 == Cyc_Absyn_Gt){ goto _LL536;}
else{ goto _LL459;} _LL536: _temp535= _temp431.f2; if( _temp535 == 0){ goto
_LL458;} else{ goto _LL459;} _LL459: _LL542: _temp541= _temp431.f1; if( _temp541
== Cyc_Absyn_Lt){ goto _LL540;} else{ goto _LL461;} _LL540: _temp539= _temp431.f2;
if( _temp539 == 0){ goto _LL460;} else{ goto _LL461;} _LL461: _LL546: _temp545=
_temp431.f1; if( _temp545 == Cyc_Absyn_Gte){ goto _LL544;} else{ goto _LL463;}
_LL544: _temp543= _temp431.f2; if( _temp543 == 0){ goto _LL462;} else{ goto
_LL463;} _LL463: _LL550: _temp549= _temp431.f1; if( _temp549 == Cyc_Absyn_Lte){
goto _LL548;} else{ goto _LL465;} _LL548: _temp547= _temp431.f2; if( _temp547 ==
0){ goto _LL464;} else{ goto _LL465;} _LL465: _LL554: _temp553= _temp431.f1; if(
_temp553 == Cyc_Absyn_Gt){ goto _LL552;} else{ goto _LL467;} _LL552: _temp551=
_temp431.f2; if( _temp551 == 1){ goto _LL466;} else{ goto _LL467;} _LL467:
_LL558: _temp557= _temp431.f1; if( _temp557 == Cyc_Absyn_Lt){ goto _LL556;}
else{ goto _LL469;} _LL556: _temp555= _temp431.f2; if( _temp555 == 1){ goto
_LL468;} else{ goto _LL469;} _LL469: _LL562: _temp561= _temp431.f1; if( _temp561
== Cyc_Absyn_Gte){ goto _LL560;} else{ goto _LL471;} _LL560: _temp559= _temp431.f2;
if( _temp559 == 1){ goto _LL470;} else{ goto _LL471;} _LL471: _LL566: _temp565=
_temp431.f1; if( _temp565 == Cyc_Absyn_Lte){ goto _LL564;} else{ goto _LL473;}
_LL564: _temp563= _temp431.f2; if( _temp563 == 1){ goto _LL472;} else{ goto
_LL473;} _LL473: _LL570: _temp569= _temp431.f1; if( _temp569 == Cyc_Absyn_Bitand){
goto _LL568;} else{ goto _LL475;} _LL568: _temp567= _temp431.f2; goto _LL474;
_LL475: _LL574: _temp573= _temp431.f1; if( _temp573 == Cyc_Absyn_Bitor){ goto
_LL572;} else{ goto _LL477;} _LL572: _temp571= _temp431.f2; goto _LL476; _LL477:
_LL578: _temp577= _temp431.f1; if( _temp577 == Cyc_Absyn_Bitxor){ goto _LL576;}
else{ goto _LL479;} _LL576: _temp575= _temp431.f2; goto _LL478; _LL479: _LL582:
_temp581= _temp431.f1; if( _temp581 == Cyc_Absyn_Bitlshift){ goto _LL580;} else{
goto _LL481;} _LL580: _temp579= _temp431.f2; goto _LL480; _LL481: _LL586:
_temp585= _temp431.f1; if( _temp585 == Cyc_Absyn_Bitlrshift){ goto _LL584;}
else{ goto _LL483;} _LL584: _temp583= _temp431.f2; goto _LL482; _LL483: _LL590:
_temp589= _temp431.f1; if( _temp589 == Cyc_Absyn_Bitarshift){ goto _LL588;}
else{ goto _LL485;} _LL588: _temp587= _temp431.f2; goto _LL484; _LL485: goto
_LL486; _LL434: i3= i1 + i2; use_i3= 1; goto _LL432; _LL436: i3= i1 * i2; use_i3=
1; goto _LL432; _LL438: i3= i1 - i2; use_i3= 1; goto _LL432; _LL440: i3= i1 / i2;
use_i3= 1; goto _LL432; _LL442: i3= i1 % i2; use_i3= 1; goto _LL432; _LL444: u3=
u1 + u2; use_u3= 1; goto _LL432; _LL446: u3= u1 * u2; use_u3= 1; goto _LL432;
_LL448: u3= u1 - u2; use_u3= 1; goto _LL432; _LL450: u3= u1 / u2; use_u3= 1;
goto _LL432; _LL452: u3= u1 % u2; use_u3= 1; goto _LL432; _LL454: b3= i1 == i2;
use_b3= 1; goto _LL432; _LL456: b3= i1 != i2; use_b3= 1; goto _LL432; _LL458: b3=
i1 > i2; use_b3= 1; goto _LL432; _LL460: b3= i1 < i2; use_b3= 1; goto _LL432;
_LL462: b3= i1 >= i2; use_b3= 1; goto _LL432; _LL464: b3= i1 <= i2; use_b3= 1;
goto _LL432; _LL466: b3= u1 > u2; use_b3= 1; goto _LL432; _LL468: b3= u1 < u2;
use_b3= 1; goto _LL432; _LL470: b3= u1 >= u2; use_b3= 1; goto _LL432; _LL472: b3=
u1 <= u2; use_b3= 1; goto _LL432; _LL474: u3= u1 & u2; use_u3= 1; goto _LL432;
_LL476: u3= u1 | u2; use_u3= 1; goto _LL432; _LL478: u3= u1 ^ u2; use_u3= 1;
goto _LL432; _LL480: u3= u1 << u2; use_u3= 1; goto _LL432; _LL482: u3= u1 >> u2;
use_u3= 1; goto _LL432; _LL484: Cyc_Evexp_exp_err( e1->loc,( struct
_tagged_string)({ char* _temp591=( char*)">>> NOT IMPLEMENTED"; struct
_tagged_string _temp592; _temp592.curr= _temp591; _temp592.base= _temp591;
_temp592.last_plus_one= _temp591 + 20; _temp592;})); return cn1; _LL486: Cyc_Evexp_exp_err(
e1->loc,( struct _tagged_string)({ char* _temp593=( char*)"bad constant expression";
struct _tagged_string _temp594; _temp594.curr= _temp593; _temp594.base= _temp593;
_temp594.last_plus_one= _temp593 + 24; _temp594;})); return cn1; _LL432:;} if(
use_i3){ return({ struct Cyc_Absyn_Int_c_struct* _temp595=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp595->tag= Cyc_Absyn_Int_c_tag;
_temp595->f1=( void*) Cyc_Absyn_Signed; _temp595->f2= i3;( void*) _temp595;});}
if( use_u3){ return({ struct Cyc_Absyn_Int_c_struct* _temp596=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp596->tag= Cyc_Absyn_Int_c_tag;
_temp596->f1=( void*) Cyc_Absyn_Unsigned; _temp596->f2=( int) u3;( void*)
_temp596;});} if( use_b3){ return({ struct Cyc_Absyn_Int_c_struct* _temp597=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp597->tag= Cyc_Absyn_Int_c_tag; _temp597->f1=( void*) Cyc_Absyn_Signed;
_temp597->f2= b3? 1: 0;( void*) _temp597;});}( void) _throw(({ struct Cyc_Core_Unreachable_struct*
_temp598=( struct Cyc_Core_Unreachable_struct*) GC_malloc( sizeof( struct Cyc_Core_Unreachable_struct));*
_temp598=( struct Cyc_Core_Unreachable_struct){.tag= Cyc_Core_Unreachable_tag,.f1=(
struct _tagged_string)({ char* _temp599=( char*)"Evexp::eval_const_binop";
struct _tagged_string _temp600; _temp600.curr= _temp599; _temp600.base= _temp599;
_temp600.last_plus_one= _temp599 + 24; _temp600;})};( struct _xenum_struct*)
_temp598;}));}}