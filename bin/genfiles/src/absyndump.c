#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2;}; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3;}; struct _tuple2{ struct
Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2;}; struct _tuple3{ struct
_tagged_string* f1; struct Cyc_Absyn_Tqual* f2; void* f3;}; struct _tuple4{
struct Cyc_Absyn_Tqual* f1; void* f2; struct Cyc_List_List* f3;}; struct _tuple5{
struct Cyc_Absyn_Tqual* f1; void* f2;}; struct _tuple6{ struct Cyc_List_List* f1;
struct Cyc_List_List* f2;}; struct _tuple7{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp*
f2;}; struct _tuple8{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2;};
typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v;}; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1;}; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1;}; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1;}; extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{
char* tag;}; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1;}; extern struct _tagged_string Cyc_Core_string_of_int(
int); extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stdout; typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t;
extern int Cyc_Stdio_fputc( int, struct Cyc_Stdio___sFILE*); extern char Cyc_Stdio_FileOpenError_tag[
14u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;};
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag;}; extern int Cyc_Stdio_file_string_write( struct Cyc_Stdio___sFILE*
fd, struct _tagged_string src, int src_offset, int max_count); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl;}; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern int Cyc_List_length( struct Cyc_List_List* x); extern char Cyc_List_List_empty_tag[
11u]; struct Cyc_List_List_empty_struct{ char* tag;}; extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag;}; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag;}; struct Cyc_Lineno_Pos{ struct
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
struct Cyc_Absyn_EmptyAnnot_struct{ char* tag;}; extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x); extern int Cyc_Absyn_is_format_prim(
void* p); struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern int Cyc_Absynpp_qvar_to_Cids;
extern int Cyc_Absynpp_exp_prec( struct Cyc_Absyn_Exp*); extern struct
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
void* t); extern void Cyc_Absyndump_dumptyp( void*); extern void Cyc_Absyndump_dumpntyp(
void* t); extern void Cyc_Absyndump_dumpexp( struct Cyc_Absyn_Exp*); extern void
Cyc_Absyndump_dumpexp_prec( int, struct Cyc_Absyn_Exp*); extern void Cyc_Absyndump_dumppat(
struct Cyc_Absyn_Pat*); extern void Cyc_Absyndump_dumpstmt( struct Cyc_Absyn_Stmt*);
extern void Cyc_Absyndump_dumpdecl( struct Cyc_Absyn_Decl*); extern void Cyc_Absyndump_dumptqtd(
struct Cyc_Absyn_Tqual*, void*, void(* f)( void*), void*); typedef struct
_tagged_string Cyc_Absyndump_dump_string_t; struct Cyc_Stdio___sFILE** Cyc_Absyndump_dump_file=&
Cyc_Stdio_stdout; void Cyc_Absyndump_ignore( void* x){ return;} static
unsigned int Cyc_Absyndump_pos= 0; void Cyc_Absyndump_dump( struct
_tagged_string s){ int sz=( int)({ struct _tagged_string _temp0= s;(
unsigned int)( _temp0.last_plus_one - _temp0.curr);}); if( !(( int)({ struct
_tagged_string _temp1= s; char* _temp3= _temp1.curr +( sz - 1); if( _temp3 <
_temp1.base? 1: _temp3 >= _temp1.last_plus_one){ _throw( Null_Exception);}*
_temp3;}))){ -- sz;} Cyc_Absyndump_pos +=( unsigned int)( sz + 1); if( Cyc_Absyndump_pos
>( unsigned int) 80){ Cyc_Absyndump_pos=( unsigned int) sz; Cyc_Stdio_fputc((
int)'\n',( struct Cyc_Stdio___sFILE*)* Cyc_Absyndump_dump_file);} else{ Cyc_Stdio_fputc((
int)' ',( struct Cyc_Stdio___sFILE*)* Cyc_Absyndump_dump_file);} Cyc_Stdio_file_string_write((
struct Cyc_Stdio___sFILE*)* Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_nospace(
struct _tagged_string s){ int sz=( int)({ struct _tagged_string _temp4= s;(
unsigned int)( _temp4.last_plus_one - _temp4.curr);}); if( !(( int)({ struct
_tagged_string _temp5= s; char* _temp7= _temp5.curr +( sz - 1); if( _temp7 <
_temp5.base? 1: _temp7 >= _temp5.last_plus_one){ _throw( Null_Exception);}*
_temp7;}))){ -- sz;} Cyc_Absyndump_pos +=( unsigned int) sz; Cyc_Stdio_file_string_write((
struct Cyc_Stdio___sFILE*)* Cyc_Absyndump_dump_file, s, 0, sz);} void Cyc_Absyndump_dump_char(
int c){ ++ Cyc_Absyndump_pos; Cyc_Stdio_fputc( c,( struct Cyc_Stdio___sFILE*)*
Cyc_Absyndump_dump_file);} void Cyc_Absyndump_dump_str( struct _tagged_string* s){
Cyc_Absyndump_dump(* s);} void Cyc_Absyndump_dump_semi(){ Cyc_Absyndump_dump_char((
int)';');} void Cyc_Absyndump_dump_sep( void(* f)( void*), struct Cyc_List_List*
l, struct _tagged_string sep){ if( l == 0){ return;} for( 0; l->tl != 0; l= l->tl){
f(( void*) l->hd); Cyc_Absyndump_dump_nospace( sep);} f(( void*) l->hd);} void
Cyc_Absyndump_dump_sep_c( void(* f)( void*, void*), void* env, struct Cyc_List_List*
l, struct _tagged_string sep){ if( l == 0){ return;} for( 0; l->tl != 0; l= l->tl){
f( env,( void*) l->hd); Cyc_Absyndump_dump_nospace( sep);} f( env,( void*) l->hd);}
void Cyc_Absyndump_group( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep){ Cyc_Absyndump_dump_nospace(
start);(( void(*)( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string sep)) Cyc_Absyndump_dump_sep)( f, l, sep); Cyc_Absyndump_dump_nospace(
end);} void Cyc_Absyndump_group_c( void(* f)( void*, void*), void* env, struct
Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end, struct
_tagged_string sep){ Cyc_Absyndump_dump_nospace( start);(( void(*)( void(* f)(
void*, void*), void* env, struct Cyc_List_List* l, struct _tagged_string sep))
Cyc_Absyndump_dump_sep_c)( f, env, l, sep); Cyc_Absyndump_dump_nospace( end);}
void Cyc_Absyndump_egroup( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep){ if(
l != 0){(( void(*)( void(* f)( void*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
f, l, start, end, sep);}} void Cyc_Absyndump_dumpqvar( struct _tuple0* v){
struct Cyc_List_List* nsl= 0;{ void* _temp8=(* v).f1; struct Cyc_List_List*
_temp20; struct Cyc_List_List* _temp22; struct Cyc_List_List* _temp24; struct
Cyc_List_List* _temp26; _LL10: if( _temp8 == Cyc_Absyn_Loc_n){ goto _LL11;}
else{ goto _LL12;} _LL12: if(( unsigned int) _temp8 > 1u?(( struct _enum_struct*)
_temp8)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL21: _temp20=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp8)->f1; if( _temp20 == 0){ goto _LL13;}
else{ goto _LL14;}} else{ goto _LL14;} _LL14: if(( unsigned int) _temp8 > 1u?((
struct _enum_struct*) _temp8)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL23: _temp22=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp8)->f1; if(
_temp22 == 0){ goto _LL15;} else{ goto _LL16;}} else{ goto _LL16;} _LL16: if((
unsigned int) _temp8 > 1u?(( struct _enum_struct*) _temp8)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL25: _temp24=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp8)->f1; goto _LL17;} else{ goto _LL18;} _LL18: if(( unsigned int) _temp8 >
1u?(( struct _enum_struct*) _temp8)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL27:
_temp26=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp8)->f1;
goto _LL19;} else{ goto _LL9;} _LL11: goto _LL13; _LL13: goto _LL15; _LL15: Cyc_Absyndump_dump_str((*
v).f2); return; _LL17: _temp26= _temp24; goto _LL19; _LL19: nsl= _temp26; goto
_LL9; _LL9:;} Cyc_Absyndump_dump_str(( struct _tagged_string*) nsl->hd); for(
nsl= nsl->tl; nsl != 0; nsl= nsl->tl){ if( Cyc_Absynpp_qvar_to_Cids){ Cyc_Absyndump_dump_char((
int)'_');} else{ Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp28=( char*)"::"; struct _tagged_string _temp29; _temp29.curr= _temp28;
_temp29.base= _temp28; _temp29.last_plus_one= _temp28 + 3; _temp29;}));} Cyc_Absyndump_dump_nospace(*((
struct _tagged_string*) nsl->hd));} if( Cyc_Absynpp_qvar_to_Cids){ Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp30=( char*)"_"; struct _tagged_string
_temp31; _temp31.curr= _temp30; _temp31.base= _temp30; _temp31.last_plus_one=
_temp30 + 2; _temp31;}));} else{ Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp32=( char*)"::"; struct _tagged_string _temp33;
_temp33.curr= _temp32; _temp33.base= _temp32; _temp33.last_plus_one= _temp32 + 3;
_temp33;}));} Cyc_Absyndump_dump_nospace(*(* v).f2);} void Cyc_Absyndump_dumptq(
struct Cyc_Absyn_Tqual* tq){ if( tq->q_restrict){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp34=( char*)"restrict"; struct _tagged_string
_temp35; _temp35.curr= _temp34; _temp35.base= _temp34; _temp35.last_plus_one=
_temp34 + 9; _temp35;}));} if( tq->q_volatile){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp36=( char*)"volatile"; struct _tagged_string
_temp37; _temp37.curr= _temp36; _temp37.base= _temp36; _temp37.last_plus_one=
_temp36 + 9; _temp37;}));} if( tq->q_const){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp38=( char*)"const"; struct _tagged_string _temp39;
_temp39.curr= _temp38; _temp39.base= _temp38; _temp39.last_plus_one= _temp38 + 6;
_temp39;}));}} void Cyc_Absyndump_dumpscope( void* sc){ void* _temp40= sc; _LL42:
if( _temp40 == Cyc_Absyn_Static){ goto _LL43;} else{ goto _LL44;} _LL44: if(
_temp40 == Cyc_Absyn_Public){ goto _LL45;} else{ goto _LL46;} _LL46: if( _temp40
== Cyc_Absyn_Extern){ goto _LL47;} else{ goto _LL48;} _LL48: if( _temp40 == Cyc_Absyn_ExternC){
goto _LL49;} else{ goto _LL50;} _LL50: if( _temp40 == Cyc_Absyn_Abstract){ goto
_LL51;} else{ goto _LL41;} _LL43: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp52=( char*)"static"; struct _tagged_string _temp53; _temp53.curr=
_temp52; _temp53.base= _temp52; _temp53.last_plus_one= _temp52 + 7; _temp53;}));
return; _LL45: return; _LL47: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp54=( char*)"extern"; struct _tagged_string _temp55; _temp55.curr= _temp54;
_temp55.base= _temp54; _temp55.last_plus_one= _temp54 + 7; _temp55;})); return;
_LL49: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp56=( char*)"extern \"C\"";
struct _tagged_string _temp57; _temp57.curr= _temp56; _temp57.base= _temp56;
_temp57.last_plus_one= _temp56 + 11; _temp57;})); return; _LL51: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp58=( char*)"abstract"; struct _tagged_string
_temp59; _temp59.curr= _temp58; _temp59.base= _temp58; _temp59.last_plus_one=
_temp58 + 9; _temp59;})); return; _LL41:;} void Cyc_Absyndump_dumpkind( void* k){
void* _temp60= k; _LL62: if( _temp60 == Cyc_Absyn_AnyKind){ goto _LL63;} else{
goto _LL64;} _LL64: if( _temp60 == Cyc_Absyn_MemKind){ goto _LL65;} else{ goto
_LL66;} _LL66: if( _temp60 == Cyc_Absyn_BoxKind){ goto _LL67;} else{ goto _LL68;}
_LL68: if( _temp60 == Cyc_Absyn_RgnKind){ goto _LL69;} else{ goto _LL70;} _LL70:
if( _temp60 == Cyc_Absyn_EffKind){ goto _LL71;} else{ goto _LL61;} _LL63: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp72=( char*)"A"; struct _tagged_string
_temp73; _temp73.curr= _temp72; _temp73.base= _temp72; _temp73.last_plus_one=
_temp72 + 2; _temp73;})); return; _LL65: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp74=( char*)"M"; struct _tagged_string _temp75;
_temp75.curr= _temp74; _temp75.base= _temp74; _temp75.last_plus_one= _temp74 + 2;
_temp75;})); return; _LL67: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp76=( char*)"B"; struct _tagged_string _temp77; _temp77.curr= _temp76;
_temp77.base= _temp76; _temp77.last_plus_one= _temp76 + 2; _temp77;})); return;
_LL69: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp78=( char*)"R";
struct _tagged_string _temp79; _temp79.curr= _temp78; _temp79.base= _temp78;
_temp79.last_plus_one= _temp78 + 2; _temp79;})); return; _LL71: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp80=( char*)"E"; struct _tagged_string
_temp81; _temp81.curr= _temp80; _temp81.base= _temp80; _temp81.last_plus_one=
_temp80 + 2; _temp81;})); return; _LL61:;} void Cyc_Absyndump_dumptps( struct
Cyc_List_List* ts){(( void(*)( void(* f)( void*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumptyp, ts,( struct _tagged_string)({
char* _temp82=( char*)"<"; struct _tagged_string _temp83; _temp83.curr= _temp82;
_temp83.base= _temp82; _temp83.last_plus_one= _temp82 + 2; _temp83;}),( struct
_tagged_string)({ char* _temp84=( char*)">"; struct _tagged_string _temp85;
_temp85.curr= _temp84; _temp85.base= _temp84; _temp85.last_plus_one= _temp84 + 2;
_temp85;}),( struct _tagged_string)({ char* _temp86=( char*)","; struct
_tagged_string _temp87; _temp87.curr= _temp86; _temp87.base= _temp86; _temp87.last_plus_one=
_temp86 + 2; _temp87;}));} void Cyc_Absyndump_dumptvar( struct Cyc_Absyn_Tvar*
tv){ Cyc_Absyndump_dump_str( tv->name);} void Cyc_Absyndump_dumpkindedtvar(
struct Cyc_Absyn_Tvar* tv){ Cyc_Absyndump_dump_str( tv->name);{ void* _temp88=(
void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
tv->kind))->v; void* _temp96; void* _temp98; _LL90: if(( unsigned int) _temp88 >
1u?(( struct _enum_struct*) _temp88)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL97:
_temp96=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp88)->f1; if( _temp96
== Cyc_Absyn_BoxKind){ goto _LL91;} else{ goto _LL92;}} else{ goto _LL92;} _LL92:
if(( unsigned int) _temp88 > 1u?(( struct _enum_struct*) _temp88)->tag == Cyc_Absyn_Eq_constr_tag:
0){ _LL99: _temp98=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp88)->f1;
goto _LL93;} else{ goto _LL94;} _LL94: goto _LL95; _LL91: goto _LL89; _LL93: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp100=( char*)"::"; struct _tagged_string
_temp101; _temp101.curr= _temp100; _temp101.base= _temp100; _temp101.last_plus_one=
_temp100 + 3; _temp101;})); Cyc_Absyndump_dumpkind( _temp98); goto _LL89; _LL95:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp102=( char*)"::?";
struct _tagged_string _temp103; _temp103.curr= _temp102; _temp103.base= _temp102;
_temp103.last_plus_one= _temp102 + 4; _temp103;})); goto _LL89; _LL89:;}} void
Cyc_Absyndump_dumptvars( struct Cyc_List_List* tvs){(( void(*)( void(* f)(
struct Cyc_Absyn_Tvar*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar, tvs,( struct _tagged_string)({ char* _temp104=( char*)"<";
struct _tagged_string _temp105; _temp105.curr= _temp104; _temp105.base= _temp104;
_temp105.last_plus_one= _temp104 + 2; _temp105;}),( struct _tagged_string)({
char* _temp106=( char*)">"; struct _tagged_string _temp107; _temp107.curr=
_temp106; _temp107.base= _temp106; _temp107.last_plus_one= _temp106 + 2;
_temp107;}),( struct _tagged_string)({ char* _temp108=( char*)","; struct
_tagged_string _temp109; _temp109.curr= _temp108; _temp109.base= _temp108;
_temp109.last_plus_one= _temp108 + 2; _temp109;}));} void Cyc_Absyndump_dumpkindedtvars(
struct Cyc_List_List* tvs){(( void(*)( void(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_egroup)( Cyc_Absyndump_dumpkindedtvar,
tvs,( struct _tagged_string)({ char* _temp110=( char*)"<"; struct _tagged_string
_temp111; _temp111.curr= _temp110; _temp111.base= _temp110; _temp111.last_plus_one=
_temp110 + 2; _temp111;}),( struct _tagged_string)({ char* _temp112=( char*)">";
struct _tagged_string _temp113; _temp113.curr= _temp112; _temp113.base= _temp112;
_temp113.last_plus_one= _temp112 + 2; _temp113;}),( struct _tagged_string)({
char* _temp114=( char*)","; struct _tagged_string _temp115; _temp115.curr=
_temp114; _temp115.base= _temp114; _temp115.last_plus_one= _temp114 + 2;
_temp115;}));} void Cyc_Absyndump_dumparg( struct _tuple5* pr){(( void(*)(
struct Cyc_Absyn_Tqual*, void*, void(* f)( int), int)) Cyc_Absyndump_dumptqtd)((
struct Cyc_Absyn_Tqual*)(* pr).f1,(* pr).f2,( void(*)( int x)) Cyc_Absyndump_ignore,
0);} void Cyc_Absyndump_dumpargs( struct Cyc_List_List* ts){(( void(*)( void(* f)(
struct _tuple5*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumparg,
ts,( struct _tagged_string)({ char* _temp116=( char*)"("; struct _tagged_string
_temp117; _temp117.curr= _temp116; _temp117.base= _temp116; _temp117.last_plus_one=
_temp116 + 2; _temp117;}),( struct _tagged_string)({ char* _temp118=( char*)")";
struct _tagged_string _temp119; _temp119.curr= _temp118; _temp119.base= _temp118;
_temp119.last_plus_one= _temp118 + 2; _temp119;}),( struct _tagged_string)({
char* _temp120=( char*)","; struct _tagged_string _temp121; _temp121.curr=
_temp120; _temp121.base= _temp120; _temp121.last_plus_one= _temp120 + 2;
_temp121;}));} int Cyc_Absyndump_next_is_pointer( struct Cyc_List_List* tms){
if( tms == 0){ return 0;}{ void* _temp122=( void*) tms->hd; struct Cyc_Absyn_Tqual*
_temp128; void* _temp130; void* _temp132; _LL124: if(( unsigned int) _temp122 >
1u?(( struct _enum_struct*) _temp122)->tag == Cyc_Absyn_Pointer_mod_tag: 0){
_LL133: _temp132=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp122)->f1;
goto _LL131; _LL131: _temp130=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp122)->f2; goto _LL129; _LL129: _temp128=( struct Cyc_Absyn_Tqual*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp122)->f3; goto _LL125;} else{ goto _LL126;}
_LL126: goto _LL127; _LL125: return 1; _LL127: return 0; _LL123:;}} static void
Cyc_Absyndump_dumprgn( void* t){ void* _temp134= Cyc_Tcutil_compress( t); _LL136:
if( _temp134 == Cyc_Absyn_HeapRgn){ goto _LL137;} else{ goto _LL138;} _LL138:
goto _LL139; _LL137: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp140=( char*)"`H"; struct _tagged_string _temp141; _temp141.curr= _temp140;
_temp141.base= _temp140; _temp141.last_plus_one= _temp140 + 3; _temp141;}));
goto _LL135; _LL139: Cyc_Absyndump_dumpntyp( t); goto _LL135; _LL135:;} static
struct _tuple6 Cyc_Absyndump_effects_split( void* t){ struct Cyc_List_List*
regions= 0; struct Cyc_List_List* effects= 0;{ void* _temp142= Cyc_Tcutil_compress(
t); void* _temp150; struct Cyc_List_List* _temp152; _LL144: if(( unsigned int)
_temp142 > 5u?(( struct _enum_struct*) _temp142)->tag == Cyc_Absyn_AccessEff_tag:
0){ _LL151: _temp150=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp142)->f1;
goto _LL145;} else{ goto _LL146;} _LL146: if(( unsigned int) _temp142 > 5u?((
struct _enum_struct*) _temp142)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL153:
_temp152=( struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp142)->f1;
goto _LL147;} else{ goto _LL148;} _LL148: goto _LL149; _LL145: regions=({ struct
Cyc_List_List* _temp154=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp154->hd=( void*) _temp150; _temp154->tl= regions; _temp154;}); goto _LL143;
_LL147: for( 0; _temp152 != 0; _temp152= _temp152->tl){ struct Cyc_List_List*
_temp157; struct Cyc_List_List* _temp159; struct _tuple6 _temp155= Cyc_Absyndump_effects_split((
void*) _temp152->hd); _LL160: _temp159= _temp155.f1; goto _LL158; _LL158:
_temp157= _temp155.f2; goto _LL156; _LL156: regions=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp159, regions); effects=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp157, effects);} goto _LL143;
_LL149: effects=({ struct Cyc_List_List* _temp161=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp161->hd=( void*) t; _temp161->tl=
effects; _temp161;}); goto _LL143; _LL143:;} return({ struct _tuple6 _temp162;
_temp162.f1= regions; _temp162.f2= effects; _temp162;});} static void Cyc_Absyndump_dumpeff(
void* t){ struct Cyc_List_List* _temp165; struct Cyc_List_List* _temp167; struct
_tuple6 _temp163= Cyc_Absyndump_effects_split( t); _LL168: _temp167= _temp163.f1;
goto _LL166; _LL166: _temp165= _temp163.f2; goto _LL164; _LL164: _temp167=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp167);
_temp165=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp165); for( 0; _temp165 != 0; _temp165= _temp165->tl){ Cyc_Absyndump_dumpntyp((
void*) _temp165->hd); Cyc_Absyndump_dump_char(( int)'+');} Cyc_Absyndump_dump_char((
int)'{'); for( 0; _temp167 != 0; _temp167= _temp167->tl){ Cyc_Absyndump_dumprgn((
void*) _temp167->hd); if( _temp167->tl != 0){ Cyc_Absyndump_dump_char(( int)',');}}
Cyc_Absyndump_dump_char(( int)'}');} void Cyc_Absyndump_dumpntyp( void* t){ void*
_temp169= t; struct Cyc_Absyn_Exp* _temp229; struct Cyc_Absyn_Tqual* _temp231;
void* _temp233; struct Cyc_Absyn_FnInfo _temp235; struct Cyc_Absyn_PtrInfo
_temp237; struct Cyc_Absyn_Tvar* _temp239; int _temp241; struct Cyc_Core_Opt*
_temp243; void* _temp245; int _temp247; struct Cyc_Core_Opt* _temp249; struct
Cyc_Core_Opt _temp251; void* _temp252; void* _temp254; struct Cyc_Absyn_Enumdecl**
_temp256; struct Cyc_List_List* _temp258; struct _tuple0* _temp260; struct Cyc_Absyn_Enumdecl**
_temp262; struct Cyc_List_List* _temp264; struct _tuple0* _temp266; struct Cyc_Absyn_Xenumdecl**
_temp268; struct _tuple0* _temp270; void* _temp272; void* _temp274; void*
_temp276; void* _temp278; void* _temp280; void* _temp282; void* _temp284; void*
_temp286; void* _temp288; void* _temp290; void* _temp292; void* _temp294; void*
_temp296; void* _temp298; void* _temp300; void* _temp302; struct Cyc_List_List*
_temp304; struct Cyc_Absyn_Structdecl** _temp306; struct Cyc_List_List* _temp308;
struct _tuple0* _temp310; struct Cyc_Absyn_Structdecl** _temp312; struct Cyc_List_List*
_temp314; struct _tuple0* _temp316; struct Cyc_Core_Opt* _temp318; struct Cyc_List_List*
_temp320; struct _tuple0* _temp322; void* _temp324; void* _temp326; struct Cyc_List_List*
_temp328; _LL171: if(( unsigned int) _temp169 > 5u?(( struct _enum_struct*)
_temp169)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL234: _temp233=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp169)->f1; goto _LL232; _LL232: _temp231=(
struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp169)->f2;
goto _LL230; _LL230: _temp229=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp169)->f3; goto _LL172;} else{ goto _LL173;} _LL173: if(( unsigned int)
_temp169 > 5u?(( struct _enum_struct*) _temp169)->tag == Cyc_Absyn_FnType_tag: 0){
_LL236: _temp235=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp169)->f1; goto _LL174;} else{ goto _LL175;} _LL175: if(( unsigned int)
_temp169 > 5u?(( struct _enum_struct*) _temp169)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL238: _temp237=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp169)->f1; goto _LL176;} else{ goto _LL177;} _LL177: if( _temp169 == Cyc_Absyn_VoidType){
goto _LL178;} else{ goto _LL179;} _LL179: if(( unsigned int) _temp169 > 5u?((
struct _enum_struct*) _temp169)->tag == Cyc_Absyn_VarType_tag: 0){ _LL240:
_temp239=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp169)->f1;
goto _LL180;} else{ goto _LL181;} _LL181: if(( unsigned int) _temp169 > 5u?((
struct _enum_struct*) _temp169)->tag == Cyc_Absyn_Evar_tag: 0){ _LL246: _temp245=(
void*)(( struct Cyc_Absyn_Evar_struct*) _temp169)->f1; goto _LL244; _LL244:
_temp243=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*) _temp169)->f2;
if( _temp243 == 0){ goto _LL242;} else{ goto _LL183;} _LL242: _temp241=( int)((
struct Cyc_Absyn_Evar_struct*) _temp169)->f3; goto _LL182;} else{ goto _LL183;}
_LL183: if(( unsigned int) _temp169 > 5u?(( struct _enum_struct*) _temp169)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL255: _temp254=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp169)->f1; goto _LL250; _LL250: _temp249=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Evar_struct*) _temp169)->f2; if( _temp249 == 0){ goto _LL185;} else{
_temp251=* _temp249; _LL253: _temp252=( void*) _temp251.v; goto _LL248;} _LL248:
_temp247=( int)(( struct Cyc_Absyn_Evar_struct*) _temp169)->f3; goto _LL184;}
else{ goto _LL185;} _LL185: if(( unsigned int) _temp169 > 5u?(( struct
_enum_struct*) _temp169)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL261: _temp260=(
struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*) _temp169)->f1; if(
_temp260 == 0){ goto _LL259;} else{ goto _LL187;} _LL259: _temp258=( struct Cyc_List_List*)((
struct Cyc_Absyn_EnumType_struct*) _temp169)->f2; goto _LL257; _LL257: _temp256=(
struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*) _temp169)->f3;
goto _LL186;} else{ goto _LL187;} _LL187: if(( unsigned int) _temp169 > 5u?((
struct _enum_struct*) _temp169)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL267:
_temp266=( struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*) _temp169)->f1;
goto _LL265; _LL265: _temp264=( struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*)
_temp169)->f2; goto _LL263; _LL263: _temp262=( struct Cyc_Absyn_Enumdecl**)((
struct Cyc_Absyn_EnumType_struct*) _temp169)->f3; goto _LL188;} else{ goto
_LL189;} _LL189: if(( unsigned int) _temp169 > 5u?(( struct _enum_struct*)
_temp169)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL271: _temp270=( struct _tuple0*)((
struct Cyc_Absyn_XenumType_struct*) _temp169)->f1; goto _LL269; _LL269: _temp268=(
struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*) _temp169)->f2;
goto _LL190;} else{ goto _LL191;} _LL191: if(( unsigned int) _temp169 > 5u?((
struct _enum_struct*) _temp169)->tag == Cyc_Absyn_IntType_tag: 0){ _LL275:
_temp274=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp169)->f1; if(
_temp274 == Cyc_Absyn_Signed){ goto _LL273;} else{ goto _LL193;} _LL273:
_temp272=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp169)->f2; if(
_temp272 == Cyc_Absyn_B1){ goto _LL192;} else{ goto _LL193;}} else{ goto _LL193;}
_LL193: if(( unsigned int) _temp169 > 5u?(( struct _enum_struct*) _temp169)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL279: _temp278=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp169)->f1; if( _temp278 == Cyc_Absyn_Unsigned){ goto _LL277;} else{ goto
_LL195;} _LL277: _temp276=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp169)->f2;
if( _temp276 == Cyc_Absyn_B1){ goto _LL194;} else{ goto _LL195;}} else{ goto
_LL195;} _LL195: if(( unsigned int) _temp169 > 5u?(( struct _enum_struct*)
_temp169)->tag == Cyc_Absyn_IntType_tag: 0){ _LL283: _temp282=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp169)->f1; if( _temp282 == Cyc_Absyn_Signed){
goto _LL281;} else{ goto _LL197;} _LL281: _temp280=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp169)->f2; if( _temp280 == Cyc_Absyn_B2){ goto _LL196;} else{ goto _LL197;}}
else{ goto _LL197;} _LL197: if(( unsigned int) _temp169 > 5u?(( struct
_enum_struct*) _temp169)->tag == Cyc_Absyn_IntType_tag: 0){ _LL287: _temp286=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp169)->f1; if( _temp286 == Cyc_Absyn_Unsigned){
goto _LL285;} else{ goto _LL199;} _LL285: _temp284=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp169)->f2; if( _temp284 == Cyc_Absyn_B2){ goto _LL198;} else{ goto _LL199;}}
else{ goto _LL199;} _LL199: if(( unsigned int) _temp169 > 5u?(( struct
_enum_struct*) _temp169)->tag == Cyc_Absyn_IntType_tag: 0){ _LL291: _temp290=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp169)->f1; if( _temp290 == Cyc_Absyn_Signed){
goto _LL289;} else{ goto _LL201;} _LL289: _temp288=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp169)->f2; if( _temp288 == Cyc_Absyn_B4){ goto _LL200;} else{ goto _LL201;}}
else{ goto _LL201;} _LL201: if(( unsigned int) _temp169 > 5u?(( struct
_enum_struct*) _temp169)->tag == Cyc_Absyn_IntType_tag: 0){ _LL295: _temp294=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp169)->f1; if( _temp294 == Cyc_Absyn_Unsigned){
goto _LL293;} else{ goto _LL203;} _LL293: _temp292=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp169)->f2; if( _temp292 == Cyc_Absyn_B4){ goto _LL202;} else{ goto _LL203;}}
else{ goto _LL203;} _LL203: if(( unsigned int) _temp169 > 5u?(( struct
_enum_struct*) _temp169)->tag == Cyc_Absyn_IntType_tag: 0){ _LL299: _temp298=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp169)->f1; if( _temp298 == Cyc_Absyn_Signed){
goto _LL297;} else{ goto _LL205;} _LL297: _temp296=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp169)->f2; if( _temp296 == Cyc_Absyn_B8){ goto _LL204;} else{ goto _LL205;}}
else{ goto _LL205;} _LL205: if(( unsigned int) _temp169 > 5u?(( struct
_enum_struct*) _temp169)->tag == Cyc_Absyn_IntType_tag: 0){ _LL303: _temp302=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp169)->f1; if( _temp302 == Cyc_Absyn_Unsigned){
goto _LL301;} else{ goto _LL207;} _LL301: _temp300=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp169)->f2; if( _temp300 == Cyc_Absyn_B8){ goto _LL206;} else{ goto _LL207;}}
else{ goto _LL207;} _LL207: if( _temp169 == Cyc_Absyn_FloatType){ goto _LL208;}
else{ goto _LL209;} _LL209: if( _temp169 == Cyc_Absyn_DoubleType){ goto _LL210;}
else{ goto _LL211;} _LL211: if(( unsigned int) _temp169 > 5u?(( struct
_enum_struct*) _temp169)->tag == Cyc_Absyn_TupleType_tag: 0){ _LL305: _temp304=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TupleType_struct*) _temp169)->f1; goto
_LL212;} else{ goto _LL213;} _LL213: if(( unsigned int) _temp169 > 5u?(( struct
_enum_struct*) _temp169)->tag == Cyc_Absyn_StructType_tag: 0){ _LL311: _temp310=(
struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp169)->f1; if(
_temp310 == 0){ goto _LL309;} else{ goto _LL215;} _LL309: _temp308=( struct Cyc_List_List*)((
struct Cyc_Absyn_StructType_struct*) _temp169)->f2; goto _LL307; _LL307:
_temp306=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp169)->f3; goto _LL214;} else{ goto _LL215;} _LL215: if(( unsigned int)
_temp169 > 5u?(( struct _enum_struct*) _temp169)->tag == Cyc_Absyn_StructType_tag:
0){ _LL317: _temp316=( struct _tuple0*)(( struct Cyc_Absyn_StructType_struct*)
_temp169)->f1; goto _LL315; _LL315: _temp314=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp169)->f2; goto _LL313; _LL313: _temp312=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp169)->f3;
goto _LL216;} else{ goto _LL217;} _LL217: if(( unsigned int) _temp169 > 5u?((
struct _enum_struct*) _temp169)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL323:
_temp322=( struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp169)->f1;
goto _LL321; _LL321: _temp320=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp169)->f2; goto _LL319; _LL319: _temp318=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp169)->f3; goto _LL218;} else{ goto _LL219;}
_LL219: if( _temp169 == Cyc_Absyn_UnionType){ goto _LL220;} else{ goto _LL221;}
_LL221: if(( unsigned int) _temp169 > 5u?(( struct _enum_struct*) _temp169)->tag
== Cyc_Absyn_RgnHandleType_tag: 0){ _LL325: _temp324=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp169)->f1; goto _LL222;} else{ goto _LL223;} _LL223: if( _temp169 == Cyc_Absyn_HeapRgn){
goto _LL224;} else{ goto _LL225;} _LL225: if(( unsigned int) _temp169 > 5u?((
struct _enum_struct*) _temp169)->tag == Cyc_Absyn_AccessEff_tag: 0){ _LL327:
_temp326=( void*)(( struct Cyc_Absyn_AccessEff_struct*) _temp169)->f1; goto
_LL226;} else{ goto _LL227;} _LL227: if(( unsigned int) _temp169 > 5u?(( struct
_enum_struct*) _temp169)->tag == Cyc_Absyn_JoinEff_tag: 0){ _LL329: _temp328=(
struct Cyc_List_List*)(( struct Cyc_Absyn_JoinEff_struct*) _temp169)->f1; goto
_LL228;} else{ goto _LL170;} _LL172: return; _LL174: return; _LL176: return;
_LL178: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp330=( char*)"void";
struct _tagged_string _temp331; _temp331.curr= _temp330; _temp331.base= _temp330;
_temp331.last_plus_one= _temp330 + 5; _temp331;})); return; _LL180: Cyc_Absyndump_dump_str(
_temp239->name); return; _LL182: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp332=( char*)"%"; struct _tagged_string _temp333; _temp333.curr=
_temp332; _temp333.base= _temp332; _temp333.last_plus_one= _temp332 + 2;
_temp333;})); Cyc_Absyndump_dumpkind( _temp245); Cyc_Absyndump_dump( xprintf("(%d)",
_temp241)); return; _LL184: Cyc_Absyndump_dumpntyp( _temp252); return; _LL186:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp334=( char*)"enum";
struct _tagged_string _temp335; _temp335.curr= _temp334; _temp335.base= _temp334;
_temp335.last_plus_one= _temp334 + 5; _temp335;})); Cyc_Absyndump_dumptps(
_temp258); return; _LL188: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp336=( char*)"enum"; struct _tagged_string _temp337; _temp337.curr= _temp336;
_temp337.base= _temp336; _temp337.last_plus_one= _temp336 + 5; _temp337;})); Cyc_Absyndump_dumpqvar((
struct _tuple0*)({ struct _tuple0* _temp338= _temp266; if( _temp338 == 0){
_throw( Null_Exception);} _temp338;})); Cyc_Absyndump_dumptps( _temp264);
return; _LL190: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp339=(
char*)"xenum"; struct _tagged_string _temp340; _temp340.curr= _temp339; _temp340.base=
_temp339; _temp340.last_plus_one= _temp339 + 6; _temp340;})); Cyc_Absyndump_dumpqvar(
_temp270); return; _LL192: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp341=( char*)"signed char"; struct _tagged_string _temp342; _temp342.curr=
_temp341; _temp342.base= _temp341; _temp342.last_plus_one= _temp341 + 12;
_temp342;})); return; _LL194: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp343=( char*)"char"; struct _tagged_string _temp344; _temp344.curr= _temp343;
_temp344.base= _temp343; _temp344.last_plus_one= _temp343 + 5; _temp344;}));
return; _LL196: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp345=(
char*)"short"; struct _tagged_string _temp346; _temp346.curr= _temp345; _temp346.base=
_temp345; _temp346.last_plus_one= _temp345 + 6; _temp346;})); return; _LL198:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp347=( char*)"unsigned short";
struct _tagged_string _temp348; _temp348.curr= _temp347; _temp348.base= _temp347;
_temp348.last_plus_one= _temp347 + 15; _temp348;})); return; _LL200: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp349=( char*)"int"; struct _tagged_string
_temp350; _temp350.curr= _temp349; _temp350.base= _temp349; _temp350.last_plus_one=
_temp349 + 4; _temp350;})); return; _LL202: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp351=( char*)"unsigned int"; struct _tagged_string
_temp352; _temp352.curr= _temp351; _temp352.base= _temp351; _temp352.last_plus_one=
_temp351 + 13; _temp352;})); return; _LL204: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp353=( char*)"long long"; struct _tagged_string
_temp354; _temp354.curr= _temp353; _temp354.base= _temp353; _temp354.last_plus_one=
_temp353 + 10; _temp354;})); return; _LL206: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp355=( char*)"unsigned long long"; struct
_tagged_string _temp356; _temp356.curr= _temp355; _temp356.base= _temp355;
_temp356.last_plus_one= _temp355 + 19; _temp356;})); return; _LL208: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp357=( char*)"float"; struct _tagged_string
_temp358; _temp358.curr= _temp357; _temp358.base= _temp357; _temp358.last_plus_one=
_temp357 + 6; _temp358;})); return; _LL210: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp359=( char*)"double"; struct _tagged_string
_temp360; _temp360.curr= _temp359; _temp360.base= _temp359; _temp360.last_plus_one=
_temp359 + 7; _temp360;})); return; _LL212: Cyc_Absyndump_dump_char(( int)'$');
Cyc_Absyndump_dumpargs( _temp304); return; _LL214: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp361=( char*)"struct"; struct _tagged_string
_temp362; _temp362.curr= _temp361; _temp362.base= _temp361; _temp362.last_plus_one=
_temp361 + 7; _temp362;})); Cyc_Absyndump_dumptps( _temp308); return; _LL216:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp363=( char*)"struct";
struct _tagged_string _temp364; _temp364.curr= _temp363; _temp364.base= _temp363;
_temp364.last_plus_one= _temp363 + 7; _temp364;})); Cyc_Absyndump_dumpqvar((
struct _tuple0*)({ struct _tuple0* _temp365= _temp316; if( _temp365 == 0){
_throw( Null_Exception);} _temp365;})); Cyc_Absyndump_dumptps( _temp314);
return; _LL218: Cyc_Absyndump_dumpqvar( _temp322), Cyc_Absyndump_dumptps(
_temp320); return; _LL220: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp366=( char*)"union {/* unimplemented */}"; struct _tagged_string _temp367;
_temp367.curr= _temp366; _temp367.base= _temp366; _temp367.last_plus_one=
_temp366 + 28; _temp367;})); return; _LL222: Cyc_Absyndump_dumprgn( _temp324);
return; _LL224: return; _LL226: return; _LL228: return; _LL170:;} void Cyc_Absyndump_dumpvaropt(
struct Cyc_Core_Opt* vo){ if( vo != 0){ Cyc_Absyndump_dump_str(( struct
_tagged_string*) vo->v);}} void Cyc_Absyndump_dumpfunarg( struct _tuple1* t){((
void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( struct Cyc_Core_Opt*),
struct Cyc_Core_Opt*)) Cyc_Absyndump_dumptqtd)(( struct Cyc_Absyn_Tqual*)(* t).f2,(*
t).f3, Cyc_Absyndump_dumpvaropt,(* t).f1);} void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List* args, int varargs, struct Cyc_Core_Opt* effopt){ Cyc_Absyndump_dump_char((
int)'('); for( 0; args != 0; args= args->tl){ Cyc_Absyndump_dumpfunarg(( struct
_tuple1*) args->hd); if( args->tl != 0? 1: varargs){ Cyc_Absyndump_dump_char((
int)',');}} if( varargs){ Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp368=( char*)"..."; struct _tagged_string _temp369; _temp369.curr= _temp368;
_temp369.base= _temp368; _temp369.last_plus_one= _temp368 + 4; _temp369;}));}
if( effopt != 0){ Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpeff(( void*)
effopt->v);} Cyc_Absyndump_dump_char(( int)')');} void Cyc_Absyndump_dumptyp(
void* t){(( void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( int), int)) Cyc_Absyndump_dumptqtd)(
0, t,( void(*)( int x)) Cyc_Absyndump_ignore, 0);} void Cyc_Absyndump_dumpdesignator(
void* d){ void* _temp370= d; struct Cyc_Absyn_Exp* _temp376; struct
_tagged_string* _temp378; _LL372: if((( struct _enum_struct*) _temp370)->tag ==
Cyc_Absyn_ArrayElement_tag){ _LL377: _temp376=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayElement_struct*) _temp370)->f1; goto _LL373;} else{ goto _LL374;}
_LL374: if((( struct _enum_struct*) _temp370)->tag == Cyc_Absyn_FieldName_tag){
_LL379: _temp378=( struct _tagged_string*)(( struct Cyc_Absyn_FieldName_struct*)
_temp370)->f1; goto _LL375;} else{ goto _LL371;} _LL373: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp380=( char*)".["; struct _tagged_string
_temp381; _temp381.curr= _temp380; _temp381.base= _temp380; _temp381.last_plus_one=
_temp380 + 3; _temp381;})); Cyc_Absyndump_dumpexp( _temp376); Cyc_Absyndump_dump_char((
int)']'); goto _LL371; _LL375: Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp378); goto _LL371; _LL371:;} void Cyc_Absyndump_dumpde( struct _tuple7* de){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* de).f1,( struct _tagged_string)({ char* _temp382=(
char*)""; struct _tagged_string _temp383; _temp383.curr= _temp382; _temp383.base=
_temp382; _temp383.last_plus_one= _temp382 + 1; _temp383;}),( struct
_tagged_string)({ char* _temp384=( char*)"="; struct _tagged_string _temp385;
_temp385.curr= _temp384; _temp385.base= _temp384; _temp385.last_plus_one=
_temp384 + 2; _temp385;}),( struct _tagged_string)({ char* _temp386=( char*)"=";
struct _tagged_string _temp387; _temp387.curr= _temp386; _temp387.base= _temp386;
_temp387.last_plus_one= _temp386 + 2; _temp387;})); Cyc_Absyndump_dumpexp((* de).f2);}
void Cyc_Absyndump_dumpexps_prec( int inprec, struct Cyc_List_List* es){(( void(*)(
void(* f)( int, struct Cyc_Absyn_Exp*), int env, struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec, inprec, es,( struct _tagged_string)({ char* _temp388=(
char*)""; struct _tagged_string _temp389; _temp389.curr= _temp388; _temp389.base=
_temp388; _temp389.last_plus_one= _temp388 + 1; _temp389;}),( struct
_tagged_string)({ char* _temp390=( char*)""; struct _tagged_string _temp391;
_temp391.curr= _temp390; _temp391.base= _temp390; _temp391.last_plus_one=
_temp390 + 1; _temp391;}),( struct _tagged_string)({ char* _temp392=( char*)",";
struct _tagged_string _temp393; _temp393.curr= _temp392; _temp393.base= _temp392;
_temp393.last_plus_one= _temp392 + 2; _temp393;}));} void Cyc_Absyndump_dumpexp_prec(
int inprec, struct Cyc_Absyn_Exp* e){ int myprec= Cyc_Absynpp_exp_prec( e); if(
inprec >= myprec){ Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp394=( char*)"("; struct _tagged_string _temp395; _temp395.curr= _temp394;
_temp395.base= _temp394; _temp395.last_plus_one= _temp394 + 2; _temp395;}));}{
void* _temp396=( void*) e->r; void* _temp480; char _temp482; void* _temp484;
void* _temp486; short _temp488; void* _temp490; void* _temp492; int _temp494;
void* _temp496; void* _temp498; int _temp500; void* _temp502; void* _temp504;
long long _temp506; void* _temp508; void* _temp510; struct _tagged_string
_temp512; void* _temp514; void* _temp516; struct _tagged_string _temp518; int
_temp520; struct _tuple0* _temp522; void* _temp524; struct _tuple0* _temp526;
struct Cyc_List_List* _temp528; void* _temp530; struct Cyc_Absyn_Exp* _temp532;
struct Cyc_Core_Opt* _temp534; struct Cyc_Absyn_Exp* _temp536; void* _temp538;
struct Cyc_Absyn_Exp* _temp540; void* _temp542; struct Cyc_Absyn_Exp* _temp544;
void* _temp546; struct Cyc_Absyn_Exp* _temp548; void* _temp550; struct Cyc_Absyn_Exp*
_temp552; struct Cyc_Absyn_Exp* _temp554; struct Cyc_Absyn_Exp* _temp556; struct
Cyc_Absyn_Exp* _temp558; struct Cyc_Absyn_Exp* _temp560; struct Cyc_Absyn_Exp*
_temp562; struct Cyc_List_List* _temp564; struct Cyc_Absyn_Exp* _temp566; struct
Cyc_List_List* _temp568; struct Cyc_Absyn_Exp* _temp570; struct Cyc_Absyn_Exp*
_temp572; struct Cyc_Absyn_Exp* _temp574; struct Cyc_List_List* _temp576; struct
Cyc_Absyn_Exp* _temp578; struct Cyc_Absyn_Exp* _temp580; void* _temp582; struct
Cyc_Absyn_Exp* _temp584; void* _temp586; struct Cyc_Absyn_Exp* _temp588; struct
_tagged_string* _temp590; struct Cyc_Absyn_Exp* _temp592; struct _tagged_string*
_temp594; struct Cyc_Absyn_Exp* _temp596; struct Cyc_Absyn_Exp* _temp598; struct
Cyc_Absyn_Exp* _temp600; struct Cyc_List_List* _temp602; struct Cyc_List_List*
_temp604; struct _tuple1* _temp606; struct Cyc_List_List* _temp608; int _temp610;
struct Cyc_Absyn_Exp* _temp612; struct Cyc_Absyn_Exp* _temp614; struct Cyc_Absyn_Vardecl*
_temp616; struct Cyc_Absyn_Structdecl* _temp618; struct Cyc_List_List* _temp620;
struct Cyc_Core_Opt* _temp622; struct _tuple0* _temp624; struct Cyc_Absyn_Enumfield*
_temp626; struct Cyc_Absyn_Enumdecl* _temp628; struct Cyc_List_List* _temp630;
struct Cyc_Core_Opt* _temp632; struct Cyc_Core_Opt* _temp634; struct Cyc_Absyn_Enumfield*
_temp636; struct Cyc_Absyn_Xenumdecl* _temp638; struct Cyc_List_List* _temp640;
struct Cyc_Core_Opt* _temp642; struct Cyc_List_List* _temp644; struct Cyc_Core_Opt*
_temp646; struct Cyc_Absyn_Stmt* _temp648; struct Cyc_Absyn_Fndecl* _temp650;
struct Cyc_Absyn_Exp* _temp652; _LL398: if((( struct _enum_struct*) _temp396)->tag
== Cyc_Absyn_Const_e_tag){ _LL481: _temp480=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp396)->f1; if(( unsigned int) _temp480 > 1u?(( struct _enum_struct*)
_temp480)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL485: _temp484=( void*)(( struct
Cyc_Absyn_Char_c_struct*) _temp480)->f1; goto _LL483; _LL483: _temp482=( char)((
struct Cyc_Absyn_Char_c_struct*) _temp480)->f2; goto _LL399;} else{ goto _LL400;}}
else{ goto _LL400;} _LL400: if((( struct _enum_struct*) _temp396)->tag == Cyc_Absyn_Const_e_tag){
_LL487: _temp486=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp396)->f1; if((
unsigned int) _temp486 > 1u?(( struct _enum_struct*) _temp486)->tag == Cyc_Absyn_Short_c_tag:
0){ _LL491: _temp490=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp486)->f1;
goto _LL489; _LL489: _temp488=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp486)->f2; goto _LL401;} else{ goto _LL402;}} else{ goto _LL402;} _LL402:
if((( struct _enum_struct*) _temp396)->tag == Cyc_Absyn_Const_e_tag){ _LL493:
_temp492=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp396)->f1; if((
unsigned int) _temp492 > 1u?(( struct _enum_struct*) _temp492)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL497: _temp496=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp492)->f1;
if( _temp496 == Cyc_Absyn_Signed){ goto _LL495;} else{ goto _LL404;} _LL495:
_temp494=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp492)->f2; goto _LL403;}
else{ goto _LL404;}} else{ goto _LL404;} _LL404: if((( struct _enum_struct*)
_temp396)->tag == Cyc_Absyn_Const_e_tag){ _LL499: _temp498=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp396)->f1; if(( unsigned int) _temp498 > 1u?(( struct _enum_struct*)
_temp498)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL503: _temp502=( void*)(( struct
Cyc_Absyn_Int_c_struct*) _temp498)->f1; if( _temp502 == Cyc_Absyn_Unsigned){
goto _LL501;} else{ goto _LL406;} _LL501: _temp500=( int)(( struct Cyc_Absyn_Int_c_struct*)
_temp498)->f2; goto _LL405;} else{ goto _LL406;}} else{ goto _LL406;} _LL406:
if((( struct _enum_struct*) _temp396)->tag == Cyc_Absyn_Const_e_tag){ _LL505:
_temp504=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp396)->f1; if((
unsigned int) _temp504 > 1u?(( struct _enum_struct*) _temp504)->tag == Cyc_Absyn_LongLong_c_tag:
0){ _LL509: _temp508=( void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp504)->f1;
goto _LL507; _LL507: _temp506=( long long)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp504)->f2; goto _LL407;} else{ goto _LL408;}} else{ goto _LL408;} _LL408:
if((( struct _enum_struct*) _temp396)->tag == Cyc_Absyn_Const_e_tag){ _LL511:
_temp510=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp396)->f1; if((
unsigned int) _temp510 > 1u?(( struct _enum_struct*) _temp510)->tag == Cyc_Absyn_Float_c_tag:
0){ _LL513: _temp512=( struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*)
_temp510)->f1; goto _LL409;} else{ goto _LL410;}} else{ goto _LL410;} _LL410:
if((( struct _enum_struct*) _temp396)->tag == Cyc_Absyn_Const_e_tag){ _LL515:
_temp514=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp396)->f1; if(
_temp514 == Cyc_Absyn_Null_c){ goto _LL411;} else{ goto _LL412;}} else{ goto
_LL412;} _LL412: if((( struct _enum_struct*) _temp396)->tag == Cyc_Absyn_Const_e_tag){
_LL517: _temp516=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp396)->f1; if((
unsigned int) _temp516 > 1u?(( struct _enum_struct*) _temp516)->tag == Cyc_Absyn_String_c_tag:
0){ _LL521: _temp520=( int)(( struct Cyc_Absyn_String_c_struct*) _temp516)->f1;
goto _LL519; _LL519: _temp518=( struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*)
_temp516)->f2; goto _LL413;} else{ goto _LL414;}} else{ goto _LL414;} _LL414:
if((( struct _enum_struct*) _temp396)->tag == Cyc_Absyn_UnknownId_e_tag){ _LL523:
_temp522=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_e_struct*) _temp396)->f1;
goto _LL415;} else{ goto _LL416;} _LL416: if((( struct _enum_struct*) _temp396)->tag
== Cyc_Absyn_Var_e_tag){ _LL527: _temp526=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp396)->f1; goto _LL525; _LL525: _temp524=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp396)->f2; goto _LL417;} else{ goto _LL418;} _LL418: if((( struct
_enum_struct*) _temp396)->tag == Cyc_Absyn_Primop_e_tag){ _LL531: _temp530=(
void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp396)->f1; goto _LL529; _LL529:
_temp528=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*) _temp396)->f2;
goto _LL419;} else{ goto _LL420;} _LL420: if((( struct _enum_struct*) _temp396)->tag
== Cyc_Absyn_AssignOp_e_tag){ _LL537: _temp536=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_AssignOp_e_struct*) _temp396)->f1; goto _LL535; _LL535: _temp534=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_AssignOp_e_struct*) _temp396)->f2; goto
_LL533; _LL533: _temp532=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp396)->f3; goto _LL421;} else{ goto _LL422;} _LL422: if((( struct
_enum_struct*) _temp396)->tag == Cyc_Absyn_Increment_e_tag){ _LL541: _temp540=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*) _temp396)->f1;
goto _LL539; _LL539: _temp538=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp396)->f2; if( _temp538 == Cyc_Absyn_PreInc){ goto _LL423;} else{ goto
_LL424;}} else{ goto _LL424;} _LL424: if((( struct _enum_struct*) _temp396)->tag
== Cyc_Absyn_Increment_e_tag){ _LL545: _temp544=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp396)->f1; goto _LL543; _LL543:
_temp542=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp396)->f2; if(
_temp542 == Cyc_Absyn_PreDec){ goto _LL425;} else{ goto _LL426;}} else{ goto
_LL426;} _LL426: if((( struct _enum_struct*) _temp396)->tag == Cyc_Absyn_Increment_e_tag){
_LL549: _temp548=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp396)->f1; goto _LL547; _LL547: _temp546=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp396)->f2; if( _temp546 == Cyc_Absyn_PostInc){ goto _LL427;} else{ goto
_LL428;}} else{ goto _LL428;} _LL428: if((( struct _enum_struct*) _temp396)->tag
== Cyc_Absyn_Increment_e_tag){ _LL553: _temp552=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Increment_e_struct*) _temp396)->f1; goto _LL551; _LL551:
_temp550=( void*)(( struct Cyc_Absyn_Increment_e_struct*) _temp396)->f2; if(
_temp550 == Cyc_Absyn_PostDec){ goto _LL429;} else{ goto _LL430;}} else{ goto
_LL430;} _LL430: if((( struct _enum_struct*) _temp396)->tag == Cyc_Absyn_Conditional_e_tag){
_LL559: _temp558=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp396)->f1; goto _LL557; _LL557: _temp556=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Conditional_e_struct*) _temp396)->f2; goto _LL555; _LL555: _temp554=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*) _temp396)->f3;
goto _LL431;} else{ goto _LL432;} _LL432: if((( struct _enum_struct*) _temp396)->tag
== Cyc_Absyn_SeqExp_e_tag){ _LL563: _temp562=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_SeqExp_e_struct*) _temp396)->f1; goto _LL561; _LL561: _temp560=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_SeqExp_e_struct*) _temp396)->f2; goto
_LL433;} else{ goto _LL434;} _LL434: if((( struct _enum_struct*) _temp396)->tag
== Cyc_Absyn_UnknownCall_e_tag){ _LL567: _temp566=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_UnknownCall_e_struct*) _temp396)->f1; goto _LL565; _LL565:
_temp564=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp396)->f2; goto _LL435;} else{ goto _LL436;} _LL436: if((( struct
_enum_struct*) _temp396)->tag == Cyc_Absyn_FnCall_e_tag){ _LL571: _temp570=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_FnCall_e_struct*) _temp396)->f1; goto
_LL569; _LL569: _temp568=( struct Cyc_List_List*)(( struct Cyc_Absyn_FnCall_e_struct*)
_temp396)->f2; goto _LL437;} else{ goto _LL438;} _LL438: if((( struct
_enum_struct*) _temp396)->tag == Cyc_Absyn_Throw_e_tag){ _LL573: _temp572=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Throw_e_struct*) _temp396)->f1; goto
_LL439;} else{ goto _LL440;} _LL440: if((( struct _enum_struct*) _temp396)->tag
== Cyc_Absyn_NoInstantiate_e_tag){ _LL575: _temp574=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_NoInstantiate_e_struct*) _temp396)->f1; goto _LL441;} else{
goto _LL442;} _LL442: if((( struct _enum_struct*) _temp396)->tag == Cyc_Absyn_Instantiate_e_tag){
_LL579: _temp578=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp396)->f1; goto _LL577; _LL577: _temp576=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Instantiate_e_struct*) _temp396)->f2; goto _LL443;} else{ goto _LL444;}
_LL444: if((( struct _enum_struct*) _temp396)->tag == Cyc_Absyn_Cast_e_tag){
_LL583: _temp582=( void*)(( struct Cyc_Absyn_Cast_e_struct*) _temp396)->f1; goto
_LL581; _LL581: _temp580=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp396)->f2; goto _LL445;} else{ goto _LL446;} _LL446: if((( struct
_enum_struct*) _temp396)->tag == Cyc_Absyn_Address_e_tag){ _LL585: _temp584=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Address_e_struct*) _temp396)->f1; goto
_LL447;} else{ goto _LL448;} _LL448: if((( struct _enum_struct*) _temp396)->tag
== Cyc_Absyn_Sizeof_e_tag){ _LL587: _temp586=( void*)(( struct Cyc_Absyn_Sizeof_e_struct*)
_temp396)->f1; goto _LL449;} else{ goto _LL450;} _LL450: if((( struct
_enum_struct*) _temp396)->tag == Cyc_Absyn_Deref_e_tag){ _LL589: _temp588=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*) _temp396)->f1; goto
_LL451;} else{ goto _LL452;} _LL452: if((( struct _enum_struct*) _temp396)->tag
== Cyc_Absyn_StructMember_e_tag){ _LL593: _temp592=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_StructMember_e_struct*) _temp396)->f1; goto _LL591; _LL591:
_temp590=( struct _tagged_string*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp396)->f2; goto _LL453;} else{ goto _LL454;} _LL454: if((( struct
_enum_struct*) _temp396)->tag == Cyc_Absyn_StructArrow_e_tag){ _LL597: _temp596=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*) _temp396)->f1;
goto _LL595; _LL595: _temp594=( struct _tagged_string*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp396)->f2; goto _LL455;} else{ goto _LL456;} _LL456: if((( struct
_enum_struct*) _temp396)->tag == Cyc_Absyn_Subscript_e_tag){ _LL601: _temp600=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp396)->f1;
goto _LL599; _LL599: _temp598=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp396)->f2; goto _LL457;} else{ goto _LL458;} _LL458: if((( struct
_enum_struct*) _temp396)->tag == Cyc_Absyn_Tuple_e_tag){ _LL603: _temp602=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_e_struct*) _temp396)->f1; goto
_LL459;} else{ goto _LL460;} _LL460: if((( struct _enum_struct*) _temp396)->tag
== Cyc_Absyn_CompoundLit_e_tag){ _LL607: _temp606=( struct _tuple1*)(( struct
Cyc_Absyn_CompoundLit_e_struct*) _temp396)->f1; goto _LL605; _LL605: _temp604=(
struct Cyc_List_List*)(( struct Cyc_Absyn_CompoundLit_e_struct*) _temp396)->f2;
goto _LL461;} else{ goto _LL462;} _LL462: if((( struct _enum_struct*) _temp396)->tag
== Cyc_Absyn_Array_e_tag){ _LL611: _temp610=( int)(( struct Cyc_Absyn_Array_e_struct*)
_temp396)->f1; goto _LL609; _LL609: _temp608=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Array_e_struct*) _temp396)->f2; goto _LL463;} else{ goto _LL464;}
_LL464: if((( struct _enum_struct*) _temp396)->tag == Cyc_Absyn_Comprehension_e_tag){
_LL617: _temp616=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp396)->f1; goto _LL615; _LL615: _temp614=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Comprehension_e_struct*) _temp396)->f2; goto _LL613; _LL613: _temp612=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Comprehension_e_struct*) _temp396)->f3;
goto _LL465;} else{ goto _LL466;} _LL466: if((( struct _enum_struct*) _temp396)->tag
== Cyc_Absyn_Struct_e_tag){ _LL625: _temp624=( struct _tuple0*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp396)->f1; goto _LL623; _LL623: _temp622=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Struct_e_struct*) _temp396)->f2; goto _LL621; _LL621: _temp620=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_e_struct*) _temp396)->f3; goto
_LL619; _LL619: _temp618=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_e_struct*)
_temp396)->f4; goto _LL467;} else{ goto _LL468;} _LL468: if((( struct
_enum_struct*) _temp396)->tag == Cyc_Absyn_Enum_e_tag){ _LL635: _temp634=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*) _temp396)->f1; goto
_LL633; _LL633: _temp632=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp396)->f2; goto _LL631; _LL631: _temp630=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Enum_e_struct*) _temp396)->f3; goto _LL629; _LL629: _temp628=( struct
Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_e_struct*) _temp396)->f4; goto
_LL627; _LL627: _temp626=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_e_struct*)
_temp396)->f5; goto _LL469;} else{ goto _LL470;} _LL470: if((( struct
_enum_struct*) _temp396)->tag == Cyc_Absyn_Xenum_e_tag){ _LL643: _temp642=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp396)->f1; goto
_LL641; _LL641: _temp640=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_e_struct*)
_temp396)->f2; goto _LL639; _LL639: _temp638=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_e_struct*) _temp396)->f3; goto _LL637; _LL637: _temp636=(
struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Xenum_e_struct*) _temp396)->f4;
goto _LL471;} else{ goto _LL472;} _LL472: if((( struct _enum_struct*) _temp396)->tag
== Cyc_Absyn_UnresolvedMem_e_tag){ _LL647: _temp646=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp396)->f1; goto _LL645; _LL645:
_temp644=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp396)->f2; goto _LL473;} else{ goto _LL474;} _LL474: if((( struct
_enum_struct*) _temp396)->tag == Cyc_Absyn_StmtExp_e_tag){ _LL649: _temp648=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_StmtExp_e_struct*) _temp396)->f1;
goto _LL475;} else{ goto _LL476;} _LL476: if((( struct _enum_struct*) _temp396)->tag
== Cyc_Absyn_Codegen_e_tag){ _LL651: _temp650=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Codegen_e_struct*) _temp396)->f1; goto _LL477;} else{ goto
_LL478;} _LL478: if((( struct _enum_struct*) _temp396)->tag == Cyc_Absyn_Fill_e_tag){
_LL653: _temp652=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Fill_e_struct*)
_temp396)->f1; goto _LL479;} else{ goto _LL397;} _LL399: Cyc_Absyndump_dump_char((
int)'\''); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp482)); Cyc_Absyndump_dump_char((
int)'\''); goto _LL397; _LL401: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp654=( char*)"(short)"; struct _tagged_string _temp655; _temp655.curr=
_temp654; _temp655.base= _temp654; _temp655.last_plus_one= _temp654 + 8;
_temp655;})); Cyc_Absyndump_dump_nospace( Cyc_Core_string_of_int(( int) _temp488));
goto _LL397; _LL403: Cyc_Absyndump_dump( Cyc_Core_string_of_int( _temp494));
goto _LL397; _LL405: Cyc_Absyndump_dump( Cyc_Core_string_of_int( _temp500)); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp656=( char*)"u"; struct _tagged_string
_temp657; _temp657.curr= _temp656; _temp657.base= _temp656; _temp657.last_plus_one=
_temp656 + 2; _temp657;})); goto _LL397; _LL407: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp658=( char*)"<<FIX LONG LONG CONSTANT>>"; struct
_tagged_string _temp659; _temp659.curr= _temp658; _temp659.base= _temp658;
_temp659.last_plus_one= _temp658 + 27; _temp659;})); goto _LL397; _LL409: Cyc_Absyndump_dump(
_temp512); goto _LL397; _LL411: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp660=( char*)"null"; struct _tagged_string _temp661; _temp661.curr=
_temp660; _temp661.base= _temp660; _temp661.last_plus_one= _temp660 + 5;
_temp661;})); goto _LL397; _LL413: if( _temp520){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp662=( char*)"new "; struct _tagged_string _temp663;
_temp663.curr= _temp662; _temp663.base= _temp662; _temp663.last_plus_one=
_temp662 + 5; _temp663;}));} Cyc_Absyndump_dump_char(( int)'"'); Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_string_escape( _temp518)); Cyc_Absyndump_dump_char(( int)'"'); goto
_LL397; _LL415: _temp526= _temp522; goto _LL417; _LL417: Cyc_Absyndump_dumpqvar(
_temp526); goto _LL397; _LL419: { struct _tagged_string pstr= Cyc_Absynpp_prim2str(
_temp530); if( ! Cyc_Absyn_is_format_prim( _temp530)){ switch((( int(*)( struct
Cyc_List_List* x)) Cyc_List_length)( _temp528)){ case 1: _LL664: if( _temp530 ==
Cyc_Absyn_Size){ Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)
_temp528->hd); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp666=(
char*)".size"; struct _tagged_string _temp667; _temp667.curr= _temp666; _temp667.base=
_temp666; _temp667.last_plus_one= _temp666 + 6; _temp667;}));} else{ Cyc_Absyndump_dump(
pstr); Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*) _temp528->hd);}
break; case 2: _LL665: Cyc_Absyndump_dumpexp_prec( myprec,( struct Cyc_Absyn_Exp*)
_temp528->hd); Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dumpexp_prec( myprec,(
struct Cyc_Absyn_Exp*)( _temp528->tl)->hd); break; default: _LL668:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp670=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp670=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp671=( char*)"Absyndump -- Bad number of arguments to primop";
struct _tagged_string _temp672; _temp672.curr= _temp671; _temp672.base= _temp671;
_temp672.last_plus_one= _temp671 + 47; _temp672;})};( struct _xenum_struct*)
_temp670;}));}} else{ Cyc_Absyndump_dump( pstr); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp673=( char*)"("; struct _tagged_string
_temp674; _temp674.curr= _temp673; _temp674.base= _temp673; _temp674.last_plus_one=
_temp673 + 2; _temp674;})); Cyc_Absyndump_dumpexps_prec( 20, _temp528); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp675=( char*)")"; struct _tagged_string
_temp676; _temp676.curr= _temp675; _temp676.base= _temp675; _temp676.last_plus_one=
_temp675 + 2; _temp676;}));} goto _LL397;} _LL421: Cyc_Absyndump_dumpexp_prec(
myprec, _temp536); if( _temp534 != 0){ Cyc_Absyndump_dump( Cyc_Absynpp_prim2str((
void*) _temp534->v));} Cyc_Absyndump_dump_nospace(( struct _tagged_string)({
char* _temp677=( char*)"="; struct _tagged_string _temp678; _temp678.curr=
_temp677; _temp678.base= _temp677; _temp678.last_plus_one= _temp677 + 2;
_temp678;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp532); goto _LL397; _LL423:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp679=( char*)"++";
struct _tagged_string _temp680; _temp680.curr= _temp679; _temp680.base= _temp679;
_temp680.last_plus_one= _temp679 + 3; _temp680;})); Cyc_Absyndump_dumpexp_prec(
myprec, _temp540); goto _LL397; _LL425: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp681=( char*)"--"; struct _tagged_string _temp682;
_temp682.curr= _temp681; _temp682.base= _temp681; _temp682.last_plus_one=
_temp681 + 3; _temp682;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp544); goto
_LL397; _LL427: Cyc_Absyndump_dumpexp_prec( myprec, _temp548); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp683=( char*)"++"; struct _tagged_string
_temp684; _temp684.curr= _temp683; _temp684.base= _temp683; _temp684.last_plus_one=
_temp683 + 3; _temp684;})); goto _LL397; _LL429: Cyc_Absyndump_dumpexp_prec(
myprec, _temp552); Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp685=(
char*)"--"; struct _tagged_string _temp686; _temp686.curr= _temp685; _temp686.base=
_temp685; _temp686.last_plus_one= _temp685 + 3; _temp686;})); goto _LL397;
_LL431: Cyc_Absyndump_dumpexp_prec( myprec, _temp558); Cyc_Absyndump_dump_char((
int)'?'); Cyc_Absyndump_dumpexp_prec( 0, _temp556); Cyc_Absyndump_dump_char((
int)':'); Cyc_Absyndump_dumpexp_prec( myprec, _temp554); goto _LL397; _LL433:
Cyc_Absyndump_dumpexp_prec( myprec, _temp562); Cyc_Absyndump_dump_char(( int)',');
Cyc_Absyndump_dumpexp_prec( myprec, _temp560); goto _LL397; _LL435: _temp570=
_temp566; _temp568= _temp564; goto _LL437; _LL437: Cyc_Absyndump_dumpexp_prec(
myprec, _temp570); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp687=( char*)"("; struct _tagged_string _temp688; _temp688.curr= _temp687;
_temp688.base= _temp687; _temp688.last_plus_one= _temp687 + 2; _temp688;})); Cyc_Absyndump_dumpexps_prec(
20, _temp568); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char*
_temp689=( char*)")"; struct _tagged_string _temp690; _temp690.curr= _temp689;
_temp690.base= _temp689; _temp690.last_plus_one= _temp689 + 2; _temp690;}));
goto _LL397; _LL439: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp691=( char*)"throw"; struct _tagged_string _temp692; _temp692.curr=
_temp691; _temp692.base= _temp691; _temp692.last_plus_one= _temp691 + 6;
_temp692;})); Cyc_Absyndump_dumpexp_prec( myprec, _temp572); goto _LL397; _LL441:
_temp578= _temp574; goto _LL443; _LL443: Cyc_Absyndump_dumpexp_prec( inprec,
_temp578); goto _LL397; _LL445: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp(
_temp582); Cyc_Absyndump_dump_char(( int)')'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp580); goto _LL397; _LL447: Cyc_Absyndump_dump_char(( int)'&'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp584); goto _LL397; _LL449: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp693=( char*)"sizeof("; struct _tagged_string
_temp694; _temp694.curr= _temp693; _temp694.base= _temp693; _temp694.last_plus_one=
_temp693 + 8; _temp694;})); Cyc_Absyndump_dumptyp( _temp586); Cyc_Absyndump_dump_char((
int)')'); goto _LL397; _LL451: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dumpexp_prec(
myprec, _temp588); goto _LL397; _LL453: Cyc_Absyndump_dumpexp_prec( myprec,
_temp592); Cyc_Absyndump_dump_char(( int)'.'); Cyc_Absyndump_dump_nospace(*
_temp590); goto _LL397; _LL455: Cyc_Absyndump_dumpexp_prec( myprec, _temp596);
Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp695=( char*)"->";
struct _tagged_string _temp696; _temp696.curr= _temp695; _temp696.base= _temp695;
_temp696.last_plus_one= _temp695 + 3; _temp696;})); Cyc_Absyndump_dump_nospace(*
_temp594); goto _LL397; _LL457: Cyc_Absyndump_dumpexp_prec( myprec, _temp600);
Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp( _temp598); Cyc_Absyndump_dump_char((
int)']'); goto _LL397; _LL459: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp697=( char*)"$("; struct _tagged_string _temp698; _temp698.curr=
_temp697; _temp698.base= _temp697; _temp698.last_plus_one= _temp697 + 3;
_temp698;})); Cyc_Absyndump_dumpexps_prec( 20, _temp602); Cyc_Absyndump_dump_char((
int)')'); goto _LL397; _LL461: Cyc_Absyndump_dump_char(( int)'('); Cyc_Absyndump_dumptyp((*
_temp606).f3); Cyc_Absyndump_dump_char(( int)')');(( void(*)( void(* f)( struct
_tuple7*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde,
_temp604,( struct _tagged_string)({ char* _temp699=( char*)"{"; struct
_tagged_string _temp700; _temp700.curr= _temp699; _temp700.base= _temp699;
_temp700.last_plus_one= _temp699 + 2; _temp700;}),( struct _tagged_string)({
char* _temp701=( char*)"}"; struct _tagged_string _temp702; _temp702.curr=
_temp701; _temp702.base= _temp701; _temp702.last_plus_one= _temp701 + 2;
_temp702;}),( struct _tagged_string)({ char* _temp703=( char*)","; struct
_tagged_string _temp704; _temp704.curr= _temp703; _temp704.base= _temp703;
_temp704.last_plus_one= _temp703 + 2; _temp704;})); goto _LL397; _LL463:(( void(*)(
void(* f)( struct _tuple7*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp608,( struct _tagged_string)({ char* _temp705=( char*)"new {";
struct _tagged_string _temp706; _temp706.curr= _temp705; _temp706.base= _temp705;
_temp706.last_plus_one= _temp705 + 6; _temp706;}),( struct _tagged_string)({
char* _temp707=( char*)"}"; struct _tagged_string _temp708; _temp708.curr=
_temp707; _temp708.base= _temp707; _temp708.last_plus_one= _temp707 + 2;
_temp708;}),( struct _tagged_string)({ char* _temp709=( char*)","; struct
_tagged_string _temp710; _temp710.curr= _temp709; _temp710.base= _temp709;
_temp710.last_plus_one= _temp709 + 2; _temp710;})); goto _LL397; _LL465: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp711=( char*)"new {for"; struct
_tagged_string _temp712; _temp712.curr= _temp711; _temp712.base= _temp711;
_temp712.last_plus_one= _temp711 + 9; _temp712;})); Cyc_Absyndump_dump_str((*
_temp616->name).f2); Cyc_Absyndump_dump_char(( int)'<'); Cyc_Absyndump_dumpexp(
_temp614); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpexp( _temp612);
Cyc_Absyndump_dump_char(( int)'}'); goto _LL397; _LL467: Cyc_Absyndump_dumpqvar(
_temp624);(( void(*)( void(* f)( struct _tuple7*), struct Cyc_List_List* l,
struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpde, _temp620,( struct
_tagged_string)({ char* _temp713=( char*)"{"; struct _tagged_string _temp714;
_temp714.curr= _temp713; _temp714.base= _temp713; _temp714.last_plus_one=
_temp713 + 2; _temp714;}),( struct _tagged_string)({ char* _temp715=( char*)"}";
struct _tagged_string _temp716; _temp716.curr= _temp715; _temp716.base= _temp715;
_temp716.last_plus_one= _temp715 + 2; _temp716;}),( struct _tagged_string)({
char* _temp717=( char*)","; struct _tagged_string _temp718; _temp718.curr=
_temp717; _temp718.base= _temp717; _temp718.last_plus_one= _temp717 + 2;
_temp718;})); goto _LL397; _LL469: _temp640= _temp630; _temp636= _temp626; goto
_LL471; _LL471: Cyc_Absyndump_dumpqvar( _temp636->name); if( _temp640 != 0){((
void(*)( void(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* l, struct
_tagged_string start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp, _temp640,( struct _tagged_string)({ char* _temp719=( char*)"(";
struct _tagged_string _temp720; _temp720.curr= _temp719; _temp720.base= _temp719;
_temp720.last_plus_one= _temp719 + 2; _temp720;}),( struct _tagged_string)({
char* _temp721=( char*)")"; struct _tagged_string _temp722; _temp722.curr=
_temp721; _temp722.base= _temp721; _temp722.last_plus_one= _temp721 + 2;
_temp722;}),( struct _tagged_string)({ char* _temp723=( char*)","; struct
_tagged_string _temp724; _temp724.curr= _temp723; _temp724.base= _temp723;
_temp724.last_plus_one= _temp723 + 2; _temp724;}));} goto _LL397; _LL473:(( void(*)(
void(* f)( struct _tuple7*), struct Cyc_List_List* l, struct _tagged_string
start, struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde, _temp644,( struct _tagged_string)({ char* _temp725=( char*)"{";
struct _tagged_string _temp726; _temp726.curr= _temp725; _temp726.base= _temp725;
_temp726.last_plus_one= _temp725 + 2; _temp726;}),( struct _tagged_string)({
char* _temp727=( char*)"}"; struct _tagged_string _temp728; _temp728.curr=
_temp727; _temp728.base= _temp727; _temp728.last_plus_one= _temp727 + 2;
_temp728;}),( struct _tagged_string)({ char* _temp729=( char*)","; struct
_tagged_string _temp730; _temp730.curr= _temp729; _temp730.base= _temp729;
_temp730.last_plus_one= _temp729 + 2; _temp730;})); goto _LL397; _LL475: Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp731=( char*)"({"; struct _tagged_string
_temp732; _temp732.curr= _temp731; _temp732.base= _temp731; _temp732.last_plus_one=
_temp731 + 3; _temp732;})); Cyc_Absyndump_dumpstmt( _temp648); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp733=( char*)"})"; struct _tagged_string
_temp734; _temp734.curr= _temp733; _temp734.base= _temp733; _temp734.last_plus_one=
_temp733 + 3; _temp734;})); goto _LL397; _LL477: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp735=( char*)"codegen("; struct _tagged_string
_temp736; _temp736.curr= _temp735; _temp736.base= _temp735; _temp736.last_plus_one=
_temp735 + 9; _temp736;})); Cyc_Absyndump_dumpdecl(({ struct Cyc_Absyn_Decl*
_temp737=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp737->r=( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp738=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp738->tag= Cyc_Absyn_Fn_d_tag;
_temp738->f1= _temp650;( void*) _temp738;}); _temp737->loc= e->loc; _temp737;}));
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp739=( char*)")"; struct
_tagged_string _temp740; _temp740.curr= _temp739; _temp740.base= _temp739;
_temp740.last_plus_one= _temp739 + 2; _temp740;})); goto _LL397; _LL479: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp741=( char*)"fill("; struct _tagged_string
_temp742; _temp742.curr= _temp741; _temp742.base= _temp741; _temp742.last_plus_one=
_temp741 + 6; _temp742;})); Cyc_Absyndump_dumpexp( _temp652); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp743=( char*)")"; struct _tagged_string
_temp744; _temp744.curr= _temp743; _temp744.base= _temp743; _temp744.last_plus_one=
_temp743 + 2; _temp744;})); goto _LL397; _LL397:;} if( inprec >= myprec){ Cyc_Absyndump_dump_char((
int)')');}} void Cyc_Absyndump_dumpexp( struct Cyc_Absyn_Exp* e){ Cyc_Absyndump_dumpexp_prec(
0, e);} void Cyc_Absyndump_dumpswitchclauses( struct Cyc_List_List* scs){ for( 0;
scs != 0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause* c=( struct Cyc_Absyn_Switch_clause*)
scs->hd; if( c->where_clause == 0?( void*)( c->pattern)->r == Cyc_Absyn_Wild_p:
0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp745=( char*)"default:";
struct _tagged_string _temp746; _temp746.curr= _temp745; _temp746.base= _temp745;
_temp746.last_plus_one= _temp745 + 9; _temp746;}));} else{ Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp747=( char*)"case"; struct _tagged_string
_temp748; _temp748.curr= _temp747; _temp748.base= _temp747; _temp748.last_plus_one=
_temp747 + 5; _temp748;})); Cyc_Absyndump_dumppat( c->pattern); if( c->where_clause
!= 0){ Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp749=( char*)"&&";
struct _tagged_string _temp750; _temp750.curr= _temp749; _temp750.base= _temp749;
_temp750.last_plus_one= _temp749 + 3; _temp750;})); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp751= c->where_clause; if(
_temp751 == 0){ _throw( Null_Exception);} _temp751;}));} Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp752=( char*)":"; struct _tagged_string
_temp753; _temp753.curr= _temp752; _temp753.base= _temp752; _temp753.last_plus_one=
_temp752 + 2; _temp753;}));} Cyc_Absyndump_dumpstmt( c->body);}} void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt* s){ void* _temp754=( void*) s->r; struct Cyc_Absyn_Exp*
_temp796; struct Cyc_Absyn_Stmt* _temp798; struct Cyc_Absyn_Stmt* _temp800;
struct Cyc_Absyn_Exp* _temp802; struct Cyc_Absyn_Exp* _temp804; struct Cyc_Absyn_Stmt*
_temp806; struct Cyc_Absyn_Stmt* _temp808; struct Cyc_Absyn_Exp* _temp810;
struct Cyc_Absyn_Stmt* _temp812; struct _tuple2 _temp814; struct Cyc_Absyn_Stmt*
_temp816; struct Cyc_Absyn_Exp* _temp818; struct Cyc_Absyn_Stmt* _temp820;
struct Cyc_Absyn_Stmt* _temp822; struct Cyc_Absyn_Stmt* _temp824; struct
_tagged_string* _temp826; struct Cyc_Absyn_Switch_clause** _temp828; struct Cyc_List_List*
_temp830; struct Cyc_Absyn_Switch_clause** _temp832; struct Cyc_List_List*
_temp834; struct Cyc_Absyn_Stmt* _temp836; struct _tuple2 _temp838; struct Cyc_Absyn_Stmt*
_temp840; struct Cyc_Absyn_Exp* _temp842; struct _tuple2 _temp844; struct Cyc_Absyn_Stmt*
_temp846; struct Cyc_Absyn_Exp* _temp848; struct Cyc_Absyn_Exp* _temp850; struct
Cyc_List_List* _temp852; struct Cyc_Absyn_Exp* _temp854; struct Cyc_Absyn_Stmt*
_temp856; struct Cyc_Absyn_Decl* _temp858; struct Cyc_Absyn_Stmt* _temp860;
struct Cyc_Absyn_Stmt* _temp862; struct Cyc_Absyn_Stmt* _temp864; struct
_tagged_string* _temp866; struct _tuple2 _temp868; struct Cyc_Absyn_Stmt*
_temp870; struct Cyc_Absyn_Exp* _temp872; struct Cyc_Absyn_Stmt* _temp874;
struct Cyc_List_List* _temp876; struct Cyc_Absyn_Stmt* _temp878; _LL756: if(
_temp754 == Cyc_Absyn_Skip_s){ goto _LL757;} else{ goto _LL758;} _LL758: if((
unsigned int) _temp754 > 1u?(( struct _enum_struct*) _temp754)->tag == Cyc_Absyn_Exp_s_tag:
0){ _LL797: _temp796=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp_s_struct*)
_temp754)->f1; goto _LL759;} else{ goto _LL760;} _LL760: if(( unsigned int)
_temp754 > 1u?(( struct _enum_struct*) _temp754)->tag == Cyc_Absyn_Seq_s_tag: 0){
_LL801: _temp800=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Seq_s_struct*)
_temp754)->f1; goto _LL799; _LL799: _temp798=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_Seq_s_struct*) _temp754)->f2; goto _LL761;} else{ goto _LL762;} _LL762:
if(( unsigned int) _temp754 > 1u?(( struct _enum_struct*) _temp754)->tag == Cyc_Absyn_Return_s_tag:
0){ _LL803: _temp802=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Return_s_struct*)
_temp754)->f1; if( _temp802 == 0){ goto _LL763;} else{ goto _LL764;}} else{ goto
_LL764;} _LL764: if(( unsigned int) _temp754 > 1u?(( struct _enum_struct*)
_temp754)->tag == Cyc_Absyn_Return_s_tag: 0){ _LL805: _temp804=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Return_s_struct*) _temp754)->f1; goto _LL765;} else{ goto
_LL766;} _LL766: if(( unsigned int) _temp754 > 1u?(( struct _enum_struct*)
_temp754)->tag == Cyc_Absyn_IfThenElse_s_tag: 0){ _LL811: _temp810=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp754)->f1; goto _LL809; _LL809:
_temp808=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp754)->f2; goto _LL807; _LL807: _temp806=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp754)->f3; goto _LL767;} else{ goto _LL768;}
_LL768: if(( unsigned int) _temp754 > 1u?(( struct _enum_struct*) _temp754)->tag
== Cyc_Absyn_While_s_tag: 0){ _LL815: _temp814=( struct _tuple2)(( struct Cyc_Absyn_While_s_struct*)
_temp754)->f1; _LL819: _temp818= _temp814.f1; goto _LL817; _LL817: _temp816=
_temp814.f2; goto _LL813; _LL813: _temp812=( struct Cyc_Absyn_Stmt*)(( struct
Cyc_Absyn_While_s_struct*) _temp754)->f2; goto _LL769;} else{ goto _LL770;}
_LL770: if(( unsigned int) _temp754 > 1u?(( struct _enum_struct*) _temp754)->tag
== Cyc_Absyn_Break_s_tag: 0){ _LL821: _temp820=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Break_s_struct*) _temp754)->f1; goto _LL771;} else{ goto _LL772;}
_LL772: if(( unsigned int) _temp754 > 1u?(( struct _enum_struct*) _temp754)->tag
== Cyc_Absyn_Continue_s_tag: 0){ _LL823: _temp822=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Absyn_Continue_s_struct*) _temp754)->f1; goto _LL773;} else{ goto
_LL774;} _LL774: if(( unsigned int) _temp754 > 1u?(( struct _enum_struct*)
_temp754)->tag == Cyc_Absyn_Goto_s_tag: 0){ _LL827: _temp826=( struct
_tagged_string*)(( struct Cyc_Absyn_Goto_s_struct*) _temp754)->f1; goto _LL825;
_LL825: _temp824=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Goto_s_struct*)
_temp754)->f2; goto _LL775;} else{ goto _LL776;} _LL776: if(( unsigned int)
_temp754 > 1u?(( struct _enum_struct*) _temp754)->tag == Cyc_Absyn_Fallthru_s_tag:
0){ _LL831: _temp830=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp754)->f1; if( _temp830 == 0){ goto _LL829;} else{ goto _LL778;} _LL829:
_temp828=( struct Cyc_Absyn_Switch_clause**)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp754)->f2; goto _LL777;} else{ goto _LL778;} _LL778: if(( unsigned int)
_temp754 > 1u?(( struct _enum_struct*) _temp754)->tag == Cyc_Absyn_Fallthru_s_tag:
0){ _LL835: _temp834=( struct Cyc_List_List*)(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp754)->f1; goto _LL833; _LL833: _temp832=( struct Cyc_Absyn_Switch_clause**)((
struct Cyc_Absyn_Fallthru_s_struct*) _temp754)->f2; goto _LL779;} else{ goto
_LL780;} _LL780: if(( unsigned int) _temp754 > 1u?(( struct _enum_struct*)
_temp754)->tag == Cyc_Absyn_For_s_tag: 0){ _LL851: _temp850=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_For_s_struct*) _temp754)->f1; goto _LL845; _LL845: _temp844=(
struct _tuple2)(( struct Cyc_Absyn_For_s_struct*) _temp754)->f2; _LL849:
_temp848= _temp844.f1; goto _LL847; _LL847: _temp846= _temp844.f2; goto _LL839;
_LL839: _temp838=( struct _tuple2)(( struct Cyc_Absyn_For_s_struct*) _temp754)->f3;
_LL843: _temp842= _temp838.f1; goto _LL841; _LL841: _temp840= _temp838.f2; goto
_LL837; _LL837: _temp836=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_For_s_struct*)
_temp754)->f4; goto _LL781;} else{ goto _LL782;} _LL782: if(( unsigned int)
_temp754 > 1u?(( struct _enum_struct*) _temp754)->tag == Cyc_Absyn_Switch_s_tag:
0){ _LL855: _temp854=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Switch_s_struct*)
_temp754)->f1; goto _LL853; _LL853: _temp852=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Switch_s_struct*) _temp754)->f2; goto _LL783;} else{ goto _LL784;}
_LL784: if(( unsigned int) _temp754 > 1u?(( struct _enum_struct*) _temp754)->tag
== Cyc_Absyn_Decl_s_tag: 0){ _LL859: _temp858=( struct Cyc_Absyn_Decl*)(( struct
Cyc_Absyn_Decl_s_struct*) _temp754)->f1; goto _LL857; _LL857: _temp856=( struct
Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Decl_s_struct*) _temp754)->f2; goto _LL785;}
else{ goto _LL786;} _LL786: if(( unsigned int) _temp754 > 1u?(( struct
_enum_struct*) _temp754)->tag == Cyc_Absyn_Cut_s_tag: 0){ _LL861: _temp860=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Cut_s_struct*) _temp754)->f1; goto
_LL787;} else{ goto _LL788;} _LL788: if(( unsigned int) _temp754 > 1u?(( struct
_enum_struct*) _temp754)->tag == Cyc_Absyn_Splice_s_tag: 0){ _LL863: _temp862=(
struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Splice_s_struct*) _temp754)->f1; goto
_LL789;} else{ goto _LL790;} _LL790: if(( unsigned int) _temp754 > 1u?(( struct
_enum_struct*) _temp754)->tag == Cyc_Absyn_Label_s_tag: 0){ _LL867: _temp866=(
struct _tagged_string*)(( struct Cyc_Absyn_Label_s_struct*) _temp754)->f1; goto
_LL865; _LL865: _temp864=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Label_s_struct*)
_temp754)->f2; goto _LL791;} else{ goto _LL792;} _LL792: if(( unsigned int)
_temp754 > 1u?(( struct _enum_struct*) _temp754)->tag == Cyc_Absyn_Do_s_tag: 0){
_LL875: _temp874=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_Do_s_struct*)
_temp754)->f1; goto _LL869; _LL869: _temp868=( struct _tuple2)(( struct Cyc_Absyn_Do_s_struct*)
_temp754)->f2; _LL873: _temp872= _temp868.f1; goto _LL871; _LL871: _temp870=
_temp868.f2; goto _LL793;} else{ goto _LL794;} _LL794: if(( unsigned int)
_temp754 > 1u?(( struct _enum_struct*) _temp754)->tag == Cyc_Absyn_TryCatch_s_tag:
0){ _LL879: _temp878=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp754)->f1; goto _LL877; _LL877: _temp876=( struct Cyc_List_List*)(( struct
Cyc_Absyn_TryCatch_s_struct*) _temp754)->f2; goto _LL795;} else{ goto _LL755;}
_LL757: Cyc_Absyndump_dump_semi(); goto _LL755; _LL759: Cyc_Absyndump_dumpexp(
_temp796); Cyc_Absyndump_dump_semi(); goto _LL755; _LL761: if( Cyc_Absynpp_is_declaration(
_temp800)){ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt( _temp800);
Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dumpstmt( _temp800);}
if( Cyc_Absynpp_is_declaration( _temp798)){ Cyc_Absyndump_dump_char(( int)'{');
Cyc_Absyndump_dumpstmt( _temp798); Cyc_Absyndump_dump_char(( int)'}');} else{
Cyc_Absyndump_dumpstmt( _temp798);} goto _LL755; _LL763: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp880=( char*)"return;"; struct _tagged_string
_temp881; _temp881.curr= _temp880; _temp881.base= _temp880; _temp881.last_plus_one=
_temp880 + 8; _temp881;})); goto _LL755; _LL765: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp882=( char*)"return"; struct _tagged_string
_temp883; _temp883.curr= _temp882; _temp883.base= _temp882; _temp883.last_plus_one=
_temp882 + 7; _temp883;})); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp884= _temp804; if( _temp884 == 0){ _throw(
Null_Exception);} _temp884;})); Cyc_Absyndump_dump_semi(); goto _LL755; _LL767:
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp885=( char*)"if(";
struct _tagged_string _temp886; _temp886.curr= _temp885; _temp886.base= _temp885;
_temp886.last_plus_one= _temp885 + 4; _temp886;})); Cyc_Absyndump_dumpexp(
_temp810); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp887=(
char*)"){"; struct _tagged_string _temp888; _temp888.curr= _temp887; _temp888.base=
_temp887; _temp888.last_plus_one= _temp887 + 3; _temp888;})); Cyc_Absyndump_dumpstmt(
_temp808); Cyc_Absyndump_dump_char(( int)'}');{ void* _temp889=( void*) _temp806->r;
_LL891: if( _temp889 == Cyc_Absyn_Skip_s){ goto _LL892;} else{ goto _LL893;}
_LL893: goto _LL894; _LL892: goto _LL890; _LL894: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp895=( char*)"else{"; struct _tagged_string _temp896;
_temp896.curr= _temp895; _temp896.base= _temp895; _temp896.last_plus_one=
_temp895 + 6; _temp896;})); Cyc_Absyndump_dumpstmt( _temp806); Cyc_Absyndump_dump_char((
int)'}'); goto _LL890; _LL890:;} goto _LL755; _LL769: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp897=( char*)"while("; struct _tagged_string
_temp898; _temp898.curr= _temp897; _temp898.base= _temp897; _temp898.last_plus_one=
_temp897 + 7; _temp898;})); Cyc_Absyndump_dumpexp( _temp818); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp899=( char*)") {"; struct _tagged_string
_temp900; _temp900.curr= _temp899; _temp900.base= _temp899; _temp900.last_plus_one=
_temp899 + 4; _temp900;})); Cyc_Absyndump_dumpstmt( _temp812); Cyc_Absyndump_dump_char((
int)'}'); goto _LL755; _LL771: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp901=( char*)"break;"; struct _tagged_string _temp902; _temp902.curr=
_temp901; _temp902.base= _temp901; _temp902.last_plus_one= _temp901 + 7;
_temp902;})); goto _LL755; _LL773: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp903=( char*)"continue;"; struct _tagged_string _temp904; _temp904.curr=
_temp903; _temp904.base= _temp903; _temp904.last_plus_one= _temp903 + 10;
_temp904;})); goto _LL755; _LL775: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp905=( char*)"goto"; struct _tagged_string _temp906; _temp906.curr=
_temp905; _temp906.base= _temp905; _temp906.last_plus_one= _temp905 + 5;
_temp906;})); Cyc_Absyndump_dump_str( _temp826); Cyc_Absyndump_dump_semi(); goto
_LL755; _LL777: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp907=(
char*)"fallthru;"; struct _tagged_string _temp908; _temp908.curr= _temp907;
_temp908.base= _temp907; _temp908.last_plus_one= _temp907 + 10; _temp908;}));
goto _LL755; _LL779: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp909=( char*)"fallthru("; struct _tagged_string _temp910; _temp910.curr=
_temp909; _temp910.base= _temp909; _temp910.last_plus_one= _temp909 + 10;
_temp910;})); Cyc_Absyndump_dumpexps_prec( 20, _temp834); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp911=( char*)");"; struct _tagged_string
_temp912; _temp912.curr= _temp911; _temp912.base= _temp911; _temp912.last_plus_one=
_temp911 + 3; _temp912;})); goto _LL755; _LL781: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp913=( char*)"for("; struct _tagged_string _temp914;
_temp914.curr= _temp913; _temp914.base= _temp913; _temp914.last_plus_one=
_temp913 + 5; _temp914;})); Cyc_Absyndump_dumpexp( _temp850); Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp( _temp848); Cyc_Absyndump_dump_semi(); Cyc_Absyndump_dumpexp(
_temp842); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp915=(
char*)"){"; struct _tagged_string _temp916; _temp916.curr= _temp915; _temp916.base=
_temp915; _temp916.last_plus_one= _temp915 + 3; _temp916;})); Cyc_Absyndump_dumpstmt(
_temp836); Cyc_Absyndump_dump_char(( int)'}'); goto _LL755; _LL783: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp917=( char*)"switch("; struct _tagged_string
_temp918; _temp918.curr= _temp917; _temp918.base= _temp917; _temp918.last_plus_one=
_temp917 + 8; _temp918;})); Cyc_Absyndump_dumpexp( _temp854); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp919=( char*)"){"; struct _tagged_string
_temp920; _temp920.curr= _temp919; _temp920.base= _temp919; _temp920.last_plus_one=
_temp919 + 3; _temp920;})); Cyc_Absyndump_dumpswitchclauses( _temp852); Cyc_Absyndump_dump_char((
int)'}'); goto _LL755; _LL785: Cyc_Absyndump_dumpdecl( _temp858); Cyc_Absyndump_dumpstmt(
_temp856); goto _LL755; _LL787: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp921=( char*)"cut"; struct _tagged_string _temp922; _temp922.curr=
_temp921; _temp922.base= _temp921; _temp922.last_plus_one= _temp921 + 4;
_temp922;})); Cyc_Absyndump_dumpstmt( _temp860); goto _LL755; _LL789: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp923=( char*)"splice"; struct _tagged_string
_temp924; _temp924.curr= _temp923; _temp924.base= _temp923; _temp924.last_plus_one=
_temp923 + 7; _temp924;})); Cyc_Absyndump_dumpstmt( _temp862); goto _LL755;
_LL791: if( Cyc_Absynpp_is_declaration( _temp864)){ Cyc_Absyndump_dump_str(
_temp866); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp925=(
char*)": {"; struct _tagged_string _temp926; _temp926.curr= _temp925; _temp926.base=
_temp925; _temp926.last_plus_one= _temp925 + 4; _temp926;})); Cyc_Absyndump_dumpstmt(
_temp864); Cyc_Absyndump_dump_char(( int)'}');} else{ Cyc_Absyndump_dump_str(
_temp866); Cyc_Absyndump_dump_char(( int)':'); Cyc_Absyndump_dumpstmt( _temp864);}
goto _LL755; _LL793: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp927=( char*)"do {"; struct _tagged_string _temp928; _temp928.curr= _temp927;
_temp928.base= _temp927; _temp928.last_plus_one= _temp927 + 5; _temp928;})); Cyc_Absyndump_dumpstmt(
_temp874); Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp929=(
char*)"} while ("; struct _tagged_string _temp930; _temp930.curr= _temp929;
_temp930.base= _temp929; _temp930.last_plus_one= _temp929 + 10; _temp930;}));
Cyc_Absyndump_dumpexp( _temp872); Cyc_Absyndump_dump_nospace(( struct
_tagged_string)({ char* _temp931=( char*)");"; struct _tagged_string _temp932;
_temp932.curr= _temp931; _temp932.base= _temp931; _temp932.last_plus_one=
_temp931 + 3; _temp932;})); goto _LL755; _LL795: Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp933=( char*)"try"; struct _tagged_string _temp934;
_temp934.curr= _temp933; _temp934.base= _temp933; _temp934.last_plus_one=
_temp933 + 4; _temp934;})); Cyc_Absyndump_dumpstmt( _temp878); Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp935=( char*)"catch {"; struct _tagged_string
_temp936; _temp936.curr= _temp935; _temp936.base= _temp935; _temp936.last_plus_one=
_temp935 + 8; _temp936;})); Cyc_Absyndump_dumpswitchclauses( _temp876); Cyc_Absyndump_dump_char((
int)'}'); goto _LL755; _LL755:;} void Cyc_Absyndump_dumpdp( struct _tuple8* dp){((
void(*)( void(* f)( void*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(* dp).f1,( struct _tagged_string)({ char* _temp937=(
char*)""; struct _tagged_string _temp938; _temp938.curr= _temp937; _temp938.base=
_temp937; _temp938.last_plus_one= _temp937 + 1; _temp938;}),( struct
_tagged_string)({ char* _temp939=( char*)"="; struct _tagged_string _temp940;
_temp940.curr= _temp939; _temp940.base= _temp939; _temp940.last_plus_one=
_temp939 + 2; _temp940;}),( struct _tagged_string)({ char* _temp941=( char*)"=";
struct _tagged_string _temp942; _temp942.curr= _temp941; _temp942.base= _temp941;
_temp942.last_plus_one= _temp941 + 2; _temp942;})); Cyc_Absyndump_dumppat((* dp).f2);}
void Cyc_Absyndump_dumppat( struct Cyc_Absyn_Pat* p){ void* _temp943=( void*) p->r;
int _temp977; void* _temp979; int _temp981; void* _temp983; char _temp985;
struct _tagged_string _temp987; struct Cyc_Absyn_Vardecl* _temp989; struct Cyc_List_List*
_temp991; struct Cyc_Absyn_Pat* _temp993; struct Cyc_Absyn_Vardecl* _temp995;
struct _tuple0* _temp997; struct Cyc_List_List* _temp999; struct Cyc_List_List*
_temp1001; struct _tuple0* _temp1003; struct Cyc_List_List* _temp1005; struct
Cyc_List_List* _temp1007; struct _tuple0* _temp1009; struct Cyc_List_List*
_temp1011; struct Cyc_List_List* _temp1013; struct Cyc_Core_Opt* _temp1015;
struct Cyc_Absyn_Structdecl* _temp1017; struct Cyc_Absyn_Enumfield* _temp1019;
struct Cyc_Absyn_Enumdecl* _temp1021; struct Cyc_List_List* _temp1023; struct
Cyc_List_List* _temp1025; struct Cyc_Core_Opt* _temp1027; struct _tuple0*
_temp1029; struct Cyc_Absyn_Enumfield* _temp1031; struct Cyc_Absyn_Xenumdecl*
_temp1033; struct Cyc_List_List* _temp1035; struct Cyc_List_List* _temp1037;
struct _tuple0* _temp1039; _LL945: if( _temp943 == Cyc_Absyn_Wild_p){ goto
_LL946;} else{ goto _LL947;} _LL947: if( _temp943 == Cyc_Absyn_Null_p){ goto
_LL948;} else{ goto _LL949;} _LL949: if(( unsigned int) _temp943 > 2u?(( struct
_enum_struct*) _temp943)->tag == Cyc_Absyn_Int_p_tag: 0){ _LL980: _temp979=(
void*)(( struct Cyc_Absyn_Int_p_struct*) _temp943)->f1; if( _temp979 == Cyc_Absyn_Signed){
goto _LL978;} else{ goto _LL951;} _LL978: _temp977=( int)(( struct Cyc_Absyn_Int_p_struct*)
_temp943)->f2; goto _LL950;} else{ goto _LL951;} _LL951: if(( unsigned int)
_temp943 > 2u?(( struct _enum_struct*) _temp943)->tag == Cyc_Absyn_Int_p_tag: 0){
_LL984: _temp983=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp943)->f1; if(
_temp983 == Cyc_Absyn_Unsigned){ goto _LL982;} else{ goto _LL953;} _LL982:
_temp981=( int)(( struct Cyc_Absyn_Int_p_struct*) _temp943)->f2; goto _LL952;}
else{ goto _LL953;} _LL953: if(( unsigned int) _temp943 > 2u?(( struct
_enum_struct*) _temp943)->tag == Cyc_Absyn_Char_p_tag: 0){ _LL986: _temp985=(
char)(( struct Cyc_Absyn_Char_p_struct*) _temp943)->f1; goto _LL954;} else{ goto
_LL955;} _LL955: if(( unsigned int) _temp943 > 2u?(( struct _enum_struct*)
_temp943)->tag == Cyc_Absyn_Float_p_tag: 0){ _LL988: _temp987=( struct
_tagged_string)(( struct Cyc_Absyn_Float_p_struct*) _temp943)->f1; goto _LL956;}
else{ goto _LL957;} _LL957: if(( unsigned int) _temp943 > 2u?(( struct
_enum_struct*) _temp943)->tag == Cyc_Absyn_Var_p_tag: 0){ _LL990: _temp989=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_p_struct*) _temp943)->f1; goto
_LL958;} else{ goto _LL959;} _LL959: if(( unsigned int) _temp943 > 2u?(( struct
_enum_struct*) _temp943)->tag == Cyc_Absyn_Tuple_p_tag: 0){ _LL992: _temp991=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Tuple_p_struct*) _temp943)->f1; goto
_LL960;} else{ goto _LL961;} _LL961: if(( unsigned int) _temp943 > 2u?(( struct
_enum_struct*) _temp943)->tag == Cyc_Absyn_Pointer_p_tag: 0){ _LL994: _temp993=(
struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Pointer_p_struct*) _temp943)->f1; goto
_LL962;} else{ goto _LL963;} _LL963: if(( unsigned int) _temp943 > 2u?(( struct
_enum_struct*) _temp943)->tag == Cyc_Absyn_Reference_p_tag: 0){ _LL996: _temp995=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Reference_p_struct*) _temp943)->f1;
goto _LL964;} else{ goto _LL965;} _LL965: if(( unsigned int) _temp943 > 2u?((
struct _enum_struct*) _temp943)->tag == Cyc_Absyn_UnknownId_p_tag: 0){ _LL998:
_temp997=( struct _tuple0*)(( struct Cyc_Absyn_UnknownId_p_struct*) _temp943)->f1;
goto _LL966;} else{ goto _LL967;} _LL967: if(( unsigned int) _temp943 > 2u?((
struct _enum_struct*) _temp943)->tag == Cyc_Absyn_UnknownCall_p_tag: 0){ _LL1004:
_temp1003=( struct _tuple0*)(( struct Cyc_Absyn_UnknownCall_p_struct*) _temp943)->f1;
goto _LL1002; _LL1002: _temp1001=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp943)->f2; goto _LL1000; _LL1000: _temp999=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnknownCall_p_struct*) _temp943)->f3; goto _LL968;} else{ goto _LL969;}
_LL969: if(( unsigned int) _temp943 > 2u?(( struct _enum_struct*) _temp943)->tag
== Cyc_Absyn_UnknownFields_p_tag: 0){ _LL1010: _temp1009=( struct _tuple0*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp943)->f1; goto _LL1008; _LL1008:
_temp1007=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp943)->f2; goto _LL1006; _LL1006: _temp1005=( struct Cyc_List_List*)((
struct Cyc_Absyn_UnknownFields_p_struct*) _temp943)->f3; goto _LL970;} else{
goto _LL971;} _LL971: if(( unsigned int) _temp943 > 2u?(( struct _enum_struct*)
_temp943)->tag == Cyc_Absyn_Struct_p_tag: 0){ _LL1018: _temp1017=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_p_struct*) _temp943)->f1; goto _LL1016; _LL1016:
_temp1015=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Struct_p_struct*) _temp943)->f2;
goto _LL1014; _LL1014: _temp1013=( struct Cyc_List_List*)(( struct Cyc_Absyn_Struct_p_struct*)
_temp943)->f3; goto _LL1012; _LL1012: _temp1011=( struct Cyc_List_List*)((
struct Cyc_Absyn_Struct_p_struct*) _temp943)->f4; goto _LL972;} else{ goto
_LL973;} _LL973: if(( unsigned int) _temp943 > 2u?(( struct _enum_struct*)
_temp943)->tag == Cyc_Absyn_Enum_p_tag: 0){ _LL1030: _temp1029=( struct _tuple0*)((
struct Cyc_Absyn_Enum_p_struct*) _temp943)->f1; goto _LL1028; _LL1028: _temp1027=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Enum_p_struct*) _temp943)->f2; goto
_LL1026; _LL1026: _temp1025=( struct Cyc_List_List*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp943)->f3; goto _LL1024; _LL1024: _temp1023=( struct Cyc_List_List*)((
struct Cyc_Absyn_Enum_p_struct*) _temp943)->f4; goto _LL1022; _LL1022: _temp1021=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_p_struct*) _temp943)->f5;
goto _LL1020; _LL1020: _temp1019=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Absyn_Enum_p_struct*)
_temp943)->f6; goto _LL974;} else{ goto _LL975;} _LL975: if(( unsigned int)
_temp943 > 2u?(( struct _enum_struct*) _temp943)->tag == Cyc_Absyn_Xenum_p_tag:
0){ _LL1040: _temp1039=( struct _tuple0*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp943)->f1; goto _LL1038; _LL1038: _temp1037=( struct Cyc_List_List*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp943)->f2; goto _LL1036; _LL1036:
_temp1035=( struct Cyc_List_List*)(( struct Cyc_Absyn_Xenum_p_struct*) _temp943)->f3;
goto _LL1034; _LL1034: _temp1033=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_p_struct*)
_temp943)->f4; goto _LL1032; _LL1032: _temp1031=( struct Cyc_Absyn_Enumfield*)((
struct Cyc_Absyn_Xenum_p_struct*) _temp943)->f5; goto _LL976;} else{ goto _LL944;}
_LL946: Cyc_Absyndump_dump_char(( int)'_'); goto _LL944; _LL948: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1041=( char*)"null"; struct _tagged_string
_temp1042; _temp1042.curr= _temp1041; _temp1042.base= _temp1041; _temp1042.last_plus_one=
_temp1041 + 5; _temp1042;})); goto _LL944; _LL950: Cyc_Absyndump_dump( xprintf("%d",
_temp977)); goto _LL944; _LL952: Cyc_Absyndump_dump( xprintf("%u",( unsigned int)
_temp981)); goto _LL944; _LL954: Cyc_Absyndump_dump(( struct _tagged_string)({
char* _temp1043=( char*)"'"; struct _tagged_string _temp1044; _temp1044.curr=
_temp1043; _temp1044.base= _temp1043; _temp1044.last_plus_one= _temp1043 + 2;
_temp1044;})); Cyc_Absyndump_dump_nospace( Cyc_Absynpp_char_escape( _temp985));
Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp1045=( char*)"'";
struct _tagged_string _temp1046; _temp1046.curr= _temp1045; _temp1046.base=
_temp1045; _temp1046.last_plus_one= _temp1045 + 2; _temp1046;})); goto _LL944;
_LL956: Cyc_Absyndump_dump( _temp987); goto _LL944; _LL958: Cyc_Absyndump_dumpqvar(
_temp989->name); goto _LL944; _LL960:(( void(*)( void(* f)( struct Cyc_Absyn_Pat*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat,
_temp991,( struct _tagged_string)({ char* _temp1047=( char*)"$("; struct
_tagged_string _temp1048; _temp1048.curr= _temp1047; _temp1048.base= _temp1047;
_temp1048.last_plus_one= _temp1047 + 3; _temp1048;}),( struct _tagged_string)({
char* _temp1049=( char*)")"; struct _tagged_string _temp1050; _temp1050.curr=
_temp1049; _temp1050.base= _temp1049; _temp1050.last_plus_one= _temp1049 + 2;
_temp1050;}),( struct _tagged_string)({ char* _temp1051=( char*)","; struct
_tagged_string _temp1052; _temp1052.curr= _temp1051; _temp1052.base= _temp1051;
_temp1052.last_plus_one= _temp1051 + 2; _temp1052;})); goto _LL944; _LL962: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1053=( char*)"&"; struct _tagged_string
_temp1054; _temp1054.curr= _temp1053; _temp1054.base= _temp1053; _temp1054.last_plus_one=
_temp1053 + 2; _temp1054;})); Cyc_Absyndump_dumppat( _temp993); goto _LL944;
_LL964: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1055=( char*)"*";
struct _tagged_string _temp1056; _temp1056.curr= _temp1055; _temp1056.base=
_temp1055; _temp1056.last_plus_one= _temp1055 + 2; _temp1056;})); Cyc_Absyndump_dumpqvar(
_temp995->name); goto _LL944; _LL966: Cyc_Absyndump_dumpqvar( _temp997); goto
_LL944; _LL968: Cyc_Absyndump_dumpqvar( _temp1003); Cyc_Absyndump_dumptvars(
_temp1001);(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp999,( struct
_tagged_string)({ char* _temp1057=( char*)"("; struct _tagged_string _temp1058;
_temp1058.curr= _temp1057; _temp1058.base= _temp1057; _temp1058.last_plus_one=
_temp1057 + 2; _temp1058;}),( struct _tagged_string)({ char* _temp1059=( char*)")";
struct _tagged_string _temp1060; _temp1060.curr= _temp1059; _temp1060.base=
_temp1059; _temp1060.last_plus_one= _temp1059 + 2; _temp1060;}),( struct
_tagged_string)({ char* _temp1061=( char*)","; struct _tagged_string _temp1062;
_temp1062.curr= _temp1061; _temp1062.base= _temp1061; _temp1062.last_plus_one=
_temp1061 + 2; _temp1062;})); goto _LL944; _LL970: Cyc_Absyndump_dumpqvar(
_temp1009); Cyc_Absyndump_dumptvars( _temp1007);(( void(*)( void(* f)( struct
_tuple8*), struct Cyc_List_List* l, struct _tagged_string start, struct
_tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp1005,( struct _tagged_string)({ char* _temp1063=( char*)"{"; struct
_tagged_string _temp1064; _temp1064.curr= _temp1063; _temp1064.base= _temp1063;
_temp1064.last_plus_one= _temp1063 + 2; _temp1064;}),( struct _tagged_string)({
char* _temp1065=( char*)"}"; struct _tagged_string _temp1066; _temp1066.curr=
_temp1065; _temp1066.base= _temp1065; _temp1066.last_plus_one= _temp1065 + 2;
_temp1066;}),( struct _tagged_string)({ char* _temp1067=( char*)","; struct
_tagged_string _temp1068; _temp1068.curr= _temp1067; _temp1068.base= _temp1067;
_temp1068.last_plus_one= _temp1067 + 2; _temp1068;})); goto _LL944; _LL972: if(
_temp1017->name != 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)( _temp1017->name)->v);}
Cyc_Absyndump_dumptvars( _temp1013);(( void(*)( void(* f)( struct _tuple8*),
struct Cyc_List_List* l, struct _tagged_string start, struct _tagged_string end,
struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumpdp,
_temp1011,( struct _tagged_string)({ char* _temp1069=( char*)"{"; struct
_tagged_string _temp1070; _temp1070.curr= _temp1069; _temp1070.base= _temp1069;
_temp1070.last_plus_one= _temp1069 + 2; _temp1070;}),( struct _tagged_string)({
char* _temp1071=( char*)"}"; struct _tagged_string _temp1072; _temp1072.curr=
_temp1071; _temp1072.base= _temp1071; _temp1072.last_plus_one= _temp1071 + 2;
_temp1072;}),( struct _tagged_string)({ char* _temp1073=( char*)","; struct
_tagged_string _temp1074; _temp1074.curr= _temp1073; _temp1074.base= _temp1073;
_temp1074.last_plus_one= _temp1073 + 2; _temp1074;})); goto _LL944; _LL974:
_temp1039= _temp1029; _temp1037= _temp1025; _temp1035= _temp1023; goto _LL976;
_LL976: Cyc_Absyndump_dumpqvar( _temp1039); Cyc_Absyndump_dumptvars( _temp1037);
if( _temp1035 != 0){(( void(*)( void(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
l, struct _tagged_string start, struct _tagged_string end, struct _tagged_string
sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dumppat, _temp1035,( struct
_tagged_string)({ char* _temp1075=( char*)"("; struct _tagged_string _temp1076;
_temp1076.curr= _temp1075; _temp1076.base= _temp1075; _temp1076.last_plus_one=
_temp1075 + 2; _temp1076;}),( struct _tagged_string)({ char* _temp1077=( char*)")";
struct _tagged_string _temp1078; _temp1078.curr= _temp1077; _temp1078.base=
_temp1077; _temp1078.last_plus_one= _temp1077 + 2; _temp1078;}),( struct
_tagged_string)({ char* _temp1079=( char*)","; struct _tagged_string _temp1080;
_temp1080.curr= _temp1079; _temp1080.base= _temp1079; _temp1080.last_plus_one=
_temp1079 + 2; _temp1080;}));} goto _LL944; _LL944:;} void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield* ef){ Cyc_Absyndump_dumpqvar( ef->name); if( ef->tag
!= 0){ Cyc_Absyndump_dump_char(( int)'='); Cyc_Absyndump_dumpexp(( struct Cyc_Absyn_Exp*)({
struct Cyc_Absyn_Exp* _temp1081= ef->tag; if( _temp1081 == 0){ _throw(
Null_Exception);} _temp1081;}));} if( ef->typs != 0){ Cyc_Absyndump_dumpargs( ef->typs);}}
void Cyc_Absyndump_dumpenumfields( struct Cyc_List_List* fields){(( void(*)(
void(* f)( struct Cyc_Absyn_Enumfield*), struct Cyc_List_List* l, struct
_tagged_string sep)) Cyc_Absyndump_dump_sep)( Cyc_Absyndump_dumpenumfield,
fields,( struct _tagged_string)({ char* _temp1082=( char*)","; struct
_tagged_string _temp1083; _temp1083.curr= _temp1082; _temp1083.base= _temp1082;
_temp1083.last_plus_one= _temp1082 + 2; _temp1083;}));} void Cyc_Absyndump_dumpstructfields(
struct Cyc_List_List* fields){ for( 0; fields != 0; fields= fields->tl){ struct
_tuple3 _temp1086; void* _temp1087; struct Cyc_Absyn_Tqual* _temp1089; struct
_tagged_string* _temp1091; struct _tuple3* _temp1084=( struct _tuple3*) fields->hd;
_temp1086=* _temp1084; _LL1092: _temp1091= _temp1086.f1; goto _LL1090; _LL1090:
_temp1089= _temp1086.f2; goto _LL1088; _LL1088: _temp1087= _temp1086.f3; goto
_LL1085; _LL1085:(( void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( struct
_tagged_string*), struct _tagged_string*)) Cyc_Absyndump_dumptqtd)(( struct Cyc_Absyn_Tqual*)
_temp1089, _temp1087, Cyc_Absyndump_dump_str, _temp1091); Cyc_Absyndump_dump_semi();}}
void Cyc_Absyndump_dumptypedefname( struct Cyc_Absyn_Typedefdecl* td){ Cyc_Absyndump_dumpqvar(
td->name); Cyc_Absyndump_dumptvars( td->tvs);} void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl* d){ void* _temp1093=( void*) d->r; struct Cyc_Absyn_Fndecl*
_temp1117; struct Cyc_Absyn_Structdecl* _temp1119; struct Cyc_Absyn_Vardecl*
_temp1121; struct Cyc_Absyn_Vardecl _temp1123; struct Cyc_Core_Opt* _temp1124;
int _temp1126; struct Cyc_Absyn_Exp* _temp1128; void* _temp1130; struct Cyc_Absyn_Tqual*
_temp1132; struct _tuple0* _temp1134; void* _temp1136; struct Cyc_Absyn_Enumdecl*
_temp1138; struct Cyc_Absyn_Xenumdecl* _temp1140; int _temp1142; struct Cyc_Absyn_Exp*
_temp1144; struct Cyc_Core_Opt* _temp1146; struct Cyc_Core_Opt* _temp1148;
struct Cyc_Absyn_Pat* _temp1150; struct Cyc_Absyn_Typedefdecl* _temp1152; struct
Cyc_List_List* _temp1154; struct _tagged_string* _temp1156; struct Cyc_List_List*
_temp1158; struct _tuple0* _temp1160; struct Cyc_List_List* _temp1162; _LL1095:
if(( unsigned int) _temp1093 > 1u?(( struct _enum_struct*) _temp1093)->tag ==
Cyc_Absyn_Fn_d_tag: 0){ _LL1118: _temp1117=( struct Cyc_Absyn_Fndecl*)(( struct
Cyc_Absyn_Fn_d_struct*) _temp1093)->f1; goto _LL1096;} else{ goto _LL1097;}
_LL1097: if(( unsigned int) _temp1093 > 1u?(( struct _enum_struct*) _temp1093)->tag
== Cyc_Absyn_Struct_d_tag: 0){ _LL1120: _temp1119=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp1093)->f1; goto _LL1098;} else{ goto
_LL1099;} _LL1099: if(( unsigned int) _temp1093 > 1u?(( struct _enum_struct*)
_temp1093)->tag == Cyc_Absyn_Var_d_tag: 0){ _LL1122: _temp1121=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1093)->f1; _temp1123=* _temp1121; _LL1137:
_temp1136=( void*) _temp1123.sc; goto _LL1135; _LL1135: _temp1134=( struct
_tuple0*) _temp1123.name; goto _LL1133; _LL1133: _temp1132=( struct Cyc_Absyn_Tqual*)
_temp1123.tq; goto _LL1131; _LL1131: _temp1130=( void*) _temp1123.type; goto
_LL1129; _LL1129: _temp1128=( struct Cyc_Absyn_Exp*) _temp1123.initializer; goto
_LL1127; _LL1127: _temp1126=( int) _temp1123.shadow; goto _LL1125; _LL1125:
_temp1124=( struct Cyc_Core_Opt*) _temp1123.region; goto _LL1100;} else{ goto
_LL1101;} _LL1101: if( _temp1093 == Cyc_Absyn_Union_d){ goto _LL1102;} else{
goto _LL1103;} _LL1103: if(( unsigned int) _temp1093 > 1u?(( struct _enum_struct*)
_temp1093)->tag == Cyc_Absyn_Enum_d_tag: 0){ _LL1139: _temp1138=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp1093)->f1; goto _LL1104;} else{ goto
_LL1105;} _LL1105: if(( unsigned int) _temp1093 > 1u?(( struct _enum_struct*)
_temp1093)->tag == Cyc_Absyn_Xenum_d_tag: 0){ _LL1141: _temp1140=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_d_struct*) _temp1093)->f1; goto _LL1106;} else{ goto
_LL1107;} _LL1107: if(( unsigned int) _temp1093 > 1u?(( struct _enum_struct*)
_temp1093)->tag == Cyc_Absyn_Let_d_tag: 0){ _LL1151: _temp1150=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp1093)->f1; goto _LL1149; _LL1149: _temp1148=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp1093)->f2; goto
_LL1147; _LL1147: _temp1146=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1093)->f3; goto _LL1145; _LL1145: _temp1144=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Let_d_struct*) _temp1093)->f4; goto _LL1143; _LL1143: _temp1142=(
int)(( struct Cyc_Absyn_Let_d_struct*) _temp1093)->f5; goto _LL1108;} else{ goto
_LL1109;} _LL1109: if(( unsigned int) _temp1093 > 1u?(( struct _enum_struct*)
_temp1093)->tag == Cyc_Absyn_Typedef_d_tag: 0){ _LL1153: _temp1152=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp1093)->f1; goto _LL1110;} else{ goto
_LL1111;} _LL1111: if(( unsigned int) _temp1093 > 1u?(( struct _enum_struct*)
_temp1093)->tag == Cyc_Absyn_Namespace_d_tag: 0){ _LL1157: _temp1156=( struct
_tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp1093)->f1; goto
_LL1155; _LL1155: _temp1154=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1093)->f2; goto _LL1112;} else{ goto _LL1113;} _LL1113: if(( unsigned int)
_temp1093 > 1u?(( struct _enum_struct*) _temp1093)->tag == Cyc_Absyn_Using_d_tag:
0){ _LL1161: _temp1160=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1093)->f1; goto _LL1159; _LL1159: _temp1158=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1093)->f2; goto _LL1114;} else{ goto
_LL1115;} _LL1115: if(( unsigned int) _temp1093 > 1u?(( struct _enum_struct*)
_temp1093)->tag == Cyc_Absyn_ExternC_d_tag: 0){ _LL1163: _temp1162=( struct Cyc_List_List*)((
struct Cyc_Absyn_ExternC_d_struct*) _temp1093)->f1; goto _LL1116;} else{ goto
_LL1094;} _LL1096: if( _temp1117->is_inline){ Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1164=( char*)"inline"; struct _tagged_string
_temp1165; _temp1165.curr= _temp1164; _temp1165.base= _temp1164; _temp1165.last_plus_one=
_temp1164 + 7; _temp1165;}));} Cyc_Absyndump_dumpscope(( void*) _temp1117->sc);{
void* t=({ struct Cyc_Absyn_FnType_struct* _temp1166=( struct Cyc_Absyn_FnType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1166->tag= Cyc_Absyn_FnType_tag;
_temp1166->f1=({ struct Cyc_Absyn_FnInfo _temp1167; _temp1167.tvars= _temp1117->tvs;
_temp1167.effect= _temp1117->effect; _temp1167.ret_typ=( void*)(( void*)
_temp1117->ret_type); _temp1167.args=(( struct Cyc_List_List*(*)( struct _tuple1*(*
f)( struct _tuple3*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Absynpp_arg_mk_opt,
_temp1117->args); _temp1167.varargs= _temp1117->varargs; _temp1167;});( void*)
_temp1166;});(( void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( struct
_tuple0*), struct _tuple0*)) Cyc_Absyndump_dumptqtd)( 0, t, Cyc_Absyndump_dumpqvar,
_temp1117->name); Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpstmt(
_temp1117->body); Cyc_Absyndump_dump_char(( int)'}'); goto _LL1094;} _LL1098:
Cyc_Absyndump_dumpscope(( void*) _temp1119->sc); Cyc_Absyndump_dump(( struct
_tagged_string)({ char* _temp1168=( char*)"struct"; struct _tagged_string
_temp1169; _temp1169.curr= _temp1168; _temp1169.base= _temp1168; _temp1169.last_plus_one=
_temp1168 + 7; _temp1169;})); if( _temp1119->name != 0){ Cyc_Absyndump_dumpqvar((
struct _tuple0*)( _temp1119->name)->v);} Cyc_Absyndump_dumptvars( _temp1119->tvs);
if( _temp1119->fields == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpstructfields(( struct Cyc_List_List*)( _temp1119->fields)->v);
Cyc_Absyndump_dump_nospace(( struct _tagged_string)({ char* _temp1170=( char*)"};";
struct _tagged_string _temp1171; _temp1171.curr= _temp1170; _temp1171.base=
_temp1170; _temp1171.last_plus_one= _temp1170 + 3; _temp1171;}));} goto _LL1094;
_LL1100: Cyc_Absyndump_dumpscope( _temp1136);(( void(*)( struct Cyc_Absyn_Tqual*,
void*, void(* f)( struct _tuple0*), struct _tuple0*)) Cyc_Absyndump_dumptqtd)((
struct Cyc_Absyn_Tqual*) _temp1132, _temp1130, Cyc_Absyndump_dumpqvar, _temp1134);
if( _temp1128 != 0){ Cyc_Absyndump_dump_char(( int)'='); Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1172= _temp1128; if(
_temp1172 == 0){ _throw( Null_Exception);} _temp1172;}));} Cyc_Absyndump_dump_semi();
goto _LL1094; _LL1102: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1173=( char*)"**UnionDecl**"; struct _tagged_string _temp1174; _temp1174.curr=
_temp1173; _temp1174.base= _temp1173; _temp1174.last_plus_one= _temp1173 + 14;
_temp1174;})); goto _LL1094; _LL1104: Cyc_Absyndump_dumpscope(( void*) _temp1138->sc);
Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1175=( char*)"enum";
struct _tagged_string _temp1176; _temp1176.curr= _temp1175; _temp1176.base=
_temp1175; _temp1176.last_plus_one= _temp1175 + 5; _temp1176;})); if( _temp1138->name
!= 0){ Cyc_Absyndump_dumpqvar(( struct _tuple0*)( _temp1138->name)->v);} Cyc_Absyndump_dumptvars(
_temp1138->tvs); Cyc_Absyndump_dump_char(( int)'{'); if( _temp1138->fields == 0){
Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpenumfields((
struct Cyc_List_List*)( _temp1138->fields)->v); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1177=( char*)"};"; struct _tagged_string
_temp1178; _temp1178.curr= _temp1177; _temp1178.base= _temp1177; _temp1178.last_plus_one=
_temp1177 + 3; _temp1178;}));} goto _LL1094; _LL1106: Cyc_Absyndump_dumpscope((
void*) _temp1140->sc); Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1179=( char*)"xenum"; struct _tagged_string _temp1180; _temp1180.curr=
_temp1179; _temp1180.base= _temp1179; _temp1180.last_plus_one= _temp1179 + 6;
_temp1180;})); Cyc_Absyndump_dumpqvar( _temp1140->name); Cyc_Absyndump_dump_char((
int)'{'); if( _temp1140->fields == 0){ Cyc_Absyndump_dump_semi();} else{ Cyc_Absyndump_dump_char((
int)'{'); Cyc_Absyndump_dumpenumfields( _temp1140->fields); Cyc_Absyndump_dump_nospace((
struct _tagged_string)({ char* _temp1181=( char*)"};"; struct _tagged_string
_temp1182; _temp1182.curr= _temp1181; _temp1182.base= _temp1181; _temp1182.last_plus_one=
_temp1181 + 3; _temp1182;}));} goto _LL1094; _LL1108: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1183=( char*)"let"; struct _tagged_string
_temp1184; _temp1184.curr= _temp1183; _temp1184.base= _temp1183; _temp1184.last_plus_one=
_temp1183 + 4; _temp1184;})); Cyc_Absyndump_dumppat( _temp1150); Cyc_Absyndump_dump_char((
int)'='); Cyc_Absyndump_dumpexp( _temp1144); Cyc_Absyndump_dump_semi(); goto
_LL1094; _LL1110: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1185=(
char*)"typedef"; struct _tagged_string _temp1186; _temp1186.curr= _temp1185;
_temp1186.base= _temp1185; _temp1186.last_plus_one= _temp1185 + 8; _temp1186;}));((
void(*)( struct Cyc_Absyn_Tqual*, void*, void(* f)( struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*)) Cyc_Absyndump_dumptqtd)( 0,( void*) _temp1152->defn,
Cyc_Absyndump_dumptypedefname, _temp1152); Cyc_Absyndump_dump_semi(); goto
_LL1094; _LL1112: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1187=(
char*)"namespace"; struct _tagged_string _temp1188; _temp1188.curr= _temp1187;
_temp1188.base= _temp1187; _temp1188.last_plus_one= _temp1187 + 10; _temp1188;}));
Cyc_Absyndump_dump_str( _temp1156); Cyc_Absyndump_dump_char(( int)'{'); for( 0;
_temp1154 != 0; _temp1154= _temp1154->tl){ Cyc_Absyndump_dumpdecl(( struct Cyc_Absyn_Decl*)
_temp1154->hd);} Cyc_Absyndump_dump_char(( int)'}'); goto _LL1094; _LL1114: Cyc_Absyndump_dump((
struct _tagged_string)({ char* _temp1189=( char*)"using"; struct _tagged_string
_temp1190; _temp1190.curr= _temp1189; _temp1190.base= _temp1189; _temp1190.last_plus_one=
_temp1189 + 6; _temp1190;})); Cyc_Absyndump_dumpqvar( _temp1160); Cyc_Absyndump_dump_char((
int)'{'); for( 0; _temp1158 != 0; _temp1158= _temp1158->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*) _temp1158->hd);} Cyc_Absyndump_dump_char(( int)'}');
goto _LL1094; _LL1116: Cyc_Absyndump_dump(( struct _tagged_string)({ char*
_temp1191=( char*)"extern \"C\" {"; struct _tagged_string _temp1192; _temp1192.curr=
_temp1191; _temp1192.base= _temp1191; _temp1192.last_plus_one= _temp1191 + 13;
_temp1192;})); for( 0; _temp1162 != 0; _temp1162= _temp1162->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*) _temp1162->hd);} Cyc_Absyndump_dump_char(( int)'}');
goto _LL1094; _LL1094:;} static void Cyc_Absyndump_dump_upperbound( struct Cyc_Absyn_Exp*
e){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( e); if( i !=( unsigned int) 1){
Cyc_Absyndump_dump_char(( int)'{'); Cyc_Absyndump_dumpexp( e); Cyc_Absyndump_dump_char((
int)'}');}} void Cyc_Absyndump_dumptms( struct Cyc_List_List* tms, void(* f)(
void*), void* a){ if( tms == 0){ f( a); return;}{ void* _temp1193=( void*) tms->hd;
struct Cyc_Absyn_Tqual* _temp1211; void* _temp1213; void* _temp1215; struct Cyc_Absyn_Exp*
_temp1217; struct Cyc_Absyn_Tqual* _temp1219; void* _temp1221; void* _temp1223;
struct Cyc_Absyn_Exp* _temp1225; struct Cyc_Absyn_Tqual* _temp1227; void*
_temp1229; void* _temp1231; struct Cyc_Absyn_Tqual* _temp1233; void* _temp1235;
struct Cyc_Absyn_Tvar* _temp1237; void* _temp1239; struct Cyc_Absyn_Exp*
_temp1241; struct Cyc_Absyn_Tqual* _temp1243; void* _temp1245; struct Cyc_Absyn_Tvar*
_temp1247; void* _temp1249; struct Cyc_Absyn_Exp* _temp1251; struct Cyc_Absyn_Tqual*
_temp1253; void* _temp1255; struct Cyc_Absyn_Tvar* _temp1257; void* _temp1259;
struct Cyc_Absyn_Tqual* _temp1261; void* _temp1263; void* _temp1265; _LL1195:
if(( unsigned int) _temp1193 > 1u?(( struct _enum_struct*) _temp1193)->tag ==
Cyc_Absyn_Pointer_mod_tag: 0){ _LL1216: _temp1215=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1193)->f1; if(( unsigned int) _temp1215 > 1u?(( struct _enum_struct*)
_temp1215)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL1218: _temp1217=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1215)->f1; goto
_LL1214;} else{ goto _LL1197;} _LL1214: _temp1213=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1193)->f2; if( _temp1213 == Cyc_Absyn_HeapRgn){ goto _LL1212;} else{ goto
_LL1197;} _LL1212: _temp1211=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1193)->f3; goto _LL1196;} else{ goto _LL1197;} _LL1197: if(( unsigned int)
_temp1193 > 1u?(( struct _enum_struct*) _temp1193)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1224: _temp1223=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1193)->f1; if(( unsigned int) _temp1223 > 1u?(( struct _enum_struct*)
_temp1223)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL1226: _temp1225=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1223)->f1; goto
_LL1222;} else{ goto _LL1199;} _LL1222: _temp1221=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1193)->f2; if( _temp1221 == Cyc_Absyn_HeapRgn){ goto _LL1220;} else{ goto
_LL1199;} _LL1220: _temp1219=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1193)->f3; goto _LL1198;} else{ goto _LL1199;} _LL1199: if(( unsigned int)
_temp1193 > 1u?(( struct _enum_struct*) _temp1193)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1232: _temp1231=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1193)->f1; if( _temp1231 == Cyc_Absyn_TaggedArray_ps){ goto _LL1230;} else{
goto _LL1201;} _LL1230: _temp1229=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1193)->f2; if( _temp1229 == Cyc_Absyn_HeapRgn){ goto _LL1228;} else{ goto
_LL1201;} _LL1228: _temp1227=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1193)->f3; goto _LL1200;} else{ goto _LL1201;} _LL1201: if(( unsigned int)
_temp1193 > 1u?(( struct _enum_struct*) _temp1193)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1240: _temp1239=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1193)->f1; if(( unsigned int) _temp1239 > 1u?(( struct _enum_struct*)
_temp1239)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL1242: _temp1241=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*) _temp1239)->f1; goto
_LL1236;} else{ goto _LL1203;} _LL1236: _temp1235=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1193)->f2; if(( unsigned int) _temp1235 > 5u?(( struct _enum_struct*)
_temp1235)->tag == Cyc_Absyn_VarType_tag: 0){ _LL1238: _temp1237=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp1235)->f1; goto _LL1234;} else{ goto
_LL1203;} _LL1234: _temp1233=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1193)->f3; goto _LL1202;} else{ goto _LL1203;} _LL1203: if(( unsigned int)
_temp1193 > 1u?(( struct _enum_struct*) _temp1193)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1250: _temp1249=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1193)->f1; if(( unsigned int) _temp1249 > 1u?(( struct _enum_struct*)
_temp1249)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL1252: _temp1251=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp1249)->f1; goto
_LL1246;} else{ goto _LL1205;} _LL1246: _temp1245=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1193)->f2; if(( unsigned int) _temp1245 > 5u?(( struct _enum_struct*)
_temp1245)->tag == Cyc_Absyn_VarType_tag: 0){ _LL1248: _temp1247=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp1245)->f1; goto _LL1244;} else{ goto
_LL1205;} _LL1244: _temp1243=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1193)->f3; goto _LL1204;} else{ goto _LL1205;} _LL1205: if(( unsigned int)
_temp1193 > 1u?(( struct _enum_struct*) _temp1193)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1260: _temp1259=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1193)->f1; if( _temp1259 == Cyc_Absyn_TaggedArray_ps){ goto _LL1256;} else{
goto _LL1207;} _LL1256: _temp1255=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1193)->f2; if(( unsigned int) _temp1255 > 5u?(( struct _enum_struct*)
_temp1255)->tag == Cyc_Absyn_VarType_tag: 0){ _LL1258: _temp1257=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp1255)->f1; goto _LL1254;} else{ goto
_LL1207;} _LL1254: _temp1253=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1193)->f3; goto _LL1206;} else{ goto _LL1207;} _LL1207: if(( unsigned int)
_temp1193 > 1u?(( struct _enum_struct*) _temp1193)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1266: _temp1265=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1193)->f1; goto _LL1264; _LL1264: _temp1263=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1193)->f2; goto _LL1262; _LL1262: _temp1261=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1193)->f3; goto _LL1208;} else{ goto
_LL1209;} _LL1209: goto _LL1210; _LL1196: Cyc_Absyndump_dump_char(( int)'*');
Cyc_Absyndump_dump_upperbound( _temp1217);(( void(*)( struct Cyc_List_List* tms,
void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)( tms->tl, f, a); return;
_LL1198: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1225);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)( tms->tl, f, a); return; _LL1200: Cyc_Absyndump_dump_char((
int)'?');(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)(
tms->tl, f, a); return; _LL1202: Cyc_Absyndump_dump_char(( int)'*'); Cyc_Absyndump_dump_upperbound(
_temp1241); Cyc_Absyndump_dump_str( _temp1237->name);(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)( tms->tl, f, a);
return; _LL1204: Cyc_Absyndump_dump_char(( int)'@'); Cyc_Absyndump_dump_upperbound(
_temp1251); Cyc_Absyndump_dump_str( _temp1247->name);(( void(*)( struct Cyc_List_List*
tms, void(* f)( void*), void* a)) Cyc_Absyndump_dumptms)( tms->tl, f, a);
return; _LL1206: Cyc_Absyndump_dump_char(( int)'?'); Cyc_Absyndump_dump_str(
_temp1257->name);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void*
a)) Cyc_Absyndump_dumptms)( tms->tl, f, a); return; _LL1208:( void) _throw(({
struct Cyc_Core_Impossible_struct* _temp1267=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp1267=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp1268=( char*)"dumptms: bad Pointer_mod";
struct _tagged_string _temp1269; _temp1269.curr= _temp1268; _temp1269.base=
_temp1268; _temp1269.last_plus_one= _temp1268 + 25; _temp1269;})};( struct
_xenum_struct*) _temp1267;})); _LL1210: { int next_is_pointer= 0; if( tms->tl !=
0){ void* _temp1270=( void*)( tms->tl)->hd; struct Cyc_Absyn_Tqual* _temp1276;
void* _temp1278; void* _temp1280; _LL1272: if(( unsigned int) _temp1270 > 1u?((
struct _enum_struct*) _temp1270)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL1281:
_temp1280=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp1270)->f1; goto
_LL1279; _LL1279: _temp1278=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1270)->f2; goto _LL1277; _LL1277: _temp1276=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1270)->f3; goto _LL1273;} else{ goto
_LL1274;} _LL1274: goto _LL1275; _LL1273: next_is_pointer= 1; goto _LL1271;
_LL1275: goto _LL1271; _LL1271:;} if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)'(');}(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)( tms->tl, f, a); if( next_is_pointer){ Cyc_Absyndump_dump_char((
int)')');}{ void* _temp1282=( void*) tms->hd; struct Cyc_Absyn_Exp* _temp1296;
void* _temp1298; struct Cyc_Core_Opt* _temp1300; int _temp1302; struct Cyc_List_List*
_temp1304; void* _temp1306; struct Cyc_Position_Segment* _temp1308; struct Cyc_List_List*
_temp1310; int _temp1312; struct Cyc_Position_Segment* _temp1314; struct Cyc_List_List*
_temp1316; struct Cyc_Absyn_Tqual* _temp1318; void* _temp1320; void* _temp1322;
_LL1284: if( _temp1282 == Cyc_Absyn_Carray_mod){ goto _LL1285;} else{ goto
_LL1286;} _LL1286: if(( unsigned int) _temp1282 > 1u?(( struct _enum_struct*)
_temp1282)->tag == Cyc_Absyn_ConstArray_mod_tag: 0){ _LL1297: _temp1296=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp1282)->f1; goto
_LL1287;} else{ goto _LL1288;} _LL1288: if(( unsigned int) _temp1282 > 1u?((
struct _enum_struct*) _temp1282)->tag == Cyc_Absyn_Function_mod_tag: 0){ _LL1299:
_temp1298=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1282)->f1; if(((
struct _enum_struct*) _temp1298)->tag == Cyc_Absyn_WithTypes_tag){ _LL1305:
_temp1304=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp1298)->f1; goto _LL1303; _LL1303: _temp1302=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp1298)->f2; goto _LL1301; _LL1301: _temp1300=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_WithTypes_struct*) _temp1298)->f3; goto _LL1289;} else{ goto
_LL1290;}} else{ goto _LL1290;} _LL1290: if(( unsigned int) _temp1282 > 1u?((
struct _enum_struct*) _temp1282)->tag == Cyc_Absyn_Function_mod_tag: 0){ _LL1307:
_temp1306=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp1282)->f1; if(((
struct _enum_struct*) _temp1306)->tag == Cyc_Absyn_NoTypes_tag){ _LL1311:
_temp1310=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*) _temp1306)->f1;
goto _LL1309; _LL1309: _temp1308=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp1306)->f2; goto _LL1291;} else{ goto _LL1292;}} else{ goto _LL1292;}
_LL1292: if(( unsigned int) _temp1282 > 1u?(( struct _enum_struct*) _temp1282)->tag
== Cyc_Absyn_TypeParams_mod_tag: 0){ _LL1317: _temp1316=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp1282)->f1; goto _LL1315; _LL1315:
_temp1314=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1282)->f2; goto _LL1313; _LL1313: _temp1312=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp1282)->f3; goto _LL1293;} else{ goto _LL1294;} _LL1294: if(( unsigned int)
_temp1282 > 1u?(( struct _enum_struct*) _temp1282)->tag == Cyc_Absyn_Pointer_mod_tag:
0){ _LL1323: _temp1322=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1282)->f1; goto _LL1321; _LL1321: _temp1320=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp1282)->f2; goto _LL1319; _LL1319: _temp1318=( struct Cyc_Absyn_Tqual*)((
struct Cyc_Absyn_Pointer_mod_struct*) _temp1282)->f3; goto _LL1295;} else{ goto
_LL1283;} _LL1285: Cyc_Absyndump_dump(( struct _tagged_string)({ char* _temp1324=(
char*)"[]"; struct _tagged_string _temp1325; _temp1325.curr= _temp1324;
_temp1325.base= _temp1324; _temp1325.last_plus_one= _temp1324 + 3; _temp1325;}));
goto _LL1283; _LL1287: Cyc_Absyndump_dump_char(( int)'['); Cyc_Absyndump_dumpexp(
_temp1296); Cyc_Absyndump_dump_char(( int)']'); goto _LL1283; _LL1289: Cyc_Absyndump_dumpfunargs(
_temp1304, _temp1302, _temp1300); goto _LL1283; _LL1291:(( void(*)( void(* f)(
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string start,
struct _tagged_string end, struct _tagged_string sep)) Cyc_Absyndump_group)( Cyc_Absyndump_dump_str,
_temp1310,( struct _tagged_string)({ char* _temp1326=( char*)"("; struct
_tagged_string _temp1327; _temp1327.curr= _temp1326; _temp1327.base= _temp1326;
_temp1327.last_plus_one= _temp1326 + 2; _temp1327;}),( struct _tagged_string)({
char* _temp1328=( char*)")"; struct _tagged_string _temp1329; _temp1329.curr=
_temp1328; _temp1329.base= _temp1328; _temp1329.last_plus_one= _temp1328 + 2;
_temp1329;}),( struct _tagged_string)({ char* _temp1330=( char*)","; struct
_tagged_string _temp1331; _temp1331.curr= _temp1330; _temp1331.base= _temp1330;
_temp1331.last_plus_one= _temp1330 + 2; _temp1331;})); goto _LL1283; _LL1293:
if( _temp1312){ Cyc_Absyndump_dumpkindedtvars( _temp1316);} else{ Cyc_Absyndump_dumptvars(
_temp1316);} goto _LL1283; _LL1295:( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp1332=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp1332=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp1333=( char*)"dumptms"; struct
_tagged_string _temp1334; _temp1334.curr= _temp1333; _temp1334.base= _temp1333;
_temp1334.last_plus_one= _temp1333 + 8; _temp1334;})};( struct _xenum_struct*)
_temp1332;})); _LL1283:;} return;} _LL1194:;}} static struct Cyc_Absyn_Tqual Cyc_Absyndump_empty_tq_v=(
struct Cyc_Absyn_Tqual){.q_const= 0,.q_volatile= 0,.q_restrict= 0}; static
struct Cyc_Absyn_Tqual* Cyc_Absyndump_empty_tq=& Cyc_Absyndump_empty_tq_v; void
Cyc_Absyndump_dumptqtd( struct Cyc_Absyn_Tqual* tq, void* t, void(* f)( void*),
void* a){ if( tq == 0){ tq=( struct Cyc_Absyn_Tqual*) Cyc_Absyndump_empty_tq;}{
struct Cyc_List_List* _temp1338; void* _temp1340; struct Cyc_Absyn_Tqual*
_temp1342; struct _tuple4 _temp1336= Cyc_Absynpp_to_tms(( struct Cyc_Absyn_Tqual*)({
struct Cyc_Absyn_Tqual* _temp1335= tq; if( _temp1335 == 0){ _throw(
Null_Exception);} _temp1335;}), t); _LL1343: _temp1342= _temp1336.f1; goto
_LL1341; _LL1341: _temp1340= _temp1336.f2; goto _LL1339; _LL1339: _temp1338=
_temp1336.f3; goto _LL1337; _LL1337: Cyc_Absyndump_dumptq( _temp1342); Cyc_Absyndump_dumpntyp(
_temp1340);(( void(*)( struct Cyc_List_List* tms, void(* f)( void*), void* a))
Cyc_Absyndump_dumptms)((( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( _temp1338), f, a);}} void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List* tdl, struct Cyc_Stdio___sFILE* f){ Cyc_Absyndump_pos= 0;*
Cyc_Absyndump_dump_file= f; for( 0; tdl != 0; tdl= tdl->tl){ Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*) tdl->hd);}}