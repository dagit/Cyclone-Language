#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2;}; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3;}; struct _tuple2{ struct
Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2;}; struct _tuple3{ void* f1; void*
f2;}; struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2;}; struct _tuple5{
struct Cyc_Absyn_Tqual* f1; void* f2;}; struct _tuple6{ void* f1; int f2;};
struct _tuple7{ struct Cyc_Absyn_Structdecl* f1;}; struct _tuple8{ struct Cyc_Core_Opt*
f1; struct Cyc_Core_Opt* f2;}; struct _tuple9{ struct _tagged_string* f1; struct
Cyc_Absyn_Tqual* f2; void* f3;}; struct _tuple10{ struct Cyc_Absyn_Enumdecl* f1;};
struct _tuple11{ struct Cyc_Absyn_Xenumdecl* f1;}; typedef unsigned int Cyc_uint;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v;}; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[
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
char* tag;}; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag;}; extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag;}; extern int Cyc_List_mem( int(* compare)( void*, void*), struct Cyc_List_List*
l, void* x); extern struct Cyc_List_List* Cyc_List_filter_c( int(* f)( void*,
void*), void* env, struct Cyc_List_List* l); struct Cyc_Stdio___sFILE; typedef
struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;};
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag;}; extern int Cyc_String_zstrptrcmp( struct _tagged_string*, struct
_tagged_string*); struct Cyc_Lineno_Pos{ struct _tagged_string logical_file;
struct _tagged_string line; int line_no; int col;}; typedef struct Cyc_Lineno_Pos*
Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[ 5u]; struct Cyc_Position_Exit_struct{
char* tag;}; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment*
Cyc_Position_seg_t; typedef void* Cyc_Position_Error_kind; extern void* Cyc_Position_Lex;
extern void* Cyc_Position_Parse; extern void* Cyc_Position_Elab; typedef void*
Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct _tagged_string
source; struct Cyc_Position_Segment* seg; void* kind; struct _tagged_string desc;};
typedef struct Cyc_Position_Error* Cyc_Position_error_t; extern char Cyc_Position_Nocontext_tag[
10u]; struct Cyc_Position_Nocontext_struct{ char* tag;}; typedef struct
_tagged_string* Cyc_Absyn_field_name; typedef struct _tagged_string* Cyc_Absyn_var;
typedef struct _tagged_string* Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_Nmspace;
typedef struct _tuple0* Cyc_Absyn_qvar; typedef struct _tuple0* Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t;
struct Cyc_Absyn_Tvar; typedef void* Cyc_Absyn_Scope; struct Cyc_Absyn_Tqual;
typedef void* Cyc_Absyn_Size_of; typedef void* Cyc_Absyn_Kind; typedef void* Cyc_Absyn_Array_kind;
typedef void* Cyc_Absyn_Sign; struct Cyc_Absyn_Conref; typedef void* Cyc_Absyn_Constraint;
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
struct Cyc_Absyn_EmptyAnnot_struct{ char* tag;}; extern int Cyc_Absyn_qvar_cmp(
struct _tuple0*, struct _tuple0*); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref*
x); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct
Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef
struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); extern struct _tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*); struct
Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set*
Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern struct Cyc_Set_Set*
Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member(
struct Cyc_Set_Set* s, void* elt); extern char Cyc_Set_Absent_tag[ 7u]; struct
Cyc_Set_Absent_struct{ char* tag;}; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict*
Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[
8u]; struct Cyc_Dict_Present_struct{ char* tag;}; extern char Cyc_Dict_Absent_tag[
7u]; extern int Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern
struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void*
data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern
struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key);
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
Cyc_Tcenv_tenv_t; extern struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(); extern
struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct Cyc_Absyn_Fndecl*); extern
struct Cyc_List_List* Cyc_Tcenv_resolve_namespace( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tagged_string*, struct Cyc_List_List*);
extern struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xenumdecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*); extern char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{
char* tag;}; extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct
_tagged_string); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*,
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern int
Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*);
extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*);
extern struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_Position_Segment*); extern void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual* tq1, struct Cyc_Absyn_Tqual* tq2); extern void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern int Cyc_Tcexp_is_const_exp(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern void Cyc_Tcstmt_tcStmt(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, int new_block); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); extern
void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int add_cyc_namespace, struct Cyc_List_List*
ds); extern struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv* te,
struct Cyc_List_List*); static void Cyc_Tc_redecl_err( struct Cyc_Position_Segment*
loc, struct _tagged_string t, struct _tagged_string* v, struct _tagged_string
new_sc, struct _tagged_string old_sc){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp0= t; struct _tagged_string _temp1=* v; struct
_tagged_string _temp2= new_sc; struct _tagged_string _temp3= old_sc; xprintf("redeclaration of %.*s %.*s cannot be %.*s when earlier definition is %.*s",
_temp0.last_plus_one - _temp0.curr, _temp0.curr, _temp1.last_plus_one - _temp1.curr,
_temp1.curr, _temp2.last_plus_one - _temp2.curr, _temp2.curr, _temp3.last_plus_one
- _temp3.curr, _temp3.curr);}));} static void* Cyc_Tc_scope_redecl_okay( void*
s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt* fields2, struct
Cyc_Position_Segment* loc, struct _tagged_string t, struct _tagged_string* v){{
struct _tuple3 _temp5=({ struct _tuple3 _temp4; _temp4.f1= s1; _temp4.f2= s2;
_temp4;}); void* _temp35; void* _temp37; void* _temp39; void* _temp41; void*
_temp43; void* _temp45; void* _temp47; void* _temp49; void* _temp51; void*
_temp53; void* _temp55; void* _temp57; void* _temp59; void* _temp61; void*
_temp63; void* _temp65; void* _temp68; void* _temp70; void* _temp72; void*
_temp74; void* _temp76; void* _temp78; void* _temp80; void* _temp82; void*
_temp85; void* _temp87; void* _temp89; void* _temp91; _LL7: _LL38: _temp37=
_temp5.f1; if( _temp37 == Cyc_Absyn_ExternC){ goto _LL36;} else{ goto _LL9;}
_LL36: _temp35= _temp5.f2; if( _temp35 == Cyc_Absyn_ExternC){ goto _LL8;} else{
goto _LL9;} _LL9: _LL42: _temp41= _temp5.f1; goto _LL40; _LL40: _temp39= _temp5.f2;
if( _temp39 == Cyc_Absyn_ExternC){ goto _LL10;} else{ goto _LL11;} _LL11: _LL46:
_temp45= _temp5.f1; if( _temp45 == Cyc_Absyn_Static){ goto _LL44;} else{ goto
_LL13;} _LL44: _temp43= _temp5.f2; if( _temp43 == Cyc_Absyn_Static){ goto _LL12;}
else{ goto _LL13;} _LL13: _LL50: _temp49= _temp5.f1; goto _LL48; _LL48: _temp47=
_temp5.f2; if( _temp47 == Cyc_Absyn_Static){ goto _LL14;} else{ goto _LL15;}
_LL15: _LL54: _temp53= _temp5.f1; if( _temp53 == Cyc_Absyn_Static){ goto _LL52;}
else{ goto _LL17;} _LL52: _temp51= _temp5.f2; if( _temp51 == Cyc_Absyn_Public){
goto _LL16;} else{ goto _LL17;} _LL17: _LL58: _temp57= _temp5.f1; if( _temp57 ==
Cyc_Absyn_Abstract){ goto _LL56;} else{ goto _LL19;} _LL56: _temp55= _temp5.f2;
if( _temp55 == Cyc_Absyn_Public){ goto _LL18;} else{ goto _LL19;} _LL19: _LL62:
_temp61= _temp5.f1; goto _LL60; _LL60: _temp59= _temp5.f2; if( _temp59 == Cyc_Absyn_Public){
goto _LL20;} else{ goto _LL21;} _LL21: _LL66: _temp65= _temp5.f1; if( _temp65 ==
Cyc_Absyn_Static){ goto _LL64;} else{ goto _LL23;} _LL64: _temp63= _temp5.f2;
if( _temp63 == Cyc_Absyn_Extern){ goto _LL22;} else{ goto _LL23;} _LL23: _LL71:
_temp70= _temp5.f1; if( _temp70 == Cyc_Absyn_Abstract){ goto _LL69;} else{ goto
_LL25;} _LL69: _temp68= _temp5.f2; if( _temp68 == Cyc_Absyn_Extern){ goto _LL67;}
else{ goto _LL25;} _LL67: if( fields2 != 0){ goto _LL24;} else{ goto _LL25;}
_LL25: _LL75: _temp74= _temp5.f1; goto _LL73; _LL73: _temp72= _temp5.f2; if(
_temp72 == Cyc_Absyn_Extern){ goto _LL26;} else{ goto _LL27;} _LL27: _LL79:
_temp78= _temp5.f1; if( _temp78 == Cyc_Absyn_Static){ goto _LL77;} else{ goto
_LL29;} _LL77: _temp76= _temp5.f2; if( _temp76 == Cyc_Absyn_Abstract){ goto
_LL28;} else{ goto _LL29;} _LL29: _LL83: _temp82= _temp5.f1; if( _temp82 == Cyc_Absyn_Public){
goto _LL81;} else{ goto _LL31;} _LL81: _temp80= _temp5.f2; if( _temp80 == Cyc_Absyn_Abstract){
goto _LL30;} else{ goto _LL31;} _LL31: _LL88: _temp87= _temp5.f1; if( _temp87 ==
Cyc_Absyn_Extern){ goto _LL86;} else{ goto _LL33;} _LL86: _temp85= _temp5.f2;
if( _temp85 == Cyc_Absyn_Abstract){ goto _LL84;} else{ goto _LL33;} _LL84: if(
fields1 != 0){ goto _LL32;} else{ goto _LL33;} _LL33: _LL92: _temp91= _temp5.f1;
goto _LL90; _LL90: _temp89= _temp5.f2; if( _temp89 == Cyc_Absyn_Abstract){ goto
_LL34;} else{ goto _LL6;} _LL8: goto _LL6; _LL10: Cyc_Tc_redecl_err( loc, t, v,(
struct _tagged_string)({ char* _temp93=( char*)"non-extern \"C\""; struct
_tagged_string _temp94; _temp94.curr= _temp93; _temp94.base= _temp93; _temp94.last_plus_one=
_temp93 + 15; _temp94;}),( struct _tagged_string)({ char* _temp95=( char*)"extern \"C\"";
struct _tagged_string _temp96; _temp96.curr= _temp95; _temp96.base= _temp95;
_temp96.last_plus_one= _temp95 + 11; _temp96;})); goto _LL6; _LL12: goto _LL6;
_LL14: Cyc_Tc_redecl_err( loc, t, v,( struct _tagged_string)({ char* _temp97=(
char*)"non-static"; struct _tagged_string _temp98; _temp98.curr= _temp97;
_temp98.base= _temp97; _temp98.last_plus_one= _temp97 + 11; _temp98;}),( struct
_tagged_string)({ char* _temp99=( char*)"static"; struct _tagged_string _temp100;
_temp100.curr= _temp99; _temp100.base= _temp99; _temp100.last_plus_one= _temp99
+ 7; _temp100;})); goto _LL6; _LL16: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp101=( char*)"static"; struct _tagged_string
_temp102; _temp102.curr= _temp101; _temp102.base= _temp101; _temp102.last_plus_one=
_temp101 + 7; _temp102;}),( struct _tagged_string)({ char* _temp103=( char*)"public";
struct _tagged_string _temp104; _temp104.curr= _temp103; _temp104.base= _temp103;
_temp104.last_plus_one= _temp103 + 7; _temp104;})); goto _LL6; _LL18: Cyc_Tc_redecl_err(
loc, t, v,( struct _tagged_string)({ char* _temp105=( char*)"abstract"; struct
_tagged_string _temp106; _temp106.curr= _temp105; _temp106.base= _temp105;
_temp106.last_plus_one= _temp105 + 9; _temp106;}),( struct _tagged_string)({
char* _temp107=( char*)"public"; struct _tagged_string _temp108; _temp108.curr=
_temp107; _temp108.base= _temp107; _temp108.last_plus_one= _temp107 + 7;
_temp108;})); goto _LL6; _LL20: s1= Cyc_Absyn_Public; goto _LL6; _LL22: Cyc_Tc_redecl_err(
loc, t, v,( struct _tagged_string)({ char* _temp109=( char*)"static"; struct
_tagged_string _temp110; _temp110.curr= _temp109; _temp110.base= _temp109;
_temp110.last_plus_one= _temp109 + 7; _temp110;}),( struct _tagged_string)({
char* _temp111=( char*)"extern"; struct _tagged_string _temp112; _temp112.curr=
_temp111; _temp112.base= _temp111; _temp112.last_plus_one= _temp111 + 7;
_temp112;})); goto _LL6; _LL24: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp113=( char*)"abstract"; struct _tagged_string
_temp114; _temp114.curr= _temp113; _temp114.base= _temp113; _temp114.last_plus_one=
_temp113 + 9; _temp114;}),( struct _tagged_string)({ char* _temp115=( char*)"transparent";
struct _tagged_string _temp116; _temp116.curr= _temp115; _temp116.base= _temp115;
_temp116.last_plus_one= _temp115 + 12; _temp116;})); goto _LL6; _LL26: goto _LL6;
_LL28: Cyc_Tc_redecl_err( loc, t, v,( struct _tagged_string)({ char* _temp117=(
char*)"static"; struct _tagged_string _temp118; _temp118.curr= _temp117;
_temp118.base= _temp117; _temp118.last_plus_one= _temp117 + 7; _temp118;}),(
struct _tagged_string)({ char* _temp119=( char*)"abstract"; struct
_tagged_string _temp120; _temp120.curr= _temp119; _temp120.base= _temp119;
_temp120.last_plus_one= _temp119 + 9; _temp120;})); goto _LL6; _LL30: Cyc_Tc_redecl_err(
loc, t, v,( struct _tagged_string)({ char* _temp121=( char*)"public"; struct
_tagged_string _temp122; _temp122.curr= _temp121; _temp122.base= _temp121;
_temp122.last_plus_one= _temp121 + 7; _temp122;}),( struct _tagged_string)({
char* _temp123=( char*)"abstract"; struct _tagged_string _temp124; _temp124.curr=
_temp123; _temp124.base= _temp123; _temp124.last_plus_one= _temp123 + 9;
_temp124;})); goto _LL6; _LL32: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp125=( char*)"[extern] transparent"; struct
_tagged_string _temp126; _temp126.curr= _temp125; _temp126.base= _temp125;
_temp126.last_plus_one= _temp125 + 21; _temp126;}),( struct _tagged_string)({
char* _temp127=( char*)"abstract"; struct _tagged_string _temp128; _temp128.curr=
_temp127; _temp128.base= _temp127; _temp128.last_plus_one= _temp127 + 9;
_temp128;})); goto _LL6; _LL34: s1= Cyc_Absyn_Abstract; goto _LL6; _LL6:;}
return s1;} static void Cyc_Tc_field_redecl_okay( struct Cyc_Absyn_Enumfield* f1,
struct Cyc_Absyn_Enumfield* f2, struct Cyc_List_List* inst, struct Cyc_Tcenv_Tenv*
te, struct _tagged_string t, struct _tagged_string* v){ struct Cyc_Position_Segment*
loc= f1->loc; if( Cyc_String_zstrptrcmp((* f1->name).f2,(* f2->name).f2) != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp129= t; struct _tagged_string
_temp130=* v; struct _tagged_string _temp131=*(* f1->name).f2; struct
_tagged_string _temp132=*(* f2->name).f2; xprintf("redeclaration of %.*s %.*s: field name mismatch %.*s != %.*s",
_temp129.last_plus_one - _temp129.curr, _temp129.curr, _temp130.last_plus_one -
_temp130.curr, _temp130.curr, _temp131.last_plus_one - _temp131.curr, _temp131.curr,
_temp132.last_plus_one - _temp132.curr, _temp132.curr);}));} if((( f1->tag == 0?
f2->tag != 0: 0)? 1:( f1->tag != 0? f2->tag == 0: 0))? 1:(( f1->tag != 0? f2->tag
!= 0: 0)? Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp133= f1->tag; if( _temp133 == 0){ _throw( Null_Exception);} _temp133;})) !=
Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp134= f2->tag; if( _temp134 == 0){ _throw( Null_Exception);} _temp134;})): 0)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp135= t; struct _tagged_string
_temp136=* v; struct _tagged_string _temp137=*(* f1->name).f2; xprintf("redeclaration of %.*s %.*s: tag mismatch for field %.*s",
_temp135.last_plus_one - _temp135.curr, _temp135.curr, _temp136.last_plus_one -
_temp136.curr, _temp136.curr, _temp137.last_plus_one - _temp137.curr, _temp137.curr);}));}{
struct Cyc_List_List* tvs1= f1->tvs; struct Cyc_List_List* tvs2= f2->tvs; if(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs1) !=(( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp138= t; struct _tagged_string _temp139=* v; struct
_tagged_string _temp140=*(* f1->name).f2; xprintf("redeclaration of %.*s %.*s, field %.*s: type parameter number mismatch",
_temp138.last_plus_one - _temp138.curr, _temp138.curr, _temp139.last_plus_one -
_temp139.curr, _temp139.curr, _temp140.last_plus_one - _temp140.curr, _temp140.curr);}));}
for( 0; tvs1 != 0; tvs1= tvs1->tl, tvs2= tvs2->tl){ inst=({ struct Cyc_List_List*
_temp141=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp141->hd=( void*)({ struct _tuple4* _temp142=( struct _tuple4*) GC_malloc(
sizeof( struct _tuple4)); _temp142->f1=( struct Cyc_Absyn_Tvar*) tvs2->hd;
_temp142->f2=({ struct Cyc_Absyn_VarType_struct* _temp143=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp143->tag= Cyc_Absyn_VarType_tag;
_temp143->f1=( struct Cyc_Absyn_Tvar*) tvs1->hd;( void*) _temp143;}); _temp142;});
_temp141->tl= inst; _temp141;});}{ struct Cyc_List_List* typs1= f1->typs; struct
Cyc_List_List* typs2= f2->typs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
typs1) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( typs2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp144= t; struct _tagged_string _temp145=* v;
struct _tagged_string _temp146=*(* f1->name).f2; xprintf("redeclaration of %.*s %.*s, field %.*s: parameter number mismatch",
_temp144.last_plus_one - _temp144.curr, _temp144.curr, _temp145.last_plus_one -
_temp145.curr, _temp145.curr, _temp146.last_plus_one - _temp146.curr, _temp146.curr);}));}
for( 0; typs1 != 0; typs1= typs1->tl, typs2= typs2->tl){ if( ! Cyc_Tcutil_equal_tqual((*((
struct _tuple5*) typs1->hd)).f1,(*(( struct _tuple5*) typs2->hd)).f1)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp147= t; struct _tagged_string _temp148=* v;
struct _tagged_string _temp149=*(* f1->name).f2; xprintf("redeclaration of %.*s %.*s, field %.*s: parameter qualifier",
_temp147.last_plus_one - _temp147.curr, _temp147.curr, _temp148.last_plus_one -
_temp148.curr, _temp148.curr, _temp149.last_plus_one - _temp149.curr, _temp149.curr);}));}{
void* subst_t2= Cyc_Tcutil_substitute( inst,(*(( struct _tuple5*) typs2->hd)).f2);
if( ! Cyc_Tcutil_unify((*(( struct _tuple5*) typs1->hd)).f2, subst_t2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp150= t; struct _tagged_string _temp151=* v;
struct _tagged_string _temp152=*(* f1->name).f2; struct _tagged_string _temp153=
Cyc_Absynpp_typ2string((*(( struct _tuple5*) typs1->hd)).f2); struct
_tagged_string _temp154= Cyc_Absynpp_typ2string( subst_t2); xprintf("redeclaration of %.*s %.*s, field %.*s: parameter type mismatch %.*s != %.*s",
_temp150.last_plus_one - _temp150.curr, _temp150.curr, _temp151.last_plus_one -
_temp151.curr, _temp151.curr, _temp152.last_plus_one - _temp152.curr, _temp152.curr,
_temp153.last_plus_one - _temp153.curr, _temp153.curr, _temp154.last_plus_one -
_temp154.curr, _temp154.curr);}));}}}}}} static int Cyc_Tc_var_redecl_okay(
struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_string* name, void* t, void* sc){ struct Cyc_Core_Opt* ans=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->ordinaries,
name); if( ans == 0){ return 0;}(*(( struct _tuple6*) ans->v)).f2= 1;{ void*
_temp155=(*(( struct _tuple6*) ans->v)).f1; void* _temp161; struct Cyc_Absyn_Vardecl*
_temp163; _LL157: if((( struct _enum_struct*) _temp155)->tag == Cyc_Tcenv_VarRes_tag){
_LL162: _temp161=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp155)->f1; if((
unsigned int) _temp161 > 1u?(( struct _enum_struct*) _temp161)->tag == Cyc_Absyn_Global_b_tag:
0){ _LL164: _temp163=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*)
_temp161)->f1; goto _LL158;} else{ goto _LL159;}} else{ goto _LL159;} _LL159:
goto _LL160; _LL158: if( sc == Cyc_Absyn_Static?( void*) _temp163->sc != Cyc_Absyn_Static:
0){ Cyc_Tcutil_warn( loc,({ struct _tagged_string _temp165=* name; xprintf("static declaration of %.*s follows non-static declaration",
_temp165.last_plus_one - _temp165.curr, _temp165.curr);}));} if(( void*)
_temp163->sc == Cyc_Absyn_Static? sc != Cyc_Absyn_Static: 0){ Cyc_Tcutil_warn(
loc,({ struct _tagged_string _temp166=* name; xprintf("non-static declaration of %.*s follows static declaration",
_temp166.last_plus_one - _temp166.curr, _temp166.curr);}));} if((( void*)
_temp163->sc == Cyc_Absyn_ExternC? sc != Cyc_Absyn_ExternC: 0)? 1:(( void*)
_temp163->sc != Cyc_Absyn_ExternC? sc == Cyc_Absyn_ExternC: 0)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp167=* name; xprintf("incompatible redeclaration of %.*s",
_temp167.last_plus_one - _temp167.curr, _temp167.curr);}));} else{ if((( sc !=
Cyc_Absyn_Extern?( void*) _temp163->sc != Cyc_Absyn_Extern: 0)? !( sc == Cyc_Absyn_Static?(
void*) _temp163->sc == Cyc_Absyn_Static: 0): 0)? !( sc == Cyc_Absyn_Public?(
void*) _temp163->sc == Cyc_Absyn_Public: 0): 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp168=* name; xprintf("incompatible redeclaration of %.*s",
_temp168.last_plus_one - _temp168.curr, _temp168.curr);}));}} if( ! Cyc_Tcutil_unify((
void*) _temp163->type, t)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp169=* name; struct _tagged_string _temp170= Cyc_Absynpp_typ2string( t);
struct _tagged_string _temp171= Cyc_Absynpp_typ2string(( void*) _temp163->type);
xprintf("redeclaration of %.*s at a different type: %.*s != %.*s", _temp169.last_plus_one
- _temp169.curr, _temp169.curr, _temp170.last_plus_one - _temp170.curr, _temp170.curr,
_temp171.last_plus_one - _temp171.curr, _temp171.curr);}));} goto _LL156; _LL160:
goto _LL156; _LL156:;} return 1;} static int Cyc_Tc_is_function_type( void* t){
void* _temp172= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp178;
_LL174: if(( unsigned int) _temp172 > 5u?(( struct _enum_struct*) _temp172)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL179: _temp178=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp172)->f1; goto _LL175;} else{ goto _LL176;}
_LL176: goto _LL177; _LL175: return 1; _LL177: return 0; _LL173:;} static void
Cyc_Tc_tcVardecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct
Cyc_Position_Segment* loc, struct Cyc_Absyn_Vardecl* vd){ struct _tagged_string*
v=(* vd->name).f2; void* t=( void*) vd->type; void* sc=( void*) vd->sc;{ void*
_temp180=(* vd->name).f1; struct Cyc_List_List* _temp188; struct Cyc_List_List*
_temp190; _LL182: if(( unsigned int) _temp180 > 1u?(( struct _enum_struct*)
_temp180)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL189: _temp188=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp180)->f1; if( _temp188 == 0){ goto _LL183;}
else{ goto _LL184;}} else{ goto _LL184;} _LL184: if(( unsigned int) _temp180 > 1u?((
struct _enum_struct*) _temp180)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL191:
_temp190=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp180)->f1;
if( _temp190 == 0){ goto _LL185;} else{ goto _LL186;}} else{ goto _LL186;}
_LL186: goto _LL187; _LL183: goto _LL181; _LL185: goto _LL181; _LL187: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp192=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp193; _temp193.curr= _temp192; _temp193.base= _temp192;
_temp193.last_plus_one= _temp192 + 43; _temp193;})); return; _LL181:;}(* vd->name).f1=({
struct Cyc_Absyn_Abs_n_struct* _temp194=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp194->tag= Cyc_Absyn_Abs_n_tag;
_temp194->f1= te->ns;( void*) _temp194;}); Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t);{ int is_redecl= Cyc_Tc_var_redecl_okay( ge, loc, v, t, sc); if( sc
== Cyc_Absyn_Extern? 1: sc == Cyc_Absyn_ExternC){ if( vd->initializer != 0){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp195=( char*)"extern declaration should not have initializer";
struct _tagged_string _temp196; _temp196.curr= _temp195; _temp196.base= _temp195;
_temp196.last_plus_one= _temp195 + 47; _temp196;}));}} else{ if( ! Cyc_Tc_is_function_type(
t)){ struct Cyc_Absyn_Exp* e; if( vd->initializer == 0){ e= Cyc_Tcutil_default_initializer(
te, t, loc); vd->initializer=( struct Cyc_Absyn_Exp*) e;} else{ e=( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp197= vd->initializer; if( _temp197 == 0){ _throw(
Null_Exception);} _temp197;});{ void* _temp198= Cyc_Tcutil_compress( t); struct
Cyc_Absyn_Exp* _temp204; struct Cyc_Absyn_Tqual* _temp206; void* _temp208;
_LL200: if(( unsigned int) _temp198 > 5u?(( struct _enum_struct*) _temp198)->tag
== Cyc_Absyn_ArrayType_tag: 0){ _LL209: _temp208=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp198)->f1; goto _LL207; _LL207: _temp206=( struct Cyc_Absyn_Tqual*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp198)->f2; goto _LL205; _LL205: _temp204=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp198)->f3; goto
_LL201;} else{ goto _LL202;} _LL202: goto _LL203; _LL201: if( Cyc_Evexp_eval_const_uint_exp(
_temp204) ==( unsigned int) 0){ void* _temp210=( void*) e->r; void* _temp220;
struct _tagged_string _temp222; int _temp224; struct Cyc_List_List* _temp226;
struct Cyc_Core_Opt* _temp228; struct Cyc_List_List* _temp230; int _temp232;
_LL212: if((( struct _enum_struct*) _temp210)->tag == Cyc_Absyn_Const_e_tag){
_LL221: _temp220=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp210)->f1; if((
unsigned int) _temp220 > 1u?(( struct _enum_struct*) _temp220)->tag == Cyc_Absyn_String_c_tag:
0){ _LL225: _temp224=( int)(( struct Cyc_Absyn_String_c_struct*) _temp220)->f1;
goto _LL223; _LL223: _temp222=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp220)->f2; goto _LL213;} else{ goto _LL214;}} else{ goto _LL214;} _LL214:
if((( struct _enum_struct*) _temp210)->tag == Cyc_Absyn_UnresolvedMem_e_tag){
_LL229: _temp228=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp210)->f1; goto _LL227; _LL227: _temp226=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp210)->f2; goto _LL215;} else{ goto
_LL216;} _LL216: if((( struct _enum_struct*) _temp210)->tag == Cyc_Absyn_Array_e_tag){
_LL233: _temp232=( int)(( struct Cyc_Absyn_Array_e_struct*) _temp210)->f1; goto
_LL231; _LL231: _temp230=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp210)->f2; goto _LL217;} else{ goto _LL218;} _LL218: goto _LL219; _LL213: t=(
void*)( vd->type=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp234=( struct
Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp234->tag= Cyc_Absyn_ArrayType_tag; _temp234->f1=( void*) _temp208; _temp234->f2=
_temp206; _temp234->f3= Cyc_Absyn_uint_exp(({ struct _tagged_string _temp235=
_temp222;( unsigned int)( _temp235.last_plus_one - _temp235.curr);}), 0);( void*)
_temp234;})); goto _LL211; _LL215: _temp230= _temp226; goto _LL217; _LL217: t=(
void*)( vd->type=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp236=( struct
Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp236->tag= Cyc_Absyn_ArrayType_tag; _temp236->f1=( void*) _temp208; _temp236->f2=
_temp206; _temp236->f3= Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp230), 0);( void*) _temp236;})); goto _LL211; _LL219:
goto _LL211; _LL211:;} goto _LL199; _LL203: goto _LL199; _LL199:;}}{ void* t2=
Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_assign(
te, e, t)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp237= Cyc_Absynpp_qvar2string(
vd->name); struct _tagged_string _temp238= Cyc_Absynpp_typ2string( t); struct
_tagged_string _temp239= Cyc_Absynpp_typ2string( t2); xprintf("%.*s is declared with type %.*s but initialized with type %.*s",
_temp237.last_plus_one - _temp237.curr, _temp237.curr, _temp238.last_plus_one -
_temp238.curr, _temp238.curr, _temp239.last_plus_one - _temp239.curr, _temp239.curr);}));}
if( ! Cyc_Tcexp_is_const_exp( te, e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp240=( char*)"initializer is not a constant expression";
struct _tagged_string _temp241; _temp241.curr= _temp240; _temp241.base= _temp240;
_temp241.last_plus_one= _temp240 + 41; _temp241;}));}}}} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple6*
_temp242=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6)); _temp242->f1=({
struct Cyc_Tcenv_VarRes_struct* _temp243=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp243->tag= Cyc_Tcenv_VarRes_tag;
_temp243->f1=( void*)({ struct Cyc_Absyn_Global_b_struct* _temp244=( struct Cyc_Absyn_Global_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp244->tag= Cyc_Absyn_Global_b_tag;
_temp244->f1= vd;( void*) _temp244;});( void*) _temp243;}); _temp242->f2=
is_redecl; _temp242;}));}} static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl*
fd){ struct _tagged_string* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void*
_temp245=(* fd->name).f1; struct Cyc_List_List* _temp253; struct Cyc_List_List*
_temp255; _LL247: if(( unsigned int) _temp245 > 1u?(( struct _enum_struct*)
_temp245)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL254: _temp253=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp245)->f1; if( _temp253 == 0){ goto _LL248;}
else{ goto _LL249;}} else{ goto _LL249;} _LL249: if(( unsigned int) _temp245 > 1u?((
struct _enum_struct*) _temp245)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL256:
_temp255=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp245)->f1;
goto _LL250;} else{ goto _LL251;} _LL251: goto _LL252; _LL248: goto _LL246;
_LL250:( void) _throw(({ struct Cyc_Core_Impossible_struct* _temp257=( struct
Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp257=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp258=( char*)"tc: Abs_n in tcFndecl"; struct
_tagged_string _temp259; _temp259.curr= _temp258; _temp259.base= _temp258;
_temp259.last_plus_one= _temp258 + 22; _temp259;})};( struct _xenum_struct*)
_temp257;})); _LL252: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp260=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp261; _temp261.curr= _temp260; _temp261.base= _temp260;
_temp261.last_plus_one= _temp260 + 43; _temp261;})); return; _LL246:;}(* fd->name).f1=({
struct Cyc_Absyn_Abs_n_struct* _temp262=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp262->tag= Cyc_Absyn_Abs_n_tag;
_temp262->f1= te->ns;( void*) _temp262;}); Cyc_Tcutil_check_fndecl_valid_type(
loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ( fd); int is_redecl= Cyc_Tc_var_redecl_okay(
ge, loc, v, t, sc); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple6* _temp263=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp263->f1=({ struct Cyc_Tcenv_VarRes_struct* _temp264=( struct Cyc_Tcenv_VarRes_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp264->tag= Cyc_Tcenv_VarRes_tag;
_temp264->f1=( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp265=( struct Cyc_Absyn_Funname_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct)); _temp265->tag= Cyc_Absyn_Funname_b_tag;
_temp265->f1= fd;( void*) _temp265;});( void*) _temp264;}); _temp263->f2=
is_redecl; _temp263;})); te->le=({ struct Cyc_Core_Opt* _temp266=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp266->v=( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp267=( struct Cyc_Tcenv_Outermost_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp267->tag= Cyc_Tcenv_Outermost_tag; _temp267->f1=( void*) Cyc_Tcenv_new_fenv(
fd);( void*) _temp267;}); _temp266;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if(
! Cyc_Tcenv_all_labels_resolved( te)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp268=( char*)"function has goto statements to undefined labels";
struct _tagged_string _temp269; _temp269.curr= _temp268; _temp269.base= _temp268;
_temp269.last_plus_one= _temp268 + 49; _temp269;}));} te->le= 0;}} static void
Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_string* v=(* td->name).f2;{ void* _temp270=(* td->name).f1; struct Cyc_List_List*
_temp278; struct Cyc_List_List* _temp280; _LL272: if(( unsigned int) _temp270 >
1u?(( struct _enum_struct*) _temp270)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL279:
_temp278=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp270)->f1;
if( _temp278 == 0){ goto _LL273;} else{ goto _LL274;}} else{ goto _LL274;}
_LL274: if(( unsigned int) _temp270 > 1u?(( struct _enum_struct*) _temp270)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL281: _temp280=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp270)->f1; if( _temp280 == 0){ goto _LL275;} else{
goto _LL276;}} else{ goto _LL276;} _LL276: goto _LL277; _LL273: goto _LL271;
_LL275: goto _LL271; _LL277: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp282=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp283; _temp283.curr= _temp282; _temp283.base= _temp282;
_temp283.last_plus_one= _temp282 + 43; _temp283;})); return; _LL271:;} if((( int(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)( ge->typedefs,
v)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp284=* v; xprintf("redeclaration of typedef %.*s",
_temp284.last_plus_one - _temp284.curr, _temp284.curr);})); return;}(* td->name).f1=({
struct Cyc_Absyn_Abs_n_struct* _temp285=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp285->tag= Cyc_Absyn_Abs_n_tag;
_temp285->f1= te->ns;( void*) _temp285;}); Cyc_Tcutil_check_unique_tvars( loc,
td->tvs); Cyc_Tcutil_check_type( loc, te, td->tvs, Cyc_Absyn_AnyKind,( void*) td->defn);
ge->typedefs=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs,
v, td);} static void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ if( sd->name
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp286=( char*)"anonymous structs are not allowed at top level";
struct _tagged_string _temp287; _temp287.curr= _temp286; _temp287.base= _temp286;
_temp287.last_plus_one= _temp286 + 47; _temp287;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)( sd->name)->v)).f2;{ struct Cyc_List_List*
tvs2= sd->tvs; for( 0; tvs2 != 0; tvs2= tvs2->tl){ struct Cyc_Absyn_Conref* c=((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*) tvs2->hd)->kind); void* _temp288=( void*) c->v; void*
_temp296; _LL290: if( _temp288 == Cyc_Absyn_No_constr){ goto _LL291;} else{ goto
_LL292;} _LL292: if(( unsigned int) _temp288 > 1u?(( struct _enum_struct*)
_temp288)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL297: _temp296=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp288)->f1; if( _temp296 == Cyc_Absyn_MemKind){
goto _LL293;} else{ goto _LL294;}} else{ goto _LL294;} _LL294: goto _LL295;
_LL291:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp298=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp298->tag= Cyc_Absyn_Eq_constr_tag; _temp298->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp298;})); continue; _LL293: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp299=* v; struct _tagged_string _temp300=*(( struct Cyc_Absyn_Tvar*)
tvs2->hd)->name; xprintf("struct %.*s attempts to abstract type variable %.*s of kind M",
_temp299.last_plus_one - _temp299.curr, _temp299.curr, _temp300.last_plus_one -
_temp300.curr, _temp300.curr);})); continue; _LL295: continue; _LL289:;}}{
struct Cyc_List_List* tvs= sd->tvs;{ void* _temp301=(*(( struct _tuple0*)( sd->name)->v)).f1;
struct Cyc_List_List* _temp309; struct Cyc_List_List* _temp311; _LL303: if((
unsigned int) _temp301 > 1u?(( struct _enum_struct*) _temp301)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL310: _temp309=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp301)->f1; if( _temp309 == 0){ goto _LL304;} else{ goto _LL305;}} else{ goto
_LL305;} _LL305: if(( unsigned int) _temp301 > 1u?(( struct _enum_struct*)
_temp301)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL312: _temp311=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp301)->f1; if( _temp311 == 0){ goto _LL306;}
else{ goto _LL307;}} else{ goto _LL307;} _LL307: goto _LL308; _LL304: goto
_LL302; _LL306: goto _LL302; _LL308: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp313=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp314; _temp314.curr= _temp313; _temp314.base= _temp313;
_temp314.last_plus_one= _temp313 + 43; _temp314;})); return; _LL302:;}(*((
struct _tuple0*)( sd->name)->v)).f1=({ struct Cyc_Absyn_Abs_n_struct* _temp315=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp315->tag= Cyc_Absyn_Abs_n_tag; _temp315->f1= te->ns;( void*) _temp315;});
Cyc_Tcutil_check_unique_tvars( loc, tvs);{ struct Cyc_Core_Opt* _temp316=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->structdecls, v); struct Cyc_Core_Opt _temp322; struct
Cyc_Absyn_Structdecl** _temp323; _LL318: if( _temp316 == 0){ goto _LL319;} else{
goto _LL320;} _LL320: if( _temp316 == 0){ goto _LL317;} else{ _temp322=*
_temp316; _LL324: _temp323=( struct Cyc_Absyn_Structdecl**) _temp322.v; goto
_LL321;} _LL319: ge->structdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl** data)) Cyc_Dict_insert)(
ge->structdecls, v,( struct Cyc_Absyn_Structdecl**)({ struct _tuple7* _temp325=(
struct _tuple7*) GC_malloc( sizeof( struct _tuple7)); _temp325->f1= sd; _temp325;}));
if( sd->fields == 0){ return;} goto _LL317; _LL321: { struct Cyc_Absyn_Structdecl*
sd2=* _temp323; struct Cyc_List_List* tvs2= sd2->tvs; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp326=* v; xprintf("redeclaration of struct %.*s has a different number of type parameters",
_temp326.last_plus_one - _temp326.curr, _temp326.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1= x1->tl, x2= x2->tl){
struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x1->hd)->kind); struct
Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x2->hd)->kind); if(( void*)
c1->v == Cyc_Absyn_No_constr? c1 != c2: 0){( void*)( c1->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp327=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp327->tag= Cyc_Absyn_Forward_constr_tag;
_temp327->f1= c2;( void*) _temp327;}));} if((( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp328=* v; struct
_tagged_string _temp329= Cyc_Absynpp_ckind2string( c1); struct _tagged_string
_temp330=*(( struct Cyc_Absyn_Tvar*) x1->hd)->name; struct _tagged_string
_temp331= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of struct %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp328.last_plus_one - _temp328.curr, _temp328.curr, _temp329.last_plus_one -
_temp329.curr, _temp329.curr, _temp330.last_plus_one - _temp330.curr, _temp330.curr,
_temp331.last_plus_one - _temp331.curr, _temp331.curr);}));}}( void*)( sd->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) sd->sc, sd->fields,( void*)
sd2->sc, sd2->fields, loc,( struct _tagged_string)({ char* _temp332=( char*)"struct";
struct _tagged_string _temp333; _temp333.curr= _temp332; _temp333.base= _temp332;
_temp333.last_plus_one= _temp332 + 7; _temp333;}), v));{ struct _tuple8 _temp335=({
struct _tuple8 _temp334; _temp334.f1= sd->fields; _temp334.f2= sd2->fields;
_temp334;}); struct Cyc_Core_Opt* _temp345; struct Cyc_Core_Opt* _temp347;
struct Cyc_Core_Opt* _temp349; struct Cyc_Core_Opt* _temp351; struct Cyc_Core_Opt*
_temp353; struct Cyc_Core_Opt* _temp355; struct Cyc_Core_Opt* _temp357; struct
Cyc_Core_Opt* _temp359; _LL337: _LL348: _temp347= _temp335.f1; if( _temp347 == 0){
goto _LL346;} else{ goto _LL339;} _LL346: _temp345= _temp335.f2; if( _temp345 ==
0){ goto _LL338;} else{ goto _LL339;} _LL339: _LL352: _temp351= _temp335.f1;
goto _LL350; _LL350: _temp349= _temp335.f2; if( _temp349 == 0){ goto _LL340;}
else{ goto _LL341;} _LL341: _LL356: _temp355= _temp335.f1; if( _temp355 == 0){
goto _LL354;} else{ goto _LL343;} _LL354: _temp353= _temp335.f2; goto _LL342;
_LL343: _LL360: _temp359= _temp335.f1; goto _LL358; _LL358: _temp357= _temp335.f2;
goto _LL344; _LL338: return; _LL340:* _temp323= sd; goto _LL336; _LL342: sd->fields=
sd2->fields; sd->tvs= tvs; return; _LL344: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0= tvs0->tl, tvs2= tvs2->tl){
inst=({ struct Cyc_List_List* _temp361=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp361->hd=( void*)({ struct _tuple4* _temp362=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp362->f1=( struct Cyc_Absyn_Tvar*)
tvs2->hd; _temp362->f2=({ struct Cyc_Absyn_VarType_struct* _temp363=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp363->tag= Cyc_Absyn_VarType_tag;
_temp363->f1=( struct Cyc_Absyn_Tvar*) tvs0->hd;( void*) _temp363;}); _temp362;});
_temp361->tl= inst; _temp361;});}}{ struct Cyc_List_List* f1s=( struct Cyc_List_List*)(
sd->fields)->v; struct Cyc_List_List* f2s=( struct Cyc_List_List*)( sd2->fields)->v;
for( 0; f1s != 0? f2s != 0: 0; f1s= f1s->tl, f2s= f2s->tl){ struct _tuple9
_temp366; void* _temp367; struct Cyc_Absyn_Tqual* _temp369; struct
_tagged_string* _temp371; struct _tuple9* _temp364=( struct _tuple9*) f1s->hd;
_temp366=* _temp364; _LL372: _temp371= _temp366.f1; goto _LL370; _LL370:
_temp369= _temp366.f2; goto _LL368; _LL368: _temp367= _temp366.f3; goto _LL365;
_LL365: { struct _tuple9 _temp375; void* _temp376; struct Cyc_Absyn_Tqual*
_temp378; struct _tagged_string* _temp380; struct _tuple9* _temp373=( struct
_tuple9*) f2s->hd; _temp375=* _temp373; _LL381: _temp380= _temp375.f1; goto
_LL379; _LL379: _temp378= _temp375.f2; goto _LL377; _LL377: _temp376= _temp375.f3;
goto _LL374; _LL374: if( Cyc_String_zstrptrcmp( _temp371, _temp380) != 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp382=* v; struct _tagged_string _temp383=*
_temp371; struct _tagged_string _temp384=* _temp380; xprintf("redeclaration of struct %.*s: field name mismatch %.*s != %.*s",
_temp382.last_plus_one - _temp382.curr, _temp382.curr, _temp383.last_plus_one -
_temp383.curr, _temp383.curr, _temp384.last_plus_one - _temp384.curr, _temp384.curr);}));}
if( ! Cyc_Tcutil_equal_tqual( _temp369, _temp378)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp385=* v; struct _tagged_string _temp386=* _temp371;
xprintf("redeclaration of struct %.*s: qualifier mismatch on field %.*s",
_temp385.last_plus_one - _temp385.curr, _temp385.curr, _temp386.last_plus_one -
_temp386.curr, _temp386.curr);}));} Cyc_Tcutil_check_type( loc, te, tvs, Cyc_Absyn_MemKind,
_temp367);{ void* subst_t2= Cyc_Tcutil_substitute( inst, _temp376); if( ! Cyc_Tcutil_unify(
_temp367, subst_t2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp387=* v;
struct _tagged_string _temp388=* _temp371; struct _tagged_string _temp389= Cyc_Absynpp_typ2string(
_temp367); struct _tagged_string _temp390= Cyc_Absynpp_typ2string( subst_t2);
xprintf("redeclaration of struct %.*s: type mismatch on field %.*s: %.*s != %.*s",
_temp387.last_plus_one - _temp387.curr, _temp387.curr, _temp388.last_plus_one -
_temp388.curr, _temp388.curr, _temp389.last_plus_one - _temp389.curr, _temp389.curr,
_temp390.last_plus_one - _temp390.curr, _temp390.curr);}));}}}} if( f2s != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp391=* v; struct
_tagged_string _temp392=*(*(( struct _tuple9*) f2s->hd)).f1; xprintf("redeclaration of struct %.*s is missing field %.*s",
_temp391.last_plus_one - _temp391.curr, _temp391.curr, _temp392.last_plus_one -
_temp392.curr, _temp392.curr);}));} if( f1s != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp393=* v; struct _tagged_string _temp394=*(*(( struct
_tuple9*) f1s->hd)).f1; xprintf("redeclaration of struct %.*s has extra field %.*s",
_temp393.last_plus_one - _temp393.curr, _temp393.curr, _temp394.last_plus_one -
_temp394.curr, _temp394.curr);}));} return;}} _LL336:;} goto _LL317;}} _LL317:;}{
struct Cyc_Absyn_Structdecl** sdp=(( struct Cyc_Absyn_Structdecl**(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->structdecls,
v);* sdp=({ struct Cyc_Absyn_Structdecl* _temp395=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp395->sc=( void*)(( void*)
sd->sc); _temp395->name= sd->name; _temp395->tvs= tvs; _temp395->fields= 0;
_temp395;});{ struct Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs=(
struct Cyc_List_List*)( sd->fields)->v; for( 0; fs != 0; fs= fs->tl){ struct
_tuple9 _temp398; void* _temp399; struct Cyc_Absyn_Tqual* _temp401; struct
_tagged_string* _temp403; struct _tuple9* _temp396=( struct _tuple9*) fs->hd;
_temp398=* _temp396; _LL404: _temp403= _temp398.f1; goto _LL402; _LL402:
_temp401= _temp398.f2; goto _LL400; _LL400: _temp399= _temp398.f3; goto _LL397;
_LL397: if((( int(*)( int(* compare)( struct _tagged_string*, struct
_tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp403)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp405=* _temp403; xprintf("duplicate field %.*s in struct",
_temp405.last_plus_one - _temp405.curr, _temp405.curr);}));} prev_fields=({
struct Cyc_List_List* _temp406=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp406->hd=( void*) _temp403; _temp406->tl=
prev_fields; _temp406;}); Cyc_Tcutil_check_type( loc, te, tvs, Cyc_Absyn_MemKind,
_temp399);}} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple6* _temp407=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp407->f1=({ struct Cyc_Tcenv_StructRes_struct* _temp408=( struct
Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp408->tag= Cyc_Tcenv_StructRes_tag; _temp408->f1= sd;( void*) _temp408;});
_temp407->f2= 1; _temp407;}));* sdp= sd;}}}}} static void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Enumdecl* ed){ if( ed->name == 0){ Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp409=( char*)"anonymous enums are not allowed at top level";
struct _tagged_string _temp410; _temp410.curr= _temp409; _temp410.base= _temp409;
_temp410.last_plus_one= _temp409 + 45; _temp410;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)( ed->name)->v)).f2; struct Cyc_List_List*
tvs= ed->tvs;{ struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0; tvs2= tvs2->tl){
struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) tvs2->hd)->kind); void*
_temp411=( void*) c->v; void* _temp419; _LL413: if( _temp411 == Cyc_Absyn_No_constr){
goto _LL414;} else{ goto _LL415;} _LL415: if(( unsigned int) _temp411 > 1u?((
struct _enum_struct*) _temp411)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL420:
_temp419=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp411)->f1; if(
_temp419 == Cyc_Absyn_MemKind){ goto _LL416;} else{ goto _LL417;}} else{ goto
_LL417;} _LL417: goto _LL418; _LL414:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp421=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp421->tag= Cyc_Absyn_Eq_constr_tag; _temp421->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp421;})); goto _LL412; _LL416: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp422=* v; struct _tagged_string _temp423=*(( struct Cyc_Absyn_Tvar*)
tvs2->hd)->name; xprintf("enum %.*s attempts to abstract type variable %.*s of kind M",
_temp422.last_plus_one - _temp422.curr, _temp422.curr, _temp423.last_plus_one -
_temp423.curr, _temp423.curr);})); goto _LL412; _LL418: goto _LL412; _LL412:;}}{
void* _temp424=(*(( struct _tuple0*)( ed->name)->v)).f1; struct Cyc_List_List*
_temp432; struct Cyc_List_List* _temp434; _LL426: if(( unsigned int) _temp424 >
1u?(( struct _enum_struct*) _temp424)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL433:
_temp432=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp424)->f1;
if( _temp432 == 0){ goto _LL427;} else{ goto _LL428;}} else{ goto _LL428;}
_LL428: if(( unsigned int) _temp424 > 1u?(( struct _enum_struct*) _temp424)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL435: _temp434=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp424)->f1; if( _temp434 == 0){ goto _LL429;} else{
goto _LL430;}} else{ goto _LL430;} _LL430: goto _LL431; _LL427: goto _LL425;
_LL429: goto _LL425; _LL431: Cyc_Tcutil_terr( loc,( struct _tagged_string)({
char* _temp436=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp437; _temp437.curr= _temp436; _temp437.base= _temp436;
_temp437.last_plus_one= _temp436 + 43; _temp437;})); return; _LL425:;}(*((
struct _tuple0*)( ed->name)->v)).f1=({ struct Cyc_Absyn_Abs_n_struct* _temp438=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp438->tag= Cyc_Absyn_Abs_n_tag; _temp438->f1= te->ns;( void*) _temp438;});
Cyc_Tcutil_check_unique_tvars( loc, tvs);{ struct Cyc_Core_Opt* _temp439=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->enumdecls, v); struct Cyc_Core_Opt _temp445; struct
Cyc_Absyn_Enumdecl** _temp446; _LL441: if( _temp439 == 0){ goto _LL442;} else{
goto _LL443;} _LL443: if( _temp439 == 0){ goto _LL440;} else{ _temp445=*
_temp439; _LL447: _temp446=( struct Cyc_Absyn_Enumdecl**) _temp445.v; goto
_LL444;} _LL442: ge->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)(
ge->enumdecls, v,( struct Cyc_Absyn_Enumdecl**)({ struct _tuple10* _temp448=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp448->f1= ed;
_temp448;})); if( ed->fields == 0){ return;} goto _LL440; _LL444: { struct Cyc_Absyn_Enumdecl*
ed2=* _temp446; struct Cyc_List_List* tvs2= ed2->tvs; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp449=* v; xprintf("redeclaration of enum %.*s has a different number of type parameters",
_temp449.last_plus_one - _temp449.curr, _temp449.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1= x1->tl, x2= x2->tl){
struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x1->hd)->kind); struct
Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x))
Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*) x2->hd)->kind); if(( void*)
c1->v == Cyc_Absyn_No_constr? c1 != c2: 0){( void*)( c1->v=( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp450=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp450->tag= Cyc_Absyn_Forward_constr_tag;
_temp450->f1= c2;( void*) _temp450;}));} if((( void*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp451=* v; struct
_tagged_string _temp452= Cyc_Absynpp_ckind2string( c1); struct _tagged_string
_temp453=*(( struct Cyc_Absyn_Tvar*) x1->hd)->name; struct _tagged_string
_temp454= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of enum %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp451.last_plus_one - _temp451.curr, _temp451.curr, _temp452.last_plus_one -
_temp452.curr, _temp452.curr, _temp453.last_plus_one - _temp453.curr, _temp453.curr,
_temp454.last_plus_one - _temp454.curr, _temp454.curr);}));}}( void*)( ed->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ed->sc, ed->fields,( void*)
ed2->sc, ed2->fields, loc,( struct _tagged_string)({ char* _temp455=( char*)"enum";
struct _tagged_string _temp456; _temp456.curr= _temp455; _temp456.base= _temp455;
_temp456.last_plus_one= _temp455 + 5; _temp456;}), v));{ struct _tuple8 _temp458=({
struct _tuple8 _temp457; _temp457.f1= ed->fields; _temp457.f2= ed2->fields;
_temp457;}); struct Cyc_Core_Opt* _temp468; struct Cyc_Core_Opt* _temp470;
struct Cyc_Core_Opt* _temp472; struct Cyc_Core_Opt* _temp474; struct Cyc_Core_Opt*
_temp476; struct Cyc_Core_Opt* _temp478; struct Cyc_Core_Opt* _temp480; struct
Cyc_Core_Opt* _temp482; _LL460: _LL471: _temp470= _temp458.f1; if( _temp470 == 0){
goto _LL469;} else{ goto _LL462;} _LL469: _temp468= _temp458.f2; if( _temp468 ==
0){ goto _LL461;} else{ goto _LL462;} _LL462: _LL475: _temp474= _temp458.f1;
goto _LL473; _LL473: _temp472= _temp458.f2; if( _temp472 == 0){ goto _LL463;}
else{ goto _LL464;} _LL464: _LL479: _temp478= _temp458.f1; if( _temp478 == 0){
goto _LL477;} else{ goto _LL466;} _LL477: _temp476= _temp458.f2; goto _LL465;
_LL466: _LL483: _temp482= _temp458.f1; goto _LL481; _LL481: _temp480= _temp458.f2;
goto _LL467; _LL461: return; _LL463:* _temp446= ed; goto _LL459; _LL465: ed->fields=
ed2->fields; ed->tvs= tvs2; return; _LL467: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0= tvs0->tl, tvs2= tvs2->tl){
inst=({ struct Cyc_List_List* _temp484=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp484->hd=( void*)({ struct _tuple4* _temp485=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp485->f1=( struct Cyc_Absyn_Tvar*)
tvs2->hd; _temp485->f2=({ struct Cyc_Absyn_VarType_struct* _temp486=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp486->tag= Cyc_Absyn_VarType_tag;
_temp486->f1=( struct Cyc_Absyn_Tvar*) tvs0->hd;( void*) _temp486;}); _temp485;});
_temp484->tl= inst; _temp484;});}}{ struct Cyc_List_List* f1s=( struct Cyc_List_List*)(
ed->fields)->v; struct Cyc_List_List* f2s=( struct Cyc_List_List*)( ed2->fields)->v;
for( 0; f1s != 0? f2s != 0: 0; f1s= f1s->tl, f2s= f2s->tl){(*(( struct Cyc_Absyn_Enumfield*)
f1s->hd)->name).f1=({ struct Cyc_Absyn_Abs_n_struct* _temp487=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp487->tag= Cyc_Absyn_Abs_n_tag;
_temp487->f1= te->ns;( void*) _temp487;});{ struct Cyc_List_List* alltvs=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tvs,(( struct Cyc_Absyn_Enumfield*) f1s->hd)->tvs); Cyc_Tcutil_check_unique_tvars(((
struct Cyc_Absyn_Enumfield*) f1s->hd)->loc, alltvs);{ struct Cyc_List_List* typs=((
struct Cyc_Absyn_Enumfield*) f1s->hd)->typs; for( 0; typs != 0; typs= typs->tl){
void* t=(*(( struct _tuple5*) typs->hd)).f2; Cyc_Tcutil_check_type((( struct Cyc_Absyn_Enumfield*)
f1s->hd)->loc, te, alltvs, Cyc_Absyn_MemKind, t);}} Cyc_Tc_field_redecl_okay((
struct Cyc_Absyn_Enumfield*) f1s->hd,( struct Cyc_Absyn_Enumfield*) f2s->hd,
inst, te,( struct _tagged_string)({ char* _temp488=( char*)"enum"; struct
_tagged_string _temp489; _temp489.curr= _temp488; _temp489.base= _temp488;
_temp489.last_plus_one= _temp488 + 5; _temp489;}), v);}} if( f1s != 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp490=* v; struct _tagged_string _temp491=*(*((
struct Cyc_Absyn_Enumfield*) f1s->hd)->name).f2; xprintf("redeclaration of enum %.*s has extra field %.*s",
_temp490.last_plus_one - _temp490.curr, _temp490.curr, _temp491.last_plus_one -
_temp491.curr, _temp491.curr);}));} if( f2s != 0){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp492=* v; struct _tagged_string _temp493=*(*(( struct
Cyc_Absyn_Enumfield*) f2s->hd)->name).f2; xprintf("redeclaration of enum %.*s is missing field %.*s",
_temp492.last_plus_one - _temp492.curr, _temp492.curr, _temp493.last_plus_one -
_temp493.curr, _temp493.curr);}));} return;}} _LL459:;} goto _LL440;}} _LL440:;}{
struct Cyc_Absyn_Enumdecl** edp=(( struct Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup)( ge->enumdecls, v);* edp=({
struct Cyc_Absyn_Enumdecl* _temp494=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp494->sc=( void*)(( void*) ed->sc);
_temp494->name= ed->name; _temp494->tvs= tvs; _temp494->fields= 0; _temp494;});{
struct Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs=( struct Cyc_List_List*)(
ed->fields)->v; for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Enumfield* f=(
struct Cyc_Absyn_Enumfield*) fs->hd; if((( int(*)( int(* compare)( struct
_tagged_string*, struct _tagged_string*), struct Cyc_List_List* l, struct
_tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,(* f->name).f2)){
Cyc_Tcutil_terr( f->loc,({ struct _tagged_string _temp495=*(* f->name).f2;
xprintf("duplicate field name %.*s", _temp495.last_plus_one - _temp495.curr,
_temp495.curr);}));} prev_fields=({ struct Cyc_List_List* _temp496=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp496->hd=( void*)(* f->name).f2;
_temp496->tl= prev_fields; _temp496;}); if( f->tag != 0){ Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp497= f->tag; if( _temp497 ==
0){ _throw( Null_Exception);} _temp497;}));}{ struct Cyc_List_List* tvs= f->tvs;
for( 0; tvs != 0; tvs= tvs->tl){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)
tvs->hd; struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind); void* _temp498=( void*) c->v; void*
_temp506; _LL500: if( _temp498 == Cyc_Absyn_No_constr){ goto _LL501;} else{ goto
_LL502;} _LL502: if(( unsigned int) _temp498 > 1u?(( struct _enum_struct*)
_temp498)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL507: _temp506=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp498)->f1; if( _temp506 == Cyc_Absyn_MemKind){
goto _LL503;} else{ goto _LL504;}} else{ goto _LL504;} _LL504: goto _LL505;
_LL501:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp508=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp508->tag= Cyc_Absyn_Eq_constr_tag; _temp508->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp508;})); goto _LL499; _LL503: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp509=*(* f->name).f2; xprintf("field %.*s abstracts type variable of kind M",
_temp509.last_plus_one - _temp509.curr, _temp509.curr);})); goto _LL499; _LL505:
goto _LL499; _LL499:;}}{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, f->tvs);
Cyc_Tcutil_check_unique_tvars( loc, alltvs);{ struct Cyc_List_List* typs= f->typs;
for( 0; typs != 0; typs= typs->tl){ void* t=(*(( struct _tuple5*) typs->hd)).f2;
Cyc_Tcutil_check_type( f->loc, te, alltvs, Cyc_Absyn_MemKind, t);}}(* f->name).f1=({
struct Cyc_Absyn_Abs_n_struct* _temp510=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp510->tag= Cyc_Absyn_Abs_n_tag;
_temp510->f1= te->ns;( void*) _temp510;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)(
ge->ordinaries,(* f->name).f2,({ struct _tuple6* _temp511=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp511->f1=({ struct Cyc_Tcenv_EnumRes_struct*
_temp512=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_EnumRes_struct));
_temp512->tag= Cyc_Tcenv_EnumRes_tag; _temp512->f1= ed; _temp512->f2= f;( void*)
_temp512;}); _temp511->f2= 1; _temp511;}));}}} ge->enumdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl**
data)) Cyc_Dict_insert)( ge->enumdecls, v,( struct Cyc_Absyn_Enumdecl**)({
struct _tuple10* _temp513=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp513->f1= ed; _temp513;}));* edp= ed;}}}} static void Cyc_Tc_tcXenumdecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Xenumdecl* xd){ struct _tagged_string* v=(* xd->name).f2;
struct Cyc_Core_Opt* xd2o;{ struct _handler_cons _temp514; _push_handler(&
_temp514);{ struct _xenum_struct* _temp515=( struct _xenum_struct*) setjmp(
_temp514.handler); if( ! _temp515){ xd2o= Cyc_Tcenv_lookup_xenumdecl( te, loc,
xd->name); _pop_handler();} else{ struct _xenum_struct* _temp517= _temp515;
_LL519: if( _temp517->tag == Cyc_Dict_Absent_tag){ goto _LL520;} else{ goto
_LL521;} _LL521: goto _LL522; _LL520: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp523= Cyc_Absynpp_qvar2string( xd->name); xprintf("qualified xenum declaration %.*s is not an existing xenum",
_temp523.last_plus_one - _temp523.curr, _temp523.curr);})); return; _LL522:(
void) _throw( _temp517); _LL518:;}}}{ void* _temp524=(* xd->name).f1; struct Cyc_List_List*
_temp534; struct Cyc_List_List* _temp536; struct Cyc_List_List* _temp538; _LL526:
if( _temp524 == Cyc_Absyn_Loc_n){ goto _LL527;} else{ goto _LL528;} _LL528: if((
unsigned int) _temp524 > 1u?(( struct _enum_struct*) _temp524)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL535: _temp534=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp524)->f1; if( _temp534 == 0){ goto _LL529;} else{ goto _LL530;}} else{ goto
_LL530;} _LL530: if(( unsigned int) _temp524 > 1u?(( struct _enum_struct*)
_temp524)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL537: _temp536=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp524)->f1; goto _LL531;} else{ goto _LL532;}
_LL532: if(( unsigned int) _temp524 > 1u?(( struct _enum_struct*) _temp524)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL539: _temp538=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp524)->f1; goto _LL533;} else{ goto _LL525;} _LL527:(
void) _throw(({ struct Cyc_Core_Impossible_struct* _temp540=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp540=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp541=( char*)"tcXenumdecl: Loc_n";
struct _tagged_string _temp542; _temp542.curr= _temp541; _temp542.base= _temp541;
_temp542.last_plus_one= _temp541 + 19; _temp542;})};( struct _xenum_struct*)
_temp540;})); _LL529:(* xd->name).f1=({ struct Cyc_Absyn_Abs_n_struct* _temp543=(
struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp543->tag= Cyc_Absyn_Abs_n_tag; _temp543->f1= te->ns;( void*) _temp543;});
goto _LL525; _LL531:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp544=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp544=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp545=( char*)"tcXenumdecl: Rel_n non-null";
struct _tagged_string _temp546; _temp546.curr= _temp545; _temp546.base= _temp545;
_temp546.last_plus_one= _temp545 + 28; _temp546;})};( struct _xenum_struct*)
_temp544;})); _LL533:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp547=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp547=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp548=( char*)"tcXenumdecl: Abs_n"; struct
_tagged_string _temp549; _temp549.curr= _temp548; _temp549.base= _temp548;
_temp549.last_plus_one= _temp548 + 19; _temp549;})};( struct _xenum_struct*)
_temp547;})); _LL525:;} if( xd2o == 0){ struct Cyc_Absyn_Xenumdecl** x=( struct
Cyc_Absyn_Xenumdecl**)({ struct _tuple11* _temp550=( struct _tuple11*) GC_malloc(
sizeof( struct _tuple11)); _temp550->f1=({ struct Cyc_Absyn_Xenumdecl* _temp551=(
struct Cyc_Absyn_Xenumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Xenumdecl));
_temp551->sc=( void*)(( void*) xd->sc); _temp551->name= xd->name; _temp551->fields=
0; _temp551;}); _temp550;}); xd2o=({ struct Cyc_Core_Opt* _temp552=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp552->v=( void*) x; _temp552;});
ge->xenumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key, struct Cyc_Absyn_Xenumdecl** data)) Cyc_Dict_insert)( ge->xenumdecls,
v, x);} else{ struct Cyc_Absyn_Xenumdecl* x=*(( struct Cyc_Absyn_Xenumdecl**)
xd2o->v);( void*)( x->sc=( void*)(( void*(*)( void* s1, struct Cyc_Core_Opt*
fields1, void* s2, struct Cyc_Core_Opt* fields2, struct Cyc_Position_Segment*
loc, struct _tagged_string t, struct _tagged_string* v)) Cyc_Tc_scope_redecl_okay)((
void*) xd->sc,({ struct Cyc_Core_Opt* _temp553=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp553->v=( void*) 0;
_temp553;}),( void*) x->sc,({ struct Cyc_Core_Opt* _temp554=( struct Cyc_Core_Opt*)
GC_malloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp554->v=( void*) 0;
_temp554;}), loc,( struct _tagged_string)({ char* _temp555=( char*)"xenum";
struct _tagged_string _temp556; _temp556.curr= _temp555; _temp556.base= _temp555;
_temp556.last_plus_one= _temp555 + 6; _temp556;}), v));}{ struct Cyc_Absyn_Xenumdecl*
xd_result=*(( struct Cyc_Absyn_Xenumdecl**) xd2o->v); struct Cyc_List_List* fs=
xd->fields; for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Enumfield* f=( struct
Cyc_Absyn_Enumfield*) fs->hd;{ void* _temp557=(* f->name).f1; struct Cyc_List_List*
_temp567; struct Cyc_List_List* _temp569; struct Cyc_List_List* _temp571; _LL559:
if(( unsigned int) _temp557 > 1u?(( struct _enum_struct*) _temp557)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL568: _temp567=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp557)->f1; if( _temp567 == 0){ goto _LL560;} else{ goto _LL561;}} else{ goto
_LL561;} _LL561: if(( unsigned int) _temp557 > 1u?(( struct _enum_struct*)
_temp557)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL570: _temp569=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp557)->f1; goto _LL562;} else{ goto _LL563;}
_LL563: if(( unsigned int) _temp557 > 1u?(( struct _enum_struct*) _temp557)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL572: _temp571=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp557)->f1; goto _LL564;} else{ goto _LL565;} _LL565:
if( _temp557 == Cyc_Absyn_Loc_n){ goto _LL566;} else{ goto _LL558;} _LL560:(* f->name).f1=({
struct Cyc_Absyn_Abs_n_struct* _temp573=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp573->tag= Cyc_Absyn_Abs_n_tag;
_temp573->f1= te->ns;( void*) _temp573;}); goto _LL558; _LL562: goto _LL564;
_LL564: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp574=( char*)"qualified declarations are not allowed\n";
struct _tagged_string _temp575; _temp575.curr= _temp574; _temp575.base= _temp574;
_temp575.last_plus_one= _temp574 + 40; _temp575;})); goto _LL558; _LL566:( void)
_throw(({ struct Cyc_Core_Impossible_struct* _temp576=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp576=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp577=( char*)"tcXenumdecl: Loc_n";
struct _tagged_string _temp578; _temp578.curr= _temp577; _temp578.base= _temp577;
_temp578.last_plus_one= _temp577 + 19; _temp578;})};( struct _xenum_struct*)
_temp576;})); goto _LL558; _LL558:;}{ struct Cyc_List_List* tvs= f->tvs; for( 0;
tvs != 0; tvs= tvs->tl){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*) tvs->hd;
struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( tv->kind); void* _temp579=( void*) c->v; void*
_temp587; _LL581: if( _temp579 == Cyc_Absyn_No_constr){ goto _LL582;} else{ goto
_LL583;} _LL583: if(( unsigned int) _temp579 > 1u?(( struct _enum_struct*)
_temp579)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL588: _temp587=( void*)((
struct Cyc_Absyn_Eq_constr_struct*) _temp579)->f1; if( _temp587 == Cyc_Absyn_MemKind){
goto _LL584;} else{ goto _LL585;}} else{ goto _LL585;} _LL585: goto _LL586;
_LL582:( void*)( c->v=( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp589=(
struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp589->tag= Cyc_Absyn_Eq_constr_tag; _temp589->f1=( void*) Cyc_Absyn_BoxKind;(
void*) _temp589;})); goto _LL580; _LL584: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp590=*(* f->name).f2; xprintf("field %.*s abstracts type variable of kind M",
_temp590.last_plus_one - _temp590.curr, _temp590.curr);})); goto _LL580; _LL586:
goto _LL580; _LL580:;}}{ struct Cyc_List_List* tvs= f->tvs; Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_List_List* typs= f->typs; for( 0; typs != 0; typs= typs->tl){
void* t=(*(( struct _tuple5*) typs->hd)).f2; Cyc_Tcutil_check_type( f->loc, te,
tvs, Cyc_Absyn_MemKind, t);}}{ int dup= 0;{ struct Cyc_List_List* prev_fs=
xd_result->fields; for( 0; prev_fs != 0; prev_fs= prev_fs->tl){ struct Cyc_Absyn_Enumfield*
prev=( struct Cyc_Absyn_Enumfield*) prev_fs->hd; if( Cyc_Absyn_qvar_cmp( f->name,
prev->name) == 0){ Cyc_Tc_field_redecl_okay( prev, f, 0, te,( struct
_tagged_string)({ char* _temp591=( char*)"xenum"; struct _tagged_string _temp592;
_temp592.curr= _temp591; _temp592.base= _temp591; _temp592.last_plus_one=
_temp591 + 6; _temp592;}), v); dup= 1; break;}}} if( dup){ continue;} xd_result->fields=({
struct Cyc_List_List* _temp593=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp593->hd=( void*) f; _temp593->tl= xd_result->fields;
_temp593;});{ struct Cyc_Tcenv_Genv* ge_f; struct Cyc_List_List* ns_f;{ void*
_temp594=(* f->name).f1; struct Cyc_List_List* _temp602; struct Cyc_List_List*
_temp604; _LL596: if(( unsigned int) _temp594 > 1u?(( struct _enum_struct*)
_temp594)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL603: _temp602=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp594)->f1; goto _LL597;} else{ goto _LL598;}
_LL598: if( _temp594 == Cyc_Absyn_Loc_n){ goto _LL599;} else{ goto _LL600;}
_LL600: if(( unsigned int) _temp594 > 1u?(( struct _enum_struct*) _temp594)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL605: _temp604=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp594)->f1; goto _LL601;} else{ goto _LL595;} _LL597:
ns_f= _temp602; goto _LL595; _LL599: goto _LL601; _LL601:( void) _throw(({
struct Cyc_Core_Impossible_struct* _temp606=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp606=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp607=( char*)"tcXenumdecl: Rel_n or Loc_n";
struct _tagged_string _temp608; _temp608.curr= _temp607; _temp608.base= _temp607;
_temp608.last_plus_one= _temp607 + 28; _temp608;})};( struct _xenum_struct*)
_temp606;})); goto _LL595; _LL595:;}{ struct _handler_cons _temp609;
_push_handler(& _temp609);{ struct _xenum_struct* _temp610=( struct
_xenum_struct*) setjmp( _temp609.handler); if( ! _temp610){ ge_f=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
ns_f); _pop_handler();} else{ struct _xenum_struct* _temp612= _temp610; _LL614:
if( _temp612->tag == Cyc_Dict_Absent_tag){ goto _LL615;} else{ goto _LL616;}
_LL616: goto _LL617; _LL615: Cyc_Tcutil_terr( f->loc,({ struct _tagged_string
_temp618= Cyc_Absynpp_qvar2string( f->name); xprintf("bad namespace for xenum field %.*s",
_temp618.last_plus_one - _temp618.curr, _temp618.curr);})); return; _LL617:(
void) _throw( _temp612); _LL613:;}}} ge_f->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)(
ge_f->ordinaries,(* f->name).f2,({ struct _tuple6* _temp619=( struct _tuple6*)
GC_malloc( sizeof( struct _tuple6)); _temp619->f1=({ struct Cyc_Tcenv_XenumRes_struct*
_temp620=( struct Cyc_Tcenv_XenumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_XenumRes_struct));
_temp620->tag= Cyc_Tcenv_XenumRes_tag; _temp620->f1= xd_result; _temp620->f2= f;(
void*) _temp620;}); _temp619->f2= 1; _temp619;}));}}}}}} static int Cyc_Tc_okay_externC(
struct Cyc_Position_Segment* loc, void* sc){ void* _temp621= sc; _LL623: if(
_temp621 == Cyc_Absyn_Static){ goto _LL624;} else{ goto _LL625;} _LL625: if(
_temp621 == Cyc_Absyn_Abstract){ goto _LL626;} else{ goto _LL627;} _LL627: if(
_temp621 == Cyc_Absyn_Public){ goto _LL628;} else{ goto _LL629;} _LL629: if(
_temp621 == Cyc_Absyn_Extern){ goto _LL630;} else{ goto _LL631;} _LL631: if(
_temp621 == Cyc_Absyn_ExternC){ goto _LL632;} else{ goto _LL622;} _LL624: Cyc_Tcutil_warn(
loc,( struct _tagged_string)({ char* _temp633=( char*)"static declaration nested within extern \"C\"";
struct _tagged_string _temp634; _temp634.curr= _temp633; _temp634.base= _temp633;
_temp634.last_plus_one= _temp633 + 44; _temp634;})); return 0; _LL626: Cyc_Tcutil_warn(
loc,( struct _tagged_string)({ char* _temp635=( char*)"abstract declaration nested within extern \"C\"";
struct _tagged_string _temp636; _temp636.curr= _temp635; _temp636.base= _temp635;
_temp636.last_plus_one= _temp635 + 46; _temp636;})); return 0; _LL628: return 1;
_LL630: return 1; _LL632: Cyc_Tcutil_warn( loc,( struct _tagged_string)({ char*
_temp637=( char*)"nested extern \"C\" declaration"; struct _tagged_string
_temp638; _temp638.curr= _temp637; _temp638.base= _temp637; _temp638.last_plus_one=
_temp637 + 30; _temp638;})); return 1; _LL622:;} static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds0, int in_externC){ struct
Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct
Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae, te->ns); struct Cyc_List_List* ds=
ds0; for( 0; ds != 0; ds= ds->tl){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)
ds->hd; struct Cyc_Position_Segment* loc= d->loc; void* _temp639=( void*) d->r;
int _temp663; struct Cyc_Absyn_Exp* _temp665; struct Cyc_Core_Opt* _temp667;
struct Cyc_Core_Opt* _temp669; struct Cyc_Absyn_Pat* _temp671; struct Cyc_Absyn_Vardecl*
_temp673; struct Cyc_Absyn_Fndecl* _temp675; struct Cyc_Absyn_Typedefdecl*
_temp677; struct Cyc_Absyn_Structdecl* _temp679; struct Cyc_Absyn_Enumdecl*
_temp681; struct Cyc_Absyn_Xenumdecl* _temp683; struct Cyc_List_List* _temp685;
struct _tagged_string* _temp687; struct Cyc_List_List* _temp689; struct _tuple0*
_temp691; struct _tuple0 _temp693; struct _tagged_string* _temp694; void*
_temp696; struct Cyc_List_List* _temp698; _LL641: if(( unsigned int) _temp639 >
1u?(( struct _enum_struct*) _temp639)->tag == Cyc_Absyn_Let_d_tag: 0){ _LL672:
_temp671=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp639)->f1;
goto _LL670; _LL670: _temp669=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp639)->f2; goto _LL668; _LL668: _temp667=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Let_d_struct*) _temp639)->f3; goto _LL666; _LL666: _temp665=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp639)->f4; goto _LL664;
_LL664: _temp663=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp639)->f5; goto
_LL642;} else{ goto _LL643;} _LL643: if( _temp639 == Cyc_Absyn_Union_d){ goto
_LL644;} else{ goto _LL645;} _LL645: if(( unsigned int) _temp639 > 1u?(( struct
_enum_struct*) _temp639)->tag == Cyc_Absyn_Var_d_tag: 0){ _LL674: _temp673=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*) _temp639)->f1; goto
_LL646;} else{ goto _LL647;} _LL647: if(( unsigned int) _temp639 > 1u?(( struct
_enum_struct*) _temp639)->tag == Cyc_Absyn_Fn_d_tag: 0){ _LL676: _temp675=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp639)->f1; goto
_LL648;} else{ goto _LL649;} _LL649: if(( unsigned int) _temp639 > 1u?(( struct
_enum_struct*) _temp639)->tag == Cyc_Absyn_Typedef_d_tag: 0){ _LL678: _temp677=(
struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp639)->f1;
goto _LL650;} else{ goto _LL651;} _LL651: if(( unsigned int) _temp639 > 1u?((
struct _enum_struct*) _temp639)->tag == Cyc_Absyn_Struct_d_tag: 0){ _LL680:
_temp679=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp639)->f1; goto _LL652;} else{ goto _LL653;} _LL653: if(( unsigned int)
_temp639 > 1u?(( struct _enum_struct*) _temp639)->tag == Cyc_Absyn_Enum_d_tag: 0){
_LL682: _temp681=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp639)->f1; goto _LL654;} else{ goto _LL655;} _LL655: if(( unsigned int)
_temp639 > 1u?(( struct _enum_struct*) _temp639)->tag == Cyc_Absyn_Xenum_d_tag:
0){ _LL684: _temp683=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*)
_temp639)->f1; goto _LL656;} else{ goto _LL657;} _LL657: if(( unsigned int)
_temp639 > 1u?(( struct _enum_struct*) _temp639)->tag == Cyc_Absyn_Namespace_d_tag:
0){ _LL688: _temp687=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp639)->f1; goto _LL686; _LL686: _temp685=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Namespace_d_struct*) _temp639)->f2; goto _LL658;} else{ goto _LL659;}
_LL659: if(( unsigned int) _temp639 > 1u?(( struct _enum_struct*) _temp639)->tag
== Cyc_Absyn_Using_d_tag: 0){ _LL692: _temp691=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp639)->f1; _temp693=* _temp691; _LL697: _temp696= _temp693.f1; goto _LL695;
_LL695: _temp694= _temp693.f2; goto _LL690; _LL690: _temp689=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp639)->f2; goto _LL660;} else{ goto _LL661;}
_LL661: if(( unsigned int) _temp639 > 1u?(( struct _enum_struct*) _temp639)->tag
== Cyc_Absyn_ExternC_d_tag: 0){ _LL699: _temp698=( struct Cyc_List_List*)((
struct Cyc_Absyn_ExternC_d_struct*) _temp639)->f1; goto _LL662;} else{ goto
_LL640;} _LL642: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp700=(
char*)"top level let-declarations are not implemented"; struct _tagged_string
_temp701; _temp701.curr= _temp700; _temp701.base= _temp700; _temp701.last_plus_one=
_temp700 + 47; _temp701;})); goto _LL640; _LL644: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp702=( char*)"union is not implemented"; struct
_tagged_string _temp703; _temp703.curr= _temp702; _temp703.base= _temp702;
_temp703.last_plus_one= _temp702 + 25; _temp703;})); goto _LL640; _LL646: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp673->sc): 0){( void*)(
_temp673->sc=( void*) Cyc_Absyn_ExternC);} Cyc_Tc_tcVardecl( te, ge, loc,
_temp673); goto _LL640; _LL648: if( in_externC? Cyc_Tc_okay_externC( d->loc,(
void*) _temp675->sc): 0){( void*)( _temp675->sc=( void*) Cyc_Absyn_ExternC);}
Cyc_Tc_tcFndecl( te, ge, loc, _temp675); goto _LL640; _LL650: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp677); goto _LL640; _LL652: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp679->sc): 0){( void*)( _temp679->sc=( void*) Cyc_Absyn_ExternC);}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp679); goto _LL640; _LL654: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp681->sc): 0){( void*)( _temp681->sc=(
void*) Cyc_Absyn_ExternC);} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp681); goto
_LL640; _LL656: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp683->sc):
0){( void*)( _temp683->sc=( void*) Cyc_Absyn_ExternC);} Cyc_Tc_tcXenumdecl( te,
ge, loc, _temp683); goto _LL640; _LL658: { struct Cyc_List_List* ns= te->ns;
struct Cyc_List_List* ns2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( ns,({ struct Cyc_List_List* _temp704=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp704->hd=(
void*) _temp687; _temp704->tl= 0; _temp704;})); if( !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_member)( ge->namespaces, _temp687)){ ge->namespaces=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)(
ge->namespaces, _temp687); te->ae=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key, struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae,
ns2, Cyc_Tcenv_empty_genv());} te->ns= ns2; Cyc_Tc_tc_decls( te, _temp685,
in_externC); te->ns= ns; goto _LL640;} _LL660: { struct _tagged_string* first;
struct Cyc_List_List* rest;{ void* _temp705= _temp696; struct Cyc_List_List*
_temp717; struct Cyc_List_List* _temp719; struct Cyc_List_List* _temp721; struct
Cyc_List_List _temp723; struct Cyc_List_List* _temp724; struct _tagged_string*
_temp726; struct Cyc_List_List* _temp728; struct Cyc_List_List _temp730; struct
Cyc_List_List* _temp731; struct _tagged_string* _temp733; _LL707: if( _temp705
== Cyc_Absyn_Loc_n){ goto _LL708;} else{ goto _LL709;} _LL709: if(( unsigned int)
_temp705 > 1u?(( struct _enum_struct*) _temp705)->tag == Cyc_Absyn_Rel_n_tag: 0){
_LL718: _temp717=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp705)->f1; if( _temp717 == 0){ goto _LL710;} else{ goto _LL711;}} else{ goto
_LL711;} _LL711: if(( unsigned int) _temp705 > 1u?(( struct _enum_struct*)
_temp705)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL720: _temp719=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp705)->f1; if( _temp719 == 0){ goto _LL712;}
else{ goto _LL713;}} else{ goto _LL713;} _LL713: if(( unsigned int) _temp705 > 1u?((
struct _enum_struct*) _temp705)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL722:
_temp721=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp705)->f1;
if( _temp721 == 0){ goto _LL715;} else{ _temp723=* _temp721; _LL727: _temp726=(
struct _tagged_string*) _temp723.hd; goto _LL725; _LL725: _temp724=( struct Cyc_List_List*)
_temp723.tl; goto _LL714;}} else{ goto _LL715;} _LL715: if(( unsigned int)
_temp705 > 1u?(( struct _enum_struct*) _temp705)->tag == Cyc_Absyn_Abs_n_tag: 0){
_LL729: _temp728=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp705)->f1; if( _temp728 == 0){ goto _LL706;} else{ _temp730=* _temp728;
_LL734: _temp733=( struct _tagged_string*) _temp730.hd; goto _LL732; _LL732:
_temp731=( struct Cyc_List_List*) _temp730.tl; goto _LL716;}} else{ goto _LL706;}
_LL708: goto _LL710; _LL710: goto _LL712; _LL712: first= _temp694; rest= 0; goto
_LL706; _LL714: _temp733= _temp726; _temp731= _temp724; goto _LL716; _LL716:
first= _temp733; rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp731,({ struct Cyc_List_List*
_temp735=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp735->hd=( void*) _temp694; _temp735->tl= 0; _temp735;})); goto _LL706;
_LL706:;}{ struct Cyc_List_List* ns2= Cyc_Tcenv_resolve_namespace( te, loc,
first, rest); ge->availables=({ struct Cyc_List_List* _temp736=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp736->hd=( void*) ns2; _temp736->tl=
ge->availables; _temp736;}); Cyc_Tc_tc_decls( te, _temp689, in_externC); ge->availables=(
ge->availables)->tl; goto _LL640;}} _LL662: Cyc_Tc_tc_decls( te, _temp698, 1);
goto _LL640; _LL640:;}} static char _temp739[ 4u]="Cyc"; static struct
_tagged_string Cyc_Tc_cyc_string=( struct _tagged_string){ _temp739, _temp739,
_temp739 + 4u}; static struct _tagged_string* Cyc_Tc_cyc_ns=& Cyc_Tc_cyc_string;
void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int add_cyc_namespace, struct Cyc_List_List*
ds){ if( add_cyc_namespace){ ds=({ struct Cyc_List_List* _temp740=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp740->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp741=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp741->r=( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp742=( struct
Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp742->tag= Cyc_Absyn_Namespace_d_tag; _temp742->f1= Cyc_Tc_cyc_ns; _temp742->f2=
ds;( void*) _temp742;}); _temp741->loc= 0; _temp741;}); _temp740->tl= 0;
_temp740;});} Cyc_Tc_tc_decls( te, ds, 0);} static int Cyc_Tc_vardecl_needed(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Decl* d){ void* _temp743=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp753; struct Cyc_List_List* _temp755; struct Cyc_List_List**
_temp757; struct _tuple0* _temp758; struct Cyc_List_List* _temp760; struct Cyc_List_List**
_temp762; struct _tagged_string* _temp763; _LL745: if(( unsigned int) _temp743 >
1u?(( struct _enum_struct*) _temp743)->tag == Cyc_Absyn_Var_d_tag: 0){ _LL754:
_temp753=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*) _temp743)->f1;
goto _LL746;} else{ goto _LL747;} _LL747: if(( unsigned int) _temp743 > 1u?((
struct _enum_struct*) _temp743)->tag == Cyc_Absyn_Using_d_tag: 0){ _LL759:
_temp758=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp743)->f1;
goto _LL756; _LL756: _temp755=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp743)->f2; _temp757=&(( struct Cyc_Absyn_Using_d_struct*) _temp743)->f2;
goto _LL748;} else{ goto _LL749;} _LL749: if(( unsigned int) _temp743 > 1u?((
struct _enum_struct*) _temp743)->tag == Cyc_Absyn_Namespace_d_tag: 0){ _LL764:
_temp763=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp743)->f1; goto _LL761; _LL761: _temp760=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Namespace_d_struct*) _temp743)->f2; _temp762=&(( struct Cyc_Absyn_Namespace_d_struct*)
_temp743)->f2; goto _LL750;} else{ goto _LL751;} _LL751: goto _LL752; _LL746:
if(( void*) _temp753->sc != Cyc_Absyn_Extern){ return 1;}{ struct _tuple0
_temp767; struct _tagged_string* _temp768; void* _temp770; struct _tuple0*
_temp765= _temp753->name; _temp767=* _temp765; _LL771: _temp770= _temp767.f1;
goto _LL769; _LL769: _temp768= _temp767.f2; goto _LL766; _LL766: { struct Cyc_List_List*
ns;{ void* _temp772= _temp770; struct Cyc_List_List* _temp780; struct Cyc_List_List*
_temp782; _LL774: if( _temp772 == Cyc_Absyn_Loc_n){ goto _LL775;} else{ goto
_LL776;} _LL776: if(( unsigned int) _temp772 > 1u?(( struct _enum_struct*)
_temp772)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL781: _temp780=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp772)->f1; goto _LL777;} else{ goto _LL778;}
_LL778: if(( unsigned int) _temp772 > 1u?(( struct _enum_struct*) _temp772)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL783: _temp782=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp772)->f1; goto _LL779;} else{ goto _LL773;} _LL775:
ns= 0; goto _LL773; _LL777: ns= _temp780; goto _LL773; _LL779: ns= _temp782;
goto _LL773; _LL773:;}{ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
ns); return(*(( struct _tuple6*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_string* key)) Cyc_Dict_lookup)( ge->ordinaries, _temp768)).f2;}}} _LL748:
_temp762= _temp757; goto _LL750; _LL750:* _temp762= Cyc_Tc_treeshake( te,*
_temp762); return 1; _LL752: return 1; _LL744:;} struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)(
int(* f)( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Decl*), struct Cyc_Tcenv_Tenv*
env, struct Cyc_List_List* l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, te, ds);}