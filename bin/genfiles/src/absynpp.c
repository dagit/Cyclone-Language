#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2;}; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3;}; struct _tuple2{ struct
Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2;}; struct _tuple3{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual* f2; void* f3;}; struct _tuple4{
struct Cyc_Absyn_Tqual* f1; void* f2; struct Cyc_List_List* f3;}; struct _tuple5{
struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2;}; struct _tuple6{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2;}; struct _tuple7{ struct Cyc_Absyn_Tqual* f1; void*
f2;}; struct _tuple8{ struct Cyc_List_List* f1; struct Cyc_List_List* f2;};
struct _tuple9{ void* f1; void* f2;}; struct _tuple10{ struct Cyc_Core_Opt* f1;
struct Cyc_Absyn_Exp* f2;}; typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring;
typedef struct _tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t;
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void*
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v;}; typedef struct
Cyc_Core_Opt* Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string(
int); extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1;}; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1;}; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1;}; extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{
char* tag;}; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1;}; extern struct _tagged_string Cyc_Core_string_of_int(
int); extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;};
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag;}; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;}; typedef
struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t;
typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag;}; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map_c( void*(* f)( void*, void*), void* env, struct Cyc_List_List* x);
extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag;}; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag;}; struct Cyc_PP_Ppstate; typedef
struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out*
Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern
void Cyc_PP_file_of_doc( struct Cyc_PP_Doc* d, int w, struct Cyc_Stdio___sFILE*
f); extern struct _tagged_string Cyc_PP_string_of_doc( struct Cyc_PP_Doc* d, int
w); extern struct Cyc_PP_Doc* Cyc_PP_text( struct _tagged_string s); extern
struct Cyc_PP_Doc* Cyc_PP_nil_doc(); extern struct Cyc_PP_Doc* Cyc_PP_line_doc();
extern struct Cyc_PP_Doc* Cyc_PP_nest( int k, struct Cyc_PP_Doc* d); extern
struct Cyc_PP_Doc* Cyc_PP_concat( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc* d2);
extern struct Cyc_PP_Doc* Cyc_PP_ppseq( struct Cyc_PP_Doc*(* pp)( void*), struct
_tagged_string sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc* Cyc_PP_seq(
struct _tagged_string sep, struct Cyc_List_List* l0); extern struct Cyc_PP_Doc*
Cyc_PP_ppseql( struct Cyc_PP_Doc*(* pp)( void*), struct _tagged_string sep,
struct Cyc_List_List* l0); extern struct Cyc_PP_Doc* Cyc_PP_seql( struct
_tagged_string sep, struct Cyc_List_List* l0); struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col;};
typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
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
struct Cyc_Absyn_EmptyAnnot_struct{ char* tag;}; extern struct Cyc_Absyn_Tqual*
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern int Cyc_Absyn_is_format_prim( void* p);
struct Cyc_Buffer_t; typedef struct Cyc_Buffer_t* Cyc_Buffer_T; extern struct
_tagged_string Cyc_String_str_sepstr( struct Cyc_List_List*, struct
_tagged_string); extern int Cyc_Absynpp_expand_typedefs; extern int Cyc_Absynpp_qvar_to_Cids;
extern void Cyc_Absynpp_decllist2file( struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE*
f); extern struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc( struct Cyc_Absyn_Decl* d);
extern struct _tagged_string Cyc_Absynpp_typ2string( void*); extern struct
_tagged_string Cyc_Absynpp_kind2string( void*); extern struct _tagged_string Cyc_Absynpp_ckind2string(
struct Cyc_Absyn_Conref*); extern struct _tagged_string Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*); extern struct _tagged_string Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*); extern struct _tagged_string Cyc_Absynpp_qvar2string(
struct _tuple0*); extern struct _tagged_string Cyc_Absynpp_decllist2string(
struct Cyc_List_List* tdl); extern struct _tagged_string Cyc_Absynpp_prim2string(
void* p); extern struct _tagged_string Cyc_Absynpp_pat2string( struct Cyc_Absyn_Pat*
p); extern int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*); extern struct
_tagged_string Cyc_Absynpp_char_escape( char); extern struct _tagged_string Cyc_Absynpp_string_escape(
struct _tagged_string); extern struct _tagged_string Cyc_Absynpp_prim2str( void*
p); extern int Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s); extern
struct _tuple1* Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg); extern struct
_tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual* tq, void* t); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); struct Cyc_Set_Set;
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
Cyc_Tcenv_tenv; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern char Cyc_Tcutil_TypeErr_tag[
8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag;}; extern void* Cyc_Tcutil_compress(
void* t); extern struct Cyc_PP_Doc* Cyc_Absynpp_egroup( struct _tagged_string
start, struct _tagged_string stop, struct _tagged_string sep, struct Cyc_List_List*
ss); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat12( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat11(
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_cat10( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_cat9( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat8(
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_cat7( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat6( struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat5( struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_cat4( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*,
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cat3(
struct Cyc_PP_Doc*, struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_cat2( struct Cyc_PP_Doc*, struct Cyc_PP_Doc*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_dp2doc( struct _tuple5* dp); extern struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs); extern struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc( void*);
extern struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc( struct Cyc_List_List*
fields); extern struct Cyc_PP_Doc* Cyc_Absynpp_scope2doc( void*); extern struct
Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct Cyc_Absyn_Stmt*); extern struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp*); extern struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec(
int inprec, struct Cyc_Absyn_Exp* e); extern struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es); extern struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0*); extern struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc( void*); extern
struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void*); extern struct Cyc_PP_Doc* Cyc_Absynpp_primapp2doc(
int inprec, void* p, struct Cyc_List_List* es); extern struct Cyc_PP_Doc* Cyc_Absynpp_de2doc(
struct _tuple6* de); extern struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc( struct Cyc_Absyn_Tqual*
tq, void* t, struct Cyc_Core_Opt* dopt); extern struct Cyc_PP_Doc* Cyc_Absynpp_funargs2doc(
struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt* effopt); extern
struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc( struct Cyc_List_List* fields);
int Cyc_Absynpp_expand_typedefs= 0; int Cyc_Absynpp_print_all_kinds= 0; int Cyc_Absynpp_qvar_to_Cids=
0; struct _tagged_string Cyc_Absynpp_char_escape( char c){ switch( c){ case '\a':
_LL0: return( struct _tagged_string)({ char* _temp2=( char*)"\\a"; struct
_tagged_string _temp3; _temp3.curr= _temp2; _temp3.base= _temp2; _temp3.last_plus_one=
_temp2 + 3; _temp3;}); case '\b': _LL1: return( struct _tagged_string)({ char*
_temp5=( char*)"\\b"; struct _tagged_string _temp6; _temp6.curr= _temp5; _temp6.base=
_temp5; _temp6.last_plus_one= _temp5 + 3; _temp6;}); case '\f': _LL4: return(
struct _tagged_string)({ char* _temp8=( char*)"\\f"; struct _tagged_string
_temp9; _temp9.curr= _temp8; _temp9.base= _temp8; _temp9.last_plus_one= _temp8 +
3; _temp9;}); case '\n': _LL7: return( struct _tagged_string)({ char* _temp11=(
char*)"\\n"; struct _tagged_string _temp12; _temp12.curr= _temp11; _temp12.base=
_temp11; _temp12.last_plus_one= _temp11 + 3; _temp12;}); case '\r': _LL10:
return( struct _tagged_string)({ char* _temp14=( char*)"\\r"; struct
_tagged_string _temp15; _temp15.curr= _temp14; _temp15.base= _temp14; _temp15.last_plus_one=
_temp14 + 3; _temp15;}); case '\t': _LL13: return( struct _tagged_string)({ char*
_temp17=( char*)"\\t"; struct _tagged_string _temp18; _temp18.curr= _temp17;
_temp18.base= _temp17; _temp18.last_plus_one= _temp17 + 3; _temp18;}); case '\v':
_LL16: return( struct _tagged_string)({ char* _temp20=( char*)"\\v"; struct
_tagged_string _temp21; _temp21.curr= _temp20; _temp21.base= _temp20; _temp21.last_plus_one=
_temp20 + 3; _temp21;}); case '\\': _LL19: return( struct _tagged_string)({ char*
_temp23=( char*)"\\\\"; struct _tagged_string _temp24; _temp24.curr= _temp23;
_temp24.base= _temp23; _temp24.last_plus_one= _temp23 + 3; _temp24;}); case '"':
_LL22: return( struct _tagged_string)({ char* _temp26=( char*)"\""; struct
_tagged_string _temp27; _temp27.curr= _temp26; _temp27.base= _temp26; _temp27.last_plus_one=
_temp26 + 2; _temp27;}); case '\'': _LL25: return( struct _tagged_string)({ char*
_temp29=( char*)"\\'"; struct _tagged_string _temp30; _temp30.curr= _temp29;
_temp30.base= _temp29; _temp30.last_plus_one= _temp29 + 3; _temp30;}); default:
_LL28: if(( int) c >=( int)' '?( int) c <=( int)'~': 0){ struct _tagged_string t=
Cyc_Core_new_string( 1);({ struct _tagged_string _temp32= t; char* _temp34=
_temp32.curr + 0; if( _temp34 < _temp32.base? 1: _temp34 >= _temp32.last_plus_one){
_throw( Null_Exception);}* _temp34= c;}); return t;} else{ struct _tagged_string
t= Cyc_Core_new_string( 4); int j= 0;({ struct _tagged_string _temp35= t; char*
_temp37= _temp35.curr +( j ++); if( _temp37 < _temp35.base? 1: _temp37 >=
_temp35.last_plus_one){ _throw( Null_Exception);}* _temp37='\\';});({ struct
_tagged_string _temp38= t; char* _temp40= _temp38.curr +( j ++); if( _temp40 <
_temp38.base? 1: _temp40 >= _temp38.last_plus_one){ _throw( Null_Exception);}*
_temp40=( char)(( int)'0' +(( int) c >> 6 & 7));});({ struct _tagged_string
_temp41= t; char* _temp43= _temp41.curr +( j ++); if( _temp43 < _temp41.base? 1:
_temp43 >= _temp41.last_plus_one){ _throw( Null_Exception);}* _temp43=( char)((
int)'0' +(( int) c >> 3 & 7));});({ struct _tagged_string _temp44= t; char*
_temp46= _temp44.curr +( j ++); if( _temp46 < _temp44.base? 1: _temp46 >=
_temp44.last_plus_one){ _throw( Null_Exception);}* _temp46=( char)(( int)'0' +((
int) c & 7));}); return t;}}} static int Cyc_Absynpp_special( struct
_tagged_string s){ int sz=( int)(({ struct _tagged_string _temp47= s;(
unsigned int)( _temp47.last_plus_one - _temp47.curr);}) -( unsigned int) 1);{
int i= 0; for( 0; i < sz; i ++){ char c=({ struct _tagged_string _temp48= s;
char* _temp50= _temp48.curr + i; if( _temp50 < _temp48.base? 1: _temp50 >=
_temp48.last_plus_one){ _throw( Null_Exception);}* _temp50;}); if(((( int) c <=(
int)' '? 1:( int) c >=( int)'~')? 1:( int) c ==( int)'"')? 1:( int) c ==( int)'\\'){
return 1;}}} return 0;} struct _tagged_string Cyc_Absynpp_string_escape( struct
_tagged_string s){ if( ! Cyc_Absynpp_special( s)){ return s;}{ int n=( int)(({
struct _tagged_string _temp51= s;( unsigned int)( _temp51.last_plus_one -
_temp51.curr);}) -( unsigned int) 1); if( n > 0?( int)({ struct _tagged_string
_temp52= s; char* _temp54= _temp52.curr + n; if( _temp54 < _temp52.base? 1:
_temp54 >= _temp52.last_plus_one){ _throw( Null_Exception);}* _temp54;}) ==( int)'\000':
0){ n --;}{ int len= 0;{ int i= 0; for( 0; i <= n; i ++){ char _temp58=({ struct
_tagged_string _temp55= s; char* _temp57= _temp55.curr + i; if( _temp57 <
_temp55.base? 1: _temp57 >= _temp55.last_plus_one){ _throw( Null_Exception);}*
_temp57;}); _LL60: if( _temp58 =='\a'){ goto _LL61;} else{ goto _LL62;} _LL62:
if( _temp58 =='\b'){ goto _LL63;} else{ goto _LL64;} _LL64: if( _temp58 =='\f'){
goto _LL65;} else{ goto _LL66;} _LL66: if( _temp58 =='\n'){ goto _LL67;} else{
goto _LL68;} _LL68: if( _temp58 =='\r'){ goto _LL69;} else{ goto _LL70;} _LL70:
if( _temp58 =='\t'){ goto _LL71;} else{ goto _LL72;} _LL72: if( _temp58 =='\v'){
goto _LL73;} else{ goto _LL74;} _LL74: if( _temp58 =='\\'){ goto _LL75;} else{
goto _LL76;} _LL76: if( _temp58 =='"'){ goto _LL77;} else{ goto _LL78;} _LL78:
goto _LL79; _LL61: goto _LL63; _LL63: goto _LL65; _LL65: goto _LL67; _LL67: goto
_LL69; _LL69: goto _LL71; _LL71: goto _LL73; _LL73: goto _LL75; _LL75: goto
_LL77; _LL77: len += 2; goto _LL59; _LL79: if(( int) _temp58 >=( int)' '?( int)
_temp58 <=( int)'~': 0){ len ++;} else{ len += 4;} goto _LL59; _LL59:;}}{ struct
_tagged_string t= Cyc_Core_new_string( len); int j= 0;{ int i= 0; for( 0; i <= n;
i ++){ char _temp83=({ struct _tagged_string _temp80= s; char* _temp82= _temp80.curr
+ i; if( _temp82 < _temp80.base? 1: _temp82 >= _temp80.last_plus_one){ _throw(
Null_Exception);}* _temp82;}); _LL85: if( _temp83 =='\a'){ goto _LL86;} else{
goto _LL87;} _LL87: if( _temp83 =='\b'){ goto _LL88;} else{ goto _LL89;} _LL89:
if( _temp83 =='\f'){ goto _LL90;} else{ goto _LL91;} _LL91: if( _temp83 =='\n'){
goto _LL92;} else{ goto _LL93;} _LL93: if( _temp83 =='\r'){ goto _LL94;} else{
goto _LL95;} _LL95: if( _temp83 =='\t'){ goto _LL96;} else{ goto _LL97;} _LL97:
if( _temp83 =='\v'){ goto _LL98;} else{ goto _LL99;} _LL99: if( _temp83 =='\\'){
goto _LL100;} else{ goto _LL101;} _LL101: if( _temp83 =='"'){ goto _LL102;}
else{ goto _LL103;} _LL103: goto _LL104; _LL86:({ struct _tagged_string _temp105=
t; char* _temp107= _temp105.curr +( j ++); if( _temp107 < _temp105.base? 1:
_temp107 >= _temp105.last_plus_one){ _throw( Null_Exception);}* _temp107='\\';});({
struct _tagged_string _temp108= t; char* _temp110= _temp108.curr +( j ++); if(
_temp110 < _temp108.base? 1: _temp110 >= _temp108.last_plus_one){ _throw(
Null_Exception);}* _temp110='a';}); goto _LL84; _LL88:({ struct _tagged_string
_temp111= t; char* _temp113= _temp111.curr +( j ++); if( _temp113 < _temp111.base?
1: _temp113 >= _temp111.last_plus_one){ _throw( Null_Exception);}* _temp113='\\';});({
struct _tagged_string _temp114= t; char* _temp116= _temp114.curr +( j ++); if(
_temp116 < _temp114.base? 1: _temp116 >= _temp114.last_plus_one){ _throw(
Null_Exception);}* _temp116='b';}); goto _LL84; _LL90:({ struct _tagged_string
_temp117= t; char* _temp119= _temp117.curr +( j ++); if( _temp119 < _temp117.base?
1: _temp119 >= _temp117.last_plus_one){ _throw( Null_Exception);}* _temp119='\\';});({
struct _tagged_string _temp120= t; char* _temp122= _temp120.curr +( j ++); if(
_temp122 < _temp120.base? 1: _temp122 >= _temp120.last_plus_one){ _throw(
Null_Exception);}* _temp122='f';}); goto _LL84; _LL92:({ struct _tagged_string
_temp123= t; char* _temp125= _temp123.curr +( j ++); if( _temp125 < _temp123.base?
1: _temp125 >= _temp123.last_plus_one){ _throw( Null_Exception);}* _temp125='\\';});({
struct _tagged_string _temp126= t; char* _temp128= _temp126.curr +( j ++); if(
_temp128 < _temp126.base? 1: _temp128 >= _temp126.last_plus_one){ _throw(
Null_Exception);}* _temp128='n';}); goto _LL84; _LL94:({ struct _tagged_string
_temp129= t; char* _temp131= _temp129.curr +( j ++); if( _temp131 < _temp129.base?
1: _temp131 >= _temp129.last_plus_one){ _throw( Null_Exception);}* _temp131='\\';});({
struct _tagged_string _temp132= t; char* _temp134= _temp132.curr +( j ++); if(
_temp134 < _temp132.base? 1: _temp134 >= _temp132.last_plus_one){ _throw(
Null_Exception);}* _temp134='r';}); goto _LL84; _LL96:({ struct _tagged_string
_temp135= t; char* _temp137= _temp135.curr +( j ++); if( _temp137 < _temp135.base?
1: _temp137 >= _temp135.last_plus_one){ _throw( Null_Exception);}* _temp137='\\';});({
struct _tagged_string _temp138= t; char* _temp140= _temp138.curr +( j ++); if(
_temp140 < _temp138.base? 1: _temp140 >= _temp138.last_plus_one){ _throw(
Null_Exception);}* _temp140='t';}); goto _LL84; _LL98:({ struct _tagged_string
_temp141= t; char* _temp143= _temp141.curr +( j ++); if( _temp143 < _temp141.base?
1: _temp143 >= _temp141.last_plus_one){ _throw( Null_Exception);}* _temp143='\\';});({
struct _tagged_string _temp144= t; char* _temp146= _temp144.curr +( j ++); if(
_temp146 < _temp144.base? 1: _temp146 >= _temp144.last_plus_one){ _throw(
Null_Exception);}* _temp146='v';}); goto _LL84; _LL100:({ struct _tagged_string
_temp147= t; char* _temp149= _temp147.curr +( j ++); if( _temp149 < _temp147.base?
1: _temp149 >= _temp147.last_plus_one){ _throw( Null_Exception);}* _temp149='\\';});({
struct _tagged_string _temp150= t; char* _temp152= _temp150.curr +( j ++); if(
_temp152 < _temp150.base? 1: _temp152 >= _temp150.last_plus_one){ _throw(
Null_Exception);}* _temp152='\\';}); goto _LL84; _LL102:({ struct _tagged_string
_temp153= t; char* _temp155= _temp153.curr +( j ++); if( _temp155 < _temp153.base?
1: _temp155 >= _temp153.last_plus_one){ _throw( Null_Exception);}* _temp155='\\';});({
struct _tagged_string _temp156= t; char* _temp158= _temp156.curr +( j ++); if(
_temp158 < _temp156.base? 1: _temp158 >= _temp156.last_plus_one){ _throw(
Null_Exception);}* _temp158='"';}); goto _LL84; _LL104: if(( int) _temp83 >=(
int)' '?( int) _temp83 <=( int)'~': 0){({ struct _tagged_string _temp159= t;
char* _temp161= _temp159.curr +( j ++); if( _temp161 < _temp159.base? 1:
_temp161 >= _temp159.last_plus_one){ _throw( Null_Exception);}* _temp161=
_temp83;});} else{({ struct _tagged_string _temp162= t; char* _temp164= _temp162.curr
+( j ++); if( _temp164 < _temp162.base? 1: _temp164 >= _temp162.last_plus_one){
_throw( Null_Exception);}* _temp164='\\';});({ struct _tagged_string _temp165= t;
char* _temp167= _temp165.curr +( j ++); if( _temp167 < _temp165.base? 1:
_temp167 >= _temp165.last_plus_one){ _throw( Null_Exception);}* _temp167=( char)((
int)'0' +(( int) _temp83 >> 6 & 7));});({ struct _tagged_string _temp168= t;
char* _temp170= _temp168.curr +( j ++); if( _temp170 < _temp168.base? 1:
_temp170 >= _temp168.last_plus_one){ _throw( Null_Exception);}* _temp170=( char)((
int)'0' +(( int) _temp83 >> 3 & 7));});({ struct _tagged_string _temp171= t;
char* _temp173= _temp171.curr +( j ++); if( _temp173 < _temp171.base? 1:
_temp173 >= _temp171.last_plus_one){ _throw( Null_Exception);}* _temp173=( char)((
int)'0' +(( int) _temp83 & 7));});} goto _LL84; _LL84:;}} return t;}}}} static
struct Cyc_PP_Doc* Cyc_Absynpp_textptr( struct _tagged_string* s){ return Cyc_PP_text(*
s);} static char _temp176[ 9u]="restrict"; static struct _tagged_string Cyc_Absynpp_restrict_string=(
struct _tagged_string){ _temp176, _temp176, _temp176 + 9u}; static char _temp179[
9u]="volatile"; static struct _tagged_string Cyc_Absynpp_volatile_string=(
struct _tagged_string){ _temp179, _temp179, _temp179 + 9u}; static char _temp182[
6u]="const"; static struct _tagged_string Cyc_Absynpp_const_string=( struct
_tagged_string){ _temp182, _temp182, _temp182 + 6u}; static struct
_tagged_string* Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string; static
struct _tagged_string* Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _tagged_string* Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_string;
struct Cyc_PP_Doc* Cyc_Absynpp_tqual2doc( struct Cyc_Absyn_Tqual* tq){ struct
Cyc_List_List* l= 0; if( tq->q_restrict){ l=({ struct Cyc_List_List* _temp183=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp183->hd=(
void*) Cyc_Absynpp_restrict_sp; _temp183->tl= l; _temp183;});} if( tq->q_volatile){
l=({ struct Cyc_List_List* _temp184=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp184->hd=( void*) Cyc_Absynpp_volatile_sp; _temp184->tl=
l; _temp184;});} if( tq->q_const){ l=({ struct Cyc_List_List* _temp185=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp185->hd=( void*)
Cyc_Absynpp_const_sp; _temp185->tl= l; _temp185;});} return Cyc_Absynpp_egroup((
struct _tagged_string)({ char* _temp186=( char*)""; struct _tagged_string
_temp187; _temp187.curr= _temp186; _temp187.base= _temp186; _temp187.last_plus_one=
_temp186 + 1; _temp187;}),( struct _tagged_string)({ char* _temp188=( char*)" ";
struct _tagged_string _temp189; _temp189.curr= _temp188; _temp189.base= _temp188;
_temp189.last_plus_one= _temp188 + 2; _temp189;}),( struct _tagged_string)({
char* _temp190=( char*)" "; struct _tagged_string _temp191; _temp191.curr=
_temp190; _temp191.base= _temp190; _temp191.last_plus_one= _temp190 + 2;
_temp191;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_textptr,
l));} struct _tagged_string Cyc_Absynpp_kind2string( void* k){ void* _temp192= k;
_LL194: if( _temp192 == Cyc_Absyn_AnyKind){ goto _LL195;} else{ goto _LL196;}
_LL196: if( _temp192 == Cyc_Absyn_MemKind){ goto _LL197;} else{ goto _LL198;}
_LL198: if( _temp192 == Cyc_Absyn_BoxKind){ goto _LL199;} else{ goto _LL200;}
_LL200: if( _temp192 == Cyc_Absyn_RgnKind){ goto _LL201;} else{ goto _LL202;}
_LL202: if( _temp192 == Cyc_Absyn_EffKind){ goto _LL203;} else{ goto _LL193;}
_LL195: return( struct _tagged_string)({ char* _temp204=( char*)"A"; struct
_tagged_string _temp205; _temp205.curr= _temp204; _temp205.base= _temp204;
_temp205.last_plus_one= _temp204 + 2; _temp205;}); _LL197: return( struct
_tagged_string)({ char* _temp206=( char*)"M"; struct _tagged_string _temp207;
_temp207.curr= _temp206; _temp207.base= _temp206; _temp207.last_plus_one=
_temp206 + 2; _temp207;}); _LL199: return( struct _tagged_string)({ char*
_temp208=( char*)"B"; struct _tagged_string _temp209; _temp209.curr= _temp208;
_temp209.base= _temp208; _temp209.last_plus_one= _temp208 + 2; _temp209;});
_LL201: return( struct _tagged_string)({ char* _temp210=( char*)"R"; struct
_tagged_string _temp211; _temp211.curr= _temp210; _temp211.base= _temp210;
_temp211.last_plus_one= _temp210 + 2; _temp211;}); _LL203: return( struct
_tagged_string)({ char* _temp212=( char*)"E"; struct _tagged_string _temp213;
_temp213.curr= _temp212; _temp213.base= _temp212; _temp213.last_plus_one=
_temp212 + 2; _temp213;}); _LL193:;} struct _tagged_string Cyc_Absynpp_ckind2string(
struct Cyc_Absyn_Conref* c){ void* _temp214=( void*)((( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)( c))->v; void* _temp220;
_LL216: if(( unsigned int) _temp214 > 1u?(( struct _enum_struct*) _temp214)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL221: _temp220=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp214)->f1; goto _LL217;} else{ goto _LL218;} _LL218: goto _LL219; _LL217:
return Cyc_Absynpp_kind2string( _temp220); _LL219: return( struct _tagged_string)({
char* _temp222=( char*)"?"; struct _tagged_string _temp223; _temp223.curr=
_temp222; _temp223.base= _temp222; _temp223.last_plus_one= _temp222 + 2;
_temp223;}); _LL215:;} struct Cyc_PP_Doc* Cyc_Absynpp_kind2doc( void* k){ return
Cyc_PP_text( Cyc_Absynpp_kind2string( k));} struct Cyc_PP_Doc* Cyc_Absynpp_ckind2doc(
struct Cyc_Absyn_Conref* c){ return Cyc_PP_text( Cyc_Absynpp_ckind2string( c));}
struct Cyc_PP_Doc* Cyc_Absynpp_group( struct _tagged_string start, struct
_tagged_string stop, struct _tagged_string sep, struct Cyc_List_List* ss){
return Cyc_Absynpp_cat3( Cyc_PP_text( start), Cyc_PP_seq( sep, ss), Cyc_PP_text(
stop));} struct Cyc_PP_Doc* Cyc_Absynpp_egroup( struct _tagged_string start,
struct _tagged_string stop, struct _tagged_string sep, struct Cyc_List_List* ss){
if( ss == 0){ return Cyc_PP_nil_doc();} else{ return Cyc_Absynpp_cat3( Cyc_PP_text(
start), Cyc_PP_seq( sep, ss), Cyc_PP_text( stop));}} struct Cyc_PP_Doc* Cyc_Absynpp_groupl(
struct _tagged_string start, struct _tagged_string stop, struct _tagged_string
sep, struct Cyc_List_List* ss){ return Cyc_Absynpp_cat3( Cyc_PP_text( start),
Cyc_PP_seql( sep, ss), Cyc_PP_text( stop));} struct Cyc_PP_Doc* Cyc_Absynpp_tps2doc(
struct Cyc_List_List* ts){ return Cyc_Absynpp_egroup(( struct _tagged_string)({
char* _temp224=( char*)"<"; struct _tagged_string _temp225; _temp225.curr=
_temp224; _temp225.base= _temp224; _temp225.last_plus_one= _temp224 + 2;
_temp225;}),( struct _tagged_string)({ char* _temp226=( char*)">"; struct
_tagged_string _temp227; _temp227.curr= _temp226; _temp227.base= _temp226;
_temp227.last_plus_one= _temp226 + 2; _temp227;}),( struct _tagged_string)({
char* _temp228=( char*)","; struct _tagged_string _temp229; _temp229.curr=
_temp228; _temp229.base= _temp228; _temp229.last_plus_one= _temp228 + 2;
_temp229;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( void*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_typ2doc, ts));} struct Cyc_PP_Doc*
Cyc_Absynpp_ktvar2doc( struct Cyc_Absyn_Tvar* tv){ void* _temp230=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
tv->kind))->v; void* _temp238; void* _temp240; _LL232: if(( unsigned int)
_temp230 > 1u?(( struct _enum_struct*) _temp230)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL239: _temp238=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp230)->f1;
if( _temp238 == Cyc_Absyn_BoxKind){ goto _LL233;} else{ goto _LL234;}} else{
goto _LL234;} _LL234: if(( unsigned int) _temp230 > 1u?(( struct _enum_struct*)
_temp230)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL241: _temp240=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp230)->f1; goto _LL235;} else{ goto
_LL236;} _LL236: goto _LL237; _LL233: return Cyc_Absynpp_textptr( tv->name);
_LL235: return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr( tv->name), Cyc_PP_text((
struct _tagged_string)({ char* _temp242=( char*)"::"; struct _tagged_string
_temp243; _temp243.curr= _temp242; _temp243.base= _temp242; _temp243.last_plus_one=
_temp242 + 3; _temp243;})), Cyc_Absynpp_kind2doc( _temp240)); _LL237: return Cyc_Absynpp_cat2(
Cyc_Absynpp_textptr( tv->name), Cyc_PP_text(( struct _tagged_string)({ char*
_temp244=( char*)"::?"; struct _tagged_string _temp245; _temp245.curr= _temp244;
_temp245.base= _temp244; _temp245.last_plus_one= _temp244 + 4; _temp245;})));
_LL231:;} struct Cyc_PP_Doc* Cyc_Absynpp_kinded_tvars2doc( struct Cyc_List_List*
tvs){ return Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp246=( char*)"<";
struct _tagged_string _temp247; _temp247.curr= _temp246; _temp247.base= _temp246;
_temp247.last_plus_one= _temp246 + 2; _temp247;}),( struct _tagged_string)({
char* _temp248=( char*)">"; struct _tagged_string _temp249; _temp249.curr=
_temp248; _temp249.base= _temp248; _temp249.last_plus_one= _temp248 + 2;
_temp249;}),( struct _tagged_string)({ char* _temp250=( char*)","; struct
_tagged_string _temp251; _temp251.curr= _temp250; _temp251.base= _temp250;
_temp251.last_plus_one= _temp250 + 2; _temp251;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_ktvar2doc, tvs));} static struct _tagged_string* Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar* tv){ return tv->name;} struct Cyc_PP_Doc* Cyc_Absynpp_tvars2doc(
struct Cyc_List_List* tvs){ if( Cyc_Absynpp_print_all_kinds){ return Cyc_Absynpp_kinded_tvars2doc(
tvs);} return Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp252=(
char*)"<"; struct _tagged_string _temp253; _temp253.curr= _temp252; _temp253.base=
_temp252; _temp253.last_plus_one= _temp252 + 2; _temp253;}),( struct
_tagged_string)({ char* _temp254=( char*)">"; struct _tagged_string _temp255;
_temp255.curr= _temp254; _temp255.base= _temp254; _temp255.last_plus_one=
_temp254 + 2; _temp255;}),( struct _tagged_string)({ char* _temp256=( char*)",";
struct _tagged_string _temp257; _temp257.curr= _temp256; _temp257.base= _temp256;
_temp257.last_plus_one= _temp256 + 2; _temp257;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_textptr,(( struct Cyc_List_List*(*)( struct _tagged_string*(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_get_name,
tvs)));} struct Cyc_PP_Doc* Cyc_Absynpp_arg2doc( struct _tuple7* t){ return Cyc_Absynpp_tqtd2doc((*
t).f1,(* t).f2, 0);} struct Cyc_PP_Doc* Cyc_Absynpp_args2doc( struct Cyc_List_List*
ts){ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp258=( char*)"(";
struct _tagged_string _temp259; _temp259.curr= _temp258; _temp259.base= _temp258;
_temp259.last_plus_one= _temp258 + 2; _temp259;}),( struct _tagged_string)({
char* _temp260=( char*)")"; struct _tagged_string _temp261; _temp261.curr=
_temp260; _temp261.base= _temp260; _temp261.last_plus_one= _temp260 + 2;
_temp261;}),( struct _tagged_string)({ char* _temp262=( char*)","; struct
_tagged_string _temp263; _temp263.curr= _temp262; _temp263.base= _temp262;
_temp263.last_plus_one= _temp262 + 2; _temp263;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_arg2doc, ts));} int Cyc_Absynpp_next_is_pointer( struct Cyc_List_List*
tms){ if( tms == 0){ return 0;}{ void* _temp264=( void*) tms->hd; struct Cyc_Absyn_Tqual*
_temp270; void* _temp272; void* _temp274; _LL266: if(( unsigned int) _temp264 >
1u?(( struct _enum_struct*) _temp264)->tag == Cyc_Absyn_Pointer_mod_tag: 0){
_LL275: _temp274=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp264)->f1;
goto _LL273; _LL273: _temp272=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp264)->f2; goto _LL271; _LL271: _temp270=( struct Cyc_Absyn_Tqual*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp264)->f3; goto _LL267;} else{ goto _LL268;}
_LL268: goto _LL269; _LL267: return 1; _LL269: return 0; _LL265:;}} extern
struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t); struct Cyc_PP_Doc* Cyc_Absynpp_dtms2doc(
struct Cyc_PP_Doc* d, struct Cyc_List_List* tms){ if( tms == 0){ return d;}{
struct Cyc_PP_Doc* rest= Cyc_Absynpp_dtms2doc( d, tms->tl); struct Cyc_PP_Doc*
p_rest= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp276=(
char*)"("; struct _tagged_string _temp277; _temp277.curr= _temp276; _temp277.base=
_temp276; _temp277.last_plus_one= _temp276 + 2; _temp277;})), rest, Cyc_PP_text((
struct _tagged_string)({ char* _temp278=( char*)")"; struct _tagged_string
_temp279; _temp279.curr= _temp278; _temp279.base= _temp278; _temp279.last_plus_one=
_temp278 + 2; _temp279;}))); void* _temp280=( void*) tms->hd; struct Cyc_Absyn_Exp*
_temp292; void* _temp294; int _temp296; struct Cyc_Position_Segment* _temp298;
struct Cyc_List_List* _temp300; struct Cyc_Absyn_Tqual* _temp302; void* _temp304;
void* _temp306; _LL282: if( _temp280 == Cyc_Absyn_Carray_mod){ goto _LL283;}
else{ goto _LL284;} _LL284: if(( unsigned int) _temp280 > 1u?(( struct
_enum_struct*) _temp280)->tag == Cyc_Absyn_ConstArray_mod_tag: 0){ _LL293:
_temp292=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp280)->f1; goto _LL285;} else{ goto _LL286;} _LL286: if(( unsigned int)
_temp280 > 1u?(( struct _enum_struct*) _temp280)->tag == Cyc_Absyn_Function_mod_tag:
0){ _LL295: _temp294=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp280)->f1;
goto _LL287;} else{ goto _LL288;} _LL288: if(( unsigned int) _temp280 > 1u?((
struct _enum_struct*) _temp280)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){ _LL301:
_temp300=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp280)->f1; goto _LL299; _LL299: _temp298=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp280)->f2; goto _LL297; _LL297:
_temp296=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp280)->f3; goto
_LL289;} else{ goto _LL290;} _LL290: if(( unsigned int) _temp280 > 1u?(( struct
_enum_struct*) _temp280)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL307: _temp306=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp280)->f1; goto _LL305;
_LL305: _temp304=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp280)->f2;
goto _LL303; _LL303: _temp302=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp280)->f3; goto _LL291;} else{ goto _LL281;} _LL283: if( Cyc_Absynpp_next_is_pointer(
tms->tl)){ rest= p_rest;} return Cyc_Absynpp_cat2( rest, Cyc_PP_text(( struct
_tagged_string)({ char* _temp308=( char*)"[]"; struct _tagged_string _temp309;
_temp309.curr= _temp308; _temp309.base= _temp308; _temp309.last_plus_one=
_temp308 + 3; _temp309;}))); _LL285: if( Cyc_Absynpp_next_is_pointer( tms->tl)){
rest= p_rest;} return Cyc_Absynpp_cat4( rest, Cyc_PP_text(( struct
_tagged_string)({ char* _temp310=( char*)"["; struct _tagged_string _temp311;
_temp311.curr= _temp310; _temp311.base= _temp310; _temp311.last_plus_one=
_temp310 + 2; _temp311;})), Cyc_Absynpp_exp2doc( _temp292), Cyc_PP_text(( struct
_tagged_string)({ char* _temp312=( char*)"]"; struct _tagged_string _temp313;
_temp313.curr= _temp312; _temp313.base= _temp312; _temp313.last_plus_one=
_temp312 + 2; _temp313;}))); _LL287: if( Cyc_Absynpp_next_is_pointer( tms->tl)){
rest= p_rest;}{ void* _temp314= _temp294; struct Cyc_Core_Opt* _temp320; int
_temp322; struct Cyc_List_List* _temp324; struct Cyc_Position_Segment* _temp326;
struct Cyc_List_List* _temp328; _LL316: if((( struct _enum_struct*) _temp314)->tag
== Cyc_Absyn_WithTypes_tag){ _LL325: _temp324=( struct Cyc_List_List*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp314)->f1; goto _LL323; _LL323: _temp322=( int)((
struct Cyc_Absyn_WithTypes_struct*) _temp314)->f2; goto _LL321; _LL321: _temp320=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_WithTypes_struct*) _temp314)->f3; goto
_LL317;} else{ goto _LL318;} _LL318: if((( struct _enum_struct*) _temp314)->tag
== Cyc_Absyn_NoTypes_tag){ _LL329: _temp328=( struct Cyc_List_List*)(( struct
Cyc_Absyn_NoTypes_struct*) _temp314)->f1; goto _LL327; _LL327: _temp326=( struct
Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*) _temp314)->f2; goto
_LL319;} else{ goto _LL315;} _LL317: return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_funargs2doc(
_temp324, _temp322, _temp320)); _LL319: return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp330=( char*)"("; struct _tagged_string
_temp331; _temp331.curr= _temp330; _temp331.base= _temp330; _temp331.last_plus_one=
_temp330 + 2; _temp331;}),( struct _tagged_string)({ char* _temp332=( char*)")";
struct _tagged_string _temp333; _temp333.curr= _temp332; _temp333.base= _temp332;
_temp333.last_plus_one= _temp332 + 2; _temp333;}),( struct _tagged_string)({
char* _temp334=( char*)","; struct _tagged_string _temp335; _temp335.curr=
_temp334; _temp335.base= _temp334; _temp335.last_plus_one= _temp334 + 2;
_temp335;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_textptr,
_temp328))); _LL315:;} _LL289: if( Cyc_Absynpp_next_is_pointer( tms->tl)){ rest=
p_rest;} if( _temp296){ return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_kinded_tvars2doc(
_temp300));} else{ return Cyc_Absynpp_cat2( rest, Cyc_Absynpp_tvars2doc(
_temp300));} _LL291: { struct Cyc_PP_Doc* ptr;{ void* _temp336= _temp306; struct
Cyc_Absyn_Exp* _temp344; struct Cyc_Absyn_Exp* _temp346; _LL338: if((
unsigned int) _temp336 > 1u?(( struct _enum_struct*) _temp336)->tag == Cyc_Absyn_Nullable_ps_tag:
0){ _LL345: _temp344=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp336)->f1; goto _LL339;} else{ goto _LL340;} _LL340: if(( unsigned int)
_temp336 > 1u?(( struct _enum_struct*) _temp336)->tag == Cyc_Absyn_NonNullable_ps_tag:
0){ _LL347: _temp346=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp336)->f1; goto _LL341;} else{ goto _LL342;} _LL342: if( _temp336 == Cyc_Absyn_TaggedArray_ps){
goto _LL343;} else{ goto _LL337;} _LL339: if( Cyc_Evexp_eval_const_uint_exp(
_temp344) ==( unsigned int) 1){ ptr= Cyc_PP_text(( struct _tagged_string)({ char*
_temp348=( char*)"*"; struct _tagged_string _temp349; _temp349.curr= _temp348;
_temp349.base= _temp348; _temp349.last_plus_one= _temp348 + 2; _temp349;}));}
else{ ptr= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp350=( char*)"*"; struct _tagged_string _temp351; _temp351.curr= _temp350;
_temp351.base= _temp350; _temp351.last_plus_one= _temp350 + 2; _temp351;})), Cyc_PP_text((
struct _tagged_string)({ char* _temp352=( char*)"{"; struct _tagged_string
_temp353; _temp353.curr= _temp352; _temp353.base= _temp352; _temp353.last_plus_one=
_temp352 + 2; _temp353;})), Cyc_Absynpp_exp2doc( _temp344), Cyc_PP_text(( struct
_tagged_string)({ char* _temp354=( char*)"}"; struct _tagged_string _temp355;
_temp355.curr= _temp354; _temp355.base= _temp354; _temp355.last_plus_one=
_temp354 + 2; _temp355;})));} goto _LL337; _LL341: if( Cyc_Evexp_eval_const_uint_exp(
_temp346) ==( unsigned int) 1){ ptr= Cyc_PP_text(( struct _tagged_string)({ char*
_temp356=( char*)"@"; struct _tagged_string _temp357; _temp357.curr= _temp356;
_temp357.base= _temp356; _temp357.last_plus_one= _temp356 + 2; _temp357;}));}
else{ ptr= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp358=( char*)"@"; struct _tagged_string _temp359; _temp359.curr= _temp358;
_temp359.base= _temp358; _temp359.last_plus_one= _temp358 + 2; _temp359;})), Cyc_PP_text((
struct _tagged_string)({ char* _temp360=( char*)"{"; struct _tagged_string
_temp361; _temp361.curr= _temp360; _temp361.base= _temp360; _temp361.last_plus_one=
_temp360 + 2; _temp361;})), Cyc_Absynpp_exp2doc( _temp346), Cyc_PP_text(( struct
_tagged_string)({ char* _temp362=( char*)"}"; struct _tagged_string _temp363;
_temp363.curr= _temp362; _temp363.base= _temp362; _temp363.last_plus_one=
_temp362 + 2; _temp363;})));} goto _LL337; _LL343: ptr= Cyc_PP_text(( struct
_tagged_string)({ char* _temp364=( char*)"?"; struct _tagged_string _temp365;
_temp365.curr= _temp364; _temp365.base= _temp364; _temp365.last_plus_one=
_temp364 + 2; _temp365;})); goto _LL337; _LL337:;}{ void* _temp366= Cyc_Tcutil_compress(
_temp304); _LL368: if( _temp366 == Cyc_Absyn_HeapRgn){ goto _LL369;} else{ goto
_LL370;} _LL370: goto _LL371; _LL369: return Cyc_Absynpp_cat2( ptr, rest);
_LL371: return Cyc_Absynpp_cat3( ptr, Cyc_Absynpp_ntyp2doc( _temp304), rest);
_LL367:;}} _LL281:;}} struct Cyc_PP_Doc* Cyc_Absynpp_rgn2doc( void* t){ void*
_temp372= Cyc_Tcutil_compress( t); _LL374: if( _temp372 == Cyc_Absyn_HeapRgn){
goto _LL375;} else{ goto _LL376;} _LL376: goto _LL377; _LL375: return Cyc_PP_text((
struct _tagged_string)({ char* _temp378=( char*)"`H"; struct _tagged_string
_temp379; _temp379.curr= _temp378; _temp379.base= _temp378; _temp379.last_plus_one=
_temp378 + 3; _temp379;})); _LL377: return Cyc_Absynpp_ntyp2doc( t); _LL373:;}
static struct _tuple8 Cyc_Absynpp_effects2docs( void* t){ struct Cyc_List_List*
regions= 0; struct Cyc_List_List* effects= 0;{ void* _temp380= Cyc_Tcutil_compress(
t); void* _temp388; struct Cyc_List_List* _temp390; _LL382: if(( unsigned int)
_temp380 > 5u?(( struct _enum_struct*) _temp380)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL389: _temp388=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp380)->f1;
goto _LL383;} else{ goto _LL384;} _LL384: if(( unsigned int) _temp380 > 5u?((
struct _enum_struct*) _temp380)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL391:
_temp390=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp380)->f1;
goto _LL385;} else{ goto _LL386;} _LL386: goto _LL387; _LL383: regions=({ struct
Cyc_List_List* _temp392=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp392->hd=( void*) Cyc_Absynpp_rgn2doc( _temp388); _temp392->tl= regions;
_temp392;}); goto _LL381; _LL385: for( 0; _temp390 != 0; _temp390= _temp390->tl){
struct Cyc_List_List* _temp395; struct Cyc_List_List* _temp397; struct _tuple8
_temp393= Cyc_Absynpp_effects2docs(( void*) _temp390->hd); _LL398: _temp397=
_temp393.f1; goto _LL396; _LL396: _temp395= _temp393.f2; goto _LL394; _LL394:
regions=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( _temp397, regions); effects=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp395, effects);} goto _LL381; _LL387: effects=({ struct Cyc_List_List*
_temp399=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp399->hd=( void*) Cyc_Absynpp_ntyp2doc( t); _temp399->tl= effects; _temp399;});
goto _LL381; _LL381:;} return({ struct _tuple8 _temp400; _temp400.f1= regions;
_temp400.f2= effects; _temp400;});} struct Cyc_PP_Doc* Cyc_Absynpp_eff2doc( void*
t){ struct Cyc_List_List* _temp403; struct Cyc_List_List* _temp405; struct
_tuple8 _temp401= Cyc_Absynpp_effects2docs( t); _LL406: _temp405= _temp401.f1;
goto _LL404; _LL404: _temp403= _temp401.f2; goto _LL402; _LL402: _temp405=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp405);
_temp403=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp403);{ struct Cyc_PP_Doc* doc1= Cyc_Absynpp_group(( struct _tagged_string)({
char* _temp407=( char*)"{"; struct _tagged_string _temp408; _temp408.curr=
_temp407; _temp408.base= _temp407; _temp408.last_plus_one= _temp407 + 2;
_temp408;}),( struct _tagged_string)({ char* _temp409=( char*)"}"; struct
_tagged_string _temp410; _temp410.curr= _temp409; _temp410.base= _temp409;
_temp410.last_plus_one= _temp409 + 2; _temp410;}),( struct _tagged_string)({
char* _temp411=( char*)","; struct _tagged_string _temp412; _temp412.curr=
_temp411; _temp412.base= _temp411; _temp412.last_plus_one= _temp411 + 2;
_temp412;}), _temp405); return Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp413=( char*)""; struct _tagged_string _temp414; _temp414.curr= _temp413;
_temp414.base= _temp413; _temp414.last_plus_one= _temp413 + 1; _temp414;}),(
struct _tagged_string)({ char* _temp415=( char*)""; struct _tagged_string
_temp416; _temp416.curr= _temp415; _temp416.base= _temp415; _temp416.last_plus_one=
_temp415 + 1; _temp416;}),( struct _tagged_string)({ char* _temp417=( char*)"+";
struct _tagged_string _temp418; _temp418.curr= _temp417; _temp418.base= _temp417;
_temp418.last_plus_one= _temp417 + 2; _temp418;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp403,({ struct Cyc_List_List* _temp419=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp419->hd=( void*) doc1; _temp419->tl= 0;
_temp419;})));}} struct Cyc_PP_Doc* Cyc_Absynpp_ntyp2doc( void* t){ struct Cyc_PP_Doc*
s;{ void* _temp420= t; struct Cyc_Absyn_Exp* _temp460; struct Cyc_Absyn_Tqual*
_temp462; void* _temp464; struct Cyc_Absyn_FnInfo _temp466; struct Cyc_Absyn_PtrInfo
_temp468; int _temp470; struct Cyc_Core_Opt* _temp472; void* _temp474; struct
Cyc_Absyn_Tvar* _temp476; struct Cyc_Absyn_Enumdecl** _temp478; struct Cyc_List_List*
_temp480; struct _tuple0* _temp482; struct Cyc_Absyn_Xenumdecl** _temp484;
struct _tuple0* _temp486; void* _temp488; void* _temp490; struct Cyc_List_List*
_temp492; struct Cyc_Absyn_Structdecl** _temp494; struct Cyc_List_List* _temp496;
struct _tuple0* _temp498; struct Cyc_Core_Opt* _temp500; struct Cyc_List_List*
_temp502; struct _tuple0* _temp504; void* _temp506; void* _temp508; struct Cyc_List_List*
_temp510; _LL422: if(( unsigned int) _temp420 > 5u?(( struct _enum_struct*)
_temp420)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL465: _temp464=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp420)->f1; goto _LL463; _LL463: _temp462=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp420)->f2;
goto _LL461; _LL461: _temp460=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp420)->f3; goto _LL423;} else{ goto _LL424;} _LL424: if(( unsigned int)
_temp420 > 5u?(( struct _enum_struct*) _temp420)->tag == Cyc_Absyn_FnType_tag: 0){
_LL467: _temp466=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp420)->f1; goto _LL425;} else{ goto _LL426;} _LL426: if(( unsigned int)
_temp420 > 5u?(( struct _enum_struct*) _temp420)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL469: _temp468=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp420)->f1; goto _LL427;} else{ goto _LL428;} _LL428: if( _temp420 == Cyc_Absyn_VoidType){
goto _LL429;} else{ goto _LL430;} _LL430: if(( unsigned int) _temp420 > 5u?((
struct _enum_struct*) _temp420)->tag == Cyc_Absyn_Evar_tag: 0){ _LL475: _temp474=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp420)->f1; goto _LL473; _LL473:
_temp472=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp420)->f2;
goto _LL471; _LL471: _temp470=( int)(( struct Cyc_Absyn_Evar_struct*) _temp420)->f3;
goto _LL431;} else{ goto _LL432;} _LL432: if(( unsigned int) _temp420 > 5u?((
struct _enum_struct*) _temp420)->tag == Cyc_Absyn_VarType_tag: 0){ _LL477:
_temp476=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp420)->f1;
goto _LL433;} else{ goto _LL434;} _LL434: if(( unsigned int) _temp420 > 5u?((
struct _enum_struct*) _temp420)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL483:
_temp482=( struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*) _temp420)->f1;
goto _LL481; _LL481: _temp480=( struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*)
_temp420)->f2; goto _LL479; _LL479: _temp478=( struct Cyc_Absyn_Enumdecl**)((
struct Cyc_Absyn_EnumType_struct*) _temp420)->f3; goto _LL435;} else{ goto
_LL436;} _LL436: if(( unsigned int) _temp420 > 5u?(( struct _enum_struct*)
_temp420)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL487: _temp486=( struct _tuple0*)((
struct Cyc_Absyn_XenumType_struct*) _temp420)->f1; goto _LL485; _LL485: _temp484=(
struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*) _temp420)->f2;
goto _LL437;} else{ goto _LL438;} _LL438: if(( unsigned int) _temp420 > 5u?((
struct _enum_struct*) _temp420)->tag == Cyc_Absyn_IntType_tag: 0){ _LL491:
_temp490=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp420)->f1; goto _LL489;
_LL489: _temp488=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp420)->f2;
goto _LL439;} else{ goto _LL440;} _LL440: if( _temp420 == Cyc_Absyn_FloatType){
goto _LL441;} else{ goto _LL442;} _LL442: if( _temp420 == Cyc_Absyn_DoubleType){
goto _LL443;} else{ goto _LL444;} _LL444: if(( unsigned int) _temp420 > 5u?((
struct _enum_struct*) _temp420)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL493:
_temp492=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp420)->f1;
goto _LL445;} else{ goto _LL446;} _LL446: if(( unsigned int) _temp420 > 5u?((
struct _enum_struct*) _temp420)->tag == Cyc_Absyn_StructType_tag: 0){ _LL499:
_temp498=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp420)->f1;
goto _LL497; _LL497: _temp496=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp420)->f2; goto _LL495; _LL495: _temp494=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp420)->f3; goto _LL447;} else{ goto
_LL448;} _LL448: if(( unsigned int) _temp420 > 5u?(( struct _enum_struct*)
_temp420)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL505: _temp504=( struct
_tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp420)->f1; goto _LL503;
_LL503: _temp502=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp420)->f2; goto _LL501; _LL501: _temp500=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp420)->f3; goto _LL449;} else{ goto _LL450;}
_LL450: if(( unsigned int) _temp420 > 5u?(( struct _enum_struct*) _temp420)->tag
== Cyc_Absyn_RgnHandleType_tag: 0){ _LL507: _temp506=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp420)->f1; goto _LL451;} else{ goto _LL452;} _LL452: if( _temp420 == Cyc_Absyn_UnionType){
goto _LL453;} else{ goto _LL454;} _LL454: if( _temp420 == Cyc_Absyn_HeapRgn){
goto _LL455;} else{ goto _LL456;} _LL456: if(( unsigned int) _temp420 > 5u?((
struct _enum_struct*) _temp420)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL509:
_temp508=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp420)->f1; goto
_LL457;} else{ goto _LL458;} _LL458: if(( unsigned int) _temp420 > 5u?(( struct
_enum_struct*) _temp420)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL511: _temp510=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp420)->f1; goto
_LL459;} else{ goto _LL421;} _LL423: return Cyc_PP_text(( struct _tagged_string)({
char* _temp512=( char*)"[[[array]]]"; struct _tagged_string _temp513; _temp513.curr=
_temp512; _temp513.base= _temp512; _temp513.last_plus_one= _temp512 + 12;
_temp513;})); _LL425: return Cyc_PP_nil_doc(); _LL427: return Cyc_PP_nil_doc();
_LL429: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp514=( char*)"void";
struct _tagged_string _temp515; _temp515.curr= _temp514; _temp515.base= _temp514;
_temp515.last_plus_one= _temp514 + 5; _temp515;})); goto _LL421; _LL431: if(
_temp472 != 0){ return Cyc_Absynpp_ntyp2doc(( void*) _temp472->v);} else{ s= Cyc_Absynpp_cat5(
Cyc_PP_text(( struct _tagged_string)({ char* _temp516=( char*)"%"; struct
_tagged_string _temp517; _temp517.curr= _temp516; _temp517.base= _temp516;
_temp517.last_plus_one= _temp516 + 2; _temp517;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp518=( char*)"("; struct _tagged_string _temp519;
_temp519.curr= _temp518; _temp519.base= _temp518; _temp519.last_plus_one=
_temp518 + 2; _temp519;})), Cyc_PP_text( xprintf("%d", _temp470)), Cyc_PP_text((
struct _tagged_string)({ char* _temp520=( char*)")::"; struct _tagged_string
_temp521; _temp521.curr= _temp520; _temp521.base= _temp520; _temp521.last_plus_one=
_temp520 + 4; _temp521;})), Cyc_Absynpp_kind2doc( _temp474));} goto _LL421;
_LL433: if( Cyc_Absynpp_print_all_kinds){ s= Cyc_Absynpp_cat3( Cyc_Absynpp_textptr(
_temp476->name), Cyc_PP_text(( struct _tagged_string)({ char* _temp522=( char*)"::";
struct _tagged_string _temp523; _temp523.curr= _temp522; _temp523.base= _temp522;
_temp523.last_plus_one= _temp522 + 3; _temp523;})), Cyc_Absynpp_ckind2doc(
_temp476->kind));} else{ s= Cyc_Absynpp_textptr( _temp476->name);} goto _LL421;
_LL435: if( _temp482 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp524=( char*)"enum "; struct _tagged_string _temp525;
_temp525.curr= _temp524; _temp525.base= _temp524; _temp525.last_plus_one=
_temp524 + 6; _temp525;})), Cyc_Absynpp_tps2doc( _temp480));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp526=( char*)"enum "; struct
_tagged_string _temp527; _temp527.curr= _temp526; _temp527.base= _temp526;
_temp527.last_plus_one= _temp526 + 6; _temp527;})), Cyc_Absynpp_qvar2doc((
struct _tuple0*)({ struct _tuple0* _temp528= _temp482; if( _temp528 == 0){
_throw( Null_Exception);} _temp528;})), Cyc_Absynpp_tps2doc( _temp480));} goto
_LL421; _LL437: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp529=( char*)"xenum "; struct _tagged_string _temp530; _temp530.curr=
_temp529; _temp530.base= _temp529; _temp530.last_plus_one= _temp529 + 7;
_temp530;})), Cyc_Absynpp_qvar2doc( _temp486)); goto _LL421; _LL439: { struct
_tagged_string sns; struct _tagged_string ts;{ void* _temp531= _temp490; _LL533:
if( _temp531 == Cyc_Absyn_Signed){ goto _LL534;} else{ goto _LL535;} _LL535: if(
_temp531 == Cyc_Absyn_Unsigned){ goto _LL536;} else{ goto _LL532;} _LL534: sns=(
struct _tagged_string)({ char* _temp537=( char*)""; struct _tagged_string
_temp538; _temp538.curr= _temp537; _temp538.base= _temp537; _temp538.last_plus_one=
_temp537 + 1; _temp538;}); goto _LL532; _LL536: sns=( struct _tagged_string)({
char* _temp539=( char*)"unsigned "; struct _tagged_string _temp540; _temp540.curr=
_temp539; _temp540.base= _temp539; _temp540.last_plus_one= _temp539 + 10;
_temp540;}); goto _LL532; _LL532:;}{ void* _temp541= _temp488; _LL543: if(
_temp541 == Cyc_Absyn_B1){ goto _LL544;} else{ goto _LL545;} _LL545: if(
_temp541 == Cyc_Absyn_B2){ goto _LL546;} else{ goto _LL547;} _LL547: if(
_temp541 == Cyc_Absyn_B4){ goto _LL548;} else{ goto _LL549;} _LL549: if(
_temp541 == Cyc_Absyn_B8){ goto _LL550;} else{ goto _LL542;} _LL544:{ void*
_temp551= _temp490; _LL553: if( _temp551 == Cyc_Absyn_Signed){ goto _LL554;}
else{ goto _LL555;} _LL555: if( _temp551 == Cyc_Absyn_Unsigned){ goto _LL556;}
else{ goto _LL552;} _LL554: sns=( struct _tagged_string)({ char* _temp557=( char*)"signed ";
struct _tagged_string _temp558; _temp558.curr= _temp557; _temp558.base= _temp557;
_temp558.last_plus_one= _temp557 + 8; _temp558;}); goto _LL552; _LL556: sns=(
struct _tagged_string)({ char* _temp559=( char*)""; struct _tagged_string
_temp560; _temp560.curr= _temp559; _temp560.base= _temp559; _temp560.last_plus_one=
_temp559 + 1; _temp560;}); goto _LL552; _LL552:;} ts=( struct _tagged_string)({
char* _temp561=( char*)"char"; struct _tagged_string _temp562; _temp562.curr=
_temp561; _temp562.base= _temp561; _temp562.last_plus_one= _temp561 + 5;
_temp562;}); goto _LL542; _LL546: ts=( struct _tagged_string)({ char* _temp563=(
char*)"short"; struct _tagged_string _temp564; _temp564.curr= _temp563; _temp564.base=
_temp563; _temp564.last_plus_one= _temp563 + 6; _temp564;}); goto _LL542; _LL548:
ts=( struct _tagged_string)({ char* _temp565=( char*)"int"; struct
_tagged_string _temp566; _temp566.curr= _temp565; _temp566.base= _temp565;
_temp566.last_plus_one= _temp565 + 4; _temp566;}); goto _LL542; _LL550: ts=(
struct _tagged_string)({ char* _temp567=( char*)"long long"; struct
_tagged_string _temp568; _temp568.curr= _temp567; _temp568.base= _temp567;
_temp568.last_plus_one= _temp567 + 10; _temp568;}); goto _LL542; _LL542:;} s=
Cyc_PP_text(({ struct _tagged_string _temp569= sns; struct _tagged_string
_temp570= ts; xprintf("%.*s%.*s", _temp569.last_plus_one - _temp569.curr,
_temp569.curr, _temp570.last_plus_one - _temp570.curr, _temp570.curr);})); goto
_LL421;} _LL441: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp571=( char*)"float";
struct _tagged_string _temp572; _temp572.curr= _temp571; _temp572.base= _temp571;
_temp572.last_plus_one= _temp571 + 6; _temp572;})); goto _LL421; _LL443: s= Cyc_PP_text((
struct _tagged_string)({ char* _temp573=( char*)"double"; struct _tagged_string
_temp574; _temp574.curr= _temp573; _temp574.base= _temp573; _temp574.last_plus_one=
_temp573 + 7; _temp574;})); goto _LL421; _LL445: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp575=( char*)"$"; struct _tagged_string
_temp576; _temp576.curr= _temp575; _temp576.base= _temp575; _temp576.last_plus_one=
_temp575 + 2; _temp576;})), Cyc_Absynpp_args2doc( _temp492)); goto _LL421;
_LL447: if( _temp498 == 0){ s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp577=( char*)"struct "; struct _tagged_string
_temp578; _temp578.curr= _temp577; _temp578.base= _temp577; _temp578.last_plus_one=
_temp577 + 8; _temp578;})), Cyc_Absynpp_tps2doc( _temp496));} else{ s= Cyc_Absynpp_cat3(
Cyc_PP_text(( struct _tagged_string)({ char* _temp579=( char*)"struct "; struct
_tagged_string _temp580; _temp580.curr= _temp579; _temp580.base= _temp579;
_temp580.last_plus_one= _temp579 + 8; _temp580;})), _temp498 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc(( struct _tuple0*)({ struct _tuple0* _temp581= _temp498;
if( _temp581 == 0){ _throw( Null_Exception);} _temp581;})), Cyc_Absynpp_tps2doc(
_temp496));} goto _LL421; _LL449: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp504), Cyc_Absynpp_tps2doc( _temp502)); goto _LL421; _LL451: s= Cyc_Absynpp_rgn2doc(
_temp506); goto _LL421; _LL453: s= Cyc_PP_text(( struct _tagged_string)({ char*
_temp582=( char*)"union {unimplemented}"; struct _tagged_string _temp583;
_temp583.curr= _temp582; _temp583.base= _temp582; _temp583.last_plus_one=
_temp582 + 22; _temp583;})); goto _LL421; _LL455: s= Cyc_Absynpp_rgn2doc( t);
goto _LL421; _LL457: s= Cyc_Absynpp_eff2doc( t); goto _LL421; _LL459: s= Cyc_Absynpp_eff2doc(
t); goto _LL421; _LL421:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt* vo){ return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*((
struct _tagged_string*) vo->v));} struct Cyc_PP_Doc* Cyc_Absynpp_funarg2doc(
struct _tuple1* t){ struct Cyc_Core_Opt* dopt=(* t).f1 == 0? 0:({ struct Cyc_Core_Opt*
_temp584=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp584->v=( void*) Cyc_PP_text(*(( struct _tagged_string*)((* t).f1)->v));
_temp584;}); return Cyc_Absynpp_tqtd2doc((* t).f2,(* t).f3, dopt);} struct Cyc_PP_Doc*
Cyc_Absynpp_funargs2doc( struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt*
effopt){ if( effopt == 0){ if( varargs){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp585=( char*)"("; struct _tagged_string _temp586;
_temp586.curr= _temp585; _temp586.base= _temp585; _temp586.last_plus_one=
_temp585 + 2; _temp586;}),( struct _tagged_string)({ char* _temp587=( char*)")";
struct _tagged_string _temp588; _temp588.curr= _temp587; _temp588.base= _temp587;
_temp588.last_plus_one= _temp587 + 2; _temp588;}),( struct _tagged_string)({
char* _temp589=( char*)","; struct _tagged_string _temp590; _temp590.curr=
_temp589; _temp590.base= _temp589; _temp590.last_plus_one= _temp589 + 2;
_temp590;}),(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)((( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple1*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc,
args),({ struct Cyc_List_List* _temp591=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp591->hd=( void*) Cyc_PP_text(( struct
_tagged_string)({ char* _temp592=( char*)"..."; struct _tagged_string _temp593;
_temp593.curr= _temp592; _temp593.base= _temp592; _temp593.last_plus_one=
_temp592 + 4; _temp593;})); _temp591->tl= 0; _temp591;})));} else{ return Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp594=( char*)"("; struct _tagged_string
_temp595; _temp595.curr= _temp594; _temp595.base= _temp594; _temp595.last_plus_one=
_temp594 + 2; _temp595;}),( struct _tagged_string)({ char* _temp596=( char*)")";
struct _tagged_string _temp597; _temp597.curr= _temp596; _temp597.base= _temp596;
_temp597.last_plus_one= _temp596 + 2; _temp597;}),( struct _tagged_string)({
char* _temp598=( char*)","; struct _tagged_string _temp599; _temp599.curr=
_temp598; _temp599.base= _temp598; _temp599.last_plus_one= _temp598 + 2;
_temp599;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args));}} else{
if( varargs){ return Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp600=(
char*)"("; struct _tagged_string _temp601; _temp601.curr= _temp600; _temp601.base=
_temp600; _temp601.last_plus_one= _temp600 + 2; _temp601;}),( struct
_tagged_string)({ char* _temp602=( char*)")"; struct _tagged_string _temp603;
_temp603.curr= _temp602; _temp603.base= _temp602; _temp603.last_plus_one=
_temp602 + 2; _temp603;}),( struct _tagged_string)({ char* _temp604=( char*)";";
struct _tagged_string _temp605; _temp605.curr= _temp604; _temp605.base= _temp604;
_temp605.last_plus_one= _temp604 + 2; _temp605;}),({ struct Cyc_List_List*
_temp606=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp606->hd=( void*) Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp608=( char*)""; struct _tagged_string _temp609; _temp609.curr= _temp608;
_temp609.base= _temp608; _temp609.last_plus_one= _temp608 + 1; _temp609;}),(
struct _tagged_string)({ char* _temp610=( char*)""; struct _tagged_string
_temp611; _temp611.curr= _temp610; _temp611.base= _temp610; _temp611.last_plus_one=
_temp610 + 1; _temp611;}),( struct _tagged_string)({ char* _temp612=( char*)",";
struct _tagged_string _temp613; _temp613.curr= _temp612; _temp613.base= _temp612;
_temp613.last_plus_one= _temp612 + 2; _temp613;}),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)((( struct
Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args),({ struct Cyc_List_List*
_temp614=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp614->hd=( void*) Cyc_PP_text(( struct _tagged_string)({ char* _temp615=(
char*)"..."; struct _tagged_string _temp616; _temp616.curr= _temp615; _temp616.base=
_temp615; _temp616.last_plus_one= _temp615 + 4; _temp616;})); _temp614->tl= 0;
_temp614;}))); _temp606->tl=({ struct Cyc_List_List* _temp607=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp607->hd=( void*) Cyc_Absynpp_eff2doc((
void*) effopt->v); _temp607->tl= 0; _temp607;}); _temp606;}));} else{ return Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp617=( char*)"("; struct _tagged_string
_temp618; _temp618.curr= _temp617; _temp618.base= _temp617; _temp618.last_plus_one=
_temp617 + 2; _temp618;}),( struct _tagged_string)({ char* _temp619=( char*)")";
struct _tagged_string _temp620; _temp620.curr= _temp619; _temp620.base= _temp619;
_temp620.last_plus_one= _temp619 + 2; _temp620;}),( struct _tagged_string)({
char* _temp621=( char*)";"; struct _tagged_string _temp622; _temp622.curr=
_temp621; _temp622.base= _temp621; _temp622.last_plus_one= _temp621 + 2;
_temp622;}),({ struct Cyc_List_List* _temp623=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp623->hd=( void*) Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp625=( char*)""; struct _tagged_string _temp626;
_temp626.curr= _temp625; _temp626.base= _temp625; _temp626.last_plus_one=
_temp625 + 1; _temp626;}),( struct _tagged_string)({ char* _temp627=( char*)"";
struct _tagged_string _temp628; _temp628.curr= _temp627; _temp628.base= _temp627;
_temp628.last_plus_one= _temp627 + 1; _temp628;}),( struct _tagged_string)({
char* _temp629=( char*)","; struct _tagged_string _temp630; _temp630.curr=
_temp629; _temp630.base= _temp629; _temp630.last_plus_one= _temp629 + 2;
_temp630;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct _tuple1*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_funarg2doc, args));
_temp623->tl=({ struct Cyc_List_List* _temp624=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp624->hd=( void*) Cyc_Absynpp_eff2doc((
void*) effopt->v); _temp624->tl= 0; _temp624;}); _temp623;}));}}} struct _tuple1*
Cyc_Absynpp_arg_mk_opt( struct _tuple3* arg){ return({ struct _tuple1* _temp631=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp631->f1=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp632=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp632->v=( void*)(* arg).f1; _temp632;}); _temp631->f2=(* arg).f2;
_temp631->f3=(* arg).f3; _temp631;});} struct Cyc_PP_Doc* Cyc_Absynpp_var2doc(
struct _tagged_string* v){ return Cyc_PP_text(* v);} struct Cyc_PP_Doc* Cyc_Absynpp_qvar2doc(
struct _tuple0* q){ struct Cyc_List_List* prefix= 0;{ void* _temp633=(* q).f1;
struct Cyc_List_List* _temp645; struct Cyc_List_List* _temp647; struct Cyc_List_List*
_temp649; struct Cyc_List_List* _temp651; _LL635: if( _temp633 == Cyc_Absyn_Loc_n){
goto _LL636;} else{ goto _LL637;} _LL637: if(( unsigned int) _temp633 > 1u?((
struct _enum_struct*) _temp633)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL646:
_temp645=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp633)->f1;
if( _temp645 == 0){ goto _LL638;} else{ goto _LL639;}} else{ goto _LL639;}
_LL639: if(( unsigned int) _temp633 > 1u?(( struct _enum_struct*) _temp633)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL648: _temp647=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp633)->f1; if( _temp647 == 0){ goto _LL640;} else{
goto _LL641;}} else{ goto _LL641;} _LL641: if(( unsigned int) _temp633 > 1u?((
struct _enum_struct*) _temp633)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL650:
_temp649=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp633)->f1;
goto _LL642;} else{ goto _LL643;} _LL643: if(( unsigned int) _temp633 > 1u?((
struct _enum_struct*) _temp633)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL652:
_temp651=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp633)->f1;
goto _LL644;} else{ goto _LL634;} _LL636: goto _LL638; _LL638: goto _LL640;
_LL640: return Cyc_Absynpp_var2doc((* q).f2); _LL642: _temp651= _temp649; goto
_LL644; _LL644: prefix= _temp651; goto _LL634; _LL634:;} if( Cyc_Absynpp_qvar_to_Cids){
return Cyc_PP_text( Cyc_String_str_sepstr((( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( prefix,({ struct
Cyc_List_List* _temp653=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp653->hd=( void*)(* q).f2; _temp653->tl= 0; _temp653;})),( struct
_tagged_string)({ char* _temp654=( char*)"_"; struct _tagged_string _temp655;
_temp655.curr= _temp654; _temp655.base= _temp654; _temp655.last_plus_one=
_temp654 + 2; _temp655;})));} else{ return Cyc_Absynpp_cat2( Cyc_Absynpp_egroup((
struct _tagged_string)({ char* _temp656=( char*)""; struct _tagged_string
_temp657; _temp657.curr= _temp656; _temp657.base= _temp656; _temp657.last_plus_one=
_temp656 + 1; _temp657;}),( struct _tagged_string)({ char* _temp658=( char*)"::";
struct _tagged_string _temp659; _temp659.curr= _temp658; _temp659.base= _temp658;
_temp659.last_plus_one= _temp658 + 3; _temp659;}),( struct _tagged_string)({
char* _temp660=( char*)"::"; struct _tagged_string _temp661; _temp661.curr=
_temp660; _temp661.base= _temp660; _temp661.last_plus_one= _temp660 + 3;
_temp661;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tagged_string*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_var2doc,
prefix)), Cyc_Absynpp_var2doc((* q).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_typ2doc(
void* t){ return Cyc_Absynpp_tqtd2doc( Cyc_Absyn_empty_tqual(), t, 0);} int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp* e){ void* _temp662=( void*) e->r; void* _temp726; void*
_temp728; struct _tuple0* _temp730; struct _tuple0* _temp732; struct Cyc_List_List*
_temp734; void* _temp736; struct Cyc_Absyn_Exp* _temp738; struct Cyc_Core_Opt*
_temp740; struct Cyc_Absyn_Exp* _temp742; void* _temp744; struct Cyc_Absyn_Exp*
_temp746; struct Cyc_Absyn_Exp* _temp748; struct Cyc_Absyn_Exp* _temp750; struct
Cyc_Absyn_Exp* _temp752; struct Cyc_Absyn_Exp* _temp754; struct Cyc_Absyn_Exp*
_temp756; struct Cyc_List_List* _temp758; struct Cyc_Absyn_Exp* _temp760; struct
Cyc_List_List* _temp762; struct Cyc_Absyn_Exp* _temp764; struct Cyc_Absyn_Exp*
_temp766; struct Cyc_Absyn_Exp* _temp768; struct Cyc_List_List* _temp770; struct
Cyc_Absyn_Exp* _temp772; struct Cyc_Absyn_Exp* _temp774; void* _temp776; struct
Cyc_Absyn_Exp* _temp778; void* _temp780; struct Cyc_Absyn_Exp* _temp782; struct
_tagged_string* _temp784; struct Cyc_Absyn_Exp* _temp786; struct _tagged_string*
_temp788; struct Cyc_Absyn_Exp* _temp790; struct Cyc_Absyn_Exp* _temp792; struct
Cyc_Absyn_Exp* _temp794; struct Cyc_List_List* _temp796; struct Cyc_List_List*
_temp798; struct _tuple1* _temp800; struct Cyc_List_List* _temp802; int _temp804;
struct Cyc_Absyn_Exp* _temp806; struct Cyc_Absyn_Exp* _temp808; struct Cyc_Absyn_Vardecl*
_temp810; struct Cyc_Absyn_Structdecl* _temp812; struct Cyc_List_List* _temp814;
struct Cyc_Core_Opt* _temp816; struct _tuple0* _temp818; struct Cyc_Absyn_Enumfield*
_temp820; struct Cyc_Absyn_Enumdecl* _temp822; struct Cyc_List_List* _temp824;
struct Cyc_Core_Opt* _temp826; struct Cyc_Core_Opt* _temp828; struct Cyc_Absyn_Enumfield*
_temp830; struct Cyc_Absyn_Xenumdecl* _temp832; struct Cyc_List_List* _temp834;
struct Cyc_Core_Opt* _temp836; struct Cyc_List_List* _temp838; struct Cyc_Core_Opt*
_temp840; struct Cyc_Absyn_Stmt* _temp842; struct Cyc_Absyn_Fndecl* _temp844;
struct Cyc_Absyn_Exp* _temp846; _LL664: if((( struct _enum_struct*) _temp662)->tag
== Cyc_Absyn_Const_e_tag){ _LL727: _temp726=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp662)->f1; goto _LL665;} else{ goto _LL666;} _LL666: if((( struct
_enum_struct*) _temp662)->tag == Cyc_Absyn_Var_e_tag){ _LL731: _temp730=( struct
_tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp662)->f1; goto _LL729; _LL729:
_temp728=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp662)->f2; goto _LL667;}
else{ goto _LL668;} _LL668: if((( struct _enum_struct*) _temp662)->tag == Cyc_Absyn_UnknownId_e_tag){
_LL733: _temp732=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp662)->f1; goto _LL669;} else{ goto _LL670;} _LL670: if((( struct
_enum_struct*) _temp662)->tag == Cyc_Absyn_Primop_e_tag){ _LL737: _temp736=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp662)->f1; goto _LL735; _LL735:
_temp734=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*) _temp662)->f2;
goto _LL671;} else{ goto _LL672;} _LL672: if((( struct _enum_struct*) _temp662)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL743: _temp742=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp662)->f1; goto _LL741; _LL741: _temp740=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp662)->f2; goto
_LL739; _LL739: _temp738=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp662)->f3; goto _LL673;} else{ goto _LL674;} _LL674: if((( struct
_enum_struct*) _temp662)->tag == Cyc_Absyn_Increment_e_tag){ _LL747: _temp746=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*) _temp662)->f1;
goto _LL745; _LL745: _temp744=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp662)->f2; goto _LL675;} else{ goto _LL676;} _LL676: if((( struct
_enum_struct*) _temp662)->tag == Cyc_Absyn_Conditional_e_tag){ _LL753: _temp752=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp662)->f1;
goto _LL751; _LL751: _temp750=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp662)->f2; goto _LL749; _LL749: _temp748=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp662)->f3; goto _LL677;} else{ goto _LL678;}
_LL678: if((( struct _enum_struct*) _temp662)->tag == Cyc_Absyn_SeqExp_e_tag){
_LL757: _temp756=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp662)->f1; goto _LL755; _LL755: _temp754=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp662)->f2; goto _LL679;} else{ goto _LL680;}
_LL680: if((( struct _enum_struct*) _temp662)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL761: _temp760=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp662)->f1; goto _LL759; _LL759: _temp758=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_e_struct*) _temp662)->f2; goto _LL681;} else{ goto _LL682;}
_LL682: if((( struct _enum_struct*) _temp662)->tag == Cyc_Absyn_FnCall_e_tag){
_LL765: _temp764=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp662)->f1; goto _LL763; _LL763: _temp762=( struct Cyc_List_List*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp662)->f2; goto _LL683;} else{ goto _LL684;}
_LL684: if((( struct _enum_struct*) _temp662)->tag == Cyc_Absyn_Throw_e_tag){
_LL767: _temp766=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp662)->f1; goto _LL685;} else{ goto _LL686;} _LL686: if((( struct
_enum_struct*) _temp662)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL769:
_temp768=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp662)->f1; goto _LL687;} else{ goto _LL688;} _LL688: if((( struct
_enum_struct*) _temp662)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL773: _temp772=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*) _temp662)->f1;
goto _LL771; _LL771: _temp770=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp662)->f2; goto _LL689;} else{ goto _LL690;} _LL690: if((( struct
_enum_struct*) _temp662)->tag == Cyc_Absyn_Cast_e_tag){ _LL777: _temp776=( void*)((
struct Cyc_Absyn_Cast_e_struct*) _temp662)->f1; goto _LL775; _LL775: _temp774=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp662)->f2; goto
_LL691;} else{ goto _LL692;} _LL692: if((( struct _enum_struct*) _temp662)->tag
== Cyc_Absyn_Address_e_tag){ _LL779: _temp778=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Address_e_struct*) _temp662)->f1; goto _LL693;} else{ goto _LL694;}
_LL694: if((( struct _enum_struct*) _temp662)->tag == Cyc_Absyn_Sizeof_e_tag){
_LL781: _temp780=( void*)(( struct Cyc_Absyn_Sizeof_e_struct*) _temp662)->f1;
goto _LL695;} else{ goto _LL696;} _LL696: if((( struct _enum_struct*) _temp662)->tag
== Cyc_Absyn_Deref_e_tag){ _LL783: _temp782=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp662)->f1; goto _LL697;} else{ goto _LL698;}
_LL698: if((( struct _enum_struct*) _temp662)->tag == Cyc_Absyn_StructMember_e_tag){
_LL787: _temp786=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp662)->f1; goto _LL785; _LL785: _temp784=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp662)->f2; goto _LL699;} else{ goto _LL700;}
_LL700: if((( struct _enum_struct*) _temp662)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL791: _temp790=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp662)->f1; goto _LL789; _LL789: _temp788=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp662)->f2; goto _LL701;} else{ goto _LL702;}
_LL702: if((( struct _enum_struct*) _temp662)->tag == Cyc_Absyn_Subscript_e_tag){
_LL795: _temp794=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp662)->f1; goto _LL793; _LL793: _temp792=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp662)->f2; goto _LL703;} else{ goto _LL704;}
_LL704: if((( struct _enum_struct*) _temp662)->tag == Cyc_Absyn_Tuple_e_tag){
_LL797: _temp796=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp662)->f1; goto _LL705;} else{ goto _LL706;} _LL706: if((( struct
_enum_struct*) _temp662)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL801: _temp800=(
struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp662)->f1; goto
_LL799; _LL799: _temp798=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp662)->f2; goto _LL707;} else{ goto _LL708;} _LL708: if((( struct
_enum_struct*) _temp662)->tag == Cyc_Absyn_Array_e_tag){ _LL805: _temp804=( int)((
struct Cyc_Absyn_Array_e_struct*) _temp662)->f1; goto _LL803; _LL803: _temp802=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*) _temp662)->f2; goto
_LL709;} else{ goto _LL710;} _LL710: if((( struct _enum_struct*) _temp662)->tag
== Cyc_Absyn_Comprehension_e_tag){ _LL811: _temp810=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp662)->f1; goto _LL809; _LL809:
_temp808=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp662)->f2; goto _LL807; _LL807: _temp806=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp662)->f3; goto _LL711;} else{ goto
_LL712;} _LL712: if((( struct _enum_struct*) _temp662)->tag == Cyc_Absyn_Struct_e_tag){
_LL819: _temp818=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp662)->f1; goto _LL817; _LL817: _temp816=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp662)->f2; goto _LL815; _LL815: _temp814=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp662)->f3; goto
_LL813; _LL813: _temp812=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp662)->f4; goto _LL713;} else{ goto _LL714;} _LL714: if((( struct
_enum_struct*) _temp662)->tag == Cyc_Absyn_Enum_e_tag){ _LL829: _temp828=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp662)->f1; goto
_LL827; _LL827: _temp826=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp662)->f2; goto _LL825; _LL825: _temp824=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp662)->f3; goto _LL823; _LL823: _temp822=( struct
Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp662)->f4; goto
_LL821; _LL821: _temp820=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp662)->f5; goto _LL715;} else{ goto _LL716;} _LL716: if((( struct
_enum_struct*) _temp662)->tag == Cyc_Absyn_Xenum_e_tag){ _LL837: _temp836=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp662)->f1; goto
_LL835; _LL835: _temp834=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp662)->f2; goto _LL833; _LL833: _temp832=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp662)->f3; goto _LL831; _LL831: _temp830=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp662)->f4;
goto _LL717;} else{ goto _LL718;} _LL718: if((( struct _enum_struct*) _temp662)->tag
== Cyc_Absyn_UnresolvedMem_e_tag){ _LL841: _temp840=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp662)->f1; goto _LL839; _LL839:
_temp838=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp662)->f2; goto _LL719;} else{ goto _LL720;} _LL720: if((( struct
_enum_struct*) _temp662)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL843: _temp842=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*) _temp662)->f1;
goto _LL721;} else{ goto _LL722;} _LL722: if((( struct _enum_struct*) _temp662)->tag
== Cyc_Absyn_Codegen_e_tag){ _LL845: _temp844=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Codegen_e_struct*) _temp662)->f1; goto _LL723;} else{ goto
_LL724;} _LL724: if((( struct _enum_struct*) _temp662)->tag == Cyc_Absyn_Fill_e_tag){
_LL847: _temp846=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp662)->f1; goto _LL725;} else{ goto _LL663;} _LL665: return 10000; _LL667:
return 10000; _LL669: return 10000; _LL671: { void* _temp848= _temp736; _LL850:
if( _temp848 == Cyc_Absyn_Plus){ goto _LL851;} else{ goto _LL852;} _LL852: if(
_temp848 == Cyc_Absyn_Times){ goto _LL853;} else{ goto _LL854;} _LL854: if(
_temp848 == Cyc_Absyn_Minus){ goto _LL855;} else{ goto _LL856;} _LL856: if(
_temp848 == Cyc_Absyn_Div){ goto _LL857;} else{ goto _LL858;} _LL858: if(
_temp848 == Cyc_Absyn_Mod){ goto _LL859;} else{ goto _LL860;} _LL860: if(
_temp848 == Cyc_Absyn_Eq){ goto _LL861;} else{ goto _LL862;} _LL862: if(
_temp848 == Cyc_Absyn_Neq){ goto _LL863;} else{ goto _LL864;} _LL864: if(
_temp848 == Cyc_Absyn_Gt){ goto _LL865;} else{ goto _LL866;} _LL866: if(
_temp848 == Cyc_Absyn_Lt){ goto _LL867;} else{ goto _LL868;} _LL868: if(
_temp848 == Cyc_Absyn_Gte){ goto _LL869;} else{ goto _LL870;} _LL870: if(
_temp848 == Cyc_Absyn_Lte){ goto _LL871;} else{ goto _LL872;} _LL872: if(
_temp848 == Cyc_Absyn_Not){ goto _LL873;} else{ goto _LL874;} _LL874: if(
_temp848 == Cyc_Absyn_Bitnot){ goto _LL875;} else{ goto _LL876;} _LL876: if(
_temp848 == Cyc_Absyn_Bitand){ goto _LL877;} else{ goto _LL878;} _LL878: if(
_temp848 == Cyc_Absyn_Bitor){ goto _LL879;} else{ goto _LL880;} _LL880: if(
_temp848 == Cyc_Absyn_Bitxor){ goto _LL881;} else{ goto _LL882;} _LL882: if(
_temp848 == Cyc_Absyn_Bitlshift){ goto _LL883;} else{ goto _LL884;} _LL884: if(
_temp848 == Cyc_Absyn_Bitlrshift){ goto _LL885;} else{ goto _LL886;} _LL886: if(
_temp848 == Cyc_Absyn_Bitarshift){ goto _LL887;} else{ goto _LL888;} _LL888: if(
_temp848 == Cyc_Absyn_Size){ goto _LL889;} else{ goto _LL890;} _LL890: if(
_temp848 == Cyc_Absyn_Printf){ goto _LL891;} else{ goto _LL892;} _LL892: if(
_temp848 == Cyc_Absyn_Fprintf){ goto _LL893;} else{ goto _LL894;} _LL894: if(
_temp848 == Cyc_Absyn_Xprintf){ goto _LL895;} else{ goto _LL896;} _LL896: if(
_temp848 == Cyc_Absyn_Scanf){ goto _LL897;} else{ goto _LL898;} _LL898: if(
_temp848 == Cyc_Absyn_Fscanf){ goto _LL899;} else{ goto _LL900;} _LL900: if(
_temp848 == Cyc_Absyn_Sscanf){ goto _LL901;} else{ goto _LL849;} _LL851: return
100; _LL853: return 110; _LL855: return 100; _LL857: return 110; _LL859: return
110; _LL861: return 70; _LL863: return 70; _LL865: return 80; _LL867: return 80;
_LL869: return 80; _LL871: return 80; _LL873: return 130; _LL875: return 130;
_LL877: return 60; _LL879: return 40; _LL881: return 50; _LL883: return 90;
_LL885: return 80; _LL887: return 80; _LL889: return 140; _LL891: return 130;
_LL893: return 130; _LL895: return 130; _LL897: return 130; _LL899: return 130;
_LL901: return 130; _LL849:;} _LL673: return 20; _LL675: return 20; _LL677:
return 30; _LL679: return 10; _LL681: return 140; _LL683: return 140; _LL685:
return 130; _LL687: return Cyc_Absynpp_exp_prec( _temp768); _LL689: return Cyc_Absynpp_exp_prec(
_temp772); _LL691: return 120; _LL693: return 130; _LL695: return 130; _LL697:
return 130; _LL699: return 140; _LL701: return 140; _LL703: return 140; _LL705:
return 150; _LL707: return 140; _LL709: return 140; _LL711: return 140; _LL713:
return 140; _LL715: return 140; _LL717: return 140; _LL719: return 140; _LL721:
return 10000; _LL723: return 140; _LL725: return 140; _LL663:;} struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc( struct Cyc_Absyn_Exp* e){ return Cyc_Absynpp_exp2doc_prec(
0, e);} struct Cyc_PP_Doc* Cyc_Absynpp_exp2doc_prec( int inprec, struct Cyc_Absyn_Exp*
e){ int myprec= Cyc_Absynpp_exp_prec( e); struct Cyc_PP_Doc* s;{ void* _temp902=(
void*) e->r; void* _temp972; void* _temp974; struct Cyc_Absyn_Vardecl* _temp976;
struct _tuple0* _temp978; void* _temp980; struct Cyc_Absyn_Vardecl* _temp982;
struct _tuple0* _temp984; void* _temp986; struct _tuple0* _temp988; struct
_tuple0* _temp990; struct Cyc_List_List* _temp992; void* _temp994; struct Cyc_Absyn_Exp*
_temp996; struct Cyc_Core_Opt* _temp998; struct Cyc_Absyn_Exp* _temp1000; void*
_temp1002; struct Cyc_Absyn_Exp* _temp1004; struct Cyc_Absyn_Exp* _temp1006;
struct Cyc_Absyn_Exp* _temp1008; struct Cyc_Absyn_Exp* _temp1010; struct Cyc_Absyn_Exp*
_temp1012; struct Cyc_Absyn_Exp* _temp1014; struct Cyc_List_List* _temp1016;
struct Cyc_Absyn_Exp* _temp1018; struct Cyc_List_List* _temp1020; struct Cyc_Absyn_Exp*
_temp1022; struct Cyc_Absyn_Exp* _temp1024; struct Cyc_Absyn_Exp* _temp1026;
struct Cyc_List_List* _temp1028; struct Cyc_Absyn_Exp* _temp1030; struct Cyc_Absyn_Exp*
_temp1032; void* _temp1034; struct Cyc_Absyn_Exp* _temp1036; void* _temp1038;
struct Cyc_Absyn_Exp* _temp1040; struct _tagged_string* _temp1042; struct Cyc_Absyn_Exp*
_temp1044; struct _tagged_string* _temp1046; struct Cyc_Absyn_Exp* _temp1048;
struct Cyc_Absyn_Exp* _temp1050; struct Cyc_Absyn_Exp* _temp1052; struct Cyc_List_List*
_temp1054; struct Cyc_List_List* _temp1056; struct _tuple1* _temp1058; struct
Cyc_List_List* _temp1060; int _temp1062; struct Cyc_List_List* _temp1064; int
_temp1066; struct Cyc_Absyn_Exp* _temp1068; struct Cyc_Absyn_Exp* _temp1070;
struct Cyc_Absyn_Vardecl* _temp1072; struct Cyc_Absyn_Structdecl* _temp1074;
struct Cyc_List_List* _temp1076; struct Cyc_Core_Opt* _temp1078; struct _tuple0*
_temp1080; struct Cyc_Absyn_Enumfield* _temp1082; struct Cyc_Absyn_Enumdecl*
_temp1084; struct Cyc_List_List* _temp1086; struct Cyc_Core_Opt* _temp1088;
struct Cyc_Core_Opt* _temp1090; struct Cyc_Absyn_Enumfield* _temp1092; struct
Cyc_Absyn_Xenumdecl* _temp1094; struct Cyc_List_List* _temp1096; struct Cyc_Core_Opt*
_temp1098; struct Cyc_List_List* _temp1100; struct Cyc_Core_Opt* _temp1102;
struct Cyc_Absyn_Stmt* _temp1104; struct Cyc_Absyn_Fndecl* _temp1106; struct Cyc_Absyn_Exp*
_temp1108; _LL904: if((( struct _enum_struct*) _temp902)->tag == Cyc_Absyn_Const_e_tag){
_LL973: _temp972=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp902)->f1;
goto _LL905;} else{ goto _LL906;} _LL906: if((( struct _enum_struct*) _temp902)->tag
== Cyc_Absyn_Var_e_tag){ _LL979: _temp978=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp902)->f1; goto _LL975; _LL975: _temp974=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp902)->f2; if(( unsigned int) _temp974 > 1u?(( struct _enum_struct*)
_temp974)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL977: _temp976=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp974)->f1; goto _LL907;} else{ goto _LL908;}}
else{ goto _LL908;} _LL908: if((( struct _enum_struct*) _temp902)->tag == Cyc_Absyn_Var_e_tag){
_LL985: _temp984=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp902)->f1;
goto _LL981; _LL981: _temp980=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp902)->f2; if(( unsigned int) _temp980 > 1u?(( struct _enum_struct*)
_temp980)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL983: _temp982=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp980)->f1; goto _LL909;} else{ goto _LL910;}}
else{ goto _LL910;} _LL910: if((( struct _enum_struct*) _temp902)->tag == Cyc_Absyn_Var_e_tag){
_LL989: _temp988=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp902)->f1;
goto _LL987; _LL987: _temp986=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp902)->f2; goto _LL911;} else{ goto _LL912;} _LL912: if((( struct
_enum_struct*) _temp902)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL991: _temp990=(
struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp902)->f1; goto
_LL913;} else{ goto _LL914;} _LL914: if((( struct _enum_struct*) _temp902)->tag
== Cyc_Absyn_Primop_e_tag){ _LL995: _temp994=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp902)->f1; goto _LL993; _LL993: _temp992=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp902)->f2; goto _LL915;} else{ goto _LL916;}
_LL916: if((( struct _enum_struct*) _temp902)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL1001: _temp1000=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp902)->f1; goto _LL999; _LL999: _temp998=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp902)->f2; goto _LL997; _LL997: _temp996=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp902)->f3;
goto _LL917;} else{ goto _LL918;} _LL918: if((( struct _enum_struct*) _temp902)->tag
== Cyc_Absyn_Increment_e_tag){ _LL1005: _temp1004=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp902)->f1; goto _LL1003; _LL1003:
_temp1002=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp902)->f2; goto
_LL919;} else{ goto _LL920;} _LL920: if((( struct _enum_struct*) _temp902)->tag
== Cyc_Absyn_Conditional_e_tag){ _LL1011: _temp1010=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp902)->f1; goto _LL1009; _LL1009:
_temp1008=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp902)->f2; goto _LL1007; _LL1007: _temp1006=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp902)->f3; goto _LL921;} else{ goto
_LL922;} _LL922: if((( struct _enum_struct*) _temp902)->tag == Cyc_Absyn_SeqExp_e_tag){
_LL1015: _temp1014=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp902)->f1; goto _LL1013; _LL1013: _temp1012=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_SeqExp_e_struct*) _temp902)->f2; goto _LL923;} else{ goto
_LL924;} _LL924: if((( struct _enum_struct*) _temp902)->tag == Cyc_Absyn_UnknownCall_e_tag){
_LL1019: _temp1018=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp902)->f1; goto _LL1017; _LL1017: _temp1016=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp902)->f2; goto _LL925;} else{ goto
_LL926;} _LL926: if((( struct _enum_struct*) _temp902)->tag == Cyc_Absyn_FnCall_e_tag){
_LL1023: _temp1022=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp902)->f1; goto _LL1021; _LL1021: _temp1020=( struct Cyc_List_List*)((
struct Cyc_Absyn_FnCall_e_struct*) _temp902)->f2; goto _LL927;} else{ goto
_LL928;} _LL928: if((( struct _enum_struct*) _temp902)->tag == Cyc_Absyn_Throw_e_tag){
_LL1025: _temp1024=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*)
_temp902)->f1; goto _LL929;} else{ goto _LL930;} _LL930: if((( struct
_enum_struct*) _temp902)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL1027:
_temp1026=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp902)->f1; goto _LL931;} else{ goto _LL932;} _LL932: if((( struct
_enum_struct*) _temp902)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL1031:
_temp1030=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp902)->f1; goto _LL1029; _LL1029: _temp1028=( struct Cyc_List_List*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp902)->f2; goto _LL933;} else{ goto
_LL934;} _LL934: if((( struct _enum_struct*) _temp902)->tag == Cyc_Absyn_Cast_e_tag){
_LL1035: _temp1034=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp902)->f1;
goto _LL1033; _LL1033: _temp1032=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp902)->f2; goto _LL935;} else{ goto _LL936;} _LL936: if((( struct
_enum_struct*) _temp902)->tag == Cyc_Absyn_Address_e_tag){ _LL1037: _temp1036=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp902)->f1; goto
_LL937;} else{ goto _LL938;} _LL938: if((( struct _enum_struct*) _temp902)->tag
== Cyc_Absyn_Sizeof_e_tag){ _LL1039: _temp1038=( void*)(( struct Cyc_Absyn_Sizeof_e_struct*)
_temp902)->f1; goto _LL939;} else{ goto _LL940;} _LL940: if((( struct
_enum_struct*) _temp902)->tag == Cyc_Absyn_Deref_e_tag){ _LL1041: _temp1040=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp902)->f1; goto
_LL941;} else{ goto _LL942;} _LL942: if((( struct _enum_struct*) _temp902)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL1045: _temp1044=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp902)->f1; goto _LL1043; _LL1043:
_temp1042=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp902)->f2; goto _LL943;} else{ goto _LL944;} _LL944: if((( struct
_enum_struct*) _temp902)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL1049:
_temp1048=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp902)->f1; goto _LL1047; _LL1047: _temp1046=( struct _tagged_string*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp902)->f2; goto _LL945;} else{ goto
_LL946;} _LL946: if((( struct _enum_struct*) _temp902)->tag == Cyc_Absyn_Subscript_e_tag){
_LL1053: _temp1052=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp902)->f1; goto _LL1051; _LL1051: _temp1050=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp902)->f2; goto _LL947;} else{ goto
_LL948;} _LL948: if((( struct _enum_struct*) _temp902)->tag == Cyc_Absyn_Tuple_e_tag){
_LL1055: _temp1054=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp902)->f1; goto _LL949;} else{ goto _LL950;} _LL950: if((( struct
_enum_struct*) _temp902)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL1059:
_temp1058=( struct _tuple1*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp902)->f1;
goto _LL1057; _LL1057: _temp1056=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp902)->f2; goto _LL951;} else{ goto _LL952;} _LL952: if((( struct
_enum_struct*) _temp902)->tag == Cyc_Absyn_Array_e_tag){ _LL1063: _temp1062=(
int)(( struct Cyc_Absyn_Array_e_struct*) _temp902)->f1; if( _temp1062 == 1){
goto _LL1061;} else{ goto _LL954;} _LL1061: _temp1060=( struct Cyc_List_List*)((
struct Cyc_Absyn_Array_e_struct*) _temp902)->f2; goto _LL953;} else{ goto _LL954;}
_LL954: if((( struct _enum_struct*) _temp902)->tag == Cyc_Absyn_Array_e_tag){
_LL1067: _temp1066=( int)(( struct Cyc_Absyn_Array_e_struct*) _temp902)->f1;
goto _LL1065; _LL1065: _temp1064=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp902)->f2; goto _LL955;} else{ goto _LL956;} _LL956: if((( struct
_enum_struct*) _temp902)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL1073:
_temp1072=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp902)->f1; goto _LL1071; _LL1071: _temp1070=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Comprehension_e_struct*) _temp902)->f2; goto _LL1069; _LL1069:
_temp1068=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp902)->f3; goto _LL957;} else{ goto _LL958;} _LL958: if((( struct
_enum_struct*) _temp902)->tag == Cyc_Absyn_Struct_e_tag){ _LL1081: _temp1080=(
struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*) _temp902)->f1; goto
_LL1079; _LL1079: _temp1078=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp902)->f2; goto _LL1077; _LL1077: _temp1076=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_e_struct*) _temp902)->f3; goto _LL1075; _LL1075:
_temp1074=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp902)->f4; goto _LL959;} else{ goto _LL960;} _LL960: if((( struct
_enum_struct*) _temp902)->tag == Cyc_Absyn_Enum_e_tag){ _LL1091: _temp1090=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp902)->f1; goto
_LL1089; _LL1089: _temp1088=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp902)->f2; goto _LL1087; _LL1087: _temp1086=( struct Cyc_List_List*)((
struct Cyc_Absyn_Enum_e_struct*) _temp902)->f3; goto _LL1085; _LL1085: _temp1084=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp902)->f4;
goto _LL1083; _LL1083: _temp1082=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp902)->f5; goto _LL961;} else{ goto _LL962;} _LL962: if((( struct
_enum_struct*) _temp902)->tag == Cyc_Absyn_Xenum_e_tag){ _LL1099: _temp1098=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp902)->f1; goto
_LL1097; _LL1097: _temp1096=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp902)->f2; goto _LL1095; _LL1095: _temp1094=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp902)->f3; goto _LL1093; _LL1093:
_temp1092=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp902)->f4; goto _LL963;} else{ goto _LL964;} _LL964: if((( struct
_enum_struct*) _temp902)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL1103:
_temp1102=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp902)->f1; goto _LL1101; _LL1101: _temp1100=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp902)->f2; goto _LL965;} else{
goto _LL966;} _LL966: if((( struct _enum_struct*) _temp902)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL1105: _temp1104=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp902)->f1; goto _LL967;} else{ goto _LL968;} _LL968: if((( struct
_enum_struct*) _temp902)->tag == Cyc_Absyn_Codegen_e_tag){ _LL1107: _temp1106=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp902)->f1;
goto _LL969;} else{ goto _LL970;} _LL970: if((( struct _enum_struct*) _temp902)->tag
== Cyc_Absyn_Fill_e_tag){ _LL1109: _temp1108=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Fill_e_struct*) _temp902)->f1; goto _LL971;} else{ goto _LL903;}
_LL905: s= Cyc_Absynpp_cnst2doc( _temp972); goto _LL903; _LL907: s= Cyc_Absynpp_qvar2doc(
_temp978); if( _temp976->shadow > 0){ s= Cyc_Absynpp_cat2( s, Cyc_PP_text(
xprintf("%d", _temp976->shadow)));} goto _LL903; _LL909: s= Cyc_Absynpp_qvar2doc(
_temp984); if( _temp982->shadow > 0){ s= Cyc_Absynpp_cat2( s, Cyc_PP_text(
xprintf("%d", _temp982->shadow)));} goto _LL903; _LL911: s= Cyc_Absynpp_qvar2doc(
_temp988); goto _LL903; _LL913: s= Cyc_Absynpp_qvar2doc( _temp990); goto _LL903;
_LL915: s= Cyc_Absynpp_primapp2doc( myprec, _temp994, _temp992); goto _LL903;
_LL917: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec( myprec, _temp1000), Cyc_PP_text((
struct _tagged_string)({ char* _temp1110=( char*)" "; struct _tagged_string
_temp1111; _temp1111.curr= _temp1110; _temp1111.base= _temp1110; _temp1111.last_plus_one=
_temp1110 + 2; _temp1111;})), _temp998 == 0? Cyc_PP_text(( struct _tagged_string)({
char* _temp1112=( char*)""; struct _tagged_string _temp1113; _temp1113.curr=
_temp1112; _temp1113.base= _temp1112; _temp1113.last_plus_one= _temp1112 + 1;
_temp1113;})): Cyc_Absynpp_prim2doc(( void*) _temp998->v), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1114=( char*)"= "; struct _tagged_string _temp1115;
_temp1115.curr= _temp1114; _temp1115.base= _temp1114; _temp1115.last_plus_one=
_temp1114 + 3; _temp1115;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp996)); goto
_LL903; _LL919: { struct Cyc_PP_Doc* es= Cyc_Absynpp_exp2doc_prec( myprec,
_temp1004);{ void* _temp1116= _temp1002; _LL1118: if( _temp1116 == Cyc_Absyn_PreInc){
goto _LL1119;} else{ goto _LL1120;} _LL1120: if( _temp1116 == Cyc_Absyn_PreDec){
goto _LL1121;} else{ goto _LL1122;} _LL1122: if( _temp1116 == Cyc_Absyn_PostInc){
goto _LL1123;} else{ goto _LL1124;} _LL1124: if( _temp1116 == Cyc_Absyn_PostDec){
goto _LL1125;} else{ goto _LL1117;} _LL1119: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1126=( char*)"++"; struct _tagged_string
_temp1127; _temp1127.curr= _temp1126; _temp1127.base= _temp1126; _temp1127.last_plus_one=
_temp1126 + 3; _temp1127;})), es); goto _LL1117; _LL1121: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1128=( char*)"--"; struct
_tagged_string _temp1129; _temp1129.curr= _temp1128; _temp1129.base= _temp1128;
_temp1129.last_plus_one= _temp1128 + 3; _temp1129;})), es); goto _LL1117;
_LL1123: s= Cyc_Absynpp_cat2( es, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1130=( char*)"++"; struct _tagged_string _temp1131; _temp1131.curr=
_temp1130; _temp1131.base= _temp1130; _temp1131.last_plus_one= _temp1130 + 3;
_temp1131;}))); goto _LL1117; _LL1125: s= Cyc_Absynpp_cat2( es, Cyc_PP_text((
struct _tagged_string)({ char* _temp1132=( char*)"--"; struct _tagged_string
_temp1133; _temp1133.curr= _temp1132; _temp1133.base= _temp1132; _temp1133.last_plus_one=
_temp1132 + 3; _temp1133;}))); goto _LL1117; _LL1117:;} goto _LL903;} _LL921:{
struct _tuple9 _temp1135=({ struct _tuple9 _temp1134; _temp1134.f1=( void*)
_temp1008->r; _temp1134.f2=( void*) _temp1006->r; _temp1134;}); _LL1137: goto
_LL1138; _LL1138: s= Cyc_Absynpp_cat5( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1010), Cyc_PP_text(( struct _tagged_string)({ char* _temp1139=( char*)" ? ";
struct _tagged_string _temp1140; _temp1140.curr= _temp1139; _temp1140.base=
_temp1139; _temp1140.last_plus_one= _temp1139 + 4; _temp1140;})), Cyc_Absynpp_exp2doc_prec(
0, _temp1008), Cyc_PP_text(( struct _tagged_string)({ char* _temp1141=( char*)" : ";
struct _tagged_string _temp1142; _temp1142.curr= _temp1141; _temp1142.base=
_temp1141; _temp1142.last_plus_one= _temp1141 + 4; _temp1142;})), Cyc_Absynpp_exp2doc_prec(
myprec, _temp1006)); goto _LL1136; _LL1136:;} goto _LL903; _LL923: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_exp2doc_prec( myprec, _temp1014), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1143=( char*)", "; struct _tagged_string _temp1144;
_temp1144.curr= _temp1143; _temp1144.base= _temp1143; _temp1144.last_plus_one=
_temp1143 + 3; _temp1144;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1012));
goto _LL903; _LL925: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1018), Cyc_PP_text(( struct _tagged_string)({ char* _temp1145=( char*)"(";
struct _tagged_string _temp1146; _temp1146.curr= _temp1145; _temp1146.base=
_temp1145; _temp1146.last_plus_one= _temp1145 + 2; _temp1146;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1016), Cyc_PP_text(( struct _tagged_string)({ char* _temp1147=( char*)")";
struct _tagged_string _temp1148; _temp1148.curr= _temp1147; _temp1148.base=
_temp1147; _temp1148.last_plus_one= _temp1147 + 2; _temp1148;}))); goto _LL903;
_LL927: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec( myprec, _temp1022), Cyc_PP_text((
struct _tagged_string)({ char* _temp1149=( char*)"("; struct _tagged_string
_temp1150; _temp1150.curr= _temp1149; _temp1150.base= _temp1149; _temp1150.last_plus_one=
_temp1149 + 2; _temp1150;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1020), Cyc_PP_text((
struct _tagged_string)({ char* _temp1151=( char*)")"; struct _tagged_string
_temp1152; _temp1152.curr= _temp1151; _temp1152.base= _temp1151; _temp1152.last_plus_one=
_temp1151 + 2; _temp1152;}))); goto _LL903; _LL929: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1153=( char*)"throw "; struct _tagged_string
_temp1154; _temp1154.curr= _temp1153; _temp1154.base= _temp1153; _temp1154.last_plus_one=
_temp1153 + 7; _temp1154;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1024));
goto _LL903; _LL931: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1026); goto
_LL903; _LL933: s= Cyc_Absynpp_exp2doc_prec( inprec, _temp1030); goto _LL903;
_LL935: s= Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1155=( char*)"("; struct _tagged_string _temp1156; _temp1156.curr=
_temp1155; _temp1156.base= _temp1155; _temp1156.last_plus_one= _temp1155 + 2;
_temp1156;})), Cyc_Absynpp_typ2doc( _temp1034), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1157=( char*)")"; struct _tagged_string _temp1158;
_temp1158.curr= _temp1157; _temp1158.base= _temp1157; _temp1158.last_plus_one=
_temp1157 + 2; _temp1158;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1032));
goto _LL903; _LL937: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({
char* _temp1159=( char*)"&"; struct _tagged_string _temp1160; _temp1160.curr=
_temp1159; _temp1160.base= _temp1159; _temp1160.last_plus_one= _temp1159 + 2;
_temp1160;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1036)); goto _LL903;
_LL939: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1161=( char*)"sizeof("; struct _tagged_string _temp1162; _temp1162.curr=
_temp1161; _temp1162.base= _temp1161; _temp1162.last_plus_one= _temp1161 + 8;
_temp1162;})), Cyc_Absynpp_typ2doc( _temp1038), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1163=( char*)")"; struct _tagged_string _temp1164;
_temp1164.curr= _temp1163; _temp1164.base= _temp1163; _temp1164.last_plus_one=
_temp1163 + 2; _temp1164;}))); goto _LL903; _LL941: s= Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1165=( char*)"*"; struct _tagged_string
_temp1166; _temp1166.curr= _temp1165; _temp1166.base= _temp1165; _temp1166.last_plus_one=
_temp1165 + 2; _temp1166;})), Cyc_Absynpp_exp2doc_prec( myprec, _temp1040));
goto _LL903; _LL943: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec( myprec,
_temp1044), Cyc_PP_text(( struct _tagged_string)({ char* _temp1167=( char*)".";
struct _tagged_string _temp1168; _temp1168.curr= _temp1167; _temp1168.base=
_temp1167; _temp1168.last_plus_one= _temp1167 + 2; _temp1168;})), Cyc_Absynpp_textptr(
_temp1042)); goto _LL903; _LL945: s= Cyc_Absynpp_cat3( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1048), Cyc_PP_text(( struct _tagged_string)({ char* _temp1169=(
char*)"->"; struct _tagged_string _temp1170; _temp1170.curr= _temp1169;
_temp1170.base= _temp1169; _temp1170.last_plus_one= _temp1169 + 3; _temp1170;})),
Cyc_Absynpp_textptr( _temp1046)); goto _LL903; _LL947: s= Cyc_Absynpp_cat4( Cyc_Absynpp_exp2doc_prec(
myprec, _temp1052), Cyc_PP_text(( struct _tagged_string)({ char* _temp1171=(
char*)"["; struct _tagged_string _temp1172; _temp1172.curr= _temp1171; _temp1172.base=
_temp1171; _temp1172.last_plus_one= _temp1171 + 2; _temp1172;})), Cyc_Absynpp_exp2doc(
_temp1050), Cyc_PP_text(( struct _tagged_string)({ char* _temp1173=( char*)"]";
struct _tagged_string _temp1174; _temp1174.curr= _temp1173; _temp1174.base=
_temp1173; _temp1174.last_plus_one= _temp1173 + 2; _temp1174;}))); goto _LL903;
_LL949: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1175=( char*)"$("; struct _tagged_string _temp1176; _temp1176.curr=
_temp1175; _temp1176.base= _temp1175; _temp1176.last_plus_one= _temp1175 + 3;
_temp1176;})), Cyc_Absynpp_exps2doc_prec( 20, _temp1054), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1177=( char*)")"; struct _tagged_string _temp1178;
_temp1178.curr= _temp1177; _temp1178.base= _temp1177; _temp1178.last_plus_one=
_temp1177 + 2; _temp1178;}))); goto _LL903; _LL951: s= Cyc_Absynpp_cat4( Cyc_PP_text((
struct _tagged_string)({ char* _temp1179=( char*)"("; struct _tagged_string
_temp1180; _temp1180.curr= _temp1179; _temp1180.base= _temp1179; _temp1180.last_plus_one=
_temp1179 + 2; _temp1180;})), Cyc_Absynpp_typ2doc((* _temp1058).f3), Cyc_PP_text((
struct _tagged_string)({ char* _temp1181=( char*)")"; struct _tagged_string
_temp1182; _temp1182.curr= _temp1181; _temp1182.base= _temp1181; _temp1182.last_plus_one=
_temp1181 + 2; _temp1182;})), Cyc_Absynpp_group(( struct _tagged_string)({ char*
_temp1183=( char*)"{"; struct _tagged_string _temp1184; _temp1184.curr=
_temp1183; _temp1184.base= _temp1183; _temp1184.last_plus_one= _temp1183 + 2;
_temp1184;}),( struct _tagged_string)({ char* _temp1185=( char*)"}"; struct
_tagged_string _temp1186; _temp1186.curr= _temp1185; _temp1186.base= _temp1185;
_temp1186.last_plus_one= _temp1185 + 2; _temp1186;}),( struct _tagged_string)({
char* _temp1187=( char*)","; struct _tagged_string _temp1188; _temp1188.curr=
_temp1187; _temp1188.base= _temp1187; _temp1188.last_plus_one= _temp1187 + 2;
_temp1188;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1056))); goto _LL903; _LL953: s= Cyc_Absynpp_group(( struct _tagged_string)({
char* _temp1189=( char*)"new {"; struct _tagged_string _temp1190; _temp1190.curr=
_temp1189; _temp1190.base= _temp1189; _temp1190.last_plus_one= _temp1189 + 6;
_temp1190;}),( struct _tagged_string)({ char* _temp1191=( char*)"}"; struct
_tagged_string _temp1192; _temp1192.curr= _temp1191; _temp1192.base= _temp1191;
_temp1192.last_plus_one= _temp1191 + 2; _temp1192;}),( struct _tagged_string)({
char* _temp1193=( char*)","; struct _tagged_string _temp1194; _temp1194.curr=
_temp1193; _temp1194.base= _temp1193; _temp1194.last_plus_one= _temp1193 + 2;
_temp1194;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1060)); goto _LL903; _LL955: s= Cyc_Absynpp_group(( struct _tagged_string)({
char* _temp1195=( char*)"{"; struct _tagged_string _temp1196; _temp1196.curr=
_temp1195; _temp1196.base= _temp1195; _temp1196.last_plus_one= _temp1195 + 2;
_temp1196;}),( struct _tagged_string)({ char* _temp1197=( char*)"}"; struct
_tagged_string _temp1198; _temp1198.curr= _temp1197; _temp1198.base= _temp1197;
_temp1198.last_plus_one= _temp1197 + 2; _temp1198;}),( struct _tagged_string)({
char* _temp1199=( char*)","; struct _tagged_string _temp1200; _temp1200.curr=
_temp1199; _temp1200.base= _temp1199; _temp1200.last_plus_one= _temp1199 + 2;
_temp1200;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1064)); goto _LL903; _LL957: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1201=( char*)"new {for "; struct _tagged_string
_temp1202; _temp1202.curr= _temp1201; _temp1202.base= _temp1201; _temp1202.last_plus_one=
_temp1201 + 10; _temp1202;})), Cyc_PP_text(*(* _temp1072->name).f2), Cyc_PP_text((
struct _tagged_string)({ char* _temp1203=( char*)" < "; struct _tagged_string
_temp1204; _temp1204.curr= _temp1203; _temp1204.base= _temp1203; _temp1204.last_plus_one=
_temp1203 + 4; _temp1204;})), Cyc_Absynpp_exp2doc( _temp1070), Cyc_PP_text((
struct _tagged_string)({ char* _temp1205=( char*)" : "; struct _tagged_string
_temp1206; _temp1206.curr= _temp1205; _temp1206.base= _temp1205; _temp1206.last_plus_one=
_temp1205 + 4; _temp1206;})), Cyc_Absynpp_exp2doc( _temp1068), Cyc_PP_text((
struct _tagged_string)({ char* _temp1207=( char*)"}"; struct _tagged_string
_temp1208; _temp1208.curr= _temp1207; _temp1208.base= _temp1207; _temp1208.last_plus_one=
_temp1207 + 2; _temp1208;}))); goto _LL903; _LL959: s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc(
_temp1080), Cyc_Absynpp_group(( struct _tagged_string)({ char* _temp1209=( char*)"{";
struct _tagged_string _temp1210; _temp1210.curr= _temp1209; _temp1210.base=
_temp1209; _temp1210.last_plus_one= _temp1209 + 2; _temp1210;}),( struct
_tagged_string)({ char* _temp1211=( char*)"}"; struct _tagged_string _temp1212;
_temp1212.curr= _temp1211; _temp1212.base= _temp1211; _temp1212.last_plus_one=
_temp1211 + 2; _temp1212;}),( struct _tagged_string)({ char* _temp1213=( char*)",";
struct _tagged_string _temp1214; _temp1214.curr= _temp1213; _temp1214.base=
_temp1213; _temp1214.last_plus_one= _temp1213 + 2; _temp1214;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_de2doc, _temp1076))); goto _LL903; _LL961: _temp1096= _temp1086;
_temp1092= _temp1082; goto _LL963; _LL963: if( _temp1096 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1092->name);} else{ s= Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp1092->name),
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1215=( char*)"(";
struct _tagged_string _temp1216; _temp1216.curr= _temp1215; _temp1216.base=
_temp1215; _temp1216.last_plus_one= _temp1215 + 2; _temp1216;}),( struct
_tagged_string)({ char* _temp1217=( char*)")"; struct _tagged_string _temp1218;
_temp1218.curr= _temp1217; _temp1218.base= _temp1217; _temp1218.last_plus_one=
_temp1217 + 2; _temp1218;}),( struct _tagged_string)({ char* _temp1219=( char*)",";
struct _tagged_string _temp1220; _temp1220.curr= _temp1219; _temp1220.base=
_temp1219; _temp1220.last_plus_one= _temp1219 + 2; _temp1220;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_exp2doc, _temp1096)));} goto _LL903; _LL965: s= Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1221=( char*)"{"; struct _tagged_string
_temp1222; _temp1222.curr= _temp1221; _temp1222.base= _temp1221; _temp1222.last_plus_one=
_temp1221 + 2; _temp1222;}),( struct _tagged_string)({ char* _temp1223=( char*)"}";
struct _tagged_string _temp1224; _temp1224.curr= _temp1223; _temp1224.base=
_temp1223; _temp1224.last_plus_one= _temp1223 + 2; _temp1224;}),( struct
_tagged_string)({ char* _temp1225=( char*)","; struct _tagged_string _temp1226;
_temp1226.curr= _temp1225; _temp1226.base= _temp1225; _temp1226.last_plus_one=
_temp1225 + 2; _temp1226;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple6*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_de2doc,
_temp1100)); goto _LL903; _LL967: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1227=( char*)"({ "; struct _tagged_string _temp1228;
_temp1228.curr= _temp1227; _temp1228.base= _temp1227; _temp1228.last_plus_one=
_temp1227 + 4; _temp1228;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1104)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1229=( char*)" })"; struct
_tagged_string _temp1230; _temp1230.curr= _temp1229; _temp1230.base= _temp1229;
_temp1230.last_plus_one= _temp1229 + 4; _temp1230;}))); goto _LL903; _LL969: s=
Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1231=( char*)"codegen(";
struct _tagged_string _temp1232; _temp1232.curr= _temp1231; _temp1232.base=
_temp1231; _temp1232.last_plus_one= _temp1231 + 9; _temp1232;})), Cyc_PP_nest( 2,
Cyc_Absynpp_decl2doc(({ struct Cyc_Absyn_Decl* _temp1233=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp1233->r=( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp1234=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp1234->tag= Cyc_Absyn_Fn_d_tag; _temp1234->f1= _temp1106;( void*) _temp1234;});
_temp1233->loc= e->loc; _temp1233;}))), Cyc_PP_text(( struct _tagged_string)({
char* _temp1235=( char*)")"; struct _tagged_string _temp1236; _temp1236.curr=
_temp1235; _temp1236.base= _temp1235; _temp1236.last_plus_one= _temp1235 + 2;
_temp1236;}))); goto _LL903; _LL971: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1237=( char*)"fill("; struct _tagged_string
_temp1238; _temp1238.curr= _temp1237; _temp1238.base= _temp1237; _temp1238.last_plus_one=
_temp1237 + 6; _temp1238;})), Cyc_PP_nest( 2, Cyc_Absynpp_exp2doc( _temp1108)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1239=( char*)")"; struct
_tagged_string _temp1240; _temp1240.curr= _temp1239; _temp1240.base= _temp1239;
_temp1240.last_plus_one= _temp1239 + 2; _temp1240;}))); goto _LL903; _LL903:;}
if( inprec >= myprec){ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({
char* _temp1241=( char*)"("; struct _tagged_string _temp1242; _temp1242.curr=
_temp1241; _temp1242.base= _temp1241; _temp1242.last_plus_one= _temp1241 + 2;
_temp1242;})), s, Cyc_PP_text(( struct _tagged_string)({ char* _temp1243=( char*)")";
struct _tagged_string _temp1244; _temp1244.curr= _temp1243; _temp1244.base=
_temp1243; _temp1244.last_plus_one= _temp1243 + 2; _temp1244;})));} return s;}
struct Cyc_PP_Doc* Cyc_Absynpp_designator2doc( void* d){ void* _temp1245= d;
struct Cyc_Absyn_Exp* _temp1251; struct _tagged_string* _temp1253; _LL1247: if(((
struct _enum_struct*) _temp1245)->tag == Cyc_Absyn_ArrayElement_tag){ _LL1252:
_temp1251=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1245)->f1; goto _LL1248;} else{ goto _LL1249;} _LL1249: if((( struct
_enum_struct*) _temp1245)->tag == Cyc_Absyn_FieldName_tag){ _LL1254: _temp1253=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp1245)->f1;
goto _LL1250;} else{ goto _LL1246;} _LL1248: return Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1255=( char*)".["; struct _tagged_string
_temp1256; _temp1256.curr= _temp1255; _temp1256.base= _temp1255; _temp1256.last_plus_one=
_temp1255 + 3; _temp1256;})), Cyc_Absynpp_exp2doc( _temp1251), Cyc_PP_text((
struct _tagged_string)({ char* _temp1257=( char*)"]"; struct _tagged_string
_temp1258; _temp1258.curr= _temp1257; _temp1258.base= _temp1257; _temp1258.last_plus_one=
_temp1257 + 2; _temp1258;}))); _LL1250: return Cyc_Absynpp_cat2( Cyc_PP_text((
struct _tagged_string)({ char* _temp1259=( char*)"."; struct _tagged_string
_temp1260; _temp1260.curr= _temp1259; _temp1260.base= _temp1259; _temp1260.last_plus_one=
_temp1259 + 2; _temp1260;})), Cyc_Absynpp_textptr( _temp1253)); _LL1246:;}
struct Cyc_PP_Doc* Cyc_Absynpp_de2doc( struct _tuple6* de){ if((* de).f1 == 0){
return Cyc_Absynpp_exp2doc((* de).f2);} else{ return Cyc_Absynpp_cat2( Cyc_Absynpp_egroup((
struct _tagged_string)({ char* _temp1261=( char*)""; struct _tagged_string
_temp1262; _temp1262.curr= _temp1261; _temp1262.base= _temp1261; _temp1262.last_plus_one=
_temp1261 + 1; _temp1262;}),( struct _tagged_string)({ char* _temp1263=( char*)"=";
struct _tagged_string _temp1264; _temp1264.curr= _temp1263; _temp1264.base=
_temp1263; _temp1264.last_plus_one= _temp1263 + 2; _temp1264;}),( struct
_tagged_string)({ char* _temp1265=( char*)"="; struct _tagged_string _temp1266;
_temp1266.curr= _temp1265; _temp1266.base= _temp1265; _temp1266.last_plus_one=
_temp1265 + 2; _temp1266;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
de).f1)), Cyc_Absynpp_exp2doc((* de).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_exps2doc_prec(
int inprec, struct Cyc_List_List* es){ return Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1267=( char*)""; struct _tagged_string _temp1268;
_temp1268.curr= _temp1267; _temp1268.base= _temp1267; _temp1268.last_plus_one=
_temp1267 + 1; _temp1268;}),( struct _tagged_string)({ char* _temp1269=( char*)"";
struct _tagged_string _temp1270; _temp1270.curr= _temp1269; _temp1270.base=
_temp1269; _temp1270.last_plus_one= _temp1269 + 1; _temp1270;}),( struct
_tagged_string)({ char* _temp1271=( char*)","; struct _tagged_string _temp1272;
_temp1272.curr= _temp1271; _temp1272.base= _temp1271; _temp1272.last_plus_one=
_temp1271 + 2; _temp1272;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es));} struct Cyc_PP_Doc* Cyc_Absynpp_cnst2doc(
void* c){ void* _temp1273= c; char _temp1293; void* _temp1295; short _temp1297;
void* _temp1299; int _temp1301; void* _temp1303; int _temp1305; void* _temp1307;
long long _temp1309; void* _temp1311; struct _tagged_string _temp1313; struct
_tagged_string _temp1315; int _temp1317; struct _tagged_string _temp1319; int
_temp1321; _LL1275: if(( unsigned int) _temp1273 > 1u?(( struct _enum_struct*)
_temp1273)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL1296: _temp1295=( void*)((
struct Cyc_Absyn_Char_c_struct*) _temp1273)->f1; goto _LL1294; _LL1294:
_temp1293=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp1273)->f2; goto
_LL1276;} else{ goto _LL1277;} _LL1277: if(( unsigned int) _temp1273 > 1u?((
struct _enum_struct*) _temp1273)->tag == Cyc_Absyn_Short_c_tag: 0){ _LL1300:
_temp1299=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp1273)->f1; goto
_LL1298; _LL1298: _temp1297=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp1273)->f2; goto _LL1278;} else{ goto _LL1279;} _LL1279: if(( unsigned int)
_temp1273 > 1u?(( struct _enum_struct*) _temp1273)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL1304: _temp1303=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1273)->f1;
if( _temp1303 == Cyc_Absyn_Signed){ goto _LL1302;} else{ goto _LL1281;} _LL1302:
_temp1301=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp1273)->f2; goto _LL1280;}
else{ goto _LL1281;} _LL1281: if(( unsigned int) _temp1273 > 1u?(( struct
_enum_struct*) _temp1273)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL1308: _temp1307=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp1273)->f1; if( _temp1307 == Cyc_Absyn_Unsigned){
goto _LL1306;} else{ goto _LL1283;} _LL1306: _temp1305=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp1273)->f2; goto _LL1282;} else{ goto _LL1283;} _LL1283: if(( unsigned int)
_temp1273 > 1u?(( struct _enum_struct*) _temp1273)->tag == Cyc_Absyn_LongLong_c_tag:
0){ _LL1312: _temp1311=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp1273)->f1;
goto _LL1310; _LL1310: _temp1309=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp1273)->f2; goto _LL1284;} else{ goto _LL1285;} _LL1285: if(( unsigned int)
_temp1273 > 1u?(( struct _enum_struct*) _temp1273)->tag == Cyc_Absyn_Float_c_tag:
0){ _LL1314: _temp1313=( struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*)
_temp1273)->f1; goto _LL1286;} else{ goto _LL1287;} _LL1287: if( _temp1273 ==
Cyc_Absyn_Null_c){ goto _LL1288;} else{ goto _LL1289;} _LL1289: if((
unsigned int) _temp1273 > 1u?(( struct _enum_struct*) _temp1273)->tag == Cyc_Absyn_String_c_tag:
0){ _LL1318: _temp1317=( int)(( struct Cyc_Absyn_String_c_struct*) _temp1273)->f1;
if( _temp1317 == 1){ goto _LL1316;} else{ goto _LL1291;} _LL1316: _temp1315=(
struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp1273)->f2; goto
_LL1290;} else{ goto _LL1291;} _LL1291: if(( unsigned int) _temp1273 > 1u?((
struct _enum_struct*) _temp1273)->tag == Cyc_Absyn_String_c_tag: 0){ _LL1322:
_temp1321=( int)(( struct Cyc_Absyn_String_c_struct*) _temp1273)->f1; goto
_LL1320; _LL1320: _temp1319=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp1273)->f2; goto _LL1292;} else{ goto _LL1274;} _LL1276: return Cyc_PP_text(({
struct _tagged_string _temp1323= Cyc_Absynpp_char_escape( _temp1293); xprintf("'%.*s'",
_temp1323.last_plus_one - _temp1323.curr, _temp1323.curr);})); _LL1278: return
Cyc_PP_text( xprintf("(short)%d",( int) _temp1297)); _LL1280: return Cyc_PP_text(
Cyc_Core_string_of_int( _temp1301)); _LL1282: return Cyc_PP_text( xprintf("%u",(
unsigned int) _temp1305)); _LL1284: return Cyc_PP_text(( struct _tagged_string)({
char* _temp1324=( char*)"<<FIX LONG LONG CONSTANT>>"; struct _tagged_string
_temp1325; _temp1325.curr= _temp1324; _temp1325.base= _temp1324; _temp1325.last_plus_one=
_temp1324 + 27; _temp1325;})); _LL1286: return Cyc_PP_text( _temp1313); _LL1288:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp1326=( char*)"null";
struct _tagged_string _temp1327; _temp1327.curr= _temp1326; _temp1327.base=
_temp1326; _temp1327.last_plus_one= _temp1326 + 5; _temp1327;})); _LL1290:
return Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char* _temp1328=(
char*)"new \""; struct _tagged_string _temp1329; _temp1329.curr= _temp1328;
_temp1329.base= _temp1328; _temp1329.last_plus_one= _temp1328 + 6; _temp1329;})),
Cyc_PP_text( Cyc_Absynpp_string_escape( _temp1315)), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1330=( char*)"\""; struct _tagged_string _temp1331;
_temp1331.curr= _temp1330; _temp1331.base= _temp1330; _temp1331.last_plus_one=
_temp1330 + 2; _temp1331;}))); _LL1292: return Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1332=( char*)"\""; struct _tagged_string
_temp1333; _temp1333.curr= _temp1332; _temp1333.base= _temp1332; _temp1333.last_plus_one=
_temp1332 + 2; _temp1333;})), Cyc_PP_text( Cyc_Absynpp_string_escape( _temp1319)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1334=( char*)"\""; struct
_tagged_string _temp1335; _temp1335.curr= _temp1334; _temp1335.base= _temp1334;
_temp1335.last_plus_one= _temp1334 + 2; _temp1335;}))); _LL1274:;} struct Cyc_PP_Doc*
Cyc_Absynpp_primapp2doc( int inprec, void* p, struct Cyc_List_List* es){ struct
Cyc_PP_Doc* ps= Cyc_Absynpp_prim2doc( p); if( p == Cyc_Absyn_Size){ if( es == 0?
1: es->tl != 0){( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1336=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1336=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=({
struct _tagged_string _temp1337= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc Size: %.*s with bad args",
_temp1337.last_plus_one - _temp1337.curr, _temp1337.curr);})};( struct
_xenum_struct*) _temp1336;}));}{ struct Cyc_PP_Doc* d= Cyc_Absynpp_exp2doc_prec(
inprec,( struct Cyc_Absyn_Exp*) es->hd); return Cyc_Absynpp_cat2( d, Cyc_PP_text((
struct _tagged_string)({ char* _temp1338=( char*)".size"; struct _tagged_string
_temp1339; _temp1339.curr= _temp1338; _temp1339.base= _temp1338; _temp1339.last_plus_one=
_temp1338 + 6; _temp1339;})));}} else{ if( ! Cyc_Absyn_is_format_prim( p)){
struct Cyc_List_List* ds=(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Absynpp_exp2doc_prec, inprec, es); if( ds == 0){( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1340=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1340=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=({
struct _tagged_string _temp1341= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with no args",
_temp1341.last_plus_one - _temp1341.curr, _temp1341.curr);})};( struct
_xenum_struct*) _temp1340;}));} else{ if( ds->tl == 0){ return Cyc_Absynpp_cat3(
ps, Cyc_PP_text(( struct _tagged_string)({ char* _temp1342=( char*)" "; struct
_tagged_string _temp1343; _temp1343.curr= _temp1342; _temp1343.base= _temp1342;
_temp1343.last_plus_one= _temp1342 + 2; _temp1343;})),( struct Cyc_PP_Doc*) ds->hd);}
else{ if(( ds->tl)->tl != 0){( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1344=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1344=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=({
struct _tagged_string _temp1345= Cyc_PP_string_of_doc( ps, 72); xprintf("Absynpp::primapp2doc: %.*s with more than 2 args",
_temp1345.last_plus_one - _temp1345.curr, _temp1345.curr);})};( struct
_xenum_struct*) _temp1344;}));} else{ return Cyc_Absynpp_cat5(( struct Cyc_PP_Doc*)
ds->hd, Cyc_PP_text(( struct _tagged_string)({ char* _temp1346=( char*)" ";
struct _tagged_string _temp1347; _temp1347.curr= _temp1346; _temp1347.base=
_temp1346; _temp1347.last_plus_one= _temp1346 + 2; _temp1347;})), ps, Cyc_PP_text((
struct _tagged_string)({ char* _temp1348=( char*)" "; struct _tagged_string
_temp1349; _temp1349.curr= _temp1348; _temp1349.base= _temp1348; _temp1349.last_plus_one=
_temp1348 + 2; _temp1349;})),( struct Cyc_PP_Doc*)( ds->tl)->hd);}}}} else{
return Cyc_Absynpp_cat4( ps, Cyc_PP_text(( struct _tagged_string)({ char*
_temp1350=( char*)"("; struct _tagged_string _temp1351; _temp1351.curr=
_temp1350; _temp1351.base= _temp1350; _temp1351.last_plus_one= _temp1350 + 2;
_temp1351;})), Cyc_Absynpp_exps2doc_prec( 20, es), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1352=( char*)")"; struct _tagged_string _temp1353;
_temp1353.curr= _temp1352; _temp1353.base= _temp1352; _temp1353.last_plus_one=
_temp1352 + 2; _temp1353;})));}}} struct _tagged_string Cyc_Absynpp_prim2str(
void* p){ void* _temp1354= p; _LL1356: if( _temp1354 == Cyc_Absyn_Plus){ goto
_LL1357;} else{ goto _LL1358;} _LL1358: if( _temp1354 == Cyc_Absyn_Times){ goto
_LL1359;} else{ goto _LL1360;} _LL1360: if( _temp1354 == Cyc_Absyn_Minus){ goto
_LL1361;} else{ goto _LL1362;} _LL1362: if( _temp1354 == Cyc_Absyn_Div){ goto
_LL1363;} else{ goto _LL1364;} _LL1364: if( _temp1354 == Cyc_Absyn_Mod){ goto
_LL1365;} else{ goto _LL1366;} _LL1366: if( _temp1354 == Cyc_Absyn_Eq){ goto
_LL1367;} else{ goto _LL1368;} _LL1368: if( _temp1354 == Cyc_Absyn_Neq){ goto
_LL1369;} else{ goto _LL1370;} _LL1370: if( _temp1354 == Cyc_Absyn_Gt){ goto
_LL1371;} else{ goto _LL1372;} _LL1372: if( _temp1354 == Cyc_Absyn_Lt){ goto
_LL1373;} else{ goto _LL1374;} _LL1374: if( _temp1354 == Cyc_Absyn_Gte){ goto
_LL1375;} else{ goto _LL1376;} _LL1376: if( _temp1354 == Cyc_Absyn_Lte){ goto
_LL1377;} else{ goto _LL1378;} _LL1378: if( _temp1354 == Cyc_Absyn_Not){ goto
_LL1379;} else{ goto _LL1380;} _LL1380: if( _temp1354 == Cyc_Absyn_Bitnot){ goto
_LL1381;} else{ goto _LL1382;} _LL1382: if( _temp1354 == Cyc_Absyn_Bitand){ goto
_LL1383;} else{ goto _LL1384;} _LL1384: if( _temp1354 == Cyc_Absyn_Bitor){ goto
_LL1385;} else{ goto _LL1386;} _LL1386: if( _temp1354 == Cyc_Absyn_Bitxor){ goto
_LL1387;} else{ goto _LL1388;} _LL1388: if( _temp1354 == Cyc_Absyn_Bitlshift){
goto _LL1389;} else{ goto _LL1390;} _LL1390: if( _temp1354 == Cyc_Absyn_Bitlrshift){
goto _LL1391;} else{ goto _LL1392;} _LL1392: if( _temp1354 == Cyc_Absyn_Bitarshift){
goto _LL1393;} else{ goto _LL1394;} _LL1394: if( _temp1354 == Cyc_Absyn_Size){
goto _LL1395;} else{ goto _LL1396;} _LL1396: if( _temp1354 == Cyc_Absyn_Printf){
goto _LL1397;} else{ goto _LL1398;} _LL1398: if( _temp1354 == Cyc_Absyn_Fprintf){
goto _LL1399;} else{ goto _LL1400;} _LL1400: if( _temp1354 == Cyc_Absyn_Xprintf){
goto _LL1401;} else{ goto _LL1402;} _LL1402: if( _temp1354 == Cyc_Absyn_Scanf){
goto _LL1403;} else{ goto _LL1404;} _LL1404: if( _temp1354 == Cyc_Absyn_Fscanf){
goto _LL1405;} else{ goto _LL1406;} _LL1406: if( _temp1354 == Cyc_Absyn_Sscanf){
goto _LL1407;} else{ goto _LL1355;} _LL1357: return( struct _tagged_string)({
char* _temp1408=( char*)"+"; struct _tagged_string _temp1409; _temp1409.curr=
_temp1408; _temp1409.base= _temp1408; _temp1409.last_plus_one= _temp1408 + 2;
_temp1409;}); _LL1359: return( struct _tagged_string)({ char* _temp1410=( char*)"*";
struct _tagged_string _temp1411; _temp1411.curr= _temp1410; _temp1411.base=
_temp1410; _temp1411.last_plus_one= _temp1410 + 2; _temp1411;}); _LL1361: return(
struct _tagged_string)({ char* _temp1412=( char*)"-"; struct _tagged_string
_temp1413; _temp1413.curr= _temp1412; _temp1413.base= _temp1412; _temp1413.last_plus_one=
_temp1412 + 2; _temp1413;}); _LL1363: return( struct _tagged_string)({ char*
_temp1414=( char*)"/"; struct _tagged_string _temp1415; _temp1415.curr=
_temp1414; _temp1415.base= _temp1414; _temp1415.last_plus_one= _temp1414 + 2;
_temp1415;}); _LL1365: return( struct _tagged_string)({ char* _temp1416=( char*)"%";
struct _tagged_string _temp1417; _temp1417.curr= _temp1416; _temp1417.base=
_temp1416; _temp1417.last_plus_one= _temp1416 + 2; _temp1417;}); _LL1367: return(
struct _tagged_string)({ char* _temp1418=( char*)"=="; struct _tagged_string
_temp1419; _temp1419.curr= _temp1418; _temp1419.base= _temp1418; _temp1419.last_plus_one=
_temp1418 + 3; _temp1419;}); _LL1369: return( struct _tagged_string)({ char*
_temp1420=( char*)"!="; struct _tagged_string _temp1421; _temp1421.curr=
_temp1420; _temp1421.base= _temp1420; _temp1421.last_plus_one= _temp1420 + 3;
_temp1421;}); _LL1371: return( struct _tagged_string)({ char* _temp1422=( char*)">";
struct _tagged_string _temp1423; _temp1423.curr= _temp1422; _temp1423.base=
_temp1422; _temp1423.last_plus_one= _temp1422 + 2; _temp1423;}); _LL1373: return(
struct _tagged_string)({ char* _temp1424=( char*)"<"; struct _tagged_string
_temp1425; _temp1425.curr= _temp1424; _temp1425.base= _temp1424; _temp1425.last_plus_one=
_temp1424 + 2; _temp1425;}); _LL1375: return( struct _tagged_string)({ char*
_temp1426=( char*)">="; struct _tagged_string _temp1427; _temp1427.curr=
_temp1426; _temp1427.base= _temp1426; _temp1427.last_plus_one= _temp1426 + 3;
_temp1427;}); _LL1377: return( struct _tagged_string)({ char* _temp1428=( char*)"<=";
struct _tagged_string _temp1429; _temp1429.curr= _temp1428; _temp1429.base=
_temp1428; _temp1429.last_plus_one= _temp1428 + 3; _temp1429;}); _LL1379: return(
struct _tagged_string)({ char* _temp1430=( char*)"!"; struct _tagged_string
_temp1431; _temp1431.curr= _temp1430; _temp1431.base= _temp1430; _temp1431.last_plus_one=
_temp1430 + 2; _temp1431;}); _LL1381: return( struct _tagged_string)({ char*
_temp1432=( char*)"~"; struct _tagged_string _temp1433; _temp1433.curr=
_temp1432; _temp1433.base= _temp1432; _temp1433.last_plus_one= _temp1432 + 2;
_temp1433;}); _LL1383: return( struct _tagged_string)({ char* _temp1434=( char*)"&";
struct _tagged_string _temp1435; _temp1435.curr= _temp1434; _temp1435.base=
_temp1434; _temp1435.last_plus_one= _temp1434 + 2; _temp1435;}); _LL1385: return(
struct _tagged_string)({ char* _temp1436=( char*)"|"; struct _tagged_string
_temp1437; _temp1437.curr= _temp1436; _temp1437.base= _temp1436; _temp1437.last_plus_one=
_temp1436 + 2; _temp1437;}); _LL1387: return( struct _tagged_string)({ char*
_temp1438=( char*)"^"; struct _tagged_string _temp1439; _temp1439.curr=
_temp1438; _temp1439.base= _temp1438; _temp1439.last_plus_one= _temp1438 + 2;
_temp1439;}); _LL1389: return( struct _tagged_string)({ char* _temp1440=( char*)"<<";
struct _tagged_string _temp1441; _temp1441.curr= _temp1440; _temp1441.base=
_temp1440; _temp1441.last_plus_one= _temp1440 + 3; _temp1441;}); _LL1391: return(
struct _tagged_string)({ char* _temp1442=( char*)">>"; struct _tagged_string
_temp1443; _temp1443.curr= _temp1442; _temp1443.base= _temp1442; _temp1443.last_plus_one=
_temp1442 + 3; _temp1443;}); _LL1393: return( struct _tagged_string)({ char*
_temp1444=( char*)">>>"; struct _tagged_string _temp1445; _temp1445.curr=
_temp1444; _temp1445.base= _temp1444; _temp1445.last_plus_one= _temp1444 + 4;
_temp1445;}); _LL1395: return( struct _tagged_string)({ char* _temp1446=( char*)"size";
struct _tagged_string _temp1447; _temp1447.curr= _temp1446; _temp1447.base=
_temp1446; _temp1447.last_plus_one= _temp1446 + 5; _temp1447;}); _LL1397: return(
struct _tagged_string)({ char* _temp1448=( char*)"printf"; struct _tagged_string
_temp1449; _temp1449.curr= _temp1448; _temp1449.base= _temp1448; _temp1449.last_plus_one=
_temp1448 + 7; _temp1449;}); _LL1399: return( struct _tagged_string)({ char*
_temp1450=( char*)"fprintf"; struct _tagged_string _temp1451; _temp1451.curr=
_temp1450; _temp1451.base= _temp1450; _temp1451.last_plus_one= _temp1450 + 8;
_temp1451;}); _LL1401: return( struct _tagged_string)({ char* _temp1452=( char*)"xprintf";
struct _tagged_string _temp1453; _temp1453.curr= _temp1452; _temp1453.base=
_temp1452; _temp1453.last_plus_one= _temp1452 + 8; _temp1453;}); _LL1403: return(
struct _tagged_string)({ char* _temp1454=( char*)"scanf"; struct _tagged_string
_temp1455; _temp1455.curr= _temp1454; _temp1455.base= _temp1454; _temp1455.last_plus_one=
_temp1454 + 6; _temp1455;}); _LL1405: return( struct _tagged_string)({ char*
_temp1456=( char*)"fscanf"; struct _tagged_string _temp1457; _temp1457.curr=
_temp1456; _temp1457.base= _temp1456; _temp1457.last_plus_one= _temp1456 + 7;
_temp1457;}); _LL1407: return( struct _tagged_string)({ char* _temp1458=( char*)"sscanf";
struct _tagged_string _temp1459; _temp1459.curr= _temp1458; _temp1459.base=
_temp1458; _temp1459.last_plus_one= _temp1458 + 7; _temp1459;}); _LL1355:;}
struct Cyc_PP_Doc* Cyc_Absynpp_prim2doc( void* p){ return Cyc_PP_text( Cyc_Absynpp_prim2str(
p));} struct Cyc_PP_Doc* Cyc_Absynpp_structarg2doc( struct _tuple10* t){ if((* t).f1
== 0){ return Cyc_Absynpp_exp2doc((* t).f2);} else{ return Cyc_Absynpp_cat3( Cyc_Absynpp_textptr((
struct _tagged_string*)((* t).f1)->v), Cyc_PP_text(( struct _tagged_string)({
char* _temp1460=( char*)" = "; struct _tagged_string _temp1461; _temp1461.curr=
_temp1460; _temp1461.base= _temp1460; _temp1461.last_plus_one= _temp1460 + 4;
_temp1461;})), Cyc_Absynpp_exp2doc((* t).f2));}} struct Cyc_PP_Doc* Cyc_Absynpp_cat2(
struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc* d2){ return Cyc_PP_concat( d1, d2);}
struct Cyc_PP_Doc* Cyc_Absynpp_cat3( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2, struct Cyc_PP_Doc* d3){ return Cyc_PP_concat( d1, Cyc_PP_concat( d2, d3));}
struct Cyc_PP_Doc* Cyc_Absynpp_cat4( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4){ return Cyc_PP_concat( d1, Cyc_PP_concat(
d2, Cyc_PP_concat( d3, d4)));} struct Cyc_PP_Doc* Cyc_Absynpp_cat5( struct Cyc_PP_Doc*
d1, struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct
Cyc_PP_Doc* d5){ return Cyc_PP_concat( d1, Cyc_PP_concat( d2, Cyc_PP_concat( d3,
Cyc_PP_concat( d4, d5))));} struct Cyc_PP_Doc* Cyc_Absynpp_cat6( struct Cyc_PP_Doc*
d1, struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct
Cyc_PP_Doc* d5, struct Cyc_PP_Doc* d6){ return Cyc_PP_concat( d1, Cyc_PP_concat(
d2, Cyc_PP_concat( d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, d6)))));} struct
Cyc_PP_Doc* Cyc_Absynpp_cat7( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc* d2,
struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct Cyc_PP_Doc* d5, struct Cyc_PP_Doc*
d6, struct Cyc_PP_Doc* d7){ return Cyc_PP_concat( d1, Cyc_PP_concat( d2, Cyc_PP_concat(
d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, Cyc_PP_concat( d6, d7))))));} struct
Cyc_PP_Doc* Cyc_Absynpp_cat8( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc* d2,
struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct Cyc_PP_Doc* d5, struct Cyc_PP_Doc*
d6, struct Cyc_PP_Doc* d7, struct Cyc_PP_Doc* d8){ return Cyc_PP_concat( d1, Cyc_PP_concat(
d2, Cyc_PP_concat( d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, Cyc_PP_concat( d6,
Cyc_PP_concat( d7, d8)))))));} struct Cyc_PP_Doc* Cyc_Absynpp_cat9( struct Cyc_PP_Doc*
d1, struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct
Cyc_PP_Doc* d5, struct Cyc_PP_Doc* d6, struct Cyc_PP_Doc* d7, struct Cyc_PP_Doc*
d8, struct Cyc_PP_Doc* d9){ return Cyc_PP_concat( d1, Cyc_PP_concat( d2, Cyc_PP_concat(
d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, Cyc_PP_concat( d6, Cyc_PP_concat( d7,
Cyc_PP_concat( d8, d9))))))));} struct Cyc_PP_Doc* Cyc_Absynpp_cat10( struct Cyc_PP_Doc*
d1, struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct
Cyc_PP_Doc* d5, struct Cyc_PP_Doc* d6, struct Cyc_PP_Doc* d7, struct Cyc_PP_Doc*
d8, struct Cyc_PP_Doc* d9, struct Cyc_PP_Doc* d10){ return Cyc_PP_concat( d1,
Cyc_PP_concat( d2, Cyc_PP_concat( d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, Cyc_PP_concat(
d6, Cyc_PP_concat( d7, Cyc_PP_concat( d8, Cyc_PP_concat( d9, d10)))))))));}
struct Cyc_PP_Doc* Cyc_Absynpp_cat11( struct Cyc_PP_Doc* d1, struct Cyc_PP_Doc*
d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct Cyc_PP_Doc* d5, struct
Cyc_PP_Doc* d6, struct Cyc_PP_Doc* d7, struct Cyc_PP_Doc* d8, struct Cyc_PP_Doc*
d9, struct Cyc_PP_Doc* d10, struct Cyc_PP_Doc* d11){ return Cyc_PP_concat( d1,
Cyc_PP_concat( d2, Cyc_PP_concat( d3, Cyc_PP_concat( d4, Cyc_PP_concat( d5, Cyc_PP_concat(
d6, Cyc_PP_concat( d7, Cyc_PP_concat( d8, Cyc_PP_concat( d9, Cyc_PP_concat( d10,
d11))))))))));} struct Cyc_PP_Doc* Cyc_Absynpp_cat12( struct Cyc_PP_Doc* d1,
struct Cyc_PP_Doc* d2, struct Cyc_PP_Doc* d3, struct Cyc_PP_Doc* d4, struct Cyc_PP_Doc*
d5, struct Cyc_PP_Doc* d6, struct Cyc_PP_Doc* d7, struct Cyc_PP_Doc* d8, struct
Cyc_PP_Doc* d9, struct Cyc_PP_Doc* d10, struct Cyc_PP_Doc* d11, struct Cyc_PP_Doc*
d12){ return Cyc_PP_concat( d1, Cyc_PP_concat( d2, Cyc_PP_concat( d3, Cyc_PP_concat(
d4, Cyc_PP_concat( d5, Cyc_PP_concat( d6, Cyc_PP_concat( d7, Cyc_PP_concat( d8,
Cyc_PP_concat( d9, Cyc_PP_concat( d10, Cyc_PP_concat( d11, d12)))))))))));} int
Cyc_Absynpp_is_declaration( struct Cyc_Absyn_Stmt* s){ void* _temp1462=( void*)
s->r; struct Cyc_Absyn_Stmt* _temp1468; struct Cyc_Absyn_Decl* _temp1470;
_LL1464: if(( unsigned int) _temp1462 > 1u?(( struct _enum_struct*) _temp1462)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL1471: _temp1470=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1462)->f1; goto _LL1469; _LL1469:
_temp1468=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1462)->f2;
goto _LL1465;} else{ goto _LL1466;} _LL1466: goto _LL1467; _LL1465: return 1;
_LL1467: return 0; _LL1463:;} struct Cyc_PP_Doc* Cyc_Absynpp_stmt2doc( struct
Cyc_Absyn_Stmt* st){ struct Cyc_PP_Doc* s;{ void* _temp1472=( void*) st->r;
struct Cyc_Absyn_Exp* _temp1512; struct Cyc_Absyn_Stmt* _temp1514; struct Cyc_Absyn_Stmt*
_temp1516; struct Cyc_Absyn_Exp* _temp1518; struct Cyc_Absyn_Stmt* _temp1520;
struct Cyc_Absyn_Stmt* _temp1522; struct Cyc_Absyn_Exp* _temp1524; struct Cyc_Absyn_Stmt*
_temp1526; struct _tuple2 _temp1528; struct Cyc_Absyn_Stmt* _temp1530; struct
Cyc_Absyn_Exp* _temp1532; struct Cyc_Absyn_Stmt* _temp1534; struct Cyc_Absyn_Stmt*
_temp1536; struct Cyc_Absyn_Stmt* _temp1538; struct _tagged_string* _temp1540;
struct Cyc_Absyn_Stmt* _temp1542; struct _tuple2 _temp1544; struct Cyc_Absyn_Stmt*
_temp1546; struct Cyc_Absyn_Exp* _temp1548; struct _tuple2 _temp1550; struct Cyc_Absyn_Stmt*
_temp1552; struct Cyc_Absyn_Exp* _temp1554; struct Cyc_Absyn_Exp* _temp1556;
struct Cyc_List_List* _temp1558; struct Cyc_Absyn_Exp* _temp1560; struct Cyc_Absyn_Switch_clause**
_temp1562; struct Cyc_List_List* _temp1564; struct Cyc_Absyn_Switch_clause**
_temp1566; struct Cyc_List_List* _temp1568; struct Cyc_Absyn_Stmt* _temp1570;
struct Cyc_Absyn_Decl* _temp1572; struct Cyc_Absyn_Stmt* _temp1574; struct Cyc_Absyn_Stmt*
_temp1576; struct Cyc_Absyn_Stmt* _temp1578; struct _tagged_string* _temp1580;
struct _tuple2 _temp1582; struct Cyc_Absyn_Stmt* _temp1584; struct Cyc_Absyn_Exp*
_temp1586; struct Cyc_Absyn_Stmt* _temp1588; struct Cyc_List_List* _temp1590;
struct Cyc_Absyn_Stmt* _temp1592; _LL1474: if( _temp1472 == Cyc_Absyn_Skip_s){
goto _LL1475;} else{ goto _LL1476;} _LL1476: if(( unsigned int) _temp1472 > 1u?((
struct _enum_struct*) _temp1472)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1513:
_temp1512=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp1472)->f1;
goto _LL1477;} else{ goto _LL1478;} _LL1478: if(( unsigned int) _temp1472 > 1u?((
struct _enum_struct*) _temp1472)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1517:
_temp1516=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1472)->f1;
goto _LL1515; _LL1515: _temp1514=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp1472)->f2; goto _LL1479;} else{ goto _LL1480;} _LL1480: if(( unsigned int)
_temp1472 > 1u?(( struct _enum_struct*) _temp1472)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL1519: _temp1518=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1472)->f1; goto _LL1481;} else{ goto _LL1482;} _LL1482: if(( unsigned int)
_temp1472 > 1u?(( struct _enum_struct*) _temp1472)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL1525: _temp1524=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1472)->f1; goto _LL1523; _LL1523: _temp1522=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1472)->f2; goto _LL1521; _LL1521:
_temp1520=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1472)->f3; goto _LL1483;} else{ goto _LL1484;} _LL1484: if(( unsigned int)
_temp1472 > 1u?(( struct _enum_struct*) _temp1472)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1529: _temp1528=( struct _tuple2)(( struct Cyc_Absyn_While_s_struct*)
_temp1472)->f1; _LL1533: _temp1532= _temp1528.f1; goto _LL1531; _LL1531:
_temp1530= _temp1528.f2; goto _LL1527; _LL1527: _temp1526=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1472)->f2; goto _LL1485;} else{ goto
_LL1486;} _LL1486: if(( unsigned int) _temp1472 > 1u?(( struct _enum_struct*)
_temp1472)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1535: _temp1534=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp1472)->f1; goto _LL1487;} else{ goto
_LL1488;} _LL1488: if(( unsigned int) _temp1472 > 1u?(( struct _enum_struct*)
_temp1472)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL1537: _temp1536=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp1472)->f1; goto _LL1489;} else{ goto
_LL1490;} _LL1490: if(( unsigned int) _temp1472 > 1u?(( struct _enum_struct*)
_temp1472)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL1541: _temp1540=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp1472)->f1; goto _LL1539;
_LL1539: _temp1538=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1472)->f2; goto _LL1491;} else{ goto _LL1492;} _LL1492: if(( unsigned int)
_temp1472 > 1u?(( struct _enum_struct*) _temp1472)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1557: _temp1556=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1472)->f1; goto _LL1551; _LL1551: _temp1550=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*)
_temp1472)->f2; _LL1555: _temp1554= _temp1550.f1; goto _LL1553; _LL1553:
_temp1552= _temp1550.f2; goto _LL1545; _LL1545: _temp1544=( struct _tuple2)((
struct Cyc_Absyn_For_s_struct*) _temp1472)->f3; _LL1549: _temp1548= _temp1544.f1;
goto _LL1547; _LL1547: _temp1546= _temp1544.f2; goto _LL1543; _LL1543: _temp1542=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1472)->f4; goto
_LL1493;} else{ goto _LL1494;} _LL1494: if(( unsigned int) _temp1472 > 1u?((
struct _enum_struct*) _temp1472)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1561:
_temp1560=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1472)->f1; goto _LL1559; _LL1559: _temp1558=( struct Cyc_List_List*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1472)->f2; goto _LL1495;} else{ goto
_LL1496;} _LL1496: if(( unsigned int) _temp1472 > 1u?(( struct _enum_struct*)
_temp1472)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1565: _temp1564=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1472)->f1; if( _temp1564 == 0){ goto
_LL1563;} else{ goto _LL1498;} _LL1563: _temp1562=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1472)->f2; goto _LL1497;} else{ goto
_LL1498;} _LL1498: if(( unsigned int) _temp1472 > 1u?(( struct _enum_struct*)
_temp1472)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1569: _temp1568=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1472)->f1; goto _LL1567; _LL1567:
_temp1566=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1472)->f2; goto _LL1499;} else{ goto _LL1500;} _LL1500: if(( unsigned int)
_temp1472 > 1u?(( struct _enum_struct*) _temp1472)->tag == Cyc_Absyn_Decl_s_tag:
0){ _LL1573: _temp1572=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp1472)->f1; goto _LL1571; _LL1571: _temp1570=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1472)->f2; goto _LL1501;} else{ goto
_LL1502;} _LL1502: if(( unsigned int) _temp1472 > 1u?(( struct _enum_struct*)
_temp1472)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL1575: _temp1574=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Cut_s_struct*) _temp1472)->f1; goto _LL1503;} else{ goto
_LL1504;} _LL1504: if(( unsigned int) _temp1472 > 1u?(( struct _enum_struct*)
_temp1472)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL1577: _temp1576=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Splice_s_struct*) _temp1472)->f1; goto _LL1505;} else{ goto
_LL1506;} _LL1506: if(( unsigned int) _temp1472 > 1u?(( struct _enum_struct*)
_temp1472)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1581: _temp1580=( struct
_tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1472)->f1; goto
_LL1579; _LL1579: _temp1578=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1472)->f2; goto _LL1507;} else{ goto _LL1508;} _LL1508: if(( unsigned int)
_temp1472 > 1u?(( struct _enum_struct*) _temp1472)->tag == Cyc_Absyn_Do_s_tag: 0){
_LL1589: _temp1588=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1472)->f1; goto _LL1583; _LL1583: _temp1582=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp1472)->f2; _LL1587: _temp1586= _temp1582.f1; goto _LL1585; _LL1585:
_temp1584= _temp1582.f2; goto _LL1509;} else{ goto _LL1510;} _LL1510: if((
unsigned int) _temp1472 > 1u?(( struct _enum_struct*) _temp1472)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1593: _temp1592=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1472)->f1; goto _LL1591; _LL1591: _temp1590=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1472)->f2; goto _LL1511;} else{ goto
_LL1473;} _LL1475: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1594=(
char*)";"; struct _tagged_string _temp1595; _temp1595.curr= _temp1594; _temp1595.base=
_temp1594; _temp1595.last_plus_one= _temp1594 + 2; _temp1595;})); goto _LL1473;
_LL1477: s= Cyc_Absynpp_cat2( Cyc_Absynpp_exp2doc( _temp1512), Cyc_PP_text((
struct _tagged_string)({ char* _temp1596=( char*)";"; struct _tagged_string
_temp1597; _temp1597.curr= _temp1596; _temp1597.base= _temp1596; _temp1597.last_plus_one=
_temp1596 + 2; _temp1597;}))); goto _LL1473; _LL1479: if( Cyc_Absynpp_is_declaration(
_temp1516)){ s= Cyc_Absynpp_cat6( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1598=( char*)"{ "; struct _tagged_string _temp1599; _temp1599.curr=
_temp1598; _temp1599.base= _temp1598; _temp1599.last_plus_one= _temp1598 + 3;
_temp1599;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1516)), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1600=( char*)"}"; struct
_tagged_string _temp1601; _temp1601.curr= _temp1600; _temp1601.base= _temp1600;
_temp1601.last_plus_one= _temp1600 + 2; _temp1601;})), Cyc_PP_line_doc(), Cyc_Absynpp_is_declaration(
_temp1514)? Cyc_Absynpp_cat4( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1602=( char*)"{ "; struct _tagged_string _temp1603; _temp1603.curr=
_temp1602; _temp1603.base= _temp1602; _temp1603.last_plus_one= _temp1602 + 3;
_temp1603;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1514)), Cyc_PP_text((
struct _tagged_string)({ char* _temp1604=( char*)"}"; struct _tagged_string
_temp1605; _temp1605.curr= _temp1604; _temp1605.base= _temp1604; _temp1605.last_plus_one=
_temp1604 + 2; _temp1605;})), Cyc_PP_line_doc()): Cyc_Absynpp_stmt2doc(
_temp1514));} else{ if( Cyc_Absynpp_is_declaration( _temp1514)){ s= Cyc_Absynpp_cat6(
Cyc_Absynpp_stmt2doc( _temp1516), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1606=( char*)"{ "; struct _tagged_string _temp1607;
_temp1607.curr= _temp1606; _temp1607.base= _temp1606; _temp1607.last_plus_one=
_temp1606 + 3; _temp1607;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1514)),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1608=( char*)"}"; struct
_tagged_string _temp1609; _temp1609.curr= _temp1608; _temp1609.base= _temp1608;
_temp1609.last_plus_one= _temp1608 + 2; _temp1609;})), Cyc_PP_line_doc());}
else{ s=(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Stmt*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_stmt2doc,(
struct _tagged_string)({ char* _temp1610=( char*)""; struct _tagged_string
_temp1611; _temp1611.curr= _temp1610; _temp1611.base= _temp1610; _temp1611.last_plus_one=
_temp1610 + 1; _temp1611;}),({ struct Cyc_List_List* _temp1612=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1612->hd=( void*) _temp1516;
_temp1612->tl=({ struct Cyc_List_List* _temp1613=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1613->hd=( void*) _temp1514;
_temp1613->tl= 0; _temp1613;}); _temp1612;}));}} goto _LL1473; _LL1481: if(
_temp1518 == 0){ s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1614=(
char*)"return;"; struct _tagged_string _temp1615; _temp1615.curr= _temp1614;
_temp1615.base= _temp1614; _temp1615.last_plus_one= _temp1614 + 8; _temp1615;}));}
else{ s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1616=( char*)"return "; struct _tagged_string _temp1617; _temp1617.curr=
_temp1616; _temp1617.base= _temp1616; _temp1617.last_plus_one= _temp1616 + 8;
_temp1617;})), _temp1518 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1618= _temp1518; if( _temp1618 == 0){
_throw( Null_Exception);} _temp1618;})), Cyc_PP_text(( struct _tagged_string)({
char* _temp1619=( char*)";"; struct _tagged_string _temp1620; _temp1620.curr=
_temp1619; _temp1620.base= _temp1619; _temp1620.last_plus_one= _temp1619 + 2;
_temp1620;})));} goto _LL1473; _LL1483: { int print_else;{ void* _temp1621=(
void*) _temp1520->r; _LL1623: if( _temp1621 == Cyc_Absyn_Skip_s){ goto _LL1624;}
else{ goto _LL1625;} _LL1625: goto _LL1626; _LL1624: print_else= 0; goto _LL1622;
_LL1626: print_else= 1; goto _LL1622; _LL1622:;} s= Cyc_Absynpp_cat7( Cyc_PP_text((
struct _tagged_string)({ char* _temp1627=( char*)"if ("; struct _tagged_string
_temp1628; _temp1628.curr= _temp1627; _temp1628.base= _temp1627; _temp1628.last_plus_one=
_temp1627 + 5; _temp1628;})), Cyc_Absynpp_exp2doc( _temp1524), Cyc_PP_text((
struct _tagged_string)({ char* _temp1629=( char*)") {"; struct _tagged_string
_temp1630; _temp1630.curr= _temp1629; _temp1630.base= _temp1629; _temp1630.last_plus_one=
_temp1629 + 4; _temp1630;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1522))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1631=( char*)"}"; struct _tagged_string _temp1632;
_temp1632.curr= _temp1631; _temp1632.base= _temp1631; _temp1632.last_plus_one=
_temp1631 + 2; _temp1632;})), print_else? Cyc_Absynpp_cat5( Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1633=( char*)"else {"; struct
_tagged_string _temp1634; _temp1634.curr= _temp1633; _temp1634.base= _temp1633;
_temp1634.last_plus_one= _temp1633 + 7; _temp1634;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1520))), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1635=( char*)"}"; struct _tagged_string
_temp1636; _temp1636.curr= _temp1635; _temp1636.base= _temp1635; _temp1636.last_plus_one=
_temp1635 + 2; _temp1636;}))): Cyc_PP_nil_doc()); goto _LL1473;} _LL1485: s= Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1637=( char*)"while ("; struct
_tagged_string _temp1638; _temp1638.curr= _temp1637; _temp1638.base= _temp1637;
_temp1638.last_plus_one= _temp1637 + 8; _temp1638;})), Cyc_Absynpp_exp2doc(
_temp1532), Cyc_PP_text(( struct _tagged_string)({ char* _temp1639=( char*)") {";
struct _tagged_string _temp1640; _temp1640.curr= _temp1639; _temp1640.base=
_temp1639; _temp1640.last_plus_one= _temp1639 + 4; _temp1640;})), Cyc_PP_nest( 2,
Cyc_Absynpp_cat2( Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( _temp1526))), Cyc_PP_line_doc(),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1641=( char*)"}"; struct
_tagged_string _temp1642; _temp1642.curr= _temp1641; _temp1642.base= _temp1641;
_temp1642.last_plus_one= _temp1641 + 2; _temp1642;}))); goto _LL1473; _LL1487: s=
Cyc_PP_text(( struct _tagged_string)({ char* _temp1643=( char*)"break;"; struct
_tagged_string _temp1644; _temp1644.curr= _temp1643; _temp1644.base= _temp1643;
_temp1644.last_plus_one= _temp1643 + 7; _temp1644;})); goto _LL1473; _LL1489: s=
Cyc_PP_text(( struct _tagged_string)({ char* _temp1645=( char*)"continue;";
struct _tagged_string _temp1646; _temp1646.curr= _temp1645; _temp1646.base=
_temp1645; _temp1646.last_plus_one= _temp1645 + 10; _temp1646;})); goto _LL1473;
_LL1491: s= Cyc_PP_text(({ struct _tagged_string _temp1647=* _temp1540; xprintf("goto %.*s;",
_temp1647.last_plus_one - _temp1647.curr, _temp1647.curr);})); goto _LL1473;
_LL1493: s= Cyc_Absynpp_cat10( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1648=( char*)"for("; struct _tagged_string _temp1649; _temp1649.curr=
_temp1648; _temp1649.base= _temp1648; _temp1649.last_plus_one= _temp1648 + 5;
_temp1649;})), Cyc_Absynpp_exp2doc( _temp1556), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1650=( char*)"; "; struct _tagged_string _temp1651;
_temp1651.curr= _temp1650; _temp1651.base= _temp1650; _temp1651.last_plus_one=
_temp1650 + 3; _temp1651;})), Cyc_Absynpp_exp2doc( _temp1554), Cyc_PP_text((
struct _tagged_string)({ char* _temp1652=( char*)"; "; struct _tagged_string
_temp1653; _temp1653.curr= _temp1652; _temp1653.base= _temp1652; _temp1653.last_plus_one=
_temp1652 + 3; _temp1653;})), Cyc_Absynpp_exp2doc( _temp1548), Cyc_PP_text((
struct _tagged_string)({ char* _temp1654=( char*)") {"; struct _tagged_string
_temp1655; _temp1655.curr= _temp1654; _temp1655.base= _temp1654; _temp1655.last_plus_one=
_temp1654 + 4; _temp1655;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1542))), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1656=( char*)"}"; struct _tagged_string _temp1657;
_temp1657.curr= _temp1656; _temp1657.base= _temp1656; _temp1657.last_plus_one=
_temp1656 + 2; _temp1657;}))); goto _LL1473; _LL1495: s= Cyc_Absynpp_cat7( Cyc_PP_text((
struct _tagged_string)({ char* _temp1658=( char*)"switch ("; struct
_tagged_string _temp1659; _temp1659.curr= _temp1658; _temp1659.base= _temp1658;
_temp1659.last_plus_one= _temp1658 + 9; _temp1659;})), Cyc_Absynpp_exp2doc(
_temp1560), Cyc_PP_text(( struct _tagged_string)({ char* _temp1660=( char*)") {";
struct _tagged_string _temp1661; _temp1661.curr= _temp1660; _temp1661.base=
_temp1660; _temp1661.last_plus_one= _temp1660 + 4; _temp1661;})), Cyc_PP_line_doc(),
Cyc_Absynpp_switchclauses2doc( _temp1558), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1662=( char*)"}"; struct _tagged_string
_temp1663; _temp1663.curr= _temp1662; _temp1663.base= _temp1662; _temp1663.last_plus_one=
_temp1662 + 2; _temp1663;}))); goto _LL1473; _LL1497: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1664=( char*)"fallthru;"; struct _tagged_string
_temp1665; _temp1665.curr= _temp1664; _temp1665.base= _temp1664; _temp1665.last_plus_one=
_temp1664 + 10; _temp1665;})); goto _LL1473; _LL1499: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1666=( char*)"fallthru("; struct
_tagged_string _temp1667; _temp1667.curr= _temp1666; _temp1667.base= _temp1666;
_temp1667.last_plus_one= _temp1666 + 10; _temp1667;})), Cyc_Absynpp_exps2doc_prec(
20, _temp1568), Cyc_PP_text(( struct _tagged_string)({ char* _temp1668=( char*)");";
struct _tagged_string _temp1669; _temp1669.curr= _temp1668; _temp1669.base=
_temp1668; _temp1669.last_plus_one= _temp1668 + 3; _temp1669;}))); goto _LL1473;
_LL1501: s= Cyc_Absynpp_cat3( Cyc_Absynpp_decl2doc( _temp1572), Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1570)); goto _LL1473; _LL1503: s= Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1670=( char*)"cut "; struct
_tagged_string _temp1671; _temp1671.curr= _temp1670; _temp1671.base= _temp1670;
_temp1671.last_plus_one= _temp1670 + 5; _temp1671;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1574))); goto _LL1473; _LL1505: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1672=( char*)"splice "; struct _tagged_string
_temp1673; _temp1673.curr= _temp1672; _temp1673.base= _temp1672; _temp1673.last_plus_one=
_temp1672 + 8; _temp1673;})), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc( _temp1576)));
goto _LL1473; _LL1507: if( Cyc_Absynpp_is_declaration( _temp1578)){ s= Cyc_Absynpp_cat6(
Cyc_Absynpp_textptr( _temp1580), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1674=( char*)": {"; struct _tagged_string _temp1675; _temp1675.curr=
_temp1674; _temp1675.base= _temp1674; _temp1675.last_plus_one= _temp1674 + 4;
_temp1675;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1578)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1676=( char*)"}"; struct _tagged_string _temp1677; _temp1677.curr=
_temp1676; _temp1677.base= _temp1676; _temp1677.last_plus_one= _temp1676 + 2;
_temp1677;})));} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_textptr( _temp1580), Cyc_PP_text((
struct _tagged_string)({ char* _temp1678=( char*)": "; struct _tagged_string
_temp1679; _temp1679.curr= _temp1678; _temp1679.base= _temp1678; _temp1679.last_plus_one=
_temp1678 + 3; _temp1679;})), Cyc_Absynpp_stmt2doc( _temp1578));} goto _LL1473;
_LL1509: s= Cyc_Absynpp_cat7( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1680=( char*)"do {"; struct _tagged_string _temp1681; _temp1681.curr=
_temp1680; _temp1681.base= _temp1680; _temp1681.last_plus_one= _temp1680 + 5;
_temp1681;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1588)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1682=( char*)"} while ("; struct _tagged_string _temp1683; _temp1683.curr=
_temp1682; _temp1683.base= _temp1682; _temp1683.last_plus_one= _temp1682 + 10;
_temp1683;})), Cyc_Absynpp_exp2doc( _temp1586), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1684=( char*)");"; struct _tagged_string _temp1685;
_temp1685.curr= _temp1684; _temp1685.base= _temp1684; _temp1685.last_plus_one=
_temp1684 + 3; _temp1685;}))); goto _LL1473; _LL1511: s= Cyc_Absynpp_cat9( Cyc_PP_text((
struct _tagged_string)({ char* _temp1686=( char*)"try"; struct _tagged_string
_temp1687; _temp1687.curr= _temp1686; _temp1687.base= _temp1686; _temp1687.last_plus_one=
_temp1686 + 4; _temp1687;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_stmt2doc(
_temp1592)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1688=( char*)"catch {"; struct _tagged_string _temp1689; _temp1689.curr=
_temp1688; _temp1689.base= _temp1688; _temp1689.last_plus_one= _temp1688 + 8;
_temp1689;})), Cyc_PP_line_doc(), Cyc_PP_nest( 2, Cyc_Absynpp_switchclauses2doc(
_temp1590)), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char*
_temp1690=( char*)"}"; struct _tagged_string _temp1691; _temp1691.curr=
_temp1690; _temp1691.base= _temp1690; _temp1691.last_plus_one= _temp1690 + 2;
_temp1691;}))); goto _LL1473; _LL1473:;} return s;} struct Cyc_PP_Doc* Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat* p){ struct Cyc_PP_Doc* s;{ void* _temp1692=( void*) p->r;
int _temp1724; void* _temp1726; char _temp1728; struct _tagged_string _temp1730;
struct Cyc_Absyn_Vardecl* _temp1732; struct Cyc_List_List* _temp1734; struct Cyc_Absyn_Pat*
_temp1736; struct Cyc_Absyn_Vardecl* _temp1738; struct _tuple0* _temp1740;
struct Cyc_List_List* _temp1742; struct Cyc_List_List* _temp1744; struct _tuple0*
_temp1746; struct Cyc_List_List* _temp1748; struct Cyc_List_List* _temp1750;
struct _tuple0* _temp1752; struct Cyc_List_List* _temp1754; struct Cyc_List_List*
_temp1756; struct Cyc_Core_Opt* _temp1758; struct Cyc_Absyn_Structdecl*
_temp1760; struct Cyc_Absyn_Enumfield* _temp1762; struct Cyc_Absyn_Enumdecl*
_temp1764; struct Cyc_List_List* _temp1766; struct Cyc_List_List* _temp1768;
struct Cyc_Core_Opt* _temp1770; struct _tuple0* _temp1772; struct Cyc_Absyn_Enumfield*
_temp1774; struct Cyc_Absyn_Xenumdecl* _temp1776; struct Cyc_List_List*
_temp1778; struct Cyc_List_List* _temp1780; struct _tuple0* _temp1782; _LL1694:
if( _temp1692 == Cyc_Absyn_Wild_p){ goto _LL1695;} else{ goto _LL1696;} _LL1696:
if( _temp1692 == Cyc_Absyn_Null_p){ goto _LL1697;} else{ goto _LL1698;} _LL1698:
if(( unsigned int) _temp1692 > 2u?(( struct _enum_struct*) _temp1692)->tag ==
Cyc_Absyn_Int_p_tag: 0){ _LL1727: _temp1726=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp1692)->f1; goto _LL1725; _LL1725: _temp1724=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp1692)->f2; goto _LL1699;} else{ goto _LL1700;} _LL1700: if(( unsigned int)
_temp1692 > 2u?(( struct _enum_struct*) _temp1692)->tag == Cyc_Absyn_Char_p_tag:
0){ _LL1729: _temp1728=( char)(( struct Cyc_Absyn_Char_p_struct*) _temp1692)->f1;
goto _LL1701;} else{ goto _LL1702;} _LL1702: if(( unsigned int) _temp1692 > 2u?((
struct _enum_struct*) _temp1692)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL1731:
_temp1730=( struct _tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp1692)->f1;
goto _LL1703;} else{ goto _LL1704;} _LL1704: if(( unsigned int) _temp1692 > 2u?((
struct _enum_struct*) _temp1692)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL1733:
_temp1732=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*)
_temp1692)->f1; goto _LL1705;} else{ goto _LL1706;} _LL1706: if(( unsigned int)
_temp1692 > 2u?(( struct _enum_struct*) _temp1692)->tag == Cyc_Absyn_Tuple_p_tag:
0){ _LL1735: _temp1734=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*)
_temp1692)->f1; goto _LL1707;} else{ goto _LL1708;} _LL1708: if(( unsigned int)
_temp1692 > 2u?(( struct _enum_struct*) _temp1692)->tag == Cyc_Absyn_Pointer_p_tag:
0){ _LL1737: _temp1736=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*)
_temp1692)->f1; goto _LL1709;} else{ goto _LL1710;} _LL1710: if(( unsigned int)
_temp1692 > 2u?(( struct _enum_struct*) _temp1692)->tag == Cyc_Absyn_Reference_p_tag:
0){ _LL1739: _temp1738=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*)
_temp1692)->f1; goto _LL1711;} else{ goto _LL1712;} _LL1712: if(( unsigned int)
_temp1692 > 2u?(( struct _enum_struct*) _temp1692)->tag == Cyc_Absyn_UnknownId_p_tag:
0){ _LL1741: _temp1740=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp1692)->f1; goto _LL1713;} else{ goto _LL1714;} _LL1714: if(( unsigned int)
_temp1692 > 2u?(( struct _enum_struct*) _temp1692)->tag == Cyc_Absyn_UnknownCall_p_tag:
0){ _LL1747: _temp1746=( struct _tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1692)->f1; goto _LL1745; _LL1745: _temp1744=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownCall_p_struct*) _temp1692)->f2; goto _LL1743; _LL1743:
_temp1742=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp1692)->f3; goto _LL1715;} else{ goto _LL1716;} _LL1716: if(( unsigned int)
_temp1692 > 2u?(( struct _enum_struct*) _temp1692)->tag == Cyc_Absyn_UnknownFields_p_tag:
0){ _LL1753: _temp1752=( struct _tuple0*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1692)->f1; goto _LL1751; _LL1751: _temp1750=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp1692)->f2; goto _LL1749; _LL1749:
_temp1748=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp1692)->f3; goto _LL1717;} else{ goto _LL1718;} _LL1718: if(( unsigned int)
_temp1692 > 2u?(( struct _enum_struct*) _temp1692)->tag == Cyc_Absyn_Struct_p_tag:
0){ _LL1761: _temp1760=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1692)->f1; goto _LL1759; _LL1759: _temp1758=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1692)->f2; goto _LL1757; _LL1757:
_temp1756=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp1692)->f3; goto _LL1755; _LL1755: _temp1754=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp1692)->f4; goto _LL1719;} else{ goto
_LL1720;} _LL1720: if(( unsigned int) _temp1692 > 2u?(( struct _enum_struct*)
_temp1692)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1773: _temp1772=( struct _tuple0*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1692)->f1; goto _LL1771; _LL1771:
_temp1770=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_p_struct*) _temp1692)->f2;
goto _LL1769; _LL1769: _temp1768=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1692)->f3; goto _LL1767; _LL1767: _temp1766=( struct Cyc_List_List*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1692)->f4; goto _LL1765; _LL1765:
_temp1764=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp1692)->f5; goto _LL1763; _LL1763: _temp1762=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_p_struct*) _temp1692)->f6; goto _LL1721;} else{ goto
_LL1722;} _LL1722: if(( unsigned int) _temp1692 > 2u?(( struct _enum_struct*)
_temp1692)->tag == Cyc_Absyn_Xenum_p_tag: 0){ _LL1783: _temp1782=( struct
_tuple0*)(( struct Cyc_Absyn_Xenum_p_struct*) _temp1692)->f1; goto _LL1781;
_LL1781: _temp1780=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1692)->f2; goto _LL1779; _LL1779: _temp1778=( struct Cyc_List_List*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1692)->f3; goto _LL1777; _LL1777:
_temp1776=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp1692)->f4; goto _LL1775; _LL1775: _temp1774=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp1692)->f5; goto _LL1723;} else{ goto
_LL1693;} _LL1695: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1784=(
char*)"_"; struct _tagged_string _temp1785; _temp1785.curr= _temp1784; _temp1785.base=
_temp1784; _temp1785.last_plus_one= _temp1784 + 2; _temp1785;})); goto _LL1693;
_LL1697: s= Cyc_PP_text(( struct _tagged_string)({ char* _temp1786=( char*)"null";
struct _tagged_string _temp1787; _temp1787.curr= _temp1786; _temp1787.base=
_temp1786; _temp1787.last_plus_one= _temp1786 + 5; _temp1787;})); goto _LL1693;
_LL1699: if( _temp1726 == Cyc_Absyn_Signed){ s= Cyc_PP_text( xprintf("%d",
_temp1724));} else{ s= Cyc_PP_text( xprintf("%u",( unsigned int) _temp1724));}
goto _LL1693; _LL1701: s= Cyc_PP_text(({ struct _tagged_string _temp1788= Cyc_Absynpp_char_escape(
_temp1728); xprintf("'%.*s'", _temp1788.last_plus_one - _temp1788.curr,
_temp1788.curr);})); goto _LL1693; _LL1703: s= Cyc_PP_text( _temp1730); goto
_LL1693; _LL1705: s= Cyc_Absynpp_qvar2doc( _temp1732->name); goto _LL1693;
_LL1707: s= Cyc_Absynpp_cat3( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1789=( char*)"$("; struct _tagged_string _temp1790; _temp1790.curr=
_temp1789; _temp1790.base= _temp1789; _temp1790.last_plus_one= _temp1789 + 3;
_temp1790;})),(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Pat*),
struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseq)( Cyc_Absynpp_pat2doc,(
struct _tagged_string)({ char* _temp1791=( char*)","; struct _tagged_string
_temp1792; _temp1792.curr= _temp1791; _temp1792.base= _temp1791; _temp1792.last_plus_one=
_temp1791 + 2; _temp1792;}), _temp1734), Cyc_PP_text(( struct _tagged_string)({
char* _temp1793=( char*)")"; struct _tagged_string _temp1794; _temp1794.curr=
_temp1793; _temp1794.base= _temp1793; _temp1794.last_plus_one= _temp1793 + 2;
_temp1794;}))); goto _LL1693; _LL1709: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1795=( char*)"&"; struct _tagged_string _temp1796;
_temp1796.curr= _temp1795; _temp1796.base= _temp1795; _temp1796.last_plus_one=
_temp1795 + 2; _temp1796;})), Cyc_Absynpp_pat2doc( _temp1736)); goto _LL1693;
_LL1711: s= Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char*
_temp1797=( char*)"*"; struct _tagged_string _temp1798; _temp1798.curr=
_temp1797; _temp1798.base= _temp1797; _temp1798.last_plus_one= _temp1797 + 2;
_temp1798;})), Cyc_Absynpp_qvar2doc( _temp1738->name)); goto _LL1693; _LL1713: s=
Cyc_Absynpp_qvar2doc( _temp1740); goto _LL1693; _LL1715: s= Cyc_Absynpp_cat3(
Cyc_Absynpp_qvar2doc( _temp1746), Cyc_Absynpp_tvars2doc( _temp1744), Cyc_Absynpp_group((
struct _tagged_string)({ char* _temp1799=( char*)"("; struct _tagged_string
_temp1800; _temp1800.curr= _temp1799; _temp1800.base= _temp1799; _temp1800.last_plus_one=
_temp1799 + 2; _temp1800;}),( struct _tagged_string)({ char* _temp1801=( char*)")";
struct _tagged_string _temp1802; _temp1802.curr= _temp1801; _temp1802.base=
_temp1801; _temp1802.last_plus_one= _temp1801 + 2; _temp1802;}),( struct
_tagged_string)({ char* _temp1803=( char*)","; struct _tagged_string _temp1804;
_temp1804.curr= _temp1803; _temp1804.base= _temp1803; _temp1804.last_plus_one=
_temp1803 + 2; _temp1804;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_pat2doc,
_temp1742))); goto _LL1693; _LL1717: s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc(
_temp1752), Cyc_Absynpp_tvars2doc( _temp1750), Cyc_Absynpp_group(( struct
_tagged_string)({ char* _temp1805=( char*)"{"; struct _tagged_string _temp1806;
_temp1806.curr= _temp1805; _temp1806.base= _temp1805; _temp1806.last_plus_one=
_temp1805 + 2; _temp1806;}),( struct _tagged_string)({ char* _temp1807=( char*)"}";
struct _tagged_string _temp1808; _temp1808.curr= _temp1807; _temp1808.base=
_temp1807; _temp1808.last_plus_one= _temp1807 + 2; _temp1808;}),( struct
_tagged_string)({ char* _temp1809=( char*)","; struct _tagged_string _temp1810;
_temp1810.curr= _temp1809; _temp1810.base= _temp1809; _temp1810.last_plus_one=
_temp1809 + 2; _temp1810;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)(
struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1748))); goto _LL1693; _LL1719: s= Cyc_Absynpp_cat3( _temp1760->name == 0?
Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc(( struct _tuple0*)( _temp1760->name)->v),
Cyc_Absynpp_tvars2doc( _temp1756), Cyc_Absynpp_group(( struct _tagged_string)({
char* _temp1811=( char*)"{"; struct _tagged_string _temp1812; _temp1812.curr=
_temp1811; _temp1812.base= _temp1811; _temp1812.last_plus_one= _temp1811 + 2;
_temp1812;}),( struct _tagged_string)({ char* _temp1813=( char*)"}"; struct
_tagged_string _temp1814; _temp1814.curr= _temp1813; _temp1814.base= _temp1813;
_temp1814.last_plus_one= _temp1813 + 2; _temp1814;}),( struct _tagged_string)({
char* _temp1815=( char*)","; struct _tagged_string _temp1816; _temp1816.curr=
_temp1815; _temp1816.base= _temp1815; _temp1816.last_plus_one= _temp1815 + 2;
_temp1816;}),(( struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_dp2doc,
_temp1754))); goto _LL1693; _LL1721: _temp1782= _temp1772; _temp1780= _temp1768;
_temp1778= _temp1766; goto _LL1723; _LL1723: if( _temp1778 == 0){ s= Cyc_Absynpp_qvar2doc(
_temp1782);} else{ s= Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc( _temp1782), Cyc_Absynpp_tvars2doc(
_temp1780), Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1817=( char*)"(";
struct _tagged_string _temp1818; _temp1818.curr= _temp1817; _temp1818.base=
_temp1817; _temp1818.last_plus_one= _temp1817 + 2; _temp1818;}),( struct
_tagged_string)({ char* _temp1819=( char*)")"; struct _tagged_string _temp1820;
_temp1820.curr= _temp1819; _temp1820.base= _temp1819; _temp1820.last_plus_one=
_temp1819 + 2; _temp1820;}),( struct _tagged_string)({ char* _temp1821=( char*)",";
struct _tagged_string _temp1822; _temp1822.curr= _temp1821; _temp1822.base=
_temp1821; _temp1822.last_plus_one= _temp1821 + 2; _temp1822;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Absynpp_pat2doc, _temp1778)));} goto _LL1693; _LL1693:;} return s;} struct
Cyc_PP_Doc* Cyc_Absynpp_dp2doc( struct _tuple5* dp){ return Cyc_Absynpp_cat2(
Cyc_Absynpp_egroup(( struct _tagged_string)({ char* _temp1823=( char*)""; struct
_tagged_string _temp1824; _temp1824.curr= _temp1823; _temp1824.base= _temp1823;
_temp1824.last_plus_one= _temp1823 + 1; _temp1824;}),( struct _tagged_string)({
char* _temp1825=( char*)"="; struct _tagged_string _temp1826; _temp1826.curr=
_temp1825; _temp1826.base= _temp1825; _temp1826.last_plus_one= _temp1825 + 2;
_temp1826;}),( struct _tagged_string)({ char* _temp1827=( char*)"="; struct
_tagged_string _temp1828; _temp1828.curr= _temp1827; _temp1828.base= _temp1827;
_temp1828.last_plus_one= _temp1827 + 2; _temp1828;}),(( struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(* f)( void*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_designator2doc,(*
dp).f1)), Cyc_Absynpp_pat2doc((* dp).f2));} struct Cyc_PP_Doc* Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause* c){ if( c->where_clause == 0?( void*)( c->pattern)->r
== Cyc_Absyn_Wild_p: 0){ return Cyc_Absynpp_cat2( Cyc_PP_text(( struct
_tagged_string)({ char* _temp1829=( char*)"default: "; struct _tagged_string
_temp1830; _temp1830.curr= _temp1829; _temp1830.base= _temp1829; _temp1830.last_plus_one=
_temp1829 + 10; _temp1830;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));} else{ if( c->where_clause == 0){ return Cyc_Absynpp_cat4(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1831=( char*)"case "; struct
_tagged_string _temp1832; _temp1832.curr= _temp1831; _temp1832.base= _temp1831;
_temp1832.last_plus_one= _temp1831 + 6; _temp1832;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1833=( char*)": "; struct
_tagged_string _temp1834; _temp1834.curr= _temp1833; _temp1834.base= _temp1833;
_temp1834.last_plus_one= _temp1833 + 3; _temp1834;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2(
Cyc_PP_line_doc(), Cyc_Absynpp_stmt2doc( c->body))));} else{ return Cyc_Absynpp_cat6(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1835=( char*)"case "; struct
_tagged_string _temp1836; _temp1836.curr= _temp1835; _temp1836.base= _temp1835;
_temp1836.last_plus_one= _temp1835 + 6; _temp1836;})), Cyc_Absynpp_pat2doc( c->pattern),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1837=( char*)" && "; struct
_tagged_string _temp1838; _temp1838.curr= _temp1837; _temp1838.base= _temp1837;
_temp1838.last_plus_one= _temp1837 + 5; _temp1838;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1839= c->where_clause; if(
_temp1839 == 0){ _throw( Null_Exception);} _temp1839;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1840=( char*)": "; struct _tagged_string _temp1841;
_temp1841.curr= _temp1840; _temp1841.base= _temp1840; _temp1841.last_plus_one=
_temp1840 + 3; _temp1841;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( c->body))));}}} struct Cyc_PP_Doc* Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List* cs){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Switch_clause*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_switchclause2doc,( struct _tagged_string)({
char* _temp1842=( char*)""; struct _tagged_string _temp1843; _temp1843.curr=
_temp1842; _temp1843.base= _temp1842; _temp1843.last_plus_one= _temp1842 + 1;
_temp1843;}), cs);} struct Cyc_PP_Doc* Cyc_Absynpp_decl2doc( struct Cyc_Absyn_Decl*
d){ struct Cyc_PP_Doc* s;{ void* _temp1844=( void*) d->r; struct Cyc_Absyn_Fndecl*
_temp1868; struct Cyc_Absyn_Structdecl* _temp1870; struct Cyc_Absyn_Vardecl*
_temp1872; struct Cyc_Absyn_Vardecl _temp1874; struct Cyc_Core_Opt* _temp1875;
int _temp1877; struct Cyc_Absyn_Exp* _temp1879; void* _temp1881; struct Cyc_Absyn_Tqual*
_temp1883; struct _tuple0* _temp1885; void* _temp1887; struct Cyc_Absyn_Enumdecl*
_temp1889; struct Cyc_Absyn_Xenumdecl* _temp1891; int _temp1893; struct Cyc_Absyn_Exp*
_temp1895; struct Cyc_Core_Opt* _temp1897; struct Cyc_Core_Opt* _temp1899;
struct Cyc_Absyn_Pat* _temp1901; struct Cyc_Absyn_Typedefdecl* _temp1903; struct
Cyc_List_List* _temp1905; struct _tagged_string* _temp1907; struct Cyc_List_List*
_temp1909; struct _tuple0* _temp1911; struct Cyc_List_List* _temp1913; _LL1846:
if(( unsigned int) _temp1844 > 1u?(( struct _enum_struct*) _temp1844)->tag ==
Cyc_Absyn_Fn_d_tag: 0){ _LL1869: _temp1868=( struct Cyc_Absyn_Fndecl*)(( struct
Cyc_Absyn_Fn_d_struct*) _temp1844)->f1; goto _LL1847;} else{ goto _LL1848;}
_LL1848: if(( unsigned int) _temp1844 > 1u?(( struct _enum_struct*) _temp1844)->tag
== Cyc_Absyn_Struct_d_tag: 0){ _LL1871: _temp1870=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp1844)->f1; goto _LL1849;} else{ goto
_LL1850;} _LL1850: if(( unsigned int) _temp1844 > 1u?(( struct _enum_struct*)
_temp1844)->tag == Cyc_Absyn_Var_d_tag: 0){ _LL1873: _temp1872=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1844)->f1; _temp1874=* _temp1872; _LL1888:
_temp1887=( void*) _temp1874.sc; goto _LL1886; _LL1886: _temp1885=( struct
_tuple0*) _temp1874.name; goto _LL1884; _LL1884: _temp1883=( struct Cyc_Absyn_Tqual*)
_temp1874.tq; goto _LL1882; _LL1882: _temp1881=( void*) _temp1874.type; goto
_LL1880; _LL1880: _temp1879=( struct Cyc_Absyn_Exp*) _temp1874.initializer; goto
_LL1878; _LL1878: _temp1877=( int) _temp1874.shadow; goto _LL1876; _LL1876:
_temp1875=( struct Cyc_Core_Opt*) _temp1874.region; goto _LL1851;} else{ goto
_LL1852;} _LL1852: if( _temp1844 == Cyc_Absyn_Union_d){ goto _LL1853;} else{
goto _LL1854;} _LL1854: if(( unsigned int) _temp1844 > 1u?(( struct _enum_struct*)
_temp1844)->tag == Cyc_Absyn_Enum_d_tag: 0){ _LL1890: _temp1889=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp1844)->f1; goto _LL1855;} else{ goto
_LL1856;} _LL1856: if(( unsigned int) _temp1844 > 1u?(( struct _enum_struct*)
_temp1844)->tag == Cyc_Absyn_Xenum_d_tag: 0){ _LL1892: _temp1891=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_d_struct*) _temp1844)->f1; goto _LL1857;} else{ goto
_LL1858;} _LL1858: if(( unsigned int) _temp1844 > 1u?(( struct _enum_struct*)
_temp1844)->tag == Cyc_Absyn_Let_d_tag: 0){ _LL1902: _temp1901=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp1844)->f1; goto _LL1900; _LL1900: _temp1899=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1844)->f2; goto
_LL1898; _LL1898: _temp1897=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1844)->f3; goto _LL1896; _LL1896: _temp1895=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Let_d_struct*) _temp1844)->f4; goto _LL1894; _LL1894: _temp1893=(
int)(( struct Cyc_Absyn_Let_d_struct*) _temp1844)->f5; goto _LL1859;} else{ goto
_LL1860;} _LL1860: if(( unsigned int) _temp1844 > 1u?(( struct _enum_struct*)
_temp1844)->tag == Cyc_Absyn_Typedef_d_tag: 0){ _LL1904: _temp1903=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp1844)->f1; goto _LL1861;} else{ goto
_LL1862;} _LL1862: if(( unsigned int) _temp1844 > 1u?(( struct _enum_struct*)
_temp1844)->tag == Cyc_Absyn_Namespace_d_tag: 0){ _LL1908: _temp1907=( struct
_tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp1844)->f1; goto
_LL1906; _LL1906: _temp1905=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1844)->f2; goto _LL1863;} else{ goto _LL1864;} _LL1864: if(( unsigned int)
_temp1844 > 1u?(( struct _enum_struct*) _temp1844)->tag == Cyc_Absyn_Using_d_tag:
0){ _LL1912: _temp1911=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1844)->f1; goto _LL1910; _LL1910: _temp1909=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1844)->f2; goto _LL1865;} else{ goto
_LL1866;} _LL1866: if(( unsigned int) _temp1844 > 1u?(( struct _enum_struct*)
_temp1844)->tag == Cyc_Absyn_ExternC_d_tag: 0){ _LL1914: _temp1913=( struct Cyc_List_List*)((
struct Cyc_Absyn_ExternC_d_struct*) _temp1844)->f1; goto _LL1867;} else{ goto
_LL1845;} _LL1847: { void* t=({ struct Cyc_Absyn_FnType_struct* _temp1915=(
struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp1915->tag= Cyc_Absyn_FnType_tag; _temp1915->f1=({ struct Cyc_Absyn_FnInfo
_temp1916; _temp1916.tvars= _temp1868->tvs; _temp1916.effect= _temp1868->effect;
_temp1916.ret_typ=( void*)(( void*) _temp1868->ret_type); _temp1916.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple3*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt, _temp1868->args); _temp1916.varargs=
_temp1868->varargs; _temp1916;});( void*) _temp1915;}); s= Cyc_Absynpp_cat7( Cyc_PP_text(
_temp1868->is_inline?( struct _tagged_string)({ char* _temp1917=( char*)"inline ";
struct _tagged_string _temp1918; _temp1918.curr= _temp1917; _temp1918.base=
_temp1917; _temp1918.last_plus_one= _temp1917 + 8; _temp1918;}):( struct
_tagged_string)({ char* _temp1919=( char*)""; struct _tagged_string _temp1920;
_temp1920.curr= _temp1919; _temp1920.base= _temp1919; _temp1920.last_plus_one=
_temp1919 + 1; _temp1920;})), Cyc_Absynpp_scope2doc(( void*) _temp1868->sc), Cyc_Absynpp_tqtd2doc(({
struct Cyc_Absyn_Tqual* _temp1921=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp1921->q_const= 0; _temp1921->q_volatile=
0; _temp1921->q_restrict= 0; _temp1921;}), t,({ struct Cyc_Core_Opt* _temp1922=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1922->v=(
void*) Cyc_Absynpp_qvar2doc( _temp1868->name); _temp1922;})), Cyc_PP_text((
struct _tagged_string)({ char* _temp1923=( char*)" {"; struct _tagged_string
_temp1924; _temp1924.curr= _temp1923; _temp1924.base= _temp1923; _temp1924.last_plus_one=
_temp1923 + 3; _temp1924;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_stmt2doc( _temp1868->body))), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1925=( char*)"}"; struct _tagged_string
_temp1926; _temp1926.curr= _temp1925; _temp1926.base= _temp1925; _temp1926.last_plus_one=
_temp1925 + 2; _temp1926;}))); goto _LL1845;} _LL1849: if( _temp1870->fields ==
0){ s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(( void*) _temp1870->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp1927=( char*)"struct "; struct
_tagged_string _temp1928; _temp1928.curr= _temp1927; _temp1928.base= _temp1927;
_temp1928.last_plus_one= _temp1927 + 8; _temp1928;})), _temp1870->name == 0? Cyc_PP_text((
struct _tagged_string)({ char* _temp1929=( char*)""; struct _tagged_string
_temp1930; _temp1930.curr= _temp1929; _temp1930.base= _temp1929; _temp1930.last_plus_one=
_temp1929 + 1; _temp1930;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)( _temp1870->name)->v),
Cyc_Absynpp_tvars2doc( _temp1870->tvs), Cyc_PP_text(( struct _tagged_string)({
char* _temp1931=( char*)";"; struct _tagged_string _temp1932; _temp1932.curr=
_temp1931; _temp1932.base= _temp1931; _temp1932.last_plus_one= _temp1931 + 2;
_temp1932;})));} else{ s= Cyc_Absynpp_cat8( Cyc_Absynpp_scope2doc(( void*)
_temp1870->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp1933=( char*)"struct ";
struct _tagged_string _temp1934; _temp1934.curr= _temp1933; _temp1934.base=
_temp1933; _temp1934.last_plus_one= _temp1933 + 8; _temp1934;})), _temp1870->name
== 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp1935=( char*)""; struct
_tagged_string _temp1936; _temp1936.curr= _temp1935; _temp1936.base= _temp1935;
_temp1936.last_plus_one= _temp1935 + 1; _temp1936;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)( _temp1870->name)->v), Cyc_Absynpp_tvars2doc( _temp1870->tvs),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1937=( char*)" {"; struct
_tagged_string _temp1938; _temp1938.curr= _temp1937; _temp1938.base= _temp1937;
_temp1938.last_plus_one= _temp1937 + 3; _temp1938;})), Cyc_PP_nest( 2, _temp1870->fields
== 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp1939=( char*)""; struct
_tagged_string _temp1940; _temp1940.curr= _temp1939; _temp1940.base= _temp1939;
_temp1940.last_plus_one= _temp1939 + 1; _temp1940;})): Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_structfields2doc(( struct Cyc_List_List*)( _temp1870->fields)->v))),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp1941=( char*)"};";
struct _tagged_string _temp1942; _temp1942.curr= _temp1941; _temp1942.base=
_temp1941; _temp1942.last_plus_one= _temp1941 + 3; _temp1942;})));} goto _LL1845;
_LL1851: { struct Cyc_PP_Doc* sn= Cyc_Absynpp_qvar2doc( _temp1885); if(
_temp1877 > 0){ sn= Cyc_Absynpp_cat2( sn, Cyc_PP_text( xprintf("%d", _temp1877)));}
s= Cyc_Absynpp_cat4( Cyc_Absynpp_scope2doc( _temp1887), Cyc_Absynpp_tqtd2doc(
_temp1883, _temp1881,({ struct Cyc_Core_Opt* _temp1943=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp1943->v=( void*) sn; _temp1943;})),
_temp1879 == 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp1944=( char*)"";
struct _tagged_string _temp1945; _temp1945.curr= _temp1944; _temp1945.base=
_temp1944; _temp1945.last_plus_one= _temp1944 + 1; _temp1945;})): Cyc_Absynpp_cat2(
Cyc_PP_text(( struct _tagged_string)({ char* _temp1946=( char*)" = "; struct
_tagged_string _temp1947; _temp1947.curr= _temp1946; _temp1947.base= _temp1946;
_temp1947.last_plus_one= _temp1946 + 4; _temp1947;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1948= _temp1879; if(
_temp1948 == 0){ _throw( Null_Exception);} _temp1948;}))), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1949=( char*)";"; struct _tagged_string _temp1950;
_temp1950.curr= _temp1949; _temp1950.base= _temp1949; _temp1950.last_plus_one=
_temp1949 + 2; _temp1950;}))); goto _LL1845;} _LL1853: s= Cyc_PP_text(( struct
_tagged_string)({ char* _temp1951=( char*)"**UnionDecl**"; struct _tagged_string
_temp1952; _temp1952.curr= _temp1951; _temp1952.base= _temp1951; _temp1952.last_plus_one=
_temp1951 + 14; _temp1952;})); goto _LL1845; _LL1855: if( _temp1889->fields == 0){
s= Cyc_Absynpp_cat5( Cyc_Absynpp_scope2doc(( void*) _temp1889->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp1953=( char*)"enum "; struct _tagged_string
_temp1954; _temp1954.curr= _temp1953; _temp1954.base= _temp1953; _temp1954.last_plus_one=
_temp1953 + 6; _temp1954;})), _temp1889->name == 0? Cyc_PP_text(( struct
_tagged_string)({ char* _temp1955=( char*)""; struct _tagged_string _temp1956;
_temp1956.curr= _temp1955; _temp1956.base= _temp1955; _temp1956.last_plus_one=
_temp1955 + 1; _temp1956;})): Cyc_Absynpp_qvar2doc(( struct _tuple0*)( _temp1889->name)->v),
Cyc_Absynpp_tvars2doc( _temp1889->tvs), Cyc_PP_text(( struct _tagged_string)({
char* _temp1957=( char*)";"; struct _tagged_string _temp1958; _temp1958.curr=
_temp1957; _temp1958.base= _temp1957; _temp1958.last_plus_one= _temp1957 + 2;
_temp1958;})));} else{ s= Cyc_Absynpp_cat8( Cyc_Absynpp_scope2doc(( void*)
_temp1889->sc), Cyc_PP_text(( struct _tagged_string)({ char* _temp1959=( char*)"enum ";
struct _tagged_string _temp1960; _temp1960.curr= _temp1959; _temp1960.base=
_temp1959; _temp1960.last_plus_one= _temp1959 + 6; _temp1960;})), _temp1889->name
== 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp1961=( char*)""; struct
_tagged_string _temp1962; _temp1962.curr= _temp1961; _temp1962.base= _temp1961;
_temp1962.last_plus_one= _temp1961 + 1; _temp1962;})): Cyc_Absynpp_qvar2doc((
struct _tuple0*)( _temp1889->name)->v), Cyc_Absynpp_tvars2doc( _temp1889->tvs),
Cyc_PP_text(( struct _tagged_string)({ char* _temp1963=( char*)" {"; struct
_tagged_string _temp1964; _temp1964.curr= _temp1963; _temp1964.base= _temp1963;
_temp1964.last_plus_one= _temp1963 + 3; _temp1964;})), Cyc_PP_nest( 2, _temp1889->fields
== 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp1965=( char*)""; struct
_tagged_string _temp1966; _temp1966.curr= _temp1965; _temp1966.base= _temp1965;
_temp1966.last_plus_one= _temp1965 + 1; _temp1966;})): Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_enumfields2doc(( struct Cyc_List_List*)( _temp1889->fields)->v))),
Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({ char* _temp1967=( char*)"};";
struct _tagged_string _temp1968; _temp1968.curr= _temp1967; _temp1968.base=
_temp1967; _temp1968.last_plus_one= _temp1967 + 3; _temp1968;})));} goto _LL1845;
_LL1857: s= Cyc_Absynpp_cat4( Cyc_Absynpp_scope2doc(( void*) _temp1891->sc), Cyc_PP_text((
struct _tagged_string)({ char* _temp1969=( char*)"xenum "; struct _tagged_string
_temp1970; _temp1970.curr= _temp1969; _temp1970.base= _temp1969; _temp1970.last_plus_one=
_temp1969 + 7; _temp1970;})), Cyc_Absynpp_qvar2doc( _temp1891->name), _temp1891->fields
== 0? Cyc_PP_text(( struct _tagged_string)({ char* _temp1971=( char*)";"; struct
_tagged_string _temp1972; _temp1972.curr= _temp1971; _temp1972.base= _temp1971;
_temp1972.last_plus_one= _temp1971 + 2; _temp1972;})): Cyc_Absynpp_cat4( Cyc_PP_text((
struct _tagged_string)({ char* _temp1973=( char*)" {"; struct _tagged_string
_temp1974; _temp1974.curr= _temp1973; _temp1974.base= _temp1973; _temp1974.last_plus_one=
_temp1973 + 3; _temp1974;})), Cyc_PP_nest( 2, Cyc_Absynpp_cat2( Cyc_PP_line_doc(),
Cyc_Absynpp_enumfields2doc( _temp1891->fields))), Cyc_PP_line_doc(), Cyc_PP_text((
struct _tagged_string)({ char* _temp1975=( char*)"};"; struct _tagged_string
_temp1976; _temp1976.curr= _temp1975; _temp1976.base= _temp1975; _temp1976.last_plus_one=
_temp1975 + 3; _temp1976;})))); goto _LL1845; _LL1859: s= Cyc_Absynpp_cat5( Cyc_PP_text((
struct _tagged_string)({ char* _temp1977=( char*)"let "; struct _tagged_string
_temp1978; _temp1978.curr= _temp1977; _temp1978.base= _temp1977; _temp1978.last_plus_one=
_temp1977 + 5; _temp1978;})), Cyc_Absynpp_pat2doc( _temp1901), Cyc_PP_text((
struct _tagged_string)({ char* _temp1979=( char*)" = "; struct _tagged_string
_temp1980; _temp1980.curr= _temp1979; _temp1980.base= _temp1979; _temp1980.last_plus_one=
_temp1979 + 4; _temp1980;})), Cyc_Absynpp_exp2doc( _temp1895), Cyc_PP_text((
struct _tagged_string)({ char* _temp1981=( char*)";"; struct _tagged_string
_temp1982; _temp1982.curr= _temp1981; _temp1982.base= _temp1981; _temp1982.last_plus_one=
_temp1981 + 2; _temp1982;}))); goto _LL1845; _LL1861: s= Cyc_Absynpp_cat3( Cyc_PP_text((
struct _tagged_string)({ char* _temp1983=( char*)"typedef "; struct
_tagged_string _temp1984; _temp1984.curr= _temp1983; _temp1984.base= _temp1983;
_temp1984.last_plus_one= _temp1983 + 9; _temp1984;})), Cyc_Absynpp_tqtd2doc(({
struct Cyc_Absyn_Tqual* _temp1985=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp1985->q_const= 0; _temp1985->q_volatile=
0; _temp1985->q_restrict= 0; _temp1985;}),( void*) _temp1903->defn,({ struct Cyc_Core_Opt*
_temp1986=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp1986->v=( void*) Cyc_Absynpp_cat2( Cyc_Absynpp_qvar2doc( _temp1903->name),
Cyc_Absynpp_tvars2doc( _temp1903->tvs)); _temp1986;})), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1987=( char*)";"; struct _tagged_string _temp1988;
_temp1988.curr= _temp1987; _temp1988.base= _temp1987; _temp1988.last_plus_one=
_temp1987 + 2; _temp1988;}))); goto _LL1845; _LL1863: s= Cyc_Absynpp_cat7( Cyc_PP_text((
struct _tagged_string)({ char* _temp1989=( char*)"namespace "; struct
_tagged_string _temp1990; _temp1990.curr= _temp1989; _temp1990.base= _temp1989;
_temp1990.last_plus_one= _temp1989 + 11; _temp1990;})), Cyc_Absynpp_textptr(
_temp1907), Cyc_PP_text(( struct _tagged_string)({ char* _temp1991=( char*)" {";
struct _tagged_string _temp1992; _temp1992.curr= _temp1991; _temp1992.base=
_temp1991; _temp1992.last_plus_one= _temp1991 + 3; _temp1992;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp1993=( char*)""; struct _tagged_string
_temp1994; _temp1994.curr= _temp1993; _temp1994.base= _temp1993; _temp1994.last_plus_one=
_temp1993 + 1; _temp1994;}), _temp1905), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp1995=( char*)"}"; struct _tagged_string _temp1996;
_temp1996.curr= _temp1995; _temp1996.base= _temp1995; _temp1996.last_plus_one=
_temp1995 + 2; _temp1996;}))); goto _LL1845; _LL1865: s= Cyc_Absynpp_cat7( Cyc_PP_text((
struct _tagged_string)({ char* _temp1997=( char*)"using "; struct _tagged_string
_temp1998; _temp1998.curr= _temp1997; _temp1998.base= _temp1997; _temp1998.last_plus_one=
_temp1997 + 7; _temp1998;})), Cyc_Absynpp_qvar2doc( _temp1911), Cyc_PP_text((
struct _tagged_string)({ char* _temp1999=( char*)" {"; struct _tagged_string
_temp2000; _temp2000.curr= _temp1999; _temp2000.base= _temp1999; _temp2000.last_plus_one=
_temp1999 + 3; _temp2000;})), Cyc_PP_line_doc(),(( struct Cyc_PP_Doc*(*)( struct
Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct _tagged_string sep, struct
Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,( struct _tagged_string)({
char* _temp2001=( char*)""; struct _tagged_string _temp2002; _temp2002.curr=
_temp2001; _temp2002.base= _temp2001; _temp2002.last_plus_one= _temp2001 + 1;
_temp2002;}), _temp1909), Cyc_PP_line_doc(), Cyc_PP_text(( struct _tagged_string)({
char* _temp2003=( char*)"}"; struct _tagged_string _temp2004; _temp2004.curr=
_temp2003; _temp2004.base= _temp2003; _temp2004.last_plus_one= _temp2003 + 2;
_temp2004;}))); goto _LL1845; _LL1867: s= Cyc_Absynpp_cat5( Cyc_PP_text(( struct
_tagged_string)({ char* _temp2005=( char*)"extern \"C\" {"; struct
_tagged_string _temp2006; _temp2006.curr= _temp2005; _temp2006.base= _temp2005;
_temp2006.last_plus_one= _temp2005 + 13; _temp2006;})), Cyc_PP_line_doc(),((
struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(* pp)( struct Cyc_Absyn_Decl*), struct
_tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)( Cyc_Absynpp_decl2doc,(
struct _tagged_string)({ char* _temp2007=( char*)""; struct _tagged_string
_temp2008; _temp2008.curr= _temp2007; _temp2008.base= _temp2007; _temp2008.last_plus_one=
_temp2007 + 1; _temp2008;}), _temp1913), Cyc_PP_line_doc(), Cyc_PP_text(( struct
_tagged_string)({ char* _temp2009=( char*)"}"; struct _tagged_string _temp2010;
_temp2010.curr= _temp2009; _temp2010.base= _temp2009; _temp2010.last_plus_one=
_temp2009 + 2; _temp2010;}))); goto _LL1845; _LL1845:;} return s;} struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc( void* sc){ void* _temp2011= sc; _LL2013: if( _temp2011 ==
Cyc_Absyn_Static){ goto _LL2014;} else{ goto _LL2015;} _LL2015: if( _temp2011 ==
Cyc_Absyn_Public){ goto _LL2016;} else{ goto _LL2017;} _LL2017: if( _temp2011 ==
Cyc_Absyn_Extern){ goto _LL2018;} else{ goto _LL2019;} _LL2019: if( _temp2011 ==
Cyc_Absyn_ExternC){ goto _LL2020;} else{ goto _LL2021;} _LL2021: if( _temp2011
== Cyc_Absyn_Abstract){ goto _LL2022;} else{ goto _LL2012;} _LL2014: return Cyc_PP_text((
struct _tagged_string)({ char* _temp2023=( char*)"static "; struct
_tagged_string _temp2024; _temp2024.curr= _temp2023; _temp2024.base= _temp2023;
_temp2024.last_plus_one= _temp2023 + 8; _temp2024;})); _LL2016: return Cyc_PP_nil_doc();
_LL2018: return Cyc_PP_text(( struct _tagged_string)({ char* _temp2025=( char*)"extern ";
struct _tagged_string _temp2026; _temp2026.curr= _temp2025; _temp2026.base=
_temp2025; _temp2026.last_plus_one= _temp2025 + 8; _temp2026;})); _LL2020:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp2027=( char*)"extern \"C\"";
struct _tagged_string _temp2028; _temp2028.curr= _temp2027; _temp2028.base=
_temp2027; _temp2028.last_plus_one= _temp2027 + 11; _temp2028;})); _LL2022:
return Cyc_PP_text(( struct _tagged_string)({ char* _temp2029=( char*)"abstract ";
struct _tagged_string _temp2030; _temp2030.curr= _temp2029; _temp2030.base=
_temp2029; _temp2030.last_plus_one= _temp2029 + 10; _temp2030;})); _LL2012:;}
struct _tuple4 Cyc_Absynpp_to_tms( struct Cyc_Absyn_Tqual* tq, void* t){ void*
_temp2031= t; struct Cyc_Absyn_Exp* _temp2045; struct Cyc_Absyn_Tqual* _temp2047;
void* _temp2049; struct Cyc_Absyn_PtrInfo _temp2051; struct Cyc_Absyn_Conref*
_temp2053; struct Cyc_Absyn_Tqual* _temp2055; struct Cyc_Absyn_Conref* _temp2057;
void* _temp2059; void* _temp2061; struct Cyc_Absyn_FnInfo _temp2063; int
_temp2065; struct Cyc_List_List* _temp2067; void* _temp2069; struct Cyc_Core_Opt*
_temp2071; struct Cyc_List_List* _temp2073; int _temp2075; struct Cyc_Core_Opt*
_temp2077; void* _temp2079; struct Cyc_Core_Opt* _temp2081; struct Cyc_List_List*
_temp2083; struct _tuple0* _temp2085; _LL2033: if(( unsigned int) _temp2031 > 5u?((
struct _enum_struct*) _temp2031)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL2050:
_temp2049=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp2031)->f1; goto
_LL2048; _LL2048: _temp2047=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2031)->f2; goto _LL2046; _LL2046: _temp2045=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ArrayType_struct*) _temp2031)->f3; goto _LL2034;} else{ goto
_LL2035;} _LL2035: if(( unsigned int) _temp2031 > 5u?(( struct _enum_struct*)
_temp2031)->tag == Cyc_Absyn_PointerType_tag: 0){ _LL2052: _temp2051=( struct
Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*) _temp2031)->f1;
_LL2062: _temp2061=( void*) _temp2051.elt_typ; goto _LL2060; _LL2060: _temp2059=(
void*) _temp2051.rgn_typ; goto _LL2058; _LL2058: _temp2057=( struct Cyc_Absyn_Conref*)
_temp2051.nullable; goto _LL2056; _LL2056: _temp2055=( struct Cyc_Absyn_Tqual*)
_temp2051.tq; goto _LL2054; _LL2054: _temp2053=( struct Cyc_Absyn_Conref*)
_temp2051.bounds; goto _LL2036;} else{ goto _LL2037;} _LL2037: if(( unsigned int)
_temp2031 > 5u?(( struct _enum_struct*) _temp2031)->tag == Cyc_Absyn_FnType_tag:
0){ _LL2064: _temp2063=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp2031)->f1; _LL2074: _temp2073=( struct Cyc_List_List*) _temp2063.tvars;
goto _LL2072; _LL2072: _temp2071=( struct Cyc_Core_Opt*) _temp2063.effect; goto
_LL2070; _LL2070: _temp2069=( void*) _temp2063.ret_typ; goto _LL2068; _LL2068:
_temp2067=( struct Cyc_List_List*) _temp2063.args; goto _LL2066; _LL2066:
_temp2065=( int) _temp2063.varargs; goto _LL2038;} else{ goto _LL2039;} _LL2039:
if(( unsigned int) _temp2031 > 5u?(( struct _enum_struct*) _temp2031)->tag ==
Cyc_Absyn_Evar_tag: 0){ _LL2080: _temp2079=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp2031)->f1; goto _LL2078; _LL2078: _temp2077=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Evar_struct*) _temp2031)->f2; goto _LL2076; _LL2076: _temp2075=(
int)(( struct Cyc_Absyn_Evar_struct*) _temp2031)->f3; goto _LL2040;} else{ goto
_LL2041;} _LL2041: if(( unsigned int) _temp2031 > 5u?(( struct _enum_struct*)
_temp2031)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL2086: _temp2085=( struct
_tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp2031)->f1; goto _LL2084;
_LL2084: _temp2083=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp2031)->f2; goto _LL2082; _LL2082: _temp2081=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_TypedefType_struct*) _temp2031)->f3; goto _LL2042;} else{ goto
_LL2043;} _LL2043: goto _LL2044; _LL2034: { struct Cyc_List_List* _temp2089;
void* _temp2091; struct Cyc_Absyn_Tqual* _temp2093; struct _tuple4 _temp2087=
Cyc_Absynpp_to_tms( _temp2047, _temp2049); _LL2094: _temp2093= _temp2087.f1;
goto _LL2092; _LL2092: _temp2091= _temp2087.f2; goto _LL2090; _LL2090: _temp2089=
_temp2087.f3; goto _LL2088; _LL2088: { void* tm; if( Cyc_Evexp_eval_const_uint_exp(
_temp2045) ==( unsigned int) 0){ tm= Cyc_Absyn_Carray_mod;} else{ tm=({ struct
Cyc_Absyn_ConstArray_mod_struct* _temp2095=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2095->tag= Cyc_Absyn_ConstArray_mod_tag;
_temp2095->f1= _temp2045;( void*) _temp2095;});} return({ struct _tuple4
_temp2096; _temp2096.f1= _temp2093; _temp2096.f2= _temp2091; _temp2096.f3=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2097=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2097->hd=( void*) tm; _temp2097->tl=
_temp2089; _temp2097;}); _temp2096;});}} _LL2036: { struct Cyc_List_List*
_temp2100; void* _temp2102; struct Cyc_Absyn_Tqual* _temp2104; struct _tuple4
_temp2098= Cyc_Absynpp_to_tms( _temp2055, _temp2061); _LL2105: _temp2104=
_temp2098.f1; goto _LL2103; _LL2103: _temp2102= _temp2098.f2; goto _LL2101;
_LL2101: _temp2100= _temp2098.f3; goto _LL2099; _LL2099: { void* ps;{ struct
_tuple9 _temp2107=({ struct _tuple9 _temp2106; _temp2106.f1=( void*)((( struct
Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
_temp2057))->v; _temp2106.f2=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp2053))->v; _temp2106;}); void* _temp2117;
void* _temp2119; struct Cyc_Absyn_Exp* _temp2121; void* _temp2123; int _temp2125;
void* _temp2127; void* _temp2129; struct Cyc_Absyn_Exp* _temp2131; void*
_temp2133; int _temp2135; void* _temp2137; void* _temp2139; void* _temp2141;
_LL2109: _LL2124: _temp2123= _temp2107.f1; if(( unsigned int) _temp2123 > 1u?((
struct _enum_struct*) _temp2123)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2126:
_temp2125=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2123)->f1; if(
_temp2125 == 1){ goto _LL2118;} else{ goto _LL2111;}} else{ goto _LL2111;}
_LL2118: _temp2117= _temp2107.f2; if(( unsigned int) _temp2117 > 1u?(( struct
_enum_struct*) _temp2117)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2120:
_temp2119=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2117)->f1; if((
unsigned int) _temp2119 > 1u?(( struct _enum_struct*) _temp2119)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2122: _temp2121=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2119)->f1; goto _LL2110;} else{ goto _LL2111;}} else{ goto _LL2111;}
_LL2111: _LL2134: _temp2133= _temp2107.f1; if(( unsigned int) _temp2133 > 1u?((
struct _enum_struct*) _temp2133)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2136:
_temp2135=( int)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2133)->f1; if(
_temp2135 == 0){ goto _LL2128;} else{ goto _LL2113;}} else{ goto _LL2113;}
_LL2128: _temp2127= _temp2107.f2; if(( unsigned int) _temp2127 > 1u?(( struct
_enum_struct*) _temp2127)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2130:
_temp2129=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp2127)->f1; if((
unsigned int) _temp2129 > 1u?(( struct _enum_struct*) _temp2129)->tag == Cyc_Absyn_Upper_b_tag:
0){ _LL2132: _temp2131=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Upper_b_struct*)
_temp2129)->f1; goto _LL2112;} else{ goto _LL2113;}} else{ goto _LL2113;}
_LL2113: _LL2142: _temp2141= _temp2107.f1; goto _LL2138; _LL2138: _temp2137=
_temp2107.f2; if(( unsigned int) _temp2137 > 1u?(( struct _enum_struct*)
_temp2137)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL2140: _temp2139=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp2137)->f1; if( _temp2139 == Cyc_Absyn_Unknown_b){
goto _LL2114;} else{ goto _LL2115;}} else{ goto _LL2115;} _LL2115: goto _LL2116;
_LL2110: ps=({ struct Cyc_Absyn_Nullable_ps_struct* _temp2143=( struct Cyc_Absyn_Nullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct)); _temp2143->tag= Cyc_Absyn_Nullable_ps_tag;
_temp2143->f1= _temp2121;( void*) _temp2143;}); goto _LL2108; _LL2112: ps=({
struct Cyc_Absyn_NonNullable_ps_struct* _temp2144=( struct Cyc_Absyn_NonNullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct)); _temp2144->tag= Cyc_Absyn_NonNullable_ps_tag;
_temp2144->f1= _temp2131;( void*) _temp2144;}); goto _LL2108; _LL2114: ps= Cyc_Absyn_TaggedArray_ps;
goto _LL2108; _LL2116: ps= Cyc_Absyn_TaggedArray_ps; goto _LL2108; _LL2108:;}{
void* tm=({ struct Cyc_Absyn_Pointer_mod_struct* _temp2145=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2145->tag= Cyc_Absyn_Pointer_mod_tag;
_temp2145->f1=( void*) ps; _temp2145->f2=( void*) _temp2059; _temp2145->f3= tq;(
void*) _temp2145;}); return({ struct _tuple4 _temp2146; _temp2146.f1= _temp2104;
_temp2146.f2= _temp2102; _temp2146.f3=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp2147=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2147->hd=( void*) tm; _temp2147->tl= _temp2100; _temp2147;}); _temp2146;});}}}
_LL2038: { struct Cyc_List_List* _temp2150; void* _temp2152; struct Cyc_Absyn_Tqual*
_temp2154; struct _tuple4 _temp2148= Cyc_Absynpp_to_tms( Cyc_Absyn_empty_tqual(),
_temp2069); _LL2155: _temp2154= _temp2148.f1; goto _LL2153; _LL2153: _temp2152=
_temp2148.f2; goto _LL2151; _LL2151: _temp2150= _temp2148.f3; goto _LL2149;
_LL2149: { struct Cyc_List_List* tms=({ struct Cyc_List_List* _temp2156=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2156->hd=( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2157=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2157->tag= Cyc_Absyn_Function_mod_tag;
_temp2157->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2158=( struct
Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2158->tag= Cyc_Absyn_WithTypes_tag; _temp2158->f1= _temp2067; _temp2158->f2=
_temp2065; _temp2158->f3= _temp2071;( void*) _temp2158;});( void*) _temp2157;});
_temp2156->tl= _temp2150; _temp2156;}); if( _temp2073 != 0){ tms=({ struct Cyc_List_List*
_temp2159=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2159->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp2160=(
struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct));
_temp2160->tag= Cyc_Absyn_TypeParams_mod_tag; _temp2160->f1= _temp2073;
_temp2160->f2= 0; _temp2160->f3= 1;( void*) _temp2160;}); _temp2159->tl= tms;
_temp2159;});} return({ struct _tuple4 _temp2161; _temp2161.f1= _temp2154;
_temp2161.f2= _temp2152; _temp2161.f3= tms; _temp2161;});}} _LL2040: if(
_temp2077 == 0){ return({ struct _tuple4 _temp2162; _temp2162.f1= tq; _temp2162.f2=
t; _temp2162.f3= 0; _temp2162;});} else{ return Cyc_Absynpp_to_tms( tq,( void*)
_temp2077->v);} _LL2042: if( _temp2081 == 0? 1: ! Cyc_Absynpp_expand_typedefs){
return({ struct _tuple4 _temp2163; _temp2163.f1= tq; _temp2163.f2= t; _temp2163.f3=
0; _temp2163;});} else{ return Cyc_Absynpp_to_tms( tq,( void*) _temp2081->v);}
_LL2044: return({ struct _tuple4 _temp2164; _temp2164.f1= tq; _temp2164.f2= t;
_temp2164.f3= 0; _temp2164;}); _LL2032:;} struct Cyc_PP_Doc* Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual* tq, void* t, struct Cyc_Core_Opt* dopt){ struct Cyc_List_List*
_temp2167; void* _temp2169; struct Cyc_Absyn_Tqual* _temp2171; struct _tuple4
_temp2165= Cyc_Absynpp_to_tms( tq, t); _LL2172: _temp2171= _temp2165.f1; goto
_LL2170; _LL2170: _temp2169= _temp2165.f2; goto _LL2168; _LL2168: _temp2167=
_temp2165.f3; goto _LL2166; _LL2166: _temp2167=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp2167); if( _temp2167 == 0?
dopt == 0: 0){ return Cyc_Absynpp_cat2( Cyc_Absynpp_tqual2doc( _temp2171), Cyc_Absynpp_ntyp2doc(
_temp2169));} else{ return Cyc_Absynpp_cat4( Cyc_Absynpp_tqual2doc( _temp2171),
Cyc_Absynpp_ntyp2doc( _temp2169), Cyc_PP_text(( struct _tagged_string)({ char*
_temp2173=( char*)" "; struct _tagged_string _temp2174; _temp2174.curr=
_temp2173; _temp2174.base= _temp2173; _temp2174.last_plus_one= _temp2173 + 2;
_temp2174;})), Cyc_Absynpp_dtms2doc( dopt == 0? Cyc_PP_nil_doc():( struct Cyc_PP_Doc*)
dopt->v, _temp2167));}} struct Cyc_PP_Doc* Cyc_Absynpp_structfield2doc( struct
_tuple3* f){ return Cyc_Absynpp_cat2( Cyc_Absynpp_tqtd2doc((* f).f2,(* f).f3,({
struct Cyc_Core_Opt* _temp2175=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2175->v=( void*) Cyc_Absynpp_textptr((* f).f1); _temp2175;})),
Cyc_PP_text(( struct _tagged_string)({ char* _temp2176=( char*)";"; struct
_tagged_string _temp2177; _temp2177.curr= _temp2176; _temp2177.base= _temp2176;
_temp2177.last_plus_one= _temp2176 + 2; _temp2177;})));} struct Cyc_PP_Doc* Cyc_Absynpp_structfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct _tuple3*), struct _tagged_string sep, struct Cyc_List_List* l0)) Cyc_PP_ppseql)(
Cyc_Absynpp_structfield2doc,( struct _tagged_string)({ char* _temp2178=( char*)"";
struct _tagged_string _temp2179; _temp2179.curr= _temp2178; _temp2179.base=
_temp2178; _temp2179.last_plus_one= _temp2178 + 1; _temp2179;}), fields);}
struct Cyc_PP_Doc* Cyc_Absynpp_enumfield2doc( struct Cyc_Absyn_Enumfield* f){
return Cyc_Absynpp_cat3( Cyc_Absynpp_qvar2doc( f->name), f->tag == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_cat2( Cyc_PP_text(( struct _tagged_string)({ char* _temp2180=( char*)"=";
struct _tagged_string _temp2181; _temp2181.curr= _temp2180; _temp2181.base=
_temp2180; _temp2181.last_plus_one= _temp2180 + 2; _temp2181;})), Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp2182= f->tag; if( _temp2182
== 0){ _throw( Null_Exception);} _temp2182;}))), f->typs == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc( f->typs));} struct Cyc_PP_Doc* Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List* fields){ return(( struct Cyc_PP_Doc*(*)( struct Cyc_PP_Doc*(*
pp)( struct Cyc_Absyn_Enumfield*), struct _tagged_string sep, struct Cyc_List_List*
l0)) Cyc_PP_ppseql)( Cyc_Absynpp_enumfield2doc,( struct _tagged_string)({ char*
_temp2183=( char*)","; struct _tagged_string _temp2184; _temp2184.curr=
_temp2183; _temp2184.base= _temp2183; _temp2184.last_plus_one= _temp2183 + 2;
_temp2184;}), fields);} void Cyc_Absynpp_decllist2file( struct Cyc_List_List*
tdl, struct Cyc_Stdio___sFILE* f){ for( 0; tdl != 0; tdl= tdl->tl){ Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc(( struct Cyc_Absyn_Decl*) tdl->hd), 72, f); fprintf( f,"\n");}}
struct _tagged_string Cyc_Absynpp_decllist2string( struct Cyc_List_List* tdl){
return Cyc_PP_string_of_doc( Cyc_PP_seql(( struct _tagged_string)({ char*
_temp2185=( char*)""; struct _tagged_string _temp2186; _temp2186.curr= _temp2185;
_temp2186.base= _temp2185; _temp2186.last_plus_one= _temp2185 + 1; _temp2186;}),((
struct Cyc_List_List*(*)( struct Cyc_PP_Doc*(* f)( struct Cyc_Absyn_Decl*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_decl2doc, tdl)), 72);}
struct _tagged_string Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp* e){ return
Cyc_PP_string_of_doc( Cyc_Absynpp_exp2doc( e), 72);} struct _tagged_string Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt* s){ return Cyc_PP_string_of_doc( Cyc_Absynpp_stmt2doc( s),
72);} struct _tagged_string Cyc_Absynpp_typ2string( void* t){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_typ2doc( t), 72);} struct _tagged_string Cyc_Absynpp_qvar2string(
struct _tuple0* v){ return Cyc_PP_string_of_doc( Cyc_Absynpp_qvar2doc( v), 72);}
struct _tagged_string Cyc_Absynpp_prim2string( void* p){ return Cyc_PP_string_of_doc(
Cyc_Absynpp_prim2doc( p), 72);} struct _tagged_string Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat* p){ return Cyc_PP_string_of_doc( Cyc_Absynpp_pat2doc( p),
72);}