#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2;}; struct _tuple1{ void* f1; struct
_tagged_string* f2;}; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual*
f2; void* f3;}; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt*
f2;}; struct _tuple4{ struct Cyc_Absyn_Tqual* f1; void* f2;}; struct _tuple5{
struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2;}; typedef unsigned int Cyc_uint;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v;}; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern void* Cyc_Core_snd(
struct _tuple0*); extern int Cyc_Core_intcmp( int, int); extern char Cyc_Core_InvalidArg_tag[
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
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[ 11u];
struct Cyc_List_List_empty_struct{ char* tag;}; extern struct Cyc_List_List* Cyc_List_tl(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map( void*(* f)(
void*), struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[ 14u];
struct Cyc_List_List_mismatch_struct{ char* tag;}; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern char Cyc_List_Nth_tag[ 4u];
struct Cyc_List_Nth_struct{ char* tag;}; struct Cyc_Dict_Dict; typedef struct
Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present_tag[ 8u]; struct Cyc_Dict_Present_struct{ char* tag;};
extern char Cyc_Dict_Absent_tag[ 7u]; struct Cyc_Dict_Absent_struct{ char* tag;};
extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*)); extern
struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void*
data); extern struct Cyc_Dict_Dict* Cyc_Dict_insert_new( struct Cyc_Dict_Dict* d,
void* key, void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* key); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t; extern char
Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1;}; extern char Cyc_Stdio_FileCloseError_tag[ 15u];
struct Cyc_Stdio_FileCloseError_struct{ char* tag;}; extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); extern void* Cyc_Assert_AssertFail(
struct _tagged_string msg); struct Cyc_Lineno_Pos{ struct _tagged_string
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
int tag; struct Cyc_List_List* f1; struct Cyc_Absyn_Switch_clause** f2;}; extern
const int Cyc_Absyn_Decl_s_tag; struct Cyc_Absyn_Decl_s_struct{ int tag; struct
Cyc_Absyn_Decl* f1; struct Cyc_Absyn_Stmt* f2;}; extern const int Cyc_Absyn_Cut_s_tag;
struct Cyc_Absyn_Cut_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1;}; extern
const int Cyc_Absyn_Splice_s_tag; struct Cyc_Absyn_Splice_s_struct{ int tag;
struct Cyc_Absyn_Stmt* f1;}; extern const int Cyc_Absyn_Label_s_tag; struct Cyc_Absyn_Label_s_struct{
int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2;}; extern const
int Cyc_Absyn_Do_s_tag; struct Cyc_Absyn_Do_s_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; struct _tuple3 f2;}; extern const int Cyc_Absyn_TryCatch_s_tag; struct Cyc_Absyn_TryCatch_s_struct{
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
struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt*
param_vardecls;}; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt*
name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields;}; struct Cyc_Absyn_Enumfield{
struct _tuple1* name; struct Cyc_Absyn_Exp* tag; struct Cyc_List_List* tvs;
struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc;}; struct Cyc_Absyn_Enumdecl{
void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields;}; struct Cyc_Absyn_Xenumdecl{ void* sc; struct _tuple1* name; struct Cyc_List_List*
fields;}; struct Cyc_Absyn_Typedefdecl{ struct _tuple1* name; struct Cyc_List_List*
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
int tag; struct _tuple1* f1; struct Cyc_List_List* f2;}; extern const int Cyc_Absyn_ExternC_d_tag;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1;}; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc;}; extern const int
Cyc_Absyn_ArrayElement_tag; struct Cyc_Absyn_ArrayElement_struct{ int tag;
struct Cyc_Absyn_Exp* f1;}; extern const int Cyc_Absyn_FieldName_tag; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_string* f1;}; extern char Cyc_Absyn_EmptyAnnot_tag[ 11u];
struct Cyc_Absyn_EmptyAnnot_struct{ char* tag;}; extern struct Cyc_Absyn_Exp*
Cyc_Absyn_true_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*, struct _tagged_string*,
struct Cyc_Position_Segment*); struct Cyc_Set_Set; typedef struct Cyc_Set_Set*
Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set*
Cyc_Set_set_t; extern char Cyc_Set_Absent_tag[ 7u]; typedef void* Cyc_Tcenv_Resolved;
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
struct Cyc_Position_Segment*, struct _tagged_string); extern void* Cyc_Tcutil_compress(
void* t); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); typedef void* Cyc_CfFlowInfo_LocalRoot; typedef void* Cyc_CfFlowInfo_Field;
struct Cyc_CfFlowInfo_Place; typedef void* Cyc_CfFlowInfo_Escaped; typedef void*
Cyc_CfFlowInfo_InitLevel; typedef void* Cyc_CfFlowInfo_InitState; typedef void*
Cyc_CfFlowInfo_PathInfo; typedef void* Cyc_CfFlowInfo_FlowInfo; typedef void*
Cyc_CfFlowInfo_local_root_t; typedef void* Cyc_CfFlowInfo_field_t; typedef
struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t; typedef void* Cyc_CfFlowInfo_escaped_t;
typedef void* Cyc_CfFlowInfo_init_level_t; typedef void* Cyc_CfFlowInfo_init_state_t;
typedef void* Cyc_CfFlowInfo_path_info_t; typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_pinfo_dict_t;
typedef void* Cyc_CfFlowInfo_flow_info_t; extern const int Cyc_CfFlowInfo_VarRoot_tag;
struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;};
extern const int Cyc_CfFlowInfo_MallocPt_tag; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1;}; extern const int Cyc_CfFlowInfo_StructF_tag;
struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_string* f1;};
extern const int Cyc_CfFlowInfo_TupleF_tag; struct Cyc_CfFlowInfo_TupleF_struct{
int tag; int f1;}; struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List*
fields;}; extern void* Cyc_CfFlowInfo_Esc; extern void* Cyc_CfFlowInfo_Unesc;
extern void* Cyc_CfFlowInfo_NoneIL; extern void* Cyc_CfFlowInfo_ThisIL; extern
void* Cyc_CfFlowInfo_AllIL; extern const int Cyc_CfFlowInfo_UnknownIS_tag;
struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag; void* f1; void* f2;}; extern
const int Cyc_CfFlowInfo_MustPointTo_tag; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1;}; extern const int Cyc_CfFlowInfo_LeafPI_tag;
struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; void* f1;}; extern const int Cyc_CfFlowInfo_TuplePI_tag;
struct Cyc_CfFlowInfo_TuplePI_struct{ int tag; struct Cyc_Dict_Dict* f1;};
extern const int Cyc_CfFlowInfo_StructPI_tag; struct Cyc_CfFlowInfo_StructPI_struct{
int tag; struct Cyc_Dict_Dict* f1;}; extern void* Cyc_CfFlowInfo_BottomFL;
extern const int Cyc_CfFlowInfo_InitsFL_tag; struct Cyc_CfFlowInfo_InitsFL_struct{
int tag; struct Cyc_Dict_Dict* f1;}; extern int Cyc_CfFlowInfo_local_root_cmp(
void*, void*); extern void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( void* t);
extern void* Cyc_CfFlowInfo_assign_unknown_dict( void*, void*); extern void* Cyc_CfFlowInfo_join_flow(
void*, void*); extern int Cyc_CfFlowInfo_flow_lessthan_approx( void* f1, void*
f2); extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt*
enclosee, struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{
struct Cyc_Dict_Dict* roots; int in_try; void* tryflow;}; typedef struct Cyc_NewControlFlow_AnalEnv*
Cyc_NewControlFlow_analenv_t; extern void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv* env, void* new_flow); extern void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv*, struct Cyc_Absyn_Stmt*, void*); void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds); extern char Cyc_CfAbsexp_BadAbsexp_tag[ 10u]; struct
Cyc_CfAbsexp_BadAbsexp_struct{ char* tag;}; typedef void* Cyc_CfAbsexp_Absexp;
typedef void* Cyc_CfAbsexp_Absexpgroup; extern void* Cyc_CfAbsexp_OrderedG;
extern void* Cyc_CfAbsexp_UnorderedG; extern void* Cyc_CfAbsexp_OneofG; typedef
void* Cyc_CfAbsexp_Absop; typedef void* Cyc_CfAbsexp_absexp_t; typedef void* Cyc_CfAbsexp_absexpgroup_t;
typedef void* Cyc_CfAbsexp_absop_t; extern void* Cyc_CfAbsexp_mkAnyOrderG();
extern void* Cyc_CfAbsexp_mkUnknownOp(); extern void* Cyc_CfAbsexp_mkAddressOp(
void* ao); extern void* Cyc_CfAbsexp_mkLocalOp( struct Cyc_Absyn_Vardecl* vd);
extern void* Cyc_CfAbsexp_mkMemberOp( void* ao, void* f); extern void* Cyc_CfAbsexp_mkMallocOp(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkDerefOp( void* ao); extern
void* Cyc_CfAbsexp_mkBottomAE(); extern void* Cyc_CfAbsexp_mkSkipAE(); extern
void* Cyc_CfAbsexp_mkAssignAE( void* l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt*
s); extern void* Cyc_CfAbsexp_mkGroupAE( void* g, void* ae1, void* ae2); extern
void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List* ael); extern int
Cyc_CfAbsexp_isUnknownOp( void*); extern void* Cyc_CfAbsexp_eval_absexp( struct
Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void* ae,
void* in_flow); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; typedef struct Cyc_Absyn_Vardecl* Cyc_NewControlFlow_vardecl_t;
typedef struct Cyc_Absyn_Stmt* Cyc_NewControlFlow_stmt_t; typedef void* Cyc_NewControlFlow_typ_t;
struct Cyc_NewControlFlow_CFStmtAnnot{ struct Cyc_Absyn_Stmt* encloser; int
visited; void* flow; struct Cyc_List_List* absexps;}; typedef struct Cyc_NewControlFlow_CFStmtAnnot
Cyc_NewControlFlow_cf_stmt_annot_t; char Cyc_NewControlFlow_CFAnnot_tag[ 8u]="CFAnnot";
struct Cyc_NewControlFlow_CFAnnot_struct{ char* tag; struct Cyc_NewControlFlow_CFStmtAnnot
f1;}; void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt* enclosee,
struct Cyc_Absyn_Stmt* encloser){ enclosee->annot=({ struct Cyc_NewControlFlow_CFAnnot_struct*
_temp0=( struct Cyc_NewControlFlow_CFAnnot_struct*) GC_malloc( sizeof( struct
Cyc_NewControlFlow_CFAnnot_struct));* _temp0=( struct Cyc_NewControlFlow_CFAnnot_struct){.tag=
Cyc_NewControlFlow_CFAnnot_tag,.f1=({ struct Cyc_NewControlFlow_CFStmtAnnot
_temp1; _temp1.encloser= encloser; _temp1.visited= 0; _temp1.flow=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1.absexps= 0; _temp1;})};( struct _xenum_struct*) _temp0;});} static struct
Cyc_NewControlFlow_CFStmtAnnot* Cyc_NewControlFlow_get_stmt_annot( struct Cyc_Absyn_Stmt*
s){ struct _xenum_struct* _temp2= s->annot; struct Cyc_NewControlFlow_CFStmtAnnot
_temp8; struct Cyc_NewControlFlow_CFStmtAnnot* _temp10; _LL4: if((*(( struct
_xenum_struct*) _temp2)).tag == Cyc_NewControlFlow_CFAnnot_tag){ _LL9: _temp8=((
struct Cyc_NewControlFlow_CFAnnot_struct*) _temp2)->f1; _temp10=&(( struct Cyc_NewControlFlow_CFAnnot_struct*)
_temp2)->f1; goto _LL5;} else{ goto _LL6;} _LL6: goto _LL7; _LL5: return _temp10;
_LL7:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp11=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp11=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp12=( char*)"ControlFlow -- wrong stmt annotation";
struct _tagged_string _temp13; _temp13.curr= _temp12; _temp13.base= _temp12;
_temp13.last_plus_one= _temp12 + 37; _temp13;})};( struct _xenum_struct*)
_temp11;})); _LL3:;} void Cyc_NewControlFlow_cf_set_absexps( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* ael){( Cyc_NewControlFlow_get_stmt_annot( s))->absexps=
ael;} struct Cyc_NewControlFlow_SharedAbsEnv{ struct Cyc_Dict_Dict* all_roots;
struct Cyc_List_List* comprehension_vars;}; struct Cyc_NewControlFlow_AbsEnv{
struct Cyc_NewControlFlow_SharedAbsEnv* shared; struct Cyc_List_List* lhsides;
struct Cyc_List_List* access_path;}; typedef struct Cyc_NewControlFlow_AbsEnv
Cyc_NewControlFlow_absenv_t; struct Cyc_NewControlFlow_AbsSynOne{ void*
inner_exp; void* assigns;}; typedef struct Cyc_NewControlFlow_AbsSynOne Cyc_NewControlFlow_abssyn_one_t;
struct Cyc_NewControlFlow_AbsSyn{ struct Cyc_NewControlFlow_AbsSynOne when_true;
struct Cyc_NewControlFlow_AbsSynOne when_false; struct Cyc_List_List* lvalues;};
typedef struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abssyn_t; void Cyc_NewControlFlow_add_var_root(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Vardecl* vd){( env.shared)->all_roots=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert_new)((
env.shared)->all_roots,({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp14=( struct
Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp14->tag= Cyc_CfFlowInfo_VarRoot_tag; _temp14->f1= vd;( void*) _temp14;}),
Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(( void*) vd->type));} void Cyc_NewControlFlow_add_malloc_root(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e, void* t){( env.shared)->all_roots=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert_new)((
env.shared)->all_roots,({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp15=(
struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp15->tag= Cyc_CfFlowInfo_MallocPt_tag; _temp15->f1= e;( void*) _temp15;}),
Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( t));} void Cyc_NewControlFlow_add_enum_root(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Enumfield*
ef){ struct Cyc_Absyn_Enumfield _temp18; struct Cyc_Position_Segment* _temp19;
struct Cyc_List_List* _temp21; struct Cyc_List_List* _temp23; struct Cyc_Absyn_Exp*
_temp25; struct _tuple1* _temp27; struct Cyc_Absyn_Enumfield* _temp16= ef;
_temp18=* _temp16; _LL28: _temp27=( struct _tuple1*) _temp18.name; goto _LL26;
_LL26: _temp25=( struct Cyc_Absyn_Exp*) _temp18.tag; goto _LL24; _LL24: _temp23=(
struct Cyc_List_List*) _temp18.tvs; goto _LL22; _LL22: _temp21=( struct Cyc_List_List*)
_temp18.typs; goto _LL20; _LL20: _temp19=( struct Cyc_Position_Segment*) _temp18.loc;
goto _LL17; _LL17: { struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)( int(*
comp)( int, int))) Cyc_Dict_empty)( Cyc_Core_intcmp);{ int i= 0; for( 0; _temp21
!= 0; _temp21= _temp21->tl, ++ i){ struct _tuple4 _temp31; void* _temp32; struct
Cyc_Absyn_Tqual* _temp34; struct _tuple4* _temp29=( struct _tuple4*) _temp21->hd;
_temp31=* _temp29; _LL35: _temp34= _temp31.f1; goto _LL33; _LL33: _temp32=
_temp31.f2; goto _LL30; _LL30: d=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, int key, void* data)) Cyc_Dict_insert)( d, i, Cyc_CfFlowInfo_typ_to_unesc_none_pinfo(
_temp32));}}( env.shared)->all_roots=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert_new)(( env.shared)->all_roots,({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp36=( struct Cyc_CfFlowInfo_MallocPt_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp36->tag= Cyc_CfFlowInfo_MallocPt_tag;
_temp36->f1= e;( void*) _temp36;}),({ struct Cyc_CfFlowInfo_TuplePI_struct*
_temp37=( struct Cyc_CfFlowInfo_TuplePI_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_TuplePI_struct));
_temp37->tag= Cyc_CfFlowInfo_TuplePI_tag; _temp37->f1= d;( void*) _temp37;}));}}
struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_change_lhs( struct Cyc_NewControlFlow_AbsEnv
src, struct Cyc_List_List* new_lhs){ return({ struct Cyc_NewControlFlow_AbsEnv
_temp38; _temp38.shared= src.shared; _temp38.lhsides= new_lhs; _temp38.access_path=
src.access_path; _temp38;});} static struct Cyc_List_List* Cyc_NewControlFlow_unknown_list=
0; static struct Cyc_List_List* Cyc_NewControlFlow_use_it_list(){ if( Cyc_NewControlFlow_unknown_list
== 0){ Cyc_NewControlFlow_unknown_list=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp39=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp39[ 0]=({ struct Cyc_List_List _temp40; _temp40.hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp40.tl= 0; _temp40;}); _temp39;});} return Cyc_NewControlFlow_unknown_list;}
struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_use_it_env( struct Cyc_NewControlFlow_AbsEnv
src){ struct Cyc_NewControlFlow_AbsEnv ans= Cyc_NewControlFlow_change_lhs( src,
Cyc_NewControlFlow_use_it_list()); ans.access_path= 0; return ans;} struct Cyc_NewControlFlow_AbsSynOne
Cyc_NewControlFlow_meet_absexp( struct Cyc_NewControlFlow_AbsSyn syn){ struct
Cyc_List_List* _temp43; struct Cyc_NewControlFlow_AbsSynOne _temp45; void*
_temp47; void* _temp49; struct Cyc_NewControlFlow_AbsSynOne _temp51; void*
_temp53; void* _temp55; struct Cyc_NewControlFlow_AbsSyn _temp41= syn; _LL52:
_temp51=( struct Cyc_NewControlFlow_AbsSynOne) _temp41.when_true; _LL56: _temp55=(
void*) _temp51.inner_exp; goto _LL54; _LL54: _temp53=( void*) _temp51.assigns;
goto _LL46; _LL46: _temp45=( struct Cyc_NewControlFlow_AbsSynOne) _temp41.when_false;
_LL50: _temp49=( void*) _temp45.inner_exp; goto _LL48; _LL48: _temp47=( void*)
_temp45.assigns; goto _LL44; _LL44: _temp43=( struct Cyc_List_List*) _temp41.lvalues;
goto _LL42; _LL42: { void* inn= _temp55 == _temp49? _temp55: Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OneofG, _temp55, _temp49); void* ass= _temp53 == _temp47? _temp53:
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG, _temp53, _temp47); return({ struct
Cyc_NewControlFlow_AbsSynOne _temp57; _temp57.inner_exp=( void*) inn; _temp57.assigns=(
void*) ass; _temp57;});}} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_mkSyn_tf(
void* ae, struct Cyc_List_List* lvals, void* assigns){ struct Cyc_NewControlFlow_AbsSynOne
one_way=({ struct Cyc_NewControlFlow_AbsSynOne _temp58; _temp58.inner_exp=( void*)
ae; _temp58.assigns=( void*) assigns; _temp58;}); return({ struct Cyc_NewControlFlow_AbsSyn
_temp59; _temp59.when_true= one_way; _temp59.when_false= one_way; _temp59.lvalues=
lvals; _temp59;});} void* Cyc_NewControlFlow_make_assigns( struct Cyc_NewControlFlow_AbsEnv
env, void* ae){ if( env.lhsides == 0){ return Cyc_CfAbsexp_mkSkipAE();} if(( env.lhsides)->tl
== 0){ return Cyc_CfAbsexp_mkAssignAE(( void*)( env.lhsides)->hd, ae);}{ struct
Cyc_List_List* l= 0;{ struct Cyc_List_List* lhsides= env.lhsides; for( 0;
lhsides != 0; lhsides= lhsides->tl){ l=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp60=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp60[ 0]=({ struct Cyc_List_List _temp61; _temp61.hd=( void*) Cyc_CfAbsexp_mkAssignAE((
void*) lhsides->hd, ae); _temp61.tl= l; _temp61;}); _temp60;});}} return Cyc_CfAbsexp_mkGroupAE_l(
Cyc_CfAbsexp_OneofG, l);}} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e); void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Stmt* s); struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_use_exp_unordered( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_List_List* es, struct Cyc_List_List* lhs){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_use_it_env( env); void* es_exp= Cyc_CfAbsexp_mkSkipAE();
void* uses_exp= Cyc_CfAbsexp_mkSkipAE(); for( 0; es != 0; es= es->tl){ struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( elt_env,( struct
Cyc_Absyn_Exp*) es->hd); void* _temp64; void* _temp66; struct Cyc_NewControlFlow_AbsSynOne
_temp62= Cyc_NewControlFlow_meet_absexp( syn); _LL67: _temp66=( void*) _temp62.inner_exp;
goto _LL65; _LL65: _temp64=( void*) _temp62.assigns; goto _LL63; _LL63: es_exp=
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, _temp66, es_exp); uses_exp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_mkAnyOrderG(), _temp64, uses_exp);} return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, es_exp, uses_exp), lhs, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_malloc_and_init(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e, void* es_exp,
void* useexp){ void* ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkMallocAE(
e), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, es_exp, useexp)); return Cyc_NewControlFlow_mkSyn_tf(
ansexp, 0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkAddressOp( Cyc_CfAbsexp_mkMallocOp(
e))));} void* Cyc_NewControlFlow_destruct_path( struct Cyc_NewControlFlow_AbsEnv
env, void* absop){{ struct Cyc_List_List* path= env.access_path; for( 0; path !=
0; path= path->tl){ void* _temp68=( void*)(( struct Cyc_Absyn_Exp*) path->hd)->r;
struct Cyc_Absyn_Exp* _temp82; struct Cyc_Absyn_Exp* _temp84; struct
_tagged_string* _temp86; struct Cyc_Absyn_Exp* _temp88; struct _tagged_string*
_temp90; struct Cyc_Absyn_Exp* _temp92; struct Cyc_Absyn_Exp* _temp94; struct
Cyc_Absyn_Exp* _temp96; _LL70: if((( struct _enum_struct*) _temp68)->tag == Cyc_Absyn_Address_e_tag){
_LL83: _temp82=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp68)->f1; goto _LL71;} else{ goto _LL72;} _LL72: if((( struct _enum_struct*)
_temp68)->tag == Cyc_Absyn_Deref_e_tag){ _LL85: _temp84=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Deref_e_struct*) _temp68)->f1; goto _LL73;} else{ goto _LL74;}
_LL74: if((( struct _enum_struct*) _temp68)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL89: _temp88=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp68)->f1; goto _LL87; _LL87: _temp86=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp68)->f2; goto _LL75;} else{ goto _LL76;} _LL76: if((( struct _enum_struct*)
_temp68)->tag == Cyc_Absyn_StructMember_e_tag){ _LL93: _temp92=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp68)->f1; goto _LL91; _LL91:
_temp90=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp68)->f2; goto _LL77;} else{ goto _LL78;} _LL78: if((( struct _enum_struct*)
_temp68)->tag == Cyc_Absyn_Subscript_e_tag){ _LL97: _temp96=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp68)->f1; goto _LL95; _LL95: _temp94=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp68)->f2;
goto _LL79;} else{ goto _LL80;} _LL80: goto _LL81; _LL71: path->tl == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp98=( char*)"new_control_flow.cyc:235 path->tl == null";
struct _tagged_string _temp99; _temp99.curr= _temp98; _temp99.base= _temp98;
_temp99.last_plus_one= _temp98 + 42; _temp99;})); return Cyc_CfAbsexp_mkAddressOp(
absop); _LL73: absop= Cyc_CfAbsexp_mkDerefOp( absop); goto _LL69; _LL75: absop=
Cyc_CfAbsexp_mkDerefOp( absop); _temp90= _temp86; goto _LL77; _LL77: absop= Cyc_CfAbsexp_mkMemberOp(
absop,({ struct Cyc_CfFlowInfo_StructF_struct* _temp100=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp100->tag= Cyc_CfFlowInfo_StructF_tag;
_temp100->f1= _temp90;( void*) _temp100;})); goto _LL69; _LL79: { unsigned int i=
Cyc_Evexp_eval_const_uint_exp( _temp94); absop= Cyc_CfAbsexp_mkMemberOp( absop,({
struct Cyc_CfFlowInfo_TupleF_struct* _temp101=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc_atomic( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp101->tag=
Cyc_CfFlowInfo_TupleF_tag; _temp101->f1=( int) i;( void*) _temp101;})); goto
_LL69;} _LL81:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp102=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp102=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp103=( char*)"abstract_exp: unexpected acces path element";
struct _tagged_string _temp104; _temp104.curr= _temp103; _temp104.base= _temp103;
_temp104.last_plus_one= _temp103 + 44; _temp104;})};( struct _xenum_struct*)
_temp102;})); _LL69:;}} return absop;} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp105=(
void*) e->r; struct Cyc_Absyn_Exp* _temp185; struct Cyc_List_List* _temp187;
struct Cyc_Absyn_Exp* _temp189; void* _temp191; int _temp193; void* _temp195;
void* _temp197; int _temp199; void* _temp201; void* _temp203; void* _temp205;
void* _temp207; struct Cyc_Absyn_Fndecl* _temp209; struct _tuple1* _temp211;
void* _temp213; struct Cyc_Absyn_Vardecl* _temp215; struct _tuple1* _temp217;
void* _temp219; struct Cyc_Absyn_Vardecl* _temp221; struct _tuple1* _temp223;
void* _temp225; struct Cyc_Absyn_Vardecl* _temp227; struct _tuple1* _temp229;
void* _temp231; struct Cyc_Absyn_Vardecl* _temp233; struct _tuple1* _temp235;
struct Cyc_Absyn_Enumfield* _temp237; struct Cyc_Absyn_Enumdecl* _temp239;
struct Cyc_List_List* _temp241; struct Cyc_Core_Opt* _temp243; struct Cyc_Core_Opt*
_temp245; struct Cyc_Absyn_Enumfield* _temp247; struct Cyc_Absyn_Xenumdecl*
_temp249; struct Cyc_List_List* _temp251; struct Cyc_Core_Opt* _temp253; struct
Cyc_Absyn_Exp* _temp255; struct Cyc_Core_Opt* _temp257; struct Cyc_Absyn_Exp*
_temp259; struct Cyc_Absyn_Exp* _temp261; struct Cyc_Core_Opt* _temp263; struct
Cyc_Absyn_Exp* _temp265; struct Cyc_List_List* _temp267; void* _temp269; struct
Cyc_Absyn_Exp* _temp271; void* _temp273; struct Cyc_List_List* _temp275; struct
Cyc_Absyn_Exp* _temp277; struct Cyc_Absyn_Exp* _temp279; struct Cyc_Absyn_Exp*
_temp281; struct Cyc_Absyn_Exp* _temp283; struct Cyc_Absyn_Exp* _temp285; struct
_tagged_string* _temp287; struct Cyc_Absyn_Exp* _temp289; struct _tagged_string*
_temp291; struct Cyc_Absyn_Exp* _temp293; struct Cyc_List_List* _temp295; struct
Cyc_Absyn_Structdecl* _temp297; struct Cyc_List_List* _temp299; struct Cyc_Core_Opt*
_temp301; struct _tuple1* _temp303; struct Cyc_List_List* _temp305; int _temp307;
void* _temp309; struct Cyc_Absyn_Exp* _temp311; struct Cyc_Absyn_Exp* _temp313;
struct Cyc_Absyn_Exp* _temp315; struct Cyc_Absyn_Exp* _temp317; struct Cyc_Absyn_Exp*
_temp319; struct Cyc_Absyn_Stmt* _temp321; struct Cyc_Absyn_Exp* _temp323;
struct Cyc_Absyn_Exp* _temp325; struct Cyc_Absyn_Exp* _temp327; struct Cyc_Absyn_Exp*
_temp329; struct Cyc_Absyn_Vardecl* _temp331; void* _temp333; struct _tuple1*
_temp335; struct _tuple1* _temp337; struct Cyc_List_List* _temp339; struct Cyc_Absyn_Exp*
_temp341; struct Cyc_List_List* _temp343; struct Cyc_Core_Opt* _temp345; struct
Cyc_List_List* _temp347; struct _tuple2* _temp349; struct Cyc_Absyn_Fndecl*
_temp351; struct Cyc_Absyn_Exp* _temp353; _LL107: if((( struct _enum_struct*)
_temp105)->tag == Cyc_Absyn_NoInstantiate_e_tag){ _LL186: _temp185=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp105)->f1; goto _LL108;} else{
goto _LL109;} _LL109: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Instantiate_e_tag){
_LL190: _temp189=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp105)->f1; goto _LL188; _LL188: _temp187=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp105)->f2; goto _LL110;} else{ goto _LL111;}
_LL111: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Const_e_tag){
_LL192: _temp191=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp105)->f1; if((
unsigned int) _temp191 > 1u?(( struct _enum_struct*) _temp191)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL196: _temp195=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp191)->f1;
if( _temp195 == Cyc_Absyn_Signed){ goto _LL194;} else{ goto _LL113;} _LL194:
_temp193=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp191)->f2; if( _temp193 ==
0){ goto _LL112;} else{ goto _LL113;}} else{ goto _LL113;}} else{ goto _LL113;}
_LL113: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Const_e_tag){
_LL198: _temp197=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp105)->f1; if((
unsigned int) _temp197 > 1u?(( struct _enum_struct*) _temp197)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL202: _temp201=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp197)->f1;
if( _temp201 == Cyc_Absyn_Signed){ goto _LL200;} else{ goto _LL115;} _LL200:
_temp199=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp197)->f2; if( _temp199 ==
1){ goto _LL114;} else{ goto _LL115;}} else{ goto _LL115;}} else{ goto _LL115;}
_LL115: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Sizeof_e_tag){
_LL204: _temp203=( void*)(( struct Cyc_Absyn_Sizeof_e_struct*) _temp105)->f1;
goto _LL116;} else{ goto _LL117;} _LL117: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Const_e_tag){ _LL206: _temp205=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp105)->f1; goto _LL118;} else{ goto _LL119;} _LL119: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Var_e_tag){ _LL212: _temp211=( struct
_tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp105)->f1; goto _LL208; _LL208:
_temp207=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp105)->f2; if((
unsigned int) _temp207 > 1u?(( struct _enum_struct*) _temp207)->tag == Cyc_Absyn_Funname_b_tag:
0){ _LL210: _temp209=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp207)->f1; goto _LL120;} else{ goto _LL121;}} else{ goto _LL121;} _LL121:
if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Var_e_tag){ _LL218:
_temp217=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp105)->f1;
goto _LL214; _LL214: _temp213=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f2; if(( unsigned int) _temp213 > 1u?(( struct _enum_struct*)
_temp213)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL216: _temp215=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp213)->f1; goto _LL122;} else{ goto
_LL123;}} else{ goto _LL123;} _LL123: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Var_e_tag){ _LL224: _temp223=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f1; goto _LL220; _LL220: _temp219=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f2; if(( unsigned int) _temp219 > 1u?(( struct _enum_struct*)
_temp219)->tag == Cyc_Absyn_Param_b_tag: 0){ _LL222: _temp221=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Param_b_struct*) _temp219)->f1; goto _LL124;} else{ goto _LL125;}}
else{ goto _LL125;} _LL125: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Var_e_tag){
_LL230: _temp229=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp105)->f1;
goto _LL226; _LL226: _temp225=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f2; if(( unsigned int) _temp225 > 1u?(( struct _enum_struct*)
_temp225)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL228: _temp227=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp225)->f1; goto _LL126;} else{ goto _LL127;}}
else{ goto _LL127;} _LL127: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Var_e_tag){
_LL236: _temp235=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp105)->f1;
goto _LL232; _LL232: _temp231=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f2; if(( unsigned int) _temp231 > 1u?(( struct _enum_struct*)
_temp231)->tag == Cyc_Absyn_Pat_b_tag: 0){ _LL234: _temp233=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Pat_b_struct*) _temp231)->f1; goto _LL128;} else{ goto _LL129;}}
else{ goto _LL129;} _LL129: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Enum_e_tag){
_LL246: _temp245=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp105)->f1; goto _LL244; _LL244: _temp243=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp105)->f2; goto _LL242; _LL242: _temp241=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Enum_e_struct*) _temp105)->f3; goto _LL240;
_LL240: _temp239=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp105)->f4; goto _LL238; _LL238: _temp237=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Enum_e_struct*) _temp105)->f5; goto _LL130;} else{ goto _LL131;}
_LL131: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Xenum_e_tag){
_LL254: _temp253=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp105)->f1; goto _LL252; _LL252: _temp251=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Xenum_e_struct*) _temp105)->f2; goto _LL250; _LL250: _temp249=( struct
Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp105)->f3; goto
_LL248; _LL248: _temp247=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp105)->f4; goto _LL132;} else{ goto _LL133;} _LL133: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_AssignOp_e_tag){ _LL260: _temp259=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp105)->f1;
goto _LL258; _LL258: _temp257=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp105)->f2; if( _temp257 == 0){ goto _LL256;} else{ goto _LL135;} _LL256:
_temp255=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp105)->f3; goto _LL134;} else{ goto _LL135;} _LL135: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_AssignOp_e_tag){ _LL266: _temp265=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp105)->f1;
goto _LL264; _LL264: _temp263=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp105)->f2; goto _LL262; _LL262: _temp261=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp105)->f3; goto _LL136;} else{ goto _LL137;}
_LL137: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Primop_e_tag){
_LL270: _temp269=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp105)->f1;
goto _LL268; _LL268: _temp267=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp105)->f2; goto _LL138;} else{ goto _LL139;} _LL139: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Cast_e_tag){ _LL274: _temp273=( void*)((
struct Cyc_Absyn_Cast_e_struct*) _temp105)->f1; goto _LL272; _LL272: _temp271=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*) _temp105)->f2; goto
_LL140;} else{ goto _LL141;} _LL141: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_FnCall_e_tag){ _LL278: _temp277=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_FnCall_e_struct*) _temp105)->f1; goto _LL276; _LL276: _temp275=(
struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp105)->f2; goto
_LL142;} else{ goto _LL143;} _LL143: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Subscript_e_tag){ _LL282: _temp281=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp105)->f1; goto _LL280; _LL280:
_temp279=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp105)->f2; goto _LL144;} else{ goto _LL145;} _LL145: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Address_e_tag){ _LL284: _temp283=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp105)->f1; goto
_LL146;} else{ goto _LL147;} _LL147: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Deref_e_tag){ _LL286: _temp285=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp105)->f1; goto _LL148;} else{ goto _LL149;}
_LL149: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL290: _temp289=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp105)->f1; goto _LL288; _LL288: _temp287=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp105)->f2; goto _LL150;} else{ goto _LL151;}
_LL151: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_StructMember_e_tag){
_LL294: _temp293=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp105)->f1; goto _LL292; _LL292: _temp291=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp105)->f2; goto _LL152;} else{ goto _LL153;}
_LL153: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Tuple_e_tag){
_LL296: _temp295=( struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*)
_temp105)->f1; goto _LL154;} else{ goto _LL155;} _LL155: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Struct_e_tag){ _LL304: _temp303=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp105)->f1; goto _LL302;
_LL302: _temp301=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp105)->f2; goto _LL300; _LL300: _temp299=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp105)->f3; goto _LL298; _LL298: _temp297=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp105)->f4;
goto _LL156;} else{ goto _LL157;} _LL157: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Array_e_tag){ _LL308: _temp307=( int)(( struct Cyc_Absyn_Array_e_struct*)
_temp105)->f1; goto _LL306; _LL306: _temp305=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Array_e_struct*) _temp105)->f2; goto _LL158;} else{ goto _LL159;}
_LL159: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Increment_e_tag){
_LL312: _temp311=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp105)->f1; goto _LL310; _LL310: _temp309=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp105)->f2; goto _LL160;} else{ goto _LL161;} _LL161: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Throw_e_tag){ _LL314: _temp313=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp105)->f1; goto
_LL162;} else{ goto _LL163;} _LL163: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Conditional_e_tag){ _LL320: _temp319=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp105)->f1; goto _LL318; _LL318:
_temp317=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp105)->f2; goto _LL316; _LL316: _temp315=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp105)->f3; goto _LL164;} else{ goto _LL165;}
_LL165: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL322: _temp321=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp105)->f1; goto _LL166;} else{ goto _LL167;} _LL167: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL326: _temp325=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp105)->f1; goto
_LL324; _LL324: _temp323=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp105)->f2; goto _LL168;} else{ goto _LL169;} _LL169: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL332:
_temp331=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp105)->f1; goto _LL330; _LL330: _temp329=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp105)->f2; goto _LL328; _LL328: _temp327=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp105)->f3;
goto _LL170;} else{ goto _LL171;} _LL171: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_Var_e_tag){ _LL336: _temp335=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f1; goto _LL334; _LL334: _temp333=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp105)->f2; if( _temp333 == Cyc_Absyn_Unresolved_b){ goto _LL172;} else{ goto
_LL173;}} else{ goto _LL173;} _LL173: if((( struct _enum_struct*) _temp105)->tag
== Cyc_Absyn_UnknownId_e_tag){ _LL338: _temp337=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp105)->f1; goto _LL174;} else{ goto _LL175;} _LL175: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL342: _temp341=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp105)->f1;
goto _LL340; _LL340: _temp339=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp105)->f2; goto _LL176;} else{ goto _LL177;} _LL177: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL346:
_temp345=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp105)->f1; goto _LL344; _LL344: _temp343=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp105)->f2; goto _LL178;} else{ goto
_LL179;} _LL179: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_CompoundLit_e_tag){
_LL350: _temp349=( struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp105)->f1; goto _LL348; _LL348: _temp347=( struct Cyc_List_List*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp105)->f2; goto _LL180;} else{ goto _LL181;}
_LL181: if((( struct _enum_struct*) _temp105)->tag == Cyc_Absyn_Codegen_e_tag){
_LL352: _temp351=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*)
_temp105)->f1; goto _LL182;} else{ goto _LL183;} _LL183: if((( struct
_enum_struct*) _temp105)->tag == Cyc_Absyn_Fill_e_tag){ _LL354: _temp353=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*) _temp105)->f1; goto
_LL184;} else{ goto _LL106;} _LL108: _temp189= _temp185; goto _LL110; _LL110:
return Cyc_NewControlFlow_abstract_exp( env, _temp189); _LL112: env.access_path
== 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp355=( char*)"new_control_flow.cyc:257 env.access_path==null";
struct _tagged_string _temp356; _temp356.curr= _temp355; _temp356.base= _temp355;
_temp356.last_plus_one= _temp355 + 47; _temp356;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp357; _temp357.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp359;
_temp359.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp359.assigns=( void*)
Cyc_CfAbsexp_mkBottomAE(); _temp359;}); _temp357.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp358; _temp358.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp358.assigns=(
void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp358;}); _temp357.lvalues= 0; _temp357;}); _LL114: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp360=( char*)"new_control_flow.cyc:262 env.access_path==null";
struct _tagged_string _temp361; _temp361.curr= _temp360; _temp361.base= _temp360;
_temp361.last_plus_one= _temp360 + 47; _temp361;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp362; _temp362.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp364;
_temp364.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp364.assigns=( void*)
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()); _temp364;});
_temp362.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp363; _temp363.inner_exp=(
void*) Cyc_CfAbsexp_mkBottomAE(); _temp363.assigns=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp363;}); _temp362.lvalues= 0; _temp362;}); _LL116: goto _LL118; _LL118: goto
_LL120; _LL120: goto _LL122; return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),
0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL122:
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL124:
_temp227= _temp221; goto _LL126; _LL126: _temp233= _temp227; goto _LL128; _LL128: {
void* absop= Cyc_NewControlFlow_destruct_path( env, Cyc_CfAbsexp_mkLocalOp(
_temp233)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),( struct
Cyc_List_List*)({ struct Cyc_List_List* _temp365=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp365[ 0]=({ struct Cyc_List_List
_temp366; _temp366.hd=( void*) absop; _temp366.tl= 0; _temp366;}); _temp365;}),
Cyc_NewControlFlow_make_assigns( env, absop));} _LL130: _temp251= _temp241;
_temp247= _temp237; goto _LL132; _LL132: env.access_path == 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp367=( char*)"new_control_flow.cyc:292 env.access_path == null";
struct _tagged_string _temp368; _temp368.curr= _temp367; _temp368.base= _temp367;
_temp368.last_plus_one= _temp367 + 49; _temp368;})); if( _temp251 == 0){ return
Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} Cyc_NewControlFlow_add_enum_root( env, e,
_temp247);{ void* es_exp= Cyc_CfAbsexp_mkSkipAE(); void* useexp= Cyc_CfAbsexp_mkSkipAE();{
int i= 0; for( 0; _temp251 != 0; _temp251= _temp251->tl, ++ i){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_change_lhs( env,( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp369=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp369[ 0]=({ struct Cyc_List_List _temp370; _temp370.hd=( void*) Cyc_CfAbsexp_mkMemberOp(
Cyc_CfAbsexp_mkMallocOp( e),({ struct Cyc_CfFlowInfo_TupleF_struct* _temp371=(
struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc_atomic( sizeof( struct Cyc_CfFlowInfo_TupleF_struct));
_temp371->tag= Cyc_CfFlowInfo_TupleF_tag; _temp371->f1= i;( void*) _temp371;}));
_temp370.tl= 0; _temp370;}); _temp369;})); struct Cyc_NewControlFlow_AbsSyn syn=
Cyc_NewControlFlow_abstract_exp( elt_env,( struct Cyc_Absyn_Exp*) _temp251->hd);
void* _temp374; void* _temp376; struct Cyc_NewControlFlow_AbsSynOne _temp372=
Cyc_NewControlFlow_meet_absexp( syn); _LL377: _temp376=( void*) _temp372.inner_exp;
goto _LL375; _LL375: _temp374=( void*) _temp372.assigns; goto _LL373; _LL373:
es_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, _temp376, es_exp);
useexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(), _temp374, useexp);}}
return Cyc_NewControlFlow_malloc_and_init( env, e, es_exp, useexp);} _LL134: {
struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp( env,
_temp259); struct Cyc_List_List* lhs= syn1.lvalues == 0? Cyc_NewControlFlow_use_it_list():
syn1.lvalues; struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_change_lhs( env, lhs), _temp255); void* _temp380; void*
_temp382; struct Cyc_NewControlFlow_AbsSynOne _temp378= Cyc_NewControlFlow_meet_absexp(
syn1); _LL383: _temp382=( void*) _temp378.inner_exp; goto _LL381; _LL381:
_temp380=( void*) _temp378.assigns; goto _LL379; _LL379: { void* _temp386; void*
_temp388; struct Cyc_NewControlFlow_AbsSynOne _temp384= Cyc_NewControlFlow_meet_absexp(
syn2); _LL389: _temp388=( void*) _temp384.inner_exp; goto _LL387; _LL387:
_temp386=( void*) _temp384.assigns; goto _LL385; _LL385: { void* ansexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, _temp382,
_temp388), _temp386); return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, _temp380);}}}
_LL136: return Cyc_NewControlFlow_use_exp_unordered( env,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp390=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp390[ 0]=({ struct Cyc_List_List _temp391;
_temp391.hd=( void*) _temp265; _temp391.tl=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp392=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp392[ 0]=({ struct Cyc_List_List _temp393; _temp393.hd=( void*) _temp261;
_temp393.tl= 0; _temp393;}); _temp392;}); _temp391;}); _temp390;}), 0); _LL138:
return Cyc_NewControlFlow_use_exp_unordered( env, _temp267, 0); _LL140: { int
both_leaf= 1;{ void* _temp394=( void*)( _temp271->topt)->v; struct Cyc_Absyn_Structdecl**
_temp402; struct Cyc_List_List* _temp404; struct _tuple1* _temp406; struct Cyc_List_List*
_temp408; _LL396: if(( unsigned int) _temp394 > 5u?(( struct _enum_struct*)
_temp394)->tag == Cyc_Absyn_StructType_tag: 0){ _LL407: _temp406=( struct
_tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp394)->f1; goto _LL405;
_LL405: _temp404=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp394)->f2; goto _LL403; _LL403: _temp402=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp394)->f3; goto _LL397;} else{ goto
_LL398;} _LL398: if(( unsigned int) _temp394 > 5u?(( struct _enum_struct*)
_temp394)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL409: _temp408=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp394)->f1; goto _LL399;} else{ goto
_LL400;} _LL400: goto _LL401; _LL397: goto _LL399; _LL399: both_leaf= 0; goto
_LL395; _LL401: goto _LL395; _LL395:;}{ void* _temp410=( void*)( e->topt)->v;
struct Cyc_Absyn_Structdecl** _temp418; struct Cyc_List_List* _temp420; struct
_tuple1* _temp422; struct Cyc_List_List* _temp424; _LL412: if(( unsigned int)
_temp410 > 5u?(( struct _enum_struct*) _temp410)->tag == Cyc_Absyn_StructType_tag:
0){ _LL423: _temp422=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp410)->f1; goto _LL421; _LL421: _temp420=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp410)->f2; goto _LL419; _LL419: _temp418=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp410)->f3;
goto _LL413;} else{ goto _LL414;} _LL414: if(( unsigned int) _temp410 > 5u?((
struct _enum_struct*) _temp410)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL425:
_temp424=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp410)->f1;
goto _LL415;} else{ goto _LL416;} _LL416: goto _LL417; _LL413: goto _LL415;
_LL415: both_leaf= 0; goto _LL411; _LL417: goto _LL411; _LL411:;} if( both_leaf){
return Cyc_NewControlFlow_abstract_exp( env, _temp271);} return Cyc_NewControlFlow_use_exp_unordered(
env,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp426=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp426[ 0]=({ struct Cyc_List_List
_temp427; _temp427.hd=( void*) _temp271; _temp427.tl= 0; _temp427;}); _temp426;}),
env.lhsides);} _LL142: return Cyc_NewControlFlow_use_exp_unordered( env,( struct
Cyc_List_List*)({ struct Cyc_List_List* _temp428=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp428[ 0]=({ struct Cyc_List_List
_temp429; _temp429.hd=( void*) _temp277; _temp429.tl= _temp275; _temp429;});
_temp428;}), 0); _LL144: { void* _temp430= Cyc_Tcutil_compress(( void*)(
_temp281->topt)->v); struct Cyc_List_List* _temp436; _LL432: if(( unsigned int)
_temp430 > 5u?(( struct _enum_struct*) _temp430)->tag == Cyc_Absyn_TupleType_tag:
0){ _LL437: _temp436=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*)
_temp430)->f1; goto _LL433;} else{ goto _LL434;} _LL434: goto _LL435; _LL433:
env.access_path=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp438=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp438[
0]=({ struct Cyc_List_List _temp439; _temp439.hd=( void*) e; _temp439.tl= env.access_path;
_temp439;}); _temp438;}); return Cyc_NewControlFlow_abstract_exp( env, _temp281);
_LL435: return Cyc_NewControlFlow_use_exp_unordered( env,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp440=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp440[ 0]=({ struct Cyc_List_List _temp441;
_temp441.hd=( void*) _temp281; _temp441.tl=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp442=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp442[ 0]=({ struct Cyc_List_List _temp443; _temp443.hd=( void*) _temp279;
_temp443.tl= 0; _temp443;}); _temp442;}); _temp441;}); _temp440;}), Cyc_NewControlFlow_use_it_list());
_LL431:;} _LL146:{ void* _temp444=( void*) _temp283->r; struct Cyc_Absyn_Structdecl*
_temp452; struct Cyc_List_List* _temp454; struct Cyc_Core_Opt* _temp456; struct
_tuple1* _temp458; struct Cyc_List_List* _temp460; _LL446: if((( struct
_enum_struct*) _temp444)->tag == Cyc_Absyn_Struct_e_tag){ _LL459: _temp458=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp444)->f1; goto _LL457;
_LL457: _temp456=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp444)->f2; goto _LL455; _LL455: _temp454=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp444)->f3; goto _LL453; _LL453: _temp452=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp444)->f4;
goto _LL447;} else{ goto _LL448;} _LL448: if((( struct _enum_struct*) _temp444)->tag
== Cyc_Absyn_Tuple_e_tag){ _LL461: _temp460=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tuple_e_struct*) _temp444)->f1; goto _LL449;} else{ goto _LL450;}
_LL450: goto _LL451; _LL447: goto _LL449; _LL449: { void* malloc_op= Cyc_CfAbsexp_mkMallocOp(
e); Cyc_NewControlFlow_add_malloc_root( env, e,( void*)( _temp283->topt)->v);{
struct Cyc_NewControlFlow_AbsEnv inner_env= Cyc_NewControlFlow_change_lhs( env,(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp462=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp462[ 0]=({ struct Cyc_List_List
_temp463; _temp463.hd=( void*) malloc_op; _temp463.tl= 0; _temp463;}); _temp462;}));
inner_env.access_path= 0;{ void* _temp466; void* _temp468; struct Cyc_NewControlFlow_AbsSynOne
_temp464= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
inner_env, _temp283)); _LL469: _temp468=( void*) _temp464.inner_exp; goto _LL467;
_LL467: _temp466=( void*) _temp464.assigns; goto _LL465; _LL465: { struct Cyc_NewControlFlow_AbsSyn
ans= Cyc_NewControlFlow_malloc_and_init( env, e, _temp468, _temp466); if( env.access_path
!= 0){ void* _temp470=( void*)(( struct Cyc_Absyn_Exp*)( env.access_path)->hd)->r;
struct Cyc_Absyn_Exp* _temp480; struct Cyc_Absyn_Exp* _temp482; struct Cyc_Absyn_Exp*
_temp484; struct _tagged_string* _temp486; struct Cyc_Absyn_Exp* _temp488;
_LL472: if((( struct _enum_struct*) _temp470)->tag == Cyc_Absyn_Subscript_e_tag){
_LL483: _temp482=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp470)->f1; goto _LL481; _LL481: _temp480=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp470)->f2; goto _LL473;} else{ goto _LL474;}
_LL474: if((( struct _enum_struct*) _temp470)->tag == Cyc_Absyn_Deref_e_tag){
_LL485: _temp484=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp470)->f1; goto _LL475;} else{ goto _LL476;} _LL476: if((( struct
_enum_struct*) _temp470)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL489: _temp488=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp470)->f1;
goto _LL487; _LL487: _temp486=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp470)->f2; goto _LL477;} else{ goto _LL478;} _LL478: goto _LL479; _LL473:
Cyc_Evexp_eval_const_uint_exp( _temp480) ==( unsigned int) 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp490=( char*)"new_control_flow.cyc:402 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp491; _temp491.curr= _temp490; _temp491.base= _temp490;
_temp491.last_plus_one= _temp490 + 61; _temp491;})); goto _LL475; _LL475: env.access_path=(
env.access_path)->tl; ans.lvalues=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp492=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp492[ 0]=({ struct Cyc_List_List _temp493; _temp493.hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp493.tl= 0; _temp493;}); _temp492;}); goto _LL471; _LL477:
env.access_path=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp494=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp494[
0]=({ struct Cyc_List_List _temp495; _temp495.hd=( void*) Cyc_Absyn_structmember_exp(
_temp488, _temp486, 0); _temp495.tl=( env.access_path)->tl; _temp495;});
_temp494;}); ans.lvalues=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp496=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp496[ 0]=({ struct Cyc_List_List _temp497; _temp497.hd=( void*) Cyc_NewControlFlow_destruct_path(
env, malloc_op); _temp497.tl= 0; _temp497;}); _temp496;}); goto _LL471; _LL479:(
void) _throw(({ struct Cyc_Core_Impossible_struct* _temp498=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp498=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp499=( char*)"bad malloc access path";
struct _tagged_string _temp500; _temp500.curr= _temp499; _temp500.base= _temp499;
_temp500.last_plus_one= _temp499 + 23; _temp500;})};( struct _xenum_struct*)
_temp498;})); _LL471:;} return ans;}}}} _LL451: goto _LL445; _LL445:;}{ struct
Cyc_List_List* _temp501= env.access_path; struct Cyc_List_List _temp513; struct
Cyc_List_List* _temp514; struct Cyc_Absyn_Exp* _temp516; struct Cyc_Absyn_Exp
_temp518; struct Cyc_Position_Segment* _temp519; void* _temp521; struct Cyc_Absyn_Exp*
_temp523; struct Cyc_Absyn_Exp* _temp525; struct Cyc_Core_Opt* _temp527; struct
Cyc_List_List _temp529; struct Cyc_List_List* _temp530; struct Cyc_Absyn_Exp*
_temp532; struct Cyc_Absyn_Exp _temp534; struct Cyc_Position_Segment* _temp535;
void* _temp537; struct Cyc_Absyn_Exp* _temp539; struct Cyc_Core_Opt* _temp541;
struct Cyc_List_List _temp543; struct Cyc_List_List* _temp544; struct Cyc_Absyn_Exp*
_temp546; struct Cyc_Absyn_Exp _temp548; struct Cyc_Position_Segment* _temp549;
void* _temp551; struct _tagged_string* _temp553; struct Cyc_Absyn_Exp* _temp555;
struct Cyc_Core_Opt* _temp557; _LL503: if( _temp501 == 0){ goto _LL504;} else{
goto _LL505;} _LL505: if( _temp501 == 0){ goto _LL507;} else{ _temp513=*
_temp501; _LL517: _temp516=( struct Cyc_Absyn_Exp*) _temp513.hd; _temp518=*
_temp516; _LL528: _temp527=( struct Cyc_Core_Opt*) _temp518.topt; goto _LL522;
_LL522: _temp521=( void*) _temp518.r; if((( struct _enum_struct*) _temp521)->tag
== Cyc_Absyn_Subscript_e_tag){ _LL526: _temp525=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp521)->f1; goto _LL524; _LL524:
_temp523=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp521)->f2; goto _LL520;} else{ goto _LL507;} _LL520: _temp519=( struct Cyc_Position_Segment*)
_temp518.loc; goto _LL515; _LL515: _temp514=( struct Cyc_List_List*) _temp513.tl;
goto _LL506;} _LL507: if( _temp501 == 0){ goto _LL509;} else{ _temp529=*
_temp501; _LL533: _temp532=( struct Cyc_Absyn_Exp*) _temp529.hd; _temp534=*
_temp532; _LL542: _temp541=( struct Cyc_Core_Opt*) _temp534.topt; goto _LL538;
_LL538: _temp537=( void*) _temp534.r; if((( struct _enum_struct*) _temp537)->tag
== Cyc_Absyn_Deref_e_tag){ _LL540: _temp539=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Deref_e_struct*) _temp537)->f1; goto _LL536;} else{ goto _LL509;}
_LL536: _temp535=( struct Cyc_Position_Segment*) _temp534.loc; goto _LL531;
_LL531: _temp530=( struct Cyc_List_List*) _temp529.tl; goto _LL508;} _LL509: if(
_temp501 == 0){ goto _LL511;} else{ _temp543=* _temp501; _LL547: _temp546=(
struct Cyc_Absyn_Exp*) _temp543.hd; _temp548=* _temp546; _LL558: _temp557=(
struct Cyc_Core_Opt*) _temp548.topt; goto _LL552; _LL552: _temp551=( void*)
_temp548.r; if((( struct _enum_struct*) _temp551)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL556: _temp555=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp551)->f1; goto _LL554; _LL554: _temp553=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp551)->f2; goto _LL550;} else{ goto _LL511;}
_LL550: _temp549=( struct Cyc_Position_Segment*) _temp548.loc; goto _LL545;
_LL545: _temp544=( struct Cyc_List_List*) _temp543.tl; goto _LL510;} _LL511:
goto _LL512; _LL504: env.access_path=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp559=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp559[ 0]=({ struct Cyc_List_List _temp560; _temp560.hd=( void*) e; _temp560.tl=
env.access_path; _temp560;}); _temp559;}); goto _LL502; _LL506: Cyc_Evexp_eval_const_uint_exp(
_temp523) ==( unsigned int) 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)((
struct _tagged_string)({ char* _temp561=( char*)"new_control_flow.cyc:430 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp562; _temp562.curr= _temp561; _temp562.base= _temp561;
_temp562.last_plus_one= _temp561 + 61; _temp562;})); _temp530= _temp514; goto
_LL508; _LL508: env.access_path= _temp530; goto _LL502; _LL510: env.access_path=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp563=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp563[ 0]=({ struct Cyc_List_List
_temp564; _temp564.hd=( void*) Cyc_Absyn_structmember_exp( _temp555, _temp553, 0);
_temp564.tl= _temp544; _temp564;}); _temp563;}); goto _LL502; _LL512:( void)
_throw(({ struct Cyc_Core_Impossible_struct* _temp565=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp565=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp566=( char*)"abstract_exp: bad access path for &";
struct _tagged_string _temp567; _temp567.curr= _temp566; _temp567.base= _temp566;
_temp567.last_plus_one= _temp566 + 36; _temp567;})};( struct _xenum_struct*)
_temp565;})); _LL502:;} return Cyc_NewControlFlow_abstract_exp( env, _temp283);
_LL148:{ struct Cyc_List_List* _temp568= env.access_path; struct Cyc_List_List
_temp574; struct Cyc_List_List* _temp575; struct Cyc_Absyn_Exp* _temp577; struct
Cyc_Absyn_Exp _temp579; struct Cyc_Position_Segment* _temp580; void* _temp582;
struct Cyc_Absyn_Exp* _temp584; struct Cyc_Core_Opt* _temp586; _LL570: if(
_temp568 == 0){ goto _LL572;} else{ _temp574=* _temp568; _LL578: _temp577=(
struct Cyc_Absyn_Exp*) _temp574.hd; _temp579=* _temp577; _LL587: _temp586=(
struct Cyc_Core_Opt*) _temp579.topt; goto _LL583; _LL583: _temp582=( void*)
_temp579.r; if((( struct _enum_struct*) _temp582)->tag == Cyc_Absyn_Address_e_tag){
_LL585: _temp584=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp582)->f1; goto _LL581;} else{ goto _LL572;} _LL581: _temp580=( struct Cyc_Position_Segment*)
_temp579.loc; goto _LL576; _LL576: _temp575=( struct Cyc_List_List*) _temp574.tl;
goto _LL571;} _LL572: goto _LL573; _LL571: env.access_path= _temp575;{ struct
Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp( env, _temp285);
if( syn1.lvalues == 0){ syn1.lvalues= Cyc_NewControlFlow_use_it_list();} return
syn1;} _LL573: goto _LL569; _LL569:;} _temp289= _temp285; goto _LL150; _LL150:
_temp293= _temp289; goto _LL152; _LL152: env.access_path=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp588=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp588[ 0]=({ struct Cyc_List_List _temp589;
_temp589.hd=( void*) e; _temp589.tl= env.access_path; _temp589;}); _temp588;});{
struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp( env,
_temp293); if( syn1.lvalues == 0){ syn1.lvalues= Cyc_NewControlFlow_use_it_list();}
return syn1;} _LL154: { struct Cyc_List_List* _temp590= env.access_path; struct
Cyc_List_List _temp598; struct Cyc_List_List* _temp599; struct Cyc_Absyn_Exp*
_temp601; struct Cyc_Absyn_Exp _temp603; struct Cyc_Position_Segment* _temp604;
void* _temp606; struct Cyc_Absyn_Exp* _temp608; struct Cyc_Absyn_Exp* _temp610;
struct Cyc_Core_Opt* _temp612; _LL592: if( _temp590 == 0){ goto _LL593;} else{
goto _LL594;} _LL594: if( _temp590 == 0){ goto _LL596;} else{ _temp598=*
_temp590; _LL602: _temp601=( struct Cyc_Absyn_Exp*) _temp598.hd; _temp603=*
_temp601; _LL613: _temp612=( struct Cyc_Core_Opt*) _temp603.topt; goto _LL607;
_LL607: _temp606=( void*) _temp603.r; if((( struct _enum_struct*) _temp606)->tag
== Cyc_Absyn_Subscript_e_tag){ _LL611: _temp610=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp606)->f1; goto _LL609; _LL609:
_temp608=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp606)->f2; goto _LL605;} else{ goto _LL596;} _LL605: _temp604=( struct Cyc_Position_Segment*)
_temp603.loc; goto _LL600; _LL600: _temp599=( struct Cyc_List_List*) _temp598.tl;
goto _LL595;} _LL596: goto _LL597; _LL593: { void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp295 != 0;
_temp295= _temp295->tl, ++ j){ struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List*
ls= env.lhsides; for( 0; ls != 0; ls= ls->tl){ if( Cyc_CfAbsexp_isUnknownOp((
void*) ls->hd)){ ith_lvals=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp614=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp614[ 0]=({ struct Cyc_List_List _temp615; _temp615.hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp615.tl= ith_lvals; _temp615;}); _temp614;});} else{ ith_lvals=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp616=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp616[ 0]=({ struct Cyc_List_List _temp617;
_temp617.hd=( void*) Cyc_CfAbsexp_mkMemberOp(( void*) ls->hd,({ struct Cyc_CfFlowInfo_TupleF_struct*
_temp618=( struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc_atomic( sizeof(
struct Cyc_CfFlowInfo_TupleF_struct)); _temp618->tag= Cyc_CfFlowInfo_TupleF_tag;
_temp618->f1= j;( void*) _temp618;})); _temp617.tl= ith_lvals; _temp617;});
_temp616;});}}}{ struct Cyc_NewControlFlow_AbsSyn ith_syn= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_change_lhs( env, ith_lvals),( struct Cyc_Absyn_Exp*) _temp295->hd);
void* _temp621; void* _temp623; struct Cyc_NewControlFlow_AbsSynOne _temp619=
Cyc_NewControlFlow_meet_absexp( ith_syn); _LL624: _temp623=( void*) _temp619.inner_exp;
goto _LL622; _LL622: _temp621=( void*) _temp619.assigns; goto _LL620; _LL620:
ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, ansexp, _temp623);
assignexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(), assignexp,
_temp621);}}} return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, assignexp);} _LL595: {
int i=( int) Cyc_Evexp_eval_const_uint_exp( _temp608); struct Cyc_NewControlFlow_AbsEnv
other_env= Cyc_NewControlFlow_change_lhs( env, 0); other_env.access_path= 0; env.access_path=
_temp599;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE(); void* assignexp= Cyc_CfAbsexp_mkSkipAE();{
int j= 0; for( 0; _temp295 != 0; _temp295= _temp295->tl, ++ j){ if( i == j){
void* _temp627; void* _temp629; struct Cyc_NewControlFlow_AbsSynOne _temp625=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,( struct
Cyc_Absyn_Exp*) _temp295->hd)); _LL630: _temp629=( void*) _temp625.inner_exp;
goto _LL628; _LL628: _temp627=( void*) _temp625.assigns; goto _LL626; _LL626:
assignexp= _temp627; ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp629);} else{ void* _temp633; void* _temp635; struct Cyc_NewControlFlow_AbsSynOne
_temp631= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
other_env,( struct Cyc_Absyn_Exp*) _temp295->hd)); _LL636: _temp635=( void*)
_temp631.inner_exp; goto _LL634; _LL634: _temp633=( void*) _temp631.assigns;
goto _LL632; _LL632: ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp635);}}} return Cyc_NewControlFlow_mkSyn_tf( ansexp,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp637=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp637[ 0]=({ struct Cyc_List_List _temp638;
_temp638.hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp638.tl= 0; _temp638;});
_temp637;}), assignexp);}} _LL597:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp639=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp639=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp640=( char*)"abstract_exp: unexpected access path to Tuple";
struct _tagged_string _temp641; _temp641.curr= _temp640; _temp641.base= _temp640;
_temp641.last_plus_one= _temp640 + 46; _temp641;})};( struct _xenum_struct*)
_temp639;})); _LL591:;} _LL156: { struct Cyc_List_List* _temp642= env.access_path;
struct Cyc_List_List _temp650; struct Cyc_List_List* _temp651; struct Cyc_Absyn_Exp*
_temp653; struct Cyc_Absyn_Exp _temp655; struct Cyc_Position_Segment* _temp656;
void* _temp658; struct _tagged_string* _temp660; struct Cyc_Absyn_Exp* _temp662;
struct Cyc_Core_Opt* _temp664; _LL644: if( _temp642 == 0){ goto _LL645;} else{
goto _LL646;} _LL646: if( _temp642 == 0){ goto _LL648;} else{ _temp650=*
_temp642; _LL654: _temp653=( struct Cyc_Absyn_Exp*) _temp650.hd; _temp655=*
_temp653; _LL665: _temp664=( struct Cyc_Core_Opt*) _temp655.topt; goto _LL659;
_LL659: _temp658=( void*) _temp655.r; if((( struct _enum_struct*) _temp658)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL663: _temp662=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp658)->f1; goto _LL661; _LL661:
_temp660=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp658)->f2; goto _LL657;} else{ goto _LL648;} _LL657: _temp656=( struct Cyc_Position_Segment*)
_temp655.loc; goto _LL652; _LL652: _temp651=( struct Cyc_List_List*) _temp650.tl;
goto _LL647;} _LL648: goto _LL649; _LL645: { void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp299 != 0; _temp299=
_temp299->tl){ struct Cyc_List_List* ith_lvals= 0;{ struct Cyc_List_List* ls=
env.lhsides; for( 0; ls != 0; ls= ls->tl){ if( Cyc_CfAbsexp_isUnknownOp(( void*)
ls->hd)){ ith_lvals=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp666=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp666[
0]=({ struct Cyc_List_List _temp667; _temp667.hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp667.tl= ith_lvals; _temp667;}); _temp666;});} else{ struct Cyc_List_List*
ds=(*(( struct _tuple5*) _temp299->hd)).f1; for( 0; ds != 0; ds= ds->tl){ void*
_temp668=( void*) ds->hd; struct Cyc_Absyn_Exp* _temp674; struct _tagged_string*
_temp676; _LL670: if((( struct _enum_struct*) _temp668)->tag == Cyc_Absyn_ArrayElement_tag){
_LL675: _temp674=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp668)->f1; goto _LL671;} else{ goto _LL672;} _LL672: if((( struct
_enum_struct*) _temp668)->tag == Cyc_Absyn_FieldName_tag){ _LL677: _temp676=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp668)->f1;
goto _LL673;} else{ goto _LL669;} _LL671:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp678=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp678=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp679=( char*)"bad struct designator"; struct
_tagged_string _temp680; _temp680.curr= _temp679; _temp680.base= _temp679;
_temp680.last_plus_one= _temp679 + 22; _temp680;})};( struct _xenum_struct*)
_temp678;})); _LL673: ith_lvals=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp681=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp681[ 0]=({ struct Cyc_List_List _temp682; _temp682.hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*) ls->hd,({ struct Cyc_CfFlowInfo_StructF_struct* _temp683=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp683->tag= Cyc_CfFlowInfo_StructF_tag;
_temp683->f1= _temp676;( void*) _temp683;})); _temp682.tl= ith_lvals; _temp682;});
_temp681;}); goto _LL669; _LL669:;}}}}{ void** ith_topt=( void*)&(((*(( struct
_tuple5*) _temp299->hd)).f2)->topt)->v; struct Cyc_NewControlFlow_AbsSyn ith_syn=
Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env, ith_lvals),(*((
struct _tuple5*) _temp299->hd)).f2); void* _temp686; void* _temp688; struct Cyc_NewControlFlow_AbsSynOne
_temp684= Cyc_NewControlFlow_meet_absexp( ith_syn); _LL689: _temp688=( void*)
_temp684.inner_exp; goto _LL687; _LL687: _temp686=( void*) _temp684.assigns;
goto _LL685; _LL685: ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp688); assignexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_mkAnyOrderG(),
assignexp, _temp686);}} return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, assignexp);}
_LL647: { struct Cyc_NewControlFlow_AbsEnv other_env= Cyc_NewControlFlow_change_lhs(
env, 0); other_env.access_path= 0; env.access_path= _temp651;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE();
void* assignexp= Cyc_CfAbsexp_mkSkipAE(); for( 0; _temp299 != 0; _temp299=
_temp299->tl){ int used= 0;{ struct Cyc_List_List* ds=(*(( struct _tuple5*)
_temp299->hd)).f1; for( 0; ds != 0; ds= ds->tl){ void* _temp690=( void*) ds->hd;
struct Cyc_Absyn_Exp* _temp696; struct _tagged_string* _temp698; _LL692: if(((
struct _enum_struct*) _temp690)->tag == Cyc_Absyn_ArrayElement_tag){ _LL697:
_temp696=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp690)->f1; goto _LL693;} else{ goto _LL694;} _LL694: if((( struct
_enum_struct*) _temp690)->tag == Cyc_Absyn_FieldName_tag){ _LL699: _temp698=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp690)->f1;
goto _LL695;} else{ goto _LL691;} _LL693:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp700=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp700=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp701=( char*)"bad struct designator"; struct
_tagged_string _temp702; _temp702.curr= _temp701; _temp702.base= _temp701;
_temp702.last_plus_one= _temp701 + 22; _temp702;})};( struct _xenum_struct*)
_temp700;})); _LL695: if( Cyc_String_zstrptrcmp( _temp660, _temp698) == 0){ used=
1;} goto _LL691; _LL691:;}} if( used){ void* _temp705; void* _temp707; struct
Cyc_NewControlFlow_AbsSynOne _temp703= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
env,(*(( struct _tuple5*) _temp299->hd)).f2)); _LL708: _temp707=( void*)
_temp703.inner_exp; goto _LL706; _LL706: _temp705=( void*) _temp703.assigns;
goto _LL704; _LL704: assignexp= _temp705; ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp707);} else{ void* _temp711; void* _temp713; struct Cyc_NewControlFlow_AbsSynOne
_temp709= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
other_env,(*(( struct _tuple5*) _temp299->hd)).f2)); _LL714: _temp713=( void*)
_temp709.inner_exp; goto _LL712; _LL712: _temp711=( void*) _temp709.assigns;
goto _LL710; _LL710: ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
ansexp, _temp713);}} return Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, assignexp);}}
_LL649:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp715=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp715=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp716=( char*)"abstract_exp: unexpected access path to Struct";
struct _tagged_string _temp717; _temp717.curr= _temp716; _temp717.base= _temp716;
_temp717.last_plus_one= _temp716 + 47; _temp717;})};( struct _xenum_struct*)
_temp715;})); _LL643:;} _LL158: return Cyc_NewControlFlow_use_exp_unordered( env,((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple5*), struct
Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct _tuple5*))
Cyc_Core_snd, _temp305), 0); _LL160: return Cyc_NewControlFlow_use_exp_unordered(
env,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp718=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp718[ 0]=({ struct Cyc_List_List
_temp719; _temp719.hd=( void*) _temp311; _temp719.tl= 0; _temp719;}); _temp718;}),
0); _LL162: { struct Cyc_NewControlFlow_AbsSyn esyn= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp313); return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp(
esyn)).inner_exp, Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());}
_LL164: { struct Cyc_List_List* _temp722; struct Cyc_NewControlFlow_AbsSynOne
_temp724; struct Cyc_NewControlFlow_AbsSynOne _temp726; struct Cyc_NewControlFlow_AbsSyn
_temp720= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp319); _LL727: _temp726=( struct Cyc_NewControlFlow_AbsSynOne) _temp720.when_true;
goto _LL725; _LL725: _temp724=( struct Cyc_NewControlFlow_AbsSynOne) _temp720.when_false;
goto _LL723; _LL723: _temp722=( struct Cyc_List_List*) _temp720.lvalues; goto
_LL721; _LL721: { struct Cyc_List_List* _temp730; struct Cyc_NewControlFlow_AbsSynOne
_temp732; struct Cyc_NewControlFlow_AbsSynOne _temp734; struct Cyc_NewControlFlow_AbsSyn
_temp728= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp317); _LL735: _temp734=( struct Cyc_NewControlFlow_AbsSynOne) _temp728.when_true;
goto _LL733; _LL733: _temp732=( struct Cyc_NewControlFlow_AbsSynOne) _temp728.when_false;
goto _LL731; _LL731: _temp730=( struct Cyc_List_List*) _temp728.lvalues; goto
_LL729; _LL729: { struct Cyc_List_List* _temp738; struct Cyc_NewControlFlow_AbsSynOne
_temp740; struct Cyc_NewControlFlow_AbsSynOne _temp742; struct Cyc_NewControlFlow_AbsSyn
_temp736= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp315); _LL743: _temp742=( struct Cyc_NewControlFlow_AbsSynOne) _temp736.when_true;
goto _LL741; _LL741: _temp740=( struct Cyc_NewControlFlow_AbsSynOne) _temp736.when_false;
goto _LL739; _LL739: _temp738=( struct Cyc_List_List*) _temp736.lvalues; goto
_LL737; _LL737: { void* true_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp726.inner_exp,( void*) _temp726.assigns), Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG,( void*) _temp734.inner_exp,( void*) _temp734.assigns)),
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp724.inner_exp,( void*) _temp724.assigns), Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG,( void*) _temp742.inner_exp,( void*) _temp742.assigns)));
void* false_exp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)
_temp726.inner_exp,( void*) _temp726.assigns), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp732.inner_exp,( void*) _temp732.assigns)), Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)
_temp724.inner_exp,( void*) _temp724.assigns), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) _temp740.inner_exp,( void*) _temp740.assigns))); return({ struct Cyc_NewControlFlow_AbsSyn
_temp744; _temp744.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp746;
_temp746.inner_exp=( void*) true_exp; _temp746.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp746;}); _temp744.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp745; _temp745.inner_exp=( void*) false_exp; _temp745.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp745;}); _temp744.lvalues= 0; _temp744;});}}}}
_LL166: { struct Cyc_NewControlFlow_AbsEnv env2= Cyc_NewControlFlow_change_lhs(
env, 0); env2.access_path= 0; Cyc_NewControlFlow_cf_prepass( env2, _temp321);{
struct Cyc_Absyn_Stmt* last_s= _temp321; while( 1) { void* _temp747=( void*)
last_s->r; struct Cyc_Absyn_Stmt* _temp757; struct Cyc_Absyn_Stmt* _temp759;
struct Cyc_Absyn_Stmt* _temp761; struct Cyc_Absyn_Decl* _temp763; struct Cyc_Absyn_Exp*
_temp765; _LL749: if(( unsigned int) _temp747 > 1u?(( struct _enum_struct*)
_temp747)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL760: _temp759=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp747)->f1; goto _LL758; _LL758: _temp757=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp747)->f2; goto
_LL750;} else{ goto _LL751;} _LL751: if(( unsigned int) _temp747 > 1u?(( struct
_enum_struct*) _temp747)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL764: _temp763=(
struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*) _temp747)->f1; goto
_LL762; _LL762: _temp761=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp747)->f2; goto _LL752;} else{ goto _LL753;} _LL753: if(( unsigned int)
_temp747 > 1u?(( struct _enum_struct*) _temp747)->tag == Cyc_Absyn_Exp_s_tag: 0){
_LL766: _temp765=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp747)->f1; goto _LL754;} else{ goto _LL755;} _LL755: goto _LL756; _LL750:
last_s= _temp757; continue; _LL752: last_s= _temp761; continue; _LL754: { struct
Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp765); void* _temp769; void* _temp771; struct Cyc_NewControlFlow_AbsSynOne
_temp767= Cyc_NewControlFlow_meet_absexp( syn); _LL772: _temp771=( void*)
_temp767.inner_exp; goto _LL770; _LL770: _temp769=( void*) _temp767.assigns;
goto _LL768; _LL768:( void*)((( Cyc_NewControlFlow_get_stmt_annot( last_s))->absexps)->hd=(
void*) Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, _temp771, _temp769));
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE( _temp321), 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} _LL756:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp773=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp773=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp774=( char*)"abstract_exp: ill-formed StmtExp";
struct _tagged_string _temp775; _temp775.curr= _temp774; _temp775.base= _temp774;
_temp775.last_plus_one= _temp774 + 33; _temp775;})};( struct _xenum_struct*)
_temp773;})); _LL748:;}}} _LL168: { struct Cyc_NewControlFlow_AbsEnv env1= Cyc_NewControlFlow_change_lhs(
env, 0); env1.access_path= 0;{ struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env1, _temp325); struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
env, _temp323); struct Cyc_NewControlFlow_AbsSynOne syn1meet= Cyc_NewControlFlow_meet_absexp(
syn1); return({ struct Cyc_NewControlFlow_AbsSyn _temp776; _temp776.when_true=({
struct Cyc_NewControlFlow_AbsSynOne _temp778; _temp778.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,( void*)( syn2.when_true).inner_exp);
_temp778.assigns=( void*)(( void*)( syn2.when_true).assigns); _temp778;});
_temp776.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp777; _temp777.inner_exp=(
void*) Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*) syn1meet.inner_exp,(
void*)( syn2.when_false).inner_exp); _temp777.assigns=( void*)(( void*)( syn2.when_false).assigns);
_temp777;}); _temp776.lvalues= 0; _temp776;});}} _LL170: env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp779=( char*)"new_control_flow.cyc:676 env.access_path==null";
struct _tagged_string _temp780; _temp780.curr= _temp779; _temp780.base= _temp779;
_temp780.last_plus_one= _temp779 + 47; _temp780;})); Cyc_NewControlFlow_add_var_root(
env, _temp331);( env.shared)->comprehension_vars=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp781=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp781[ 0]=({ struct Cyc_List_List _temp782;
_temp782.hd=( void*) _temp331; _temp782.tl=( env.shared)->comprehension_vars;
_temp782;}); _temp781;});{ struct Cyc_NewControlFlow_AbsEnv elt_env= Cyc_NewControlFlow_use_it_env(
env); void* _temp785; void* _temp787; struct Cyc_NewControlFlow_AbsSynOne
_temp783= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
elt_env, _temp329)); _LL788: _temp787=( void*) _temp783.inner_exp; goto _LL786;
_LL786: _temp785=( void*) _temp783.assigns; goto _LL784; _LL784: { void*
_temp791; void* _temp793; struct Cyc_NewControlFlow_AbsSynOne _temp789= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( elt_env, _temp327)); _LL794: _temp793=( void*)
_temp789.inner_exp; goto _LL792; _LL792: _temp791=( void*) _temp789.assigns;
goto _LL790; _LL790: { void* ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,
_temp787, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, _temp785, Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp( _temp331),
Cyc_CfAbsexp_mkUnknownOp()), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, _temp793, _temp791), Cyc_CfAbsexp_mkSkipAE())))); return
Cyc_NewControlFlow_mkSyn_tf( ansexp, 0, Cyc_NewControlFlow_make_assigns( env,
Cyc_CfAbsexp_mkUnknownOp()));}}} _LL172: goto _LL174; _LL174: goto _LL176;
_LL176: goto _LL178; _LL178: goto _LL180; _LL180: goto _LL182; _LL182: goto
_LL184; _LL184:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp795=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp795=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp796=( char*)"abstract_exp, unexpected exp form";
struct _tagged_string _temp797; _temp797.curr= _temp796; _temp797.base= _temp796;
_temp797.last_plus_one= _temp796 + 34; _temp797;})};( struct _xenum_struct*)
_temp795;})); _LL106:;} void* Cyc_NewControlFlow_abstract_exp_top( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ void* _temp800; void* _temp802; struct Cyc_NewControlFlow_AbsSynOne
_temp798= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,
e)); _LL803: _temp802=( void*) _temp798.inner_exp; goto _LL801; _LL801: _temp800=(
void*) _temp798.assigns; goto _LL799; _LL799: return Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,
_temp802, _temp800);} struct _tuple0 Cyc_NewControlFlow_abstract_guard( struct
Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
syn= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env), e);
return({ struct _tuple0 _temp804; _temp804.f1= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*)( syn.when_true).inner_exp,( void*)( syn.when_true).assigns); _temp804.f2=
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)( syn.when_false).inner_exp,(
void*)( syn.when_false).assigns); _temp804;});} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
ans= 0; for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause _temp807;
struct Cyc_Position_Segment* _temp808; struct Cyc_Absyn_Stmt* _temp810; struct
Cyc_Absyn_Exp* _temp812; struct Cyc_Core_Opt* _temp814; struct Cyc_Absyn_Pat*
_temp816; struct Cyc_Absyn_Switch_clause* _temp805=( struct Cyc_Absyn_Switch_clause*)
scs->hd; _temp807=* _temp805; _LL817: _temp816=( struct Cyc_Absyn_Pat*) _temp807.pattern;
goto _LL815; _LL815: _temp814=( struct Cyc_Core_Opt*) _temp807.pat_vars; goto
_LL813; _LL813: _temp812=( struct Cyc_Absyn_Exp*) _temp807.where_clause; goto
_LL811; _LL811: _temp810=( struct Cyc_Absyn_Stmt*) _temp807.body; goto _LL809;
_LL809: _temp808=( struct Cyc_Position_Segment*) _temp807.loc; goto _LL806;
_LL806: if( _temp814 == 0){( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp818=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp818=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp819=( char*)"switch clause vds not set";
struct _tagged_string _temp820; _temp820.curr= _temp819; _temp820.base= _temp819;
_temp820.last_plus_one= _temp819 + 26; _temp820;})};( struct _xenum_struct*)
_temp818;}));}{ struct Cyc_List_List* vds=( struct Cyc_List_List*) _temp814->v;
for( 0; vds != 0; vds= vds->tl){ Cyc_NewControlFlow_add_var_root( env,( struct
Cyc_Absyn_Vardecl*) vds->hd);}} if( _temp812 != 0){ void* _temp824; void*
_temp826; struct _tuple0 _temp822= Cyc_NewControlFlow_abstract_guard( env,(
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp821= _temp812; if( _temp821
== 0){ _throw( Null_Exception);} _temp821;})); _LL827: _temp826= _temp822.f1;
goto _LL825; _LL825: _temp824= _temp822.f2; goto _LL823; _LL823: ans=( struct
Cyc_List_List*)({ struct Cyc_List_List* _temp828=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp828[ 0]=({ struct Cyc_List_List
_temp829; _temp829.hd=( void*) _temp826; _temp829.tl=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp830=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp830[ 0]=({ struct Cyc_List_List _temp831;
_temp831.hd=( void*) _temp824; _temp831.tl= ans; _temp831;}); _temp830;});
_temp829;}); _temp828;});} Cyc_NewControlFlow_cf_prepass( env, _temp810);}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
ans);} void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_Absyn_Stmt* s){ env.lhsides == 0? 0:(( int(*)( struct _tagged_string
msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char* _temp832=( char*)"new_control_flow.cyc:728 env.lhsides == null";
struct _tagged_string _temp833; _temp833.curr= _temp832; _temp833.base= _temp832;
_temp833.last_plus_one= _temp832 + 45; _temp833;})); env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp834=( char*)"new_control_flow.cyc:729 env.access_path == null";
struct _tagged_string _temp835; _temp835.curr= _temp834; _temp835.base= _temp834;
_temp835.last_plus_one= _temp834 + 49; _temp835;}));{ void* _temp836=( void*) s->r;
struct Cyc_Absyn_Exp* _temp882; struct Cyc_Absyn_Stmt* _temp884; struct Cyc_Absyn_Stmt*
_temp886; struct Cyc_Absyn_Exp* _temp888; struct Cyc_Absyn_Exp* _temp890; struct
Cyc_Absyn_Stmt* _temp892; struct Cyc_Absyn_Stmt* _temp894; struct Cyc_Absyn_Exp*
_temp896; struct _tuple3 _temp898; struct Cyc_Absyn_Stmt* _temp900; struct Cyc_Absyn_Exp*
_temp902; struct Cyc_Absyn_Stmt* _temp904; struct Cyc_Absyn_Stmt* _temp906;
struct _tuple3 _temp908; struct Cyc_Absyn_Stmt* _temp910; struct Cyc_Absyn_Exp*
_temp912; struct Cyc_Absyn_Stmt* _temp914; struct Cyc_Absyn_Stmt* _temp916;
struct Cyc_Absyn_Stmt* _temp918; struct _tagged_string* _temp920; struct Cyc_Absyn_Stmt*
_temp922; struct _tagged_string* _temp924; struct Cyc_Absyn_Stmt* _temp926;
struct _tuple3 _temp928; struct Cyc_Absyn_Stmt* _temp930; struct Cyc_Absyn_Exp*
_temp932; struct _tuple3 _temp934; struct Cyc_Absyn_Stmt* _temp936; struct Cyc_Absyn_Exp*
_temp938; struct Cyc_Absyn_Exp* _temp940; struct Cyc_List_List* _temp942; struct
Cyc_Absyn_Exp* _temp944; struct Cyc_Absyn_Switch_clause** _temp946; struct Cyc_List_List*
_temp948; struct Cyc_Absyn_Stmt* _temp950; struct Cyc_Absyn_Decl* _temp952;
struct Cyc_Absyn_Decl _temp954; struct Cyc_Position_Segment* _temp955; void*
_temp957; struct Cyc_Absyn_Vardecl* _temp959; struct Cyc_Absyn_Stmt* _temp961;
struct Cyc_Absyn_Decl* _temp963; struct Cyc_Absyn_Decl _temp965; struct Cyc_Position_Segment*
_temp966; void* _temp968; int _temp970; struct Cyc_Absyn_Exp* _temp972; struct
Cyc_Core_Opt* _temp974; struct Cyc_Core_Opt* _temp976; struct Cyc_Core_Opt
_temp978; struct Cyc_List_List* _temp979; struct Cyc_Absyn_Pat* _temp981; struct
Cyc_Absyn_Stmt* _temp983; struct Cyc_Absyn_Decl* _temp985; struct Cyc_Absyn_Stmt*
_temp987; struct Cyc_Absyn_Stmt* _temp989; struct Cyc_Absyn_Stmt* _temp991;
struct _tagged_string* _temp993; struct Cyc_List_List* _temp995; struct Cyc_Absyn_Stmt*
_temp997; _LL838: if( _temp836 == Cyc_Absyn_Skip_s){ goto _LL839;} else{ goto
_LL840;} _LL840: if(( unsigned int) _temp836 > 1u?(( struct _enum_struct*)
_temp836)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL883: _temp882=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp_s_struct*) _temp836)->f1; goto _LL841;} else{ goto _LL842;}
_LL842: if(( unsigned int) _temp836 > 1u?(( struct _enum_struct*) _temp836)->tag
== Cyc_Absyn_Seq_s_tag: 0){ _LL887: _temp886=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Seq_s_struct*) _temp836)->f1; goto _LL885; _LL885: _temp884=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp836)->f2; goto _LL843;}
else{ goto _LL844;} _LL844: if(( unsigned int) _temp836 > 1u?(( struct
_enum_struct*) _temp836)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL889: _temp888=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*) _temp836)->f1; if(
_temp888 == 0){ goto _LL845;} else{ goto _LL846;}} else{ goto _LL846;} _LL846:
if(( unsigned int) _temp836 > 1u?(( struct _enum_struct*) _temp836)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL891: _temp890=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp836)->f1; goto _LL847;} else{ goto _LL848;} _LL848: if(( unsigned int)
_temp836 > 1u?(( struct _enum_struct*) _temp836)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL897: _temp896=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp836)->f1; goto _LL895; _LL895: _temp894=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp836)->f2; goto _LL893; _LL893: _temp892=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp836)->f3;
goto _LL849;} else{ goto _LL850;} _LL850: if(( unsigned int) _temp836 > 1u?((
struct _enum_struct*) _temp836)->tag == Cyc_Absyn_Do_s_tag: 0){ _LL905: _temp904=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*) _temp836)->f1; goto
_LL899; _LL899: _temp898=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp836)->f2; _LL903: _temp902= _temp898.f1; goto _LL901; _LL901: _temp900=
_temp898.f2; goto _LL851;} else{ goto _LL852;} _LL852: if(( unsigned int)
_temp836 > 1u?(( struct _enum_struct*) _temp836)->tag == Cyc_Absyn_While_s_tag:
0){ _LL909: _temp908=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp836)->f1; _LL913: _temp912= _temp908.f1; goto _LL911; _LL911: _temp910=
_temp908.f2; goto _LL907; _LL907: _temp906=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_While_s_struct*) _temp836)->f2; goto _LL853;} else{ goto _LL854;}
_LL854: if(( unsigned int) _temp836 > 1u?(( struct _enum_struct*) _temp836)->tag
== Cyc_Absyn_Break_s_tag: 0){ _LL915: _temp914=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp836)->f1; goto _LL855;} else{ goto _LL856;}
_LL856: if(( unsigned int) _temp836 > 1u?(( struct _enum_struct*) _temp836)->tag
== Cyc_Absyn_Continue_s_tag: 0){ _LL917: _temp916=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp836)->f1; goto _LL857;} else{ goto
_LL858;} _LL858: if(( unsigned int) _temp836 > 1u?(( struct _enum_struct*)
_temp836)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL921: _temp920=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp836)->f1; goto _LL919;
_LL919: _temp918=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp836)->f2; if( _temp918 == 0){ goto _LL859;} else{ goto _LL860;}} else{ goto
_LL860;} _LL860: if(( unsigned int) _temp836 > 1u?(( struct _enum_struct*)
_temp836)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL925: _temp924=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp836)->f1; goto _LL923;
_LL923: _temp922=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp836)->f2; goto _LL861;} else{ goto _LL862;} _LL862: if(( unsigned int)
_temp836 > 1u?(( struct _enum_struct*) _temp836)->tag == Cyc_Absyn_For_s_tag: 0){
_LL941: _temp940=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp836)->f1; goto _LL935; _LL935: _temp934=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp836)->f2; _LL939: _temp938= _temp934.f1; goto _LL937; _LL937: _temp936=
_temp934.f2; goto _LL929; _LL929: _temp928=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp836)->f3; _LL933: _temp932= _temp928.f1; goto _LL931; _LL931: _temp930=
_temp928.f2; goto _LL927; _LL927: _temp926=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_For_s_struct*) _temp836)->f4; goto _LL863;} else{ goto _LL864;} _LL864:
if(( unsigned int) _temp836 > 1u?(( struct _enum_struct*) _temp836)->tag == Cyc_Absyn_Switch_s_tag:
0){ _LL945: _temp944=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp836)->f1; goto _LL943; _LL943: _temp942=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Switch_s_struct*) _temp836)->f2; goto _LL865;} else{ goto _LL866;}
_LL866: if(( unsigned int) _temp836 > 1u?(( struct _enum_struct*) _temp836)->tag
== Cyc_Absyn_Fallthru_s_tag: 0){ _LL949: _temp948=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp836)->f1; goto _LL947; _LL947:
_temp946=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp836)->f2; goto _LL867;} else{ goto _LL868;} _LL868: if(( unsigned int)
_temp836 > 1u?(( struct _enum_struct*) _temp836)->tag == Cyc_Absyn_Decl_s_tag: 0){
_LL953: _temp952=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp836)->f1; _temp954=* _temp952; _LL958: _temp957=( void*) _temp954.r; if((
unsigned int) _temp957 > 1u?(( struct _enum_struct*) _temp957)->tag == Cyc_Absyn_Var_d_tag:
0){ _LL960: _temp959=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp957)->f1; goto _LL956;} else{ goto _LL870;} _LL956: _temp955=( struct Cyc_Position_Segment*)
_temp954.loc; goto _LL951; _LL951: _temp950=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp836)->f2; goto _LL869;} else{ goto _LL870;}
_LL870: if(( unsigned int) _temp836 > 1u?(( struct _enum_struct*) _temp836)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL964: _temp963=( struct Cyc_Absyn_Decl*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp836)->f1; _temp965=* _temp963; _LL969: _temp968=(
void*) _temp965.r; if(( unsigned int) _temp968 > 1u?(( struct _enum_struct*)
_temp968)->tag == Cyc_Absyn_Let_d_tag: 0){ _LL982: _temp981=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp968)->f1; goto _LL977; _LL977: _temp976=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp968)->f2; if(
_temp976 == 0){ goto _LL872;} else{ _temp978=* _temp976; _LL980: _temp979=(
struct Cyc_List_List*) _temp978.v; goto _LL975;} _LL975: _temp974=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp968)->f3; goto _LL973; _LL973: _temp972=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp968)->f4; goto
_LL971; _LL971: _temp970=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp968)->f5;
goto _LL967;} else{ goto _LL872;} _LL967: _temp966=( struct Cyc_Position_Segment*)
_temp965.loc; goto _LL962; _LL962: _temp961=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp836)->f2; goto _LL871;} else{ goto _LL872;}
_LL872: if(( unsigned int) _temp836 > 1u?(( struct _enum_struct*) _temp836)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL986: _temp985=( struct Cyc_Absyn_Decl*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp836)->f1; goto _LL984; _LL984: _temp983=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp836)->f2; goto _LL873;}
else{ goto _LL874;} _LL874: if(( unsigned int) _temp836 > 1u?(( struct
_enum_struct*) _temp836)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL988: _temp987=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*) _temp836)->f1; goto
_LL875;} else{ goto _LL876;} _LL876: if(( unsigned int) _temp836 > 1u?(( struct
_enum_struct*) _temp836)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL990: _temp989=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*) _temp836)->f1; goto
_LL877;} else{ goto _LL878;} _LL878: if(( unsigned int) _temp836 > 1u?(( struct
_enum_struct*) _temp836)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL994: _temp993=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp836)->f1; goto
_LL992; _LL992: _temp991=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp836)->f2; goto _LL879;} else{ goto _LL880;} _LL880: if(( unsigned int)
_temp836 > 1u?(( struct _enum_struct*) _temp836)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL998: _temp997=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp836)->f1; goto _LL996; _LL996: _temp995=( struct Cyc_List_List*)(( struct
Cyc_Absyn_TryCatch_s_struct*) _temp836)->f2; goto _LL881;} else{ goto _LL837;}
_LL839: return; _LL841: { void* ae= Cyc_NewControlFlow_abstract_exp_top( env,
_temp882); Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp999=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp999[ 0]=({ struct Cyc_List_List _temp1000;
_temp1000.hd=( void*) ae; _temp1000.tl= 0; _temp1000;}); _temp999;})); return;}
_LL843: Cyc_NewControlFlow_cf_prepass( env, _temp886); Cyc_NewControlFlow_cf_prepass(
env, _temp884); return; _LL845: return; _LL847: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1001= _temp890; if( _temp1001 == 0){ _throw( Null_Exception);} _temp1001;}));
Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1002=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1002[ 0]=({ struct Cyc_List_List _temp1003; _temp1003.hd=( void*) ae;
_temp1003.tl= 0; _temp1003;}); _temp1002;})); return;} _LL849: { void* _temp1006;
void* _temp1008; struct _tuple0 _temp1004= Cyc_NewControlFlow_abstract_guard(
env, _temp896); _LL1009: _temp1008= _temp1004.f1; goto _LL1007; _LL1007:
_temp1006= _temp1004.f2; goto _LL1005; _LL1005: Cyc_NewControlFlow_cf_set_absexps(
s,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1010=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1010[ 0]=({ struct Cyc_List_List
_temp1011; _temp1011.hd=( void*) _temp1008; _temp1011.tl=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1012=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1012[ 0]=({ struct Cyc_List_List _temp1013;
_temp1013.hd=( void*) _temp1006; _temp1013.tl= 0; _temp1013;}); _temp1012;});
_temp1011;}); _temp1010;})); Cyc_NewControlFlow_cf_prepass( env, _temp894); Cyc_NewControlFlow_cf_prepass(
env, _temp892); return;} _LL851: _temp912= _temp902; _temp910= _temp900;
_temp906= _temp904; goto _LL853; _LL853: { void* _temp1016; void* _temp1018;
struct _tuple0 _temp1014= Cyc_NewControlFlow_abstract_guard( env, _temp912);
_LL1019: _temp1018= _temp1014.f1; goto _LL1017; _LL1017: _temp1016= _temp1014.f2;
goto _LL1015; _LL1015: Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1020=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1020[ 0]=({ struct Cyc_List_List _temp1021;
_temp1021.hd=( void*) _temp1018; _temp1021.tl=( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp1022=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp1022[ 0]=({ struct Cyc_List_List _temp1023; _temp1023.hd=( void*)
_temp1016; _temp1023.tl= 0; _temp1023;}); _temp1022;}); _temp1021;}); _temp1020;}));
Cyc_NewControlFlow_cf_prepass( env, _temp906); return;} _LL855: return; _LL857:
return; _LL859:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp1024=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp1024=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp1025=( char*)"cf_prepass: goto w/o destination";
struct _tagged_string _temp1026; _temp1026.curr= _temp1025; _temp1026.base=
_temp1025; _temp1026.last_plus_one= _temp1025 + 33; _temp1026;})};( struct
_xenum_struct*) _temp1024;})); _LL861: { struct Cyc_Absyn_Stmt* my_encloser=(
Cyc_NewControlFlow_get_stmt_annot( s))->encloser; struct Cyc_Absyn_Stmt*
dest_encloser=( Cyc_NewControlFlow_get_stmt_annot(( struct Cyc_Absyn_Stmt*)({
struct Cyc_Absyn_Stmt* _temp1027= _temp922; if( _temp1027 == 0){ _throw(
Null_Exception);} _temp1027;})))->encloser; while( dest_encloser != my_encloser) {
struct Cyc_Absyn_Stmt* next_encloser=( Cyc_NewControlFlow_get_stmt_annot(
my_encloser))->encloser; if( next_encloser == my_encloser){ Cyc_Tcutil_terr( s->loc,(
struct _tagged_string)({ char* _temp1028=( char*)"goto enters local scope or exception handler";
struct _tagged_string _temp1029; _temp1029.curr= _temp1028; _temp1029.base=
_temp1028; _temp1029.last_plus_one= _temp1028 + 45; _temp1029;})); break;}
my_encloser= next_encloser;} return;} _LL863: { void* ae1= Cyc_NewControlFlow_abstract_exp_top(
env, _temp940); void* _temp1032; void* _temp1034; struct _tuple0 _temp1030= Cyc_NewControlFlow_abstract_guard(
env, _temp938); _LL1035: _temp1034= _temp1030.f1; goto _LL1033; _LL1033:
_temp1032= _temp1030.f2; goto _LL1031; _LL1031: { void* ae3= Cyc_NewControlFlow_abstract_exp_top(
env, _temp932); Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1036=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1036[ 0]=({ struct Cyc_List_List _temp1037;
_temp1037.hd=( void*) ae1; _temp1037.tl=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1038=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1038[ 0]=({ struct Cyc_List_List _temp1039; _temp1039.hd=( void*) _temp1034;
_temp1039.tl=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1040=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1040[ 0]=({
struct Cyc_List_List _temp1041; _temp1041.hd=( void*) _temp1032; _temp1041.tl=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp1042=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1042[ 0]=({ struct Cyc_List_List
_temp1043; _temp1043.hd=( void*) ae3; _temp1043.tl= 0; _temp1043;}); _temp1042;});
_temp1041;}); _temp1040;}); _temp1039;}); _temp1038;}); _temp1037;}); _temp1036;}));
Cyc_NewControlFlow_cf_prepass( env, _temp926); return;}} _LL865: { void* ae= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp944); struct Cyc_List_List* ael= Cyc_NewControlFlow_cf_prepass_scs(
env, _temp942); Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1044=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1044[ 0]=({ struct Cyc_List_List _temp1045;
_temp1045.hd=( void*) ae; _temp1045.tl= ael; _temp1045;}); _temp1044;}));
return;} _LL867: { void* _temp1048; void* _temp1050; struct Cyc_NewControlFlow_AbsSynOne
_temp1046= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered(
env, _temp948, 0)); _LL1051: _temp1050=( void*) _temp1046.inner_exp; goto
_LL1049; _LL1049: _temp1048=( void*) _temp1046.assigns; goto _LL1047; _LL1047: {
void* ae= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, _temp1050, _temp1048);
Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp1052=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp1052[ 0]=({ struct Cyc_List_List _temp1053; _temp1053.hd=( void*) ae;
_temp1053.tl= 0; _temp1053;}); _temp1052;})); return;}} _LL869: Cyc_NewControlFlow_add_var_root(
env, _temp959); if( _temp959->initializer != 0){ struct Cyc_NewControlFlow_AbsEnv
init_env= Cyc_NewControlFlow_change_lhs( env,( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp1054=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp1054[ 0]=({ struct Cyc_List_List _temp1055; _temp1055.hd=( void*) Cyc_CfAbsexp_mkLocalOp(
_temp959); _temp1055.tl= 0; _temp1055;}); _temp1054;})); void* ae= Cyc_NewControlFlow_abstract_exp_top(
init_env,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1056= _temp959->initializer;
if( _temp1056 == 0){ _throw( Null_Exception);} _temp1056;})); Cyc_NewControlFlow_cf_set_absexps(
s,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp1057=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp1057[ 0]=({ struct Cyc_List_List
_temp1058; _temp1058.hd=( void*) ae; _temp1058.tl= 0; _temp1058;}); _temp1057;}));}
Cyc_NewControlFlow_cf_prepass( env, _temp950); return; _LL871: for( 0; _temp979
!= 0; _temp979= _temp979->tl){ Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)
_temp979->hd);}{ void* ae= Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env(
env), _temp972); Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp1059=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp1059[ 0]=({ struct Cyc_List_List _temp1060;
_temp1060.hd=( void*) ae; _temp1060.tl= 0; _temp1060;}); _temp1059;})); Cyc_NewControlFlow_cf_prepass(
env, _temp961); return;} _LL873: goto _LL875; _LL875: goto _LL877; _LL877:( void)
_throw(({ struct Cyc_Core_Impossible_struct* _temp1061=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp1061=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp1062=( char*)"cf_prepass: bad stmt form";
struct _tagged_string _temp1063; _temp1063.curr= _temp1062; _temp1063.base=
_temp1062; _temp1063.last_plus_one= _temp1062 + 26; _temp1063;})};( struct
_xenum_struct*) _temp1061;})); _LL879: Cyc_NewControlFlow_cf_prepass( env,
_temp991); return; _LL881: Cyc_NewControlFlow_cf_prepass( env, _temp997);{
struct Cyc_List_List* ael= Cyc_NewControlFlow_cf_prepass_scs( env, _temp995);
Cyc_NewControlFlow_cf_set_absexps( s, ael); return;} _LL837:;}} static int Cyc_NewControlFlow_iterate_cf_check=
0; static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
new_flow,( void*) env->tryflow));}} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_pre_stmt_check( void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( annot->flow=( void*) Cyc_CfFlowInfo_join_flow( in_flow,( void*)
annot->flow)); ++ annot->visited; annot->visited == Cyc_NewControlFlow_iteration_num?
0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp1064=( char*)"new_control_flow.cyc:857 annot->visited == iteration_num";
struct _tagged_string _temp1065; _temp1065.curr= _temp1064; _temp1065.base=
_temp1064; _temp1065.last_plus_one= _temp1064 + 57; _temp1065;})); return annot;}
static void Cyc_NewControlFlow_update_flow( struct Cyc_Absyn_Stmt* s, void* flow){
struct Cyc_NewControlFlow_CFStmtAnnot* annot= Cyc_NewControlFlow_get_stmt_annot(
s); void* new_flow= Cyc_CfFlowInfo_join_flow( flow,( void*) annot->flow); if( !
Cyc_CfFlowInfo_flow_lessthan_approx( new_flow,( void*) annot->flow)){( void*)(
annot->flow=( void*) new_flow); if( annot->visited == Cyc_NewControlFlow_iteration_num){
Cyc_NewControlFlow_iterate_cf_check= 1;}}} static void* Cyc_NewControlFlow_add_init_vars_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1066= flow; struct Cyc_Dict_Dict* _temp1072; _LL1068: if( _temp1066
== Cyc_CfFlowInfo_BottomFL){ goto _LL1069;} else{ goto _LL1070;} _LL1070: if((
unsigned int) _temp1066 > 1u?(( struct _enum_struct*) _temp1066)->tag == Cyc_CfFlowInfo_InitsFL_tag:
0){ _LL1073: _temp1072=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1066)->f1; goto _LL1071;} else{ goto _LL1067;} _LL1069: return Cyc_CfFlowInfo_BottomFL;
_LL1071: for( 0; vds != 0; vds= vds->tl){ void* root=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1074=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1074->tag= Cyc_CfFlowInfo_VarRoot_tag; _temp1074->f1=( struct Cyc_Absyn_Vardecl*)
vds->hd;( void*) _temp1074;}); void* pinfo= Cyc_CfFlowInfo_assign_unknown_dict(
Cyc_CfFlowInfo_AllIL,(( void*(*)( struct Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)(
env->roots, root)); _temp1072=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert)( _temp1072, root, pinfo);} return({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp1075=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1075->tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1075->f1= _temp1072;( void*) _temp1075;}); _LL1067:;} static void* Cyc_NewControlFlow_add_vardecls_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp1076= flow; struct Cyc_Dict_Dict* _temp1082; _LL1078: if( _temp1076
== Cyc_CfFlowInfo_BottomFL){ goto _LL1079;} else{ goto _LL1080;} _LL1080: if((
unsigned int) _temp1076 > 1u?(( struct _enum_struct*) _temp1076)->tag == Cyc_CfFlowInfo_InitsFL_tag:
0){ _LL1083: _temp1082=( struct Cyc_Dict_Dict*)(( struct Cyc_CfFlowInfo_InitsFL_struct*)
_temp1076)->f1; goto _LL1081;} else{ goto _LL1077;} _LL1079: return Cyc_CfFlowInfo_BottomFL;
_LL1081: for( 0; vds != 0; vds= vds->tl){ void* root=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1084=( struct Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1084->tag= Cyc_CfFlowInfo_VarRoot_tag; _temp1084->f1=( struct Cyc_Absyn_Vardecl*)
vds->hd;( void*) _temp1084;}); _temp1082=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, void* key, void* data)) Cyc_Dict_insert)( _temp1082, root,(( void*(*)( struct
Cyc_Dict_Dict* d, void* key)) Cyc_Dict_lookup)( env->roots, root));} return({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp1085=( struct Cyc_CfFlowInfo_InitsFL_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp1085->tag= Cyc_CfFlowInfo_InitsFL_tag;
_temp1085->f1= _temp1082;( void*) _temp1085;}); _LL1077:;} static struct _tuple0
Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env, struct
Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if(( void*)
aes->hd ==( void*)( aes->tl)->hd){ void* out_flow= Cyc_CfAbsexp_eval_absexp( env,
loc,( void*) aes->hd, in_flow); return({ struct _tuple0 _temp1086; _temp1086.f1=
out_flow; _temp1086.f2= out_flow; _temp1086;});} return({ struct _tuple0
_temp1087; _temp1087.f1= Cyc_CfAbsexp_eval_absexp( env, loc,( void*) aes->hd,
in_flow); _temp1087.f2= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)( aes->tl)->hd,
in_flow); _temp1087;});} static void Cyc_NewControlFlow_cf_analyze_switch_clauses(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause
_temp1090; struct Cyc_Position_Segment* _temp1091; struct Cyc_Absyn_Stmt*
_temp1093; struct Cyc_Absyn_Exp* _temp1095; struct Cyc_Core_Opt* _temp1097;
struct Cyc_Absyn_Pat* _temp1099; struct Cyc_Absyn_Switch_clause* _temp1088=(
struct Cyc_Absyn_Switch_clause*) scs->hd; _temp1090=* _temp1088; _LL1100:
_temp1099=( struct Cyc_Absyn_Pat*) _temp1090.pattern; goto _LL1098; _LL1098:
_temp1097=( struct Cyc_Core_Opt*) _temp1090.pat_vars; goto _LL1096; _LL1096:
_temp1095=( struct Cyc_Absyn_Exp*) _temp1090.where_clause; goto _LL1094; _LL1094:
_temp1093=( struct Cyc_Absyn_Stmt*) _temp1090.body; goto _LL1092; _LL1092:
_temp1091=( struct Cyc_Position_Segment*) _temp1090.loc; goto _LL1089; _LL1089: {
void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct
Cyc_List_List*) _temp1097->v); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow,( struct Cyc_List_List*) _temp1097->v);{ void* body_outflow;
if( _temp1095 != 0){ void* _temp1104; void* _temp1106; struct _tuple0 _temp1102=
Cyc_NewControlFlow_cf_evalguard( env,(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1101= _temp1095; if( _temp1101 == 0){ _throw( Null_Exception);} _temp1101;}))->loc,
aes, body_inflow); _LL1107: _temp1106= _temp1102.f1; goto _LL1105; _LL1105:
_temp1104= _temp1102.f2; goto _LL1103; _LL1103: aes=( aes->tl)->tl; in_flow=
_temp1104; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1093,
_temp1106);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1093, body_inflow);}{ void* _temp1108= body_outflow; _LL1110: if( _temp1108
== Cyc_CfFlowInfo_BottomFL){ goto _LL1111;} else{ goto _LL1112;} _LL1112: goto
_LL1113; _LL1111: goto _LL1109; _LL1113: Cyc_Tcutil_terr( _temp1093->loc,(
struct _tagged_string)({ char* _temp1114=( char*)"switch clause may implicitly fallthru";
struct _tagged_string _temp1115; _temp1115.curr= _temp1114; _temp1115.base=
_temp1114; _temp1115.last_plus_one= _temp1114 + 38; _temp1115;})); goto _LL1109;
_LL1109:;}}}}} void* Cyc_NewControlFlow_cf_analyze_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Absyn_Stmt* s, void* in_flow){ struct Cyc_NewControlFlow_CFStmtAnnot*
annot= Cyc_NewControlFlow_pre_stmt_check( in_flow, s); in_flow=( void*) annot->flow;{
struct Cyc_List_List* aes= annot->absexps; void* _temp1116=( void*) s->r; struct
Cyc_Absyn_Exp* _temp1158; struct Cyc_Absyn_Exp* _temp1160; struct Cyc_Absyn_Exp*
_temp1162; struct Cyc_Absyn_Stmt* _temp1164; struct Cyc_Absyn_Stmt* _temp1166;
struct Cyc_Absyn_Stmt* _temp1168; struct Cyc_Absyn_Stmt* _temp1170; struct Cyc_Absyn_Exp*
_temp1172; struct Cyc_Absyn_Stmt* _temp1174; struct _tuple3 _temp1176; struct
Cyc_Absyn_Stmt* _temp1178; struct Cyc_Absyn_Exp* _temp1180; struct _tuple3
_temp1182; struct Cyc_Absyn_Stmt* _temp1184; struct Cyc_Absyn_Exp* _temp1186;
struct Cyc_Absyn_Stmt* _temp1188; struct Cyc_Absyn_Stmt* _temp1190; struct
_tuple3 _temp1192; struct Cyc_Absyn_Stmt* _temp1194; struct Cyc_Absyn_Exp*
_temp1196; struct _tuple3 _temp1198; struct Cyc_Absyn_Stmt* _temp1200; struct
Cyc_Absyn_Exp* _temp1202; struct Cyc_Absyn_Exp* _temp1204; struct Cyc_Absyn_Stmt*
_temp1206; struct Cyc_Absyn_Switch_clause** _temp1208; struct Cyc_Absyn_Switch_clause*
_temp1210; struct Cyc_List_List* _temp1211; struct Cyc_Absyn_Stmt* _temp1213;
struct Cyc_Absyn_Stmt* _temp1215; struct Cyc_Absyn_Stmt* _temp1217; struct
_tagged_string* _temp1219; struct Cyc_List_List* _temp1221; struct Cyc_Absyn_Exp*
_temp1223; struct Cyc_List_List* _temp1225; struct Cyc_Absyn_Stmt* _temp1227;
struct Cyc_Absyn_Stmt* _temp1229; struct Cyc_Absyn_Decl* _temp1231; struct Cyc_Absyn_Decl
_temp1233; struct Cyc_Position_Segment* _temp1234; void* _temp1236; struct Cyc_Absyn_Vardecl*
_temp1238; struct Cyc_Absyn_Stmt* _temp1240; struct Cyc_Absyn_Decl* _temp1242;
struct Cyc_Absyn_Decl _temp1244; struct Cyc_Position_Segment* _temp1245; void*
_temp1247; int _temp1249; struct Cyc_Absyn_Exp* _temp1251; struct Cyc_Core_Opt*
_temp1253; struct Cyc_Core_Opt* _temp1255; struct Cyc_Core_Opt _temp1257; struct
Cyc_List_List* _temp1258; struct Cyc_Absyn_Pat* _temp1260; struct Cyc_Absyn_Stmt*
_temp1262; struct _tagged_string* _temp1264; _LL1118: if( _temp1116 == Cyc_Absyn_Skip_s){
goto _LL1119;} else{ goto _LL1120;} _LL1120: if(( unsigned int) _temp1116 > 1u?((
struct _enum_struct*) _temp1116)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1159:
_temp1158=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp1116)->f1; if( _temp1158 == 0){ goto _LL1121;} else{ goto _LL1122;}} else{
goto _LL1122;} _LL1122: if(( unsigned int) _temp1116 > 1u?(( struct _enum_struct*)
_temp1116)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL1161: _temp1160=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp1116)->f1; goto _LL1123;} else{ goto
_LL1124;} _LL1124: if(( unsigned int) _temp1116 > 1u?(( struct _enum_struct*)
_temp1116)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL1163: _temp1162=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp_s_struct*) _temp1116)->f1; goto _LL1125;} else{ goto
_LL1126;} _LL1126: if(( unsigned int) _temp1116 > 1u?(( struct _enum_struct*)
_temp1116)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1167: _temp1166=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1116)->f1; goto _LL1165; _LL1165: _temp1164=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1116)->f2; goto
_LL1127;} else{ goto _LL1128;} _LL1128: if(( unsigned int) _temp1116 > 1u?((
struct _enum_struct*) _temp1116)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL1173:
_temp1172=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1116)->f1; goto _LL1171; _LL1171: _temp1170=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1116)->f2; goto _LL1169; _LL1169:
_temp1168=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1116)->f3; goto _LL1129;} else{ goto _LL1130;} _LL1130: if(( unsigned int)
_temp1116 > 1u?(( struct _enum_struct*) _temp1116)->tag == Cyc_Absyn_While_s_tag:
0){ _LL1177: _temp1176=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp1116)->f1; _LL1181: _temp1180= _temp1176.f1; goto _LL1179; _LL1179:
_temp1178= _temp1176.f2; goto _LL1175; _LL1175: _temp1174=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_While_s_struct*) _temp1116)->f2; goto _LL1131;} else{ goto
_LL1132;} _LL1132: if(( unsigned int) _temp1116 > 1u?(( struct _enum_struct*)
_temp1116)->tag == Cyc_Absyn_Do_s_tag: 0){ _LL1189: _temp1188=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Do_s_struct*) _temp1116)->f1; goto _LL1183; _LL1183: _temp1182=(
struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*) _temp1116)->f2; _LL1187:
_temp1186= _temp1182.f1; goto _LL1185; _LL1185: _temp1184= _temp1182.f2; goto
_LL1133;} else{ goto _LL1134;} _LL1134: if(( unsigned int) _temp1116 > 1u?((
struct _enum_struct*) _temp1116)->tag == Cyc_Absyn_For_s_tag: 0){ _LL1205:
_temp1204=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*) _temp1116)->f1;
goto _LL1199; _LL1199: _temp1198=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1116)->f2; _LL1203: _temp1202= _temp1198.f1; goto _LL1201; _LL1201:
_temp1200= _temp1198.f2; goto _LL1193; _LL1193: _temp1192=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1116)->f3; _LL1197: _temp1196= _temp1192.f1;
goto _LL1195; _LL1195: _temp1194= _temp1192.f2; goto _LL1191; _LL1191: _temp1190=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1116)->f4; goto
_LL1135;} else{ goto _LL1136;} _LL1136: if(( unsigned int) _temp1116 > 1u?((
struct _enum_struct*) _temp1116)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1207:
_temp1206=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1116)->f1; if( _temp1206 == 0){ goto _LL1137;} else{ goto _LL1138;}} else{
goto _LL1138;} _LL1138: if(( unsigned int) _temp1116 > 1u?(( struct _enum_struct*)
_temp1116)->tag == Cyc_Absyn_Fallthru_s_tag: 0){ _LL1212: _temp1211=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp1116)->f1; goto _LL1209; _LL1209:
_temp1208=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1116)->f2; if( _temp1208 == 0){ goto _LL1140;} else{ _temp1210=* _temp1208;
goto _LL1139;}} else{ goto _LL1140;} _LL1140: if(( unsigned int) _temp1116 > 1u?((
struct _enum_struct*) _temp1116)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL1214:
_temp1213=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*)
_temp1116)->f1; goto _LL1141;} else{ goto _LL1142;} _LL1142: if(( unsigned int)
_temp1116 > 1u?(( struct _enum_struct*) _temp1116)->tag == Cyc_Absyn_Continue_s_tag:
0){ _LL1216: _temp1215=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*)
_temp1116)->f1; goto _LL1143;} else{ goto _LL1144;} _LL1144: if(( unsigned int)
_temp1116 > 1u?(( struct _enum_struct*) _temp1116)->tag == Cyc_Absyn_Goto_s_tag:
0){ _LL1220: _temp1219=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp1116)->f1; goto _LL1218; _LL1218: _temp1217=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Goto_s_struct*) _temp1116)->f2; goto _LL1145;} else{ goto
_LL1146;} _LL1146: if(( unsigned int) _temp1116 > 1u?(( struct _enum_struct*)
_temp1116)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1224: _temp1223=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1116)->f1; goto _LL1222; _LL1222:
_temp1221=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1116)->f2; goto _LL1147;} else{ goto _LL1148;} _LL1148: if(( unsigned int)
_temp1116 > 1u?(( struct _enum_struct*) _temp1116)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL1228: _temp1227=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1116)->f1; goto _LL1226; _LL1226: _temp1225=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1116)->f2; goto _LL1149;} else{ goto
_LL1150;} _LL1150: if(( unsigned int) _temp1116 > 1u?(( struct _enum_struct*)
_temp1116)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1232: _temp1231=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1116)->f1; _temp1233=* _temp1231; _LL1237:
_temp1236=( void*) _temp1233.r; if(( unsigned int) _temp1236 > 1u?(( struct
_enum_struct*) _temp1236)->tag == Cyc_Absyn_Var_d_tag: 0){ _LL1239: _temp1238=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*) _temp1236)->f1;
goto _LL1235;} else{ goto _LL1152;} _LL1235: _temp1234=( struct Cyc_Position_Segment*)
_temp1233.loc; goto _LL1230; _LL1230: _temp1229=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1116)->f2; goto _LL1151;} else{ goto
_LL1152;} _LL1152: if(( unsigned int) _temp1116 > 1u?(( struct _enum_struct*)
_temp1116)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1243: _temp1242=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1116)->f1; _temp1244=* _temp1242; _LL1248:
_temp1247=( void*) _temp1244.r; if(( unsigned int) _temp1247 > 1u?(( struct
_enum_struct*) _temp1247)->tag == Cyc_Absyn_Let_d_tag: 0){ _LL1261: _temp1260=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1247)->f1; goto
_LL1256; _LL1256: _temp1255=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1247)->f2; if( _temp1255 == 0){ goto _LL1154;} else{ _temp1257=* _temp1255;
_LL1259: _temp1258=( struct Cyc_List_List*) _temp1257.v; goto _LL1254;} _LL1254:
_temp1253=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1247)->f3;
goto _LL1252; _LL1252: _temp1251=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1247)->f4; goto _LL1250; _LL1250: _temp1249=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp1247)->f5; goto _LL1246;} else{ goto _LL1154;} _LL1246: _temp1245=( struct
Cyc_Position_Segment*) _temp1244.loc; goto _LL1241; _LL1241: _temp1240=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1116)->f2; goto _LL1153;}
else{ goto _LL1154;} _LL1154: if(( unsigned int) _temp1116 > 1u?(( struct
_enum_struct*) _temp1116)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL1265: _temp1264=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp1116)->f1; goto
_LL1263; _LL1263: _temp1262=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1116)->f2; goto _LL1155;} else{ goto _LL1156;} _LL1156: goto _LL1157;
_LL1119: return in_flow; _LL1121: return Cyc_CfFlowInfo_BottomFL; _LL1123: Cyc_CfAbsexp_eval_absexp(
env, _temp1160->loc,( void*) aes->hd, in_flow); return Cyc_CfFlowInfo_BottomFL;
_LL1125: return Cyc_CfAbsexp_eval_absexp( env, _temp1162->loc,( void*) aes->hd,
in_flow); _LL1127: return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1164,
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1166, in_flow)); _LL1129: { void*
_temp1268; void* _temp1270; struct _tuple0 _temp1266= Cyc_NewControlFlow_cf_evalguard(
env, _temp1172->loc, aes, in_flow); _LL1271: _temp1270= _temp1266.f1; goto
_LL1269; _LL1269: _temp1268= _temp1266.f2; goto _LL1267; _LL1267: return Cyc_CfFlowInfo_join_flow(
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1170, _temp1270), Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1168, _temp1268));} _LL1131: { void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
in_flow, _temp1178))->flow; void* _temp1274; void* _temp1276; struct _tuple0
_temp1272= Cyc_NewControlFlow_cf_evalguard( env, _temp1180->loc, aes, e_inflow);
_LL1277: _temp1276= _temp1272.f1; goto _LL1275; _LL1275: _temp1274= _temp1272.f2;
goto _LL1273; _LL1273: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1174, _temp1276); Cyc_NewControlFlow_update_flow( _temp1178,
body_outflow); return _temp1274;}} _LL1133: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1188, in_flow); void* e_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1184))->flow; void* _temp1280; void* _temp1282; struct
_tuple0 _temp1278= Cyc_NewControlFlow_cf_evalguard( env, _temp1186->loc, aes,
e_inflow); _LL1283: _temp1282= _temp1278.f1; goto _LL1281; _LL1281: _temp1280=
_temp1278.f2; goto _LL1279; _LL1279: Cyc_NewControlFlow_update_flow( _temp1188,
_temp1282); return _temp1280;} _LL1135: { void* e1_outflow= Cyc_CfAbsexp_eval_absexp(
env, _temp1204->loc,( void*) aes->hd, in_flow); void* e2_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
e1_outflow, _temp1200))->flow; void* _temp1286; void* _temp1288; struct _tuple0
_temp1284= Cyc_NewControlFlow_cf_evalguard( env, _temp1202->loc, aes->tl,
e2_inflow); _LL1289: _temp1288= _temp1284.f1; goto _LL1287; _LL1287: _temp1286=
_temp1284.f2; goto _LL1285; _LL1285: { void* body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1190, _temp1288); void* e3_inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
body_outflow, _temp1194))->flow; void* e3_outflow= Cyc_CfAbsexp_eval_absexp( env,
_temp1196->loc,( void*)((( aes->tl)->tl)->tl)->hd, e3_inflow); Cyc_NewControlFlow_update_flow(
_temp1200, e3_outflow); return _temp1286;}} _LL1137: return Cyc_CfFlowInfo_BottomFL;
_LL1139: in_flow= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*) aes->hd,
in_flow);{ struct Cyc_List_List* ftvars=( struct Cyc_List_List*)( _temp1210->pat_vars)->v;
in_flow= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, ftvars); in_flow=
Cyc_NewControlFlow_add_init_vars_flow( env, in_flow, ftvars); _temp1213=( struct
Cyc_Absyn_Stmt*) _temp1210->body; goto _LL1141;} _LL1141: _temp1215= _temp1213;
goto _LL1143; _LL1143: _temp1217= _temp1215; goto _LL1145; _LL1145: Cyc_NewControlFlow_update_flow((
struct Cyc_Absyn_Stmt*)({ struct Cyc_Absyn_Stmt* _temp1290= _temp1217; if(
_temp1290 == 0){ _throw( Null_Exception);} _temp1290;}), in_flow); return Cyc_CfFlowInfo_BottomFL;
_LL1147: in_flow= Cyc_CfAbsexp_eval_absexp( env, _temp1223->loc,( void*) aes->hd,
in_flow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1221, aes->tl,
in_flow); return Cyc_CfFlowInfo_BottomFL; _LL1149: { struct Cyc_NewControlFlow_AnalEnv
_temp1293; void* _temp1294; int _temp1296; struct Cyc_Dict_Dict* _temp1298;
struct Cyc_NewControlFlow_AnalEnv* _temp1291= env; _temp1293=* _temp1291;
_LL1299: _temp1298=( struct Cyc_Dict_Dict*) _temp1293.roots; goto _LL1297;
_LL1297: _temp1296=( int) _temp1293.in_try; goto _LL1295; _LL1295: _temp1294=(
void*) _temp1293.tryflow; goto _LL1292; _LL1292: env->in_try= 1;( void*)( env->tryflow=(
void*) in_flow);{ void* s_outflow= Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1227, in_flow); void* scs_inflow=( void*) env->tryflow; env->in_try=
_temp1296;( void*)( env->tryflow=( void*) _temp1294); Cyc_NewControlFlow_update_tryflow(
env, scs_inflow); Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp1225,
aes, scs_inflow); return s_outflow;}} _LL1151: { struct Cyc_List_List l=({
struct Cyc_List_List _temp1300; _temp1300.hd=( void*) _temp1238; _temp1300.tl= 0;
_temp1300;}); void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow( env,
in_flow,( struct Cyc_List_List*)& l); struct Cyc_Absyn_Exp* e= _temp1238->initializer;
if( e != 0){ body_inflow= Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1301= e; if( _temp1301 == 0){ _throw( Null_Exception);}
_temp1301;}))->loc,( void*) aes->hd, body_inflow);} return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1229, body_inflow);} _LL1153: { void* body_inflow= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1258); body_inflow= Cyc_CfAbsexp_eval_absexp( env, _temp1251->loc,(
void*) aes->hd, body_inflow); body_inflow= Cyc_NewControlFlow_add_init_vars_flow(
env, body_inflow, _temp1258); return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1240, body_inflow);} _LL1155: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1262, in_flow); _LL1157:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp1302=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp1302=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp1303=( char*)"cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form";
struct _tagged_string _temp1304; _temp1304.curr= _temp1303; _temp1304.base=
_temp1303; _temp1304.last_plus_one= _temp1303 + 62; _temp1304;})};( struct
_xenum_struct*) _temp1302;})); _LL1117:;}} void Cyc_NewControlFlow_cf_postpass(
struct Cyc_Absyn_Stmt* s){ s->annot=({ struct Cyc_Absyn_EmptyAnnot_struct*
_temp1305=( struct Cyc_Absyn_EmptyAnnot_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_EmptyAnnot_struct));* _temp1305=( struct Cyc_Absyn_EmptyAnnot_struct){.tag=
Cyc_Absyn_EmptyAnnot_tag};( struct _xenum_struct*) _temp1305;});{ void*
_temp1306=( void*) s->r; struct Cyc_Absyn_Stmt* _temp1328; struct Cyc_Absyn_Stmt*
_temp1330; struct Cyc_Absyn_Stmt* _temp1332; struct Cyc_Absyn_Stmt* _temp1334;
struct Cyc_Absyn_Exp* _temp1336; struct Cyc_Absyn_Stmt* _temp1338; struct
_tuple3 _temp1340; struct Cyc_Absyn_Stmt* _temp1342; struct Cyc_Absyn_Exp*
_temp1344; struct _tuple3 _temp1346; struct Cyc_Absyn_Stmt* _temp1348; struct
Cyc_Absyn_Exp* _temp1350; struct Cyc_Absyn_Exp* _temp1352; struct Cyc_Absyn_Stmt*
_temp1354; struct _tuple3 _temp1356; struct Cyc_Absyn_Stmt* _temp1358; struct
Cyc_Absyn_Exp* _temp1360; struct _tuple3 _temp1362; struct Cyc_Absyn_Stmt*
_temp1364; struct Cyc_Absyn_Exp* _temp1366; struct Cyc_Absyn_Stmt* _temp1368;
struct Cyc_Absyn_Stmt* _temp1370; struct _tagged_string* _temp1372; struct Cyc_Absyn_Stmt*
_temp1374; struct Cyc_Absyn_Decl* _temp1376; struct Cyc_List_List* _temp1378;
struct Cyc_Absyn_Stmt* _temp1380; struct Cyc_List_List* _temp1382; struct Cyc_Absyn_Exp*
_temp1384; _LL1308: if(( unsigned int) _temp1306 > 1u?(( struct _enum_struct*)
_temp1306)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL1331: _temp1330=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Seq_s_struct*) _temp1306)->f1; goto _LL1329; _LL1329: _temp1328=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp1306)->f2; goto
_LL1309;} else{ goto _LL1310;} _LL1310: if(( unsigned int) _temp1306 > 1u?((
struct _enum_struct*) _temp1306)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL1337:
_temp1336=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1306)->f1; goto _LL1335; _LL1335: _temp1334=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1306)->f2; goto _LL1333; _LL1333:
_temp1332=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1306)->f3; goto _LL1311;} else{ goto _LL1312;} _LL1312: if(( unsigned int)
_temp1306 > 1u?(( struct _enum_struct*) _temp1306)->tag == Cyc_Absyn_For_s_tag:
0){ _LL1353: _temp1352=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp1306)->f1; goto _LL1347; _LL1347: _temp1346=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp1306)->f2; _LL1351: _temp1350= _temp1346.f1; goto _LL1349; _LL1349:
_temp1348= _temp1346.f2; goto _LL1341; _LL1341: _temp1340=( struct _tuple3)((
struct Cyc_Absyn_For_s_struct*) _temp1306)->f3; _LL1345: _temp1344= _temp1340.f1;
goto _LL1343; _LL1343: _temp1342= _temp1340.f2; goto _LL1339; _LL1339: _temp1338=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*) _temp1306)->f4; goto
_LL1313;} else{ goto _LL1314;} _LL1314: if(( unsigned int) _temp1306 > 1u?((
struct _enum_struct*) _temp1306)->tag == Cyc_Absyn_While_s_tag: 0){ _LL1357:
_temp1356=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*) _temp1306)->f1;
_LL1361: _temp1360= _temp1356.f1; goto _LL1359; _LL1359: _temp1358= _temp1356.f2;
goto _LL1355; _LL1355: _temp1354=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*)
_temp1306)->f2; goto _LL1315;} else{ goto _LL1316;} _LL1316: if(( unsigned int)
_temp1306 > 1u?(( struct _enum_struct*) _temp1306)->tag == Cyc_Absyn_Do_s_tag: 0){
_LL1369: _temp1368=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp1306)->f1; goto _LL1363; _LL1363: _temp1362=( struct _tuple3)(( struct Cyc_Absyn_Do_s_struct*)
_temp1306)->f2; _LL1367: _temp1366= _temp1362.f1; goto _LL1365; _LL1365:
_temp1364= _temp1362.f2; goto _LL1317;} else{ goto _LL1318;} _LL1318: if((
unsigned int) _temp1306 > 1u?(( struct _enum_struct*) _temp1306)->tag == Cyc_Absyn_Label_s_tag:
0){ _LL1373: _temp1372=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*)
_temp1306)->f1; goto _LL1371; _LL1371: _temp1370=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Label_s_struct*) _temp1306)->f2; goto _LL1319;} else{ goto
_LL1320;} _LL1320: if(( unsigned int) _temp1306 > 1u?(( struct _enum_struct*)
_temp1306)->tag == Cyc_Absyn_Decl_s_tag: 0){ _LL1377: _temp1376=( struct Cyc_Absyn_Decl*)((
struct Cyc_Absyn_Decl_s_struct*) _temp1306)->f1; goto _LL1375; _LL1375:
_temp1374=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp1306)->f2;
goto _LL1321;} else{ goto _LL1322;} _LL1322: if(( unsigned int) _temp1306 > 1u?((
struct _enum_struct*) _temp1306)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL1381:
_temp1380=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1306)->f1; goto _LL1379; _LL1379: _temp1378=( struct Cyc_List_List*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp1306)->f2; goto _LL1323;} else{ goto
_LL1324;} _LL1324: if(( unsigned int) _temp1306 > 1u?(( struct _enum_struct*)
_temp1306)->tag == Cyc_Absyn_Switch_s_tag: 0){ _LL1385: _temp1384=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Switch_s_struct*) _temp1306)->f1; goto _LL1383; _LL1383:
_temp1382=( struct Cyc_List_List*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp1306)->f2; goto _LL1325;} else{ goto _LL1326;} _LL1326: goto _LL1327;
_LL1309: _temp1334= _temp1330; _temp1332= _temp1328; goto _LL1311; _LL1311: Cyc_NewControlFlow_cf_postpass(
_temp1334); Cyc_NewControlFlow_cf_postpass( _temp1332); return; _LL1313: Cyc_NewControlFlow_cf_postpass(
_temp1342); _temp1358= _temp1338; _temp1354= _temp1348; goto _LL1315; _LL1315:
_temp1368= _temp1354; _temp1364= _temp1358; goto _LL1317; _LL1317: Cyc_NewControlFlow_cf_postpass(
_temp1364); _temp1370= _temp1368; goto _LL1319; _LL1319: _temp1374= _temp1370;
goto _LL1321; _LL1321: Cyc_NewControlFlow_cf_postpass( _temp1374); return;
_LL1323: Cyc_NewControlFlow_cf_postpass( _temp1380); _temp1382= _temp1378; goto
_LL1325; _LL1325: for( 0; _temp1382 != 0; _temp1382= _temp1382->tl){ Cyc_NewControlFlow_cf_postpass(((
struct Cyc_Absyn_Switch_clause*) _temp1382->hd)->body);} return; _LL1327:
return; _LL1307:;}} void Cyc_NewControlFlow_cf_check_fun( struct Cyc_Absyn_Fndecl*
fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct Cyc_NewControlFlow_AbsEnv
_temp1386; _temp1386.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv* _temp1387=(
struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_SharedAbsEnv)
* 1); _temp1387[ 0]=({ struct Cyc_NewControlFlow_SharedAbsEnv _temp1388;
_temp1388.all_roots=(( struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*)))
Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp); _temp1388.comprehension_vars= 0;
_temp1388;}); _temp1387;}); _temp1386.lhsides= 0; _temp1386.access_path= 0;
_temp1386;}); struct Cyc_List_List* arg_vds=( struct Cyc_List_List*)( fd->param_vardecls)->v;{
struct Cyc_List_List* vds= arg_vds; for( 0; vds != 0; vds= vds->tl){ Cyc_NewControlFlow_add_var_root(
absenv,( struct Cyc_Absyn_Vardecl*) vds->hd);}} Cyc_NewControlFlow_cf_prepass(
absenv, fd->body);{ struct Cyc_List_List* _temp1391; struct Cyc_List_List*
_temp1393; struct Cyc_NewControlFlow_SharedAbsEnv* _temp1395; struct Cyc_NewControlFlow_SharedAbsEnv
_temp1397; struct Cyc_List_List* _temp1398; struct Cyc_Dict_Dict* _temp1400;
struct Cyc_NewControlFlow_AbsEnv _temp1389= absenv; _LL1396: _temp1395=( struct
Cyc_NewControlFlow_SharedAbsEnv*) _temp1389.shared; _temp1397=* _temp1395;
_LL1401: _temp1400=( struct Cyc_Dict_Dict*) _temp1397.all_roots; goto _LL1399;
_LL1399: _temp1398=( struct Cyc_List_List*) _temp1397.comprehension_vars; goto
_LL1394; _LL1394: _temp1393=( struct Cyc_List_List*) _temp1389.lhsides; goto
_LL1392; _LL1392: _temp1391=( struct Cyc_List_List*) _temp1389.access_path; goto
_LL1390; _LL1390: { struct Cyc_NewControlFlow_AnalEnv* analenv=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1402=( struct Cyc_NewControlFlow_AnalEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_AnalEnv)
* 1); _temp1402[ 0]=({ struct Cyc_NewControlFlow_AnalEnv _temp1403; _temp1403.roots=
_temp1400; _temp1403.in_try= 0; _temp1403.tryflow=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1403;}); _temp1402;}); void* in_flow=({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1404=( struct Cyc_CfFlowInfo_InitsFL_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1404->tag= Cyc_CfFlowInfo_InitsFL_tag; _temp1404->f1=(( struct Cyc_Dict_Dict*(*)(
int(* comp)( void*, void*))) Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp);(
void*) _temp1404;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow( analenv,
in_flow, arg_vds); in_flow= Cyc_NewControlFlow_add_vardecls_flow( analenv,
in_flow, _temp1398);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num=
0; Cyc_NewControlFlow_iterate_cf_check= 1; while( Cyc_NewControlFlow_iterate_cf_check) {
++ Cyc_NewControlFlow_iteration_num; Cyc_NewControlFlow_iterate_cf_check= 0;
out_flow= Cyc_NewControlFlow_cf_analyze_stmt( analenv, fd->body, in_flow);}{
void* _temp1405= out_flow; _LL1407: if( _temp1405 == Cyc_CfFlowInfo_BottomFL){
goto _LL1408;} else{ goto _LL1409;} _LL1409: goto _LL1410; _LL1408: goto _LL1406;
_LL1410: if(( void*) fd->ret_type != Cyc_Absyn_VoidType){ Cyc_Tcutil_terr(( fd->body)->loc,(
struct _tagged_string)({ char* _temp1411=( char*)"function may complete without returning a value";
struct _tagged_string _temp1412; _temp1412.curr= _temp1411; _temp1412.base=
_temp1411; _temp1412.last_plus_one= _temp1411 + 48; _temp1412;}));} goto _LL1406;
_LL1406:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds != 0; ds= ds->tl){ void* _temp1413=( void*)((
struct Cyc_Absyn_Decl*) ds->hd)->r; struct Cyc_Absyn_Fndecl* _temp1423; struct
Cyc_List_List* _temp1425; struct _tuple1* _temp1427; struct Cyc_List_List*
_temp1429; struct _tagged_string* _temp1431; _LL1415: if(( unsigned int)
_temp1413 > 1u?(( struct _enum_struct*) _temp1413)->tag == Cyc_Absyn_Fn_d_tag: 0){
_LL1424: _temp1423=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp1413)->f1; goto _LL1416;} else{ goto _LL1417;} _LL1417: if(( unsigned int)
_temp1413 > 1u?(( struct _enum_struct*) _temp1413)->tag == Cyc_Absyn_Using_d_tag:
0){ _LL1428: _temp1427=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1413)->f1; goto _LL1426; _LL1426: _temp1425=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1413)->f2; goto _LL1418;} else{ goto
_LL1419;} _LL1419: if(( unsigned int) _temp1413 > 1u?(( struct _enum_struct*)
_temp1413)->tag == Cyc_Absyn_Namespace_d_tag: 0){ _LL1432: _temp1431=( struct
_tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp1413)->f1; goto
_LL1430; _LL1430: _temp1429=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1413)->f2; goto _LL1420;} else{ goto _LL1421;} _LL1421: goto _LL1422;
_LL1416: Cyc_NewControlFlow_cf_check_fun( _temp1423); goto _LL1414; _LL1418:
_temp1429= _temp1425; goto _LL1420; _LL1420: Cyc_NewControlFlow_cf_check(
_temp1429); goto _LL1414; _LL1422: goto _LL1414; _LL1414:;}}