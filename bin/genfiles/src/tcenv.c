#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2;}; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3;}; struct _tuple2{ struct
Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2;}; struct _tuple3{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2;}; struct _tuple4{ struct Cyc_Absyn_Stmt* f1;
struct Cyc_List_List* f2; struct Cyc_List_List* f3;}; struct _tuple5{ void* f1;
int f2;}; struct _tuple6{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual* f2;
void* f3;}; typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef
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
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stderr; typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t;
extern int Cyc_Stdio_fflush( struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileOpenError_tag[
14u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;};
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag;}; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;}; typedef
struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t;
typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag;}; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag;}; extern struct Cyc_List_List*
Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag;}; struct Cyc_Dict_Dict; typedef struct
Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;};
extern char Cyc_Dict_Absent_tag[ 7u]; struct Cyc_Dict_Absent_struct{ char* tag;};
extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*)); extern
int Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Dict_Dict*
Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void* data); extern void*
Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Core_Opt*
Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key); struct Cyc_Set_Set;
typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern struct Cyc_Set_Set* Cyc_Set_empty(
int(* comp)( void*, void*)); extern int Cyc_Set_member( struct Cyc_Set_Set* s,
void* elt); extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_SlowDict_Dict;
typedef struct Cyc_SlowDict_Dict* Cyc_SlowDict_hdict_t; typedef struct Cyc_SlowDict_Dict*
Cyc_SlowDict_dict_t; extern char Cyc_SlowDict_Present_tag[ 8u]; extern char Cyc_SlowDict_Absent_tag[
7u]; extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_empty( int(* comp)( void*,
void*)); extern int Cyc_SlowDict_is_empty( struct Cyc_SlowDict_Dict* d); extern
struct Cyc_SlowDict_Dict* Cyc_SlowDict_insert( struct Cyc_SlowDict_Dict* d, void*
key, void* data); extern struct Cyc_Core_Opt* Cyc_SlowDict_lookup_opt( struct
Cyc_SlowDict_Dict* d, void* key); extern struct Cyc_SlowDict_Dict* Cyc_SlowDict_delete(
struct Cyc_SlowDict_Dict* d, void* key); struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col;};
typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
5u]; struct Cyc_Position_Exit_struct{ char* tag;}; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; typedef void* Cyc_Position_Error_kind;
extern void* Cyc_Position_Lex; extern void* Cyc_Position_Parse; extern void* Cyc_Position_Elab;
typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc;}; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag;}; extern int Cyc_String_zstrptrcmp( struct _tagged_string*, struct
_tagged_string*); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; typedef struct _tagged_string* Cyc_Absyn_field_name;
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
int tag; struct Cyc_List_List* f1; struct Cyc_Absyn_Stmt* f2;}; extern const int
Cyc_Absyn_Decl_s_tag; struct Cyc_Absyn_Decl_s_struct{ int tag; struct Cyc_Absyn_Decl*
f1; struct Cyc_Absyn_Stmt* f2;}; extern const int Cyc_Absyn_Cut_s_tag; struct
Cyc_Absyn_Cut_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1;}; extern const int
Cyc_Absyn_Splice_s_tag; struct Cyc_Absyn_Splice_s_struct{ int tag; struct Cyc_Absyn_Stmt*
f1;}; extern const int Cyc_Absyn_Label_s_tag; struct Cyc_Absyn_Label_s_struct{
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
struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ;}; struct Cyc_Absyn_Structdecl{
void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields;}; struct Cyc_Absyn_Enumfield{ struct _tuple0* name; struct Cyc_Absyn_Exp*
tag; struct Cyc_List_List* tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment*
loc;}; struct Cyc_Absyn_Enumdecl{ void* sc; struct Cyc_Core_Opt* name; struct
Cyc_List_List* tvs; struct Cyc_Core_Opt* fields;}; struct Cyc_Absyn_Xenumdecl{
void* sc; struct _tuple0* name; struct Cyc_List_List* fields;}; struct Cyc_Absyn_Typedefdecl{
struct _tuple0* name; struct Cyc_List_List* tvs; void* defn;}; extern const int
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
int tag; struct _tuple0* f1; struct Cyc_List_List* f2;}; extern const int Cyc_Absyn_ExternC_d_tag;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1;}; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc;}; extern const int
Cyc_Absyn_ArrayElement_tag; struct Cyc_Absyn_ArrayElement_struct{ int tag;
struct Cyc_Absyn_Exp* f1;}; extern const int Cyc_Absyn_FieldName_tag; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_string* f1;}; extern char Cyc_Absyn_EmptyAnnot_tag[ 11u];
struct Cyc_Absyn_EmptyAnnot_struct{ char* tag;}; extern int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*, struct Cyc_List_List*); extern struct _tuple0* Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Xenumdecl* Cyc_Absyn_exn_xed; extern void* Cyc_Absyn_exn_typ;
extern struct _tagged_string Cyc_Absynpp_typ2string( void*); extern struct
_tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*); typedef void* Cyc_Tcenv_Resolved;
extern const int Cyc_Tcenv_VarRes_tag; struct Cyc_Tcenv_VarRes_struct{ int tag;
void* f1;}; extern const int Cyc_Tcenv_StructRes_tag; struct Cyc_Tcenv_StructRes_struct{
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
Cyc_Tcenv_tenv; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern struct
Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(); extern struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv();
extern struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct Cyc_Absyn_Fndecl*);
extern struct Cyc_List_List* Cyc_Tcenv_resolve_namespace( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tagged_string*, struct Cyc_List_List*);
extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple0*); extern struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xenumdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern void* Cyc_Tcenv_return_typ(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*); extern struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* continue_dest); extern struct
Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch( struct Cyc_Tcenv_Tenv*); extern struct
Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru( struct Cyc_Tcenv_Tenv* te, struct
_tuple3* pat_typ, struct Cyc_Absyn_Stmt* body); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_clear_fallthru( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_next( struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Stmt**); extern
void Cyc_Tcenv_process_break( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Stmt**); extern void Cyc_Tcenv_process_goto( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*, struct _tagged_string*, struct Cyc_Absyn_Stmt**); extern
struct _tuple4* Cyc_Tcenv_process_fallthru( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Stmt**); extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_label( struct Cyc_Tcenv_Tenv*, struct _tagged_string*, struct Cyc_Absyn_Stmt*);
extern int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*); extern struct
Cyc_Tcenv_Tenv* Cyc_Tcenv_new_block( struct Cyc_Tcenv_Tenv*); extern void* Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*); extern void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, void* rgn); extern void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff); extern
char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag;};
extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_string);
extern struct Cyc_Core_Opt* Cyc_Tcutil_empty_var_set; extern int Cyc_Tcutil_subset_effect(
int set_to_empty, void* e1, void* e2); extern int Cyc_Tcutil_region_in_effect(
int constrain, void* r, void* e); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar( void* k); char Cyc_Tcenv_Env_error_tag[ 10u]="Env_error";
struct Cyc_Tcenv_Env_error_struct{ char* tag;}; void* Cyc_Tcenv_env_err( struct
_tagged_string msg){({ struct _tagged_string _temp0= msg; fprintf( Cyc_Stdio_stderr,"Error: %.*s\n",
_temp0.last_plus_one - _temp0.curr, _temp0.curr);}); Cyc_Stdio_fflush(( struct
Cyc_Stdio___sFILE*) Cyc_Stdio_stderr);( void) _throw(({ struct Cyc_Tcenv_Env_error_struct*
_temp1=( struct Cyc_Tcenv_Env_error_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Tcenv_Env_error_struct));* _temp1=( struct Cyc_Tcenv_Env_error_struct){.tag=
Cyc_Tcenv_Env_error_tag};( struct _xenum_struct*) _temp1;}));} struct Cyc_Tcenv_Tenv;
struct Cyc_Tcenv_Genv; const int Cyc_Tcenv_VarRes_tag= 0; const int Cyc_Tcenv_StructRes_tag=
1; const int Cyc_Tcenv_EnumRes_tag= 2; const int Cyc_Tcenv_XenumRes_tag= 3;
const int Cyc_Tcenv_Outermost_tag= 0; const int Cyc_Tcenv_Frame_tag= 1; const
int Cyc_Tcenv_Hidden_tag= 2; void* Cyc_Tcenv_NotLoop_j=( void*) 0; void* Cyc_Tcenv_CaseEnd_j=(
void*) 1; void* Cyc_Tcenv_FnEnd_j=( void*) 2; const int Cyc_Tcenv_Stmt_j_tag= 0;
struct Cyc_Tcenv_CtrlEnv{ void* continue_stmt; void* break_stmt; struct _tuple4*
fallthru_stmt; void* next_stmt;}; typedef struct Cyc_Tcenv_CtrlEnv* Cyc_Tcenv_ctrl_env_t;
struct Cyc_Tcenv_SharedFenv{ void* return_typ; struct Cyc_Dict_Dict* seen_labels;
struct Cyc_SlowDict_Dict* needed_labels;}; struct Cyc_Tcenv_Fenv{ struct Cyc_Tcenv_SharedFenv*
shared; struct Cyc_List_List* type_vars; struct Cyc_Dict_Dict* locals; struct
Cyc_Absyn_Stmt* encloser; struct Cyc_Tcenv_CtrlEnv* ctrl_env; void* capability;
void* curr_rgn;}; struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init(){ Cyc_Tcutil_empty_var_set=({
struct Cyc_Core_Opt* _temp2=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2->v=( void*)(( struct Cyc_Set_Set*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp);
_temp2;});{ struct Cyc_Tcenv_Genv* ge= Cyc_Tcenv_empty_genv(); struct Cyc_List_List*
top_ns= 0; Cyc_Absyn_exn_xed=({ struct Cyc_Absyn_Xenumdecl* _temp3=( struct Cyc_Absyn_Xenumdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Xenumdecl) * 1); _temp3[ 0]=({ struct Cyc_Absyn_Xenumdecl
_temp4; _temp4.sc=( void*) Cyc_Absyn_Extern; _temp4.name= Cyc_Absyn_exn_name;
_temp4.fields= 0; _temp4;}); _temp3;}); Cyc_Absyn_exn_typ=({ struct Cyc_Absyn_XenumType_struct*
_temp5=( struct Cyc_Absyn_XenumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XenumType_struct));
_temp5->tag= Cyc_Absyn_XenumType_tag; _temp5->f1= Cyc_Absyn_exn_name; _temp5->f2=(
struct Cyc_Absyn_Xenumdecl**)& Cyc_Absyn_exn_xed;( void*) _temp5;}); ge->xenumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Xenumdecl** data)) Cyc_Dict_insert)( ge->xenumdecls,(* Cyc_Absyn_exn_name).f2,&
Cyc_Absyn_exn_xed);{ struct Cyc_Dict_Dict* ae=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Tcenv_Genv* data))
Cyc_Dict_insert)((( struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*,
struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp), top_ns, ge);
return({ struct Cyc_Tcenv_Tenv* _temp6=( struct Cyc_Tcenv_Tenv*) GC_malloc(
sizeof( struct Cyc_Tcenv_Tenv)); _temp6->ns= top_ns; _temp6->ae= ae; _temp6->le=
0; _temp6;});}}} static struct Cyc_Tcenv_Genv* Cyc_Tcenv_lookup_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct
_tagged_string* n, struct Cyc_List_List* ns){ return(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
Cyc_Tcenv_resolve_namespace( te, loc, n, ns));} static struct Cyc_List_List* Cyc_Tcenv_outer_namespace(
struct Cyc_List_List* ns){ if( ns == 0){ return(( struct Cyc_List_List*(*)(
struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp7=( char*)"Tcenv::outer_namespace"; struct _tagged_string _temp8; _temp8.curr=
_temp7; _temp8.base= _temp7; _temp8.last_plus_one= _temp7 + 23; _temp8;}));}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)((((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( ns))->tl);}
static void Cyc_Tcenv_check_repeat( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void*(* lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct
_tagged_string* v, struct Cyc_List_List* nss){ for( 0; nss != 0; nss= nss->tl){
struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)
nss->hd); struct _handler_cons _temp9; _push_handler(& _temp9);{ struct
_xenum_struct* _temp10=( struct _xenum_struct*) setjmp( _temp9.handler); if( !
_temp10){ lookup( ge2, v); Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp11=*
v; xprintf("%.*s is ambiguous", _temp11.last_plus_one - _temp11.curr, _temp11.curr);}));
_pop_handler();} else{ struct _xenum_struct* _temp13= _temp10; _LL15: if(
_temp13->tag == Cyc_Dict_Absent_tag){ goto _LL16;} else{ goto _LL17;} _LL17:
goto _LL18; _LL16: goto _LL14; _LL18:( void) _throw( _temp13); _LL14:;}}}
return;} static void* Cyc_Tcenv_scoped_lookup( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v){ struct Cyc_List_List* cns= te->ns;
void* result; while( 1) { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
cns);{ struct _handler_cons _temp19; _push_handler(& _temp19);{ struct
_xenum_struct* _temp20=( struct _xenum_struct*) setjmp( _temp19.handler); if( !
_temp20){ result= lookup( ge, v);(( void(*)( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v, struct Cyc_List_List* nss)) Cyc_Tcenv_check_repeat)(
te, loc, lookup, v, ge->availables);{ void* _temp21= result; _npop_handler( 0u);
return _temp21;} _pop_handler();} else{ struct _xenum_struct* _temp23= _temp20;
_LL25: if( _temp23->tag == Cyc_Dict_Absent_tag){ goto _LL26;} else{ goto _LL27;}
_LL27: goto _LL28; _LL26: goto _LL24; _LL28:( void) _throw( _temp23); _LL24:;}}}{
struct Cyc_List_List* nss= ge->availables; for( 0; nss != 0; nss= nss->tl){
struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,( struct Cyc_List_List*)
nss->hd); struct _handler_cons _temp29; _push_handler(& _temp29);{ struct
_xenum_struct* _temp30=( struct _xenum_struct*) setjmp( _temp29.handler); if( !
_temp30){ result= lookup( ge2, v);(( void(*)( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void*(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v, struct Cyc_List_List* nss)) Cyc_Tcenv_check_repeat)(
te, loc, lookup, v, nss->tl);{ void* _temp31= result; _npop_handler( 0u); return
_temp31;} _pop_handler();} else{ struct _xenum_struct* _temp33= _temp30; _LL35:
if( _temp33->tag == Cyc_Dict_Absent_tag){ goto _LL36;} else{ goto _LL37;} _LL37:
goto _LL38; _LL36: goto _LL34; _LL38:( void) _throw( _temp33); _LL34:;}}}} if(
cns == 0){( void) _throw(({ struct Cyc_Dict_Absent_struct* _temp39=( struct Cyc_Dict_Absent_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Dict_Absent_struct));* _temp39=( struct Cyc_Dict_Absent_struct){.tag=
Cyc_Dict_Absent_tag};( struct _xenum_struct*) _temp39;}));} cns= Cyc_Tcenv_outer_namespace(
cns);}} static void* Cyc_Tcenv_lookup_ordinary_global_f( struct Cyc_Tcenv_Genv*
ge, struct _tagged_string* v){ struct _tuple5* ans=(( struct _tuple5*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries,
v);(* ans).f2= 1; return(* ans).f1;} static void* Cyc_Tcenv_lookup_ordinary_global(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct _tuple0* q){
struct _tuple0 _temp42; struct _tagged_string* _temp43; void* _temp45; struct
_tuple0* _temp40= q; _temp42=* _temp40; _LL46: _temp45= _temp42.f1; goto _LL44;
_LL44: _temp43= _temp42.f2; goto _LL41; _LL41: { void* _temp47= _temp45; struct
Cyc_List_List* _temp57; struct Cyc_List_List* _temp59; struct Cyc_List_List
_temp61; struct Cyc_List_List* _temp62; struct _tagged_string* _temp64; struct
Cyc_List_List* _temp66; _LL49: if( _temp47 == Cyc_Absyn_Loc_n){ goto _LL50;}
else{ goto _LL51;} _LL51: if(( unsigned int) _temp47 > 1u?(( struct _enum_struct*)
_temp47)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL58: _temp57=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp47)->f1; if( _temp57 == 0){ goto _LL52;}
else{ goto _LL53;}} else{ goto _LL53;} _LL53: if(( unsigned int) _temp47 > 1u?((
struct _enum_struct*) _temp47)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL60: _temp59=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp47)->f1; if(
_temp59 == 0){ goto _LL55;} else{ _temp61=* _temp59; _LL65: _temp64=( struct
_tagged_string*) _temp61.hd; goto _LL63; _LL63: _temp62=( struct Cyc_List_List*)
_temp61.tl; goto _LL54;}} else{ goto _LL55;} _LL55: if(( unsigned int) _temp47 >
1u?(( struct _enum_struct*) _temp47)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL67:
_temp66=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp47)->f1;
goto _LL56;} else{ goto _LL48;} _LL50: goto _LL52; _LL52: return(( void*(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void*(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)(
te, loc, Cyc_Tcenv_lookup_ordinary_global_f, _temp43); _LL54: { struct Cyc_Tcenv_Genv*
ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp64, _temp62); return Cyc_Tcenv_lookup_ordinary_global_f(
ge, _temp43);} _LL56: return Cyc_Tcenv_lookup_ordinary_global_f((( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp66), _temp43); _LL48:;}} struct Cyc_List_List* Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct
_tagged_string* n, struct Cyc_List_List* ns){ struct Cyc_List_List* cns= te->ns;
struct Cyc_List_List* possible_results= 0; while( 1) { struct Cyc_Tcenv_Genv* ge=((
struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)( te->ae, cns);{ struct Cyc_List_List* nss= ge->availables; for(
0; nss != 0; nss= nss->tl){ struct Cyc_Tcenv_Genv* ge2=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,(
struct Cyc_List_List*) nss->hd); if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_string* elt)) Cyc_Set_member)( ge2->namespaces, n)){ possible_results=({
struct Cyc_List_List* _temp68=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp68->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( cns,({ struct Cyc_List_List*
_temp69=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp69->hd=( void*) n; _temp69->tl= ns; _temp69;})); _temp68->tl=
possible_results; _temp68;});}}} if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_string* elt)) Cyc_Set_member)( ge->namespaces, n)){ possible_results=({
struct Cyc_List_List* _temp70=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp70->hd=( void*)(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( cns,({ struct Cyc_List_List*
_temp71=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp71->hd=( void*) n; _temp71->tl= ns; _temp71;})); _temp70->tl=
possible_results; _temp70;});} if( possible_results != 0){ if( possible_results->tl
!= 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp72=* n; xprintf("%.*s is ambiguous",
_temp72.last_plus_one - _temp72.curr, _temp72.curr);}));} return( struct Cyc_List_List*)
possible_results->hd;} if( cns == 0){( void) _throw(({ struct Cyc_Dict_Absent_struct*
_temp73=( struct Cyc_Dict_Absent_struct*) GC_malloc_atomic( sizeof( struct Cyc_Dict_Absent_struct));*
_temp73=( struct Cyc_Dict_Absent_struct){.tag= Cyc_Dict_Absent_tag};( struct
_xenum_struct*) _temp73;}));} cns= Cyc_Tcenv_outer_namespace( cns);}} static
struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl_f( struct Cyc_Tcenv_Genv*
ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Structdecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->structdecls,
v);} struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp76;
struct _tagged_string* _temp77; void* _temp79; struct _tuple0* _temp74= q;
_temp76=* _temp74; _LL80: _temp79= _temp76.f1; goto _LL78; _LL78: _temp77=
_temp76.f2; goto _LL75; _LL75: { void* _temp81= _temp79; struct Cyc_List_List*
_temp91; struct Cyc_List_List* _temp93; struct Cyc_List_List* _temp95; struct
Cyc_List_List _temp97; struct Cyc_List_List* _temp98; struct _tagged_string*
_temp100; _LL83: if( _temp81 == Cyc_Absyn_Loc_n){ goto _LL84;} else{ goto _LL85;}
_LL85: if(( unsigned int) _temp81 > 1u?(( struct _enum_struct*) _temp81)->tag ==
Cyc_Absyn_Rel_n_tag: 0){ _LL92: _temp91=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp81)->f1; if( _temp91 == 0){ goto _LL86;} else{ goto _LL87;}} else{ goto
_LL87;} _LL87: if(( unsigned int) _temp81 > 1u?(( struct _enum_struct*) _temp81)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL94: _temp93=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp81)->f1; goto _LL88;} else{ goto _LL89;} _LL89: if(( unsigned int) _temp81
> 1u?(( struct _enum_struct*) _temp81)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL96:
_temp95=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp81)->f1;
if( _temp95 == 0){ goto _LL82;} else{ _temp97=* _temp95; _LL101: _temp100=(
struct _tagged_string*) _temp97.hd; goto _LL99; _LL99: _temp98=( struct Cyc_List_List*)
_temp97.tl; goto _LL90;}} else{ goto _LL82;} _LL84: goto _LL86; _LL86: return((
struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Structdecl**(* lookup)( struct Cyc_Tcenv_Genv*, struct
_tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)( te, loc,
Cyc_Tcenv_lookup_structdecl_f, _temp77); _LL88: { struct Cyc_Tcenv_Genv* ge=((
struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)( te->ae, _temp93); return(( struct Cyc_Absyn_Structdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->structdecls,
_temp77);} _LL90: { struct Cyc_Tcenv_Genv* ge= Cyc_Tcenv_lookup_namespace( te,
loc, _temp100, _temp98); return(( struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->structdecls, _temp77);}
_LL82:;}} static struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl_f( struct
Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls,
v);} struct Cyc_Absyn_Enumdecl** Cyc_Tcenv_lookup_enumdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp104; struct _tagged_string* _temp105; void* _temp107; struct _tuple0*
_temp102= q; _temp104=* _temp102; _LL108: _temp107= _temp104.f1; goto _LL106;
_LL106: _temp105= _temp104.f2; goto _LL103; _LL103: { void* _temp109= _temp107;
struct Cyc_List_List* _temp119; struct Cyc_List_List* _temp121; struct Cyc_List_List
_temp123; struct Cyc_List_List* _temp124; struct _tagged_string* _temp126;
struct Cyc_List_List* _temp128; _LL111: if( _temp109 == Cyc_Absyn_Loc_n){ goto
_LL112;} else{ goto _LL113;} _LL113: if(( unsigned int) _temp109 > 1u?(( struct
_enum_struct*) _temp109)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL120: _temp119=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp109)->f1; if(
_temp119 == 0){ goto _LL114;} else{ goto _LL115;}} else{ goto _LL115;} _LL115:
if(( unsigned int) _temp109 > 1u?(( struct _enum_struct*) _temp109)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL122: _temp121=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp109)->f1; if( _temp121 == 0){ goto _LL117;} else{ _temp123=* _temp121;
_LL127: _temp126=( struct _tagged_string*) _temp123.hd; goto _LL125; _LL125:
_temp124=( struct Cyc_List_List*) _temp123.tl; goto _LL116;}} else{ goto _LL117;}
_LL117: if(( unsigned int) _temp109 > 1u?(( struct _enum_struct*) _temp109)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL129: _temp128=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp109)->f1; goto _LL118;} else{ goto _LL110;} _LL112:
goto _LL114; _LL114: return(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Enumdecl**(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)(
te, loc, Cyc_Tcenv_lookup_enumdecl_f, _temp105); _LL116: { struct Cyc_Tcenv_Genv*
ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp126, _temp124); return(( struct
Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup)( ge->enumdecls, _temp105);} _LL118: { struct Cyc_Tcenv_Genv* ge=((
struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)( te->ae, _temp128); return(( struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls,
_temp105);} _LL110:;}} static struct Cyc_Absyn_Xenumdecl** Cyc_Tcenv_lookup_xenumdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Xenumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->xenumdecls,
v);} struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xenumdecl( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0 _temp132;
struct _tagged_string* _temp133; void* _temp135; struct _tuple0* _temp130= q;
_temp132=* _temp130; _LL136: _temp135= _temp132.f1; goto _LL134; _LL134:
_temp133= _temp132.f2; goto _LL131; _LL131: { void* _temp137= _temp135; struct
Cyc_List_List* _temp147; struct Cyc_List_List* _temp149; struct Cyc_List_List
_temp151; struct Cyc_List_List* _temp152; struct _tagged_string* _temp154;
struct Cyc_List_List* _temp156; _LL139: if(( unsigned int) _temp137 > 1u?((
struct _enum_struct*) _temp137)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL148:
_temp147=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp137)->f1;
if( _temp147 == 0){ goto _LL140;} else{ goto _LL141;}} else{ goto _LL141;}
_LL141: if( _temp137 == Cyc_Absyn_Loc_n){ goto _LL142;} else{ goto _LL143;}
_LL143: if(( unsigned int) _temp137 > 1u?(( struct _enum_struct*) _temp137)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL150: _temp149=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp137)->f1; if( _temp149 == 0){ goto _LL145;} else{
_temp151=* _temp149; _LL155: _temp154=( struct _tagged_string*) _temp151.hd;
goto _LL153; _LL153: _temp152=( struct Cyc_List_List*) _temp151.tl; goto _LL144;}}
else{ goto _LL145;} _LL145: if(( unsigned int) _temp137 > 1u?(( struct
_enum_struct*) _temp137)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL157: _temp156=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp137)->f1; goto
_LL146;} else{ goto _LL138;} _LL140: { struct _handler_cons _temp158;
_push_handler(& _temp158);{ struct _xenum_struct* _temp159=( struct
_xenum_struct*) setjmp( _temp158.handler); if( ! _temp159){{ struct Cyc_Core_Opt*
_temp161=({ struct Cyc_Core_Opt* _temp160=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp160->v=( void*)(( struct Cyc_Absyn_Xenumdecl**(*)(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Xenumdecl**(*
lookup)( struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string*
v)) Cyc_Tcenv_scoped_lookup)( te, loc, Cyc_Tcenv_lookup_xenumdecl_f, _temp133);
_temp160;}); _npop_handler( 0u); return _temp161;} _pop_handler();} else{ struct
_xenum_struct* _temp163= _temp159; _LL165: if( _temp163->tag == Cyc_Dict_Absent_tag){
goto _LL166;} else{ goto _LL167;} _LL167: goto _LL168; _LL166: return 0; _LL168:(
void) _throw( _temp163); _LL164:;}}} _LL142: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp169=( char*)"lookup_xenumdecl: impossible"; struct
_tagged_string _temp170; _temp170.curr= _temp169; _temp170.base= _temp169;
_temp170.last_plus_one= _temp169 + 29; _temp170;})); return 0; _LL144: { struct
Cyc_Tcenv_Genv* ge;{ struct _handler_cons _temp171; _push_handler(& _temp171);{
struct _xenum_struct* _temp172=( struct _xenum_struct*) setjmp( _temp171.handler);
if( ! _temp172){ ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp154, _temp152);
_pop_handler();} else{ struct _xenum_struct* _temp174= _temp172; _LL176: if(
_temp174->tag == Cyc_Dict_Absent_tag){ goto _LL177;} else{ goto _LL178;} _LL178:
goto _LL179; _LL177: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp180=( char*)"bad qualified name for xenum"; struct _tagged_string _temp181;
_temp181.curr= _temp180; _temp181.base= _temp180; _temp181.last_plus_one=
_temp180 + 29; _temp181;}));( void) _throw(({ struct Cyc_Dict_Absent_struct*
_temp182=( struct Cyc_Dict_Absent_struct*) GC_malloc_atomic( sizeof( struct Cyc_Dict_Absent_struct));*
_temp182=( struct Cyc_Dict_Absent_struct){.tag= Cyc_Dict_Absent_tag};( struct
_xenum_struct*) _temp182;})); _LL179:( void) _throw( _temp174); _LL175:;}}}
return({ struct Cyc_Core_Opt* _temp183=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp183->v=( void*)(( struct Cyc_Absyn_Xenumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->xenumdecls,
_temp133); _temp183;});} _LL146: { struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
_temp156); return({ struct Cyc_Core_Opt* _temp184=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp184->v=( void*)(( struct Cyc_Absyn_Xenumdecl**(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->xenumdecls,
_temp133); _temp184;});} _LL138:;}} static struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl_f(
struct Cyc_Tcenv_Genv* ge, struct _tagged_string* v){ return(( struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->typedefs,
v);} struct Cyc_Absyn_Typedefdecl* Cyc_Tcenv_lookup_typedefdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tuple0* q){ struct _tuple0
_temp187; struct _tagged_string* _temp188; void* _temp190; struct _tuple0*
_temp185= q; _temp187=* _temp185; _LL191: _temp190= _temp187.f1; goto _LL189;
_LL189: _temp188= _temp187.f2; goto _LL186; _LL186: { void* _temp192= _temp190;
struct Cyc_List_List* _temp202; struct Cyc_List_List* _temp204; struct Cyc_List_List
_temp206; struct Cyc_List_List* _temp207; struct _tagged_string* _temp209;
struct Cyc_List_List* _temp211; _LL194: if( _temp192 == Cyc_Absyn_Loc_n){ goto
_LL195;} else{ goto _LL196;} _LL196: if(( unsigned int) _temp192 > 1u?(( struct
_enum_struct*) _temp192)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL203: _temp202=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp192)->f1; if(
_temp202 == 0){ goto _LL197;} else{ goto _LL198;}} else{ goto _LL198;} _LL198:
if(( unsigned int) _temp192 > 1u?(( struct _enum_struct*) _temp192)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL205: _temp204=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp192)->f1; if( _temp204 == 0){ goto _LL200;} else{ _temp206=* _temp204;
_LL210: _temp209=( struct _tagged_string*) _temp206.hd; goto _LL208; _LL208:
_temp207=( struct Cyc_List_List*) _temp206.tl; goto _LL199;}} else{ goto _LL200;}
_LL200: if(( unsigned int) _temp192 > 1u?(( struct _enum_struct*) _temp192)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL212: _temp211=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp192)->f1; goto _LL201;} else{ goto _LL193;} _LL195:
goto _LL197; _LL197: return(( struct Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl*(* lookup)(
struct Cyc_Tcenv_Genv*, struct _tagged_string*), struct _tagged_string* v)) Cyc_Tcenv_scoped_lookup)(
te, loc, Cyc_Tcenv_lookup_typedefdecl_f, _temp188); _LL199: { struct Cyc_Tcenv_Genv*
ge= Cyc_Tcenv_lookup_namespace( te, loc, _temp209, _temp207); return(( struct
Cyc_Absyn_Typedefdecl*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup)( ge->typedefs, _temp188);} _LL201: { struct Cyc_Tcenv_Genv* ge=((
struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key))
Cyc_Dict_lookup)( te->ae, _temp211); return(( struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->typedefs,
_temp188);} _LL193:;}} struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(){ return({
struct Cyc_Tcenv_Genv* _temp213=( struct Cyc_Tcenv_Genv*) GC_malloc( sizeof(
struct Cyc_Tcenv_Genv)); _temp213->namespaces=( struct Cyc_Set_Set*) Cyc_Tcutil_empty_var_set->v;
_temp213->structdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp213->enumdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp213->xenumdecls=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp213->typedefs=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp213->ordinaries=(( struct Cyc_Dict_Dict*(*)( int(* comp)( struct
_tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp);
_temp213->availables= 0; _temp213;});} void* Cyc_Tcenv_get_visible( void* f){
while( 1) { void* _temp214= f; void* _temp222; void* _temp224; void* _temp226;
void* _temp228; void* _temp230; _LL216: if((( struct _enum_struct*) _temp214)->tag
== Cyc_Tcenv_Outermost_tag){ _LL223: _temp222=( void*)(( struct Cyc_Tcenv_Outermost_struct*)
_temp214)->f1; goto _LL217;} else{ goto _LL218;} _LL218: if((( struct
_enum_struct*) _temp214)->tag == Cyc_Tcenv_Frame_tag){ _LL227: _temp226=( void*)((
struct Cyc_Tcenv_Frame_struct*) _temp214)->f1; goto _LL225; _LL225: _temp224=(
void*)(( struct Cyc_Tcenv_Frame_struct*) _temp214)->f2; goto _LL219;} else{ goto
_LL220;} _LL220: if((( struct _enum_struct*) _temp214)->tag == Cyc_Tcenv_Hidden_tag){
_LL231: _temp230=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp214)->f1; goto
_LL229; _LL229: _temp228=( void*)(( struct Cyc_Tcenv_Hidden_struct*) _temp214)->f2;
goto _LL221;} else{ goto _LL215;} _LL217: return _temp222; _LL219: return
_temp226; _LL221: f= _temp228; goto _LL215; _LL215:;}} void* Cyc_Tcenv_put_visible(
void* f, void* x){ void* _temp232= f; void* _temp240; void* _temp242; void*
_temp244; void* _temp246; void* _temp248; _LL234: if((( struct _enum_struct*)
_temp232)->tag == Cyc_Tcenv_Outermost_tag){ _LL241: _temp240=( void*)(( struct
Cyc_Tcenv_Outermost_struct*) _temp232)->f1; goto _LL235;} else{ goto _LL236;}
_LL236: if((( struct _enum_struct*) _temp232)->tag == Cyc_Tcenv_Frame_tag){
_LL245: _temp244=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp232)->f1; goto
_LL243; _LL243: _temp242=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp232)->f2;
goto _LL237;} else{ goto _LL238;} _LL238: if((( struct _enum_struct*) _temp232)->tag
== Cyc_Tcenv_Hidden_tag){ _LL249: _temp248=( void*)(( struct Cyc_Tcenv_Hidden_struct*)
_temp232)->f1; goto _LL247; _LL247: _temp246=( void*)(( struct Cyc_Tcenv_Hidden_struct*)
_temp232)->f2; goto _LL239;} else{ goto _LL233;} _LL235: return({ struct Cyc_Tcenv_Outermost_struct*
_temp250=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp250->tag= Cyc_Tcenv_Outermost_tag; _temp250->f1=( void*) x;( void*)
_temp250;}); _LL237: return({ struct Cyc_Tcenv_Frame_struct* _temp251=( struct
Cyc_Tcenv_Frame_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct));
_temp251->tag= Cyc_Tcenv_Frame_tag; _temp251->f1=( void*) x; _temp251->f2=( void*)
_temp242;( void*) _temp251;}); _LL239: return({ struct Cyc_Tcenv_Hidden_struct*
_temp252=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp252->tag= Cyc_Tcenv_Hidden_tag; _temp252->f1=( void*) _temp248; _temp252->f2=(
void*)(( void*(*)( void* f, void* x)) Cyc_Tcenv_put_visible)( _temp246, x);(
void*) _temp252;}); _LL233:;} void* Cyc_Tcenv_apply_to_visible( void*(* g)( void*),
void* f){ return(( void*(*)( void* f, void* x)) Cyc_Tcenv_put_visible)( f, g(((
void*(*)( void* f)) Cyc_Tcenv_get_visible)( f)));} void* Cyc_Tcenv_map_frames(
void*(* g)( void*), void* f){ void* _temp253= f; void* _temp261; void* _temp263;
void* _temp265; void* _temp267; void* _temp269; _LL255: if((( struct
_enum_struct*) _temp253)->tag == Cyc_Tcenv_Outermost_tag){ _LL262: _temp261=(
void*)(( struct Cyc_Tcenv_Outermost_struct*) _temp253)->f1; goto _LL256;} else{
goto _LL257;} _LL257: if((( struct _enum_struct*) _temp253)->tag == Cyc_Tcenv_Frame_tag){
_LL266: _temp265=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp253)->f1; goto
_LL264; _LL264: _temp263=( void*)(( struct Cyc_Tcenv_Frame_struct*) _temp253)->f2;
goto _LL258;} else{ goto _LL259;} _LL259: if((( struct _enum_struct*) _temp253)->tag
== Cyc_Tcenv_Hidden_tag){ _LL270: _temp269=( void*)(( struct Cyc_Tcenv_Hidden_struct*)
_temp253)->f1; goto _LL268; _LL268: _temp267=( void*)(( struct Cyc_Tcenv_Hidden_struct*)
_temp253)->f2; goto _LL260;} else{ goto _LL254;} _LL256: return({ struct Cyc_Tcenv_Outermost_struct*
_temp271=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp271->tag= Cyc_Tcenv_Outermost_tag; _temp271->f1=( void*) g( _temp261);(
void*) _temp271;}); _LL258: return({ struct Cyc_Tcenv_Frame_struct* _temp272=(
struct Cyc_Tcenv_Frame_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Frame_struct));
_temp272->tag= Cyc_Tcenv_Frame_tag; _temp272->f1=( void*) g( _temp265); _temp272->f2=(
void*)(( void*(*)( void*(* g)( void*), void* f)) Cyc_Tcenv_map_frames)( g,
_temp263);( void*) _temp272;}); _LL260: return({ struct Cyc_Tcenv_Hidden_struct*
_temp273=( struct Cyc_Tcenv_Hidden_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Hidden_struct));
_temp273->tag= Cyc_Tcenv_Hidden_tag; _temp273->f1=( void*) g( _temp269);
_temp273->f2=( void*)(( void*(*)( void*(* g)( void*), void* f)) Cyc_Tcenv_map_frames)(
g, _temp267);( void*) _temp273;}); _LL254:;} struct Cyc_Tcenv_Fenv* Cyc_Tcenv_get_fenv(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){( void) _throw(({ struct Cyc_Tcenv_Env_error_struct*
_temp274=( struct Cyc_Tcenv_Env_error_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Tcenv_Env_error_struct));* _temp274=( struct Cyc_Tcenv_Env_error_struct){.tag=
Cyc_Tcenv_Env_error_tag};( struct _xenum_struct*) _temp274;}));} return(( struct
Cyc_Tcenv_Fenv*(*)( void* f)) Cyc_Tcenv_get_visible)(( void*)( te->le)->v);}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_put_fenv( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Fenv*
fe){ if( te->le == 0){( void) _throw(({ struct Cyc_Tcenv_Env_error_struct*
_temp275=( struct Cyc_Tcenv_Env_error_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Tcenv_Env_error_struct));* _temp275=( struct Cyc_Tcenv_Env_error_struct){.tag=
Cyc_Tcenv_Env_error_tag};( struct _xenum_struct*) _temp275;}));} return({ struct
Cyc_Tcenv_Tenv* _temp276=( struct Cyc_Tcenv_Tenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Tenv));
_temp276->ns= te->ns; _temp276->ae= te->ae; _temp276->le=({ struct Cyc_Core_Opt*
_temp277=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp277->v=( void*)(( void*(*)( void* f, struct Cyc_Tcenv_Fenv* x)) Cyc_Tcenv_put_visible)((
void*)( te->le)->v, fe); _temp277;}); _temp276;});} static struct Cyc_Tcenv_Fenv*
Cyc_Tcenv_copy_fenv_old_ctrl( struct Cyc_Tcenv_Fenv* f){ return({ struct Cyc_Tcenv_Fenv*
_temp278=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof( struct Cyc_Tcenv_Fenv));
_temp278->shared= f->shared; _temp278->type_vars= f->type_vars; _temp278->locals=
f->locals; _temp278->encloser= f->encloser; _temp278->ctrl_env= f->ctrl_env;
_temp278->capability=( void*)(( void*) f->capability); _temp278->curr_rgn=( void*)((
void*) f->curr_rgn); _temp278;});} static struct Cyc_Tcenv_Fenv* Cyc_Tcenv_copy_fenv_new_ctrl(
struct Cyc_Tcenv_Fenv* f){ struct Cyc_Tcenv_CtrlEnv* c= f->ctrl_env; return({
struct Cyc_Tcenv_Fenv* _temp279=( struct Cyc_Tcenv_Fenv*) GC_malloc( sizeof(
struct Cyc_Tcenv_Fenv)); _temp279->shared= f->shared; _temp279->type_vars= f->type_vars;
_temp279->locals= f->locals; _temp279->encloser= f->encloser; _temp279->ctrl_env=({
struct Cyc_Tcenv_CtrlEnv* _temp280=( struct Cyc_Tcenv_CtrlEnv*) GC_malloc(
sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp280->continue_stmt=( void*)(( void*) c->continue_stmt);
_temp280->break_stmt=( void*)(( void*) c->break_stmt); _temp280->fallthru_stmt=
c->fallthru_stmt; _temp280->next_stmt=( void*)(( void*) c->next_stmt); _temp280;});
_temp279->capability=( void*)(( void*) f->capability); _temp279->curr_rgn=( void*)((
void*) f->curr_rgn); _temp279;});} void* Cyc_Tcenv_return_typ( struct Cyc_Tcenv_Tenv*
te){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp281=( char*)"Tcenv: unexpected return_typ";
struct _tagged_string _temp282; _temp282.curr= _temp281; _temp282.base= _temp281;
_temp282.last_plus_one= _temp281 + 29; _temp282;}));} return( void*)(( Cyc_Tcenv_get_fenv(
te))->shared)->return_typ;} struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return 0;} return( Cyc_Tcenv_get_fenv(
te))->type_vars;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* tvs){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({
char* _temp283=( char*)"Tcenv: unexpected add_type_vars"; struct _tagged_string
_temp284; _temp284.curr= _temp283; _temp284.base= _temp283; _temp284.last_plus_one=
_temp283 + 32; _temp284;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct Cyc_List_List* new_tvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, fe->type_vars);
Cyc_Tcutil_check_unique_tvars( loc, new_tvs); fe->type_vars= new_tvs; return Cyc_Tcenv_put_fenv(
te, fe);}} static int Cyc_Tcenv_shadow_ctr= 1; struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_local_var(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl*
vd){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp285=( char*)"Tcenv: unexpected add_local_var";
struct _tagged_string _temp286; _temp286.curr= _temp285; _temp286.base= _temp285;
_temp286.last_plus_one= _temp285 + 32; _temp286;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); struct _tagged_string* v=(*
vd->name).f2;{ void* _temp287=(* vd->name).f1; _LL289: if( _temp287 == Cyc_Absyn_Loc_n){
goto _LL290;} else{ goto _LL291;} _LL291: goto _LL292; _LL290: goto _LL288;
_LL292:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp293=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp293=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp294=( char*)"add_local_var: called with Rel_n";
struct _tagged_string _temp295; _temp295.curr= _temp294; _temp295.base= _temp294;
_temp295.last_plus_one= _temp294 + 33; _temp295;})};( struct _xenum_struct*)
_temp293;})); goto _LL288; _LL288:;} if((( int(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_member)( fe->locals, v)){ vd->shadow=( ++
Cyc_Tcenv_shadow_ctr);} fe->locals=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)( fe->locals, v,({
struct Cyc_Absyn_Local_b_struct* _temp296=( struct Cyc_Absyn_Local_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Local_b_struct)); _temp296->tag= Cyc_Absyn_Local_b_tag;
_temp296->f1= vd;( void*) _temp296;})); return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var( struct Cyc_Position_Segment* loc,
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Vardecl* vd){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({
char* _temp297=( char*)"Tcenv: unexpected add_pat_var"; struct _tagged_string
_temp298; _temp298.curr= _temp297; _temp298.base= _temp297; _temp298.last_plus_one=
_temp297 + 30; _temp298;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); struct _tagged_string* v=(* vd->name).f2; if((( int(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)( fe->locals,
v)){ vd->shadow=( ++ Cyc_Tcenv_shadow_ctr);} fe->locals=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(
fe->locals, v,({ struct Cyc_Absyn_Pat_b_struct* _temp299=( struct Cyc_Absyn_Pat_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pat_b_struct)); _temp299->tag= Cyc_Absyn_Pat_b_tag;
_temp299->f1= vd;( void*) _temp299;})); return Cyc_Tcenv_put_fenv( te, fe);}}
void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple0* q){ struct _tuple0 _temp302; struct _tagged_string*
_temp303; void* _temp305; struct _tuple0* _temp300= q; _temp302=* _temp300;
_LL306: _temp305= _temp302.f1; goto _LL304; _LL304: _temp303= _temp302.f2; goto
_LL301; _LL301: { void* _temp307= _temp305; struct Cyc_List_List* _temp316;
_LL309: if( _temp307 == Cyc_Absyn_Loc_n){ goto _LL310;} else{ goto _LL311;}
_LL311: if(( unsigned int) _temp307 > 1u?(( struct _enum_struct*) _temp307)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL317: _temp316=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp307)->f1; if( _temp316 == 0){ goto _LL315;} else{
goto _LL313;}} else{ goto _LL313;} _LL315: if( te->le != 0){ goto _LL312;} else{
goto _LL313;} _LL313: goto _LL314; _LL310: if( te->le == 0){( void) _throw(({
struct Cyc_Dict_Absent_struct* _temp318=( struct Cyc_Dict_Absent_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Dict_Absent_struct));* _temp318=( struct
Cyc_Dict_Absent_struct){.tag= Cyc_Dict_Absent_tag};( struct _xenum_struct*)
_temp318;}));} else{ goto _LL312;} _LL312: { struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv(
te); struct _handler_cons _temp319; _push_handler(& _temp319);{ struct
_xenum_struct* _temp320=( struct _xenum_struct*) setjmp( _temp319.handler); if(
! _temp320){{ void* _temp322=({ struct Cyc_Tcenv_VarRes_struct* _temp321=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp321->tag= Cyc_Tcenv_VarRes_tag; _temp321->f1=( void*)(( void*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( fe->locals,
_temp303);( void*) _temp321;}); _npop_handler( 0u); return _temp322;}
_pop_handler();} else{ struct _xenum_struct* _temp324= _temp320; _LL326: if(
_temp324->tag == Cyc_Dict_Absent_tag){ goto _LL327;} else{ goto _LL328;} _LL328:
goto _LL329; _LL327: return Cyc_Tcenv_lookup_ordinary_global( te, loc, q);
_LL329:( void) _throw( _temp324); _LL325:;}}} _LL314: { struct _handler_cons
_temp330; _push_handler(& _temp330);{ struct _xenum_struct* _temp331=( struct
_xenum_struct*) setjmp( _temp330.handler); if( ! _temp331){{ void* _temp332= Cyc_Tcenv_lookup_ordinary_global(
te, loc, q); _npop_handler( 0u); return _temp332;} _pop_handler();} else{ struct
_xenum_struct* _temp334= _temp331; _LL336: if( _temp334->tag == Cyc_Dict_Absent_tag){
goto _LL337;} else{ goto _LL338;} _LL338: goto _LL339; _LL337: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp340= Cyc_Absynpp_qvar2string( q); xprintf("unbound identifer: %.*s",
_temp340.last_plus_one - _temp340.curr, _temp340.curr);})); return({ struct Cyc_Tcenv_VarRes_struct*
_temp341=( struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp341->tag= Cyc_Tcenv_VarRes_tag; _temp341->f1=( void*) Cyc_Absyn_Unresolved_b;(
void*) _temp341;}); _LL339:( void) _throw( _temp334); _LL335:;}}} _LL308:;}}
void Cyc_Tcenv_process_continue( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
s, struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp342=( char*)"Tcenv: unexpected process_continue"; struct _tagged_string
_temp343; _temp343.curr= _temp342; _temp343.base= _temp342; _temp343.last_plus_one=
_temp342 + 35; _temp343;}));}{ void* _temp344=( void*)(( Cyc_Tcenv_get_fenv( te))->ctrl_env)->continue_stmt;
struct Cyc_Absyn_Stmt* _temp354; _LL346: if(( unsigned int) _temp344 > 3u?((
struct _enum_struct*) _temp344)->tag == Cyc_Tcenv_Stmt_j_tag: 0){ _LL355:
_temp354=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Tcenv_Stmt_j_struct*) _temp344)->f1;
goto _LL347;} else{ goto _LL348;} _LL348: if( _temp344 == Cyc_Tcenv_NotLoop_j){
goto _LL349;} else{ goto _LL350;} _LL350: if( _temp344 == Cyc_Tcenv_CaseEnd_j){
goto _LL351;} else{ goto _LL352;} _LL352: if( _temp344 == Cyc_Tcenv_FnEnd_j){
goto _LL353;} else{ goto _LL345;} _LL347: _temp354->non_local_preds=({ struct
Cyc_List_List* _temp356=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp356->hd=( void*) s; _temp356->tl= _temp354->non_local_preds; _temp356;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp354; return; _LL349: Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp357=( char*)"continue not in a loop"; struct
_tagged_string _temp358; _temp358.curr= _temp357; _temp358.base= _temp357;
_temp358.last_plus_one= _temp357 + 23; _temp358;})); return; _LL351:(( void(*)(
struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp359=( char*)"Tcenv: continue dest is CaseEnd"; struct _tagged_string
_temp360; _temp360.curr= _temp359; _temp360.base= _temp359; _temp360.last_plus_one=
_temp359 + 32; _temp360;})); return; _LL353:(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp361=( char*)"Tcenv: continue dest is CaseEnd";
struct _tagged_string _temp362; _temp362.curr= _temp361; _temp362.base= _temp361;
_temp362.last_plus_one= _temp361 + 32; _temp362;})); return; _LL345:;}} void Cyc_Tcenv_process_break(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt**
sopt){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp363=( char*)"Tcenv: unexpected process_break";
struct _tagged_string _temp364; _temp364.curr= _temp363; _temp364.base= _temp363;
_temp364.last_plus_one= _temp363 + 32; _temp364;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); void* _temp365=( void*)( fe->ctrl_env)->break_stmt;
struct Cyc_Absyn_Stmt* _temp375; _LL367: if(( unsigned int) _temp365 > 3u?((
struct _enum_struct*) _temp365)->tag == Cyc_Tcenv_Stmt_j_tag: 0){ _LL376:
_temp375=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Tcenv_Stmt_j_struct*) _temp365)->f1;
goto _LL368;} else{ goto _LL369;} _LL369: if( _temp365 == Cyc_Tcenv_NotLoop_j){
goto _LL370;} else{ goto _LL371;} _LL371: if( _temp365 == Cyc_Tcenv_FnEnd_j){
goto _LL372;} else{ goto _LL373;} _LL373: if( _temp365 == Cyc_Tcenv_CaseEnd_j){
goto _LL374;} else{ goto _LL366;} _LL368: _temp375->non_local_preds=({ struct
Cyc_List_List* _temp377=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp377->hd=( void*) s; _temp377->tl= _temp375->non_local_preds; _temp377;});*
sopt=( struct Cyc_Absyn_Stmt*) _temp375; return; _LL370: Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp378=( char*)"break not in a loop or switch";
struct _tagged_string _temp379; _temp379.curr= _temp378; _temp379.base= _temp378;
_temp379.last_plus_one= _temp378 + 30; _temp379;})); return; _LL372: if(( void*)(
fe->shared)->return_typ != Cyc_Absyn_VoidType){ Cyc_Tcutil_terr( s->loc,( struct
_tagged_string)({ char* _temp380=( char*)"break causes function not to return a value";
struct _tagged_string _temp381; _temp381.curr= _temp380; _temp381.base= _temp380;
_temp381.last_plus_one= _temp380 + 44; _temp381;}));} return; _LL374: Cyc_Tcutil_terr(
s->loc,( struct _tagged_string)({ char* _temp382=( char*)"break causes outer switch clause to implicitly fallthru";
struct _tagged_string _temp383; _temp383.curr= _temp382; _temp383.base= _temp382;
_temp383.last_plus_one= _temp382 + 56; _temp383;})); return; _LL366:;}} void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct _tagged_string* l,
struct Cyc_Absyn_Stmt** sopt){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp384=( char*)"Tcenv: unexpected process_goto"; struct _tagged_string
_temp385; _temp385.curr= _temp384; _temp385.base= _temp384; _temp385.last_plus_one=
_temp384 + 31; _temp385;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_get_fenv( te);
struct Cyc_Core_Opt* sopt2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key)) Cyc_Dict_lookup_opt)(( fe->shared)->seen_labels, l);
if( sopt2 == 0){ struct Cyc_SlowDict_Dict* needed=( fe->shared)->needed_labels;
struct Cyc_Core_Opt* slopt=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_string* key)) Cyc_SlowDict_lookup_opt)( needed, l); if( slopt
== 0){ slopt=({ struct Cyc_Core_Opt* _temp386=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp386->v=( void*) 0; _temp386;});}( fe->shared)->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict* d, struct _tagged_string*
key, struct Cyc_List_List* data)) Cyc_SlowDict_insert)( needed, l,({ struct Cyc_List_List*
_temp387=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp387->hd=( void*) s; _temp387->tl=( struct Cyc_List_List*) slopt->v;
_temp387;}));} else{(( struct Cyc_Absyn_Stmt*) sopt2->v)->non_local_preds=({
struct Cyc_List_List* _temp388=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp388->hd=( void*) s; _temp388->tl=(( struct Cyc_Absyn_Stmt*)
sopt2->v)->non_local_preds; _temp388;});* sopt=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Stmt*) sopt2->v);}}} struct _tuple4* Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Stmt**
sopt){ if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp389=( char*)"Tcenv: unexpected process_fallthru";
struct _tagged_string _temp390; _temp390.curr= _temp389; _temp390.base= _temp389;
_temp390.last_plus_one= _temp389 + 35; _temp390;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); struct _tuple4* ans=( fe->ctrl_env)->fallthru_stmt; if(
ans != 0){((* ans).f1)->non_local_preds=({ struct Cyc_List_List* _temp391=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp391->hd=(
void*) s; _temp391->tl=((* ans).f1)->non_local_preds; _temp391;});* sopt=(
struct Cyc_Absyn_Stmt*)(* ans).f1;} return ans;}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru(
struct Cyc_Tcenv_Tenv* te, struct _tuple3* pat_typ, struct Cyc_Absyn_Stmt* body){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp392=( char*)"Tcenv: unexpected set_fallthru";
struct _tagged_string _temp393; _temp393.curr= _temp392; _temp393.base= _temp392;
_temp393.last_plus_one= _temp392 + 31; _temp393;}));}{ struct Cyc_List_List*
ft_typ= 0;{ struct Cyc_List_List* vds=(* pat_typ).f2; for( 0; vds != 0; vds= vds->tl){
ft_typ=({ struct Cyc_List_List* _temp394=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp394->hd=( void*)(( void*)(( struct Cyc_Absyn_Vardecl*)
vds->hd)->type); _temp394->tl= ft_typ; _temp394;});}} ft_typ=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( ft_typ);{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_stmt=({
struct _tuple4* _temp395=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp395->f1= body; _temp395->f2=(* pat_typ).f1; _temp395->f3= ft_typ; _temp395;});
return Cyc_Tcenv_put_fenv( te, fe);}}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp396=( char*)"Tcenv: unexpected clear_fallthru";
struct _tagged_string _temp397; _temp397.curr= _temp396; _temp397.base= _temp396;
_temp397.last_plus_one= _temp396 + 33; _temp397;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( fe->ctrl_env)->fallthru_stmt=
0; return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_loop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* continue_dest){ if( te->le ==
0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct
_tagged_string)({ char* _temp398=( char*)"Tcenv: unexpected set_in_loop"; struct
_tagged_string _temp399; _temp399.curr= _temp398; _temp399.base= _temp398;
_temp399.last_plus_one= _temp398 + 30; _temp399;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->continue_stmt=(
void*)({ struct Cyc_Tcenv_Stmt_j_struct* _temp400=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp400->tag= Cyc_Tcenv_Stmt_j_tag;
_temp400->f1= continue_dest;( void*) _temp400;}));( void*)(( fe->ctrl_env)->break_stmt=(
void*)(( void*)( fe->ctrl_env)->next_stmt));( void*)(( fe->ctrl_env)->next_stmt=(
void*)({ struct Cyc_Tcenv_Stmt_j_struct* _temp401=( struct Cyc_Tcenv_Stmt_j_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp401->tag= Cyc_Tcenv_Stmt_j_tag;
_temp401->f1= continue_dest;( void*) _temp401;})); return Cyc_Tcenv_put_fenv( te,
fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch( struct Cyc_Tcenv_Tenv* te){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp402=( char*)"Tcenv: unexpected set_in_switch";
struct _tagged_string _temp403; _temp403.curr= _temp402; _temp403.base= _temp402;
_temp403.last_plus_one= _temp402 + 32; _temp403;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_new_ctrl( Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->break_stmt=(
void*)(( void*)( fe->ctrl_env)->next_stmt));( void*)(( fe->ctrl_env)->next_stmt=(
void*) Cyc_Tcenv_CaseEnd_j); return Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_next( struct Cyc_Tcenv_Tenv* te, void* j){ if( te->le == 0){((
void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({
char* _temp404=( char*)"Tcenv: unexpected set_next"; struct _tagged_string
_temp405; _temp405.curr= _temp404; _temp405.base= _temp404; _temp405.last_plus_one=
_temp404 + 27; _temp405;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv( te));( void*)(( fe->ctrl_env)->next_stmt=( void*) j); return
Cyc_Tcenv_put_fenv( te, fe);}} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv* te, struct _tagged_string* v, struct Cyc_Absyn_Stmt* s){
if( te->le == 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp406=( char*)"Tcenv: unexpected add_label";
struct _tagged_string _temp407; _temp407.curr= _temp406; _temp407.base= _temp406;
_temp407.last_plus_one= _temp406 + 28; _temp407;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_get_fenv( te); struct Cyc_SlowDict_Dict* needed=( fe->shared)->needed_labels;
struct Cyc_Core_Opt* sl_opt=(( struct Cyc_Core_Opt*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_string* key)) Cyc_SlowDict_lookup_opt)( needed, v); if( sl_opt
!= 0){( fe->shared)->needed_labels=(( struct Cyc_SlowDict_Dict*(*)( struct Cyc_SlowDict_Dict*
d, struct _tagged_string* key)) Cyc_SlowDict_delete)( needed, v);{ struct Cyc_List_List*
sl=( struct Cyc_List_List*) sl_opt->v; s->non_local_preds= sl; for( 0; sl != 0;
sl= sl->tl){ void* _temp408=( void*)(( struct Cyc_Absyn_Stmt*) sl->hd)->r;
struct Cyc_Absyn_Stmt* _temp414; struct Cyc_Absyn_Stmt** _temp416; struct
_tagged_string* _temp417; _LL410: if(( unsigned int) _temp408 > 1u?(( struct
_enum_struct*) _temp408)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL418: _temp417=(
struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp408)->f1; goto
_LL415; _LL415: _temp414=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp408)->f2; _temp416=&(( struct Cyc_Absyn_Goto_s_struct*) _temp408)->f2; goto
_LL411;} else{ goto _LL412;} _LL412: goto _LL413; _LL411:* _temp416=( struct Cyc_Absyn_Stmt*)
s; goto _LL409; _LL413:(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)((
struct _tagged_string)({ char* _temp419=( char*)"Tcenv: add_label backpatching of non-goto";
struct _tagged_string _temp420; _temp420.curr= _temp419; _temp420.base= _temp419;
_temp420.last_plus_one= _temp419 + 42; _temp420;})); goto _LL409; _LL409:;}}}
if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)((
fe->shared)->seen_labels, v)){ Cyc_Tcutil_terr( s->loc,({ struct _tagged_string
_temp421=* v; xprintf("Repeated label: %.*s", _temp421.last_plus_one - _temp421.curr,
_temp421.curr);}));}( fe->shared)->seen_labels=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Stmt* data))
Cyc_Dict_insert)(( fe->shared)->seen_labels, v, s); return te;}} int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv* te){ return(( int(*)( struct Cyc_SlowDict_Dict* d)) Cyc_SlowDict_is_empty)(((
Cyc_Tcenv_get_fenv( te))->shared)->needed_labels);} struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){(( void(*)( struct _tagged_string
msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char* _temp422=( char*)"Tcenv: unexpected get_encloser";
struct _tagged_string _temp423; _temp423.curr= _temp422; _temp423.base= _temp422;
_temp423.last_plus_one= _temp422 + 31; _temp423;}));} return( Cyc_Tcenv_get_fenv(
te))->encloser;} struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s){ if( te->le == 0){(( void(*)( struct
_tagged_string msg)) Cyc_Tcenv_env_err)(( struct _tagged_string)({ char*
_temp424=( char*)"Tcenv: unexpected set_encloser"; struct _tagged_string
_temp425; _temp425.curr= _temp424; _temp425.base= _temp424; _temp425.last_plus_one=
_temp424 + 31; _temp425;}));}{ struct Cyc_Tcenv_Fenv* fe= Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv( te)); fe->encloser= s; return Cyc_Tcenv_put_fenv( te, fe);}}
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_block( struct Cyc_Tcenv_Tenv* te){ if( te->le
== 0){(( void(*)( struct _tagged_string msg)) Cyc_Tcenv_env_err)(( struct
_tagged_string)({ char* _temp426=( char*)"Tcenv: unexpected new_block"; struct
_tagged_string _temp427; _temp427.curr= _temp426; _temp427.base= _temp426;
_temp427.last_plus_one= _temp426 + 28; _temp427;}));}{ struct Cyc_Tcenv_Fenv* fe=
Cyc_Tcenv_copy_fenv_old_ctrl( Cyc_Tcenv_get_fenv( te)); struct Cyc_Absyn_Tvar*
block_rgn= Cyc_Tcutil_new_tvar( Cyc_Absyn_RgnKind); void* block_typ=({ struct
Cyc_Absyn_VarType_struct* _temp428=( struct Cyc_Absyn_VarType_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_VarType_struct)); _temp428->tag= Cyc_Absyn_VarType_tag;
_temp428->f1= block_rgn;( void*) _temp428;}); fe->type_vars=({ struct Cyc_List_List*
_temp429=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp429->hd=( void*) block_rgn; _temp429->tl= fe->type_vars; _temp429;});( void*)(
fe->capability=( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp430=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp430->tag= Cyc_Absyn_JoinEff_tag;
_temp430->f1=({ struct Cyc_List_List* _temp431=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp431->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp433=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp433->tag= Cyc_Absyn_AccessEff_tag; _temp433->f1=( void*) block_typ;( void*)
_temp433;}); _temp431->tl=({ struct Cyc_List_List* _temp432=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp432->hd=( void*)(( void*) fe->capability);
_temp432->tl= 0; _temp432;}); _temp431;});( void*) _temp430;}));( void*)( fe->curr_rgn=(
void*) block_typ); return Cyc_Tcenv_put_fenv( te, fe);}} void* Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv* te){ if( te->le == 0){ return Cyc_Absyn_HeapRgn;} return(
void*)( Cyc_Tcenv_get_fenv( te))->curr_rgn;} void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* rgn){ if( te->le
!= 0){ void* capability=( void*)( Cyc_Tcenv_get_fenv( te))->capability; if( Cyc_Tcutil_region_in_effect(
0, rgn, capability)? 1: Cyc_Tcutil_region_in_effect( 1, rgn, capability)){
return;}} Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp434= Cyc_Absynpp_typ2string(
rgn); xprintf("Expression accesses unavailable region %.*s", _temp434.last_plus_one
- _temp434.curr, _temp434.curr);}));} void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* eff){ void*
capability; if( te->le == 0){ capability=({ struct Cyc_Absyn_JoinEff_struct*
_temp435=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp435->tag= Cyc_Absyn_JoinEff_tag; _temp435->f1= 0;( void*) _temp435;});}
else{ struct Cyc_Tcenv_Fenv* fenv= Cyc_Tcenv_get_fenv( te); capability=( void*)
fenv->capability;} if( ! Cyc_Tcutil_subset_effect( 1, eff, capability)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp436= Cyc_Absynpp_typ2string( capability);
struct _tagged_string _temp437= Cyc_Absynpp_typ2string( eff); xprintf("Capability %.*s does not cover function's effect %.*s",
_temp436.last_plus_one - _temp436.curr, _temp436.curr, _temp437.last_plus_one -
_temp437.curr, _temp437.curr);}));}} struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv(
struct Cyc_Absyn_Fndecl* fd){ struct Cyc_Dict_Dict* locals=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Dict_empty)(
Cyc_String_zstrptrcmp); struct Cyc_Absyn_Tvar* rgn0= Cyc_Tcutil_new_tvar( Cyc_Absyn_RgnKind);
void* param_rgn=({ struct Cyc_Absyn_VarType_struct* _temp438=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp438->tag= Cyc_Absyn_VarType_tag;
_temp438->f1= rgn0;( void*) _temp438;});{ struct Cyc_List_List* args= fd->args;
for( 0; args != 0; args= args->tl){ locals=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key, void* data)) Cyc_Dict_insert)(
locals,(*(( struct _tuple6*) args->hd)).f1,({ struct Cyc_Absyn_Param_b_struct*
_temp439=( struct Cyc_Absyn_Param_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Param_b_struct));
_temp439->tag= Cyc_Absyn_Param_b_tag; _temp439->f1=({ struct Cyc_Absyn_Vardecl*
_temp440=( struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl)
* 1); _temp440[ 0]=({ struct Cyc_Absyn_Vardecl _temp441; _temp441.sc=( void*)
Cyc_Absyn_Public; _temp441.name=({ struct _tuple0* _temp444=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0) * 1); _temp444[ 0]=({ struct _tuple0 _temp445;
_temp445.f1= Cyc_Absyn_Loc_n; _temp445.f2=(*(( struct _tuple6*) args->hd)).f1;
_temp445;}); _temp444;}); _temp441.tq=(*(( struct _tuple6*) args->hd)).f2;
_temp441.type=( void*)(*(( struct _tuple6*) args->hd)).f3; _temp441.initializer=
0; _temp441.shadow= 0; _temp441.region=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp442=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp442[ 0]=({ struct Cyc_Core_Opt _temp443; _temp443.v=( void*) param_rgn;
_temp443;}); _temp442;}); _temp441;}); _temp440;});( void*) _temp439;}));}}
return({ struct Cyc_Tcenv_Fenv* _temp446=( struct Cyc_Tcenv_Fenv*) GC_malloc(
sizeof( struct Cyc_Tcenv_Fenv)); _temp446->shared=({ struct Cyc_Tcenv_SharedFenv*
_temp453=( struct Cyc_Tcenv_SharedFenv*) GC_malloc( sizeof( struct Cyc_Tcenv_SharedFenv));
_temp453->return_typ=( void*)(( void*) fd->ret_type); _temp453->seen_labels=((
struct Cyc_Dict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Dict_empty)( Cyc_String_zstrptrcmp); _temp453->needed_labels=((
struct Cyc_SlowDict_Dict*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_SlowDict_empty)( Cyc_String_zstrptrcmp); _temp453;});
_temp446->type_vars=({ struct Cyc_List_List* _temp452=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp452->hd=( void*) rgn0; _temp452->tl=
fd->tvs; _temp452;}); _temp446->locals= locals; _temp446->encloser= fd->body;
_temp446->ctrl_env=({ struct Cyc_Tcenv_CtrlEnv* _temp451=( struct Cyc_Tcenv_CtrlEnv*)
GC_malloc( sizeof( struct Cyc_Tcenv_CtrlEnv)); _temp451->continue_stmt=( void*)
Cyc_Tcenv_NotLoop_j; _temp451->break_stmt=( void*) Cyc_Tcenv_NotLoop_j; _temp451->fallthru_stmt=
0; _temp451->next_stmt=( void*) Cyc_Tcenv_FnEnd_j; _temp451;}); _temp446->capability=(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp447=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp447->tag= Cyc_Absyn_JoinEff_tag;
_temp447->f1=({ struct Cyc_List_List* _temp448=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp448->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp450=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp450->tag= Cyc_Absyn_AccessEff_tag; _temp450->f1=( void*) param_rgn;( void*)
_temp450;}); _temp448->tl=({ struct Cyc_List_List* _temp449=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp449->hd=( void*)(( void*)( fd->effect)->v);
_temp449->tl= 0; _temp449;}); _temp448;});( void*) _temp447;}); _temp446->curr_rgn=(
void*) param_rgn; _temp446;});}