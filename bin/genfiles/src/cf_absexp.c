#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2;}; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3;}; struct _tuple2{ struct
Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2;}; struct _tuple3{ void* f1; void*
f2;}; struct _tuple4{ void* f1; void* f2; void* f3;}; struct _tuple5{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2;}; typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring;
typedef struct _tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t;
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void*
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v;}; typedef struct
Cyc_Core_Opt* Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string(
int); extern int Cyc_Core_ptrcmp( void**, void**); extern char Cyc_Core_InvalidArg_tag[
11u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1;};
extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1;}; extern char Cyc_Core_Impossible_tag[ 11u]; struct
Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1;}; extern char
Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char* tag;};
extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1;}; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t; extern char
Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1;}; extern char Cyc_Stdio_FileCloseError_tag[ 15u];
struct Cyc_Stdio_FileCloseError_struct{ char* tag;}; struct Cyc_List_List{ void*
hd; struct Cyc_List_List* tl;}; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag;}; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag;}; extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag;}; extern int Cyc_List_memq( struct Cyc_List_List* l, void* x); struct
Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct
Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[ 8u]; struct
Cyc_Dict_Present_struct{ char* tag;}; extern char Cyc_Dict_Absent_tag[ 7u];
struct Cyc_Dict_Absent_struct{ char* tag;}; extern struct Cyc_Dict_Dict* Cyc_Dict_empty(
int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern void* Cyc_Dict_lookup(
struct Cyc_Dict_Dict* d, void* key); extern int Cyc_Dict_lookup_bool( struct Cyc_Dict_Dict*
d, void* key, void** ans_place); extern void Cyc_Dict_iter2_c( void(* f)( void*,
void*, void*), void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2);
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Lineno_Pos{ struct _tagged_string
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
8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag;}; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); typedef void* Cyc_CfFlowInfo_LocalRoot;
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
extern void* Cyc_CfFlowInfo_mkLeafPI( void* esc, void* il); extern void* Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place); extern struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_insert_place( struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place*
place, void* pinfo); extern int Cyc_CfFlowInfo_isAllInit( struct Cyc_Dict_Dict*
pinfo_dict, void* pinfo); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_pointsto(
void* pinfo, struct Cyc_Dict_Dict* d); extern void* Cyc_CfFlowInfo_assign_unknown_dict(
void*, void*); extern void* Cyc_CfFlowInfo_join_flow( void*, void*); extern int
Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void* f2); struct Cyc_NewControlFlow_AnalEnv{
struct Cyc_Dict_Dict* roots; int in_try; void* tryflow;}; typedef struct Cyc_NewControlFlow_AnalEnv*
Cyc_NewControlFlow_analenv_t; extern void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv* env, void* new_flow); extern void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv*, struct Cyc_Absyn_Stmt*, void*); void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds); extern char Cyc_CfAbsexp_BadAbsexp_tag[ 10u]; struct
Cyc_CfAbsexp_BadAbsexp_struct{ char* tag;}; typedef void* Cyc_CfAbsexp_Absexp;
typedef void* Cyc_CfAbsexp_Absexpgroup; extern void* Cyc_CfAbsexp_OrderedG;
extern void* Cyc_CfAbsexp_UnorderedG; extern void* Cyc_CfAbsexp_OneofG; typedef
void* Cyc_CfAbsexp_Absop; typedef void* Cyc_CfAbsexp_absexp_t; typedef void* Cyc_CfAbsexp_absexpgroup_t;
typedef void* Cyc_CfAbsexp_absop_t; extern void Cyc_CfAbsexp_check_absexp( void*);
extern struct _tagged_string Cyc_CfAbsexp_absexp2string( void* ae, int depth);
extern void* Cyc_CfAbsexp_mkAnyOrderG(); extern void* Cyc_CfAbsexp_mkUnknownOp();
extern void* Cyc_CfAbsexp_mkAddressOp( void* ao); extern void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd); extern void* Cyc_CfAbsexp_mkMemberOp( void* ao,
void* f); extern void* Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp* e); extern
void* Cyc_CfAbsexp_mkDerefOp( void* ao); extern void* Cyc_CfAbsexp_mkBottomAE();
extern void* Cyc_CfAbsexp_mkSkipAE(); extern void* Cyc_CfAbsexp_mkUseAE( void*
ao); extern void* Cyc_CfAbsexp_mkAssignAE( void* l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt*
s); extern void* Cyc_CfAbsexp_mkGroupAE( void* g, void* ae1, void* ae2); extern
void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List* ael); extern int
Cyc_CfAbsexp_isUnknownOp( void*); extern void* Cyc_CfAbsexp_eval_absexp( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void* ae,
void* in_flow); char Cyc_CfAbsexp_BadAbsexp_tag[ 10u]="BadAbsexp"; typedef
struct Cyc_Absyn_Vardecl* Cyc_CfAbsexp_vardecl_t; typedef struct Cyc_Dict_Dict*
Cyc_CfAbsexp_dict_t; typedef struct Cyc_Position_Segment* Cyc_CfAbsexp_seg_t;
void* Cyc_CfAbsexp_OrderedG=( void*) 0; void* Cyc_CfAbsexp_UnorderedG=( void*) 1;
void* Cyc_CfAbsexp_OneofG=( void*) 2; void* Cyc_CfAbsexp_mkAnyOrderG(){ return
Cyc_CfAbsexp_OrderedG;} void* Cyc_CfAbsexp_BottomAE=( void*) 0; void* Cyc_CfAbsexp_SkipAE=(
void*) 1; const int Cyc_CfAbsexp_UseAE_tag= 0; struct Cyc_CfAbsexp_UseAE_struct{
int tag; void* f1;}; const int Cyc_CfAbsexp_AssignAE_tag= 1; struct Cyc_CfAbsexp_AssignAE_struct{
int tag; void* f1; void* f2;}; const int Cyc_CfAbsexp_MallocAE_tag= 2; struct
Cyc_CfAbsexp_MallocAE_struct{ int tag; void* f1; int f2;}; const int Cyc_CfAbsexp_StmtAE_tag=
3; struct Cyc_CfAbsexp_StmtAE_struct{ int tag; struct Cyc_Absyn_Stmt* f1;};
const int Cyc_CfAbsexp_GroupAE_tag= 4; struct Cyc_CfAbsexp_GroupAE_struct{ int
tag; void* f1; struct Cyc_List_List* f2;}; void* Cyc_CfAbsexp_UnknownOp=( void*)
0; const int Cyc_CfAbsexp_AddressOp_tag= 0; struct Cyc_CfAbsexp_AddressOp_struct{
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
_temp82->tag= Cyc_CfAbsexp_MallocAE_tag; _temp82->f1=( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct*
_temp83=( struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp83->tag= Cyc_CfFlowInfo_MallocPt_tag; _temp83->f1= e;( void*) _temp83;});
_temp82->f2= Cyc_CfAbsexp_mallocpt_int( e);( void*) _temp82;});} void* Cyc_CfAbsexp_mkStmtAE(
struct Cyc_Absyn_Stmt* s){ return({ struct Cyc_CfAbsexp_StmtAE_struct* _temp84=(
struct Cyc_CfAbsexp_StmtAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_StmtAE_struct));
_temp84->tag= Cyc_CfAbsexp_StmtAE_tag; _temp84->f1= s;( void*) _temp84;});} void*
Cyc_CfAbsexp_mkUseAE( void* ao){ void* _temp85= ao; _LL87: if( _temp85 == Cyc_CfAbsexp_UnknownOp){
goto _LL88;} else{ goto _LL89;} _LL89: goto _LL90; _LL88: return Cyc_CfAbsexp_SkipAE;
_LL90: return({ struct Cyc_CfAbsexp_UseAE_struct* _temp91=( struct Cyc_CfAbsexp_UseAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_UseAE_struct)); _temp91->tag= Cyc_CfAbsexp_UseAE_tag;
_temp91->f1=( void*) ao;( void*) _temp91;}); _LL86:;} void* Cyc_CfAbsexp_mkAssignAE(
void* l, void* r){ struct _tuple3 _temp93=({ struct _tuple3 _temp92; _temp92.f1=
l; _temp92.f2= r; _temp92;}); void* _temp101; void* _temp103; void* _temp105;
void* _temp107; _LL95: _LL104: _temp103= _temp93.f1; if( _temp103 == Cyc_CfAbsexp_UnknownOp){
goto _LL102;} else{ goto _LL97;} _LL102: _temp101= _temp93.f2; if( _temp101 ==
Cyc_CfAbsexp_UnknownOp){ goto _LL96;} else{ goto _LL97;} _LL97: _LL108: _temp107=
_temp93.f1; if( _temp107 == Cyc_CfAbsexp_UnknownOp){ goto _LL106;} else{ goto
_LL99;} _LL106: _temp105= _temp93.f2; goto _LL98; _LL99: goto _LL100; _LL96:
return Cyc_CfAbsexp_SkipAE; _LL98: return Cyc_CfAbsexp_mkUseAE( r); _LL100: Cyc_CfAbsexp_ok_lvalue(
l); return({ struct Cyc_CfAbsexp_AssignAE_struct* _temp109=( struct Cyc_CfAbsexp_AssignAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_AssignAE_struct)); _temp109->tag= Cyc_CfAbsexp_AssignAE_tag;
_temp109->f1=( void*) l; _temp109->f2=( void*) r;( void*) _temp109;}); _LL94:;}
void* Cyc_CfAbsexp_mkGroupAE( void* g, void* ae1, void* ae2){ struct _tuple4
_temp111=({ struct _tuple4 _temp110; _temp110.f1= g; _temp110.f2= ae1; _temp110.f3=
ae2; _temp110;}); void* _temp134; void* _temp136; void* _temp138; void* _temp141;
void* _temp143; void* _temp145; void* _temp147; void* _temp149; void* _temp151;
void* _temp153; void* _temp155; void* _temp157; void* _temp159; void* _temp161;
void* _temp163; void* _temp166; struct Cyc_List_List* _temp168; void* _temp170;
void* _temp172; struct Cyc_List_List* _temp174; void* _temp176; void* _temp178;
void* _temp181; void* _temp183; struct Cyc_List_List* _temp185; void* _temp187;
void* _temp189; void* _temp191; void* _temp193; struct Cyc_List_List* _temp195;
void* _temp197; void* _temp199; void* _temp202; struct Cyc_List_List* _temp204;
void* _temp206; void* _temp208; void* _temp210; _LL113: _LL139: _temp138=
_temp111.f1; goto _LL137; _LL137: _temp136= _temp111.f2; goto _LL135; _LL135:
_temp134= _temp111.f3; if( _temp134 == Cyc_CfAbsexp_SkipAE){ goto _LL133;} else{
goto _LL115;} _LL133: if( g != Cyc_CfAbsexp_OneofG){ goto _LL114;} else{ goto
_LL115;} _LL115: _LL146: _temp145= _temp111.f1; goto _LL144; _LL144: _temp143=
_temp111.f2; if( _temp143 == Cyc_CfAbsexp_SkipAE){ goto _LL142;} else{ goto
_LL117;} _LL142: _temp141= _temp111.f3; goto _LL140; _LL140: if( g != Cyc_CfAbsexp_OneofG){
goto _LL116;} else{ goto _LL117;} _LL117: _LL152: _temp151= _temp111.f1; if(
_temp151 == Cyc_CfAbsexp_OneofG){ goto _LL150;} else{ goto _LL119;} _LL150:
_temp149= _temp111.f2; goto _LL148; _LL148: _temp147= _temp111.f3; if( _temp147
== Cyc_CfAbsexp_BottomAE){ goto _LL118;} else{ goto _LL119;} _LL119: _LL158:
_temp157= _temp111.f1; if( _temp157 == Cyc_CfAbsexp_OneofG){ goto _LL156;} else{
goto _LL121;} _LL156: _temp155= _temp111.f2; if( _temp155 == Cyc_CfAbsexp_BottomAE){
goto _LL154;} else{ goto _LL121;} _LL154: _temp153= _temp111.f3; goto _LL120;
_LL121: _LL164: _temp163= _temp111.f1; if( _temp163 == Cyc_CfAbsexp_OrderedG){
goto _LL162;} else{ goto _LL123;} _LL162: _temp161= _temp111.f2; if( _temp161 ==
Cyc_CfAbsexp_BottomAE){ goto _LL160;} else{ goto _LL123;} _LL160: _temp159=
_temp111.f3; goto _LL122; _LL123: _LL179: _temp178= _temp111.f1; goto _LL173;
_LL173: _temp172= _temp111.f2; if(( unsigned int) _temp172 > 2u?(( struct
_enum_struct*) _temp172)->tag == Cyc_CfAbsexp_GroupAE_tag: 0){ _LL177: _temp176=(
void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp172)->f1; goto _LL175; _LL175:
_temp174=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp172)->f2; goto _LL167;} else{ goto _LL125;} _LL167: _temp166= _temp111.f3;
if(( unsigned int) _temp166 > 2u?(( struct _enum_struct*) _temp166)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL171: _temp170=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp166)->f1;
goto _LL169; _LL169: _temp168=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp166)->f2; goto _LL165;} else{ goto _LL125;} _LL165: if( _temp178 ==
_temp176? _temp178 == _temp170: 0){ goto _LL124;} else{ goto _LL125;} _LL125:
_LL190: _temp189= _temp111.f1; goto _LL184; _LL184: _temp183= _temp111.f2; if((
unsigned int) _temp183 > 2u?(( struct _enum_struct*) _temp183)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL188: _temp187=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp183)->f1;
goto _LL186; _LL186: _temp185=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp183)->f2; goto _LL182;} else{ goto _LL127;} _LL182: _temp181= _temp111.f3;
goto _LL180; _LL180: if( _temp189 == _temp187? _temp189 != Cyc_CfAbsexp_OrderedG:
0){ goto _LL126;} else{ goto _LL127;} _LL127: _LL200: _temp199= _temp111.f1; if(
_temp199 == Cyc_CfAbsexp_OrderedG){ goto _LL194;} else{ goto _LL129;} _LL194:
_temp193= _temp111.f2; if(( unsigned int) _temp193 > 2u?(( struct _enum_struct*)
_temp193)->tag == Cyc_CfAbsexp_GroupAE_tag: 0){ _LL198: _temp197=( void*)((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp193)->f1; if( _temp197 == Cyc_CfAbsexp_OrderedG){
goto _LL196;} else{ goto _LL129;} _LL196: _temp195=( struct Cyc_List_List*)((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp193)->f2; goto _LL192;} else{ goto
_LL129;} _LL192: _temp191= _temp111.f3; goto _LL128; _LL129: _LL211: _temp210=
_temp111.f1; goto _LL209; _LL209: _temp208= _temp111.f2; goto _LL203; _LL203:
_temp202= _temp111.f3; if(( unsigned int) _temp202 > 2u?(( struct _enum_struct*)
_temp202)->tag == Cyc_CfAbsexp_GroupAE_tag: 0){ _LL207: _temp206=( void*)((
struct Cyc_CfAbsexp_GroupAE_struct*) _temp202)->f1; goto _LL205; _LL205:
_temp204=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp202)->f2; goto _LL201;} else{ goto _LL131;} _LL201: if( _temp210 ==
_temp206){ goto _LL130;} else{ goto _LL131;} _LL131: goto _LL132; _LL114: return
ae1; _LL116: return ae2; _LL118: return ae1; _LL120: return ae2; _LL122: return
Cyc_CfAbsexp_BottomAE; _LL124: return({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp212=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp212->tag= Cyc_CfAbsexp_GroupAE_tag; _temp212->f1=( void*) _temp178;
_temp212->f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp174, _temp168);( void*) _temp212;}); _LL126: return({
struct Cyc_CfAbsexp_GroupAE_struct* _temp213=( struct Cyc_CfAbsexp_GroupAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp213->tag= Cyc_CfAbsexp_GroupAE_tag;
_temp213->f1=( void*) _temp189; _temp213->f2=( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp214=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp214[ 0]=({ struct Cyc_List_List _temp215; _temp215.hd=( void*) ae2;
_temp215.tl= _temp185; _temp215;}); _temp214;});( void*) _temp213;}); _LL128:
return({ struct Cyc_CfAbsexp_GroupAE_struct* _temp216=( struct Cyc_CfAbsexp_GroupAE_struct*)
GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct)); _temp216->tag= Cyc_CfAbsexp_GroupAE_tag;
_temp216->f1=( void*) Cyc_CfAbsexp_OrderedG; _temp216->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp195,(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp217=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp217[ 0]=({ struct Cyc_List_List
_temp218; _temp218.hd=( void*) ae2; _temp218.tl= 0; _temp218;}); _temp217;}));(
void*) _temp216;}); _LL130: return({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp219=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp219->tag= Cyc_CfAbsexp_GroupAE_tag; _temp219->f1=( void*) _temp210;
_temp219->f2=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp220=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp220[ 0]=({
struct Cyc_List_List _temp221; _temp221.hd=( void*) ae1; _temp221.tl= _temp204;
_temp221;}); _temp220;});( void*) _temp219;}); _LL132: return({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp222=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp222->tag= Cyc_CfAbsexp_GroupAE_tag; _temp222->f1=( void*) g; _temp222->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp223=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp223[ 0]=({ struct Cyc_List_List
_temp224; _temp224.hd=( void*) ae1; _temp224.tl=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp225=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp225[ 0]=({ struct Cyc_List_List _temp226;
_temp226.hd=( void*) ae2; _temp226.tl= 0; _temp226;}); _temp225;}); _temp224;});
_temp223;});( void*) _temp222;}); _LL112:;} void* Cyc_CfAbsexp_mkGroupAE_l( void*
g, struct Cyc_List_List* ael){ if( ael == 0){ return Cyc_CfAbsexp_SkipAE;} if(
ael->tl == 0){ return( void*) ael->hd;} return({ struct Cyc_CfAbsexp_GroupAE_struct*
_temp227=( struct Cyc_CfAbsexp_GroupAE_struct*) GC_malloc( sizeof( struct Cyc_CfAbsexp_GroupAE_struct));
_temp227->tag= Cyc_CfAbsexp_GroupAE_tag; _temp227->f1=( void*) g; _temp227->f2=
ael;( void*) _temp227;});} int Cyc_CfAbsexp_isUnknownOp( void* ao){ return ao ==
Cyc_CfAbsexp_UnknownOp;} struct _tagged_string Cyc_CfAbsexp_absop2string( void*
ao){ void* _temp228= ao; void* _temp242; struct Cyc_Absyn_Vardecl* _temp244;
void* _temp246; void* _temp248; int _temp250; struct Cyc_Absyn_Exp* _temp252;
void* _temp254; _LL230: if( _temp228 == Cyc_CfAbsexp_UnknownOp){ goto _LL231;}
else{ goto _LL232;} _LL232: if(( unsigned int) _temp228 > 1u?(( struct
_enum_struct*) _temp228)->tag == Cyc_CfAbsexp_AddressOp_tag: 0){ _LL243:
_temp242=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp228)->f1; goto
_LL233;} else{ goto _LL234;} _LL234: if(( unsigned int) _temp228 > 1u?(( struct
_enum_struct*) _temp228)->tag == Cyc_CfAbsexp_LocalOp_tag: 0){ _LL245: _temp244=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp228)->f1;
goto _LL235;} else{ goto _LL236;} _LL236: if(( unsigned int) _temp228 > 1u?((
struct _enum_struct*) _temp228)->tag == Cyc_CfAbsexp_MemberOp_tag: 0){ _LL249:
_temp248=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp228)->f1; goto
_LL247; _LL247: _temp246=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp228)->f2; goto _LL237;} else{ goto _LL238;} _LL238: if(( unsigned int)
_temp228 > 1u?(( struct _enum_struct*) _temp228)->tag == Cyc_CfAbsexp_MallocOp_tag:
0){ _LL253: _temp252=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp228)->f1; goto _LL251; _LL251: _temp250=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp228)->f2; goto _LL239;} else{ goto _LL240;} _LL240: if(( unsigned int)
_temp228 > 1u?(( struct _enum_struct*) _temp228)->tag == Cyc_CfAbsexp_DerefOp_tag:
0){ _LL255: _temp254=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp228)->f1;
goto _LL241;} else{ goto _LL229;} _LL231: return( struct _tagged_string)({ char*
_temp256=( char*)"Unknown"; struct _tagged_string _temp257; _temp257.curr=
_temp256; _temp257.base= _temp256; _temp257.last_plus_one= _temp256 + 8;
_temp257;}); _LL233: return({ struct _tagged_string _temp258= Cyc_CfAbsexp_absop2string(
_temp242); xprintf("& %.*s", _temp258.last_plus_one - _temp258.curr, _temp258.curr);});
_LL235: return*(* _temp244->name).f2; _LL237: { struct _tagged_string fs;{ void*
_temp259= _temp246; struct _tagged_string* _temp265; int _temp267; _LL261: if(((
struct _enum_struct*) _temp259)->tag == Cyc_CfFlowInfo_StructF_tag){ _LL266:
_temp265=( struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*)
_temp259)->f1; goto _LL262;} else{ goto _LL263;} _LL263: if((( struct
_enum_struct*) _temp259)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL268: _temp267=(
int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp259)->f1; goto _LL264;} else{
goto _LL260;} _LL262: fs=* _temp265; goto _LL260; _LL264: fs= xprintf("%d",
_temp267); goto _LL260; _LL260:;} return({ struct _tagged_string _temp269= Cyc_CfAbsexp_absop2string(
_temp248); struct _tagged_string _temp270= fs; xprintf("%.*s.%.*s", _temp269.last_plus_one
- _temp269.curr, _temp269.curr, _temp270.last_plus_one - _temp270.curr, _temp270.curr);});}
_LL239: return xprintf("<mpt%d>", _temp250); _LL241: return({ struct
_tagged_string _temp271= Cyc_CfAbsexp_absop2string( _temp254); xprintf("(*%.*s)",
_temp271.last_plus_one - _temp271.curr, _temp271.curr);}); _LL229:;} struct
_tagged_string Cyc_CfAbsexp_absexp2string( void* ae, int depth){ struct
_tagged_string ans= Cyc_Core_new_string( depth);{ int i= 0; for( 0;(
unsigned int) i <({ struct _tagged_string _temp272= ans;( unsigned int)(
_temp272.last_plus_one - _temp272.curr);}); ++ i){({ struct _tagged_string
_temp273= ans; char* _temp275= _temp273.curr + i; if( _temp275 < _temp273.base?
1: _temp275 >= _temp273.last_plus_one){ _throw( Null_Exception);}* _temp275=' ';});}}{
void* _temp276= ae; void* _temp294; int _temp296; void* _temp298; struct Cyc_Absyn_Exp*
_temp300; void* _temp302; void* _temp304; struct Cyc_List_List* _temp306; void*
_temp308; struct Cyc_Absyn_Stmt* _temp310; _LL278: if( _temp276 == Cyc_CfAbsexp_SkipAE){
goto _LL279;} else{ goto _LL280;} _LL280: if(( unsigned int) _temp276 > 2u?((
struct _enum_struct*) _temp276)->tag == Cyc_CfAbsexp_UseAE_tag: 0){ _LL295:
_temp294=( void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp276)->f1; goto
_LL281;} else{ goto _LL282;} _LL282: if(( unsigned int) _temp276 > 2u?(( struct
_enum_struct*) _temp276)->tag == Cyc_CfAbsexp_MallocAE_tag: 0){ _LL299: _temp298=(
void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp276)->f1; if((( struct
_enum_struct*) _temp298)->tag == Cyc_CfFlowInfo_MallocPt_tag){ _LL301: _temp300=(
struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*) _temp298)->f1;
goto _LL297;} else{ goto _LL284;} _LL297: _temp296=( int)(( struct Cyc_CfAbsexp_MallocAE_struct*)
_temp276)->f2; goto _LL283;} else{ goto _LL284;} _LL284: if(( unsigned int)
_temp276 > 2u?(( struct _enum_struct*) _temp276)->tag == Cyc_CfAbsexp_AssignAE_tag:
0){ _LL305: _temp304=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp276)->f1;
goto _LL303; _LL303: _temp302=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp276)->f2; goto _LL285;} else{ goto _LL286;} _LL286: if(( unsigned int)
_temp276 > 2u?(( struct _enum_struct*) _temp276)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL309: _temp308=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp276)->f1;
goto _LL307; _LL307: _temp306=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp276)->f2; goto _LL287;} else{ goto _LL288;} _LL288: if(( unsigned int)
_temp276 > 2u?(( struct _enum_struct*) _temp276)->tag == Cyc_CfAbsexp_StmtAE_tag:
0){ _LL311: _temp310=( struct Cyc_Absyn_Stmt*)(( struct Cyc_CfAbsexp_StmtAE_struct*)
_temp276)->f1; goto _LL289;} else{ goto _LL290;} _LL290: if( _temp276 == Cyc_CfAbsexp_BottomAE){
goto _LL291;} else{ goto _LL292;} _LL292: goto _LL293; _LL279: ans=({ struct
_tagged_string _temp312= ans; xprintf("%.*sskip", _temp312.last_plus_one -
_temp312.curr, _temp312.curr);}); goto _LL277; _LL281: ans=({ struct
_tagged_string _temp313= ans; struct _tagged_string _temp314= Cyc_CfAbsexp_absop2string(
_temp294); xprintf("%.*suse %.*s", _temp313.last_plus_one - _temp313.curr,
_temp313.curr, _temp314.last_plus_one - _temp314.curr, _temp314.curr);}); goto
_LL277; _LL283: ans=({ struct _tagged_string _temp315= ans; int _temp316=
_temp296; xprintf("%.*smalloc %i", _temp315.last_plus_one - _temp315.curr,
_temp315.curr, _temp316);}); goto _LL277; _LL285: ans=({ struct _tagged_string
_temp317= ans; struct _tagged_string _temp318= Cyc_CfAbsexp_absop2string(
_temp304); struct _tagged_string _temp319= Cyc_CfAbsexp_absop2string( _temp302);
xprintf("%.*sassign %.*s %.*s", _temp317.last_plus_one - _temp317.curr, _temp317.curr,
_temp318.last_plus_one - _temp318.curr, _temp318.curr, _temp319.last_plus_one -
_temp319.curr, _temp319.curr);}); goto _LL277; _LL287:{ void* _temp320= _temp308;
_LL322: if( _temp320 == Cyc_CfAbsexp_OrderedG){ goto _LL323;} else{ goto _LL324;}
_LL324: if( _temp320 == Cyc_CfAbsexp_UnorderedG){ goto _LL325;} else{ goto
_LL326;} _LL326: if( _temp320 == Cyc_CfAbsexp_OneofG){ goto _LL327;} else{ goto
_LL321;} _LL323: ans=({ struct _tagged_string _temp328= ans; xprintf("%.*sordered",
_temp328.last_plus_one - _temp328.curr, _temp328.curr);}); goto _LL321; _LL325:
ans=({ struct _tagged_string _temp329= ans; xprintf("%.*sunordered", _temp329.last_plus_one
- _temp329.curr, _temp329.curr);}); goto _LL321; _LL327: ans=({ struct
_tagged_string _temp330= ans; xprintf("%.*soneof", _temp330.last_plus_one -
_temp330.curr, _temp330.curr);}); goto _LL321; _LL321:;} for( 0; _temp306 != 0;
_temp306= _temp306->tl){ ans=({ struct _tagged_string _temp331= ans; struct
_tagged_string _temp332= Cyc_CfAbsexp_absexp2string(( void*) _temp306->hd, depth
+ 1); xprintf("%.*s\n%.*s", _temp331.last_plus_one - _temp331.curr, _temp331.curr,
_temp332.last_plus_one - _temp332.curr, _temp332.curr);});} goto _LL277; _LL289:
ans=({ struct _tagged_string _temp333= ans; xprintf("%.*s<<stmt>>", _temp333.last_plus_one
- _temp333.curr, _temp333.curr);}); goto _LL277; _LL291: ans=({ struct
_tagged_string _temp334= ans; xprintf("%.*sbottom", _temp334.last_plus_one -
_temp334.curr, _temp334.curr);}); goto _LL277; _LL293:( void) _throw(({ struct
Cyc_CfAbsexp_BadAbsexp_struct* _temp335=( struct Cyc_CfAbsexp_BadAbsexp_struct*)
GC_malloc_atomic( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct));* _temp335=(
struct Cyc_CfAbsexp_BadAbsexp_struct){.tag= Cyc_CfAbsexp_BadAbsexp_tag};( struct
_xenum_struct*) _temp335;})); _LL277:;} return ans;} static void Cyc_CfAbsexp_check_absop(
void* ao){ void* _temp336= ao; void* _temp350; void* _temp352; void* _temp354;
void* _temp356; struct Cyc_Absyn_Vardecl* _temp358; int _temp360; struct Cyc_Absyn_Exp*
_temp362; _LL338: if( _temp336 == Cyc_CfAbsexp_UnknownOp){ goto _LL339;} else{
goto _LL340;} _LL340: if(( unsigned int) _temp336 > 1u?(( struct _enum_struct*)
_temp336)->tag == Cyc_CfAbsexp_AddressOp_tag: 0){ _LL351: _temp350=( void*)((
struct Cyc_CfAbsexp_AddressOp_struct*) _temp336)->f1; goto _LL341;} else{ goto
_LL342;} _LL342: if(( unsigned int) _temp336 > 1u?(( struct _enum_struct*)
_temp336)->tag == Cyc_CfAbsexp_DerefOp_tag: 0){ _LL353: _temp352=( void*)((
struct Cyc_CfAbsexp_DerefOp_struct*) _temp336)->f1; goto _LL343;} else{ goto
_LL344;} _LL344: if(( unsigned int) _temp336 > 1u?(( struct _enum_struct*)
_temp336)->tag == Cyc_CfAbsexp_MemberOp_tag: 0){ _LL357: _temp356=( void*)((
struct Cyc_CfAbsexp_MemberOp_struct*) _temp336)->f1; goto _LL355; _LL355:
_temp354=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp336)->f2; goto
_LL345;} else{ goto _LL346;} _LL346: if(( unsigned int) _temp336 > 1u?(( struct
_enum_struct*) _temp336)->tag == Cyc_CfAbsexp_LocalOp_tag: 0){ _LL359: _temp358=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*) _temp336)->f1;
goto _LL347;} else{ goto _LL348;} _LL348: if(( unsigned int) _temp336 > 1u?((
struct _enum_struct*) _temp336)->tag == Cyc_CfAbsexp_MallocOp_tag: 0){ _LL363:
_temp362=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp336)->f1; goto _LL361; _LL361: _temp360=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp336)->f2; goto _LL349;} else{ goto _LL337;} _LL339: return; _LL341: Cyc_CfAbsexp_check_absop(
_temp350); Cyc_CfAbsexp_ok_address_arg( _temp350); return; _LL343: Cyc_CfAbsexp_check_absop(
_temp352); Cyc_CfAbsexp_ok_deref_arg( _temp352); return; _LL345: Cyc_CfAbsexp_check_absop(
_temp356); Cyc_CfAbsexp_ok_member_arg( _temp356); return; _LL347: return; _LL349:
return; _LL337:;} void Cyc_CfAbsexp_check_absexp( void* ae){ void* _temp364= ae;
int _temp382; void* _temp384; struct Cyc_Absyn_Vardecl* _temp386; int _temp388;
void* _temp390; struct Cyc_Absyn_Stmt* _temp392; struct Cyc_List_List* _temp394;
void* _temp396; void* _temp398; void* _temp400; void* _temp402; _LL366: if(
_temp364 == Cyc_CfAbsexp_BottomAE){ goto _LL367;} else{ goto _LL368;} _LL368:
if( _temp364 == Cyc_CfAbsexp_SkipAE){ goto _LL369;} else{ goto _LL370;} _LL370:
if(( unsigned int) _temp364 > 2u?(( struct _enum_struct*) _temp364)->tag == Cyc_CfAbsexp_MallocAE_tag:
0){ _LL385: _temp384=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp364)->f1;
if((( struct _enum_struct*) _temp384)->tag == Cyc_CfFlowInfo_VarRoot_tag){
_LL387: _temp386=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfFlowInfo_VarRoot_struct*)
_temp384)->f1; goto _LL383;} else{ goto _LL372;} _LL383: _temp382=( int)((
struct Cyc_CfAbsexp_MallocAE_struct*) _temp364)->f2; goto _LL371;} else{ goto
_LL372;} _LL372: if(( unsigned int) _temp364 > 2u?(( struct _enum_struct*)
_temp364)->tag == Cyc_CfAbsexp_MallocAE_tag: 0){ _LL391: _temp390=( void*)((
struct Cyc_CfAbsexp_MallocAE_struct*) _temp364)->f1; goto _LL389; _LL389:
_temp388=( int)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp364)->f2; goto
_LL373;} else{ goto _LL374;} _LL374: if(( unsigned int) _temp364 > 2u?(( struct
_enum_struct*) _temp364)->tag == Cyc_CfAbsexp_StmtAE_tag: 0){ _LL393: _temp392=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_CfAbsexp_StmtAE_struct*) _temp364)->f1;
goto _LL375;} else{ goto _LL376;} _LL376: if(( unsigned int) _temp364 > 2u?((
struct _enum_struct*) _temp364)->tag == Cyc_CfAbsexp_GroupAE_tag: 0){ _LL397:
_temp396=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp364)->f1; goto
_LL395; _LL395: _temp394=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp364)->f2; goto _LL377;} else{ goto _LL378;} _LL378: if(( unsigned int)
_temp364 > 2u?(( struct _enum_struct*) _temp364)->tag == Cyc_CfAbsexp_UseAE_tag:
0){ _LL399: _temp398=( void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp364)->f1;
goto _LL379;} else{ goto _LL380;} _LL380: if(( unsigned int) _temp364 > 2u?((
struct _enum_struct*) _temp364)->tag == Cyc_CfAbsexp_AssignAE_tag: 0){ _LL403:
_temp402=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp364)->f1; goto
_LL401; _LL401: _temp400=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp364)->f2; goto _LL381;} else{ goto _LL365;} _LL367: return; _LL369: return;
_LL371:( void) _throw(({ struct Cyc_CfAbsexp_BadAbsexp_struct* _temp404=( struct
Cyc_CfAbsexp_BadAbsexp_struct*) GC_malloc_atomic( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct));*
_temp404=( struct Cyc_CfAbsexp_BadAbsexp_struct){.tag= Cyc_CfAbsexp_BadAbsexp_tag};(
struct _xenum_struct*) _temp404;})); _LL373: return; _LL375: return; _LL377:
for( 0; _temp394 != 0; _temp394= _temp394->tl){ Cyc_CfAbsexp_check_absexp(( void*)
_temp394->hd);} return; _LL379: Cyc_CfAbsexp_check_absop( _temp398); return;
_LL381: Cyc_CfAbsexp_check_absop( _temp402); Cyc_CfAbsexp_check_absop( _temp400);
Cyc_CfAbsexp_ok_lvalue( _temp402); return; _LL365:;} void* Cyc_CfAbsexp_eval_absop_r(
struct Cyc_Dict_Dict* pinfo_dict, void* ao); void* Cyc_CfAbsexp_eval_absop_l(
struct Cyc_Dict_Dict* pinfo_dict, void* ao){ void* _temp405= ao; struct Cyc_Absyn_Vardecl*
_temp419; int _temp421; struct Cyc_Absyn_Exp* _temp423; void* _temp425; void*
_temp427; void* _temp429; void* _temp431; _LL407: if(( unsigned int) _temp405 >
1u?(( struct _enum_struct*) _temp405)->tag == Cyc_CfAbsexp_LocalOp_tag: 0){
_LL420: _temp419=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp405)->f1; goto _LL408;} else{ goto _LL409;} _LL409: if(( unsigned int)
_temp405 > 1u?(( struct _enum_struct*) _temp405)->tag == Cyc_CfAbsexp_MallocOp_tag:
0){ _LL424: _temp423=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp405)->f1; goto _LL422; _LL422: _temp421=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp405)->f2; goto _LL410;} else{ goto _LL411;} _LL411: if(( unsigned int)
_temp405 > 1u?(( struct _enum_struct*) _temp405)->tag == Cyc_CfAbsexp_MemberOp_tag:
0){ _LL428: _temp427=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp405)->f1;
goto _LL426; _LL426: _temp425=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp405)->f2; goto _LL412;} else{ goto _LL413;} _LL413: if(( unsigned int)
_temp405 > 1u?(( struct _enum_struct*) _temp405)->tag == Cyc_CfAbsexp_DerefOp_tag:
0){ _LL430: _temp429=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp405)->f1;
goto _LL414;} else{ goto _LL415;} _LL415: if( _temp405 == Cyc_CfAbsexp_UnknownOp){
goto _LL416;} else{ goto _LL417;} _LL417: if(( unsigned int) _temp405 > 1u?((
struct _enum_struct*) _temp405)->tag == Cyc_CfAbsexp_AddressOp_tag: 0){ _LL432:
_temp431=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp405)->f1; goto
_LL418;} else{ goto _LL406;} _LL408: return({ struct Cyc_CfFlowInfo_MustPointTo_struct*
_temp433=( struct Cyc_CfFlowInfo_MustPointTo_struct*) GC_malloc( sizeof( struct
Cyc_CfFlowInfo_MustPointTo_struct)); _temp433->tag= Cyc_CfFlowInfo_MustPointTo_tag;
_temp433->f1=({ struct Cyc_CfFlowInfo_Place* _temp434=( struct Cyc_CfFlowInfo_Place*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place) * 1); _temp434[ 0]=({ struct Cyc_CfFlowInfo_Place
_temp435; _temp435.root=( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp436=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp436->tag= Cyc_CfFlowInfo_VarRoot_tag; _temp436->f1= _temp419;( void*)
_temp436;}); _temp435.fields= 0; _temp435;}); _temp434;});( void*) _temp433;});
_LL410: return({ struct Cyc_CfFlowInfo_MustPointTo_struct* _temp437=( struct Cyc_CfFlowInfo_MustPointTo_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MustPointTo_struct)); _temp437->tag=
Cyc_CfFlowInfo_MustPointTo_tag; _temp437->f1=({ struct Cyc_CfFlowInfo_Place*
_temp438=( struct Cyc_CfFlowInfo_Place*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place)
* 1); _temp438[ 0]=({ struct Cyc_CfFlowInfo_Place _temp439; _temp439.root=( void*)({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp440=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp440->tag= Cyc_CfFlowInfo_MallocPt_tag;
_temp440->f1= _temp423;( void*) _temp440;}); _temp439.fields= 0; _temp439;});
_temp438;});( void*) _temp437;}); _LL412: { void* inner_state= Cyc_CfAbsexp_eval_absop_l(
pinfo_dict, _temp427); void* _temp441= inner_state; void* _temp447; void*
_temp449; struct Cyc_CfFlowInfo_Place* _temp451; struct Cyc_CfFlowInfo_Place
_temp453; struct Cyc_List_List* _temp454; void* _temp456; _LL443: if((( struct
_enum_struct*) _temp441)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL450: _temp449=(
void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp441)->f1; goto _LL448;
_LL448: _temp447=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp441)->f2;
goto _LL444;} else{ goto _LL445;} _LL445: if((( struct _enum_struct*) _temp441)->tag
== Cyc_CfFlowInfo_MustPointTo_tag){ _LL452: _temp451=( struct Cyc_CfFlowInfo_Place*)((
struct Cyc_CfFlowInfo_MustPointTo_struct*) _temp441)->f1; _temp453=* _temp451;
_LL457: _temp456=( void*) _temp453.root; goto _LL455; _LL455: _temp454=( struct
Cyc_List_List*) _temp453.fields; goto _LL446;} else{ goto _LL442;} _LL444:
return inner_state; _LL446: return({ struct Cyc_CfFlowInfo_MustPointTo_struct*
_temp458=( struct Cyc_CfFlowInfo_MustPointTo_struct*) GC_malloc( sizeof( struct
Cyc_CfFlowInfo_MustPointTo_struct)); _temp458->tag= Cyc_CfFlowInfo_MustPointTo_tag;
_temp458->f1=({ struct Cyc_CfFlowInfo_Place* _temp459=( struct Cyc_CfFlowInfo_Place*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_Place) * 1); _temp459[ 0]=({ struct Cyc_CfFlowInfo_Place
_temp460; _temp460.root=( void*) _temp456; _temp460.fields=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp454,(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp461=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp461[ 0]=({ struct Cyc_List_List
_temp462; _temp462.hd=( void*) _temp425; _temp462.tl= 0; _temp462;}); _temp461;}));
_temp460;}); _temp459;});( void*) _temp458;}); _LL442:;} _LL414: { void*
inner_pinfo= Cyc_CfAbsexp_eval_absop_r( pinfo_dict, _temp429); void* _temp463=
inner_pinfo; void* _temp471; struct Cyc_Dict_Dict* _temp473; struct Cyc_Dict_Dict*
_temp475; _LL465: if((( struct _enum_struct*) _temp463)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL472: _temp471=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp463)->f1;
goto _LL466;} else{ goto _LL467;} _LL467: if((( struct _enum_struct*) _temp463)->tag
== Cyc_CfFlowInfo_TuplePI_tag){ _LL474: _temp473=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp463)->f1; goto _LL468;} else{ goto
_LL469;} _LL469: if((( struct _enum_struct*) _temp463)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL476: _temp475=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp463)->f1; goto _LL470;} else{ goto _LL464;} _LL466: return _temp471; _LL468:
goto _LL470; _LL470:( void) _throw(({ struct Cyc_CfAbsexp_BadAbsexp_struct*
_temp477=( struct Cyc_CfAbsexp_BadAbsexp_struct*) GC_malloc_atomic( sizeof(
struct Cyc_CfAbsexp_BadAbsexp_struct));* _temp477=( struct Cyc_CfAbsexp_BadAbsexp_struct){.tag=
Cyc_CfAbsexp_BadAbsexp_tag};( struct _xenum_struct*) _temp477;})); _LL464:;}
_LL416: goto _LL418; _LL418:( void) _throw(({ struct Cyc_CfAbsexp_BadAbsexp_struct*
_temp478=( struct Cyc_CfAbsexp_BadAbsexp_struct*) GC_malloc_atomic( sizeof(
struct Cyc_CfAbsexp_BadAbsexp_struct));* _temp478=( struct Cyc_CfAbsexp_BadAbsexp_struct){.tag=
Cyc_CfAbsexp_BadAbsexp_tag};( struct _xenum_struct*) _temp478;})); _LL406:;}
void* Cyc_CfAbsexp_eval_absop_r( struct Cyc_Dict_Dict* pinfo_dict, void* ao){
void* _temp479= ao; struct Cyc_Absyn_Vardecl* _temp493; int _temp495; struct Cyc_Absyn_Exp*
_temp497; void* _temp499; void* _temp501; void* _temp503; void* _temp505; _LL481:
if(( unsigned int) _temp479 > 1u?(( struct _enum_struct*) _temp479)->tag == Cyc_CfAbsexp_LocalOp_tag:
0){ _LL494: _temp493=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_CfAbsexp_LocalOp_struct*)
_temp479)->f1; goto _LL482;} else{ goto _LL483;} _LL483: if(( unsigned int)
_temp479 > 1u?(( struct _enum_struct*) _temp479)->tag == Cyc_CfAbsexp_MallocOp_tag:
0){ _LL498: _temp497=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp479)->f1; goto _LL496; _LL496: _temp495=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp479)->f2; goto _LL484;} else{ goto _LL485;} _LL485: if(( unsigned int)
_temp479 > 1u?(( struct _enum_struct*) _temp479)->tag == Cyc_CfAbsexp_MemberOp_tag:
0){ _LL502: _temp501=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp479)->f1;
goto _LL500; _LL500: _temp499=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp479)->f2; goto _LL486;} else{ goto _LL487;} _LL487: if(( unsigned int)
_temp479 > 1u?(( struct _enum_struct*) _temp479)->tag == Cyc_CfAbsexp_DerefOp_tag:
0){ _LL504: _temp503=( void*)(( struct Cyc_CfAbsexp_DerefOp_struct*) _temp479)->f1;
goto _LL488;} else{ goto _LL489;} _LL489: if( _temp479 == Cyc_CfAbsexp_UnknownOp){
goto _LL490;} else{ goto _LL491;} _LL491: if(( unsigned int) _temp479 > 1u?((
struct _enum_struct*) _temp479)->tag == Cyc_CfAbsexp_AddressOp_tag: 0){ _LL506:
_temp505=( void*)(( struct Cyc_CfAbsexp_AddressOp_struct*) _temp479)->f1; goto
_LL492;} else{ goto _LL480;} _LL482: return(( void*(*)( struct Cyc_Dict_Dict* d,
void* key)) Cyc_Dict_lookup)( pinfo_dict,({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp507=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp507->tag= Cyc_CfFlowInfo_VarRoot_tag; _temp507->f1= _temp493;( void*)
_temp507;})); _LL484: return(( void*(*)( struct Cyc_Dict_Dict* d, void* key))
Cyc_Dict_lookup)( pinfo_dict,({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp508=(
struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp508->tag= Cyc_CfFlowInfo_MallocPt_tag; _temp508->f1= _temp497;( void*)
_temp508;})); _LL486: { void* inner_pinfo= Cyc_CfAbsexp_eval_absop_r( pinfo_dict,
_temp501); struct _tuple3 _temp510=({ struct _tuple3 _temp509; _temp509.f1=
inner_pinfo; _temp509.f2= _temp499; _temp509;}); void* _temp520; void* _temp522;
void* _temp524; void* _temp526; void* _temp528; void* _temp530; int _temp532;
void* _temp534; struct Cyc_Dict_Dict* _temp536; void* _temp538; struct
_tagged_string* _temp540; void* _temp542; struct Cyc_Dict_Dict* _temp544; _LL512:
_LL523: _temp522= _temp510.f1; if((( struct _enum_struct*) _temp522)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL525: _temp524=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp522)->f1;
if((( struct _enum_struct*) _temp524)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL529: _temp528=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp524)->f1;
goto _LL527; _LL527: _temp526=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp524)->f2; goto _LL521;} else{ goto _LL514;}} else{ goto _LL514;} _LL521:
_temp520= _temp510.f2; goto _LL513; _LL514: _LL535: _temp534= _temp510.f1; if(((
struct _enum_struct*) _temp534)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL537:
_temp536=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_TuplePI_struct*)
_temp534)->f1; goto _LL531;} else{ goto _LL516;} _LL531: _temp530= _temp510.f2;
if((( struct _enum_struct*) _temp530)->tag == Cyc_CfFlowInfo_TupleF_tag){ _LL533:
_temp532=( int)(( struct Cyc_CfFlowInfo_TupleF_struct*) _temp530)->f1; goto
_LL515;} else{ goto _LL516;} _LL516: _LL543: _temp542= _temp510.f1; if((( struct
_enum_struct*) _temp542)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL545: _temp544=(
struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*) _temp542)->f1;
goto _LL539;} else{ goto _LL518;} _LL539: _temp538= _temp510.f2; if((( struct
_enum_struct*) _temp538)->tag == Cyc_CfFlowInfo_StructF_tag){ _LL541: _temp540=(
struct _tagged_string*)(( struct Cyc_CfFlowInfo_StructF_struct*) _temp538)->f1;
goto _LL517;} else{ goto _LL518;} _LL518: goto _LL519; _LL513: return
inner_pinfo; _LL515: return(( void*(*)( struct Cyc_Dict_Dict* d, int key)) Cyc_Dict_lookup)(
_temp536, _temp532); _LL517: return(( void*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( _temp544, _temp540); _LL519:( void)
_throw(({ struct Cyc_CfAbsexp_BadAbsexp_struct* _temp546=( struct Cyc_CfAbsexp_BadAbsexp_struct*)
GC_malloc_atomic( sizeof( struct Cyc_CfAbsexp_BadAbsexp_struct));* _temp546=(
struct Cyc_CfAbsexp_BadAbsexp_struct){.tag= Cyc_CfAbsexp_BadAbsexp_tag};( struct
_xenum_struct*) _temp546;})); _LL511:;} _LL488: { void* inner_pinfo= Cyc_CfAbsexp_eval_absop_r(
pinfo_dict, _temp503); void* _temp547= inner_pinfo; void* _temp555; void*
_temp557; void* _temp559; void* _temp561; struct Cyc_CfFlowInfo_Place* _temp563;
_LL549: if((( struct _enum_struct*) _temp547)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL556: _temp555=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp547)->f1;
if((( struct _enum_struct*) _temp555)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL560: _temp559=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp555)->f1;
goto _LL558; _LL558: _temp557=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp555)->f2; goto _LL550;} else{ goto _LL551;}} else{ goto _LL551;} _LL551:
if((( struct _enum_struct*) _temp547)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL562:
_temp561=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp547)->f1; if(((
struct _enum_struct*) _temp561)->tag == Cyc_CfFlowInfo_MustPointTo_tag){ _LL564:
_temp563=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp561)->f1; goto _LL552;} else{ goto _LL553;}} else{ goto _LL553;} _LL553:
goto _LL554; _LL550: return inner_pinfo; _LL552: return Cyc_CfFlowInfo_lookup_place(
pinfo_dict, _temp563); _LL554:( void) _throw(({ struct Cyc_CfAbsexp_BadAbsexp_struct*
_temp565=( struct Cyc_CfAbsexp_BadAbsexp_struct*) GC_malloc_atomic( sizeof(
struct Cyc_CfAbsexp_BadAbsexp_struct));* _temp565=( struct Cyc_CfAbsexp_BadAbsexp_struct){.tag=
Cyc_CfAbsexp_BadAbsexp_tag};( struct _xenum_struct*) _temp565;})); _LL548:;}
_LL490: return Cyc_CfFlowInfo_mkLeafPI( Cyc_CfFlowInfo_Esc, Cyc_CfFlowInfo_AllIL);
_LL492: return({ struct Cyc_CfFlowInfo_LeafPI_struct* _temp566=( struct Cyc_CfFlowInfo_LeafPI_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_LeafPI_struct)); _temp566->tag= Cyc_CfFlowInfo_LeafPI_tag;
_temp566->f1=( void*) Cyc_CfAbsexp_eval_absop_l( pinfo_dict, _temp505);( void*)
_temp566;}); _LL480:;} void Cyc_CfAbsexp_assign_escape( struct Cyc_List_List**
escaping_states, void* old_pinfo, void* new_pinfo){ struct _tuple3 _temp568=({
struct _tuple3 _temp567; _temp567.f1= old_pinfo; _temp567.f2= new_pinfo;
_temp567;}); void* _temp580; void* _temp582; void* _temp584; void* _temp586;
void* _temp588; void* _temp590; void* _temp592; void* _temp594; void* _temp596;
void* _temp598; void* _temp600; struct Cyc_Dict_Dict* _temp602; void* _temp604;
struct Cyc_Dict_Dict* _temp606; void* _temp608; struct Cyc_Dict_Dict* _temp610;
void* _temp612; struct Cyc_Dict_Dict* _temp614; _LL570: _LL585: _temp584=
_temp568.f1; if((( struct _enum_struct*) _temp584)->tag == Cyc_CfFlowInfo_LeafPI_tag){
_LL587: _temp586=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp584)->f1;
if((( struct _enum_struct*) _temp586)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL591: _temp590=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp586)->f1;
if( _temp590 == Cyc_CfFlowInfo_Esc){ goto _LL589;} else{ goto _LL572;} _LL589:
_temp588=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp586)->f2; goto
_LL581;} else{ goto _LL572;}} else{ goto _LL572;} _LL581: _temp580= _temp568.f2;
if((( struct _enum_struct*) _temp580)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL583:
_temp582=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp580)->f1; goto
_LL571;} else{ goto _LL572;} _LL572: _LL597: _temp596= _temp568.f1; if((( struct
_enum_struct*) _temp596)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL599: _temp598=(
void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp596)->f1; goto _LL593;}
else{ goto _LL574;} _LL593: _temp592= _temp568.f2; if((( struct _enum_struct*)
_temp592)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL595: _temp594=( void*)(( struct
Cyc_CfFlowInfo_LeafPI_struct*) _temp592)->f1; goto _LL573;} else{ goto _LL574;}
_LL574: _LL605: _temp604= _temp568.f1; if((( struct _enum_struct*) _temp604)->tag
== Cyc_CfFlowInfo_TuplePI_tag){ _LL607: _temp606=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp604)->f1; goto _LL601;} else{ goto
_LL576;} _LL601: _temp600= _temp568.f2; if((( struct _enum_struct*) _temp600)->tag
== Cyc_CfFlowInfo_TuplePI_tag){ _LL603: _temp602=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp600)->f1; goto _LL575;} else{ goto
_LL576;} _LL576: _LL613: _temp612= _temp568.f1; if((( struct _enum_struct*)
_temp612)->tag == Cyc_CfFlowInfo_StructPI_tag){ _LL615: _temp614=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_StructPI_struct*) _temp612)->f1; goto _LL609;} else{ goto
_LL578;} _LL609: _temp608= _temp568.f2; if((( struct _enum_struct*) _temp608)->tag
== Cyc_CfFlowInfo_StructPI_tag){ _LL611: _temp610=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_StructPI_struct*) _temp608)->f1; goto _LL577;} else{ goto
_LL578;} _LL578: goto _LL579; _LL571:* escaping_states=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp616=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp616[ 0]=({ struct Cyc_List_List _temp617;
_temp617.hd=( void*) new_pinfo; _temp617.tl=* escaping_states; _temp617;});
_temp616;}); return; _LL573: return; _LL575:(( void(*)( void(* f)( struct Cyc_List_List**,
void*, void*), struct Cyc_List_List** env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_iter2_c)( Cyc_CfAbsexp_assign_escape, escaping_states, _temp606,
_temp602); return; _LL577:(( void(*)( void(* f)( struct Cyc_List_List**, void*,
void*), struct Cyc_List_List** env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_iter2_c)( Cyc_CfAbsexp_assign_escape, escaping_states, _temp614,
_temp610); return; _LL579:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp618=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp618=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp619=( char*)"bad pinfos in assign_escape";
struct _tagged_string _temp620; _temp620.curr= _temp619; _temp620.base= _temp619;
_temp620.last_plus_one= _temp619 + 28; _temp620;})};( struct _xenum_struct*)
_temp618;})); _LL569:;} int Cyc_CfAbsexp_is_ok_malloc_assign( void* lval, struct
Cyc_List_List* ok_mallocs){ void* _temp621= lval; int _temp629; struct Cyc_Absyn_Exp*
_temp631; void* _temp633; void* _temp635; _LL623: if(( unsigned int) _temp621 >
1u?(( struct _enum_struct*) _temp621)->tag == Cyc_CfAbsexp_MallocOp_tag: 0){
_LL632: _temp631=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp621)->f1; goto _LL630; _LL630: _temp629=( int)(( struct Cyc_CfAbsexp_MallocOp_struct*)
_temp621)->f2; goto _LL624;} else{ goto _LL625;} _LL625: if(( unsigned int)
_temp621 > 1u?(( struct _enum_struct*) _temp621)->tag == Cyc_CfAbsexp_MemberOp_tag:
0){ _LL636: _temp635=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*) _temp621)->f1;
goto _LL634; _LL634: _temp633=( void*)(( struct Cyc_CfAbsexp_MemberOp_struct*)
_temp621)->f2; goto _LL626;} else{ goto _LL627;} _LL627: goto _LL628; _LL624:
return(( int(*)( struct Cyc_List_List* l, struct Cyc_Absyn_Exp* x)) Cyc_List_memq)(
ok_mallocs, _temp631); _LL626: return Cyc_CfAbsexp_is_ok_malloc_assign( _temp635,
ok_mallocs); _LL628: return 0; _LL622:;} struct _tuple5 Cyc_CfAbsexp_unordered_mallocs(
void* ae, struct Cyc_List_List* ok_mallocs){ struct Cyc_List_List* assigns= 0;
struct Cyc_List_List* mallocs= 0;{ void* _temp637= ae; void* _temp659; void*
_temp661; void* _temp663; int _temp665; void* _temp667; struct Cyc_Absyn_Exp*
_temp669; int _temp671; void* _temp673; struct Cyc_Absyn_Stmt* _temp675; struct
Cyc_List_List* _temp677; void* _temp679; struct Cyc_List_List* _temp681; void*
_temp683; struct Cyc_List_List* _temp685; void* _temp687; _LL639: if( _temp637
== Cyc_CfAbsexp_BottomAE){ goto _LL640;} else{ goto _LL641;} _LL641: if(
_temp637 == Cyc_CfAbsexp_SkipAE){ goto _LL642;} else{ goto _LL643;} _LL643: if((
unsigned int) _temp637 > 2u?(( struct _enum_struct*) _temp637)->tag == Cyc_CfAbsexp_UseAE_tag:
0){ _LL660: _temp659=( void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp637)->f1;
goto _LL644;} else{ goto _LL645;} _LL645: if(( unsigned int) _temp637 > 2u?((
struct _enum_struct*) _temp637)->tag == Cyc_CfAbsexp_AssignAE_tag: 0){ _LL664:
_temp663=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp637)->f1; goto
_LL662; _LL662: _temp661=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp637)->f2; goto _LL646;} else{ goto _LL647;} _LL647: if(( unsigned int)
_temp637 > 2u?(( struct _enum_struct*) _temp637)->tag == Cyc_CfAbsexp_MallocAE_tag:
0){ _LL668: _temp667=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp637)->f1;
if((( struct _enum_struct*) _temp667)->tag == Cyc_CfFlowInfo_MallocPt_tag){
_LL670: _temp669=( struct Cyc_Absyn_Exp*)(( struct Cyc_CfFlowInfo_MallocPt_struct*)
_temp667)->f1; goto _LL666;} else{ goto _LL649;} _LL666: _temp665=( int)((
struct Cyc_CfAbsexp_MallocAE_struct*) _temp637)->f2; goto _LL648;} else{ goto
_LL649;} _LL649: if(( unsigned int) _temp637 > 2u?(( struct _enum_struct*)
_temp637)->tag == Cyc_CfAbsexp_MallocAE_tag: 0){ _LL674: _temp673=( void*)((
struct Cyc_CfAbsexp_MallocAE_struct*) _temp637)->f1; goto _LL672; _LL672:
_temp671=( int)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp637)->f2; goto
_LL650;} else{ goto _LL651;} _LL651: if(( unsigned int) _temp637 > 2u?(( struct
_enum_struct*) _temp637)->tag == Cyc_CfAbsexp_StmtAE_tag: 0){ _LL676: _temp675=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_CfAbsexp_StmtAE_struct*) _temp637)->f1;
goto _LL652;} else{ goto _LL653;} _LL653: if(( unsigned int) _temp637 > 2u?((
struct _enum_struct*) _temp637)->tag == Cyc_CfAbsexp_GroupAE_tag: 0){ _LL680:
_temp679=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp637)->f1; if(
_temp679 == Cyc_CfAbsexp_OrderedG){ goto _LL678;} else{ goto _LL655;} _LL678:
_temp677=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp637)->f2; goto _LL654;} else{ goto _LL655;} _LL655: if(( unsigned int)
_temp637 > 2u?(( struct _enum_struct*) _temp637)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL684: _temp683=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp637)->f1;
if( _temp683 == Cyc_CfAbsexp_OneofG){ goto _LL682;} else{ goto _LL657;} _LL682:
_temp681=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp637)->f2; goto _LL656;} else{ goto _LL657;} _LL657: if(( unsigned int)
_temp637 > 2u?(( struct _enum_struct*) _temp637)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL688: _temp687=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp637)->f1;
if( _temp687 == Cyc_CfAbsexp_UnorderedG){ goto _LL686;} else{ goto _LL638;}
_LL686: _temp685=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp637)->f2; goto _LL658;} else{ goto _LL638;} _LL640: goto _LL638; _LL642:
goto _LL638; _LL644: goto _LL638; _LL646: if( Cyc_CfAbsexp_is_ok_malloc_assign(
_temp663, ok_mallocs)){ assigns=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp689=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp689[ 0]=({ struct Cyc_List_List _temp690; _temp690.hd=( void*) ae; _temp690.tl=
assigns; _temp690;}); _temp689;});} goto _LL638; _LL648: mallocs=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp691=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp691[ 0]=({ struct Cyc_List_List _temp692;
_temp692.hd=( void*) _temp669; _temp692.tl= mallocs; _temp692;}); _temp691;});
goto _LL638; _LL650:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp693=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp693=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp694=( char*)"local variable in MallocAE";
struct _tagged_string _temp695; _temp695.curr= _temp694; _temp695.base= _temp694;
_temp695.last_plus_one= _temp694 + 27; _temp695;})};( struct _xenum_struct*)
_temp693;})); _LL652: goto _LL638; _LL654: for( 0; _temp677 != 0; _temp677=
_temp677->tl){ struct Cyc_List_List* _temp698; struct Cyc_List_List* _temp700;
struct _tuple5 _temp696= Cyc_CfAbsexp_unordered_mallocs(( void*) _temp677->hd,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
mallocs, ok_mallocs)); _LL701: _temp700= _temp696.f1; goto _LL699; _LL699:
_temp698= _temp696.f2; goto _LL697; _LL697: assigns=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp700,
assigns); mallocs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( _temp698, mallocs);} goto _LL638; _LL656:
goto _LL638; _LL658: for( 0; _temp685 != 0; _temp685= _temp685->tl){ struct Cyc_List_List*
_temp704; struct Cyc_List_List* _temp706; struct _tuple5 _temp702= Cyc_CfAbsexp_unordered_mallocs((
void*) _temp685->hd, ok_mallocs); _LL707: _temp706= _temp702.f1; goto _LL705;
_LL705: _temp704= _temp702.f2; goto _LL703; _LL703: assigns=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp706,
assigns); mallocs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( _temp704, mallocs);} goto _LL638; _LL638:;}
return({ struct _tuple5 _temp708; _temp708.f1= assigns; _temp708.f2= mallocs;
_temp708;});} void* Cyc_CfAbsexp_eval_absexp( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Position_Segment* loc, void* ae, void* in_flow){ struct Cyc_Dict_Dict*
pinfo_dict;{ void* _temp709= in_flow; struct Cyc_Dict_Dict* _temp715; _LL711:
if( _temp709 == Cyc_CfFlowInfo_BottomFL){ goto _LL712;} else{ goto _LL713;}
_LL713: if(( unsigned int) _temp709 > 1u?(( struct _enum_struct*) _temp709)->tag
== Cyc_CfFlowInfo_InitsFL_tag: 0){ _LL716: _temp715=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp709)->f1; goto _LL714;} else{ goto
_LL710;} _LL712: return Cyc_CfFlowInfo_BottomFL; _LL714: pinfo_dict= _temp715;
goto _LL710; _LL710:;}{ void* _temp717= ae; void* _temp737; void* _temp739; void*
_temp741; int _temp743; void* _temp745; struct Cyc_Absyn_Stmt* _temp747; struct
Cyc_List_List* _temp749; void* _temp751; struct Cyc_List_List* _temp753; void*
_temp755; struct Cyc_List_List* _temp757; void* _temp759; _LL719: if((
unsigned int) _temp717 > 2u?(( struct _enum_struct*) _temp717)->tag == Cyc_CfAbsexp_AssignAE_tag:
0){ _LL740: _temp739=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*) _temp717)->f1;
goto _LL738; _LL738: _temp737=( void*)(( struct Cyc_CfAbsexp_AssignAE_struct*)
_temp717)->f2; goto _LL720;} else{ goto _LL721;} _LL721: if(( unsigned int)
_temp717 > 2u?(( struct _enum_struct*) _temp717)->tag == Cyc_CfAbsexp_UseAE_tag:
0){ _LL742: _temp741=( void*)(( struct Cyc_CfAbsexp_UseAE_struct*) _temp717)->f1;
goto _LL722;} else{ goto _LL723;} _LL723: if(( unsigned int) _temp717 > 2u?((
struct _enum_struct*) _temp717)->tag == Cyc_CfAbsexp_MallocAE_tag: 0){ _LL746:
_temp745=( void*)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp717)->f1; goto
_LL744; _LL744: _temp743=( int)(( struct Cyc_CfAbsexp_MallocAE_struct*) _temp717)->f2;
goto _LL724;} else{ goto _LL725;} _LL725: if( _temp717 == Cyc_CfAbsexp_BottomAE){
goto _LL726;} else{ goto _LL727;} _LL727: if( _temp717 == Cyc_CfAbsexp_SkipAE){
goto _LL728;} else{ goto _LL729;} _LL729: if(( unsigned int) _temp717 > 2u?((
struct _enum_struct*) _temp717)->tag == Cyc_CfAbsexp_StmtAE_tag: 0){ _LL748:
_temp747=( struct Cyc_Absyn_Stmt*)(( struct Cyc_CfAbsexp_StmtAE_struct*)
_temp717)->f1; goto _LL730;} else{ goto _LL731;} _LL731: if(( unsigned int)
_temp717 > 2u?(( struct _enum_struct*) _temp717)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL752: _temp751=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp717)->f1;
if( _temp751 == Cyc_CfAbsexp_OrderedG){ goto _LL750;} else{ goto _LL733;} _LL750:
_temp749=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp717)->f2; goto _LL732;} else{ goto _LL733;} _LL733: if(( unsigned int)
_temp717 > 2u?(( struct _enum_struct*) _temp717)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL756: _temp755=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp717)->f1;
if( _temp755 == Cyc_CfAbsexp_OneofG){ goto _LL754;} else{ goto _LL735;} _LL754:
_temp753=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp717)->f2; goto _LL734;} else{ goto _LL735;} _LL735: if(( unsigned int)
_temp717 > 2u?(( struct _enum_struct*) _temp717)->tag == Cyc_CfAbsexp_GroupAE_tag:
0){ _LL760: _temp759=( void*)(( struct Cyc_CfAbsexp_GroupAE_struct*) _temp717)->f1;
if( _temp759 == Cyc_CfAbsexp_UnorderedG){ goto _LL758;} else{ goto _LL718;}
_LL758: _temp757=( struct Cyc_List_List*)(( struct Cyc_CfAbsexp_GroupAE_struct*)
_temp717)->f2; goto _LL736;} else{ goto _LL718;} _LL720:{ void* _temp761= Cyc_CfAbsexp_eval_absop_l(
pinfo_dict, _temp739); void* _temp767; void* _temp769; struct Cyc_CfFlowInfo_Place*
_temp771; _LL763: if((( struct _enum_struct*) _temp761)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL770: _temp769=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp761)->f1;
goto _LL768; _LL768: _temp767=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp761)->f2; goto _LL764;} else{ goto _LL765;} _LL765: if((( struct
_enum_struct*) _temp761)->tag == Cyc_CfFlowInfo_MustPointTo_tag){ _LL772:
_temp771=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_CfFlowInfo_MustPointTo_struct*)
_temp761)->f1; goto _LL766;} else{ goto _LL762;} _LL764: goto _LL762; _LL766: {
void* old_pinfo= Cyc_CfFlowInfo_lookup_place( pinfo_dict, _temp771); void*
new_pinfo= Cyc_CfAbsexp_eval_absop_r( pinfo_dict, _temp737); struct _tuple3
_temp774=({ struct _tuple3 _temp773; _temp773.f1= old_pinfo; _temp773.f2=
new_pinfo; _temp773;}); void* _temp782; void* _temp784; void* _temp786; void*
_temp788; void* _temp790; struct Cyc_Dict_Dict* _temp792; void* _temp794; void*
_temp796; void* _temp798; void* _temp800; void* _temp802; struct Cyc_Dict_Dict*
_temp804; _LL776: _LL791: _temp790= _temp774.f1; if((( struct _enum_struct*)
_temp790)->tag == Cyc_CfFlowInfo_TuplePI_tag){ _LL793: _temp792=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_TuplePI_struct*) _temp790)->f1; goto _LL783;} else{ goto
_LL778;} _LL783: _temp782= _temp774.f2; if((( struct _enum_struct*) _temp782)->tag
== Cyc_CfFlowInfo_LeafPI_tag){ _LL785: _temp784=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*)
_temp782)->f1; if((( struct _enum_struct*) _temp784)->tag == Cyc_CfFlowInfo_UnknownIS_tag){
_LL789: _temp788=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp784)->f1;
goto _LL787; _LL787: _temp786=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp784)->f2; goto _LL777;} else{ goto _LL778;}} else{ goto _LL778;} _LL778:
_LL803: _temp802= _temp774.f1; if((( struct _enum_struct*) _temp802)->tag == Cyc_CfFlowInfo_StructPI_tag){
_LL805: _temp804=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_StructPI_struct*)
_temp802)->f1; goto _LL795;} else{ goto _LL780;} _LL795: _temp794= _temp774.f2;
if((( struct _enum_struct*) _temp794)->tag == Cyc_CfFlowInfo_LeafPI_tag){ _LL797:
_temp796=( void*)(( struct Cyc_CfFlowInfo_LeafPI_struct*) _temp794)->f1; if(((
struct _enum_struct*) _temp796)->tag == Cyc_CfFlowInfo_UnknownIS_tag){ _LL801:
_temp800=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*) _temp796)->f1; goto
_LL799; _LL799: _temp798=( void*)(( struct Cyc_CfFlowInfo_UnknownIS_struct*)
_temp796)->f2; goto _LL779;} else{ goto _LL780;}} else{ goto _LL780;} _LL780:
goto _LL781; _LL777: _temp798= _temp786; goto _LL779; _LL779: new_pinfo= Cyc_CfFlowInfo_assign_unknown_dict(
_temp798, old_pinfo); pinfo_dict= Cyc_CfFlowInfo_insert_place( pinfo_dict,
_temp771, new_pinfo);{ void* ans=({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp806=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp806->tag= Cyc_CfFlowInfo_InitsFL_tag; _temp806->f1= pinfo_dict;( void*)
_temp806;}); Cyc_NewControlFlow_update_tryflow( env, ans); return ans;} _LL781: {
struct Cyc_List_List* escaping_pinfos= 0; Cyc_CfAbsexp_assign_escape(&
escaping_pinfos, old_pinfo, new_pinfo); for( 0; escaping_pinfos != 0;
escaping_pinfos= escaping_pinfos->tl){ void* esc_pinfo=( void*) escaping_pinfos->hd;
if( ! Cyc_CfFlowInfo_isAllInit( pinfo_dict, esc_pinfo)){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp807=( char*)"uninitialized value assigned to escaped place";
struct _tagged_string _temp808; _temp808.curr= _temp807; _temp808.base= _temp807;
_temp808.last_plus_one= _temp807 + 46; _temp808;})); return Cyc_CfFlowInfo_BottomFL;}
pinfo_dict= Cyc_CfFlowInfo_escape_pointsto( esc_pinfo, pinfo_dict);} pinfo_dict=
Cyc_CfFlowInfo_insert_place( pinfo_dict, _temp771, new_pinfo);{ void* ans=({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp809=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp809->tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp809->f1= pinfo_dict;( void*) _temp809;}); Cyc_NewControlFlow_update_tryflow(
env, ans); return ans;}} _LL775:;} _LL762:;} _temp741= _temp737; goto _LL722;
_LL722: { void* pinfo= Cyc_CfAbsexp_eval_absop_r( pinfo_dict, _temp741); if( !
Cyc_CfFlowInfo_isAllInit( pinfo_dict, pinfo)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp810= Cyc_CfAbsexp_absop2string( _temp741); xprintf("%.*s must be fully initialized to be used here",
_temp810.last_plus_one - _temp810.curr, _temp810.curr);})); return Cyc_CfFlowInfo_BottomFL;}{
struct Cyc_Dict_Dict* out_dict= Cyc_CfFlowInfo_escape_pointsto( pinfo,
pinfo_dict); if( out_dict == pinfo_dict){ return in_flow;}{ void* ans=({ struct
Cyc_CfFlowInfo_InitsFL_struct* _temp811=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp811->tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp811->f1= pinfo_dict;( void*) _temp811;}); Cyc_NewControlFlow_update_tryflow(
env, ans); return ans;}}} _LL724: return({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp812=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp812->tag= Cyc_CfFlowInfo_InitsFL_tag; _temp812->f1=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert)( pinfo_dict,
_temp745,(( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots,
_temp745));( void*) _temp812;}); _LL726: return Cyc_CfFlowInfo_BottomFL; _LL728:
return in_flow; _LL730: return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp747,
in_flow); _LL732: for( 0; _temp749 != 0; _temp749= _temp749->tl){ in_flow= Cyc_CfAbsexp_eval_absexp(
env, loc,( void*) _temp749->hd, in_flow);} return in_flow; _LL734: { void*
out_flow= Cyc_CfFlowInfo_BottomFL; for( 0; _temp753 != 0; _temp753= _temp753->tl){
out_flow= Cyc_CfFlowInfo_join_flow( out_flow, Cyc_CfAbsexp_eval_absexp( env, loc,(
void*) _temp753->hd, in_flow));} return out_flow;} _LL736: { void* out_flow=
in_flow; do { in_flow= out_flow; out_flow= in_flow;{ struct Cyc_List_List* ael2=
_temp757; for( 0; ael2 != 0; ael2= ael2->tl){ out_flow= Cyc_CfFlowInfo_join_flow(
out_flow, Cyc_CfAbsexp_eval_absexp( env, loc,( void*) ael2->hd, out_flow));}}} while (
! Cyc_CfFlowInfo_flow_lessthan_approx( out_flow, in_flow));{ struct Cyc_Dict_Dict*
pinfo_dict;{ void* _temp813= in_flow; struct Cyc_Dict_Dict* _temp819; _LL815:
if( _temp813 == Cyc_CfFlowInfo_BottomFL){ goto _LL816;} else{ goto _LL817;}
_LL817: if(( unsigned int) _temp813 > 1u?(( struct _enum_struct*) _temp813)->tag
== Cyc_CfFlowInfo_InitsFL_tag: 0){ _LL820: _temp819=( struct Cyc_Dict_Dict*)((
struct Cyc_CfFlowInfo_InitsFL_struct*) _temp813)->f1; goto _LL818;} else{ goto
_LL814;} _LL816: return Cyc_CfFlowInfo_BottomFL; _LL818: pinfo_dict= _temp819;
goto _LL814; _LL814:;}{ struct Cyc_List_List* _temp823; struct Cyc_List_List*
_temp825; struct _tuple5 _temp821= Cyc_CfAbsexp_unordered_mallocs( ae, 0);
_LL826: _temp825= _temp821.f1; goto _LL824; _LL824: _temp823= _temp821.f2; goto
_LL822; _LL822: for( 0; _temp823 != 0; _temp823= _temp823->tl){ void* malloc_pt=({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp827=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp827->tag= Cyc_CfFlowInfo_MallocPt_tag;
_temp827->f1=( struct Cyc_Absyn_Exp*) _temp823->hd;( void*) _temp827;});
pinfo_dict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void*
data)) Cyc_Dict_insert)( pinfo_dict, malloc_pt,(( void*(*)( struct Cyc_Dict_Dict*
d, void* key)) Cyc_Dict_lookup)( env->roots, malloc_pt));} out_flow=({ struct
Cyc_CfFlowInfo_InitsFL_struct* _temp828=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp828->tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp828->f1= pinfo_dict;( void*) _temp828;}); for( 0; _temp825 != 0; _temp825=
_temp825->tl){ out_flow= Cyc_CfAbsexp_eval_absexp( env, loc,( void*) _temp825->hd,
out_flow);} return out_flow;}}} _LL718:;}}