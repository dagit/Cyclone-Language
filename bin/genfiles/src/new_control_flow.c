#include "cyc_include.h"

 struct _tuple0{ void* f1; void* f2;}; struct _tuple1{ void* f1; struct
_tagged_string* f2;}; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual*
f2; void* f3;}; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt*
f2;}; struct _tuple4{ struct Cyc_Absyn_Tqual* f1; void* f2;}; struct _tuple5{
struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2;}; struct _tuple6{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual* f2; void* f3;}; typedef unsigned int
Cyc_uint; typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string;
typedef struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
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
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag;}; extern int Cyc_List_memq( struct
Cyc_List_List* l, void* x); struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict*
Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[
8u]; struct Cyc_Dict_Present_struct{ char* tag;}; extern char Cyc_Dict_Absent_tag[
7u]; struct Cyc_Dict_Absent_struct{ char* tag;}; extern struct Cyc_Dict_Dict*
Cyc_Dict_empty( int(* comp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* key, void* data); extern struct Cyc_Dict_Dict*
Cyc_Dict_insert_new( struct Cyc_Dict_Dict* d, void* key, void* data); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stdout; typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1;}; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag;}; extern int Cyc_String_zstrptrcmp(
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
struct Cyc_Absyn_EmptyAnnot_struct{ char* tag;}; extern struct Cyc_Absyn_Exp*
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
8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag;}; extern void* Cyc_Tcutil_compress(
void* t); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); typedef void* Cyc_CfFlowInfo_LocalRoot; typedef void* Cyc_CfFlowInfo_Field;
struct Cyc_CfFlowInfo_Place; typedef void* Cyc_CfFlowInfo_InitState; typedef
void* Cyc_CfFlowInfo_PathInfo; typedef void* Cyc_CfFlowInfo_FlowInfo; typedef
void* Cyc_CfFlowInfo_local_root_t; typedef void* Cyc_CfFlowInfo_field_t; typedef
struct Cyc_CfFlowInfo_Place* Cyc_CfFlowInfo_place_t; typedef void* Cyc_CfFlowInfo_init_state_t;
typedef void* Cyc_CfFlowInfo_path_info_t; typedef struct Cyc_Dict_Dict* Cyc_CfFlowInfo_path_info_dict_t;
typedef void* Cyc_CfFlowInfo_flow_info_t; extern const int Cyc_CfFlowInfo_VarRoot_tag;
struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;};
extern const int Cyc_CfFlowInfo_MallocPt_tag; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1;}; extern const int Cyc_CfFlowInfo_StructF_tag;
struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_string* f1;};
extern const int Cyc_CfFlowInfo_TupleF_tag; struct Cyc_CfFlowInfo_TupleF_struct{
int tag; int f1;}; struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List*
fields;}; extern void* Cyc_CfFlowInfo_Uninit; extern void* Cyc_CfFlowInfo_ThisInit;
extern void* Cyc_CfFlowInfo_AllInit; extern const int Cyc_CfFlowInfo_MustPointTo_tag;
struct Cyc_CfFlowInfo_MustPointTo_struct{ int tag; struct Cyc_CfFlowInfo_Place*
f1;}; extern const int Cyc_CfFlowInfo_LeafPI_tag; struct Cyc_CfFlowInfo_LeafPI_struct{
int tag; void* f1;}; extern const int Cyc_CfFlowInfo_TuplePI_tag; struct Cyc_CfFlowInfo_TuplePI_struct{
int tag; struct Cyc_Dict_Dict* f1;}; extern const int Cyc_CfFlowInfo_StructPI_tag;
struct Cyc_CfFlowInfo_StructPI_struct{ int tag; struct Cyc_Dict_Dict* f1;};
extern void* Cyc_CfFlowInfo_BottomFL; extern const int Cyc_CfFlowInfo_InitsFL_tag;
struct Cyc_CfFlowInfo_InitsFL_struct{ int tag; struct Cyc_Dict_Dict* f1; struct
Cyc_Dict_Dict* f2;}; extern int Cyc_CfFlowInfo_local_root_cmp( void*, void*);
extern void* Cyc_CfFlowInfo_typ_to_pathinfo_uninit( void* t); extern void* Cyc_NewControlFlow_cf_analyze_stmt(
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
void* Cyc_CfAbsexp_mkAssignAE( void* l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE(
struct Cyc_Absyn_Exp* e); extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt*
s); extern void* Cyc_CfAbsexp_mkGroupAE( void* g, void* ae1, void* ae2); extern
void* Cyc_CfAbsexp_mkGroupAE_l( void* g, struct Cyc_List_List* ael); extern int
Cyc_CfAbsexp_isUnknownOp( void*); extern struct Cyc_Absyn_Stmt* Cyc_ControlFlow_get_encloser(
struct Cyc_Absyn_Stmt* enclosee); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate*
Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t;
struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct
_tagged_string Cyc_Absynpp_exp2string( struct Cyc_Absyn_Exp*); typedef struct
Cyc_Absyn_Vardecl* Cyc_NewControlFlow_vardecl_t; typedef struct Cyc_Absyn_Stmt*
Cyc_NewControlFlow_stmt_t; typedef void* Cyc_NewControlFlow_typ_t; struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt* encloser; struct Cyc_List_List* non_local_preds; int
visited; void* old_env; struct Cyc_List_List* absexps;}; typedef struct Cyc_NewControlFlow_CFStmtAnnot
Cyc_NewControlFlow_cf_stmt_annot_t; char Cyc_NewControlFlow_CFAnnot_tag[ 8u]="CFAnnot";
struct Cyc_NewControlFlow_CFAnnot_struct{ char* tag; struct Cyc_NewControlFlow_CFStmtAnnot
f1;}; static struct Cyc_NewControlFlow_CFStmtAnnot* Cyc_NewControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt* s){ struct _xenum_struct* _temp0= s->annot; struct Cyc_NewControlFlow_CFStmtAnnot
_temp6; struct Cyc_NewControlFlow_CFStmtAnnot* _temp8; _LL2: if((*(( struct
_xenum_struct*) _temp0)).tag == Cyc_NewControlFlow_CFAnnot_tag){ _LL7: _temp6=((
struct Cyc_NewControlFlow_CFAnnot_struct*) _temp0)->f1; _temp8=&(( struct Cyc_NewControlFlow_CFAnnot_struct*)
_temp0)->f1; goto _LL3;} else{ goto _LL4;} _LL4: goto _LL5; _LL3: return _temp8;
_LL5:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp9=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp9=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp10=( char*)"ControlFlow -- wrong stmt annotation";
struct _tagged_string _temp11; _temp11.curr= _temp10; _temp11.base= _temp10;
_temp11.last_plus_one= _temp10 + 37; _temp11;})};( struct _xenum_struct*) _temp9;}));
_LL1:;} void Cyc_NewControlFlow_cf_decorate_stmt( struct Cyc_Absyn_Stmt* s){
struct Cyc_Absyn_Stmt* encloser= Cyc_ControlFlow_get_encloser( s); s->annot=({
struct Cyc_NewControlFlow_CFAnnot_struct* _temp12=( struct Cyc_NewControlFlow_CFAnnot_struct*)
GC_malloc( sizeof( struct Cyc_NewControlFlow_CFAnnot_struct));* _temp12=( struct
Cyc_NewControlFlow_CFAnnot_struct){.tag= Cyc_NewControlFlow_CFAnnot_tag,.f1=({
struct Cyc_NewControlFlow_CFStmtAnnot _temp13; _temp13.encloser= encloser;
_temp13.non_local_preds= s->non_local_preds; _temp13.visited= 0; _temp13.old_env=(
void*) Cyc_CfFlowInfo_BottomFL; _temp13.absexps= 0; _temp13;})};( struct
_xenum_struct*) _temp12;});} void Cyc_NewControlFlow_cf_set_absexps( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* ael){( Cyc_NewControlFlow_get_stmt_annot( s))->absexps=
ael;} struct Cyc_NewControlFlow_SharedAbsEnv{ struct Cyc_Dict_Dict* all_roots;};
struct Cyc_NewControlFlow_AbsEnv{ struct Cyc_NewControlFlow_SharedAbsEnv* shared;
struct Cyc_List_List* lhsides; struct Cyc_List_List* access_path;}; typedef
struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_absenv_t; struct Cyc_NewControlFlow_AbsSyn{
void* when_true; void* when_false; struct Cyc_List_List* lvalues;}; typedef
struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abssyn_t; void Cyc_NewControlFlow_add_var_root(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Vardecl* vd){( env.shared)->all_roots=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert_new)((
env.shared)->all_roots,({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp14=( struct
Cyc_CfFlowInfo_VarRoot_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp14->tag= Cyc_CfFlowInfo_VarRoot_tag; _temp14->f1= vd;( void*) _temp14;}),
Cyc_CfFlowInfo_typ_to_pathinfo_uninit(( void*) vd->type));} void Cyc_NewControlFlow_add_malloc_root(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e, void* t){( env.shared)->all_roots=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* key, void* data)) Cyc_Dict_insert_new)((
env.shared)->all_roots,({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp15=(
struct Cyc_CfFlowInfo_MallocPt_struct*) GC_malloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp15->tag= Cyc_CfFlowInfo_MallocPt_tag; _temp15->f1= e;( void*) _temp15;}),
Cyc_CfFlowInfo_typ_to_pathinfo_uninit( t));} void Cyc_NewControlFlow_add_enum_root(
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
d, int key, void* data)) Cyc_Dict_insert)( d, i, Cyc_CfFlowInfo_typ_to_pathinfo_uninit(
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
Cyc_NewControlFlow_use_it_list()); ans.access_path= 0; return ans;} void* Cyc_NewControlFlow_meet_absexp(
struct Cyc_NewControlFlow_AbsSyn syn){ if(( void*) syn.when_true ==( void*) syn.when_false){
return( void*) syn.when_true;} return Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG,(
void*) syn.when_true,( void*) syn.when_false);} struct Cyc_Stdio___sFILE** Cyc_NewControlFlow_dump_file=&
Cyc_Stdio_stdout; void Cyc_NewControlFlow_test_translation( struct Cyc_NewControlFlow_AbsSyn
syn, struct Cyc_Absyn_Exp* e){ Cyc_CfAbsexp_check_absexp(( void*) syn.when_true);
if(( void*) syn.when_true !=( void*) syn.when_false){ Cyc_CfAbsexp_check_absexp((
void*) syn.when_false);} if(( void*) syn.when_true ==( void*) syn.when_false){({
struct _tagged_string _temp41= Cyc_Absynpp_exp2string( e); struct _tagged_string
_temp42= Cyc_CfAbsexp_absexp2string(( void*) syn.when_true, 0); fprintf(* Cyc_NewControlFlow_dump_file,"%.*s\n%.*s\n",
_temp41.last_plus_one - _temp41.curr, _temp41.curr, _temp42.last_plus_one -
_temp42.curr, _temp42.curr);});} else{ Cyc_CfAbsexp_check_absexp(( void*) syn.when_false);({
struct _tagged_string _temp43= Cyc_Absynpp_exp2string( e); struct _tagged_string
_temp44= Cyc_CfAbsexp_absexp2string(( void*) syn.when_true, 4); struct
_tagged_string _temp45= Cyc_CfAbsexp_absexp2string(( void*) syn.when_false, 5);
fprintf(* Cyc_NewControlFlow_dump_file,"%.*s\nTRUE %.*s\nFALSE %.*s\n", _temp43.last_plus_one
- _temp43.curr, _temp43.curr, _temp44.last_plus_one - _temp44.curr, _temp44.curr,
_temp45.last_plus_one - _temp45.curr, _temp45.curr);});}} void Cyc_NewControlFlow_dump_roots(
struct Cyc_NewControlFlow_AbsEnv env){ fprintf(* Cyc_NewControlFlow_dump_file,"\n ROOTS NOT DONE");
fprintf(* Cyc_NewControlFlow_dump_file,"\n");} void* Cyc_NewControlFlow_make_assigns(
struct Cyc_NewControlFlow_AbsEnv env, void* ae){ if( env.lhsides == 0){ return
Cyc_CfAbsexp_mkSkipAE();} if(( env.lhsides)->tl == 0){ return Cyc_CfAbsexp_mkAssignAE((
void*)( env.lhsides)->hd, ae);}{ struct Cyc_List_List* l= 0;{ struct Cyc_List_List*
lhsides= env.lhsides; for( 0; lhsides != 0; lhsides= lhsides->tl){ l=( struct
Cyc_List_List*)({ struct Cyc_List_List* _temp46=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp46[ 0]=({ struct Cyc_List_List
_temp47; _temp47.hd=( void*) Cyc_CfAbsexp_mkAssignAE(( void*) lhsides->hd, ae);
_temp47.tl= l; _temp47;}); _temp46;});}} return Cyc_CfAbsexp_mkGroupAE_l( Cyc_CfAbsexp_OneofG,
l);}} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp( struct
Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e); void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Stmt* s); struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_use_exp_unordered( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_List_List* es, struct Cyc_List_List* lhs){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_use_it_env( env); void* ansexp= Cyc_CfAbsexp_mkSkipAE();
for( 0; es != 0; es= es->tl){ void* next_ae= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
elt_env,( struct Cyc_Absyn_Exp*) es->hd)); ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
next_ae, ansexp);} ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, ansexp,
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); return({
struct Cyc_NewControlFlow_AbsSyn _temp48; _temp48.when_true=( void*) ansexp;
_temp48.when_false=( void*) ansexp; _temp48.lvalues= lhs; _temp48;});} struct
Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ void* _temp49=( void*) e->r; struct Cyc_Absyn_Exp*
_temp129; struct Cyc_List_List* _temp131; struct Cyc_Absyn_Exp* _temp133; struct
Cyc_Absyn_Exp* _temp135; void* _temp137; void* _temp139; int _temp141; void*
_temp143; void* _temp145; int _temp147; void* _temp149; void* _temp151; void*
_temp153; void* _temp155; struct Cyc_Absyn_Fndecl* _temp157; struct _tuple1*
_temp159; void* _temp161; struct Cyc_Absyn_Vardecl* _temp163; struct _tuple1*
_temp165; void* _temp167; struct Cyc_Absyn_Vardecl* _temp169; struct _tuple1*
_temp171; void* _temp173; struct Cyc_Absyn_Vardecl* _temp175; struct _tuple1*
_temp177; void* _temp179; struct Cyc_Absyn_Vardecl* _temp181; struct _tuple1*
_temp183; struct Cyc_Absyn_Enumfield* _temp185; struct Cyc_Absyn_Enumdecl*
_temp187; struct Cyc_List_List* _temp189; struct Cyc_Core_Opt* _temp191; struct
Cyc_Core_Opt* _temp193; struct Cyc_Absyn_Enumfield* _temp195; struct Cyc_Absyn_Xenumdecl*
_temp197; struct Cyc_List_List* _temp199; struct Cyc_Core_Opt* _temp201; struct
Cyc_Absyn_Exp* _temp203; struct Cyc_Core_Opt* _temp205; struct Cyc_Absyn_Exp*
_temp207; struct Cyc_Absyn_Exp* _temp209; struct Cyc_Core_Opt* _temp211; struct
Cyc_Absyn_Exp* _temp213; struct Cyc_List_List* _temp215; void* _temp217; struct
Cyc_List_List* _temp219; struct Cyc_Absyn_Exp* _temp221; struct Cyc_Absyn_Exp*
_temp223; struct Cyc_Absyn_Exp* _temp225; struct Cyc_Absyn_Exp* _temp227; struct
Cyc_Absyn_Exp* _temp229; struct _tagged_string* _temp231; struct Cyc_Absyn_Exp*
_temp233; struct _tagged_string* _temp235; struct Cyc_Absyn_Exp* _temp237;
struct Cyc_List_List* _temp239; struct Cyc_Absyn_Structdecl* _temp241; struct
Cyc_List_List* _temp243; struct Cyc_Core_Opt* _temp245; struct _tuple1* _temp247;
struct Cyc_List_List* _temp249; int _temp251; void* _temp253; struct Cyc_Absyn_Exp*
_temp255; struct Cyc_Absyn_Exp* _temp257; struct Cyc_Absyn_Exp* _temp259; struct
Cyc_Absyn_Exp* _temp261; struct Cyc_Absyn_Exp* _temp263; struct Cyc_Absyn_Stmt*
_temp265; struct Cyc_Absyn_Exp* _temp267; struct Cyc_Absyn_Exp* _temp269; struct
Cyc_Absyn_Exp* _temp271; struct Cyc_Absyn_Exp* _temp273; struct Cyc_Absyn_Vardecl*
_temp275; void* _temp277; struct _tuple1* _temp279; struct _tuple1* _temp281;
struct Cyc_List_List* _temp283; struct Cyc_Absyn_Exp* _temp285; struct Cyc_List_List*
_temp287; struct Cyc_Core_Opt* _temp289; struct Cyc_List_List* _temp291; struct
_tuple2* _temp293; struct Cyc_Absyn_Fndecl* _temp295; struct Cyc_Absyn_Exp*
_temp297; _LL51: if((( struct _enum_struct*) _temp49)->tag == Cyc_Absyn_NoInstantiate_e_tag){
_LL130: _temp129=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp49)->f1; goto _LL52;} else{ goto _LL53;} _LL53: if((( struct _enum_struct*)
_temp49)->tag == Cyc_Absyn_Instantiate_e_tag){ _LL134: _temp133=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Instantiate_e_struct*) _temp49)->f1; goto _LL132; _LL132:
_temp131=( struct Cyc_List_List*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp49)->f2; goto _LL54;} else{ goto _LL55;} _LL55: if((( struct _enum_struct*)
_temp49)->tag == Cyc_Absyn_Cast_e_tag){ _LL138: _temp137=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp49)->f1; goto _LL136; _LL136: _temp135=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Cast_e_struct*) _temp49)->f2; goto _LL56;} else{ goto _LL57;} _LL57:
if((( struct _enum_struct*) _temp49)->tag == Cyc_Absyn_Const_e_tag){ _LL140:
_temp139=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp49)->f1; if((
unsigned int) _temp139 > 1u?(( struct _enum_struct*) _temp139)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL144: _temp143=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp139)->f1;
if( _temp143 == Cyc_Absyn_Signed){ goto _LL142;} else{ goto _LL59;} _LL142:
_temp141=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp139)->f2; if( _temp141 ==
0){ goto _LL58;} else{ goto _LL59;}} else{ goto _LL59;}} else{ goto _LL59;}
_LL59: if((( struct _enum_struct*) _temp49)->tag == Cyc_Absyn_Const_e_tag){
_LL146: _temp145=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp49)->f1; if((
unsigned int) _temp145 > 1u?(( struct _enum_struct*) _temp145)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL150: _temp149=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp145)->f1;
if( _temp149 == Cyc_Absyn_Signed){ goto _LL148;} else{ goto _LL61;} _LL148:
_temp147=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp145)->f2; if( _temp147 ==
1){ goto _LL60;} else{ goto _LL61;}} else{ goto _LL61;}} else{ goto _LL61;}
_LL61: if((( struct _enum_struct*) _temp49)->tag == Cyc_Absyn_Sizeof_e_tag){
_LL152: _temp151=( void*)(( struct Cyc_Absyn_Sizeof_e_struct*) _temp49)->f1;
goto _LL62;} else{ goto _LL63;} _LL63: if((( struct _enum_struct*) _temp49)->tag
== Cyc_Absyn_Const_e_tag){ _LL154: _temp153=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp49)->f1; goto _LL64;} else{ goto _LL65;} _LL65: if((( struct _enum_struct*)
_temp49)->tag == Cyc_Absyn_Var_e_tag){ _LL160: _temp159=( struct _tuple1*)((
struct Cyc_Absyn_Var_e_struct*) _temp49)->f1; goto _LL156; _LL156: _temp155=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp49)->f2; if(( unsigned int)
_temp155 > 1u?(( struct _enum_struct*) _temp155)->tag == Cyc_Absyn_Funname_b_tag:
0){ _LL158: _temp157=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp155)->f1; goto _LL66;} else{ goto _LL67;}} else{ goto _LL67;} _LL67: if(((
struct _enum_struct*) _temp49)->tag == Cyc_Absyn_Var_e_tag){ _LL166: _temp165=(
struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp49)->f1; goto _LL162;
_LL162: _temp161=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp49)->f2; if((
unsigned int) _temp161 > 1u?(( struct _enum_struct*) _temp161)->tag == Cyc_Absyn_Global_b_tag:
0){ _LL164: _temp163=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*)
_temp161)->f1; goto _LL68;} else{ goto _LL69;}} else{ goto _LL69;} _LL69: if(((
struct _enum_struct*) _temp49)->tag == Cyc_Absyn_Var_e_tag){ _LL172: _temp171=(
struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp49)->f1; goto _LL168;
_LL168: _temp167=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp49)->f2; if((
unsigned int) _temp167 > 1u?(( struct _enum_struct*) _temp167)->tag == Cyc_Absyn_Param_b_tag:
0){ _LL170: _temp169=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Param_b_struct*)
_temp167)->f1; goto _LL70;} else{ goto _LL71;}} else{ goto _LL71;} _LL71: if(((
struct _enum_struct*) _temp49)->tag == Cyc_Absyn_Var_e_tag){ _LL178: _temp177=(
struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp49)->f1; goto _LL174;
_LL174: _temp173=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp49)->f2; if((
unsigned int) _temp173 > 1u?(( struct _enum_struct*) _temp173)->tag == Cyc_Absyn_Local_b_tag:
0){ _LL176: _temp175=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Local_b_struct*)
_temp173)->f1; goto _LL72;} else{ goto _LL73;}} else{ goto _LL73;} _LL73: if(((
struct _enum_struct*) _temp49)->tag == Cyc_Absyn_Var_e_tag){ _LL184: _temp183=(
struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp49)->f1; goto _LL180;
_LL180: _temp179=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp49)->f2; if((
unsigned int) _temp179 > 1u?(( struct _enum_struct*) _temp179)->tag == Cyc_Absyn_Pat_b_tag:
0){ _LL182: _temp181=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Pat_b_struct*)
_temp179)->f1; goto _LL74;} else{ goto _LL75;}} else{ goto _LL75;} _LL75: if(((
struct _enum_struct*) _temp49)->tag == Cyc_Absyn_Enum_e_tag){ _LL194: _temp193=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp49)->f1; goto
_LL192; _LL192: _temp191=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp49)->f2; goto _LL190; _LL190: _temp189=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp49)->f3; goto _LL188; _LL188: _temp187=( struct
Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp49)->f4; goto
_LL186; _LL186: _temp185=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp49)->f5; goto _LL76;} else{ goto _LL77;} _LL77: if((( struct _enum_struct*)
_temp49)->tag == Cyc_Absyn_Xenum_e_tag){ _LL202: _temp201=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp49)->f1; goto _LL200; _LL200: _temp199=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp49)->f2; goto
_LL198; _LL198: _temp197=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp49)->f3; goto _LL196; _LL196: _temp195=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp49)->f4; goto _LL78;} else{ goto _LL79;}
_LL79: if((( struct _enum_struct*) _temp49)->tag == Cyc_Absyn_AssignOp_e_tag){
_LL208: _temp207=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp49)->f1; goto _LL206; _LL206: _temp205=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp49)->f2; if( _temp205 == 0){ goto _LL204;} else{ goto _LL81;} _LL204:
_temp203=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp49)->f3;
goto _LL80;} else{ goto _LL81;} _LL81: if((( struct _enum_struct*) _temp49)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL214: _temp213=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp49)->f1; goto _LL212; _LL212: _temp211=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp49)->f2; goto
_LL210; _LL210: _temp209=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp49)->f3; goto _LL82;} else{ goto _LL83;} _LL83: if((( struct _enum_struct*)
_temp49)->tag == Cyc_Absyn_Primop_e_tag){ _LL218: _temp217=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp49)->f1; goto _LL216; _LL216: _temp215=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Primop_e_struct*) _temp49)->f2; goto _LL84;} else{ goto _LL85;} _LL85:
if((( struct _enum_struct*) _temp49)->tag == Cyc_Absyn_FnCall_e_tag){ _LL222:
_temp221=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp49)->f1;
goto _LL220; _LL220: _temp219=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp49)->f2; goto _LL86;} else{ goto _LL87;} _LL87: if((( struct _enum_struct*)
_temp49)->tag == Cyc_Absyn_Subscript_e_tag){ _LL226: _temp225=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp49)->f1; goto _LL224; _LL224:
_temp223=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp49)->f2; goto _LL88;} else{ goto _LL89;} _LL89: if((( struct _enum_struct*)
_temp49)->tag == Cyc_Absyn_Address_e_tag){ _LL228: _temp227=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp49)->f1; goto _LL90;} else{ goto _LL91;}
_LL91: if((( struct _enum_struct*) _temp49)->tag == Cyc_Absyn_Deref_e_tag){
_LL230: _temp229=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp49)->f1; goto _LL92;} else{ goto _LL93;} _LL93: if((( struct _enum_struct*)
_temp49)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL234: _temp233=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp49)->f1; goto _LL232; _LL232:
_temp231=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp49)->f2; goto _LL94;} else{ goto _LL95;} _LL95: if((( struct _enum_struct*)
_temp49)->tag == Cyc_Absyn_StructMember_e_tag){ _LL238: _temp237=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp49)->f1; goto _LL236; _LL236:
_temp235=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp49)->f2; goto _LL96;} else{ goto _LL97;} _LL97: if((( struct _enum_struct*)
_temp49)->tag == Cyc_Absyn_Tuple_e_tag){ _LL240: _temp239=( struct Cyc_List_List*)((
struct Cyc_Absyn_Tuple_e_struct*) _temp49)->f1; goto _LL98;} else{ goto _LL99;}
_LL99: if((( struct _enum_struct*) _temp49)->tag == Cyc_Absyn_Struct_e_tag){
_LL248: _temp247=( struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp49)->f1;
goto _LL246; _LL246: _temp245=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp49)->f2; goto _LL244; _LL244: _temp243=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp49)->f3; goto _LL242; _LL242: _temp241=( struct
Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp49)->f4; goto
_LL100;} else{ goto _LL101;} _LL101: if((( struct _enum_struct*) _temp49)->tag
== Cyc_Absyn_Array_e_tag){ _LL252: _temp251=( int)(( struct Cyc_Absyn_Array_e_struct*)
_temp49)->f1; goto _LL250; _LL250: _temp249=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Array_e_struct*) _temp49)->f2; goto _LL102;} else{ goto _LL103;}
_LL103: if((( struct _enum_struct*) _temp49)->tag == Cyc_Absyn_Increment_e_tag){
_LL256: _temp255=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp49)->f1; goto _LL254; _LL254: _temp253=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp49)->f2; goto _LL104;} else{ goto _LL105;} _LL105: if((( struct
_enum_struct*) _temp49)->tag == Cyc_Absyn_Throw_e_tag){ _LL258: _temp257=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp49)->f1; goto
_LL106;} else{ goto _LL107;} _LL107: if((( struct _enum_struct*) _temp49)->tag
== Cyc_Absyn_Conditional_e_tag){ _LL264: _temp263=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Conditional_e_struct*) _temp49)->f1; goto _LL262; _LL262:
_temp261=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp49)->f2; goto _LL260; _LL260: _temp259=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp49)->f3; goto _LL108;} else{ goto _LL109;}
_LL109: if((( struct _enum_struct*) _temp49)->tag == Cyc_Absyn_StmtExp_e_tag){
_LL266: _temp265=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp49)->f1; goto _LL110;} else{ goto _LL111;} _LL111: if((( struct
_enum_struct*) _temp49)->tag == Cyc_Absyn_SeqExp_e_tag){ _LL270: _temp269=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp49)->f1; goto
_LL268; _LL268: _temp267=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp49)->f2; goto _LL112;} else{ goto _LL113;} _LL113: if((( struct
_enum_struct*) _temp49)->tag == Cyc_Absyn_Comprehension_e_tag){ _LL276: _temp275=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp49)->f1;
goto _LL274; _LL274: _temp273=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp49)->f2; goto _LL272; _LL272: _temp271=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp49)->f3; goto _LL114;} else{ goto _LL115;}
_LL115: if((( struct _enum_struct*) _temp49)->tag == Cyc_Absyn_Var_e_tag){
_LL280: _temp279=( struct _tuple1*)(( struct Cyc_Absyn_Var_e_struct*) _temp49)->f1;
goto _LL278; _LL278: _temp277=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp49)->f2;
if( _temp277 == Cyc_Absyn_Unresolved_b){ goto _LL116;} else{ goto _LL117;}}
else{ goto _LL117;} _LL117: if((( struct _enum_struct*) _temp49)->tag == Cyc_Absyn_UnknownId_e_tag){
_LL282: _temp281=( struct _tuple1*)(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp49)->f1; goto _LL118;} else{ goto _LL119;} _LL119: if((( struct
_enum_struct*) _temp49)->tag == Cyc_Absyn_UnknownCall_e_tag){ _LL286: _temp285=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp49)->f1;
goto _LL284; _LL284: _temp283=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp49)->f2; goto _LL120;} else{ goto _LL121;} _LL121: if((( struct
_enum_struct*) _temp49)->tag == Cyc_Absyn_UnresolvedMem_e_tag){ _LL290: _temp289=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp49)->f1;
goto _LL288; _LL288: _temp287=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp49)->f2; goto _LL122;} else{ goto _LL123;} _LL123: if((( struct
_enum_struct*) _temp49)->tag == Cyc_Absyn_CompoundLit_e_tag){ _LL294: _temp293=(
struct _tuple2*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp49)->f1; goto
_LL292; _LL292: _temp291=( struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp49)->f2; goto _LL124;} else{ goto _LL125;} _LL125: if((( struct
_enum_struct*) _temp49)->tag == Cyc_Absyn_Codegen_e_tag){ _LL296: _temp295=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Codegen_e_struct*) _temp49)->f1;
goto _LL126;} else{ goto _LL127;} _LL127: if((( struct _enum_struct*) _temp49)->tag
== Cyc_Absyn_Fill_e_tag){ _LL298: _temp297=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp49)->f1; goto _LL128;} else{ goto _LL50;} _LL52: _temp133= _temp129; goto
_LL54; _LL54: return Cyc_NewControlFlow_abstract_exp( env, _temp133); _LL56: {
void* ansexp= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp135)); ansexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, ansexp, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()));
return({ struct Cyc_NewControlFlow_AbsSyn _temp299; _temp299.when_true=( void*)
ansexp; _temp299.when_false=( void*) ansexp; _temp299.lvalues= 0; _temp299;});}
_LL58: env.access_path == 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)((
struct _tagged_string)({ char* _temp300=( char*)"new_control_flow.cyc:259 env.access_path==null";
struct _tagged_string _temp301; _temp301.curr= _temp300; _temp301.base= _temp300;
_temp301.last_plus_one= _temp300 + 47; _temp301;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp302; _temp302.when_true=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp302.when_false=(
void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp302.lvalues= 0; _temp302;}); _LL60: env.access_path == 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp303=( char*)"new_control_flow.cyc:262 env.access_path==null"; struct
_tagged_string _temp304; _temp304.curr= _temp303; _temp304.base= _temp303;
_temp304.last_plus_one= _temp303 + 47; _temp304;})); return({ struct Cyc_NewControlFlow_AbsSyn
_temp305; _temp305.when_true=( void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp305.when_false=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp305.lvalues= 0;
_temp305;}); _LL62: goto _LL64; _LL64: goto _LL66; _LL66: env.access_path == 0?
0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp306=( char*)"new_control_flow.cyc:269 env.access_path==null";
struct _tagged_string _temp307; _temp307.curr= _temp306; _temp307.base= _temp306;
_temp307.last_plus_one= _temp306 + 47; _temp307;}));{ void* ansexp= Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); return({ struct Cyc_NewControlFlow_AbsSyn
_temp308; _temp308.when_true=( void*) ansexp; _temp308.when_false=( void*)
ansexp; _temp308.lvalues= 0; _temp308;});} _LL68: { void* ansexp= Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); return({ struct Cyc_NewControlFlow_AbsSyn
_temp309; _temp309.when_true=( void*) ansexp; _temp309.when_false=( void*)
ansexp; _temp309.lvalues= Cyc_NewControlFlow_use_it_list(); _temp309;});} _LL70:
_temp175= _temp169; goto _LL72; _LL72: _temp181= _temp175; goto _LL74; _LL74: {
void* absop= Cyc_CfAbsexp_mkLocalOp( _temp181);{ struct Cyc_List_List* path= env.access_path;
for( 0; path != 0; path= path->tl){ void* _temp310=( void*)(( struct Cyc_Absyn_Exp*)
path->hd)->r; struct Cyc_Absyn_Exp* _temp324; struct Cyc_Absyn_Exp* _temp326;
struct _tagged_string* _temp328; struct Cyc_Absyn_Exp* _temp330; struct
_tagged_string* _temp332; struct Cyc_Absyn_Exp* _temp334; struct Cyc_Absyn_Exp*
_temp336; struct Cyc_Absyn_Exp* _temp338; _LL312: if((( struct _enum_struct*)
_temp310)->tag == Cyc_Absyn_Address_e_tag){ _LL325: _temp324=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Address_e_struct*) _temp310)->f1; goto _LL313;} else{ goto
_LL314;} _LL314: if((( struct _enum_struct*) _temp310)->tag == Cyc_Absyn_Deref_e_tag){
_LL327: _temp326=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp310)->f1; goto _LL315;} else{ goto _LL316;} _LL316: if((( struct
_enum_struct*) _temp310)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL331: _temp330=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp310)->f1;
goto _LL329; _LL329: _temp328=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp310)->f2; goto _LL317;} else{ goto _LL318;} _LL318: if((( struct
_enum_struct*) _temp310)->tag == Cyc_Absyn_StructMember_e_tag){ _LL335: _temp334=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*) _temp310)->f1;
goto _LL333; _LL333: _temp332=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp310)->f2; goto _LL319;} else{ goto _LL320;} _LL320: if((( struct
_enum_struct*) _temp310)->tag == Cyc_Absyn_Subscript_e_tag){ _LL339: _temp338=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp310)->f1;
goto _LL337; _LL337: _temp336=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp310)->f2; goto _LL321;} else{ goto _LL322;} _LL322: goto _LL323; _LL313:
path->tl == 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)((
struct _tagged_string)({ char* _temp340=( char*)"new_control_flow.cyc:290 path->tl == null";
struct _tagged_string _temp341; _temp341.curr= _temp340; _temp341.base= _temp340;
_temp341.last_plus_one= _temp340 + 42; _temp341;}));{ void* rval= Cyc_CfAbsexp_mkAddressOp(
absop); void* ae= Cyc_NewControlFlow_make_assigns( env, rval); return({ struct
Cyc_NewControlFlow_AbsSyn _temp342; _temp342.when_true=( void*) ae; _temp342.when_false=(
void*) ae; _temp342.lvalues= 0; _temp342;});} _LL315: absop= Cyc_CfAbsexp_mkDerefOp(
absop); goto _LL311; _LL317: absop= Cyc_CfAbsexp_mkDerefOp( absop); _temp332=
_temp328; goto _LL319; _LL319: absop= Cyc_CfAbsexp_mkMemberOp( absop,({ struct
Cyc_CfFlowInfo_StructF_struct* _temp343=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp343->tag= Cyc_CfFlowInfo_StructF_tag;
_temp343->f1= _temp332;( void*) _temp343;})); goto _LL311; _LL321: {
unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp336); absop= Cyc_CfAbsexp_mkMemberOp(
absop,({ struct Cyc_CfFlowInfo_TupleF_struct* _temp344=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc_atomic( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp344->tag=
Cyc_CfFlowInfo_TupleF_tag; _temp344->f1=( int) i;( void*) _temp344;})); goto
_LL311;} _LL323:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp345=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp345=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp346=( char*)"abstract_exp: unexpected acces path element";
struct _tagged_string _temp347; _temp347.curr= _temp346; _temp347.base= _temp346;
_temp347.last_plus_one= _temp346 + 44; _temp347;})};( struct _xenum_struct*)
_temp345;})); _LL311:;}}{ void* ae= Cyc_NewControlFlow_make_assigns( env, absop);
return({ struct Cyc_NewControlFlow_AbsSyn _temp348; _temp348.when_true=( void*)
ae; _temp348.when_false=( void*) ae; _temp348.lvalues=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp349=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp349[ 0]=({ struct Cyc_List_List _temp350;
_temp350.hd=( void*) absop; _temp350.tl= 0; _temp350;}); _temp349;}); _temp348;});}}
_LL76: _temp199= _temp189; _temp195= _temp185; goto _LL78; _LL78: env.access_path
== 0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp351=( char*)"new_control_flow.cyc:312 env.access_path == null";
struct _tagged_string _temp352; _temp352.curr= _temp351; _temp352.base= _temp351;
_temp352.last_plus_one= _temp351 + 49; _temp352;})); if( _temp199 == 0){ void*
ansexp= Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
return({ struct Cyc_NewControlFlow_AbsSyn _temp353; _temp353.when_true=( void*)
ansexp; _temp353.when_false=( void*) ansexp; _temp353.lvalues= 0; _temp353;});}
Cyc_NewControlFlow_add_enum_root( env, e, _temp195);{ void* malloc_op= Cyc_CfAbsexp_mkMallocOp(
e); void* ansexp= Cyc_CfAbsexp_mkSkipAE();{ int i= 0; for( 0; _temp199 != 0;
_temp199= _temp199->tl, ++ i){ void* lhs= Cyc_CfAbsexp_mkMemberOp( malloc_op,({
struct Cyc_CfFlowInfo_TupleF_struct* _temp354=( struct Cyc_CfFlowInfo_TupleF_struct*)
GC_malloc_atomic( sizeof( struct Cyc_CfFlowInfo_TupleF_struct)); _temp354->tag=
Cyc_CfFlowInfo_TupleF_tag; _temp354->f1= i;( void*) _temp354;})); struct Cyc_NewControlFlow_AbsEnv
field_env= Cyc_NewControlFlow_change_lhs( env,( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp355=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)
* 1); _temp355[ 0]=({ struct Cyc_List_List _temp356; _temp356.hd=( void*) lhs;
_temp356.tl= 0; _temp356;}); _temp355;})); ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( field_env,(
struct Cyc_Absyn_Exp*) _temp199->hd)), ansexp);}} ansexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkMallocAE( e), ansexp); ansexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, ansexp, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkAddressOp(
malloc_op))); return({ struct Cyc_NewControlFlow_AbsSyn _temp357; _temp357.when_true=(
void*) ansexp; _temp357.when_false=( void*) ansexp; _temp357.lvalues= 0;
_temp357;});} _LL80: { struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_change_lhs( env, 0), _temp207); struct Cyc_List_List* lhs=
syn1.lvalues == 0? Cyc_NewControlFlow_use_it_list(): syn1.lvalues; struct Cyc_NewControlFlow_AbsSyn
syn2= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env, lhs),
_temp203); struct Cyc_List_List* assigns= 0; for( 0; lhs != 0; lhs= lhs->tl){
assigns=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp358=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp358[ 0]=({ struct Cyc_List_List
_temp359; _temp359.hd=( void*) Cyc_NewControlFlow_make_assigns( env,( void*) lhs->hd);
_temp359.tl= assigns; _temp359;}); _temp358;});}{ void* ansexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, Cyc_NewControlFlow_meet_absexp(
syn1), Cyc_NewControlFlow_meet_absexp( syn2)), Cyc_CfAbsexp_mkGroupAE_l( Cyc_CfAbsexp_OneofG,
assigns)); return({ struct Cyc_NewControlFlow_AbsSyn _temp360; _temp360.when_true=(
void*) ansexp; _temp360.when_false=( void*) ansexp; _temp360.lvalues= 0;
_temp360;});}} _LL82: return Cyc_NewControlFlow_use_exp_unordered( env,( struct
Cyc_List_List*)({ struct Cyc_List_List* _temp361=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp361[ 0]=({ struct Cyc_List_List
_temp362; _temp362.hd=( void*) _temp213; _temp362.tl=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp363=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp363[ 0]=({ struct Cyc_List_List _temp364;
_temp364.hd=( void*) _temp209; _temp364.tl= 0; _temp364;}); _temp363;});
_temp362;}); _temp361;}), 0); _LL84: return Cyc_NewControlFlow_use_exp_unordered(
env, _temp215, 0); _LL86: return Cyc_NewControlFlow_use_exp_unordered( env,(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp365=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp365[ 0]=({ struct Cyc_List_List
_temp366; _temp366.hd=( void*) _temp221; _temp366.tl= _temp219; _temp366;});
_temp365;}), 0); _LL88: { void* _temp367= Cyc_Tcutil_compress(( void*)( _temp225->topt)->v);
struct Cyc_List_List* _temp373; _LL369: if(( unsigned int) _temp367 > 5u?((
struct _enum_struct*) _temp367)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL374:
_temp373=( struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp367)->f1;
goto _LL370;} else{ goto _LL371;} _LL371: goto _LL372; _LL370: env.access_path=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp375=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp375[ 0]=({ struct Cyc_List_List
_temp376; _temp376.hd=( void*) e; _temp376.tl= env.access_path; _temp376;});
_temp375;}); return Cyc_NewControlFlow_abstract_exp( env, _temp225); _LL372:
return Cyc_NewControlFlow_use_exp_unordered( env,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp377=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp377[ 0]=({ struct Cyc_List_List _temp378;
_temp378.hd=( void*) _temp225; _temp378.tl=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp379=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp379[ 0]=({ struct Cyc_List_List _temp380; _temp380.hd=( void*) _temp223;
_temp380.tl= 0; _temp380;}); _temp379;}); _temp378;}); _temp377;}), Cyc_NewControlFlow_use_it_list());
_LL368:;} _LL90:{ void* _temp381=( void*) _temp227->r; struct Cyc_Absyn_Structdecl*
_temp389; struct Cyc_List_List* _temp391; struct Cyc_Core_Opt* _temp393; struct
_tuple1* _temp395; struct Cyc_List_List* _temp397; _LL383: if((( struct
_enum_struct*) _temp381)->tag == Cyc_Absyn_Struct_e_tag){ _LL396: _temp395=(
struct _tuple1*)(( struct Cyc_Absyn_Struct_e_struct*) _temp381)->f1; goto _LL394;
_LL394: _temp393=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp381)->f2; goto _LL392; _LL392: _temp391=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp381)->f3; goto _LL390; _LL390: _temp389=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*) _temp381)->f4;
goto _LL384;} else{ goto _LL385;} _LL385: if((( struct _enum_struct*) _temp381)->tag
== Cyc_Absyn_Tuple_e_tag){ _LL398: _temp397=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Tuple_e_struct*) _temp381)->f1; goto _LL386;} else{ goto _LL387;}
_LL387: goto _LL388; _LL384: goto _LL386; _LL386: { void* malloc_op= Cyc_CfAbsexp_mkMallocOp(
e); Cyc_NewControlFlow_add_malloc_root( env, e,( void*)( _temp227->topt)->v);{
struct Cyc_NewControlFlow_AbsEnv inner_env= Cyc_NewControlFlow_change_lhs( env,(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp399=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp399[ 0]=({ struct Cyc_List_List
_temp400; _temp400.hd=( void*) malloc_op; _temp400.tl= 0; _temp400;}); _temp399;}));
inner_env.access_path= 0;{ void* ansexp= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
inner_env, _temp227)); ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkMallocAE( e), ansexp); ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,
ansexp, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkAddressOp(
malloc_op))); return({ struct Cyc_NewControlFlow_AbsSyn _temp401; _temp401.when_true=(
void*) ansexp; _temp401.when_false=( void*) ansexp; _temp401.lvalues= 0;
_temp401;});}}} _LL388: goto _LL382; _LL382:;}{ struct Cyc_List_List* _temp402=
env.access_path; struct Cyc_List_List _temp414; struct Cyc_List_List* _temp415;
struct Cyc_Absyn_Exp* _temp417; struct Cyc_Absyn_Exp _temp419; struct Cyc_Position_Segment*
_temp420; void* _temp422; struct Cyc_Absyn_Exp* _temp424; struct Cyc_Absyn_Exp*
_temp426; struct Cyc_Core_Opt* _temp428; struct Cyc_List_List _temp430; struct
Cyc_List_List* _temp431; struct Cyc_Absyn_Exp* _temp433; struct Cyc_Absyn_Exp
_temp435; struct Cyc_Position_Segment* _temp436; void* _temp438; struct Cyc_Absyn_Exp*
_temp440; struct Cyc_Core_Opt* _temp442; struct Cyc_List_List _temp444; struct
Cyc_List_List* _temp445; struct Cyc_Absyn_Exp* _temp447; struct Cyc_Absyn_Exp
_temp449; struct Cyc_Position_Segment* _temp450; void* _temp452; struct
_tagged_string* _temp454; struct Cyc_Absyn_Exp* _temp456; struct Cyc_Core_Opt*
_temp458; _LL404: if( _temp402 == 0){ goto _LL405;} else{ goto _LL406;} _LL406:
if( _temp402 == 0){ goto _LL408;} else{ _temp414=* _temp402; _LL418: _temp417=(
struct Cyc_Absyn_Exp*) _temp414.hd; _temp419=* _temp417; _LL429: _temp428=(
struct Cyc_Core_Opt*) _temp419.topt; goto _LL423; _LL423: _temp422=( void*)
_temp419.r; if((( struct _enum_struct*) _temp422)->tag == Cyc_Absyn_Subscript_e_tag){
_LL427: _temp426=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp422)->f1; goto _LL425; _LL425: _temp424=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Subscript_e_struct*) _temp422)->f2; goto _LL421;} else{ goto _LL408;}
_LL421: _temp420=( struct Cyc_Position_Segment*) _temp419.loc; goto _LL416;
_LL416: _temp415=( struct Cyc_List_List*) _temp414.tl; goto _LL407;} _LL408: if(
_temp402 == 0){ goto _LL410;} else{ _temp430=* _temp402; _LL434: _temp433=(
struct Cyc_Absyn_Exp*) _temp430.hd; _temp435=* _temp433; _LL443: _temp442=(
struct Cyc_Core_Opt*) _temp435.topt; goto _LL439; _LL439: _temp438=( void*)
_temp435.r; if((( struct _enum_struct*) _temp438)->tag == Cyc_Absyn_Deref_e_tag){
_LL441: _temp440=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp438)->f1; goto _LL437;} else{ goto _LL410;} _LL437: _temp436=( struct Cyc_Position_Segment*)
_temp435.loc; goto _LL432; _LL432: _temp431=( struct Cyc_List_List*) _temp430.tl;
goto _LL409;} _LL410: if( _temp402 == 0){ goto _LL412;} else{ _temp444=*
_temp402; _LL448: _temp447=( struct Cyc_Absyn_Exp*) _temp444.hd; _temp449=*
_temp447; _LL459: _temp458=( struct Cyc_Core_Opt*) _temp449.topt; goto _LL453;
_LL453: _temp452=( void*) _temp449.r; if((( struct _enum_struct*) _temp452)->tag
== Cyc_Absyn_StructArrow_e_tag){ _LL457: _temp456=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructArrow_e_struct*) _temp452)->f1; goto _LL455; _LL455:
_temp454=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp452)->f2; goto _LL451;} else{ goto _LL412;} _LL451: _temp450=( struct Cyc_Position_Segment*)
_temp449.loc; goto _LL446; _LL446: _temp445=( struct Cyc_List_List*) _temp444.tl;
goto _LL411;} _LL412: goto _LL413; _LL405: env.access_path=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp460=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp460[ 0]=({ struct Cyc_List_List _temp461;
_temp461.hd=( void*) e; _temp461.tl= env.access_path; _temp461;}); _temp460;});
goto _LL403; _LL407: Cyc_Evexp_eval_const_uint_exp( _temp424) ==( unsigned int)
0? 0:(( int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp462=( char*)"new_control_flow.cyc:408 Evexp::eval_const_uint_exp(e2) == 0";
struct _tagged_string _temp463; _temp463.curr= _temp462; _temp463.base= _temp462;
_temp463.last_plus_one= _temp462 + 61; _temp463;})); env.access_path= _temp415;
goto _LL403; _LL409: env.access_path= _temp431; goto _LL403; _LL411: env.access_path=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp464=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp464[ 0]=({ struct Cyc_List_List
_temp465; _temp465.hd=( void*) Cyc_Absyn_structmember_exp( _temp456, _temp454, 0);
_temp465.tl= _temp445; _temp465;}); _temp464;}); goto _LL403; _LL413:( void)
_throw(({ struct Cyc_Core_Impossible_struct* _temp466=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp466=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp467=( char*)"abstract_exp: bad access path for &";
struct _tagged_string _temp468; _temp468.curr= _temp467; _temp468.base= _temp467;
_temp468.last_plus_one= _temp467 + 36; _temp468;})};( struct _xenum_struct*)
_temp466;})); _LL403:;} return Cyc_NewControlFlow_abstract_exp( env, _temp227);
_LL92:{ struct Cyc_List_List* _temp469= env.access_path; struct Cyc_List_List
_temp475; struct Cyc_List_List* _temp476; struct Cyc_Absyn_Exp* _temp478; struct
Cyc_Absyn_Exp _temp480; struct Cyc_Position_Segment* _temp481; void* _temp483;
struct Cyc_Absyn_Exp* _temp485; struct Cyc_Core_Opt* _temp487; _LL471: if(
_temp469 == 0){ goto _LL473;} else{ _temp475=* _temp469; _LL479: _temp478=(
struct Cyc_Absyn_Exp*) _temp475.hd; _temp480=* _temp478; _LL488: _temp487=(
struct Cyc_Core_Opt*) _temp480.topt; goto _LL484; _LL484: _temp483=( void*)
_temp480.r; if((( struct _enum_struct*) _temp483)->tag == Cyc_Absyn_Address_e_tag){
_LL486: _temp485=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*)
_temp483)->f1; goto _LL482;} else{ goto _LL473;} _LL482: _temp481=( struct Cyc_Position_Segment*)
_temp480.loc; goto _LL477; _LL477: _temp476=( struct Cyc_List_List*) _temp475.tl;
goto _LL472;} _LL473: goto _LL474; _LL472: env.access_path= _temp476;{ struct
Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp( env, _temp229);
if( syn1.lvalues == 0){ syn1.lvalues= Cyc_NewControlFlow_use_it_list();} return
syn1;} _LL474: goto _LL470; _LL470:;} _temp233= _temp229; goto _LL94; _LL94:
_temp237= _temp233; goto _LL96; _LL96: env.access_path=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp489=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp489[ 0]=({ struct Cyc_List_List _temp490;
_temp490.hd=( void*) e; _temp490.tl= env.access_path; _temp490;}); _temp489;});{
struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp( env,
_temp237); if( syn1.lvalues == 0){ syn1.lvalues= Cyc_NewControlFlow_use_it_list();}
return syn1;} _LL98: { struct Cyc_List_List* _temp491= env.access_path; struct
Cyc_List_List _temp499; struct Cyc_List_List* _temp500; struct Cyc_Absyn_Exp*
_temp502; struct Cyc_Absyn_Exp _temp504; struct Cyc_Position_Segment* _temp505;
void* _temp507; struct Cyc_Absyn_Exp* _temp509; struct Cyc_Absyn_Exp* _temp511;
struct Cyc_Core_Opt* _temp513; _LL493: if( _temp491 == 0){ goto _LL495;} else{
_temp499=* _temp491; _LL503: _temp502=( struct Cyc_Absyn_Exp*) _temp499.hd;
_temp504=* _temp502; _LL514: _temp513=( struct Cyc_Core_Opt*) _temp504.topt;
goto _LL508; _LL508: _temp507=( void*) _temp504.r; if((( struct _enum_struct*)
_temp507)->tag == Cyc_Absyn_Subscript_e_tag){ _LL512: _temp511=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Subscript_e_struct*) _temp507)->f1; goto _LL510; _LL510:
_temp509=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp507)->f2; goto _LL506;} else{ goto _LL495;} _LL506: _temp505=( struct Cyc_Position_Segment*)
_temp504.loc; goto _LL501; _LL501: _temp500=( struct Cyc_List_List*) _temp499.tl;
goto _LL494;} _LL495: if( _temp491 == 0){ goto _LL496;} else{ goto _LL497;}
_LL497: goto _LL498; _LL494: { int i=( int) Cyc_Evexp_eval_const_uint_exp(
_temp509); struct Cyc_NewControlFlow_AbsEnv other_env= Cyc_NewControlFlow_change_lhs(
env, 0); other_env.access_path= 0; env.access_path= _temp500;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE();{
int j= 0; for( 0; _temp239 != 0; _temp239= _temp239->tl, ++ j){ void* next= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( i == j? env: other_env,( struct Cyc_Absyn_Exp*)
_temp239->hd)); ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, next,
ansexp);}} return({ struct Cyc_NewControlFlow_AbsSyn _temp515; _temp515.when_true=(
void*) ansexp; _temp515.when_false=( void*) ansexp; _temp515.lvalues= 0;
_temp515;});}} _LL496: { void* ansexp= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for(
0; _temp239 != 0; _temp239= _temp239->tl, ++ j){ struct Cyc_List_List* ith_lvals=
0;{ struct Cyc_List_List* ls= env.lhsides; for( 0; ls != 0; ls= ls->tl){ if( Cyc_CfAbsexp_isUnknownOp((
void*) ls->hd)){ ith_lvals=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp516=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp516[ 0]=({ struct Cyc_List_List _temp517; _temp517.hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp517.tl= ith_lvals; _temp517;}); _temp516;});} else{ ith_lvals=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp518=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp518[ 0]=({ struct Cyc_List_List _temp519;
_temp519.hd=( void*) Cyc_CfAbsexp_mkMemberOp(( void*) ls->hd,({ struct Cyc_CfFlowInfo_TupleF_struct*
_temp520=( struct Cyc_CfFlowInfo_TupleF_struct*) GC_malloc_atomic( sizeof(
struct Cyc_CfFlowInfo_TupleF_struct)); _temp520->tag= Cyc_CfFlowInfo_TupleF_tag;
_temp520->f1= j;( void*) _temp520;})); _temp519.tl= ith_lvals; _temp519;});
_temp518;});}}}{ void* next= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_change_lhs( env, ith_lvals),( struct Cyc_Absyn_Exp*) _temp239->hd));
ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG, next, ansexp);}}}
return({ struct Cyc_NewControlFlow_AbsSyn _temp521; _temp521.when_true=( void*)
ansexp; _temp521.when_false=( void*) ansexp; _temp521.lvalues= 0; _temp521;});}
_LL498:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp522=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp522=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp523=( char*)"abstract_exp: unexpected access path to Tuple";
struct _tagged_string _temp524; _temp524.curr= _temp523; _temp524.base= _temp523;
_temp524.last_plus_one= _temp523 + 46; _temp524;})};( struct _xenum_struct*)
_temp522;})); _LL492:;} _LL100: { struct Cyc_List_List* _temp525= env.access_path;
struct Cyc_List_List _temp533; struct Cyc_List_List* _temp534; struct Cyc_Absyn_Exp*
_temp536; struct Cyc_Absyn_Exp _temp538; struct Cyc_Position_Segment* _temp539;
void* _temp541; struct _tagged_string* _temp543; struct Cyc_Absyn_Exp* _temp545;
struct Cyc_Core_Opt* _temp547; _LL527: if( _temp525 == 0){ goto _LL529;} else{
_temp533=* _temp525; _LL537: _temp536=( struct Cyc_Absyn_Exp*) _temp533.hd;
_temp538=* _temp536; _LL548: _temp547=( struct Cyc_Core_Opt*) _temp538.topt;
goto _LL542; _LL542: _temp541=( void*) _temp538.r; if((( struct _enum_struct*)
_temp541)->tag == Cyc_Absyn_StructMember_e_tag){ _LL546: _temp545=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp541)->f1; goto _LL544; _LL544:
_temp543=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp541)->f2; goto _LL540;} else{ goto _LL529;} _LL540: _temp539=( struct Cyc_Position_Segment*)
_temp538.loc; goto _LL535; _LL535: _temp534=( struct Cyc_List_List*) _temp533.tl;
goto _LL528;} _LL529: if( _temp525 == 0){ goto _LL530;} else{ goto _LL531;}
_LL531: goto _LL532; _LL528: { struct Cyc_NewControlFlow_AbsEnv other_env= Cyc_NewControlFlow_change_lhs(
env, 0); other_env.access_path= 0; env.access_path= _temp534;{ void* ansexp= Cyc_CfAbsexp_mkSkipAE();
for( 0; _temp243 != 0; _temp243= _temp243->tl){ int used= 0;{ struct Cyc_List_List*
ds=(*(( struct _tuple5*) _temp243->hd)).f1; for( 0; ds != 0; ds= ds->tl){ void*
_temp549=( void*) ds->hd; struct Cyc_Absyn_Exp* _temp555; struct _tagged_string*
_temp557; _LL551: if((( struct _enum_struct*) _temp549)->tag == Cyc_Absyn_ArrayElement_tag){
_LL556: _temp555=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp549)->f1; goto _LL552;} else{ goto _LL553;} _LL553: if((( struct
_enum_struct*) _temp549)->tag == Cyc_Absyn_FieldName_tag){ _LL558: _temp557=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp549)->f1;
goto _LL554;} else{ goto _LL550;} _LL552:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp559=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp559=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp560=( char*)"bad struct designator"; struct
_tagged_string _temp561; _temp561.curr= _temp560; _temp561.base= _temp560;
_temp561.last_plus_one= _temp560 + 22; _temp561;})};( struct _xenum_struct*)
_temp559;})); _LL554: if( Cyc_String_zstrptrcmp( _temp543, _temp557) == 0){ used=
1;} goto _LL550; _LL550:;}}{ void* next= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
used? env: other_env,(*(( struct _tuple5*) _temp243->hd)).f2)); ansexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_UnorderedG, next, ansexp);}} return({ struct Cyc_NewControlFlow_AbsSyn
_temp562; _temp562.when_true=( void*) ansexp; _temp562.when_false=( void*)
ansexp; _temp562.lvalues= 0; _temp562;});}} _LL530: { void* ansexp= Cyc_CfAbsexp_mkSkipAE();
for( 0; _temp243 != 0; _temp243= _temp243->tl){ struct Cyc_List_List* ith_lvals=
0;{ struct Cyc_List_List* ls= env.lhsides; for( 0; ls != 0; ls= ls->tl){ if( Cyc_CfAbsexp_isUnknownOp((
void*) ls->hd)){ ith_lvals=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp563=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp563[ 0]=({ struct Cyc_List_List _temp564; _temp564.hd=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp564.tl= ith_lvals; _temp564;}); _temp563;});} else{ struct Cyc_List_List*
ds=(*(( struct _tuple5*) _temp243->hd)).f1; for( 0; ds != 0; ds= ds->tl){ void*
_temp565=( void*) ds->hd; struct Cyc_Absyn_Exp* _temp571; struct _tagged_string*
_temp573; _LL567: if((( struct _enum_struct*) _temp565)->tag == Cyc_Absyn_ArrayElement_tag){
_LL572: _temp571=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayElement_struct*)
_temp565)->f1; goto _LL568;} else{ goto _LL569;} _LL569: if((( struct
_enum_struct*) _temp565)->tag == Cyc_Absyn_FieldName_tag){ _LL574: _temp573=(
struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*) _temp565)->f1;
goto _LL570;} else{ goto _LL566;} _LL568:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp575=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp575=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp576=( char*)"bad struct designator"; struct
_tagged_string _temp577; _temp577.curr= _temp576; _temp577.base= _temp576;
_temp577.last_plus_one= _temp576 + 22; _temp577;})};( struct _xenum_struct*)
_temp575;})); _LL570: ith_lvals=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp578=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp578[ 0]=({ struct Cyc_List_List _temp579; _temp579.hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*) ls->hd,({ struct Cyc_CfFlowInfo_StructF_struct* _temp580=( struct Cyc_CfFlowInfo_StructF_struct*)
GC_malloc( sizeof( struct Cyc_CfFlowInfo_StructF_struct)); _temp580->tag= Cyc_CfFlowInfo_StructF_tag;
_temp580->f1= _temp573;( void*) _temp580;})); _temp579.tl= ith_lvals; _temp579;});
_temp578;}); goto _LL566; _LL566:;}}}}{ void** ith_topt=( void*)&(((*(( struct
_tuple5*) _temp243->hd)).f2)->topt)->v; void* next= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env, ith_lvals),(*((
struct _tuple5*) _temp243->hd)).f2)); ansexp= Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_UnorderedG,
next, ansexp);}} return({ struct Cyc_NewControlFlow_AbsSyn _temp581; _temp581.when_true=(
void*) ansexp; _temp581.when_false=( void*) ansexp; _temp581.lvalues= 0;
_temp581;});} _LL532:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp582=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp582=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp583=( char*)"abstract_exp: unexpected access path to Struct";
struct _tagged_string _temp584; _temp584.curr= _temp583; _temp584.base= _temp583;
_temp584.last_plus_one= _temp583 + 47; _temp584;})};( struct _xenum_struct*)
_temp582;})); _LL526:;} _LL102: return Cyc_NewControlFlow_use_exp_unordered( env,((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple5*), struct
Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct _tuple5*))
Cyc_Core_snd, _temp249), 0); _LL104: return Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env(
env), _temp255); _LL106: env= Cyc_NewControlFlow_use_it_env( env);{ struct Cyc_NewControlFlow_AbsSyn
esyn= Cyc_NewControlFlow_abstract_exp( env, _temp257); void* ansexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, Cyc_NewControlFlow_meet_absexp( esyn), Cyc_CfAbsexp_mkBottomAE());
return({ struct Cyc_NewControlFlow_AbsSyn _temp585; _temp585.when_true=( void*)
ansexp; _temp585.when_false=( void*) ansexp; _temp585.lvalues= esyn.lvalues;
_temp585;});} _LL108: { struct Cyc_NewControlFlow_AbsSyn e1syn= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp263); struct Cyc_NewControlFlow_AbsSyn
e2syn= Cyc_NewControlFlow_abstract_exp( env, _temp261); struct Cyc_NewControlFlow_AbsSyn
e3syn= Cyc_NewControlFlow_abstract_exp( env, _temp259); void* ans_true_exp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,( void*)
e1syn.when_true,( void*) e2syn.when_true), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) e1syn.when_false,( void*) e3syn.when_true)); void* ans_false_exp; if((
void*) e2syn.when_true ==( void*) e2syn.when_true?( void*) e3syn.when_true ==(
void*) e3syn.when_false: 0){ ans_false_exp= ans_true_exp;} else{ ans_false_exp=
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) e1syn.when_true,( void*) e2syn.when_false), Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG,(
void*) e1syn.when_false,( void*) e3syn.when_false));}{ struct Cyc_List_List*
lvalues=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( e2syn.lvalues, e3syn.lvalues); if(( e2syn.lvalues == 0? !((
int(*)( struct Cyc_List_List* l, void* x)) Cyc_List_memq)( e3syn.lvalues, Cyc_CfAbsexp_mkUnknownOp()):
0)? 1:( e3syn.lvalues == 0? !(( int(*)( struct Cyc_List_List* l, void* x)) Cyc_List_memq)(
e2syn.lvalues, Cyc_CfAbsexp_mkUnknownOp()): 0)){ lvalues=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp586=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp586[ 0]=({ struct Cyc_List_List _temp587;
_temp587.hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp587.tl= lvalues; _temp587;});
_temp586;});} return({ struct Cyc_NewControlFlow_AbsSyn _temp588; _temp588.when_true=(
void*) ans_true_exp; _temp588.when_false=( void*) ans_false_exp; _temp588.lvalues=
lvalues; _temp588;});}} _LL110: { struct Cyc_List_List* access_path= env.access_path;
struct Cyc_NewControlFlow_AbsEnv env2= Cyc_NewControlFlow_change_lhs( env, 0);
env2.access_path= 0; Cyc_NewControlFlow_cf_prepass( env2, _temp265); while( 1) {
void* _temp589=( void*) _temp265->r; struct Cyc_Absyn_Stmt* _temp599; struct Cyc_Absyn_Stmt*
_temp601; struct Cyc_Absyn_Stmt* _temp603; struct Cyc_Absyn_Decl* _temp605;
struct Cyc_Absyn_Exp* _temp607; _LL591: if(( unsigned int) _temp589 > 1u?((
struct _enum_struct*) _temp589)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL602:
_temp601=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp589)->f1;
goto _LL600; _LL600: _temp599=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp589)->f2; goto _LL592;} else{ goto _LL593;} _LL593: if(( unsigned int)
_temp589 > 1u?(( struct _enum_struct*) _temp589)->tag == Cyc_Absyn_Decl_s_tag: 0){
_LL606: _temp605=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp589)->f1; goto _LL604; _LL604: _temp603=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp589)->f2; goto _LL594;} else{ goto _LL595;}
_LL595: if(( unsigned int) _temp589 > 1u?(( struct _enum_struct*) _temp589)->tag
== Cyc_Absyn_Exp_s_tag: 0){ _LL608: _temp607=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Exp_s_struct*) _temp589)->f1; goto _LL596;} else{ goto _LL597;} _LL597:
goto _LL598; _LL592: _temp265= _temp599; continue; _LL594: _temp265= _temp603;
continue; _LL596:( void*)((( Cyc_NewControlFlow_get_stmt_annot( _temp265))->absexps)->hd=(
void*) Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,
_temp607)));{ void* ae= Cyc_CfAbsexp_mkStmtAE( _temp265); return({ struct Cyc_NewControlFlow_AbsSyn
_temp609; _temp609.when_true=( void*) ae; _temp609.when_false=( void*) ae;
_temp609.lvalues= 0; _temp609;});} _LL598:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp610=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp610=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp611=( char*)"abstract_exp: ill-formed StmtExp";
struct _tagged_string _temp612; _temp612.curr= _temp611; _temp612.base= _temp611;
_temp612.last_plus_one= _temp611 + 33; _temp612;})};( struct _xenum_struct*)
_temp610;})); _LL590:;}} _LL112: { struct Cyc_NewControlFlow_AbsSyn syn2= Cyc_NewControlFlow_abstract_exp(
env, _temp267); env.access_path= 0;{ struct Cyc_NewControlFlow_AbsSyn syn1= Cyc_NewControlFlow_abstract_exp(
env, _temp269); void* ae1= Cyc_NewControlFlow_meet_absexp( syn1); return({
struct Cyc_NewControlFlow_AbsSyn _temp613; _temp613.when_true=( void*) Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, ae1,( void*) syn2.when_true); _temp613.when_false=( void*)
Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OrderedG, ae1,( void*) syn2.when_false);
_temp613.lvalues= 0; _temp613;});}} _LL114: env.access_path == 0? 0:(( int(*)(
struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({
char* _temp614=( char*)"new_control_flow.cyc:605 env.access_path==null"; struct
_tagged_string _temp615; _temp615.curr= _temp614; _temp615.base= _temp614;
_temp615.last_plus_one= _temp614 + 47; _temp615;})); Cyc_NewControlFlow_add_var_root(
env, _temp275); env= Cyc_NewControlFlow_use_it_env( env);{ void* ae1= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env, _temp273)); void* ae2= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env, _temp271)); void* ansexp= Cyc_CfAbsexp_mkGroupAE(
Cyc_CfAbsexp_OrderedG, ae1, Cyc_CfAbsexp_mkGroupAE( Cyc_CfAbsexp_OneofG, ae2,
Cyc_CfAbsexp_mkSkipAE())); return({ struct Cyc_NewControlFlow_AbsSyn _temp616;
_temp616.when_true=( void*) ansexp; _temp616.when_false=( void*) ansexp;
_temp616.lvalues= 0; _temp616;});} _LL116: goto _LL118; _LL118: goto _LL120;
_LL120: goto _LL122; _LL122: goto _LL124; _LL124: goto _LL126; _LL126: goto
_LL128; _LL128:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp617=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp617=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp618=( char*)"abstract_exp, unexpected exp form";
struct _tagged_string _temp619; _temp619.curr= _temp618; _temp619.base= _temp618;
_temp619.last_plus_one= _temp618 + 34; _temp619;})};( struct _xenum_struct*)
_temp617;})); _LL50:;} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
ans= 0; for( 0; scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause _temp622;
struct Cyc_Position_Segment* _temp623; struct Cyc_Absyn_Stmt* _temp625; struct
Cyc_Absyn_Exp* _temp627; struct Cyc_Core_Opt* _temp629; struct Cyc_Absyn_Pat*
_temp631; struct Cyc_Absyn_Switch_clause* _temp620=( struct Cyc_Absyn_Switch_clause*)
scs->hd; _temp622=* _temp620; _LL632: _temp631=( struct Cyc_Absyn_Pat*) _temp622.pattern;
goto _LL630; _LL630: _temp629=( struct Cyc_Core_Opt*) _temp622.pat_vars; goto
_LL628; _LL628: _temp627=( struct Cyc_Absyn_Exp*) _temp622.where_clause; goto
_LL626; _LL626: _temp625=( struct Cyc_Absyn_Stmt*) _temp622.body; goto _LL624;
_LL624: _temp623=( struct Cyc_Position_Segment*) _temp622.loc; goto _LL621;
_LL621: if( _temp629 == 0){( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp633=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp633=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp634=( char*)"switch clause vds not set";
struct _tagged_string _temp635; _temp635.curr= _temp634; _temp635.base= _temp634;
_temp635.last_plus_one= _temp634 + 26; _temp635;})};( struct _xenum_struct*)
_temp633;}));}{ struct Cyc_List_List* vds=( struct Cyc_List_List*) _temp629->v;
for( 0; vds != 0; vds= vds->tl){ Cyc_NewControlFlow_add_var_root( env,( struct
Cyc_Absyn_Vardecl*) vds->hd);}} if( _temp627 != 0){ struct Cyc_List_List*
_temp639; void* _temp641; void* _temp643; struct Cyc_NewControlFlow_AbsSyn
_temp637= Cyc_NewControlFlow_abstract_exp( env,( struct Cyc_Absyn_Exp*)({ struct
Cyc_Absyn_Exp* _temp636= _temp627; if( _temp636 == 0){ _throw( Null_Exception);}
_temp636;})); _LL644: _temp643=( void*) _temp637.when_true; goto _LL642; _LL642:
_temp641=( void*) _temp637.when_false; goto _LL640; _LL640: _temp639=( struct
Cyc_List_List*) _temp637.lvalues; goto _LL638; _LL638: ans=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp645=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp645[ 0]=({ struct Cyc_List_List _temp646;
_temp646.hd=( void*) _temp643; _temp646.tl=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp647=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp647[ 0]=({ struct Cyc_List_List _temp648; _temp648.hd=( void*) _temp641;
_temp648.tl= ans; _temp648;}); _temp647;}); _temp646;}); _temp645;});} Cyc_NewControlFlow_cf_prepass(
env, _temp625);} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( ans);} void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Stmt* s){ env.lhsides == 0? 0:(( int(*)( struct
_tagged_string msg)) Cyc_Assert_AssertFail)(( struct _tagged_string)({ char*
_temp649=( char*)"new_control_flow.cyc:645 env.lhsides == null"; struct
_tagged_string _temp650; _temp650.curr= _temp649; _temp650.base= _temp649;
_temp650.last_plus_one= _temp649 + 45; _temp650;})); env.access_path == 0? 0:((
int(*)( struct _tagged_string msg)) Cyc_Assert_AssertFail)(( struct
_tagged_string)({ char* _temp651=( char*)"new_control_flow.cyc:646 env.access_path == null";
struct _tagged_string _temp652; _temp652.curr= _temp651; _temp652.base= _temp651;
_temp652.last_plus_one= _temp651 + 49; _temp652;})); Cyc_NewControlFlow_cf_decorate_stmt(
s);{ void* _temp653=( void*) s->r; struct Cyc_Absyn_Exp* _temp699; struct Cyc_Absyn_Stmt*
_temp701; struct Cyc_Absyn_Stmt* _temp703; struct Cyc_Absyn_Exp* _temp705;
struct Cyc_Absyn_Exp* _temp707; struct Cyc_Absyn_Stmt* _temp709; struct Cyc_Absyn_Stmt*
_temp711; struct Cyc_Absyn_Exp* _temp713; struct _tuple3 _temp715; struct Cyc_Absyn_Stmt*
_temp717; struct Cyc_Absyn_Exp* _temp719; struct Cyc_Absyn_Stmt* _temp721;
struct Cyc_Absyn_Stmt* _temp723; struct _tuple3 _temp725; struct Cyc_Absyn_Stmt*
_temp727; struct Cyc_Absyn_Exp* _temp729; struct Cyc_Absyn_Stmt* _temp731;
struct Cyc_Absyn_Stmt* _temp733; struct Cyc_Absyn_Stmt* _temp735; struct
_tagged_string* _temp737; struct Cyc_Absyn_Stmt* _temp739; struct _tagged_string*
_temp741; struct Cyc_Absyn_Stmt* _temp743; struct _tuple3 _temp745; struct Cyc_Absyn_Stmt*
_temp747; struct Cyc_Absyn_Exp* _temp749; struct _tuple3 _temp751; struct Cyc_Absyn_Stmt*
_temp753; struct Cyc_Absyn_Exp* _temp755; struct Cyc_Absyn_Exp* _temp757; struct
Cyc_List_List* _temp759; struct Cyc_Absyn_Exp* _temp761; struct Cyc_Absyn_Stmt*
_temp763; struct Cyc_List_List* _temp765; struct Cyc_Absyn_Stmt* _temp767;
struct Cyc_Absyn_Decl* _temp769; struct Cyc_Absyn_Decl _temp771; struct Cyc_Position_Segment*
_temp772; void* _temp774; struct Cyc_Absyn_Vardecl* _temp776; struct Cyc_Absyn_Stmt*
_temp778; struct Cyc_Absyn_Decl* _temp780; struct Cyc_Absyn_Decl _temp782;
struct Cyc_Position_Segment* _temp783; void* _temp785; int _temp787; struct Cyc_Absyn_Exp*
_temp789; struct Cyc_Core_Opt* _temp791; struct Cyc_Core_Opt* _temp793; struct
Cyc_Core_Opt _temp795; struct Cyc_List_List* _temp796; struct Cyc_Absyn_Pat*
_temp798; struct Cyc_Absyn_Stmt* _temp800; struct Cyc_Absyn_Decl* _temp802;
struct Cyc_Absyn_Stmt* _temp804; struct Cyc_Absyn_Stmt* _temp806; struct Cyc_Absyn_Stmt*
_temp808; struct _tagged_string* _temp810; struct Cyc_List_List* _temp812;
struct Cyc_Absyn_Stmt* _temp814; _LL655: if( _temp653 == Cyc_Absyn_Skip_s){ goto
_LL656;} else{ goto _LL657;} _LL657: if(( unsigned int) _temp653 > 1u?(( struct
_enum_struct*) _temp653)->tag == Cyc_Absyn_Exp_s_tag: 0){ _LL700: _temp699=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*) _temp653)->f1; goto
_LL658;} else{ goto _LL659;} _LL659: if(( unsigned int) _temp653 > 1u?(( struct
_enum_struct*) _temp653)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL704: _temp703=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp653)->f1; goto
_LL702; _LL702: _temp701=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp653)->f2; goto _LL660;} else{ goto _LL661;} _LL661: if(( unsigned int)
_temp653 > 1u?(( struct _enum_struct*) _temp653)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL706: _temp705=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp653)->f1; if( _temp705 == 0){ goto _LL662;} else{ goto _LL663;}} else{ goto
_LL663;} _LL663: if(( unsigned int) _temp653 > 1u?(( struct _enum_struct*)
_temp653)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL708: _temp707=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp653)->f1; goto _LL664;} else{ goto
_LL665;} _LL665: if(( unsigned int) _temp653 > 1u?(( struct _enum_struct*)
_temp653)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL714: _temp713=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp653)->f1; goto _LL712; _LL712:
_temp711=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp653)->f2; goto _LL710; _LL710: _temp709=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp653)->f3; goto _LL666;} else{ goto _LL667;}
_LL667: if(( unsigned int) _temp653 > 1u?(( struct _enum_struct*) _temp653)->tag
== Cyc_Absyn_Do_s_tag: 0){ _LL722: _temp721=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Do_s_struct*) _temp653)->f1; goto _LL716; _LL716: _temp715=( struct
_tuple3)(( struct Cyc_Absyn_Do_s_struct*) _temp653)->f2; _LL720: _temp719=
_temp715.f1; goto _LL718; _LL718: _temp717= _temp715.f2; goto _LL668;} else{
goto _LL669;} _LL669: if(( unsigned int) _temp653 > 1u?(( struct _enum_struct*)
_temp653)->tag == Cyc_Absyn_While_s_tag: 0){ _LL726: _temp725=( struct _tuple3)((
struct Cyc_Absyn_While_s_struct*) _temp653)->f1; _LL730: _temp729= _temp725.f1;
goto _LL728; _LL728: _temp727= _temp725.f2; goto _LL724; _LL724: _temp723=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_While_s_struct*) _temp653)->f2; goto
_LL670;} else{ goto _LL671;} _LL671: if(( unsigned int) _temp653 > 1u?(( struct
_enum_struct*) _temp653)->tag == Cyc_Absyn_Break_s_tag: 0){ _LL732: _temp731=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Break_s_struct*) _temp653)->f1; goto
_LL672;} else{ goto _LL673;} _LL673: if(( unsigned int) _temp653 > 1u?(( struct
_enum_struct*) _temp653)->tag == Cyc_Absyn_Continue_s_tag: 0){ _LL734: _temp733=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Continue_s_struct*) _temp653)->f1;
goto _LL674;} else{ goto _LL675;} _LL675: if(( unsigned int) _temp653 > 1u?((
struct _enum_struct*) _temp653)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL738:
_temp737=( struct _tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp653)->f1;
goto _LL736; _LL736: _temp735=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp653)->f2; if( _temp735 == 0){ goto _LL676;} else{ goto _LL677;}} else{ goto
_LL677;} _LL677: if(( unsigned int) _temp653 > 1u?(( struct _enum_struct*)
_temp653)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL742: _temp741=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp653)->f1; goto _LL740;
_LL740: _temp739=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp653)->f2; goto _LL678;} else{ goto _LL679;} _LL679: if(( unsigned int)
_temp653 > 1u?(( struct _enum_struct*) _temp653)->tag == Cyc_Absyn_For_s_tag: 0){
_LL758: _temp757=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_For_s_struct*)
_temp653)->f1; goto _LL752; _LL752: _temp751=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp653)->f2; _LL756: _temp755= _temp751.f1; goto _LL754; _LL754: _temp753=
_temp751.f2; goto _LL746; _LL746: _temp745=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*)
_temp653)->f3; _LL750: _temp749= _temp745.f1; goto _LL748; _LL748: _temp747=
_temp745.f2; goto _LL744; _LL744: _temp743=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_For_s_struct*) _temp653)->f4; goto _LL680;} else{ goto _LL681;} _LL681:
if(( unsigned int) _temp653 > 1u?(( struct _enum_struct*) _temp653)->tag == Cyc_Absyn_Switch_s_tag:
0){ _LL762: _temp761=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp653)->f1; goto _LL760; _LL760: _temp759=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Switch_s_struct*) _temp653)->f2; goto _LL682;} else{ goto _LL683;}
_LL683: if(( unsigned int) _temp653 > 1u?(( struct _enum_struct*) _temp653)->tag
== Cyc_Absyn_Fallthru_s_tag: 0){ _LL766: _temp765=( struct Cyc_List_List*)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp653)->f1; goto _LL764; _LL764:
_temp763=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp653)->f2; goto _LL684;} else{ goto _LL685;} _LL685: if(( unsigned int)
_temp653 > 1u?(( struct _enum_struct*) _temp653)->tag == Cyc_Absyn_Decl_s_tag: 0){
_LL770: _temp769=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp653)->f1; _temp771=* _temp769; _LL775: _temp774=( void*) _temp771.r; if((
unsigned int) _temp774 > 1u?(( struct _enum_struct*) _temp774)->tag == Cyc_Absyn_Var_d_tag:
0){ _LL777: _temp776=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp774)->f1; goto _LL773;} else{ goto _LL687;} _LL773: _temp772=( struct Cyc_Position_Segment*)
_temp771.loc; goto _LL768; _LL768: _temp767=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp653)->f2; goto _LL686;} else{ goto _LL687;}
_LL687: if(( unsigned int) _temp653 > 1u?(( struct _enum_struct*) _temp653)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL781: _temp780=( struct Cyc_Absyn_Decl*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp653)->f1; _temp782=* _temp780; _LL786: _temp785=(
void*) _temp782.r; if(( unsigned int) _temp785 > 1u?(( struct _enum_struct*)
_temp785)->tag == Cyc_Absyn_Let_d_tag: 0){ _LL799: _temp798=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp785)->f1; goto _LL794; _LL794: _temp793=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp785)->f2; if(
_temp793 == 0){ goto _LL689;} else{ _temp795=* _temp793; _LL797: _temp796=(
struct Cyc_List_List*) _temp795.v; goto _LL792;} _LL792: _temp791=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp785)->f3; goto _LL790; _LL790: _temp789=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp785)->f4; goto
_LL788; _LL788: _temp787=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp785)->f5;
goto _LL784;} else{ goto _LL689;} _LL784: _temp783=( struct Cyc_Position_Segment*)
_temp782.loc; goto _LL779; _LL779: _temp778=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp653)->f2; goto _LL688;} else{ goto _LL689;}
_LL689: if(( unsigned int) _temp653 > 1u?(( struct _enum_struct*) _temp653)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL803: _temp802=( struct Cyc_Absyn_Decl*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp653)->f1; goto _LL801; _LL801: _temp800=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp653)->f2; goto _LL690;}
else{ goto _LL691;} _LL691: if(( unsigned int) _temp653 > 1u?(( struct
_enum_struct*) _temp653)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL805: _temp804=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*) _temp653)->f1; goto
_LL692;} else{ goto _LL693;} _LL693: if(( unsigned int) _temp653 > 1u?(( struct
_enum_struct*) _temp653)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL807: _temp806=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*) _temp653)->f1; goto
_LL694;} else{ goto _LL695;} _LL695: if(( unsigned int) _temp653 > 1u?(( struct
_enum_struct*) _temp653)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL811: _temp810=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp653)->f1; goto
_LL809; _LL809: _temp808=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp653)->f2; goto _LL696;} else{ goto _LL697;} _LL697: if(( unsigned int)
_temp653 > 1u?(( struct _enum_struct*) _temp653)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL815: _temp814=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp653)->f1; goto _LL813; _LL813: _temp812=( struct Cyc_List_List*)(( struct
Cyc_Absyn_TryCatch_s_struct*) _temp653)->f2; goto _LL698;} else{ goto _LL654;}
_LL656: return; _LL658: { struct Cyc_NewControlFlow_AbsSyn syn= Cyc_NewControlFlow_abstract_exp(
env, _temp699); void* ae= Cyc_NewControlFlow_meet_absexp( syn); Cyc_NewControlFlow_cf_set_absexps(
s,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp816=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp816[ 0]=({ struct Cyc_List_List
_temp817; _temp817.hd=( void*) ae; _temp817.tl= 0; _temp817;}); _temp816;}));
return;} _LL660: Cyc_NewControlFlow_cf_prepass( env, _temp703); Cyc_NewControlFlow_cf_prepass(
env, _temp701); return; _LL662: return; _LL664: { void* ae= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp818= _temp707; if( _temp818 == 0){
_throw( Null_Exception);} _temp818;}))); Cyc_NewControlFlow_cf_set_absexps( s,(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp819=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp819[ 0]=({ struct Cyc_List_List
_temp820; _temp820.hd=( void*) ae; _temp820.tl= 0; _temp820;}); _temp819;}));
return;} _LL666: { struct Cyc_List_List* _temp823; void* _temp825; void*
_temp827; struct Cyc_NewControlFlow_AbsSyn _temp821= Cyc_NewControlFlow_abstract_exp(
env, _temp713); _LL828: _temp827=( void*) _temp821.when_true; goto _LL826;
_LL826: _temp825=( void*) _temp821.when_false; goto _LL824; _LL824: _temp823=(
struct Cyc_List_List*) _temp821.lvalues; goto _LL822; _LL822: Cyc_NewControlFlow_cf_set_absexps(
s,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp829=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp829[ 0]=({ struct Cyc_List_List
_temp830; _temp830.hd=( void*) _temp827; _temp830.tl=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp831=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp831[ 0]=({ struct Cyc_List_List _temp832;
_temp832.hd=( void*) _temp825; _temp832.tl= 0; _temp832;}); _temp831;});
_temp830;}); _temp829;})); Cyc_NewControlFlow_cf_prepass( env, _temp711); Cyc_NewControlFlow_cf_prepass(
env, _temp709); return;} _LL668: _temp729= _temp719; _temp723= _temp721; goto
_LL670; _LL670: { struct Cyc_List_List* _temp835; void* _temp837; void* _temp839;
struct Cyc_NewControlFlow_AbsSyn _temp833= Cyc_NewControlFlow_abstract_exp( env,
_temp729); _LL840: _temp839=( void*) _temp833.when_true; goto _LL838; _LL838:
_temp837=( void*) _temp833.when_false; goto _LL836; _LL836: _temp835=( struct
Cyc_List_List*) _temp833.lvalues; goto _LL834; _LL834: Cyc_NewControlFlow_cf_set_absexps(
s,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp841=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp841[ 0]=({ struct Cyc_List_List
_temp842; _temp842.hd=( void*) _temp839; _temp842.tl=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp843=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp843[ 0]=({ struct Cyc_List_List _temp844;
_temp844.hd=( void*) _temp837; _temp844.tl= 0; _temp844;}); _temp843;});
_temp842;}); _temp841;})); Cyc_NewControlFlow_cf_prepass( env, _temp723);
return;} _LL672: return; _LL674: return; _LL676:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp845=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp845=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp846=( char*)"cf_prepass: goto w/o destination";
struct _tagged_string _temp847; _temp847.curr= _temp846; _temp847.base= _temp846;
_temp847.last_plus_one= _temp846 + 33; _temp847;})};( struct _xenum_struct*)
_temp845;})); _LL678: return; _LL680: { void* ae1= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env, _temp757)); struct Cyc_List_List* _temp850;
void* _temp852; void* _temp854; struct Cyc_NewControlFlow_AbsSyn _temp848= Cyc_NewControlFlow_abstract_exp(
env, _temp755); _LL855: _temp854=( void*) _temp848.when_true; goto _LL853;
_LL853: _temp852=( void*) _temp848.when_false; goto _LL851; _LL851: _temp850=(
struct Cyc_List_List*) _temp848.lvalues; goto _LL849; _LL849: { void* ae3= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env, _temp749)); Cyc_NewControlFlow_cf_set_absexps(
s,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp856=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp856[ 0]=({ struct Cyc_List_List
_temp857; _temp857.hd=( void*) ae1; _temp857.tl=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp858=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp858[ 0]=({ struct Cyc_List_List _temp859;
_temp859.hd=( void*) _temp854; _temp859.tl=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp860=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1);
_temp860[ 0]=({ struct Cyc_List_List _temp861; _temp861.hd=( void*) _temp852;
_temp861.tl=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp862=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp862[ 0]=({
struct Cyc_List_List _temp863; _temp863.hd=( void*) ae3; _temp863.tl= 0;
_temp863;}); _temp862;}); _temp861;}); _temp860;}); _temp859;}); _temp858;});
_temp857;}); _temp856;})); Cyc_NewControlFlow_cf_prepass( env, _temp743);
return;}} _LL682: { void* ae= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
env, _temp761)); struct Cyc_List_List* ael= Cyc_NewControlFlow_cf_prepass_scs(
env, _temp759); Cyc_NewControlFlow_cf_set_absexps( s,( struct Cyc_List_List*)({
struct Cyc_List_List* _temp864=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp864[ 0]=({ struct Cyc_List_List _temp865;
_temp865.hd=( void*) ae; _temp865.tl= ael; _temp865;}); _temp864;})); return;}
_LL684: { struct Cyc_List_List* ael= 0; for( 0; _temp765 != 0; _temp765=
_temp765->tl){ void* ae= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
env,( struct Cyc_Absyn_Exp*) _temp765->hd)); ael=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp866=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List) * 1); _temp866[ 0]=({ struct Cyc_List_List _temp867;
_temp867.hd=( void*) ae; _temp867.tl= ael; _temp867;}); _temp866;});} Cyc_NewControlFlow_cf_set_absexps(
s,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( ael));
return;} _LL686: Cyc_NewControlFlow_add_var_root( env, _temp776); if( _temp776->initializer
!= 0){ struct Cyc_NewControlFlow_AbsEnv init_env= Cyc_NewControlFlow_change_lhs(
env,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp868=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp868[ 0]=({ struct Cyc_List_List
_temp869; _temp869.hd=( void*) Cyc_CfAbsexp_mkLocalOp( _temp776); _temp869.tl= 0;
_temp869;}); _temp868;})); void* ae= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
init_env,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp870= _temp776->initializer;
if( _temp870 == 0){ _throw( Null_Exception);} _temp870;}))); Cyc_NewControlFlow_cf_set_absexps(
s,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp871=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp871[ 0]=({ struct Cyc_List_List
_temp872; _temp872.hd=( void*) ae; _temp872.tl= 0; _temp872;}); _temp871;}));}
Cyc_NewControlFlow_cf_prepass( env, _temp767); return; _LL688: for( 0; _temp796
!= 0; _temp796= _temp796->tl){ Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)
_temp796->hd);}{ void* ae= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp789)); Cyc_NewControlFlow_cf_set_absexps(
s,( struct Cyc_List_List*)({ struct Cyc_List_List* _temp873=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List) * 1); _temp873[ 0]=({ struct Cyc_List_List
_temp874; _temp874.hd=( void*) ae; _temp874.tl= 0; _temp874;}); _temp873;}));
Cyc_NewControlFlow_cf_prepass( env, _temp778); return;} _LL690: goto _LL692;
_LL692: goto _LL694; _LL694:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp875=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp875=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp876=( char*)"cf_prepass: bad stmt form";
struct _tagged_string _temp877; _temp877.curr= _temp876; _temp877.base= _temp876;
_temp877.last_plus_one= _temp876 + 26; _temp877;})};( struct _xenum_struct*)
_temp875;})); _LL696: Cyc_NewControlFlow_cf_prepass( env, _temp808); return;
_LL698: Cyc_NewControlFlow_cf_prepass( env, _temp814);{ struct Cyc_List_List*
ael= Cyc_NewControlFlow_cf_prepass_scs( env, _temp812); Cyc_NewControlFlow_cf_set_absexps(
s, ael); return;} _LL654:;}} void* Cyc_NewControlFlow_cf_analyze_stmt( struct
Cyc_Absyn_Stmt* s, void* f){ return Cyc_CfFlowInfo_BottomFL;} void Cyc_NewControlFlow_cf_analysis(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Fndecl* fd){;} void Cyc_NewControlFlow_cf_postpass(
struct Cyc_Absyn_Stmt* s){ s->annot=({ struct Cyc_Absyn_EmptyAnnot_struct*
_temp878=( struct Cyc_Absyn_EmptyAnnot_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Absyn_EmptyAnnot_struct));* _temp878=( struct Cyc_Absyn_EmptyAnnot_struct){.tag=
Cyc_Absyn_EmptyAnnot_tag};( struct _xenum_struct*) _temp878;});{ void* _temp879=(
void*) s->r; struct Cyc_Absyn_Stmt* _temp901; struct Cyc_Absyn_Stmt* _temp903;
struct Cyc_Absyn_Stmt* _temp905; struct Cyc_Absyn_Stmt* _temp907; struct Cyc_Absyn_Exp*
_temp909; struct Cyc_Absyn_Stmt* _temp911; struct _tagged_string* _temp913;
struct Cyc_Absyn_Stmt* _temp915; struct _tuple3 _temp917; struct Cyc_Absyn_Stmt*
_temp919; struct _tuple3 _temp921; struct _tuple3 _temp923; struct Cyc_Absyn_Exp*
_temp925; struct Cyc_Absyn_Stmt* _temp927; struct Cyc_Absyn_Decl* _temp929;
struct _tuple3 _temp931; struct Cyc_Absyn_Stmt* _temp933; struct Cyc_List_List*
_temp935; struct Cyc_Absyn_Stmt* _temp937; struct Cyc_List_List* _temp939;
struct Cyc_Absyn_Exp* _temp941; _LL881: if(( unsigned int) _temp879 > 1u?((
struct _enum_struct*) _temp879)->tag == Cyc_Absyn_Seq_s_tag: 0){ _LL904:
_temp903=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*) _temp879)->f1;
goto _LL902; _LL902: _temp901=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp879)->f2; goto _LL882;} else{ goto _LL883;} _LL883: if(( unsigned int)
_temp879 > 1u?(( struct _enum_struct*) _temp879)->tag == Cyc_Absyn_IfThenElse_s_tag:
0){ _LL910: _temp909=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp879)->f1; goto _LL908; _LL908: _temp907=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp879)->f2; goto _LL906; _LL906: _temp905=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp879)->f3;
goto _LL884;} else{ goto _LL885;} _LL885: if(( unsigned int) _temp879 > 1u?((
struct _enum_struct*) _temp879)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL914:
_temp913=( struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp879)->f1;
goto _LL912; _LL912: _temp911=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp879)->f2; goto _LL886;} else{ goto _LL887;} _LL887: if(( unsigned int)
_temp879 > 1u?(( struct _enum_struct*) _temp879)->tag == Cyc_Absyn_While_s_tag:
0){ _LL918: _temp917=( struct _tuple3)(( struct Cyc_Absyn_While_s_struct*)
_temp879)->f1; goto _LL916; _LL916: _temp915=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_While_s_struct*) _temp879)->f2; goto _LL888;} else{ goto _LL889;}
_LL889: if(( unsigned int) _temp879 > 1u?(( struct _enum_struct*) _temp879)->tag
== Cyc_Absyn_For_s_tag: 0){ _LL926: _temp925=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_For_s_struct*) _temp879)->f1; goto _LL924; _LL924: _temp923=( struct
_tuple3)(( struct Cyc_Absyn_For_s_struct*) _temp879)->f2; goto _LL922; _LL922:
_temp921=( struct _tuple3)(( struct Cyc_Absyn_For_s_struct*) _temp879)->f3; goto
_LL920; _LL920: _temp919=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*)
_temp879)->f4; goto _LL890;} else{ goto _LL891;} _LL891: if(( unsigned int)
_temp879 > 1u?(( struct _enum_struct*) _temp879)->tag == Cyc_Absyn_Decl_s_tag: 0){
_LL930: _temp929=( struct Cyc_Absyn_Decl*)(( struct Cyc_Absyn_Decl_s_struct*)
_temp879)->f1; goto _LL928; _LL928: _temp927=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp879)->f2; goto _LL892;} else{ goto _LL893;}
_LL893: if(( unsigned int) _temp879 > 1u?(( struct _enum_struct*) _temp879)->tag
== Cyc_Absyn_Do_s_tag: 0){ _LL934: _temp933=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Do_s_struct*) _temp879)->f1; goto _LL932; _LL932: _temp931=( struct
_tuple3)(( struct Cyc_Absyn_Do_s_struct*) _temp879)->f2; goto _LL894;} else{
goto _LL895;} _LL895: if(( unsigned int) _temp879 > 1u?(( struct _enum_struct*)
_temp879)->tag == Cyc_Absyn_TryCatch_s_tag: 0){ _LL938: _temp937=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_TryCatch_s_struct*) _temp879)->f1; goto _LL936; _LL936:
_temp935=( struct Cyc_List_List*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp879)->f2; goto _LL896;} else{ goto _LL897;} _LL897: if(( unsigned int)
_temp879 > 1u?(( struct _enum_struct*) _temp879)->tag == Cyc_Absyn_Switch_s_tag:
0){ _LL942: _temp941=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp879)->f1; goto _LL940; _LL940: _temp939=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Switch_s_struct*) _temp879)->f2; goto _LL898;} else{ goto _LL899;}
_LL899: goto _LL900; _LL882: _temp907= _temp903; _temp905= _temp901; goto _LL884;
_LL884: Cyc_NewControlFlow_cf_postpass( _temp907); Cyc_NewControlFlow_cf_postpass(
_temp905); return; _LL886: _temp915= _temp911; goto _LL888; _LL888: _temp919=
_temp915; goto _LL890; _LL890: _temp927= _temp919; goto _LL892; _LL892: _temp933=
_temp927; goto _LL894; _LL894: Cyc_NewControlFlow_cf_postpass( _temp933);
return; _LL896: Cyc_NewControlFlow_cf_postpass( _temp937); _temp939= _temp935;
goto _LL898; _LL898: for( 0; _temp939 != 0; _temp939= _temp939->tl){ Cyc_NewControlFlow_cf_postpass(((
struct Cyc_Absyn_Switch_clause*) _temp939->hd)->body);} return; _LL900: return;
_LL880:;}} void Cyc_NewControlFlow_cf_check_fun( struct Cyc_Absyn_Fndecl* fd){
struct Cyc_NewControlFlow_AbsEnv env=({ struct Cyc_NewControlFlow_AbsEnv
_temp943; _temp943.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv* _temp944=(
struct Cyc_NewControlFlow_SharedAbsEnv*) GC_malloc( sizeof( struct Cyc_NewControlFlow_SharedAbsEnv)
* 1); _temp944[ 0]=({ struct Cyc_NewControlFlow_SharedAbsEnv _temp945; _temp945.all_roots=((
struct Cyc_Dict_Dict*(*)( int(* comp)( void*, void*))) Cyc_Dict_empty)( Cyc_CfFlowInfo_local_root_cmp);
_temp945;}); _temp944;}); _temp943.lhsides= 0; _temp943.access_path= 0; _temp943;});{
struct Cyc_List_List* args= fd->args; for( 0; args != 0; args= args->tl){ Cyc_NewControlFlow_add_var_root(
env,({ struct Cyc_Absyn_Vardecl* _temp946=( struct Cyc_Absyn_Vardecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Vardecl) * 1); _temp946[ 0]=({ struct Cyc_Absyn_Vardecl
_temp947; _temp947.sc=( void*) Cyc_Absyn_Public; _temp947.name=({ struct _tuple1*
_temp948=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1) * 1); _temp948[ 0]=({
struct _tuple1 _temp949; _temp949.f1= Cyc_Absyn_Loc_n; _temp949.f2=(*(( struct
_tuple6*) args->hd)).f1; _temp949;}); _temp948;}); _temp947.tq=(*(( struct
_tuple6*) args->hd)).f2; _temp947.type=( void*)(*(( struct _tuple6*) args->hd)).f3;
_temp947.initializer= 0; _temp947.shadow= 0; _temp947.region= 0; _temp947;});
_temp946;}));}} Cyc_NewControlFlow_cf_prepass( env, fd->body); Cyc_NewControlFlow_cf_analysis(
env, fd); Cyc_NewControlFlow_cf_postpass( fd->body);} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds != 0; ds= ds->tl){ void* _temp950=( void*)((
struct Cyc_Absyn_Decl*) ds->hd)->r; struct Cyc_Absyn_Fndecl* _temp960; struct
Cyc_List_List* _temp962; struct _tuple1* _temp964; struct Cyc_List_List*
_temp966; struct _tagged_string* _temp968; _LL952: if(( unsigned int) _temp950 >
1u?(( struct _enum_struct*) _temp950)->tag == Cyc_Absyn_Fn_d_tag: 0){ _LL961:
_temp960=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp950)->f1;
goto _LL953;} else{ goto _LL954;} _LL954: if(( unsigned int) _temp950 > 1u?((
struct _enum_struct*) _temp950)->tag == Cyc_Absyn_Using_d_tag: 0){ _LL965:
_temp964=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*) _temp950)->f1;
goto _LL963; _LL963: _temp962=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp950)->f2; goto _LL955;} else{ goto _LL956;} _LL956: if(( unsigned int)
_temp950 > 1u?(( struct _enum_struct*) _temp950)->tag == Cyc_Absyn_Namespace_d_tag:
0){ _LL969: _temp968=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp950)->f1; goto _LL967; _LL967: _temp966=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Namespace_d_struct*) _temp950)->f2; goto _LL957;} else{ goto _LL958;}
_LL958: goto _LL959; _LL953: Cyc_NewControlFlow_cf_check_fun( _temp960); goto
_LL951; _LL955: _temp966= _temp962; goto _LL957; _LL957: Cyc_NewControlFlow_cf_check(
_temp966); goto _LL951; _LL959: goto _LL951; _LL951:;}}