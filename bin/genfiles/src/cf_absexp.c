#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2;}; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3;}; struct _tuple2{ struct
Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2;}; struct _tuple3{ void* f1; void*
f2;}; struct _tuple4{ void* f1; void* f2; void* f3;}; typedef unsigned int Cyc_uint;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v;}; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern struct
_tagged_string Cyc_Core_new_string( int); extern int Cyc_Core_ptrcmp( void**,
void**); extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
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
struct Cyc_List_List_empty_struct{ char* tag;}; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag;}; extern struct Cyc_List_List*
Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern char
Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{ char* tag;}; struct Cyc_Dict_Dict;
typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict*
Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{
char* tag;}; extern char Cyc_Dict_Absent_tag[ 7u]; struct Cyc_Dict_Absent_struct{
char* tag;}; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*,
void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d,
void* key, void* data); extern int Cyc_Dict_lookup_bool( struct Cyc_Dict_Dict* d,
void* key, void** ans_place); struct Cyc_Set_Set; typedef struct Cyc_Set_Set*
Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set*
Cyc_Set_set_t; extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Lineno_Pos{
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
struct Cyc_Absyn_EmptyAnnot_struct{ char* tag;}; typedef void* Cyc_Tcenv_Resolved;
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
Cyc_Tcenv_tenv; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern char Cyc_Tcutil_TypeErr_tag[
8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag;}; typedef void* Cyc_CfFlowInfo_LocalRoot;
typedef void* Cyc_CfFlowInfo_Field; struct Cyc_CfFlowInfo_Place; typedef void*
Cyc_CfFlowInfo_InitState; typedef void* Cyc_CfFlowInfo_PathInfo; typedef void*
Cyc_CfFlowInfo_FlowInfo; typedef void* Cyc_CfFlowInfo_local_root_t; typedef void*
Cyc_CfFlowInfo_field_t; typedef struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t;
typedef void* Cyc_CfFlowInfo_init_state_t; typedef void* Cyc_CfFlowInfo_path_info_t;
typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_path_info_dict_t; typedef void* Cyc_CfFlowInfo_flow_info_t;
extern const int Cyc_CfFlowInfo_VarRoot_tag; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1;}; extern const int Cyc_CfFlowInfo_MallocPt_tag;
struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1;};
extern const int Cyc_CfFlowInfo_StructF_tag; struct Cyc_CfFlowInfo_StructF_struct{
int tag; struct _tagged_string* f1;}; extern const int Cyc_CfFlowInfo_TupleF_tag;
struct Cyc_CfFlowInfo_TupleF_struct{ int tag; int f1;}; struct Cyc_CfFlowInfo_Place{
void* root; struct Cyc_List_List* fields;}; extern void* Cyc_CfFlowInfo_Uninit;
extern void* Cyc_CfFlowInfo_ThisInit; extern void* Cyc_CfFlowInfo_AllInit;
extern const int Cyc_CfFlowInfo_MustPointTo_tag; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1;}; extern const int Cyc_CfFlowInfo_LeafPI_tag;
struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; void* f1;}; extern const int Cyc_CfFlowInfo_TuplePI_tag;
struct Cyc_CfFlowInfo_TuplePI_struct{ int tag; struct Cyc_Dict_Dict* f1;};
extern const int Cyc_CfFlowInfo_StructPI_tag; struct Cyc_CfFlowInfo_StructPI_struct{
int tag; struct Cyc_Dict_Dict* f1;}; extern void* Cyc_CfFlowInfo_BottomFL;
extern const int Cyc_CfFlowInfo_InitsFL_tag; struct Cyc_CfFlowInfo_InitsFL_struct{
int tag; struct Cyc_Dict_Dict* f1; struct Cyc_Dict_Dict* f2;}; extern void* Cyc_CfFlowInfo_join_flow(
void*, void*); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t; extern char
Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1;}; extern char Cyc_Stdio_FileCloseError_tag[ 15u];
struct Cyc_Stdio_FileCloseError_struct{ char* tag;}; extern void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_Absyn_Stmt*, void*); void Cyc_NewControlFlow_cf_check( struct Cyc_List_List*
ds); extern char Cyc_CfAbsexp_BadAbsexp_tag[ 10u]; struct Cyc_CfAbsexp_BadAbsexp_struct{
char* tag;}; typedef void* Cyc_CfAbsexp_Absexp; typedef void* Cyc_CfAbsexp_Absexpgroup;
extern void* Cyc_CfAbsexp_OrderedG; extern void* Cyc_CfAbsexp_UnorderedG; extern
void* Cyc_CfAbsexp_OneofG; typedef void* Cyc_CfAbsexp_Absop; typedef void* Cyc_CfAbsexp_absexp_t;
typedef void* Cyc_CfAbsexp_absexpgroup_t; typedef void* Cyc_CfAbsexp_absop_t;
extern void Cyc_CfAbsexp_check_absexp( void*); extern struct _tagged_string Cyc_CfAbsexp_absexp2string(
void* ae, int depth); extern void* Cyc_CfAbsexp_mkUnknownOp(); extern void* Cyc_CfAbsexp_mkAddressOp(
void* ao); extern void* Cyc_CfAbsexp_mkLocalOp( struct Cyc_Absyn_Vardecl* vd);
extern void* Cyc_CfAbsexp_mkMemberOp( void* ao, void* f); extern void* Cyc_CfAbsexp_mkMallocOp(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkDerefOp( void* ao); extern
void* Cyc_CfAbsexp_mkBottomAE(); extern void* Cyc_CfAbsexp_mkSkipAE(); extern
void* Cyc_CfAbsexp_mkUseAE( void* ao); extern void* Cyc_CfAbsexp_mkAssignAE(
void* l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE( struct Cyc_Absyn_Exp* e);
extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt* s); extern void* Cyc_CfAbsexp_mkGroupAE(
void* g, void* ae1, void* ae2); extern void* Cyc_CfAbsexp_mkGroupAE_l( void* g,
struct Cyc_List_List* ael); extern int Cyc_CfAbsexp_isUnknownOp( void*); char
Cyc_CfAbsexp_BadAbsexp_tag[ 10u]="BadAbsexp"; typedef struct Cyc_Absyn_Vardecl*
Cyc_CfAbsexp_vardecl_t; typedef struct Cyc_Dict_Dict* Cyc_CfAbsexp_dict_t;
typedef struct Cyc_Position_Segment* Cyc_CfAbsexp_seg_t; void* Cyc_CfAbsexp_OrderedG=(
void*) 0; void* Cyc_CfAbsexp_UnorderedG=( void*) 1; void* Cyc_CfAbsexp_OneofG=(
void*) 2; void* Cyc_CfAbsexp_BottomAE=( void*) 0; void* Cyc_CfAbsexp_SkipAE=(
void*) 1; const int Cyc_CfAbsexp_UseAE_tag= 0; struct Cyc_CfAbsexp_UseAE_struct{
int tag; void* f1;}; const int Cyc_CfAbsexp_AssignAE_tag= 1; struct Cyc_CfAbsexp_AssignAE_struct{
int tag; void* f1; void* f2;}; const int Cyc_CfAbsexp_MallocAE_tag= 2; struct
Cyc_CfAbsexp_MallocAE_struct{ int tag; struct Cyc_Absyn_Exp* f1; int f2;}; const
int Cyc_CfAbsexp_StmtAE_tag= 3; struct Cyc_CfAbsexp_StmtAE_struct{ int tag;
struct Cyc_Absyn_Stmt* f1;}; const int Cyc_CfAbsexp_GroupAE_tag= 4; struct Cyc_CfAbsexp_GroupAE_struct{
int tag; void* f1; struct Cyc_List_List* f2;}; void* Cyc_CfAbsexp_UnknownOp=(
void*) 0; const int Cyc_CfAbsexp_AddressOp_tag= 0; struct Cyc_CfAbsexp_AddressOp_struct{
int tag; void* f1;}; const int Cyc_CfAbsexp_DerefOp_tag= 1; struct Cyc_CfAbsexp_DerefOp_struct{
int tag; void* f1;}; const int Cyc_CfAbsexp_MemberOp_tag= 2; struct Cyc_CfAbsexp_MemberOp_struct{
int tag; void* f1; void* f2;}; const int Cyc_CfAbsexp_LocalOp_tag= 3; struct Cyc_CfAbsexp_LocalOp_struct{
int tag; struct Cyc_Absyn_Vardecl* f1;}; const int Cyc_CfAbsexp_MallocOp_tag= 4;
struct Cyc_CfAbsexp_MallocOp_struct{ int tag; struct Cyc_Absyn_Exp* f1; int f2;};
static void Cyc_CfAbsexp_ok_address_arg( void* ao){ void* _temp0= ao; struct Cyc_Absyn_Vardecl*
_temp10; void* _temp12; void* _temp14; int _temp16; struct Cyc_Absyn_Exp*
_temp18; _LL2: if(( unsigned int) _temp0 > 1u?(( struct _enum_struct*) _temp0)->tag
== Cyc_CfAbsexp_LocalOp_tag: 0){ _LL11: _temp10=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_CfAbsexp_LocalOp_struct*) _temp0)->f1; goto _LL3;} else{ goto _LL4;}
_LL4: if(( unsigned int) _temp0 > 1u?(( struct _enum_struct*) _temp0)->tag ==
Cyc_CfAbsexp_MemberOp_tag: 0){ _LL15: _temp14=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp0)->f1; goto _LL13; _LL13: _temp12=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp0)->f2; goto _LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp0 > 1u?((
struct _enum_struct*) _temp0)->tag == Cyc_CfAbsexp_MallocOp_tag: 0){ _LL19:
_temp18=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp0)->f1;
goto _LL17; _LL17: _temp16=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*) _temp0)->f2;
goto _LL7;} else{ goto _LL8;} _LL8: goto _LL9; _LL3: return; _LL5: return; _LL7:
return; _LL9:( void) _throw(({ struct Cyc_CfAbsexp_BadAbsexp_struct* _temp20=(
struct Cyc_CfAbsexp_BadAbsexp_struct*) GC_malloc_atomic( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct));*
_temp20=( struct Cyc_CfAbsexp_BadAbsexp_struct){.tag= Cyc_CfAbsexp_BadAbsexp_tag};(
struct _xenum_struct*) _temp20;})); _LL1:;} static void Cyc_CfAbsexp_ok_member_arg(
void* ao){ void* _temp21= ao; struct Cyc_Absyn_Vardecl* _temp33; void* _temp35;
void* _temp37; int _temp39; struct Cyc_Absyn_Exp* _temp41; void* _temp43; _LL23:
if(( unsigned int) _temp21 > 1u?(( struct _enum_struct*) _temp21)->tag == Cyc_CfAbsexp_LocalOp_tag:
0){ _LL34: _temp33=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp21)->f1; goto _LL24;} else{ goto _LL25;} _LL25: if(( unsigned int) _temp21
> 1u?(( struct _enum_struct*) _temp21)->tag == Cyc_CfAbsexp_MemberOp_tag: 0){
_LL38: _temp37=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp21)->f1;
goto _LL36; _LL36: _temp35=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp21)->f2; goto _LL26;} else{ goto _LL27;} _LL27: if(( unsigned int) _temp21
> 1u?(( struct _enum_struct*) _temp21)->tag == Cyc_CfAbsexp_MallocOp_tag: 0){
_LL42: _temp41=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp21)->f1; goto _LL40; _LL40: _temp39=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp21)->f2; goto _LL28;} else{ goto _LL29;} _LL29: if(( unsigned int) _temp21
> 1u?(( struct _enum_struct*) _temp21)->tag == Cyc_CfAbsexp_DerefOp_tag: 0){
_LL44: _temp43=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp21)->f1;
goto _LL30;} else{ goto _LL31;} _LL31: goto _LL32; _LL24: return; _LL26: return;
_LL28: return; _LL30: return; _LL32:( void) _throw(({ struct Cyc_CfAbsexp_BadAbsexp_struct*
_temp45=( struct Cyc_CfAbsexp_BadAbsexp_struct*) GC_malloc_atomic( sizeof(
struct Cyc_CfAbsexp_BadAbsexp_struct));* _temp45=( struct Cyc_CfAbsexp_BadAbsexp_struct){.tag=
Cyc_CfAbsexp_BadAbsexp_tag};( struct _xenum_struct*) _temp45;})); _LL22:;}
static void Cyc_CfAbsexp_ok_deref_arg( void* ao){ void* _temp46= ao; struct Cyc_Absyn_Vardecl*
_temp56; void* _temp58; void* _temp60; void* _temp62; _LL48: if(( unsigned int)
_temp46 > 1u?(( struct _enum_struct*) _temp46)->tag == Cyc_CfAbsexp_LocalOp_tag:
0){ _LL57: _temp56=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp46)->f1; goto _LL49;} else{ goto _LL50;} _LL50: if(( unsigned int) _temp46
> 1u?(( struct _enum_struct*) _temp46)->tag == Cyc_CfAbsexp_MemberOp_tag: 0){
_LL61: _temp60=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp46)->f1;
goto _LL59; _LL59: _temp58=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp46)->f2; goto _LL51;} else{ goto _LL52;} _LL52: if(( unsigned int) _temp46
> 1u?(( struct _enum_struct*) _temp46)->tag == Cyc_CfAbsexp_DerefOp_tag: 0){
_LL63: _temp62=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp46)->f1;
goto _LL53;} else{ goto _LL54;} _LL54: goto _LL55; _LL49: return; _LL51: return;
_LL53: return; _LL55:( void) _throw(({ struct Cyc_CfAbsexp_BadAbsexp_struct*
_temp64=( struct Cyc_CfAbsexp_BadAbsexp_struct*) GC_malloc_atomic( sizeof(
struct Cyc_CfAbsexp_BadAbsexp_struct));* _temp64=( struct Cyc_CfAbsexp_BadAbsexp_struct){.tag=
Cyc_CfAbsexp_BadAbsexp_tag};( struct _xenum_struct*) _temp64;})); _LL47:;}
static void Cyc_CfAbsexp_ok_lvalue( void* ao){ void* _temp65= ao; void* _temp73;
_LL67: if( _temp65 == Cyc_CfAbsexp_UnknownOp){ goto _LL68;} else{ goto _LL69;}
_LL69: if(( unsigned int) _temp65 > 1u?(( struct _enum_struct*) _temp65)->tag ==
Cyc_CfAbsexp_AddressOp_tag: 0){ _LL74: _temp73=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*)
_temp65)->f1; goto _LL70;} else{ goto _LL71;} _LL71: goto _LL72; _LL68: goto
_LL70; _LL70:( void) _throw(({ struct Cyc_CfAbsexp_BadAbsexp_struct* _temp75=(
struct Cyc_CfAbsexp_BadAbsexp_struct*) GC_malloc_atomic( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct));*
_temp75=( struct Cyc_CfAbsexp_BadAbsexp_struct){.tag= Cyc_CfAbsexp_BadAbsexp_tag};(
struct _xenum_struct*) _temp75;})); _LL72: return; _LL66:;} static int Cyc_CfAbsexp_num_mallocpts=
0; static struct Cyc_Dict_Dict** Cyc_CfAbsexp_mallocpt_dict= 0; int Cyc_CfAbsexp_mallocpt_int(
struct Cyc_Absyn_Exp* e){ if( Cyc_CfAbsexp_mallocpt_dict == 0){ Cyc_CfAbsexp_mallocpt_dict=(
struct Cyc_Dict_Dict**)({ struct Cyc_Dict_Dict** _temp76=( struct Cyc_Dict_Dict**)
GC_malloc( sizeof( struct Cyc_Dict_Dict*) * 1); _temp76[ 0]=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*))) Cyc_Dict_empty)((
int(*)( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*)) Cyc_Core_ptrcmp); _temp76;});}{
int i= 0; if( !(( int(*)( struct Cyc_Dict_Dict* d, struct Cyc_Absyn_Exp* key,
int* ans_place)) Cyc_Dict_lookup_bool)(* Cyc_CfAbsexp_mallocpt_dict, e,& i)){*
Cyc_CfAbsexp_mallocpt_dict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_Absyn_Exp* key, int data)) Cyc_Dict_insert)(* Cyc_CfAbsexp_mallocpt_dict,
e,( i=( ++ Cyc_CfAbsexp_num_mallocpts)));} return i;}} void* Cyc_CfAbsexp_mkUnknownOp(){
return Cyc_CfAbsexp_UnknownOp;} void* Cyc_CfAbsexp_mkAddressOp( void* ao){ Cyc_CfAbsexp_ok_address_arg(
ao); return({ struct Cyc_CfAbsexp_AddressOp_struct* _temp77=( struct Cyc_CfAbsexp_AddressOp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_AddressOp_struct)); _temp77->tag= Cyc_CfAbsexp_AddressOp_tag;
_temp77->f1=( void*) ao;( void*) _temp77;});} void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd){ return({ struct Cyc_CfAbsexp_LocalOp_struct*
_temp78=( struct Cyc_CfAbsexp_LocalOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_LocalOp_struct));
_temp78->tag= Cyc_CfAbsexp_LocalOp_tag; _temp78->f1= vd;( void*) _temp78;});}
void* Cyc_CfAbsexp_mkMemberOp( void* ao, void* f){ Cyc_CfAbsexp_ok_member_arg(
ao); return({ struct Cyc_CfAbsexp_MemberOp_struct* _temp79=( struct Cyc_CfAbsexp_MemberOp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_MemberOp_struct)); _temp79->tag= Cyc_CfAbsexp_MemberOp_tag;
_temp79->f1=( void*) ao; _temp79->f2=( void*) f;( void*) _temp79;});} void* Cyc_CfAbsexp_mkMallocOp(
struct Cyc_Absyn_Exp* e){ return({ struct Cyc_CfAbsexp_MallocOp_struct* _temp80=(
struct Cyc_CfAbsexp_MallocOp_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_MallocOp_struct));
_temp80->tag= Cyc_CfAbsexp_MallocOp_tag; _temp80->f1= e; _temp80->f2= Cyc_CfAbsexp_mallocpt_int(
e);( void*) _temp80;});} void* Cyc_CfAbsexp_mkDerefOp( void* ao){ Cyc_CfAbsexp_ok_deref_arg(
ao); return({ struct Cyc_CfAbsexp_DerefOp_struct* _temp81=( struct Cyc_CfAbsexp_DerefOp_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_DerefOp_struct)); _temp81->tag= Cyc_CfAbsexp_DerefOp_tag;
_temp81->f1=( void*) ao;( void*) _temp81;});} void* Cyc_CfAbsexp_mkBottomAE(){
return Cyc_CfAbsexp_BottomAE;} void* Cyc_CfAbsexp_mkSkipAE(){ return Cyc_CfAbsexp_SkipAE;}
void* Cyc_CfAbsexp_mkMallocAE( struct Cyc_Absyn_Exp* e){ return({ struct Cyc_CfAbsexp_MallocAE_struct*
_temp82=( struct Cyc_CfAbsexp_MallocAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_MallocAE_struct));
_temp82->tag= Cyc_CfAbsexp_MallocAE_tag; _temp82->f1= e; _temp82->f2= Cyc_CfAbsexp_mallocpt_int(
e);( void*) _temp82;});} void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt* s){
return({ struct Cyc_CfAbsexp_StmtAE_struct* _temp83=( struct Cyc_CfAbsexp_StmtAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_StmtAE_struct)); _temp83->tag= Cyc_CfAbsexp_StmtAE_tag;
_temp83->f1= s;( void*) _temp83;});} void* Cyc_CfAbsexp_mkUseAE( void* ao){ void*
_temp84= ao; _LL86: if( _temp84 == Cyc_CfAbsexp_UnknownOp){ goto _LL87;} else{
goto _LL88;} _LL88: goto _LL89; _LL87: return Cyc_CfAbsexp_SkipAE; _LL89: return({
struct Cyc_CfAbsexp_UseAE_struct* _temp90=( struct Cyc_CfAbsexp_UseAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_UseAE_struct)); _temp90->tag= Cyc_CfAbsexp_UseAE_tag;
_temp90->f1=( void*) ao;( void*) _temp90;}); _LL85:;} void* Cyc_CfAbsexp_mkAssignAE(
void* l, void* r){ struct _tuple3 _temp92=({ struct _tuple3 _temp91; _temp91.f1=
l; _temp91.f2= r; _temp91;}); void* _temp100; void* _temp102; void* _temp104;
void* _temp106; _LL94: _LL103: _temp102= _temp92.f1; if( _temp102 == Cyc_CfAbsexp_UnknownOp){
goto _LL101;} else{ goto _LL96;} _LL101: _temp100= _temp92.f2; if( _temp100 ==
Cyc_CfAbsexp_UnknownOp){ goto _LL95;} else{ goto _LL96;} _LL96: _LL107: _temp106=
_temp92.f1; if( _temp106 == Cyc_CfAbsexp_UnknownOp){ goto _LL105;} else{ goto
_LL98;} _LL105: _temp104= _temp92.f2; goto _LL97; _LL98: goto _LL99; _LL95:
return Cyc_CfAbsexp_SkipAE; _LL97: return Cyc_CfAbsexp_mkUseAE( r); _LL99: Cyc_CfAbsexp_ok_lvalue(
l); return({ struct Cyc_CfAbsexp_AssignAE_struct* _temp108=( struct Cyc_CfAbsexp_AssignAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_AssignAE_struct)); _temp108->tag= Cyc_CfAbsexp_AssignAE_tag;
_temp108->f1=( void*) l; _temp108->f2=( void*) r;( void*) _temp108;}); _LL93:;}
void* Cyc_CfAbsexp_mkGroupAE( void* g, void* ae1, void* ae2){ struct _tuple4
_temp110=({ struct _tuple4 _temp109; _temp109.f1= g; _temp109.f2= ae1; _temp109.f3=
ae2; _temp109;}); void* _temp133; void* _temp135; void* _temp137; void* _temp140;
void* _temp142; void* _temp144; void* _temp146; void* _temp148; void* _temp150;
void* _temp152; void* _temp154; void* _temp156; void* _temp158; void* _temp160;
void* _temp162; void* _temp165; struct Cyc_List_List* _temp167; void* _temp169;
void* _temp171; struct Cyc_List_List* _temp173; void* _temp175; void* _temp177;
void* _temp180; void* _temp182; struct Cyc_List_List* _temp184; void* _temp186;
void* _temp188; void* _temp190; void* _temp192; struct Cyc_List_List* _temp194;
void* _temp196; void* _temp198; void* _temp201; struct Cyc_List_List* _temp203;
void* _temp205; void* _temp207; void* _temp209; _LL112: _LL138: _temp137=
_temp110.f1; goto _LL136; _LL136: _temp135= _temp110.f2; goto _LL134; _LL134:
_temp133= _temp110.f3; if( _temp133 == Cyc_CfAbsexp_SkipAE){ goto _LL132;} else{
goto _LL114;} _LL132: if( g != Cyc_CfAbsexp_OneofG){ goto _LL113;} else{ goto
_LL114;} _LL114: _LL145: _temp144= _temp110.f1; goto _LL143; _LL143: _temp142=
_temp110.f2; if( _temp142 == Cyc_CfAbsexp_SkipAE){ goto _LL141;} else{ goto
_LL116;} _LL141: _temp140= _temp110.f3; goto _LL139; _LL139: if( g != Cyc_CfAbsexp_OneofG){
goto _LL115;} else{ goto _LL116;} _LL116: _LL151: _temp150= _temp110.f1; if(
_temp150 == Cyc_CfAbsexp_OneofG){ goto _LL149;} else{ goto _LL118;} _LL149:
_temp148= _temp110.f2; goto _LL147; _LL147: _temp146= _temp110.f3; if( _temp146
== Cyc_CfAbsexp_BottomAE){ goto _LL117;} else{ goto _LL118;} _LL118: _LL157:
_temp156= _temp110.f1; if( _temp156 == Cyc_CfAbsexp_OneofG){ goto _LL155;} else{
goto _LL120;} _LL155: _temp154= _temp110.f2; if( _temp154 == Cyc_CfAbsexp_BottomAE){
goto _LL153;} else{ goto _LL120;} _LL153: _temp152= _temp110.f3; goto _LL119;
_LL120: _LL163: _temp162= _temp110.f1; if( _temp162 == Cyc_CfAbsexp_OrderedG){
goto _LL161;} else{ goto _LL122;} _LL161: _temp160= _temp110.f2; if( _temp160 ==
Cyc_CfAbsexp_BottomAE){ goto _LL159;} else{ goto _LL122;} _LL159: _temp158=
_temp110.f3; goto _LL121; _LL122: _LL178: _temp177= _temp110.f1; goto _LL172;
_LL172: _temp171= _temp110.f2; if(( unsigned int) _temp171 > 2u?(( struct
_enum_struct*) _temp171)->tag == Cyc_CfAbsexp_GroupAE_tag: 0){ _LL176: _temp175=(
void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp171)->f1; goto _LL174; _LL174:
_temp173=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp171)->f2; goto _LL166;} else{ goto _LL124;} _LL166: _temp165= _temp110.f3;
if(( unsigned int) _temp165 > 2u?(( struct _enum_struct*) _temp165)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL170: _temp169=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp165)->f1;
goto _LL168; _LL168: _temp167=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp165)->f2; goto _LL164;} else{ goto _LL124;} _LL164: if( _temp177 ==
_temp175? _temp177 == _temp169: 0){ goto _LL123;} else{ goto _LL124;} _LL124:
_LL189: _temp188= _temp110.f1; goto _LL183; _LL183: _temp182= _temp110.f2; if((
unsigned int) _temp182 > 2u?(( struct _enum_struct*) _temp182)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL187: _temp186=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp182)->f1;
goto _LL185; _LL185: _temp184=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp182)->f2; goto _LL181;} else{ goto _LL126;} _LL181: _temp180= _temp110.f3;
goto _LL179; _LL179: if( _temp188 == _temp186? _temp188 != Cyc_CfAbsexp_OrderedG:
0){ goto _LL125;} else{ goto _LL126;} _LL126: _LL199: _temp198= _temp110.f1; if(
_temp198 == Cyc_CfAbsexp_OrderedG){ goto _LL193;} else{ goto _LL128;} _LL193:
_temp192= _temp110.f2; if(( unsigned int) _temp192 > 2u?(( struct _enum_struct*)
_temp192)->tag == Cyc_CfAbsexp_GroupAE_tag: 0){ _LL197: _temp196=( void*)((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp192)->f1; if( _temp196 == Cyc_CfAbsexp_OrderedG){
goto _LL195;} else{ goto _LL128;} _LL195: _temp194=( struct Cyc_List_List*)((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp192)->f2; goto _LL191;} else{ goto
_LL128;} _LL191: _temp190= _temp110.f3; goto _LL127; _LL128: _LL210: _temp209=
_temp110.f1; goto _LL208; _LL208: _temp207= _temp110.f2; goto _LL202; _LL202:
_temp201= _temp110.f3; if(( unsigned int) _temp201 > 2u?(( struct _enum_struct*)
_temp201)->tag == Cyc_CfAbsexp_GroupAE_tag: 0){ _LL206: _temp205=( void*)((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp201)->f1; goto _LL204; _LL204:
_temp203=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp201)->f2; goto _LL200;} else{ goto _LL130;} _LL200: if( _temp209 ==
_temp205){ goto _LL129;} else{ goto _LL130;} _LL130: goto _LL131; _LL113: return
ae1; _LL115: return ae2; _LL117: return ae1; _LL119: return ae2; _LL121: return
Cyc_CfAbsexp_BottomAE; _LL123: return({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp211=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp211->tag= Cyc_CfAbsexp_GroupAE_tag; _temp211->f1=( void*) _temp177;
_temp211->f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp173, _temp167);( void*) _temp211;}); _LL125: return({
struct Cyc_CfAbsexp_GroupAE_struct* _temp212=( struct Cyc_CfAbsexp_GroupAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp212->tag= Cyc_CfAbsexp_GroupAE_tag;
_temp212->f1=( void*) _temp188; _temp212->f2=( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp213=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp213[ 0]=({ struct Cyc_List_List _temp214; _temp214.hd=( void*) ae2;
_temp214.tl= _temp184; _temp214;}); _temp213;});( void*) _temp212;}); _LL127:
return({ struct Cyc_CfAbsexp_GroupAE_struct* _temp215=( struct Cyc_CfAbsexp_GroupAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp215->tag= Cyc_CfAbsexp_GroupAE_tag;
_temp215->f1=( void*) Cyc_CfAbsexp_OrderedG; _temp215->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp194,(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp216=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp216[ 0]=({ struct Cyc_List_List
_temp217; _temp217.hd=( void*) ae2; _temp217.tl= 0; _temp217;}); _temp216;}));(
void*) _temp215;}); _LL129: return({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp218=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp218->tag= Cyc_CfAbsexp_GroupAE_tag; _temp218->f1=( void*) _temp209;
_temp218->f2=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp219=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp219[ 0]=({
struct Cyc_List_List _temp220; _temp220.hd=( void*) ae1; _temp220.tl= _temp203;
_temp220;}); _temp219;});( void*) _temp218;}); _LL131: return({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp221=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp221->tag= Cyc_CfAbsexp_GroupAE_tag; _temp221->f1=( void*) g; _temp221->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp222=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp222[ 0]=({ struct Cyc_List_List
_temp223; _temp223.hd=( void*) ae1; _temp223.tl=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp224=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp224[ 0]=({ struct Cyc_List_List _temp225;
_temp225.hd=( void*) ae2; _temp225.tl= 0; _temp225;}); _temp224;}); _temp223;});
_temp222;});( void*) _temp221;}); _LL111:;} void* Cyc_CfAbsexp_mkGroupAE_l( void*
g, struct Cyc_List_List* ael){ if( ael == 0){ return Cyc_CfAbsexp_SkipAE;} if(
ael->tl == 0){ return( void*) ael->hd;} return({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp226=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp226->tag= Cyc_CfAbsexp_GroupAE_tag; _temp226->f1=( void*) g; _temp226->f2=
ael;( void*) _temp226;});} int Cyc_CfAbsexp_isUnknownOp( void* ao){ return ao ==
Cyc_CfAbsexp_UnknownOp;} void* Cyc_CfAbsexp_eval_absop_l( struct Cyc_Position_Segment*
loc, void* ao, void* flow){ return Cyc_CfFlowInfo_Uninit;} void* Cyc_CfAbsexp_eval_absop_r(
struct Cyc_Position_Segment* loc, void* ao, void* flow){ return Cyc_CfFlowInfo_Uninit;}
void* Cyc_CfAbsexp_eval_absexp( struct Cyc_Position_Segment* loc, void* ae, void*
in_flow){ void* _temp227= ae; void* _temp247; void* _temp249; void* _temp251;
int _temp253; struct Cyc_Absyn_Exp* _temp255; struct Cyc_Absyn_Stmt* _temp257;
struct Cyc_List_List* _temp259; void* _temp261; struct Cyc_List_List* _temp263;
void* _temp265; struct Cyc_List_List* _temp267; void* _temp269; _LL229: if((
unsigned int) _temp227 > 2u?(( struct _enum_struct*) _temp227)->tag == Cyc_CfAbsexp_UseAE_tag:
0){ _LL248: _temp247=( void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp227)->f1;
goto _LL230;} else{ goto _LL231;} _LL231: if(( unsigned int) _temp227 > 2u?((
struct _enum_struct*) _temp227)->tag == Cyc_CfAbsexp_AssignAE_tag: 0){ _LL252:
_temp251=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp227)->f1; goto
_LL250; _LL250: _temp249=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp227)->f2; goto _LL232;} else{ goto _LL233;} _LL233: if(( unsigned int)
_temp227 > 2u?(( struct _enum_struct*) _temp227)->tag == Cyc_CfAbsexp_MallocAE_tag:
0){ _LL256: _temp255=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp227)->f1; goto _LL254; _LL254: _temp253=( int)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp227)->f2; goto _LL234;} else{ goto _LL235;} _LL235: if( _temp227 == Cyc_CfAbsexp_BottomAE){
goto _LL236;} else{ goto _LL237;} _LL237: if( _temp227 == Cyc_CfAbsexp_SkipAE){
goto _LL238;} else{ goto _LL239;} _LL239: if(( unsigned int) _temp227 > 2u?((
struct _enum_struct*) _temp227)->tag == Cyc_CfAbsexp_StmtAE_tag: 0){ _LL258:
_temp257=( struct Cyc_Absyn_Stmt*)(( struct Cyc_CfAbsexp_StmtAE_struct*)
_temp227)->f1; goto _LL240;} else{ goto _LL241;} _LL241: if(( unsigned int)
_temp227 > 2u?(( struct _enum_struct*) _temp227)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL262: _temp261=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp227)->f1;
if( _temp261 == Cyc_CfAbsexp_OrderedG){ goto _LL260;} else{ goto _LL243;} _LL260:
_temp259=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp227)->f2; goto _LL242;} else{ goto _LL243;} _LL243: if(( unsigned int)
_temp227 > 2u?(( struct _enum_struct*) _temp227)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL266: _temp265=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp227)->f1;
if( _temp265 == Cyc_CfAbsexp_OneofG){ goto _LL264;} else{ goto _LL245;} _LL264:
_temp263=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp227)->f2; goto _LL244;} else{ goto _LL245;} _LL245: if(( unsigned int)
_temp227 > 2u?(( struct _enum_struct*) _temp227)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL270: _temp269=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp227)->f1;
if( _temp269 == Cyc_CfAbsexp_UnorderedG){ goto _LL268;} else{ goto _LL228;}
_LL268: _temp267=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp227)->f2; goto _LL246;} else{ goto _LL228;} _LL230: { void* state= Cyc_CfAbsexp_eval_absop_r(
loc, _temp247, in_flow); return in_flow;} _LL232:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp271=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp271=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp272=( char*)""; struct _tagged_string
_temp273; _temp273.curr= _temp272; _temp273.base= _temp272; _temp273.last_plus_one=
_temp272 + 1; _temp273;})};( struct _xenum_struct*) _temp271;})); _LL234:( void)
_throw(({ struct Cyc_Core_Impossible_struct* _temp274=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp274=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp275=( char*)"";
struct _tagged_string _temp276; _temp276.curr= _temp275; _temp276.base= _temp275;
_temp276.last_plus_one= _temp275 + 1; _temp276;})};( struct _xenum_struct*)
_temp274;})); _LL236: return Cyc_CfFlowInfo_BottomFL; _LL238: return in_flow;
_LL240: return Cyc_NewControlFlow_cf_analyze_stmt( _temp257, in_flow); _LL242:
for( 0; _temp259 != 0; _temp259= _temp259->tl){ in_flow= Cyc_CfAbsexp_eval_absexp(
loc,( void*) _temp259->hd, in_flow);} return in_flow; _LL244: { void* out_flow=
Cyc_CfFlowInfo_BottomFL; for( 0; _temp263 != 0; _temp263= _temp263->tl){
out_flow= Cyc_CfFlowInfo_join_flow( out_flow, Cyc_CfAbsexp_eval_absexp( loc,(
void*) _temp263->hd, in_flow));} return out_flow;} _LL246: { void* out_flow;
do { out_flow= in_flow; for( 0; _temp267 != 0; _temp267= _temp267->tl){ out_flow=
Cyc_CfFlowInfo_join_flow( out_flow, Cyc_CfAbsexp_eval_absexp( loc,( void*)
_temp267->hd, out_flow));}} while ( out_flow != in_flow); return out_flow;}
_LL228:;} struct _tagged_string Cyc_CfAbsexp_absop2string( void* ao){ void*
_temp277= ao; void* _temp291; struct Cyc_Absyn_Vardecl* _temp293; void* _temp295;
void* _temp297; int _temp299; struct Cyc_Absyn_Exp* _temp301; void* _temp303;
_LL279: if( _temp277 == Cyc_CfAbsexp_UnknownOp){ goto _LL280;} else{ goto _LL281;}
_LL281: if(( unsigned int) _temp277 > 1u?(( struct _enum_struct*) _temp277)->tag
== Cyc_CfAbsexp_AddressOp_tag: 0){ _LL292: _temp291=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*)
_temp277)->f1; goto _LL282;} else{ goto _LL283;} _LL283: if(( unsigned int)
_temp277 > 1u?(( struct _enum_struct*) _temp277)->tag == Cyc_CfAbsexp_LocalOp_tag:
0){ _LL294: _temp293=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp277)->f1; goto _LL284;} else{ goto _LL285;} _LL285: if(( unsigned int)
_temp277 > 1u?(( struct _enum_struct*) _temp277)->tag == Cyc_CfAbsexp_MemberOp_tag:
0){ _LL298: _temp297=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp277)->f1;
goto _LL296; _LL296: _temp295=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp277)->f2; goto _LL286;} else{ goto _LL287;} _LL287: if(( unsigned int)
_temp277 > 1u?(( struct _enum_struct*) _temp277)->tag == Cyc_CfAbsexp_MallocOp_tag:
0){ _LL302: _temp301=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp277)->f1; goto _LL300; _LL300: _temp299=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp277)->f2; goto _LL288;} else{ goto _LL289;} _LL289: if(( unsigned int)
_temp277 > 1u?(( struct _enum_struct*) _temp277)->tag == Cyc_CfAbsexp_DerefOp_tag:
0){ _LL304: _temp303=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp277)->f1;
goto _LL290;} else{ goto _LL278;} _LL280: return( struct _tagged_string)({ char*
_temp305=( char*)"Unknown"; struct _tagged_string _temp306; _temp306.curr=
_temp305; _temp306.base= _temp305; _temp306.last_plus_one= _temp305 + 8;
_temp306;}); _LL282: return({ struct _tagged_string _temp307= Cyc_CfAbsexp_absop2string(
_temp291); xprintf("& %.*s", _temp307.last_plus_one - _temp307.curr, _temp307.curr);});
_LL284: return*(* _temp293->name).f2; _LL286: { struct _tagged_string fs;{ void*
_temp308= _temp295; struct _tagged_string* _temp314; int _temp316; _LL310: if(((
struct _enum_struct*) _temp308)->tag == Cyc_CfFlowInfo_StructF_tag){ _LL315:
_temp314=( struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp308)->f1; goto _LL311;} else{ goto _LL312;} _LL312: if((( struct
_enum_struct*) _temp308)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL317: _temp316=(
int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp308)->f1; goto _LL313;} else{
goto _LL309;} _LL311: fs=* _temp314; goto _LL309; _LL313: fs= xprintf("%d",
_temp316); goto _LL309; _LL309:;} return({ struct _tagged_string _temp318= Cyc_CfAbsexp_absop2string(
_temp297); struct _tagged_string _temp319= fs; xprintf("%.*s.%.*s", _temp318.last_plus_one
- _temp318.curr, _temp318.curr, _temp319.last_plus_one - _temp319.curr, _temp319.curr);});}
_LL288: return xprintf("<mpt%d>", _temp299); _LL290: return({ struct
_tagged_string _temp320= Cyc_CfAbsexp_absop2string( _temp303); xprintf("(*%.*s)",
_temp320.last_plus_one - _temp320.curr, _temp320.curr);}); _LL278:;} struct
_tagged_string Cyc_CfAbsexp_absexp2string( void* ae, int depth){ struct
_tagged_string ans= Cyc_Core_new_string( depth);{ int i= 0; for( 0;(
unsigned int) i <({ struct _tagged_string _temp321= ans;( unsigned int)(
_temp321.last_plus_one - _temp321.curr);}); ++ i){({ struct _tagged_string
_temp322= ans; char* _temp324= _temp322.curr + i; if( _temp324 < _temp322.base?
1: _temp324 >= _temp322.last_plus_one){ _throw( Null_Exception);}* _temp324=' ';});}}{
void* _temp325= ae; void* _temp341; int _temp343; struct Cyc_Absyn_Exp* _temp345;
void* _temp347; void* _temp349; struct Cyc_List_List* _temp351; void* _temp353;
struct Cyc_Absyn_Stmt* _temp355; _LL327: if( _temp325 == Cyc_CfAbsexp_SkipAE){
goto _LL328;} else{ goto _LL329;} _LL329: if(( unsigned int) _temp325 > 2u?((
struct _enum_struct*) _temp325)->tag == Cyc_CfAbsexp_UseAE_tag: 0){ _LL342:
_temp341=( void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp325)->f1; goto
_LL330;} else{ goto _LL331;} _LL331: if(( unsigned int) _temp325 > 2u?(( struct
_enum_struct*) _temp325)->tag == Cyc_CfAbsexp_MallocAE_tag: 0){ _LL346: _temp345=(
struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp325)->f1;
goto _LL344; _LL344: _temp343=( int)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp325)->f2; goto _LL332;} else{ goto _LL333;} _LL333: if(( unsigned int)
_temp325 > 2u?(( struct _enum_struct*) _temp325)->tag == Cyc_CfAbsexp_AssignAE_tag:
0){ _LL350: _temp349=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp325)->f1;
goto _LL348; _LL348: _temp347=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp325)->f2; goto _LL334;} else{ goto _LL335;} _LL335: if(( unsigned int)
_temp325 > 2u?(( struct _enum_struct*) _temp325)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL354: _temp353=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp325)->f1;
goto _LL352; _LL352: _temp351=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp325)->f2; goto _LL336;} else{ goto _LL337;} _LL337: if(( unsigned int)
_temp325 > 2u?(( struct _enum_struct*) _temp325)->tag == Cyc_CfAbsexp_StmtAE_tag:
0){ _LL356: _temp355=( struct Cyc_Absyn_Stmt*)(( struct Cyc_CfAbsexp_StmtAE_struct*)
_temp325)->f1; goto _LL338;} else{ goto _LL339;} _LL339: if( _temp325 == Cyc_CfAbsexp_BottomAE){
goto _LL340;} else{ goto _LL326;} _LL328: ans=({ struct _tagged_string _temp357=
ans; xprintf("%.*sskip", _temp357.last_plus_one - _temp357.curr, _temp357.curr);});
goto _LL326; _LL330: ans=({ struct _tagged_string _temp358= ans; struct
_tagged_string _temp359= Cyc_CfAbsexp_absop2string( _temp341); xprintf("%.*suse %.*s",
_temp358.last_plus_one - _temp358.curr, _temp358.curr, _temp359.last_plus_one -
_temp359.curr, _temp359.curr);}); goto _LL326; _LL332: ans=({ struct
_tagged_string _temp360= ans; int _temp361= _temp343; xprintf("%.*smalloc %i",
_temp360.last_plus_one - _temp360.curr, _temp360.curr, _temp361);}); goto _LL326;
_LL334: ans=({ struct _tagged_string _temp362= ans; struct _tagged_string
_temp363= Cyc_CfAbsexp_absop2string( _temp349); struct _tagged_string _temp364=
Cyc_CfAbsexp_absop2string( _temp347); xprintf("%.*sassign %.*s %.*s", _temp362.last_plus_one
- _temp362.curr, _temp362.curr, _temp363.last_plus_one - _temp363.curr, _temp363.curr,
_temp364.last_plus_one - _temp364.curr, _temp364.curr);}); goto _LL326; _LL336:{
void* _temp365= _temp353; _LL367: if( _temp365 == Cyc_CfAbsexp_OrderedG){ goto
_LL368;} else{ goto _LL369;} _LL369: if( _temp365 == Cyc_CfAbsexp_UnorderedG){
goto _LL370;} else{ goto _LL371;} _LL371: if( _temp365 == Cyc_CfAbsexp_OneofG){
goto _LL372;} else{ goto _LL366;} _LL368: ans=({ struct _tagged_string _temp373=
ans; xprintf("%.*sordered", _temp373.last_plus_one - _temp373.curr, _temp373.curr);});
goto _LL366; _LL370: ans=({ struct _tagged_string _temp374= ans; xprintf("%.*sunordered",
_temp374.last_plus_one - _temp374.curr, _temp374.curr);}); goto _LL366; _LL372:
ans=({ struct _tagged_string _temp375= ans; xprintf("%.*soneof", _temp375.last_plus_one
- _temp375.curr, _temp375.curr);}); goto _LL366; _LL366:;} for( 0; _temp351 != 0;
_temp351= _temp351->tl){ ans=({ struct _tagged_string _temp376= ans; struct
_tagged_string _temp377= Cyc_CfAbsexp_absexp2string(( void*) _temp351->hd, depth
+ 1); xprintf("%.*s\n%.*s", _temp376.last_plus_one - _temp376.curr, _temp376.curr,
_temp377.last_plus_one - _temp377.curr, _temp377.curr);});} goto _LL326; _LL338:
ans=({ struct _tagged_string _temp378= ans; xprintf("%.*s<<stmt>>", _temp378.last_plus_one
- _temp378.curr, _temp378.curr);}); goto _LL326; _LL340: ans=({ struct
_tagged_string _temp379= ans; xprintf("%.*sbottom", _temp379.last_plus_one -
_temp379.curr, _temp379.curr);}); goto _LL326; _LL326:;} return ans;} static
void Cyc_CfAbsexp_check_absop( void* ao){ void* _temp380= ao; void* _temp394;
void* _temp396; void* _temp398; void* _temp400; struct Cyc_Absyn_Vardecl*
_temp402; int _temp404; struct Cyc_Absyn_Exp* _temp406; _LL382: if( _temp380 ==
Cyc_CfAbsexp_UnknownOp){ goto _LL383;} else{ goto _LL384;} _LL384: if((
unsigned int) _temp380 > 1u?(( struct _enum_struct*) _temp380)->tag == Cyc_CfAbsexp_AddressOp_tag:
0){ _LL395: _temp394=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp380)->f1;
goto _LL385;} else{ goto _LL386;} _LL386: if(( unsigned int) _temp380 > 1u?((
struct _enum_struct*) _temp380)->tag == Cyc_CfAbsexp_DerefOp_tag: 0){ _LL397:
_temp396=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp380)->f1; goto
_LL387;} else{ goto _LL388;} _LL388: if(( unsigned int) _temp380 > 1u?(( struct
_enum_struct*) _temp380)->tag == Cyc_CfAbsexp_MemberOp_tag: 0){ _LL401: _temp400=(
void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp380)->f1; goto _LL399;
_LL399: _temp398=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp380)->f2;
goto _LL389;} else{ goto _LL390;} _LL390: if(( unsigned int) _temp380 > 1u?((
struct _enum_struct*) _temp380)->tag == Cyc_CfAbsexp_LocalOp_tag: 0){ _LL403:
_temp402=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp380)->f1; goto _LL391;} else{ goto _LL392;} _LL392: if(( unsigned int)
_temp380 > 1u?(( struct _enum_struct*) _temp380)->tag == Cyc_CfAbsexp_MallocOp_tag:
0){ _LL407: _temp406=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp380)->f1; goto _LL405; _LL405: _temp404=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp380)->f2; goto _LL393;} else{ goto _LL381;} _LL383: return; _LL385: Cyc_CfAbsexp_check_absop(
_temp394); Cyc_CfAbsexp_ok_address_arg( _temp394); return; _LL387: Cyc_CfAbsexp_check_absop(
_temp396); Cyc_CfAbsexp_ok_deref_arg( _temp396); return; _LL389: Cyc_CfAbsexp_check_absop(
_temp400); Cyc_CfAbsexp_ok_member_arg( _temp400); return; _LL391: return; _LL393:
return; _LL381:;} void Cyc_CfAbsexp_check_absexp( void* ae){ void* _temp408= ae;
int _temp424; struct Cyc_Absyn_Exp* _temp426; struct Cyc_Absyn_Stmt* _temp428;
struct Cyc_List_List* _temp430; void* _temp432; void* _temp434; void* _temp436;
void* _temp438; _LL410: if( _temp408 == Cyc_CfAbsexp_BottomAE){ goto _LL411;}
else{ goto _LL412;} _LL412: if( _temp408 == Cyc_CfAbsexp_SkipAE){ goto _LL413;}
else{ goto _LL414;} _LL414: if(( unsigned int) _temp408 > 2u?(( struct
_enum_struct*) _temp408)->tag == Cyc_CfAbsexp_MallocAE_tag: 0){ _LL427: _temp426=(
struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp408)->f1;
goto _LL425; _LL425: _temp424=( int)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp408)->f2; goto _LL415;} else{ goto _LL416;} _LL416: if(( unsigned int)
_temp408 > 2u?(( struct _enum_struct*) _temp408)->tag == Cyc_CfAbsexp_StmtAE_tag:
0){ _LL429: _temp428=( struct Cyc_Absyn_Stmt*)(( struct Cyc_CfAbsexp_StmtAE_struct*)
_temp408)->f1; goto _LL417;} else{ goto _LL418;} _LL418: if(( unsigned int)
_temp408 > 2u?(( struct _enum_struct*) _temp408)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL433: _temp432=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp408)->f1;
goto _LL431; _LL431: _temp430=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp408)->f2; goto _LL419;} else{ goto _LL420;} _LL420: if(( unsigned int)
_temp408 > 2u?(( struct _enum_struct*) _temp408)->tag == Cyc_CfAbsexp_UseAE_tag:
0){ _LL435: _temp434=( void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp408)->f1;
goto _LL421;} else{ goto _LL422;} _LL422: if(( unsigned int) _temp408 > 2u?((
struct _enum_struct*) _temp408)->tag == Cyc_CfAbsexp_AssignAE_tag: 0){ _LL439:
_temp438=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp408)->f1; goto
_LL437; _LL437: _temp436=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp408)->f2; goto _LL423;} else{ goto _LL409;} _LL411: return; _LL413: return;
_LL415: return; _LL417: return; _LL419: for( 0; _temp430 != 0; _temp430=
_temp430->tl){ Cyc_CfAbsexp_check_absexp(( void*) _temp430->hd);} return; _LL421:
Cyc_CfAbsexp_check_absop( _temp434); return; _LL423: Cyc_CfAbsexp_check_absop(
_temp438); Cyc_CfAbsexp_check_absop( _temp436); Cyc_CfAbsexp_ok_lvalue( _temp438);
return; _LL409:;}