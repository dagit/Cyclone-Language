#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2;}; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3;}; struct _tuple2{ struct
Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2;}; struct _tuple3{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2;}; struct _tuple4{ struct Cyc_Absyn_Switch_clause*
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;}; struct _tuple5{ struct
Cyc_Absyn_Tvar* f1; void* f2;}; typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring;
typedef struct _tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t;
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
extern int Cyc_List_length( struct Cyc_List_List* x); extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag;}; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map_c( void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag;}; extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x);
extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{ char* tag;};
struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct _tagged_string
line; int line_no; int col;}; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t;
extern char Cyc_Position_Exit_tag[ 5u]; struct Cyc_Position_Exit_struct{ char*
tag;}; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
typedef void* Cyc_Position_Error_kind; extern void* Cyc_Position_Lex; extern
void* Cyc_Position_Parse; extern void* Cyc_Position_Elab; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc;}; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
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
int try_depth; struct _xenum_struct* annot;}; extern void* Cyc_Absyn_Wild_p;
extern const int Cyc_Absyn_Var_p_tag; struct Cyc_Absyn_Var_p_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1;}; extern void* Cyc_Absyn_Null_p; extern const int
Cyc_Absyn_Int_p_tag; struct Cyc_Absyn_Int_p_struct{ int tag; void* f1; int f2;};
extern const int Cyc_Absyn_Char_p_tag; struct Cyc_Absyn_Char_p_struct{ int tag;
char f1;}; extern const int Cyc_Absyn_Float_p_tag; struct Cyc_Absyn_Float_p_struct{
int tag; struct _tagged_string f1;}; extern const int Cyc_Absyn_Tuple_p_tag;
struct Cyc_Absyn_Tuple_p_struct{ int tag; struct Cyc_List_List* f1;}; extern
const int Cyc_Absyn_Pointer_p_tag; struct Cyc_Absyn_Pointer_p_struct{ int tag;
struct Cyc_Absyn_Pat* f1;}; extern const int Cyc_Absyn_Reference_p_tag; struct
Cyc_Absyn_Reference_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;}; extern
const int Cyc_Absyn_Struct_p_tag; struct Cyc_Absyn_Struct_p_struct{ int tag;
struct Cyc_Absyn_Structdecl* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
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
struct Cyc_Absyn_EmptyAnnot_struct{ char* tag;}; extern void* Cyc_Absyn_exn_typ;
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt( void* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt*
s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;};
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag;}; struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef
struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{ char* tag;};
struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef
struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[ 8u];
struct Cyc_Dict_Present_struct{ char* tag;}; extern char Cyc_Dict_Absent_tag[ 7u];
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
Cyc_Tcenv_tenv_t; extern void* Cyc_Tcenv_return_typ( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_pat_var( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Vardecl*); extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Stmt* continue_dest); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru(
struct Cyc_Tcenv_Tenv* te, struct _tuple3* pat_typ, struct Cyc_Absyn_Switch_clause*
clause); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_next( struct Cyc_Tcenv_Tenv*, void*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try( struct Cyc_Tcenv_Tenv* te);
extern int Cyc_Tcenv_get_try_depth( struct Cyc_Tcenv_Tenv* te); extern void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Stmt**); extern
void Cyc_Tcenv_process_break( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Stmt**); extern void Cyc_Tcenv_process_goto( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*, struct _tagged_string*, struct Cyc_Absyn_Stmt**); extern
struct _tuple4* Cyc_Tcenv_process_fallthru( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Switch_clause***); extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_label( struct Cyc_Tcenv_Tenv*, struct _tagged_string*, struct Cyc_Absyn_Stmt*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_block( struct Cyc_Tcenv_Tenv*);
extern void* Cyc_Tcenv_curr_rgn( struct Cyc_Tcenv_Tenv*); extern char Cyc_Tcutil_TypeErr_tag[
8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag;}; extern void* Cyc_Tcutil_impos(
struct _tagged_string); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern int
Cyc_Tcutil_coerce_arg( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*);
extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_coerce_use( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*,
void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern struct _tuple5* Cyc_Tcutil_make_inst_var(
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List* bound_tvars, void* k, void*);
extern void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*);
extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*, void**, struct
Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e, struct _tagged_string msg_part); extern struct _tuple3* Cyc_Tcpat_tcPat(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p, void** region_opt); extern
void Cyc_Tcpat_check_pat_regions( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p); extern void Cyc_Tcpat_check_switch_exhaustive( struct Cyc_Position_Segment*,
struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive( struct
Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); typedef void* Cyc_CfFlowInfo_LocalRoot;
typedef void* Cyc_CfFlowInfo_Field; struct Cyc_CfFlowInfo_Place; typedef void*
Cyc_CfFlowInfo_Escaped; typedef void* Cyc_CfFlowInfo_InitLevel; typedef void*
Cyc_CfFlowInfo_InitState; typedef void* Cyc_CfFlowInfo_PathInfo; typedef void*
Cyc_CfFlowInfo_FlowInfo; typedef void* Cyc_CfFlowInfo_local_root_t; typedef void*
Cyc_CfFlowInfo_field_t; typedef struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t;
typedef void* Cyc_CfFlowInfo_escaped_t; typedef void* Cyc_CfFlowInfo_init_level_t;
typedef void* Cyc_CfFlowInfo_init_state_t; typedef void* Cyc_CfFlowInfo_path_info_t;
typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_pinfo_dict_t; typedef void* Cyc_CfFlowInfo_flow_info_t;
extern const int Cyc_CfFlowInfo_VarRoot_tag; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1;}; extern const int Cyc_CfFlowInfo_MallocPt_tag;
struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1;};
extern const int Cyc_CfFlowInfo_StructF_tag; struct Cyc_CfFlowInfo_StructF_struct{
int tag; struct _tagged_string* f1;}; extern const int Cyc_CfFlowInfo_TupleF_tag;
struct Cyc_CfFlowInfo_TupleF_struct{ int tag; int f1;}; struct Cyc_CfFlowInfo_Place{
void* root; struct Cyc_List_List* fields;}; extern void* Cyc_CfFlowInfo_Esc;
extern void* Cyc_CfFlowInfo_Unesc; extern void* Cyc_CfFlowInfo_NoneIL; extern
void* Cyc_CfFlowInfo_ThisIL; extern void* Cyc_CfFlowInfo_AllIL; extern const int
Cyc_CfFlowInfo_UnknownIS_tag; struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag;
void* f1; void* f2;}; extern const int Cyc_CfFlowInfo_MustPointTo_tag; struct
Cyc_CfFlowInfo_MustPointTo_struct{ int tag; struct Cyc_CfFlowInfo_Place* f1;};
extern const int Cyc_CfFlowInfo_LeafPI_tag; struct Cyc_CfFlowInfo_LeafPI_struct{
int tag; void* f1;}; extern const int Cyc_CfFlowInfo_TuplePI_tag; struct Cyc_CfFlowInfo_TuplePI_struct{
int tag; struct Cyc_Dict_Dict* f1;}; extern const int Cyc_CfFlowInfo_StructPI_tag;
struct Cyc_CfFlowInfo_StructPI_struct{ int tag; struct Cyc_Dict_Dict* f1;};
extern void* Cyc_CfFlowInfo_BottomFL; extern const int Cyc_CfFlowInfo_InitsFL_tag;
struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1;};
extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt* enclosee,
struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{ struct Cyc_Dict_Dict*
roots; int in_try; void* tryflow;}; typedef struct Cyc_NewControlFlow_AnalEnv*
Cyc_NewControlFlow_analenv_t; void Cyc_NewControlFlow_cf_check( struct Cyc_List_List*
ds); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
s, int new_block); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); static void Cyc_Tcstmt_decorate_stmt( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
s){ Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te)); s->try_depth=
Cyc_Tcenv_get_try_depth( te);} static void Cyc_Tcstmt_pattern_synth( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* tvs, struct Cyc_List_List*
vs, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* where_opt, int new_block){
struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars( loc, te, tvs); if(
new_block){ te2= Cyc_Tcenv_new_block( te2);}{ struct Cyc_Core_Opt* ropt=({
struct Cyc_Core_Opt* _temp0=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp0->v=( void*) Cyc_Tcenv_curr_rgn( te2); _temp0;});{ struct
Cyc_List_List* vs2= vs; for( 0; vs2 != 0; vs2= vs2->tl){ te2= Cyc_Tcenv_add_pat_var(
loc, te2,( struct Cyc_Absyn_Vardecl*) vs2->hd);(( struct Cyc_Absyn_Vardecl*) vs2->hd)->region=(
struct Cyc_Core_Opt*) ropt;}} if( where_opt != 0){ Cyc_Tcexp_tcTest( te2,(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1= where_opt; if( _temp1 ==
0){ _throw( Null_Exception);} _temp1;}),( struct _tagged_string)({ char* _temp2=(
char*)"switch clause guard"; struct _tagged_string _temp3; _temp3.curr= _temp2;
_temp3.base= _temp2; _temp3.last_plus_one= _temp2 + 20; _temp3;}));} if( vs != 0){
te2= Cyc_Tcenv_set_encloser( te2, s);} Cyc_Tcstmt_tcStmt( te2, s, 0); if( vs !=
0){ Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te));}}} void
Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s0, int
new_block){ Cyc_Tcstmt_decorate_stmt( te, s0);{ void* _temp4=( void*) s0->r;
struct Cyc_Absyn_Exp* _temp42; struct Cyc_Absyn_Stmt* _temp44; struct Cyc_Absyn_Stmt*
_temp46; struct Cyc_Absyn_Exp* _temp48; struct Cyc_Absyn_Stmt* _temp50; struct
Cyc_Absyn_Stmt* _temp52; struct Cyc_Absyn_Exp* _temp54; struct Cyc_Absyn_Stmt*
_temp56; struct _tuple2 _temp58; struct Cyc_Absyn_Stmt* _temp60; struct Cyc_Absyn_Exp*
_temp62; struct Cyc_Absyn_Stmt* _temp64; struct _tuple2 _temp66; struct Cyc_Absyn_Stmt*
_temp68; struct Cyc_Absyn_Exp* _temp70; struct _tuple2 _temp72; struct Cyc_Absyn_Stmt*
_temp74; struct Cyc_Absyn_Exp* _temp76; struct Cyc_Absyn_Exp* _temp78; struct
_tuple2 _temp80; struct Cyc_Absyn_Stmt* _temp82; struct Cyc_Absyn_Exp* _temp84;
struct Cyc_Absyn_Stmt* _temp86; struct Cyc_Absyn_Stmt* _temp88; struct Cyc_Absyn_Stmt**
_temp90; struct Cyc_Absyn_Stmt* _temp91; struct Cyc_Absyn_Stmt** _temp93; struct
Cyc_Absyn_Stmt* _temp94; struct Cyc_Absyn_Stmt** _temp96; struct _tagged_string*
_temp97; struct Cyc_Absyn_Switch_clause** _temp99; struct Cyc_Absyn_Switch_clause***
_temp101; struct Cyc_List_List* _temp102; struct Cyc_Absyn_Stmt* _temp104;
struct _tagged_string* _temp106; struct Cyc_List_List* _temp108; struct Cyc_Absyn_Exp*
_temp110; struct Cyc_List_List* _temp112; struct Cyc_Absyn_Stmt* _temp114;
struct Cyc_Absyn_Stmt* _temp116; struct Cyc_Absyn_Decl* _temp118; struct Cyc_Absyn_Stmt*
_temp120; struct Cyc_Absyn_Stmt* _temp122; _LL6: if( _temp4 == Cyc_Absyn_Skip_s){
goto _LL7;} else{ goto _LL8;} _LL8: if(( unsigned int) _temp4 > 1u?(( struct
_enum_struct*) _temp4)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL43: _temp42=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp4)->f1; goto _LL9;} else{
goto _LL10;} _LL10: if(( unsigned int) _temp4 > 1u?(( struct _enum_struct*)
_temp4)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL47: _temp46=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp4)->f1; goto _LL45; _LL45: _temp44=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp4)->f2; goto _LL11;}
else{ goto _LL12;} _LL12: if(( unsigned int) _temp4 > 1u?(( struct _enum_struct*)
_temp4)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL49: _temp48=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp4)->f1; goto _LL13;} else{ goto _LL14;}
_LL14: if(( unsigned int) _temp4 > 1u?(( struct _enum_struct*) _temp4)->tag ==
Cyc_Absyn_IfThenElse_s_tag: 0){ _LL55: _temp54=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp4)->f1; goto _LL53; _LL53: _temp52=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp4)->f2; goto
_LL51; _LL51: _temp50=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp4)->f3; goto _LL15;} else{ goto _LL16;} _LL16: if(( unsigned int) _temp4 >
1u?(( struct _enum_struct*) _temp4)->tag == Cyc_Absyn_While_s_tag: 0){ _LL59:
_temp58=( struct _tuple2)(( struct Cyc_Absyn_While_s_struct*) _temp4)->f1; _LL63:
_temp62= _temp58.f1; goto _LL61; _LL61: _temp60= _temp58.f2; goto _LL57; _LL57:
_temp56=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*) _temp4)->f2;
goto _LL17;} else{ goto _LL18;} _LL18: if(( unsigned int) _temp4 > 1u?(( struct
_enum_struct*) _temp4)->tag == Cyc_Absyn_For_s_tag: 0){ _LL79: _temp78=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*) _temp4)->f1; goto _LL73; _LL73:
_temp72=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*) _temp4)->f2; _LL77:
_temp76= _temp72.f1; goto _LL75; _LL75: _temp74= _temp72.f2; goto _LL67; _LL67:
_temp66=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*) _temp4)->f3; _LL71:
_temp70= _temp66.f1; goto _LL69; _LL69: _temp68= _temp66.f2; goto _LL65; _LL65:
_temp64=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp4)->f4;
goto _LL19;} else{ goto _LL20;} _LL20: if(( unsigned int) _temp4 > 1u?(( struct
_enum_struct*) _temp4)->tag == Cyc_Absyn_Do_s_tag: 0){ _LL87: _temp86=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp4)->f1; goto _LL81; _LL81:
_temp80=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*) _temp4)->f2; _LL85:
_temp84= _temp80.f1; goto _LL83; _LL83: _temp82= _temp80.f2; goto _LL21;} else{
goto _LL22;} _LL22: if(( unsigned int) _temp4 > 1u?(( struct _enum_struct*)
_temp4)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL89: _temp88=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp4)->f1; _temp90=&(( struct Cyc_Absyn_Break_s_struct*)
_temp4)->f1; goto _LL23;} else{ goto _LL24;} _LL24: if(( unsigned int) _temp4 >
1u?(( struct _enum_struct*) _temp4)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL92:
_temp91=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*) _temp4)->f1;
_temp93=&(( struct Cyc_Absyn_Continue_s_struct*) _temp4)->f1; goto _LL25;} else{
goto _LL26;} _LL26: if(( unsigned int) _temp4 > 1u?(( struct _enum_struct*)
_temp4)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL98: _temp97=( struct _tagged_string*)((
struct Cyc_Absyn_Goto_s_struct*) _temp4)->f1; goto _LL95; _LL95: _temp94=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*) _temp4)->f2; _temp96=&((
struct Cyc_Absyn_Goto_s_struct*) _temp4)->f2; goto _LL27;} else{ goto _LL28;}
_LL28: if(( unsigned int) _temp4 > 1u?(( struct _enum_struct*) _temp4)->tag ==
Cyc_Absyn_Fallthru_s_tag: 0){ _LL103: _temp102=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Fallthru_s_struct*) _temp4)->f1; goto _LL100; _LL100: _temp99=( struct
Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*) _temp4)->f2;
_temp101=&(( struct Cyc_Absyn_Fallthru_s_struct*) _temp4)->f2; goto _LL29;}
else{ goto _LL30;} _LL30: if(( unsigned int) _temp4 > 1u?(( struct _enum_struct*)
_temp4)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL107: _temp106=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp4)->f1; goto _LL105;
_LL105: _temp104=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp4)->f2; goto _LL31;} else{ goto _LL32;} _LL32: if(( unsigned int) _temp4 >
1u?(( struct _enum_struct*) _temp4)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL111:
_temp110=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*) _temp4)->f1;
goto _LL109; _LL109: _temp108=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp4)->f2; goto _LL33;} else{ goto _LL34;} _LL34: if(( unsigned int) _temp4 >
1u?(( struct _enum_struct*) _temp4)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL115:
_temp114=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*) _temp4)->f1;
goto _LL113; _LL113: _temp112=( struct Cyc_List_List*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp4)->f2; goto _LL35;} else{ goto _LL36;} _LL36: if(( unsigned int) _temp4 >
1u?(( struct _enum_struct*) _temp4)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL119:
_temp118=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp4)->f1;
goto _LL117; _LL117: _temp116=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp4)->f2; goto _LL37;} else{ goto _LL38;} _LL38: if(( unsigned int) _temp4 >
1u?(( struct _enum_struct*) _temp4)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL121:
_temp120=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*) _temp4)->f1;
goto _LL39;} else{ goto _LL40;} _LL40: if(( unsigned int) _temp4 > 1u?(( struct
_enum_struct*) _temp4)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL123: _temp122=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*) _temp4)->f1; goto
_LL41;} else{ goto _LL5;} _LL7: return; _LL9: Cyc_Tcexp_tcExp( te, 0, _temp42);
return; _LL11: { struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_set_next( te,({ struct
Cyc_Tcenv_Stmt_j_struct* _temp124=( struct Cyc_Tcenv_Stmt_j_struct*) GC_malloc(
sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp124->tag= Cyc_Tcenv_Stmt_j_tag;
_temp124->f1= _temp44;( void*) _temp124;})); Cyc_Tcstmt_tcStmt( te2, _temp46, 1);
while( 1) { void* _temp125=( void*) _temp44->r; struct Cyc_Absyn_Stmt* _temp131;
struct Cyc_Absyn_Stmt* _temp133; _LL127: if(( unsigned int) _temp125 > 1u?((
struct _enum_struct*) _temp125)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL134:
_temp133=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp125)->f1;
goto _LL132; _LL132: _temp131=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp125)->f2; goto _LL128;} else{ goto _LL129;} _LL129: goto _LL130; _LL128:
Cyc_Tcstmt_decorate_stmt( te, _temp44); te2= Cyc_Tcenv_set_next( te,({ struct
Cyc_Tcenv_Stmt_j_struct* _temp135=( struct Cyc_Tcenv_Stmt_j_struct*) GC_malloc(
sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp135->tag= Cyc_Tcenv_Stmt_j_tag;
_temp135->f1= _temp131;( void*) _temp135;})); Cyc_Tcstmt_tcStmt( te2, _temp133,
1); _temp44= _temp131; continue; _LL130: goto seq_end; _LL126:;} seq_end: Cyc_Tcstmt_tcStmt(
te, _temp44, 1); return;} _LL13: { void* t= Cyc_Tcenv_return_typ( te); if(
_temp48 == 0){ void* _temp136= Cyc_Tcutil_compress( t); _LL138: if( _temp136 ==
Cyc_Absyn_VoidType){ goto _LL139;} else{ goto _LL140;} _LL140: goto _LL141;
_LL139: goto _LL137; _LL141: Cyc_Tcutil_terr( s0->loc,({ struct _tagged_string
_temp142= Cyc_Absynpp_typ2string( t); xprintf("must return a value of type %.*s",
_temp142.last_plus_one - _temp142.curr, _temp142.curr);})); goto _LL137; _LL137:;}
else{ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp143= _temp48; if( _temp143 == 0){ _throw( Null_Exception);} _temp143;});
Cyc_Tcexp_tcExp( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_use( te, e, t)){
Cyc_Tcutil_terr( s0->loc,({ struct _tagged_string _temp144= Cyc_Absynpp_typ2string((
void*)( e->topt)->v); struct _tagged_string _temp145= Cyc_Absynpp_typ2string( t);
xprintf("returns value of type %.*s but requires %.*s", _temp144.last_plus_one -
_temp144.curr, _temp144.curr, _temp145.last_plus_one - _temp145.curr, _temp145.curr);}));}{
void* _temp146= Cyc_Tcutil_compress( t); _LL148: if( _temp146 == Cyc_Absyn_VoidType){
goto _LL149;} else{ goto _LL150;} _LL150: goto _LL151; _LL149: Cyc_Tcutil_terr(
s0->loc,( struct _tagged_string)({ char* _temp152=( char*)"function returns void expression";
struct _tagged_string _temp153; _temp153.curr= _temp152; _temp153.base= _temp152;
_temp153.last_plus_one= _temp152 + 33; _temp153;})); goto _LL147; _LL151: goto
_LL147; _LL147:;}} return;} _LL15: Cyc_Tcexp_tcTest( te, _temp54,( struct
_tagged_string)({ char* _temp154=( char*)"if statement"; struct _tagged_string
_temp155; _temp155.curr= _temp154; _temp155.base= _temp154; _temp155.last_plus_one=
_temp154 + 13; _temp155;})); Cyc_Tcstmt_tcStmt( te, _temp52, 1); Cyc_Tcstmt_tcStmt(
te, _temp50, 1); return; _LL17: Cyc_Tcstmt_decorate_stmt( te, _temp60); Cyc_Tcexp_tcTest(
te, _temp62,( struct _tagged_string)({ char* _temp156=( char*)"while loop";
struct _tagged_string _temp157; _temp157.curr= _temp156; _temp157.base= _temp156;
_temp157.last_plus_one= _temp156 + 11; _temp157;})); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_in_loop(
te, _temp60), _temp56, 1); return; _LL19: Cyc_Tcstmt_decorate_stmt( te, _temp74);
Cyc_Tcstmt_decorate_stmt( te, _temp68); Cyc_Tcexp_tcExp( te, 0, _temp78); Cyc_Tcexp_tcTest(
te, _temp76,( struct _tagged_string)({ char* _temp158=( char*)"for loop"; struct
_tagged_string _temp159; _temp159.curr= _temp158; _temp159.base= _temp158;
_temp159.last_plus_one= _temp158 + 9; _temp159;})); te= Cyc_Tcenv_set_in_loop(
te, _temp68); Cyc_Tcstmt_tcStmt( te, _temp64, 1); Cyc_Tcexp_tcExp( te, 0,
_temp70); return; _LL21: Cyc_Tcstmt_decorate_stmt( te, _temp82); Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_in_loop( te, _temp82), _temp86, 1); Cyc_Tcexp_tcTest( te, _temp84,(
struct _tagged_string)({ char* _temp160=( char*)"do loop"; struct _tagged_string
_temp161; _temp161.curr= _temp160; _temp161.base= _temp160; _temp161.last_plus_one=
_temp160 + 8; _temp161;})); return; _LL23: Cyc_Tcenv_process_break( te, s0,
_temp90); return; _LL25: Cyc_Tcenv_process_continue( te, s0, _temp93); return;
_LL27: Cyc_Tcenv_process_goto( te, s0, _temp97, _temp96); return; _LL29: {
struct _tuple4* trip_opt= Cyc_Tcenv_process_fallthru( te, s0, _temp101); if(
trip_opt == 0){ Cyc_Tcutil_terr( s0->loc,( struct _tagged_string)({ char*
_temp162=( char*)"fallthru not in a non-last case"; struct _tagged_string
_temp163; _temp163.curr= _temp162; _temp163.base= _temp162; _temp163.last_plus_one=
_temp162 + 32; _temp163;})); return;}{ struct _tuple4 _temp167; struct Cyc_List_List*
_temp168; struct Cyc_List_List* _temp170; struct Cyc_Absyn_Switch_clause*
_temp172; struct _tuple4* _temp165=( struct _tuple4*)({ struct _tuple4* _temp164=
trip_opt; if( _temp164 == 0){ _throw( Null_Exception);} _temp164;}); _temp167=*
_temp165; _LL173: _temp172= _temp167.f1; goto _LL171; _LL171: _temp170= _temp167.f2;
goto _LL169; _LL169: _temp168= _temp167.f3; goto _LL166; _LL166: { struct Cyc_List_List*
instantiation=(( struct Cyc_List_List*(*)( struct _tuple5*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcutil_make_inst_var, _temp170);
_temp168=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_List_List*, void*),
struct Cyc_List_List* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_substitute,
instantiation, _temp168); for( 0; _temp168 != 0? _temp102 != 0: 0; _temp168=
_temp168->tl, _temp102= _temp102->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)
_temp102->hd); if( ! Cyc_Tcutil_coerce_arg( te,( struct Cyc_Absyn_Exp*) _temp102->hd,(
void*) _temp168->hd)){ Cyc_Tcutil_terr( s0->loc,({ struct _tagged_string
_temp174= Cyc_Absynpp_typ2string(( void*)((( struct Cyc_Absyn_Exp*) _temp102->hd)->topt)->v);
struct _tagged_string _temp175= Cyc_Absynpp_typ2string(( void*) _temp168->hd);
xprintf("fallthru argument has type %.*s but pattern variable has type %.*s",
_temp174.last_plus_one - _temp174.curr, _temp174.curr, _temp175.last_plus_one -
_temp175.curr, _temp175.curr);}));}} if( _temp102 != 0){ Cyc_Tcutil_terr( s0->loc,(
struct _tagged_string)({ char* _temp176=( char*)"too many arguments to explicit fallthru";
struct _tagged_string _temp177; _temp177.curr= _temp176; _temp177.base= _temp176;
_temp177.last_plus_one= _temp176 + 40; _temp177;}));} if( _temp168 != 0){ Cyc_Tcutil_terr(
s0->loc,( struct _tagged_string)({ char* _temp178=( char*)"too few arguments to explicit fallthru";
struct _tagged_string _temp179; _temp179.curr= _temp178; _temp179.base= _temp178;
_temp179.last_plus_one= _temp178 + 39; _temp179;}));} return;}}} _LL31: Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_add_label( te, _temp106, _temp104), _temp104, 1); return; _LL33: Cyc_Tcexp_tcExp(
te, 0, _temp110);{ void* t=( void*)( _temp110->topt)->v; te= Cyc_Tcenv_set_in_switch(
te); te= Cyc_Tcenv_clear_fallthru( te);{ struct Cyc_List_List* scs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp108); for( 0; scs != 0; scs= scs->tl){
struct Cyc_Absyn_Pat* p=(( struct Cyc_Absyn_Switch_clause*) scs->hd)->pattern;
struct _tuple3* pat_typ= Cyc_Tcpat_tcPat( te, p, 0); struct _tuple3 _temp182;
struct Cyc_List_List* _temp183; struct Cyc_List_List* _temp185; struct _tuple3*
_temp180= pat_typ; _temp182=* _temp180; _LL186: _temp185= _temp182.f1; goto
_LL184; _LL184: _temp183= _temp182.f2; goto _LL181; _LL181: if( ! Cyc_Tcutil_unify((
void*)( p->topt)->v, t)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Switch_clause*)
scs->hd)->loc,({ struct _tagged_string _temp187= Cyc_Absynpp_typ2string( t);
struct _tagged_string _temp188= Cyc_Absynpp_typ2string(( void*)( p->topt)->v);
xprintf("switch on type %.*s, but case expects type %.*s", _temp187.last_plus_one
- _temp187.curr, _temp187.curr, _temp188.last_plus_one - _temp188.curr, _temp188.curr);}));}
else{ Cyc_Tcpat_check_pat_regions( te, p);}(( struct Cyc_Absyn_Switch_clause*)
scs->hd)->pat_vars=({ struct Cyc_Core_Opt* _temp189=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp189->v=( void*) _temp183;
_temp189;}); Cyc_Tcstmt_pattern_synth((( struct Cyc_Absyn_Switch_clause*) scs->hd)->loc,
te, _temp185, _temp183,(( struct Cyc_Absyn_Switch_clause*) scs->hd)->body,((
struct Cyc_Absyn_Switch_clause*) scs->hd)->where_clause, 1); te= Cyc_Tcenv_set_fallthru(
te,( struct _tuple3*) pat_typ,( struct Cyc_Absyn_Switch_clause*) scs->hd);}} Cyc_Tcpat_check_switch_exhaustive(
s0->loc, _temp108); return;} _LL35:( void*)( _temp114->r=( void*)(( void*)( Cyc_Absyn_seq_stmt(
Cyc_Absyn_new_stmt(( void*) _temp114->r, _temp114->loc), Cyc_Absyn_skip_stmt(
_temp114->loc), _temp114->loc))->r)); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
Cyc_Tcenv_enter_try( te), s0), _temp114, 1); te= Cyc_Tcenv_set_in_switch( te);
te= Cyc_Tcenv_clear_fallthru( te);{ struct Cyc_List_List* scs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( _temp112); for( 0; scs != 0; scs= scs->tl){
struct Cyc_Absyn_Pat* p=(( struct Cyc_Absyn_Switch_clause*) scs->hd)->pattern;
struct _tuple3* pat_typ= Cyc_Tcpat_tcPat( te, p, 0); struct _tuple3 _temp192;
struct Cyc_List_List* _temp193; struct Cyc_List_List* _temp195; struct _tuple3*
_temp190= pat_typ; _temp192=* _temp190; _LL196: _temp195= _temp192.f1; goto
_LL194; _LL194: _temp193= _temp192.f2; goto _LL191; _LL191: if( ! Cyc_Tcutil_unify((
void*)( p->topt)->v, Cyc_Absyn_exn_typ)){ Cyc_Tcutil_terr((( struct Cyc_Absyn_Switch_clause*)
scs->hd)->loc,({ struct _tagged_string _temp197= Cyc_Absynpp_typ2string(( void*)(
p->topt)->v); xprintf("expected xenum exn but found %.*s", _temp197.last_plus_one
- _temp197.curr, _temp197.curr);}));} else{ Cyc_Tcpat_check_pat_regions( te, p);}((
struct Cyc_Absyn_Switch_clause*) scs->hd)->pat_vars=({ struct Cyc_Core_Opt*
_temp198=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp198->v=( void*) _temp193; _temp198;}); Cyc_Tcstmt_pattern_synth((( struct
Cyc_Absyn_Switch_clause*) scs->hd)->loc, te, _temp195, _temp193,(( struct Cyc_Absyn_Switch_clause*)
scs->hd)->body,(( struct Cyc_Absyn_Switch_clause*) scs->hd)->where_clause, 1);
te= Cyc_Tcenv_set_fallthru( te,( struct _tuple3*) pat_typ,( struct Cyc_Absyn_Switch_clause*)
scs->hd);}} Cyc_Tcpat_check_catch_overlap( s0->loc, _temp112); return; _LL37: {
struct _tagged_string unimp_msg_part; if( new_block){ te= Cyc_Tcenv_new_block(
te);}{ void* _temp199=( void*) _temp118->r; struct Cyc_Absyn_Vardecl* _temp223;
int _temp225; int* _temp227; struct Cyc_Absyn_Exp* _temp228; struct Cyc_Core_Opt*
_temp230; struct Cyc_Core_Opt** _temp232; struct Cyc_Core_Opt* _temp233; struct
Cyc_Core_Opt** _temp235; struct Cyc_Absyn_Pat* _temp236; struct Cyc_List_List*
_temp238; struct _tagged_string* _temp240; struct Cyc_List_List* _temp242;
struct _tuple0* _temp244; struct Cyc_Absyn_Fndecl* _temp246; struct Cyc_Absyn_Structdecl*
_temp248; struct Cyc_Absyn_Enumdecl* _temp250; struct Cyc_Absyn_Xenumdecl*
_temp252; struct Cyc_Absyn_Typedefdecl* _temp254; struct Cyc_List_List* _temp256;
_LL201: if(( unsigned int) _temp199 > 1u?(( struct _enum_struct*) _temp199)->tag
== Cyc_Absyn_Var_d_tag: 0){ _LL224: _temp223=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp199)->f1; goto _LL202;} else{ goto _LL203;}
_LL203: if(( unsigned int) _temp199 > 1u?(( struct _enum_struct*) _temp199)->tag
== Cyc_Absyn_Let_d_tag: 0){ _LL237: _temp236=( struct Cyc_Absyn_Pat*)(( struct
Cyc_Absyn_Let_d_struct*) _temp199)->f1; goto _LL234; _LL234: _temp233=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp199)->f2; _temp235=&((
struct Cyc_Absyn_Let_d_struct*) _temp199)->f2; goto _LL231; _LL231: _temp230=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp199)->f3; _temp232=&((
struct Cyc_Absyn_Let_d_struct*) _temp199)->f3; goto _LL229; _LL229: _temp228=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp199)->f4; goto
_LL226; _LL226: _temp225=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp199)->f5;
_temp227=&(( struct Cyc_Absyn_Let_d_struct*) _temp199)->f5; goto _LL204;} else{
goto _LL205;} _LL205: if(( unsigned int) _temp199 > 1u?(( struct _enum_struct*)
_temp199)->tag == Cyc_Absyn_Namespace_d_tag: 0){ _LL241: _temp240=( struct
_tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp199)->f1; goto
_LL239; _LL239: _temp238=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp199)->f2; goto _LL206;} else{ goto _LL207;} _LL207: if(( unsigned int)
_temp199 > 1u?(( struct _enum_struct*) _temp199)->tag == Cyc_Absyn_Using_d_tag:
0){ _LL245: _temp244=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp199)->f1; goto _LL243; _LL243: _temp242=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Using_d_struct*) _temp199)->f2; goto _LL208;} else{ goto _LL209;}
_LL209: if(( unsigned int) _temp199 > 1u?(( struct _enum_struct*) _temp199)->tag
== Cyc_Absyn_Fn_d_tag: 0){ _LL247: _temp246=( struct Cyc_Absyn_Fndecl*)(( struct
Cyc_Absyn_Fn_d_struct*) _temp199)->f1; goto _LL210;} else{ goto _LL211;} _LL211:
if(( unsigned int) _temp199 > 1u?(( struct _enum_struct*) _temp199)->tag == Cyc_Absyn_Struct_d_tag:
0){ _LL249: _temp248=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp199)->f1; goto _LL212;} else{ goto _LL213;} _LL213: if( _temp199 == Cyc_Absyn_Union_d){
goto _LL214;} else{ goto _LL215;} _LL215: if(( unsigned int) _temp199 > 1u?((
struct _enum_struct*) _temp199)->tag == Cyc_Absyn_Enum_d_tag: 0){ _LL251:
_temp250=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp199)->f1; goto _LL216;} else{ goto _LL217;} _LL217: if(( unsigned int)
_temp199 > 1u?(( struct _enum_struct*) _temp199)->tag == Cyc_Absyn_Xenum_d_tag:
0){ _LL253: _temp252=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*)
_temp199)->f1; goto _LL218;} else{ goto _LL219;} _LL219: if(( unsigned int)
_temp199 > 1u?(( struct _enum_struct*) _temp199)->tag == Cyc_Absyn_Typedef_d_tag:
0){ _LL255: _temp254=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp199)->f1; goto _LL220;} else{ goto _LL221;} _LL221: if(( unsigned int)
_temp199 > 1u?(( struct _enum_struct*) _temp199)->tag == Cyc_Absyn_ExternC_d_tag:
0){ _LL257: _temp256=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp199)->f1; goto _LL222;} else{ goto _LL200;} _LL202: { struct Cyc_Absyn_Vardecl
_temp260; struct Cyc_Core_Opt* _temp261; struct Cyc_Core_Opt** _temp263; int
_temp264; struct Cyc_Absyn_Exp* _temp266; void* _temp268; struct Cyc_Absyn_Tqual*
_temp270; struct _tuple0* _temp272; struct _tuple0 _temp274; struct
_tagged_string* _temp275; void* _temp277; void* _temp279; struct Cyc_Absyn_Vardecl*
_temp258= _temp223; _temp260=* _temp258; _LL280: _temp279=( void*) _temp260.sc;
goto _LL273; _LL273: _temp272=( struct _tuple0*) _temp260.name; _temp274=*
_temp272; _LL278: _temp277= _temp274.f1; goto _LL276; _LL276: _temp275= _temp274.f2;
goto _LL271; _LL271: _temp270=( struct Cyc_Absyn_Tqual*) _temp260.tq; goto
_LL269; _LL269: _temp268=( void*) _temp260.type; goto _LL267; _LL267: _temp266=(
struct Cyc_Absyn_Exp*) _temp260.initializer; goto _LL265; _LL265: _temp264=( int)
_temp260.shadow; goto _LL262; _LL262: _temp261=( struct Cyc_Core_Opt*) _temp260.region;
_temp263=&(* _temp258).region; goto _LL259; _LL259: { void* curr_bl= Cyc_Tcenv_curr_rgn(
te);* _temp263=({ struct Cyc_Core_Opt* _temp281=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp281->v=( void*) curr_bl; _temp281;});{
void* _temp282= _temp277; struct Cyc_List_List* _temp292; struct Cyc_List_List*
_temp294; _LL284: if( _temp282 == Cyc_Absyn_Loc_n){ goto _LL285;} else{ goto
_LL286;} _LL286: if(( unsigned int) _temp282 > 1u?(( struct _enum_struct*)
_temp282)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL293: _temp292=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp282)->f1; if( _temp292 == 0){ goto _LL287;}
else{ goto _LL288;}} else{ goto _LL288;} _LL288: if(( unsigned int) _temp282 > 1u?((
struct _enum_struct*) _temp282)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL295:
_temp294=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp282)->f1;
goto _LL289;} else{ goto _LL290;} _LL290: goto _LL291; _LL285: goto _LL283;
_LL287:(* _temp223->name).f1= Cyc_Absyn_Loc_n; goto _LL283; _LL289:( void)
_throw((( struct _xenum_struct*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp296=( char*)"tcstmt: Abs_n declaration";
struct _tagged_string _temp297; _temp297.curr= _temp296; _temp297.base= _temp296;
_temp297.last_plus_one= _temp296 + 26; _temp297;}))); _LL291: Cyc_Tcutil_terr(
_temp118->loc,( struct _tagged_string)({ char* _temp298=( char*)"cannot declare a qualified local variable";
struct _tagged_string _temp299; _temp299.curr= _temp298; _temp299.base= _temp298;
_temp299.last_plus_one= _temp298 + 42; _temp299;})); goto _LL283; _LL283:;} Cyc_Tcutil_check_type(
s0->loc, te, Cyc_Tcenv_lookup_type_vars( te), Cyc_Absyn_MemKind, _temp268); if(
_temp266 != 0){{ void* _temp300= Cyc_Tcutil_compress( _temp268); struct Cyc_Absyn_Exp*
_temp306; struct Cyc_Absyn_Tqual* _temp308; void* _temp310; _LL302: if((
unsigned int) _temp300 > 5u?(( struct _enum_struct*) _temp300)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL311: _temp310=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp300)->f1;
goto _LL309; _LL309: _temp308=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp300)->f2; goto _LL307; _LL307: _temp306=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp300)->f3; goto _LL303;} else{ goto _LL304;}
_LL304: goto _LL305; _LL303: if( Cyc_Evexp_eval_const_uint_exp( _temp306) ==(
unsigned int) 0){ void* _temp312=( void*) _temp266->r; void* _temp322; struct
_tagged_string _temp324; int _temp326; struct Cyc_List_List* _temp328; struct
Cyc_Core_Opt* _temp330; struct Cyc_List_List* _temp332; int _temp334; _LL314:
if((( struct _enum_struct*) _temp312)->tag == Cyc_Absyn_Const_e_tag){ _LL323:
_temp322=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp312)->f1; if((
unsigned int) _temp322 > 1u?(( struct _enum_struct*) _temp322)->tag == Cyc_Absyn_String_c_tag:
0){ _LL327: _temp326=( int)(( struct Cyc_Absyn_String_c_struct*) _temp322)->f1;
goto _LL325; _LL325: _temp324=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp322)->f2; goto _LL315;} else{ goto _LL316;}} else{ goto _LL316;} _LL316:
if((( struct _enum_struct*) _temp312)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL331: _temp330=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp312)->f1; goto _LL329; _LL329: _temp328=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp312)->f2; goto _LL317;} else{ goto
_LL318;} _LL318: if((( struct _enum_struct*) _temp312)->tag == Cyc_Absyn_Array_e_tag){
_LL335: _temp334=( int)(( struct Cyc_Absyn_Array_e_struct*) _temp312)->f1; goto
_LL333; _LL333: _temp332=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp312)->f2; goto _LL319;} else{ goto _LL320;} _LL320: goto _LL321; _LL315:
_temp268=( void*)( _temp223->type=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp336=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp336->tag= Cyc_Absyn_ArrayType_tag; _temp336->f1=( void*) _temp310; _temp336->f2=
_temp308; _temp336->f3= Cyc_Absyn_uint_exp(({ struct _tagged_string _temp337=
_temp324;( unsigned int)( _temp337.last_plus_one - _temp337.curr);}), 0);( void*)
_temp336;})); goto _LL313; _LL317: _temp332= _temp328; goto _LL319; _LL319:
_temp268=( void*)( _temp223->type=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp338=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp338->tag= Cyc_Absyn_ArrayType_tag; _temp338->f1=( void*) _temp310; _temp338->f2=
_temp308; _temp338->f3= Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp332), 0);( void*) _temp338;})); goto _LL313; _LL321:
goto _LL313; _LL313:;} goto _LL301; _LL305: goto _LL301; _LL301:;} Cyc_Tcexp_tcExpInitializer(
te,( void**)& _temp268,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp339=
_temp266; if( _temp339 == 0){ _throw( Null_Exception);} _temp339;})); if( ! Cyc_Tcutil_coerce_assign(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp340= _temp266; if(
_temp340 == 0){ _throw( Null_Exception);} _temp340;}), _temp268)){ Cyc_Tcutil_terr(
_temp118->loc,({ struct _tagged_string _temp341=* _temp275; struct
_tagged_string _temp342= Cyc_Absynpp_typ2string( _temp268); struct
_tagged_string _temp343= Cyc_Absynpp_typ2string(( void*)( _temp266->topt)->v);
xprintf("%.*s declared with type %.*s, initialized with type %.*s", _temp341.last_plus_one
- _temp341.curr, _temp341.curr, _temp342.last_plus_one - _temp342.curr, _temp342.curr,
_temp343.last_plus_one - _temp343.curr, _temp343.curr);}));}} Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_encloser( Cyc_Tcenv_add_local_var( _temp116->loc, te, _temp223),
s0), _temp116, 0); return;}} _LL204: { struct Cyc_Core_Opt* topt=* _temp232; Cyc_Tcexp_tcExpInitializer(
te, 0, _temp228);* _temp232= _temp228->topt;{ struct _tuple3 _temp346; struct
Cyc_List_List* _temp347; struct Cyc_List_List* _temp349; struct _tuple3*
_temp344= Cyc_Tcpat_tcPat( te, _temp236, 0); _temp346=* _temp344; _LL350:
_temp349= _temp346.f1; goto _LL348; _LL348: _temp347= _temp346.f2; goto _LL345;
_LL345:* _temp235=({ struct Cyc_Core_Opt* _temp351=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp351->v=( void*) _temp347;
_temp351;}); if( _temp349 != 0){ Cyc_Tcutil_terr( _temp118->loc,( struct
_tagged_string)({ char* _temp352=( char*)"can't introduce type variables in let-clause";
struct _tagged_string _temp353; _temp353.curr= _temp352; _temp353.base= _temp352;
_temp353.last_plus_one= _temp352 + 45; _temp353;}));} if( topt != 0? ! Cyc_Tcutil_unify((
void*)( _temp236->topt)->v,( void*) topt->v): 0){ Cyc_Tcutil_terr( _temp118->loc,(
struct _tagged_string)({ char* _temp354=( char*)"type of pattern does not match declared type";
struct _tagged_string _temp355; _temp355.curr= _temp354; _temp355.base= _temp354;
_temp355.last_plus_one= _temp354 + 45; _temp355;}));} if( ! Cyc_Tcutil_unify((
void*)( _temp236->topt)->v,( void*)( _temp228->topt)->v)){ Cyc_Tcutil_terr(
_temp118->loc,({ struct _tagged_string _temp356= Cyc_Absynpp_typ2string(( void*)(
_temp236->topt)->v); struct _tagged_string _temp357= Cyc_Absynpp_typ2string((
void*)( _temp228->topt)->v); xprintf("pattern type %.*s does not match definition type %.*s",
_temp356.last_plus_one - _temp356.curr, _temp356.curr, _temp357.last_plus_one -
_temp357.curr, _temp357.curr);}));} else{ Cyc_Tcpat_check_pat_regions( te,
_temp236);}* _temp227= Cyc_Tcpat_check_let_pat_exhaustive( _temp236->loc,
_temp236); Cyc_Tcstmt_pattern_synth( s0->loc, te, 0, _temp347, _temp116, 0, 0);
return;}} _LL206: unimp_msg_part=( struct _tagged_string)({ char* _temp358=(
char*)"namespace"; struct _tagged_string _temp359; _temp359.curr= _temp358;
_temp359.base= _temp358; _temp359.last_plus_one= _temp358 + 10; _temp359;});
goto _LL200; _LL208: unimp_msg_part=( struct _tagged_string)({ char* _temp360=(
char*)"using"; struct _tagged_string _temp361; _temp361.curr= _temp360; _temp361.base=
_temp360; _temp361.last_plus_one= _temp360 + 6; _temp361;}); goto _LL200; _LL210:
unimp_msg_part=( struct _tagged_string)({ char* _temp362=( char*)"function";
struct _tagged_string _temp363; _temp363.curr= _temp362; _temp363.base= _temp362;
_temp363.last_plus_one= _temp362 + 9; _temp363;}); goto _LL200; _LL212:
unimp_msg_part=( struct _tagged_string)({ char* _temp364=( char*)"struct";
struct _tagged_string _temp365; _temp365.curr= _temp364; _temp365.base= _temp364;
_temp365.last_plus_one= _temp364 + 7; _temp365;}); goto _LL200; _LL214:
unimp_msg_part=( struct _tagged_string)({ char* _temp366=( char*)"union"; struct
_tagged_string _temp367; _temp367.curr= _temp366; _temp367.base= _temp366;
_temp367.last_plus_one= _temp366 + 6; _temp367;}); goto _LL200; _LL216:
unimp_msg_part=( struct _tagged_string)({ char* _temp368=( char*)"enum"; struct
_tagged_string _temp369; _temp369.curr= _temp368; _temp369.base= _temp368;
_temp369.last_plus_one= _temp368 + 5; _temp369;}); goto _LL200; _LL218:
unimp_msg_part=( struct _tagged_string)({ char* _temp370=( char*)"xenum"; struct
_tagged_string _temp371; _temp371.curr= _temp370; _temp371.base= _temp370;
_temp371.last_plus_one= _temp370 + 6; _temp371;}); goto _LL200; _LL220:
unimp_msg_part=( struct _tagged_string)({ char* _temp372=( char*)"typedef";
struct _tagged_string _temp373; _temp373.curr= _temp372; _temp373.base= _temp372;
_temp373.last_plus_one= _temp372 + 8; _temp373;}); goto _LL200; _LL222:
unimp_msg_part=( struct _tagged_string)({ char* _temp374=( char*)"extern \"C\"";
struct _tagged_string _temp375; _temp375.curr= _temp374; _temp375.base= _temp374;
_temp375.last_plus_one= _temp374 + 11; _temp375;}); goto _LL200; _LL200:;}( void)
_throw((( struct _xenum_struct*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(({
struct _tagged_string _temp376= unimp_msg_part; xprintf("tcStmt: nested %.*s declarations unimplemented",
_temp376.last_plus_one - _temp376.curr, _temp376.curr);})));} _LL39:( void)
_throw((( struct _xenum_struct*(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp377=( char*)"tcStmt: cut is not implemented";
struct _tagged_string _temp378; _temp378.curr= _temp377; _temp378.base= _temp377;
_temp378.last_plus_one= _temp377 + 31; _temp378;}))); _LL41:( void) _throw(((
struct _xenum_struct*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp379=( char*)"tcStmt: splice is not implemented";
struct _tagged_string _temp380; _temp380.curr= _temp379; _temp380.base= _temp379;
_temp380.last_plus_one= _temp379 + 34; _temp380;}))); _LL5:;}}